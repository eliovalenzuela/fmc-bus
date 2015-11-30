DIRS = \
	kernel \
	tools \
	sdb-lib \

.PHONY: all clean modules install modules_install $(DIRS) sdb-lib

all clean modules install modules_install: $(DIRS)

clean: TARGET = clean
modules: TARGET = modules
install: TARGET = install
modules_install: TARGET = modules_install

$(DIRS):
	$(MAKE) -C $@ $(TARGET)
