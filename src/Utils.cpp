#include "Utils.h"

#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void bluetooth::queryDevices(std::vector<BluetoothAddress> &devices, int max_rsp)
{
	inquiry_info *ii = NULL;
	int num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = {0};
	char name[1024] = {0};
	dev_id = hci_get_route(NULL);
	if (dev_id == -1) {
		throw std::runtime_error("cannot hci_get_route");
	}
	sock = hci_open_dev(dev_id);
	if (sock == -1) {
		throw std::runtime_error("cannot hci_open_dev");
	}
	len = 8;
	flags = IREQ_CACHE_FLUSH;
	ii = new inquiry_info[max_rsp];
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp == -1) {
		delete []ii;
		close(sock);
		throw std::runtime_error("cannot call hci_inquiry");
	}
	for (int i = 0; i < num_rsp; i++) {
		const unsigned char * addr = (const unsigned char *) &(ii + i)->bdaddr.b;
		const unsigned char * devClass = (const unsigned char *) &(ii + i)->dev_class;
		std::string strName = "";
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) == 0) {
			strName = name;
		} else strName = "[unknown]";
		devices.push_back(BluetoothAddress(addr, devClass, strName));
	}
}
