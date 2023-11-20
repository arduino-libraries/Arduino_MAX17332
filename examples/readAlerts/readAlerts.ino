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
    BMS.hasAlerts() ? Serial.println("BMS HAS ALERTS") : Serial.println("BMS HAS NO ALERTS");
    BMS.isProtectionAlert() ? Serial.println("BMS PROTECTION ALERT") : Serial.println("PA OK");
    BMS.isChargingAlert() ? Serial.println("BMS CHARGING ALERT") : Serial.println("CA OK");
    BMS.isOverVoltage() ? Serial.println("BMS OV ALERT") : Serial.println("OV OK");
    BMS.isUnderVoltage() ? Serial.println("BMS UV ALERT") : Serial.println("UV OK");
    BMS.isOverCurrent() ? Serial.println("BMS OC ALERT") : Serial.println("OC OK");
    BMS.isUnderCurrent() ? Serial.println("BMS UC ALERT") : Serial.println("UC OK");
    BMS.isOverTemperature() ? Serial.println("BMS OT ALERT") : Serial.println("OT OK");
    BMS.isUnderTemperature() ? Serial.println("BMS UT ALERT") : Serial.println("UT OK");
    BMS.isOverSOC() ? Serial.println("BMS OS ALERT") : Serial.println("OS OK");
    BMS.isUnderSOC() ? Serial.println("BMS US ALERT") : Serial.println("US OK");

    BMS.update();
    
    if (BMS.hasAlerts()) {
        Serial.println("STATUS");
        Serial.println(BMS.status.status_reg, BIN);
        Serial.println("PROTECTION ALERT");
        Serial.println(BMS.status.prot_alrt, BIN);
        Serial.println("CHARGING STATUS");
        Serial.println(BMS.status.chg_stat, BIN);
    }

    delay(500);
}