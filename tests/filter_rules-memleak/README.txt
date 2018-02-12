Title: audit_filter_rules cred structure memory leak


Description:
This test detects kernels that leak cred structures in
kernel/auditsc.c:audit_filter_rules() when two rules matching the same event
are tested in decreasing priority (syscall, then watch?).

It does this by setting up two rules that should both match a trigger condition
but expecting the latter rule to not be executed due to the first having a
higher rule execution priority.  The exit condition for the latter rule forgets
to put_cred().  A trigger condition is provoked many times while measuring
memory conditions to detect the bug.


How to run the test:

- Change to the working directory of the test.

	cd tests_manual/filter_rules-memleak

- Run the test itself that records the current amount of memory, runs a
  triggering command that is expected to match both rules many times, then
  again records the amount of memory used checking for a significant
  difference.

	./test

  If there is, it will report unusual memory use on output.



Author: Richard Guy Briggs <rgb@redhat.com> 2017-04-11
