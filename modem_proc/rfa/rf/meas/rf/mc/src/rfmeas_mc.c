/*===========================================================================


   R F  D r i v e r  Meas(InterFreq meas) Main Control


DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the WCDMA Main Control.

Copyright (c) 2008 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/rf/mc/src/rfmeas_mc.c#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/16   vs      Dont call W meas exit twice
01/22/15   as      Remove duplicate WTR power vote off during L2L abort
07/31/15   par     Remove shadow backup code as that is not needed anymore
11/21/14   par     Adding additional debug info for iRAT
08/25/14   piy     Removed Klockwork Errors
08/21/14   piy     Removed Compiler Warnings
08/11/14   par     Make sure the reset script is called once for target and once for source techs
11/27/13   pl      clean trigger buffer before building tune-back scripts
10/31/13   shb     Switch to C++ TRX common interface
10/18/13   pl      support building for preload and trigger script
07/22/13   nsh     Reduce f3 msg
05/31/13   pl      Avoid backing up shadow when building X2G cleanup script
04/24/13   pl      Enable Shadow back up and restore
04/08/13   pl      Adding pre-configuration into the meas framework
12/13/12   sc      Fix Klocwork errors
11/07/12   pl      Move buffer clean from script building to meas framework
11/07/12   pl      Added building of cleanup script building
10/05/12   aa      avoid W measure to be called twice in case of wtow ifreq
10/04/12   sr      Corrected the order of API calls, first call the target tech API
                   and then the src tech API. 
10/04/12   sr      IRAT code cleanup for dime. 
03/29/12   jfc     Declare global meas_script_entry_no variables
10/14/08   sr      Initial version of meas Main Control.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfmeas_types.h"
#include "rfmeas_mc.h"
#include "rfmeas_mdsp.h"
#include "rfcommon_core_device_manager.h"
#include "rfc_card.h"

// These are referenced in dtm_meas file and need to be cleaned-up.
uint8 cur_meas_script_entry_no;
uint8 cur_meas_script_entry_page = 0;
uint8 cur_cleanup_script_entry_no = 0;
uint8 cur_cleanup_script_entry_page = 0;

/*----------------------------------------------------------------------------*/
#if 0
extern void*
rfc_common_get_cmn_device_param
(
  rfdevice_type_enum_type dev_type, uint8 instance
);
#endif
/*----------------------------------------------------------------------------*/
// IRAT interface function ptrs for each tech.
static rfmeas_mc_func_tbl_type *rfmeas_mc_func_tbl[RFCOM_NUM_MODES] = { NULL };
static uint8 rfm_meas_dev_backup_token[RFCMN_CORE_MAX_PHY_DEVICES] = {0xFF,0xFF,0xFF};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function will register the IRAT API function ptrs from each tech.
 
  @param
  tech: Tech which is registering its IRAT APIs func_ptr table.
  func_tbl_ptr : tech's IRAT APIs table.
 
  @retval 
  Returns TRUE if the registration is successful, else FALSE.
*/

boolean rfmeas_mc_register(rfcom_mode_enum_type tech, rfmeas_mc_func_tbl_type *func_tbl_ptr)
{
  boolean status = FALSE;

  if (tech >= RFCOM_NUM_MODES)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech (%d) during rfmeas API registeration!", tech);
    return status;
  }

  if (func_tbl_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"func_tbl_ptr == NULL during rfmeas API registeration!", 0);
    return status;
  }

  rfmeas_mc_func_tbl[tech] = func_tbl_ptr;
  status = TRUE;

  return (status);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    rfm_meas_common_get_irat_info_param function will basically allow RFSW to pass to
    L1 how much the start-up and clean-up scripts take, and IRAT specific info if necessary
    for individual IRAT combination.
 
  @param rfm_meas_irat_info_type: data structure for all info to be returned
  @param rfm_cb_handler_type: callback 
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/

rfm_meas_result_type rfmeas_mc_irat_info_get(rfm_meas_irat_info_type *irat_info_param, 
                                             rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;

  if (irat_info_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Pointer irat_info_param is NULL!", 0);
    return result;
  }

  src_tech = irat_info_param->header.source_tech;
  target_tech = irat_info_param->header.target_tech;

  if (src_tech >= RFCOM_NUM_MODES || src_tech == RFM_PARKED_MODE || src_tech == RFM_ALL_MODES ||
      target_tech >= RFCOM_NUM_MODES || target_tech == RFM_PARKED_MODE || target_tech == RFM_ALL_MODES)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech parameters: src_tech=%d, target_tech=%d", src_tech, target_tech);
    return result;
  }

  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    if (rfmeas_mc_func_tbl[src_tech] == NULL || rfmeas_mc_func_tbl[target_tech] == NULL)
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfmeas_mc_func_tbl  is NULL! src(0x%x) target(0x%x)", 
            rfmeas_mc_func_tbl[src_tech], rfmeas_mc_func_tbl[target_tech]);
      return result;
    }
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfmeas_mc_irat_info_get(): source_tech= %d, target_tech= %d", 
          src_tech, target_tech);

  // tech's module to populate the required information(startup and cleanup timing info.) for IRAT operation.

  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    if (rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_irat_info_get_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_irat_info_get_fp(irat_info_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"src_tech=%d rfmeas_mc_irat_info_get_fp == NULL", src_tech);
    }

    if (rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_irat_info_get_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_irat_info_get_fp(irat_info_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_mc_irat_info_get_fp == NULL", target_tech);
    }
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called first when the measurement is requested. Each technology
    basically prepares for the measurement.
 
  @param rfm_meas_enter_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_init() must have already been called.
  We should be in a certain technology.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfmeas_mc_enter( rfm_meas_enter_param_type *meas_enter_param, 
                                      rfm_cb_handler_type cb_handler )
{
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;

  if (meas_enter_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_enter_param is NULL!", 0);
    return result;
  }

  src_tech = meas_enter_param->header.source_tech;
  target_tech = meas_enter_param->header.target_tech;

  if (src_tech >= RFCOM_NUM_MODES || src_tech == RFM_PARKED_MODE || src_tech == RFM_ALL_MODES ||
      target_tech >= RFCOM_NUM_MODES || target_tech == RFM_PARKED_MODE || target_tech == RFM_ALL_MODES)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech parameters: src_tech=%d, target_tech=%d", src_tech, target_tech);
    return result;
  }

  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    if (rfmeas_mc_func_tbl[src_tech] == NULL || rfmeas_mc_func_tbl[target_tech] == NULL)
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfmeas_mc_func_tbl  is NULL! src(0x%x) target(0x%x)", 
            rfmeas_mc_func_tbl[src_tech], rfmeas_mc_func_tbl[target_tech]);
      return result;
    }
  }

  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfmeas_mc_enter(): source_tech= %d, target_tech= %d source dev %d target dev %d", 
        src_tech, target_tech, meas_enter_param->header.source_param[0].device, meas_enter_param->header.target_param.device );

  // tech's module to populate the required information(startup and cleanup timing info.) for IRAT operation.
  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    if (rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_enter_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_enter_fp(meas_enter_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_mc_enter_fp == NULL", target_tech);
    }

    if (rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_enter_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_enter_fp(meas_enter_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"src_tech=%d rfmeas_mc_enter_fp == NULL", src_tech);
    }
  }

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to prepare the clean-up and start-up scripts.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_meas_common_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfmeas_build_scripts( rfm_meas_setup_param_type *meas_scripts_param, 
                                           rfm_cb_handler_type cb_handler )
{
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rf_buffer_intf *script_buf = NULL;
  rfm_meas_program_script_type script_type = RFM_MEAS_PROGRAM_STARTUP;
  rfcmn_core_dev_id_type dev_id = RFCMN_CORE_PHY_DEVICE_0;
  rfdevice_rxtx_common_class *temp_cmn_dev_ptr = NULL;

  if (meas_scripts_param == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"meas_scripts_param is NULL!", 0);
    return result;
  }

  src_tech = meas_scripts_param->header.source_tech;
  target_tech = meas_scripts_param->header.target_tech;

  if (src_tech >= RFCOM_NUM_MODES || src_tech == RFM_PARKED_MODE || src_tech == RFM_ALL_MODES ||
      target_tech >= RFCOM_NUM_MODES || target_tech == RFM_PARKED_MODE || target_tech == RFM_ALL_MODES)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech parameters: src_tech=%d, target_tech=%d", src_tech, target_tech);
    return result;
  }

  if (rfmeas_mc_func_tbl[src_tech] == NULL || rfmeas_mc_func_tbl[target_tech] == NULL)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfmeas_mc_func_tbl  is NULL! src(0x%x) target(0x%x)", 
          rfmeas_mc_func_tbl[src_tech], rfmeas_mc_func_tbl[target_tech]);
    return result;
  }
  
  /*Commnet the following f3 msg due to the requirement from FT team*/
  /*MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfmeas_build_scripts(): source_tech= %d, target_tech= %d, script_type=%d", 
        src_tech, target_tech, meas_scripts_param->script_type); */

  /* Before building any script, clear the buffer of previous scripts from previous gaps */
  script_type = RFM_MEAS_PROGRAM_STARTUP;
  script_buf = rfmeas_mdsp_buffer_get(script_type);
  rf_buffer_clear(script_buf);

  script_type = RFM_MEAS_PROGRAM_PRELOAD;
  script_buf = rfmeas_mdsp_buffer_get(script_type);
  rf_buffer_clear(script_buf);

  script_type = RFM_MEAS_PROGRAM_TRIGGER;
  script_buf = rfmeas_mdsp_buffer_get(script_type);
  rf_buffer_clear(script_buf);

  script_type = RFM_MEAS_PROGRAM_CLEANUP;
  script_buf = rfmeas_mdsp_buffer_get(script_type);
  rf_buffer_clear(script_buf);

  /* Start Building scripts */
  if ( ( target_tech == RFCOM_WCDMA_MODE ) && ( src_tech == RFCOM_WCDMA_MODE ) )
  {
    // Pre-configure any HW that can be programed at the time of build script
    if(rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_pre_config_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_pre_config_fp(meas_scripts_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_build_pre_config_scripts_fp == NULL", target_tech);
    }
    result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_build_scripts_fp(meas_scripts_param);
  }
  else
  {
    // tech's module to populate the required information(startup and cleanup scripts.) for IRAT operation.

    // Pre-configure any HW that can be programed at the time of build script
    if(rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_pre_config_fp != NULL)
    {
       result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_pre_config_fp(meas_scripts_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_build_pre_config_scripts_fp == NULL", target_tech);
    }
    
    // Reset any source tech settings to default for any non X2G cases
    if ( (target_tech == RFCOM_GSM_MODE) &&  (meas_scripts_param->script_type != RFM_MEAS_PROGRAM_STARTUP))
    {
      // Do nothing
    }
    else if ((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES))
    {
      if(rfmeas_mc_func_tbl[src_tech]->rfmeas_build_reset_scripts_fp != NULL)
      {
        result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_build_reset_scripts_fp(meas_scripts_param);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"source_tech=%d rfmeas_build_reset_scripts_fp == NULL", src_tech);
      }
    }

    // Call target tech API first to build the Setup scripts. 
    if (target_tech == RFCOM_GSM_MODE &&  meas_scripts_param->script_type != RFM_MEAS_PROGRAM_STARTUP)
    {
      // don't call the x2G cleanup build scripts API. because for X2G there will two separate API
      // calls from L1 to build startup and cleanup scripts.
    }
    else if ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES))
    {
      if (rfmeas_mc_func_tbl[target_tech]->rfmeas_build_scripts_fp != NULL)
      {
        /* may need to split the build scripts into two APIs:
         target_tech -> cleanup() and src_tech -> startup() */
        result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_build_scripts_fp(meas_scripts_param);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_build_scripts_fp == NULL", target_tech);
      }
    }

    script_type = RFM_MEAS_PROGRAM_TRIGGER;
    script_buf = rfmeas_mdsp_buffer_get(script_type);
    rf_buffer_clear(script_buf);

    // Reset any target tech settings to default for any non X2G cases
    if ( (target_tech == RFCOM_GSM_MODE) &&  (meas_scripts_param->script_type == RFM_MEAS_PROGRAM_STARTUP))
    {
      // Do nothing
    }
    else if ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES))
    {
      if (rfmeas_mc_func_tbl[target_tech]->rfmeas_build_reset_scripts_fp != NULL)
      {
        result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_build_reset_scripts_fp(meas_scripts_param);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_build_reset_scripts_fp == NULL", target_tech);
      }
    }

    // next call the source tech API to build the clean-up scripts.
    if (target_tech == RFCOM_GSM_MODE &&  meas_scripts_param->script_type != RFM_MEAS_PROGRAM_CLEANUP)
    {
      // don't call the x2G startup build scripts API. because for X2G there will two separate API
      // calls from L1 to build startup and cleanup scripts.
    }
    else if ((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES))
    {
      if (rfmeas_mc_func_tbl[src_tech]->rfmeas_build_scripts_fp != NULL)
      {
        /* may need to split the build scripts into two APIs:
         src_tech -> cleanup() and target_tech -> startup() */

        result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_build_scripts_fp(meas_scripts_param);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"src_tech=%d rfmeas_build_scripts_fp == NULL", src_tech);
      }
    }
  } 

  return(result);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
    This function is called to exit the measurement mode. It is used to put the RF
    in the right state and do any clean-ups required.
 
  @param rfm_meas_setup_param : This is a pointer that basically has a header that has
  the target technology and the source technology. Based on this header, the rest is casted as
  a tech specific pointer.
  @param cb_handler : This is the call back function.
 
  @pre
  rfm_meas_common_script_enter function must have been called.

  @retval
  Returns an enum type (rfm_meas_result_type) denoting whether the function did work
  correctly or not.
*/
rfm_meas_result_type rfmeas_mc_exit( rfm_meas_exit_param_type *meas_exit_param, 
                                     rfm_cb_handler_type cb_handler )
{
  rfm_meas_result_type result = RFM_MEAS_COMMON_SUCCESS;
  rfcom_mode_enum_type target_tech = RFM_INVALID_MODE;
  rfcom_mode_enum_type src_tech = RFM_INVALID_MODE;
  rfcmn_core_dev_id_type dev_id = RFCMN_CORE_PHY_DEVICE_0;
  rfdevice_rxtx_common_class *temp_cmn_dev_ptr = NULL;

  if (meas_exit_param == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "meas_exit_param is NULL");
    return RFM_MEAS_COMMON_FAILURE;
  }

  src_tech = meas_exit_param->header.source_tech;
  target_tech = meas_exit_param->header.target_tech;

  if (src_tech >= RFCOM_NUM_MODES || src_tech == RFM_PARKED_MODE || src_tech == RFM_ALL_MODES ||
      target_tech >= RFCOM_NUM_MODES || target_tech == RFM_PARKED_MODE || target_tech == RFM_ALL_MODES)
  {
    result = RFM_MEAS_COMMON_FAILURE;
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech parameters: src_tech=%d, target_tech=%d", src_tech, target_tech);
    return result;
  }

  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    if (rfmeas_mc_func_tbl[src_tech] == NULL || rfmeas_mc_func_tbl[target_tech] == NULL)
    {
      result = RFM_MEAS_COMMON_FAILURE;
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfmeas_mc_func_tbl  is NULL! src(0x%x) target(0x%x)", 
            rfmeas_mc_func_tbl[src_tech], rfmeas_mc_func_tbl[target_tech]);
      return result;
    }
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfmeas_mc_exit(): source_tech= %d, target_tech= %d", 
          src_tech, target_tech);

  if (((RFM_PARKED_MODE < src_tech) && (src_tech < RFCOM_NUM_MODES)) && 
      ((RFM_PARKED_MODE < target_tech) && (target_tech < RFCOM_NUM_MODES)))
  {  
    // tech's module to perform meas exit functionality.
    if (rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_exit_fp != NULL)
    {
      result &= rfmeas_mc_func_tbl[target_tech]->rfmeas_mc_exit_fp(meas_exit_param);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"target_tech=%d rfmeas_mc_exit_fp == NULL", target_tech);
    }

    // when source tech and target tech both are LTE/WCDMA dont perform the exit twice. 
    if ( !((src_tech == target_tech) && ((src_tech == RFCOM_LTE_MODE)||(src_tech == RFCOM_WCDMA_MODE))) )
    {
      if (rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_exit_fp != NULL)
      {
        result &= rfmeas_mc_func_tbl[src_tech]->rfmeas_mc_exit_fp(meas_exit_param);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"src_tech=%d rfmeas_mc_exit_fp == NULL", src_tech);
      }
    }
  }    

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "rfmeas_mc_exit status = %d, Gap abort = %d", 
        result,
        meas_exit_param->is_measurement_gap_aborted);

  return(result);
}

