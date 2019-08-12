#ifndef RFCOMMON_AUTOPIN_H
#define RFCOMMON_AUTOPIN_H

/*!
  @file
  rfcommon_autopin.h

  @brief
  This module contains internal prototypes and definitions used by common AutoPin.
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/autopin/inc/rfcommon_autopin.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/05/15   whz     Move some interface to another file 
11/02/15   kma     Added flag to enable WTR debug mem dump
10/29/15   whz     Remove timers completely
10/22/15   kma     Add capture control and pin update API for PAR algo  
08/03/15   kg      Add header "rfcommon_core_txlin_types.h" 
06/04/15   whz     Initial version

==============================================================================*/

#include "rfcommon_autopin_api.h"
#include "rfcommon_core_txlin_types.h"
#include "rf_dispatch.h"

/*WTR debug mem size*/
#define WTR_DEBUG_MEM_SIZE 4096

extern boolean wtr_debug_mem_enable;

extern uint8 wtr_debug_mem[WTR_DEBUG_MEM_SIZE];

void rfcommon_autopin_get_nv_info(rfm_mode_enum_type rfm_tech);

rflm_tech_id_t rfcommon_autopin_mc_convert_rfm_mode_to_rflm_tech(rfm_mode_enum_type rfm_tech);

void rfcommon_autopin_capture_control(rflm_handle_tx_t handle, rfcommon_autopin_capture_control_type capture_input);

void rfcommon_autopin_update_pin(rflm_handle_tx_t handle, int16 wtr_par, int16 reflog_par, uint16 peak_avg_pwr );

void rfcommon_autopin_reflog_done_response(void *req_ptr,
                                           rf_dispatch_cid_info_type *cid_info,
                                           void *cb_data);

#endif /* RFCOMMON_AUTOPIN_H */
