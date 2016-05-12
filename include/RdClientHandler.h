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

  void sleep(uint64 millis) {
    lock();
    _sleeping = true;
    while (_sleeping) {
      wait(millis);
      _sleeping = false;
    }
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

/*
 *  Each incoming connection is handed to this class which delegates the reading/writing
 * to the asyncInbound and asyncOutbound.
 * The class is templated so it can be used with any socket. The only requirement
 * for the template argument is to have read and write methods.
 * 
 */
template<typename Socket_T> class RdClientHandler<Socket_T*> : public Thread, public Wakeable {
  /*
   *  Allows the callback functions to modify the internal state of the object
   * not very clever but works for now.
   */
  class SharedContext {
  public:
    SharedContext(RdClientHandler *context) : context(context) {
    }
  protected:
    RdClientHandler *context;
  };

  friend class RdClientHandler<Socket_T*>::SharedContext;
  
  /*
   *  Callback class used in async inbound and outbound to
   *  wakeUp the handler, which will terminate connection.
   */
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
  /**
   * Initiates the connection ( makes handshake with the client )
   * and then hands off the read/write to the corresponding workers
   * when the connection is terminated the workers are joined.
   */
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
  /**
   * Makes 2 way handshake with the client to initiate the data transfer
   */
  void initiateConnection() {
    Log::logMsg("RdClientHandler::initiateConnection()");
    Packet::PacketType type = asyncInbound->readPacketType();
    if (type != Packet::CONNECTION_INIT) {
      throw std::runtime_error("invalid state error");
    }
    Log::logMsg("RdClientHandler::initiateConnection() ConnectionInitPacket read");
    OutbountAckPacket ack;
    asyncOutbound->writePacket(&ack);
    Log::logMsg("RdClientHandler::initiateConnection() OutbountAckPacket written");
  }
};
}

#endif /* RDCLIENTHANDLER_H */

