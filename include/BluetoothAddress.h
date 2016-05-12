#ifndef BLUETOOTHADDRESS_H
#define BLUETOOTHADDRESS_H

#include <string>
#include <cstring>
#include <vector>

namespace bluetooth {

class BluetoothAddress {
  unsigned char addr[6];
  unsigned char devClass[3];
  std::string devName;
public:

  BluetoothAddress(const unsigned char addr[6],
                   const unsigned char devClass[6],
                   const std::string &devName) : devName(devName) {
    memcpy(&this->addr, addr, sizeof (this->addr));
    memcpy(&this->devClass, devClass, sizeof (this->devClass));
  }

  BluetoothAddress(const BluetoothAddress &o) : devName(o.devName) {
    memcpy(&addr, &o.addr, sizeof (addr));
    memcpy(&devClass, &o.devClass, sizeof (devClass));
  }

  BluetoothAddress& operator=(const BluetoothAddress &o) {
    memcpy(&addr, &o.addr, sizeof (addr));
    memcpy(&devClass, &o.devClass, sizeof (devClass));
    devName = o.devName;
    return *this;
  }

  const unsigned char * address() const {
    return addr;
  }

  const unsigned char * deviceClass() const {
    return devClass;
  }

  const std::string & deviceName() const {
    return devName;
  }
};


}
#endif // BLUETOOTHADDRESS_H
