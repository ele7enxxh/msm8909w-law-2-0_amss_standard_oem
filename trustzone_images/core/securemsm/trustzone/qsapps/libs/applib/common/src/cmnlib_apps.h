/*
@file cmnlib_apps.h
@brief Common TZ App Library. This library is shared between all TZ apps.

*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#ifndef _CMNLIB_APPS_H
#define _CMNLIB_APPS_H

/* DAL Heap API's */
#ifdef _TZ_PROXY_LIB
typedef int (*CMNLIB_MEMHEAPLITE_INIT)(void *, void *,void *, unsigned long, unsigned int, unsigned int);
typedef int (*CMNLIB_MEMHEAPLITE_DEINIT)(void *,void *, unsigned int, unsigned int);
typedef void *(*CMNLIB_MEMHEAPLITE_MALLOC)(void *,void *, unsigned long, unsigned int, unsigned int);
typedef int (*CMNLIB_MEMHEAPLITE_FREE)(void *, void *,void *, unsigned int, unsigned int);
#else 
typedef int (*CMNLIB_MEMHEAPLITE_INIT)(mem_heap_type *, mem_magic_number_struct *,void *, unsigned long, unsigned int, unsigned int);
typedef int (*CMNLIB_MEMHEAPLITE_DEINIT)(mem_heap_type *, mem_magic_number_struct *,unsigned int, unsigned int);
typedef void *(*CMNLIB_MEMHEAPLITE_MALLOC)(mem_heap_type *, mem_magic_number_struct *,unsigned long, unsigned int, unsigned int);
typedef int (*CMNLIB_MEMHEAPLITE_FREE)(mem_heap_type *, mem_magic_number_struct *,void *, unsigned int, unsigned int);
#endif

typedef void (*CMNLIB_RELEASE_CTZ_LIB)(void *, void *,void *, unsigned int, unsigned int);
typedef void* (*CMNLIB_QSEE_MALLOC)(size_t);
typedef void (*CMNLIB_QSEE_FREE)(void *);

typedef struct CMNLIB_TZAPP_LIB_S {
  /* Qsee Malloc & Free */
  CMNLIB_QSEE_MALLOC        p_qsee_malloc; /* Used by common library */
  CMNLIB_QSEE_FREE          p_qsee_free;   /* Used by common library */

  /* DAL Heap API's */
  CMNLIB_MEMHEAPLITE_INIT       p_MemHeapLite_Init;         
  CMNLIB_MEMHEAPLITE_DEINIT     p_MemHeapLite_Deinit;         
  CMNLIB_MEMHEAPLITE_MALLOC     p_MemHeapLite_Malloc;       /* Used by TZ App. */
  CMNLIB_MEMHEAPLITE_FREE       p_MemHeapLite_Free;         /* Used by TZ App. */

  /* Library Interface Release */
  CMNLIB_RELEASE_CTZ_LIB    p_cmnlib_release_lib; 
}CMNLIB_TZAPP_LIB, *PCMNLIB_TZAPP_LIB;

typedef struct CMNLIB_HEAP_PARAMS_STRUCT {
  void *pHeap;
  void *pMagicNumber;
  void *pHeapMem; 
  unsigned int iHeapSize;
}CMNLIB_HEAP_PARAMS, *PCMNLIB_HEAP_PARAMS;
#endif /* _CMNLIB_APPS_H */

