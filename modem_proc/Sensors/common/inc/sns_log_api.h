#ifndef SNS_LOG_API_H
#define SNS_LOG_API_H

/*============================================================================

@file 
sns_log_api.h

@brief
Contains function prototypes of loggings API's.

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*===========================================================================

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/inc/sns_log_api.h#1 $

when       who    what, where, why
(mm/dd/yyyy)
--------   ---    ----------------------------------------------------------
04/04/2014  MW     Moved sns_logpkt_malloc and sns_logpkt_commit to 
					sns_log_api_public.h.
11/09/2010  ad     added support for log filtering 
09/17/2010  sj     Usage of <type>_t (avoids including comdef)
8/12/2010   sj     Created
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_diag_dsps_v01.h"
#include "sns_log_api_public.h"
#include <stddef.h>

/*===========================================================================

  FUNCTION:   sns_logpkt_shorten

===========================================================================*/
/*!
  @brief
  Shortens a log packet allocated with sns_logpkt_malloc. This will reduce
  the size of the log when it is sent via the DIAG protocol. The memory
  allocated in sns_logpkt_malloc will still be released when sns_logpkt_commit
  is called.

  This must be called prior to calling sns_logpkt_commit.
   
  @param[i] log_pkt_ptr   : Pointer to the allocated log packet
  @param[i] new_pkt_size  : New size

  @return
  sns_err_code_e: SNS_SUCCESS if the allocation had no errors.
                  All other values indicate an error has occurred.
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_shorten(void* log_pkt_ptr,
                                  uint32_t new_pkt_size);

/*===========================================================================

  FUNCTION:   sns_diag_dsps_set_log_mask

===========================================================================*/
/*!
  @brief
  Sets the log mask on DSPS
  
   
  @param[i] 
  msg_ptr: pointer to message containing log mask that indicates which
           log packets are enabled/disabled.

  @return
  none
*/
/*=========================================================================*/
void sns_diag_dsps_set_log_mask(sns_diag_set_log_mask_req_msg_v01* msg_ptr);

#endif /* SNS_LOG_API_H */
