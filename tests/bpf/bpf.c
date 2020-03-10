#include <errno.h>
#include <string.h>
#include <linux/filter.h>
#include <linux/bpf.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BPF_EXIT_INSN() \
	((struct bpf_insn) { .code = BPF_JMP | BPF_EXIT, .dst_reg = 0, .src_reg = 0, .off = 0, .imm = 0 })

#define BPF_MOV64_IMM(DST, IMM) \
	((struct bpf_insn) { .code = BPF_ALU64 | BPF_MOV | BPF_K, .dst_reg = DST, .src_reg = 0, .off = 0, .imm = IMM })

static inline __u64 ptr_to_u64(const void *ptr)
{
	return (__u64) (unsigned long) ptr;
}

int rlimit_fix(void)
{
	struct rlimit limit = {
		.rlim_cur = RLIM_INFINITY, .rlim_max = RLIM_INFINITY
	};

	if (setrlimit(RLIMIT_MEMLOCK, &limit))
		return errno;

	return 0;
}

int main(int argc, char **argv)
{
	int rc;
	struct bpf_insn insns[] = {
		BPF_MOV64_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN(),
	};
	union bpf_attr attr;

	rc = rlimit_fix();
	if (rc)
		return rc;

	memset(&attr, 0, sizeof(attr));
	attr.prog_type = BPF_PROG_TYPE_SOCKET_FILTER;
	attr.insn_cnt = 2;
	attr.insns = ptr_to_u64(insns);
	attr.license = ptr_to_u64("GPL");
	if (syscall(__NR_bpf, BPF_PROG_LOAD, &attr, sizeof(attr)))
		return errno;

	return 0;
}
