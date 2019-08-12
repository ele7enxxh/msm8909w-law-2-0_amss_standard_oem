/*!
  @file
  rflm_rf_event_intf.h

  @brief
  Common modem HAL interface file for RF event definitions.
 
  @details
  This header file replaces the rf_event_intf.h header file, which causes
  file name conflict issues in Bolt.  
 
*/
#ifndef RFLM_RF_EVENT_INTF_H
#define RFLM_RF_EVENT_INTF_H

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/ccs/rflm_rf_event_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/25/13   bmg     Created.
==============================================================================*/
 
#include "rflm_ccs_task_defs.h"
#include "rflm_ccs_rf_event_intf.h"


#define RFLM_RF_EVENT_ID_BASE_GSM      RFLM_CCS_RF_EVENT_ID_BASE_GSM    
#define RFLM_RF_EVENT_ID_BASE_WCDMA    RFLM_CCS_RF_EVENT_ID_BASE_WCDMA  
#define RFLM_RF_EVENT_ID_BASE_CDMA     RFLM_CCS_RF_EVENT_ID_BASE_CDMA   
#define RFLM_RF_EVENT_ID_BASE_LTE      RFLM_CCS_RF_EVENT_ID_BASE_LTE    
#define RFLM_RF_EVENT_ID_BASE_TDSCDMA  RFLM_CCS_RF_EVENT_ID_BASE_TDSCDMA
#define RFLM_RF_EVENT_ID_BASE_IRAT     RFLM_CCS_RF_EVENT_ID_BASE_IRAT   
#define RFLM_RF_EVENT_MAX_TECH_ID      RFLM_CCS_RF_EVENT_MAX_TECH_ID    
#define RFLM_RF_EVENT_ID_BASE_RFCMD    RFLM_CCS_RF_EVENT_ID_BASE_RFCMD  
#define RFLM_RF_EVENT_MAX_RFCMD_ID     RFLM_CCS_RF_EVENT_MAX_RFCMD_ID   
/*! @brief Event Status Type
*/
typedef enum
{
  RFLM_RF_CCS_EVENT_IDLE      =     RFLM_CCS_RF_EVENT_IDLE,       /*!< Event may be overwritten */
  RFLM_RF_CCS_EVENT_BUILDING  =     RFLM_CCS_RF_EVENT_BUILDING,   /*!< Event is being constructed */
  RFLM_RF_CCS_EVENT_COMMITTED =     RFLM_CCS_RF_EVENT_COMMITTED,  /*!< Event is finalized and pending FW/CCS processing */
  RFLM_RF_CCS_EVENT_COMPLETE  =     RFLM_CCS_RF_EVENT_COMPLETE,   /*!< Event is completed and data/status bytes are ready */
  RFLM_RF_CCS_EVENT_SW_ERROR  =     RFLM_CCS_RF_EVENT_SW_ERROR,   /*!< Event has some issue with its construction, e.g. memory overflow */
} rflm_rf_ccs_event_status_type;

/*! @brief Event Header Type
*/
typedef struct 
{
  uint32  event_id;           /*!< Event ID */
  boolean response_required;  /*!< Response to event is required or not */
  boolean make_atomic;        /*!< Make this event atomic (e.g. ISSUE_SEQ) */
  uint8 priority_if_atomic;   /*!< Priority of the event/task when event is atomic */
  uint8 num_tasks;            /*!< Number of tasks in this event*/
  rflm_rf_ccs_event_status_type event_status; /*!< Status of the data in this event */
  uint32 scratch_mem[2];      /*!< Scratch memory for FW to use per event */

} rflm_rf_ccs_event_description_buffer_header_type;

#endif /* RFLM_RF_EVENT_INTF_H */

