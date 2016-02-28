#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "PacketWriterAsync.h"
#include "Log.h"

#include "suinput.h"
#include "Mouse.h"
#include "Thread.h"

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

class Interruptable : public Thread, Lockable {
	
public:
	Interruptable() : _interrupted(false)
	{
		
	}
	void interrupted(bool interrupted)
	{
		Log::logMsg("Interruptable::interrupted("+std::string((interrupted?"true":"false"))+")");
		lock();
		_interrupted = interrupted;
		unlock();
	}
private:
	bool _interrupted;
protected:
	bool interrupted()
	{
		lock();
		bool interrupted = _interrupted;
		unlock();
		return interrupted;
	}
};

namespace srv {

	class EventProcessorAsync : public Consumer<InboundPacket*> {
	public:

		virtual void proccess(InboundPacket*& item)
		{
			Log::logMsg("EventProcessorAsync::proccess(InboundPacket*& item)");
			switch (item->type()) {
			case Packet::MOUSE_EVENT:
				process(dynamic_cast<MouseEventPacket*> (item));
				break;
			case Packet::KEYBOARD_EVENT:
				process(dynamic_cast<KeyboardEventPacket*> (item));
				break;
			default:
				delete item;
				throw std::runtime_error("not valid packet type.");
			}

			delete item;
		}
	private:

		vkm::Mouse mouse;

		void process(MouseEventPacket *pkt)
		{
			Log::logMsg("EventProcessorAsync::process(MouseEventPacket *pkt)");
			mouse.moveTo(vkm::Point(pkt->x(), pkt->y()));
			switch (pkt->eventType()) {
			case MouseEventPacket::LEFT_BUTTON:
				mouse.clickLeft();
				Log::logMsg("EventProcessorAsync::process(MouseEventPacket *pkt) LEFT_BUTTON");
				break;
			case MouseEventPacket::RIGHT_BUTTON:
				mouse.clickRight();
				Log::logMsg("EventProcessorAsync::process(MouseEventPacket *pkt) RIGHT_BUTTON");
				break;
			case MouseEventPacket::DOUBLE_LEFT_BUTTON:
				mouse.clickLeft();
				usleep(1000 * 100);
				mouse.clickLeft();
				Log::logMsg("EventProcessorAsync::process(MouseEventPacket *pkt) DOUBLE_LEFT_BUTTON");
				break;
			default: throw std::runtime_error("not valid MouseEventPacket type");
			}
		}

		void process(KeyboardEventPacket *pkt)
		{
			Log::logMsg("EventProcessorAsync::process(KeyboardEventPacket *pkt)");
			switch (pkt->keyboardEventType()) {
			case KeyboardEventPacket::SINGLE_KEY:
				process(dynamic_cast<SingleKeyKeyboardEvent*> (pkt));
				break;
			case KeyboardEventPacket::STRING_KEY:
				process(dynamic_cast<StringKeyKeyboardEvent*> (pkt));
				break;
			case KeyboardEventPacket::COMBINATION_KEY:
				process(dynamic_cast<CombinationKeyEvent*> (pkt));
				break;
			}
		}

		void process(SingleKeyKeyboardEvent* pkt)
		{
			throw std::runtime_error("not supported operation");
		}

		void process(StringKeyKeyboardEvent* pkt)
		{
			throw std::runtime_error("not supported operation");
		}

		void process(CombinationKeyEvent* pkt)
		{
			throw std::runtime_error("not supported operation");
		}
	};

	template<typename Socket_T> class RdClientHandler : public Thread {
	};

	template<typename Socket_T> class RdClientHandler<Socket_T*> : public Interruptable {

		class SharedContext {
		public:

			SharedContext(RdClientHandler *context) : context(context)
			{

			}
		protected:
			RdClientHandler *context;
		};

		class ScreenTimer : public Interruptable, SharedContext {
		public:

			ScreenTimer(unsigned int interval, RdClientHandler *context) :
				SharedContext(context), interval(interval)
			{
					
			}

			virtual void run()
			{
				Log::logMsg("RdClientHandler<Socket_T*>::ScreenTimer::run()");
				while (!interrupted()) {
					usleep(1000 * interval);
					SharedContext::context->asyncOutbound->add(new ScreenFramePacket);
				}
			}

		private:
			unsigned int interval;
		};
		
		class OnPacketWriterError : public PacketWriterAsync<Socket_T*>::OnWriteErrorCallback, SharedContext {
		public:
			OnPacketWriterError(RdClientHandler *context) : SharedContext(context) {
				
			}
			virtual void onWriteError(const std::exception &err) {
				Log::logMsg(
				"RdClientHandler::OnPacketWriterError::onWriteError("+std::string(err.what())+")");
			}
		};
		
		friend class RdClientHandler<Socket_T*>::SharedContext;

	public:

		RdClientHandler(Socket_T *socket) : socket(socket)
		{
			asyncOutbound = new PacketWriterAsync<Socket_T*>(socket);
			errCallback = new OnPacketWriterError(this);
			asyncOutbound->setOnWriteErrorCallback(errCallback);
			
			asyncEventProcessor = new EventProcessorAsync;
			
			screenTimer = new ScreenTimer(500, this);
		}

		virtual ~RdClientHandler()
		{
			delete asyncOutbound;
			delete errCallback;
			delete asyncEventProcessor;
			delete screenTimer;
		}

		virtual void run()
		{
			try {
				initiateConnection();
			} catch (const std::exception &err) {
				return;
			}
			
			startAsyncWorkers();
			
			try {
				dataTransfer();
			} catch (const std::exception &err) {
				shudownAsyncWorkers();
				return;
			}
			
			shudownAsyncWorkers();
		}

	private:
		Socket_T *socket;
		PacketWriterAsync<Socket_T*> *asyncOutbound;
		OnPacketWriterError *errCallback;
		EventProcessorAsync *asyncEventProcessor;
		ScreenTimer *screenTimer;
	private:

		void shudownAsyncWorkers()
		{
			Log::logMsg("RdClientHandler::shudownAsyncWorkers()");
			
			screenTimer->interrupted(true);
			screenTimer->join();
			Log::logMsg("RdClientHandler::shudownAsyncWorkers() screenTimer->join()");
			
			asyncOutbound->shutdown();
			asyncOutbound->join();
			Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncOutbound->join()");
			
			asyncEventProcessor->shutdown();
			asyncEventProcessor->join();
			Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncEventProcessor->join()");
			
			Log::logMsg("~RdClientHandler::shudownAsyncWorkers()");
		}
		
		void startAsyncWorkers(){
			asyncOutbound->start();
			asyncEventProcessor->start();
			screenTimer->start();
		}
		
		void dataTransfer()
		{
			Log::logMsg("RdClientHandler::dataTransfer()");
			while (!interrupted()) {
				readInboundPacket();
				Log::logMsg("~RdClientHandler::dataTransfer() readInboundPacket()");
			}
			Log::logMsg("~RdClientHandler::dataTransfer()");
		}

		void readInboundPacket()
		{
			Log::logMsg("RdClientHandler::readInboundPacket()");
			Packet::PacketType packetType = readPacketType();

			switch (packetType) {
			case Packet::MOUSE_EVENT:
				processMouseEvent();
				break;
			case Packet::REQUEST_SCREEN_REFRESH:
				break;
			default: throw std::runtime_error("not expected packet type");
			}
			Log::logMsg("~RdClientHandler::readInboundPacket()");
		}

		void processMouseEvent()
		{
			Log::logMsg("RdClientHandler::processMouseEvent()");
			double x;
			double y;
			char button;
			socket->read(sizeof(double), &x);
			socket->read(sizeof(double), &y);
			socket->read(1, &button);
			MouseEventPacket *mousePkt = new MouseEventPacket(x, y, (MouseEventPacket::MouseEventType)button);
			asyncEventProcessor->add(mousePkt);
		}

		void initiateConnection()
		{
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

		Packet::PacketType readPacketType()
		{
			Log::logMsg("RdClientHandler::readPacketType() readPacketType()");
			char b;
			socket->read(1, &b);
			Log::logMsg("~RdClientHandler::readPacketType() readPacketType()");
			return(Packet::PacketType)b;
		}
	};
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
		clientHandler.join();
		delete socket;
	}
}