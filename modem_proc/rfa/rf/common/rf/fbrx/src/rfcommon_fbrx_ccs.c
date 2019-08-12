/*!
  @file
  rfcommon_fbrx_ccs.c

  @brief
  This module contains interface and implementation of common FBRx CCS Events.
  
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

  $Header: //source/qcom/qct/modem/rfa3/main/3.0/rf/common/rf/fbrx/src/rfcommon_fbrx_ccs.c

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/08/14    ka     Replace rfc with device api for retrieving device pointer
08/19/14    aa     FBRx + GPS concurrency SM implementation
08/14/14    pv     Added support for FBRx + GPS concurrency based scripts.
08/06/14   aro     Interface to export FBRx event handler
07/25/14    aa     Change event status to COMPLETE after event build is successfull 
07/02/14    aa     Change event status to IDLE state after event build is successfull 
07/01/14    aa     Added fix to clean up heap allocation by fbrx ccs events
06/09/14    aa     Change FBRx setup event type to dynamic to fix trigger time issue
05/13/14    aa     Set fbrx event handle pointers to NULL
05/02/14    aa     Use rflm_fbrx_ccs_event_type_t instead of rfmeas_event_infor_type
04/01/14    aa     Initial version - Support for FBRX CCS Events
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcommon_fbrx_ccs.h"
#include "rfcommon_msg.h"
#include "rfcommon_core_utils.h"

static rfcommon_mdsp_event_handle* rfcommon_fbrx_ccs_event_handle_ptr[RFLM_FBRX_CCS_NUM_MAX_EVENT];

/*============================================================================*/

/*==============================================================================
                            LOCAL FUNCTION DEFINITIONS
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @name rfcommon_fbrx_ccs_allocate_associate_script

  @brief
  This API will allocate buffers for all static and dynamic
  events using calls to the RFLM layer
  
  @param fbrx_handle
  The fbrx_handle for which buffer allocation is required
 
  @param event_id
  The rflm event for which the script is required. 
 
  @return
  TRUE if buffers are allocated
  FALSE if there is an error in buffer allocation
*/
boolean
rfcommon_fbrx_ccs_allocate_associate_script
(
  lm_handle_type                fbrx_handle,
  rflm_fbrx_ccs_event_type_e    event_id 
)
{
  boolean api_status = TRUE ;
  rflm_dm_script_meta_s buff_meta ;
  script_exe_callback_func_t cb = NULL ; 
  /* Local var to store buf ptr */
  uint32* buf_ptr = NULL ;

  /* Owner will always be FBRX in this context */
  buff_meta.owner = RFLM_USR_FBRX ;

  /* For a given handle and ID only allocate the handle once. Use the same 
     buffer for subsequent wakeup-sleep pairs. The buffers will be automatically 
     deallocated and disassociated once LM deltes the lm handle these
     scripts are associated with.  
  */
  if ( NULL == rflm_dm_get_script_buf_ptr ( fbrx_handle, event_id ) )
  {
    /* Check for NULL ptr, allocate buf ptr for each script type */
    switch ( event_id ) 
    {
      /* Allocate buf ptr for FBRX Enable script */
      case RFLM_FBRX_CCS_SETUP_EVENT:
      {
        buff_meta.type = SCRIPT_TYPE_COMMON;
        buff_meta.size_in_byte = RFCOMMON_FBRX_DEFAULT_SCRIPT_SIZE;
        buff_meta.fmt = SCRIPT_FMT_DEVICE_FMT;
        break ;
      }

      /* Allocate buf ptr for FBRX Disable script */
      case RFLM_FBRX_CCS_DISABLE_EVENT:
      {
        buff_meta.type = SCRIPT_TYPE_COMMON;
        buff_meta.size_in_byte = RFCOMMON_FBRX_DEFAULT_SCRIPT_SIZE;
        buff_meta.fmt = SCRIPT_FMT_DEVICE_FMT;
        break ;
      }

      /* Allocate buf ptr for FBRX always ON script */
      case RFLM_FBRX_CCS_ALWAYS_ON_EVENT:
      {
        buff_meta.type = SCRIPT_TYPE_COMMON;
        buff_meta.size_in_byte = RFCOMMON_FBRX_DEFAULT_SCRIPT_SIZE;
        buff_meta.fmt = SCRIPT_FMT_DEVICE_FMT;
        break ;
      }

       /* Allocate buf ptr for FBRX Disable script */
       case RFLM_FBRX_CCS_READBACK_POWER_EVENT:
       {
         buff_meta.type = SCRIPT_TYPE_COMMON;
         buff_meta.size_in_byte = RFCOMMON_FBRX_DEFAULT_SCRIPT_SIZE;
         buff_meta.fmt = SCRIPT_FMT_DEVICE_FMT;
         break ;
       }

      default:
      {
        api_status = FALSE ;
        RF_MSG_2 ( RF_ERROR,"rfcommon_fbrx_ccs_allocate_associate_script: lm_handle " 
                   "DM buffer pointer is NULL for LM Id : %d , passed event ID: " , 
                   fbrx_handle , event_id ) ; 
      }
    }/* SWITCH */

    /* Get pointer to the allocated script buffer from RFLM using the meta information */
    if ( api_status == TRUE )
    {
      buf_ptr = rflm_dm_allocate_script_buf_for_handle ( &buff_meta, fbrx_handle, 
                                                         event_id, cb ) ;

      /* Store buf ptr if not NULL*/
      if( buf_ptr == NULL )
      {
        api_status = FALSE ;
        RF_MSG_2 ( RF_ERROR , 
                   "rfcommon_fbrx_ccs_allocate_associate_script(): "
                   "lm_handle[%d]: , rflm_script_id: event_id " 
                   "buffer allocation failed!" , 
                   fbrx_handle , event_id ) ; 

      }
    }
  }

  return api_status ;
}/*rfcommon_fbrx_ccs_allocate_associate_script*/

/*----------------------------------------------------------------------------*/
/*! @brief 
  This API is specific to Bolt modem and it returns the address of the
  DM buffer for a given event_id.

  @details
  The API takes the CCS event_id and returns the DM memory pointer for
  the given event_id. 
 
  @param fbrx_handle
  The fbrx_handle for which buffer allocation is required
 
  @param event_id
  The rflm event for which the script is required.
 
  @param ccs_event_transaction_params
  Pointer to the ccs event transaction params which needs to be updated.
 
  @param is_event_issue_seq
  Pointer to update if event is issue_seq type or not.
 
  @return
  void* Address of the associated DM buffer.
*/
void*
rfcommon_fbrx_ccs_get_buffer
(
  lm_handle_type                            fbrx_handle,
  rflm_fbrx_ccs_event_type_e                event_id,
  rfcommon_mdsp_transaction_settings_type   *ccs_event_transaction_params, 
  boolean                                   *is_event_issue_seq
) 
{
  uint32* dm = NULL ;

  switch ( event_id )
  {
    case RFLM_FBRX_CCS_SETUP_EVENT: 
    {
      /* update task priority */
      ccs_event_transaction_params->priority = 15;
      /* Additionaly notify calling API if the event is issue seq type */
      *is_event_issue_seq = FALSE;
      break;
    }

    case RFLM_FBRX_CCS_DISABLE_EVENT: 
    {
      /* update task priority */
      ccs_event_transaction_params->priority = 15;
      /* Additionaly notify calling API if the event is issue seq type */
      *is_event_issue_seq = FALSE;
      break;
    }

    case RFLM_FBRX_CCS_ALWAYS_ON_EVENT: 
    {
      /* update task priority */
      ccs_event_transaction_params->priority = 15;
      /* Additionaly notify calling API if the event is issue seq type */
      *is_event_issue_seq = FALSE;
      break;
    }

    case RFLM_FBRX_CCS_READBACK_POWER_EVENT:
    {
      /* update task priority */
      ccs_event_transaction_params->priority = 15;
      /* Additionaly notify calling API if the event is issue seq type */
      *is_event_issue_seq = FALSE;
      break;
    }

    default:
    {
      /* Throw out error message if event is invalid */
      RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_get_buffer: Invalid event ID" );
      break;
    }

  } /*switch ( event_id )*/

  if ( TRUE != rfcommon_fbrx_ccs_allocate_associate_script( fbrx_handle , event_id ) )
  {
    RF_MSG ( RF_ERROR , "rfcommon_fbrx_ccs_get_buffer: "
             "rfcommon_fbrx_ccs_allocate_associate_script() returned FALSE" ) ;
  }
  else
  {
    dm = rflm_dm_get_script_buf_ptr( fbrx_handle , event_id ) ;
  }

  return ((void*) (dm) ) ;

} /* rfcommon_fbrx_ccs_get_buffer  */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provide generic interface to start/push/commit RF event scripts 

  @param event_settings
  Holds the various CCS settings for the event
 
  @param buf_obj_ptr
  Data structure containing scriptiong informaiton
 
  @param fbrx_handle
  The fbrx_handle for which buffer allocation is required
 
  @param tx_handle
  The tx_handle for which the scripts are updated to DM
 
  @param event_id
  Represents the type of event (Setup FBRx, Disable FBRx, etc)

  @param event_handle
  This is an unique identifier that CCS needs for a given event  
 
  @return
  Flag indicating the function execution success
 
*/
boolean
rfcommon_fbrx_ccs_configure_event
(
  const rfcommon_mdsp_event_settings_type   *event_settings,
  rf_buffer_intf                            *buf_obj_ptr,
  lm_handle_type                            fbrx_handle,
  rflm_handle_tx_t                          tx_handle,
  rflm_fbrx_ccs_event_type_e                event_id,
  rfcommon_mdsp_event_handle                **event_handle
)
{
  boolean api_status = TRUE ; 
  /* Flag to check for events to be issue seq/non issue seq type, FALSE by default */
  boolean is_event_issue_seq = FALSE;
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  rflm_fbrx_ccs_event_infor_type* event_ptr = NULL;

  /* Transaction parameters for CCS event */
  /* Initialize to default values, the values can be modified as per
  the requriement of the specific event being executed */
  rfcommon_mdsp_transaction_settings_type ccs_event_transaction_params = 
  {
    RFCOMMON_MDSP_TRANSACTION_TRIGGER_DYNAMIC, /* trigger_type */
    2,     /* default priority (0-lowest, 15-highest) */
    FALSE, /* preempt_error */
    FALSE, /* use_ssbi_access */
    0      /* time_offset */
  };

  if( buf_obj_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_event: NULL pointer "
                      "for buffer interface object!");
    api_status = FALSE;
    return api_status;
  } 

  /* Get FBRX DM data pointer */
  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);

  if (fbrx_data_ptr != NULL)
  {
    fbrx_data_ptr->fbrx_script_ptr[event_id] = rfcommon_fbrx_ccs_get_buffer( fbrx_handle,
                                                                             event_id,
                                                                             &ccs_event_transaction_params,
                                                                             &is_event_issue_seq );

    /* For non issue seq events use the DM memory location returned by the above 
    API , since they do not need to copy over data and convert it into issue seq
    stype events, while for events that are issue seq, use the locally declared
    memory location and update it with the settings, this location is then used
    to create an issue seq stype event in the DM memory location returned*/ 
    if ( fbrx_data_ptr->fbrx_script_ptr[event_id] != NULL  )
    {
      /* Execute CCS Event Sequence: Start, Push and Commit*/
      if( is_event_issue_seq == FALSE )
      {
        api_status = rfcommon_mdsp_event_start_push_and_commit( event_id,
                                                                event_handle,
                                                                event_settings,
                                                                &ccs_event_transaction_params,
                                                                fbrx_data_ptr->fbrx_script_ptr[event_id],
                                                                sizeof(rflm_fbrx_ccs_event_infor_type),
                                                                buf_obj_ptr,
                                                                FALSE  );

        if ( api_status == TRUE )
        {
          RF_MSG_4( RF_LOW, "rfcommon_fbrx_ccs_configure_event: Begin CCS Flow: "
                    "Event ID 0x%08x, Event handle 0x%08x, Shared mem ptr 0x%08x, Priority %d",
                    event_id, event_handle, fbrx_data_ptr->fbrx_script_ptr[event_id], 
                    ccs_event_transaction_params.priority ); 
          /* Change the CCS event status to IDLE state */
          event_ptr = (rflm_fbrx_ccs_event_infor_type*)fbrx_data_ptr->fbrx_script_ptr[event_id];
          event_ptr->header_task.header.event_status = RFLM_CCS_RF_EVENT_COMPLETE;
        }
        else
        {
          RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_event: rfcommon_mdsp_event_start_push_and_commit() "
                            "API Failed" );
        }
      }/* if( is_event_issue_seq == FALSE )*/
    }
    else
    {
      RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_event: rfcommon_fbrx_ccs_get_buffer() API "
                        "returned NULL pointer" );
      api_status = FALSE;
    }
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfcommon_fbrx_ccs_configure_event: NULL fbrx_data_ptr returned "
                        "for tx handle =%d", tx_handle );
    api_status = FALSE;
  }

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_ccs_configure_on_off_scripts
 
  @details
  Creates the FBRX ON/OFF event script. The scripts can be either 
  SSBI, RFFE or GRFC based, depends on what is supported
 
  @param device
  Device which needs to be configured for FBRX ON/OFF
  
  @param fbrx_lm_handle
  The fbrx_handle for which buffer allocation is required
 
  @param tx_handle
  The tx_handle for which the scripts are updated to DM
 
  @return TRUE if device APIs return TRUE, FALSE otherwise

*/
boolean 
rfcommon_fbrx_ccs_configure_on_off_scripts
( 
  rfm_device_enum_type    device,
  lm_handle_type          fbrx_handle,
  rflm_handle_tx_t        tx_handle 
)
{
  boolean ldo_on_to_off_script_status = TRUE;
  boolean ldo_off_to_on_script_status = TRUE;
  boolean ldo_always_on_script_status = TRUE;
  boolean api_status = TRUE;

  //FBRX LDO ON to OFF EVENT
  ldo_on_to_off_script_status = rfcommon_fbrx_ccs_configure_ldo_scripts( device,
                                                        fbrx_handle,
                                                        RFLM_FBRX_CCS_DISABLE_EVENT,
                                                        FALSE,
                                                        RFDEVICE_FBRX_LDO_ON_TO_OFF,
                                                        tx_handle);
  if ( !ldo_on_to_off_script_status )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_on_off_scripts: "
                      " RFDEVICE_FBRX_LDO_ON_TO_OFF configure script failed" );
    api_status = FALSE;
  }

  //FBRX LDO OFF to ON EVENT
  ldo_off_to_on_script_status = rfcommon_fbrx_ccs_configure_ldo_scripts( device,
                                                        fbrx_handle,
                                                        RFLM_FBRX_CCS_SETUP_EVENT,
                                                        TRUE,
                                                        RFDEVICE_FBRX_LDO_OFF_TO_ON,
                                                        tx_handle);
  if ( !ldo_off_to_on_script_status )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_on_off_scripts: "
                      " RFDEVICE_FBRX_LDO_OFF_TO_ON configure script failed" );
    api_status = FALSE;
  }

  //FBRX LDO ALWAYS EVENT
  ldo_always_on_script_status = rfcommon_fbrx_ccs_configure_ldo_scripts( device,
                                                        fbrx_handle,
                                                        RFLM_FBRX_CCS_ALWAYS_ON_EVENT,
                                                        TRUE,
                                                        RFDEVICE_FBRX_LDO_ALWAYS_ON,
                                                        tx_handle);
  if ( !ldo_always_on_script_status )
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_on_off_scripts: "
                      " RFDEVICE_FBRX_LDO_ALWAYS_ON configure script failed" );
    api_status = FALSE;
  }

  return api_status;
}

/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Get Pointer to FBRx event handle
 
  @return
  Pointer to the structure containing all FBRx event handles
*/
rfcommon_mdsp_event_handle**
rfcommon_fbrx_get_event_handle_ptr
(
  rflm_fbrx_ccs_event_type_e event_id
)
{
  return rfcommon_fbrx_ccs_event_handle_ptr;

  if ( event_id < RFLM_FBRX_CCS_NUM_MAX_EVENT )
  {
    return (&(rfcommon_fbrx_ccs_event_handle_ptr[event_id]));
  }
  else
  {
    return NULL;
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfcommon_fbrx_ccs_configure_ldo_scripts
 
  @details
  Creates the FBRX LDO ON/OFF event script. The scripts can be either 
  SSBI, RFFE or GRFC based, depends on what is supported
 
  @param device
  Device which needs to be configured for FBRX LDO ON/OFF
  
  @param fbrx_lm_handle
  The fbrx_handle for which buffer allocation is required
 
  @param fbrx_event_id
  The fbrx script type
 
  @param fbrx_get_setup_script_flag
  Flag which determines if FBRX setup is to be used or fbrx disable to create the FBRX script
 
  @param fbrx_ldo_scenario
  The FBRX LDO setting scenario
 
  @param tx_handle
  The tx_handle for which the scripts are updated to DM
 
  @return TRUE if device APIs return TRUE, FALSE otherwise

*/
boolean 
rfcommon_fbrx_ccs_configure_ldo_scripts
( 
  rfm_device_enum_type        device,
  lm_handle_type              fbrx_handle,
  rflm_fbrx_ccs_event_type_e  fbrx_event_id,
  boolean                     fbrx_get_setup_script_flag,
  rfdevice_fbrx_scenario_type fbrx_ldo_scenario, 
  rflm_handle_tx_t            tx_handle 
)
{
  boolean api_status = TRUE;
  uint8 setup_time_us = 0;
  rfdevice_rxtx_common_class *device_ptr = NULL; 
  rflm_fbrx_dm_template_t *fbrx_data_ptr = NULL;
  rfcom_band_type_u band_union;

  /* Buffer object to hold FBRX LDO script */
  rf_buffer_intf *rf_fbrx_script = NULL;

  /* Unique CCS Event handles */
  rfcommon_mdsp_event_handle** fbrx_event_handle = NULL; 

  /* Generic CCS event settings */
  const rfcommon_mdsp_event_settings_type event_settings_generic = 
    { 
      FALSE,     /*! Make_atomic */
      FALSE,     /*! Response_required */ 
      NULL,      /*! Callback_ptr */
      &rfcommon_msm_convert_microsecs_to_ustmr_units
    };

  fbrx_data_ptr = (rflm_fbrx_dm_template_t*)rflm_dm_get_fbrx_buf_ptr(tx_handle);
  if (fbrx_data_ptr != NULL)
  {
    band_union = rfcommon_fbrx_mc_convert_uint8_to_rfcom_band((rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                              fbrx_data_ptr->fbrx_ctl.fbrx_band);
    device_ptr = rfcommon_core_get_tx_device_ptr(device, 
                                                 (rfm_mode_enum_type)fbrx_data_ptr->fbrx_ctl.rfm_mode, 
                                                 band_union);
  }
  if (device_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_ldo_scripts: Null WTR device ptr" );
    api_status = FALSE;
    return api_status;
  }

  /*Get RF FBRX event handle*/
  fbrx_event_handle = &(rfcommon_fbrx_ccs_event_handle_ptr[fbrx_event_id]); 

  /* Create generic buffer interface object to hold script settings */
  rf_fbrx_script = (rf_buffer_intf*) rf_buffer_create ( RFCOMMON_FBRX_MAX_SSBI_SCRIPT_SIZE , 
                                                        RFCOMMON_FBRX_MAX_RFFE_SCRIPT_SIZE , 
                                                        RFCOMMON_FBRX_MAX_GRFC_SCRIPT_SIZE ) ;

  /* Check if valid pointer was returned for buffer object */
  if( rf_fbrx_script != NULL )
  {
    /* Clear buffer */
    (void)rf_buffer_clear(rf_fbrx_script);

    if ( fbrx_get_setup_script_flag )
    {
      api_status = rfdevice_cmn_get_fbrx_setup_script( device_ptr,
                                                       RFDEVICE_CREATE_SCRIPT,
                                                       rf_fbrx_script,
                                                       &setup_time_us,
                                                       fbrx_ldo_scenario);
    }
    else 
    {
      api_status = rfdevice_cmn_get_fbrx_disable_script( device_ptr,
                                                         RFDEVICE_CREATE_SCRIPT,
                                                         rf_fbrx_script,
                                                         fbrx_ldo_scenario);
    }

    if ( api_status == TRUE )
    {
      *fbrx_event_handle = NULL;
      /* Configure FBRX event using the obtained settings*/
      api_status = rfcommon_fbrx_ccs_configure_event( &event_settings_generic,
                                                      rf_fbrx_script,
                                                      fbrx_handle,
                                                      tx_handle,
                                                      fbrx_event_id,
                                                      fbrx_event_handle );
      rfcommon_mdsp_event_cleanup(fbrx_event_handle);

    }
    else
    {
      RF_MSG_1 ( RF_ERROR, "rfcommon_fbrx_ccs_configure_ldo_scripts: rfdevice_cmn_get_fbrx_setup_script FAILED "
                        "for dev_ptr:%d", device_ptr );
    }

    rf_buffer_destroy(rf_fbrx_script);
  }
  else
  {
    RF_MSG( RF_ERROR, "rfcommon_fbrx_ccs_configure_ldo_scripts: "
                      "Unable to create buffer object for fbrx event settings");
    api_status = FALSE;
  }

  return api_status;
}

/*----------------------------------------------------------------------------*/



/*! @} */


