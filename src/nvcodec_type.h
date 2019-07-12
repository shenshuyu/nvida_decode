/*************************************************************************
** file: nvcodec_type.h
** desc:
*************************************************************************/
#ifndef _NVCODEC_TYPE_H
#define _NVCODEC_TYPE_H
//
#include <stdint.h>
#include "bufex.h"


/** cuvid decode handle */
typedef void* nvcodechdl_t;


/** @brief This API supports the following video stream formats */
typedef enum _en_nvcodecid_t {
    NVCODEC_ID_MPEG1=0,                 /**<  MPEG1   */
    NVCODEC_ID_MPEG2,                   /**<  MPEG2  */
    NVCODEC_ID_MPEG4,                   /**<  MPEG4   */
    NVCODEC_ID_VC1,                     /**<  VC1   */
    NVCODEC_ID_H264,                    /**<  H264   */
    NVCODEC_ID_JPEG,                    /**<  JPEG   */
    NVCODEC_ID_H264_SVC,                /**<  H264-SVC   */
    NVCODEC_ID_H264_MVC,                /**<  H264-MVC   */
    NVCODEC_ID_HEVC,                    /**<  HEVC   */
    NVCODEC_ID_VP8,                     /**<  VP8   */
    NVCODEC_ID_VP9,                     /**<  VP9   */
    NVCODEC_ID_UNKNOWN
}en_nvcodecid_t;

/** @brief This API supports the following output video stream chroma format */
typedef enum _en_nvcodec_outfmt_t {
	NVCODEC_OUTFMT_NV12,
	NVCODEC_OUTFMT_I420,
	NVCODEC_OUTFMT_BGR,
	NVCODEC_OUTFMT_NUM
}en_nvcodec_outfmt_t;

#define ISVALID_NVCODEC_OUTFMT(ofmt) ((ofmt) >= NVCODEC_OUTFMT_NV12 && (ofmt) < NVCODEC_OUTFMT_NUM)

#define     NVCODEC_ENC_ERR_BASE     1000
/**
 * Error codes
 */
typedef enum _en_nvcodec_err_t
{
    /**
     * The API call returned with no errors. In the case of query calls, this
     * can also mean that the operation being queried is complete (see
     * ::cuEventQuery() and ::cuStreamQuery()).
     */
    NVCODEC_SUCCESS                              = 0,

    /**
     * This indicates that one or more of the parameters passed to the API call
     * is not within an acceptable range of values.
     */
    NVCODEC_ERR_INVALID_VALUE                  = 1,

    /**
     * The API call failed because it was unable to allocate enough memory to
     * perform the requested operation.
     */
    NVCODEC_ERR_OUT_OF_MEMORY                  = 2,

    /**
     * This indicates that the NVCODEC driver has not been initialized with
     * ::cuInit() or that initialization has failed.
     */
    NVCODEC_ERR_NOT_INITIALIZED                = 3,

    /**
     * This indicates that the NVCODEC driver is in the process of shutting down.
     */
    NVCODEC_ERR_DEINITIALIZED                  = 4,

    /**
     * This indicates profiling APIs are called while application is running
     * in visual profiler mode.
    */
    NVCODEC_ERR_PROFILER_DISABLED           = 5,
    /**
     * This indicates profiling has not been initialized for this context.
     * Call cuProfilerInitialize() to resolve this.
    */
    NVCODEC_ERR_PROFILER_NOT_INITIALIZED       = 6,
    /**
     * This indicates profiler has already been started and probably
     * cuProfilerStart() is incorrectly called.
    */
    NVCODEC_ERR_PROFILER_ALREADY_STARTED       = 7,
    /**
     * This indicates profiler has already been stopped and probably
     * cuProfilerStop() is incorrectly called.
    */
    NVCODEC_ERR_PROFILER_ALREADY_STOPPED       = 8,
    /**
     * This indicates that no NVCODEC-capable devices were detected by the installed
     * NVCODEC driver.
     */
    NVCODEC_ERR_NO_DEVICE                      = 100,

    /**
     * This indicates that the device ordinal supplied by the user does not
     * correspond to a valid NVCODEC device.
     */
    NVCODEC_ERR_INVALID_DEVICE                 = 101,


    /**
     * This indicates that the device kernel image is invalid. This can also
     * indicate an invalid NVCODEC module.
     */
    NVCODEC_ERR_INVALID_IMAGE                  = 200,

    /**
     * This most frequently indicates that there is no context bound to the
     * current thread. This can also be returned if the context passed to an
     * API call is not a valid handle (such as a context that has had
     * ::cuCtxDestroy() invoked on it). This can also be returned if a user
     * mixes different API versions (i.e. 3010 context with 3020 API calls).
     * See ::cuCtxGetApiVersion() for more details.
     */
    NVCODEC_ERR_INVALID_CONTEXT                = 201,

    /**
     * This indicated that the context being supplied as a parameter to the
     * API call was already the active context.
     * \deprecated
     * This error return is deprecated as of NVCODEC 3.2. It is no longer an
     * error to attempt to push the active context via ::cuCtxPushCurrent().
     */
    NVCODEC_ERR_CONTEXT_ALREADY_CURRENT        = 202,

    /**
     * This indicates that a map or register operation has failed.
     */
    NVCODEC_ERR_MAP_FAILED                     = 205,

    /**
     * This indicates that an unmap or unregister operation has failed.
     */
    NVCODEC_ERR_UNMAP_FAILED                   = 206,

    /**
     * This indicates that the specified array is currently mapped and thus
     * cannot be destroyed.
     */
    NVCODEC_ERR_ARRAY_IS_MAPPED                = 207,

    /**
     * This indicates that the resource is already mapped.
     */
    NVCODEC_ERR_ALREADY_MAPPED                 = 208,

    /**
     * This indicates that there is no kernel image available that is suitable
     * for the device. This can occur when a user specifies code generation
     * options for a particular NVCODEC source file that do not include the
     * corresponding device configuration.
     */
    NVCODEC_ERR_NO_BINARY_FOR_GPU              = 209,

    /**
     * This indicates that a resource has already been acquired.
     */
    NVCODEC_ERR_ALREADY_ACQUIRED               = 210,

    /**
     * This indicates that a resource is not mapped.
     */
    NVCODEC_ERR_NOT_MAPPED                     = 211,

    /**
     * This indicates that a mapped resource is not available for access as an
     * array.
     */
    NVCODEC_ERR_NOT_MAPPED_AS_ARRAY            = 212,

    /**
     * This indicates that a mapped resource is not available for access as a
     * pointer.
     */
    NVCODEC_ERR_NOT_MAPPED_AS_POINTER          = 213,

    /**
     * This indicates that an uncorrectable ECC error was detected during
     * execution.
     */
    NVCODEC_ERR_ECC_UNCORRECTABLE              = 214,

    /**
     * This indicates that the ::CUlimit passed to the API call is not
     * supported by the active device.
     */
    NVCODEC_ERR_UNSUPPORTED_LIMIT              = 215,

    /**
     * This indicates that the ::CUcontext passed to the API call can
     * only be bound to a single CPU thread at a time but is already
     * bound to a CPU thread.
     */
    NVCODEC_ERR_CONTEXT_ALREADY_IN_USE         = 216,

    /**
     * This indicates that the device kernel source is invalid.
     */
    NVCODEC_ERR_INVALID_SOURCE                 = 300,

    /**
     * This indicates that the file specified was not found.
     */
    NVCODEC_ERR_FILE_NOT_FOUND                 = 301,

    /**
     * This indicates that a link to a shared object failed to resolve.
     */
    NVCODEC_ERR_SHARED_OBJECT_SYMBOL_NOT_FOUND = 302,

    /**
     * This indicates that initialization of a shared object failed.
     */
    NVCODEC_ERR_SHARED_OBJECT_INIT_FAILED      = 303,

    /**
     * This indicates that an OS call failed.
     */
    NVCODEC_ERR_OPERATING_SYSTEM               = 304,


    /**
     * This indicates that a resource handle passed to the API call was not
     * valid. Resource handles are opaque types like ::CUstream and ::CUevent.
     */
    NVCODEC_ERR_INVALID_HANDLE                 = 400,


    /**
     * This indicates that a named symbol was not found. Examples of symbols
     * are global/constant variable names, texture names, and surface names.
     */
    NVCODEC_ERR_NOT_FOUND                      = 500,


    /**
     * This indicates that asynchronous operations issued previously have not
     * completed yet. This result is not actually an error, but must be indicated
     * differently than ::NVCODEC_SUCCESS (which indicates completion). Calls that
     * may return this value include ::cuEventQuery() and ::cuStreamQuery().
     */
    NVCODEC_ERR_NOT_READY                      = 600,


    /**
     * An exception occurred on the device while executing a kernel. Common
     * causes include dereferencing an invalid device pointer and accessing
     * out of bounds shared memory. The context cannot be used, so it must
     * be destroyed (and a new one should be created). All existing device
     * memory allocations from this context are invalid and must be
     * reconstructed if the program is to continue using NVCODEC.
     */
    NVCODEC_ERR_LAUNCH_FAILED                  = 700,

    /**
     * This indicates that a launch did not occur because it did not have
     * appropriate resources. This error usually indicates that the user has
     * attempted to pass too many arguments to the device kernel, or the
     * kernel launch specifies too many threads for the kernel's register
     * count. Passing arguments of the wrong size (i.e. a 64-bit pointer
     * when a 32-bit int is expected) is equivalent to passing too many
     * arguments and can also result in this error.
     */
    NVCODEC_ERR_LAUNCH_OUT_OF_RESOURCES        = 701,

    /**
     * This indicates that the device kernel took too long to execute. This can
     * only occur if timeouts are enabled - see the device attribute
     * ::CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT for more information. The
     * context cannot be used (and must be destroyed similar to
     * ::NVCODEC_DEC_ERR_LAUNCH_FAILED). All existing device memory allocations from
     * this context are invalid and must be reconstructed if the program is to
     * continue using NVCODEC.
     */
    NVCODEC_ERR_LAUNCH_TIMEOUT                 = 702,

    /**
     * This error indicates a kernel launch that uses an incompatible texturing
     * mode.
     */
    NVCODEC_ERR_LAUNCH_INCOMPATIBLE_TEXTURING  = 703,

    /**
     * This error indicates that a call to ::cuCtxEnablePeerAccess() is
     * trying to re-enable peer access to a context which has already
     * had peer access to it enabled.
     */
    NVCODEC_ERR_PEER_ACCESS_ALREADY_ENABLED = 704,

    /**
     * This error indicates that a call to ::cuMemPeerRegister is trying to
     * register memory from a context which has not had peer access
     * enabled yet via ::cuCtxEnablePeerAccess(), or that
     * ::cuCtxDisablePeerAccess() is trying to disable peer access
     * which has not been enabled yet.
     */
    NVCODEC_ERR_PEER_ACCESS_NOT_ENABLED    = 705,

    /**
     * This error indicates that a call to ::cuMemPeerRegister is trying to
     * register already-registered memory.
     */
    NVCODEC_ERR_PEER_MEMORY_ALREADY_REGISTERED = 706,

    /**
     * This error indicates that a call to ::cuMemPeerUnregister is trying to
     * unregister memory that has not been registered.
     */
    NVCODEC_ERR_PEER_MEMORY_NOT_REGISTERED     = 707,

    /**
     * This error indicates that ::cuCtxCreate was called with the flag
     * ::CU_CTX_PRIMARY on a device which already has initialized its
     * primary context.
     */
    NVCODEC_ERR_PRIMARY_CONTEXT_ACTIVE         = 708,

    /**
     * This error indicates that the context current to the calling thread
     * has been destroyed using ::cuCtxDestroy, or is a primary context which
     * has not yet been initialized.
     */
    NVCODEC_ERR_CONTEXT_IS_DESTROYED           = 709,

    /**
     * This indicates that an unknown internal error has occurred.
     */
    NVCODEC_ERR_UNKNOWN                        = 999,

    /**
     * This indicates that API call returned with no errors.
     */
    NVCODEC_ENC_SUCCESS                            = NVCODEC_ENC_ERR_BASE,

    /**
     * This indicates that no encode capable devices were detected.
     */
    NVCODEC_ENC_ERR_NO_ENCODE_DEVICE,

    /**
     * This indicates that devices pass by the client is not supported.
     */
    NVCODEC_ENC_ERR_UNSUPPORTED_DEVICE,

    /**
     * This indicates that the encoder device supplied by the client is not
     * valid.
     */
    NVCODEC_ENC_ERR_INVALID_ENCODERDEVICE,

    /**
     * This indicates that device passed to the API call is invalid.
     */
    NVCODEC_ENC_ERR_INVALID_DEVICE,

    /**
     * This indicates that device passed to the API call is no longer available and
     * needs to be reinitialized. The clients need to destroy the current encoder
     * session by freeing the allocated input output buffers and destroying the device
     * and create a new encoding session.
     */
    NVCODEC_ENC_ERR_DEVICE_NOT_EXIST,

    /**
     * This indicates that one or more of the pointers passed to the API call
     * is invalid.
     */
    NVCODEC_ENC_ERR_INVALID_PTR,

    /**
     * This indicates that completion event passed in ::NvEncEncodePicture() call
     * is invalid.
     */
    NVCODEC_ENC_ERR_INVALID_EVENT,

    /**
     * This indicates that one or more of the parameter passed to the API call
     * is invalid.
     */
    NVCODEC_ENC_ERR_INVALID_PARAM,

    /**
     * This indicates that an API call was made in wrong sequence/order.
     */
    NVCODEC_ENC_ERR_INVALID_CALL,

    /**
     * This indicates that the API call failed because it was unable to allocate
     * enough memory to perform the requested operation.
     */
    NVCODEC_ENC_ERR_OUT_OF_MEMORY,

    /**
     * This indicates that the encoder has not been initialized with
     * ::NvEncInitializeEncoder() or that initialization has failed.
     * The client cannot allocate input or output buffers or do any encoding
     * related operation before successfully initializing the encoder.
     */
    NVCODEC_ENC_ERR_ENCODER_NOT_INITIALIZED,

    /**
     * This indicates that an unsupported parameter was passed by the client.
     */
    NVCODEC_ENC_ERR_UNSUPPORTED_PARAM,

    /**
     * This indicates that the ::NvEncLockBitstream() failed to lock the output
     * buffer. This happens when the client makes a non blocking lock call to
     * access the output bitstream by passing NVCODEC_ENC_LOCK_BITSTREAM::doNotWait flag.
     * This is not a fatal error and client should retry the same operation after
     * few milliseconds.
     */
    NVCODEC_ENC_ERR_LOCK_BUSY,

    /**
     * This indicates that the size of the user buffer passed by the client is
     * insufficient for the requested operation.
     */
    NVCODEC_ENC_ERR_NOT_ENOUGH_BUFFER,

    /**
     * This indicates that an invalid struct version was used by the client.
     */
    NVCODEC_ENC_ERR_INVALID_VERSION,

    /**
     * This indicates that ::NvEncMapInputResource() API failed to map the client
     * provided input resource.
     */
    NVCODEC_ENC_ERR_MAP_FAILED,

    /**
     * This indicates encode driver requires more input buffers to produce an output
     * bitstream. If this error is returned from ::NvEncEncodePicture() API, this
     * is not a fatal error. If the client is encoding with B frames then,
     * ::NvEncEncodePicture() API might be buffering the input frame for re-ordering.
     *
     * A client operating in synchronous mode cannot call ::NvEncLockBitstream()
     * API on the output bitstream buffer if ::NvEncEncodePicture() returned the
     * ::NVCODEC_ENC_ERR_NEED_MORE_INPUT error code.
     * The client must continue providing input frames until encode driver returns
     * ::NVCODEC_ENC_SUCCESS. After receiving ::NV_ENC_SUCCESS status the client can call
     * ::NvEncLockBitstream() API on the output buffers in the same order in which
     * it has called ::NvEncEncodePicture().
     */
    NVCODEC_ENC_ERR_NEED_MORE_INPUT,

    /**
     * This indicates that the HW encoder is busy encoding and is unable to encode
     * the input. The client should call ::NvEncEncodePicture() again after few
     * milliseconds.
     */
    NVCODEC_ENC_ERR_ENCODER_BUSY,

    /**
     * This indicates that the completion event passed in ::NvEncEncodePicture()
     * API has not been registered with encoder driver using ::NvEncRegisterAsyncEvent().
     */
    NVCODEC_ENC_ERR_EVENT_NOT_REGISTERD,

    /**
     * This indicates that an unknown internal error has occurred.
     */
    NVCODEC_ENC_ERR_GENERIC,

    /**
     * This indicates that the client is attempting to use a feature
     * that is not available for the license type for the current system.
     */
    NVCODEC_ENC_ERR_INCOMPATIBLE_CLIENT_KEY,

    /**
     * This indicates that the client is attempting to use a feature
     * that is not implemented for the current version.
     */
    NVCODEC_ENC_ERR_UNIMPLEMENTED,

    /**
     * This indicates that the ::NvEncRegisterResource API failed to register the resource.
     */
    NVCODEC_ENC_ERR_RESOURCE_REGISTER_FAILED,

    /**
     * This indicates that the client is attempting to unregister a resource
     * that has not been successfully registered.
     */
    NVCODEC_ENC_ERR_RESOURCE_NOT_REGISTERED,

    /**
     * This indicates that the client is attempting to unmap a resource
     * that has not been successfully mapped.
     */
    NVCODEC_ENC_ERR_RESOURCE_NOT_MAPPED,
}en_nvcodec_err_t;


#endif
