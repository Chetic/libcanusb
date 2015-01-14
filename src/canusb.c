#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "canusb.h"
#include "circbuf.h"

// Constant field lengths in nibbles:
#define FLDLEN_ID 3
#define FLDLEN_LENGTH 1
#define FLDLEN_TIMESTAMP 4

typedef enum
{
	INITIAL, ID, LENGTH, DATA, TIMESTAMP
} canusb_parse_states_t;

static canusb_parse_states_t s_parsing_state = INITIAL;
static char s_buf[SERIAL_BUFFER_LENGTH];
static int s_frame_buffer_index = -1;
static CANFrame s_frame_buffer[CANFRAME_BUFFER_LENGTH];

void canusb_reset(void)
{
	s_frame_buffer_index = -1;
}

int canusb_init(char* portname)
{
	int ret;
	canusb_reset();
	ret = serial_init(portname);
	usleep(100000);
	serial_write("\r\r\r", 3);
	serial_read(s_buf, sizeof s_buf);
	return ret;
}

int canusb_send_cmd(char* cmd)
{
	int n, i;

	serial_write(cmd, strlen(cmd));
	usleep(100000);
	n = serial_read(s_buf, sizeof s_buf);

	printf("[ ");
	for (i = 0; i < n; i++)
	{
		if (s_buf[i] == 7)
			printf("ERR ");
		else if (s_buf[i] == 10)
			printf("OK (10) ");
		else if (s_buf[i] == 13)
			printf("OK (13) ");
		else
			printf("%d ", s_buf[i]);
	}
	printf("]\n");
}

void canusb_set_filter11(int id, int idmask, int rtr, int rtrmask, int data, int datamask, int id2, int idmask2, int rtr2, int rtrmask2)
{
	/*
	organizational:
	I = ID
	R = RTR
	D = Data

	filter 1	.Reg0    .Reg1.  .Reg1.Reg3.
			 76543210 7654    3210 3210

			 IIIIIIII IIIR    DDDD DDDD

			 76543210 7654
	filter 2	.Reg2    .Reg3.


	raw bytes:
	.Reg0    .Reg1    .Reg2    .Reg3    .   <- register
	 76543210 76543210 76543210 76543210    <- register bit
	 IIIIIIII IIIRDDDD IIIIIIII IIIRDDDD    <- meaning
	 11111111 11111111 22222222 22221111	<- filter #

	*/

	//filter 1
	//use only 11 bits
	id = id & 0x7FF;
	idmask = idmask & 0x7FF;

	//use only 1 bit
	rtr = rtr & 0x1;
	rtrmask = rtrmask & 0x1;

	//use only 8 bits
	data = data & 0xFF;
	datamask = datamask & 0xFF;
	//split data and datamask up into high/low nibbles since that's how they are packed
	int datahigh = (data & 0xF0) >> 4;
	int datalow = data & 0xF;
	int datamaskhigh = (datamask & 0xF0) >> 4;
	int datamasklow = datamask & 0xF;


	//filter 2
	//use only 11 bits
	id2 = id2 & 0x7FF;
	idmask2 = idmask2 & 0x7FF;

	//use only 1 bit
	rtr2 = rtr2 & 0x1;
	rtrmask2 = rtrmask2 & 0x1;


	//code
	int ACR = (id << 21) | (rtr << 20) | (datahigh << 16) | (id2 << 5) | (rtr2 << 4) | datalow;
	//mask
	int ACM = (idmask << 21) | (rtrmask << 20) | (datamaskhigh << 16) | (idmask2 << 5) | (rtrmask2 << 4) | datamasklow;

	char cACR[16];
	char cACM[16];
	snprintf(cACR, sizeof cACR, "M%08X\r", ACR);
	snprintf(cACM, sizeof cACM, "m%08X\r", ACM);

	canusb_send_cmd(cACR);
	canusb_send_cmd(cACM);
}

void canusb_filter_id(unsigned short id)
{
	/* edit cm
	char code_cmd[] = "M00000000\r";
	char mask_cmd[] = "m00000010\r"; //Ignore RTR bit
	id <<= 5;

	sprintf(code_cmd+5, "%03X", id);

	canusb_send_cmd(code_cmd);
	canusb_send_cmd(mask_cmd);
	*/
	//don't use filter1 (all fields 0)
	//filter2 id must match exactly (all bits 0)
	//ignore filter2's rtr bit (rtrmask2 = 1)
	canusb_set_filter11(0x000, 0x000, 0x0, 0x0, 0x00, 0x00, id, 0x000, 0x0, 0x1);
}

void canusb_print_version(void)
{
	int bytes_read;

	serial_write("V\r", 2);
	usleep(1000000); //TODO: Wait appropriate amount
	bytes_read = serial_read(s_buf, sizeof s_buf);

	printf("CANUSB ");
	printf("%s\n", s_buf);
}

void canusb_print_serial_number(void)
{
	int bytes_read;

	serial_write("N\r", 2);
	usleep(1000000); //TODO: Wait appropriate amount
	bytes_read = serial_read(s_buf, sizeof s_buf);

	printf("%s\n", s_buf);
}

void canusb_print_status(void)
{
	int bytes_read;

	serial_write("F\r", 2);
	usleep(1000000); //TODO: Wait appropriate amount
	bytes_read = serial_read(s_buf, sizeof s_buf);

	printf("%s\n", s_buf);
}

void canusb_enable_timestamps(void)
{
	canusb_send_cmd("Z1\r");
}

void canusb_disable_timestamps(void)
{
	canusb_send_cmd("Z0\r");
}

void canusb_read(void)
{
	int n, i;
	char* bufSearch;
	n = serial_read(s_buf, sizeof s_buf);
	for (i = 0; i < n; i++)
	{
		printf("%c", s_buf[i]);
	}
	circbuf_add(s_buf, n);
}

void canusb_reset_frame(CANFrame* frame)
{
	int i;

	frame->id = 0;
	frame->length = 0;
	for (i = 0; i < 8; i++)
		frame->data[i] = 0;
	frame->timestamp = 0;
}

unsigned int ascii_to_hex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return 10 + c - 'A';
	if (c >= 'a' && c <= 'f')
		return 10 + c - 'a';
	return -1;
}

int canusb_parse(void)
{
	static int i;
	static int di;
	int bufidx_pre_parse = s_frame_buffer_index;
	static CANFrame s_currentFrame;

	while (circbuf_len())
	{
		char c = circbuf_pop();
		//c == '\r' ? parsing_state = INITIAL : false;
		switch (s_parsing_state)
		{
		case INITIAL:
			if (c == 't')
			{
				i = 0;
				s_parsing_state = ID;
				canusb_reset_frame(&s_currentFrame);
			}
			break;

		case ID:
			// i == 1 => first char in ID string
			s_currentFrame.id |= (ascii_to_hex(c) << 4 * (FLDLEN_ID - i));
			if (i == FLDLEN_ID)
			{
				s_parsing_state = LENGTH;
			}
			break;

		case LENGTH:
			s_currentFrame.length = ascii_to_hex(c);
			s_parsing_state = DATA;
			break;

		case DATA:
			di = i - (1 + FLDLEN_ID + FLDLEN_LENGTH); //So far read e.g. "tABC8", followed by data
			s_currentFrame.data[di / 2] |= (ascii_to_hex(c) //Index by bytes instead of nibbles
			<< 4 * ((1 - di) % 2)); //bitwise or with every other nibble leftshifted 4 bits
			if (i == 4 + s_currentFrame.length * 2)
			{
				s_parsing_state = TIMESTAMP;
			}
			break;

		case TIMESTAMP:
			di = i - (1 + FLDLEN_ID + FLDLEN_LENGTH + s_currentFrame.length*2);
			s_currentFrame.timestamp |= (ascii_to_hex(c)
					<< 4 * (FLDLEN_TIMESTAMP - 1 - di));
			if (i == FLDLEN_ID + FLDLEN_LENGTH + s_currentFrame.length * 2 + FLDLEN_TIMESTAMP)
			{
				memcpy(&s_frame_buffer[0], &s_currentFrame, sizeof(CANFrame));
				s_frame_buffer_index++;
				s_parsing_state = INITIAL;
			}
			break;

		default:
			break;
		}
		i++;
	}
	return s_frame_buffer_index - bufidx_pre_parse;
}

int canusb_poll(void)
{
	canusb_read();
	return canusb_parse();
}

CANFrame* canusb_get_frame(unsigned int index)
{
	if (s_frame_buffer_index == -1 || index > s_frame_buffer_index)
		return NULL;
	else
		return &s_frame_buffer[index];
}
