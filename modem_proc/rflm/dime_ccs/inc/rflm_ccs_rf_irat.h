#ifndef RFLM_CCS_RF_IRAT_H
#define RFLM_CCS_RF_IRAT_H

/*!
  @file
  rflm_ccs_rf_irat.h

  @brief
  Provides API to program RF script during IRAT for all techs
*/

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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dime_ccs/inc/rflm_ccs_rf_irat.h#1 $ 
$Date: 2016/12/13 $   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
07/31/13   ra     Initial version
==============================================================================*/


#include "rflm_ccs_rf_event.h"
#include "rflm_ccs_rf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_ccs_irat_intf.h" 

#define FEATURE_RF_BOLT_PRE_SILICON
#ifndef FEATURE_RF_BOLT_PRE_SILICON
#include "fw_crm.h"
#endif 

#ifndef FEATURE_RF_BOLT_PRE_SILICON
rflm_ccs_event_info_s rflm_ccs_enqueue_common_rf_irat_event(
  /*!< Head pointer for event (including header and tasks) */
  rfmeas_event_infor_type *rfm_p,
  /*!< Start time of script, if time needs to be immediate, 
    pass FW_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset,
  /*!< CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*!< Length of destination CCS memory in bytes */
  uint32 length_bytes,
  /*!< Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback, 
  /*!< Argument to callback function */
  void *callback_arg,          
  /*!< TQ for programming the CCS RF tasks */
  rflm_ccs_rf_tq_handle_t *tq
  );

rflm_ccs_event_info_s rflm_ccs_program_common_rf_script_pair(
   /*!< Buffer Index for script */
   uint32 buf_idx,
   /*!< Start time of script, if time needs to be immediate, 
     pass FW_RF_EVENT_TIME_IMMEDIATE */
   uint32 start_offset,
   /*!< CCS desination memory for SSBI/ RFFE addr/data information. 
     This is pointer in CCS memory */
   uint32 *ccs_dest_ptr,
   /*!< Length of destination CCS memory in bytes */
   uint32 length_bytes,
   /*!< Callback function for task completion */
   rflm_ccs_rf_task_callback_t callback, 
   /*!< Argument to callback function */
   void *callback_arg,          
   /*!< TQ_0 for programming the GRFC/ SBI/ RFFE tasks */
   rflm_ccs_rf_tq_handle_t *tq0_handle,
   /*!< TQ_1 for programming the GRFC/ SBI/ RFFE tasks 
        Assign NULL if only use one TQ */
   rflm_ccs_rf_tq_handle_t *tq1_handle,
   /*!< Tech doing tuning */
//#ifndef FEATURE_RF_BOLT_PRE_SILICON
//   fw_crm_rat_e rat
//#endif 
  );

rflm_ccs_event_info_s rflm_ccs_program_common_rf_script(
  /*! Buffer Index for script buffer */
  uint32 buf_idx,
  /*! Start time of script, if time needs to be immediate, 
    pass FW_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset,
  /*! CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*! Length of destination CCS memory in bytes */
  uint32 length_bytes,
  /*!< Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback, 
  /*!< Argument to callback function */
  void *callback_arg,          
  /*! TQ for programming the GRFC tasks */
  rflm_ccs_rf_tq_handle_t *tq,
  /*! Tech doing tuning */
  fw_crm_rat_e rat
  );

void rflm_ccs_irat_script_start_time_callback ( 
  /*! RXLM buffer ID */
  uint32 rxlm_buf_index,
  /*! Tech doing callback */
  fw_crm_rat_e rat );
#endif 
rfmeas_event_infor_type * rflm_ccs_get_irat_script_addr(
  /*!< Buffer Id */
  uint32 buf_idx );
rfmeas_event_infor_type * rflm_ccs_get_rfcmd_event_addr( void );

#endif  /*RFLM_CCS_RF_IRAT_H*/

