TARGETS=$(patsubst %.c,%,$(wildcard *.c))

LDLIBS += -lpthread

all: $(TARGETS)

execve_arg_gen: execve_arg_gen.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
