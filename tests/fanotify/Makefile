TARGETS=$(patsubst %.c,%,$(wildcard *.c))

all: $(TARGETS)

fanotify: fanotify.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
