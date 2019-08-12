#ifndef RFCOMMON_FW_RESPONSE_H
#define RFCOMMON_FW_RESPONSE_H

/*! 
  @file
  rfcommon_fw_response.h
 
  @brief
  This file includes functions pertaining to handling of COMMON FW responses
*/
 
/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_fw_response.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
11/30/12   Saul    MSGR. fw_response_dereg APIs extract client from data structs.
11/27/12   Saul    Sample Capture. Bug fix in checking for processing done.
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
08/28/12   rsr     Added support for TXAGC override.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rfcommon_semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for Allocate DPD Tables Response message */
typedef struct
{
  /*! Indicates validity of fw rsp data */
  boolean is_valid_fwrsp;

  /*! semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */
  rfcommon_semaphore_token_type *sema_token;

  /*! Flag indicating, if the message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */
  boolean blocking_call;
    
  /*! The bank mask returned by FW */
  uint16 bank_mask;

  /*! The number of set bits in the mask requested by SW */
  uint8 num_set_bits_sw;
  
} rfcommon_alloc_dpd_tables_rsp_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for Free DPD Tables Response message */
typedef struct
{
  /*! semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */
  rfcommon_semaphore_token_type *sema_token;

  /*! Flag indicating, if the message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */
  boolean blocking_call;
    
} rfcommon_free_dpd_tables_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for Write DPD Tables Response message */
typedef struct
{
  /*! semaphore associated with the message. This will be used if the 
  message is sent as a blocking call. */
  rfcommon_semaphore_token_type *sema_token;
  
  /*! Flag indicating, if the message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */
  boolean blocking_call;
    
} rfcommon_blocking_call_rsp_data_type; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the Write DPD Tables data passed with the callback function
*/
typedef struct
{
  /*! Pointer to FW response housekeeping data for Response */
  rfcommon_blocking_call_rsp_data_type *fw_rsp;
  
} rfcommon_blocking_fw_rsp_cb_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for Capture Done Response message */
typedef struct
{
  /*! Flag indicating, if the message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */
  boolean blocking_call;
    
  /*! Buffer ID for capture done */
  uint8 capt_buf_id;
  
} rfcommon_capture_done_rsp_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store housekeeping data for Processing Done Response message */
typedef struct
{
  /*! Flag indicating, if the message was sent as blocking call or 
  not. If it were a blocking call, the handler function will post 
  the semaphore. */
  boolean blocking_call;
    
  /*! Buffer ID for capture done */
  uint8 capt_buf_id;
  
  /*! Buffer ID for processing done */
  uint8 proc_buf_id;

} rfcommon_processing_done_rsp_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure containing housekeeping data for all RF COMMON commands */
typedef struct
{
  /*! Housekeeping data for Allocate DPD Tables Response  */
  rfcommon_alloc_dpd_tables_rsp_data_type alloc_dpd_tables_rsp;

  /*! Housekeeping data for Free DPD Tables Response  */
  rfcommon_free_dpd_tables_rsp_data_type free_dpd_tables_rsp;

  /*! Housekeeping data for Write DPD Tables Response  */
  rfcommon_blocking_call_rsp_data_type write_dpd_tables_rsp;

  /*! Housekeeping data for TXAGC_ORIDE Response  */
  rfcommon_blocking_call_rsp_data_type txagc_oride_rsp;

  /*! Housekeeping data for Capture Done Response  */
  rfcommon_capture_done_rsp_data_type capture_done_rsp;

  /*! Housekeeping data for Processing Done Response  */
  rfcommon_processing_done_rsp_data_type processing_done_rsp;

} rfcommon_fw_response_data_type;

/*----------------------------------------------------------------------------*/
boolean
rfcommon_fw_response_registration
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_fw_response_deregistration
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_FW_RESPONSE_H */


