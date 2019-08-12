#ifndef __MVM_API_H__
#define __MVM_API_H__

/*
   Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/mvm_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"

typedef enum mvm_callindex_enum_t
{
  /* public */  MVM_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */  MVM_CALLINDEX_ENUM_INIT = 1,
  /* public */  MVM_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */  MVM_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */  MVM_CALLINDEX_ENUM_DEINIT = 4,
  /* private */ MVM_CALLINDEX_ENUM_RUN = 5,
  /* public */  MVM_CALLINDEX_ENUM_INVALID
}
  mvm_callindex_enum_t;

APR_EXTERNAL int32_t mvm_call (
  mvm_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif /* __MVM_API_H__ */

