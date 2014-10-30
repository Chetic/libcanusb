CC := gcc
LD := gcc
CCFLAGS := -c -ggdb -O0
O_FILES := serial.o canusb.o log.o

all: test_log canusb

test_log: test_log.o $(O_FILES)
	$(LD) -g $^ -o $@

canusb: main.o $(O_FILES)
	$(LD) -g $^ -o $@

clean:
	rm test_log || :
	rm canusb || :
	rm *.o || :

%.o: %.c
	$(CC) $(CCFLAGS) $^ -o $@
