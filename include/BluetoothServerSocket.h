#ifndef BLUETOOTHSERVERSOCKET_H
#define BLUETOOTHSERVERSOCKET_H

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "BluetoothSocket.h"
#include "BluetoothAddress.h"

#include <stdexcept>

namespace bluetooth {

class BluetoothServerSocket {
  int fd;
  int _port;

  void construct();

  BluetoothSocket * _accept();

  BluetoothServerSocket(const BluetoothServerSocket &o);

  BluetoothServerSocket& operator=(const BluetoothServerSocket &o);

  void _listen(int backlog);

public:
  /**
   * creates socket and binds it on the specified port
   * @param port
   */
  BluetoothServerSocket(int port);
  /**
   * creates socket and binds it on the first available port
   */
  BluetoothServerSocket();

  virtual ~BluetoothServerSocket();

  int port() const {
    return _port;
  }
  /**
   * Mark the socket as listening
   * @param backlog how many syns ( tcp term ) can wait before accepted
   */
  void listen(int backlog = 1) {
    _listen(backlog);
  }
  /**
   * The call blocks the current thread until a connection request arrives
   * @return the client socket
   */
  BluetoothSocket * accept() {
    return _accept();
  }
};

}

#endif // BLUETOOTHSERVERSOCKET_H
