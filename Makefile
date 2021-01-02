PREFIX ?= /usr/local
AR ?= ar
CC ?= clang

CFLAGS := -Wall -Wextra -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include
LDFLAGS :=

OBJ = mcpack.o 

all: libmcpack.so libmcpack.a

libmcpack.so: $(OBJ)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJ) 


libmcpack.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
	-rm -r libmcpack.a libmcpack.so $(OBJ)

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	install -m 0755 libmcpack.so $(PREFIX)/lib
	install -m 0755 libmcpack.a $(PREFIX)/lib
	install -m 0644 mcpack.h $(PREFIX)/include

uninstall:
	-rm -f $(PREFIX)/lib/libmcpack.so 
	-rm -f $(PREFIX)/lib/libmcpack.a 
	-rm -f $(PREFIX)/include/mcpack.h

.PHONY: clean install uninstall
.SUFFIXES: .c .o
