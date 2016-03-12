#ifndef PACKET_H
#define PACKET_H
#include <iostream>

namespace srv {

    struct Packet {

	enum PacketType {
	    SCREE_FRAME = 1,
	    KEYBOARD_EVENT = 2,
	    MOUSE_EVENT = 3,
	    REQUEST_SCREEN_REFRESH = 4,
	    CONNECTION_INIT = 5,
	    ACK = 6
	};
	virtual PacketType type() = 0;

	virtual ~Packet() {
	}
    };

    struct InboundPacket : Packet {
    };

    struct OutboundPacket : Packet {
    };

    struct ScreenFramePacket : OutboundPacket {

	virtual Packet::PacketType type() {
	    return Packet::SCREE_FRAME;
	}
    };

    struct KeyboardEventPacket : InboundPacket {

	enum KeyboardEventType {
	    SINGLE_KEY = 1,
	    COMBINATION_KEY = 2,
	    STRING_KEY = 3,
	    
	};

	virtual Packet::PacketType type() {
	    return Packet::KEYBOARD_EVENT;
	}
	virtual KeyboardEventType keyboardEventType() = 0;
    };

    struct SingleKeyKeyboardEvent : KeyboardEventPacket {

	SingleKeyKeyboardEvent() : keyCode(-1) {
	}

	SingleKeyKeyboardEvent(unsigned short key) : keyCode(key) {
	}

	virtual KeyboardEventPacket::KeyboardEventType keyboardEventType() {
	    return KeyboardEventPacket::SINGLE_KEY;
	}

	unsigned short key() const {
	    return keyCode;
	}
    private:
	unsigned short keyCode;
    };

    struct StringKeyKeyboardEvent : KeyboardEventPacket {

	StringKeyKeyboardEvent() {
	}

	void add(SingleKeyKeyboardEvent ev) {
	    sequence.push_back(ev);
	}

	size_t length() {
	    return sequence.size();
	}

	const SingleKeyKeyboardEvent& operator[](int i) const {
	    return sequence[i];
	}

	KeyboardEventType keyboardEventType() {
	    return KeyboardEventPacket::STRING_KEY;
	}
    private:
	std::vector<SingleKeyKeyboardEvent> sequence;
    };

    struct CombinationKeyEvent : StringKeyKeyboardEvent {

	virtual KeyboardEventPacket::KeyboardEventType keyboardEventType() {
	    return KeyboardEventPacket::COMBINATION_KEY;
	}
    };

    struct MouseEventPacket : InboundPacket {

	enum MouseEventType {
	    NOT_VALID = -1,
	    LEFT_BUTTON = 1,
	    RIGHT_BUTTON = 2,
	    MIDDLE_BUTTON = 3,
	    DOUBLE_LEFT_BUTTON = 4
	};

	MouseEventPacket() :
	xCoordinate(-1), yCoordinate(-1), mouseEventType(NOT_VALID) {
	}

	MouseEventPacket(double x, double y, MouseEventType eventType) :
	xCoordinate(x), yCoordinate(y), mouseEventType(eventType) {

	}

	virtual Packet::PacketType type() {
	    return Packet::MOUSE_EVENT;
	}

	double x() const {
	    return xCoordinate;
	}

	double y() const {
	    return yCoordinate;
	}

	MouseEventType eventType() {
	    return mouseEventType;
	}

    private:
	double xCoordinate;
	double yCoordinate;
	MouseEventType mouseEventType;
    };

    struct RequestScreenRefreshPacket : InboundPacket {

	virtual Packet::PacketType type() {
	    return Packet::REQUEST_SCREEN_REFRESH;
	}
    };

    struct ConnectionInitPacket : InboundPacket {

	virtual PacketType type() {
	    return Packet::CONNECTION_INIT;
	}
    };

    struct InboundAckPacket : InboundPacket {

	virtual PacketType type() {
	    return Packet::ACK;
	}
    };

    struct OutbountAckPacket : OutboundPacket {

	virtual PacketType type() {
	    return Packet::ACK;
	}
    };

}

#endif /* PACKET_H */

