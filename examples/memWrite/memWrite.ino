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

const uint8_t config[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x61, 0xA5, 0x00, 0x00, 
0x80, 0x10, 0x43, 0x20, 0x8C, 0x07, 0x80, 0x08, 0x00, 0x00, 0x48, 0x0D, 0xCC, 0x08, 0x3E, 0x22, 0x00, 0x00, 0x48, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x83, 0x2A, 0x00, 0x00, 0x85, 0x07, 0x00, 0x00, 0x41, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x09, 0x26, 0xB2, 0x84, 0x38, 0xA1, 0x01, 0x00, 0x11, 
0x09, 0x42, 0x80, 0x02, 0x1F, 0x14, 0x72, 0xE2, 0xA7, 0x00, 0x00, 0xAA, 0x2B, 0x1A, 0x00, 0x20, 0x00, 0x40, 0x88, 0x51, 0xBE, 0x71, 0x3F, 0x3E, 0x05, 0x50, 0x66, 0x66, 0xE8, 0x2F, 0x55, 0x55, 
0x4C, 0x3D, 0x38, 0x20, 0x00, 0x70, 0x07, 0x51, 0x28, 0xA0, 0x05, 0x30, 0x28, 0x7A, 0x00, 0x30, 0x00, 0x00, 0x82, 0x01, 0x81, 0xFA, 0x0A, 0xFE, 0x3D, 0x9B, 0xA2, 0x0E, 0x55, 0x43, 0x48, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void setup() {
    Serial.begin(9600);
    while (!Serial);
    if (!BMS.begin()) {
        Serial.println("Failed to initialize BMS");
        while(1);
    }
    BMS.writeShadowMem(config);
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