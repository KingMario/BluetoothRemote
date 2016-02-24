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
	BluetoothServerSocket(int port);

	BluetoothServerSocket();

	virtual ~BluetoothServerSocket();

	int port() const {
	    return _port;
	}

	void listen(int backlog = 1) {
	    _listen(backlog);
	}

	BluetoothSocket * accept() {
	    return _accept();
	}
    };

}

#endif // BLUETOOTHSERVERSOCKET_H
