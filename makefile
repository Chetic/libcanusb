.PHONY: clean

CC := arm-unknown-linux-gnueabi-gcc
LD := arm-unknown-linux-gnueabi-gcc
CCFLAGS := -c -ggdb -O0
O_FILES := obj/serial.o obj/canusb.o obj/log.o

all: obj bin bin/test_log bin/canusb

bin/test_log: obj/test_log.o $(O_FILES)
	$(LD) -g $^ -o $@

bin/canusb: obj/main.o $(O_FILES)
	$(LD) -g $^ -o $@

obj bin:
	mkdir -p $@

clean:
	rm -rf bin || :
	rm -rf obj || :

obj/%.o: src/%.c obj
	$(CC) $(CCFLAGS) $^ -o $@
