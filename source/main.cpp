/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include <map>

#include "Friend.h"


BLE ble;
//DigitalOut led1(p29, 1);
//DigitalOut led2(p28, 1);
std::map<int, int> totalS;

CUES::Friend f;
int minorSize = 10;
int minorAccpeted[10] = {1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251};

DigitalOut leds[2] = {DigitalOut(p29, 0), DigitalOut(p28, 0)};

static int OFF = 0;
static int ON = 1;

void blinkCallback(void)
{
  for(int i = 0; i != minorSize; i++) {
     std::map<int,int>::iterator it = totalS.find(minorAccpeted[i]);
     if (it != totalS.end()) {
       int minor = minorAccpeted[i];
       totalS[minor] = max(0,totalS[minor] - 1);
       if (totalS[minor] == 0) {
         leds[i%2].write(OFF);
       }
     }
   }


}

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{

struct iBeacon {
  char start[9];
  char uuid[16];
  char major[2];
  char minor[2];
  char power[1];
};

  const uint8_t uuid[] = {0xE3, 0xAA, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                          0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
  if (params->advertisingDataLen == 30) {


    iBeacon *b = (iBeacon*)params->advertisingData;
    if (memcmp(b->uuid, uuid, sizeof(uuid)) == 0) {
      int minor =  b->minor[0] * 256 + b->minor[1];
      //if (f.canConnect(minor)) {
      std::map<int,int>::iterator it;
      it = totalS.find(minor);
      if (it != totalS.end()) {
        totalS[minor] = 10;
        for(int i = 0; i != minorSize; i++) {
          if (minorAccpeted[i] == minor)
            leds[i%2] = ON;
        }
      }
      else {
        totalS.insert(std::pair<int,int>(minor,10));
      }
    }
  //  }
  }
}

void app_start(int argc, char *argv[])
{
    minar::Scheduler::postCallback(blinkCallback).period(minar::milliseconds(500));
    ble.init();

    /**
     * The Beacon payload has the following composition:
     * 128-Bit / 16byte UUID = E2 0A 39 F4 73 F5 4B C4 A1 2F 17 D1 AD 07 A9 61
     * Major/Minor  = 0x1122 / 0x3344
     * Tx Power     = 0xC8 = 200, 2's compliment is 256-200 = (-56dB)
     *
     * Note: please remember to calibrate your beacons TX Power for more accurate results.
     */


    const uint8_t uuid[] = {0xE3, 0xAA, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                            0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
    uint16_t majorNumber = 1122;
    uint16_t minorNumber = 1244;
    uint16_t txPower     = 0xC9;
    iBeacon ibeacon(ble, uuid, majorNumber, minorNumber, txPower);


    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
    ble.gap().setScanParams(1800 /* scan interval */, 1500 /* scan window */);
    ble.gap().startScan(advertisementCallback);
    //led1 = 0;
}
