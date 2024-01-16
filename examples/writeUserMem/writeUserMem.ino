/*

	Arduino MAX17332 library

	Copyright (c) 2023 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#include <Arduino_MAX17332.h>
#include <Wire.h>

TwoWire WIRE;
MAX17332 BMS(WIRE);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    if (!BMS.begin()) {
        Serial.println("Failed to initialize BMS");
        while(1);
    }
}

void loop() {
    Serial.println("Initial user mem 1C6 value:");
    Serial.println(BMS.readUserMem1C6(), HEX);
    Serial.println("Reading comm stat reg");
    Serial.println(BMS.readCommStat(), HEX);
    Serial.println("Writing 0xafbf to 1C6:");
    BMS.writeUserMem1C6(0xafbf);
    delay(1000);
    Serial.println("Reading user mem 1C6:");
    Serial.println(BMS.readUserMem1C6(), HEX);
    Serial.println("Reading comm stat again:");
    Serial.println(BMS.readCommStat(), HEX);
    delay(1000);
}