/*!
  @file
  ftm_calibration_v3_tx.c

  @brief
  This module contains common RF calibration code for Tx measurement

  @addtogroup RF_COMMON_FTM_CALV3_TX

  @brief
  This module provides a common framework to perform Measure Tx (0x8000) OpCode
  operation. This module is responsible for calling the WAN technology specific
  handler function to perform various Measure Tx operation. This module is also
  responsible for populating the Tx results buffer with appropriate calibration
  data.
  @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_tx.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/12/14   dj      PFix for populate read FB power results
11/07/14    br     TxRSB-Add support to compute TxIQMC offsets and process final Tx IQMC 
07/17/14    dj     Tx Measure opcode update to read FB power
02/13/14    id     Updates needed for FBRx Cal API to match FBRx capture API
02/13/14    id     Fix for FB Gain Meas addition in Tx Meas Opcode
10/30/13   pk      Added F3s enable/disable conditions to optimize the cal time
07/23/13   ych     Initialized calv4 opcodes 	
07/16/13   APU     Fixed a bug where the HPM HDET was being referred to instead 
                   of LPM HDET in LPM calibration code.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/23/13   cri     Add reference channel to tx override
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/02/13   aro     Migrated to inline Math functions
04/02/13   aro     Debug message update
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Updated Tx Override OpCode to support modular payload
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
11/30/12   aro     Initial support for Tx measure opcode
11/21/12   aro     Fixed tx override payload misalignment
11/19/12   aro     NULL pointer fix for Tx override unpack pointer
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
#include "math.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_tx.h"
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

/*============================================================================*/
/*!
  @name Tx Config OpCode handling

  @brief
  This section contains functions for Tx Config opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Tx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_config_tx_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_config_tx_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.config_tx);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_config_tx_unpack;
  reg_data.process_fp = ftm_calibration_config_tx_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_CONFIG_TX,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_sweep_configure_tx;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_configure_tx;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_sweep_configure_tx;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                             ftm_tdscdma_calv3_sweep_configure_tx;
  #endif
    

  return ret_val;

} /* ftm_calibration_config_tx_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Tx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Tx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_config_tx_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_config_tx_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Tx Config OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Tx Config OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_config_tx_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_config_tx_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_CONFIG_TX,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_config_tx_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Tx Config OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Tx Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_config_tx_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_config_tx_data_type *cfg_data;
  ftm_cal_instr_payload_config_tx_type *p_ptr;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.config_tx);
  p_ptr = (ftm_cal_instr_payload_config_tx_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_CFG_TX_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Get Current Mode */
  current_rf_mode = cal_data->status.current_rf_mode;

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
                           (1 << FTM_CAL_SEG_ERR_CFG_TX_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
                   "Process : FP failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_CONFIG_TX );
      } /* if ( ret_val == FALSE ) */

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_CFG_TX_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
                 "Process : Null FB",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_CONFIG_TX );
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
               "Process : Bad RFM",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_7( FTM_HIGH, "[calv3][Segment %3d][CONFIG_TX.{0x%x}] "
               "Process : [Status %d] - CalAction 0x%x PA %d RGI %d dMask 0x%x",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX,
               ret_val,
               p_ptr->cal_action,
               p_ptr->pa_state,
               p_ptr->rgi,
               p_ptr->device_flag );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_config_tx_process */

/*! @} */

/*============================================================================*/
/*!
  @name Meas HDET OpCode handling

  @brief
  This section contains functions for Meas HDET opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Meas HDET OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Meas HDET OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_meas_hdet_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_meas_hdet_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_hdet);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_meas_hdet_unpack;
  reg_data.process_fp = ftm_calibration_meas_hdet_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;
  if ( cfg_data->tx_result.tx_buffer == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->tx_result.tx_buffer = modem_mem_alloc( 
                                          FTM_CAL_SEG_TX_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->tx_result.tx_buffer != NULL )
    {
      cfg_data->tx_result.max_tx_buffer_len = FTM_CAL_SEG_TX_BUFF_SIZE_MAX;
      cfg_data->tx_result.num_of_tx_reads = 0xFF;
      reg_data.res_buff_info.buff_ptr = cfg_data->tx_result.tx_buffer;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_SEG_TX_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->tx_result.tx_buffer != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
                 "Init : Result buff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_MEAS_HDET );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->tx_result.tx_buffer != NULL ) */
  } /* if ( cfg_data->tx_result.tx_buffer == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "Init : Buff not Null",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_HDET );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */


  /* Register OpCOde */
  if ( ret_val == TRUE )
  {
    ret_val = ftm_calibration_register_opcode( cal_data,
                                               FTM_CAL_OPCODE_MEAS_HDET,
                                               reg_data );
  } /* if ( ret_val == TRUE ) */

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_sweep_measure_tx;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_measure_tx;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_sweep_measure_tx;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                          ftm_tdscdma_calv3_sweep_measure_tx;
  #endif
    
  return ret_val;

} /* ftm_calibration_meas_hdet_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Meas HDET OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Meas HDET OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_meas_hdet_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_meas_hdet_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_hdet);

  if ( cfg_data->tx_result.tx_buffer != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->tx_result.tx_buffer, MODEM_MEM_CLIENT_RFA );
    cfg_data->tx_result.tx_buffer = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "DeInit : Buff Null",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_HDET );
    ret_val = FALSE;
  } /* if ! ( cfg_data->tx_result.tx_buffer != NULL ) */

  return ret_val;
} /* ftm_calibration_meas_hdet_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Meas HDET OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Meas HDET OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_meas_hdet_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_meas_hdet_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_MEAS_HDET,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_meas_hdet_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Meas HDET OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Meas HDET OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_meas_hdet_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_meas_hdet_data_type *cfg_data;
  ftm_cal_instr_payload_meas_hdet_type *p_ptr;
  uint8 expected_meas_num = 0;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.meas_hdet);
  p_ptr = (ftm_cal_instr_payload_meas_hdet_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_MEAS_HDET_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_HDET );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Get Current Mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Execute only if the current mode is valid */
  if ( current_rf_mode < RFM_NUM_MODES  )
  {
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /* Call tech specific function to process opcode */
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])( p_ptr,
                                                       &(cfg_data->tx_result) );

      /* Error Check */
      if ( ret_val == FALSE )
      {
        /* Flag segment error */
        cal_data->status.seg_error_log.seg_error |= 
                           (1 << FTM_CAL_SEG_ERR_MEAS_HDET_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
                   "Process : FP Failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_MEAS_HDET );
      } /* if ( ret_val == FALSE ) */
      else
      {
        if ( p_ptr->hdet_sample > 0 )
        {
          expected_meas_num += rf_count_one_bits( p_ptr->device_flag );
        } /* if ( cal_instr.meas_hdet->hdet_sample > 0 ) */

        if ( p_ptr->lpm_hdet_sample > 0 )
        {
          expected_meas_num += rf_count_one_bits( p_ptr->device_flag );
        } /* if ( cal_instr.meas_hdet->lpm_hdet_sample > 0 ) */
        /* Calculate the total results size */
        oc_disp->res_buff_info.buff_sz = expected_meas_num*
                                   sizeof(ftm_calibration_tx_meas_data_type);
      } /* if ! ( ret_val == FALSE ) */

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_MEAS_HDET_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_MEAS_HDET );
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_HDET );

  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][MEAS_HDET.{0x%x}] "
               "Process : [Status %d] - dMask 0x%x HDET %d LPM_HDET %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_MEAS_HDET,
               ret_val,
               p_ptr->device_flag,
               p_ptr->hdet_sample,
               p_ptr->lpm_hdet_sample );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_meas_hdet_process */

/*! @} */

/*============================================================================*/
/*!
  @name Tx Override OpCode handling

  @brief
  This section contains functions for Tx Override opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx Override OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Tx Override OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_tx_override_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_tx_override_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.tx_override);

  reg_data.unpack_fp = ftm_calibration_tx_override_unpack;
  reg_data.process_fp = ftm_calibration_tx_override_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_TX_OVERRIDE,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
               ftm_1x_calibration_v3_tx_override;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_v3_tx_override;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                ftm_wcdma_calv3_sweep_tx_override;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                ftm_tdscdma_calv3_sweep_tx_override;
  #endif
    

  return ret_val;

} /* ftm_calibration_tx_override_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Tx Override OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Tx Override OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_tx_override_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_tx_override_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Tx Override OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Tx Override OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_tx_override_unpack
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

  ftm_cal_tx_override_type_mask_type ovr_t_mask_1bit = 
                       (ftm_cal_tx_override_type_mask_type)(-1); 
  ftm_cal_tx_override_type_mask_type ovr_t_mask = 0; 
  ftm_cal_tx_override_type_mask_enum ovr_t_ind; /* Param Bit */

  ftm_cal_tx_ovr_power_param_type p_param_mask = 0;
  ftm_cal_tx_ovr_power_param_type p_param_1bit = 
                       (ftm_cal_tx_ovr_power_param_type)(-1); 
  ftm_cal_tx_ovr_power_param_enum p_param_ind = 0;

  ftm_cal_tx_override_parameter_type *to_unpack = NULL;
  ftm_cal_tx_override_data_type *cfg_data;

  /* NULL Pointer check */
  if ( ( instr_payload == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR,"[calv3][Segment X][TX_OVERRIDE.{0x%x}] "
               "Unpack : Bad Param",
               FTM_CAL_OPCODE_TX_OVERRIDE) ;
    return 0;
  } /* if ( instr_payload == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.tx_override);

  /* Get pointer to memory to hold tx Override unpacked data */
  to_unpack = &(cfg_data->up_param);

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;

  /* Reset all data */
  memset( to_unpack, 0x0, sizeof(ftm_cal_tx_override_parameter_type) );

  /* Extarct Device Data */
  to_unpack->debug_mode_en = 
    ((ftm_cal_tx_override_instr_payload_type*)payload_ptr)->debug_mode_en;
  to_unpack->flag = 
    ((ftm_cal_tx_override_instr_payload_type*)payload_ptr)->flag;
  to_unpack->device_mask = 
    ((ftm_cal_tx_override_instr_payload_type*)payload_ptr)->device_mask;

  /* Update the payload size and Payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_cal_tx_override_instr_payload_type) );

  /* Fetch the device mask sent from tools */
  device_mask = to_unpack->device_mask;

  /* For the current device mask, extract the bit mask for the first
  enabled device. The first bit will be from LSB. */
  device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  /* Loop until, the Device Mask comes out to be zero */
  while ( device_mask_1bit != 0 )
  {
    /* Compute the location of the one bit location */
    device_ind = inline_rf_extract_first_one_bit_lsb_location(device_mask_1bit);

    /* Proceed only if the device_ind is valid, to avois array out of bounce */
    if ( device_ind < FTM_CAL_V3_TX_MAX_DEV )
    {

      memscpy( &(to_unpack->tx_param[device_ind].xpt_mode),
               sizeof(ftm_cal_tx_override_xpt_mode_type),
               payload_ptr,
               sizeof(ftm_cal_tx_override_xpt_mode_type) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                         sizeof(ftm_cal_tx_override_xpt_mode_type) );

      memscpy( &(to_unpack->tx_param[device_ind].override_type),
               sizeof(ftm_cal_tx_override_type_mask_type),
               payload_ptr, sizeof(ftm_cal_tx_override_type_mask_type) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                         sizeof(ftm_cal_tx_override_type_mask_type) );

      /* Initialize param_mask with the data from Payload */
      ovr_t_mask = to_unpack->tx_param[device_ind].override_type;

      /* For the current param mask, extract the bit mask for the first
      enabled tx param. The first bit will be from LSB. */
      ovr_t_mask_1bit = inline_rf_extract_first_one_bit_mask( ovr_t_mask );

      while ( ovr_t_mask_1bit != 0 )
      {
        /* Find the bit location. */
        ovr_t_ind = 
          inline_rf_extract_first_one_bit_lsb_location(ovr_t_mask_1bit);

        if ( ovr_t_ind ==  TX_OVR_POWER )
        {

          memscpy( &(to_unpack->tx_param[device_ind].power_ovr.power_ovr_type),
                   sizeof(ftm_cal_tx_ovr_power_ovr_type),
                   payload_ptr, sizeof(ftm_cal_tx_ovr_power_ovr_type) );

          /* Update the payload size and Payload pointer */
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_tx_ovr_power_ovr_type) );

          if ( to_unpack->tx_param[device_ind].power_ovr.power_ovr_type ==
               TX_OVR_POWER_IND_PARAM )
          {

            memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                       individual_param.param_mask),
                     sizeof(ftm_cal_tx_ovr_power_param_type),
                    payload_ptr, sizeof(ftm_cal_tx_ovr_power_param_type) );

            /* Update the payload size and Payload pointer */
            UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                               sizeof(ftm_cal_tx_ovr_power_param_type) );

            /* Initialize param_mask with the data from Payload */
            p_param_mask = 
              to_unpack->tx_param[device_ind].power_ovr.individual_param.\
              param_mask;

            /* For the current param mask, extract the bit mask for the first
            enabled tx param. The first bit will be from LSB. */
            p_param_1bit = inline_rf_extract_first_one_bit_mask( p_param_mask );

            while ( p_param_1bit != 0 )
            {
              /* Find the bit location. */
              p_param_ind = 
                inline_rf_extract_first_one_bit_lsb_location(p_param_1bit);

              switch ( p_param_ind )
              {
              case TX_OVR_PA_STATE:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.pa_state.gain_state),
                         sizeof(ftm_cal_tx_ovr_pa_state_payload_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_ovr_pa_state_payload_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                               sizeof(ftm_cal_tx_ovr_pa_state_payload_type) );
                break;
              
              case TX_OVR_RGI:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.rgi.gain_index),
                         sizeof(ftm_cal_tx_ovr_rgi_payload_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_ovr_rgi_payload_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_ovr_rgi_payload_type) );
                break;
              
              case TX_OVR_PA_BIAS:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.bias.pa_bias),
                         sizeof(ftm_cal_tx_ovr_bias_payload_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_ovr_bias_payload_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_ovr_bias_payload_type) );
                break;
              
              case TX_OVR_IQ_GAIN:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.iq_gain.ovr_action),
                         sizeof(ftm_cal_tx_override_action_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_override_action_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_action_type) );
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.iq_gain.iq_gain),
                         sizeof(ftm_cal_tx_override_iq_gain_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_override_iq_gain_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_iq_gain_type) );
                break;
              
              case TX_OVR_ENV_SCALE:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.env_scale.ovr_action),
                         sizeof(ftm_cal_tx_override_action_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_override_action_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_action_type) );
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.env_scale.env_scale),
                        sizeof(ftm_cal_tx_override_env_scale_type),
                        payload_ptr,
                        sizeof(ftm_cal_tx_override_env_scale_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_env_scale_type) );
                break;
              
              case TX_OVR_QUISCENT_CURRENT:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.current.quiscent_current),
                         sizeof(ftm_cal_tx_ovr_quiscent_payload_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_ovr_quiscent_payload_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                 sizeof(ftm_cal_tx_ovr_quiscent_payload_type) );
                break;
              
              case TX_OVR_DELAY:
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.delay.ovr_action),
                         sizeof(ftm_cal_tx_override_action_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_override_action_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_action_type) );
                memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                          individual_param.delay.delay_val),
                         sizeof(ftm_cal_tx_override_env_scale_type),
                         payload_ptr,
                         sizeof(ftm_cal_tx_override_env_scale_type) );
                UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                                   sizeof(ftm_cal_tx_override_env_scale_type) );
                break;
              
              default:
                FTM_MSG_3( FTM_FATAL,"[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
                           "Unpack : Invalid Tx Param %d",
                           cal_data->seg_data.cal_instance,
                           FTM_CAL_OPCODE_TX_OVERRIDE,
                           p_param_ind );
                break;

              } /* switch ( p_param_ind ) */

              /* Update param Mask to set the processed bit to zero. */
              p_param_mask = p_param_mask & (~p_param_1bit);

              /* For the current param mask, extract the bit mask for the first
              enabled tx param. The first bit will be from LSB. */
              p_param_1bit = inline_rf_extract_first_one_bit_mask( 
                                                             p_param_mask );

            } /* while ( p_param_1bit != 0 ) */
          } /* if ( to_unpack->tx_param[device_ind].power_ovr.power_ovr_type ==
               TX_OVR_POWER_IND_PARAM ) */
          else if ( to_unpack->tx_param[device_ind].power_ovr.power_ovr_type ==
                    TX_OVR_POWER_ABS )
          {
            memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                      abs_power.power_dbm10),
                     sizeof(ftm_cal_tx_ovr_power_payload_type),
                     payload_ptr,
                     sizeof(ftm_cal_tx_ovr_power_payload_type) );
            UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                               sizeof(ftm_cal_tx_ovr_power_payload_type) );

            memscpy( &(to_unpack->tx_param[device_ind].power_ovr.\
                     abs_power.ref_channel),
                     sizeof(ftm_cal_tx_ovr_ref_channel_payload_type),
                     payload_ptr,sizeof(ftm_cal_tx_ovr_ref_channel_payload_type) );
            UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                               sizeof(ftm_cal_tx_ovr_ref_channel_payload_type) );
          } /*else if(to_unpack->tx_param[device_ind].power_ovr.power_ovr_type==
                    TX_OVR_POWER_ABS ) */
          else
          {
            FTM_MSG_3( FTM_FATAL, "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
                       "Unpack : Bad PowerOvr %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_OVERRIDE,
                      to_unpack->tx_param[device_ind].power_ovr.power_ovr_type);
          }/* if ! ( to_unpack->tx_param[device_ind].power_ovr.power_ovr_type ==
               TX_OVR_POWER_IND_PARAM ) */

        } /* if ( ovr_t_ind ==  TX_OVR_POWER ) */
        else if ( ovr_t_ind ==  TX_OVR_APPLY_DPD )
        {
          memscpy( &(to_unpack->tx_param[device_ind].apply_dpd.dpd.dpd_index),
                   sizeof(ftm_cal_tx_ovr_dpd_payload_type),
                   payload_ptr,
                   sizeof(ftm_cal_tx_ovr_dpd_payload_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr,
                             sizeof(ftm_cal_tx_ovr_dpd_payload_type) );
        } /* else if ( ovr_t_ind ==  TX_OVR_APPLY_DPD ) */

        /* Update param Mask to set the processed bit to zero. */
        ovr_t_mask = ovr_t_mask & (~ovr_t_mask_1bit);
      
        /* For the current param mask, extract the bit mask for the first
        enabled tx param. The first bit will be from LSB. */
        ovr_t_mask_1bit = inline_rf_extract_first_one_bit_mask( ovr_t_mask );
          
      } /* while ( param_mask_1bit != 0 ) */
    } /* if ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */
    else
    {
      ERR_FATAL( "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
                 "Max Device is %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_TX_OVERRIDE,
                 FTM_CAL_V3_TX_MAX_DEV );
    } /* if ! ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */

    /* Update Device Mask to set the processed bit to zero. */
    device_mask = device_mask & (~device_mask_1bit);

    /* For the current device mask, extract the bit mask for the first
    enabled device. The first bit will be from LSB. */
    device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  } /* while ( device_mask_1bit != 0 ) */

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_OVERRIDE,
               payload_size );
  } /* if ( to_unpack->debug_mode_en == TRUE ) */

  /* return Payload size */
  return payload_size;

} /* ftm_calibration_tx_override_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Tx Override OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Tx Override OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_tx_override_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  ftm_cal_tx_override_fp_type to_fp;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_tx_override_data_type *cfg_data;
  
  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_OVERRIDE.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_TX_OVERRIDE );
    return 0;
  } /* if ( cal_data == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.tx_override);

  /* Get current mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Make sure that the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /* Get the Tx override Data */
    to_fp = cfg_data->oc_process_fp[current_rf_mode];

    /* Execute the callback function only if it is valid */
    if ( to_fp != NULL )
    {
      ret_val &= to_fp( &(cfg_data->up_param) );
    } /* if ( rs_data->cb_func != NULL ) */
    else
    {
      FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
                 "Process : Null FP %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_TX_OVERRIDE,
                 current_rf_mode );
      ret_val = FALSE;
    } /* if ! ( rs_data->cb_func != NULL ) */
  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
               "Process : Bad RFM Mode %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_OVERRIDE,
               current_rf_mode );
    ret_val = FALSE;
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */

  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][TX_OVERRIDE.{0x%x}] "
               "Process : [Status %d] dMASK 0x%x",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_OVERRIDE,
               ret_val,
               cfg_data->up_param.device_mask );
  } /* if ( cfg_data->up_param.debug_mode_en == TRUE ) */

  /* return */
  return ret_val;

} /* ftm_calibration_tx_override_process */

/*! @} */

/*============================================================================*/
/*!
  @name Tx Meas Results
 
  @brief
  Functions specific to Tx measurement results
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the results buffer for Tx Measurement
 
  @details
  This function will populate the results buffer with the Tx measurement data
  produced as a result Tx Meas.
 
  @param cal_result
  Pointer to the data structure which contains the Tx Measurement data.
 
  @param tx_meas_result
  Pointer to the memeory where the Tx meas result is to be populated
 
  @param tx_meas_result_size
  Pointer to the variable which needs to be populated with Tx Meas result size
  
  @return
  Flag indicating the status of function
*/
inline static boolean
ftm_cal_tx_measure_populate_results
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp,
  ftm_cal_tx_meas_result_data_type *cal_result
)
{
  boolean ret_val = TRUE; /* return value */ 
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint16 free_size = 0; /* Total free size */
  uint8 res_type = FTM_CAL_OPCODE_TX_MEASURE;
  uint8 version = 0;
  uint8 device_mask_1bit = 0xFF; /* Device Mask */
  uint8 device_mask = 0; /* Device Mask storage */
  uint8 device_ind;

  ftm_cal_tx_meas_action_params_mask_type action_mask_1bit = 0xFF; 
  ftm_cal_tx_meas_action_params_mask_type action_mask = 0; 
  ftm_cal_tx_meas_action_enum_type action_ind; /* Param Bit */

  /* Param vaildation */
  if ( ( cal_result == NULL ) || ( oc_disp == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "ResultPush : Null Argument",
               FTM_CAL_OPCODE_TX_MEASURE );

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
    memscpy( res_write_ptr, sizeof(uint8),&(res_type), sizeof(uint8));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, sizeof(uint8) );
    free_size -= sizeof(uint8);
  } /* if ( free_size >= sizeof(uint8) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "ResultPush : ResType - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               sizeof(uint8),
               free_size );
  } /* if ( free_size >= sizeof(uint8) ) */

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
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "ResultPush : Version - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               sizeof(uint8),
               free_size );
  } /* if ! ( free_size >= sizeof(uint8) ) */

  /*==========================================================================*/
  /* Populate Device mask */
  /*==========================================================================*/

  if ( free_size >= sizeof(ftm_cal_tx_meas_device_mask_type) )
  {
    memscpy( res_write_ptr, sizeof(ftm_cal_tx_meas_device_mask_type), 
             &(cal_result->device_mask), sizeof(ftm_cal_tx_meas_device_mask_type));
    UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                       sizeof(ftm_cal_tx_meas_device_mask_type) );
    free_size -= sizeof(ftm_cal_tx_meas_device_mask_type);
  } /* if ( free_size >= sizeof(ftm_cal_tx_meas_device_mask_type) ) */
  else
  {
    FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "ResultPush : dMask - ReqSize %d FreeSize %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               sizeof(ftm_cal_tx_meas_device_mask_type),
               free_size );
  } /* if ! ( free_size >= sizeof(ftm_cal_tx_meas_device_mask_type) ) */

  /*==========================================================================*/
  /* Populate TxAGC Result Data */
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
    if ( device_ind < FTM_CAL_V3_TX_MAX_DEV )
    {

      /*----------------------------------------------------------------------*/
      /* Populate TxAGC Meas Action */
      /*----------------------------------------------------------------------*/
      if ( free_size >= sizeof(ftm_cal_tx_meas_action_params_mask_type) )
      {
        memscpy( res_write_ptr, sizeof(ftm_cal_tx_meas_action_params_mask_type),
                &(cal_result->tx_meas_param[device_ind].tx_meas_action),
                sizeof(ftm_cal_tx_meas_action_params_mask_type));
        UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                           sizeof(ftm_cal_tx_meas_action_params_mask_type) );
        free_size -= sizeof(ftm_cal_tx_meas_action_params_mask_type);
      } /*if ( free_size >= sizeof(ftm_cal_tx_meas_action_params_mask_type) )*/
      else
      {
        FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                   "ResultPush : MeasAction - ReqSize %d FreeSize %d",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_TX_MEASURE,
                   sizeof(ftm_cal_tx_meas_action_params_mask_type),
                   free_size );
      } /* if !( free_size >= sizeof(ftm_cal_tx_meas_action_params_mask_type))*/
      
      /* Initialize the Mask */
      action_mask_1bit = 0xFFFF;

      /* Initialize the Tx meas action */
      action_mask = cal_result->tx_meas_param[device_ind].tx_meas_action;

      /* For the current param mask, extract the bit mask for the first
      enabled tx param. The first bit will be from LSB. */
      action_mask_1bit = inline_rf_extract_first_one_bit_mask( action_mask );

      while ( action_mask_1bit != 0 )
      {
        /* Find the bit location. */
        action_ind = 
          inline_rf_extract_first_one_bit_lsb_location(action_mask_1bit);

        /* Based on the Action Bit append data */
        switch ( action_ind )
        {

        case TX_MEAS_HDET:
          /*------------------------------------------------------------------*/
          /* Populate HDET read */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_tx_meas_hdet_read_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_tx_meas_hdet_read_type),
                    &(cal_result->tx_meas_param[device_ind].hdet.hdet_read),
                    sizeof(ftm_cal_tx_meas_hdet_read_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_tx_meas_hdet_read_type) );
            free_size -= sizeof(ftm_cal_tx_meas_hdet_read_type);
          } /* if ( free_size >= sizeof(ftm_cal_tx_meas_hdet_read_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                       "ResultPush : HDET - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_MEASURE,
                       sizeof(ftm_cal_tx_meas_hdet_read_type),
                       free_size );
          } /* if !( free_size >= sizeof(ftm_cal_tx_meas_hdet_read_type) ) */

          break;

        case TX_MEAS_LPM_HDET:
          /*------------------------------------------------------------------*/
          /* Populate LPM HDET read */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_tx_meas_lpm_hdet_read_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_tx_meas_lpm_hdet_read_type),
                    &(cal_result->tx_meas_param[device_ind].lpm_hdet.hdet_read),
                    sizeof(ftm_cal_tx_meas_lpm_hdet_read_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_tx_meas_lpm_hdet_read_type) );
            free_size -= sizeof(ftm_cal_tx_meas_hdet_read_type);
          } /* if ( free_size >= sizeof(ftm_cal_tx_meas_lpm_hdet_read_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                       "ResultPush : LPM HDET - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_MEASURE,
                       sizeof(ftm_cal_tx_meas_lpm_hdet_read_type),
                       free_size );
          } /* if !( free_size >= sizeof(ftm_cal_tx_meas_lpm_hdet_read_type) )*/

          break;

        case TX_MEAS_LINEARIZER_THERM:
          /*------------------------------------------------------------------*/
          /* Populate Linearizer Therm read */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_tx_meas_lin_therm_read_type) )
          {
            memscpy( 
               res_write_ptr, sizeof(ftm_cal_tx_meas_lin_therm_read_type),
               &(cal_result->tx_meas_param[device_ind].lin_therm.therm_read),
               sizeof(ftm_cal_tx_meas_lin_therm_read_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_tx_meas_lin_therm_read_type) );
            free_size -= sizeof(ftm_cal_tx_meas_lin_therm_read_type);
          } /*if ( free_size >= sizeof(ftm_cal_tx_meas_lin_therm_read_type) )*/
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                       "ResultPush : Therm - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_MEASURE,
                       sizeof(ftm_cal_tx_meas_lin_therm_read_type),
                       free_size );
          } /* if !(free_size >= sizeof(ftm_cal_tx_meas_lin_therm_read_type))*/

          break;

        case TX_MEAS_FB_GAIN:
          /*------------------------------------------------------------------*/
          /* Populate FB Gain Meas Results */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_tx_meas_fb_gain_res_type) )
          {
            memscpy( 
               res_write_ptr, sizeof(ftm_cal_tx_meas_fb_gain_res_type),
               &(cal_result->tx_meas_param[device_ind].fb_gain_meas.fb_meas_res),
               sizeof(ftm_cal_tx_meas_fb_gain_res_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_tx_meas_fb_gain_res_type) );
            free_size -= sizeof(ftm_cal_tx_meas_fb_gain_res_type);
          } /*if ( free_size >= sizeof(ftm_cal_tx_meas_fb_gain_res_type) )*/
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                       "ResultPush : FB Gain Meas - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_MEASURE,
                       sizeof(ftm_cal_tx_meas_fb_gain_res_type),
                       free_size );
          } /* if !(free_size >= sizeof(ftm_cal_tx_meas_fb_gain_res_type))*/

          break;

        case TX_MEAS_FB_PWR:
          /*------------------------------------------------------------------*/
          /* Populate FB PWR result */
          /*------------------------------------------------------------------*/
          if ( free_size >= sizeof(ftm_cal_tx_meas_fb_power_res_type) )
          {
            memscpy( res_write_ptr, sizeof(ftm_cal_tx_meas_fb_power_res_type),
                    &(cal_result->tx_meas_param[device_ind].fb_power_meas.fb_pwr_meas_res),
                    sizeof(ftm_cal_tx_meas_fb_power_res_type));
            UPDATE_SIZE_N_PTR( written_size, res_write_ptr, 
                               sizeof(ftm_cal_tx_meas_fb_power_res_type) );
            free_size -= sizeof(ftm_cal_tx_meas_fb_power_res_type);
          } /* if ( free_size >= sizeof(ftm_cal_tx_meas_fb_power_res_type) ) */
          else
          {
            FTM_MSG_4( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                       "ResultPush : FB POWER - ReqSize %d FreeSize %d",
                       cal_data->seg_data.cal_instance,
                       FTM_CAL_OPCODE_TX_MEASURE,
                       sizeof(ftm_cal_tx_meas_fb_power_res_type),
                       free_size );
          } /* if !( free_size >= sizeof(ftm_cal_tx_meas_fb_power_res_type) )*/

          break;

        default:
          break;

        } /* switch ( action_ind ) */

        /* Update Device Mask to set the processed bit to zero. */
        action_mask = action_mask & (~action_mask_1bit);

        /* For the current device mask, extract the bit mask for the first
        enabled device. The first bit will be from LSB. */
        action_mask_1bit = 
          (ftm_cal_tx_meas_action_params_mask_type)inline_rf_extract_first_one_bit_mask( action_mask );
        
      } /* while ( action_mask_1bit != 0 ) */

    } /* if ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */
    else
    {
      ERR_FATAL( "[calv3][Segment %3d][TX_MEASURE.{0x%x}] Max Device is "
                 "%d", 
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_TX_MEASURE,
                 FTM_CAL_V3_TX_MAX_DEV);
    } /* if ! ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */
    
    /* Update Device Mask to set the processed bit to zero. */
    device_mask = device_mask & (~device_mask_1bit);

    /* For the current device mask, extract the bit mask for the first
    enabled device. The first bit will be from LSB. */
    device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  } /* while ( device_mask_1bit != 0 ) */

  oc_disp->res_buff_info.buff_sz = written_size;

  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "ResultPush : %d bytes",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               written_size );
  }

  /* return */
  return ret_val;

} /* ftm_cal_tx_measure_populate_results */

/*! @} */

/*============================================================================*/
/*!
  @name Tx Measure OpCode handling

  @brief
  This section contains functions for Tx Measure opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Tx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_tx_measure_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_tx_measure_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.tx_measure);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_tx_measure_unpack;
  reg_data.process_fp = ftm_calibration_tx_measure_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;
  if ( cfg_data->res_buff == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->res_buff = modem_mem_alloc( FTM_CAL_TX_MEAS_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->res_buff != NULL )
    {
      reg_data.res_buff_info.buff_ptr = cfg_data->res_buff;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_TX_MEAS_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->res_buff != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                 "Init : ResBuff malloc failed",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_TX_MEASURE );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->res_buff != NULL ) */
  } /* if ( cfg_data->res_buff == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Init : ResBuff not NULL",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */


  /* Register OpCOde */
  if ( ret_val == TRUE )
  {
    ret_val = ftm_calibration_register_opcode( cal_data,
                                               FTM_CAL_OPCODE_TX_MEASURE,
                                               reg_data );
  } /* if ( ret_val == TRUE ) */

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = ftm_1x_calibration_v3_tx_measure;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = rflte_ftm_calibration_v3_tx_measure;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = ftm_wcdma_calv3_tx_measure;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = ftm_tdscdma_calv3_tx_measure;
  #endif
    
  return ret_val;

} /* ftm_calibration_tx_measure_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Tx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Tx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_tx_measure_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_tx_measure_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.tx_measure);

  if ( cfg_data->res_buff != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->res_buff, MODEM_MEM_CLIENT_RFA );
    cfg_data->res_buff = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Deinit : Null ResBuff",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff != NULL ) */

  return ret_val;
} /* ftm_calibration_tx_measure_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Tx Measure OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Tx Measure OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_tx_measure_unpack
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

  ftm_cal_tx_meas_action_params_mask_type tx_meas_1bit = 0xFF; 
  ftm_cal_tx_meas_action_params_mask_type tx_meas = 0; 
  ftm_cal_tx_meas_action_enum_type tx_meas_ind; /* Param Bit */

  ftm_cal_tx_meas_parameter_type *tm_unpack = NULL;

  /* NULL Pointer check */
  if ( ( instr_payload == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "Unpack : Null Argument",
               FTM_CAL_OPCODE_TX_MEASURE );

    return 0;
  } /* if ( instr_payload == NULL ) */

  /* Get pointer to memory to hold Tx measure unpacked data */
  tm_unpack = &(cal_data->seg_data.oc_payload.tx_measure.up_param);

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)instr_payload;
  payload_size = 0;

  /* Reset all data */
  memset( tm_unpack, 0x0, sizeof(ftm_cal_tx_meas_parameter_type) );

  /* Update the payload size and Payload pointer with version*/
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer */
  memscpy( &(tm_unpack->debug_mode_en), sizeof(uint8),
           payload_ptr, sizeof(uint8) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

  /* Update the payload size and Payload pointer */
  memscpy( &(tm_unpack->flag), sizeof(uint16), 
          payload_ptr, sizeof(uint16) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

  /* Update the payload size and Payload pointer */
  memscpy( &(tm_unpack->device_mask), sizeof(ftm_cal_tx_meas_device_mask_type),
           payload_ptr, sizeof(ftm_cal_tx_meas_device_mask_type) );
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_cal_tx_meas_device_mask_type) );

  /* Fetch the device mask sent from tools */
  device_mask = tm_unpack->device_mask;

  /* For the current device mask, extract the bit mask for the first
  enabled device. The first bit will be from LSB. */
  device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  /* Loop until, the Device Mask comes out to be zero */
  while ( device_mask_1bit != 0 )
  {
    /* Compute the location of the one bit location */
    device_ind = inline_rf_extract_first_one_bit_lsb_location(device_mask_1bit);

    /* Proceed only if the device_ind is valid, to avois array out of bounce */
    if ( device_ind < FTM_CAL_V3_TX_MAX_DEV )
    {
      memscpy( &(tm_unpack->tx_meas_param[device_ind].tx_meas_action),
               sizeof(ftm_cal_tx_meas_action_params_mask_type),
               payload_ptr,
               sizeof(ftm_cal_tx_meas_action_params_mask_type) );

      /* Update the payload size and Payload pointer */
      UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                         sizeof(ftm_cal_tx_meas_action_params_mask_type) );

      /* Initialize the Mask */
      tx_meas_1bit = 0xFFFF;

      /* Initialize the Tx Meas param */
      tx_meas = tm_unpack->tx_meas_param[device_ind].tx_meas_action;

      /* For the current param mask, extract the bit mask for the first
      enabled Rx Meas. The first bit will be from LSB. */
      tx_meas_1bit = inline_rf_extract_first_one_bit_mask( tx_meas );

      while ( tx_meas_1bit != 0 )
      {
        /* Find the bit location. */
        tx_meas_ind = 
          inline_rf_extract_first_one_bit_lsb_location(tx_meas_1bit);

        /* Based on the Param Bit append the paylaod size */
        switch ( tx_meas_ind )
        {

        case TX_MEAS_HDET:
          memscpy( &(tm_unpack->tx_meas_param[device_ind].hdet.hdet_sample),
                   sizeof(ftm_cal_tx_meas_hdet_sample_type),
                   payload_ptr, sizeof(ftm_cal_tx_meas_hdet_sample_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_tx_meas_hdet_sample_type) );
          break;

        case TX_MEAS_LPM_HDET:
          memscpy( &(tm_unpack->tx_meas_param[device_ind].lpm_hdet.hdet_sample),
                   sizeof(ftm_cal_tx_meas_lpm_hdet_sample_type),
                   payload_ptr, sizeof(ftm_cal_tx_meas_lpm_hdet_sample_type) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                             sizeof(ftm_cal_tx_meas_lpm_hdet_sample_type) );
          break;

        case TX_MEAS_LINEARIZER_THERM:
          break;

        case TX_MEAS_FB_GAIN:
          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.xpt_mode),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.gain_comp),
                   sizeof(int16), payload_ptr, sizeof(int16) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(int16) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.fb_gain_state),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.ubias),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.rx_scale),
                   sizeof(uint16), payload_ptr, sizeof(uint16) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint16) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_gain.fb_gain_params.debug_mode_en),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          break;

        case TX_MEAS_FB_PWR:
          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_power.fp_power_params.fb_gain_state),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          memscpy( &(tm_unpack->tx_meas_param[device_ind].fb_power.fp_power_params.fb_power_samples),
                   sizeof(uint8), payload_ptr, sizeof(uint8) );
          UPDATE_SIZE_N_PTR( payload_size, payload_ptr, sizeof(uint8) );

          break;

        default:
          FTM_MSG_3( FTM_FATAL, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                     "Unpack : Invalid Tx Meas %d",
                     cal_data->seg_data.cal_instance + 1,
                     FTM_CAL_OPCODE_TX_MEASURE,
                     tx_meas_ind );
          break;

        } /* switch ( param_bit ) */

        /* Update param Mask to set the processed bit to zero. */
        tx_meas = tx_meas & (~tx_meas_1bit);

        /* For the current param mask, extract the bit mask for the first
        enabled Rx meas. The first bit will be from LSB. */
        tx_meas_1bit = inline_rf_extract_first_one_bit_mask( tx_meas );

      } /* while ( param_mask_1bit != 0 ) */
    } /* if ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */
    else
    {
      ERR_FATAL( "[calv3][Segment %3d][TX_MEASURE.{0x%x}] Max Device is "
                 "%d", 
                 cal_data->seg_data.cal_instance + 1,
                 FTM_CAL_OPCODE_TX_MEASURE, FTM_CAL_V3_TX_MAX_DEV );
    } /* if ! ( device_ind < FTM_CAL_V3_TX_MAX_DEV ) */

    /* Update Device Mask to set the processed bit to zero. */
    device_mask = device_mask & (~device_mask_1bit);

    /* For the current device mask, extract the bit mask for the first
    enabled device. The first bit will be from LSB. */
    device_mask_1bit = inline_rf_extract_first_one_bit_mask( device_mask );

  } /* while ( device_mask_1bit != 0 ) */

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  if ( cal_data->cal_config.debug_mode_en)
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_TX_MEASURE,
               payload_size );
  } /* if ( p_ptr->debug_mode_en == TRUE ) */
  
  /* return Payload size */
  return payload_size;

} /* ftm_calibration_tx_measure_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Tx Measure OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Tx Measure OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_tx_measure_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  ftm_cal_tx_measure_fp_type tm_fp;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_tx_meas_result_data_type *cal_result = NULL;
  ftm_cal_tx_measure_data_type *cfg_data;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][TX_MEASURE.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_TX_MEASURE );
    return 0;
  } /* if ( cal_data == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.tx_measure);

  /* Get current mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Make sure that the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /* Get the Tx measure Data */
    tm_fp = cfg_data->oc_process_fp[current_rf_mode];

    /* Execute the callback function only if it is valid */
    if ( tm_fp != NULL )
    {
      /* Malloc memory for cal results */
      cal_result = modem_mem_alloc( sizeof(ftm_cal_tx_meas_result_data_type),
                                    MODEM_MEM_CLIENT_RFA );

      /* Only if the malloc was successful */ 
      if ( cal_result != NULL )
      {
        /* Call tech specific function to perform tx measure */
        ret_val &= tm_fp( &(cfg_data->up_param), cal_result );

        /* Populate the results buffer */
        ret_val &= ftm_cal_tx_measure_populate_results( cal_data,
                                                        oc_disp, 
                                                        cal_result );

        /* Fee-up the memory */
        modem_mem_free( cal_result, MODEM_MEM_CLIENT_RFA );
        cal_result = NULL;
      } /* if ( cal_result != NULL ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                   "Process : Malloc failed",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_TX_MEASURE );
      } /* if ! ( cal_result != NULL ) */
    } /* if ( rm_fp != NULL ) */
    else
    {
      FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
                 "Process : Null FP %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_TX_MEASURE,
                 current_rf_mode );
      ret_val = FALSE;
    } /* if ! ( rs_data->cb_func != NULL ) */
  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Process : Invalid RFM Mode %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               current_rf_mode );
    ret_val = FALSE;
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en )
  {
    FTM_MSG_4( FTM_HIGH, "[calv3][Segment %3d][TX_MEASURE.{0x%x}] "
               "Process : [Status %d] dMask 0x%x",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_TX_MEASURE,
               ret_val,
               cfg_data->up_param.device_mask );
  } /* if ( cfg_data->up_param.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_tx_measure_process */

boolean 
ftm_calibration_txiqmc_override(ftm_rsb_coeff *rsb_coeff,uint8 counter,uint16 *alpha, uint16 *beta)
{

    boolean ret_val = TRUE;
    uint16 pre_alpha = 0;
    uint16 pre_beta = 0;
    int16 alpha_sgn16 = 0;
    int16 beta_sgn16 = 0;
    int32 alpha_32 = 0;
    int32 beta_32 = 0;
    int32 delta_phi_neg = 0;
    int64 alpham1_64 = 0;
    int64 beta_64 = 0;    
    uint16 alpham1_val = 0;
    uint16 alpham1_sign = 0;
    uint16 alpham1_res = 0;
    uint16 beta_val = 0;
    uint16 beta_sign = 0;
    uint16 beta_res = 0;

    
    pre_alpha = rsb_coeff->alpha;
    pre_beta = rsb_coeff->beta;
    
    // print rsb_coeff->alpha, rsb_coeff->beta
    FTM_MSG_2(FTM_ERROR," RSB coeff used for computing the next rsb_comp-alpham1 = %d, beta = %d",pre_alpha,pre_beta);

    //Connvert uint16 to int16 (Sign extension)
    alpha_sgn16 = pre_alpha  | ((pre_alpha & 0x4000) << 1);
    beta_sgn16 = pre_beta | ((pre_beta  & 0x4000) <<1 );
    FTM_MSG_2(FTM_ERROR,"(int16)alpham1 = %d, (int16)beta = %d",alpha_sgn16,beta_sgn16);

    //Convert to int32 
    alpha_32 = ((int32)alpha_sgn16 <<13) + (1<<30);
    beta_32 = (int32)beta_sgn16<< 14;
    FTM_MSG_2(FTM_ERROR,"(int32)alpha = %d, (int32) beta =%d",alpha_32,beta_32);

    //Print constants
    FTM_MSG_2(FTM_ERROR,"Delta_phi = %d,Alpha_factor = %d",delta_phi[counter],alpha_factor[counter]);

    //Get the values of int64 alpha_n and beta_n
    alpham1_64 = ((int64)alpha_32*(int64)alpha_factor[counter]) - ((int64)1<<60);
    delta_phi_neg = -delta_phi[counter];
    beta_64 = ((int64)alpha_32*(int64)delta_phi_neg) + ((int64)beta_32 << 30);
    
  

    //Get the computed values of uint16 alpha
    if (alpham1_64 > 0) 
    {
      alpham1_val = (uint16)(((alpham1_64 + ((int64)1 << 42)) >> 43) & 0x3FFF );
    }
    else if (alpham1_64 < 0)
    {
      alpham1_val = (uint16)(((alpham1_64 - ((int64)1 << 42)) >> 43) & 0x3FFF );
    }
    alpham1_sign = (uint16)(alpham1_64<0)<<14;
    alpham1_res = alpham1_val |  ((uint16)(alpham1_64<0)<<14) ;

    //Get the uint16 values of beta
    if (beta_64 > 0) 
    {
        beta_val = (uint16)(((beta_64 + ((int64)1 << 43) )>> 44) & 0x3FFF);
    }
    else if (beta_64 < 0) 
    {
        beta_val = (uint16)(((beta_64 - ((int64)1 << 43) )>> 44) & 0x3FFF);
    }
    beta_sign = (uint16)(beta_64<0)<<14;
    beta_res= beta_val | ((uint16)(beta_64<0)<<14) ;

    //Print final alpha and beta values
    FTM_MSG_2(FTM_ERROR,"alpham1= %d, beta = %d",alpham1_res,beta_res);

    alpha[counter] = alpham1_res;
    beta[counter] = beta_res;

    return ret_val;
    
}/* ftm_calibration_txiqmc_override */

boolean
ftm_calibration_process_txrsb(ftm_rsb_coeff *rsb_coeff,int32 *rrsb_res,uint32 *txrsb_final)
{
    boolean ret_val = TRUE;
    int64 eps1 = 0;
    int32 eps = 0;
    int64 dphi1 = 0;
    int32 dphi = 0;
    int32 Bc1 = 0;
    int64 Bc2_int64 = 0;
    int32 Bc2 = 0;
    int32 Bc = 0;
    int32 Bcal = 0;
    int16 B0 = 0;
    int64 Ac1_int64 = 0;
    int32 Ac1 = 0;
    int64 Ac_int64 = 0;
    int32 Ac = 0;
    int64 Am1cal_int64 = 0;
    int32 Am1cal = 0;
    int16 Am10 = 0;
    int32 A0 = 0;
    int16 Am1cal17 = 0;
    int16 Am1cal15 = 0;
    uint16 Am1cal15u = 0;
    int16 Bcal16 = 0;
    int16 Bcal15 = 0;
    uint16 Bcal15u = 0;

    FTM_MSG_4(FTM_ERROR, "RSB values: %d, %d, %d, %d", rrsb_res[0], rrsb_res[1], rrsb_res[2], rrsb_res[3]);
    eps1 = (int64)cfinal *(int64)(rrsb_res[0] - rrsb_res[1]- rrsb_res[2] + rrsb_res[3]); //4/root2 >> 2^30
    eps = (int32)(eps1>>27);
    dphi1 = (int64)((int64)cfinal *(int64)(rrsb_res[0] + rrsb_res[1] - rrsb_res[2] - rrsb_res[3]));
    dphi = (int32)(dphi1 >> 27);
    FTM_MSG_2(FTM_ERROR, "Epsilon = %d,dPhi = %d", eps, dphi);

    Bc1 = (int32)((int32)(1 << 30)- eps);
    Bc2_int64 = (int64)((int64)Bc1 *(int64)dphi);
    Bc2 = (int32)(Bc2_int64 >> 30);
    B0 = rsb_coeff->beta |((rsb_coeff->beta  & 0x4000)<< 1);
    Bcal = ((int32)B0 << 14)+ Bc2;

    Ac1_int64 = pow(dphi, 2);
    Ac1 = (int32)(1 << 30)-(int32)(Ac1_int64 >> 31);
    Ac_int64 = (int64)Ac1 *(int64)Bc1;
    Ac = (int32)(Ac_int64 >> 30);
    Am10 = rsb_coeff->alpha  |((rsb_coeff->alpha & 0x4000)<< 1);
    A0 = ((int32)Am10 << 13)+(1 << 30);
    Am1cal_int64 = (int64)A0 *(int64)Ac;
    Am1cal = (int32)(Am1cal_int64 >> 30)-(int32)(1 << 30);

    FTM_MSG_2(FTM_ERROR, "Bcal = %d, Am1cal =%d",Bcal,Am1cal); 


    if (Am1cal > 0) 
    {
        Am1cal17 = (int16)((Am1cal + (int32)(1<<12) ) >> 13);
    }
     
    else if (Am1cal < 0)
    {
        Am1cal17 = (int16)((Am1cal - (int32)(1<<12) ) >> 13);

    }
    
   
    if (Am1cal17<(-(1<<14))) 
    {
        Am1cal17 = -(1<<14);
    }
    
    if (Am1cal17 > ((1<<14)-1) ) 
    {
        Am1cal17 = (1<<14)-1;
    }

    Am1cal15 = Am1cal17;

   

    if (Bcal > 0) 
    {
       Bcal16 = (int16)((Bcal + (int32)(1<<13))>> 14);
    }

    else if(Bcal < 0)
    {
       Bcal16 = (int16)((Bcal - (int32)(1<<13))>> 14);
    }

    if (Bcal16<(-(1<<14))) 
    {
        Bcal16 = -(1<<14);
    }
    
    if (Bcal16 > ((1<<14)-1) ) 
    {
        Bcal16 = (1<<14)-1;
    }

    Bcal15 = Bcal16;

    *txrsb_final = (uint32)((Bcal15 & 0x7FFF)<<15) | (uint32)(Am1cal15 & 0x7FFF);
    FTM_MSG_1(FTM_ERROR,"RSB calculated = %d",*txrsb_final);

    return ret_val;

    }/*ftm_calibration_process_txrsb*/


/*! @} */

/*! @} */

