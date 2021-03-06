#ifndef EVENTPROCESSORASYNC_H
#define EVENTPROCESSORASYNC_H
#include "Consumer.h"
#include "Packet.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <stdexcept>

namespace srv {

    class EventProcessorAsync : public Consumer<InboundPacket*> {
    public:

	virtual void proccess(InboundPacket*& item);

    private:

	vkm::Mouse mouse;
	vkm::Keyboard keyboard;

	void process(MouseEventPacket *pkt);

	void process(KeyboardEventPacket *pkt);

	void process(SingleKeyKeyboardEvent* pkt);

	void process(StringKeyKeyboardEvent* pkt) {
	    throw std::runtime_error("not supported operation");
	}

	void process(CombinationKeyEvent* pkt) {
	    throw std::runtime_error("not supported operation");
	}
    };
}
#endif /* EVENTPROCESSORASYNC_H */

