#include "d2xxif.h"
#include "canusb.h"
#include <stdio.h>

static FT_STATUS ftStatus;
static FT_HANDLE ftHandle;
static char strACR[16]; // Filter registers
static char strACM[16]; // Filter registers
static int filtersConfigured = 0;
 
int canusb_init(int iport)
{
  int i;
  char c;
  int init_failed = 0;
 
  // Note!
  // The second version of open should work from version 0.4.9 it may be prefered
  // in many situations. On Fedora Core 4, kernal 2.6.15 it fails however.
  ftStatus = FT_Open(iport, &ftHandle);
  //ftStatus = FT_OpenEx( "LWO65RKA", FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
  //ftStatus = FT_OpenEx( NULL, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle); // First found
  if(ftStatus != FT_OK) {
    /* This can fail if the ftdi_sio driver is loaded
       use lsmod to check this and rmmod ftdi_sio to remove
       also rmmod usbserial */
    printf("FT_Open(%d) failed. rv=%d\n", iport, (int)ftStatus);
    return 1;
  }

  ftStatus = FT_SetTimeouts(ftHandle, 3000, 3000 ); // 3 second read timeout
  if(ftStatus != FT_OK) {
    printf("WARNING: FT_SetTimeouts failed. ftStatus=%d\n", (int)ftStatus);
  }
     
  while ( 0x0a == ( c = getchar() ));

  if (filtersConfigured) {
	init_failed |= sendDeviceMsg(ftHandle, strACR);
	init_failed |= sendDeviceMsg(ftHandle, strACM);
  }

  return init_failed;
}

void canusb_close(void)
{
  FT_Close(ftHandle);
}

int canusb_send_frame(CANMsg* frame)
{
  return sendFrame(ftHandle, frame);
}

int canusb_get_frame(CANMsg* frame)
{
  if (!readFrame(ftHandle, frame)) {
    printf("WARNING: Error while attempting to read frame\n");
    return 0;
  }
  return (frame != NULL);
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

	snprintf(strACR, sizeof strACR, "M%08X\r", ACR);
	snprintf(strACM, sizeof strACM, "m%08X\r", ACM);
	filtersConfigured = 1;
}

void canusb_filter_id(unsigned short id)
{
	//don't use filter1 (all fields 0)
	//filter2 id must match exactly (all idmask2 bits 0)
	//ignore filter2's rtr bit (rtrmask2 = 1)
	canusb_set_filter11(0x000, 0x000, 0x0, 0x0, 0x00, 0x00, id, 0x000, 0x0, 0x1);
}

void canusb_print_version(void)
{
}

void canusb_print_serial_number(void)
{
}

void canusb_enable_timestamps(void)
{
	if (!sendDeviceMsg(ftHandle, "Z1\r")) {
		printf("WARNING: Unable to enable timestamps\n");
	}
}

void canusb_disable_timestamps(void)
{
	if (!sendDeviceMsg(ftHandle, "Z0\r")) {
		printf("WARNING: Unable to enable timestamps\n");
	}
}

