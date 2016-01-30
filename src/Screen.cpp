#include "Screen.h"


#include <iostream>
#include <stdexcept>

vkm::Screen vkm::Screen::getDefaultScreen()  {
    ::Screen*  s = DefaultScreenOfDisplay(dispManager.disp);
    if(s == NULL) {
        throw std::runtime_error("Cannot obtain the default screen.");
    }
    int width = s->width;
    int height = s->height;
    return Screen(width, height, dispManager.disp);
}

vkm::ScreenImage *vkm::Screen::captureScreen() const {
	    Window root = DefaultRootWindow(mDisp);
	    XWindowAttributes gwa;
	    XGetWindowAttributes(mDisp, root, &gwa);
	    int width = gwa.width;
	    int height = gwa.height;
	    XImage *image = XGetImage(mDisp, root, 0, 0, width, height, 0x00ffffff, ZPixmap);
	    ScreenImage *screenImage = new ScreenImage(image);
	    return screenImage;
}

void vkm::Screen::recycleScreen(vkm::ScreenImage *img) const {
	delete img;
}

//vkm::Screen::DisplayManager vkm::Screen::dispManager();
vkm::Screen::DisplayManager vkm::Screen::dispManager;
