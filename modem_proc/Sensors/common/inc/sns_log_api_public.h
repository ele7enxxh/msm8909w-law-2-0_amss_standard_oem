#ifndef SNS_LOG_API_PUBLIC_H
#define SNS_LOG_API_PUBLIC_H

/*============================================================================

@file
sns_log_api_public.h

@brief
Contains function prototypes of loggings API's.

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include <stddef.h>

#ifdef SENSORS_DD_DEV_FLAG

#include "sns_log_types_public.h"
typedef sns_log_id_public_e log_pkt_t;

#else

#include "sns_log_types.h"
#include "sns_common.h"
typedef sns_log_id_e log_pkt_t;

#endif

/*===========================================================================

  FUNCTION:   sns_logpkt_malloc

===========================================================================*/
/*!
  @brief
  Allocates memory for the log packet. On the Apps processor this function
  would implement a call to the DIAG logging api's.

  @param[i]
  log_pkt_type  : Log Packet type
  pkt_size      : Size
  log_pkt_ptr   : Pointer of the location in which to place the
                  allocated log packet

  @return
  sns_err_code_e: SNS_SUCCESS if the allocation had no errors.
                  All other values indicate an error has occurred.
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_malloc(log_pkt_t log_pkt_type,
                                 uint32_t pkt_size,
                                 void** log_pkt_ptr);

/*===========================================================================

  FUNCTION:   sns_logpkt_commit

===========================================================================*/
/*!
  @brief
  Commits the log packet to DIAG. On the Apps processor this function
  would implement a call to the DIAG logging api's.

  @param[i]
  log_pkt_type: Log Packet type
  log_pkt_ptr : Pointer to the log packet to commit

  @return
  sns_err_code_e: SNS_SUCCESS if the commit had no errors.
                  All other values indicate an error has occurred.
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_commit(log_pkt_t log_pkt_type,
                                 void* log_pkt_ptr);

/*===========================================================================

  FUNCTION:   sns_logpkt_submit

===========================================================================*/
/*!
  @brief
  Allocates and submits a log packet. The log packet may be buffered before
  being sent to DIAG.

  @param[i]
  log_pkt_id    : Log Packet ID
  log_pkt_size  : sizeof the log packet
  log_pkt       : Log packet to submit

  @return
  sns_err_code_e: SNS_SUCCESS if the submit had no errors.
                  SNS_ERR_NOMEM If memory is unavailable to store the packet
                  SNS_ERR_FAILED The submit process failed
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_submit(uint32_t log_pkt_id,
                                 uint32_t log_pkt_size,
                                 PACK(void*) log_pkt);

#endif /* SNS_LOG_API_PUBLIC_H */
