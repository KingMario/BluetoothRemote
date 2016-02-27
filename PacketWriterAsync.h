#ifndef PACKETWRITERASYNC_H
#define PACKETWRITERASYNC_H
#include "Consumer.h"
#include "Packet.h"
#include "BluetoothSocket.h"
#include "Screen.h"

namespace srv {

    template<typename Socket_T>
    void writeScreenFramePngCairo(ScreenFramePacket *pkt, Socket_T *socket);

    template<typename Socket_T>
    class PacketWriterAsync : Consumer<srv::Packet*> {
    };

    template<typename Socket_T>
    class PacketWriterAsync<Socket_T*> : public Consumer<srv::OutboundPacket*> {
    public:

	PacketWriterAsync(Socket_T *socket) :
	socket(socket), onWriteError(0) {

	}

	class OnWriteErrorCallback {
	public:
	    void onWriteError(const std::exception &err);

	    virtual ~OnWriteErrorCallback() {
	    }
	};

	void setOnWriteErrorCallback(OnWriteErrorCallback *callback) {
	    onWriteError = callback;
	}

	virtual void proccess(srv::OutboundPacket*& item) {
	    try {
		writePacket(item);
	    } catch (const std::exception &err) {
		if (onWriteError != 0) onWriteError->onWriteError(err);
	    }
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
	    socket->write(1, &pkt->type());
	}

    private:
	Socket_T *socket;
	OnWriteErrorCallback *onWriteError;
    };
}
#endif /* PACKETWRITERASYNC_H */

