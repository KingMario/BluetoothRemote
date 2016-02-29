#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "PacketWriterAsync.h"
#include "EventProcessorAsync.h"
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



namespace srv {

	template<typename Socket_T> class RdClientHandler : public Thread {
	};

	template<typename Socket_T> class RdClientHandler<Socket_T*> : public Thread {

		class SharedContext {
		public:

			SharedContext(RdClientHandler *context) : context(context)
			{
			}
			
		protected:
			RdClientHandler *context;
		};

		class OnPacketWriterError : public PacketWriterAsync<Socket_T*>::OnWriteErrorCallback, SharedContext {
		public:

			OnPacketWriterError(RdClientHandler *context) : SharedContext(context)
			{
			}

			virtual void onWriteError(const std::exception &err)
			{
				Log::logMsg("virtual void onWriteError("+std::string(err.what())+")");
			}
		};

		friend class RdClientHandler<Socket_T*>::SharedContext;

	public:

		RdClientHandler(Socket_T *socket) : socket(socket)
		{
			asyncOutbound = new PacketWriterAsync<Socket_T*>(500, socket);
			errCallback = new OnPacketWriterError(this);
			asyncOutbound->setOnWriteErrorCallback(errCallback);

			asyncEventProcessor = new EventProcessorAsync;
		}

		virtual ~RdClientHandler()
		{
			delete asyncOutbound;
			delete errCallback;
			delete asyncEventProcessor;
		}

		virtual void run()
		{
			Log::logMsg("RdClientHandler::run()");

			try {
				initiateConnection();
			} catch (const std::exception &err) {
				Log::logMsg("RdClientHandler::run() initiateConnection() error");
				Log::logMsg("~RdClientHandler::run()");
				return;
			}

			startAsyncWorkers();

			try {
				dataTransfer();
			} catch (const std::exception &err) {
				shudownAsyncWorkers();
				Log::logMsg("~RdClientHandler::run()");
				return;
			}
			
			shudownAsyncWorkers();
			Log::logMsg("~RdClientHandler::run()");
		}

	private:
		Socket_T *socket;
		PacketWriterAsync<Socket_T*> *asyncOutbound;
		OnPacketWriterError *errCallback;
		EventProcessorAsync *asyncEventProcessor;
	private:

		void shudownAsyncWorkers()
		{
			Log::logMsg("RdClientHandler::shudownAsyncWorkers()");

			asyncOutbound->interrupted(true);
			asyncOutbound->join();
			Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncOutbound->join()");

			asyncEventProcessor->shutdown();
			asyncEventProcessor->join();
			Log::logMsg("RdClientHandler::shudownAsyncWorkers() asyncEventProcessor->join()");

			Log::logMsg("~RdClientHandler::shudownAsyncWorkers()");
		}

		void startAsyncWorkers()
		{
			asyncOutbound->start();
			asyncEventProcessor->start();
		}

		void dataTransfer()
		{
			Log::logMsg("RdClientHandler::dataTransfer()");
			while (true) {
				Log::logMsg("~RdClientHandler::dataTransfer() readInboundPacket()");
				readInboundPacket();
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
		Log::logMsg("::main() clientHandler.join()");
		clientHandler.join();
		Log::logMsg("::main() delete socket");
		delete socket;
	}

}