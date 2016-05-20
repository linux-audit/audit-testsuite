
export CFLAGS+=-g -O0 -Wall -D_GNU_SOURCE

DISTRO = $(shell ./os_detect)

SUBDIRS := \
	filter_exclude \
	exec_name \
	file_create \
	file_delete \
	file_rename \
	syscalls_file \
	login_tty \
	user_msg

.PHONY: all test clean

all: 
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
	chmod +x */test

test: all
	@SUBDIRS="$(SUBDIRS)" PATH=/usr/bin:/bin:/usr/sbin:/sbin ./runtests.pl

clean: 
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
