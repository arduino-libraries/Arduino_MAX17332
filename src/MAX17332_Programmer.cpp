/*

	Arduino MAX17332 library

	Copyright (c) 2023 Arduino SA

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#include "MAX17332_Programmer.h"

MAX17332_Programmer::MAX17332_Programmer(MAX17332& bms): _bms(&bms){}
MAX17332_Programmer::~MAX17332_Programmer(){}

int MAX17332_Programmer::writeNVM(const uint8_t* data) {
    return _bms->writeNVM(data);
}
