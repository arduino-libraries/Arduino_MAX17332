#ifndef __ARDUINO_MAX17332_H__
#define __ARDUINO_MAX17332_H__

#include "Arduino.h"

#define MAX17332_DEFAULT_SENSE_RESISTOR 10.0

#define MAX17332_ADDRESS 0x36
#define MAX17332_SEC_ADDRESS 0x0b



class Arduino_MAX17332{
    private:
        float sense_resistor;
    public:
        Arduino_MAX17332();
        int begin();

};


#endif