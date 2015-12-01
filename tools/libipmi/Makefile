# If it exists includes Makefile.specific. In this Makefile, you should put
# specific Makefile code that you want to run before this. For example,
# build a particular environment.
-include Makefile.specific

# include parent_common.mk for buildsystem's defines
# It allows you to inherit an environment configuration from larger project
REPO_PARENT=../../..
-include $(REPO_PARENT)/parent_common.mk

OBJ=ipmi.o
OUT=libipmi.a
OUT_SO=libipmi.so

GIT_VERSION := $(shell git describe --dirty --long --tags)
CFLAGS+=-fPIC -shared -Wall -Wextra -ggdb $(EXTRACFLAGS)
CFLAGS += -DGIT_VERSION="\"$(GIT_VERSION)\""

all: $(OUT) $(OUT_SO)

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)

$(OUT_SO): $(OBJ)
	$(CC) $< $(CFLAGS) -shared -fPIC -L. -Wl,-soname,$@ -o $@

clean:
	rm -rf $(OBJ) $(OUT) $(OUT_SO)
