/****************************************************************
** file: bufex_nv.h
** desc:
*****************************************************************/
#ifndef _BUFEX_NV_H
#define _BUFEX_NV_H
//
#include "bufex.h"
#include "bufex_inner.h"

int bufex_nvalloc(bufex_t **pp_bufex, int size, int devno);
void bufex_nvfree(bufex_t **pp_bufex);
int bufex_nvresize(bufex_t **pp_bufex, int newsize);
int bufex_nvread(bufex_t *p_bufex, void *data, int datasize, mem_type_t datatype);

#endif

