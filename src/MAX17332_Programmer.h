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