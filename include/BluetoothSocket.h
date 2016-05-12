#ifndef BLUETOOTHSOCKET_H
#define BLUETOOTHSOCKET_H

#include "BluetoothAddress.h"
namespace bluetooth {

class BluetoothSocket {
  
  friend class BluetoothServerSocket;

  BluetoothAddress addr;
  int fd;

  BluetoothSocket(BluetoothAddress addr, int fd);

public:
  BluetoothSocket(BluetoothAddress addr);

  unsigned char read();

  int read(size_t numBytes, void *dstBuf);

  int write(size_t numBytes, const void *srcBuf);

  virtual ~BluetoothSocket();

protected:
  BluetoothSocket& operator=(const BluetoothSocket &o) {
    
  }
private:
};
}
#endif // BLUETOOTHSOCKET_H
