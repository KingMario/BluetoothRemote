#ifndef ONERRORCALLBACK_H
#define ONERRORCALLBACK_H

class OnErrorCallback {
public:
    virtual void onError() = 0;
    virtual ~OnErrorCallback(){
	
    }
};

#endif /* ONERRORCALLBACK_H */

