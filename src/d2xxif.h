#ifndef D2XXIF_H
#define D2XXIF_H

#include <stdbool.h>
#include "ftd2xx.h"

// CAN Frame
typedef struct {
  unsigned long id;         // Message id
  unsigned long timestamp;  // timestamp in milliseconds
  unsigned char flags;      // [extended_id|1][RTR:1][reserver:6]
  unsigned char len;        // Frame size (0.8)
  unsigned char data[ 8 ];  // Databytes 0..7
} CANMsg;

#define SPEED   6  // Speed for interface - See CANUSB 'O' command
#define BUF_SIZE 0x1000

// Message flags
#define CANMSG_EXTENDED   0x80 // Extended CAN id
#define CANMSG_RTR        0x40 // Remote frame

bool getVersionInfo(FT_HANDLE ftHandle, char* str);
bool getSerialNumber(FT_HANDLE ftHandle, char* str);

// nSpeed 3 = 100kbps
bool openChannel(FT_HANDLE ftHandle, int nSpeed);
bool closeChannel(FT_HANDLE ftHandle);

bool sendDeviceMsg(FT_HANDLE ftHandle, char* str);

bool sendFrame(FT_HANDLE ftHandle, CANMsg *pmsg);

// Writes pMsg to a readable string str
bool canusbToCanMsg(CANMsg *pMsg, char* str);

// Read a frame from the buffer and place it in frame pointed to by 'frame'
bool readFrame(FT_HANDLE ftHandle, CANMsg *frame);
#endif

