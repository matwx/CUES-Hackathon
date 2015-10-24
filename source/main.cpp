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
//2 - 1 - 6 - 26 - 59 - 11 - 0 - 32 - 255 - 0 - 0 - 0 - 160 - 41 - 0 - 32 - 252 - 41 - 0 - 32 - 176 - 44 - 0 - 32 - 0 -





//2-1-6-26-255-76-0-2-21-0-0-0-0-0-245-75-196-161-47-23-209-173-7-169-97-3-233-13-16-200-
//2-1-6-26-255-76-0-2-21-0-0-0-0-0-245-75-196-161-47-23-209-173-7-169-97-3-233-13-16-200-
//2-1-6-26-255-76-0-2-21-227-170-57-244-115-245-75-196-161-47-23-209-173-7-169-97-4-98-4-220-201
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"

BLE ble;
DigitalOut led1(LED1, 1);
int size = 9+16;
void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{

struct iBeacon {
  char start[9];
  char uuid[16];
  char major[2];
  char minor[2];
  char power[1];
};
  led1 = !led1;
  const uint8_t uuid[] = {0xE3, 0xAA, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4,
                          0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
  if (params->advertisingDataLen == 30) {
    char dest[size];
    (dest, params->advertisingData, sizeof(size));
    iBeacon *b = (iBeacon*)params->advertisingData;
    printf("%i\n\r", memcmp(b->uuid, uuid, sizeof(uuid)));
  }
}

void app_start(int argc, char *argv[])
{
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
  //  ble2.init();
    ble.gap().setScanParams(1800 /* scan interval */, 1500 /* scan window */);
    ble.gap().startScan(advertisementCallback);
    led1 = 0;
}
