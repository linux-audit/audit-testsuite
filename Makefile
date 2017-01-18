SUBDIRS = tests 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all ; done

test:
	make -C tests test

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

check-syntax:
	@./tools/check-syntax

