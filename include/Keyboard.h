#ifndef KEYBOARD_H
#define KEYBOARD_H

struct uinput_user_dev ;

namespace vkm {
class Keyboard {
public:
    
    Keyboard();
    Keyboard(const Keyboard& orig);
    virtual ~Keyboard();
    
    void pressBtn(unsigned short button);
    
private:
    
    void init();
    int uinputFd;
    uinput_user_dev *userDev;
    const static unsigned short KEYBOARD_EVENTS[];
};
}
#endif /* KEYBOARD_H */

