#include <iostream>
#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "Consumer.h"
#include "Packet.h"

#include <X11/Xlib.h>

using namespace std;s
using namespace bluetooth;

void registerService(const BluetoothServerSocket &bsock) {
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
  BluetoothServerSocket bsrv;
  registerService(bsrv);
  
  
  
}