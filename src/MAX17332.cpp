/*

	Transform library
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

void MAX17332::begin() {
    _wire->begin();
}

void MAX17332::end() {
    _wire->end();
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
    uint8_t value;

    if (readRegisters(address, &value, sizeof(value)) != 1) {
        return -1;
    }

    return value;
}

int MAX17332::writeRegister(uint16_t address, uint16_t value)
{
    freeMem();
    uint8_t i2c_address = get_i2c_address(address);
    _wire->beginTransmission(i2c_address);
    _wire->write(address & 0xFF);
    _wire->write(value & 0xFF);         // write LSB
    _wire->write((value >> 8) & 0xFF);  // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // Check for register to be correctly written?

    resetFirmware();
    protectMem();

    return 1;
}

int MAX17332::writeRegisters(uint16_t address, const uint8_t* data, const uint32_t length)
{
    freeMem();
    uint8_t i2c_address = get_i2c_address(address);
    _wire->beginTransmission(i2c_address);
    _wire->write(address & 0xFF);

    for (uint32_t i=0; i<length; i++) {
        _wire->write(data[i]);
    }

    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // Check for registers to be correctly written?

    resetFirmware();
    protectMem();

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

int MAX17332::resetFirmware() {
    _wire->beginTransmission(_address_l);
    _wire->write(MAX17332_CONFIG2_REG & 0xFF);
    _wire->write(0x00);         // write LSB
    _wire->write(0x80);         // write MSB
    if (_wire->endTransmission() != 0) {
      return 0;
    }

    // Wait for POR_CMD bit to be cleared
    while ((readRegister(MAX17332_CONFIG2_REG) & 0x8000) != 0) {}

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

int MAX17332::readLocks() {

    uint16_t val;

    if (!readRegisters(MAX17332_LOCK_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;

}

uint16_t MAX17332::readCommStat() {
    uint16_t val;

    if (!readRegisters(MAX17332_COMMSTAT_REG, (uint8_t*) &val, sizeof(val))) {
        return 0xffff;
    }

    return val;
}

int MAX17332::writeUserMem1C6(uint16_t value) {
    if (value == readUserMem1C6()) {
        return 1;
    }
    return writeRegister(MAX17332_USERMEM_1C6, value);

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
    return writeRegisters(NVM_START_ADDRESS, data, NVM_SIZE);
}