#ifndef PACKET_H
#define PACKET_H
#include <stdexcept>

namespace rcs_proto {

enum PacketType {
    SCREE_FRAME = 1,
    KEYBOARD_EVENT,
    MOUSE_EVENT
};

class Packet {
public:
    virtual PacketType type() const = 0;
    virtual ~Packet(){}
};

class MouseEventPacket : public Packet {
public:
    enum Button {
        LEFT = 1,
        MIDDLE,
        RIGHT
    };
private:
    double x;
    double y;
    Button btn;
public:
    MouseEventPacket(double x, double y, Button btn) :
        x(x), y(y) {
        if(btn < 1 || btn > 3) {
            throw std::runtime_error("Invalid button");
        }
        this->btn = btn;
    }

    double X() const { return x; }
    double Y() const { return y; }
    Button button() const { return btn; }

    PacketType type() const {
        return MOUSE_EVENT;
    }
};

}

#endif // PACKET_H
