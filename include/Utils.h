#ifndef UTILS_H
#define UTILS_H

#include "BluetoothAddress.h"

namespace bluetooth {

    void queryDevices(std::vector<BluetoothAddress> &devices, int max_rsp = 255);

}

#endif // UTILS_H
