/*

	Arduino MAX17332 library
	Copyright (C) 2023 Lucio Rossi, Giovanni Bruno

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "MAX17332.h"

MAX17332::MAX17332(TwoWire& wire, uint16_t address_l, uint16_t address_h): _wire(&wire), _address_l(address_l), _address_h(address_h) {}
MAX17332::~MAX17332(){}

int MAX17332::begin() {
    _wire->begin();

    if (readDevName() != MAX17332_DEVICE_NAME){
        end();
        return 0;
    }

    return 1;
}

void MAX17332::end() {
    _wire->end();
}

void MAX17332::update() {
    status.status_reg =  readRegister(MAX17332_STATUS_REG);
    status.f_prot_stat = readRegister(MAX17332_FPROTSTAT_REG);
    status.n_batt_status = readRegister(MAX17332_N_BATT_STATUS_REG);
    status.prot_status = readRegister(MAX17332_PROT_STATUS_REG);
    status.prot_alrt = readRegister(MAX17332_PROT_ALRT_REG);
    status.chg_stat = readRegister(MAX17332_CHGSTAT_REG);
}

uint8_t MAX17332::get_i2c_address(uint16_t reg_address)
{

    if (reg_address < 0x100) {
        return _address_l;
    } else {
        return _address_h;
    }

}

int MAX17332::readRegisters(uint16_t address, uint8_t* data, size_t length)
{

    uint8_t i2c_address = get_i2c_address(address);
    _wire->beginTransmission(i2c_address);
    _wire->write(address & 0xFF);

    if (_wire->endTransmission(false) != 0) {
        return -1;
    }

    if (_wire->requestFrom(i2c_address, length) != length) {
        return 0;
    }

    for (size_t i = 0; i < length; i++) {
        *data++ = _wire->read();
    }

    return 1;
}

int MAX17332::readRegister(uint16_t address)
{
    uint16_t value;

    if (readRegisters(address, (uint8_t*) &value, sizeof(value)) != 1) {
        return -1;
    }

    return value;
}

int MAX17332::writeRegister(uint16_t address, uint16_t value)
{
    uint8_t i2c_address = get_i2c_address(address);
    _wire->beginTransmission(i2c_address);
    _wire->write(address & 0xFF);
    _wire->write(value & 0xFF);         // write LSB
    _wire->write((value >> 8) & 0xFF);  // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    return 1;
}

int MAX17332::writeRegisters(uint16_t address, const uint8_t* data, const uint32_t length)
{
    uint8_t i2c_address = get_i2c_address(address);
    _wire->beginTransmission(i2c_address);
    _wire->write(address & 0xFF);

    for (uint32_t i=0; i<length; i++) {
        _wire->write(data[i]);
    }

    if (_wire->endTransmission() != 0) {
      return 0;
    }

    return 1;
}

int MAX17332::freeMem() {

    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // MUST BE DONE TWICE

    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    return 1;

}

int MAX17332::protectMem() {

    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0xF9);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // MUST BE DONE TWICE

    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0xF9);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    return 1;
}

int MAX17332::writeNVM(const uint8_t* data) {

    freeMem();

    if (!writeRegisters(NVM_START_ADDRESS, data, NVM_SIZE)) {
        return 0;
    }

    // Verify memory write

    // Clear CommStat.NVError flag
    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // This initiates BLOCK COPY!!!
    sendCommand(COPY_NV_BLOCK_CMD);

    // Wait for tBLOCK
    delay(TBLOCK);

    // Wait for CommStat.NVBusy to clear
    while ((readCommStat() & COMMSTAT_NVBUSY_MASK) != 0);

    // Check CommStat.NVError flag
    if ((readCommStat() & COMMSTAT_NVERROR_MASK) != 0) {
        protectMem();
        return -1;
    }

    // Hardware reset. Recall NVM content to the shadow RAM
    resetHardware();

    // Verify all of the nonvolatile memory locations are recalled correctly

    // Write 0x0000 to the CommStat register (0x061) 3 times in a row to unlock Write Protection and clear NVError bit
    freeMem();

    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_COMMSTAT_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x00);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    resetFirmware();
    protectMem();

    return 1;
}

int MAX17332::resetFirmware() {
    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_CONFIG2_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x80);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // Wait for POR_CMD bit to be cleared
    while ((readRegister(MAX17332_CONFIG2_REG) & 0x8000) != 0);

    return 1;
}

int MAX17332::resetHardware() {

    if (!sendCommand(HARDWARE_RESET_CMD)) {
        return 0;
    }

    delay(10);

    return 1;
}

uint16_t MAX17332::readDevName()
{   
    uint16_t dev_name;

    if (!readRegisters(MAX17332_DEVNAME_REG, (uint8_t*) &dev_name, sizeof(dev_name))) {
        return 0;
    }

    return dev_name;

}

float MAX17332::readVCell()
{
    uint16_t v_int;

    if (!readRegisters(MAX17332_VCELLREP_REG, (uint8_t*) &v_int, sizeof(v_int))) {
        return 0.0;
    }

    return (float) v_int * VOLTAGE_LSB;

}

float MAX17332::readCurrent()
{
    uint16_t val;

    if (!readRegisters(MAX17332_CURRREP_REG, (uint8_t*) &val, sizeof(val))) {
        return 0.0;
    }

    int16_t curr = static_cast<int16_t>(val);

    return (float) curr * (CURRENT_LSB / RSENSE_DEFAULT);

}

float MAX17332::readRSense()
{
    int value = readRegister(MAX17332_RSENSE_REG);

    if (!value) {
        return 0.0;
    }

    return (float) value * RSENSE_LSB;

}

float MAX17332::readTemp()
{
    uint16_t val;

    if (!readRegisters(MAX17332_TEMP_REG, (uint8_t*) &val, sizeof(val))) {
        return 0.0;
    }

    int16_t temp = static_cast<int16_t>(val);

    return (float) val * TEMP_LSB;

}

float MAX17332::readSoc()
{
    uint16_t val;

    if (!readRegisters(MAX17332_REPSOC_REG, (uint8_t*) &val, sizeof(val))) {
        return 0.0;
    }

    int16_t soc = static_cast<int16_t>(val);

    return (float) soc * PERC_LSB;

}

bool MAX17332::isCharging() {

    return ((readFProtStat() & FPROTSTAT_ISDIS_MASK) == 0);

}

bool MAX17332::isPermFail() {

    return ((readnBattStatus() & NBATTSTATUS_PERMFAIL_MASK) != 0);

}

bool MAX17332::hasAlerts() {

    return ((readStatus() & STATUS_ALERT_MASK) != 0);

}

bool MAX17332::isOverVoltage() {

    return ((readStatus() & STATUS_OVERVOLTAGE_MASK) != 0);

}

bool MAX17332::isUnderVoltage() {

    return ((readStatus() & STATUS_UNDERVOLTAGE_MASK) != 0);

}

bool MAX17332::isOverCurrent() {

    return ((readStatus() & STATUS_OVERCURRENT_MASK) != 0);

}

bool MAX17332::isUnderCurrent() {

    return ((readStatus() & STATUS_UNDERCURRENT_MASK) != 0);

}

bool MAX17332::isOverTemperature() {

    return ((readStatus() & STATUS_OVERTEMP_MASK) != 0);

}

bool MAX17332::isUnderTemperature() {

    return ((readStatus() & STATUS_UNDERTEMP_MASK) != 0);

}

bool MAX17332::isOverSOC() {

    return ((readStatus() & STATUS_OVERSOC_MASK) != 0);

}

bool MAX17332::isUnderSOC() {

    return ((readStatus() & STATUS_UNDERSOC_MASK) != 0);

}

bool MAX17332::isProtectionAlert() {

    return ((readStatus() & STATUS_PROTECTIONALERT_MASK) != 0);

}

bool MAX17332::isChargingAlert() {

    return ((readStatus() & STATUS_CHARGINGALERT_MASK) != 0);

}

uint16_t MAX17332::readLocks() {

    uint16_t val;

    if (!readRegisters(MAX17332_LOCK_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;

}

int MAX17332::sendCommand(uint16_t cmd) {

    return writeRegister(MAX17332_COMMAND_REG, cmd);

}

uint16_t MAX17332::readStatus() {
    uint16_t val;

    if (!readRegisters(MAX17332_STATUS_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;
}

void MAX17332::clearStatus() {

    freeMem();
    writeRegister(MAX17332_PROT_ALRT_REG, 0x0000);
    writeRegister(MAX17332_STATUS_REG, 0b0000000000000000);
    protectMem();

}

uint16_t MAX17332::readCommStat() {
    uint16_t val;

    if (!readRegisters(MAX17332_COMMSTAT_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;
}

uint16_t MAX17332::readFProtStat() {
    uint16_t val;

    if (!readRegisters(MAX17332_FPROTSTAT_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;
}

uint16_t MAX17332::readnBattStatus() {
    uint16_t val;

    if (!readRegisters(MAX17332_N_BATT_STATUS_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;
}

int MAX17332::writeUserMem1C6(uint16_t value) {
    if (value == readUserMem1C6()) {
        return 1;
    }

    freeMem();

    if (!writeRegister(MAX17332_USERMEM_1C6, value)) {
        return 0;
    }

    resetFirmware();
    protectMem();

    return 1;

}

uint16_t MAX17332::readUserMem1C6() {
    uint16_t val;

    if (!readRegisters(MAX17332_USERMEM_1C6, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;

}

int MAX17332::shadowMemDump(uint8_t* data) {
    return readRegisters(NVM_START_ADDRESS, data, NVM_SIZE);
}

int MAX17332::writeShadowMem(const uint8_t* data) {

    freeMem();

    if (!writeRegisters(NVM_START_ADDRESS, data, NVM_SIZE)) {
        return 0;
    }

    resetFirmware();
    protectMem();

    return 1;
}