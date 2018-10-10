
export CFLAGS+=-g -O0 -Wall -D_GNU_SOURCE

DISTRO := $(shell ./os_detect)
MODE := $(shell [ $(shell uname -i) == "i386" ] && echo "32" || echo "64")

TESTS := \
	exec_execve \
	exec_execve_blank \
	exec_name \
	file_create \
	file_delete \
	file_rename \
	filter_exclude \
	filter_sessionid \
	login_tty \
	lost_reset \
	netfilter_pkt \
	syscalls_file \
	syscall_module \
	syscall_socketcall \
	user_msg

.PHONY: all test clean

all: 
	@for subdir in $(TESTS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
	chmod +x */test

list:
	@echo
	@echo "Tests"
	@echo "====="
	@echo "$(TESTS)" | tr ' ' '\n'
	@echo

test: all
	@DISTRO="$(DISTRO)"                 \
	 MODE="$(MODE)"                     \
	 TESTS="$(TESTS)"                   \
	 PATH=/usr/bin:/bin:/usr/sbin:/sbin \
	 ./runtests.pl

clean: 
	@for subdir in $(TESTS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
