TARGETS=libfuse libfuse/example/fusexmp

LDLIBS += -lpthread

all: $(TARGETS)

libfuse:
	git clone git://github.com/libfuse/libfuse.git -b fuse_2_9_bugfix

# needs git, libtool, gettext-devel, util-linux?
libfuse/example/fusexmp: libfuse/example/fusexmp.c
	cd libfuse/ && ./makeconf.sh && ./configure && make

clean:
	rm -fr $(TARGETS)
