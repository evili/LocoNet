/****************************************************************************
 *      Copyright (C) 2017 Evili del Rio
 * 
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 * 
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 * 
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 *      IMPORTANT:
 * 
 *      Some of the message formats used in this code are Copyright Digitrax, Inc.
 *      and are used with permission as part of the EmbeddedLocoNet project. That
 *      permission does not extend to uses in other software products. If you wish
 *      to use this code, algorithm or these message formats outside of
 *      EmbeddedLocoNet, please contact Digitrax Inc, for specific permission.
 * 
 *      Note: The sale any LocoNet device hardware (including bare PCB's) that
 *      uses this or any other LocoNet software, requires testing and certification
 *      by Digitrax Inc. and will be subject to a licensing agreement.
 * 
 *      Please contact Digitrax Inc. for details.
 * /
/*****************************************************************************
* 
* Title :   LocoNet USB Buffer arduino sketch
* Author:   Evili del Rio <evili.del.rio@gmail.com>
* Date:     07-02-2017
* Software: AVR-GCC
* Target:   Arduino
* 
* DESCRIPTION
* This is an arduino sketch that acts as a buffer between loconet and Arduino's USB.
*
* See the README.md for details on how to use it
*
*****************************************************************************/

#include <LocoNet.h>
#include "LocoNetBufferUSB.h"

static lnMsg *rxLnPacket;
static LnBuf txLnBuffer;

void setup()
{
    LocoNet.init(LOCONET_BUFFER_PIN);
    Serial.begin(LOCONET_BUFFER_SPEED);
    
}

void loop()
{
    if(LocoNet.available())
    {
        rxLnPacket = LocoNet.receive();
        sendLoconetMessage(rxLnPacket);
    }
}

void sendLoconetMessage(lnMsg *msg)
{
    uint8_t msgSize = getLnMsgSize(msg);
    while(Serial.availableForWrite() < msgSize){}
    Serial.write(msg->data, msgSize);
}

void serialEvent()
{   static lnMsg *lnUSBmsg;
    while(Serial.available())
    {
      uint8_t usbByte = Serial.read();
      addByteLnBuf(&txLnBuffer, usbByte);
      lnUSBmsg = recvLnMsg( &txLnBuffer );
      if(lnUSBmsg)
      {
        LocoNet.send(lnUSBmsg);
      }
    }
}
