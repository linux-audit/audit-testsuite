// SPDX-License-Identifier: GPL-2.0
/*
 * Microbenchmark for Linux audit syscall overhead.
 *
 * This program does not configure audit.  Install rules manually to compare
 * the same workload under different policies.
 */

#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_ITERATIONS	10000000ULL
#define DEFAULT_REPETITIONS	10

static int compare_double(const void *left, const void *right)
{
	const double a = *(const double *)left;
	const double b = *(const double *)right;

	return (a > b) - (a < b);
}

static void print_summary(double *samples, unsigned int repetitions)
{
	double mean = 0.0;
	double squared_deviations = 0.0;
	double median;
	double stddev;
	unsigned int i;

	for (i = 0; i < repetitions; i++)
		mean += samples[i];
	mean /= repetitions;

	for (i = 0; i < repetitions; i++) {
		double deviation = samples[i] - mean;

		squared_deviations += deviation * deviation;
	}
	stddev = repetitions > 1 ?
		sqrt(squared_deviations / (repetitions - 1)) : 0.0;

	qsort(samples, repetitions, sizeof(*samples), compare_double);
	if (repetitions % 2)
		median = samples[repetitions / 2];
	else
		median = (samples[repetitions / 2 - 1] +
			  samples[repetitions / 2]) / 2.0;

	printf("==========================================\n");
	printf("summary (ns/op): median=%.f", median);
	printf(" mean=%.f", mean);
	printf(" stddev=%.f (%.f%%)", stddev,
	       mean ? stddev * 100.0 / mean : 0.0);
	printf(" range=%.f..%.f\n",
	       samples[0], samples[repetitions - 1]);
}

static void usage(const char *program)
{
	printf("Usage: %s [OPTIONS]\n", program);
	printf("\n");
	printf("Options:\n");
	printf("  -c, --cpu CPU          pin the benchmark to CPU\n");
	printf("  -h, --help             show this help\n");
	printf("  -n, --iterations N     measured operations per repetition\n");
	printf("                         (default: %llu)\n",
	       DEFAULT_ITERATIONS);
	printf("  -r, --repetitions N    number of measured repetitions\n");
	printf("                         (default: %d)\n",
	       DEFAULT_REPETITIONS);
	printf("  -w, --warmup N         warm-up operations (default N/10)\n");
}

static uint64_t parse_u64(const char *value, const char *name)
{
	uint64_t parsed;
	char *end;

	if (*value < '0' || *value > '9')
		errx(EXIT_FAILURE, "invalid %s: %s", name, value);

	errno = 0;
	parsed = strtoull(value, &end, 0);
	if (errno || *value == '\0' || *end != '\0')
		errx(EXIT_FAILURE, "invalid %s: %s", name, value);

	return parsed;
}

static unsigned int parse_uint(const char *value, const char *name)
{
	uint64_t parsed = parse_u64(value, name);

	if (parsed > UINT_MAX)
		errx(EXIT_FAILURE, "%s is too large: %s", name, value);

	return parsed;
}

static void pin_to_cpu(unsigned int cpu)
{
	cpu_set_t set;

	if (cpu >= CPU_SETSIZE)
		errx(EXIT_FAILURE, "CPU must be less than %d", CPU_SETSIZE);

	CPU_ZERO(&set);
	CPU_SET(cpu, &set);
	if (sched_setaffinity(0, sizeof(set), &set))
		err(EXIT_FAILURE, "sched_setaffinity(%u)", cpu);
}

static uint64_t elapsed_ns(const struct timespec *start,
			   const struct timespec *end)
{
	return (end->tv_sec - start->tv_sec) * 1000000000ULL +
		end->tv_nsec - start->tv_nsec;
}

static void run_getpid(uint64_t iterations)
{
	uint64_t i;

	for (i = 0; i < iterations; i++)
		syscall(SYS_getpid);
}

int main(int argc, char **argv)
{
	static const struct option options[] = {
		{ "cpu", required_argument, NULL, 'c' },
		{ "help", no_argument, NULL, 'h' },
		{ "iterations", required_argument, NULL, 'n' },
		{ "repetitions", required_argument, NULL, 'r' },
		{ "warmup", required_argument, NULL, 'w' },
		{ }
	};
	uint64_t iterations = DEFAULT_ITERATIONS;
	uint64_t warmup = 0;
	unsigned int repetitions = DEFAULT_REPETITIONS;
	unsigned int cpu = 0;
	bool warmup_set = false;
	bool cpu_set = false;
	double *samples;
	int option;
	unsigned int repetition;

	while ((option = getopt_long(argc, argv, "c:hn:r:w:", options,
				     NULL)) != -1) {
		switch (option) {
		case 'c':
			cpu = parse_uint(optarg, "CPU");
			cpu_set = true;
			break;
		case 'h':
			usage(argv[0]);
			return EXIT_SUCCESS;
		case 'n':
			iterations = parse_u64(optarg, "iteration count");
			break;
		case 'r':
			repetitions = parse_uint(optarg, "repetition count");
			break;
		case 'w':
			warmup = parse_u64(optarg, "warm-up count");
			warmup_set = true;
			break;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (optind != argc)
		errx(EXIT_FAILURE, "unexpected positional argument: %s",
		     argv[optind]);
	if (!iterations || !repetitions)
		errx(EXIT_FAILURE, "iterations and repetitions must be nonzero");
	if (!warmup_set)
		warmup = iterations / 10;
	if (cpu_set)
		pin_to_cpu(cpu);

	samples = calloc(repetitions, sizeof(*samples));
	if (!samples)
		err(EXIT_FAILURE, "calloc(samples)");

	printf("getpid iterations=%" PRIu64 " warmup=%" PRIu64
	       " repetitions=%u\n", iterations, warmup, repetitions);

	run_getpid(warmup);
	for (repetition = 0; repetition < repetitions; repetition++) {
		struct timespec start, end;
		uint64_t duration;
		double ns_per_operation;

		if (clock_gettime(CLOCK_MONOTONIC_RAW, &start))
			err(EXIT_FAILURE, "clock_gettime(start)");
		run_getpid(iterations);
		if (clock_gettime(CLOCK_MONOTONIC_RAW, &end))
			err(EXIT_FAILURE, "clock_gettime(end)");

		duration = elapsed_ns(&start, &end);
		ns_per_operation = (double)duration / iterations;
		samples[repetition] = ns_per_operation;
		printf("%u: %.2f ns/op\n", repetition + 1,
		       ns_per_operation);
	}

	print_summary(samples, repetitions);
	free(samples);
	return EXIT_SUCCESS;
}
