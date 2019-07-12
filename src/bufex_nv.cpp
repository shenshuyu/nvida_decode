#include "bufex_nv.h"
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
#if 0
#include <nvcuvid.h>
#include <cuda_runtime.h>
#else
#include "dynlink_nvcuvid.h"
#include "dynlink_cuda.h"
#endif
//
#include "common.h"
#include "nvcodec_type.h"
#include <stdio.h>

///////////////////////////////////////////////////////////

int bufex_nvalloc(bufex_t **pp_bufex, int size, int devno)
{
    bufex_t *p_bufex;
    uint8_t *p = NULL;
    CUdevice cuDev;
    CUresult ret;
    char *errstr;
    int attr;

    if (NULL == pp_bufex || size <= 0) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    p = (uint8_t*)malloc(BUFEX_SIZE + BUFEX_INNER_SIZE);
    if (NULL == p) {
        return NVCODEC_ERR_OUT_OF_MEMORY;
    }

    memset(p, 0x00, BUFEX_SIZE + BUFEX_INNER_SIZE);
    p_bufex = (bufex_t*)p;
    p_bufex->data = p + BUFEX_SIZE;

    ret = cuDeviceGet(&cuDev, devno);
    if (ret != CUDA_SUCCESS) {
        free(p_bufex);
        return NVCODEC_ERR_NO_DEVICE;
    }

    ret = cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, cuDev);
    if (ret != CUDA_SUCCESS) {
        return NVCODEC_ERR_NO_DEVICE;
    }
    if (attr) {
        ret = cuMemHostAlloc((void**)&p, size, CU_MEMHOSTREGISTER_DEVICEMAP);
        if (ret != CUDA_SUCCESS) {
            free(p_bufex);
            return NVCODEC_ERR_OUT_OF_MEMORY;
        }
        BUFEX_IFIELD(p_bufex, mem) = p;
        BUFEX_IFIELD(p_bufex, memsize) = size;
        BUFEX_IFIELD(p_bufex, devno) = devno;
        BUFEX_IFIELD(p_bufex, mapflag) = 1;

        cuMemHostGetDevicePointer((CUdeviceptr*)&BUFEX_IFIELD(p_bufex, reserved), p, 0);
    }
    else {
        ret = cuMemAllocHost((void**)&p, size);
         if (ret != CUDA_SUCCESS) {
            return NVCODEC_ERR_OUT_OF_MEMORY;
        }
        BUFEX_IFIELD(p_bufex, mem) = p;
        BUFEX_IFIELD(p_bufex, memsize) = size;
        BUFEX_IFIELD(p_bufex, devno) = devno;

        ret = cuMemAlloc((CUdeviceptr*)&p, size);
        if (ret != CUDA_SUCCESS) {
            return NVCODEC_ERR_OUT_OF_MEMORY;
        }
        BUFEX_IFIELD(p_bufex, reserved) = p;
    }

    cuCtxGetCurrent((CUcontext*)&BUFEX_IFIELD(p_bufex, ctx));

    *pp_bufex = p_bufex;

    return NVCODEC_SUCCESS;
}

void bufex_nvfree(bufex_t **pp_bufex)
{
    if (NULL == pp_bufex || NULL == *pp_bufex) {
        return;
    }

    if (BUFEX_IFIELD(*pp_bufex, mem) != NULL) {
        cuMemFreeHost(BUFEX_IFIELD(*pp_bufex, mem));
        BUFEX_IFIELD(*pp_bufex, mem) = NULL;
    }
}

int bufex_nvresize(bufex_t **pp_bufex, int newsize)
{
    int devno;

    if (NULL == pp_bufex || newsize <= 0 ) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    devno = BUFEX_IFIELD(*pp_bufex, devno);
    bufex_nvfree(pp_bufex);

    return bufex_nvalloc(pp_bufex, newsize, devno);
}

int bufex_nvread(bufex_t *p_bufex, void *data, int datasize, mem_type_t datatype)
{
    CUcontext cuDstCtx;
    CUcontext cuCtx;
    CUresult ret;
    char *errstr;

    if (NULL == p_bufex || NULL == data || datasize < p_bufex->datasize ||
        BUFEX_IFIELD(p_bufex, devno) < 0) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    switch (datatype) {
    case MEM_TYPE_CPU:
#if 1
        cuCtxGetCurrent(&cuCtx);
        if (NULL == cuCtx) {
            cuCtxSetCurrent((CUcontext)BUFEX_IFIELD(p_bufex, ctx));
        }
 #endif
        ret = cuMemcpy((CUdeviceptr)data, (CUdeviceptr)BUFEX_IFIELD(p_bufex,mem), BUFEX_IFIELD(p_bufex,datasize));
        if (ret != CUDA_SUCCESS) {
        }
        break;

    case MEM_TYPE_GPU:
        cuCtxGetCurrent(&cuDstCtx);
        cuCtxPushCurrent((CUcontext)BUFEX_IFIELD(p_bufex, ctx));
        if (NULL == cuDstCtx || cuDstCtx == BUFEX_IFIELD(p_bufex, ctx)) {
            cuMemcpyDtoD((CUdeviceptr)data, (CUdeviceptr)BUFEX_IFIELD(p_bufex,reserved), BUFEX_IFIELD(p_bufex,datasize));
        }
        else {
            cuMemcpyPeer((CUdeviceptr)data, cuDstCtx, (CUdeviceptr)BUFEX_IFIELD(p_bufex,reserved),
                (CUcontext)BUFEX_IFIELD(p_bufex,ctx), BUFEX_IFIELD(p_bufex,datasize));
        }
        cuCtxPopCurrent(NULL);
        break;

    default:
        return NVCODEC_ERR_INVALID_VALUE;
    }

    return NVCODEC_SUCCESS;
}

