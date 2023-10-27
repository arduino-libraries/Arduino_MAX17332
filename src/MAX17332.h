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
#define NVM_SIZE                    224             ///< bytes
#define NVM_START_ADDRESS           0x180
#define VOLTAGE_LSB                 78.125e-6
#define CURRENT_LSB                 1.5625e-6
#define RSENSE_LSB                  1e-3
#define RSENSE_DEFAULT              10e-3
#define TEMP_LSB                    0.00390625      ///<  1/256°C
#define PERC_LSB                    0.00390625      ///<  1/256%


class MAX17332 {
    public:
        MAX17332(TwoWire& wire, uint16_t address_l=MAX17332_ADDRESS_L, uint16_t address_h=MAX17332_ADDRESS_H);
        ~MAX17332();

        /**
            @brief  MAX17332 startup operations
        */
        void begin();

        /**
            @brief  MAX17332 cleanup operations
        */
        void end();

        /**
            @brief  Returns the 2-bytes device name (0x4130)
        */
        uint16_t readDevName();

        /**
            @brief  Returns the cell avg voltage from VCELLREP_REG (Volts)
        */
        float readVCell();

        /**
            @brief  Returns the RSense value (mOhms)
        */
        float readRSense();
        
        /**
            @brief  Returns the battery avg current from CURRREP_REG (Amps)
        */
        float readCurrent();
        
        /**
            @brief  Returns the (thermistor or die) Temp (°C)
        */
        float readTemp();
        
        /**
            @brief  Returns the battery State Of Charge from REPSOC_REG (%)
        */
        float readSoc();
        
        /**
            @brief  Returns the battery State Of Charge from REPSOC_REG (%)
        */
        int readLocks();
        
        /**
            @brief  Reads the COMMSTAT_REG
        */
        uint16_t readCommStat();
        
        /**
            @brief  Writes value to the UserMem1C6 REG (0x1C6) (shadow RAM)
        */
        int writeUserMem1C6(uint16_t value);

        /**
            @brief  Reads the UserMem1C6 REG (0x1C6)
        */
        uint16_t readUserMem1C6();

        /**
            @brief  Dumps the contents of the shadow RAM (0x180 - 0x1EF) into the data array
            @param  data uint8_t output data array. Must be of size NVM_SIZE
            @return 1 if OK; -1 on transmission error; 0 if bytes received are less than length
        */
        int shadowMemDump(uint8_t* data);

    private:
        /**
            @brief  Returns the right i2c slave address (H/L) according to location of reg_address
            @param  reg_address 9-bit address
            @return i2c slave address to use
        */
        uint8_t get_i2c_address(uint16_t reg_address);

        /**
            @brief  Removes memory write protection (COMMSTAT)
            @return 1 if OK; 0 on transmission error
        */
        int freeMem();

        /**
            @brief  Restores memory write protection (COMMSTAT)
            @return 1 if OK; 0 on transmission error
        */
        int protectMem();

        /**
            @brief  Initiates POR sequence and waits for completion (CONFIG2_REG POR_CMD bit)
            @return 1 if OK; 0 on transmission error
        */
        int resetFirmware();

        /**
            @brief  Reads register @address
            @param  address 9-bit address
            @return rgister content or -1 on transmission error
        */
        int readRegister(uint16_t address);
        
        /**
            @brief  Reads length bytes starting from address
            @param  address 9-bit address
            @param  data uint8_t output data array
            @param  length size of data (bytes) to read
            @return 1 if OK; -1 on transmission error; 0 if bytes received are less than length
        */
        int readRegisters(uint16_t address, uint8_t* data, size_t length);

        /**
            @brief  Writes register @address
            @param  address 9-bit address
            @param  value the data word
            @return rgister content or -1 on transmission error
        */
        int writeRegister(uint16_t address, uint16_t value);

    private:
        uint16_t _address_l;    ///< i2c address for low mem block
        uint16_t _address_h;    ///< i2c address for high mem block (shadow RAM)
        TwoWire* _wire;         ///< Pointer to i2c interface

};
