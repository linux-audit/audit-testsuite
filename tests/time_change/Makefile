TARGETS=$(patsubst %.c,%,$(wildcard *.c))

all: $(TARGETS)

test_settime: test_settime.c
	$(CC) $(CFLAGS) -o $@ $^

test_adjtime: test_adjtime.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
