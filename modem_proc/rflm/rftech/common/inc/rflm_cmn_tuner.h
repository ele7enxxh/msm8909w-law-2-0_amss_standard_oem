/*!
  @file
  rflm_cmn_tuner.h

  @brief
  RFLM common tuner events
  
  @detail
  Provides the following API's for TECH RFLM to call:
  - TUNER AOL EVENT UPDATE
  - TUNER CL EVENT UPDATE
 
  @addtogroup RFLM_CMN_TUNER
  @{
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/common/inc/rflm_cmn_tuner.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
23/01/15  ndb     Added the prototype of the API rflm_cmn_tuner_open_loop_abort_pending_snum()
12/04/14  ndb     Added API's to support FBRx based CL
11/26/14  ac      Workaround to ABORT all pending Snums during disable Tx
10/28/14  ndb/yb   SNUM debug code 
04/28/14  kg       Change to CL Timeout API
3/26/14   kg       Add new Tuner CL allocate/deallocate DM API's 
3/12/14   kg       Add additional paramater to 
                   rflm_cmn_tuner_closed_loop_enqueue_event & 
                   rflm_cmn_tuner_open_loop_enqueue_event 
12/12/13   zhw     Documentation Update
12/02/13   zhw     Initial Revision

===========================================================================*/

#ifndef RFLM_CMN_TUNER_H
#define RFLM_CMN_TUNER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "rflm_cmn_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/

/*Structure for debugging the SNUM Crash issue (Common for AOL/CL)*/

/*----------------------------------------------------------------------------*/
typedef struct{
  uint32 seq_num;
  boolean abort_event;
  boolean abort_done;
  boolean resp_rcvd;
  rflm_cmn_tuner_event_status_type tuner_event_status;
  uint32 mode;
  uint32 lm_handle;
  boolean clear;
  boolean lm_enque;
}rflm_snum_debug;


/*Expose to all the modules*/
extern rflm_snum_debug rflm_aol_debug_array[10]; 
extern rflm_snum_debug rflm_cl_debug_array[10];
extern uint8 rflm_aol_debug_index ;
extern uint8 rflm_cl_debug_index ;

/*----------------------------------------------------------------------------*/
rflm_cmn_tuner_event_status_type 
rflm_cmn_tuner_event_get_status
(
   rflm_cmn_tuner_event_data_type* open_loop_event_ptr,
   rflm_cmn_tuner_event_data_type* closed_loop_event_ptr
);

/*----------------------------------------------------------------------------*/
rflm_ccs_event_info_s 
rflm_cmn_tuner_open_loop_enqueue_event
( 
  rflm_cmn_tuner_event_data_type* tuner_event_ptr,
  uint32 start_offset,
  uint32 *ccs_dest_ptr,
  uint32 length_bytes,
  rflm_ccs_rf_tq_handle_t *tq,
  boolean register_ccs_callback
);

/*----------------------------------------------------------------------------*/
uint32 
rflm_cmn_tuner_open_loop_abort_event
(
  rflm_cmn_tuner_event_data_type* tuner_event_ptr
);

/*----------------------------------------------------------------------------*/
rflm_ccs_event_info_s 
rflm_cmn_tuner_closed_loop_enqueue_event
(  
  rflm_cmn_tuner_event_data_type* tuner_event_ptr,
  uint32 start_offset,
  uint32 *ccs_dest_ptr,
  uint32 length_bytes,
  rflm_ccs_rf_tq_handle_t *tq,
  int32 tx_agc_power,
  boolean register_ccs_callback
);

/*----------------------------------------------------------------------------*/
rflm_cmn_tuner_setting_type* 
rflm_cmn_tuner_closed_loop_get_setting
(
  rflm_cmn_tuner_event_data_type* closed_loop_event_ptr 
);

/*----------------------------------------------------------------------------*/
uint32 
rflm_cmn_tuner_closed_loop_time_out_event
(
  rflm_cmn_tuner_event_data_type* closed_loop_event_ptr,
  int32 tx_agc_power
);

/*----------------------------------------------------------------------------*/
boolean 
rflm_cmn_tuner_cl_allocate_memory
( 
uint32 handle_id
);

/*----------------------------------------------------------------------------*/
boolean rflm_cmn_tuner_is_fbrx_vswr_meas_pending
(
  rflm_handle_tx_t handle_id,
  uint32 current_action_time,
  int32 current_txagc
);

/*----------------------------------------------------------------------------*/
uint32 
rflm_cmn_tuner_closed_loop_fbrx_time_out_event
(
  rflm_cmn_event_fbrx_cl_data_type* closed_loop_event_ptr,
  int32 tx_agc_power
);

/*----------------------------------------------------------------------------*/
void rflm_cmn_tuner_closed_loop_fbrx_vswr_meas_done
(
  uint32 sequence_number
);

/*----------------------------------------------------------------------------*/
void
rflm_cmn_tuner_open_loop_abort_pending_snum
(
uint32 token
);

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_CMN_TUNER_H */
