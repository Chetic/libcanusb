#define SERIAL_BUFFER_LENGTH 1024
#define CANFRAME_BUFFER_LENGTH 128

typedef struct
{
	unsigned short id;
	unsigned char length;
	unsigned char data[8];
	unsigned short timestamp;
} CANFrame;

// id - 2 bytes & 0x0FFF
// length - 1 byte
// data[8] - 8 bytes
// timestamp - 2 bytes (optional)

extern void canusb_reset(void);
extern int canusb_init(char* portname);
extern int canusb_send_cmd(char* cmd);

extern void canusb_set_filter11(int id, int idmask, int rtr, int rtrmask, int data, int datamask, int id2, int idmask2, int rtr2, int rtrmask2);
/** Set adapter to only print CAN messages with ID 'id'. Only supports 11-bit identifiers */
extern void canusb_filter_id(unsigned short id);

extern void canusb_print_version(void);
extern void canusb_print_serial_number(void);
extern void canusb_enable_timestamps(void);
extern void canusb_disable_timestamps(void);
extern CANFrame* canusb_get_frame(unsigned int index);

/** To be called regularly. Reads and parses CAN frames.
 * Returns number of frames parsed per call */
extern int canusb_poll(void);
