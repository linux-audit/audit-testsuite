Title: module load PATH filter


Description:
This test detects kernels that flood the audit logs with hundreds of
PATH SYSCALL auxiliary records and the modules that provoke them.

It does this by setting up an audit rule in the config file to log all
init_module and finit_module syscalls, then once the machine is rebooted
the test script detects SYSCALL records with extra PATH records along
with their provoking modules since the most recent boot.


How to run the test:

1) Change to the working directory of the test.

	cd tests_manual/syscall_module_path_filter

2) Set up the test, which adds the
   tests_manual-syscall_module_path_filter.rules audit rule file to the
   /etc/audit/rules.d/ audit rules directory to log all init_module and
   finit_module syscalls and to filter all tracefs and debugfs
   filesystem entries.

	./setup

3) Reboot the machine to trigger the test.

	shutdown -r now

4) Run the test itself that looks for *init_module syscall records since
   this last boot and searches for the number of items listed in the
   syscall record.

	cd tests_manual/syscall_module_path.rules
	./test

   If there are, the test will fail and if ATS_DEBUG is set, it will
   list which modules cause the records.

5) Restore the config file back to its previous state and delete the
   audit rule.

	./cleanup


Todo:

Automate the test so that a reboot is not needed and dependence on
absent or in-use modules is not necessary to detect the bug.


Author: Richard Guy Briggs <rgb@redhat.com> 2017-02-05
