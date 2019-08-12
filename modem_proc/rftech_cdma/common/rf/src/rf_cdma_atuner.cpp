/*!
  @file
  rf_cdma_atuner.cpp

  @brief
  <b>This is the Antenna Tuner interface exposed to the MC layer.</b>
  This module contains all CDMA-Antenna Tuner-specific data and implementations. 

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{
    @addtogroup RF_RFC_CDMA_TUNER_WRAPPER
    @{
*/

/*==============================================================================

  Copyright (c) 2013-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_atuner.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/27/15   vc      Propogating immediate flag for FTM AsDiv support
10/21/14   yb      Added priority configuration to config Tuners during CRAT scenarios
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/19/14   zhw     Fix hard coded RFM_DEVICE_0 in asdiv_config_switch
07/07/14   zhw     Compiler fix for non-CDMA configuration
05/29/14   zhw     Clean up deprecated Tuner CL APIs
05/14/14   APU     Suppy CL tuner with valid TxLM handle.
02/20/14   zhw     KW error fix
02/13/14   zhw     Compiler warning fix
02/04/14   zhw     Support for updating tuner list during runtime
3/26/14    kg      Added support for CL LM handle
03/10/14   as/spa  Added callback function as part of antenna switch
03/10/14   as/spa  Deprecated asdiv_obj from config_switch API
03/04/14   hdz     Added c2k support for AsDiv
01/03/14   zhw     Update Tune Code API. Remove deprecated interface
11/26/13   yb      Added tune_code_override API
10/28/13   vb      Tuner force_detune() for supporting FTM_PROGRAM_QFE_SCENARIO
09/04/13   zhw     Tuner AsDiv support
07/12/13   spa     Update #def being used for max possible instances per band
07/12/13   spa     Use master_token and sub_token for acknowledgement
07/05/13   zhw     Tuner CL NV support
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/27/13   ndb     Added rfcommon_nv_ant_tuner_cl_ctrl_type in tuner tx mode 
06/25/13   zhw     Tuner CL support. Added Tx disable API which clears CL timer
06/21/13   zhw     Tuner CL support. Added RL meas call back registration
06/19/13   spa     Removed debug messages (not required)
06/10/13   spa     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X

#include "rf_cdma_atuner.h"
#include "rfc_cdma_data.h"
#include "rfcommon_msg.h"
#include "rfcommon_tuner.h"
#include "rf_cdma_data.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_tuner_manager.h"
#include "rf_cdma_mc.h"
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device init
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param master_token
  Token for tuner device
 
  @retval TRUE
  If atuner device init was successful
 
  @retval FALSE
  False if device init failed
*/
boolean
rf_cdma_atuner_init
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
)
{
  /* Call wrapper interface */
  return( atuner_ptr->init( atuner_ptr, mode, dev_action,
                               script_buffer, script_timing, master_token ) );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device disable
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param script_buffer
  Immediate or scripting mode (Script execution type)
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param master_token
  Token for tuner device
 
  @retval TRUE
  If atuner device disable was successful
 
  @retval FALSE
  False if device disable failed
*/
boolean
rf_cdma_atuner_disable
(
  rf_cdma_atuner *atuner_ptr,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
)
{
  /* Call wrapper interface */
  return( atuner_ptr->disable(  atuner_ptr,  dev_action,
                               script_buffer, script_timing, master_token ) );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device tx disable
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
   
  @param mode 
  RFM mode 
   
  @param chan_num 
  Channel Number 
   
  @param dev_action 
  Immediate write or Scripting 
   
  @param script_buffer
  Immediate or scripting mode (Script execution type)
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param master_token
  Token for tuner device
 
  @retval TRUE
  If atuner device tx disable was successful
 
  @retval FALSE
  False if device tx disable failed
*/
boolean
rf_cdma_atuner_tx_disable
(
  rf_cdma_atuner *atuner_ptr,
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
)
{
  /* Call wrapper interface */
  return( atuner_ptr->tx_disable( atuner_ptr,
                                  mode,
                                  band,
                                  chan_num,
                                  dev_action,
                                  script_buffer,
                                  script_timing,
                                  master_token ) );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device Rx configuration
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param master_token
  Token for tuner device
 
  @retval TRUE
  If atuner device set Rx mode was successful
 
  @retval FALSE
  False if device set Rx mode failed
*/
boolean
rf_cdma_atuner_configure_for_rx
(
  rfcom_antenna_tuner_priority_cfg priority_cfg,
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
)
{
  /* Call wrapper interface */
  return( atuner_ptr->configure_for_rx(  priority_cfg, atuner_ptr, mode, band, chan_num, 
                                       dev_action, script_buffer, script_timing,
                                       master_token ) );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device Tx configuration
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuner
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param master_token
  Token for tuner device

  @param tuner_cl_nv_ptr
  Tuner CL NV data structure 

  @retval TRUE
  If atuner device set Tx mode was successful
 
  @retval FALSE
  False if device set Tx mode failed
*/
boolean
rf_cdma_atuner_configure_for_tx
(
  lm_handle_type txlm_handle,
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token,
  rfcommon_nv_ant_tuner_cl_ctrl_type *tuner_cl_nv_ptr
)
{
  /* Call wrapper interface */
  return( atuner_ptr->configure_for_tx(  txlm_handle , atuner_ptr, mode, band, 
                                         chan_num, dev_action, script_buffer, 
                                         script_timing, master_token, 
                                         tuner_cl_nv_ptr ) );
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device to get dynamically
  re-tuned by updating tune code
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param is_this_rx_operation
  TRUE: API is called in Rx context/ FALSE : API is not in Rx context
 
  @param tuner_cb_data
  Tuner callback data,info used when response is sent from FW
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param script_token
  Token for tuner device
 
  @retval TRUE
  If atuner device update tune code was successful
 
  @retval FALSE
  False if device update tune code failed
*/
boolean
rf_cdma_atuner_update_tune_code
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation,
  uint16 tuner_mdsp_chain,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  void* tuner_cb_data
)
{
  /* Call wrapper interface */
  return( atuner_ptr->update_tune_code(  atuner_ptr, mode, band, 
                                       chan_num, 
                                       is_this_rx_operation,
                                       tuner_cb_data,
                                       tuner_mdsp_chain,
                                       dev_action ,script_buffer,
                                       script_timing ));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device to get dynamically 
  re-tuned by updating tune code
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param is_this_rx_operation
  TRUE: API is called in Rx context/ FALSE : API is not in Rx context
 
  @retval TRUE
  If atuner device update tune code was successful
 
  @retval FALSE
  False if device update tune code failed
*/
boolean
rf_cdma_atuner_update_ftm_tune_code
(
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation
)
{
  /* Call wrapper interface */
  return( atuner_ptr->update_ftm_tune_code(atuner_ptr, 
                                           mode, 
                                           band, 
                                           chan_num, 
                                           is_this_rx_operation ));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device Tx configuration
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param master_token
  Token for tuner device
 
  @retval TRUE
  If atuner device script ack was successful
 
  @retval FALSE
  False if device script ack failed
*/
boolean
rf_cdma_atuner_ack_script
(
  rf_cdma_atuner *atuner_ptr,
  ant_tuner_device_tokens_type *master_token
)
{
  /* Call wrapper interface */
  return( atuner_ptr->ack_script( atuner_ptr, master_token ) );
}

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer for atuner device close loop call back 
  registration 
 
  @param device
  RFM device on which the antenna switch is to be initiated
 
  @param rfm_mode
  RFM mode active on the current device
 
  @param asdiv_obj
  Pointer to the oject containing ASDiv
 
  @param antenna_position
  New antenna position
 
  @param script_ptr
  Pointer to the buffer allocated by Tech to store the script for antenna
  switch.
 
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
rf_cdma_atuner_asdiv_config_switch
(
  rf_cdma_atuner *atuner_ptr, 
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *script_ptr,
  rfcommon_asdiv_asm_conifg_type asm_script_cfg,
  rfcommon_asdiv_send_script_type send_script_cfg,
  rfcommon_asdiv_switch_start_notify_type switch_start_cfg,
  rfcommon_asdiv_update_tuner_type update_tuner_cb,
  rfm_l1_cb_type l1_cb_func,
  void *l1_cb_data,
  boolean gen_script_only
)
{
  if( atuner_ptr != NULL )
  {
    return( atuner_ptr->asdiv_config_switch( device,
                                             rfm_mode,
                                             antenna_position, 
                                             script_ptr, 
                                             asm_script_cfg,
                                             send_script_cfg,
                                             switch_start_cfg,
                                             update_tuner_cb,
                                             l1_cb_func, 
                                             l1_cb_data, 
                                             gen_script_only  ) );
  }
  else
  {
    return TRUE;
  }
}

#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface functionality to MC layer to override current tuner setting
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param override_flag
  Tuner override flag: Any value greater than '1' indicates the current tuner settings 
  can be overriden.

  @param data
  Pointer to the tuner tunecode config structure 

  @param tuner_nv_ptr
  Pointer to the NV data structure

  @param tuner_id_ptr   
  Pointer to the tuner_id
 
  @return
  rfdevice_antenna_tuner_device_override_status_type 
*/
rfdevice_antenna_tuner_device_override_status_type
rf_cdma_atuner_tune_code_override
(
  rf_cdma_atuner *atuner_ptr,
  uint8  override_flag,
  void*  data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
)
{
  /* Call wrapper interface */
  return( atuner_ptr->tune_code_override(atuner_ptr, 
                                         override_flag,
                                         data,
                                         tuner_nv_ptr,
                                         tuner_id_ptr));
}

/*----------------------------------------------------------------------------*/
/* CDMA ATUNER class */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constructor - Initializes the parameters for the atuners wrapper object

  @param rfm_device
  Device for which the atuner wrapper object is created 
*/
rf_cdma_atuner::rf_cdma_atuner( int rfm_device, uint8 default_ant_num )
{
  /* Data iterator */
  uint8 i;

  this->rfm_device = rfm_device;
  this->default_atuner_num = 0;
  this->default_antenna = default_ant_num;
   
  for ( i = 0; i < RFC_MAX_DEVICE_INSTANCES_PER_BAND; i++ )
  {
    this->default_tuner_list[i] = NULL;
  }

  this->updated_tuner_list_length = 0;
  this->updated_tuner_list = NULL;

  #ifdef FEATURE_RF_ASDIV
  {
    rfdevice_class* asd_tuner_mgr = NULL;
    rfc_intf *rfc_cmn = rfc_intf::get_instance();
    if ( rfc_cmn != NULL ) 
    {
      asd_tuner_mgr = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0); 
    }

    if ( asd_tuner_mgr != NULL )
    {
     this->tuner_asdiv_enabled = TRUE;
    }
    else
    {
      this->tuner_asdiv_enabled = FALSE;
    }
  }
  #endif
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Add RF ATUNER device to this wrapper object

  @details
  This wrapper object stores pointers, upto a maximum allowed instance value, 
  to RF atuner device objects. Duplicate objects are not included in the wrapper.
 
  @param new_device
  Pointer to atuner device which must be added to the wrapper object

  @retval TRUE
  If ATUNER device was added successfully
 
  @retval FALSE
  If adding a new device failed
*/
boolean 
rf_cdma_atuner::add_default_tuner_instance
( 
  rfcommon_atuner_intf* new_device 
) 
{
  boolean ret_val = TRUE;
  uint8 num_atuners = this->default_atuner_num;
  uint8 i;
  boolean device_already_exists = FALSE;

  if ( new_device != NULL )
  {
    /* Search if this wrapper object already contains the device 
    to be added */
    for ( i = 0; i < num_atuners; i++ )
    {
      if ( this->default_tuner_list[i] == new_device )
      {
        /* Found a match */
        device_already_exists = TRUE;
        break;
      }
    }

    /* If a device is to be added, update the wrapper object */
    if ( num_atuners < RFC_MAX_DEVICE_INSTANCES_PER_BAND &&
         device_already_exists == FALSE )
    {
      this->default_tuner_list[num_atuners] = new_device;
      this->default_atuner_num++;
      ret_val = TRUE;
    }
    else if ( num_atuners >= RFC_MAX_DEVICE_INSTANCES_PER_BAND )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_atuner::add_device_instance: Cannot "
                          "add more than allowed atuner instances. "
                          "Max allowed %d",
                RFC_MAX_DEVICE_INSTANCES_PER_BAND );
      ret_val = FALSE;
    }
  }/*if ( new_device != NULL )*/
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_atuner::add_device_instance: Requesting "
                      "to add NULL device" );
    ret_val = FALSE;
  }/*if ( new_device == NULL )*/

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device init. Init is executed on 
  all valid atuner devices in the wrapper object
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner settings

  @param dev_action
  Script execution type
 
  @retval TRUE
  If atuner device init was successful
 
  @retval FALSE
  False if device init failed
*/
boolean
rf_cdma_atuner::init
(  
  rf_cdma_atuner *atuner_ptr, 
  rfm_mode_enum_type mode, 
  rf_device_execution_type dev_action, 
  rf_buffer_intf *script_buffer, 
  int16 script_timing, 
  ant_tuner_device_tokens_type *master_token 
) 
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;

  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Wakeup ATUNER */    
      result &= rfcommon_atuner_manager_init( atuner_device, mode,dev_action,
                                              script_buffer,                                             
                                              script_timing,
                                              &this->tuner_sub_tokens[i].token ); 

      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::device_init: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if (ATUNER_device != NULL )*/ 
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_atuner::device_init: "
                        "ATUNER is null" );
    }/*if ( ATUNER_device == NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device disable. Disable is executed 
  on all valid atuner devices in the wrapper object
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner settings

  @param dev_action
  Script execution type
 
  @retval TRUE
  If atuner device sleep was successful
 
  @retval FALSE
  False if device sleep failed
*/
boolean
rf_cdma_atuner::disable
( 
  rf_cdma_atuner *atuner_ptr, 
  rf_device_execution_type dev_action, 
  rf_buffer_intf *script_buffer, 
  int16 script_timing, 
  ant_tuner_device_tokens_type *master_token
) 
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;

  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Put ATUNER to sleep */
      result &= rfcommon_atuner_manager_disable( atuner_device,dev_action,
                                                 script_buffer,
                                                 script_timing,
                                                 &this->tuner_sub_tokens[i].token );
      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::device_disable: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( ATUNER_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device tx disable. Tx disable is 
  executed on all valid atuner devices in the wrapper object
   
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
   
  @param mode 
  RFM mode 
   
  @param chan_num 
  Channel Number 
   
  @param dev_action 
  Immediate write or Scripting 
   
  @param script_buffer
  Immediate or scripting mode (Script execution type)
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param script_token
  Token for tuner device
 
  @retval TRUE
  If atuner device tx disable was successful
 
  @retval FALSE
  False if device tx disable failed
*/
boolean
rf_cdma_atuner::tx_disable
( 
  rf_cdma_atuner *atuner_ptr, 
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
) 
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;

  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Tuner Disable TX sequence */
      result &= rfcommon_atuner_manager_tx_disable( atuner_device,
                                                    mode,
                                                    band,
                                                    chan_num,
                                                    dev_action,
                                                    script_buffer,
                                                    script_timing,
                                                    &this->tuner_sub_tokens[i].token );
      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::tx_disable: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( ATUNER_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_atuner::tx_disable: "
                        "ATUNER is null" );
    }/*if ( ATUNER_device == NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device retune disable. Retune disable 
  is executed on all valid atuner devices in the wrapper object
   
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
   
  @param mode 
  RFM mode 
   
  @param chan_num 
  Channel Number 
   
  @param dev_action 
  Immediate write or Scripting 
   
  @param script_buffer
  Immediate or scripting mode (Script execution type)
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param script_token
  Token for tuner device
 
  @retval TRUE
  If atuner device tx disable was successful
 
  @retval FALSE
  False if device tx disable failed
*/

boolean
rf_cdma_atuner::retune_disable
(
   rf_cdma_atuner *atuner_ptr, 
   rfcom_mode_enum_type mode,
   rfcom_band_type_u band,
   uint16 chan_num,
   rf_device_execution_type dev_action,
   rf_buffer_intf *script_buffer,
   int16 script_timing,
   ant_tuner_device_tokens_type *master_token,
   boolean is_tx_retune
)
{  
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {      
      if( is_tx_retune == TRUE )
      {
        /* Tuner Disable TX sequence */
        result &= rfcommon_atuner_manager_tx_disable( atuner_device,
                                                      mode,
                                                      band,
                                                      chan_num,
                                                      dev_action,
                                                      script_buffer,
                                                      script_timing,
                                                      &this->tuner_sub_tokens[i].token );
      }
        /* Put ATUNER to sleep */
      result &= rfcommon_atuner_manager_disable( atuner_device,
                                                 dev_action,
                                                 script_buffer,
                                                 script_timing,
                                                 &this->tuner_sub_tokens[i].token );

      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::retune_disable: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;

}/* rf_cdma_atuner::retune_disable */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device Rx configuration. Rx config 
  is executed on all valid atuner devices in the wrapper object
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner settings

  @param dev_action
  Script execution type
 
  @retval TRUE
  If atuner device config rx was successful
 
  @retval FALSE
  False if device config rx failed
*/
boolean
rf_cdma_atuner::configure_for_rx
(
  rfcom_antenna_tuner_priority_cfg priority_cfg,
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing,
  ant_tuner_device_tokens_type *master_token
)
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;

  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Configure ATUNER for Rx */
      result &= rfcommon_atuner_manager_set_rx_mode( priority_cfg, atuner_device, 
                                              mode, band, chan_num ,dev_action, 
                                              script_buffer, script_timing,
                                              &this->tuner_sub_tokens[i].token );

      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::device_set_rx_mode: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device Tx configuration. Tx config 
  is executed on all valid atuner devices in the wrapper object
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuner
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param script_token
  Token for tuner device

  @param tuner_cl_nv_ptr
  Tuner CL NV data structure  

  @retval TRUE
  If atuner device set Tx mode was successful
 
  @retval FALSE
  False if device set Tx mode failed
*/
boolean
rf_cdma_atuner::configure_for_tx
( 
  lm_handle_type txlm_handle,
  rf_cdma_atuner *atuner_ptr, 
  rfm_mode_enum_type mode, 
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action, 
  rf_buffer_intf *script_buffer, 
  int16 script_timing, 
  ant_tuner_device_tokens_type *master_token,
  rfcommon_nv_ant_tuner_cl_ctrl_type *tuner_cl_nv_ptr
)
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  rfcommon_atuner_cl_ctrl_info_type cl_info;

  cl_info.cl_nv_ctrl_info = tuner_cl_nv_ptr;
  cl_info.lm_handle = txlm_handle ; 

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Config ATUNER for Tx*/
      result &= rfcommon_atuner_manager_set_tx_mode( atuner_device, 
                                              mode, band, chan_num ,dev_action, 
                                              script_buffer, script_timing,
                                              &this->tuner_sub_tokens[i].token,
                                              &cl_info );

      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::device_set_tx_mode: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( atuner_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_atuner::device_set_tx_mode: "
                        "ATUNER is null" );
    }/*if ( atuner_device == NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device Retune configuration.
  Retune config is executed on all valid atuner devices in the wrapper object
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuner
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param dev_action
  Immediate or scripting mode (Script execution type)
 
  @param script_buffer
  Script buffer pointer, which must be populated with the required atuner
  settings
 
  @param script_timing
  timing info for the script
 
  @param script_token
  Token for tuner device

  @param tuner_cl_nv_ptr
  Tuner CL NV data structure  

  @retval TRUE
  If atuner device set Tx mode was successful
 
  @retval FALSE
  False if device set Tx mode failed
*/

boolean
rf_cdma_atuner::configure_for_retune
( 
  rfcom_antenna_tuner_priority_cfg priority_cfg,
  rf_cdma_atuner *atuner_ptr, 
  rfm_mode_enum_type mode, 
  rfcom_band_type_u band,
  uint16 chan_num,
  rf_device_execution_type dev_action, 
  rf_buffer_intf *script_buffer, 
  int16 script_timing, 
  ant_tuner_device_tokens_type *master_token,
  rfcommon_atuner_cl_ctrl_info_type *tuner_cl_nv_ptr,
  boolean is_tx_retune
)
{  
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  /* Set the master token sent from MC layer to FALSE by default
  This will be updated if any of the tuners have valid script  */
  master_token->token_valid = FALSE;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {      
      /* Wakeup ATUNER */    
      result &= rfcommon_atuner_manager_init( atuner_device,
                                              mode,
                                              dev_action,
                                              script_buffer,                                             
                                              script_timing,
                                              &this->tuner_sub_tokens[i].token ); 

      /* Configure ATUNER for Rx */
      result &= rfcommon_atuner_manager_set_rx_mode( priority_cfg,
                                                     atuner_device, 
                                                     mode, 
                                                     band, 
                                                     chan_num,
                                                     dev_action,
                                                     script_buffer, 
                                                     script_timing,
                                                     &this->tuner_sub_tokens[i].token );


      if( is_tx_retune == TRUE )
      {
        /* Config ATUNER for Tx*/
        result &= rfcommon_atuner_manager_set_tx_mode( atuner_device, 
                                                       mode, 
                                                       band, 
                                                       chan_num,
                                                       dev_action,
                                                       script_buffer, 
                                                       script_timing,
                                                       &this->tuner_sub_tokens[i].token,
                                                       tuner_cl_nv_ptr );
      }

      if ( result == TRUE )
      {
        this->tuner_sub_tokens[i].token_valid = TRUE;
        /* Set the master token sent from MC layer to TRUE , since
        there is a valid script in atleast one of the tuners*/
        master_token->token_valid = TRUE;
      }
      else
      {
        this->tuner_sub_tokens[i].token_valid = FALSE;
        RF_MSG_1( RF_ERROR, "rf_cdma_atuner::configure for retune: Tuner driver "
                            " failure for tuner idx: %d ", i);
      }
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;

}/* rf_cdma_atuner::configure_for_retune */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for dynamically re-tuning atuner by updating tune
  code. This action is executed on all valid atuner devices in the wrapper object
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param is_this_rx_operation
  TRUE: API is called in Rx context/ FALSE : API is not in Rx context
 
  @retval TRUE
  If atuner update tune code was successful
 
  @retval FALSE
  False if atuner update tune code failed
*/
boolean
rf_cdma_atuner::update_tune_code
( 
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation,
  void* tuner_cb_data,
  uint16 tuner_mdsp_chain,
  rf_device_execution_type dev_action,
  rf_buffer_intf *script_buffer,
  int16 script_timing 
) 
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;
  /* SCRIPT TOKEN */
  script_token_type* script_token = NULL;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Use index Zero for getting a pointer to the tuner manager for the 
    callback data, this is a limitation on part of the tuner driver, because 
    the response handler only supports one tuner per chain, use the token for 
    that tuner for the callback*/ 
    if ( i == 0 )
    {
      rfcommon_tuner_update_cb_data_type* cb_data = 
        (rfcommon_tuner_update_cb_data_type*) tuner_cb_data ;  
      /* 1. Assign tuner device pointer to call back data */
      cb_data->tuner_data[tuner_mdsp_chain].ant_tuner =  atuner_device;

      /* 2. Obtain address of token and ccs event handle from call back data*/
      script_token = &(cb_data->tuner_data[tuner_mdsp_chain].
                                                                   tuner_token);
    }
    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* 3. Config ATUNER for Tx*/
      result &= rfcommon_atuner_manager_update_tune_code( atuner_device, 
                                              mode, 
                                              band, 
                                              chan_num ,
                                              is_this_rx_operation, dev_action,
                                              script_buffer, script_timing,
                                              script_token );
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for dynamically re-tuning atuner by updating tune
  code in ftm mode. This action is executed on all valid atuner devices in the
  wrapper object
 
  @param atuner_ptr
  Tuner module pointer, which may contain one or more atuners
 
  @param mode
  API calling Tech
 
  @param band
  Band type
 
  @param chan_num
  Channel
 
  @param is_this_rx_operation
  TRUE: API is called in Rx context/ FALSE : API is not in Rx context
 
  @retval TRUE
  If atuner update tune code was successful
 
  @retval FALSE
  False if atuner update tune code failed
*/
boolean
rf_cdma_atuner::update_ftm_tune_code
( 
  rf_cdma_atuner *atuner_ptr,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  uint16 chan_num,
  boolean is_this_rx_operation
) 
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;
  /* Temporary Dummy token */
  script_token_type dummy_script_token = 0;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* 3. Call Tuner force_detune API */
      result &= rfcommon_atuner_manager_force_detune( atuner_device, 
                                              mode, 
                                              band, 
                                              chan_num,
                                              is_this_rx_operation,
                                              RFDEVICE_EXECUTE_IMMEDIATE, 
                                              NULL, 
                                              0,
                                              &dummy_script_token );
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device Tx configuration. Tx config 
  is executed on all valid atuner devices in the wrapper object
 
  @param rf_script
  Script buffer pointer, which must be populated with the required atuner settings

  @param dev_action
  Script execution type
 
  @retval TRUE
  If atuner device config tx was successful
 
  @retval FALSE
  False if device config tx failed
*/
boolean
rf_cdma_atuner::ack_script
( 
  rf_cdma_atuner *atuner_ptr, 
  ant_tuner_device_tokens_type *master_token 
)
{
  boolean result = TRUE;
  /* ATUNER device iterator */
  uint8 i;
  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  update_tuner_object_list();

  for ( i = 0; i < this->updated_tuner_list_length; i++  )
  {
    /*Retrieve device object*/
    atuner_device = this->updated_tuner_list[i];

    /* Perform NULL pointer check */
    if ( atuner_device != NULL )
    {
      /* Proceed only if master token is true*/
      if ( master_token->token_valid == TRUE )
      {
        /* Config ATUNER for Tx*/
        if ( this->tuner_sub_tokens[i].token_valid == TRUE )
        {
          result &= rfcommon_atuner_manager_acknowledge_script( atuner_device, 
                                      this->tuner_sub_tokens[i].token ); 
        }

        if ( result == FALSE )
        {
          RF_MSG_1( RF_ERROR, "rf_cdma_atuner::device_ack_script: Tuner driver "
                              " failure for tuner idx: %d ", i);
        }
      }/*  if ( script_token->token_valid == TRUE )*/
      else
      {
        RF_MSG( RF_ERROR, "rf_cdma_atuner::device_ack_script:Token valid"
                            " is FALSE "); 
      }/*  if ! ( script_token->token_valid == TRUE )*/
    }/*if ( atuner_device != NULL )*/
  }/*for ( i = 0; i < this->num_atuner_instances; i++  )*/

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to override the current tuner setting in Tuner hardware

  @details
  rf_cdma_atuner::tune_code_override

  @param atuner_ptr 
  Pointer to cdma antenna tuner device 
  
  @param override_flag
  Tuner override flag: Any value greater than '1' indicates the current tuner settings 
  can be overriden.

  @param data
  Pointer to the tuner tunecode config structure 

  @param tuner_nv_ptr
  Pointer to the NV data structure

  @param tuner_id_ptr   
  Pointer to the tuner_id
 
  @return
  rfdevice_antenna_tuner_device_override_status_type 
*/
rfdevice_antenna_tuner_device_override_status_type
rf_cdma_atuner::tune_code_override
(
  rf_cdma_atuner *atuner_ptr,
  uint8  override_flag,
  void*  data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
  
)
{
  rfdevice_antenna_tuner_device_override_status_type result = RFDEVICE_ANTENNA_TUNER_ERROR;

  /* Pointer to device class to hold common object */
  rfcommon_atuner_intf *atuner_device = NULL;

  update_tuner_object_list();

  if ( this->updated_tuner_list != NULL )
  {
  /*Retrieve device object. Using 0 index as support is only for 1 tuner per chain */
  atuner_device = this->updated_tuner_list[0];
  }
  else
  {
    atuner_device = NULL;
  }

  /* Perform NULL pointer check */
  if ( atuner_device != NULL )
  {
    /* 3. Config ATUNER for override value */
    result = rfcommon_atuner_tune_code_override( atuner_device,
                                                 override_flag,
                                                 data,
                                                 tuner_nv_ptr,
                                                 tuner_id_ptr);
  }/*if ( atuner_device != NULL )*/
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_atuner::tune_code_override: "
                      "ATUNER is null" );
  }/*if ( atuner_device == NULL )*/

  return result;
}

void
rf_cdma_atuner::update_tuner_object_list()
{
  #ifdef FEATURE_RF_ASDIV
  if( this->tuner_asdiv_enabled )
  {
    rfcommon_atuner_intf** asd_tuner_list = NULL;
    rfcommon_asdiv_tuner_manager *asd_tuner_mgr = NULL;

    rfc_intf *rfc_cmn = rfc_intf::get_instance();

    asd_tuner_mgr = (rfcommon_asdiv_tuner_manager*)rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0); 

    if ( asd_tuner_mgr != NULL )
    {
      asd_tuner_list = (rfcommon_atuner_intf **)( 
                               asd_tuner_mgr->get_asdiv_device_list( 
                                             this->default_antenna, 
                                             RFDEVICE_TUNER_MANAGER, 
                                             (rfdevice_class **)(this->default_tuner_list) 
                                             ) ); 
    }

    this->updated_tuner_list = asd_tuner_list;

    if ( this->updated_tuner_list != NULL )
    {
    this->updated_tuner_list_length = RFC_ASDIV_MAX_DEVICES_PER_ANT_PATH;
    RF_MSG_4(RF_MED, "rf_cdma_atuner tuner list updated, "
                     "tuner_1: 0x%08x tuner_2:0x%08x tuner_3:0x%08x, "
                     "list_length %d",
                     this->updated_tuner_list[0], 
                     this->updated_tuner_list[1], 
                     this->updated_tuner_list[2],
                     this->updated_tuner_list_length );
  }
  else
    {
      this->updated_tuner_list_length = 0;
      RF_MSG(RF_ERROR, "rf_cdma_atuner RFC returned NULL list ptr" ); 
    }

  }
  else
  #endif
  {
    this->updated_tuner_list = this->default_tuner_list;
    this->updated_tuner_list_length = this->default_atuner_num;

    RF_MSG_2(RF_MED, "rf_cdma_atuner tuner list updated to default, "
                     "Tuner_list: 0x%08x, list_length: %d",
                     this->updated_tuner_list, 
                     this->updated_tuner_list_length );

  }

  return;
}

#ifdef FEATURE_RF_ASDIV
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for atuner device closeloop callback 
  registration which is executed on all valid atuner devices 
  in the wrapper object 
   
  @param device
  RFM device on which the antenna switch is to be initiated
 
  @param rfm_mode
  RFM mode active on the current device
 
  @param asdiv_obj
  Pointer to the oject containing ASDiv
 
  @param antenna_position
  New antenna position
 
  @param script_ptr
  Pointer to the buffer allocated by Tech to store the script for antenna
  switch.
 
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
rf_cdma_atuner::asdiv_config_switch
(
  rfm_device_enum_type device,
  rfm_mode_enum_type rfm_mode,
  rfcommon_asdiv_position_type antenna_position,
  rf_buffer_intf *script_ptr,
  rfcommon_asdiv_asm_conifg_type asm_script_cfg,
  rfcommon_asdiv_send_script_type send_script_cfg,
  rfcommon_asdiv_switch_start_notify_type switch_start_cfg,
  rfcommon_asdiv_update_tuner_type update_tuner_cb,
  rfm_l1_cb_type l1_cb_func,
  void *l1_cb_data,
  boolean gen_script_only
)
{

  boolean result = TRUE;
  /* AsDiv Rxlm handle ID, for C2K, it is PRx, dev0*/
  uint32 asd_rxlm_handle;
  /* Pointer to dev0 */
  const rf_cdma_data_status_type *dev_asd;
  rf_cdma_asdiv_xsw_script_data *asm_script_data;
  boolean is_immediate;

  /* Get the ASM script data */
  asm_script_data = (rf_cdma_asdiv_xsw_script_data*)asm_script_cfg.cb_data;

  /* NULL pointer check */
  if ( asm_script_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_atuner::asdiv_config_switch:"
		              "NULL callback data" );
    return FALSE;
  } /* if ( asm_script_data == NULL ) */

  is_immediate = asm_script_data->immediate ;

  /* Get Device Status */
  dev_asd = rf_cdma_get_device_status(device);

  /* NULL Pointer check */
  if ( dev_asd == NULL )
  {
    RF_MSG_1( RF_ERROR, "asdiv_config_switch: Dev %d - NULL "
                        "device or RFC data", device );
    return FALSE;
  } /* if ( dev_asd == NULL ) */

  /* Check for RF mode */
  if( dev_asd->rf_mode != RFM_1X_MODE && dev_asd->rf_mode != RFM_1XEVDO_MODE )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_mc_set_antenna_to_position: invalid mode %d", 
                        dev_asd->rf_mode );
    return FALSE; 
  }
  
  /* Get AsD handle*/
  asd_rxlm_handle = (uint32)dev_asd->rxlm_handle;
  
  /* pointer for tuner manager. However the code will still do a loop for possible
  extension in the future.*/

  /*Retrieve device object*/
  RF_MSG_1( RF_MED, "rf_cdma_atuner::asdiv_config_switch: "
                    "antenna position %d",
                     antenna_position );

  result &= rfcommon_asdiv_manager_config_switch( device,
                                                  rfm_mode,
                                                  antenna_position, 
                                                  script_ptr, 
                                                  script_ptr, 
                                                  asm_script_cfg,
                                                  send_script_cfg,
                                                  switch_start_cfg,
                                                  update_tuner_cb,
                                                  l1_cb_func, 
                                                  l1_cb_data, 
                                                  gen_script_only, 
                                                  is_immediate , 0, asd_rxlm_handle ) ;

  return result;
}

#endif
/*! @} */

/*! @} */
#endif /* FEATURE_CDMA1X */
