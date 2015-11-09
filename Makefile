DIRS = kernel tools

.PHONY: all clean modules install modules_install $(DIRS) sdb-lib

all clean modules install modules_install: $(DIRS)
# additionally clean sdb-lib
clean: sdb-lib

clean: TARGET = clean
modules: TARGET = modules
install: TARGET = install
modules_install: TARGET = modules_install

$(DIRS) sdb-lib:
	$(MAKE) -C $@ $(TARGET)
