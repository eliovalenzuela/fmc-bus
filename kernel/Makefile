
LINUX ?= /lib/modules/$(shell uname -r)/build

ccflags-y += -I$M/include

obj-m = fmc.o
obj-m += fmc-fakedev.o
obj-m += fmc-trivial.o
obj-m += fmc-write-eeprom.o
obj-m += fmc-chardev.o

fmc-y = fmc-core.o
fmc-y += fmc-match.o
fmc-y += fmc-sdb.o
fmc-y += fru-parse.o
fmc-y += fmc-dump.o

GIT_VERSION = $(shell cd $(src); git describe --dirty --long --tags)

# The library includes <sdb.h>, so point -I directtly there
ccflags-y += -I$M/include/linux -DGIT_VERSION=\"$(GIT_VERSION)\"

fmc-y += ../sdb-lib/access.o
fmc-y += ../sdb-lib/glue.o

all modules:
	$(MAKE) -C $(LINUX) M=$(shell /bin/pwd) modules

install modules_install:
	$(MAKE) -C $(LINUX) M=$(shell /bin/pwd) modules_install

# be able to run the "clean" rule even if $(LINUX) is not valid
clean:
	rm -rf *.o *~  .*.cmd *.ko *.mod.c .tmp_versions Module.symvers \
		Module.markers modules.order
