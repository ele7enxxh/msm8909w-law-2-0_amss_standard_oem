#ifndef RFLM_CCS_RF_EVENT_H
#define RFLM_CCS_RF_EVENT_H

/*!
  @file
  rflm_ccs_event.h

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
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dime_ccs/inc/rflm_ccs_rf_event.h#1 $ 
$Date: 2016/12/13 $   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
11/19/13   ra     Export data structure and prototype in RFLM CCS API component
07/31/13   ra     Initial version
==============================================================================*/
#include "rflm_ccs_rf_irat.h"
#include "rflm_ccs_rf.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Indicator for immediate time CCS RF task programming */
#define RFLM_CCS_RF_EVENT_TIME_IMMEDIATE 0x7FFFFFFF

/*! @brief Delta in microseconds to be added to immediate action time */
/* If task is immediate it does not matter if it goes out in 40us */
#define RFLM_CCS_RF_EVENT_IMMEDIATE_TIME_DELTA_US     40

/*! @brief Margin to be added to immediate action time */
#define RFLM_CCS_RF_UNIV_STMR_IMM_MARGIN ((uint32)(19200000/1000000.0*RFLM_CCS_RF_EVENT_IMMEDIATE_TIME_DELTA_US + 1))



void rflm_ccs_program_common_event_task_params ( 
  /*!< Whether interrupt is enabled */
  uint32 int_enable,
  /*!< Start time for the event */
  uint32 start_offset,
  /*!< Pointer RF task that needs to be generated 
    for copy to FW memory or for enqueue */
  ccs_rf_task_t *ccs_task_ptr,
  /*!< Task descriptor pointer */
  ccs_rf_task_t *task_desc_p );

void rflm_ccs_program_grfc ( 
  /*! Source CCS RF task pointer, with common fields of task set */
  ccs_rf_task_t *rflm_ccs_rf_inp_task_p,
  /*! Task descriptor pointer */
  ccs_rf_task_grfc_write_t *task_desc_p,
  /*! Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback, 
  /*! Argument to callback function */
  void *callback_arg,
  /*! TQ for programming the GRFC tasks */
  rflm_ccs_rf_tq_handle_t *grfc_tq );

uint32 rflm_ccs_program_sbi_rffe (
  /*! Source CCS RF task pointer, with common fields of task set */
  ccs_rf_task_t *rflm_ccs_rf_inp_task_p,
  /*! Task descripor pointer  */
  ccs_rf_task_buf_item_t *task_desc_p,
  /*! CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*! Length of destination CCS memory in bytes */
  uint32 length_bytes,
  /*! Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback, 
  /*!< Argument to callback function */
  void *callback_arg,          
  /*! TQ for programming the SBI/ RFFE tasks */
  rflm_ccs_rf_tq_handle_t *sbi_tq );



/*! @brief Structure containing header and tasks for any event */


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  /*RFLM_CCS_RF_EVENT_H*/

