/*

	Transform library
	Copyright (C) 2023 Lucio Rossi

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

MAX17332::MAX17332(TwoWire& wire): _wire(&wire) {}

MAX17332::~MAX17332()
{
}

void MAX17332::begin() {
    _wire->begin();
}

void MAX17332::end() {
    _wire->end();
}

uint8_t MAX17332::get_i2c_address(uint16_t reg_address)
{

    if (reg_address < 0x100) {
        return MAX17332_ADDRESS_L;
    } else {
        return MAX17332_ADDRESS_H;
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