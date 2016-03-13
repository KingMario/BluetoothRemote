#ifndef MOUSE_H
#define MOUSE_H
#include "Screen.h"

namespace vkm {

    struct Point {
	int x;
	int y;

	Point(int x, int y) :
	x(x), y(y) {
	}

	Point() {
	}
    };

    class Mouse {
	Mouse(const Mouse &);

	Mouse operator=(const Mouse &);

	void mouseCreate();

	void mouseDestroy();

	void moveMouse(const Point &p);

	void leftClickMouse();

	void rightClickMouse();

	void sync();

	Screen scr;

	int uinput_fd;

    public:
	Mouse(Screen screen);

	Mouse();

	~Mouse();

	void moveTo(const Point &p);

	void clickRight();

	void clickLeft();

	Point queryMousePosition();
    };

}

#endif // MOUSE_H
