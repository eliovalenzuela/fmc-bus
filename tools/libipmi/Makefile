# If it exists includes Makefile.specific. In this Makefile, you should put
# specific Makefile code that you want to run before this. For example,
# build a particular environment.
-include Makefile.specific

OBJ=ipmi.o
OUT=libipmi.a
OUT_SO=libipmi.so

CFLAGS+=-fPIC -shared -Wall -Wextra -ggdb $(EXTRACFLAGS)

all: $(OUT) $(OUT_SO)

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

$(OUT_SO): $(OBJ)
	$(CC) $< $(CFLAGS) -shared -fPIC -L. -Wl,-soname,$@ -o $@

clean:
	rm -rf $(OBJ) $(OUT) $(OUT_SO)
