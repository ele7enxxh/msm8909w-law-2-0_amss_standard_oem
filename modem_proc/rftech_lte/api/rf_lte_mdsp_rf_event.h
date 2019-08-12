#ifndef RFLTE_MDSP_RF_EVENT_H
#define RFLTE_MDSP_RF_EVENT_H
/*! 
  @file
  rf_lte_mdsp_rf_event.h
 
  @brief
  This file defines the RF events.
*/

/*==============================================================================

  Copyright (c) 2012 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rf_lte_mdsp_rf_event.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/13   php     Add ERR FATAL if CCS event creation fails at any step 
04/12/13   php     Add priority for CCS events
10/31/12   vss     Add parameter to change start_delta in the resource script; add parameter to 
                   start_push_commit API for using time conversion or not. 
10/02/12   vss     Add support for multiple script push in CCS format
07/16/12   vss     Initial Version
 
==============================================================================*/

#include "comdef.h"
#include "rf_hal_buffer.h"

/*----------------------------------------------------------------------------*/
void rf_lte_mdsp_rf_event_start_push_commit_to_memory
(
    uint32 event_id, 
    uint8 num_scripts, 
    const rf_hal_bus_resource_script_type ** script_ptr_list, 
    void * shared_mem_buf_ptr, 
    uint32 shared_mem_max_size_bytes, 
    boolean is_issue_seq,
    uint8 max_tasks_allocated,
    boolean use_timer_conversion,
    int16 time_offset,
    uint8 event_priority
);
/*----------------------------------------------------------------------------*/
int16 rflte_mdsp_time_convert_lstmr_to_ustmr
( 
  int16 ts
);

#endif /* RFLTE_MDSP_RF_EVENT_H */
