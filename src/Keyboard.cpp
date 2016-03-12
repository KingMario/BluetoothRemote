#include "Keyboard.h"
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <stdexcept>
#include <errno.h>

void vkm::Keyboard::init(){
	userDev = (struct uinput_user_dev*) malloc(sizeof(struct uinput_user_dev));
	memset(userDev, 0, sizeof(struct uinput_user_dev));
	strcpy(userDev->name, "vkm-keyboard");
	uinputFd = suinput_open();
	if (uinputFd == -1) {
		perror("suinput_open");
		throw std::runtime_error("Cannot create mouse");
	}
	int evNum = 54;
	for (int i = 0; i < evNum; i++) {
		if(suinput_enable_event(uinputFd, EV_KEY, KEYBOARD_EVENTS[i])==-1) {
			throw std::runtime_error("cannot suinput_enable_event");
		}
	}
	if(suinput_create(uinputFd, userDev) == -1) {
		throw std::runtime_error("cannot suinput_create");
	}
}

vkm::Keyboard::Keyboard()
{
	init();
}

void vkm::Keyboard::pressBtn(unsigned short button) {
	if(suinput_emit_click(uinputFd, button) == -1) {
		throw std::runtime_error("cannot suinput_emit_click");
	}
	if(suinput_syn(uinputFd) == -1) {
		throw std::runtime_error("cannot suinput_syn");
	}
}

vkm::Keyboard::Keyboard(const Keyboard& orig)
{
	throw std::runtime_error("cannot copy mouse");
}

vkm::Keyboard::~Keyboard()
{
	free(userDev);
}

const unsigned short vkm::Keyboard::KEYBOARD_EVENTS[] = {

	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL,

	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_BACKSPACE,

	KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_ENTER,

	KEY_LEFTSHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_LEFTBRACE, KEY_RIGHTBRACE, KEY_COMMA, KEY_DOT,

	KEY_LEFTCTRL, KEY_LEFTALT, KEY_SPACE, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_SLASH, KEY_BACKSLASH

};

