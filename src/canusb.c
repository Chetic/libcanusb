#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "serial.h"
#include "log.h"
#include "canusb.h"
#include "circbuf.h"

static char buf[SERIAL_BUFFER_LENGTH];
static CANFrame frameBuffer[CANFRAME_BUFFER_LENGTH];

typedef enum {
    INITIAL,
    ID,
    LENGTH,
    DATA,
    TIMESTAMP
} canusb_parse_states_t;
static canusb_parse_states_t parsing_state = INITIAL;

int canusb_init(char* portname)
{
        return serial_init(portname);
}

int canusb_send_cmd(char* cmd)
{
        int n, i;

        serial_write(cmd, strlen(cmd));
        usleep(100000);
        n = serial_read(buf, sizeof buf);

        log_write("[ ");
        for (i = 0; i < n; i++)
        {
                if (buf[i] == 7)
                        log_write("ERR ");
                else if (buf[i] == 10)
                        log_write("OK (10) ");
                else if (buf[i] == 13)
                        log_write("OK (13) ");
                else
                        log_write("%d ", buf[i]);
        }
        log_write("]\n");
}

void canusb_print_version(void)
{
        int bytes_read;

        serial_write("V\r", 2);
        usleep(1000000); //TODO: Wait appropriate amount
        bytes_read = serial_read(buf, sizeof buf);

        log_write("CANUSB ");
        log_write("%s\n", buf);
}

void canusb_print_serial_number(void)
{
        int bytes_read;

        serial_write("N\r", 2);
        usleep(1000000); //TODO: Wait appropriate amount
        bytes_read = serial_read(buf, sizeof buf);

        log_write("%s\n", buf);
}

void canusb_print_status(void)
{
        int bytes_read;

        serial_write("F\r", 2);
        usleep(1000000); //TODO: Wait appropriate amount
        bytes_read = serial_read(buf, sizeof buf);

        log_write("%s\n", buf);
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
    int n;
    char* bufSearch;
    n = serial_read(buf, sizeof buf);
    log_write("%s", buf);
    circbuf_add(buf, n);
}

void canusb_parse(void)
{
    while (circbuf_len())
    {
        char c = circbuf_pop();
        switch(parsing_state)
        {
            case INITIAL:
            break;

            case ID:
            break;

            case LENGTH:
            break;

            case DATA:
            break;

            case TIMESTAMP:
            break;

            default:
            break;
        }
    }
}

void canusb_poll(void)
{
	canusb_read();
	canusb_parse();
}
