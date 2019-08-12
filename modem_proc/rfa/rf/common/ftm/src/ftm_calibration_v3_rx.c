/*!
  @file
  ftm_calibration_v3_rx.c

  @brief
  This module contains common RF calibration code for Rx measurement

  @addtogroup RF_COMMON_FTM_CALV3_RX

  @brief
  This module provides a common framework to perform Measure Rx (0x4000) OpCode
  operation. This module is responsible for calling the WAN technology specific
  handler function to perform various Measure Rx operation. This module is also
  responsible for populating the Rx results buffer with appropriate calibration
  data.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_rx.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
1/13/15    vs      Init Rx settling time for WCDMA during Rx config
30/10/13   pk      Added F3s enable/disable conditions to optimize the cal time
09/18/13   jmf     [CalV4] Support for Raw Rx AGC read in RX_MEASURE OpCode for FBRxCal
09/10/13   aa      Enable/Disable NBEE RXCAL on the fly
08/14/13   aro     Support for DC calibration in RX_MEASURE OpCode
07/23/13   ych     Initialized calv4 opcodes 
06/27/13   sar     Fixed compiler error due to a typo. 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/07/13   zhw     Reduce 1x settling time to 900 us
04/07/13   php     Add NBEE settling time of 1000us
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Changed the settling time to #define
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Debug message update
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
04/01/13   aro     KW fix
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/27/13   aro     Data structure restructing
12/06/12   aro     Fixed the bug in free_size check
12/05/12   aro     Code to initialize Measurement mask
12/05/12   aro     Off-target compiler warning fix
12/05/12   aro     Fixed accounting of version field
12/03/12   aro     Doc update
12/03/12   aro     Fixed infinite loop bug
11/30/12   aro     Fixed memset in unpacking function
11/30/12   aro     Added free buffer size check
11/21/12   aro     Support for RX_MEASURE result 
11/15/12   aro     Tx Override 1x handler function
11/15/12   aro     Fixed the sequence of bit mask extraction
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Removed OpCode init from opcode modules to common cal v3
                   init function
11/05/12   aro     Re-organized cal v3 data structure to be more modular
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking
10/31/12   aro     Updated oayload size query function to decrypt teh payload
                   to send it to callback function during opcode processing
10/31/12   aro     Function to process Tx override Opcode
10/31/12   aro     Added function to query the paylaod size of the current
                   Tx override opcode
10/30/12   aro     Interface to initialize tx override opcode
10/30/12   aro     Interface to deinitialize tx override opcode
10/29/12   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "stringl.h" /* for memscpy */
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_rx.h"
#include "ftm_msg.h"
#include "rfcommon_math.h"
#include "modem_mem.h"

#ifdef FEATURE_CDMA1X
#include "ftm_1x_calibration.h"
#endif
#ifdef FEATURE_LTE
#include "rflte_ftm_calibration.h"
#endif
#ifdef FEATURE_WCDMA
#include "ftm_wcdma_calv3.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_calv3.h"
#endif

#ifdef FEATURE_CDMA1X
/*----------------------------------------------------------------------------*/
/*! RxAGC settling time in microseconds to be used during Calibration v3 */
#define FTM_1X_CAL_V3_RXAGC_SETTLING_TIME_US                                 900
                
#endif

/*============================================================================*/
/*!
  @name Rx Config OpCode handling

  @brief
  This section contains functions for Rx Config opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Rx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Rx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_config_rx_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_config_rx_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.config_rx);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_config_rx_unpack;
  reg_data.process_fp = ftm_calibration_config_rx_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_CONFIG_RX,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_sweep_configure_rx;
  cfg_data->rxagc_settling_time_us[RFM_1X_MODE] = 
                             FTM_1X_CAL_V3_RXAGC_SETTLING_TIME_US;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_configure_rx;
  cfg_data->rxagc_settling_time_us[RFM_LTE_MODE] = 
                             FTM_LTE_CALV3_NBEE_SETTLING_TIME;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_sweep_configure_rx;
  cfg_data->rxagc_settling_time_us[RFM_IMT_MODE] = 
                             ftm_wcdma_calv3_get_rx_settling_time();
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                             ftm_tdscdma_calv3_sweep_configure_rx;
  cfg_data->rxagc_settling_time_us[RFM_TDSCDMA_MODE] = 
                             FTM_TDSCDMA_CALV3_RX_AGC_SETTLING_TIME;
  #endif
    

  return ret_val;

} /* ftm_calibration_config_rx_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Rx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Rx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_config_rx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_config_rx_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Rx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Rx Config OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_config_rx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_config_rx_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_CONFIG_RX,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_config_rx_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Rx Config OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Rx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_config_rx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_config_rx_data_type *cfg_data;
  ftm_cal_instr_payload_config_rx_type *p_ptr;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.config_rx);
  p_ptr = (ftm_cal_instr_payload_config_rx_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_CFG_RX_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RX );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Get Current Mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Init settling for WCDMA as NBEE is enabled only during Rx config*/
#ifdef FEATURE_WCDMA
   cfg_data->rxagc_settling_time_us[RFM_IMT_MODE] = ftm_wcdma_calv3_get_rx_settling_time();
 #endif

  /* Execute only if the current mode is valid */
  if ( current_rf_mode < RFM_NUM_MODES  )
  {
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /* Call tech specific function to process opcode */
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])( p_ptr );

      /* Error Check */
      if ( ret_val == FALSE )
      {
        /* Flag segment error */
        cal_data->status.seg_error_log.seg_error |= 
                           (1 << FTM_CAL_SEG_ERR_CFG_RX_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
                   "Process : FP Failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_CONFIG_RX );
      } /* if ( ret_val == FALSE ) */
      else
      {
        /* Start Time profiling to find the total time taken by
        Tx measurement. This will be used to substract from 
        the RxAGC settling time to decide on the additional wait before Rx 
        Measurement.*/
        if ( cfg_data->rx_prof_in_progress == FALSE )
        {
          /* Time profiling is started only if rx_meas_prof_in_progress = 0.
          Rx Configuration and Measurement can happen in two diff segment.
          If they happen in two different segments and the time interval is 
          still less than the RxAGC settling time, then additional clock busy 
          wait should be done before RxAGC Measurment. Therefore 
          rf_timetick_get_difference for this profiling should be done only
          when Rx  Meas opcode is sent. Therefore to avoid the loss of 
          timetick_get value of rx_cfg_and_meas_lapse, this variable is 
          defined static and is reset to 0, only after 
          rf_timetick_get_difference is done. */
          cfg_data->rx_cfg_time = rf_time_get_tick();
          cfg_data->rx_prof_in_progress = TRUE;
        } /* if ( rx_cfg_and_meas_lapse == 0 ) */
      } /* if ! ( ret_val == FALSE ) */
    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_CFG_RX_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_CONFIG_RX );
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RX );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][CONFIG_RX.{0x%x}] "
               "Process : [Status %d] - dMask 0x%x PM %d CalAction 0x%x",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RX,
               ret_val,
               p_ptr->device_flag,
               p_ptr->power_mode,
               p_ptr->cal_action );
 } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_config_rx_process */

/*! @} */

/*============================================================================*/
/*!
  @name Meas Rx OpCode handling

  @brief
  This section contains functions for Meas Rx opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Meas Rx OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Meas Rx OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_meas_rx_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_meas_rx_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_rx);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_meas_rx_unpack;
  reg_data.process_fp = ftm_calibration_meas_rx_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;
  if ( cfg_data->rx_result.rx_buffer == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->rx_result.rx_buffer = modem_mem_alloc( 
                                          FTM_CAL_SEG_RX_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->rx_result.rx_buffer != NULL )
    {
      cfg_data->rx_result.max_rx_buffer_len = FTM_CAL_SEG_RX_BUFF_SIZE_MAX;
      cfg_data->rx_result.num_of_rx_reads = 0xFF;
      reg_data.res_buff_info.buff_ptr = cfg_data->rx_result.rx_buffer;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_SEG_RX_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->rx_result.rx_buffer != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
                 "Init : Result buff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_MEAS_RX );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->rx_result.rx_buffer != NULL ) */
  } /* if ( cfg_data->rx_result.rx_buffer == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "Init : Buff not Null",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_RX );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */


  /* Register OpCOde */
  if ( ret_val == TRUE )
  {
    ret_val = ftm_calibration_register_opcode( cal_data,
                                               FTM_CAL_OPCODE_MEAS_RX,
                                               reg_data );
  } /* if ( ret_val == TRUE ) */

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_sweep_measure_rx;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_measure_rx;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_sweep_measure_rx;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                          ftm_tdscdma_calv3_sweep_measure_rx;
  #endif
    
  return ret_val;

} /* ftm_calibration_meas_rx_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Meas Rx OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Meas Rx OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_meas_rx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_meas_rx_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_rx);

  if ( cfg_data->rx_result.rx_buffer != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->rx_result.rx_buffer, MODEM_MEM_CLIENT_RFA );
    cfg_data->rx_result.rx_buffer = NULL;
  } /* if ( cfg_data->rx_result.rx_buffer != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "DeInit : Null Buff",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_RX );
    ret_val = FALSE;
  } /* if ! ( cfg_data->rx_result.rx_buffer != NULL ) */

  return ret_val;
} /* ftm_calibration_meas_rx_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Meas Rx OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Meas Rx OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_meas_rx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_meas_rx_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_MEAS_RX,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_meas_rx_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Meas Rx OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Meas Rx OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_meas_rx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_meas_rx_data_type *cfg_data;
  ftm_cal_instr_payload_meas_rx_type *p_ptr;
  uint8 expected_meas_num = 1;
  int32 additional_rx_agc_wait = 0; /* Additional Wait time required for
  Rx AGC to settle. This will be calculated by taking the difference between
  rx_cfg_and_meas_lapse and rx_agc settling time*/

  ftm_cal_config_rx_data_type *cfg_rx_data;
  rf_time_type elapsed_time;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_rx);
  cfg_rx_data = &(cal_data->seg_data.oc_payload.config_rx);
  p_ptr = (ftm_cal_instr_payload_meas_rx_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_MEAS_RX_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_RX );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Get Current Mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Execute only if the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  )
  {
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /*------------------------------------------------------------------*/
      /* Compute if additional clock busy wait is required before the
         start of RxAGC Sampling */
      /*------------------------------------------------------------------*/

      if ( cfg_rx_data->rx_prof_in_progress == TRUE )
      {
        /* Stop Time profiling to find the total time taken by Tx
        Configuration and/or Tx measurement and/or additional segment between 
        Rx Config and Rx Measure opcodes. This will be used to substract from 
        the RxAGC settling time to decide on the additional wait before Rx 
        Measurement */
        elapsed_time = rf_time_get_elapsed( cfg_rx_data->rx_cfg_time,
                                            RF_USEC );

        /* Calculate the additional wait time by taking the difference of the
        time taken by Tx Cfg - measurement and Total RxAGC settling time */
        additional_rx_agc_wait = 
          ( cfg_rx_data->rxagc_settling_time_us[current_rf_mode] - 
            elapsed_time );
        
        /* If Tx configuraion and Measurement take more than actual RxAGC
        settling time, then there is no need to wait. Thus set the wait time 
        to 0.*/
        if ( additional_rx_agc_wait < 0 )
        {
          additional_rx_agc_wait = 0;
        }
        
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_5( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
                     "Waiting for %d ( %d - %d ) ms before RxAGC Measurement", 
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_MEAS_RX,
                     additional_rx_agc_wait,
                     cfg_rx_data->rxagc_settling_time_us[current_rf_mode],
                     elapsed_time );

        } /* if ( cal_data->debug_mode_en == TRUE ) */
        
        /* Perform Clock Busy wait */
        DALSYS_BusyWait( additional_rx_agc_wait );
        
          /* Reset rx_meas_prof_in_progress; so that time profiling can start
        for next Rx Measure Opcode. */
        cfg_rx_data->rx_prof_in_progress = FALSE;
      } /* if ( cfg_rx_data->rx_prof_in_progress == TRUE ) */

      /* Call tech specific function to process opcode */
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])( p_ptr,
                                                       &(cfg_data->rx_result) );

      /* Error Check */
      if ( ret_val == FALSE )
      {
        /* Flag segment error */
        cal_data->status.seg_error_log.seg_error |= 
                           (1 << FTM_CAL_SEG_ERR_MEAS_RX_FAILURE);
        if (cal_data->cal_config.debug_mode_en)
        {		
        FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
                   "Process : FP Failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_MEAS_RX );
	}
      } /* if ( ret_val == FALSE ) */
      else
      {
        /* Count the total number of expected measurement */
        expected_meas_num = 1;
        expected_meas_num *= rf_count_one_bits(p_ptr->cal_action);
        expected_meas_num *= rf_count_one_bits(p_ptr->device_flag);

        /* Calculate the toal results size */
        oc_disp->res_buff_info.buff_sz = expected_meas_num * 
                             sizeof(ftm_calibration_rx_meas_data_type);
      } /* if ! ( ret_val == FALSE ) */

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_MEAS_RX_NULL_CB);
      if (cal_data->cal_config.debug_mode_en)
      {
      FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_MEAS_RX );
      }
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    if (cal_data->cal_config.debug_mode_en)
    {
    FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_RX );
    }
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_7( FTM_HIGH, "[calv3][Segment %3d][MEAS_RX.{0x%x}] "
               "Process : [Status %d] - dMask 0x%x CalAction 0x%x eAGC %d "
               "PM %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_RX,
               ret_val,
               p_ptr->device_flag,
               p_ptr->cal_action,
               p_ptr->exp_rxagc,
               p_ptr->power_mode );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_meas_rx_process */

/*! @} */


/*============================================================================*/
/*!
  @name Rx Meas Results
 
  @brief
  Functions specific to Rx measurement results
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the results buffer for Rx Measurement
 
  @details
  This function will populate the results buffer with the Rx measurement data
  produced as a result Rx Meas.
 
  @param cal_result
  Pointer to the data structure which contains the Rx Measurement data.
 
  @param rx_meas_result
  Pointer to the memeory where the Rx meas result is to be populated
 
  @param rx_meas_result_size
  Pointer to the variable which needs to be populated with Rx Meas result size
  
  @return
  Flag indicating the status of function
*/
inline static boolean
ftm_cal_rx_measure_populate_results
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp,
  ftm_cal_rx_meas_result_data_type *cal_result
)
{
  boolean ret_val = TRUE; /* return value */ 
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_RX_MEASURE;
  uint8 version = 0;
  uint8 device_mask_1bit = 0xFF; /* Device Mask */
  uint8 device_mask = 0; /* Device Mask storage */
  uint8 device_ind;

  ftm_cal_rx_meas_action_params_mask_type action_mask_1bit = 0xFF; 
  ftm_cal_rx_meas_action_params_mask_type action_mask = 0; 
  ftm_cal_rx_meas_action_enum_type action_ind; /* Param Bit */

  /* Param vaildation */
  if ( ( cal_result == NULL ) || ( oc_disp == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][RX_MEASURE.{0x%x}] "
               "ResultPush : Null Argument",
               FTM_CAL_OPCODE_RX_MEASURE );
    return FALSE;
  } /* if ( ( cal_result == NULL ) || ( oc_disp == NULL ) ) */

  /* Initialize the free size */ 
  free_size = oc_disp->res_buff_info.buff_sz_max;

  /*==========================================================================*/
  /* Populate the results buffer */
  /*==========================================================================*/

  /* Initialize the write pointer */
  res_write_ptr = oc_disp->res_buff_info.buff_ptr;

  /*==========================================================================*/
  /* Populate Results Type */
  /*==========================================================================*/

  if ( free_size >= sizeof(uint8) )
  {
    memscpy( res_write_ptr, sizeof(uint8), &(res_type), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "ResultPush : ResType - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               sizeof(uint8), 
               free_size );
  } /* if ! ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
  /* Populate version Type */
  /*==========================================================================*/

  if ( free_size >= sizeof(uint8) )
  {
    memscpy( res_write_ptr, sizeof(uint8),&(version), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "ResultPush : Version - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               sizeof(uint8), 
               free_size );
  } /* if ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
  /* Populate Device mask */
  /*==========================================================================*/

  if ( free_size >= sizeof(ftm_cal_rx_meas_device_mask_type) )
  {
    memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_device_mask_type), &(cal_result->device_mask), 
            sizeof(ftm_cal_rx_meas_device_mask_type));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                       sizeof(ftm_cal_rx_meas_device_mask_type) );
    free_size -= sizeof(ftm_cal_rx_meas_device_mask_type);
  } /* if ( free_size >= sizeof(ftm_cal_rx_meas_device_mask_type) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "ResultPush : dMask - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               sizeof(ftm_cal_rx_meas_device_mask_type), 
               free_size );
  } /* if ( free_size >= sizeof(ftm_cal_rx_meas_device_mask_type) ) */

  /*==========================================================================*/
  /* Populate RxAGC Result Data */
  /*==========================================================================*/
  
  /* Fetch the device mask sent from tools */
  device_mask = cal_result->device_mask;

  /* For the current device mask, extract the bit mask for the first
  enabled device. The first bit will be from LSB. */
  device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  /* Loop until, the Device Mask comes out to be zero */
  while ( device_mask_1bit != 0 )
  {
    /* Compute the location of the one bit location */
    device_ind = inline_rf_extract_first_one_bit_lsb_location(device_mask_1bit);

    /* Proceed only if the device_ind is valid, to avois array out of bounce */
    if ( device_ind < FTM_CAL_V3_RX_MAX_DEV )
    {

      /*----------------------------------------------------------------------*/
      /* Populate RxAGC Meas Action */
      /*----------------------------------------------------------------------*/
      if ( free_size >= sizeof(ftm_cal_rx_meas_action_params_mask_type) )
      {
        memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_action_params_mask_type),
                &(cal_result->rx_meas_param[device_ind].rx_meas_action),
                sizeof(ftm_cal_rx_meas_action_params_mask_type));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(ftm_cal_rx_meas_action_params_mask_type) );
        free_size -= sizeof(ftm_cal_rx_meas_action_params_mask_type);
      } /*if ( free_size >= sizeof(ftm_cal_rx_meas_action_params_mask_type) )*/
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                   "ResultPush : MeasAction - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_RX_MEASURE,
                   sizeof(ftm_cal_rx_meas_action_params_mask_type), 
                   free_size );
      } /* if ( free_size >= sizeof(ftm_cal_rx_meas_action_params_mask_type))*/
      
      /* Initialize the Mask */
      action_mask_1bit = 0xFFFF;

      /* Initialize the Rx meas action */
      action_mask = cal_result->rx_meas_param[device_ind].rx_meas_action;
      
      /* For the current param mask, extract the bit mask for the first
      enabled rx param. The first bit will be from LSB. */
      action_mask_1bit = inline_rf_extract_first_one_bit_mask( action_mask );

      while ( action_mask_1bit != 0 )
      {
        /* Find the bit location. */
        action_ind = inline_rf_extract_first_one_bit_lsb_location(
                                                          action_mask_1bit);

        /* Based on the Action Bit append data */
        switch ( action_ind )
        {

        case RX_MEAS_LNA_OFFSET:
          /*------------------------------------------------------------------*/
          /* Populate LNA Offset */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_state_type),
                  &(cal_result->rx_meas_param[device_ind].lna_offset.lna_state),
                    sizeof(ftm_cal_rx_meas_lna_state_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_state_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_state_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : LNA State - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_state_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate Power Mode */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_power_mode_type),
                 &(cal_result->rx_meas_param[device_ind].lna_offset.power_mode),
                    sizeof(ftm_cal_rx_meas_power_mode_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_power_mode_type) );
            free_size -= sizeof(ftm_cal_rx_meas_power_mode_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : PowerMode - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_power_mode_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate LNA Offset */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_offset_type),
                 &(cal_result->rx_meas_param[device_ind].lna_offset.lna_offset),
                    sizeof(ftm_cal_rx_meas_lna_offset_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_offset_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_offset_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : LNAoffset - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_offset_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */

          break;

        case RX_MEAS_DC_LNA_OFFSET:
          /*------------------------------------------------------------------*/
          /* Populate LNA Offset */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_state_type),
                  &(cal_result->rx_meas_param[device_ind].dc_lna_offset.lna_state),
                    sizeof(ftm_cal_rx_meas_lna_state_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_state_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_state_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : DC LNA State - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_state_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate Power Mode */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_power_mode_type),
                 &(cal_result->rx_meas_param[device_ind].dc_lna_offset.power_mode),
                    sizeof(ftm_cal_rx_meas_power_mode_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_power_mode_type) );
            free_size -= sizeof(ftm_cal_rx_meas_power_mode_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : DC PowerMode - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_power_mode_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate LNA Offset */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_offset_type),
                 &(cal_result->rx_meas_param[device_ind].dc_lna_offset.lna_offset),
                    sizeof(ftm_cal_rx_meas_lna_offset_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_offset_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_offset_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : DC LNAoffset - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_offset_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */

          break;
        case RX_MEAS_RAW_RX_AGC:
          /*------------------------------------------------------------------*/
          /* Populate LNA STATE */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_state_type),
                  &(cal_result->rx_meas_param[device_ind].raw_rx_agc.lna_state),
                    sizeof(ftm_cal_rx_meas_lna_state_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_state_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_state_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : Raw Rx AGC LNA State - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_state_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_state_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate Power Mode */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_power_mode_type),
                 &(cal_result->rx_meas_param[device_ind].raw_rx_agc.power_mode),
                    sizeof(ftm_cal_rx_meas_power_mode_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_power_mode_type) );
            free_size -= sizeof(ftm_cal_rx_meas_power_mode_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : Raw Rx AGC PowerMode - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_power_mode_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_power_mode_type) ) */

          /*------------------------------------------------------------------*/
          /* Populate Raw Rx AGC */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_rx_meas_lna_offset_type),
                 &(cal_result->rx_meas_param[device_ind].raw_rx_agc.raw_rx_agc),
                    sizeof(ftm_cal_rx_meas_lna_offset_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_rx_meas_lna_offset_type) );
            free_size -= sizeof(ftm_cal_rx_meas_lna_offset_type);
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                       "ResultPush : Raw Rx AGC - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_RX_MEASURE,
                       sizeof(ftm_cal_rx_meas_lna_offset_type), 
                       free_size );
          } /* if ( free_size >= sizeof(ftm_cal_rx_meas_lna_offset_type) ) */

          break;
        default:
          break;

        } /* switch ( action_ind ) */

        /* Update Device Mask to set the processed bit to zero. */
        action_mask = action_mask & (~action_mask_1bit);

        /* For the current device mask, extract the bit mask for the first
        enabled device. The first bit will be from LSB. */
        action_mask_1bit = (ftm_cal_rx_meas_action_params_mask_type)\
                           inline_rf_extract_first_one_bit_mask( action_mask );

      } /* while ( action_mask_1bit != 0 ) */

    } /* if ( device_ind < FTM_CAL_V3_RX_MAX_DEV ) */
    else
    {
      ERR_FATAL( "[calv3][Segment %3d][RX_MEASURE.{0x%x}] Max Device is "
                 "%d", 
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_RX_MEASURE,
                 FTM_CAL_V3_RX_MAX_DEV );
    } /* if ! ( device_ind < FTM_CAL_V3_RX_MAX_DEV ) */
    
    /* Update Device Mask to set the processed bit to zero. */
    device_mask = device_mask & (~device_mask_1bit);

    /* For the current device mask, extract the bit mask for the first
    enabled device. The first bit will be from LSB. */
    device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  } /* while ( device_mask_1bit != 0 ) */

  oc_disp->res_buff_info.buff_sz = written_size;

  if ( cal_data->seg_data.oc_payload.rx_measure.up_param.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "ResultPush : %d bytes",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               written_size );
  }

  /* return */
  return ret_val;

} /* ftm_cal_rx_measure_populate_results */

/*! @} */

/*============================================================================*/
/*!
  @name Rx Measure OpCode handling

  @brief
  This section contains functions for Rx Measure opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Rx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Rx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_rx_measure_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_rx_measure_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.rx_measure);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_rx_measure_unpack;
  reg_data.process_fp = ftm_calibration_rx_measure_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;
  if ( cfg_data->res_buff == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->res_buff = modem_mem_alloc( FTM_CAL_RX_MEAS_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->res_buff != NULL )
    {
      reg_data.res_buff_info.buff_ptr = cfg_data->res_buff;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_RX_MEAS_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->res_buff != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                 "Init : ResBuff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_RX_MEASURE );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->res_buff != NULL ) */
  } /* if ( cfg_data->res_buff == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "Init : ResBuff not Null",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */


  /* Register OpCOde */
  if ( ret_val == TRUE )
  {
    ret_val = ftm_calibration_register_opcode( cal_data,
                                               FTM_CAL_OPCODE_RX_MEASURE,
                                               reg_data );
  } /* if ( ret_val == TRUE ) */

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_v3_rx_measure;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_v3_rx_measure;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_rx_measure;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = ftm_tdscdma_calv3_rx_measure;
  #endif
    
  return ret_val;

} /* ftm_calibration_rx_measure_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Rx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Rx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_rx_measure_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_rx_measure_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.rx_measure);

  if ( cfg_data->res_buff != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->res_buff, MODEM_MEM_CLIENT_RFA );
    cfg_data->res_buff = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "DeInit : ResBuff Null",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff != NULL ) */

  return ret_val;
} /* ftm_calibration_rx_measure_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Rx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Rx Measure OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_rx_measure_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size = 0; /* Payload Size */
  uint8 *payload_ptr = NULL; /* Paylaod pointer */

  uint8 device_mask_1bit = 0xFF; /* Device Mask */
  uint8 device_mask = 0; /* Device Mask storage */
  uint8 device_ind;

  ftm_cal_rx_meas_action_params_mask_type rx_meas_1bit = 0xFF; 
  ftm_cal_rx_meas_action_params_mask_type rx_meas = 0; 
  ftm_cal_rx_meas_action_enum_type rx_meas_ind; /* Param Bit */

  ftm_cal_rx_meas_parameter_type *rm_unpack = NULL;
  ftm_cal_rx_measure_data_type *cfg_data = NULL;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][RX_MEASURE.{0x%x}] "
               "Unpack : Null Argument",
               FTM_CAL_OPCODE_RX_MEASURE );
    return 0;
  } /* if ( cal_data == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.rx_measure);

  /* Get pointer to memory to hold Rx measure unpacked data */
  rm_unpack = &(cfg_data->up_param);

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;

  /* Reset all data */
  memset( rm_unpack, 0x0, sizeof(ftm_cal_rx_meas_parameter_type) );
  
  /* Update the payload size and Payload pointer with version*/
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer */
  memscpy( &(rm_unpack->debug_mode_en), sizeof(uint8),
          payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer */
  memscpy( &(rm_unpack->flag),  sizeof(uint16),
          payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

  /* Update the payload size and Payload pointer */
  memscpy( &(rm_unpack->device_mask),  sizeof(ftm_cal_rx_meas_device_mask_type),
           payload_ptr, sizeof(ftm_cal_rx_meas_device_mask_type) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_cal_rx_meas_device_mask_type) );

  /* Fetch the device mask sent from tools */
  device_mask = rm_unpack->device_mask;

  /* For the current device mask, extract the bit mask for the first
  enabled device. The first bit will be from LSB. */
  device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  /* Loop until, the Device Mask comes out to be zero */
  while ( device_mask_1bit != 0 )
  {
    /* Compute the location of the one bit location */
    device_ind = inline_rf_extract_first_one_bit_lsb_location(device_mask_1bit);

    /* Proceed only if the device_ind is valid, to avois array out of bounce */
    if ( device_ind < FTM_CAL_V3_RX_MAX_DEV )
    {
      memscpy( &(rm_unpack->rx_meas_param[device_ind].rx_meas_action), 
              sizeof(ftm_cal_rx_meas_action_params_mask_type),
              payload_ptr, sizeof(ftm_cal_rx_meas_action_params_mask_type) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                         sizeof(ftm_cal_rx_meas_action_params_mask_type) );

      /* Initialize the Mask */
      rx_meas_1bit = 0xFFFF;

      /* Initialize the Rx Meas param */
      rx_meas = rm_unpack->rx_meas_param[device_ind].rx_meas_action;
      
      /* For the current param mask, extract the bit mask for the first
      enabled Rx Meas. The first bit will be from LSB. */
      rx_meas_1bit = inline_rf_extract_first_one_bit_mask( rx_meas );

      while ( rx_meas_1bit != 0 )
      {
        /* Find the bit location. */
        rx_meas_ind = inline_rf_extract_first_one_bit_lsb_location(
                                                           rx_meas_1bit);

        /* Based on the Param Bit append the paylaod size */
        switch ( rx_meas_ind )
        {

        case RX_MEAS_LNA_OFFSET:
          memscpy( &(rm_unpack->rx_meas_param[device_ind].lna_offset.lna_state),
                   sizeof(ftm_cal_rx_meas_lna_state_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_lna_state_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_lna_state_type) );
          memscpy( &(rm_unpack->rx_meas_param[device_ind].lna_offset.power_mode),
                   sizeof(ftm_cal_rx_meas_power_mode_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_power_mode_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_power_mode_type) );
          memscpy( &(rm_unpack->rx_meas_param[device_ind].lna_offset.expected_exagc),
                  sizeof(ftm_cal_rx_meas_exp_agc_type),
                  payload_ptr, sizeof(ftm_cal_rx_meas_exp_agc_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_exp_agc_type) );
          break;

        case RX_MEAS_DC_LNA_OFFSET:
          memscpy( &(rm_unpack->rx_meas_param[device_ind].dc_lna_offset.lna_state),
                   sizeof(ftm_cal_rx_meas_lna_state_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_lna_state_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_lna_state_type) );
          memscpy( &(rm_unpack->rx_meas_param[device_ind].dc_lna_offset.power_mode),
                   sizeof(ftm_cal_rx_meas_power_mode_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_power_mode_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_power_mode_type) );
          memscpy( &(rm_unpack->rx_meas_param[device_ind].dc_lna_offset.expected_exagc),
                  sizeof(ftm_cal_rx_meas_exp_agc_type),
                  payload_ptr, sizeof(ftm_cal_rx_meas_exp_agc_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_exp_agc_type) );
          break;

        case RX_MEAS_RAW_RX_AGC:
          memscpy( &(rm_unpack->rx_meas_param[device_ind].raw_rx_agc.lna_state),
                   sizeof(ftm_cal_rx_meas_lna_state_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_lna_state_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_lna_state_type) );
          memscpy( &(rm_unpack->rx_meas_param[device_ind].raw_rx_agc.power_mode),
                   sizeof(ftm_cal_rx_meas_power_mode_type),
                   payload_ptr, sizeof(ftm_cal_rx_meas_power_mode_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_rx_meas_power_mode_type) );
          break;

        default:
          FTM_MSG_3( FTM_FATAL, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                     "Invalid Rx Meas %d to un pack",
                     cal_data->seg_data.cal_instance + 1,
                     FTM_CAL_OPCODE_RX_MEASURE,
                     rx_meas_ind );
          break;

        } /* switch ( param_bit ) */

        /* Update param Mask to set the processed bit to zero. */
        rx_meas = rx_meas & (~rx_meas_1bit);
      
        /* For the current param mask, extract the bit mask for the first
        enabled Rx meas. The first bit will be from LSB. */
        rx_meas_1bit = inline_rf_extract_first_one_bit_mask( rx_meas );
          
      } /* while ( param_mask_1bit != 0 ) */
    } /* if ( device_ind < FTM_CAL_V3_RX_MAX_DEV ) */
    else
    {
      ERR_FATAL( "[[calv3][Segment %3d][RX_MEASURE.{0x%x}] Max Device is "
                 "%d", 
                 cal_data->seg_data.cal_instance + 1,
                 FTM_CAL_OPCODE_RX_MEASURE, 
                 FTM_CAL_V3_RX_MAX_DEV );
    } /* if ! ( device_ind < FTM_CAL_V3_RX_MAX_DEV ) */

    /* Update Device Mask to set the processed bit to zero. */
    device_mask = device_mask & (~device_mask_1bit);

    /* For the current device mask, extract the bit mask for the first
    enabled device. The first bit will be from LSB. */
    device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  } /* while ( device_mask_1bit != 0 ) */

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  if ( rm_unpack->debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_RX_MEASURE,
               payload_size );
  } /* if ( rm_unpack->debug_mode_en == TRUE ) */
  
  /* return Payload size */
  return payload_size;

} /* ftm_calibration_rx_measure_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Rx Measure OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Rx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_rx_measure_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_rx_measure_data_type *cfg_data;
  ftm_cal_rx_meas_instr_payload_type *p_ptr;
  int32 additional_rx_agc_wait = 0; /* Additional Wait time required for
  Rx AGC to settle. This will be calculated by taking the difference between
  rx_cfg_and_meas_lapse and rx_agc settling time*/
  ftm_cal_rx_meas_result_data_type *cal_result = NULL;
  ftm_cal_config_rx_data_type *cfg_rx_data;
  rf_time_type elapsed_time;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.rx_measure);
  cfg_rx_data = &(cal_data->seg_data.oc_payload.config_rx);
  p_ptr = (ftm_cal_rx_meas_instr_payload_type*)oc_disp->payload_ptr;

  /* Get Current Mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Execute only if the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  )
  {
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /*------------------------------------------------------------------*/
      /* Compute if additional clock busy wait is required before the
         start of RxAGC Sampling */
      /*------------------------------------------------------------------*/

      if ( cfg_rx_data->rx_prof_in_progress == TRUE )
      {
        /* Stop Time profiling to find the total time taken by Tx
        Configuration and/or Tx measurement and/or additional segment between 
        Rx Config and Rx Measure opcodes. This will be used to substract from 
        the RxAGC settling time to decide on the additional wait before Rx 
        Measurement */
        elapsed_time = rf_time_get_elapsed( cfg_rx_data->rx_cfg_time,
                                            RF_USEC );

        /* Calculate the additional wait time by taking the difference of the
        time taken by Tx Cfg - measurement and Total RxAGC settling time */
        additional_rx_agc_wait = 
          ( cfg_rx_data->rxagc_settling_time_us[current_rf_mode] - 
            elapsed_time );
        
        /* If Tx configuraion and Measurement take more than actual RxAGC
        settling time, then there is no need to wait. Thus set the wait time 
        to 0.*/
        if ( additional_rx_agc_wait < 0 )
        {
          additional_rx_agc_wait = 0;
        }
        
        if ( cal_data->cal_config.debug_mode_en == TRUE )
        {
          FTM_MSG_5( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                     "Process : Waiting for %d ( %d - %d ) ms before "
                     "RxAGC Measurement",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_RX_MEASURE,
                     additional_rx_agc_wait,
                     cfg_rx_data->rxagc_settling_time_us[current_rf_mode],
                     elapsed_time );
        } /* if ( cal_data->debug_mode_en == TRUE ) */
        
        /* Perform Clock Busy wait */
        DALSYS_BusyWait( additional_rx_agc_wait );
        
          /* Reset rx_meas_prof_in_progress; so that time profiling can start
        for next Rx Measure Opcode. */
        cfg_rx_data->rx_prof_in_progress = FALSE;
      } /* if ( cfg_rx_data->rx_prof_in_progress == TRUE ) */

      /* Malloc memory for cal results */
      cal_result = modem_mem_alloc( sizeof(ftm_cal_rx_meas_result_data_type),
                                    MODEM_MEM_CLIENT_RFA );

      /* Only if the malloc was successful */ 
      if ( cal_result != NULL )
      {
        /* Call tech specific function to process opcode */
        ret_val = (cfg_data->oc_process_fp[current_rf_mode])( 
                                &(cfg_data->up_param), cal_result );

        /* Error Check */
        if ( ret_val == FALSE )
        {
          /* Flag segment error */
          cal_data->status.seg_error_log.seg_error |= 
                             (1 << FTM_CAL_SEG_ERR_RX_MEASURE_FAILURE);
          FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                     "Process : FP failure",
                     cal_data->seg_data.cal_instance,
                     FTM_CAL_OPCODE_RX_MEASURE );
        } /* if ( ret_val == FALSE ) */
        else
        {
          /* Populate the results buffer */
          ret_val &= ftm_cal_rx_measure_populate_results( cal_data,
                                                          oc_disp,
                                                          cal_result );
        } /* if ( ret_val == FALSE ) */

        /* Fee-up the memory */
        modem_mem_free( cal_result, MODEM_MEM_CLIENT_RFA );
        cal_result = NULL;
      } /* if ( cal_result != NULL ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                   "Process : Malloc failed",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_RX_MEASURE );
      } /* if ! ( cal_result != NULL ) */

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_RX_MEASURE_FAILURE);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_RX_MEASURE );
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               current_rf_mode );
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */

  /* Debug Message */
  if ( p_ptr->debug_mode_en == TRUE )
  {
    FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][RX_MEASURE.{0x%x}] "
               "Process : [Status %d] - dMask 0x%x",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RX_MEASURE,
               ret_val,
               p_ptr->device_mask );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_rx_measure_process */

/*! @} */

/*! @} */


