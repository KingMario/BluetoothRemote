#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "Packet.h"
#include "Consumer.h"
#include "Screen.h"
#include <cairo-xlib.h>
#include <stdexcept>
#include <cairo.h>

namespace srv {
    
    class OnPacketProcessedCallback {
    public:
        virtual void onPacketProcessed(Packet *packet) = 0;
        virtual ~OnPacketProcessedCallback() { }
    };
    
    class OnConnectionErrorCallback {
    public:
        virtual void onConnectionError(const std::runtime_error &error) = 0; 
        virtual ~OnConnectionErrorCallback() {  }
    };
    
    template<typename Socket_T, typename Packet_T> 
    class PacketHandler : public Consumer<Packet_T> {
    public:

        PacketHandler(Socket_T &socket) :   
            socket(socket), onConnectionError(0), onPacketProcessed(0) {
        }
        
        void setOnConnectionErrorCallback(OnConnectionErrorCallback *callback) {
            onConnectionError = callback;
        }
        
        void setOnPacketProcessedCallback(OnPacketProcessedCallback *callback) {
            onPacketProcessed = callback;
        }

        virtual ~PacketHandler() { }

    protected:
        Socket_T &socket;
        OnConnectionErrorCallback *onConnectionError;
        OnPacketProcessedCallback *onPacketProcessed;
    };

    template<typename Socket_T> 
    class OutboundPacketHandler : public PacketHandler<Socket_T, OutboundPacket*> {
    public:

        OutboundPacketHandler(Socket_T &socket) : PacketHandler<Socket_T, OutboundPacket*>(socket) {

        }

        virtual void proccess(OutboundPacket*& item) {
            if(item->type() == Packet::SCREE_FRAME) {
                try {
                    writeScreenFrame(dynamic_cast<ScreenFramePacket*>(item));
                } catch (const std::runtime_error &err) {
                    PacketHandler<Socket_T, OutboundPacket*>::onConnectionError->onConnectionError(err);
                }
            }
            if(PacketHandler<Socket_T, OutboundPacket*>::onPacketProcessed != 0) {
                PacketHandler<Socket_T, OutboundPacket*>::onPacketProcessed->onPacketProcessed(item);
            }
            delete item;
        }

    private:

        static cairo_status_t writeCallback(void * closure, const unsigned char *data, unsigned int length)  {
            Socket_T * socket = (Socket_T*) closure;
            try {
                socket->write(length, data);
            } catch (const std::runtime_error &err) {
                return CAIRO_STATUS_WRITE_ERROR;
            }
            return CAIRO_STATUS_SUCCESS;
        }

        void writeScreenFrame(ScreenFramePacket *screenFrame){
            static vkm::Screen screen = vkm::Screen::getDefaultScreen();

            Display *disp = screen.display();
            int scr = DefaultScreen(disp);
            Window root = DefaultRootWindow(disp);

            cairo_surface_t *surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
                    DisplayWidth(disp, scr),
                    DisplayHeight(disp, scr));

            int res = cairo_surface_write_to_png_stream(
                    surface,
                    OutboundPacketHandler<Socket_T>::writeCallback, &this->socket);

            if (res != CAIRO_STATUS_SUCCESS) {
                throw std::runtime_error("cannot write screen frame packet.");
            }
        }
    };

    template<typename Socket_T> 
    class InboundPacketHandler : public PacketHandler<Socket_T, InboundPacket*> {
    public:
        
        InboundPacketHandler(Socket_T &socket) : PacketHandler<Socket_T, InboundPacket*>(socket) {
        }

        virtual void proccess(InboundPacket*& item) {
            
        }
        
    private:

    };


}
#endif /* PACKETHANDLER_H */

