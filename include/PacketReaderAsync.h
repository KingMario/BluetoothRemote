#ifndef PACKETREADERASYNC_H
#define PACKETREADERASYNC_H

#include "OnErrorCallback.h"
#include "Interruptable.h"
#include "EventProcessorAsync.h"


namespace srv {

    template<typename Socket_T>
    class PacketReaderAsync : public Interruptable {
    };

    template<typename Socket_T>
    class PacketReaderAsync<Socket_T*> : public Interruptable {
    public:

	PacketReaderAsync(EventProcessorAsync *asyncEventProcessor,
		Socket_T *socket,
		Lockable &readLock) :
	socket(socket),
	asyncEventProcessor(asyncEventProcessor),
	readLock(readLock),
	onErrorListener(0) {

	}

	void setOnErrorListener(OnErrorCallback *onErrorListener) {
	    this->onErrorListener = onErrorListener;
	}

	virtual void run() {
	    while (!interrupted()) {
		try {
		    readLock.lock();
		    readInboundPacket();
		    readLock.unlock();
		} catch (const std::runtime_error &err) {
		    Log::logMsg("PacketReaderAsync::run() error " + std::string(err.what()));
		    handleError();
		    readLock.unlock();
		    break;
		} catch (const Thread_exception &thrdErr) {
		    Log::logMsg("PacketReaderAsync::run() error " + std::string(thrdErr.message()));
		    handleError();
		    break;
		} catch (...) {
		    Log::logMsg("PacketReaderAsync::run() error unknown");
		    handleError();
		    readLock.unlock();
		    break;
		}
	    }
	}

    private:

	void handleError() {
	    interrupted(true);
	    if (onErrorListener != 0) {
		onErrorListener->onError();
	    }
	}

	void readInboundPacket() {
	    Log::logMsg("AsyncPacketReader::readInboundPacket()");
	    Packet::PacketType packetType = readPacketType();

	    switch (packetType) {
		case Packet::MOUSE_EVENT:
		    readMouseEvent();
		    break;
		case Packet::REQUEST_SCREEN_REFRESH:
		    break;
		case Packet::KEYBOARD_EVENT:
		    readKeyboardEvent();
		    break;
		default: throw std::runtime_error("not expected packet type");
	    }
	    Log::logMsg("~AsyncPacketReader::readInboundPacket()");
	}

	void readMouseEvent() {
	    Log::logMsg("AsyncPacketReader::processMouseEvent()");
	    double x;
	    double y;
	    char button;
	    socket->read(sizeof (double), &x);
	    socket->read(sizeof (double), &y);
	    socket->read(1, &button);
	    MouseEventPacket *mousePkt = new MouseEventPacket(x, y, (MouseEventPacket::MouseEventType)button);
	    asyncEventProcessor->add(mousePkt);
	}

	void readKeyboardEvent() {
	    Log::logMsg("AsyncPacketReader::processKeyboardEvent()");
	    unsigned char keyboardEventType = 0;
	    socket->read(sizeof (unsigned char), &keyboardEventType);
	    switch (keyboardEventType) {
		case KeyboardEventPacket::SINGLE_KEY:
		{
		    unsigned char keyCode = 0;
		    socket->read(sizeof (unsigned char), &keyCode);
		    SingleKeyKeyboardEvent * keyboardEvent = new SingleKeyKeyboardEvent(keyCode);
		    asyncEventProcessor->add(keyboardEvent);
		}
		    break;
		case KeyboardEventPacket::COMBINATION_KEY:

		    break;
		case KeyboardEventPacket::STRING_KEY:

		    break;
		default:
		    throw std::runtime_error("not valid KeyboardEventPacket");
	    }
	}

	Packet::PacketType readPacketType() {
	    Log::logMsg("AsyncPacketReader::readPacketType() readPacketType()");
	    char b;
	    socket->read(1, &b);
	    Log::logMsg("~AsyncPacketReader::readPacketType() readPacketType()");
	    return (Packet::PacketType)b;
	}

    private:

	EventProcessorAsync *asyncEventProcessor;
	Socket_T *socket;
	Lockable &readLock;
	OnErrorCallback *onErrorListener;

    };
}

#endif /* PACKETREADERASYNC_H */

