TARGETS=$(patsubst %.c,%,$(wildcard *.c))

LDLIBS += -lpthread

all: $(TARGETS)

conn: conn.c
	$(CC) $(CFLAGS) -m32 -o $@ $^

clean:
	rm -f $(TARGETS)
