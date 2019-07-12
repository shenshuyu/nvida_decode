/****************************************************************
** file: bufex_inner.h
** desc:
*****************************************************************/
#ifndef _BUFEX_INNER_H
#define _BUFEX_INNER_H
//
#include <stdint.h>
//
#include "bufex.h"

/////////////////////////////////////////////////////////////////
typedef enum _en_bufstatus_t {
    BUFSTATUS_FREE,
    BUFSTATUS_CODEC,
    BUFSTATUS_APP,
}en_bufstatus_t;

typedef struct _bufex_inner_t {
    en_bufstatus_t          status;

    int                     devno;
    int                     devtype;    /* refer to macro: DEVTYPE_XXX */

    void*                   mem;
    int                     memsize;
    int                     datasize;
    int                     dataoff;

    int                     width;
    int                     height;
    int64_t                 ts;

    int                     mapflag;

    void*                   ctx;

    void*                   reserved;
}bufex_inner_t;

#define BUFEX_INNER_SIZE    sizeof(bufex_inner_t)

#define BUFEX_SETFIELD(p_bufex, field, value) do { \
        (p_bufex)->(field) = (value); \
    } while (0)

#define BUFEX_SETIFIELD(p_bufex, field, value) do { \
        if ((p_bufex) != NULL) { \
            ((bufex_inner_t*)((p_bufex)->data))->field = (value); \
        } \
    } while (0)

#define BUFEX_IFIELD(p_bufex, field) (((bufex_inner_t*)((p_bufex)->data))->field)

#endif

