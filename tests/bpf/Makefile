TARGETS=$(patsubst %.c,%,$(wildcard *.c))

all: $(TARGETS)

bpf: bpf.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
