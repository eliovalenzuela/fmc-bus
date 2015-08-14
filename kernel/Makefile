
LINUX ?= /lib/modules/$(shell uname -r)/build

ccflags-y += -I$(src)/include

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
fmc-y += fmc-debug.o

GIT_VERSION = $(shell cd $(src); git describe --dirty --long --tags)

# For this CSM_VERSION, please see ohwr.org/csm documentation
ifdef CONFIG_CSM_VERSION
  ccflags-y += -D"CERN_SUPER_MODULE=MODULE_VERSION(\"$(CONFIG_CSM_VERSION)\")"
else
  ccflags-y += -DCERN_SUPER_MODULE=""
endif

# The library includes <sdb.h>, so point -I directtly there
# include our header before to avoid conflicts with the kernel
LINUXINCLUDE :=  -I$(src)/include -I$(src)/include/linux $(LINUXINCLUDE)
ccflags-y += -DGIT_VERSION=\"$(GIT_VERSION)\"

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
