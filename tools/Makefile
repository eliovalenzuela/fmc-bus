
CFLAGS = -Wall -ggdb -O2 -I../kernel/include $(EXTRACFLAGS)
PROGS = fru-dump fmc-mem

all: $(PROGS)

all:
	$(MAKE) -C libipmi $@
clean:
	$(MAKE) -C libipmi $@
	rm $(PROGS)

# placeholders, so ../Makefile is happy
modules install modules_install:
