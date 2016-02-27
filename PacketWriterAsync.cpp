#include "PacketWriterAsync.h"
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

template<typename Socket_T>
cairo_status_t writeScreenCallback(void * closure, const unsigned char *data, unsigned int length)
{
	Socket_T * socket = (Socket_T*) closure;
	try {
		socket->write(length, data);
	} catch (const std::runtime_error &err) {
		return CAIRO_STATUS_WRITE_ERROR;
	}
	return CAIRO_STATUS_SUCCESS;
}

template<typename Socket_T>
void srv::writeScreenFramePngCairo(ScreenFramePacket *pkt, Socket_T *socket)
{
	socket->write(1, &pkt->type());
	vkm::Screen &screen = vkm::Screen::getDefaultScreen();
	Display *disp = screen.display();
	int scr = DefaultScreen(disp);
	Window root = DefaultRootWindow(disp);
	cairo_surface_t *surface = cairo_xlib_surface_create(
			disp, root, DefaultVisual(disp, scr),
			DisplayWidth(disp, scr),
			DisplayHeight(disp, scr));
	int res = cairo_surface_write_to_png_stream(surface, writeScreenCallback, socket);

	if (res == CAIRO_STATUS_WRITE_ERROR) {
		throw std::runtime_error("cannot write current frame");
	}
	cairo_surface_destroy(surface);
}