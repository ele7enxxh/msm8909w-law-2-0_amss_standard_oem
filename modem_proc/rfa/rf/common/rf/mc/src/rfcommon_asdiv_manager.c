/*!
  @file
  rfcommon_asdiv_manager.c

  @brief
  This file contains all declarations and definitions necessary to use the RF
  Antenna Switch Diversity 
*/

/*==============================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfcommon_asdiv_manager.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/06/15   dyc     Reverted pre-wakeup ASDIV and ASDIV FTM Log
04/04/15   dw      Disabled tune code imm update timer in rfcommon_asdiv_manager_complete_switch()
01/13/15   sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
12/01/14   as/dyc  Exposed ASDiv manager lock and release APIs
11/18/14   pk      Abort the pending ASDIV event
09/07/14   ak      ASDIV : Adding Mutex lock to prevent concurrent access.
07/29/14   zhh     create rfcommon_asdiv_get_previous_position API
03/18/14   kg      Merge changes from Dime
03/18/14   as      Removed check for switch in progress in switch config
02/21/14   hdz     Added AsDiv support
10/25/13   sg      Removing redundant F3 message
10/23/13   sg      Added API to check if ASDiv is supported in the card being used
10/09/13   nsh     Enable HWID check MSG for ASDIV
09/04/13   pl      Added new interface to support init of default state and 
                   querying of switch in progress
09/04/13   pl      remove large local variable to avoid stack overflow
08/23/13   sml     New API to ack Antenna switch complete and updating
                   asdiv_manager_state only
08/23/13   sml     Adding condition check as whether ASD is supported on a RF 
                   card or not
08/20/13   pl      Fix GSM crash due to Asdiv Tuner manager
08/19/13   pl      Fix warning
08/14/13   pl      Integrate asdiv tuner manger to generate tuner switch script
08/01/13   sml     Correcting Compilation error
07/26/13   aro     Fixed early resetting of L1 callback param
07/25/13   aro     Removed early return for the case when aTuner is not
                   present
07/25/13   aro     Send script only if it is not script_gen_only
07/19/13   aro     Featurized ASDIV
07/17/13   aro     Moved the L1 callback definition to rfm.c
07/15/13   aro     typedef for antenna position
07/15/13   aro     Interface to query current antenna position
07/13/13   aro     Doxygen compatibility added
07/13/13   aro/pl  Consolidated all asdiv functionality to the module
06/06/13   pl      Initial version

============================================================================*/

#include "comdef.h"
#include "err.h"
#include "rfcommon_msg.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_asdiv_manager.h"
#include "rfcommon_locks.h"
#include "modem_mem.h"
#include "rfc_card.h"
#include "rfm_device_config_type.h"
#include "rfdevice_msg_log.h"
#include "rf_hal_buffer.h"

#if defined(FEATURE_RF_ASDIV) || defined(FEATURE_RF_HAS_TPTUNER_SWITCH)

/*----------------------------------------------------------------------------*/
/*! Structure to hold ASDiv Manager state Information */
typedef struct
{
  rf_lock_data_type api_lock;
  /*!< Lock Token to be used to lock ASDiv code */

  boolean init_done;
  /*!< Flag indicating the completion of first time initialization */

  rfcommon_asdiv_position_type current_sw_pos;
  /*!< This variable holds the current switch position */

  rfcommon_asdiv_position_type prev_sw_pos;
  /*!< This variable holds the previos switch positon */

  boolean switch_in_process;
  /*!< Flag indicating the switch is in progress. This variable will only be 
  set when the switch completion message is received from firmware */

  rfcommon_mdsp_event_handle* tuner_ccs_event_handle; 
  /*!< Pointer to the CCS event handle for CCS event */

  void *l1_switch_complete_cb_data;
  /*!< Data passed by L1 to be used with callback function */

  rfm_l1_cb_type l1_switch_complete_cb_func;
  /*!< Callback function passed by L1 to be executed by RF at the 
  end of switch completion */

  rfcommon_asdiv_update_tuner_type update_tuner_cb;
  /*!< Callback function and data passed by tech to execute update tune code
  by same tech after switch completion*/

} rfcommon_asdiv_manager_state_type;

/*----------------------------------------------------------------------------*/
/*! Variable maintaining the ASDiv manager state */
static rfcommon_asdiv_manager_state_type asdiv_manager_state;

/* Callback pointer to the immediate update tune code timer */
static timer_type rfcommon_asdiv_update_tune_code_clk_cb;

/*  Asdiv immediate update tune code timer call back interwal */
static const uint32 ASDIV_UPDATE_TUNE_CODE_START_INT = 5; /* milliseconds */



extern boolean
rfcommon_asdiv_tuner_get_switch_script
(
   rf_device_execution_type dev_action,
   rf_buffer_intf *script_buffer,
   int16 script_timing,
   rfcommon_asdiv_position_type position
);

extern boolean 
rfcommon_asdiv_tuner_acknowledge_switch_script
( 
   void *asdiv_port_obj
);

extern void
rfcommon_asdiv_tuner_update_tuner_code
( 
  rfcommon_asdiv_update_tuner_type update_tuner_cb
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Complete Antenna switch and update asdiv_manager_state only
 
  @details
  This function updates the asdiv_manager_state when antenna switch is 
  complete. The completion of this function indicates that the last 
  switch request has been completed
 
  @return
  Flag indicating the status of execution
*/
boolean
rfcommon_asdiv_manager_state_ack( void )
{
  boolean ret_val = TRUE;

  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  /* Handle only if switch is in process */
  if( asdiv_manager_state.switch_in_process == TRUE )
  {
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [1] Update state variables to indicate switch completion */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    asdiv_manager_state.switch_in_process = FALSE;
    asdiv_manager_state.prev_sw_pos = asdiv_manager_state.current_sw_pos;
  } /* if( asdiv_manager_state.switch_in_process == TRUE ) */
  else
  {
    RF_MSG_1( RF_ERROR, "[ASDiv Manager] Complete Switch [ERROR : SwitchPos %d - Switch not in Process]",
              asdiv_manager_state.current_sw_pos);

    ret_val = FALSE;
  } /* if( asdiv_manager_state.switch_in_process == TRUE ) */
  /* Releae ASDIV manager interface*/
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));

  RFDEVICE_LOG_MSG_2(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] State_ack [Done : SwitchPos %d - Status %d]",
            asdiv_manager_state.current_sw_pos, ret_val );

  /* Return */
  return ret_val;
} /* rfcommon_asdiv_manager_state_ack */
  
/*! @} */

/*============================================================================*/
/*!
  @name Switch Helper

  @brief
  This section contains internal helper functions to be called to perform 
  antenna switch.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Complete Antenna switch
 
  @details
  This function will be registered with the response handler task to be
  executed when a response from FW is received indicating that FW has
  executed the script for antenna switch. The completion of this function
  indicates that the last switch request has been completed
 
  @param cmd_ptr
  Pointer to the structure containing command received by the response
  handler
 
  @param snum_info
  Pointer to the data structure containing second level SNUM dispatching info
 
  @return
  Flag indicating the status of execution
*/
static boolean 
rfcommon_asdiv_manager_complete_switch
( 
  void *cmd_ptr,
  const rf_dispatch_snum_info_type *snum_info
)
{
  boolean ret_val = TRUE;

  rfcommon_asdiv_rsp_cb_data_type *asdiv_cb_data = NULL;

  /* RExtract the ASDIV callback data */
  asdiv_cb_data = (rfcommon_asdiv_rsp_cb_data_type*)(snum_info->snum_cb_data);

  /* NULL Pointer check */
  if ( asdiv_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "[ASDiv Manager] Complete Switch [ERROR : NULL CB Data]");
    return FALSE;
  } /* if ( asdiv_cb_data == NULL ) */

 /*Check the status of ASD Event*/ 
 if(!rfcommon_mdsp_asdiv_check_aborted_event(asdiv_cb_data->update_buffer_ptr))
 {
  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  RFDEVICE_LOG_MSG_3(LOG_RFA_ASDIV_C, MSG_LEGACY_MED,
                     "[ASDiv Manager][FW->RF] ASD_SWITCH_RSP 0x%x [ %d | %d ]", 
            snum_info->snum, 
            RFCOMMON_MDSP_SNUM_TECH(snum_info->snum),
            RFCOMMON_MDSP_SNUM_COUNT(snum_info->snum) );

  /* Handle only if switch is in process */
  if( asdiv_manager_state.switch_in_process == TRUE )
  {
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [1] Acknowledge all tokens from switching to AsDiv Atuner */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if 0
    ret_val = rfcommon_asdiv_tuner_acknowledge_switch_script(asdiv_cb_data->asdiv_tuner_obj_ptr);
    if (ret_val == FALSE) 
    {
      RF_MSG_3( RF_ERROR, "[ASDiv Manager] Complete Switch [ERROR : Dev %d "
                "Mode %d SwitchPos %d - Tuner script acknowledge failed]",
                asdiv_cb_data->device,
                asdiv_cb_data->rfm_mode,
                asdiv_cb_data->ant_position );
    }
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [2] Update state variables to indicate switch completion */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    asdiv_manager_state.switch_in_process = FALSE;
    asdiv_manager_state.prev_sw_pos = asdiv_manager_state.current_sw_pos;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [3] Update tune code after switch completion
           In concurrency scenarios (e.g. SVLTE/SGLTE/DSDA) tuners are not
           guaranteed to be in good state after switch completetion. 
           Update tune code is required on primary antenna for mode which completed
           asdiv switching. Update tune code will be started after immediate update
           timer expires, currently set to 5 ms.*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Register timer only for valide callback function*/
#if 0
    if(asdiv_manager_state.update_tuner_cb.cb_func != NULL)
    {
      RFDEVICE_LOG_MSG(LOG_RFA_ASDIV_C, MSG_LEGACY_MED,
                 "[ASDiv Manager]rfcommon_asdiv_manager_complete_switch" 
                   "immediate update tune code timer started]");     
    
      timer_reg( &rfcommon_asdiv_update_tune_code_clk_cb,
               (timer_t2_cb_type)rfcommon_asdiv_update_tune_code_timer_trigger,
               0,
               ASDIV_UPDATE_TUNE_CODE_START_INT,
               0 );
    }
#endif
  } /* if( asdiv_manager_state.switch_in_process == TRUE ) */
  else
  {
    RF_MSG_3( RF_ERROR, "[ASDiv Manager] Complete Switch [ERROR : Dev %d "
              "Mode %d SwitchPos %d - Switch not in Process]",
              asdiv_cb_data->device,
              asdiv_cb_data->rfm_mode,
              asdiv_cb_data->ant_position );

    ret_val = FALSE;
  } /* if( asdiv_manager_state.switch_in_process == TRUE ) */
  /* Releae ASDIV manager interface*/
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));

  /* Execute the callback function */
  if ( asdiv_manager_state.l1_switch_complete_cb_func != NULL )
  {
    asdiv_manager_state.l1_switch_complete_cb_func(
                    asdiv_manager_state.l1_switch_complete_cb_data );
    asdiv_manager_state.l1_switch_complete_cb_func = NULL;
    asdiv_manager_state.l1_switch_complete_cb_data = NULL;
  } /* if ( asdiv_manager_state.l1_switch_complete_cb_func != NULL ) */

 MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"[ASDiv Manager] Complete Switch [Done : Dev %d "
            "Mode %d SwitchPos %d - Status %d]",
            asdiv_cb_data->device,
            asdiv_cb_data->rfm_mode,
            asdiv_cb_data->ant_position,
            ret_val );

  }/*if the event is aborted, call the abort API*/
  else
  {
    rfcommon_asdiv_manager_abort_switch(asdiv_cb_data->device, asdiv_cb_data->rfm_mode,NULL);
  }  
  /* Clean up CCS event handle */
  rfcommon_mdsp_ccs_cleanup_event( &(asdiv_cb_data->asdiv_ccs_event_handle) );
  if ( asdiv_cb_data != NULL )
  {
    modem_mem_free( asdiv_cb_data, MODEM_MEM_CLIENT_RFA );
    asdiv_cb_data = NULL;
  } /* if ( asdiv_cb_data != NULL ) */  

  /* Return */
  return ret_val;
} /* rfcommon_asdiv_manager_complete_switch */
  
/*! @} */

/*============================================================================*/
/*!
  @name Init Interface

  @brief
  This section contains interfaces to be called to perform initialization and
  deinitialization.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  AsDIV Manager initialization

  @details
  AsDIV Manager initialization, which initializes API mutex, callback structure
  token structure, etc.
 
  @return
  Flag indicating the Status of initialization
*/
boolean 
rfcommon_asdiv_manager_init
(
  void
)
{
  boolean init_status = TRUE;

  /* Perform init only if it was never done before */
  if (asdiv_manager_state.init_done == FALSE)
  {
    asdiv_manager_state.current_sw_pos = 0;
    asdiv_manager_state.prev_sw_pos = 0;
    asdiv_manager_state.switch_in_process = FALSE;
    asdiv_manager_state.l1_switch_complete_cb_data = NULL;
    asdiv_manager_state.l1_switch_complete_cb_func = NULL;
    /* initialize update tune code timer call back */
/*    rfcommon_asdiv_update_tune_code_timer_init(); // Removing 5 ms timer */
    init_status &= 
      rf_common_init_critical_section(&(asdiv_manager_state.api_lock));
    if (init_status == TRUE) 
    {
      asdiv_manager_state.init_done = TRUE;
    } /* if (init_status == TRUE)  */
  } /* if (asdiv_manager_state.init_done == FALSE) */
  else
  {
    init_status = TRUE;
  } /* if ! (asdiv_manager_state.init_done == FALSE) */

  RFDEVICE_LOG_MSG_1(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] Init [Status %d]", init_status);

  /* Return */
  return init_status;
} /* rfcommon_asdiv_manager_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  AsDIV Manager de-initialization

  @details
  AsDIV Manager de-initialization, which clears API mutex, callback structure
  token structure, etc.
 
  @return
*/
boolean 
rfcommon_asdiv_manager_deinit
(
  void
)
{
  boolean deinit_status = TRUE;

  /* De-init only if it was initialized */
  if ( asdiv_manager_state.init_done == TRUE )
  {
    deinit_status = 
      rf_common_deinit_critical_section(&(asdiv_manager_state.api_lock));
    asdiv_manager_state.current_sw_pos = 0;
    asdiv_manager_state.prev_sw_pos = 0;
    asdiv_manager_state.switch_in_process = FALSE;
    asdiv_manager_state.l1_switch_complete_cb_data = NULL;
    asdiv_manager_state.l1_switch_complete_cb_func = NULL;
    asdiv_manager_state.init_done = FALSE;
  }

  RFDEVICE_LOG_MSG_1(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] DeInit [Status %d]", deinit_status);

  return deinit_status;
} /* rfcommon_asdiv_manager_deinit */

/*! @} */

/*============================================================================*/
/*!
  @name Switch Interface

  @brief
  This section contains interfaces to be called to perform antenna switch.
*/
/*! @{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initiate Antenna Switch Process

  @details
  This function is called by individual tech to initiate antenna switch process.
  It can either generate script and push it to SMEM, or perform
  a direct write to hardware. This function will execute the tech specific 
  functions passed by tech MC to generate script
 
  @param device
  RFM device on which the antenna switch is to be initiated
 
  @param rfm_mode
  RFM mode active on the current device
 
  @param antenna_position
  New antenna position
 
  @param switch_script_ptr
  Pointer to the buffer allocated by Tech to store the script for antenna
  switch which is GRFC based control

  @param tuner_script_ptr
   Pointer to the buffer allocated by Tech to store the script for antenna
   tuner scripts
 
  @param asm_script_cfg
  Structure containing the configuration data to generate ASM script
 
  @param send_script_cfg
  Structure containing the configuration data to send script
 
  @param send_script_cfg
  Structure containing the configuration data to send script
 
  @param switch_start_cfg
  Structure containing the configuraion data to notify switch start
 
  @param update_tuner_cb
  Structure containing callback funciton pointer to be called at the end of 
  switch completion to update tune code and callback data 
  
  @param l1_cb_func
  L1 callback funciton pointer to be called at the end of switch completion
 
  @param l1_cb_data
  L1 callback data to be passed to L1 callback function
 
  @param gen_script_only
  Flag indicating that the function should generate script only. This will
  not update the ADIV state
  
  @retval TRUE
  Indicates that the script is generated successfully
 
  @retval FALSE
  Indicates that the script generation error occurred
*/
boolean 
rfcommon_asdiv_manager_config_switch
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *switch_script_ptr,
  rf_buffer_intf *tuner_script_ptr,
  rfcommon_asdiv_asm_conifg_type asm_script_cfg,
  rfcommon_asdiv_send_script_type send_script_cfg,
  rfcommon_asdiv_switch_start_notify_type switch_start_cfg,
  rfcommon_asdiv_update_tuner_type update_tuner_cb,
  rfm_l1_cb_type l1_cb_func,
  void *l1_cb_data,
  boolean gen_script_only,
  boolean immediate,
  int16 tuner_start_delta,
  uint32 rxlm_handle
)
{
  boolean ret_val = TRUE; /* Return value */
  rfm_devices_configuration_type *dev_cfg = NULL;
  
  dev_cfg = (rfm_devices_configuration_type *)modem_mem_alloc(sizeof(rfm_devices_configuration_type), MODEM_MEM_CLIENT_RFA);  
  if (dev_cfg == NULL) 
  {
    RF_MSG( RF_ERROR, "Cannot allocate memory for device config");
    return FALSE;
  }
  memset(dev_cfg, 0, sizeof(rfm_devices_configuration_type));
  if(rfc_get_devices_configuration(dev_cfg))
  {
    if (dev_cfg->antenna_swap_supported == FALSE)
    {
       /*Uncomment the below F3 once hack for GSM ASD is removed*/
       RF_MSG_3( RF_HIGH, "ASD is not supported on this card: %d", dev_cfg->antenna_swap_supported, 0, 0 );
       modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
       return TRUE;
    }
    modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
  }
  else
  {
    RF_MSG_3( RF_ERROR, "Not able to retrieve device configuration", 0, 0, 0 );
    modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }

  /* Parameter Validation for Script pointer object */
  if ((tuner_script_ptr == NULL) || (switch_script_ptr == NULL))
  {
    RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
              "Mode %d SwitchPos %d - Null Script Buffer]",
              device,
              rfm_mode,
              antenna_position );
    return FALSE;
  } /* if (script_ptr == NULL) */

  /* Parameter Validation for RFM Mode */
  if ( (rfm_mode >= RFM_NUM_MODES) || (rfm_mode <= RFM_PARKED_MODE) )
  {
    RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
              "Mode %d SwitchPos %d - Bad Mode]",
              device,
              rfm_mode,
              antenna_position );
    return FALSE;
  }/* if ( (rfm_mode >= RFM_NUM_MODES) || (rfm_mode <= RFM_PARKED_MODE) ) */

  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* [1] Call get switch script */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(immediate == TRUE)
    { 
      ret_val = rfcommon_asdiv_tuner_get_switch_script( RFDEVICE_EXECUTE_IMMEDIATE,
                                                      tuner_script_ptr,
                                                      0,
                                                      antenna_position);
    }
    else
    {
    ret_val = rfcommon_asdiv_tuner_get_switch_script( RFDEVICE_CREATE_SCRIPT,
                                                      tuner_script_ptr,
                                                      tuner_start_delta,
                                                      antenna_position);
    }
    if (ret_val == FALSE) 
    {
      RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
                "Mode %d SwitchPos %d - Tuner script generation failed]",
                device,
                rfm_mode,
                antenna_position );
    }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* [2] Generate ASM script */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  if ( asm_script_cfg.cb_func != NULL ) 
  {
    /* Execute the callback function */
    if ( asm_script_cfg.cb_func( antenna_position,
                                 switch_script_ptr,
                                 asm_script_cfg.cb_data ) == FALSE )
    {
      RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
                "Mode %d SwitchPos %d - ASM script generation failed]",
                device,
                rfm_mode,
                antenna_position );
      ret_val = FALSE;
    } /* if ( asm_script_cfg.cb_func( antenna_position,
                                      script_ptr,
                                      asm_script_cfg.cb_data ) == FALSE ) */
  } /* if ( asm_script_cfg.cb_func != NULL )  */

  /* If failed to generate ASM/GRFC script. Skip all the rest */
  if( ret_val == TRUE )
  {
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* [3] Update State variables. (If only need the script, do not update AsDIV 
    switch in progress. Otherwise, only script generate was needed) */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Save the current position as the previous position */
    asdiv_manager_state.prev_sw_pos = asdiv_manager_state.current_sw_pos;

    /* Store the new position as current position */
    asdiv_manager_state.current_sw_pos = antenna_position;

    if ( gen_script_only == FALSE )
    {
      if (immediate == FALSE)
      	{
         /* Indicate that the switch is in process*/
           asdiv_manager_state.switch_in_process = TRUE;
         }
        

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [4] Push the script to SMEM */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( send_script_cfg.cb_func != NULL ) 
    {
      /* Store L1 callbakc function and data  */
      asdiv_manager_state.l1_switch_complete_cb_func = l1_cb_func;
      asdiv_manager_state.l1_switch_complete_cb_data = l1_cb_data;
    
        /* Store callback function for update tune code  */
        asdiv_manager_state.update_tuner_cb = update_tuner_cb;
            
        /* Push to SMEM only if valid script is present and we give GRFC script more */
        /* priority then tuner scripts */
        if ( switch_script_ptr != NULL )
      {
        /* Execute the callback to send script */
        if (  send_script_cfg.cb_func( device,
                                       rfm_mode,
                                       antenna_position,
                                       switch_script_ptr,
                                       rxlm_handle ) == FALSE )
        {
          RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
                    "Mode %d SwitchPos %d - Script Push failed]",
                    device,
                    rfm_mode,
                    antenna_position );
          ret_val = FALSE;
        } /* if (  send_script_cfg.cb_func( device,
                                       rfm_mode,
                                       script_ptr,
                                       send_script_cfg.cb_data ) == FALSE) */
      } /* if ( script_ptr != NULL ) */
      else
      {
          RFDEVICE_LOG_MSG_3(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                             "[ASDiv Manager] Config Switch [Dev %d "
                  "Mode %d SwitchPos %d - Skipping Script Push]",
                  device,
                  rfm_mode,
                  antenna_position );
        } /* if ( script_ptr != NULL ) */
    } /* if ( send_script_cfg.cb_func != NULL )  */
  } /* if (gen_script_only == FALSE) */

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* [5] Notify that the switching has intiated */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  if ( switch_start_cfg.cb_func != NULL ) 
  {
    if ( switch_start_cfg.cb_func( switch_start_cfg.cb_data ) == FALSE )
    {
      RF_MSG_3( RF_ERROR, "[ASDiv Manager] Config Switch [ERROR : Dev %d "
                "Mode %d SwitchPos %d - Notification failed]",
                device,
                rfm_mode,
                antenna_position );
      ret_val = FALSE;
    } /* if ( switch_start_cfg.cb_func( switch_start_cfg.cb_data ) == FALSE ) */
  } /* if ( switch_start_cfg.cb_func != NULL )  */

  }/* if( ret_val == TRUE ) */

  /* Releae ASDIV manager interface*/
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));

  RFDEVICE_LOG_MSG_4(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] Config Switch [Done : Dev %d "
            "Mode %d SwitchPos %d - Status %d]",
            device,
            rfm_mode,
            antenna_position,
            ret_val );
 
  /* Return */
  return ret_val;

} /* rfcommon_asdiv_manager_config_switch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Abort the antenna switch process
 
  @details
  This function will be called by tech MC when a request by l1 is made to
  abort the switch after the script has been created.
 
  @param device
  RFM device on which switch abort is to be initiated
 
  @param rfm_mode
  RFM mode for which switch abort is to be initiated
 
  @param asdiv_obj
  Pointer to ASDIV tuner object
 
  @return
  Flag indicating the status of switch abort
*/
boolean 
rfcommon_asdiv_manager_abort_switch
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  void *asdiv_obj
)
{
  boolean ret_val = TRUE; /* Return value */
  rf_buffer_intf *rf_script_dummy_buffer = NULL;  

 /* Parameter Validation for RFM mode  */
 if ( (rfm_mode >= RFM_NUM_MODES) || (rfm_mode <= RFM_PARKED_MODE) )
  {
    RF_MSG_2( RF_ERROR, "[ASDiv Manager] Abort Switch [ERROR : Dev %d "
			 "Invalid Mode %d ]",
              device,
              rfm_mode );
    return FALSE;
 } 

  /* Acquire lock */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  /* switch_in_process needs to be TRUE for the function to continue */
  if( asdiv_manager_state.switch_in_process == TRUE )
  {

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [1] Update AsDIV tuners to go back to previous position */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    rf_script_dummy_buffer = rf_buffer_create ( 0, 100, 0);

    if ( NULL != rf_script_dummy_buffer)
    {
      ret_val = rfcommon_asdiv_tuner_get_switch_script( RFDEVICE_CREATE_SCRIPT,
                                                        rf_script_dummy_buffer, 
                                                        0,                                                         
                                                        asdiv_manager_state.prev_sw_pos);
      if (ret_val == FALSE) 
      {
        RF_MSG_3( RF_ERROR, "[ASDiv Manager] Abort Switch [ERROR : Dev %d "
                 "Mode %d SwitchPos %d - Tuner script generation failed]",
                 device,
                 rfm_mode,
                 asdiv_manager_state.prev_sw_pos );
      }
      else
      {
        RFDEVICE_LOG_MSG(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH, 
                         "[ASDiv Manager] Abort Switch: Tuner states reverted successfully]");
      }

      (void)rf_buffer_clear(rf_script_dummy_buffer);
      rf_buffer_destroy(rf_script_dummy_buffer);
      rf_script_dummy_buffer = NULL;	  
    }
	else
    {
      RF_MSG( RF_ERROR, "Tuner dummy Script Buffer Allocation failed! ");
    }
	
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [2] go back to previous position */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    asdiv_manager_state.current_sw_pos = asdiv_manager_state.prev_sw_pos;
    asdiv_manager_state.switch_in_process = FALSE;

	
    RFDEVICE_LOG_MSG_3(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH, "[ASDiv Manager] Abort Switch - Position reverted: Current Pos = %d," 
					   "Previous Pos = %d, Switch in process = %d",
					   asdiv_manager_state.current_sw_pos,
					   asdiv_manager_state.prev_sw_pos,
					   asdiv_manager_state.switch_in_process );

     /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* [3] Reset Data */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    asdiv_manager_state.l1_switch_complete_cb_func = NULL;
    asdiv_manager_state.l1_switch_complete_cb_data = NULL;
    
  } /* if( asdiv_manager_state.switch_in_process == TRUE ) */
  else
  {
    RF_MSG_2( RF_ERROR, "[ASDiv Manager] Abort Switch [ERROR : Dev %d "
              "Mode %d - Switch not in Process]",
              device,
              rfm_mode );
    ret_val = FALSE;
  } /* if! ( asdiv_manager_state.switch_in_process == TRUE ) */

  /* Release Lock */
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));

  RFDEVICE_LOG_MSG_3(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] Abort Switch [Done : Dev %d "
            "Mode %d - Status %d]",
            device,
            rfm_mode,
            ret_val );
 
  /* return */
  return ret_val;

} /* rfcommon_asdiv_manager_abort_switch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send Script to shared memory
 
  @details
  This function is a helper function for tech MC to push the antenna switch
  script to shared memory for FW to execute. This function passed as an
  argument to the config_switch function.
 
  @param device
  RFM device for which script is to be pushed
 
  @param rfm_mode
  RFM mode for which the script is to be pushed
 
  @param asdiv_script
  Buffer containing the script for antenna switch
 
  @return
  Flag indicating the status of execution of the function
*/
boolean 
rfcommon_asdiv_manager_send_script
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *asdiv_script,
  uint32 rxlm_handle
)
{
  boolean ret_val = TRUE;

  rfcommon_asdiv_rsp_cb_data_type *asdiv_cb_data = NULL; 

  /* Allocate memory for ASDIV response callback data */
  asdiv_cb_data = modem_mem_alloc( MODEM_MEM_CLIENT_RFA, 
                                   sizeof(rfcommon_asdiv_rsp_cb_data_type) );

  /* NULL pointer check */ 
  if (asdiv_cb_data == NULL)
  {
    RF_MSG_2( RF_ERROR, "[ASDiv Manager] Send Script [ERROR : Dev %d "
              "Mode %d - Malloc failed]",
              device,
              rfm_mode );
    return FALSE;
  } /* if (asdiv_cb_data == NULL) */

  /* Update the callback data structure */
  asdiv_cb_data->device = device;
  asdiv_cb_data->rfm_mode = rfm_mode;
  asdiv_cb_data->ant_position = antenna_position;
  asdiv_cb_data->asdiv_ccs_event_handle = NULL;
  ret_val &= rfcommon_mdsp_asdiv_queue_script( device, 
                                     rfm_mode,
                                     asdiv_script,
                                     &(asdiv_cb_data->asdiv_ccs_event_handle),
                                     rfcommon_asdiv_manager_complete_switch,
                                     asdiv_cb_data,
                                     rxlm_handle);

  RFDEVICE_LOG_MSG_3(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] Send Script [Done : Dev %d "
            "Mode %d - Status %d]",
            device,
            rfm_mode,
            ret_val );

  return ret_val;

} /* rfcommon_asdiv_manager_send_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  ASDiv cheanup function for CCS event
 
  @details
  This function will be passed to RF-MDSP layer as a callback function to
  be executed in case mdsp_queue_atuner_update needs to be executed before 
  FW response happens, hence not executing the registered fwrsp call back 
  function. This function free up resources that should have been cleared by the 
  registered fwrsp call back function and also reset all the state variables.
  
  @param registered_cb_data
  callback data that got passed in along with the registered call back. It was 
  meant to be handled by the registered call back but since registered call back 
  never got executed, we handle the call back data here including cleaning up
 
  @return
  Status of cleanup action
*/
boolean
rfcommon_asdiv_manager_cleanup
( 
  void *registered_cb_data
)
{
  rfcommon_asdiv_rsp_cb_data_type *asdiv_cb_data = NULL;

  asdiv_cb_data = (rfcommon_asdiv_rsp_cb_data_type*)(registered_cb_data);

  /* NULL Pointer check */
  if ( asdiv_cb_data == NULL )
  {
    RF_MSG( RF_ERROR, "[ASDiv Manager] Cleanup [ERROR : Null Callback Data" );
    return FALSE;
  } /* if ( fws_cfg_cb != NULL ) */

  /* Abort CCS event and clean up CCS event handle */
  rfcommon_mdsp_ccs_abort_event( &(asdiv_cb_data->asdiv_ccs_event_handle) );
  rfcommon_mdsp_ccs_cleanup_event( &(asdiv_cb_data->asdiv_ccs_event_handle) );

  if ( asdiv_cb_data != NULL )
  {
    modem_mem_free( asdiv_cb_data, MODEM_MEM_CLIENT_RFA );
    asdiv_cb_data = NULL;
  } /* if ( asdiv_cb_data != NULL ) */

  RFDEVICE_LOG_MSG(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH, "[ASDiv Manager] Cleanup [Done]" );

  return TRUE;
}/* rfcommon_asdiv_manager_send_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if switch is in progress
 
  @details
  This function will return TRUE if switch is in progress,
  FALSE is there is no switch in progress
 
  @param None

 
  @return
  TRUE : switch in progress
  FALSE: switch not in progress
*/
boolean
rfcommon_asdiv_is_switch_in_progress
(
  void
)
{
  return asdiv_manager_state.switch_in_process;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the switch default position
 
  @details
  Initialize the switch default position. 
 
  @param antenna_position
  The default position to set to
 
  @return
  None
*/
void
rfcommon_asdiv_init_switch_state
(
   rfcommon_asdiv_position_type antenna_position
)
{
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  if(antenna_position < ASDIV_POSITION_MAX)
  {
    asdiv_manager_state.current_sw_pos = antenna_position;
    asdiv_manager_state.prev_sw_pos = antenna_position;

    RFDEVICE_LOG_MSG_1(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH,
                     "[ASDiv Manager] Init switch state [Done : Position %d]",antenna_position);
  }
  else
  {
    RFDEVICE_LOG_MSG_1(LOG_RFA_ASDIV_C, MSG_LEGACY_HIGH, 
                     "[ASDiv Manager] Init switch state [Invalid : Position %d]",antenna_position);
  }

  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));
}
/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  This section contains functions to query if ASDIV is supported 
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query ASDIV support status
 
  @details
  This function will return if ASDiv is supported on a given card
 
  @return
  boolean value
*/
boolean rfcommon_asdiv_is_supported(void)
{
  static boolean checked_once = FALSE;
  static boolean rfc_is_asdiv = FALSE;

  rfm_devices_configuration_type *dev_cfg = NULL;	

  // Only check once
  if (checked_once)
  {
    return rfc_is_asdiv;
  }  
 	
  dev_cfg = (rfm_devices_configuration_type *)modem_mem_alloc(sizeof(rfm_devices_configuration_type), MODEM_MEM_CLIENT_RFA);

  if (dev_cfg == NULL)	
  {
    RF_MSG( RF_ERROR, "Cannot allocate memory for device config");
    return FALSE;
  }

  memset(dev_cfg, 0, sizeof(rfm_devices_configuration_type));
  
  if(rfc_get_devices_configuration(dev_cfg))
  {
    if (dev_cfg->antenna_swap_supported == FALSE)
    {
      modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
      dev_cfg = NULL;
      checked_once = TRUE;
      rfc_is_asdiv = FALSE;
      return rfc_is_asdiv;
    }
    modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
    dev_cfg = NULL;
  }
  else
  {
    RF_MSG( RF_ERROR, "Not able to retrieve device configuration" );
    modem_mem_free(dev_cfg, MODEM_MEM_CLIENT_RFA);
    dev_cfg = NULL;
    checked_once = TRUE;
    rfc_is_asdiv = FALSE;
    return rfc_is_asdiv;
  }

  checked_once = TRUE;
  rfc_is_asdiv = TRUE;
  
  return rfc_is_asdiv;
}

/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  This section contains functions used by tech MC to query status
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query current antenna position
 
  @details
  This function will return the current antenna position
 
  @param device
  RFM deivce for which the antenna position is queried
 
  @return
  Current antenna position
*/
rfcommon_asdiv_position_type
rfcommon_asdiv_get_current_position
(
  rfm_device_enum_type device
)
{
  rfcommon_asdiv_position_type current_pos; 
  
  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  current_pos = asdiv_manager_state.current_sw_pos;

  /* Release ASDIV manager interface*/
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));  

  return current_pos; 
} /* rfcommon_asdiv_get_current_position */


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Query current antenna position
 
  @details
  This function will return the current antenna position
 
  @param device
  RFM deivce for which the antenna position is queried
 
  @return
  Current antenna position
*/
void
rfcommon_asdiv_set_current_position
(
  rfcommon_asdiv_position_type pos
)
{
  asdiv_manager_state.current_sw_pos = pos;
} 

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the update tune code timer callback.
 
  @details

*/
void rfcommon_asdiv_update_tune_code_timer_init(void)
{
  /* define the immediate update tune code timer callback */
  timer_def2( &rfcommon_asdiv_update_tune_code_clk_cb, NULL );
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is the call back function and is called after update tune code timer expires. 

*/
void rfcommon_asdiv_update_tune_code_timer_trigger(uint32 t_unused, timer_cb_data_type data)
{
  /* Execute the callback function registered by tech for update tune code */
  if ( asdiv_manager_state.update_tuner_cb.cb_func != NULL )
  {
    rfcommon_asdiv_tuner_update_tuner_code(asdiv_manager_state.update_tuner_cb);
    asdiv_manager_state.update_tuner_cb.cb_func = NULL;
    asdiv_manager_state.update_tuner_cb.cb_data = FALSE;    
  } /* if ( asdiv_manager_state.update_tuner_cb != NULL ) */
  
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_asdiv_update_tune_code_timer_trigger, NULL cb function" );
  }
}

/*-------------------------------------------------------------------------*/
/*!
  @brief
  Query previous antenna position
 
  @details
  This function will return the previous antenna position
 
  @param device
  RFM deivce for which the antenna position is queried
 
  @return
  previous antenna position
*/
rfcommon_asdiv_position_type
rfcommon_asdiv_get_previous_position
(
  rfm_device_enum_type device
)
{
  rfcommon_asdiv_position_type prev_pos; 
  
  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));

  prev_pos = asdiv_manager_state.prev_sw_pos;

  /* Release ASDIV manager interface*/
  rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));
  
  return prev_pos;
} /* rfcommon_asdiv_get_previous_position */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to LOCK the asdiv manager interface. 

  @details
  This API can be used in the context of ASDiv, Tuner System. 

  @return
  
*/
void rfcommon_asdiv_manager_enter_lock(void)
{
  /* Lock the ASDIV manager interface */
  rf_common_enter_critical_section(&(asdiv_manager_state.api_lock));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to UNLOCK the asdiv manager interface. 

  @details
  This API can be used in the context of ASDiv, Tuner System. 

  @return
  
*/
void rfcommon_asdiv_manager_release_lock(void)
{
 /* Release ASDIV manager interface*/
 rf_common_leave_critical_section(&(asdiv_manager_state.api_lock));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to LOCK the asdiv manager interface if its available 
  else return FALSE

  @details
  This API can be used in the context of ASDiv, Tuner System. 

  @retval TRUE
  The critical section is now locked and owned by the calling task.
 
  @retval FALSE
  The critical section was locked by another task and is not owned by the
  calling task.
  
*/

boolean rfcommon_asdiv_manager_try_enter_lock(void)
{
  /* Return True if critical section is available */
  return rf_common_try_enter_critical_section(&(asdiv_manager_state.api_lock));
}


/*---------------------------------------------------------------------------*/

#endif /*  defined(FEATURE_RF_ASDIV) || defined(FEATURE_RF_HAS_TPTUNER_SWITCH) */

