#ifndef PACKETWRITERASYNC_H
#define PACKETWRITERASYNC_H
#include "Consumer.h"
#include "BluetoothSocket.h"
#include "Screen.h"
#include "Log.h"
#include "Packet.h"

#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <stdexcept>

namespace srv {

    class Interruptable : public Thread, Lockable {
    public:

	Interruptable() : _interrupted(false) {

	}

	void interrupted(bool interrupted) {
	    Log::logMsg("Interruptable::interrupted(" + std::string((interrupted ? "true" : "false")) + ")");
	    lock();
	    _interrupted = interrupted;
	    unlock();
	}
    private:
	bool _interrupted;
    protected:

	bool interrupted() {
	    lock();
	    bool interrupted = _interrupted;
	    unlock();
	    return interrupted;
	}
    };

    template<typename Socket_T>
    cairo_status_t writeScreenCallback(void * closure, const unsigned char *data, unsigned int length) {
	Socket_T * socket = (Socket_T*) closure;
	try {
	    socket->write(length, data);
	} catch (const std::runtime_error &err) {
	    return CAIRO_STATUS_WRITE_ERROR;
	}
	return CAIRO_STATUS_SUCCESS;
    }

    template<typename Socket_T>
    void writeScreenFramePngCairo(ScreenFramePacket *pkt, Socket_T *socket) {
	char type = pkt->type();
	socket->write(1, &type);

	vkm::Screen screen = vkm::Screen::getDefaultScreen();
	Display *disp = screen.display();
	int scr = DefaultScreen(disp);
	Window root = DefaultRootWindow(disp);
	cairo_surface_t *surface = cairo_xlib_surface_create(
		disp, root, DefaultVisual(disp, scr),
		DisplayWidth(disp, scr),
		DisplayHeight(disp, scr));
	int res = cairo_surface_write_to_png_stream(surface, writeScreenCallback<Socket_T>, socket);

	if (res == CAIRO_STATUS_WRITE_ERROR) {
	    throw std::runtime_error("cannot write current frame");
	}
	cairo_surface_destroy(surface);
    }

    template<typename Socket_T>
    class PacketWriterAsync : public Interruptable {
    };

    template<typename Socket_T>
    class PacketWriterAsync<Socket_T*> : public Interruptable {
    public:

	PacketWriterAsync(unsigned int interval, Socket_T *socket) :
	interval(interval), socket(socket), onWriteError(0) {

	}

	class OnWriteErrorCallback {
	public:
	    virtual void onWriteError(const std::exception &err) = 0;

	    virtual ~OnWriteErrorCallback() {
	    }
	};

	void setOnWriteErrorCallback(OnWriteErrorCallback *callback) {
	    onWriteError = callback;
	}

	virtual void run() {
	    Log::logMsg("PacketWriterAsync::proccess(srv::OutboundPacket*& item)");
	    while (!interrupted()) {
		usleep(1000 * interval);
		try {
		    ScreenFramePacket * frame = new ScreenFramePacket;
		    writeLock.lock();
		    writePacket(frame);
		    writeLock.unlock();
		} catch (const std::exception &err) {
		    if (onWriteError != 0) onWriteError->onWriteError(err);
		    interrupted(true);
		    return;
		}
	    }
	    Log::logMsg("~PacketWriterAsync::proccess(srv::OutboundPacket*& item)");
	}

    private:

	void writePacket(OutboundPacket *pkt) {
	    switch (pkt->type()) {
		case Packet::SCREE_FRAME:
		    writeScreenFrame(dynamic_cast<ScreenFramePacket*> (pkt));
		    break;
		case Packet::ACK:
		    writeAck(dynamic_cast<OutbountAckPacket*> (pkt));
		    break;
	    }
	}
	
    private:

	void writeScreenFrame(ScreenFramePacket *pkt) {
	    writeScreenFramePngCairo(pkt, socket);
	}

	void writeAck(OutbountAckPacket *pkt) {
	    char type = pkt->type();
	    socket->write(1, &type);
	}

    private:
	unsigned int interval;
	Socket_T *socket;
	Lockable writeLock;
	OnWriteErrorCallback *onWriteError;
    };
}
#endif /* PACKETWRITERASYNC_H */

