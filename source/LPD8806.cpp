/*
Arduino library to control LPD8806-based RGB LED Strips
Copyright (C) Adafruit Industries
MIT license
*/

#include "LPD8806.h"
#include "mbed.h"

SPI spi(SPIS_PSELMOSI, SPIS_PSELMISO, SPIS_PSELSCK);

#define spi_out(n) (void)spi.write(n)

/*****************************************************************************/

// Constructor for use with hardware SPI (specific clock/data pins):
LPD8806::LPD8806(uint16_t n) {
  pixels = NULL;
  begun  = false;
  updateLength(n);
  updatePins();
}

// Activate hard/soft SPI as appropriate:
void LPD8806::begin(void) {
  startSPI();
  begun = true;
}

// Change pin assignments post-constructor, switching to hardware SPI:
void LPD8806::updatePins(void) {
  // If begin() was previously invoked, init the SPI hardware now:
  if(begun == true) startSPI();
  // Otherwise, SPI is NOT initted until begin() is explicitly called.
}

// Enable SPI hardware and set up protocol details:
void LPD8806::startSPI(void) {
  //XXX SPI.setBitOrder(MSBFIRST);
  //XXX SPI.setDataMode(SPI_MODE0);
  
  // SPI bus is run at 2MHz.  Although the LPD8806 should, in theory,
  // work up to 20MHz, the unshielded wiring from the Arduino is more
  // susceptible to interference.  Experiment and see what you get.
  spi.frequency(1000000);
  
  // Issue initial latch/reset to strip:
  for(uint16_t i=((numLEDs+31)/32); i>0; i--) spi_out(0);
}

// Change strip length (see notes with empty constructor, above):
void LPD8806::updateLength(uint16_t n) {
  uint8_t  latchBytes;
  uint16_t dataBytes, totalBytes;

  numLEDs = numBytes = 0;
  if(pixels) free(pixels); // Free existing data (if any)

  dataBytes  = n * 3;
  latchBytes = (n + 31) / 32;
  totalBytes = dataBytes + latchBytes;
  if((pixels = (uint8_t *)malloc(totalBytes))) { // Alloc new data
    numLEDs  = n;
    numBytes = totalBytes;
    memset( pixels           , 0x80, dataBytes);  // Init to RGB 'off' state
    memset(&pixels[dataBytes], 0   , latchBytes); // Clear latch bytes
  }
  // 'begun' state does not change -- pins retain prior modes
}

uint16_t LPD8806::numPixels(void) {
  return numLEDs;
}

void LPD8806::show(void) {
  uint8_t  *ptr = pixels;
  uint16_t i    = numBytes;

  // This doesn't need to distinguish among individual pixel color
  // bytes vs. latch data, etc.  Everything is laid out in one big
  // flat buffer and issued the same regardless of purpose.
  while(i--) spi_out(*ptr++);
}

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t LPD8806::Color(byte r, byte g, byte b) {
  return ((uint32_t)(g | 0x80) << 16) |
         ((uint32_t)(r | 0x80) <<  8) |
                     b | 0x80 ;
}

// Set pixel color from separate 7-bit R, G, B components:
void LPD8806::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    *p++ = g | 0x80; // Strip color order is GRB,
    *p++ = r | 0x80; // not the more common RGB,
    *p++ = b | 0x80; // so the order here is intentional; don't "fix"
  }
}

// Set pixel color from 'packed' 32-bit GRB (not RGB) value:
void LPD8806::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) { // Arrays are 0-indexed, thus NOT '<='
    uint8_t *p = &pixels[n * 3];
    *p++ = (c >> 16) | 0x80;
    *p++ = (c >>  8) | 0x80;
    *p++ =  c        | 0x80;
  }
}

// Query color from previously-set pixel (returns packed 32-bit GRB value)
uint32_t LPD8806::getPixelColor(uint16_t n) {
  if(n < numLEDs) {
    uint16_t ofs = n * 3;
    return ((uint32_t)(pixels[ofs    ] & 0x7f) << 16) |
           ((uint32_t)(pixels[ofs + 1] & 0x7f) <<  8) |
            (uint32_t)(pixels[ofs + 2] & 0x7f);
  }

  return 0; // Pixel # is out of bounds
}

