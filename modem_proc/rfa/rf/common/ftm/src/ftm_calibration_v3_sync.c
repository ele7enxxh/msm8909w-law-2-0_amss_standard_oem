/*!
  @file
  ftm_calibration_v3_sync.c

  @brief
  This module contains prototypes and definitions used by common RF calibration
  Sync OpCode.

  @addtogroup RF_COMMON_FTM_CALV3_SYNC
 
  @brief
  This module provides a Sync extensions to calibration v3 framework
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_sync.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/15/13   cri     Added SYNC opcode support
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/02/13   aro     Debug message update
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/27/13   aro     Data structure restructing
11/19/12   aro     NULL pointer fix for Sync unpack pointer
11/05/12   aro     Interface to query the offset to be applied during
                   ISR scheduling
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Added process instruction function
11/05/12   aro     Code to unpack sync opcode
11/05/12   aro     Initial version of Sync OpCode

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_sync.h"
#include "ftm_msg.h"

#include "stringl.h" /* for memscpy */
#ifdef FEATURE_LTE
#include "rflte_ftm_calibration.h"
#endif

/*============================================================================*/
/*!
  @name Sync OpCode handling

  @brief
  This section contains functions for Sync opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Sync OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Sync OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_sync_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_sync_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.sync);

  reg_data.unpack_fp = ftm_calibration_sync_unpack;
  reg_data.process_fp = ftm_calibration_sync_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_SYNC,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = NULL;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_v3_sync;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = NULL;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = NULL;
  #endif
    

  return ret_val;

} /* ftm_calibration_sync_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Sync OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Sync OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_sync_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_sync_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Radio Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Radio Setup OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_sync_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size = 0; /* Payload Size */
  uint8 *payload_ptr = NULL; /* Paylaod pointer */
  ftm_cal_sync_data_type *cfg_data = NULL;
  ftm_cal_sync_parameter_type *sy_unpack = NULL;

  /* NULL Pointer check */
  if ( ( instr_payload == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][SYNC.{0x%x}] "
               "Unpack : Null Argument",
               FTM_CAL_OPCODE_SYNC );
    return 0;
  } /* if ( instr_payload == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.sync);

  /* Get pointer to memory to hold Sync unpacked data */
  sy_unpack = &(cfg_data->up_param);

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;

  /* Extarct Device Data */
  sy_unpack->debug_en = 
    ((ftm_cal_sync_instr_payload_type*)payload_ptr)->debug_mode_en;
  sy_unpack->flag = ((ftm_cal_sync_instr_payload_type*)payload_ptr)->flag;
  sy_unpack->sync_event = 
    ((ftm_cal_sync_instr_payload_type*)payload_ptr)->sync_event;

  /* Update the payload size and Payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     ( sizeof(ftm_cal_sync_instr_payload_type) - 
                       sizeof(ftm_cal_sync_event_payload_type) ) );

  /* Based on the sync event copy the payload and update the pointers */
  switch ( sy_unpack->sync_event )
  {
  case SYNC_EVENT_TX_SYSTEM_BOUNDARY:
    memscpy( &(sy_unpack->event_payload.tx_sys_boundary.device),
             sizeof(ftm_cal_sync_tx_sys_boundary_payload_type),
             payload_ptr, sizeof(ftm_cal_sync_tx_sys_boundary_payload_type) );
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                       sizeof(ftm_cal_sync_tx_sys_boundary_payload_type) );
    break;
  default:
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][SYNC.{0x%x}] "
               "Unpack : Invalid Event %d",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_SYNC,
               sy_unpack->sync_event  );
    break;
  } /* switch ( sy_unpack->sync_event ) */

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  if ( sy_unpack->debug_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][SYNC.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_SYNC,
               payload_size );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */

  /* return Payload size */
  return payload_size;

} /* ftm_calibration_sync_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Sync OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Sync OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_sync_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  int32 offset_us = 0;
  boolean ret_val = TRUE;
  ftm_cal_sync_get_offset_fp_type sy_fp;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_sync_data_type *cfg_data;
  ftm_cal_sync_instr_payload_type *p_ptr;
  
  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][SYNC.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_SYNC );
    return 0;
  } /* if ( cal_data == NULL ) */

  p_ptr = (ftm_cal_sync_instr_payload_type*)oc_disp->payload_ptr;

  cfg_data = &(cal_data->seg_data.oc_payload.sync);

  /* Get current mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Make sure that the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /* Get the Tx override Data */
    sy_fp = cfg_data->oc_process_fp[current_rf_mode];

    /* Execute the callback function only if it is valid */
    if ( sy_fp != NULL )
    {
      offset_us = sy_fp( &(cfg_data->up_param) );

      /* Store the Offset in micro seconds */
      cal_data->status.isr_duration_offset = offset_us;

      cal_data->status.apply_isr_offset = TRUE;
    } /* if ( sy_fp->cb_func != NULL ) */
    else
    {
      FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][SYNC.{0x%x}] "
                 "Process : Null FP %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_SYNC,
                 current_rf_mode );
      ret_val = FALSE;
    } /* if ! ( rs_data->cb_func != NULL ) */
  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][SYNC.{0x%x}] "
               "Process : Invalid RFM mode %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_SYNC,
               current_rf_mode );
    ret_val = FALSE;
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */

  if ( p_ptr->debug_mode_en == TRUE )
  {
    FTM_MSG_5( FTM_HIGH, "[calv3][Segment %3d][SYNC.{0x%x}] "
               "Process : [Status %d] Event 0x%x Offset %d uS",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_SYNC,
               ret_val,
               cfg_data->up_param.sync_event,
               cal_data->status.isr_duration_offset );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */

  /* return */
  return ret_val;

} /* ftm_calibration_sync_process */

/*! @} */

/*============================================================================*/
/*!
  @name Sync OpCode handling

  @brief
  This section contains functions for Sync opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the offset generated by previous sync opcode
 
  @details
  This function will return the sync offset that needs to be applied during
  ISR scheduling. Since this offset should be applied only once, the value
  will be reset back to zero after the offset is sent back.
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @return
  The time offset value in microseconds that needs to be applied during
  ISR rescheduling.
*/
int32
ftm_calibration_sync_get_offset
(
  struct ftm_calibration_data_type_t *cal_data,
  rf_time_tick_type start_sweep_tick
)
{
  int32 offset_us = 0;
  rf_time_type segment_duration_time;
  
  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_calibration_sync_get_offset: "
                        "NULL Argument" );
    return 0;
  } /* if ( cal_data == NULL ) */

  if ( cal_data->status.apply_isr_offset == TRUE )
  {
    segment_duration_time = rf_time_get_elapsed( start_sweep_tick, RF_USEC );
    /* Store the Offset in micro seconds */
    offset_us = cal_data->status.isr_duration_offset + 
                segment_duration_time;

    FTM_MSG_3( FTM_HIGH, "OC.0x1000 SYNC_ISR: Offset [ %d -> %d + %d ] uS", 
               offset_us, cal_data->status.isr_duration_offset,
               segment_duration_time );

    /* Invalidate the offset */
    cal_data->status.isr_duration_offset = 0;
    cal_data->status.apply_isr_offset = FALSE;
  }

  /* return */
  return offset_us;

} /* ftm_calibration_sync_get_offset */

/*! @} */

/*! @} */

