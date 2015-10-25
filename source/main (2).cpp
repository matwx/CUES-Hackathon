/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "ble/Gap.h"
#include "ButtonService.h"
#include "LPD8806.h"

DigitalOut  led1(LED1, 1);
DigitalOut ledb(p28, 1);
DigitalOut vdd(p29, 1);
InterruptIn button(p7);

const static char     DEVICE_NAME[] = "bu";
static const uint16_t uuid16_list[] = {ButtonService::BUTTON_SERVICE_UUID};

void change_from_input(uint16_t num, uint8_t red, uint8_t green, uint8_t blue);

LPD8806 strip(6);

int ii = 0;
int jj = 0;

ButtonService *buttonServicePtr;

void flip(){
	led1 = !led1;
	ledb = !ledb;
}

void buttonPressedCallback(void)
{
    buttonServicePtr->updateButtonState(true);
}

void buttonReleasedCallback(void)
{
    buttonServicePtr->updateButtonState(false);
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void blinkCallback(void)
{
    led1 = !led1; /* Do blinky on LED1 to indicate system aliveness. */
}

void Colorchange(void)
{
	jj = ii % 6;
	change_from_input(jj, 50, 70, 100);
	ii = ii + 1;


}

void change_from_input(uint16_t num, uint8_t red, uint8_t green, uint8_t blue)
{
	strip.setPixelColor(num, red, green, blue);
	strip.show();
}

//void rainbow(uint8_t wait) {
//	int i, j;
//	for (j = 0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
//		for (i = 0; i < strip.numPixels(); i++) {
//			strip.setPixelColor(i, Wheel((i + j) % 384));
//		}
//		strip.show();   // write all the pixels out
//		delay(wait);
//	}
//}

