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
