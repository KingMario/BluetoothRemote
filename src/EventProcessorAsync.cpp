#include "EventProcessorAsync.h"
#include "Log.h"
#include <unistd.h>

void srv::EventProcessorAsync::proccess(InboundPacket*& item)
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

void srv::EventProcessorAsync::process(MouseEventPacket *pkt)
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

void srv::EventProcessorAsync::process(KeyboardEventPacket *pkt)
{
	Log::logMsg("EventProcessorAsync::process(KeyboardEventPacket *pkt)");
	switch (pkt->keyboardEventType()) {
	case KeyboardEventPacket::SINGLE_KEY:
		process(dynamic_cast<SingleKeyKeyboardEvent*> (pkt));
		Log::logMsg("EventProcessorAsync::process(KeyboardEventPacket *pkt) SINGLE_KEY");
		break;
	case KeyboardEventPacket::STRING_KEY:
		process(dynamic_cast<StringKeyKeyboardEvent*> (pkt));
		Log::logMsg("EventProcessorAsync::process(KeyboardEventPacket *pkt) STRING_KEY");
		break;
	case KeyboardEventPacket::COMBINATION_KEY:
		process(dynamic_cast<CombinationKeyEvent*> (pkt));
		Log::logMsg("EventProcessorAsync::process(KeyboardEventPacket *pkt) COMBINATION_KEY");
		break;
	}
}

void srv::EventProcessorAsync::process(SingleKeyKeyboardEvent* pkt) {
	Log::logMsg("EventProcessorAsync::process(SingleKeyKeyboardEvent* pkt)");
	keyboard.pressBtn(pkt->key());
}