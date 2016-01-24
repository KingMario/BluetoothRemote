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

//vkm::Screen::DisplayManager vkm::Screen::dispManager();
vkm::Screen::DisplayManager vkm::Screen::dispManager;
