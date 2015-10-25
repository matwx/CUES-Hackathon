#ifndef LPD8806_H
#define LPD8806_H

#include <stdint.h>

typedef uint8_t byte;
typedef bool boolean;

class LPD8806 {

 public:

  LPD8806(uint16_t n, uint8_t dpin, uint8_t cpin); // Configurable pins
  LPD8806(uint16_t n); // Use SPI hardware; specific pins only
  LPD8806(void); // Empty constructor; init pins & strip length later
  void
    begin(void),
    setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b),
    setPixelColor(uint16_t n, uint32_t c),
    show(void),
    updatePins(void),                       // Change pins, hardware SPI
    updateLength(uint16_t n);               // Change strip length
  uint16_t
    numPixels(void);
  uint32_t
    Color(byte, byte, byte),
    getPixelColor(uint16_t n);

 private:

  uint16_t
    numLEDs,    // Number of RGB LEDs in strip
    numBytes;   // Size of 'pixels' buffer below
  uint8_t
    *pixels,    // Holds LED color values (3 bytes each) + latch bytes
    clkpin    , datapin;     // Clock & data pin numbers
  void
    startSPI(void);
  boolean
    begun;       // If 'true', begin() method was previously invoked
};

#endif
