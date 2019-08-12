#ifndef DLW_H
#define DLW_H
/*==============================================================================

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Values for dlopen `mode'. */
#define DLW_RTLD_LAZY 1
#define DLW_RTLD_NOW 2

/* Special handle arguments for dlsym() */   
#define	DLW_RTLD_NEXT ((void*)-1)	/* Search subsequent objects */
#define	DLW_RTLD_DEFAULT ((void*)-2)	/* Use default search algorithm */
#define	DLW_RTLD_SELF ((void*)-3)	/* Search the caller itself */

typedef struct {
  const char *dli_fname;	/* File defining the symbol */
  void *dli_fbase;	/* Base address */
  const char *dli_sname;	/* Symbol name */
  const void *dli_saddr;	/* Symbol address */
} dlw_info;

int dlw_Init(void);
void* dlw_Open(const char*, int);
int dlw_Close(void*);
void* dlw_Sym(void* __restrict, const char* __restrict);
int dlw_Addr(const void* __restrict, dlw_info* __restrict);
char* dlw_Error(void);

#ifdef __cplusplus
}
#endif

#endif // DLW_H
