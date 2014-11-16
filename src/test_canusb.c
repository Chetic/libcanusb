#include <stdio.h>
#include <string.h>
#include "circbuf.h"
#include "canusb.h"

int test_parsing(void)
{
	int i;
	char *frame1_data[] = {"tAB", "C80", "0010203", "04050607F", "FFF"};
	char *frame2_data = "tABC80001020304050607FFFF";
	CANFrame* frame;

	for (i = 0; i < 4; i++)
	{
		circbuf_add(frame1_data[i], strlen(frame1_data[i]));
		canusb_parse();
	}
	circbuf_add(frame1_data[4], strlen(frame1_data[4]));
	circbuf_add(frame2_data, strlen(frame2_data));
	if (canusb_parse() != 2)
		return 0;

	frame = canusb_get_frame(0);
	if (frame)
	{
		printf("ID:        %03x\n", frame->id);
		printf("Length:    %d\n", frame->length);
		printf("Data:      %02x%02x%02x%02x%02x%02x%02x%02x\n", frame->data[0],
				frame->data[1], frame->data[2], frame->data[3], frame->data[4],
				frame->data[5], frame->data[6], frame->data[7]);
		printf("Timestamp: %04x\n", frame->timestamp);

		if (frame->id != 0xABC)
			return 0;
		if (frame->length != 8)
			return 0;
		for (i = 0; i < frame->length; i++)
		{
			if (frame->data[i] != i)
				return 0;
		}
		if (frame->timestamp != 0xFFFF)
			return 0;

		return 1;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char* argv[])
{
	int success = 0;

	success |= test_parsing();

	if (success)
		printf("Success!\n");
	else
		printf("Test failed.\n");

	return (success == 1);
}
