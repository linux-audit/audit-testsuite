/*
 * audit-testsuite OPENAT2 test tool
 *
 * Copyright (c) 2021 Red Hat <rgb@redhat.com>
 * Author: Richard Guy Briggs <rgb@tricolour.ca>
 */

/*
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses>.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* linux/openat2.h not available yet on travis-ci.org */
#if 0
#include <linux/openat2.h>
#else
#include <linux/types.h>
struct open_how {
	__u64 flags;
	__u64 mode;
	__u64 resolve;
};
#define RESOLVE_NO_MAGICLINKS	0x02
#define RESOLVE_BENEATH		0x08
#endif

#include <sys/syscall.h>

#ifndef __NR_openat2
/* on all current Linux ABIs openat2() is 437 */
#define __NR_openat2		437
#endif

int main(int argc, char **argv)
{
	int dir_fd, file_fd;
	char *test_dir = argv[1] ? : ".";
	char *file_name = argv[2] ? : "file-openat2";
	struct open_how how = {
		.flags = O_CREAT | O_RDWR | O_EXCL,
		.mode = 0600,
		.resolve = RESOLVE_BENEATH | RESOLVE_NO_MAGICLINKS,
	};

	dir_fd = open(test_dir, O_RDONLY | O_DIRECTORY);
	if (dir_fd == -1) {
		printf("cannot open directory %s\n", test_dir);
		return 1;
	}

	printf("create file=%s/%s with"
	       " flags=0%llo, mode=0%llo, resolve=0x%0llx\n",
	       test_dir, file_name, how.flags, how.mode, how.resolve);
	file_fd = syscall(__NR_openat2, dir_fd, file_name, &how, sizeof(how));
	if (file_fd == -1) {
		if (errno == ENOSYS) {
			printf("openat2 syscall not supported\n");
			return 2;
		}
		printf("file creation failed\n");
		return 1;
	}
	close(file_fd);
	close(dir_fd);
	return 0;
}
