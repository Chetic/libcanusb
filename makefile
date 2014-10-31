.PHONY: clean
ifeq ($(shell uname -a | grep armv6),)
CC := arm-unknown-linux-gnueabi-gcc
LD := arm-unknown-linux-gnueabi-gcc
AR := arm-unknown-linux-gnueabi-ar
else
CC := gcc
LD := gcc
AR := ar
endif
CCFLAGS := -c -ggdb -O0
LDFLAGS := -g -Llib
LIB_OBJS := obj/serial.o obj/canusb.o obj/log.o

all: obj bin lib lib/libcanusb.a bin/test_log bin/canusb

lib/libcanusb.a: $(LIB_OBJS)
	$(AR) rcs $@ $^

bin/test_log: obj/test_log.o $(O_FILES)
	$(LD) $(LDFLAGS) $^ -o $@ -lcanusb

bin/canusb: obj/main.o $(O_FILES)
	$(LD) $(LDFLAGS) $^ -o $@ -lcanusb

obj bin lib:
	mkdir -p $@

clean:
	rm -rf bin || :
	rm -rf obj || :

obj/%.o: src/%.c obj
	$(CC) $(CCFLAGS) $^ -o $@
