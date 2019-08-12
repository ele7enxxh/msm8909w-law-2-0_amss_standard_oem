#ifndef RFWCDMA_MDSP_CCS_EVENTS_H 
#define RFWCDMA_MDSP_CCS_EVENTS_H 

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-mDSP AGC interface.
*/

/*===========================================================================


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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp_ccs_events.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
05/16/14   aro     Exported Interface to get resource script
02/10/14    kg     Adding Tuner Open Loop Event Support
09/18/13    dw     Added support for LM handle based event script 
                   Apply LM handle mask to event ID
08/29/13    dw     Update with FED DM support
08/23/13    dw     Initial Verision
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfwcdma_mdsp_types.h"
#include "rflm_dm_api.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Applies the RF Common event base offset to an event ID.
  @details
  The event ID OR'd with the common base offset
 
  Note: Tech mask can only be applied at MSB, other bits are reserved. Caller
  needs to check for 32 bit integer overlow
 
*/
#define APPLY_TECH_EVENT_MASK(e) ((RFCOMMON_MDSP_EVENT_ID_BASE_WCDMA&0xFF000000)|e)

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Applies RF LM handle offset to an event ID
  @details
  The event ID OR'd with an offset of RFLM handle Mask at Bit mask 0x0FF0.
  This is to support same event ID built for different LM handle at the same time
 
  32 bit event ID mask:
  |Tech Mask|LM handle Mask|Base ID|
      00      00      00      00
 
  Note: this assumes max LM handle can not be bigger than 15! Caller needs to
  check for 32 bit integer overflow
 
*/
#define APPLY_LM_HANDLE_EVENT_MASK(h,e) ((1<<(8+h))|e)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Commits RF scripts to CCS by executing the whole CCS event sequence,
  including event_start, event_push, event_commit and event_cleanup.
 
  @details
  This API should be used for commiting scripts to CCS. This
  API also performs all the checks before claiing the rfcommon
  function. It prepares the event by calling event_start, push
  the scripts by calling event_push, commits the event by
  calling event_commit, and cleanup afterwards by calling
  event_cleanup. Before calling this API, user should have
  designated ssbi/grfc/rffe scripts ready. Meanwhile, setup for
  CCS should also be ready, namely event_id, event_settings,
  transaction_params, and shared memory parameters.
 
  @param event_id 
  The event ID to start
 
  @param device_script
  Pointer to rf script buffer object arrays. Mutiple device script/task
  for the same event building is supported
 
  @param num_tasks
  number of tasks of the event
 
  @param rflm_handle
  RFLM handle associated with the event for pushing the script to DM
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_build_ccs_event
(
  uint16 event_id,
  rfwcdma_mdsp_device_script_type *device_script,
  uint8 num_tasks,
  rflm_dm_handle_id_t rflm_handle
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function update the event specific settings in the RF-MDSP data module
 
  @details
 
  @param event_id 
  The event ID to start
 
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_ccs_update_event_settings
(
  uint16 event_id
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clean up event shared memory by aborting event
 
  @details 
  This API cleans up shared memory event status bit field by calling abort event 
   
  @param event ID
  event ID to abort
 
  @return
  True for successfully finished ccs event sequence. False for not.
 
*/
boolean
rf_wcdma_mdsp_ccs_abort_event
(
  uint16 event_id
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  associats the script event with RFD DM buffer. 
 
  @details
  This API should be used for script association
  Associate an event ID with RFD DM script buffer and RFLM handle.
 
  @param event_id 
  The event ID to start
 
  @param rflm_handle for this task
  
  @return
  Flag indicating the function execution success
*/
boolean rfwcdma_mdsp_associate_event_id
(
  uint16 event_id,
  rflm_dm_handle_id_t rflm_handle
);

/*----------------------------------------------------------------------------*/
const rf_hal_bus_resource_script_type* rfwcdma_mdsp_get_resource_script_from_buffer
(
  rf_buffer_intf *buffer_ptr
);

#endif /* RFWCDMA_MDSP_CCS_EVENTS_H */
