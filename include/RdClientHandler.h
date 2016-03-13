#ifndef RDCLIENTHANDLER_H
#define RDCLIENTHANDLER_H
#include "Conditional.h"
#include "Thread.h"
#include "OnErrorCallback.h"
#include "PacketReaderAsync.h"
#include "PacketWriterAsync.h"
#include "EventProcessorAsync.h"

namespace srv {

    class Wakeable : Conditional {
	bool _sleeping;
    public:

	void sleep() {
	    lock();
	    _sleeping = true;
	    while (_sleeping)
		wait();
	    unlock();
	}

	void wakeUp() {
	    lock();
	    _sleeping = false;
	    signal();
	    unlock();
	}
    };

    template<typename Socket_T> class RdClientHandler : public Thread, public Wakeable {
    };

    template<typename Socket_T> class RdClientHandler<Socket_T*> : public Thread, public Wakeable {

	class SharedContext {
	public:

	    SharedContext(RdClientHandler *context) : context(context) {
	    }

	protected:
	    RdClientHandler *context;
	};

	friend class RdClientHandler<Socket_T*>::SharedContext;
	
	class OnErrorHandler : public OnErrorCallback, public SharedContext {
	public:
	    OnErrorHandler(RdClientHandler *context) : SharedContext(context) {
		
	    }
	    virtual void onError() {
		Log::logMsg("RdClientHandler<Socket_T*>::OnErrorHandler::onError()");
		SharedContext::context->wakeUp();
		Log::logMsg("~RdClientHandler<Socket_T*>::OnErrorHandler::onError()");
	    }
	};

    public:
	
	RdClientHandler(Socket_T *socket) : socket(socket) {
	    asyncEventProcessor = new EventProcessorAsync;
	    asyncOutbound = new PacketWriterAsync<Socket_T*>(500, socket, writeLock);
	    asyncInbound = new PacketReaderAsync<Socket_T*>(asyncEventProcessor, socket, readLock);
	    onErrorCallback = new OnErrorHandler(this);
	    asyncInbound->setOnErrorListener(onErrorCallback);
	    asyncOutbound->setOnErrorListener(onErrorCallback);
	}

	virtual ~RdClientHandler() {
	    delete asyncInbound;
	    delete asyncOutbound;
	    delete asyncEventProcessor;
	    delete onErrorCallback;
	}

	virtual void run() {
	    Log::logMsg("RdClientHandler::run()");

	    try {
		initiateConnection();
	    } catch (const std::exception &err) {
		Log::logMsg("RdClientHandler::run() initiateConnection() error");
		Log::logMsg("~RdClientHandler::run()");
		return;
	    }

	    startAsyncWorkers();

	    sleep();

	    shudownAsyncWorkers();
	    Log::logMsg("~RdClientHandler::run()");
	}

    private:
	Socket_T *socket;
	Lockable readLock;
	Lockable writeLock;
	
	OnErrorCallback * onErrorCallback;
	PacketWriterAsync<Socket_T*> *asyncOutbound;
	PacketReaderAsync<Socket_T*> *asyncInbound;
	EventProcessorAsync *asyncEventProcessor;

    private:

	void shudownAsyncWorkers() {
	    Log::logMsg("RdClientHandler::shudownAsyncWorkers()");

	    asyncOutbound->interrupted(true);
	    asyncOutbound->join();
	    Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncOutbound->join()");

	    asyncInbound->interrupted(true);
	    asyncInbound->join();
	    Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncInbound->join()");

	    asyncEventProcessor->shutdown();
	    asyncEventProcessor->join();
	    Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncEventProcessor->join()");

	    Log::logMsg("~RdClientHandler::shudownAsyncWorkers()");
	}

	void startAsyncWorkers() {
	    asyncOutbound->start();
	    asyncEventProcessor->start();
	    asyncInbound->start();
	}

	void initiateConnection() {
	    Log::logMsg("RdClientHandler::initiateConnection()");
	    Packet::PacketType type = readPacketType();
	    if (type != Packet::CONNECTION_INIT) {
		throw std::runtime_error("invalid state error");
	    }
	    Log::logMsg("RdClientHandler::initiateConnection() ConnectionInitPacket read");
	    OutbountAckPacket ack;
	    char acktype = ack.type();
	    socket->write(1, &acktype);
	    Log::logMsg("RdClientHandler::initiateConnection() OutbountAckPacket written");
	}

	Packet::PacketType readPacketType() {
	    Log::logMsg("RdClientHandler::readPacketType() readPacketType()");
	    char b;
	    socket->read(1, &b);
	    Log::logMsg("~RdClientHandler::readPacketType() readPacketType()");
	    return (Packet::PacketType)b;
	}
    };
}

#endif /* RDCLIENTHANDLER_H */

