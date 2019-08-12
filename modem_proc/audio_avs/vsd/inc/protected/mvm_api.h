#ifndef __MVM_API_H__
#define __MVM_API_H__

/*
   Copyright (C) 2009-2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/protected/mvm_api.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "drv_api.h"

/**
  Entry command to any CVD based synchronous blocking APIs.
  The individual cmd_id payload needs to be type cast to void* 
  with size of payload specified in "size" argument.

  @par[in] cmd_id
  @par[in] params
  @par[in] size of params
  
  @return 
  APR_EOK on success.

  @dependencies
  None.
 
  @comments

*/
APR_EXTERNAL int32_t mvm_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /* __MVM_API_H__ */

