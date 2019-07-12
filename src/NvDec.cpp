#include "NvDec.h"
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#define gettid() syscall(SYS_gettid)

#include "nvcodec_type.h"
#include "common.h"
#include "bufex_nv.h"



///////////////////////////////////////////////////////////////////////////////////////////
void nvwrite_file(char *file, unsigned char *data, int datasize, int overwrite_flag)
{
	FILE* fp;

	fp = fopen(file, overwrite_flag ? "wb" : "ab");
	if (NULL == fp) {
		return;
	}

	fwrite(data, 1, datasize,fp);

	fclose(fp);
}


///////////////////////////////////////////////////////////////////////////////////////////
CNvDec::CNvDec()
{
    m_devno = -1;

    m_initflag = false;

    m_cuCtx = NULL;
    m_cuParser = NULL;
    m_cuDecoder = NULL;
    m_cuCodecID = cudaVideoCodec_NumCodecs;

    m_iPicWidth = -1;
    m_iPicHeight = -1;
    m_fps = 0.0f;
    m_ofmt = NVCODEC_OUTFMT_I420;
    m_iPicWidth_out = -1;
    m_iPicHeight_out = -1;
    m_iPicWidth_out_max = -1;
    m_iPicHeight_out_max = -1;

    m_iFrameIdx = 0;
    m_outframe = NULL;
    m_outframe_size = 0;
    m_outframe_tmp = NULL;
    m_outframe_tmp_size = 0;
    memset(m_outbufs, 0x00, sizeof(m_outbufs));
    m_iCurOutBufIdx = 0;

    m_pPicQueue = NULL;

    m_iInternalErrorCode = NVCODEC_SUCCESS;
}

CNvDec::~CNvDec()
{
}

cudaVideoCodec CNvDec::NvCodecID2cuvidCodecID(en_nvcodecid_t nvcodecid)
{
    cudaVideoCodec ret = cudaVideoCodec_NumCodecs;

    switch (nvcodecid) {
        case NVCODEC_ID_MPEG1:
            ret = cudaVideoCodec_MPEG1;
            break;

        case NVCODEC_ID_MPEG2:
            ret = cudaVideoCodec_MPEG2;
            break;

        case NVCODEC_ID_MPEG4:
            ret = cudaVideoCodec_MPEG4;
            break;

        case NVCODEC_ID_VC1:
            ret = cudaVideoCodec_VC1;
            break;

        case NVCODEC_ID_H264:
            ret = cudaVideoCodec_H264;
            break;

        case NVCODEC_ID_JPEG:
            ret = cudaVideoCodec_JPEG;
            break;

        case NVCODEC_ID_H264_SVC:
            ret = cudaVideoCodec_H264_SVC;
            break;

        case NVCODEC_ID_H264_MVC:
            ret = cudaVideoCodec_H264_MVC;
            break;

        case NVCODEC_ID_HEVC:
            ret = cudaVideoCodec_HEVC;
            break;

        case NVCODEC_ID_VP8:
            ret = cudaVideoCodec_VP8;
            break;

        case NVCODEC_ID_VP9:
            ret = cudaVideoCodec_VP9;
            break;

        default:
            ret = cudaVideoCodec_NumCodecs;
            break;
    }

    return ret;
}

int CNvDec::init(int id, int devno, int ofmt, int w_out, int h_out)
{
    CUresult ret;
    int deviceCnt;
    char *errstr;
    int attr;
    CUVIDPARSERPARAMS parserParams;
    CUdevice cuDev;

    double start;
    double end;

    if (m_initflag) {
        return NVCODEC_SUCCESS;
    }

    if (!ISVALID_NVCODEC_OUTFMT(ofmt)) {
        return NVCODEC_ERR_INVALID_VALUE;
    }
    m_ofmt = ofmt;

    if ((w_out > 0 && (w_out%4) != 0) || (h_out > 0 && (h_out%4) != 0)) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    if (w_out > 0 && h_out > 0) {
        m_iPicWidth_out = w_out;
        m_iPicHeight_out = h_out;
    }
    else {
        m_iPicWidth_out = -1;
        m_iPicHeight_out = -1;
    }

    m_cuCodecID = NvCodecID2cuvidCodecID((en_nvcodecid_t)id);
    if (m_cuCodecID == cudaVideoCodec_NumCodecs) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    m_devno = devno;

    ret = cuDeviceGetCount(&deviceCnt);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }
    printf("cu device count: %d \n", deviceCnt);
    if (devno < 0 || devno >= deviceCnt) {
        devno = devno < 0 ? 0 : deviceCnt - 1;
    }

    char b[1024];
    cuDeviceGetName(b, sizeof(b), 0);
    printf("#0:cu device name :%s \n", b);
    memset(b, 0, sizeof(b));
    cuDeviceGetName(b, sizeof(b), 1);
    printf("#1: cu device name :%s \n", b);
    
    ret = cuDeviceGet(&cuDev, devno);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

    ret = cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, cuDev);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

#if 1 // ===
	ret = cuCtxCreate(&m_cuCtx, CU_CTX_MAP_HOST, cuDev);
#else
    ret = cuCtxCreate(&m_cuCtx, CU_CTX_SCHED_BLOCKING_SYNC, cuDev);
#endif
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

    memset(&parserParams, 0x00, sizeof(parserParams));
    parserParams.CodecType = m_cuCodecID;
    parserParams.ulMaxNumDecodeSurfaces = MAX_DECODE_SURFACES;
    parserParams.ulMaxDisplayDelay = 0;
    parserParams.pUserData = this;
    parserParams.pfnSequenceCallback = HandleVideoSequenceProc;
    parserParams.pfnDecodePicture = HandlePictureDecodeProc;
    parserParams.pfnDisplayPicture = HandlePictureDisplayProc;
    parserParams.ulErrorThreshold = 0;

    ret = cuvidCreateVideoParser(&m_cuParser, &parserParams);
    if (ret != CUDA_SUCCESS) {

        return ret;
    }


    m_pPicQueue = new CPicQueue(MAX_DECODE_SURFACES);

    m_initflag = true;

    return NVCODEC_SUCCESS;
}

int CNvDec::init2(int id, int devno, int ofmt, int w_out_max, int h_out_max)
{
    CUresult ret;
    int deviceCnt;
    char *errstr;
    int attr;
    CUVIDPARSERPARAMS parserParams;
    CUdevice cuDev;

    double start;
    double end;

    if (m_initflag) {
        return NVCODEC_SUCCESS;
    }

    if (!ISVALID_NVCODEC_OUTFMT(ofmt)) {
        return NVCODEC_ERR_INVALID_VALUE;
    }
    m_ofmt = ofmt;

    if ((w_out_max > 0 && (w_out_max%4) != 0) || (h_out_max > 0 && (h_out_max%4) != 0)) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    if (w_out_max > 0 && h_out_max > 0) {
        m_iPicWidth_out_max = w_out_max;
        m_iPicHeight_out_max = h_out_max;
    }
    else {
        m_iPicWidth_out_max = -1;
        m_iPicHeight_out_max = -1;
    }

    m_cuCodecID = NvCodecID2cuvidCodecID((en_nvcodecid_t)id);
    if (m_cuCodecID == cudaVideoCodec_NumCodecs) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    m_devno = devno;

    ret = cuDeviceGetCount(&deviceCnt);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }
    if (devno < 0 || devno >= deviceCnt) {
        devno = devno < 0 ? 0 : deviceCnt - 1;
    }

    ret = cuDeviceGet(&cuDev, devno);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

    ret = cuDeviceGetAttribute(&attr, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, cuDev);
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

#if 1 // ===
    ret = cuCtxCreate(&m_cuCtx, CU_CTX_MAP_HOST, cuDev);
#else
    ret = cuCtxCreate(&m_cuCtx, CU_CTX_SCHED_BLOCKING_SYNC, cuDev);
#endif
    if (ret != CUDA_SUCCESS)
    {
        return NVCODEC_ERR_NO_DEVICE;
    }

    memset(&parserParams, 0x00, sizeof(parserParams));
    parserParams.CodecType = m_cuCodecID;
    parserParams.ulMaxNumDecodeSurfaces = MAX_DECODE_SURFACES;
    parserParams.ulMaxDisplayDelay = 0;
    parserParams.pUserData = this;
    parserParams.pfnSequenceCallback = HandleVideoSequenceProc;
    parserParams.pfnDecodePicture = HandlePictureDecodeProc;
    parserParams.pfnDisplayPicture = HandlePictureDisplayProc;
    parserParams.ulErrorThreshold = 0;

    ret = cuvidCreateVideoParser(&m_cuParser, &parserParams);
    if (ret != CUDA_SUCCESS) {

        return ret;
    }


    m_pPicQueue = new CPicQueue(MAX_DECODE_SURFACES);

    m_initflag = true;

    return NVCODEC_SUCCESS;
}


void CNvDec::deinit()
{
    int i = 0;
    void *p;

    if (!m_initflag) {
        return;
    }

    if (m_pPicQueue != NULL) {
        m_pPicQueue->SetTermFlag();
    }


    if (m_cuParser) {
        cuvidDestroyVideoParser(m_cuParser);
        m_cuParser = NULL;
    }


    if (m_cuDecoder) {

        cuvidDestroyDecoder(m_cuDecoder);
        m_cuDecoder = NULL;
    }

    if (m_outframe != NULL) {
        cuMemFreeHost(m_outframe);
        m_outframe= NULL;
    }
    if (m_outframe_tmp != NULL) {
        cuMemFreeHost(m_outframe_tmp);
        m_outframe_tmp= NULL;
    }

    for (i = 0; i < sizeof(m_outbufs)/sizeof(m_outbufs[0]); i++) {
        if (NULL == m_outbufs[i]) {
            bufex_nvfree(&m_outbufs[i]);
        }
    }

    if (m_cuCtx != NULL) {
        cuCtxDestroy(m_cuCtx);
        m_cuCtx = NULL;
    }

    if (m_pPicQueue != NULL) {
        delete m_pPicQueue;
        m_pPicQueue = NULL;
    }
}

int CNvDec::insert_frame(void *pkt, int pktsize, int64_t ts)
{
    CUVIDSOURCEDATAPACKET cuPkt = {0};
    CUresult ret;

    if (!m_initflag) {
        return NVCODEC_ERR_NOT_INITIALIZED;
    }

    cuPkt.payload = (unsigned char*)pkt;
    cuPkt.payload_size = pktsize;
    if (ts >=0) {
        cuPkt.flags |= CUVID_PKT_TIMESTAMP;
        cuPkt.timestamp = ts;
    }

    ret = cuvidParseVideoData(m_cuParser, &cuPkt);
    if (ret != CUDA_SUCCESS) {
        return ret;
    }

    return m_iInternalErrorCode;
}


int CNvDec::get_frame(bufex_t **frame, int *w, int *h, double *fps, int64_t *ts, int *got_frame)
{
    CUresult ret;

    if (NULL == got_frame) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    if (!m_initflag) {
        return NVCODEC_ERR_NOT_INITIALIZED;
    }

    CUcontext cuCtx;
    cuCtxGetCurrent(&cuCtx);
    if (NULL == cuCtx) {
        cuCtxSetCurrent(m_cuCtx);
    }

    GetFrame(frame, w, h, m_ofmt, got_frame);

    if (*got_frame) {
        if (fps != NULL) {
            *fps = m_fps;
        }

        if (ts != NULL) {
            *ts = BUFEX_IFIELD(*frame, ts);
        }
    }

    return m_iInternalErrorCode;
}


int CUDAAPI CNvDec::HandleVideoSequenceProc(void *pUserData, CUVIDEOFORMAT *pVideoFmt)
{
    return ((CNvDec *)pUserData)->HandleVideoSequence(pVideoFmt);
}

int CUDAAPI CNvDec::HandlePictureDecodeProc(void *pUserData, CUVIDPICPARAMS *pPicParams)
{
    return ((CNvDec *)pUserData)->HandlePictureDecode(pPicParams);
}

int CUDAAPI CNvDec::HandlePictureDisplayProc(void *pUserData, CUVIDPARSERDISPINFO *pDispInfo)
{
    return ((CNvDec *)pUserData)->HandlePictureDisplay(pDispInfo);
}

int CNvDec::HandleVideoSequence(CUVIDEOFORMAT *pVideoFmt) {
    CUresult ret;
    char *errstr;
    CUVIDDECODECREATEINFO cinfo = { 0 };


    m_iPicWidth = pVideoFmt->display_area.right - pVideoFmt->display_area.left;
    m_iPicHeight = pVideoFmt->display_area.bottom - pVideoFmt->display_area.top;
    if (pVideoFmt->frame_rate.numerator && pVideoFmt->frame_rate.denominator) {
        m_fps = (double)pVideoFmt->frame_rate.numerator / (double)pVideoFmt->frame_rate.denominator;
    }
    m_cuCodecID = pVideoFmt->codec;

    if (m_iPicWidth_out <= 0 || m_iPicHeight_out <= 0) {
        if (m_iPicWidth_out_max > 0 && m_iPicHeight_out_max > 0 &&
            (m_iPicWidth_out_max < m_iPicWidth || m_iPicHeight_out_max < m_iPicHeight)) {
            m_iPicWidth_out = m_iPicWidth_out_max;
            m_iPicHeight_out = m_iPicHeight_out_max;
        }
        else {
            m_iPicWidth_out = m_iPicWidth;
            m_iPicHeight_out = m_iPicHeight;
        }
    }

    cinfo.CodecType = pVideoFmt->codec;
    cinfo.ulWidth = pVideoFmt->coded_width;
    cinfo.ulHeight = pVideoFmt->coded_height;
    cinfo.ChromaFormat = pVideoFmt->chroma_format;
    cinfo.OutputFormat = cudaVideoSurfaceFormat_NV12;
    cinfo.DeinterlaceMode = cudaVideoDeinterlaceMode_Adaptive;
    cinfo.ulTargetWidth = m_iPicWidth_out;
    cinfo.ulTargetHeight = m_iPicHeight_out;
    cinfo.ulCreationFlags = cudaVideoCreate_PreferCUVID;
    cinfo.ulNumDecodeSurfaces = MAX_DECODE_SURFACES;
    cinfo.ulNumOutputSurfaces = MAX_OUTPUT_SURFACES;


    cuCtxPushCurrent(m_cuCtx);
    ret = cuvidCreateDecoder(&m_cuDecoder, &cinfo);
    if (ret != CUDA_SUCCESS) {

        m_iInternalErrorCode = ret;
        return -1;
    }
    ret = cuCtxPopCurrent(NULL);


    return 1;
}

int CNvDec::HandlePictureDecode(CUVIDPICPARAMS *pPicParams) {
    CUresult ret;
    char *errstr;

    //dbgprintf_libnvcodec("XXX fun=%s\n", __PRETTY_FUNCTION__);

    if (!m_cuDecoder) {
        return 0;
    }

    if (!m_pPicQueue->WaitUntilPicAvailable(pPicParams->CurrPicIdx)) {
        return 0;
    }


    cuvidDecodePicture(m_cuDecoder, pPicParams);


    return 1;
}

int CNvDec::HandlePictureDisplay(CUVIDPARSERDISPINFO *pDispInfo)
{
    m_pPicQueue->EnQueue(pDispInfo);

    return 1;
}

void CNvDec::cuLock()
{
}

void CNvDec::cuUnlock()
{

}

bufex_t* CNvDec::GetAFreeOutBuf(int size)
{
    int ret;

    if (NULL == m_outbufs[m_iCurOutBufIdx]) {
        ret = bufex_nvalloc(&m_outbufs[m_iCurOutBufIdx], size, m_devno);
        if (ret != NVCODEC_SUCCESS) {
            return NULL;
        }
    }
    else if (BUFEX_IFIELD(m_outbufs[m_iCurOutBufIdx],memsize) < size) {
        ret = bufex_nvresize(&m_outbufs[m_iCurOutBufIdx], size);
        if (ret != NVCODEC_SUCCESS) {
            return NULL;
        }
    }

    return m_outbufs[m_iCurOutBufIdx];
}

int CNvDec::GetOutBufSizeByFmt(int w,int h,int ofmt)
{
    int size = 0;

    switch (ofmt) {
    case NVCODEC_OUTFMT_NV12:
    case NVCODEC_OUTFMT_I420:
        size = w * h * 3 /2 ;
        break;

    case NVCODEC_OUTFMT_BGR:
        size = w * h * 3;
        break;
    };

    return size;
}

void CNvDec::CopyFrame2OutBuf(unsigned char *nv12, int pitch, int w, int h, int ofmt, bufex_t *outbuf)
{
    int size;

    switch (ofmt) {
    case NVCODEC_OUTFMT_NV12:
        size = w * h * 3 / 2;
        cuMemcpy((CUdeviceptr)BUFEX_IFIELD(outbuf, mem), (CUdeviceptr)nv12, size);
        BUFEX_IFIELD(outbuf, datasize) = size;
        BUFEX_IFIELD(outbuf, width) = w;
        BUFEX_IFIELD(outbuf, height) = h;
        outbuf->datasize = size;
        break;

    case NVCODEC_OUTFMT_I420:
        size = w * h * 3 / 2;
        nv12_to_yuv420p(nv12, pitch, w, h, (unsigned char*)BUFEX_IFIELD(outbuf, mem));
        BUFEX_IFIELD(outbuf, datasize) = size;
        BUFEX_IFIELD(outbuf, width) = w;
        BUFEX_IFIELD(outbuf, height) = h;
        outbuf->datasize = size;

        break;
    };
}

void CNvDec::nv12_to_yuv420p(unsigned char* nv12, int pitch, int w, int h, unsigned char* yuv420p)
{
    int i, j;
    int pitch_half = pitch >> 1;
    int w_half = w >> 1;
    int h_half = h >> 1;
    int size = w * h;
    int size_half = size >> 1;

    unsigned char* src_y = nv12;
    unsigned char* src_uv = nv12 + pitch * h;
    int src_off;

    unsigned char* dst_y = yuv420p;
    unsigned char* dst_u = yuv420p + w * h;
    unsigned char* dst_v = yuv420p + w * h * 5 / 4;
    int dst_off;

    double start;
    double end;

    if (pitch == w) {
        size = w * h;

        cuMemcpy((CUdeviceptr)dst_y, (CUdeviceptr)src_y, size);

        for (j = 0, i = 0; j < size_half; j += 2, i++)
        {
            dst_u[i] = src_uv[j];
            dst_v[i] = src_uv[j+1];
        }
    }
    else {
        for (i = 0; i < h; i++) {
            cuMemcpy((CUdeviceptr)(dst_y + i * w), (CUdeviceptr)(src_y + i*pitch), w);
        }

        for (i = 0; i < h_half; i++) {
            src_off = i*pitch;
            dst_off = i*w_half;
            for (j = 0; j < w_half; j++) {
                dst_u[dst_off+j] = src_uv[src_off+j*2];
                dst_v[dst_off+j] = src_uv[src_off+j*2+1];;
            }
        }
    }
}

bool CNvDec::GetFrame(bufex_t **frame, int *w, int *h, int ofmt, int *got_frame)
{
    CUVIDPARSERDISPINFO *pDispInfo;

    CUVIDPROCPARAMS procparams = {0};
    CUdeviceptr cuPicDevPtr = NULL;
    CUresult ret;
    bufex_t *p_outbuf;
    int size;
    char *errstr;
    int iPicPitch;
    int64_t ts;

    double start;
    double end;

    *got_frame = 0;

    if (!m_pPicQueue->DeQueue(&pDispInfo)) {
        return false;
    }

    ts = pDispInfo->timestamp;

    procparams.progressive_frame = pDispInfo->progressive_frame;
    procparams.second_field = 0;
    procparams.top_field_first = pDispInfo->top_field_first;
    procparams.unpaired_field = pDispInfo->progressive_frame == 1;

    ret = cuvidMapVideoFrame(m_cuDecoder, pDispInfo->picture_index, &cuPicDevPtr, (unsigned int*)&iPicPitch, &procparams);
    if (ret != CUDA_SUCCESS || NULL == cuPicDevPtr || iPicPitch <= 0) {
        m_pPicQueue->ReleasePic(pDispInfo);
        return false;
    }

    if (NULL == m_outframe) {
        m_outframe_size = iPicPitch * m_iPicHeight_out* 3 / 2;
        ret = cuMemAllocHost((void**)&m_outframe, m_outframe_size);
        if (ret != CUDA_SUCCESS) {
            cuvidUnmapVideoFrame(m_cuDecoder, cuPicDevPtr);
            m_pPicQueue->ReleasePic(pDispInfo);
            return false;
        }
    }
    cuMemcpyDtoH(m_outframe, cuPicDevPtr, m_outframe_size);
    cuvidUnmapVideoFrame(m_cuDecoder, cuPicDevPtr);

    m_pPicQueue->ReleasePic(pDispInfo);

    size = GetOutBufSizeByFmt(m_iPicWidth_out, m_iPicHeight_out, ofmt);
    p_outbuf = GetAFreeOutBuf(size);
    if (NULL == p_outbuf) {
        return false;
    }

    if (NVCODEC_OUTFMT_BGR == ofmt && NULL == m_outframe_tmp) {
        m_outframe_tmp_size = m_iPicWidth_out * m_iPicHeight_out* 3 / 2;
        ret = cuMemAllocHost((void**)&m_outframe_tmp, m_outframe_tmp_size);
        if (ret != CUDA_SUCCESS) {
            return false;
        }
    }

    CopyFrame2OutBuf((unsigned char*)m_outframe, iPicPitch, m_iPicWidth_out, m_iPicHeight_out, ofmt, p_outbuf);
    BUFEX_IFIELD(p_outbuf, ts) = ts;

    *frame= p_outbuf;
    if (w != NULL) {
        *w = m_iPicWidth;
    }
    if (h != NULL) {
        *h = m_iPicHeight;
    }

    m_iCurOutBufIdx = (m_iCurOutBufIdx + 1) % DECOUTBUF_MAX;

    *got_frame = 1;

    return true;
}

