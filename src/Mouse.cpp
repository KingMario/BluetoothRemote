#include "Mouse.h"
#include <stdexcept>
#include <cstring>
#include <X11/Xlib.h>
#include <unistd.h>

vkm::Mouse::Mouse(Screen screen) : scr(screen), uinput_fd(-1)
{
	mouseCreate();
}

vkm::Mouse::Mouse() : scr(Screen::getDefaultScreen()), uinput_fd(-1)
{
	mouseCreate();
}

vkm::Mouse::~Mouse()
{
	mouseDestroy();
}

vkm::Mouse::Mouse(const Mouse &) :
scr(Screen::getDefaultScreen())
{
	throw std::runtime_error("Cannot copy mouse");
}

vkm::Mouse vkm::Mouse::operator=(const Mouse &)
{
	throw std::runtime_error("Cannot copy mouse");
}

void vkm::Mouse::mouseCreate()
{
	int btns[] = {BTN_LEFT, BTN_RIGHT, BTN_MIDDLE};
	int rel_axes[] = {REL_X, REL_Y, REL_WHEEL};
	struct uinput_user_dev user_dev;
	memset(&user_dev, 0, sizeof(struct uinput_user_dev));
	strcpy(user_dev.name, "vkm-mouse");
	uinput_fd = suinput_open();
	if (uinput_fd == -1) {
		throw std::runtime_error("Cannot create mouse");
	}
	for (int i = 0; i < 3; ++i) {
		if (suinput_enable_event(uinput_fd, EV_KEY, btns[i]) == -1) {
			suinput_destroy(uinput_fd);
			throw std::runtime_error("Cannot enable event");
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (suinput_enable_event(uinput_fd, EV_REL, rel_axes[i]) == -1) {
			suinput_destroy(uinput_fd);
			throw std::runtime_error("Cannot enable event");
		}
	}
	if (suinput_create(uinput_fd, &user_dev) == -1) {
		throw std::runtime_error("Cannot suinput_create");
	}
}

void vkm::Mouse::mouseDestroy()
{
	suinput_destroy(uinput_fd);
}

void vkm::Mouse::moveMouse(const Point &p)
{
	Point newPoint;
	newPoint.x = p.x < 0 ? 0 : p.x;
	newPoint.x = p.x >= scr.width() ? scr.width() - 1 : p.x;
	newPoint.y = p.y < 0 ? 0 : p.y;
	newPoint.y = p.y >= scr.height() ? scr.height() - 1 : p.y;
	Point current = queryMousePosition();
	Point delta(
			newPoint.x - current.x,
			newPoint.y - current.y
			);
	for (int i = 0; i < 1; i++) {
		if (suinput_emit(uinput_fd, EV_REL, REL_X, delta.x) == -1) {
			throw std::runtime_error("cannot suinput_emit");
		}
		if (suinput_emit(uinput_fd, EV_REL, REL_Y, delta.y) == -1) {
			throw std::runtime_error("cannot suinput_emit");
		}
	}
	sync();
}

void vkm::Mouse::leftClickMouse()
{
	if (suinput_emit_click(uinput_fd, BTN_LEFT) == -1) {
		throw std::runtime_error("cannot suinput_emit_click");
	}
	sync();
}

void vkm::Mouse::rightClickMouse()
{
	if (suinput_emit_click(uinput_fd, BTN_RIGHT) == -1) {
		throw std::runtime_error("cannot suinput_emit_click");
	}
	sync();
}

void vkm::Mouse::sync()
{
	if (suinput_syn(uinput_fd) == -1) {
		throw std::runtime_error("cannot suinput_syn");
	}
}

void vkm::Mouse::moveTo(const Point &p)
{
	moveMouse(p);
}

void vkm::Mouse::clickRight()
{
	rightClickMouse();
}

void vkm::Mouse::clickLeft()
{
	leftClickMouse();
}

vkm::Point vkm::Mouse::queryMousePosition()
{
	Display *dsp = XOpenDisplay(NULL);

	if (!dsp) {
		throw std::runtime_error("cannot XOpenDisplay");
	}

	XEvent event;

	/* get info about current pointer position */
	XQueryPointer(dsp, RootWindow(dsp, DefaultScreen(dsp)),
			&event.xbutton.root, &event.xbutton.window,
			&event.xbutton.x_root, &event.xbutton.y_root,
			&event.xbutton.x, &event.xbutton.y,
			&event.xbutton.state);

	Point position(event.xbutton.x, event.xbutton.y);

	XCloseDisplay(dsp);

	return position;
}
