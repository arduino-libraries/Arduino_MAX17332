#include <Arduino_MAX17332.h>
#include <Wire.h>

TwoWire WIRE;
MAX17332 BMS(WIRE);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    BMS.begin();
}

void loop() {
    Serial.println("BATTERY VOLTAGE:");
    Serial.println(BMS.readVCell(), 4);
    Serial.println("RSENSE VALUE:");
    Serial.println(BMS.readRSense());
    Serial.println("CURRENT:");
    Serial.println(BMS.readCurrent(), 6);
    Serial.println("TEMPERATURE:");
    Serial.println(BMS.readTemp(), 4);
    Serial.println("STATE OF CHARGE %:");
    Serial.println(BMS.readSoc(), 4);
    delay(500);
}