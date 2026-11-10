// SPDX-License-Identifier: Apache-2.0
// Exercise the actual parser/state/backend code without creating sockets.
// Build: c++ -std=c++11 -Wall -Wextra -Werror tests.cpp -o led-state-test
#include "../LedController.cpp"
#include "../RgbLed.h"
#include "../StatusCommand.h"

static unsigned checks = 0;
static void check(bool ok) {
  ++checks;
  if (!ok) throw std::runtime_error("check " + std::to_string(checks) + " failed");
}
static void put(const std::string& path, const std::string& value) {
  std::ofstream f(path);
  f << value << '\n';
  check(bool(f));
}
static void directory(const std::string& path) { check(!mkdir(path.c_str(), 0755)); }
int main() {
  char temp[] = "/tmp/led-state-test-XXXXXX";
  const char* dir = mkdtemp(temp);
  if (!dir) return 1;
  try {
    runtime = std::string(dir) + "/run"; directory(runtime);
    sysfs = std::string(dir) + "/leds"; directory(sysfs);
    startup = std::string(dir) + "/started";
    disabled = std::string(dir) + "/disabled";
    sigset_t blocked, previous;
    sigemptyset(&blocked); sigaddset(&blocked, SIGTERM);
    check(!pthread_sigmask(SIG_BLOCK, &blocked, &previous));
    int child = runStatusCommand("exit 7");
    check(child >= 0 && WIFEXITED(child) && WEXITSTATUS(child) == 7);
    child = runStatusCommand("kill -TERM $$; exit 1");
    check(child >= 0 && WIFSIGNALED(child) && WTERMSIG(child) == SIGTERM);
    check(!pthread_sigmask(SIG_SETMASK, &previous, nullptr));
    RgbLed producer;
    producer.set(4, 1, 499);
    Pattern received;
    check(reports.take(received) && received.wire() == "4 1 499 499");
    check(!reports.take(received));
    producer.set(1, 0, 0);
    producer.set(2, 0, 0);
    check(reports.take(received) && received.wire() == "2 2 0 0");
    producer.set(8, 0, 10);
    check(!reports.take(received));
    Pattern p;
    p.a = 4; p.b = 1; p.da = 250; p.db = 750;
    uint64_t delay = 0;
    check(phaseAt(p, 0, delay) == 4 && delay == 250);
    check(phaseAt(p, 249, delay) == 4 && delay == 1);
    check(phaseAt(p, 250, delay) == 1 && delay == 750);
    check(phaseAt(p, 999, delay) == 1 && delay == 1);
    check(phaseAt(p, 1000, delay) == 4 && delay == 250);
    check(phaseAt(p, 1000000350ULL, delay) == 1 && delay == 650);
    check(makeRequest({"--led", "rpi-rf-mod", "alternate", "blue", "red", "250", "750"}) == "1 override 4 1 250 750");
    check(makeRequest({"--led", "rpi-rf-mod", "system", "magenta", "100"}) == "1 system 5 0 100 100");
    check(makeRequest({"--led", "rpi-rf-mod", "blue"}) == "1 override 4 4 0 0");
    check(makeRequest({"--led", "rpi-rf-mod", "off", "100"}) == "1 override 0 0 0 0");
    check(makeRequest({"--led", "rpi-rf-mod", "release"}) == "1 release");
    check(makeRequest({"--led", "rpi-rf-mod", "alternate", "green", "yellow", "000499"}) == "1 override 2 3 499 499");
    check(makeRequest({"--led", "rpi-rf-mod", "#FF8000"}) == "1 override #ff8000 #ff8000 0 0");
    check(makeRequest({"--led", "rpi-rf-mod", "#00ff00", "slow"}) == "1 override 2 0 500 500");
    check(makeRequest({"--led", "rpi-rf-mod", "system", "alternate", "#102030", "red", "fast"}) ==
          "1 system #102030 1 100 100");
    check(makeRequest({"--led", "rpi-rf-mod", "#000000", "100"}) == "1 override 0 0 0 0");
    for (auto args : std::vector<std::vector<std::string>>{{}, {"invalid"}, {"blue", "-1"}, {"#ff80"},
         {"ff8000"}, {"#ff8000x"}, {"#gg8000"}, {"4"}, {"brightness", "50"},
         {"blue", "99999999999999999"}, {"alternate", "red", "blue", "0"},
         {"alternate", "red", "blue", "10", "0"}, {"blue", "10", "20"}, {"system"}}) {
      bool rejected = false;
      args.insert(args.begin(), {"--led", "rpi-rf-mod"});
      try { (void)makeRequest(args); } catch (const std::runtime_error&) { rejected = true; }
      check(rejected);
    }
    for (auto args : std::vector<std::vector<std::string>>{
         {"blue"}, {"status"}, {"release"}, {"auto"}, {"off"},
         {"alternate", "blue", "red", "250"}, {"system", "yellow"}, {"list", "extra"}}) {
      bool rejected = false;
      try { (void)makeRequest(args); }
      catch (const std::runtime_error& e) { rejected = std::string(e.what()).find("missing --led") != std::string::npos; }
      check(rejected);
    }
    check(makeRequest({"--led", "rpi-rf-mod", "blue", "slow"}) == "1 override 4 0 500 500");
    check(makeRequest({"--led", "rpi-rf-mod", "system", "magenta", "fast"}) == "1 system 5 0 100 100");
    check(makeRequest({"--led", "rpi-rf-mod", "alternate", "blue", "red", "slow", "fast"}) == "1 override 4 1 500 100");
    check(makeRequest({"--led", "rpi-rf-mod", "alternate", "green", "yellow", "fast", "750"}) == "1 override 2 3 100 750");
    check(makeRequest({"--led", "ACT", "blink", "slow"}) == "1 led ACT blink 500 500");
    check(makeRequest({"--led", "ACT", "blink", "fast", "slow"}) == "1 led ACT blink 100 500");
    check(makeRequest({"--led", "ACT", "blink", "250", "fast"}) == "1 led ACT blink 250 100");
    for (auto args : std::vector<std::vector<std::string>>{
         {"--led", "rpi-rf-mod", "blue", "Slow"}, {"--led", "ACT", "blink", "quick"},
         {"--led", "ACT", "brightness", "slow"}, {"--led", "rpi-rf-mod", "alternate", "blue", "red", "fast", "0"}}) {
      bool rejected = false;
      try { (void)makeRequest(args); } catch (const std::runtime_error&) { rejected = true; }
      check(rejected);
    }
    State state;
    Backend backend;
    check(backend.discover() && backend.name == "missing");
    auto command = [&](const std::string& msg, uid_t uid = 0) {
      return dispatch(state, msg, uid, 1234, backend, false);
    };
    check(command("1 report 4 4 0 0", 1234) == "OK accepted");
    check(state.effective().a == maskRgb(3));
    put(startup, "");
    check(command("1 auto") == "OK accepted" && state.effective().a == maskRgb(4));
    check(command("1 override 2 3 250 750") == "OK accepted" && state.effective().a == maskRgb(2));
    check(command("1 report 1 1 0 0", 1234) == "OK accepted" && state.effective().a == maskRgb(2));
    State restored;
    check(restored.load(readText(runtime + "/state")) && restored.wire() == state.wire());
    check(command("1 release") == "OK accepted" && state.effective().a == maskRgb(1));
    // Free colors: hex on the wire, a name's exact value keeps its number.
    check(command("1 override #ff8000 #FF8000 0 0") == "OK accepted" && state.effective().a == 0xFF8000);
    check(state.effective().wire() == "#ff8000 #ff8000 0 0");
    check(restored.load(readText(runtime + "/state")) && restored.wire() == state.wire());
    check(command("1 override #0000ff 0 500 500") == "OK accepted" && state.effective().wire() == "4 0 500 500");
    check(command("1 override 4 1 499 499") == "OK accepted");
    unlink(startup.c_str());
    check(state.effective().a == maskRgb(3));
    check(command("1 system 3 3 0 0") == "OK accepted" && !state.overrideActive && !state.automatic);
    check(command("1 report 4 4 0 0", 1234) == "OK accepted" && state.effective().a == maskRgb(3));
    check(command("1 status").find(" disabled=0 ") != std::string::npos);
    check(command("1 status").find(" brightness=100 dimmable=0") != std::string::npos);
    put(disabled, ""); check(state.effective().a == 0);
    check(command("1 status").find(" disabled=1 pattern=0 0 0 0") != std::string::npos);
    unlink(disabled.c_str());
    std::string before = state.wire();
    for (const auto& msg : {"1 override 9 0 10 10", "1 report 1 2 0 0", "1 auto extra", "2 release", "1 system 1 2 0 1",
                            "1 override #ff800 #ff800 0 0", "1 override #gg8000 0 10 10", "1 override ff8000 0 10 10",
                            "1 override #ff80000 0 10 10", "1 override -1 0 10 10"})
      check(command(msg).compare(0, 3, "ERR") == 0 && state.wire() == before);
    check(command(std::string("1 auto\0", 7)).compare(0, 3, "ERR") == 0);
    check(command("1 override 1 1 0 0", 1234) == "ERR permission denied");
    check(command("1 report 1 1 0 0", 5678) == "ERR permission denied");
    check(command("1 status", 5678).compare(0, 2, "OK") == 0);
    std::string savedRuntime = runtime;
    runtime += "/absent";
    check(command("1 override 7 7 0 0") == "ERR cannot save runtime state" && state.wire() == before);
    runtime = savedRuntime;
    std::string rgb = sysfs + "/rpi_rf_mod:rgb:status";
    directory(rgb);
    for (auto a : {"trigger", "brightness", "multi_intensity", "delay_on", "delay_off"}) put(rgb + "/" + a, "0");
    put(rgb + "/trigger", "[none] timer"); put(rgb + "/multi_index", "blue red green"); put(rgb + "/max_brightness", "255");
    check(backend.discover() && backend.name == "rgb");
    check(backend.dimmable());
    for (unsigned mask = 0; mask < 8; ++mask) {
      Pattern solid; solid.a = solid.b = maskRgb(mask);
      check(backend.apply(solid, solid.a, 100));
      check(readText(rgb + "/multi_intensity") == std::to_string(mask & 4 ? 255 : 0) + " " +
          std::to_string(mask & 1 ? 255 : 0) + " " + std::to_string(mask & 2 ? 255 : 0) + "\n");
    }
    Pattern blink; blink.a = maskRgb(5); blink.da = blink.db = 100;
    check(backend.kernelBlink(blink) && backend.apply(blink, blink.a, 100));
    check(readText(rgb + "/trigger") == "timer\n" && readText(rgb + "/delay_on") == "100\n");
    // A timer attribute may not be writable until udev handles TRIGGER.
    // Retrying must preserve the active trigger and its current off phase.
    // The backend must use the configured on-color even with an off-phase mask.
    unlink((rgb + "/delay_on").c_str());
    check(!backend.apply(blink, 0, 100));
    put(rgb + "/brightness", "0");
    put(rgb + "/delay_on", "0");
    check(backend.apply(blink, 0, 100));
    check(readText(rgb + "/multi_intensity") == "255 255 0\n");
    check(readText(rgb + "/trigger") == "timer\n");
    check(readText(rgb + "/brightness") == "0\n");
    Pattern q; q.a = maskRgb(4); q.b = maskRgb(1); q.da = 250; q.db = 750;
    check(backend.apply(q, q.a, 100) && backend.setColor(q.b, 100));
    check(readText(rgb + "/multi_intensity") == "0 255 0\n");
    check(readText(rgb + "/brightness") == "255\n");
    // Mixed and dimmed colors (order blue red green): gamma 2.2, 3 % floor.
    check(level(255, 100, 255) == 255 && level(128, 100, 255) == 56 && level(1, 100, 255) == 8);
    check(level(255, 50, 255) == 55 && level(255, 1, 255) == 8 && level(0, 100, 255) == 0);
    check(level(127, 100, 1) == 0 && level(128, 100, 1) == 1 && level(255, 1, 1) == 1);
    check(backend.setColor(0xFF8000, 100) && readText(rgb + "/multi_intensity") == "0 255 56\n");
    check(backend.setColor(0xFF8000, 50) && readText(rgb + "/multi_intensity") == "0 55 12\n");
    check(backend.setColor(0x000000, 50) && readText(rgb + "/multi_intensity") == "0 0 0\n");
    // A group without levels (GPIO LEDs, adapters) shows the nearest name.
    // A new device (inode) is rediscovered; max_brightness is read then.
    auto replaceGroup = [&](const char* maximum) {
      unlink((rgb + "/multi_index").c_str()); check(backend.discover() && backend.name == "missing");
      put(rgb + "/multi_index", "blue red green"); put(rgb + "/max_brightness", maximum);
    };
    replaceGroup("1");
    check(backend.discover() && backend.name == "rgb" && !backend.dimmable());
    check(backend.setColor(0xFF8000, 30) && readText(rgb + "/multi_intensity") == "0 1 1\n");
    check(backend.setColor(0x7F7FFF, 30) && readText(rgb + "/multi_intensity") == "1 0 0\n");
    replaceGroup("255");
    check(backend.discover() && backend.dimmable());
    // The brightness setting: 1-100, anything else is 100.
    brightnessFile = std::string(dir) + "/brightness";
    check(!loadBrightness() && brightnessPercent == 100);
    put(brightnessFile, "40");
    check(loadBrightness() && brightnessPercent == 40 && !loadBrightness());
    check(command("1 status").find(" brightness=40 dimmable=1") != std::string::npos);
    for (auto invalid : {"0", "101", "abc", "40 50", ""}) {
      put(brightnessFile, invalid);
      (void)loadBrightness();
      check(brightnessPercent == 100);
      put(brightnessFile, "40"); check(loadBrightness() && brightnessPercent == 40);
    }
    unlink(brightnessFile.c_str());
    check(loadBrightness() && brightnessPercent == 100);
    writeBrightness("25");
    check(readText(brightnessFile) == "25\n" && loadBrightness() && brightnessPercent == 25);
    for (auto invalid : {"0", "101", "-1", "x"}) {
      bool rejected = false;
      try { writeBrightness(invalid); } catch (const std::runtime_error&) { rejected = true; }
      check(rejected && readText(brightnessFile) == "25\n");
    }
    unlink(brightnessFile.c_str()); check(loadBrightness() && brightnessPercent == 100);
    for (auto a : {"trigger", "brightness", "multi_intensity", "delay_on", "delay_off", "multi_index", "max_brightness"})
      unlink((rgb + "/" + a).c_str());
    rmdir(rgb.c_str()); check(backend.discover() && backend.name == "missing");
    for (auto c : {"red", "green", "blue"}) {
      std::string path = sysfs + "/rpi_rf_mod:" + c;
      directory(path); put(path + "/trigger", "none"); put(path + "/brightness", "0"); put(path + "/max_brightness", "1");
    }
    check(backend.discover() && backend.name == "legacy" && !backend.dimmable());
    check(backend.apply(q, maskRgb(2), 100)); check(backend.setColor(maskRgb(3), 100));
    check(readText(sysfs + "/rpi_rf_mod:red/brightness") == "1\n" &&
          readText(sysfs + "/rpi_rf_mod:green/brightness") == "1\n" &&
          readText(sysfs + "/rpi_rf_mod:blue/brightness") == "0\n");
    check(backend.setColor(0x2080FF, 10));
    check(readText(sysfs + "/rpi_rf_mod:red/brightness") == "0\n" &&
          readText(sysfs + "/rpi_rf_mod:green/brightness") == "1\n" &&
          readText(sysfs + "/rpi_rf_mod:blue/brightness") == "1\n");
    directory(rgb); put(rgb + "/multi_index", "red red blue"); put(rgb + "/max_brightness", "255");
    check(backend.discover() && backend.name == "error" && !backend.apply(q, maskRgb(7), 100));
    check(!restored.load("1 0 0 broken"));
    check(makeRequest({"--led", "rpi-rf-mod", "alternate", "blue", "red", "250"}) == "1 override 4 1 250 250");
    check(makeRequest({"--led", "ACT", "blink", "120"}) == "1 led ACT blink 120 120");
    check(makeRequest({"--led", "blue:status", "blink", "100", "900"}) == "1 led blue:status blink 100 900");
    check(makeRequest({"--led", "green:", "trigger", "heartbeat"}) == "1 led green: trigger heartbeat");
    check(makeRequest({"list"}) == "1 list");
    for (auto args : std::vector<std::vector<std::string>>{
         {"--led"}, {"--led", "ACT"}, {"--led", "../ACT", "on"},
         {"--led", "/sys/class/leds/ACT", "on"}, {"--led", "ACT x", "on"},
         {"--led", "rpi_rf_mod:red", "on"}, {"--led", "ACT", "green"},
         {"--led", "ACT", "blink", "0"}, {"--led", "ACT", "blink", "10", "0"},
         {"--led", "ACT", "brightness", "4294967296"}, {"--led", "ACT", "trigger", "x y"}}) {
      bool rejected = false;
      try { (void)makeRequest(args); } catch (const std::runtime_error&) { rejected = true; }
      check(rejected);
    }
    BoardLeds board;
    auto boardCommand = [&](const std::string& msg, uid_t uid = 0) { return board.dispatch(words(msg), uid); };
    for (auto name : {"ACT", "green:", "blue:status"}) {
      std::string path = sysfs + "/" + name;
      directory(path); put(path + "/brightness", "1"); put(path + "/max_brightness", "255");
      put(path + "/trigger", "none timer [heartbeat]");
      const std::string command = std::string("1 led ") + name;
      check(boardCommand(command + " status", 5678).find("trigger=heartbeat") != std::string::npos);
      check(boardCommand(command + " on", 5678) == "ERR permission denied");
      check(boardCommand(command + " brightness 256") == "ERR invalid LED command");
      check(boardCommand(command + " trigger missing") == "ERR trigger unavailable");
      check(readText(path + "/trigger") == "none timer [heartbeat]\n");
      check(boardCommand(command + " off") == "OK accepted");
      check(readText(path + "/brightness") == "0\n" && readText(path + "/trigger") == "none\n");
      check(boardCommand(command + " on") == "OK accepted" && readText(path + "/brightness") == "255\n");
      check(boardCommand(command + " brightness 42") == "OK accepted" && readText(path + "/brightness") == "42\n");
      put(path + "/trigger", "[none] timer heartbeat");
      check(boardCommand(command + " blink 100 900") == "OK accepted");
      check(boardCommand(command + " status").find("pending=1") != std::string::npos);
      put(path + "/brightness", "0");
      put(path + "/delay_on", "0"); put(path + "/delay_off", "0");
      board.tick(nowMs());
      check(boardCommand(command + " status").find("pending=0") != std::string::npos);
      check(readText(path + "/delay_on") == "100\n" && readText(path + "/delay_off") == "900\n");
      check(readText(path + "/brightness") == "0\n");
      put(path + "/trigger", "none [timer] heartbeat");
      check(boardCommand(command + " trigger heartbeat") == "OK accepted");
      check(readText(path + "/trigger") == "heartbeat\n");
    }
    check(board.list().find("ACT\tscalar max_brightness=255") != std::string::npos);
    check(board.list().find("blue:status\tscalar") != std::string::npos);
    check(board.list().find("rpi_rf_mod:") == std::string::npos);
    check(boardCommand("1 led ../ACT on") == "ERR invalid LED request");
    check(boardCommand("1 led rpi_rf_mod:red on").compare(0, 3, "ERR") == 0);
    check(boardCommand("1 led absent on") == "ERR scalar LED unavailable or unsupported");
    std::string act = sysfs + "/ACT";
    put(act + "/multi_index", "red green blue");
    check(boardCommand("1 led ACT on") == "ERR scalar LED unavailable or unsupported");
    unlink((act + "/multi_index").c_str());
    // Permission retry times out without disrupting other LEDs or RGB state.
    put(act + "/trigger", "[none] timer heartbeat"); unlink((act + "/delay_off").c_str());
    check(boardCommand("1 led ACT blink 20 30") == "OK accepted");
    board.tick(nowMs() + 6000);
    check(boardCommand("1 led ACT status").find("error=timer-attributes-not-writable") != std::string::npos);
    // An explicit replacement cancels pending writes from an earlier command.
    check(boardCommand("1 led ACT blink 10 20") == "OK accepted");
    check(boardCommand("1 led ACT off") == "OK accepted");
    put(act + "/delay_off", "888");
    board.tick(nowMs());
    check(readText(act + "/delay_off") == "888\n" && readText(act + "/brightness") == "0\n");
    // Do not reapply a pending command when a device or external trigger changes.
    put(act + "/trigger", "[none] timer heartbeat"); unlink((act + "/delay_off").c_str());
    check(boardCommand("1 led ACT blink 10 20") == "OK accepted");
    put(act + "/trigger", "none timer [heartbeat]");
    board.tick(nowMs());
    check(boardCommand("1 led ACT status").find("error=device-or-trigger-changed") != std::string::npos);
    put(act + "/trigger", "[none] timer heartbeat");
    check(boardCommand("1 led ACT blink 10 20") == "OK accepted");
    check(!rename((act + "/brightness").c_str(), (act + "/old-brightness").c_str()));
    put(act + "/brightness", "0"); board.tick(nowMs());
    check(boardCommand("1 led ACT status").find("error=device-or-trigger-changed") != std::string::npos);
    check(state.wire() == before);
    std::cout << "PASS: " << checks << " state/protocol/phase/backend assertions\n";
    // The temporary fixture is printed so the test runner can remove it.
    std::cout << "fixture=" << dir << '\n';
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << " (fixture " << dir << ")\n"; return 1;
  }
}
