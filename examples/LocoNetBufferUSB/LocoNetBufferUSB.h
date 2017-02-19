#ifndef _LOCONET_BUFFER_USB_H
#define _LOCONET_BUFFER_USB_H


/* Debug over Serial1 (or 2, 3) only if available */

#ifdef DEBUG
#include "debug.h"
#endif /* DEBUG */

// Default LocoNet pin
#define LOCONET_BUFFER_PIN 7
#define LOCONET_BUFFER_SPEED 57600

#if 0
class LocoNetBufferUSB {
 private:
  LnBuf   LnBuffer ;

 public:
  LocoNetBufferUSB(Serial serial);
  ~LocoNetBufferUSB();
  void init();
}
#endif

#endif /*_LOCONET_BUFFER_USB_H */
