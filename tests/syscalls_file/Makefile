TARGETS=$(patsubst %.c,%,$(wildcard *.c))

LDLIBS += -lpthread

all: $(TARGETS)

openat2: openat2.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

