SUBDIRS = tests 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all ; done

clean:
	make -C tests clean

test:
	make -C tests test

