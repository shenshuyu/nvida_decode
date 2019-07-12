/****************************************************************
** file: bufex.h
** desc:
*****************************************************************/
#ifndef _BUFEX_H
#define _BUFEX_H

////////////////////////////////////////////////////////////////
// The extended buffer
typedef struct _bufex_t {
    int     datasize;
    void*   data;
}bufex_t;

#define BUFEX_SIZE  sizeof(bufex_t)

// The type of memory
typedef enum _en_mem_type_t {
    MEM_TYPE_CPU,
    MEM_TYPE_GPU
}mem_type_t;

typedef struct _pairbuf_t {
    bufex_t                 *in;
    bufex_t                 *out;

    struct _pairbuf_t*      next;
}pairbuf_t;

#endif

