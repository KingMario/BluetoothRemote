#ifndef PACKETWRITERASYNC_H
#define PACKETWRITERASYNC_H
#include "BluetoothSocket.h"
#include "Screen.h"
#include "Interruptable.h"
#include "Log.h"
#include "Packet.h"
#include "OnErrorCallback.h"

#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <stdexcept>
#include <errno.h>
#include <sstream>


namespace srv {

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

	PacketWriterAsync(unsigned int interval, Socket_T *socket, Lockable &writeLock) :
	    interval(interval), socket(socket), writeLock(writeLock), onErrorListener(0) {
		
	    timerRequest.tv_sec = interval/1000;
	    timerRequest.tv_nsec = 1000000*(interval%1000);
	}
	
	void setOnErrorListener(OnErrorCallback *onErrorListener) {
	    this->onErrorListener = onErrorListener;
	}
	    
	virtual void run() {
	    Log::logMsg("PacketWriterAsync::proccess(srv::OutboundPacket*& item)");
	    while (!interrupted()) {
		try {
		    ScreenFramePacket * frame = new ScreenFramePacket;
		    writeLock.lock();
		    Log::logMsg("~PacketWriterAsync::proccess(srv::OutboundPacket*& item) start write");
		    writePacket(frame);
		    Log::logMsg("~PacketWriterAsync::proccess(srv::OutboundPacket*& item) end write");
		    writeLock.unlock();
		} catch (const Thread_exception &thrdErr) {
		    Log::logMsg("AsyncPacketReader::run() error "+std::string(thrdErr.message()));
		    handleError();
		    break;
		}  catch (const std::exception &err) {
		    Log::logMsg("AsyncPacketReader::run() error "+std::string(err.what()));
		    writeLock.unlock();
		    handleError();
		    break;
		} catch (...) {
		    Log::logMsg("AsyncPacketReader::run() unknown error ");
		    handleError();
		    break;
		}
		std::ostringstream sstr;
		sstr << "Sleep " << timerRequest.tv_sec << " " << timerRequest.tv_nsec;
		Log::logMsg("~PacketWriterAsync::proccess(srv::OutboundPacket*& item) "+sstr.str());
		nanosleep(&timerRequest, NULL);
	    }
	    Log::logMsg("~PacketWriterAsync::proccess(srv::OutboundPacket*& item)");
	}

    private:
	
	void handleError() {
	    interrupted(true);
	    if(onErrorListener != 0) {
		onErrorListener->onError();
	    }
	}
	
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
	
	void writeScreenFrame(ScreenFramePacket *pkt) {
	    writeScreenFramePngCairo(pkt, socket);
	}

	void writeAck(OutbountAckPacket *pkt) {
	    char type = pkt->type();
	    socket->write(1, &type);
	}

	unsigned int interval;
	Socket_T *socket;
	Lockable &writeLock;
	
	timespec timerRequest;
	
	char printBuffer[2048];
	
	OnErrorCallback *onErrorListener;
	
    };
}
#endif /* PACKETWRITERASYNC_H */

