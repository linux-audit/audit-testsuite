
export CFLAGS+=-g -O0 -Wall -D_GNU_SOURCE

DISTRO := $(shell ./os_detect)
MODE := $(shell [ $(shell uname -i) == "i386" ] && echo "32" || echo "64")

SUBDIRS := \
	exec_execve \
	exec_name \
	file_create \
	file_delete \
	file_rename \
	filter_exclude \
	filter_sessionid \
	login_tty \
	syscalls_file \
	syscall_module \
	syscall_socketcall \
	user_msg

.PHONY: all test clean

all: 
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
	chmod +x */test

test: all
	@DISTRO="$(DISTRO)"                 \
	 MODE="$(MODE)"                     \
	 SUBDIRS="$(SUBDIRS)"               \
	 PATH=/usr/bin:/bin:/usr/sbin:/sbin \
	 ./runtests.pl

clean: 
	@for subdir in $(SUBDIRS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
