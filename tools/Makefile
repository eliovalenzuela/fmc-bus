
CFLAGS = -Wall -ggdb -O2 -I../kernel/include $(EXTRACFLAGS)

all: fru-dump fmc-mem

all clean:
	$(MAKE) -C libipmi $@

# placeholders, so ../Makefile is happy
modules install modules_install:
