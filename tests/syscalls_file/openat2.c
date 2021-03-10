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

/* not available yet on travis-ci.org
 * #include <linux/openat2.h>
 */
struct open_how {
	long long unsigned flags;
	long long unsigned mode;
	long long unsigned resolve;
};
#define RESOLVE_NO_MAGICLINKS	0x02
#define RESOLVE_BENEATH		0x08

#include <sys/syscall.h>

int main(int argc, char **argv)
{
	char *test_dir = argv[1] ? : ".";
	int dir_fd, file_fd;
	char *file_name = argv[2] ? : "file-openat2";
	struct open_how how = { .flags = O_CREAT | O_RDWR | O_EXCL,
		       .mode = 0600,
		       .resolve = RESOLVE_BENEATH | RESOLVE_NO_MAGICLINKS,
	};

	dir_fd = open(test_dir, O_RDONLY | O_DIRECTORY);
	if (dir_fd == -1) {
		printf("cannot open directory %s\n", test_dir);
		return 1;
	}

	/* Try to create a file.  */
	printf("create file=%s in directory=%s with flags=0%llo, mode=0%llo, resolve=0x%0llx\n",
	       file_name, test_dir, how.flags, how.mode, how.resolve);
	file_fd = syscall(437, dir_fd, file_name, &how, sizeof(how)); //__NR_openat2
	if (file_fd == -1) {
		if (errno == ENOSYS) {
			printf("openat2 function not supported\n");
			return 2;
		}
		printf("file creation failed\n");
		return 1;
	}
	close(file_fd);
	close(dir_fd);
	return 0;
}
