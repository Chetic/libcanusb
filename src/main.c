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

void sig_handler(int signo) {
	if (signo == SIGINT)
		s_running = 0;
}

void shutdown()
{
	log_write("\rClosing CAN channel... \t\t");
	canusb_send_cmd("C\r");
	log_close();
}

int main(int argc, char* argv[]) {
	log_init();

	if (argc < 2)
	{
		log_write("Too few arguments. Please provide the path to your serial port (e.g. /dev/ttyUSB0)");
		log_close();
		exit(1);
	}

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		log_write("\ncan't catch SIGINT\n");

	canusb_init(argv[1]);
	canusb_print_version();

	log_write("Setting CANUSB to 100kbps... \t");
	canusb_send_cmd("S3\r");

	log_write("Opening CAN channel... \t\t");
	canusb_send_cmd("O\r");

	while (s_running) //key not pressed
	{
		canusb_read();
		fflush(stdout);
	}

	shutdown();

	return 0;
}
