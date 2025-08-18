
export CFLAGS+=-g -O0 -Wall -D_GNU_SOURCE

DISTRO := $(shell ./os_detect)
MACHINE := $(shell uname -m)
# NOTE: this assumes that only i386 is 32-bit which is obviously wrong, but
#       until we can test this on other 32-bit ABIs limit MODE=32 to i386
ifeq ($(MACHINE),i386)
MODE := 32
else
MODE := 64
endif

# all of the tests
TESTS := \
	amcast_joinpart \
	backlog_wait_time_actual_reset \
	bpf \
	exec_execve \
	exec_name \
	fanotify \
	file_create \
	file_delete \
	file_permission \
	file_rename \
	filter_exclude \
	filter_exit \
	filter_filetype \
	filter_saddr_fam \
	filter_sessionid \
	io_uring \
	login_tty \
	lost_reset \
	netfilter_pkt \
	signal \
	syscalls_file \
	syscall_module \
	syscall_socketcall \
	time_change \
	user_msg

# apply any ABI restrictions to the tests
ifneq ($(MACHINE),$(filter i386 x86_64,$(MACHINE)))
	TESTS := $(filter-out syscall_socketcall,$(TESTS))
endif

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
	@DISTRO="$(DISTRO)" \
	 MODE="$(MODE)"     \
	 TESTS="$(TESTS)"   \
	 ./runtests.pl

clean: 
	@for subdir in $(TESTS); do \
		(cd $$subdir && $(MAKE) $@) || exit 1; \
	done
