#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/timex.h>

static int adjtimex_direct(struct timex *txc)
{
	return syscall(SYS_adjtimex, txc);
}

static int adjtimex_posix(struct timex *txc)
{
	return syscall(SYS_clock_adjtime, CLOCK_REALTIME, txc);
}

static int op_set_value(int (*f_adjtimex)(struct timex *txc), int modes,
			long (*getter)(const struct timex *txc),
			void (*setter)(struct timex *txc, long v),
			long v1, long v2)
{
	struct timex txc;
	long set_val, reset_val;
	int ret, i;

	txc.modes = 0;
	ret = f_adjtimex(&txc);
	if (ret < 0) {
		perror("adjtimex (get)");
		return 1;
	}

	reset_val = getter(&txc);

	if (v1 == 0)
		set_val = reset_val + 1;
	else
		set_val = reset_val != v1 ? v1 : v2;

	/* do set twice to test that no no-change event is generated */
	for (i = 0; i < 2; i++) {
		txc.modes = modes;
		setter(&txc, set_val);

		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (set)");
			return 1;
		}
	}

	txc.modes = modes;
	setter(&txc, reset_val);

	ret = f_adjtimex(&txc);
	if (ret < 0) {
		perror("adjtimex (reset)");
		return 1;
	}

	if (modes != ADJ_TAI)
		return 0;

	/* workaround for broken TAI handling */
	txc.modes = 0;
	ret = f_adjtimex(&txc);
	if (ret < 0) {
		perror("adjtimex (get 2)");
		return 1;
	}

	if (getter(&txc) != reset_val) {
		/* we tried to reset to 0, but got rejected; set to 1 instead */
		txc.modes = modes;
		setter(&txc, reset_val + 1);

		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (reset 2)");
			return 1;
		}
	}
	return 0;
}

static long getter_offset(const struct timex *txc)
{
	return txc->offset;
}
static long getter_freq(const struct timex *txc)
{
	return txc->freq;
}
static long getter_tai(const struct timex *txc)
{
	return txc->tai;
}
static long getter_tick(const struct timex *txc)
{
	return txc->tick;
}

static void setter_offset(struct timex *txc, long v)
{
	txc->offset = v;
}
static void setter_freq(struct timex *txc, long v)
{
	txc->freq = v;
}
static void setter_tai(struct timex *txc, long v)
{
	txc->constant = v;
}
static void setter_tick(struct timex *txc, long v)
{
	txc->tick = v;
}

int main(int argc, char **argv)
{
	int (*f_adjtimex)(struct timex * txc);
	const char *mode, *op;
	int ret;

	if (argc < 3)
		goto err_args;

	mode = argv[1];
	op = argv[2];

	if (strcmp(mode, "posix") == 0) {
		f_adjtimex = &adjtimex_posix;
	} else if (strcmp(mode, "adjtimex") == 0) {
		f_adjtimex = &adjtimex_direct;
	} else {
		fprintf(stderr, "%s: invalid mode: %s\n", argv[0], mode);
		return 1;
	}

	if (strcmp(op, "setoffset") == 0) {
		struct timex txc;
		struct timex txc_reset;
		if (argc != 5)
			goto err_args;

		txc.modes = ADJ_SETOFFSET;
		txc.time.tv_sec = (time_t)atoll(argv[3]);
		txc.time.tv_usec = (suseconds_t)atol(argv[4]);

		txc_reset.modes = ADJ_SETOFFSET;
		txc_reset.time.tv_sec = -txc.time.tv_sec;
		if (txc.time.tv_usec > 0) {
			txc_reset.time.tv_usec = (suseconds_t)1000000 - txc.time.tv_usec;
			txc_reset.time.tv_sec -= 1;
		} else {
			txc_reset.time.tv_usec = 0;
		}

		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (set)");
			return 1;
		}
		ret = f_adjtimex(&txc_reset);
		if (ret < 0) {
			perror("adjtimex (reset)");
			return 1;
		}
	} else if (strcmp(op, "adjust") == 0) {
		if (argc != 5)
			goto err_args;

		return op_set_value(f_adjtimex, ADJ_OFFSET_SINGLESHOT,
				    &getter_offset, &setter_offset,
				    atol(argv[3]), atol(argv[4]));
	} else if (strcmp(op, "offset") == 0) {
		struct timex txc;

		if (argc != 5)
			goto err_args;

		/* offset is only applicable in STA_PLL status */
		txc.modes = ADJ_STATUS;
		txc.status = STA_PLL;
		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (STA_PLL set)");
			return 1;
		}

		ret = op_set_value(f_adjtimex, ADJ_OFFSET,
				   &getter_offset, &setter_offset,
				   atol(argv[3]), atol(argv[4]));

		txc.modes = ADJ_STATUS;
		txc.status = STA_UNSYNC;
		if (f_adjtimex(&txc) < 0) {
			perror("adjtimex (STA_PLL unset)");
			return 1;
		}
		return ret;

	} else if (strcmp(op, "freq") == 0) {
		if (argc != 5)
			goto err_args;

		return op_set_value(f_adjtimex, ADJ_FREQUENCY,
				    &getter_freq, &setter_freq,
				    atol(argv[3]), atol(argv[4]));
	} else if (strcmp(op, "tai") == 0) {
		if (argc != 5)
			goto err_args;

		return op_set_value(f_adjtimex, ADJ_TAI,
				    &getter_tai, &setter_tai,
				    atol(argv[3]), atol(argv[4]));
	} else if (strcmp(op, "tick") == 0) {
		if (argc != 5)
			goto err_args;

		return op_set_value(f_adjtimex, ADJ_TICK,
				    &getter_tick, &setter_tick,
				    atol(argv[3]), atol(argv[4]));
	} else if (strcmp(op, "status") == 0) {
		struct timex txc;
		int modes_set, modes_reset, i;

		txc.modes = 0;
		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (get)");
			return 1;
		}

		if (txc.status & STA_NANO) {
			modes_set = ADJ_MICRO;
			modes_reset = ADJ_NANO;
		} else {
			modes_set = ADJ_NANO;
			modes_reset = ADJ_MICRO;
		}

		/* do set twice to test that no no-change event is generated */
		for (i = 0; i < 2; i++) {
			txc.modes = modes_set;

			ret = f_adjtimex(&txc);
			if (ret < 0) {
				perror("adjtimex (set)");
				return 1;
			}
		}

		txc.modes = modes_reset;

		ret = f_adjtimex(&txc);
		if (ret < 0) {
			perror("adjtimex (reset)");
			return 1;
		}
	} else {
		fprintf(stderr, "%s: invalid op: %s\n", argv[0], op);
		return 1;
	}

	return 0;

err_args:
	fprintf(stderr, "%s: wrong number of arguments\n", argv[0]);
	return 1;
}
