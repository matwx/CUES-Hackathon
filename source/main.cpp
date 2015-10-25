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
#include "lightStripUtility.h"


BLE ble;
std::map<int, int> totalS;

DigitalOut vib(p5, 0);

CUES::Friend f;
int minorSize = 10;
int minorAccpeted[10] = {1244, 1245, 1246, 1247, 1248, 1249, 1250, 1251};

const uint8_t uuid[] = {0xE3, 0xAA, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                        0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
uint16_t majorNumber = 1122;

bool stationBeacon = true;

int clientColour = 1; //Needs to be hardcoded and unique for each friend (0 == RED, 1 == GREEN, 2 == BLUE)

DigitalOut leds[2] = {DigitalOut(p29, 0), DigitalOut(p28, 0)};
InterruptIn button(p4);

bool enabled = true;

static int OFF = 0;
static int ON = 1;

int redA = 127, blueA = 127, greenA = 127;
int numLit = 0;

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
//      if((b->major[0] * 256 + b->major[1]) != majorNumber) {
//          stationBeacon = true;
//          return;
//      }
      int minor =  b->minor[0] * 256 + b->minor[1];
      std::map<int,int>::iterator it;
      it = totalS.find(minor);
      if (it != totalS.end()) {
        totalS[minor] = 10;
        for(int i = 0; i != minorSize; i++) {
          if (minorAccpeted[i] == minor) {
              leds[i % 2] = ON;

              switch (i) {
                  case 0:
                      redA = 127;
                      break;
                  case 1:
                      greenA = 127;
                      break;
                  case 2:
                      blueA = 127;
              }
              numLit = 6;
          }
        }
      }
      else {
        totalS.insert(std::pair<int,int>(minor,10));
      }
    }
  }
}

void startupBLE(){
    ble.init();

    /**
     * The Beacon payload has the following composition:
     * 128-Bit / 16byte UUID = E2 0A 39 F4 73 F5 4B C4 A1 2F 17 D1 AD 07 A9 61
     * Major/Minor  = 0x1122 / 0x3344
     * Tx Power     = 0xC8 = 200, 2's compliment is 256-200 = (-56dB)
     *
     * Note: please remember to calibrate your beacons TX Power for more accurate results.
     */

    uint16_t minorNumber = 1244 + clientColour;
    uint16_t txPower     = 0xC9;
    iBeacon ibeacon(ble, uuid, majorNumber, minorNumber, txPower);


    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
    ble.gap().setScanParams(1800 /* scan interval */, 1500 /* scan window */);
    ble.gap().startScan(advertisementCallback);
}


int debounceLeft = 0;
void flip(){
    if (debounceLeft == 0){
        debounceLeft = 10;
    }
}

//void flip(){
//    buttonLightsLeft = 10;
//    enabled = !enabled;
//
//    if (!enabled){
//        ble.gap().stopScan();
//        ble.gap().stopAdvertising();
//
//        //Reset LEDs
//        for (int i = 0; i < 6; ++i) {
//            setLEDColours(i,0,0,0);
//        }
//    }else{
//        startupBLE();
//    }
//}

int buttonLightsLeft = 0;

void updateLEDs(){
    //Some button stuff
    if (debounceLeft > 0){
        debounceLeft--;
        if(debounceLeft == 0){
            buttonLightsLeft = 10;
            enabled = !enabled;

            if (!enabled){
                ble.gap().stopScan();
                ble.gap().stopAdvertising();

                //Reset LEDs
                for (int i = 0; i < 6; ++i) {
                    setLEDColours(i,0,0,0);
                }
            }else{
                startupBLE();
            }
        }
        return;
    }

    if (!enabled){return;}

    if (buttonLightsLeft > 0){
        buttonLightsLeft--;
        for (int i = 0; i < 6; ++i) {
            setLEDColours(i,127,127,127);
        }
        return;
    }

    switch (clientColour){
        case 0:
            redA = 127;
            setLEDColours(0,redA,0,0);
            setLEDColours(1,redA,0,0);
            setLEDColours(2,0,0,greenA);
            setLEDColours(3,0,0,greenA*greenA*greenA/16219);
            setLEDColours(4,0,blueA,0);
            setLEDColours(5,0,blueA*blueA*blueA/16219,0);

            greenA = max(0, greenA-1);
            blueA = max(0, blueA-1);
            break;
        case 1:
            greenA = 127;
            setLEDColours(2,redA,0,0);
            setLEDColours(3,redA*redA*redA/16219,0,0);
            setLEDColours(0,0,0,greenA);
            setLEDColours(1,0,0,greenA);
            setLEDColours(4,0,blueA,0);
            setLEDColours(5,0,blueA*blueA*blueA/16219,0);

            redA = max(0, redA-1);
            blueA = max(0, blueA-1);
            break;
        case 2:
            blueA = 127;
            setLEDColours(4,redA,0,0);
            setLEDColours(5,redA*redA*redA/16219,0,0);
            setLEDColours(2,0,0,greenA);
            setLEDColours(3,0,0,greenA*greenA*greenA/16219);
            setLEDColours(0,0,blueA,0);
            setLEDColours(1,0,blueA,0);

            redA = max(0, redA-1);
            greenA = max(0, greenA-1);
    }
}

//void haptic() {
//    if (stationBeacon) {
//        stationBeacon = false;
//        vib = 1;
//
//        return;
//    }
//    vib = 0;
//}

void app_start(int argc, char *argv[])
{
    button.rise(&flip);
    button.mode(PullDown);

    minar::Scheduler::postCallback(blinkCallback).period(minar::milliseconds(500));
    //minar::Scheduler::postCallback(haptic).period(minar::milliseconds(100));
    minar::Scheduler::postCallback(updateLEDs).period(minar::milliseconds(50));


    //Reset LEDs

    for (int i = 0; i < 6; ++i) {
        setLEDColours(i,0,0,0);
    }

    startupBLE();
}
