#ifndef __VOCSVC_NV_API_H__
#define __VOCSVC_NV_API_H__

/*
   Copyright (C) 2012 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/protected/vocsvc_nv_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"

#define VOCSVC_NV_MAJOR_VERSION ( 3 )
#define VOCSVC_NV_MINOR_VERSION ( 1 )   

typedef struct vocsvc_nv_eamr_config_t
{
  bool_t enable_2g;
  bool_t enable_3g;
  bool_t enable_tdscdma;                
}
  vocsvc_nv_eamr_config_t;

typedef struct vocsvc_nv_evrc_plus_config_t
{
  bool_t evrc_plus_is_enabled;
} 
  vocsvc_nv_evrc_plus_config_t;

typedef struct vocsvc_nv_save_mvs_profiling_buffer_t
{
  uint8_t* buffer;
  uint32_t size;
}
  vocsvc_nv_save_mvs_profiling_buffer_t;

typedef enum vocsvc_nv_callindex_enum_t
{
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_INIT = 1,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_GET_EAMR_CONFIG = 5,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_CREATE_NV_FILE = 6,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_GET_EVRC_PLUS_CONFIG = 7,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_SAVE_MVS_PROFILING_BUFFER = 8,
  /* public */ VOCSVC_NV_CALLINDEX_ENUM_INVALID
}
  vocsvc_nv_callindex_enum_t;

APR_EXTERNAL int32_t vocsvc_nv_call
(
  vocsvc_nv_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif  /* __VOCSVC_NV_API_H__ */

