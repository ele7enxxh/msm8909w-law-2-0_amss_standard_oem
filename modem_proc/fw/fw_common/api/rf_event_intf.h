/*!
  @file
  rf_event_intf.h

  @brief
  Types for RF CCS events inside shared memory
 
*/
#ifndef RF_EVENT_INTF_H
#define RF_EVENT_INTF_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_common/api/rf_event_intf.h#1 $
$DateTime: 2016/12/13 08:00:08 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/04/12   jfc     Add Event ID base for command interface 
07/17/12   jfc     Move RF event IDs to rf_event_intf.h
07/03/12   jfc     Fix remnants of old file name
05/21/12   jfc     Change IDLE state to have value 0x0
05/13/12   jfc     Remove "comdef.h" and remove types defined by CCS intf
05/09/12   jfc     Swap fields around in task type for better packing
05/07/12   jfc     Initial version
============================================================================*/



#include "ccs_rf_intf.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define RF_EVENT_INTF_MINOR_VERSION   0
#define RF_EVENT_INTF_MAJOR_VERSION   2

#define RF_EVENT_INTF_VERSION    (RF_EVENT_INTF_MAJOR_VERSION<<16)|(RF_EVENT_INTF_MINOR_VERSION)


/*! @brief Base IDs for RF events. All tech event IDs should be 
  in the range RF_EVENT_ID_BASE_XXX to (RF_EVENT_ID_BASE_XXX+RF_EVENT_MAX_TECH_ID).
  The execption is RF command interface tasks/events which have the range 0x80000000 - 0xFFFFFFFF
*/
#define RF_EVENT_ID_BASE_GSM     0x01000000
#define RF_EVENT_ID_BASE_WCDMA   0x02000000
#define RF_EVENT_ID_BASE_CDMA    0x03000000
#define RF_EVENT_ID_BASE_LTE     0x04000000
#define RF_EVENT_ID_BASE_TDSCDMA 0x05000000
#define RF_EVENT_ID_BASE_IRAT    0x06000000
#define RF_EVENT_MAX_TECH_ID     0x00FFFFFF
#define RF_EVENT_ID_BASE_RFCMD   0x80000000
#define RF_EVENT_MAX_RFCMD_ID    0xEFFFFFFF


/*! @brief Event Status Type
*/
typedef enum
{
  RF_CCS_EVENT_IDLE           = 0x0,      /*!< Event may be overwritten */
  RF_CCS_EVENT_BUILDING       = 0x1,      /*!< Event is being constructed */
  RF_CCS_EVENT_COMMITTED      = 0x2,      /*!< Event is finalized and pending FW/CCS processing */
  RF_CCS_EVENT_COMPLETE       = 0x3,      /*!< Event is completed and data/status bytes are ready */
  RF_CCS_EVENT_SW_ERROR       = 0xFF,     /*!< Event has some issue with its construction, e.g. memory overflow */
} rf_ccs_event_status_type;


/*! @brief Event Header Type
*/
typedef struct 
{
  uint32  event_id;           /*!< Event ID */
  boolean response_required;  /*!< Response to event is required or not */
  boolean make_atomic;        /*!< Make this event atomic (e.g. ISSUE_SEQ) */
  uint8 priority_if_atomic;   /*!< Priority of the event/task when event is atomic */
  uint8 num_tasks;            /*!< Number of tasks in this event*/
  rf_ccs_event_status_type event_status; /*!< Status of the data in this event */
  uint32 scratch_mem[2];      /*!< Scratch memory for FW to use per event */

} rf_ccs_event_description_buffer_header_type;


typedef ccs_rf_task_buf_item_t rf_ccs_event_task_description_type;


#endif /* RF_EVENT_INTF_H */
