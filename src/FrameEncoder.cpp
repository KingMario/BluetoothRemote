/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FrameEncoder.cpp
 * Author: nikola
 * 
 * Created on January 29, 2016, 8:20 PM
 */

#include "FrameEncoder.h"
#include "Screen.h"
#include <stdexcept>

FrameEncoder::FrameEncoder(
		AVCodecID codec_id,
		int inputWidth,
		int inputHeight,
		int bitRate,
		int scaledWidth,
	    int scaledHeight
		) :
		gotOutput(0), currentFrame(0), 
		inputWidth(inputWidth), inputHeight(inputHeight),
		scaledWidth(scaledWidth), scaledHeight(scaledHeight)
{
	codec = avcodec_find_encoder(codec_id);
	if (codec == NULL) {
		throw std::runtime_error("Codec not found\n");
	}

	codecContext = avcodec_alloc_context3(codec);
	if (codecContext == NULL) {
		throw std::runtime_error("Could not allocate video codec context\n");
	}
	int destWidth = inputWidth;
	int destHeight = inputHeight;
	if(scaledWidth != -1) {
		destWidth = scaledWidth;
	}
	if(scaledHeight != -1) {
		destHeight = scaledHeight;
	}
	codecContext->bit_rate = bitRate;
	codecContext->width = destWidth;
	codecContext->height = destHeight;
	codecContext->time_base = (AVRational){1, 25};
	codecContext->gop_size = 4;
	codecContext->max_b_frames = 1;
	codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

	if (codec_id == AV_CODEC_ID_H264) {
		av_opt_set(codecContext->priv_data, "preset", "slow", 0);
	}

	if (avcodec_open2(codecContext, codec, NULL) < 0) {
		av_free(codecContext);
		throw std::runtime_error("Could not open codec\n");
	}

	frame = av_frame_alloc();
	if (frame == NULL) {
		avcodec_close(codecContext);
		av_free(codecContext);
		throw std::runtime_error("Could not allocate video frame\n");
	}
	frame->format = codecContext->pix_fmt;
	frame->width = codecContext->width;
	frame->height = codecContext->height;

	int ret = av_image_alloc(frame->data, frame->linesize, codecContext->width,
			codecContext->height, codecContext->pix_fmt, 32);
	if (ret < 0) {
		avcodec_close(codecContext);
		av_free(codecContext);
		throw std::runtime_error("Could not allocate raw picture buffer\n");
	}
	ctx = sws_getContext(inputWidth, inputHeight,
			PIX_FMT_BGRA,
			destWidth, destHeight,
			AV_PIX_FMT_YUV420P,
			0, 0, 0, 0);

}

Frame * FrameEncoder::encodeFrame(XImage * image)
{
	av_init_packet(&pkt);
	pkt.data = NULL; // packet data will be allocated by the encoder
	pkt.size = 0;
	uint8_t * inData[1] = {(uint8_t*) image->data}; // RGBA32 have one plane
	int inLinesize[1] = {image->bytes_per_line}; // RGBA stride
	sws_scale(ctx, inData, inLinesize, 0, inputHeight, frame->data, frame->linesize);
	frame->pts = currentFrame++;
	int ret = avcodec_encode_video2(codecContext, &pkt, frame, &gotOutput);
	if (ret < 0) {
		clean();
		throw std::runtime_error("Error encoding frame\n");
	}
	if(gotOutput) {
		Frame *frm = new Frame;
		frm->packet = &pkt;
		return frm;
	}
	return NULL;
}

Frame * FrameEncoder::getDelayedFrames() {
	if(gotOutput) {
		int ret = avcodec_encode_video2(codecContext, &pkt, NULL, &gotOutput);
		if(ret < 0) {
			clean();
			throw std::runtime_error("Error encoding frame\n");
		}
		if(gotOutput) {
			Frame *frm = new Frame;
			frm->packet = &pkt;
			return frm;
		}
	}
	return NULL;
}

void FrameEncoder::recycleFrame(Frame * frame) {
	av_free_packet(frame->packet);
	delete frame;
}


FrameEncoder::FrameEncoder(const FrameEncoder& orig)
{
	throw std::runtime_error("No coping allowed");
}

FrameEncoder& FrameEncoder::operator=(const FrameEncoder& orig)
{
	throw std::runtime_error("No coping allowed");
}

FrameEncoder::~FrameEncoder()
{
	clean();
}

void FrameEncoder::clean()
{
	avcodec_close(codecContext);
	av_free(codecContext);
	av_freep(&frame->data[0]);
	av_frame_free(&frame);
}

FFMpegEnv FrameEncoder::env;