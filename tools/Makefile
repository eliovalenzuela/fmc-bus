# If it exists includes Makefile.specific. In this Makefile, you should put
# specific Makefile code that you want to run before this. For example,
# build a particular environment.
-include Makefile.specific

CFLAGS = -Wall -ggdb -O2 -I../kernel/include $(EXTRACFLAGS)
PROGS = fru-dump fmc-mem

all: $(PROGS)

all:
	$(MAKE) -C libipmi $@
clean:
	$(MAKE) -C libipmi $@
	rm -f $(PROGS)

# placeholders, so ../Makefile is happy
modules install modules_install:
