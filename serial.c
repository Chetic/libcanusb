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

static int fd;
static const char *portname = "/dev/ttyUSB0";

void serial_set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr(fd, &tty) != 0)
        {
                printf("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
                printf("error %d setting term attributes", errno);
}

int serial_set_interface_attribs(int fd, int speed, int parity)
{
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr(fd, &tty) != 0)
        {
                printf("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
                printf("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

int serial_init(void)
{
        fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);

        if (fd < 0)
        {
                log_write("error %d opening %s: %s", errno, portname, strerror(errno));
                return -1;
        }

        serial_set_interface_attribs(fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
        serial_set_blocking(fd, 0);                // set no blocking

        return 0;
}

void serial_write(char* data, int length)
{
        write(fd, data, strlen(data));
}

int serial_read(char* data, int maxLength)
{
        return read(fd, data, maxLength);
}
