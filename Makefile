SUBDIRS = tests 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all ; done

test:
	make -C tests test

clean:
	make -C tests clean

check-syntax:
	@./tools/check-syntax

