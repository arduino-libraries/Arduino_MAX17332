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
#define MAX17332_TEMP_REG           0x01B
#define MAX17332_AVSOC_REG          0x00E
#define MAX17332_REPSOC_REG         0x006
#define MAX17332_COMMSTAT_REG       0x061
#define MAX17332_LOCK_REG           0x07F
#define MAX17332_CONFIG2_REG        0x0AB


// REGISTERS USING MAX17332_ADDRESS_H
#define MAX17332_RSENSE_REG         0x19C
#define MAX17332_USERMEM_1C6        0x1C6
#define MAX17332_USERMEM_1E0        0x1E0


// CONSTANTS
#define VOLTAGE_LSB     78.125e-6
#define CURRENT_LSB     1.5625e-6
#define RSENSE_LSB      1e-3
#define RSENSE_DEFAULT  10e-3
#define TEMP_LSB        0.00390625      //  1/256°C
#define PERC_LSB        0.00390625      //  1/256%


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
        float readTemp();
        float readSoc();
        int readLocks();
        uint16_t readCommStat();
        int writeUserMem1C6(uint16_t value);
        uint16_t readUserMem1C6();

    private:
        uint8_t get_i2c_address(uint16_t reg_address);
        int freeMem();
        int protectMem();
        int resetFirmware();
        int readRegister(uint16_t address);
        int readRegisters(uint16_t address, uint8_t* data, size_t length);
        int writeRegister(uint16_t address, uint16_t value);
    private:
        TwoWire* _wire;

};
