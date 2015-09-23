#define SERIAL_BUFFER_LENGTH 1024
#define CANFRAME_BUFFER_LENGTH 128

// id - 2 bytes & 0x0FFF
// length - 1 byte
// data[8] - 8 bytes
// timestamp - 2 bytes (optional)

// Opens a connection to the CANUSB device and activates the filter configuration if it is set
extern int canusb_init(int iport);
extern void canusb_close(void);

// Sends frame and returns number of frames left in buffer
extern int canusb_send_frame(CANMsg* frame);

// frame == null on failure
extern int canusb_get_frame(CANMsg* frame);

extern void canusb_set_filter11(int id, int idmask, int rtr, int rtrmask, int data, int datamask, int id2, int idmask2, int rtr2, int rtrmask2);
/** Set adapter to only print CAN messages with ID 'id'. Only supports 11-bit identifiers */
extern void canusb_filter_id(unsigned short id);

extern void canusb_print_version(void);
extern void canusb_print_serial_number(void);
extern void canusb_enable_timestamps(void);
extern void canusb_disable_timestamps(void);

