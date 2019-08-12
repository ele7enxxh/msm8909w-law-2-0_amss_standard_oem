#ifndef __MVM_API_I_H__
#define __MVM_API_I_H__

/*
   Copyright (C) 2014 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/protected/mvm_api_i.h#1 $
   $Author: mplcsds1 $
*/

#include "mvm_api.h"
#include "cvd_cal_protected_i.h"

/****************************************************************************
 * CVD calibration utility APIs                                             *
 ****************************************************************************/
/* Following utility APIs need to be used with mvm_call() API.

   All the utility APIs are synchronous blocking calls.
*/

/**
  Command used to retrieve the search output tagged to a valid query_handle. 
  If the query_handle is valid, the cal utility will call the call back function 
  registered ( #cvd_cal_query_cb_fn_t ) multiple times for all the mid, pid pairs 
  for all matching entries.

  @par Payload
  #vss_imvm_cmd_cal_query_t

  @return 
  APR_EOK on success

  @dependencies
  None.

  @comments

*/

#define MVM_CMDID_CAL_QUERY ( 0xDEADBE01 )


/** @brief Type definition for #vss_imvm_cmd_cal_query_t.
*/
typedef struct vss_imvm_cmd_cal_query_t vss_imvm_cmd_cal_query_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #MVM_CMDID_CAL_QUERY command.
*/
struct vss_imvm_cmd_cal_query_t
{
  uint32_t query_handle;
  cvd_cal_query_cb_fn_t cb_fn;
  const void* client_data;
}
#include "apr_pack_end.h"
;

#endif /* __MVM_API_I_H__ */

