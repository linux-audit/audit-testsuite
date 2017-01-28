SUBDIRS = tests 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

list:
	make -C tests list

test:
	make -C tests test

