#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "ServiceRegistrer.h"
#include "BluetoothServerSocket.h"
#include "BluetoothSocket.h"
#include "Consumer.h"
#include "Packet.h"
#include "Screen.h"
#include "FrameEncoder.h"

using namespace std;
using namespace bluetooth;

void
registerService(const BluetoothServerSocket &bsock)
{

	unsigned int bluuid[4] = {0x0, 0x0, 0x0, 0x0000abcd};

	ServiceRecord sr = ServiceRecord(bluuid,
			"Remote control server",
			"Remote control server for Linux machines",
			"RCS Daemon",
			bsock.port());

	ServiceRegistrer::registerService(sr);

}

int main()
{

	//BluetoothServerSocket bsrv;
	//registerService(bsrv);
	vkm::Screen screen = vkm::Screen::getDefaultScreen();
	FrameEncoder frameEncoder(AV_CODEC_ID_MPEG2VIDEO, screen.width(), screen.height(), 1258291, 1024, 576);

	FILE *outputFile;
	outputFile = fopen("sample.mpeg", "wb");
	if (!outputFile)
    {
        fprintf(stderr, "Could not open %s\n", "sample.mpeg");
        exit(1);
    }
	
	for (int i = 0; i < 100; i++) {
		XImage *img = screen.captureScreen();
		Frame *frame = frameEncoder.encodeFrame(img);
		if (frame != NULL) {
			fwrite(frame->packet->data, 1, frame->packet->size, outputFile);
			frameEncoder.recycleFrame(frame);
		}
		screen.recycleScreen(img);
		usleep(1000*10);
	}
	
	while(Frame * frame = frameEncoder.getDelayedFrames()) {
		fwrite(frame->packet->data, 1, frame->packet->size, outputFile);
		frameEncoder.recycleFrame(frame);
	}
	
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	fwrite(endcode, 1, sizeof(endcode), outputFile);
    fclose(outputFile);
}