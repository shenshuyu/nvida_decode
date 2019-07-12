#include <iostream>
#include <stdio.h>
#include <thread>
#include "nvcodec.h"
#include "DemuxFile.h"

using namespace AvDemuxFile;

void GetFrameTh(nvcodechdl_t hdl)
{
    bufex_t *pRawdata = new(bufex_t);
    int rawW, rawH;
    double rawFps;
    int64_t rawTs;
    int gotflag;
    while (true) {
        if (0 == nvcodec_vdec_get_frame(hdl, &pRawdata, &rawW, &rawH, &rawFps, &rawTs, &gotflag)) {
            std::cout << "```````````````````GotRawFrame: " << ", W= " << rawW << ", H= " << rawH << ", fps:" << rawFps << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    nvcodechdl_t decodehdl;
    if (0 != nvcodec_vdec_init2(&decodehdl, NVCODEC_ID_H264, 0, 0, 1920, 1080)) {
        printf("init decoder fail! \n");
        return -1;
    }

    BaseAvFile *pVideoFile = nullptr;
    pVideoFile = new H264File();
  
    if(!pVideoFile->open("test.h264")) {
        printf("Open video file failed.\n");
        return 0;
    }
    int bufSize = 1024 * 1024;
    uint8_t *frameBuf = new uint8_t[bufSize];

    std::thread getframe = std::thread(GetFrameTh, decodehdl);
    getframe.detach();

    int64_t ts;
    while (true) {
        bool bEndOfFrame;
        int frameSize = pVideoFile->readFrame((char*)frameBuf, bufSize, &bEndOfFrame);
        if(frameSize > 0) {
            printf("insert frame, size:%d \n", frameSize);
            nvcodec_vdec_insert_frame(decodehdl, frameBuf, frameSize, ts+=40*1000);
        }
    }


    
    return 0;
}
