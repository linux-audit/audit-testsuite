Basic Audit Regression Test Suite for the Linux Kernel
===============================================================================
https://github.com/linux-audit/audit-testsuite

[![Build Status](https://img.shields.io/travis/linux-audit/audit-testsuite/master.svg)](https://travis-ci.org/linux-audit/audit-testsuite)

The audit-testsuite project provides a simple, self-contained regression test
suite for the Linux Kernel's audit subsystem.

## Online Resources

The test suite's source repository currently lives on GitHub at the following
URL:

* https://github.com/linux-audit/audit-testsuite

## Installation

The audit-testsuite requires the audit userspace, a C compiler (gcc), Perl 5,
and some additional Perl modules (see the list below). Some tests also require
32-bit glibc packages.

To install all these dependencies on your distribution on x86_64 architecture
please follow the instructions below.

### RHEL / Centos

	# yum install audit \
	              gcc \
	              glibc.i686 \
	              glibc-devel.i686 \
	              libgcc.i686 \
	              perl \
	              perl-Test \
	              perl-Test-Harness \
	              perl-File-Which \
	              perl-Time-HiRes \
	              nmap-ncat

### Fedora

	# dnf install audit \
	              gcc \
	              glibc.i686 \
	              glibc-devel.i686 \
	              perl \
	              perl-Test \
	              perl-Test-Harness \
	              perl-File-Which \
	              perl-Time-HiRes \
	              nmap-ncat

### Debian Based Systems

On Debian you need to install a number of dependencies as well as perform some
additional system configuration.  Install the dependencies using the commands
below:

	# apt-get install auditd \
	                  build-essential \
	                  libc6-i386 \
	                  libc6-dev-i386 \
	                  perl-modules \
	                  netcat

After the dependencies are installed you should ensure that BASH is installed
on the system and that /bin/sh points to BASH, not Dash:

	# apt-get install bash
	# dpkg-reconfigure dash

## Execution

Please notice that tests are changing kernel audit rules and hence it might be
a good idea to back them up prior testing and restore them afterwards. The
tests need to be executed as root.

All of the commands listed below should be executed from the audit-testsuite
top level directory.

### Build the Tests

	# make

### List the Tests

	# make list

### Execute the Entire Test Suite

	# make test

### Execute a Subset of the Test Suite

	# TESTS="test1 test2 ..." make -e test

### Enable Additional Debugging Output

	# ATS_DEBUG=1 make test
