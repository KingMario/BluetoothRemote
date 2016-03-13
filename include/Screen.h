#ifndef SCREEN_H
#define SCREEN_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace vkm {

    struct ScreenImage {
	XImage * ximg;

	ScreenImage(XImage *ximg) : ximg(ximg) {

	}

	~ScreenImage() {
	    XDestroyImage(ximg);
	}
    };

    class Screen {
    public:

	struct DisplayManager {
	    Display *disp;

	    DisplayManager() {
		disp = XOpenDisplay(NULL);
	    }

	    ~DisplayManager() {
		XCloseDisplay(disp);
	    }

	};
    private:

	int w, h;

	static DisplayManager dispManager;

	Display *mDisp;

	Screen(int width, int height, Display *disp) :
	w(width), h(height), mDisp(disp) {
	}

    public:

	static Screen getDefaultScreen();

	int width() const {
	    return w;
	}

	int height() const {
	    return h;
	}

	Display *display() {
	    return mDisp;
	}

	ScreenImage *captureScreen() const;

	void recycleScreen(ScreenImage *img) const;

    };

}

#endif // SCREEN_H
