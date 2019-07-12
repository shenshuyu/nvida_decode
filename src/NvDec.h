/*************************************************************************
** file: NvDec.h
** desc:
*************************************************************************/
#ifndef _NVDEC_H
#define _NVDEC_H
//
#include <stdint.h>
#include <assert.h>
#include <pthread.h>
//
#include "dynlink_cuda.h"
#include "dynlink_nvcuvid.h"
//
#include "bufex_nv.h"
#include "nvcodec_type.h"
#include "PicQueue.h"

#define DECOUTBUF_MAX       16
#define MAX_DECODE_SURFACES  DECOUTBUF_MAX
#define MAX_OUTPUT_SURFACES 1


///////////////////////////////////////////////////////////////////////////
class CNvDec {
public:
	CNvDec();
    ~CNvDec();

    int init(int id, int devno, int ofmt, int w_out, int h_out);
    int init2(int id, int devno, int ofmt, int w_out_max, int h_out_max);
    void deinit();

    int insert_frame(void *pkt, int pktsize, int64_t ts);
    int get_frame(bufex_t **frame, int *w, int *h, double *fps, int64_t *ts, int *got_frame);

    int GetWidth() { return m_iPicWidth; }
    int GetHeight() { return m_iPicHeight; }

    int GetDevNo() { return m_devno; };

private:
    cudaVideoCodec NvCodecID2cuvidCodecID(en_nvcodecid_t nvcodecid);

    static int CUDAAPI HandleVideoSequenceProc(void *pUserData, CUVIDEOFORMAT *pVideoFmt);
    static int CUDAAPI HandlePictureDecodeProc(void *pUserData, CUVIDPICPARAMS *pPicParams);
    static int CUDAAPI HandlePictureDisplayProc(void *pUserData, CUVIDPARSERDISPINFO *pDispInfo);
    int HandleVideoSequence(CUVIDEOFORMAT *pVideoFmt);
    int HandlePictureDecode(CUVIDPICPARAMS *pPicParams);
    int HandlePictureDisplay(CUVIDPARSERDISPINFO *pDispInfo);

    void cuLock();
    void cuUnlock();

    bufex_t* GetAFreeOutBuf(int size);
    int GetOutBufSizeByFmt(int w, int h, int ofmt);
    void CopyFrame2OutBuf(unsigned char *nv12, int pitch, int w, int h, int ofmt, bufex_t *outbuf);

    static void nv12_to_yuv420p(unsigned char* nv12, int pitch, int w, int h, unsigned char* yuv420p);

    bool GetFrame(bufex_t **frame, int *w, int *h, int ofmt, int *got_frame);

private:
    int                 m_devno;

    bool                m_initflag;

    CUcontext           m_cuCtx;
    CUvideoparser       m_cuParser;
    CUvideodecoder      m_cuDecoder;
    cudaVideoCodec      m_cuCodecID;

    int                 m_iPicWidth;
    int                 m_iPicHeight;
    double              m_fps;
    int                 m_ofmt;                     /* refer to enum en_nvcodec_outfmt_t */
    int                 m_iPicWidth_out;
    int                 m_iPicHeight_out;
    int                 m_iPicWidth_out_max;
    int                 m_iPicHeight_out_max;

    int                 m_iFrameIdx;
    unsigned char*      m_outframe;
    int                 m_outframe_size;
    unsigned char*      m_outframe_tmp;             /* For nv12 -> bgr */
    int                 m_outframe_tmp_size;
	bufex_t*            m_outbufs[DECOUTBUF_MAX];
    int                 m_iCurOutBufIdx;

    CPicQueue*          m_pPicQueue;

    int                 m_iInternalErrorCode;
};

#endif
