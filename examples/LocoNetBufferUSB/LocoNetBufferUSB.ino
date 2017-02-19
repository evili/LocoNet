/****************************************************************************
 * 	Copyright (C) 2011 Jeffrey Marten Gillmor
 * 
 * 	This library is free software; you can redistribute it and/or
 * 	modify it under the terms of the GNU Lesser General Public
 * 	License as published by the Free Software Foundation; either
 * 	version 2.1 of the License, or (at your option) any later version.
 * 
 * 	This library is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * 	Lesser General Public License for more details.
 * 
 * 	You should have received a copy of the GNU Lesser General Public
 * 	License along with this library; if not, write to the Free Software
 * 	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 * 	IMPORTANT:
 * 
 * 	Some of the message formats used in this code are Copyright Digitrax, Inc.
 * 	and are used with permission as part of the EmbeddedLocoNet project. That
 * 	permission does not extend to uses in other software products. If you wish
 * 	to use this code, algorithm or these message formats outside of
 * 	EmbeddedLocoNet, please contact Digitrax Inc, for specific permission.
 * 
 * 	Note: The sale any LocoNet device hardware (including bare PCB's) that
 * 	uses this or any other LocoNet software, requires testing and certification
 * 	by Digitrax Inc. and will be subject to a licensing agreement.
 * 
 * 	Please contact Digitrax Inc. for details.
 * /
/*****************************************************************************
* 
* Title :   LocoNet USB Buffer arduino sketch
* Author:   Jeffrey Marten Gillmor jeffrey.martengillmor@gmail.com
* Date:     15-Nov-2011
* Software: AVR-GCC
* Target:   Arduino
* Review:   Dani Guisado Serra (dguisado@gmail.com) from ClubNCaldes http://www.clubncaldes.com
* Review Date: 5/7/2016
* 
* DESCRIPTION
* This is an arduino sketch that acts as a buffer between loconet and ethernet.
* It uses the LoconetOverTCP protocol that was designed by Stefan Bormann, a
* description of which is available here:
*        http://loconetovertcp.sourceforge.net/Protocol/index.html
* 
* 	As TCP packets are received, they are scanned for loconet messages, if a 
*   loconet message is successfuly decoded, it is then transmitted out over 
*   loconet. Likewise, if a loconet mesage is received via loconet, this is
*   packaged up and sent out over TCP.
* 
*   The Loconet RX is on arudino pin 8, the tx on arduino pin 6.
*   The Ethernet connection is via an arduino ethernet shield connected to 
*   arduino pins 13,12,11 & 10.
*****************************************************************************/

/* enable this to print debug information via the serial port */
#define DEBUG 1

#include <LocoNet.h>
#include "LocoNetBufferUSB.h"
#include "debug.h"


const char digitMap[] = "0123456789abcdef";
const char receiveString[] = "RECEIVE ";
static   LnBuf        LnTxBuffer ;
static   lnMsg        *LnPacket;

boolean gotASerialMessage = false; // whether or not you got a message from the client yet


static void SendLoconetMessageViaSerial(lnMsg *LoconetPacket, uint8_t PacketSize);
static boolean ProcessSerialRxByte(uint8_t offset, uint8_t rxByte, LnBuf* LnTxBuffer);

#ifdef DEBUG
static void dumpPacket(char* dPacket);
#endif

void setup()
{
	/* First initialize the LocoNet interface */
	LocoNet.init(LOCONET_BUFFER_PIN);

	/*Initialize a LocoNet packet buffer to buffer bytes from the PC */
	initLnBuf(&LnTxBuffer) ;

	/* initialise the Serial device */
	Serial.begin(57600);
    while(!Serial) {}
    Serial.write("VERSION ArduinoLoconetEtherBuffer V0.2 built 17 November 2011\r\n");

	/* Configure the console port for 57600 baud */
 #ifdef DEBUG
    if(console != Serial)
    {  
      console.begin(57600);
      console.println("LoconetOverSerial buffer started!!");
    }
 #endif

}

void loop()
{  
	char RxByte = 0;
	static boolean IdenFound = false;
	static int IdenBytes = 0;
	int MsgIdx = 8;

	int gotSerial = Serial.available();

#ifdef DEBUG
    if(console.available())
      getConsoleInput();
#endif


	/* Check for any received loconet packets */
	LnPacket = LocoNet.receive() ;
	if( LnPacket )
	{
		/* If a packet has been received, get the length of the 
       loconet packet */
		uint8_t LnPacketSize = getLnMsgSize( LnPacket ) ;


#ifdef DEBUG
		console.print("Received a message of ");
		console.print(LnPacketSize, DEC);
		console.println(" Bytes from loconet,");
#endif

		/* Send the loconet packet out over Serial */
		SendLoconetMessageViaSerial(LnPacket, LnPacketSize);
	}


	/* Check for any received Serial packets */
	if (gotSerial)
	{
		if (!gotASerialMessage)
		{
      #ifdef DEBUG
			console.println("We have a new client");
      #endif
			gotASerialMessage = true;
		}

		RxByte = Serial.read();

		switch (RxByte)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ProcessSerialRxByte(0x30, (uint8_t)RxByte, &LnTxBuffer);
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'F':
			ProcessSerialRxByte(0x37, (uint8_t)RxByte, &LnTxBuffer);
			break;

		case ' ':
			/* Do nothing */
			break;

		case 'S':
			IdenBytes = 1;
			if (IdenFound == true)
			{
				IdenFound = false;
			}
			break;

		case 'E':
			if(IdenFound == true)
				ProcessSerialRxByte(0x37, (uint8_t)RxByte, &LnTxBuffer);
			else
				IdenBytes++;
			break;

		case 'N':
			IdenBytes++;
			break;

		case 'D':
			if(IdenFound == true)
      {
				ProcessSerialRxByte(0x37, (uint8_t)RxByte, &LnTxBuffer);				
      }
			else
			{
				if(IdenBytes == 3)
				{
#ifdef DEBUG
					console.print("Received a Serial message: ");
#endif
					IdenFound = true;
				}
				IdenBytes = 0;
			}
			break;

		case -1:
		default:
			if (IdenFound == true)
			{
				Serial.write("SENT OK \r\n");
#ifdef DEBUG
        console.println("!! SENT OK !! ");
#endif
				IdenFound = false;
				IdenBytes = 0;
			}
			break;
		}
	}	
	else
	{
		IdenFound = false;
		IdenBytes = 0;
	}

}



static boolean ProcessSerialRxByte(uint8_t offset, uint8_t rxByte, LnBuf* LnTxBuffer)
{
	static boolean highNibble = true;
	static uint8_t loconetByte = 0; 
	static lnMsg   *LoconetPacket;

	if (highNibble)
	{
		loconetByte = (rxByte - offset) << 4;
		highNibble = false;
		return false;
	}
	else
	{
		loconetByte |= (rxByte - offset);
		highNibble = true;

#ifdef DEBUG
		console.print(" 0x");
		if(loconetByte < 16)
		{
		    console.print("0");
		}
		console.print(loconetByte, HEX);
#endif
		/* Add it to the buffer */
		addByteLnBuf( LnTxBuffer, loconetByte ) ;

		/* Check to see if we have received a complete packet yet */
		LoconetPacket = recvLnMsg( LnTxBuffer ) ;
		if(LoconetPacket )
		{
			/* Send the packet received via Serial to the LocoNet*/
			LocoNet.send( LoconetPacket ) ;
#ifdef DEBUG
				console.println(" ");
#endif	
			return true;
		}
		return false;    
	}
}


static void SendLoconetMessageViaSerial(lnMsg *LoconetPacket, uint8_t PacketSize)
{
	char serialTxMsg[8+(PacketSize*3)+2] ;
	uint8_t MsgIdx = 8;

	/* Initialise the the buffer that will be use to construct the message that is to 
           be transmitted over LoconetOverTCP*/
	memset(serialTxMsg,0,sizeof(serialTxMsg));
	memcpy(serialTxMsg,receiveString, sizeof(receiveString));


	// Send the received packet out byte by byte to the PC
	for( uint8_t Index = 0; Index < PacketSize; Index++ )
	{
		/* Convert the high nibble of the current Loconet Packet byte to ASCII */
		serialTxMsg[MsgIdx++] = (digitMap[(LoconetPacket->data[Index] >> 4) & 0xF]);

		/* Convert the low nibble of the current Loconet Packet byte to ASCII */
		serialTxMsg[MsgIdx++] =(digitMap[(LoconetPacket->data[ Index ] & 0xF)]);

		/* Add a "space" between the current and next "loconet byte" */
		serialTxMsg[MsgIdx++] = ' ';
	}
	/* Decrement the last MsgIdx, as there is no need for a space after the last byte */
	MsgIdx--;

	/* For some reason JMRI likes to have a "\r\n" at the end of the Serial message, though the
       loconetOverTCP standard states that an \r or an \n is accepetable */
	serialTxMsg[MsgIdx++] = 0xD;
	serialTxMsg[MsgIdx++] = 0xA;
	serialTxMsg[MsgIdx] = 0;

	/* Send the message out over Serial */
	Serial.write(serialTxMsg);

#ifdef DEBUG
	dumpPacket(serialTxMsg);
#endif
}



#ifdef DEBUG
static void dumpPacket(char* dPacket)
{
	int i = 0;
	console.println("# Dumping TX TCP Packet");
	console.print("## 0: ");
	while(*dPacket != 0)
	{
		console.print("0x:");
		if(*dPacket < 16)
		{
			console.print("0");
		}
		console.print(*dPacket,HEX);
		i++;
		*dPacket++;
		if((i%10) == 0)
		{
                        console.println("  ");
			console.print(i, DEC);
			console.print("  ");
		}
		else
		{
			console.print(" ");
		}
	} 
	console.println("\r\n");
}

#endif
