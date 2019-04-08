#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

static int gettimeofday_syscall(struct timeval *tv)
{
	return syscall(SYS_gettimeofday, tv, NULL);
}

static int settimeofday_syscall(const struct timeval *tv)
{
	return syscall(SYS_settimeofday, tv, NULL);
}

int main(int argc, char **argv)
{
	const char *mode;
	struct timespec ts, ts_orig;
	struct timeval tv, tv_orig;
	int ret;

	if (argc != 4) {
		fprintf(stderr, "%s: wrong number of arguments\n", argv[0]);
		return 1;
	}

	mode = argv[1];

	tv.tv_sec = (time_t)atoll(argv[2]);
	tv.tv_usec = (suseconds_t)atol(argv[3]);

	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = (long)tv.tv_usec * 1000;

	/*
	 * NOTE: This will shift the system clock backwards a bit, but NTP
	 * should fix it after a while...
	 */
	if (strcmp(mode, "posix") == 0) {
		ret = clock_gettime(CLOCK_REALTIME, &ts_orig);
		if (ret) {
			perror("clock_gettime");
			return 1;
		}
		ret = clock_settime(CLOCK_REALTIME, &ts);
		if (ret) {
			perror("clock_settime (set)");
			return 1;
		}
		ret = clock_settime(CLOCK_REALTIME, &ts_orig);
		if (ret) {
			perror("clock_settime (reset)");
			return 1;
		}
	} else if (strcmp(mode, "settimeofday") == 0) {
		ret = gettimeofday_syscall(&tv_orig);
		if (ret) {
			perror("gettimeofday");
			return 1;
		}
		ret = settimeofday_syscall(&tv);
		if (ret) {
			perror("settimeofday (set)");
			return 1;
		}
		ret = settimeofday_syscall(&tv_orig);
		if (ret) {
			perror("settimeofday (reset)");
			return 1;
		}
	} else {
		fprintf(stderr, "%s: invalid mode: %s\n", argv[0], mode);
		return 1;
	}
	return 0;
}
