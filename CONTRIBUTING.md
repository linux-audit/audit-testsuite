How to Submit Patches to the audit-testsuite Project
===============================================================================
https://github.com/linux-audit/audit-testsuite

This document is intended to act as a guide to help you contribute to the
audit-testsuite project.  It is not perfect, and there will always be
exceptions to the rules described here, but by following the instructions below
you should have a much easier time getting your work merged with the upstream
project.

## Test Your Code

There are three possible tests you can run to verify your code.  The first
test is used to check the formatting and coding style of your changes, you
can run the test with the following command:

	# make check-syntax

... if there are any problems with your changes a diff/patch will be shown
which indicates the problems and how to fix them.

The second possible test is to test your patch in standalone mode.  You can do
this by changing to the individual test directory and running the test
manually:

	# cd tests/<test_dir>
	# ./test

... there should be no errors when running the test in standalone mode and
non-harness output should be kept to a minimum.

The third possible test is to run the full test suite as described in the
documentation.  There should be no errors or non-hardness output in the test
results.

## Generate the Patch(es)

Depending on how you decided to work with the audit-testsuite code base and
what tools you are using there are different ways to generate your patch(es).
However, regardless of what tools you use, you should always generate your
patches using the "unified" diff/patch format and the patches should always
apply to the audit-testsuite source tree using the following command from the
top directory of the audit-testsuite sources:

	# patch -p1 < changes.patch

If you are not using git, stacked git (stgit), or some other tool which can
generate patch files for you automatically, you may find the following command
helpful in generating patches, where "audit-testsuite.orig/" is the unmodified
source code directory and "audit-testsuite/" is the source code directory with
your changes:

	# diff -purN audit-testsuite.orig/ audit-testsuite/

When in doubt please generate your patch and try applying it to an unmodified
copy of the audit-testsuite sources; if it fails for you, it will fail for the
rest of us.

## Explain Your Work

At the top of every patch you should include a description of the problem you
are trying to solve, how you solved it, and why you chose the solution you
implemented.  If you are submitting a bug fix, it is also incredibly helpful
if you can describe/include a reproducer for the problem in the description as
well as instructions on how to test for the bug and verify that it has been
fixed.

## Sign Your Work

The sign-off is a simple line at the end of the patch description, which
certifies that you wrote it or otherwise have the right to pass it on as an
open-source patch.  The "Developer's Certificate of Origin" pledge is taken
from the Linux Kernel and the rules are pretty simple:

	Developer's Certificate of Origin 1.1

	By making a contribution to this project, I certify that:

	(a) The contribution was created in whole or in part by me and I
	    have the right to submit it under the open source license
	    indicated in the file; or

	(b) The contribution is based upon previous work that, to the best
	    of my knowledge, is covered under an appropriate open source
	    license and I have the right under that license to submit that
	    work with modifications, whether created in whole or in part
	    by me, under the same open source license (unless I am
	    permitted to submit under a different license), as indicated
	    in the file; or

	(c) The contribution was provided directly to me by some other
	    person who certified (a), (b) or (c) and I have not modified
	    it.

	(d) I understand and agree that this project and the contribution
	    are public and that a record of the contribution (including all
	    personal information I submit with it, including my sign-off) is
	    maintained indefinitely and may be redistributed consistent with
	    this project or the open source license(s) involved.

... then you just add a line to the bottom of your patch description, with
your real name, saying:

	Signed-off-by: Random J Developer <random@developer.example.org>
