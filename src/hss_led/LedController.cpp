// SPDX-License-Identifier: Apache-2.0
// One controller for radio status and explicitly selected LED-class devices.
#include "LedProtocol.h"
#include "LedController.h"
#include "LedCli.h"
#include <atomic>
#include <mutex>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <dirent.h>
#include <map>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/file.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <vector>

static std::string runtime = "/run/hss_led";
static std::string sysfs = "/sys/class/leds";
static std::string startup = "/var/status/startupFinished";
static std::string disabled = "/etc/config/disableLED";
static const char* colors[] = {"off", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
static std::atomic<bool> statusEnabled(false);
static const unsigned maxDelay = 86400000;
static uid_t administrator = 0;

static uint64_t nowMs() {
  timespec t = {};
  if (clock_gettime(CLOCK_MONOTONIC, &t)) throw std::runtime_error("clock_gettime");
  return uint64_t(t.tv_sec) * 1000 + t.tv_nsec / 1000000;
}
static bool exists(const std::string& p) { return access(p.c_str(), F_OK) == 0; }
static std::string readText(const std::string& p) {
  std::ifstream f(p);
  std::ostringstream s;
  s << f.rdbuf();
  return s.str();
}
static bool number(const std::string& s, unsigned& n, unsigned maximum) {
  if (s.empty() || s.size() > 16) return false;
  n = 0;
  for (char c : s) {
    if (c < '0' || c > '9' || n > maximum / 10 ||
        (n == maximum / 10 && unsigned(c - '0') > maximum % 10)) return false;
    n = n * 10 + unsigned(c - '0');
  }
  return true;
}
static bool duration(const std::string& s, unsigned& value) {
  if (s == "slow") { value = LedCli::slowMs; return true; }
  if (s == "fast") { value = LedCli::fastMs; return true; }
  return number(s, value, maxDelay);
}
static std::vector<std::string> words(const std::string& s) {
  std::istringstream in(s);
  std::vector<std::string> v;
  std::string w;
  while (in >> w) v.push_back(w);
  return v;
}
static unsigned color(const std::string& name) {
  for (unsigned i = 0; i < 8; ++i) if (name == colors[i]) return i;
  throw std::runtime_error("unknown color");
}
struct Pattern {
  unsigned a = 0, b = 0, da = 0, db = 0;
  bool operator==(const Pattern& p) const { return a == p.a && b == p.b && da == p.da && db == p.db; }
  bool alternating() const { return da && a != b; }
  std::string wire() const {
    return std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(da) + " " + std::to_string(db);
  }
};
// Only the controller thread owns State and sysfs. Status calculation publishes
// the latest pattern through this bounded in-process mailbox; no self-IPC.
struct ReportMailbox {
  std::mutex mutex;
  Pattern pattern;
  bool pending = false;
  void publish(const Pattern& p) {
    std::lock_guard<std::mutex> lock(mutex);
    pattern = p;
    pending = true;
  }
  bool take(Pattern& p) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!pending) return false;
    p = pattern;
    pending = false;
    return true;
  }
};
static ReportMailbox reports;

void LedController::report(unsigned first, unsigned second, unsigned period) {
  if (first > 7 || second > 7 || period > maxDelay) return;
  Pattern p;
  p.a = first; p.b = period ? second : first; p.da = p.db = period;
  reports.publish(p);
}
bool LedController::statusActive() { return statusEnabled.load(); }

static bool parsePattern(const std::vector<std::string>& v, size_t offset, Pattern& p) {
  return v.size() >= offset + 4 && number(v[offset], p.a, 7) && number(v[offset+1], p.b, 7) &&
      number(v[offset+2], p.da, maxDelay) && number(v[offset+3], p.db, maxDelay) &&
      ((!p.da && !p.db && p.a == p.b) || (p.da && p.db));
}
static bool writeValue(const std::string& path, const std::string& value) {
  int fd = open(path.c_str(), O_WRONLY | O_CLOEXEC | O_TRUNC);
  if (fd < 0) return false;
  std::string line = value + "\n";
  ssize_t n;
  do { n = write(fd, line.data(), line.size()); } while (n < 0 && errno == EINTR);
  bool ok = n == static_cast<ssize_t>(line.size());
  if (close(fd)) ok = false;
  return ok;
}

// Explicit scalar LED targets use the kernel LED-class API. They are never
// included in automatic CCU status output. RPI-RF-MOD components are reserved
// for the RGB owner, so a second command cannot fight that owner's timer.
static bool ledName(const std::string& name) {
  if (name.empty() || name.size() > 255 || name == "." || name == "..") return false;
  for (unsigned char c : name)
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == ':' || c == '_' || c == '-' || c == '.')) return false;
  return true;
}
static bool radioNode(const std::string& name) { return name.compare(0, 11, "rpi_rf_mod:") == 0; }
static std::string selectedTrigger(const std::string& path) {
  for (const auto& item : words(readText(path + "/trigger")))
    if (item.size() > 2 && item.front() == '[' && item.back() == ']')
      return item.substr(1, item.size() - 2);
#ifdef LED_TEST_BUILD
  auto items = words(readText(path + "/trigger"));
  if (items.size() == 1) return items[0];
#endif
  return "";
}
static bool hasTrigger(const std::string& path, const std::string& trigger) {
  for (const auto& item : words(readText(path + "/trigger")))
    if (item == trigger || item == "[" + trigger + "]") return true;
  return false;
}
static bool scalarMaximum(const std::string& path, unsigned& maximum) {
  auto v = words(readText(path + "/max_brightness"));
  return !exists(path + "/multi_index") && v.size() == 1 &&
      number(v[0], maximum, UINT32_MAX) && maximum && exists(path + "/brightness");
}
class BoardLeds {
  struct Pending {
    unsigned on = 0, off = 0;
    uint64_t deadline = 0;
    dev_t device = 0;
    ino_t inode = 0;
    bool pending = false;
    std::string error;
  };
  // Only timer setup needs retries: udev grants permissions on delay_on/off
  // after selecting timer. Never sleep here or recreate the trigger on retry.
  std::map<std::string, Pending> changes;
  static bool identity(const std::string& path, Pending& p) {
    struct stat st = {};
    if (stat((path + "/brightness").c_str(), &st)) return false;
    p.device = st.st_dev; p.inode = st.st_ino;
    return true;
  }
public:
  void tick(uint64_t now) {
    for (auto& item : changes) {
      Pending& p = item.second;
      if (!p.pending) continue;
      std::string path = sysfs + "/" + item.first;
      Pending current;
      if (!identity(path, current) || current.device != p.device || current.inode != p.inode ||
          selectedTrigger(path) != "timer") {
        p.error = "device-or-trigger-changed"; p.pending = false;
      } else if (writeValue(path + "/delay_on", std::to_string(p.on)) &&
                 writeValue(path + "/delay_off", std::to_string(p.off))) {
        p.pending = false; p.error.clear();
      } else if (now >= p.deadline) {
        p.error = "timer-attributes-not-writable"; p.pending = false;
      }
    }
  }
  std::string list() const {
    DIR* directory = opendir(sysfs.c_str());
    if (!directory) return "ERR cannot enumerate LED class";
    std::vector<std::string> names;
    while (dirent* entry = readdir(directory)) {
      std::string name(entry->d_name);
      if (ledName(name) && name != "rpi-rf-mod" && !radioNode(name)) names.push_back(name);
    }
    closedir(directory);
    std::sort(names.begin(), names.end());
    std::string result = "OK\nrpi-rf-mod\trgb/legacy (use --led rpi-rf-mod status for availability)\n";
    for (const auto& name : names) {
      unsigned maximum;
      if (!scalarMaximum(sysfs + "/" + name, maximum)) continue;
      result += name + "\tscalar max_brightness=" + std::to_string(maximum) + "\n";
      if (result.size() >= 60000) return "ERR LED list exceeds reply limit";
    }
    return result;
  }
  std::string dispatch(const std::vector<std::string>& v, uid_t uid) {
    // Wire format: 1 led NAME OP [ARG...]. Names are class basenames only.
    if (v.size() < 4 || v[0] != "1" || v[1] != "led" || !ledName(v[2])) return "ERR invalid LED request";
    if (radioNode(v[2]) || v[2] == "rpi-rf-mod") return "ERR use --led rpi-rf-mod for the radio LED";
    std::string path = sysfs + "/" + v[2];
    unsigned maximum;
    if (!scalarMaximum(path, maximum)) return "ERR scalar LED unavailable or unsupported";
    const std::string& op = v[3];
    if (op == "status" && v.size() == 4) {
      auto entry = changes.find(v[2]);
      auto brightness = words(readText(path + "/brightness"));
      std::string result = "OK led=" + v[2] + " brightness=" +
          (brightness.size() == 1 ? brightness[0] : "unknown") + " max_brightness=" + std::to_string(maximum) +
          " trigger=" + selectedTrigger(path) + " pending=" +
          std::to_string(entry != changes.end() && entry->second.pending);
      if (entry != changes.end() && !entry->second.error.empty()) result += " error=" + entry->second.error;
      return result;
    }
    if (uid != administrator) return "ERR permission denied";
    unsigned value = 0, on = 0, off = 0;
    std::string trigger;
    if ((op == "on" || op == "off") && v.size() == 4) value = op == "on" ? maximum : 0;
    else if (op == "brightness" && v.size() == 5 && number(v[4], value, maximum)) {}
    else if (op == "blink" && v.size() == 6 && number(v[4], on, maxDelay) && on &&
             number(v[5], off, maxDelay) && off) trigger = "timer";
    else if (op == "trigger" && v.size() == 5 && ledName(v[4])) trigger = v[4];
    else return "ERR invalid LED command";
    if (!trigger.empty() && !hasTrigger(path, trigger)) return "ERR trigger unavailable";
    // Validate before cancelling any earlier timer setup.
    auto entry = changes.find(v[2]);
    if (entry == changes.end() && changes.size() >= 64) {
      for (auto it = changes.begin(); it != changes.end();) {
        if (!it->second.pending) it = changes.erase(it); else ++it;
      }
      if (changes.size() >= 64) return "ERR too many pending LED commands";
    }
    Pending next;
    if (!identity(path, next)) return "ERR LED disappeared";
    changes[v[2]] = next;
    auto fail = [&]() {
      changes[v[2]].error = "sysfs-write-failed";
      return std::string("ERR cannot write LED attributes");
    };
    if (on) {
      if (!writeValue(path + "/brightness", std::to_string(maximum)) ||
          !writeValue(path + "/trigger", "timer")) return fail();
      next.on = on; next.off = off; next.deadline = nowMs() + 5000; next.pending = true;
      changes[v[2]] = next;
      tick(nowMs());
    } else if (!trigger.empty()) {
      if (!writeValue(path + "/trigger", trigger)) return fail();
    } else {
      if (exists(path + "/trigger") && !writeValue(path + "/trigger", "none")) return fail();
      if (!writeValue(path + "/brightness", std::to_string(value))) return fail();
    }
    return "OK accepted";
  }
};

// Use elapsed monotonic time, not the number of timer events, so missed
// intervals are skipped and unequal phase durations do not accumulate drift.
static unsigned phaseAt(const Pattern& p, uint64_t elapsed, uint64_t& delay) {
  if (!p.alternating()) { delay = 0; return p.a; }
  uint64_t cycle = uint64_t(p.da) + p.db;
  uint64_t phase = elapsed % cycle;
  delay = phase < p.da ? p.da - phase : cycle - phase;
  return phase < p.da ? p.a : p.b;
}

class Backend {
  std::string signature;
  std::string rgb;
  unsigned maximum = 0;
  unsigned order[3] = {};
  unsigned levels[3] = {};
  bool timer = false;
  std::string component(unsigned i) const { return sysfs + "/rpi_rf_mod:" + colors[1U << i]; }
  bool scalar(const std::string& path, unsigned& value) {
    auto v = words(readText(path));
    return v.size() == 1 && number(v[0], value, 65535) && value;
  }
public:
  std::string name = "missing";
  // Device identity changes on unload/reload, even when its sysfs name stays
  // the same. Do not fall back to read-only component LEDs of a broken group.
  bool discover() {
    std::string next;
    rgb = sysfs + "/rpi_rf_mod:rgb:status";
    bool multi = exists(rgb + "/multi_index");
    std::vector<std::string> paths;
    if (multi) paths.push_back(rgb + "/multi_index");
    else for (unsigned i = 0; i < 3; ++i) paths.push_back(component(i) + "/brightness");
    for (const auto& path : paths) {
      struct stat st = {};
      if (stat(path.c_str(), &st)) { next = "missing"; break; }
      next += std::to_string(st.st_dev) + ":" + std::to_string(st.st_ino) + ";";
    }
    if (signature == next && name != "error") return false;
    signature = next;
    name = "error";
    if (next == "missing") { name = "missing"; return true; }
    if (multi) {
      auto index = words(readText(rgb + "/multi_index"));
      unsigned seen = 0;
      if (index.size() != 3 || !scalar(rgb + "/max_brightness", maximum)) return true;
      for (unsigned i = 0; i < 3; ++i) {
        unsigned bit = index[i] == "red" ? 1 : index[i] == "green" ? 2 : index[i] == "blue" ? 4 : 0;
        if (!bit || (seen & bit)) return true;
        seen |= bit; order[i] = bit;
      }
      auto triggers = words(readText(rgb + "/trigger"));
      timer = std::find(triggers.begin(), triggers.end(), "timer") != triggers.end() ||
          std::find(triggers.begin(), triggers.end(), "[timer]") != triggers.end();
      name = "rgb";
    } else {
      for (unsigned i = 0; i < 3; ++i)
        if (!exists(component(i) + "/trigger") || !scalar(component(i) + "/max_brightness", levels[i])) return true;
      name = "legacy";
    }
    return true;
  }
  bool kernelBlink(const Pattern& p) const { return name == "rgb" && timer && p.alternating() && p.a && !p.b; }
  bool setColor(unsigned mask) {
    if (name == "rgb") {
      std::string values;
      for (unsigned i = 0; i < 3; ++i) values += (i ? " " : "") + std::to_string(mask & order[i] ? maximum : 0);
      return writeValue(rgb + "/multi_intensity", values);
    }
    if (name != "legacy") return false;
    // Old GPIO nodes still require separate writes. Remove unwanted channels
    // first, then enable new channels, using a single common phase clock.
    for (unsigned on = 0; on < 2; ++on)
      for (unsigned i = 0; i < 3; ++i)
        if (!!(mask & (1U << i)) == !!on &&
            !writeValue(component(i) + "/brightness", std::to_string(on ? levels[i] : 0))) return false;
    return true;
  }
  bool apply(const Pattern& p, unsigned mask) {
    if (name == "rgb") {
      if (kernelBlink(p)) {
        if (!setColor(p.a)) return false;
        auto triggers = words(readText(rgb + "/trigger"));
        bool selected = std::find(triggers.begin(), triggers.end(), "[timer]") != triggers.end();
#ifdef LED_TEST_BUILD
        selected = selected || readText(rgb + "/trigger") == "timer\n";
#endif
        if (!selected && (!writeValue(rgb + "/brightness", std::to_string(maximum)) ||
                          !writeValue(rgb + "/trigger", "timer"))) return false;
        return writeValue(rgb + "/delay_on", std::to_string(p.da)) &&
               writeValue(rgb + "/delay_off", std::to_string(p.db));
      }
      if (!writeValue(rgb + "/trigger", "none") || !writeValue(rgb + "/brightness", "0") ||
          !setColor(mask) || !writeValue(rgb + "/brightness", std::to_string(mask || p.alternating() ? maximum : 0))) return false;
      return true;
    }
    if (name != "legacy") return false;
    for (unsigned i = 0; i < 3; ++i) if (!writeValue(component(i) + "/trigger", "none")) return false;
    return setColor(mask);
  }
};

struct State {
  bool automatic = false, overrideActive = false;
  Pattern system, report, manual;
  State() { system.a = system.b = 3; report.a = report.b = 4; }
  std::string wire() const {
    return "1 " + std::to_string(automatic) + " " + std::to_string(overrideActive) + " " +
        system.wire() + " " + report.wire() + " " + manual.wire();
  }
  bool load(const std::string& s) {
    auto v = words(s);
    unsigned a, o;
    if (v.size() != 15 || v[0] != "1" || !number(v[1], a, 1) || !number(v[2], o, 1) ||
        !parsePattern(v, 3, system) || !parsePattern(v, 7, report) || !parsePattern(v, 11, manual)) return false;
    automatic = a; overrideActive = o; return true;
  }
  Pattern effective() const {
    if (exists(disabled)) return Pattern();
    // Removal of startupFinished is a safety net for an interrupted shutdown
    // script: late reports must not restore the normal/override pattern.
    if (automatic && !exists(startup)) return system;
    return overrideActive ? manual : automatic ? report : system;
  }
};

static bool save(const State& state) {
  std::string path = runtime + "/state.tmp";
  int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_NOFOLLOW, 0600);
  if (fd < 0) return false;
  std::string data = state.wire() + "\n";
  ssize_t n;
  do { n = write(fd, data.data(), data.size()); } while (n < 0 && errno == EINTR);
  bool ok = n == static_cast<ssize_t>(data.size());
  if (close(fd)) ok = false;
  if (ok && rename(path.c_str(), (runtime + "/state").c_str()) == 0) return true;
  unlink(path.c_str());
  return false;
}

static std::string dispatch(State& state, const std::string& message, uid_t uid, uid_t reporter,
                            const Backend& backend, bool dirty) {
  auto v = words(message);
  if (message.size() >= 512 || message.find('\0') != std::string::npos || v.size() < 2 || v[0] != "1")
    return "ERR invalid request";
  State changed = state;
  bool valid = false;
  if (v[1] == "status" && v.size() == 2) {
    return "OK owner=" + std::string(state.overrideActive ? "override" : state.automatic ? "auto" : "system") +
        " backend=" + backend.name + " pending=" + std::to_string(dirty) + " pattern=" + state.effective().wire();
  }
  if ((uid == administrator || uid == reporter) && v[1] == "report" && v.size() == 6) {
    valid = parsePattern(v, 2, changed.report);
  } else if (uid == administrator) {
    if (v[1] == "system" && v.size() == 6) {
      valid = parsePattern(v, 2, changed.system);
      changed.automatic = false; changed.overrideActive = false;
    } else if (v[1] == "override" && v.size() == 6) {
      valid = parsePattern(v, 2, changed.manual); changed.overrideActive = true;
    } else if (v[1] == "release" && v.size() == 2) {
      changed.overrideActive = false; valid = true;
    } else if (v[1] == "auto" && v.size() == 2) {
      changed.automatic = true; changed.overrideActive = false; valid = true;
    }
  } else return "ERR permission denied";
  if (!valid) return "ERR invalid request";
  if (changed.wire() != state.wire() && !save(changed)) return "ERR cannot save runtime state";
  state = changed;
  return "OK accepted";
}

struct Connection { int fd; uid_t uid; uint64_t deadline; };
static int serve(void (*startStatus)()) {
  if (mkdir(runtime.c_str(), 0755) && errno != EEXIST) throw std::runtime_error("cannot create runtime directory");
  struct stat st = {};
  if (lstat(runtime.c_str(), &st) || !S_ISDIR(st.st_mode) || st.st_uid != geteuid() || (st.st_mode & 0022))
    throw std::runtime_error("unsafe runtime directory");
  int lock = open((runtime + "/lock").c_str(), O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600);
  if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB)) throw std::runtime_error("LED service already running or lock unavailable");
  sigset_t signals;
  sigemptyset(&signals); sigaddset(&signals, SIGTERM); sigaddset(&signals, SIGINT); sigaddset(&signals, SIGHUP);
  if (sigprocmask(SIG_BLOCK, &signals, nullptr)) throw std::runtime_error("sigprocmask");
  int signalFd = signalfd(-1, &signals, SFD_CLOEXEC | SFD_NONBLOCK);
  int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  int listener = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
  if (signalFd < 0 || timerFd < 0 || listener < 0) throw std::runtime_error("cannot create event descriptors");
  std::string sock = runtime + "/control";
  sockaddr_un address = {};
  address.sun_family = AF_UNIX;
  if (sock.size() >= sizeof(address.sun_path)) throw std::runtime_error("socket path too long");
  std::memcpy(address.sun_path, sock.c_str(), sock.size() + 1);
  unlink(sock.c_str());
  // The containing directory is not writable by clients. Only root and the
  // status group can connect, and report requests also verify SO_PEERCRED.
  mode_t oldMask = umask(0077);
  int bound = bind(listener, reinterpret_cast<sockaddr*>(&address), sizeof(address));
  umask(oldMask);
  if (bound) throw std::runtime_error("bind");
#ifndef LED_TEST_BUILD
  if (group* g = getgrnam("status")) {
    if (chown(sock.c_str(), geteuid(), g->gr_gid)) throw std::runtime_error("socket group");
  }
#endif
  if (chmod(sock.c_str(), 0660) || listen(listener, 16)) throw std::runtime_error("listen/permissions");
  uid_t reporter = static_cast<uid_t>(-1);
  if (passwd* p = getpwnam("hssled")) reporter = p->pw_uid;
  State state, restored;
  if (restored.load(readText(runtime + "/state"))) state = restored;
  Backend backend;
  BoardLeds board;
  Pattern active;
  bool first = true, dirty = true;
  unsigned lastColor = 8;
  uint64_t epoch = nowMs(), maintenance = 0;
  std::vector<Connection> clients;
  // Signals are blocked before spawning the status thread. The controller
  // alone consumes them, including while a status query is blocked.
  if (startStatus) startStatus();
  while (true) {
    uint64_t now = nowMs();
    if (now >= maintenance) {
      if (backend.discover()) dirty = true;
      board.tick(now);
      maintenance = now + 250;
    }
    Pattern report;
    if (reports.take(report)) {
      State changed = state;
      changed.report = report;
      if (changed.wire() != state.wire() && !save(changed))
        std::cerr << "hss_led: cannot save automatic LED status\n";
      state = changed;
    }
    statusEnabled.store(exists(startup));
    Pattern desired = state.effective();
    if (first || !(active == desired)) {
      active = desired; epoch = now; first = false; dirty = true;
    }
    uint64_t next = maintenance;
    unsigned mask = active.a;
    if (active.alternating() && !backend.kernelBlink(active)) {
      uint64_t delay;
      mask = phaseAt(active, now - epoch, delay);
      next = std::min(next, now + delay);
    }
    if (dirty) {
      dirty = !backend.apply(active, mask);
      if (!dirty) lastColor = mask;
    } else if (mask != lastColor && !backend.kernelBlink(active)) {
      dirty = !backend.setColor(mask);
      if (!dirty) lastColor = mask;
    }
    itimerspec timer = {};
    next = std::max(next, now + 1);
    timer.it_value.tv_sec = next / 1000;
    timer.it_value.tv_nsec = (next % 1000) * 1000000;
    if (timerfd_settime(timerFd, TFD_TIMER_ABSTIME, &timer, nullptr)) throw std::runtime_error("timerfd_settime");
    std::vector<pollfd> fds = {{listener, POLLIN, 0}, {timerFd, POLLIN, 0}, {signalFd, POLLIN, 0}};
    for (const auto& c : clients) fds.push_back({c.fd, POLLIN, 0});
    int ready = poll(fds.data(), fds.size(), -1);
    if (ready < 0) { if (errno == EINTR) continue; throw std::runtime_error("poll"); }
    if (fds[2].revents) break;
    if (fds[1].revents) {
      uint64_t ticks;
      const ssize_t count = read(timerFd, &ticks, sizeof(ticks));
      if (count < 0 && errno != EAGAIN && errno != EINTR) throw std::runtime_error("timer read");
    }
    // Existing clients first, before adding newly accepted descriptors.
    for (size_t i = clients.size(); i-- > 0;) {
      const auto c = clients[i];
      if (!fds[i+3].revents && nowMs() < c.deadline) continue;
      char buffer[512];
      ssize_t count = recv(c.fd, buffer, sizeof(buffer), MSG_DONTWAIT | MSG_TRUNC);
      std::string reply = "ERR invalid request";
      if (count > 0 && count < static_cast<ssize_t>(sizeof(buffer))) {
        std::string message(buffer, static_cast<size_t>(count));
        auto v = words(message);
        if (message.find('\0') == std::string::npos && v.size() >= 2 && v[0] == "1" && v[1] == "led")
          reply = board.dispatch(v, c.uid);
        else if (message == "1 list") reply = board.list();
        else reply = dispatch(state, message, c.uid, reporter, backend, dirty);
      }
      (void)send(c.fd, reply.data(), reply.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
      close(c.fd); clients.erase(clients.begin() + i);
    }
    if (fds[0].revents & POLLIN) {
      // Bound acceptance per iteration so a busy client cannot starve timers.
      for (unsigned n = 0; n < 16; ++n) {
        int fd = accept4(listener, nullptr, nullptr, SOCK_CLOEXEC | SOCK_NONBLOCK);
        if (fd < 0) break;
        ucred peer = {}; socklen_t length = sizeof(peer);
        if (clients.size() >= 32 || getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &peer, &length)) close(fd);
        else clients.push_back({fd, peer.uid, nowMs() + 1000});
      }
    }
  }
  for (const auto& c : clients) close(c.fd);
  // Preserve the final color/kernel blink during shutdown and for restart.
  // No more userspace phase changes occur after the daemon exits.
  unlink(sock.c_str()); close(listener); close(timerFd); close(signalFd); close(lock);
  return 0;
}

static const char usage[] =
    "Usage: hss_ledctl --led rpi-rf-mod [system|override] COLOR [MS]\n"
    "       hss_ledctl --led rpi-rf-mod [system|override] alternate COLOR1 COLOR2 MS1 [MS2]\n"
    "       hss_ledctl --led rpi-rf-mod release|auto|off|stop|status\n"
    "       hss_ledctl --led NAME on|off|status\n"
    "       hss_ledctl --led NAME brightness LEVEL\n"
    "       hss_ledctl --led NAME blink ON_MS [OFF_MS]\n"
    "       hss_ledctl --led NAME trigger TRIGGER\n"
    "       hss_ledctl list\n"
    "       hss_ledctl -h|--help|-V|--version\n\n"
    "Targets: --led NAME is required except for list/help/version.\n"
    "  rpi-rf-mod: the radio module's RGB or legacy LED group.\n"
    "  Other NAME: an individual LED-class device shown by list.\n\n"
    "Radio colors: off, red, green, yellow, blue, magenta, cyan, white.\n"
    "  COLOR [MS]: steady color, or color/off blinking when MS > 0.\n"
    "  alternate: switch between two colors; MS2 defaults to MS1.\n"
    "  override (default): manual pattern; release returns to system/auto.\n"
    "  system: set a boot/shutdown pattern and leave automatic mode.\n"
    "  auto: use CCU status and clear manual overrides.\n"
    "  off/stop: turn the LED off; these do not stop the daemon.\n\n"
    "Individual LEDs: on/off, brightness LEVEL, blink ON_MS [OFF_MS],\n"
    "  trigger NAME, status. Blink requires the kernel timer trigger.\n\n"
    "Every MS argument accepts milliseconds, slow (=500), or fast (=100).\n"
    "  Maximum: 86400000 ms. Alternate/blink require positive durations.\n"
    "  COLOR 0 selects a steady color; unequal phase durations are allowed.\n\n"
    "Commands return after acceptance; status shows pending hardware updates.\n"
    "Control changes require root. The hss_led daemon must be running.\n\n"
    "Examples:\n"
    "  hss_ledctl --led rpi-rf-mod alternate blue red slow\n"
    "  hss_ledctl --led rpi-rf-mod magenta fast\n"
    "  hss_ledctl --led ACT blink fast slow\n";
static std::string makeRequest(std::vector<std::string> v) {
  if (v.empty()) throw std::runtime_error(usage);
  if (v[0] == "--led") {
    if (v.size() < 3 || !ledName(v[1])) throw std::runtime_error("invalid LED target");
    std::string target = v[1];
    v.erase(v.begin(), v.begin() + 2);
    if (target != "rpi-rf-mod") {
      if (radioNode(target)) throw std::runtime_error("use --led rpi-rf-mod for the radio LED");
      unsigned a, b;
      bool valid = (v.size() == 1 && (v[0] == "on" || v[0] == "off" || v[0] == "status")) ||
          (v.size() == 2 && v[0] == "brightness" && number(v[1], a, UINT32_MAX)) ||
          (v.size() == 2 && v[0] == "trigger" && ledName(v[1]));
      if ((v.size() == 2 || v.size() == 3) && v[0] == "blink" &&
          duration(v[1], a) && a && duration(v.size() == 3 ? v[2] : v[1], b) && b) {
        v[1] = std::to_string(a);
        if (v.size() == 2) v.push_back(std::to_string(b));
        else v[2] = std::to_string(b);
        valid = true;
      }
      if (!valid) throw std::runtime_error("invalid scalar LED command (see --help)");
      std::string message = "1 led " + target;
      for (const auto& word : v) message += " " + word;
      return message;
    }
  } else if (v.size() == 1 && v[0] == "list") return "1 list";
  else throw std::runtime_error("missing --led NAME (see --help)");
  std::string operation = "override";
  if (v[0] == "system" || v[0] == "override") { operation = v[0]; v.erase(v.begin()); }
  std::string message;
  if (v.size() == 1 && (v[0] == "status" || v[0] == "release" || v[0] == "auto")) message = "1 " + v[0];
  else {
    Pattern p;
    if (v.size() == 1 && v[0] == "stop") v[0] = "off";
    if (v.size() >= 4 && v.size() <= 5 && v[0] == "alternate") {
      p.a = color(v[1]); p.b = color(v[2]);
      if (!duration(v[3], p.da) || !p.da ||
          !duration(v.size() == 5 ? v[4] : v[3], p.db) || !p.db) throw std::runtime_error("invalid phase duration");
    } else if (v.size() == 1 || v.size() == 2) {
      p.a = p.b = color(v[0]);
      if (v.size() == 2) {
        if (!duration(v[1], p.da)) throw std::runtime_error("invalid blink duration");
        if (p.da && p.a) { p.b = 0; p.db = p.da; }
        else p.da = 0;
      }
    } else throw std::runtime_error("invalid arguments");
    message = "1 " + operation + " " + p.wire();
  }
  return message;
}
static int client(int argc, char** argv) {
  if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
    LedCli::version(std::cout, "hss_ledctl");
    std::cout << "Control the hss_led daemon.\n\n" << usage;
    return 0;
  }
  if (argc == 2 && (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-V")) {
    LedCli::version(std::cout, "hss_ledctl");
    return 0;
  }
  std::string message = makeRequest(std::vector<std::string>(argv + 1, argv + argc));
  std::string reply;
  if (!LedProtocol::request(runtime + "/control", message, reply, 2000)) {
    std::cerr << (reply.empty() ? "LED service unavailable" : reply) << '\n';
    return 1;
  }
  auto v = words(message);
  if (message == "1 status" || message == "1 list" ||
      (v.size() == 4 && v[1] == "led" && v[3] == "status")) std::cout << reply << '\n';
  return 0;
}
void LedController::configureTestPaths() {
#ifdef LED_TEST_BUILD
  administrator = geteuid();
  if (const char* p = std::getenv("LED_TEST_RUNTIME")) runtime = p;
  if (const char* p = std::getenv("LED_TEST_SYSFS")) sysfs = p;
  if (const char* p = std::getenv("LED_TEST_STARTUP")) startup = p;
  if (const char* p = std::getenv("LED_TEST_DISABLED")) disabled = p;
#endif
}
int LedController::run(void (*startStatus)()) { return serve(startStatus); }
int LedController::client(int argc, char** argv) { return ::client(argc, argv); }
