/*
 * eq3_char_loop restart test
 *
 * Simulates a multimacd restart (master closes and a new master re-creates the
 * channel) while a slave (rfd / HMIPServer) keeps its slave device open, and
 * checks that the driver hangs up the stale slave connection instead of
 * silently re-attaching it to the new master.
 *
 * Build:  gcc -O2 -Wall -o eq3loop_restart_test eq3loop_restart_test.c
 * Run:    insmod eq3_char_loop.ko && ./eq3loop_restart_test   (as root)
 *
 * Best run in a VM: it only uses its own "mmd_test" channel, but against an
 * unpatched driver step 5 leaves an unkillable process spinning in the kernel.
 */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define EQ3LOOP_IOC_MAGIC 'L'
#define EQ3LOOP_IOCSCREATESLAVE _IOW(EQ3LOOP_IOC_MAGIC, 1, uint32_t)
#define EQ3LOOP_IOCGEVENTS _IOR(EQ3LOOP_IOC_MAGIC, 2, uint32_t)

#define SLAVE "mmd_test"
#define SLAVEDEV "/dev/" SLAVE

static int fails;
#define CHECK(cond, ...) do { int ok_ = !!(cond); printf("%s: ", ok_ ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); if (!ok_) fails++; } while (0)

static int new_master(void)
{
	int fd = open("/dev/eq3loop", O_RDWR | O_NONBLOCK); /* like multimacd */
	if (fd < 0 || ioctl(fd, EQ3LOOP_IOCSCREATESLAVE, SLAVE)) {
		perror("create master");
		return -1;
	}
	usleep(200000); /* let devtmpfs create the node */
	return fd;
}

static unsigned long events(int m)
{
	unsigned long ev = 0;
	if (ioctl(m, EQ3LOOP_IOCGEVENTS, &ev))
		perror("IOCGEVENTS");
	return ev;
}

int main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	char buf[256];
	ssize_t r;
	int m, s, s2, i, err;
	struct pollfd pfd;
	struct termios tio;

	printf("=== 1. normal operation ===\n");
	m = new_master();
	s = open(SLAVEDEV, O_RDWR | O_NOCTTY | O_NONBLOCK); /* like RXTX */
	CHECK(s >= 0, "open slave (%s)", s < 0 ? strerror(errno) : "ok");
	CHECK(events(m) & 1, "master sees SLAVE_OPENED");
	CHECK(write(s, "hello", 5) == 5, "slave write");
	r = read(m, buf, sizeof(buf));
	CHECK(r == 5 && !memcmp(buf, "hello", 5), "master reads slave data (%zd)", r);

	printf("=== 2. multimacd stops, slave keeps its fd ===\n");
	close(m);
	r = write(s, "x", 1); err = errno;
	CHECK(r < 0 && err == ENODEV, "slave write after master close -> %zd (%s)", r, r < 0 ? strerror(err) : "accepted");

	printf("=== 3. multimacd restarts (new master), stale slave fd still open ===\n");
	m = new_master();
	CHECK(m >= 0, "new master created channel");
	pfd.fd = s; pfd.events = POLLIN | POLLOUT; pfd.revents = 0;
	poll(&pfd, 1, 0);
	CHECK(pfd.revents & POLLHUP, "stale slave poll reports POLLHUP (revents=0x%x)", pfd.revents);
	for (i = 0; i < 20; i++) {
		r = write(s, buf, 100); err = errno;
		if (r < 0)
			break;
	}
	CHECK(i == 0 && r < 0 && err == ENODEV, "stale slave write -> %d frames accepted into a channel nobody reads, then %s",
	      i, r < 0 ? strerror(err) : "still accepted");
	r = read(s, buf, sizeof(buf)); err = errno;
	CHECK(r < 0 && err == ENODEV, "stale slave read -> %zd (%s)", r, r < 0 ? strerror(err) : "data");
	r = ioctl(s, TIOCINQ, &i); err = errno;
	CHECK(r < 0 && err == ENODEV, "stale slave ioctl(TIOCINQ) -> %zd (%s)", r, r < 0 ? strerror(err) : "ok");
	memset(&tio, 0, sizeof(tio));
	r = ioctl(s, TCGETS, &tio); err = errno;
	CHECK(r < 0 && err == ENODEV, "stale slave ioctl(TCGETS) -> %zd (%s)", r, r < 0 ? strerror(err) : "ok");
	r = ioctl(s, TCSETS, &tio); err = errno;
	CHECK(r < 0 && err == ENODEV, "stale slave ioctl(TCSETS) -> %zd (%s)", r, r < 0 ? strerror(err) : "ok");
	r = read(m, buf, sizeof(buf));
	printf("INFO: new master read() -> %zd (%s), events=0x%lx\n", r, r < 0 ? strerror(errno) : "-", events(m));

	printf("=== 4. slave closes and re-opens ===\n");
	close(s);
	s2 = open(SLAVEDEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
	CHECK(s2 >= 0, "re-open slave (%s)", s2 < 0 ? strerror(errno) : "ok");
	CHECK(events(m) & 1, "new master sees SLAVE_OPENED");
	CHECK(write(s2, "world", 5) == 5, "slave write after re-open");
	r = read(m, buf, sizeof(buf));
	CHECK(r == 5 && !memcmp(buf, "world", 5), "new master reads data of re-opened slave (%zd)", r);
	CHECK(write(m, "pong", 4) == 4 && read(s2, buf, sizeof(buf)) == 4, "master -> slave direction works");
	close(s2);
	close(m);

	printf("=== 5. blocking slave writer on a full buffer, master closes ===\n");
	m = new_master();
	s = open(SLAVEDEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
	while (write(s, buf, 100) == 100)
		;
	while (write(s, buf, 1) == 1)
		;
	pid_t pid = fork();
	if (pid == 0) {
		close(m);
		fcntl(s, F_SETFL, 0); /* blocking */
		r = write(s, buf, 1);
		_exit(r < 0 && errno == ENODEV ? 0 : 1);
	}
	usleep(300000);
	close(m);
	int status = -1;
	for (i = 0; i < 30; i++) {
		if (waitpid(pid, &status, WNOHANG) == pid)
			break;
		usleep(100000);
	}
	if (i == 30) {
		kill(pid, SIGKILL);
		usleep(500000);
		CHECK(0, "blocked writer did NOT return within 3s after master close, %s",
		      waitpid(pid, &status, WNOHANG) == pid ? "only SIGKILL released it" : "and it is not even killable (busy loop in kernel)");
	} else {
		CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0, "blocked writer woke up with ENODEV");
	}
	close(s);

	printf("=== RESULT: %d failure(s) ===\n", fails);
	return fails ? 1 : 0;
}
