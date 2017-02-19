#ifndef _LOCONET_BUFFER_USB_H
#define _LOCONET_BUFFER_USB_H


/* Debug over Serial1 (or 2, 3) only if available */

#if defined(DEBUG)

#if defined(HAVE_HWSERIAL1)
HardwareSerial console = Serial1;
#else
HardwareSerial console = Serial;
#endif /* HAVE_HWSERIAL1 */

#define _DEBUG_(x) console.println(x)
#define _KLDEBUG_(x)  console.print(x)

#else

#define _DEBUG_(x)
#define _KLDEBUG_(x)
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
