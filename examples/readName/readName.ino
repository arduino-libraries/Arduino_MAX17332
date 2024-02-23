/*

	Arduino MAX17332 library

	Copyright (c) 2023 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#include <Arduino_MAX17332.h>
#include <Wire.h>

MAX17332 BMS(Wire);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    if (!BMS.begin()) {
        Serial.println("Failed to initialize BMS");
        //while(1);
    }
}

void loop() {
    Serial.println("DEVICE NAME:");
    Serial.println(BMS.readDevName(), HEX);
    delay(500);
}