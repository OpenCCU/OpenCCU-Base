// SPDX-License-Identifier: Apache-2.0
#ifndef HSS_STATUS_COMMAND_H
#define HSS_STATUS_COMMAND_H
#include <cerrno>
#include <csignal>
#include <spawn.h>
#include <sys/wait.h>
extern char** environ;

// system() changes process-wide SIGINT/SIGQUIT dispositions, interfering with
// the controller's signalfd. Spawn with an empty child signal mask instead;
// helper programs must not inherit the daemon's blocked termination signals.
inline int runStatusCommand(const char* command) {
  posix_spawnattr_t attr;
  int error = posix_spawnattr_init(&attr);
  if (error) { errno = error; return -1; }
  sigset_t mask;
  sigemptyset(&mask);
  error = posix_spawnattr_setsigmask(&attr, &mask);
  if (!error) error = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);
  pid_t child = -1;
  char* args[] = {const_cast<char*>("sh"), const_cast<char*>("-c"),
                  const_cast<char*>(command), nullptr};
  if (!error) error = posix_spawn(&child, "/bin/sh", nullptr, &attr, args, environ);
  posix_spawnattr_destroy(&attr);
  if (error) { errno = error; return -1; }
  int status;
  while (waitpid(child, &status, 0) < 0) {
    if (errno != EINTR) return -1;
  }
  return status;
}
#endif
