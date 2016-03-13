#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "RdClientHandler.h"
#include "Log.h"

#include "suinput.h"
#include "Mouse.h"
#include "Thread.h"
#include "Keyboard.h"

using namespace std;
using namespace bluetooth;

void
registerService(const BluetoothServerSocket &bsock)
{

	unsigned int bluuid[4] = {0x0, 0x0, 0x0, 0x0000abcd};

	ServiceRecord sr = ServiceRecord(bluuid,
			"Remote control server",
			"Remote control server for Linux machines",
			"RCS Daemon",
			bsock.port());

	ServiceRegistrer::registerService(sr);
}

int main()
{
	Log::logMsg("::main()");
	BluetoothServerSocket server;
	Log::logMsg("::main() server created.");
	registerService(server);
	Log::logMsg("::main() registerService(server)");
	server.listen();
	Log::logMsg("::main() server.listen()");

	Log::logMsg("::main() while(true)");

	while (true) {
		BluetoothSocket *socket = server.accept();
		Log::logMsg("::main() server.accept()");
		srv::RdClientHandler<BluetoothSocket*> clientHandler(socket);
		clientHandler.start();
		Log::logMsg("::main() clientHandler.join()");
		clientHandler.join();
		Log::logMsg("::main() delete socket");
		delete socket;
	}
	
}