# If it exists includes Makefile.specific. In this Makefile, you should put
# specific Makefile code that you want to run before this. For example,
# build a particular environment.
-include Makefile.specific

# include parent_common.mk for buildsystem's defines
# It allows you to inherit an environment configuration from larger project
REPO_PARENT=../..
-include $(REPO_PARENT)/parent_common.mk

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
