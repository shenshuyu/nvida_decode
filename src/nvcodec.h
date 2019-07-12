#ifndef _NVCODEC_H
#define _NVCODEC_H
//
#include "nvcodec_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** Initialize video decoder
**
** @param p_hdl decoder handle
** @param id refer to en_nvcodecid_t
** @param devno the specified device
** @param ofmt output format, refer to enum en_nvcodec_outfmt_t
** @param w_out the width of target resolution, must be 4 multiples, <=0 no use
** @param h_out the height of target resolution, must be 4 multiples, <=0 no use
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_vdec_init(nvcodechdl_t *p_hdl, int id, int devno, int ofmt, int w_out, int h_out);

/**
** Initialize video decoder
**
** @param p_hdl decoder handle
** @param id refer to en_nvcodecid_t
** @param devno the specified device
** @param ofmt output format, refer to enum en_nvcodec_outfmt_t
** @param w_out_max the maximum width of target resolution, must be 4 multiples, <=0 no use
** @param h_out_max the minimum height of target resolution, must be 4 multiples, <=0 no use
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_vdec_init2(nvcodechdl_t *p_hdl, int id, int devno, int ofmt, int w_out_max, int h_out_max);


/**
** Deinitialize video decoder
**
** @param hdl returned by nvcodec_vdec_init()
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_vdec_deinit(nvcodechdl_t hdl);

/**
** Insert a video frame into the decoder
**
** @param hdl returned by nvcodec_vdec_init()
** @param pkt the encoded video frame buffer, cann't be null
** @param pktsize the size of the encoded video frame buffer
** @param ts timestamp
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_vdec_insert_frame(nvcodechdl_t hdl, void *pkt, int pktsize, int64_t ts);

/**
** Get a decoded frame from the decoder with extended buffer
**
** @param hdl returned by nvcodec_vdec_init()
** @param frame the decoded frame
** @param w the pointer of width
** @param h the pointer of height
** @param fps the frame rate
** @param ts the pointer of timestamp
** @param got_frame indicates whether there is a frame output, 1 or 0
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
**
*/
int nvcodec_vdec_get_frame(nvcodechdl_t hdl, bufex_t **frame, int *w, int *h, double *fps, int64_t *ts, int *got_frame);

/**
** Read the extended buffer
**
** @param p_bufex the extended buffer
** @param data the pointer of data
** @param datasize the size of data
** @param datatype the type of data
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_read_bufex(bufex_t *p_bufex, void *data, int datasize, mem_type_t datatype);


/**
** Get the number of devices
**
** @param dev_cnt the pointer of the number of devices
**
** @return NVCODEC_DEC_SUCCESS sucess or error code otherwise
*/
int nvcodec_get_dev_cnt(int *dev_cnt);

#ifdef __cplusplus
}
#endif

#endif // __CUVID_DECODER_API_H__
