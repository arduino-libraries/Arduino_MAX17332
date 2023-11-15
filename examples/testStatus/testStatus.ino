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
    BMS.isPermFail() ? Serial.println("BATTERY IS FAILED PERMANENTLY") : Serial.println("BATTERY OK");
    BMS.isCharging() ? Serial.println("BATTERY IS CHARGING") : Serial.println("BATTERY IS DISCHARGING");
    BMS.update();

    Serial.println("BATTERY STATUS");
    Serial.print("STATUS\t\t");
    Serial.println(BMS.status.status_reg, BIN);
    Serial.print("FPROT_STAT\t");
    Serial.println(BMS.status.f_prot_stat, BIN);
    Serial.print("NBATT_STATUS\t");
    Serial.println(BMS.status.n_batt_status, BIN);
    Serial.print("PROT_STATUS\t");
    Serial.println(BMS.status.prot_status, BIN);
    Serial.print("PROT_ALRT\t");
    Serial.println(BMS.status.prot_alrt, BIN);

    delay(500);
}