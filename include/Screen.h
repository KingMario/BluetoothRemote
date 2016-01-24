#ifndef SCREEN_H
#define SCREEN_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace vkm {

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

    XImage *captureScreen() const {
        Window root = DefaultRootWindow(mDisp);
        XWindowAttributes gwa;
        XGetWindowAttributes(mDisp, root, &gwa);
        int width = gwa.width;
        int height = gwa.height;
        XImage *image = XGetImage(mDisp ,root, 0,0 , width, height, 0x00ffffff, ZPixmap);
        return image;
    }

    void deleteCaptureScreen(XImage *img) const {
        XDestroyImage(img);
    }

};

}

#endif // SCREEN_H
