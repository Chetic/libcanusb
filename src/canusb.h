#define SERIAL_BUFFER_LENGTH 1024
#define CANFRAME_BUFFER_LENGTH 128

typedef struct
{
	union {
		unsigned short id;
		unsigned char length;
	};
    unsigned char data[8];
    unsigned short timestamp;
} CANFrame;

// id - 2 bytes & 0x0FFF
// length - 1 byte
// data[8] - 8 bytes
// timestamp - 2 bytes (optional)

extern int canusb_init(char* portname);
extern int canusb_send_cmd(char* cmd);
extern void canusb_print_version(void);
extern void canusb_print_serial_number(void);
extern void canusb_enable_timestamps(void);
extern void canusb_disable_timestamps(void);

/** To be called regularly. Reads and parses CAN frames */
extern void canusb_poll(void);
