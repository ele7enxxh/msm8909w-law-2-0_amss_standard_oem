#ifndef RFCOMMON_MDSP_TUNER_H
#define RFCOMMON_MDSP_TUNER_H

/*! 
  @file
  rfcommon_mdsp_tuner.h
 
  @brief
  Thie module defines the MDSP interfaces related to tuner update eventss
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_common/inc/rfcommon_mdsp_tuner.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/06/13   zhw      Initial Revision.

==============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rf_hal_buffer.h"
#include "fw_rf_common_intf.h"
#include "rfcommon_mdsp.h"
#include "rf_dispatch_snum.h"
#include "rflm_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                       Common Antenna Tuner Update Module Definitions
=============================================================================*/

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_mdsp_tuner_open_loop_queue_script
(
  void* tuner_open_loop_buffer,
  uint32 tuner_open_loop_event_id,
  rf_buffer_intf *tuner_open_loop_script,
  rfcommon_mdsp_event_handle** tuner_open_loop_event_handle_ptr,
  rf_dispatch_snum_handler_fp_type fwrsp_callback,
  void *tuner_open_loop_cb_data
);

/*----------------------------------------------------------------------------*/
void 
rfcommon_mdsp_tuner_open_loop_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_tuner_open_loop_check_aborted_event
(
  void* tuner_update_buffer_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_tuner_open_loop_reset_status
(
  void* tuner_update_buffer_ptr
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_mdsp_tuner_closed_loop_queue_script
(
  void* tuner_closed_loop_buffer,
  uint32 tuner_closed_loop_event_id,
  rf_buffer_intf *tuner_closed_loop_script,
  rfcommon_mdsp_event_handle** tuner_closed_loop_event_handle_ptr,
  rf_dispatch_snum_handler_fp_type fwrsp_callback,
  void *tuner_closed_loop_cb_data,
  uint32 time_out_value_ms,
  int32 tx_agc_threshold,
  uint32 settling_time_us
);

/*----------------------------------------------------------------------------*/
void 
rfcommon_mdsp_tuner_closed_loop_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
uint16 
rfcommon_mdsp_tuner_closed_loop_get_meas_results 
(
  rfcommon_mdsp_event_handle* tuner_event_handle_ptr,
  rf_hal_bus_rffe_type *tuner_script,
  uint16 max_size
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_tuner_closed_loop_check_timeout_event
(
  void* tuner_update_buffer_ptr
);

/*----------------------------------------------------------------------------*/
int32
rfcommon_mdsp_tuner_closed_loop_get_txagc_level
(
  void* tuner_update_buffer_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_mdsp_tuner_closed_loop_reset_status
(
   void* tuner_update_buffer_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MDSP_H */

