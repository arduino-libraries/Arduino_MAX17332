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
    Serial.println("DEVICE NAME:");
    Serial.println(BMS.readDevName(), HEX);
    delay(500);
}