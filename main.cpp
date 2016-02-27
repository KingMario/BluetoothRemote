#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>


#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"


#include "suinput.h"
#include "Mouse.h"

using namespace std;
using namespace bluetooth;
//
//void
//registerService(const BluetoothServerSocket &bsock)
//{
//
//	unsigned int bluuid[4] = {0x0, 0x0, 0x0, 0x0000abcd};
//
//	ServiceRecord sr = ServiceRecord(bluuid,
//			"Remote control server",
//			"Remote control server for Linux machines",
//			"RCS Daemon",
//			bsock.port());
//
//	ServiceRegistrer::registerService(sr);
//}
//
//cairo_status_t write_to_stream(void * closure, const unsigned char *data, unsigned int length)
//{
//
//	BluetoothSocket * socket = (BluetoothSocket*) closure;
//
//	try {
//		socket->write(length, data);
//	} catch (const std::runtime_error &err) {
//		std::cout << err.what() << endl;
//		return CAIRO_STATUS_WRITE_ERROR;
//	}
//
//	return CAIRO_STATUS_SUCCESS;
//}
//
//void writeCurrentSnapshot(vkm::Screen &screen, BluetoothSocket *client)
//{
//
//	Display *disp = screen.display();
//	int scr = DefaultScreen(disp);
//	Window root = DefaultRootWindow(disp);
//	cairo_surface_t *surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
//			DisplayWidth(disp, scr),
//			DisplayHeight(disp, scr));
//
//	int res = cairo_surface_write_to_png_stream(surface, write_to_stream, client);
//
//	if (res == CAIRO_STATUS_WRITE_ERROR) {
//		throw std::runtime_error("cannot write current frame");
//	}
//
//	cairo_surface_destroy(surface);
//}

//MousePacket readMouseEvent(BluetoothSocket *conn)
//{
//	double x;
//	double y;
//	char button;
//	conn->read(sizeof(double), &x);
//	conn->read(sizeof(double), &y);
//	conn->read(1, &button);
//	MousePacket pkt(x, y, (MousePacket::MouseEventType)button);
//
//	cout << "event [ x=" << x << ", y=" << y << ", btn=" << (int) button << "]" << endl;
//	return pkt;
//}
//
//void readInboundEvent(BluetoothSocket *conn, vkm::Mouse &mouse)
//{
//	char type;
//	conn->read(1, &type);
//	cout << "read " << (int) type << endl;
//	if (type == InboundPacket::MOUSE_EVENT) {
//		MousePacket pkt = readMouseEvent(conn);
//		mouse.moveTo(vkm::Point(pkt.x(), pkt.y()));
//		cout << "mouse moved" << endl;
//		usleep(10000);
//		mouse.clickLeft();
//		cout << "mouse clicked" << endl;
//	} else if (type == InboundPacket::REQUEST_SCREEN_REFRESH) {
//		cout << "Frame refresh request arrived" << endl;
//	}
//}
//
//void run()
//{
//
//	BluetoothServerSocket srv;
//	vkm::Mouse mouse;
//	vkm::Screen screen = vkm::Screen::getDefaultScreen();
//	bool interupted = false;
//
//	registerService(srv);
//	srv.listen(1);
//	BluetoothSocket * conn = srv.accept();
//
//	cout << "Connection accepted" << endl;
//
//	while (!interupted) {
//
//		try {
//			writeCurrentSnapshot(screen, conn);
//		} catch (const std::runtime_error &err) {
//			cout << err.what() << endl;
//		}
//
//		cout << "Frame written" << endl;
//
//		try {
//			readInboundEvent(conn, mouse);
//		} catch (const std::runtime_error &err) {
//			cout << err.what() << endl;
//		}
//
//		usleep(100000);
//
//	}
//	delete conn;
//}


int main()
{

}