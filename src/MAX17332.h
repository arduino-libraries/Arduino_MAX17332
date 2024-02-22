/*

	Arduino MAX17332 library

	Copyright (c) 2023 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef  _MAX17332_H_
#define  _MAX17332_H_

#include <Arduino.h>
#include <Wire.h>

// I2C ADDRESSES
#define MAX17332_ADDRESS_L          0x36
#define MAX17332_ADDRESS_H          0x0B

// REGISTERS USING MAX17332_ADDRESS_L
#define MAX17332_STATUS_REG         0x000
#define MAX17332_VCELL_REG          0x01A
#define MAX17332_VCELLREP_REG       0x012
#define MAX17332_CURR_REG           0x01C
#define MAX17332_CURRREP_REG        0x022
#define MAX17332_DEVNAME_REG        0x021
#define MAX17332_TEMP_REG           0x01B
#define MAX17332_AVSOC_REG          0x00E
#define MAX17332_REPSOC_REG         0x006
#define MAX17332_COMMAND_REG        0x060
#define MAX17332_COMMSTAT_REG       0x061
#define MAX17332_LOCK_REG           0x07F
#define MAX17332_CONFIG2_REG        0x0AB
#define MAX17332_FPROTSTAT_REG      0x0DA
#define MAX17332_PROT_STATUS_REG    0x0D9
#define MAX17332_PROT_ALRT_REG      0x0AF
#define MAX17332_CHGSTAT_REG        0x0A3

// REGISTERS USING MAX17332_ADDRESS_H
#define MAX17332_N_BATT_STATUS_REG  0x1A8
#define MAX17332_RSENSE_REG         0x19C
#define MAX17332_USERMEM_1C6        0x1C6
#define MAX17332_USERMEM_1E0        0x1E0

// CONSTANTS
#define MAX17332_DEVICE_NAME        0x4130
#define NVM_SIZE                    224             ///< bytes
#define NVM_START_ADDRESS           0x180
#define TBLOCK                      7500            ///< Block programming time (max is 7360 according to datasheet)
#define VOLTAGE_LSB                 78.125e-6
#define CURRENT_LSB                 1.5625e-6
#define RSENSE_LSB                  1e-3
#define RSENSE_DEFAULT              10e-3
#define TEMP_LSB                    0.00390625      ///<  1/256°C
#define PERC_LSB                    0.00390625      ///<  1/256%

// COMMANDS
#define COPY_NV_BLOCK_CMD           0xE904          ///< Copy shadow RAM to NVM
#define NV_RECALL_CMD               0xE001          ///< Recall NVM to RAM
#define HARDWARE_RESET_CMD          0x000F          ///< Recalls nonvolatile memory into RAM and resets the IC hardware

// MASKS
#define FPROTSTAT_ISDIS_MASK        0b0000000000100000  ///< Charging/Discharging state mask (IsDis bit on FPROTSTAT)
#define NBATTSTATUS_PERMFAIL_MASK   0b1000000000000000  ///< Permanent battery failure mask
#define COMMSTAT_NVBUSY_MASK        0b0000000000000010  ///< CommStat.NVBusy flag
#define COMMSTAT_NVERROR_MASK       0b0000000000000100  ///< CommStat.NVError flag
#define STATUS_OVERVOLTAGE_MASK     0b0001000000000000  ///< Status.Vmx
#define STATUS_UNDERVOLTAGE_MASK    0b0000000100000000  ///< Status.Vmn
#define STATUS_OVERCURRENT_MASK     0b0000000001000000  ///< Status.Imx
#define STATUS_UNDERCURRENT_MASK    0b0000000000000100  ///< Status.Imn
#define STATUS_OVERTEMP_MASK        0b0010000000000000  ///< Status.Tmx
#define STATUS_UNDERTEMP_MASK       0b0000001000000000  ///< Status.Tmn
#define STATUS_OVERSOC_MASK         0b0100000000000000  ///< Status.Smx
#define STATUS_UNDERSOC_MASK        0b0000010000000000  ///< Status.Smn
#define STATUS_SOCCHANGE_MASK       0b0000000010000000  ///< Status.dSOCi
#define STATUS_PROTECTIONALERT_MASK 0b1000000000000000  ///< Status.PA
#define STATUS_CHARGINGALERT_MASK   0b0000100000000000  ///< Status.CA
#define STATUS_ALERT_MASK           0b1111111101000100  ///< Status all alerts (OV OC OT OS CA PA UV UC UT US)

/**
 * Struct for storing MAX17332 complex status
*/
typedef struct
{
    int status_reg;
    int f_prot_stat;
    int n_batt_status;
    int prot_status;
    int prot_alrt;
    int chg_stat;

} MAX17332_Status;


class MAX17332 {
    public:
        MAX17332(TwoWire& wire, uint16_t address_l=MAX17332_ADDRESS_L, uint16_t address_h=MAX17332_ADDRESS_H);
        ~MAX17332();

        /**
            @brief  MAX17332 startup operations
            @return 1 if OK, 0 on error
        */
        int begin();

        /**
            @brief  MAX17332 cleanup operations
        */
        void end();

        /**
            @brief  MAX17332 status update
        */
        void update();

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
            @brief  Uses readnBattStatus. Returns true if battry is in permanent fail status.
        */
        bool isPermFail();

        /**
            @brief  Uses readFProtStat. Returns true if battry is charging false if discharging.
        */
        bool isCharging();

        /**
            @brief  Uses readStatus. Returns true if battry status reg 0x000 has any alert bit set.
        */
        bool hasAlerts();

        /**
            @brief  Uses readStatus. Returns true if battry is in OV Alert.
        */
        bool isOverVoltage();

        /**
            @brief  Uses readStatus. Returns true if battry is in UV Alert.
        */
        bool isUnderVoltage();

        /**
            @brief  Uses readStatus. Returns true if battry is in OC Alert.
        */
        bool isOverCurrent();

        /**
            @brief  Uses readStatus. Returns true if battry is in UC Alert.
        */
        bool isUnderCurrent();

        /**
            @brief  Uses readStatus. Returns true if battry is in OT Alert.
        */
        bool isOverTemperature();

        /**
            @brief  Uses readStatus. Returns true if battry is in UT Alert.
        */
        bool isUnderTemperature();

        /**
            @brief  Uses readStatus. Returns true if battry is in OS Alert.
        */
        bool isOverSOC();

        /**
            @brief  Uses readStatus. Returns true if battry is in US Alert.
        */
        bool isUnderSOC();

        /**
            @brief  Uses readStatus. Returns true if battry is in PA Alert.
        */
        bool isProtectionAlert();

        /**
            @brief  Uses readStatus. Returns true if battry is in CA Alert.
        */
        bool isChargingAlert();

        /**
            @brief  Reads the status of permanent locks in the LOCK_REG
        */
        uint16_t readLocks();
        
        /**
            @brief  Sends a command to COMMAND_REG
        */
        int sendCommand(uint16_t cmd);

        /**
            @brief  Reads the COMMSTAT_REG
        */
        uint16_t readCommStat();
        
        /**
            @brief  Reads the STATUS_REG
        */
        uint16_t readStatus();

        /**
            @brief  Clears the STATUS_REG
        */
        void clearStatus();

        /**
            @brief  Reads the FPROTSTAT_REG
        */
        uint16_t readFProtStat();

        /**
            @brief  Reads the N_BATT_STATUS_REG
        */
        uint16_t readnBattStatus();

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

        /**
            @brief  Compares input array with Shadow RAM content
            @param  data uint8_t output data array. Must be of size NVM_SIZE
            @return 1 if content is equal to input; 0 if they differ; -1 on error
        */
        int compareWithMem(const uint8_t* data);

        /**
            @brief  Writes data to the shadow RAM (0x180 - 0x1EF). Data is NOT flashed on the NVM
            @param  data const uint8_t input data array. Must be of size NVM_SIZE
            @return 1 if OK; -1 on transmission error; 0 if bytes received are less than length
        */
        int writeShadowMem(const uint8_t* data);

        /**
            This declares MAX17332_Programmer as a friend class
        */
        friend class MAX17332_Programmer;

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
            @brief  Flashes data to the NVM (0x180 - 0x1EF). NVM is limited to seven writes maximum. Use at own risk. Verification is not implemented
            @param  data const uint8_t input data array. Must be of size NVM_SIZE
            @return 1 if OK; 0 on transmission error; -1 on NVError; -2 on verification error
        */
        int writeNVM(const uint8_t* data);

        /**
            @brief  Initiates POR sequence and waits for completion (CONFIG2_REG POR_CMD bit)
            @return 1 if OK; 0 on transmission error
        */
        int resetFirmware();

        /**
            @brief  Write 0x000F to the Command register 0x060 to POR the IC
            @return 1 if OK; 0 on transmission error
        */
        int resetHardware();

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
            @brief  Writes register @address. For some registers mem should be freed in advance
            @param  address 9-bit address
            @param  value the data word
            @return rgister content or -1 on transmission error
        */
        int writeRegister(uint16_t address, uint16_t value);

        /**
            @brief  Writes length bytes starting from address. Word bytes must be arranged LSB first. For some registers mem should be freed in advance
            @param  address 9-bit address
            @param  data uint8_t input data array [LSB, MSB, ...]
            @param  length size of data (bytes) to write
            @return 1 if OK; 0 on transmission error
        */
        int writeRegisters(uint16_t address, const uint8_t* data, const uint32_t length);

    public:
        MAX17332_Status status;

    private:
        uint16_t _address_l;    ///< i2c address for low mem block
        uint16_t _address_h;    ///< i2c address for high mem block (shadow RAM)
        TwoWire* _wire;         ///< Pointer to i2c interface

};

#endif