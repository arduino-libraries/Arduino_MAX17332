/*

	Arduino MAX17332 library

	Copyright (c) 2023 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef  _MAX17332_PROGRAMMER_H_
#define  _MAX17332_PROGRAMMER_H_

#include "MAX17332.h"

class MAX17332_Programmer {

    public:
        MAX17332_Programmer(MAX17332& bms);
        ~MAX17332_Programmer();

        /**
            @brief  Exposes MAX17332::writeNVM protected method. NVM is limited to seven writes maximum. Use at own risk!
            @param  data const uint8_t input data array. Must be of size NVM_SIZE
            @return 1 if OK; -1 if NVError; 0 on transmission error
        */
        int writeNVM(const uint8_t* data);

    private:
        MAX17332* _bms;

};

#endif