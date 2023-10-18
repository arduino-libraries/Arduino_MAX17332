#include <Arduino_MAX17332.h>
#include <Wire.h>

TwoWire WIRE();
MAX17332 BMS(WIRE);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    BMS.begin();
}

void loop() {
    Serial.println("BATTERY VOLTAGE:");
    Serial.println(BMS.readVCell());
    Serial.println("RSENSE VALUE:");
    Serial.println(BMS.readRSense());
    Serial.println("CURRENT:");
    Serial.println(BMS.readCurrent());
    delay(500);
}