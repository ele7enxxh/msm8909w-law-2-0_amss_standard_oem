/*!
  @file
  ftm_common_calibration_v3_xpt.c

  @brief
  This module contains common RF calibration code for XPT

  @addtogroup RF_COMMON_FTM_CALV3_XPT

  @brief
  This module provides a XPT extensions to calibration v3 framework
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_calibration_v3_xpt.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/09/14   ych     Added XPT IQ capture offloading to RF thread
11/18/13   cri     Support for SW allocation of DPD sample capture buffers
08/29/13   jmf     [xPT] Integrate leakage reduction in delay cal call flow
08/19/13   kb      [XPT] Support for cal_config and iq_capture API for TDS.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/22/13   jmf     Implement xPT processing Enum Type
04/16/13   aro     Moved Opcode Data to HK data structure
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
04/03/13   aro     Compiler wanring fix for GPS-only builds
04/01/13   aro     Fixed random buffer size initialization
04/01/13   aro     Debug flag to switch between old and new calv3 implementation
04/01/13   aro     Added hookup for new calv3 infrastructure
04/01/13   aro     F3 update
03/28/13   kai     Added xpt opcode support for WCDMA
03/26/13   cri     XPT opcode support for LTE 
03/18/13   aro     Changes to be compatible with XPT opcode defintion
                   update
01/22/13   aro     Removed DPD index from IQ capture interface
01/22/13   aro     Renamed IQ capture and config interfaces
01/18/13   aro     Warning fix
01/18/13   aro     Deleted nikel-XPT specific code
12/06/12   aro     F3 update
11/17/12   aro     Updated XPT results to have result type field
11/15/12   aro     Mainlined enabling of variable length OpCode
11/15/12   aro     F3 message update
11/05/12   aro     Re-organized cal v3 data structure to be more modular
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/17/12   aro     Updated Payload size computation with FB Config Opcode
08/11/12   vs      Fix incorrect payload size for XPT path gain
08/07/12   aro     Tips compilation error fix
08/07/12   vs      Modifications to Cal V3 Tx config data packet
08/04/12   vs      Add IQ gain setpoint in Cal V3 Tx config data packet
07/26/12   aro     F3 message update
07/25/12   aro     XPT Payload update
07/25/12   aro     Grouped XPT header into new structure
07/25/12   aro     F3 message update for EPT
07/25/12   aro     Added time profiling for EPT
07/25/12   aro     Enabled XPT debug mode flag
07/19/12   aro     Documentation update
07/19/12   aro     80 chars for ftm_calibration_xpt_param_init()
07/19/12   aro     Added initial implementation of Polling OpOde
07/19/12   aro     F3 update + XPT variable name change
07/18/12   aro     Converted results push function to STATIC for this file 
07/18/12   aro     Renamed the XPT type definitions
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/17/12   aro     Updated XPT Opcode payload
07/17/12   aro     Enhanced support of configure feedback path
07/17/12   aro     Added XPT opcode to configure feedback path
07/17/12   aro     Renamed XPT opcode enums
07/17/12   aro     F3 update
07/06/12   aro     F3 message Update
07/06/12   aro     Rearranged arguments for the XPT process function
06/21/12   aro     F3 message Update for XPT processing
06/14/12   anr     Updates and bug fixes to common framework for EPT 
06/14/12   aro     Added Doxygen block
06/14/12   aro     Added function to populate EPT results buffer
06/13/12   aro     Doxygen documentation update
06/13/12   aro     Updated function to compute paylaod size to pass the pointer
                   to cal v3 data
06/13/12   aro     Copyright date
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/05/12   anr     Initial EPT Support
06/04/12   ks      Add FTM-PC interface for XPT


==============================================================================*/

#include "stringl.h" /* for memscpy */
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_xpt.h"
#include "ftm_msg.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "ftm_1x_calibration.h"
#ifdef FEATURE_LTE
#include "rflte_ftm_calibration.h"
#endif
#include "ftm_wcdma_calv3.h"
#include "ftm_tdscdma_calv3.h"
#endif
#include "rfcommon_core_txlin.h"
#include "rfcommon_msg.h"
#include "rfcommon_mc.h"
#include "rfm_internal.h"
#include "modem_mem.h"
#include "rfc_card.h"
#include "rfdevice_cmn_intf.h"

/*============================================================================*/
/*!
  @name EPT Results
 
  @brief
  Functions specific to EPT calibration results
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populate the results buffer for EPT
 
  @details
  This function will populate the results buffer with the ept coefficient
  produced as a result of IQ capture.
 
  @param xpt_result
  Pointer to the EPT results buffer
 
  @param ept_coeff
  Pointer to the structure containing the EPT coefficients from RF
 
  @return
  Flag indicating the status of function
*/
static boolean
ftm_calibration_xpt_populate_results
(
  ftm_cal_opcode_dispatch_info_type *oc_disp,
  ftm_calibration_xpt_action_type xpt_opcode,
  ftm_calibration_xpt_error_code_type xpt_error_code,
  ftm_calibration_xpt_result_mask_type xpt_result_mask,
  rfcommon_ept_coefficient_type *ept_coeff
)
{
  uint8 *res_write_ptr; /* results write pointer */
  uint16 written_size = 0; /* Total written size */
  uint8 res_type = FTM_CAL_OPCODE_XPT;
  ftm_calibration_xpt_result_version_type version = 0;

  /*------------------------------------------------------------------------*/
  /* Populate the results buffer */
  /*------------------------------------------------------------------------*/

  /* Initialize the write pointer */
  res_write_ptr = oc_disp->res_buff_info.buff_ptr;

  /*==========================================================================*/
  /* Populate Results Type */
  /*==========================================================================*/

  memscpy( res_write_ptr, sizeof(uint8), 
           &(res_type),sizeof(uint8));
  res_write_ptr += sizeof(uint8);
  written_size +=sizeof(uint8);

  /*==========================================================================*/
  /* Populate Version */
  /*==========================================================================*/

  memscpy( res_write_ptr, sizeof(ftm_calibration_xpt_result_version_type),
           &(version),sizeof(ftm_calibration_xpt_result_version_type));
  res_write_ptr += sizeof(ftm_calibration_xpt_result_version_type);
  written_size +=sizeof(ftm_calibration_xpt_result_version_type);


  /*==========================================================================*/
  /* Populate Action */
  /*==========================================================================*/

  memscpy( res_write_ptr, sizeof( ftm_calibration_xpt_action_type),
           &(xpt_opcode),sizeof(ftm_calibration_xpt_action_type));
  res_write_ptr += sizeof(ftm_calibration_xpt_action_type);
  written_size +=sizeof(ftm_calibration_xpt_action_type);

  /*==========================================================================*/
  /* Populate Error Code */
  /*==========================================================================*/

  memscpy( res_write_ptr, sizeof(ftm_calibration_xpt_error_code_type),
           &(xpt_error_code),sizeof(ftm_calibration_xpt_error_code_type));
  res_write_ptr += sizeof(ftm_calibration_xpt_error_code_type);
  written_size +=sizeof(ftm_calibration_xpt_error_code_type);
  
  /* Update the number of result component */
  oc_disp->res_buff_info.buff_sz = written_size;

  FTM_MSG_2( FTM_LOW, "[calv3][XPT][0x%x] : Result %d bytes",
             FTM_CAL_OPCODE_XPT,
             oc_disp->res_buff_info.buff_sz );

  return TRUE;
} /* ftm_calibration_xpt_populate_results */

/*! @} */

/*============================================================================*/
/*!
  @name XPT OpCode handling

  @brief
  This section contains functions for XPT opcode handling.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize XPT OpCode
 
  @details
  This function is called from common sweep function to perform
  data initialization of XPT OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of initialization
*/
boolean
ftm_calibration_xpt_init
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_opcode_reg_data_type reg_data;
  ftm_cal_xpt_data_type *cfg_data;

  cfg_data = &(cal_data->seg_data.oc_payload.xpt);

  reg_data.unpack_fp = ftm_calibration_xpt_unpack;
  reg_data.process_fp = ftm_calibration_xpt_process;
  reg_data.res_buff_info.buff_ptr = NULL;
  reg_data.res_buff_info.buff_sz_max = 0;

  if ( cfg_data->res_buff == NULL )
  {
    /* Allocate memory for results buffer to store results for the current
    segment */
    cfg_data->res_buff = modem_mem_alloc( FTM_CAL_SEG_XPT_BUFF_SIZE_MAX,
                                          MODEM_MEM_CLIENT_RFA );

    /* Check if the malloc was successfull */
    if ( cfg_data->res_buff != NULL )
    {
      reg_data.res_buff_info.buff_ptr = cfg_data->res_buff;
      reg_data.res_buff_info.buff_sz_max = FTM_CAL_SEG_XPT_BUFF_SIZE_MAX;
      reg_data.res_buff_info.buff_sz = 0;
    } /* if ( cfg_data->rx_result.rx_buffer != NULL ) */
    else
    {
      FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
                 "Result buff malloc failed", FTM_CAL_OPCODE_XPT,
                 cal_data->seg_data.cal_instance );
      reg_data.res_buff_info.buff_ptr = 0;
      reg_data.res_buff_info.buff_sz_max = 0;
      reg_data.res_buff_info.buff_sz = 0;
      ret_val = FALSE;
    } /* if !( cfg_data->rx_result.rx_buffer != NULL ) */
  } /* if (  cfg_data->res_buff == NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
               "Buff not NULL", FTM_CAL_OPCODE_XPT,
               cal_data->seg_data.cal_instance );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff == NULL ) */

  /* Register OpCOde */
  ret_val = ftm_calibration_register_opcode( cal_data,
                                             FTM_CAL_OPCODE_XPT,
                                             reg_data );

  /* Init tech specific functions */
  #ifdef FEATURE_CDMA1X
  cfg_data->oc_process_fp[RFM_1X_MODE].configure_ept_fp = 
                         ftm_1x_cal_xpt_configure;
  cfg_data->oc_process_fp[RFM_1X_MODE].capture_ept_iq_fp = 
                         ftm_1x_cal_xpt_iq_capture;
  #endif

  #ifdef FEATURE_LTE
  cfg_data->oc_process_fp[RFM_LTE_MODE].configure_ept_fp = 
                         rflte_ftm_calibration_xpt_configure;
  cfg_data->oc_process_fp[RFM_LTE_MODE].capture_ept_iq_fp = 
                         rflte_ftm_calibration_xpt_iq_capture;
#endif

  #ifdef FEATURE_WCDMA
  cfg_data->oc_process_fp[RFM_IMT_MODE].configure_ept_fp = 
                         ftm_wcdma_cal_xpt_configure;
  cfg_data->oc_process_fp[RFM_IMT_MODE].capture_ept_iq_fp = 
                         ftm_wcdma_calv3_sweep_xpt_iq_capture;
  #endif

  #ifdef FEATURE_TDSCDMA
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE].configure_ept_fp = 
                          ftm_tdscdma_cal_xpt_configure;
  cfg_data->oc_process_fp[RFM_TDSCDMA_MODE].capture_ept_iq_fp = 
                          ftm_tdscdma_calv3_sweep_xpt_iq_capture;
  #endif
    
  return ret_val;

} /* ftm_calibration_xpt_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deinitialize XPT OpCode
 
  @details
  This function is called from common sweep function to perform
  data deinitialization of XPT OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
  
  @return
  Status of deinitialization
*/
boolean
ftm_calibration_xpt_deinit
(
  struct ftm_calibration_data_type_t *cal_data
)
{
  boolean ret_val = TRUE;
  ftm_cal_xpt_data_type *cfg_data;

  /* Shorten data structure pointers */
  cfg_data = &(cal_data->seg_data.oc_payload.xpt);

  if ( cfg_data->res_buff != NULL )
  {
    /* De-allocate memory which was allocated for results */
    modem_mem_free( cfg_data->res_buff, MODEM_MEM_CLIENT_RFA );
    cfg_data->res_buff = NULL;
  } /* if ( cfg_data->res_buff != NULL ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
               "NULL Buffer", FTM_CAL_OPCODE_XPT,
               cal_data->seg_data.cal_instance );
    ret_val = FALSE;
  } /* if ! ( cfg_data->res_buff != NULL ) */

  return ret_val;
} /* ftm_calibration_xpt_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unpack XPT OpCode
 
  @details
  This function is called from common sweep function to perform
  unpacking of XPT OpCode payload
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param instr_payload
  Pointer to the memory containing the payload
 
  @return
  Total size of the payload
*/
uint16
ftm_calibration_xpt_unpack
(
  struct ftm_calibration_data_type_t *cal_data,
  struct ftm_cal_opcode_dispatch_info_type_t *oc_disp,
  void const *instr_payload
)
{
  uint8 payload_size = 0; /* Payload Size */
  ftm_cal_xpt_instr_payload_type *p_ptr;
  ftm_calibration_xpt_action_type xpt_opcode;

  p_ptr = (ftm_cal_xpt_instr_payload_type*)instr_payload;

  xpt_opcode = p_ptr->xpt_header.xpt_action;

  /* Update the Payload size */
  payload_size += sizeof(ftm_cal_xpt_instr_payload_header_type);

  /* Update the Payload size if EPT CONFIG sub opcode is enabled */
  if(xpt_opcode & XPT_OPCODE_MASK_EPT_CONFIG)
  {
    payload_size += sizeof(ftm_cal_xpt_instr_payload_ept_config_type);    
  }

  /* Update the Payload size if EPT CONFIG sub opcode is enabled */
  if(xpt_opcode & XPT_OPCODE_MASK_EPT_CAPTURE_IQ)
  {
    payload_size += sizeof(ftm_cal_xpt_instr_payload_iq_capture_type);
  }

  /* Store the instruction Pointer */
  oc_disp->payload_ptr = (uint8*)instr_payload;

  /* Print F3 message, if debug mode is enabled */
  if ( p_ptr->xpt_header.debug_mode_en == TRUE )
  {
    FTM_MSG_3( FTM_MED, "[calv3][XPT][0x%x] : Instance %d : "
               "Unpacked Payload &d bytes", FTM_CAL_OPCODE_XPT,
               cal_data->seg_data.cal_instance, payload_size );
  } /* if ( cal_data->debug_mode_en == TRUE ) */

  /* return Payload size */
  return payload_size;
} /* ftm_calibration_radio_config_unpack */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process XPT OpCode
 
  @details
  This function is called from common sweep function to perform the
  processing of the XPT OpCode
 
  @param cal_data
  Pointer to the data structure containing calibration data
 
  @param oc_disp
  Pointer to the data structure containing current opcode dispatch data
 
  @return
  Status of opcode processing
*/
boolean
ftm_calibration_xpt_process
(
  struct ftm_calibration_data_type_t *cal_data,
  ftm_cal_opcode_dispatch_info_type *oc_disp
)
{
  ftm_cal_xpt_instr_payload_type *p_ptr;
  uint8* curr_p_ptr;
  boolean cb_return = FALSE; /* Flag to store Callback return */
  ftm_calibration_xpt_fp_type* curr_xpt_fp_set;
  ftm_calibration_xpt_action_type xpt_action;
  boolean return_val = TRUE;
  ftm_cal_xpt_instr_payload_ept_config_type *ept_config_payload;
  ftm_cal_xpt_instr_payload_iq_capture_type *ept_capture_iq_payload;
  ftm_cal_xpt_data_type *cfg_data;
  rfm_mode_enum_type current_rf_mode;

  ftm_calibration_xpt_result_mask_type xpt_result_mask = 0x0;
  ftm_calibration_xpt_error_code_type xpt_error_code = 0x0;
  rfcommon_ept_coefficient_type ept_coeff;
  rf_time_tick_type prof_cal_func ; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Variable */
  boolean is_reduced_leakage_on = FALSE;
  const rfc_common_logical_device_params_type *common_logical_device_ptr = NULL;

  cfg_data = &(cal_data->seg_data.oc_payload.xpt);
  p_ptr = (ftm_cal_xpt_instr_payload_type*)oc_disp->payload_ptr;
  curr_p_ptr = (uint8*)p_ptr;

  current_rf_mode = cal_data->status.current_rf_mode;

  /* ------------------------------------------------------------------------ */
  if ( p_ptr->xpt_header.debug_mode_en == TRUE )
  {
    prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */
    
  } /* if ( xpt_instr_payload->debug_mode_en == TRUE ) */
  /* ------------------------------------------------------------------------ */

  xpt_action = p_ptr->xpt_header.xpt_action;
  curr_p_ptr += sizeof(ftm_cal_xpt_instr_payload_header_type); 

  curr_xpt_fp_set = &(cfg_data->oc_process_fp[current_rf_mode]);

  if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS )
  {
    /*------------------------------------------------------------------------*/
    /* 0x1 : EPT CONFIG */
    /*------------------------------------------------------------------------*/
    if( xpt_action & XPT_OPCODE_MASK_EPT_CONFIG )
    {
      ept_config_payload = (ftm_cal_xpt_instr_payload_ept_config_type*)
                                              curr_p_ptr; 
   
      curr_p_ptr +=
         sizeof(ftm_cal_xpt_instr_payload_ept_config_type);

      if ( curr_xpt_fp_set->configure_ept_fp != NULL )
      {
        if ( p_ptr->xpt_header.debug_mode_en == TRUE )
        {
          FTM_MSG_4( FTM_HIGH, "[calv3][XPT][0x%x] : Instance %d : "
                     "XPT Config Dev [%d, %d]", 
                     FTM_CAL_OPCODE_XPT,
                     cal_data->seg_data.cal_instance,
                     ept_config_payload->primary_device,
                     ept_config_payload->feedback_device );


        } /* if ( xpt_instr_payload->debug_mode_en == TRUE ) */

        rfcommon_mc_allocate_dpd_buffer();

        /* Call tech specific CB function to configure ept*/
        cb_return = (curr_xpt_fp_set->configure_ept_fp)(ept_config_payload);
        /* If CB returns FALSE, then Flag segment error */
        if ( cb_return == FALSE )
        {
          xpt_error_code |= (1 << XPT_ERR_CAL_CONFIG_FAILURE);
          return_val = FALSE;
        } /* if ( cb_return == FALSE ) */
      } /* Configure EPT CB is not NULL */
      else
      {
        /* Flag segment error */
        xpt_error_code |= (1 << XPT_ERR_NULL_CB);
        FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
                   "Config XPT Null CB", FTM_CAL_OPCODE_XPT,
                   cal_data->seg_data.cal_instance );

        return_val = FALSE;
      } /* Configure EPT CB is not NULL */
    } /* if( xpt_opcode & XPT_OPCODE_MASK_EPT_CONFIG ) */

    /*------------------------------------------------------------------------*/
    /* 0x2 : IQ CAPTURE */
    /*------------------------------------------------------------------------*/
    if(xpt_action & XPT_OPCODE_MASK_EPT_CAPTURE_IQ)
    {
      ept_capture_iq_payload = (ftm_cal_xpt_instr_payload_iq_capture_type*)
                                              curr_p_ptr; 
   
      curr_p_ptr +=
         sizeof(ftm_cal_xpt_instr_payload_iq_capture_type);
   
      if ( curr_xpt_fp_set->capture_ept_iq_fp != NULL )
      {
        if ( p_ptr->xpt_header.debug_mode_en == TRUE )
        {
          FTM_MSG_9( FTM_HIGH, "[calv3][XPT][0x%x] : Instance %d : "
                     "IQ Capture Dev [%d, %d] Samples %d FTrig %d, "
                     "LTrig:%d, ProcFlag %d, delayCap %d", 
                     FTM_CAL_OPCODE_XPT,
                     cal_data->seg_data.cal_instance,
                     ept_capture_iq_payload->primary_device,
                     ept_capture_iq_payload->feedback_device,
                     ept_capture_iq_payload->num_samples,
                     ept_capture_iq_payload->first_trigger,
                     ept_capture_iq_payload->last_trigger,
                     ept_capture_iq_payload->proc_flag,
                     ept_capture_iq_payload->capture_type );
        } /* if ( xpt_instr_payload->debug_mode_en == TRUE ) */
        

        if ( ((rfcommon_xpt_capture_type)ept_capture_iq_payload->capture_type == XPT_CAPTURE_DELAY) && (ept_capture_iq_payload->first_trigger == 1) )
        {
          common_logical_device_ptr = rfc_common_get_logical_device_params(ept_capture_iq_payload->feedback_device);
          if (common_logical_device_ptr == NULL)
          {
            FTM_MSG_1( FTM_ERROR, "ftm_calibration_xpt_process: "
                       "NULL logical device for device %d",
                       ept_capture_iq_payload->feedback_device);
          }
          else
          {
            FTM_MSG_1( FTM_HIGH, "ftm_calibration_xpt_process: RF device %d reduced leakage cfg ON.",
                       ept_capture_iq_payload->feedback_device );
            is_reduced_leakage_on = TRUE;
            rfdevice_cmn_cmd_dispatch( common_logical_device_ptr->cmn_device,
                                       RFDEVICE_CFG_REDUCED_LEAKAGE,
                                       &is_reduced_leakage_on );
          }
        }
        /* Call tech specific CB function to ept capture iq*/
        cb_return = (curr_xpt_fp_set->capture_ept_iq_fp)(
                      ept_capture_iq_payload);

            
        /* If CB returns FALSE, then Flag segment error */
        if ( cb_return == FALSE )
        {
          if  ((rfcommon_xpt_capture_type)ept_capture_iq_payload->capture_type == XPT_CAPTURE_DELAY) 
          {
            common_logical_device_ptr = rfc_common_get_logical_device_params(ept_capture_iq_payload->feedback_device);
            if (common_logical_device_ptr == NULL)
            {
              FTM_MSG_1( FTM_ERROR, "ftm_calibration_xpt_process: "
                         "NULL logical device for device %d",
                         ept_capture_iq_payload->feedback_device);
            }
            else
            {
              FTM_MSG_1( FTM_HIGH, "ftm_calibration_xpt_process: RF device %d reduced leakage cfg OFF.",
                         ept_capture_iq_payload->feedback_device );
              is_reduced_leakage_on = FALSE;
              rfdevice_cmn_cmd_dispatch( common_logical_device_ptr->cmn_device,
                                         RFDEVICE_CFG_REDUCED_LEAKAGE,
                                         &is_reduced_leakage_on );
            }
          }
          xpt_error_code |= (1 << XPT_ERR_IQ_CAPTURE_FAILURE);
          return_val = FALSE;
        } /* if ( cb_return == FALSE ) */

      } /* ept capture iq CB is not NULL */
      else
      {
        /* Flag segment error */
        xpt_error_code |= (1 << XPT_ERR_NULL_CB);
        return_val = FALSE;

        FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
                   "IQ Capture Null CB", FTM_CAL_OPCODE_XPT,
                   cal_data->seg_data.cal_instance );

      } /* ept capture iq CB is not NULL */
    } /* if(xpt_opcode & XPT_OPCODE_MASK_EPT_CAPTURE_IQ) */

    /* Populate the results buffer */
    return_val &= ftm_calibration_xpt_populate_results( oc_disp,
                                                        xpt_action,
                                                        xpt_error_code,
                                                        xpt_result_mask,
                                                        &ept_coeff );

  } /* if ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */
  else
  {
    /* Flag segment error */
    cal_data->status.seg_error_log.seg_error |= 
                       (1 << FTM_CAL_SEG_ERR_INVALID_RFM_MODE);
    FTM_MSG_2( FTM_ERROR, "[calv3][XPT][0x%x] : Instance %d : "
               "Bad RFM Mode", FTM_CAL_OPCODE_XPT,
               cal_data->seg_data.cal_instance );
  } /* if ! ( current_rf_mode < FTM_CALIBRATION_NUM_OF_TECHS  ) */
    
  /* ------------------------------------------------------------------------ */
  if ( p_ptr->xpt_header.debug_mode_en == TRUE )
  {
    /* Get time taken for this function to execute */
    prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

    FTM_MSG_4( FTM_HIGH, "[calv3][XPT][0x%x] : Instance %d : "
               "XPTAction %d Time %d", FTM_CAL_OPCODE_XPT,
               cal_data->seg_data.cal_instance,
               xpt_action, prof_cal_func_val );
  } /* if ( cal_data->debug_mode_en == TRUE ) */
  /* ------------------------------------------------------------------------ */

 return return_val;

} /* ftm_calibration_xpt_process */

/*! @} */

/*============================================================================*/
/*!
  @name XPT Helper function
  
  @brief
  Helper function for XPT operation
 
  @details
  This function will map processing type of XPT to processing flag.
 
  @param cal_data
  Return proper processing flag

*/
/*! @{ */

rfcommon_xpt_processing_type
ftm_calibration_xpt_map_proc_type(ftm_calibration_xpt_iq_capture_proc_enum xpt_proc_flag)
{
  rfcommon_xpt_processing_type mapped_proc_flag;
  switch (xpt_proc_flag)
  {
  case XPT_IQCAP_PROC_0:
    mapped_proc_flag = PROC_NONE;
    break;
  case XPT_IQCAP_PROC_1:
    mapped_proc_flag = PROC_AM_PM_CURVE;
    break;
  case XPT_IQCAP_PROC_2:
    mapped_proc_flag = PROC_DPD;
    break;
  default:
    mapped_proc_flag = PROC_DPD; // do full processing by default
  }
  return mapped_proc_flag;
}

/*! @} */

/*! @} */

