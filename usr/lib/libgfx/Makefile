CFILES = $(shell find . -name "*.c")


libgfx.a: cfiles
	ar -rc $@ *.o
	rm *.o

.PHONY: cfiles
cfiles: $(CFILES)
	gcc -c $^ -I include/ -I../libc/include/ -Wno-builtin-declaration-mismatch
