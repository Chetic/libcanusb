extern int serial_init(char* portname);
extern void serial_write(char* data, int length);
extern int serial_read(char* data, int maxLength);
