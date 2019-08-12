/*
@file cmnlib_cntxt.h
@brief Common TZ App Context Header.

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
#ifndef _CMNLIB_CNTXT_H
#define _CMNLIB_CNTXT_H

typedef void* (*APP_MALLOC)(size_t);
typedef void (*APP_FREE)(void *);
typedef struct CMNLIB_CNTXT_S {
  void *p_malloc_cb;
  void *p_free_cb;
  void *p_app_name;
  uint32 cmnlib_rw_base; 
  uint32 app_rw_base;
}CMNLIB_CNTXT;

extern void* cmnlib_malloc(size_t);
extern void cmnlib_free(void *);

#define CMNLIB_STORE_APP_CONTEXT()            \
  uint32 loc_cmnlib_rw_base = ((CMNLIB_CNTXT *)pAppCntxt)->cmnlib_rw_base;  \
  uint32 loc_app_rw_base = ((CMNLIB_CNTXT *)pAppCntxt)->app_rw_base;        \
  set_cmnlib_rw_base(loc_cmnlib_rw_base);     \
  cur_app_rw_base = loc_app_rw_base;          \
  pgAppCntxt = (CMNLIB_CNTXT *)pAppCntxt       

#define CMNLIB_RESTORE_APP_RW_BASE()      \
  set_cmnlib_rw_base(loc_app_rw_base)    

#endif /* _CMNLIB_CNTXT_H */

