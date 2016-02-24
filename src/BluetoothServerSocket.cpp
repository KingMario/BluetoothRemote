#include "BluetoothServerSocket.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
using namespace std;

void bluetooth::BluetoothServerSocket::construct()
{
	fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (fd == -1) {
		throw std::runtime_error("cannot create bluetooth rfcomm socket.");
	}
	sockaddr_rc loc_addr;
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = {
		{0, 0, 0, 0, 0, 0}
	};
	int res;
	if (_port == -1) {
		for (int i = 1; i <= 30; i++) {
			_port = i;
			loc_addr.rc_channel = _port;
			if ((res = bind(fd, (struct sockaddr *) &loc_addr, sizeof(loc_addr))) < 0) {
				continue;
			} else break;
		}
	} else {
		loc_addr.rc_channel = _port;
		res = bind(fd, (struct sockaddr *) &loc_addr, sizeof(loc_addr));
	}
	if (res == -1) {
		close(fd);
		throw std::runtime_error("cannot bind to the local address.");
	}
}

bluetooth::BluetoothSocket * bluetooth::BluetoothServerSocket::_accept()
{
	sockaddr_rc remote;
	socklen_t sz = sizeof(remote);
	int client = ::accept(fd, (struct sockaddr *) &remote, &sz);
	if (client == -1) {
		throw std::runtime_error("cannot perform accept.");
	}
	// for now this will suffice
	const unsigned char dummyClass[3] = {0, 0, 0};

	BluetoothSocket * clientSocket = new BluetoothSocket(
			BluetoothAddress((const unsigned char*) remote.rc_bdaddr.b,
			dummyClass, "[unknown]"), client);

	return clientSocket;
}

bluetooth::BluetoothServerSocket::
BluetoothServerSocket(const BluetoothServerSocket &o) :
fd(o.fd), _port(o._port)
{

}

bluetooth::BluetoothServerSocket& bluetooth::
		BluetoothServerSocket::operator=(const BluetoothServerSocket &o)
{
	fd = o.fd;
	_port = o._port;
	return *this;
}

void bluetooth::
BluetoothServerSocket::_listen(int backlog)
{
	if (::listen(fd, backlog) == -1) {
		throw std::runtime_error("cannot perform listen");
	}
}

bluetooth::BluetoothServerSocket::BluetoothServerSocket(int port) :
fd(-1), _port(port)
{
	construct();
}

bluetooth::BluetoothServerSocket::BluetoothServerSocket() :
fd(-1), _port(-1)
{
	construct();
}

bluetooth::BluetoothServerSocket::~BluetoothServerSocket()
{
	close(fd);
}
