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

#include <Arduino.h>
#include <Wire.h>

// I2C ADDRESSES
#define MAX17332_ADDRESS_L          0x36
#define MAX17332_ADDRESS_H          0x0B

// REGISTERS USING MAX17332_ADDRESS_L
#define MAX17332_VCELL_REG          0x01A
#define MAX17332_VCELLREP_REG       0x012
#define MAX17332_CURR_REG           0x01C
#define MAX17332_CURRREP_REG        0x022
#define MAX17332_DEVNAME_REG        0x021


// REGISTERS USING MAX17332_ADDRESS_H
#define MAX17332_RSENSE_REG         0x19C


// CONSTANTS
#define VOLTAGE_LSB     78.125e-6
#define CURRENT_LSB     1.5625e-6
#define RSENSE_LSB      1e-3
#define RSENSE_DEFAULT  10e-3

class MAX17332 {
    public:
        MAX17332(TwoWire& wire);
        ~MAX17332();

        void begin();
        void end();

        uint16_t readDevName();
        float readVCell();
        float readRSense();
        float readCurrent();

    private:
        uint8_t get_i2c_address(uint16_t reg_address);
        int readRegister(uint16_t address);
        int readRegisters(uint16_t address, uint8_t* data, size_t length);
    private:
        TwoWire* _wire;

};
