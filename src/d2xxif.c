#include <stdio.h>
#include <string.h>
#include "ftd2xx.h"
#include "d2xxif.h"

#define MAX( a, b )( ( (a) > (b) ) ? (a) : (b) )
#define MIN( a, b )( ( (a) < (b) ) ? (a) : (b) )
#define ABS( a )(( (int) (a) < 0 ) ? ((a) ^ 0xffffffff) + 1 : (a) )

#define CANUSB_STATE_NONE  0
#define CANUSB_STATE_MSG   1

// Globals
static char gbufferRx[ BUF_SIZE ];
static int gnReceivedFrames;

bool getVersionInfo(FT_HANDLE ftHandle, char* str)
{
  FT_STATUS status;
  char buf[80];
  char c;
  char *p;
  unsigned long nBytesWritten;
  unsigned long eventStatus;
  unsigned long nRxCnt;// Number of characters in receive queue
  unsigned long nTxCnt;// Number of characters in transmit queue

  memset( buf, 0, sizeof( buf ) );

  FT_Purge( ftHandle, FT_PURGE_RX | FT_PURGE_TX );

  sprintf( buf, "V\r" );
  if ( FT_OK != ( status = FT_Write( ftHandle, buf, strlen( buf ), &nBytesWritten ) ) ) {
    printf("Error: Failed to write command. return code = %d\n", (int)status );
    return false;
  }
  
  // Check if there is something to receive
  while (1) {
    if ( FT_OK == FT_GetStatus( ftHandle, &nRxCnt, &nTxCnt, &eventStatus ) ) {
      // If there are characters to receive
      if ( nRxCnt ) {
	if ( FT_OK != ( status = FT_Read( ftHandle, buf, nRxCnt, &nBytesWritten ) ) ) {
	  printf("Error: Failed to read data. return code = %d\n", (int)status );
	  return false;
	}
	
	p = buf;
	while ( *p ) {
	  if ( 0x0d == *p ) {
	    *p = 0;
	    break;
	  }
	  p++;
	}
	printf( "Version = %s\n", buf );
	return true;
      }
    }
    else {
      printf("Error: Failed to get status. return code = %d\n", (int)status );
      return false;
    }
  }
}

bool getSerialNumber(FT_HANDLE ftHandle, char* str)
{
  FT_STATUS status;
  char buf[80];
  char c;
  char *p;
  unsigned long nBytesWritten;
  unsigned long eventStatus;
  unsigned long nRxCnt;// Number of characters in receive queue
  unsigned long nTxCnt;// Number of characters in transmit queue

  memset( buf, 0, sizeof( buf ) );

  FT_Purge( ftHandle, FT_PURGE_RX | FT_PURGE_TX );

  sprintf( buf, "N\r" );
  if ( FT_OK != ( status = FT_Write( ftHandle, buf, strlen( buf ), &nBytesWritten ) ) ) {
    printf("Error: Failed to write command. return code = %d\n", (int)status );
    return false;
  }
  
  // Check if there is something to receive
  while (1) {
    if ( FT_OK == FT_GetStatus( ftHandle, &nRxCnt, &nTxCnt, &eventStatus ) ) {
      // If there are characters to receive
      if ( nRxCnt ) {
	if ( FT_OK != ( status = FT_Read( ftHandle, buf, nRxCnt, &nBytesWritten ) ) ) {
	  printf("Error: Failed to read data. return code = %d\n", (int)status );
	  return false;
	}
	
	p = buf;
	while ( *p ) {
	  if ( 0x0d == *p ) {
	    *p = 0;
	    break;
	  }
	  p++;
	}
	sprintf(str, "%s", buf);
	return true;
      }
    }
    else {
      printf("Error: Failed to get status. return code = %d\n", (int)status);
      return false;
    }
  }
}

bool openChannel(FT_HANDLE ftHandle, int nSpeed)
{
  char buf[80];
  unsigned long size;
  unsigned long retLen;
  
  // Set baudrate
  FT_Purge( ftHandle, FT_PURGE_RX );

  sprintf( buf, "S%d\r", nSpeed );
  size = 3;
  if ( !( FT_OK == FT_Write( ftHandle, buf, size, &retLen ) ) ) { 
    printf("Write failed\n");
    return false;
  }
      
  // Open device
  FT_Purge( ftHandle, FT_PURGE_RX );
  strcpy( buf, "O\r" );
  size = 2;
  if ( !( FT_OK == FT_Write( ftHandle, buf, size, &retLen ) ) ) {
    printf("Write failed\n");
    return false;
  }

  return true;
}

bool closeChannel( FT_HANDLE ftHandle )
{
  char buf[80];
  unsigned long size;
  unsigned long retLen;

  // Close device
  FT_Purge( ftHandle, FT_PURGE_RX | FT_PURGE_TX );
  strcpy( buf, "C\r" );
  size = 2;
  if ( !( FT_OK == FT_Write( ftHandle, buf, size, &retLen ) ) ) { 
    return false;
  }

  
  return true;
}

bool sendDeviceMsg(FT_HANDLE ftHandle, char* str)
{
  unsigned long size;
  unsigned long retLen;

  size = strlen(str);
  FT_Purge( ftHandle, FT_PURGE_RX | FT_PURGE_TX ); 

  return (FT_OK == FT_Write(ftHandle, str, size, &retLen));
}

bool sendFrame( FT_HANDLE ftHandle, CANMsg *pmsg )
{
  int i; 
  char txbuf[80];
  unsigned long size;
  unsigned long retLen;

  if ( pmsg->flags & CANMSG_EXTENDED ) {
    if ( pmsg->flags & CANMSG_RTR ) {
      sprintf( txbuf, "R%8.8lX%i", pmsg->id, pmsg->len );
      pmsg->len = 0; 
    }
    else {
      sprintf( txbuf, "T%8.8lX%i", pmsg->id, pmsg->len );
    }
  }
  else {
    if ( pmsg->flags & CANMSG_RTR ) {
      sprintf( txbuf, "r%3.3lX%i", pmsg->id, pmsg->len );
      pmsg->len = 0; // Just dlc no data for RTR
    }
    else {
      sprintf( txbuf, "t%3.3lX%i", pmsg->id, pmsg->len );
    }
  }

  if ( pmsg->len ) {
    char hex[5];
    
    for ( i= 0; i< pmsg->len; i++ ) {
      sprintf( hex, "%2.2X", pmsg->data[i] );
      strcat( txbuf, hex );
    }
  }

  // Add CR
  strcat( txbuf, "\r" );

  size = strlen( txbuf );

  // Transmit fram
  if ( !( FT_OK == FT_Write( ftHandle, txbuf, size, &retLen ) ) ) { 
    return false;
  }
  

  return true;
}

bool canusbToCanMsg(CANMsg *pMsg, char* str)
{
  int val;
  int i;
  short data_offset;   // Offset to dlc byte
  char save;
  char* p = str;

  if ( 't' == *p ) {
    // Standard frame
    pMsg->flags = 0;
    data_offset = 5;
    pMsg->len = p[ 4 ] - '0';
    p[ 4 ] = 0;
    sscanf( p + 1, "%lx", &pMsg->id  );
  }
  else if ( 'r' == *p ) {
    // Standard remote  frame
    pMsg->len = p[ 4 ] - '0';
    pMsg->flags = CANMSG_RTR;
    //data_offset = 5 - 1;// To make timestamp work
    data_offset = 5;
    //save = p[ 4 ];
    p[ 4 ] = 0;
    sscanf( p + 1, "%lx", &pMsg->id  );
    //p[ 4 ] = save;
  }
  else if ( 'T' == *p ) {
    // Extended frame
    pMsg->flags = CANMSG_EXTENDED;
    data_offset = 10;
    pMsg->len = p[ 9 ] - '0';
    p[ 9 ] = 0;
    sscanf( p + 1, "%lx", &pMsg->id );
  }
  else if ( 'R' == *p ) {
    // Extended remote frame
    pMsg->flags = CANMSG_EXTENDED | CANMSG_RTR;
    //data_offset = 10 - 1;// To make timestamp work
    data_offset = 10;
    pMsg->len = p[ 9 ] - '0';
    //save = p[ 9 ];
    p[ 9 ] = 0;
    sscanf( p + 1, "%lx", &pMsg->id );
    //p[ 9 ] = save;
  }
  
  save = *(p + data_offset + 2 * pMsg->len );
  
  // Fill in data
  if ( !( pMsg->flags & CANMSG_RTR ) ) {
    for ( i= MIN( pMsg->len, 8); i > 0; i-- ) {
      *(p + data_offset + 2 * (i-1) + 2 )= 0;
      sscanf( p + data_offset + 2 * (i-1), "%x", &val );
      pMsg->data[ i - 1 ] = val;
    }
  }

  *(p + data_offset + 2 * pMsg->len ) = save;

  if ( !( pMsg->flags & CANMSG_RTR ) ) {
    // If timestamp is active - fetch it
    if ( 0x0d != *( p + data_offset + 2 * pMsg->len ) ) {
      p[ data_offset + 2 * ( pMsg->len ) + 4 ] = 0;
      sscanf( ( p + data_offset + 2 * ( pMsg->len ) ), "%x", &val );
      pMsg->timestamp = val;
    }
    else {
      pMsg->timestamp = 0;
    }
  }
  else {
    
    if ( 0x0d != *( p + data_offset ) ) {
      p[ data_offset + 4 ] = 0;
      sscanf( ( p + data_offset ), "%x", &val );
      pMsg->timestamp = val;
    }
    else {
      pMsg->timestamp = 0;
    }
  }

  return true;
}

bool readFrame( FT_HANDLE ftHandle, CANMsg *frame )
{
  CANMsg msg;
  int i,j;
  char buf[80];
  unsigned long nRxCnt;
  unsigned long nTxCnt;
  unsigned long eventStatus;
  unsigned long nRcvCnt;
  char c;
  
  static char msgReceiveBuf[80];
  static int cntMsgRcv = 0;
  static int state = CANUSB_STATE_NONE;
  
  // Check if there is something to receive
  if ( FT_OK == FT_GetStatus( ftHandle, &nRxCnt, &nTxCnt, &eventStatus ) ) {
    // If there are characters to receive
    if ( nRxCnt ) {
      // Must fit to buffer
      if ( nRxCnt > sizeof( gbufferRx ) ) {
	nRxCnt = sizeof( gbufferRx );
      }
      // Read data
      if ( ( FT_OK == FT_Read( ftHandle, gbufferRx, nRxCnt, &nRcvCnt ) ) 
	   && ( nRcvCnt > 0 ) ) {
	
	for ( i=0; i<nRcvCnt; i++ ) {
	  // Get character
	  c = gbufferRx[ i ];
	  
	  if ( CANUSB_STATE_NONE == state ) {
	    if ( ('t' == c ) || 
		 ( 'T' == c ) || 
		 ('r' == c ) || 
		 ( 'R' == c ) ) {
	      state = CANUSB_STATE_MSG;
	      memset( msgReceiveBuf, 0, sizeof( msgReceiveBuf ) );
	      msgReceiveBuf[ 0 ] = c;
	      cntMsgRcv = 1;
	    }
	  }
	  else if ( CANUSB_STATE_MSG == state ) {
	    msgReceiveBuf[ cntMsgRcv++ ] = c;
	    if ( 0x0d == c ) {
	      printf("Raw Msg = %s\n", msgReceiveBuf );
	      if ( !canusbToCanMsg( (CANMsg*)msgReceiveBuf, (char*)&msg ) ) {
		printf("Message conversion failed!\n");
		state = CANUSB_STATE_NONE;
		return false;
	      }
	       
	      if ( msg.flags & CANMSG_EXTENDED  ) {
		printf("Extended ");
	      }
	      else {
		printf("Standard ");
	      }

	      printf("message received: id=%X len=%d timestamp=%X ", 
		     (unsigned int)msg.id, 
		     (unsigned int)msg.len, 
		     (unsigned int)msg.timestamp ); 
	      
	      if ( msg.len ) {
		printf("data=");
		for ( j=0; j<msg.len; j++ ) {
		  printf("%02X ", msg.data[j]);
		}
	      }

	      printf("\n");
	      gnReceivedFrames++;
	      state = CANUSB_STATE_NONE;
	    } // full message
	  } // STATE_MSG
	} // for each char
      } // Read data
    } // characters to receive
  } // getstatus

  if (cntMsgRcv == 0) frame = NULL; 
  return true;
}
