/*!
  @file
  ftm_hdr_control.c

  @brief
  This module contains HDR common control code for FTM.

  @addtogroup RF_FTM_HDR_CONTROL
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/src/ftm_hdr_control.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
10/23/14   ck      Fix off targer error
10/22/14   ck      Temporarily unhooking ftm concurrency manager due to incomplete standalone test
10/22/14   ck      adapt the hooking to ftm conmgr due to interface change
09/15/14   pk      Compiler Warning Fix
07/29/14   pk      Reverting 3 carrier HDR RxAGC read support
07/23/14   pk      Compiler Error fix on DPM.2.0 1x compiled out build
07/15/14   pk      Compiler warning fix
07/08/14   pk      Implementation of HDR FTM module
11/25/13   hdz     Added fws_app_enable(FW_APPS_RFCMD)
09/23/13   sty     compiler warning fixes
06/20/13   JJ      Changed func ftm_hdr_deregister_fw_msgs()
06/19/13   JJ      Add functions for register/de-register fw msgs 
12/18/12   sty     KW fix for invalid device
12/04/12   aro     Added HDR support for RF-FW synchronization
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
06/26/12   spa     Fixed diversity bug in ftm_hdr_tear_down() 
06/07/12   spa     Fixed ftm_hdr_tear_down to use RFM HDR teardown API 
06/04/12   spa     Reverted ftm_hdr_tear_down to old method temporarily 
05/31/12   spa     Updated ftm_hdr_tear_down to use RFM HDR teardown API 
04/24/12   APU     Added profiling for ftm_hdr_configure_fw_state() and 
                   ftm_hdr_configure_mcpm_state(). 
04/18/12   aro     Moved FWS transition logic in upper layer
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/13/12   aro     Added support to enable new RF_TASK dispatching. Code
                   is protected with debug flag
11/18/11   aro     Added code not to do FW and MCPM voting during calibration
                   mode to allow power cycle less calibration between techs
11/17/11   aro     Enabled voting for CDMA FW State config and MCPM config
11/17/11   aro     Enabled FTM callback functions for RFM APIs
11/17/11   aro     Enabled voting for FW state config
11/17/11   aro     Added RFM callback function for FTM
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
11/11/11   aro     Added check not to handle FW State change response for
                   nonFTM mode
11/10/11   aro     Added RF Task handler to handle response message for HDR
                   FW state change
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/16/11   sar     Included internal rf hdr interface.
06/06/11   sty     Revert state to sleep instead of Invalid - since invalid
                   results in a deadlock condition
05/10/11   sar     Remove hdrmdsp.h and hdrsrch.h to cleanup rfa/api.
04/13/11   aro     Removed HDR Enable FW interface to replace with RFM
03/24/11   aro     Renamed FTM system control filename
03/01/11   aro     Moved  FTM HDR enter and exit to system control file
02/23/11   aro     Compiler Warning Fix
02/16/11   aro     Added placeholder for MCPM Call
02/16/11   aro     Compiler Warning removal
02/16/11   aro     [1] Added HDR Tear down interface
                   [2] Added 1x Tear down in HDR exit
02/16/11   aro     Added FTM HDR enter and exit functions
01/20/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#ifdef FEATURE_FACTORY_TESTMODE

#include "rfm_hdr.h"
#include "rfm_hdr_ftm.h"
#include "rfm_internal.h"
#include "rf_cdma_data.h"
#include "ftm_hdr_control.h"
#include "ftm_hdr_msm.h"
#include "ftm_cdma_data.h"
#include "ftm_common_control.h"
#include "ftm_msg.h"
#include "ftm.h"
#include "lm_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "fws.h"
#include "rfcommon_time_profile.h"
#include "rf_cdma_utils_freq.h"
#include "rf_cdma_utils.h"
#include "ftm_cdma_control.h"

/*============================================================================*/
/*!
  @name Firmware State Control

  @brief
  Functions for the FTM to control the radio
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote FW State Change
 
  @details
  This funciton will update the vote mask wit the vote for current device,
 
  @param device
  RF Device for which FW Disable vote is to be done.
*/
void
ftm_hdr_vote_fw_state
(
  rfm_device_enum_type device
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_vote_fw_state: NULL FW data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  if ( rfm_get_calibration_state() != TRUE )
  {
    fw_data->hdr_fw.fw_state_vote_mask |= (1 << device);
  }
  else
  {
    fw_data->hdr_fw.fw_state_vote_mask = 0;
  }

  FTM_MSG_2( FTM_HIGH, "ftm_hdr_vote_fw_state : New Vote 0x%x with Dev %d",
             fw_data->hdr_fw.fw_state_vote_mask, device );

  return;

} /* ftm_hdr_vote_fw_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unvote FW State Change
 
  @details
  This funciton will update the vote mask wit the vote for current device,
 
  @param device
  RF Device for which FW Disable vote is to be done.
*/
void
ftm_hdr_unvote_fw_state
(
  rfm_device_enum_type device
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;
  uint32 new_vote_mask = 0;

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_unvote_fw_state: NULL FW data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  if ( rfm_get_calibration_state() != TRUE )
  {
    new_vote_mask |= (1 << device );
    new_vote_mask = ~new_vote_mask;
  
    /* Update the Vote Mask */
    fw_data->hdr_fw.fw_state_vote_mask &= new_vote_mask;
  }
  else
  {
    fw_data->hdr_fw.fw_state_vote_mask = 0;
  }

  FTM_MSG_2( FTM_HIGH, "ftm_hdr_unvote_fw_state : New Vote 0x%x with Dev %d",
             fw_data->hdr_fw.fw_state_vote_mask, device );

  return;

} /* ftm_hdr_unvote_fw_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure HDR firmware state
 
  @details
  This function will configure the firmware state for FTM mode usage. However
  if the new FW state is RFM_CDMA_FW_STATE_DISABLE, then before proceeding
  with FW state change, current vote is checked.
 
  @param device
  RFM device on which HDR FW state is to be configured
 
  @param fw_state
  New firmware state to which FW transition is to be done.
 
  @return
  Flag indicating if the FW state transition was successful or not.
*/
boolean
ftm_hdr_configure_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
)
{
  ftm_cdma_fw_state_config_data_type *fw_data;
  boolean proceed_fw_state_chg = TRUE;
  boolean ret_val;
  rfm_cdma_fw_state_type current_fw_state; /* Current FW state */
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  /* Get FW Data */
  fw_data = ftm_cdma_get_fw_state_config_data();

  /* NULL pointer check */
  if ( fw_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_configure_fw_state: NULL FW data" );
    return FALSE;
  }

  /* To allow FW state transition to DISABLE, the fw_disbale_vote_mask should
  be 0. Otherwise FW disable cannot allowed. */
  if ( ( ( fw_state == RFM_CDMA_FW_STATE_DISABLE ) &&
         ( fw_data->hdr_fw.fw_state_vote_mask != 0 ) ) ||
       ( fw_state == fw_data->hdr_fw.fw_state ) )
  {
    proceed_fw_state_chg = FALSE;
  } /* if ( ( ( fw_state == RFM_CDMA_FW_STATE_DISABLE ) &&
         ( fw_data->hdr_fw.fw_state_vote_mask != 0 ) ) ||
       ( fw_state == fw_data->hdr_fw.fw_state ) ) */

  current_fw_state = fw_data->hdr_fw.fw_state;

  /* Perform FW state change if allowed */
  if ( proceed_fw_state_chg == TRUE )
  {
    ret_val = TRUE; /* Start with TRUE return value */

    /* Switch based on new requested FW Satte */
    switch( fw_state )
    {
    case RFM_CDMA_FW_STATE_ACTIVE:
    case RFM_CDMA_FW_STATE_MEAS:
      {
        /* Switch based on current FW State */
        switch ( current_fw_state )
        { /* switch ( current_fw_state ) */
        case RFM_CDMA_FW_STATE_DISABLE:
          fws_app_enable(FW_APP_RFCMD);
          fws_app_enable(FW_APP_HDR);
          break;
        default:
          break;
        } /* switch ( current_fw_state ) */
        ret_val = rfm_hdr_ftm_config_fw_state(device, fw_state);
      } /* switch( fw_state ) : case RFM_CDMA_FW_STATE_ACTIVE */
      break;
    
    case RFM_CDMA_FW_STATE_DISABLE:
      {
        ret_val = rfm_hdr_ftm_config_fw_state(device, fw_state);
        fws_app_disable(FW_APP_RFCMD);
        fws_app_disable(FW_APP_HDR);
      } /* switch( fw_state ) : case RFM_CDMA_FW_STATE_DISABLE */
      break;
    
    default:
      FTM_MSG( FTM_ERROR, "ftm_1x_configure_fw_state: Requested fw state "
                          "not supported" );
      break;
    } /* switch( fw_state ) */

    /* Update the FW state */
    fw_data->hdr_fw.fw_state = fw_state;
  } /* if ( proceed_fw_state_chg == TRUE ) */
  else
  {
    FTM_MSG_3( FTM_HIGH, "ftm_hdr_configure_fw_state: Vote 0x%x - Skipped FW "
               "state change [%d --> %d]", fw_data->hdr_fw.fw_state_vote_mask,
               current_fw_state, fw_state );
    ret_val = FALSE;
  } /* if ! ( proceed_fw_state_chg == TRUE ) */

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_4( FTM_HIGH, "ftm_hdr_configure_fw_state: New State [%d --> %d], "
             "Vote 0x%x, Execution Time: %d", current_fw_state, fw_state, 
             fw_data->hdr_fw.fw_state_vote_mask ,
             profiler_var_val ) ;

  return ret_val;

} /* ftm_hdr_configure_fw_state */

/*! \} */

/*============================================================================*/
/*!
  @name MCPM State Control

  @brief
  Functions for the FTM to control MCPM
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote MCPM
 
  @details
  This funciton will update the MCPM vote mask with the added vote for current 
  device
 
  @param device
  RF Device for which MCPM vote is to be done.
*/
void
ftm_hdr_vote_mcpm
(
  rfm_device_enum_type device
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_vote_mcpm: NULL MCPM data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  if ( rfm_get_calibration_state() != TRUE )
  {
    mcpm_data->hdr_mcpm.mcpm_vote_mask |= (1 << device);
  }
  else
  {
    mcpm_data->hdr_mcpm.mcpm_vote_mask = 0;
  }

  FTM_MSG_2( FTM_HIGH, "ftm_hdr_vote_mcpm : New Vote 0x%x with Dev %d",
             mcpm_data->hdr_mcpm.mcpm_vote_mask, device );

  return;

} /* ftm_hdr_vote_mcpm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unvote MCPM
 
  @details
  This funciton will update the MCPM vote mask with the removed vote for current 
  device
 
  @param device
  RF Device for which MCPM vote is to be done.
*/
void
ftm_hdr_unvote_mcpm
(
  rfm_device_enum_type device
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;
  uint32 new_vote_mask = 0;

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_unvote_mcpm: NULL MCPM data" );
    return;
  } /* if ( fw_data == NULL ) */

  /* Update the Vote mask only if the Calibration mode is disabled */
  if ( rfm_get_calibration_state() != TRUE )
  {
    new_vote_mask |= (1 << device );
    new_vote_mask = ~new_vote_mask;
  
    /* Update the Vote Mask */
    mcpm_data->hdr_mcpm.mcpm_vote_mask &= new_vote_mask;
  }
  else
  {
    mcpm_data->hdr_mcpm.mcpm_vote_mask = 0;
  }

  FTM_MSG_2( FTM_HIGH, "ftm_hdr_unvote_mcpm : New Vote 0x%x with Dev %d",
             mcpm_data->hdr_mcpm.mcpm_vote_mask, device );

  return;

} /* ftm_hdr_unvote_mcpm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure HDR MCPM state
 
  @details
  This function will configure the MCPM state for FTM mode usage. However
  if the new FW state is DISABLE, then before proceeding
  with MCPM state change, current vote is checked.
 
  @param enable_mcpm
  New MCPM state to which MCPM transition is to be done.
 
  @return
  Flag indicating if the MCPM state transition was successful or not.
*/
boolean
ftm_hdr_configure_mcpm_state
(
  boolean enable_mcpm
)
{
  ftm_cdma_mcpm_config_data_type *mcpm_data;
  boolean proceed_mcpm_state_chg = TRUE;
  boolean ret_val;
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  /* Get FW Data */
  mcpm_data = ftm_cdma_get_mcpm_state_ptr();

  /* NULL pointer check */
  if ( mcpm_data == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_configure_mcpm_state: NULL MCPM data" );
    return FALSE;
  }

  /* To allow MCPM state transition to DISABLE, the mcpm_vote_mask should
  be 0. Otherwise FW disable cannot allowed. */
  if ( ( enable_mcpm == FALSE ) &&
       ( mcpm_data->hdr_mcpm.mcpm_vote_mask != 0 ) )
  {
    proceed_mcpm_state_chg = FALSE;
  }

  /* Perform FW state change if allowed */
  if ( proceed_mcpm_state_chg == TRUE )
  {
    if ( enable_mcpm == TRUE )
    {
      if ( mcpm_data->hdr_mcpm.mcpm_configured == FALSE )
      {
        mcpm_data->hdr_mcpm.mcpm_configured = TRUE;
        ftm_hdr_enable_mcpm();
      }
      else
      {
        FTM_MSG( FTM_HIGH, "ftm_hdr_configure_mcpm_state: Already enabled" );
      }
    }
    else
    {
      if ( mcpm_data->hdr_mcpm.mcpm_configured == TRUE )
      {
        mcpm_data->hdr_mcpm.mcpm_configured = FALSE;
        ftm_hdr_disable_mcpm();
      }
      else
      {
        FTM_MSG( FTM_HIGH, "ftm_hdr_configure_mcpm_state: Already disabled" );
      }
    }

    ret_val = TRUE;
  } /* if ( proceed_fw_state_chg == TRUE ) */
  else
  {
    FTM_MSG_1( FTM_HIGH, "ftm_hdr_configure_mcpm_state: Vote 0x%x - Skipped "
               "MCPM state change", mcpm_data->hdr_mcpm.mcpm_vote_mask );
    ret_val = FALSE;
  }

   /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3( FTM_HIGH, "ftm_hdr_configure_mcpm_state: New State %d, "
             "Vote 0x%x, Execution Time: %d",
             mcpm_data->hdr_mcpm.mcpm_configured,
             mcpm_data->hdr_mcpm.mcpm_vote_mask,
             profiler_var_val ) ; 

  return ret_val;

} /* ftm_hdr_configure_mcpm_state */

/*! \} */

/*============================================================================*/
/*!
  @name RFM Callback functions

  @brief
  Functions pertaining to RFM callback
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function to be executed when RFM functions are called by FTM
 
  @details
  This function will be passed as a argument to the RFM APIs which will be
  executed at the end of RFM API execution.
 
  @param rfm_event
  RFM event for which this callback is called
 
  @param data
  Pointer to the echoed data received
*/
void
ftm_hdr_rfm_callback
(
  rfm_cb_event_enum_type rfm_event,
  void *cb_data
)
{  
  /* Handle cases based on type of RFM Event */
  switch( rfm_event )
  {
  case RFM_HDR_RX_WAKEUP_COMPLETE:
  case RFM_HDR_RX_SLEEP_COMPLETE:
  case RFM_HDR_ENABLE_COMPLETE:
  case RFM_HDR_DISABLE_COMPLETE:
  case RFM_HDR_RX_WAKEUP_PREP_COMPLETE:
  case RFM_HDR_TX_WAKEUP_COMPLETE:
  case RFM_HDR_TX_SLEEP_COMPLETE:
  case RFM_HDR_TUNE_COMPLETE:
  case RFM_HDR_ENABLE_DIV_COMPLETE:
  case RFM_HDR_DISABLE_DIV_COMPLETE:
  default:
    break;
  }

  FTM_MSG_1( FTM_MED, "ftm_hdr_rfm_callback : Event %d Handled", rfm_event );

} /* ftm_hdr_rfm_callback */

/*! \} */

/*============================================================================*/
/*!
  @name Common Radio Control

  @brief
  Function related common HDR Radio Control.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear Down RF

  @details
  This function will Tear Down RF to Sleep State, no mater what the current
  state is.
 
  @param device RF Device on which RF is to be torn down
 
  @return
  Flag for the function to indicate whether tear down was required or not. Based 
  on this parameter, rfm_enter_mode() will be called in calling function. If 
  TRUE, rfm_enter_mode() is not called. This will also be FALSE for the case,
  when RFM failure is seen during tear-down; thus indicating the calling
  function that rfm_enter_mode() has to be done to reset everything.
*/
boolean
ftm_hdr_tear_down
(
  rfm_device_enum_type device
)
{
  /* Flag indicating if tear down is needed */ 
  boolean was_tear_down_req = TRUE; 
  ftm_cdma_data_status_type *dev_status = NULL; /* FTM Device Status */
  ftm_cdma_data_status_type *assoc_dev_status = NULL; /*FTM assocev Status*/
  rfm_device_enum_type assoc_dev;

  /* Query the current RF State Ptr for master device */
  dev_status = ftm_cdma_data_get_device_ptr(device);

  /* NULL pointer check*/
  if( dev_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_hdr_tear_down: NULL data for device %d",device);

    was_tear_down_req = FALSE;
  }/* if(  dev_status == NULL ) */
  else
  {
    /* Tear down radio using RFM tear down API*/
    was_tear_down_req = rfm_hdr_ftm_tear_down_radio( device );
    
    /* Reset FTM variables based on RFM Tear Down action */
    if ( was_tear_down_req )
    {
      /* Store the associated device, if present */
      assoc_dev = dev_status->assoc_dev;

      /* Reset the Data structure for given Device */
      dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
      dev_status->assoc_dev = RFM_INVALID_DEVICE;

      /* Check if the associated device is valid */
      if ( assoc_dev < RFM_MAX_DEVICES )
      {
        /* Get associated device pointer*/
        assoc_dev_status = 
                 ftm_cdma_data_get_device_ptr( assoc_dev );  
              
      /* NULL pointer check*/
        if( assoc_dev_status == NULL )
      {
          FTM_MSG_1( FTM_ERROR,"ftm_hdr_tear_down: NULL data for Assoc "
                              "device %d", dev_status->assoc_dev );
      }/* if(  ftm_assoc_dev_status == NULL ) */
      else
      {

          /* Reset the state of associated device which is in DIV state.
          This is done because, when a teardown request for Slave device is
          received, there is no need to put the master device to sleep.
          Howeverm if there is a request for master device teardown, there
          we need need to put the slave device to sleep. */
          if ( assoc_dev_status->rf_state == FTM_CDMA_STATE_RXDIV )
          {
            assoc_dev_status->rf_state = FTM_CDMA_STATE_SLEEP;
            assoc_dev_status->assoc_dev = RFM_INVALID_DEVICE;
          } /* if ( assoc_dev_status->rf_state == RF_CDMA_STATE_RXDIV ) */
         
       }/* if!(  ftm_assoc_dev_status == NULL ) */
      } /* if ( assoc_dev != RFM_INVALID_DEVICE ) */
     }/* if ( was_tear_down_req )*/
     else
     {
       FTM_MSG_1(FTM_HIGH,"ftm_hdr_tear_down: Teardown not required on "
       "device:%d", device); 
     } /* if ! ( was_tear_down_req )*/ 
   } /* if ! (  dev_status == NULL ) */

  /* Return */
  return was_tear_down_req;
} /* ftm_hdr_tear_down */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register FTM related HDR FW MSG responses with FWRSP Task

  @details
  This function check if hdr fw msgs is qualified for registration, if it's qualified,
  it calls rfm layer to register hdr fw msgs. 
 
*/
void
ftm_hdr_register_fw_msgs
(
  void
)
{
  rfm_device_enum_type dev_idx;  /* Device for Looping */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  uint8 counter = 0;   /* Counter for hdr mode in all devices */

  /* Loop all device to check if none of them has rfmode in hdr, */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get rfmode of each device */
    ftm_rfmode = ftm_get_current_state(dev_idx);
    /* If it's in 1x mode, increase counter by 1*/
    if ( ftm_rfmode == FTM_STATE_HDR || ftm_rfmode == FTM_STATE_HDR_NS ) 
    {
       counter++;
       break;
    }
  }

  /* If counter is 0, register fw msgs */
  if ( counter == 0 ) 
  {
    /*Register hdr fw messages */
    rfm_hdr_ftm_register_fw_msgs();
    FTM_MSG( FTM_LOW, "ftm_hdr_register_fw_msgs: hdr FW msgs is registered "
               "successfully. " );
  }
  else
  {
    FTM_MSG( FTM_LOW, "ftm_hdr_register_fw_msgs: hdr FW msgs is already "
               "registered, no more hdr msgs registration is needed. " );
  }  

}/*ftm_hdr_register_fw_msgs*/    

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register FTM related HDR FW MSG responses with FWRSP Task

  @details
  This function check if hdr fw msgs is qualified for de-registration, if it's
  qualified, it calls rfm layer to de-register hdr fw msgs. 
 
*/
void
ftm_hdr_deregister_fw_msgs
(
  void
)
{
  rfm_device_enum_type dev_idx;  /* Device for Looping */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  uint8 counter = 0;   /* Counter for hdr mode in all devices */

  /* Loop all device to check if input device is the only device
  whose rfmode is hdr, */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get rfmode of each device */
    ftm_rfmode = ftm_get_current_state(dev_idx);
    /* If it's in hdr mode, increase counter by 1*/
    if ( ftm_rfmode == FTM_STATE_HDR || ftm_rfmode == FTM_STATE_HDR_NS ) 
    {
       counter++;
    }
  }

  /* If counter is 1, de-register fw msgs */
  if ( counter == 1 ) 
  {
    /*De-register hdr fw messages */
    rfm_hdr_ftm_deregister_fw_msgs();
    FTM_MSG( FTM_LOW, "ftm_hdr_deregister_fw_msgs: hdr FW msgs is de-registered "
               "successfully. " );
  }
  else
  {
    FTM_MSG( FTM_LOW, "ftm_hdr_deregister_fw_msgs: hdr FW msgs can't be "
      "de-registered, other device is still in hdr mode" );
  }  

}/*ftm_hdr_deregister_fw_msgs*/   

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set HDR Mode

  @details
  This function will set specific Mode of operation for RF SW. This will enter
  and enable HDR Mode of Operation. 
 
  To supports existing calibration procedure, this function will store the
  band class information sent in FTM Data structure. RF will be tuned to this
  band class when channel information is later passed using FTM_SET_CHAN
  command.
 
  Associated FTM Command : <b>FTM_HDR_SET_MODE</b>

  @param device RF device whose mode is to be set.
  @param mode FTM mode sent from the tools side

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_hdr_set_mode
(	
  rfm_device_enum_type device,
  ftm_mode_id_type mode
)
{

  sys_channel_type band_chan; /* Band Class to which Mode is mapped */
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  boolean rfm_ret = TRUE; /* Return val from RFM */
  boolean ftm_exit_status = FALSE; /* Status for FTM exit */
  ftm_cdma_rsp_type ret_val = FTM_CDMA_GOOD_RSP; /* Return Value */
  rf_time_type profiler_var_val; 

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ;   

  /* Must be in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_mode: Not in FTM Mode - %d", ftm_mode );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Parameter Validation */
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_mode: Invalid Device - %d", device );
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  (void)ftm_cdma_data_get_device_status( device, &dev_state );
  
    /* Resolve Band Class and Channel for given mode, since we dont have separate HDR API
    using the 1x API to resolve the band class*/
  band_chan.band = ftm_1x_resolve_bandclass( mode );                                             /* should I write new API for HDR */
  
  /* Populate Invalid Channel as Channel is unknown at this stage. Channel
  will be populated during FTM_SET_CHAN */
  band_chan.chan_num = FTM_CDMA_INVALID_CHAN;

  /* Check if the mode. For SLEEP mode, put the hdr radio on the given device                                                    
  to sleep. This will put the device in sleep mode based on the input mode command*/
  if ( mode == FTM_PHONE_MODE_SLEEP )
  {
    /* Perform hdr Exit */
    if ( ftm_get_current_state(device) == FTM_STATE_HDR )
    {
      ftm_exit_status = ftm_rfmode_exit( device, FTM_STATE_PARK );
    }

    if ( ftm_exit_status == FALSE )
    {
      ret_val = FTM_CDMA_BAD_RSP;
      FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_mode: FTM exit on dev %d failed",
                 device );
    } /* if ( ftm_exit_status = FALSE ) */
    else
    {
      FTM_MSG_1( FTM_LOW, "ftm_hdr_set_mode: Sleep for hdr Dev %d", device );
    }

  } /* if ( mode == FTM_PHONE_MODE_SLEEP ) */
  else
  {
    /* Update FTM Data structure with band and chan */
    ftm_cdma_data_update_band_chan(device, 
                     rf_cdma_sys_band_type_to_rf_band_type(band_chan.band),
                                  (rfm_cdma_chan_type )band_chan.chan_num);

    rfm_ret &= (ftm_hdr_enter_mode ( device, RFM_1XEVDO_MODE, NULL, NULL) == 0);

    rfm_ret &= ftm_cdma_control_enable_tech( RFM_1XEVDO_MODE,
                                           device, 
                                           band_chan,
                                           dev_state.rxlm_handle, 
                                           NULL, NULL );

    /* Since RFM failed, flag return value as BAD RESPONSE */
    if ( rfm_ret == FALSE )
    {
      ret_val = FTM_CDMA_BAD_RSP;
      /* should we put it in sleep state? or invalid state*/
      ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );                      
    } /* if ( rfm_ret == FALSE ) */
    else
    {
      ret_val = FTM_CDMA_GOOD_RSP;
        
      /* Update FTM Data Structure */
      ftm_cdma_data_update_rf_state( device, FTM_CDMA_STATE_SLEEP );
    } /* if ! ( rfm_ret == FALSE ) */
  } /* if ! ( mode == FTM_PHONE_MODE_SLEEP ) */

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3( FTM_LOW, "ftm_hdr_set_mode: Done for hdr Dev %d, Mode %d, "
             "Execution TIme: %d", device, mode , profiler_var_val ) ;

  return ret_val;

} /* ftm_hdr_set_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tune to Band and Channel through wakeup_rx

  @details
  This function tunes the receiver to a particular band and channel using the wakeup_rx APIs. 
 
  Associated FTM Command : <b>FTM_SET_CHAN</b>

  @param device RF device that needs to tune
  @param num_band_chan number of carriers
  @param band_chan_array array that contains the list of bands and channels

  @return
  Status of execution completion of the function execution
*/
ftm_cdma_rsp_type
ftm_hdr_set_channel
(
    const rfm_device_enum_type device,
    const uint8 num_band_chan,
    const uint16 *band_chan_array
)
{

  /* Define variable to Extract Useful Data */  
  sys_channel_type band_chan_list[RFM_CDMA_MAX_CARRIER];
  int32 carrier_index[RFM_CDMA_MAX_CARRIER] = {0,1,2};
  rfm_hdr_agc_acq_type agc_info;
  lm_handle_type rxlm_handle;
  ftm_cdma_data_status_type dev_status;

  /* Response data to the ftm_hdr_dispatch */
  ftm_cdma_rsp_type ftm_rsp_data;			

  /* Define : Response Variable for ftm_hdr_wakeup_rx */
  int32 rf_status;

  /* Get RF Device Status of the requested device */
  ftm_cdma_data_get_device_status( device, &dev_status );

  rxlm_handle = dev_status.rxlm_handle;

  /* Extract Useful Data */
  band_chan_list[0].band = (sys_band_class_e_type)dev_status.curr_band;
  band_chan_list[0].chan_num = (sys_channel_num_type)band_chan_array[0];
  band_chan_list[1].band = (sys_band_class_e_type)dev_status.curr_band;
  band_chan_list[1].chan_num = (sys_channel_num_type)band_chan_array[1];
  band_chan_list[2].band = (sys_band_class_e_type)dev_status.curr_band;
  band_chan_list[2].chan_num = (sys_channel_num_type)band_chan_array[2];

  /* FTM tool does not have the exposure to the agc algo, so hard-cording it with norma AGC mode */
  agc_info.mode = RFM_HDR_AGC_MODE_FAST;
  agc_info.params.fast_acq.agc_rtc_offset = 0;

  rf_status = ftm_hdr_wakeup_rx(device, rxlm_handle, num_band_chan, band_chan_list,
                                carrier_index, &agc_info, NULL, NULL);
    
	
  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data = FTM_CDMA_BAD_RSP;
  }
  else
  {
    ftm_rsp_data = FTM_CDMA_GOOD_RSP;
  }
	
  FTM_MSG_2( FTM_MED, "ftm_hdr_set_channel : Dev %d, RxLM %d : [Done]", 
	         device, rxlm_handle );
	
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_set_channel */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the transmitter. This API mimics the calls that L1 would make
  to enable the transmitter

  @details
  This function is used to configure and enable the transmitter.
 
  Mimic the L1 sequence here 
  -# enable relevant clocks needed for stable TX operation
  -# configure and enable modulator for 1x
  -# configure and enable Tx 
 
  Associated FTM Command :
  -# <b>FTM_SET_TX_ON</b>
 
  @param device
  RF Device on which Tx is to be turned on or off
  
  @return int32
  Status of execution completion of the function execution
*/

ftm_cdma_rsp_type
ftm_hdr_set_tx_on
(
  const rfm_device_enum_type device
)
{
  /* Response data to the ftm_hdr_dispatch */
  ftm_cdma_rsp_type ftm_rsp_data;

  /* Define : Response Variable for ftm_hdr_wakeup_rx */
  int32 rf_status;
  
  rf_status = ftm_hdr_wakeup_tx (device, NULL, NULL);

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data = FTM_CDMA_BAD_RSP;
	FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_channel : Failed!! statis: %d", 
	         rf_status);
  }
  else
  {
    ftm_rsp_data = FTM_CDMA_GOOD_RSP;
  }
	
  FTM_MSG_1( FTM_LOW, "ftm_hdr_set_channel : Dev %d, [Done]", 
	         device);
	
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_set_tx_on */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the transmitter. This API mimics the calls that L1 would make
  to disable the transmitter

  @details
  This function is used to configure and disable the transmitter.
 
  Associated FTM Command :
  -# <b>FTM_SET_TX_OFF</b>
 
  @param device
  RF Device on which Tx is to be turned on or off
  
  @return int32
  Status of execution completion of the function execution
*/


ftm_cdma_rsp_type
ftm_hdr_set_tx_off
(
  const rfm_device_enum_type device
)
{
  /* Response data to the ftm_hdr_dispatch */
  ftm_cdma_rsp_type ftm_rsp_data;

  /* Define : Response Variable for ftm_hdr_wakeup_rx */
  int32 rf_status;
  
  rf_status = ftm_hdr_sleep_tx (device, NULL, NULL);

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_rsp_data = FTM_CDMA_BAD_RSP;
  }
  else
  {
    ftm_rsp_data = FTM_CDMA_GOOD_RSP;
  }
	
  FTM_MSG_1( FTM_MED, "ftm_hdr_set_tx_off : Dev %d, [Done]", 
	     device);
	
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_hdr_set_tx_off */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the secondary chain to the given mode (enable or disable diversity)

  @details
  Sets the secondary chain to the given mode (enable or disable diversity)
 
  Associated FTM Command : <b>FTM_SET_SECONDARY_CHAIN</b>

  @param device
  RF slave device on which the given channel is to be tuned
 
  @param sec_chain_mode
  Indicates if secondary chain is to tuned in diversity  or be 
  disabled

  @return
  Status of execution completion of the function execution
*/

ftm_cdma_rsp_type
ftm_hdr_set_second_chain_mode
(
  rfm_device_enum_type device,
  ftm_sec_chain_ctl_enum_type sec_chain_mode
)
{

  ftm_cdma_rsp_type status = FTM_CDMA_GOOD_RSP; /* Return Value */
  rfm_wait_time_t ret_val;
  rf_time_tick_type profiler_var;
  rf_time_type profiler_var_val;

  rfm_device_enum_type m_device; /* master device of "device" in arg list*/
      
  /* find out the master for the diversity device */
  m_device = ftm_cdma_resolve_master_device(device);

  /* check if driver is in FTM_MODE */
  if(ftm_mode != FTM_MODE)
  {
    FTM_MSG( FTM_ERROR, "ftm_hdr_set_second_chain_mode: Not in FTM Mode");
    return FTM_CDMA_BAD_RSP; /* Failure */
  }

  /* Get starting time tick to profile the function */
  profiler_var = rf_time_get_tick(); 

  /* Based on the input command, the slave device will be put in RxDiv or Disable RxDiv state */
  switch( sec_chain_mode )
  {
    case DISABLE_SECONDARY_CHAIN:
    {
      ret_val = ftm_hdr_disable_diversity(device, NULL, NULL);
    }
    break;
	
    case RECEIVE_DIVERSITY:
    {
      ret_val = ftm_hdr_enable_diversity(m_device, device, NULL, NULL);
    }
    break;

    default:
      status = FTM_CDMA_BAD_RSP;
	  ret_val = RFM_CDMA_ERROR_FAILURE;
      FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_second_chain_mode: Unsupported value = %d", 
                 sec_chain_mode);
    break;
  }

  if (ret_val < 0)
  {
    status = FTM_CDMA_BAD_RSP;
  }

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_3 ( FTM_LOW, "ftm_hdr_set_second_chain_mode: Done for Device %d, Operation %d, Execution Time: %d", 
              device, sec_chain_mode, profiler_var_val ) ;

  /* Return the State */
  return status;

} /* ftm_hdr_set_second_chain_mode */



/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM HDR API for rx wakeup

  @details
  This function wakes up the rx using through rfm wakeup rx apis and update the 
  ftm state machine to the corresponding state based on the result. 

  @param device RF device that needs to tune
  @param rxlm_handle  rxlm handle for the device passed
  @param num_band_chan number of carriers
  @param band_chan_array array that contains the list of bands and channels
  @param carrier_index index of the carriers passes in the band chan array
  @param agc_mode agc mode that the rune process should start with
  @param callback call back function that will be called after the execution
  @param userdata user data for the call back function passed  

  @return
  Status of execution completion of the function execution
*/
int32
ftm_hdr_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type *band_chan_array,
  const int32 *carrier_index,
  const rfm_hdr_agc_acq_type* agc_mode,
  const rfm_cb_handler_type callback,
  void* const userdata   
)
{

  /* Define : Response Variable*/
  int32 rf_status;

  /* Parameter Validation */
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_wakeup_rx: Invalid Device - %d", device );
    return RFM_CDMA_ERROR_FAILURE; /* Failure */
  }

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "ftm_hdr_wakeup_rx : START - Dev %d, Band %d, "
	                 "Chan %d", device );


  rf_status = rfm_hdr_prep_wakeup_rx( device, rxlm_handle, num_band_chan, 
				      band_chan_array, carrier_index, agc_mode, 
                                      callback, userdata );

  rf_status &= rfm_hdr_exec_wakeup_rx( device, rxlm_handle, 
				       callback, userdata);

  /* Send back a BAD response packet for failures */
  if ( rf_status < 0 )
  {
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_SLEEP );
  }
  else 
  {
    /* Update FTM Data Structure with current band and chan */
    ftm_cdma_data_update_band_chan( device, 
	rf_cdma_sys_band_type_to_rf_band_type(band_chan_array[0].band),
		    (rfm_cdma_chan_type )band_chan_array[0].chan_num );	  
    ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RX );
  }

  return rf_status;

} /* ftm_hdr_wakeup_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the radio and tune to the given band and channel on HDR Mode.

  @details
  This function calls RFM API to turn on the radio and tune to given
  band and channel in HDR Mode.

  @param device RF device whose mode is to be set.
  @param mode FTM mode sent from the tools side
  @param user_data_ptr user data ptr that has the required data for the callback handler.
  @param cb_handler callback function that will be called at the end of execution  

*/
int32
ftm_hdr_enter_mode
(
  const rfcom_device_enum_type device,
  const rfcom_mode_enum_type rf_mode,
  void * const user_data_ptr,
  rfm_cb_handler_type cb_handler
)
{

  /* Define : Response Variable*/
  int32 rf_status = RFM_CDMA_HEALTHY_STATE;

  ftm_cdma_data_status_type dev_state; /* Var to hold current state */        

  /* ensure that FTM env for hdr has been set up */
  if (!ftm_rfmode_enter( device, FTM_STATE_HDR ) )                                                    /* Can I direclty check the status inside the if condigion check */
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_set_mode: Failed FTM init for %d", device );
    return RFM_CDMA_ERROR_FAILURE; /* FTM environment is not set for HDR */
  }

  /* fetch the device status for the requested device */
  (void)ftm_cdma_data_get_device_status( device, &dev_state );
  
  /* Tear Down RF to enter HDR mode */	
  if ( !ftm_hdr_tear_down(device) )
  {
    rf_status = rfm_enter_mode( device, rf_mode, user_data_ptr, cb_handler,
   						   dev_state.rxlm_handle );
    if (rf_status != 0)
    {
      rf_status = RFM_CDMA_ERROR_FAILURE;
    }
  } /* if ( !ftm_hdr_tear_down(device) ) */
  else 
  {
    /* rfm_enter_mode return type is wait time that is zero in pass case so assigne negative value for fail case */
    rf_status = RFM_CDMA_ERROR_FAILURE;
  } 

  return rf_status;

} /* ftm_hdr_enter_mode */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn on the transmitter. This API mimics the calls that L1 would make
  to enable the transmitter

  @details
  This function is used to configure and enable the transmitter.
 
  Mimic the L1 sequence here 
  -# enable relevant clocks needed for stable TX operation
  -# configure and enable modulator for 1x
  -# configure and enable Tx 
 
  Associated FTM Command :
  -# <b>FTM_SET_TX_ON</b>
 
  @param device
  RF Device on which Tx is to be turned on or off
  @param callback
  Call back funciton to be called once tranmitter is turned on
  @param userdata
  Data required for the callback function
  
  @return int32
  Status of execution completion of the function execution
*/


int32
ftm_hdr_wakeup_tx
(
  const rfm_device_enum_type device,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{

  /* Define : Response Variable*/
  int32 rf_status;

  ftm_cdma_data_status_type dev_status;
  
  uint8 carrier_idx; /* Variable for carrier indexing */
  uint8 mask; /* Contains carrier mask for mdsp config */
  /* Represents which carriers to enable/disable */
  rfm_cdma_carrier_en_mask_type carrier_mask;

    /* Contains frequency offsets */
  int32 freq_offset_hz[RFM_CDMA_CARRIER_NUM];

  /* Parameter Validation */
  if ( device >= RFM_MAX_DEVICES )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_wakeup_rx: Invalid Device - %d", device );
    return RFM_CDMA_ERROR_FAILURE; /* Failure */
  }

  /* Get RF Device Status */
  if (ftm_cdma_data_get_device_status( device, &dev_status ))
  {   
    /* 2> Call RFM Function to Wakeup HDR TX */
    rf_status = rfm_hdr_wakeup_tx( device, dev_status.txlm_handle,
                                   callback, userdata );

    /* continue only if above call was successful */
    if ( rf_status >= 0 )
    {
      /* Device Status Read Pointer */
      const rf_cdma_data_status_type *dev_status_r; 

      dev_status_r = rf_cdma_get_device_status( device );

      /* NULL pointer check*/
      if(  dev_status_r == NULL )
      {
        FTM_MSG_1( FTM_ERROR,"ftm_hdr_wakeup_tx: NULL data for device %d",
                   device );
        rf_status  = RFM_CDMA_ERROR_FAILURE;
      }/* if(  dev_status_r == NULL ) */
      else
      {    
        rf_cdma_compute_carrier_info( 
                                      dev_status_r->curr_band,
                                      dev_status_r->curr_chans,
                                      dev_status_r->num_carriers, 
                                      dev_status_r->carrier_index, 
                                      &carrier_mask,
                                      freq_offset_hz, 
                                      0 /* no need to add anything to calculated
                                           freq offset */
                                    );
      
        /* 3> force hdr into traffic mode */
        rfm_hdr_set_pa_access_probe_config( device, TRUE );
      
        /* 4> need to enable modulator */
        mask = 0;
        for( carrier_idx = 0; carrier_idx < dev_status_r->num_carriers; 
             ++carrier_idx ) 
        {
          mask |= (carrier_mask.enable[0] << carrier_idx);
        }
        if( mask == 0 )
        {
          rf_status  = RFM_CDMA_ERROR_FAILURE;
          FTM_MSG( FTM_ERROR, 
                   "ftm_hdr_wakeup_tx: Failed ..carrier mask is zero");
        }
      
        (void) ftm_hdr_enable_modulator( mask, FALSE /* skip_dac_toggle */ );

        if (rf_status >= 0)
        {
          /* Update FTM Data Structure with current Tx state */
          ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RXTX );
        }
        else
        {
          FTM_MSG( FTM_ERROR, "ftm_hdr_wakeup_tx: Failed");
        }
      }
    }/*if ( rf_status >= 0 )*/
  }/*if (ftm_cdma_data_get_device_status( device, &dev_status ))*/
 
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_wakeup_tx: Device: object is NULL", device);
    rf_status = RFM_CDMA_ERROR_FAILURE;
  }

return rf_status;

} /* ftm_hdr_wakeup_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn OFF the radio and put baseband to sleep for HDR Mode
  This API mimics the calls that L1 would make to disable the transmitter

  @details
  This function calls RFM API which in turn puts the RTR and MSM to sleep for 
  the given device

  @param device on which sleep tx should be called and the callback function to be called
*/

int32
ftm_hdr_sleep_tx
(
  const rfm_device_enum_type device,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{

  lm_handle_type txlm_handle;
  ftm_cdma_data_status_type dev_status;

    /* Define : Response Variable*/
  int32 rf_status = 0;

  /* Get RF Device Status */
  if (ftm_cdma_data_get_device_status( device, &dev_status ))
  {
    if ( dev_status.rf_state == FTM_CDMA_STATE_RXTX )
    {
      txlm_handle = dev_status.txlm_handle;
        
      /* Disable HDR modulator */
      (void) ftm_hdr_disable_modulator();                                               

      rf_status = rfm_hdr_sleep_tx( device, txlm_handle, callback,
                                    userdata );

      FTM_MSG_2( FTM_LOW, "ftm_hdr_sleep_tx : Dev %d, TxLM %d : [Done]", 
                 device, txlm_handle );

        if (rf_status >= 0)
        {
          /* Update FTM Data Structure with current Tx state */
          ftm_cdma_data_update_rf_state ( device, FTM_CDMA_STATE_RX );
        }
        else
        {
          FTM_MSG( FTM_ERROR, "ftm_hdr_sleep_tx: Failed");
        }

    } /* if ( dev_status.rf_state == FTM_CDMA_STATE_RXTX ) */
    else
    {
      rf_status = RFM_CDMA_ERROR_FAILURE;
      FTM_MSG_3( FTM_ERROR, "ftm_hdr_sleep_tx: Device %d in RF State %d "
                 "Expected State (RxTX) %d", device,
                 dev_status.rf_state, FTM_CDMA_STATE_RXTX );      
    }

  } /* if (ftm_cdma_data_get_device_status( device, &dev_status )) */
  else
  {
    rf_status = RFM_CDMA_ERROR_FAILURE;
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_sleep_tx: Could not get handle to "
                          "device %d", device );
  }
  
  /* Return Response Packet */
  return rf_status;

} /* ftm_hdr_sleep_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable diversity for HDR 
  This API mimics the calls that L1 would make to enable the diversity
  
  @details
  This function calls the RFM API responsible for enabling diversity. 

  @param device on which diversity should be disabled and the callback fucntion to be called
*/

int32
ftm_hdr_enable_diversity
(
  const rfm_device_enum_type master_device,
  const rfm_device_enum_type slave_device,
  const rfm_cb_handler_type callback,
  void* const userdata

)
{
  /* device status for primary and diversity */
  ftm_cdma_data_status_type dev_status_m, dev_status_s;
  int32 rf_status;
  int32 hdr_enter_mode_rf_status;

  hdr_enter_mode_rf_status = ftm_hdr_enter_mode(slave_device, RFM_1XEVDO_MODE, NULL, NULL);

  if (hdr_enter_mode_rf_status == RFM_CDMA_HEALTHY_STATE)
  {
    /* Get RF Device Status */
    ftm_cdma_data_get_device_status( slave_device, &dev_status_s );
    
    /* Get RF Device Status for master device */
    ftm_cdma_data_get_device_status( master_device, &dev_status_m );
    
    rf_status = rfm_hdr_enable_diversity( master_device, dev_status_m.rxlm_handle, 
                                          slave_device, dev_status_s.rxlm_handle, 
                                          callback, userdata);   
  
    /* update the ftm state machine to sleep state incase enable diversity is failed */
    if ( rf_status < 0 )
    {
      ftm_cdma_data_update_rf_state ( slave_device, FTM_CDMA_STATE_SLEEP );
    }
    else 
    {
    
      /* Update FTM Data Structure with current band and chan */
      ftm_cdma_data_update_band_chan( slave_device,
                                      dev_status_m.curr_band,
                                      dev_status_m.curr_chan );      
      ftm_cdma_data_update_rf_state ( slave_device, FTM_CDMA_STATE_RXDIV );
    
      /* associate the two devices */
      ftm_cdma_data_update_assoc_device( slave_device, master_device);
      ftm_cdma_data_update_assoc_device( master_device, slave_device);
    }
  } /*if (hdr_enter_mode_rf_status == RFM_CDMA_HEALTHY_STATE)*/
  else
  {
    /* enter-mode failed - so report error */
    rf_status = RFM_CDMA_ERROR_FAILURE;
    FTM_MSG_2( FTM_ERROR, "ftm_hdr_enable_diversity : Failed enter mode on" 
               "device %d : Status =%d", slave_device, hdr_enter_mode_rf_status);
  }
  return rf_status;
} /* ftm_hdr_enable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable diversity for HDR 
  This API mimics the calls that L1 would make to enable the diversity
  
  @details
  This function calls the RFM API responsible for disabling diversity. 

  @param device on which diversity should be disabled and the callback fucntion to be called
*/

int32
ftm_hdr_disable_diversity
(
  const rfm_device_enum_type device_s,
  const rfm_cb_handler_type callback,
  void* const userdata

)
{

  /* Define : Response Variable*/
  int32 rf_status;

  lm_handle_type rxlm_handle_s;
  ftm_cdma_data_status_type dev_status_s;

  if ( device_s >= RFM_MAX_DEVICES  )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_hdr_disable_diversity: Unsupported device %d", 
               device_s );
    rf_status = RFM_CDMA_ERROR_FAILURE;
    return rf_status;
  }

  /* Get RF Device Status */
  ftm_cdma_data_get_device_status( device_s, &dev_status_s );
  rxlm_handle_s = dev_status_s.rxlm_handle;

  rf_status = rfm_hdr_disable_diversity( device_s, rxlm_handle_s, 
                                         callback, userdata );


  FTM_MSG_2( FTM_LOW, "ftm_hdr_disable_diversity: "
             "DevS %d, RxLMs %d : [Done]", device_s, rxlm_handle_s );
  
  /* Return Response Packet */
  return rf_status;

} /* ftm_hdr_disable_diversity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to get Rx Power Level

  @details
  Returns the Rx level, for a specified receiver, in dBm units.

  @param device
  RF Device on which Rx Level will be measured
 
  @param rx_level_agc
  Stores the raw AGC data for the measured Rx_Level
 
  @param rx_level_dbm
  Stores the dbm10 equivalent for the measured Rx_Level
 
  @return
  RxAGC in dBm10 units on success and False on failure
*/

ftm_cdma_rsp_type
ftm_hdr_get_rx_level_dbm
(
  rfm_device_enum_type device,
  int16 *rx_level_agc,
  int16 *rx_level_dbm
)
{
  /* Define : Response Variable*/
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  ftm_cdma_rsp_type api_res = FTM_CDMA_GOOD_RSP; /* final result of API */
  
  /* Call RFM Function to query RxAGC Data */
  rfm_hdr_ftm_get_rx_agc_data( device, &rx_agc_data, 
                                    RFM_CDMA_AGC_FORMAT__DBM10);

 /* safe to assume RFM_CDMA_CARRIER_0 since this is 1x */
 rx_level_dbm[RFM_CDMA_CARRIER_0] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];
 rx_level_agc[RFM_CDMA_CARRIER_0] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];
 /* This has been commented out because of the tools support. 
 Tools side implementation has not been done. It will be included 
 once the tools side changes are ready */
    
// rx_level_dbm[RFM_CDMA_CARRIER_1] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_1];
// rx_level_dbm[RFM_CDMA_CARRIER_2] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_2];
// rx_level_dbm[RFM_CDMA_CARRIER_CUMULATIVE] = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_CUMULATIVE];
 
    
 FTM_MSG_2( FTM_MED, "ftm_hdr_get_rx_level_dbm: Dev : %d, RxAGC0 : %d",
            device, rx_level_dbm[RFM_CDMA_CARRIER_0]);

 
  /* Return Response Packet */
  return api_res ;

} /* ftm_hdr_get_rx_level_dbm */

/*! \} */

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FEATURE_CDMA1X */
/*! @} */
