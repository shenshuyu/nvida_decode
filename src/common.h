/*********************************************************************
** file: common.h
** desc:
*********************************************************************/
#ifndef _COMMON_H
#define _COMMON_H
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(_WIN32) || defined(_WIN64)
#include "stdint.h"
#else
#include <unistd.h>
#include "sync.h"
#endif

#define MODULE_NAME "libnvcodec"

/////////////////////////////////////////////////////////////////////
#define SAFE_FREE(p) do { \
    if ((p) != NULL) { \
        free(p); \
        (p) = NULL; \
    } \
} while (0)

#define SAFE_DELETE(p) do { \
	if ((p) != NULL) { \
		delete(p); \
		(p) = NULL; \
	} \
} while (0)

#define SAFE_DELETEA(a) do { \
	if ((a) != NULL) { \
	delete [] (a); \
	(a) = NULL; \
	} \
} while (0)

#define SAFE_CLOSEFP(fp) do { \
	if ((fp) != -1) { \
		fclose(fp); \
		(fp) = -1; \
	} \
} while (0)

#define SAFE_CLOSEFD(fd) do { \
	if ((fd) != -1) { \
            close(fd); \
            (fd) = -1; \
        } \
} while (0)

#define SAFE_RELEASE(p) do { \
    if ((p) != NULL) \
    { \
        (p)->Release(); \
        (p) = NULL; \
    } \
} while (0)


#if defined(_WIN32) || defined(_WIN64)
#define SAFE_CLOSEHDL(hdl) do { \
	if (NULL != (hdl) && (hdl) != INVALID_HANDLE_VALUE) { \
		CloseHandle(hdl); \
		(hdl) = NULL; \
	} \
} while (0)

#define SAFE_CLOSESOCKET(hSock) do { \
	if (INVALID_SOCKET != (hSock)) { \
		closesocket(hSock); \
		(hSock) = INVALID_SOCKET; \
	} \
} while (0)

#define SAFE_RELEASE(p) do { \
        if ((p) != NULL) \
        { \
        (p)->Release(); \
        (p) = NULL; \
        } \
    } while (0)
#endif

#define ASSIGN_P(p, val) do { \
    if (NULL != (p)) { \
        *(p) = (val); \
    } \
} while (0)
#endif
