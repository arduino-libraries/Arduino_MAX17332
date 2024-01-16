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
    uint8_t data[NVM_SIZE];

    BMS.shadowMemDump(data);

    Serial.println("[");

    for (int page = 0; page < 7; page++) {
        for (int j = 0; j < 32; j++) {
            Serial.print("0x");
            if (data[page*32 + j] < 16) {
                Serial.print("0");
            }
            Serial.print(data[page*32 + j], HEX);
            Serial.print(", ");
        }
        Serial.println("");
    }

    Serial.println("]");
    delay(10000);

}