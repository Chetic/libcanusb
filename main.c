#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "log.h"
#include "canusb.h"

static int s_running = 1;

void sig_handler(int signo)
{
        if (signo == SIGINT)
                s_running = 0;
}

int main(int argc, char* argv[])
{
        if (signal(SIGINT, sig_handler) == SIG_ERR)
                log_write("\ncan't catch SIGINT\n");

	log_init() ? exit(1) : 0;
        canusb_init() ? exit(1):0;
	canusb_print_version();

        log_write("Setting CANUSB to 100kbps... \t");
	canusb_send_cmd("S3\r");

        log_write("Opening CAN channel... \t\t");
	canusb_send_cmd("O\r");

        while(s_running)//key not pressed
        {
                canusb_read();
                fflush(stdout);
        }

        log_write("\rClosing CAN channel... \t\t");
	canusb_send_cmd("C\r");
	log_close();

	return 0;
}
