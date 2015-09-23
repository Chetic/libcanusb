.PHONY: clean
CC := gcc
LD := gcc
AR := ar
CCFLAGS := -c -ggdb -O0
LDFLAGS := -g -Llib
LIB_OBJS := obj/d2xxif.o obj/canusbnew.o

all: obj bin lib lib/libcanusb.a

lib/libcanusb.a: $(LIB_OBJS)
	$(AR) rcs $@ $^

obj bin lib:
	mkdir -p $@

clean:
	rm -rf bin || :
	rm -rf obj || :
	rm -rf lib || :

obj/%.o: src/%.c
	$(CC) $(CCFLAGS) $^ -o $@
