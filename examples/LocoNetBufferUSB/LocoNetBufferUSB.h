#ifndef _LOCONET_BUFFER_USB_H
#define _LOCONET_BUFFER_USB_H


/* Debug over Serial1 (or 2, 3) only if available */
#if defined(HAVE_HWSERIAL1)
#if defined(DEBUG)
#define LOCONET_BUFFER_USB_DEBUG Serial1
#endif
#else

#define LOCONET_BUFFER_USB_DEBUG 0

#endif /* HAVE_HWSERIAL1 */

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
