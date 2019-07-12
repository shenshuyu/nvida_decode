#include "nvcodec.h"
#include "NvDec.h"
#include "bufex_nv.h"
#include "NvSdk.h"
#include <stdio.h>
#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif

__attribute ((visibility("default"))) \
int nvcodec_vdec_init(nvcodechdl_t *p_hdl, int id, int devno, int ofmt, int w_out, int h_out)
{
    int ret;
	CNvDec* pNvDec;

    pNvDec = new CNvDec();
	if (NULL == pNvDec) {
        return NVCODEC_ERR_OUT_OF_MEMORY;
	}

    ret = pNvDec->init(id, devno, ofmt, w_out, h_out);
	if (ret != NVCODEC_SUCCESS) {
        return ret;
	}

    *p_hdl = pNvDec;

	return NVCODEC_SUCCESS;
}

int nvcodec_vdec_init2(nvcodechdl_t *p_hdl, int id, int devno, int ofmt, int w_out_max, int h_out_max)
{
    int ret;
	CNvDec* pNvDec;


    pNvDec = new CNvDec();
	if (NULL == pNvDec) {
        return NVCODEC_ERR_OUT_OF_MEMORY;
	}

    ret = pNvDec->init2(id, devno, ofmt, w_out_max, h_out_max);
	if (ret != NVCODEC_SUCCESS) {
        return ret;
	}

    *p_hdl = pNvDec;

	return NVCODEC_SUCCESS;
}

__attribute ((visibility("default"))) \
int nvcodec_vdec_deinit(nvcodechdl_t hdl)
{
	CNvDec *pNvDec;
    int devno;

    pNvDec = static_cast<CNvDec*>(hdl);
    if (NULL == pNvDec) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    devno = pNvDec->GetDevNo();

    pNvDec->deinit();
    delete pNvDec;
    pNvDec = NULL;

    return NVCODEC_SUCCESS;
}

__attribute ((visibility("default"))) \
int nvcodec_vdec_insert_frame(nvcodechdl_t hdl, void *pkt, int pktsize, int64_t ts)
{
    int ret;
    CNvDec *pNvDec;

    pNvDec = static_cast<CNvDec*>(hdl);
    if (NULL == pNvDec) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    ret = pNvDec->insert_frame(pkt, pktsize, ts);
    if (ret != NVCODEC_SUCCESS) {
        return ret;
    }

    return NVCODEC_SUCCESS;
}

__attribute ((visibility("default"))) \
int nvcodec_vdec_get_frame(nvcodechdl_t hdl, bufex_t **frame, int *w, int *h, double *fps, int64_t *ts, int *got_frame)
{
    int ret;
    CNvDec *pNvDec;

    pNvDec = static_cast<CNvDec*>(hdl);
    if (NULL == pNvDec) {
        return NVCODEC_ERR_INVALID_VALUE;
    }


    ret = pNvDec->get_frame(frame, w, h, fps, ts, got_frame);
    if (ret != NVCODEC_SUCCESS) {
        return ret;
    }

    return NVCODEC_SUCCESS;
}

__attribute ((visibility("default"))) \
int nvcodec_read_bufex(bufex_t *p_bufex, void *data, int datasize, mem_type_t datatype)
{
    return bufex_nvread(p_bufex, data, datasize, datatype);
}

__attribute ((visibility("default"))) \
int nvcodec_get_dev_cnt(int *dev_cnt)
{
    if (NULL == dev_cnt) {
        return NVCODEC_ERR_INVALID_VALUE;
    }

    *dev_cnt = gNvSdk.m_devnum;

    return NVCODEC_SUCCESS;
}


#ifdef __cplusplus
}
#endif
