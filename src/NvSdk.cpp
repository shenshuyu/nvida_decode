#include "NvSdk.h"
//
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
//
#include "dynlink_cuda.h"
#include "dynlink_nvcuvid.h"

#include "common.h"
#include "nvcodec_type.h"

CNvSdk gNvSdk;

//////////////////////////////////////////////////////////////
CNvSdk::CNvSdk()
{
    m_bLoadFlag = LoadSdk();
}

CNvSdk::~CNvSdk()
{

}

bool CNvSdk::LoadSdk()
{
    CUresult ret;
    char *errstr;
    int i;
    CUdevice cuDev;
    char cDevName[256] = {0};

    double start;
    double end;

    ret = cuInit(0, __CUDA_API_VERSION, NULL);
    if (ret != CUDA_SUCCESS)
    {
        return false;
    }

    ret = cuvidInit(0);
    if (ret != CUDA_SUCCESS)
    {
        return false;
    }

    ret = cuDeviceGetCount(&m_devnum);
    if (ret != CUDA_SUCCESS || !m_devnum) {
        return false;
    }

    for (i = 0; i < m_devnum; i++) {
        cuDeviceGet(&cuDev, i);
        cuDeviceGetName(cDevName, sizeof(cDevName), cuDev);
    }

    return true;
}

