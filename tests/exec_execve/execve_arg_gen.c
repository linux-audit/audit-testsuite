/**
 * audit-testsuite EXECVE test tool
 *
 * Copyright (c) 2016 Red Hat <pmoore@redhat.com>
 * Author: Paul Moore <paul@paul-moore.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BADCHAR		0x20

char **arg_setup(const char *name, unsigned int size)
{
	char **args = NULL;

	args = malloc(sizeof(char *) * (size + 3));
	if (!args)
		exit(1);

	args[0] = strdup("testing");
	if (!args[0])
		exit(1);
	args[1] = strdup(name);
	if (!args[1])
		exit(1);

	return args;
}

char *arg_gen(unsigned int length, char insert)
{
	int iter;
	char val;
	char *buf;

	buf = malloc(length + 1);
	if (!buf)
		exit(1);

	for (iter = 0; iter < length; iter++) {
		if (insert && iter % 2) {
			buf[iter] = insert;
		} else {
			/* ascii: 0x0..0xF */
			val = iter % 0x10;
			buf[iter] = (val > 9 ? 55 + val : 48 + val);
		}
	}
	buf[length] = '\0';

	return buf;
}

int main(int argc, char *argv[])
{
	int rc;
	int iter;
	int test_cfg;
	char **exec_argv = NULL;

	/* check if we are calling ourselves for testing purposes */
	if ((argc >= 1) && (strcmp(argv[0], "testing") == 0))
		return 0;

	/* run a specific test? */
	if (argc == 3) {
		test_cfg = atoi(argv[2]);

		if (strcmp(argv[1], "count") == 0) {
			exec_argv = arg_setup("count", test_cfg);
			for (iter = 0; iter < test_cfg; iter++)
				exec_argv[iter + 2] = arg_gen(1, 0);
			exec_argv[test_cfg + 2] = NULL;
		} else if (strcmp(argv[1], "size") == 0) {
			exec_argv = arg_setup("size", 1);
			exec_argv[2] = arg_gen(test_cfg, 0);
			exec_argv[3] = NULL;
		} else if (strcmp(argv[1], "hex") == 0) {
			exec_argv = arg_setup("hex", 1);
			exec_argv[2] = arg_gen(test_cfg, BADCHAR);
			exec_argv[3] = NULL;
		}

		rc = execve(argv[0], exec_argv, NULL);
		return (rc < 0 ? errno : 0);
	}

	/* no idea what we were supposed to do */
	return 2;
}
