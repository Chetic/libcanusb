.PHONY: clean
CC := gcc
LD := gcc
AR := ar
CCFLAGS := -c -ggdb -O0
LDFLAGS := -g -Llib
LIB_OBJS := obj/serial.o obj/canusb.o obj/circbuf.o

all: obj bin lib lib/libcanusb.a bin/test_canusb

lib/libcanusb.a: $(LIB_OBJS)
	$(AR) rcs $@ $^

bin/test_canusb: obj/test_canusb.o $(O_FILES)
	$(LD) $(LDFLAGS) $^ -o $@ -lcanusb

obj bin lib:
	mkdir -p $@

clean:
	rm -rf bin || :
	rm -rf obj || :
	rm -rf lib || :

obj/%.o: src/%.c
	$(CC) $(CCFLAGS) $^ -o $@
