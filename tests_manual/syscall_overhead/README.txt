Audit syscall overhead benchmark
================================

Build it with:

  make -C tests_manual/syscall_overhead

The benchmark repeatedly invokes getpid(2). It does not install or remove
audit rules. Configure the policy explicitly with auditctl, then run the same
workload for each policy.

For example:

  auditctl -a always,exit -F arch=b64 -S openat
  ./tests_manual/syscall_overhead/audit_bench
  auditctl -d always,exit -F arch=b64 -S openat

The getpid workload exposes the fixed per-syscall audit overhead without
adding filesystem work. Useful comparisons are no rules, increasing numbers
of unrelated syscall rules, and a clean state versus one where a watch or
tree rule was removed automatically. Keep the machine idle, pin with --cpu
when possible, and collect profiles with perf stat and perf record. Report
the kernel commit, CPU model, audit status, policy and auditd state with every
comparison.

After printing each repetition, the benchmark reports the median, arithmetic
mean, sample standard deviation, coefficient of variation and observed range
of per-operation latency across repetitions. The coefficient of variation
makes noisy runs easy to identify; increase the iteration count or investigate
system noise when it is high.
