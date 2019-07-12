#include <iostream>
#include <stdio.h>
#include "nvcodec.h"
#include "DemuxFile.h"

using namespace AvDemuxFile;
int main()
{
    nvcodechdl_t decodehdl;
    if (0 != nvcodec_vdec_init(&decodehdl, 0, 0, 0, 1080, 720)) {
        printf("init decoder fail! \n");
        return -1;
    }

    BaseAvFile *pVideoFile = nullptr;
    pVideoFile = new H264File();
  
    if(!pVideoFile->open("test.h264")) {
        printf("Open video file failed.\n");
        return 0;
    }
    int bufSize = 500000;
    uint8_t *frameBuf = new uint8_t[bufSize];

    while (1) {
        bool bEndOfFrame;
        int frameSize = pVideoFile->readFrame((char*)frameBuf, bufSize, &bEndOfFrame);
        if(frameSize > 0) {
            printf("insert frame, size:%d \n", frameSize);
            nvcodec_vdec_insert_frame(decodehdl, frameBuf, frameSize, 0);
        }
    }
    
    return 0;
}
