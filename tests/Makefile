
export CFLAGS+=-g -O0 -Wall -D_GNU_SOURCE

DISTRO := $(shell ./os_detect)

TESTS := \
	exec_execve \
	exec_name \
	file_create \
	file_delete \
	file_rename \
	login_tty \
	syscalls_file \
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
