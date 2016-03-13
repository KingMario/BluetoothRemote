#ifndef FRAMEENCODER_H
#define FRAMEENCODER_H

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "Screen.h"

class FFMpegEnv {
public:

    FFMpegEnv() {
	avcodec_register_all();
    }
};

struct Frame {
    AVPacket * packet;
};

class FrameEncoder {
    AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket pkt;
    SwsContext * ctx;
    int gotOutput;
    int currentFrame;
    int inputWidth;
    int inputHeight;
    int scaledWidth;
    int scaledHeight;
public:
    FrameEncoder(
	    AVCodecID codec_id,
	    int outputWidth,
	    int outputHeight,
	    int bitRate,
	    int scaledWidth = -1,
	    int scaledHeight = -1
	    );
    Frame * encodeFrame(vkm::ScreenImage * image);
    Frame * getDelayedFrames();
    void recycleFrame(Frame * frame);
    virtual ~FrameEncoder();
private:
    FrameEncoder(const FrameEncoder& orig);
    FrameEncoder& operator=(const FrameEncoder& orig);
    static FFMpegEnv env;
    void clean();
};

#endif /* FRAMEENCODER_H */

