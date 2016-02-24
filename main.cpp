#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "Consumer.h"
#include "Packet.h"
#include "Screen.h"
#include "FrameEncoder.h"
#include "suinput.h"
#include "Mouse.h"

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

cairo_status_t write_to_stream(void * closure, const unsigned char *data, unsigned int length)
{

	BluetoothSocket * socket = (BluetoothSocket*) closure;

	try {
		socket->write(length, data);
	} catch (const std::runtime_error &err) {
		std::cout << err.what() << endl;
		return CAIRO_STATUS_WRITE_ERROR;
	}

	return CAIRO_STATUS_SUCCESS;
}

void writeCurrentSnapshot(vkm::Screen &screen, BluetoothSocket *client)
{

	Display *disp = screen.display();
	int scr = DefaultScreen(disp);
	Window root = DefaultRootWindow(disp);
	cairo_surface_t *surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
			DisplayWidth(disp, scr),
			DisplayHeight(disp, scr));

	int res = cairo_surface_write_to_png_stream(surface, write_to_stream, client);

	if (res == CAIRO_STATUS_WRITE_ERROR) {
		throw std::runtime_error("cannot write current frame");
	}

	cairo_surface_destroy(surface);
}

struct Packet {

	enum PacketType {
		SCREE_FRAME = 1,
		KEYBOARD_EVENT = 2,
		MOUSE_EVENT = 3,
		REQUEST_SCREEN_REFRESH = 4
	};
	virtual PacketType type() = 0;
private:
	PacketType _type;
};

struct InboundPacket : Packet {
};

class MousePacket : public InboundPacket {
public:

	enum MouseEventType {
		NOT_VALID = -1,
		LEFT_BUTTON = 1,
		RIGHT_BUTTON = 2,
		MIDDLE_BUTTON = 3,
		DOUBLE_LEFT_BUTTON = 4
	};
public:

	MousePacket() :
	mousex(-1.0), mousey(-1.0), _type(NOT_VALID)
	{
	}

	MousePacket(double x, double y, MouseEventType type) :
	mousex(x), mousey(y), _type(type)
	{

	}

	virtual PacketType type()
	{
		return MOUSE_EVENT;
	}

	MouseEventType mouseEventType() const
	{
		return _type;
	}

	double x() const
	{
		return mousex;
	}

	double y() const
	{
		return mousey;
	}
private:
	double mousex, mousey;
	MouseEventType _type;
};

MousePacket readMouseEvent(BluetoothSocket *conn)
{
	double x;
	double y;
	char button;
	conn->read(sizeof(double), &x);
	conn->read(sizeof(double), &y);
	conn->read(1, &button);
	MousePacket pkt(x, y, (MousePacket::MouseEventType)button);

	cout << "event [ x=" << x << ", y=" << y << ", btn=" << (int) button << "]" << endl;
	return pkt;
}

void readInboundEvent(BluetoothSocket *conn, vkm::Mouse &mouse)
{
	char type;
	conn->read(1, &type);
	cout << "read " << (int) type << endl;
	if (type == InboundPacket::MOUSE_EVENT) {
		MousePacket pkt = readMouseEvent(conn);
		mouse.moveTo(vkm::Point(pkt.x(), pkt.y()));
		cout << "mouse moved" << endl;
		usleep(10000);
		mouse.clickLeft();
		cout << "mouse clicked" << endl;
	} else if (type == InboundPacket::REQUEST_SCREEN_REFRESH) {
		cout << "Frame refresh request arrived" << endl;
	}
}

void run()
{

	BluetoothServerSocket srv;
	vkm::Mouse mouse;
	vkm::Screen screen = vkm::Screen::getDefaultScreen();
	bool interupted = false;

	registerService(srv);
	srv.listen(1);
	BluetoothSocket * conn = srv.accept();

	cout << "Connection accepted" << endl;

	while (!interupted) {

		try {
			writeCurrentSnapshot(screen, conn);
		} catch (const std::runtime_error &err) {
			cout << err.what() << endl;
		}

		cout << "Frame written" << endl;

		try {
			readInboundEvent(conn, mouse);
		} catch (const std::runtime_error &err) {
			cout << err.what() << endl;
		}

		usleep(100000);

	}
	delete conn;
}

struct OutboundPacket : Packet {
};

struct ScreenFramePacket : public OutboundPacket {

	virtual Packet::PacketType type()
	{
		return SCREE_FRAME;
	}

};

template<typename Socket_T, typename Packet_T> class Handler : public Consumer<Packet_T> {
public:

	Handler(Socket_T &socket) : socket(socket), onConnectionError(0)
	{
	}

	class OnConnectionErrorCallback {
	public:
		virtual void onConnectionError(const std::runtime_error &error) = 0;
		virtual ~OnConnectionErrorCallback(){}
	};

	void setOnConnectionErrorCallback(OnConnectionErrorCallback *callback)
	{
		onConnectionError = callback;
	}

	virtual ~Handler()
	{
		if (onConnectionError != 0) {
			delete onConnectionError;
		}
	}

protected:
	Socket_T &socket;
	OnConnectionErrorCallback *onConnectionError;
};

template<typename Socket_T> class InboundHandler : public Handler<Socket_T, InboundPacket*> {
public:

	InboundHandler(Socket_T &socket) : Handler<Socket_T, InboundPacket*>(socket)
	{

	}

	virtual void proccess(InboundPacket*& item)
	{

	}
private:

};

template<typename Socket_T> class OutboundHandler : public Handler<Socket_T, OutboundPacket*> {
public:

	OutboundHandler(Socket_T &socket) : Handler<Socket_T, OutboundPacket*>(socket)
	{

	}

	virtual void proccess(OutboundPacket*& item)
	{
		switch (item->type()) {
		case OutboundPacket::SCREE_FRAME:
			try {
				writeScreenFrame(dynamic_cast<ScreenFramePacket*> (item));
			} catch (const std::runtime_error &err) {
				if (this->onConnectionError != 0) {
					this->onConnectionError->onConnectionError(err);
				}
				cout << err.what() << endl;
			}
			break;
		}
	}

private:

	static cairo_status_t writeCallback(void * closure, const unsigned char *data, unsigned int length)
	{
		Socket_T * socket = (Socket_T*) closure;
		try {
			socket->write(length, data);
		} catch (const std::runtime_error &err) {
			return CAIRO_STATUS_WRITE_ERROR;
		}
		return CAIRO_STATUS_SUCCESS;
	}

	void writeScreenFrame(ScreenFramePacket *screenFrame)
	{
		static vkm::Screen screen = vkm::Screen::getDefaultScreen();

		Display *disp = screen.display();
		int scr = DefaultScreen(disp);
		Window root = DefaultRootWindow(disp);

		cairo_surface_t *surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
				DisplayWidth(disp, scr),
				DisplayHeight(disp, scr));

		int res = cairo_surface_write_to_png_stream(
					surface, 
					OutboundHandler<Socket_T>::writeCallback, &this->socket);
		
		if (res != CAIRO_STATUS_SUCCESS) {
			throw std::runtime_error("cannot write screen frame packet.");
		}
	}

};

template<typename Socket_T> class RdClientHandler : public Thread {

	class OnHandlerConnectionError : public Handler<Socket_T, void>::OnConnectionErrorCallback {
	public:

		OnHandlerConnectionError(RdClientHandler *outter) : outter(outter)
		{

		}

		virtual void onConnectionError(const std::runtime_error &error)
		{

		}

	private:
		RdClientHandler<Socket_T> *outter;
	};

	friend class OnHandlerConnectionError;

public:

	RdClientHandler(Socket_T &socket) : socket(socket)
	{
		inboundHandler = new InboundHandler<Socket_T>(socket);
		outboundHandler = new OutboundHandler<Socket_T>(socket);
	}

	virtual ~RdClientHandler()
	{
		delete inboundHandler;
		delete outboundHandler;
	}
	

	virtual void run() {
		
		inboundHandler->start();
		outboundHandler->start();
		
		
	}

	
private:
	Socket_T &socket;
	InboundHandler<Socket_T> *inboundHandler;
	OutboundHandler<Socket_T> *outboundHandler;
};

int main()
{
	//	BluetoothServerSocket sock;
	//	registerService(sock);
	//	vkm::Screen screen = vkm::Screen::getDefaultScreen();
	//	
	//	sock.listen(1);
	//	
	//	BluetoothSocket *client = sock.accept();
	//	cout << "Connection created." << endl;
	//	
	//	while(true) {
	//		writeCurrentSnapshot(screen, client);
	//		cout << "screen writen" << endl;
	//		usleep(1000*200);
	//	}
	//	vkm::Mouse mouse;
	//	vkm::Point curr = mouse.queryMousePosition();
	//	mouse.moveTo(vkm::Point(17, 35));
	//	mouse.clickLeft();
	//	cout << curr.x << " " << curr.y << endl;
	//	
	//	pause();
	unsigned char dummy[6];
	BluetoothAddress addr(dummy, dummy, "sample");
	BluetoothSocket socket(addr);

	RdClientHandler<BluetoothSocket> clientHandler(socket);

	//	
	//	delete client;
}