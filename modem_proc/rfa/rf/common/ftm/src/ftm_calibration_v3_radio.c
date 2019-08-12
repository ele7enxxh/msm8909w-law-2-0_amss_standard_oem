/*!
  @file
  ftm_calibration_v3_radio.c

  @brief
  This module contains common RF calibration code for Radio Configuration

  @addtogroup RF_COMMON_FTM_CALV3_RADIO

  @brief
  This module provides a common framework to perform Radio Setup (0x100) OpCode
  operation. This module is responsible for calling the WAN technology specific
  handler function to bring up the radio to desired state.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_radio.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/22/14   shk     Added support for ver 1.0 of RADIO SETUP opcode 
10/23/13   nrk     Fix compiler warnings
08/21/13    dj     Updated feedback setup function pointers
08/19/13    kb     [XPT] Add new TDS DPD waveform.
08/02/13    dj     Updated feedback setup to use fb_state instead of pa_state
                   Added new FB Setup Action to configure rxlm for fbagc
                   updated the existing rxlm configuration to be used for xpt
07/23/13   ych     Initialized calv4 opcodes
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/24/13    dj     Minor change: clean up messages for Feedback Setup
04/24/13    dj     Updated tech specific function pointers for Feedback Setup
04/19/13    dj     Support for Feedback Setup Opcode
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/09/13    id     Support for storing current band and channel in global status
04/02/13   aro     KW Fix
04/02/13   aro     Debug message update
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/29/13   aro     Radio Config/Retune optimized opcode proc functions
03/29/13   aro     Tx Config/Meas optimized opcode proc functions
03/29/13   aro     Rx Config/Meas optimized opcode proc functions
03/27/13   aro     Data structure restructing
12/03/12   aro     Doc update
11/05/12   aro     Removed instruction payload pointer from process
                   instruction function
11/05/12   aro     Added RFM Mode validity checking
11/05/12   aro     Removed OpCode init from opcode modules to common cal v3
                   init function
11/05/12   aro     Re-organized cal v3 data structure to be more modular
11/02/12   aro     Doc Update
11/02/12   aro     Converted payload computation function to perform
                   opcode payload unpacking
10/31/12   aro     Added Tx waveform config support in Radio Setup
10/29/12   aro     Interface to process radio setup opcode
10/29/12   aro     Doc update
10/29/12   aro     Interface to query the payload size for Radoio setup opcode
10/29/12   aro     Added interface to initialize Radio OpCode
10/29/12   aro     Added interface to deinitialize Radio OpCode
10/24/12   aro     Initial version of Radio Config code

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_radio.h"
#include "ftm_msg.h"
#include "ftm_common_control.h"

#include "stringl.h" /* for memscpy */
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
  @name Radio Tear Down handling

  @brief
  This section contains functions for Radio Tear Down handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tear Down
 
  @details
  This function is called from common sweep function to perform
  data initialization for Tear Down
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_tear_down_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_tear_down_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.tear_down);

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
               ftm_1x_calibration_sweep_tear_down_radio;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_tear_down_radio;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                ftm_wcdma_calv3_sweep_tear_down_radio;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                ftm_tdscdma_calv3_sweep_tear_down_radio;
  #endif
    
  return ret_val;

} /* ftm_calibration_tear_down_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Tear Down
 
  @details
  This function is called from common sweep function to perform
  data deinitialization for Radio Tear Down
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_tear_down_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_tear_down_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Tear Down
 
  @details
  This function is called from common sweep function to perform the
  processing of the Tear Down
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_tear_down_process
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_tear_down_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.tear_down);

  /* Get the current RFM mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Dispatch only if the RFM mode is valid */
  if ( current_rf_mode < RFM_NUM_MODES  )
  {
    /* NULL pointer check */
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /* Call the tech specific function pointer */
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])();

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      FTM_MSG_1( FTM_ERROR, "[calv3][Segment %3d][TEAR_DOWN] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance );
    } /* if ! ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment %3d][TEAR_DOWN] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_2( FTM_HIGH, "[calv3][Segment %3d][TEAR_DOWN] "
               "Process : [Status %d]",
               cal_data->seg_data.cal_instance,
               ret_val );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_tear_down_process */

/*! @} */

/*============================================================================*/
/*!
  @name Radio Config OpCode handling

  @brief
  This section contains functions for Radio Config opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Radio Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Radio Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_radio_config_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_radio_config_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.radio_config);

  reg_data.unpack_fp = ftm_calibration_radio_config_unpack;
  reg_data.process_fp = ftm_calibration_radio_config_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_CONFIG_RADIO,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
               ftm_1x_calibration_sweep_configure_radio;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_configure_radio;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                ftm_wcdma_calv3_sweep_configure_radio;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                ftm_tdscdma_calv3_sweep_configure_radio;
  #endif
    

  return ret_val;

} /* ftm_calibration_radio_config_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Radio Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Radio Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_radio_config_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_radio_config_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Radio Config OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Radio Config OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_radio_config_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_config_radio_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_CONFIG_RADIO,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;
} /* ftm_calibration_radio_config_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Radio Config OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Radio Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_radio_config_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_radio_config_data_type *cfg_data;
  ftm_cal_instr_payload_config_radio_type *p_ptr;

  /* Get config data */
  cfg_data = &(cal_data->seg_data.oc_payload.radio_config);
  p_ptr = (ftm_cal_instr_payload_config_radio_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_CFG_RADIO_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RADIO );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Store the current RFM mode */
  cal_data->status.current_rf_mode = 
    ftm_common_resolve_rf_mode(p_ptr->rf_mode);

  current_rf_mode = cal_data->status.current_rf_mode;

  /* Dispatch only if the RFM mode is valid */
  if ( current_rf_mode < RFM_NUM_MODES  )
  {
    /* NULL pointer check */
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      /* Call the tech specific function pointer */
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])( p_ptr );

      /* Error code based on function return */
      if ( ret_val == FALSE )
      {
        /* Flag segment error */
        cal_data->status.seg_error_log.seg_error |= 
                           (1 << FTM_CAL_SEG_ERR_CFG_RADIO_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
                   "Process : Tech FP failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_CONFIG_RADIO );
      } /* if ( ret_val == FALSE ) */
    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_CFG_RADIO_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
                 "Process : Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_CONFIG_RADIO );
    } /* if ! ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RADIO );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_8( FTM_HIGH, "[calv3][Segment %3d][RADIO_CONFIG.{0x%x}] "
               "Process : [Status %d] - mDev 0x%x sDev 0x%x RFMode %d Chan %d "
               "ChanOffset %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_RADIO,
               ret_val,
               p_ptr->device_master, 
               p_ptr->device_slave, 
               p_ptr->rf_mode,
               p_ptr->channel,
               p_ptr->dc_channel_offset );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_radio_config_process */

/*! @} */

/*============================================================================*/
/*!
  @name Radio Retune OpCode handling

  @brief
  This section contains functions for Radio Retune opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Radio retune OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Radio retune OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_radio_retune_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_radio_retune_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.radio_retune);
  reg_data.unpack_fp = ftm_calibration_radio_retune_unpack;
  reg_data.process_fp = ftm_calibration_radio_retune_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_RETUNE_RADIO,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
               ftm_1x_calibration_sweep_retune_radio;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_retune_radio;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                ftm_wcdma_calv3_sweep_retune_radio;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                ftm_tdscdma_calv3_sweep_retune_radio;
  #endif
    

  return ret_val;

} /* ftm_calibration_radio_retune_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Radio retune OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Radio retune OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_radio_retune_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_radio_retune_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Radio Retune OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Radio Retune OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_radio_retune_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_retune_radio_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_RETUNE_RADIO,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_radio_retune_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Radio Retune OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Radio Retune OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_radio_retune_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_radio_retune_data_type *cfg_data;
  ftm_cal_instr_payload_retune_radio_type *p_ptr;

  cfg_data = &(cal_data->seg_data.oc_payload.radio_retune);
  p_ptr = (ftm_cal_instr_payload_retune_radio_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_RETUNE_RADIO_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RETUNE_RADIO );
    return FALSE;
  } /* if ( p_ptr == NULL ) */

  /* Get current mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Call RFM FP, only for valid FP */
  if ( current_rf_mode < RFM_NUM_MODES  )
  {
    if ( cfg_data->oc_process_fp[current_rf_mode] != NULL )
    {
      ret_val = (cfg_data->oc_process_fp[current_rf_mode])( p_ptr );

      if ( ret_val == FALSE )
      {
        /* Flag segment error */
        cal_data->status.seg_error_log.seg_error |= 
                           (1 << FTM_CAL_SEG_ERR_RETUNE_RADIO_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
                   "Process : FB failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_RETUNE_RADIO );
      } /* if ( ret_val == FALSE ) */
    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_RETUNE_RADIO_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
                 "Process : Null FB",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_RETUNE_RADIO );
    } /* if ! ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
               "Process : Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RETUNE_RADIO );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][RADIO_RETUNE.{0x%x}] "
               "Process : [Status %data] dMask 0x%x Channel %d Offset %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RETUNE_RADIO,
               ret_val,
               p_ptr->device_master,
               p_ptr->channel,
               p_ptr->dc_channel_offset );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_radio_retune_process */

/*! @} */

/*============================================================================*/
/*!
  @name Tx Waveform Config OpCode handling

  @brief
  This section contains functions for Tx Waveform Config opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx Waveform Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Tx Waveform Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_tx_waveform_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_tx_waveform_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.tx_wf);

  /* Populate opcode registration data */
  reg_data.unpack_fp = ftm_calibration_tx_waveform_unpack;
  reg_data.process_fp = ftm_calibration_tx_waveform_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_CONFIG_TX_WF,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = NULL;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_cfg_tx_wf;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = NULL;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = NULL;
  #endif
    

  return ret_val;

} /* ftm_calibration_tx_waveform_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Tx Waveform Config OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Tx Waveform Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_tx_waveform_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_tx_waveform_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Tx Waveform Config OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Tx Waveform Config OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_tx_waveform_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size;

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  payload_size = sizeof(ftm_cal_instr_payload_config_tx_wf_type);

  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][WF_CFG.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_CONFIG_TX_WF,
               payload_size );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return payload_size;

} /* ftm_calibration_tx_waveform_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Tx Waveform Config OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Tx Waveform Config OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_tx_waveform_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_tx_waveform_data_type *cfg_data;
  ftm_cal_instr_payload_config_tx_wf_type *p_ptr;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.tx_wf);
  p_ptr = (ftm_cal_instr_payload_config_tx_wf_type*)oc_disp->payload_ptr;

  /* NULL pointer check */
  if ( p_ptr == NULL )
  {
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_CFG_TX_WF_FAILURE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][WF_CFG.{0x%x}] Process : "
               "NULL payload PTR",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX_WF );
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
                           (1 << FTM_CAL_SEG_ERR_CFG_TX_WF_FAILURE);
        FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][WF_CFG.{0x%x}] Process : "
                   "FP Failure",
                   cal_data->seg_data.cal_instance,
                   FTM_CAL_OPCODE_CONFIG_TX_WF );
      } /* if ( ret_val == FALSE ) */

    } /* if ( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */
    else
    {
      /* Flag segment error */
      cal_data->status.seg_error_log.seg_error |= 
                         (1 << FTM_CAL_SEG_ERR_CFG_TX_WF_NULL_CB);
      FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][WF_CFG.{0x%x}] "
                 "Null FP",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_CONFIG_TX_WF );
    } /* if !( cfg_data->oc_process_fp[current_rf_mode] != NULL ) */

  } /* if ( current_rf_mode < RFM_NUM_MODES  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][WF_CFG.{0x%x}] Process : "
               "Bad RFM Mode",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX_WF );
  } /* if ! ( current_rf_mode < RFM_NUM_MODES  ) */

  /* Debug Message */
  if ( cal_data->cal_config.debug_mode_en == TRUE )
  {
    FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][WF_CFG.{0x%x}] Process : "
               "[Status %d] WF 0x%x StartRB %d NumRB %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_CONFIG_TX_WF,
               ret_val,
               p_ptr->waveform,
               p_ptr->start_rb,
               p_ptr->num_of_rb );
  } /* if ( cal_data->cal_config.debug_mode_en == TRUE ) */

  return ret_val;

} /* ftm_calibration_tx_waveform_process */

/*! @} */

/*============================================================================*/
/*!
  @name Radio Setup OpCode handling

  @brief
  This section contains functions for Radio Setup opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Radio Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of Radio Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_radio_setup_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_radio_setup_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.radio_setup);

  reg_data.unpack_fp = ftm_calibration_radio_setup_unpack;
  reg_data.process_fp = ftm_calibration_radio_setup_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_RADIO_SETUP,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
               ftm_1x_calibration_v3_radio_setup;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                rflte_ftm_calibration_v3_radio_setup;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                ftm_wcdma_calv3_sweep_radio_setup;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                ftm_tdscdma_calv3_sweep_radio_setup;
  #endif
    

  return ret_val;

} /* ftm_calibration_radio_setup_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Radio Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Radio Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_radio_setup_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_radio_setup_deinit */

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
ftm_calibration_radio_setup_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  ftm_cal_radio_setup_instr_payload_type const *p_ptr;
  uint16 payload_size = 0; /* Payload Size */
  uint8 *payload_ptr = NULL;
  ftm_cal_radio_setup_parameter_type *rs_unpack = NULL;
  ftm_cal_radio_setup_chan_info_type *chan_info = NULL;
  uint8 chan_idx;
  ftm_cal_radio_setup_action_type action_type = 0x0;
  ftm_cal_radio_setup_data_type *cfg_data;

  /* Get the pointer */
  p_ptr = (ftm_cal_radio_setup_instr_payload_type*)instr_payload;

  /* NULL Pointer check */
  if ( ( p_ptr == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][RADIO_SETUP.{0x%x}] "
               "Unpack : Null Param",
               FTM_CAL_OPCODE_RADIO_SETUP );
    return 0;
  } /* if ( instr_payload == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.radio_setup);

  /* Get pointer to memory to hold Radio config unpacked data */
  rs_unpack = &(cfg_data->up_param);

  /* Populate static length data from instr_payload */
  rs_unpack->debug_mode_en = p_ptr->debug_mode_en;
  /* Get the opcode version type to unpack the payload if we have 
  a radio setup action word */
  rs_unpack->version = (ftm_cal_opcode_version_type)p_ptr->version;
  if( rs_unpack->debug_mode_en == TRUE )
  {
    FTM_MSG_3(FTM_HIGH,"[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
                     "Opcode version : %d ",
                     cal_data->seg_data.cal_instance + 1,
                     FTM_CAL_OPCODE_RADIO_SETUP,
                     rs_unpack->version);
  }

  rs_unpack->flag = p_ptr->flag;
  rs_unpack->mdevice = p_ptr->master_device;
  rs_unpack->sdevice = p_ptr->slave_device;
  rs_unpack->mdevice_state = p_ptr->master_radio_state;
  rs_unpack->sdevice_state = p_ptr->slave_radio_state;
  rs_unpack->tech = p_ptr->technology;
  rs_unpack->band = p_ptr->band;
  rs_unpack->num_of_chan = p_ptr->num_of_channel;

  /* Make sure that number of channel is not more than 
  FTM_RADIO_SETUP_MAX_CHAN */
  if ( p_ptr->num_of_channel > FTM_RADIO_SETUP_MAX_CHAN )
  {
    ERR_FATAL( "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
               "Num of channel %d ( > FTM_RADIO_SETUP_MAX_CHAN )",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_RADIO_SETUP,
               p_ptr->num_of_channel );
    return 0;
  } /* if ( instr_payload->num_of_channel > FTM_RADIO_SETUP_MAX_CHAN ) */

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)p_ptr;

  /* Update the payload size and Payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     ( sizeof(ftm_cal_radio_setup_instr_payload_type) - 
                       sizeof(ftm_cal_radio_setup_chan_info_type) ) );

  /* Copy the channel param list */
  for ( chan_idx = 0; chan_idx < p_ptr->num_of_channel; chan_idx++ )
  {
    chan_info = (ftm_cal_radio_setup_chan_info_type*)payload_ptr;
    rs_unpack->chan_list[chan_idx].channel = chan_info->channel;
    rs_unpack->chan_list[chan_idx].rx_bw = chan_info->channel_rx_bw;
    rs_unpack->chan_list[chan_idx].tx_bw = chan_info->channel_tx_bw;

    /* Update the payload size and payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                       sizeof(ftm_cal_radio_setup_chan_info_type) );
  } /* for ( chan_idx = 0; chan_idx < instr_payload->num_of_channel; 
       chan_idx++ ) */

  /* Copy the waveform type */
  memscpy( &rs_unpack->wfm_type, sizeof(ftm_cal_radio_setup_wfm_type), 
           payload_ptr, sizeof(ftm_cal_radio_setup_wfm_type) );

  /* Update the payload size and payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                     sizeof(ftm_cal_radio_setup_wfm_type) );

  /* Based on the waveform type update the payload size */
  switch ( rs_unpack->wfm_type )
  {

  case TX_WFM_DEFAULT:
  case TX_WFM_CW:
  case TX_WFM_DPD:
    /* No Payload */
    break;

  case TX_WFM_OFFSET_CW:
    rs_unpack->wfm_param.ocw_wfm.offset_hz = 
      ((ftm_cal_radio_setup_wfm_ocw_info_type*)(payload_ptr))->offset_hz;
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                       sizeof(ftm_cal_radio_setup_wfm_ocw_info_type) );
    break;

  case TX_WFM_OFFSET_LTE_PUSCH:
    rs_unpack->wfm_param.pusch_wfm.num_of_rbs = 
      ((ftm_cal_radio_setup_wfm_pusch_info_type*)(payload_ptr))->num_of_rbs;
    rs_unpack->wfm_param.pusch_wfm.start_rbs = 
      ((ftm_cal_radio_setup_wfm_pusch_info_type*)(payload_ptr))->start_rb;
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                       sizeof(ftm_cal_radio_setup_wfm_pusch_info_type) );
    break;

  case TX_WFM_OFFSET_EUL:
    rs_unpack->wfm_param.eul_wfm.subtest = 
      ((ftm_cal_radio_setup_wfm_eul_info_type*)(payload_ptr))->sub_test;
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
                       sizeof(ftm_cal_radio_setup_wfm_eul_info_type) );
    break;

  default:
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
               "Unpack : Invalid WF %d",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_RADIO_SETUP,
               rs_unpack->wfm_type );
  } /* switch ( wfm_type ) */

  if ( rs_unpack->version == OPCODE_VERSION_V1 )
  {
    /* Copy radio setup action type payload only if tools sent an updated
    version number of the radio setup opcode */
    memscpy( &action_type, sizeof(ftm_cal_radio_setup_action_type),
             payload_ptr, sizeof(ftm_cal_radio_setup_action_type) );
    UPDATE_SIZE_N_PTR(payload_size, payload_ptr,
                      sizeof(ftm_cal_radio_setup_action_type) );

    if ( action_type & (1 <<  FTM_SPLIT_BAND_PATH_OVERRIDE) )
    {
      if ( rs_unpack->debug_mode_en == TRUE )
      {
        FTM_MSG_3(FTM_HIGH,"[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
                "Action Type : %d ",
                cal_data->seg_data.cal_instance + 1,
                FTM_CAL_OPCODE_RADIO_SETUP,
                FTM_SPLIT_BAND_PATH_OVERRIDE );
      }  
      rs_unpack->split_band_path_override.master_path_idx= 
        ((ftm_cal_radio_setup_device_idx_type*)(payload_ptr))->master_path_idx;
      rs_unpack->split_band_path_override.slave_path_idx = 
        ((ftm_cal_radio_setup_device_idx_type*)(payload_ptr))->slave_path_idx;
      rs_unpack->split_band_path_override.valid = TRUE;

      UPDATE_SIZE_N_PTR(payload_size, payload_ptr,
                        sizeof(ftm_cal_radio_setup_device_idx_type) );

      if ( rs_unpack->debug_mode_en == TRUE )
      {
        FTM_MSG_2(FTM_HIGH,"SPLIT BAND ORIDE: Master Path Idx = %d, Slave Path Idx = %d",
                  rs_unpack->split_band_path_override.master_path_idx,
                  rs_unpack->split_band_path_override.slave_path_idx);
      }
    }/*if ( action_type & (1 <<  FTM_SPLIT_BAND_PATH_OVERRIDE) )*/
    else /* Else invalidate the variables and set valid flag to FALSE */
    {
      rs_unpack->split_band_path_override.valid = FALSE;
      rs_unpack->split_band_path_override.master_path_idx = 0xFF;
      rs_unpack->split_band_path_override.slave_path_idx = 0xFF;
    }
    
    if ( action_type & (1 << FTM_ALT_PATH_OVERRIDE ) )
    {
      if ( rs_unpack->debug_mode_en == TRUE )
      {
        FTM_MSG_3(FTM_HIGH,"[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
                  "Action Type : %d ",
                  cal_data->seg_data.cal_instance + 1,
                  FTM_CAL_OPCODE_RADIO_SETUP,
                  FTM_ALT_PATH_OVERRIDE );
      }
      rs_unpack->alt_path_override.master_path_idx = 
        ((ftm_cal_radio_setup_device_idx_type*)(payload_ptr))->master_path_idx;
      rs_unpack->alt_path_override.slave_path_idx = 
        ((ftm_cal_radio_setup_device_idx_type*)(payload_ptr))->slave_path_idx;
      rs_unpack->alt_path_override.valid = TRUE;
      UPDATE_SIZE_N_PTR(payload_size, payload_ptr,
                        sizeof(ftm_cal_radio_setup_device_idx_type) );

      if ( rs_unpack->debug_mode_en == TRUE )
      {
        FTM_MSG_2(FTM_HIGH,"ALT PATH ORIDE: Master Path = %d, Slave Path = %d",
                  rs_unpack->alt_path_override.master_path_idx,
                  rs_unpack->alt_path_override.slave_path_idx);
      }
    }//if ( action_type & (1 << FTM_ALT_PATH_OVERRIDE ) )
    else /* Else invalidate the variables and set valid flag to FALSE */
    {
      rs_unpack->alt_path_override.valid = FALSE;
      rs_unpack->alt_path_override.master_path_idx = 0xFF;
      rs_unpack->alt_path_override.slave_path_idx = 0xFF;
    }
  }/* if ( rs_unpack->version == OPCODE_VERSION_V1 ) */
  /* Even if the opcode version is 0, still set valid flags to FALSE
  to prevent triggering in common dispatch implementation */
  else
  {
    rs_unpack->split_band_path_override.valid = FALSE;
    rs_unpack->alt_path_override.valid = FALSE;
  }

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  /* Debug message */
  if ( rs_unpack->debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_RADIO_SETUP,
               payload_size );
  } /* if ( rs_unpack->debug_mode_en == TRUE ) */

  /* Update the Calibration Data structure with Band and Channel; so that
      other opcodes can use this information. */
  cal_data->status.curr_band = rs_unpack->band;
  cal_data->status.curr_chan = rs_unpack->chan_list[0].channel;

  /* return Payload size */
  return payload_size;

} /* ftm_calibration_radio_setup_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Radio Setup OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Radio Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_radio_setup_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  ftm_cal_radio_setup_fp_type rs_fp;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_radio_setup_parameter_type *rs_data;

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][RADIO_SETUP.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_RADIO_SETUP );
    return 0;
  } /* if ( cal_data == NULL ) */

  /* Get radio setup unopacked data */
  rs_data = &(cal_data->seg_data.oc_payload.radio_setup.up_param);

  /* Store the current Mode enum */
  cal_data->status.current_rf_mode = ftm_common_ftm_tech_to_rfm_mode(
     (ftm_rf_technology_type)rs_data->tech);
  
  /* Get current mode */
  current_rf_mode = cal_data->status.current_rf_mode;

  /* Make sure that the current mode is valid */
  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /* Get the Radio Setup Data */
    rs_fp = \
      cal_data->seg_data.oc_payload.radio_setup.oc_process_fp[current_rf_mode];
    
    /* Execute the callback function only if it is valid */
    if ( rs_fp != NULL )
    {
      ret_val &= rs_fp( rs_data );
    } /* if ( rs_data->cb_func != NULL ) */
    else
    {
      FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
                 "Process : Null FP %d",
                 cal_data->seg_data.cal_instance,
                 FTM_CAL_OPCODE_RADIO_SETUP,
                 cal_data->status.current_rf_mode );
      ret_val = FALSE;
    } /* if ! ( rs_data->cb_func != NULL ) */
  } /* if ( cal_data->current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */
  else
  {
    FTM_MSG_3( FTM_ERROR, "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
               "Process : Bad RFM Mode %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RADIO_SETUP,
               cal_data->status.current_rf_mode );
    ret_val = FALSE;
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS ) */

  if ( rs_data->debug_mode_en == TRUE )
  {
    FTM_MSG_9( FTM_HIGH, "[calv3][Segment %3d][RADIO_SETUP.{0x%x}] "
               "Process : [Status %d] mDev%d:%d sDev%d:%d Band %d "
               "NumChan %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_RADIO_SETUP,
               ret_val,
               rs_data->mdevice, 
               rs_data->mdevice_state,
               rs_data->sdevice, 
               rs_data->sdevice_state,
               rs_data->band, 
               rs_data->num_of_chan );
  } /* if ( rs_data->debug_mode_en == TRUE ) */

  /* return */
  return ret_val;

} /* ftm_calibration_radio_setup_process */

/*! @} */

/*============================================================================*/
/*!
  @name Feedback Setup OpCode handling

  @brief
  This section contains functions for Feedback Setup opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Feedback Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of FeedBback Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_feedback_setup_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_feedback_setup_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.feedback_setup);

  reg_data.unpack_fp = ftm_calibration_feedback_setup_unpack;
  reg_data.process_fp = ftm_calibration_feedback_setup_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;
  reg_data.res_buff_info.buff_sz = 0;

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_FEEDBACK_SETUP,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE] = 
                                      ftm_1x_calibration_v3_feedback_setup;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE] = 
                                      rflte_ftm_calibration_v3_feedback_setup;
  #endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE] = 
                                      ftm_wcdma_calv3_sweep_feedback_setup;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE] = 
                                      ftm_tdscdma_calv3_sweep_feedback_setup;
  #endif

  return ret_val;

} /* ftm_calibration_feedback_setup_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize Feedback Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of Feedback Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_feedback_setup_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  return TRUE;
} /* ftm_calibration_feedback_setup_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack Feedback Setup OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of Feedback Setup OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_feedback_setup_unpack
( 
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint16 payload_size = 0; /* Payload Size */
  uint8 *payload_ptr = NULL;
  ftm_cal_feedback_setup_instr_payload_type const *p_ptr;
  ftm_cal_feedback_setup_parameter_type *rs_unpack = NULL;
  ftm_cal_feedback_setup_instr_payload_configure_wtr_type *conf_wtr = NULL;
  ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_xpt_type *conf_rxlm_for_xpt = NULL;
  ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_fbagc_type *conf_rxlm_for_fbagc = NULL;
  ftm_calibration_feedback_setup_action_type feedback_setup_action;

  ftm_cal_feedback_setup_data_type *cfg_data;

  /* Get the pointer */
  p_ptr = (ftm_cal_feedback_setup_instr_payload_type*)instr_payload;

  /* NULL Pointer check */
  if ( ( p_ptr == NULL ) || ( cal_data == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][FEEDBACK_SETUP.{0x%x}] "
               "Unpack : Null Param",
               FTM_CAL_OPCODE_FEEDBACK_SETUP );
    return 0;
  } /* if ( instr_payload == NULL ) */

  cfg_data = &(cal_data->seg_data.oc_payload.feedback_setup);

  /* Get pointer to memory to hold Radio config unpacked data */
  rs_unpack = &(cfg_data->up_param);

  /* Populate static length data from instr_payload */
  rs_unpack->debug_mode_en = 
    (boolean)(p_ptr->feedback_setup_header.debug_mode_en);
  rs_unpack->flag = p_ptr->feedback_setup_header.flag;
  rs_unpack->primary_device = 
    (rfm_device_enum_type)(p_ptr->feedback_setup_header.primary_device);
  rs_unpack->feedback_device = 
    (rfm_device_enum_type)(p_ptr->feedback_setup_header.feedback_device);
  rs_unpack->feedback_setup_action = 
    (ftm_calibration_feedback_setup_action_type)
    (p_ptr->feedback_setup_header.feedback_setup_action);

  feedback_setup_action = rs_unpack->feedback_setup_action;

  /* Initialize the payload pointer */
  payload_ptr = (uint8*)p_ptr;

  /* Update the payload size and Payload pointer */
  UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
         sizeof(ftm_cal_feedback_setup_instr_parameter_header_type) );

  if ( feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR )
  {
    conf_wtr = 
      (ftm_cal_feedback_setup_instr_payload_configure_wtr_type *) payload_ptr;

    rs_unpack->configure_wtr.enable_mode = conf_wtr->enable_mode;
    rs_unpack->configure_wtr.fb_state = conf_wtr->fb_state;

    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
         sizeof(ftm_cal_feedback_setup_instr_payload_configure_wtr_type) );
  }

  if ( feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT )
  {
    conf_rxlm_for_xpt =
      (ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_xpt_type *) payload_ptr;

    rs_unpack->configure_rxlm_for_xpt.enable_mode = conf_rxlm_for_xpt->enable_mode;

    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
      sizeof(ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_xpt_type) );
  }

  if ( feedback_setup_action & FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC )
  {
    conf_rxlm_for_fbagc =
      (ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_fbagc_type *) payload_ptr;

    rs_unpack->configure_rxlm_for_fbagc.enable_mode = conf_rxlm_for_fbagc->enable_mode;

    /* Update the payload size and Payload pointer */
    UPDATE_SIZE_N_PTR( payload_size, payload_ptr, 
      sizeof(ftm_cal_feedback_setup_instr_payload_configure_rxlm_for_fbagc_type) );
  }
  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  /* Debug message */
  if ( p_ptr->feedback_setup_header.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_HIGH, "[calv3][Segment %3d][FEEDBACK_SETUP.{0x%x}] "
               "Unpack : %d bytes",
               cal_data->seg_data.cal_instance + 1,
               FTM_CAL_OPCODE_FEEDBACK_SETUP,
               payload_size );
  } /* if ( p_ptr->feedback_config_header.debug_mode_en == TRUE ) */

  /* return Payload size */
  return payload_size;

} /* ftm_calibration_feedback_setup_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process Feedback Setup OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the Feedback Setup OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_feedback_setup_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  boolean ret_val = TRUE;
  ftm_cal_feedback_setup_fp_type fbsetup_fp;
  rfm_mode_enum_type current_rf_mode;
  ftm_cal_feedback_setup_parameter_type *fbsetup_data;

  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Variable */

  /* NULL Pointer check */
  if ( cal_data == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "[calv3][Segment X][FEEDBACK_SETUP.{0x%x}] "
               "Process : Null Argument",
               FTM_CAL_OPCODE_FEEDBACK_SETUP );
    return FALSE;
  } /* if ( cal_data == NULL ) */

  /* Get radio setup unopacked data */
  fbsetup_data = &(cal_data->seg_data.oc_payload.feedback_setup.up_param);

  if ( fbsetup_data->debug_mode_en == TRUE )
  {
    prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */
    
  } /* if ( p_ptr->feedback_setup_header.debug_mode_en == TRUE ) */

  current_rf_mode = cal_data->status.current_rf_mode;

  fbsetup_fp = \
    cal_data->seg_data.oc_payload.feedback_setup.oc_process_fp[current_rf_mode];

  if ( fbsetup_fp != NULL )
  {
    ret_val &= fbsetup_fp(fbsetup_data);
  }
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][Segment %3d][FEEDBACK_SETUP.{0x%x}] "
               "Process : Null FP %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_FEEDBACK_SETUP);
    ret_val = FALSE;
  } /* if ! ( fbsetup_fp != NULL ) */

  if (fbsetup_data->debug_mode_en == TRUE )
  {
    /* Get time taken for this function to execute */
    prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

    FTM_MSG_6( FTM_HIGH, "[calv3][Segment %3d][FEEDBACK_SETUP.{0x%x}] "
               "Process : Status %d, pDev %d, fbDev %d, Action: %d",
               cal_data->seg_data.cal_instance,
               FTM_CAL_OPCODE_FEEDBACK_SETUP,
               ret_val,
               fbsetup_data->primary_device, 
               fbsetup_data->feedback_device,
               fbsetup_data->feedback_setup_action);

    FTM_MSG_1( FTM_LOW, "ftm_calibration_feedback_setup_process() "
                        "execution time %d",prof_cal_func_val);
  } /* if ( fbsetup_data->debug_mode_en == TRUE ) */

  /* return */
  return ret_val;

} /* ftm_calibration_feedback_setup_process */

/*! @} */

/*! @} */

