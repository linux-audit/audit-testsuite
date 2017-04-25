Title: check BPRM_FCAPS record not generated when executing setuid applications


Description:
The audit subsystem is adding a BPRM_FCAPS record when auditing setuid
application execution. This is not expected as it was supposed to be limited to
when the file system actually had capabilities in an extended attribute.
This was observed on Fedora kernel 4.1.5-200.fc22.x86_64.

Reproducer:
# auditctl -a always,exit -F arch=b64 -S execve -C uid!=euid -F euid=0 -F key=setuid-exec
# su - root
# ausearch --start recent -k setuid-exec -i

Goal:
Make certain a BPRM_FCAPS record is not generated because it lists all
capabilities making the event really ugly to parse what is
happening.  The PATH record correctly records the setuid bit and owner.


How to run the test:

1) As root, change to the working directory of the test.

	# cd tests_manual/syscall_execve_noBPRM_FCAPS

2) As root, set up the test, which adds an audit rule to log all execve syscalls
where the uid and euid are not equivalent.

	# ./setup

3) Transition from unprivileged user to superuser

	$ su - root

4) As root, run the test itself that looks for execve syscall records since
this last rule was added and searches for a BPRM_FCAPS record accompanying the
syscall record in that event.

	# ./test

If there are BPRM_FCAPS records, it will list which commands cause the records.

5) As root, restore the config file back to its previous state and delete the
audit rule.

	# ./cleanup


Todo:
Automate the test so that manually running the transition from unprivileged
account to root is not necessary.


Author: Richard Guy Briggs <rgb@redhat.com> 2017-03-01
