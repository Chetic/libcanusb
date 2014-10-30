CC := gcc
LD := gcc
CCFLAGS := -c -ggdb -O0
O_FILES := main.o serial.o canusb.o log.o

canusb: $(O_FILES)
	$(LD) -g $^ -o $@

clean:
	rm canusb || :
	rm $(O_FILES) || :

%.o: %.c
	$(CC) $(CCFLAGS) $^ -o $@
