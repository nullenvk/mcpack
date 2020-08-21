CFLAGS := -Wall -Wextra -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include
LDFLAGS :=

AR ?= ar
CC ?= clang

LIB_STATIC := obsidian_serialize.a
LIB_SHARED := obsidian_serialize.so
OBJ = serialize.o 

all: $(LIB_SHARED) $(LIB_STATIC)

$(LIB_SHARED): $(OBJ)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJ) 


$(LIB_STATIC): $(OBJ)
	$(AR) rcs $@ $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean:
	-rm -r $(LIB_SHARED) $(LIB_STATIC) $(OBJ)

.PHONY: clean
.SUFFIXES: .c .o
