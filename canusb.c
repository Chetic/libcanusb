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

char buf[1024];

int canusb_init(void)
{
        return serial_init();
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
        int n, i;

        serial_write("V\r", 2);
        usleep(1000000);
        n = serial_read(buf, sizeof buf);

        log_write("CANUSB ");
        log_write("%s", buf);
}

void canusb_read(void)
{
        int n, i;
        n = serial_read(buf, sizeof buf);

        for (i = 0; i < n; i++)
        {
                log_write("%d ", buf[i]);
        }
}
