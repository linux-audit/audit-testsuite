TARGETS=$(patsubst %.c,%,$(wildcard *.c))

all: $(TARGETS)

iouring: iouring.c
	$(CC) $(CFLAGS) -o $@ $^ -luring -lrt

clean:
	rm -f $(TARGETS)
