#include <stdlib.h>
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
	log_init();

	if (argc < 2)
	{
		log_write(
				"Too few arguments. Please provide the path to your serial port (e.g. /dev/ttyUSB0)");
		log_close();
		exit(1);
	}

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		log_write("\ncan't catch SIGINT\n");

	canusb_init(argv[1]);

	log_write("Version:\t");
	canusb_print_version();
	log_write("Serial number:\t");
	canusb_print_serial_number();
	log_write("Status flags:\t");
	canusb_print_status();

	log_write("Disabling timestamps... \t");
	canusb_disable_timestamps();
	log_write("Setting acceptance code... \t");
	canusb_send_cmd("M00001D60\r");
	log_write("Setting acceptance mask... \t");
	canusb_send_cmd("m00001FF0\r");
	log_write("Setting CANUSB to 100kbps... \t");
	canusb_send_cmd("S3\r");
	log_write("Opening CAN channel... \t\t");
	canusb_send_cmd("O\r");

	while (s_running)
	{
		int i;
		CANFrame* frame;
		int n;

		n = canusb_poll();

		for (i = 0; i < n; i++)
		{
			frame = canusb_get_frame(i);

			if (frame->id == 0x1D6)
			{
				log_write("Steering wheel: %02x%02x\n", frame->data[0], frame->data[1]);
			}
		}
		canusb_reset();
	}

	log_write("\rClosing CAN channel... \t\t");
	canusb_send_cmd("C\r");
	log_close();

	return 0;
}
