/*!
  @file
  rf_cdma_asm.cpp

  @brief
  <b>This is the ASM interface exposed to the MC layer.</b> This module
  contains all CDMA-ASM-specific data and implementations.

  @addtogroup RF_RFC_CDMA_MC_INTF
  @{

*/

/*==============================================================================

  Copyright (c) 2008 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

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
  written permission of Qualcomm Technologies Incorporated.d.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_asm.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/28/15   rs      fix crash for FTM_GET_RFFE_DEVICE_INFO cmd
09/18/14   sn      Remove hard-coded rfm_device in all functions
06/03/14   zhw     Remove hard-coded mode & band in sleep/wakeup callflow
05/05/14   sn      Added ASM sleep wakeup callflow
10/02/13   APU     Split ASM logically into Rx and TX ASMs
07/12/13   spa     Update #def being used for max possible instances per band
07/08/13   yzw     Add method get_asm_device_info()
06/10/13   sty     Deleted unused method
01/15/13   cd      Add support for CDMA ASM aggregator functionality
11/06/12   spa     Removed references to unused asm_type and asm_bus_type
07/14/12   APU     Initial version

==============================================================================*/

#include "rf_cdma_asm.h"
#include "rfc_cdma_data.h"
#include "rfcommon_msg.h"
#include "rfdevice_asm_intf.h"

/*----------------------------------------------------------------------------*/
/* C interface functionality */
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device wakeup for RX

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device wakeup was successful

    @retval FALSE
    False if device wakeup failed
  */
  boolean
  rf_cdma_asm_device_wakeup_rx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band, 
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_wakeup_rx( mode, band, rf_script, dev_action ) );
  }


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device wakeup for TX

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device wakeup was successful

    @retval FALSE
    False if device wakeup failed
  */
  boolean
  rf_cdma_asm_device_wakeup_tx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band, 
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_wakeup_tx( mode, band, rf_script, dev_action ) );
  }
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device sleep for RX

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device sleep was successful

    @retval FALSE
    False if device sleep failed
  */
  boolean
  rf_cdma_asm_device_sleep_rx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band, 
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_sleep_rx( mode, band, rf_script, dev_action ) );
  }
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device sleep for TX

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device sleep was successful

    @retval FALSE
    False if device sleep failed
  */
  boolean
  rf_cdma_asm_device_sleep_tx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band, 
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_sleep_tx( mode, band, rf_script, dev_action ) );
  }
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device Rx enable

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param mode
    RFM Mode for which enable must be performed

    @param band
    CDMA Band class information

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device Rx enable was successful

    @retval FALSE
    False if device Rx enable failed
  */
  boolean
  rf_cdma_asm_device_enable_rx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band,
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_enable_rx( mode, band,
                                  rf_script, dev_action ) );
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device Tx enable

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device tx enable was successful

    @retval FALSE
    False if device tx enable failed
  */
  boolean
  rf_cdma_asm_device_enable_tx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band,
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_enable_tx( mode, band,
                                  rf_script, dev_action ) );
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device Rx disable

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param mode
    RFM Mode for which disable must be performed

    @param band
    CDMA Band class information

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device Rx disable was successful

    @retval FALSE
    False if device Rx disable failed
  */
  boolean
  rf_cdma_asm_device_disable_rx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band,
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_disable_rx( mode, band,
                                   rf_script, dev_action ) );
  }

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Interface functionality to MC layer for ASM device Tx disable

    @param sm
    Switch module pointer, which may contain one or more ASMs

    @param rf_script
    Script buffer pointer, which must be populated with the required ASM settings

    @param dev_action
    Script execution type

    @retval TRUE
    If ASM device tx disable was successful

    @retval FALSE
    False if device tx disable failed
  */
  boolean
  rf_cdma_asm_device_disable_tx
  (
    rf_cdma_asm* sm,
    rfm_mode_enum_type mode,
    rfm_cdma_band_class_type band,
    const rf_buffer_intf* rf_script,
    rf_device_execution_type dev_action
  )
  {
    /* Call wrapper interface */
    return( sm->device_disable_tx( mode, band,
                                   rf_script, dev_action ) );
  }
#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------*/
/* CDMA ASM class */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constructor - Initializes the parameters for the ASM wrapper object

  @param ssbi_script_size
  Size of SSBI script to be allocated inside buffer. 0 implies no SSBI script
  needed.

  @param rfm_device
  Device for which the ASM wrapper object is created
*/
rf_cdma_asm::rf_cdma_asm( int rfm_device )
{
  /* Data iterator */
  uint8 i;

  this->rfm_device = rfm_device;
  this->num_asm_instances[RX_ASM] = 0;
  this->num_asm_instances[TX_ASM] = 0;


  for ( i = 0; i < RFC_MAX_DEVICE_INSTANCES_PER_BAND; i++ )
  {
    this->misc_device_descriptor[RX_ASM][i] = NULL;
    this->misc_device_descriptor[TX_ASM][i] = NULL;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Add RF ASM device to this wrapper object

  @details
  This wrapper object stores pointers, upto a maximum allowed instance value,
  to RF ASM device objects. Duplicate objects are not included in the wrapper.

  @param new_device
  Pointer to ASM device which must be added to the wrapper object

  @retval TRUE
  If ASM device was added successfully

  @retval FALSE
  If adding a new device failed
*/
bool
rf_cdma_asm::add_device_instance( void* new_device , rf_cdma_asm_type asm_type )
{
  bool ret_val = TRUE;
  uint8 num_asms = this->num_asm_instances[asm_type] ;
  uint8 i;
  bool device_already_exists = FALSE;

  if ( new_device != NULL )
  {
    /* Search if this wrapper object already contains the device
    to be added */
    for ( i = 0; i < num_asms; i++ )
    {
      if ( this->misc_device_descriptor[asm_type][i] == new_device )
      {
        /* Found a match */
        device_already_exists = TRUE;
        break;
      }
    }

    /* If a device is to be added, update the wrapper object */
    if ( num_asms < RFC_MAX_DEVICE_INSTANCES_PER_BAND &&
         device_already_exists == FALSE )
    {
      this->misc_device_descriptor[asm_type][num_asms] = new_device;
      this->num_asm_instances[asm_type]++;
      ret_val = TRUE;
    }
    else if ( num_asms >= RFC_MAX_DEVICE_INSTANCES_PER_BAND )
    {
      RF_MSG_1( RF_ERROR, "rf_cdma_asm::add_device_instance: Cannot "
                "add more than allowed ASM instances. "
                "Max allowed %d",
                RFC_MAX_DEVICE_INSTANCES_PER_BAND );
      ret_val = FALSE;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_asm::add_device_instance: Requesting "
            "to add NULL device" );
    ret_val = FALSE;
  }

  return ret_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates RF ASM device to the wrapper object created during
  boot.

  @details
  Updates RF ASM device to the wrapper object created during
  boot. When configuring the alt-paths, one might want to update
  the wrapper objects updated.. 

  @param updated_device
  Pointer to ASM device which must be updated in the wrapper
  object
 
  @param asm_type
  Rx or Tx ASM
 
  @retval
  TRUE If ASM device was added successfully
  FALSE If adding a new device failed
*/
bool
rf_cdma_asm::update_device_instance 
( 
   void* updated_device , 
   rf_cdma_asm_type asm_type ,
   uint8 asm_number
)
{
  bool rv = TRUE ; 
  if ( ( asm_number < RFC_MAX_DEVICE_INSTANCES_PER_BAND ) && 
        ( asm_type < MAX_CDMA_ASM ) )
  {
    misc_device_descriptor[asm_type][asm_number] = updated_device ;
  }
  else
  {
    rv = FALSE ;
  }
  return rv ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM RX device wakeup. Wakeup is executed on
  all valid RX ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device wakeup was successful

  @retval FALSE
  False if device wakeup failed
*/
bool
rf_cdma_asm::device_wakeup_rx( rfm_mode_enum_type mode,
                               rfm_cdma_band_class_type band, 
                               const rf_buffer_intf* rf_script,
                               rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;
  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[RX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[RX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_WAKEUP;

      /* Wakeup ASM */
      result &= rfdevice_asm_wakeup( &asm_cfg_s, asm_device, (void*)rf_script,
                                     dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_wakeup: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM TX device wakeup. Wakeup is executed on
  all valid TX ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device wakeup was successful

  @retval FALSE
  False if device wakeup failed
*/
bool
rf_cdma_asm::device_wakeup_tx( rfm_mode_enum_type mode,
                               rfm_cdma_band_class_type band, 
                               const rf_buffer_intf* rf_script,
                               rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;
  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[TX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[TX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_WAKEUP;

      /* Wakeup ASM */
      result &= rfdevice_asm_wakeup( &asm_cfg_s, asm_device, (void*)rf_script,
                                     dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_wakeup: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM RX device sleep. Sleep is executed on
  all valid RX ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device sleep was successful

  @retval FALSE
  False if device sleep failed
*/
bool
rf_cdma_asm::device_sleep_rx( rfm_mode_enum_type mode,
                              rfm_cdma_band_class_type band, 
                              const rf_buffer_intf* rf_script,
                              rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;
  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[RX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[RX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_SLEEP;

      /* Put ASM to sleep */
      result &= rfdevice_asm_sleep( &asm_cfg_s, asm_device, (void*)rf_script,
                                    dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_sleep: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM TX device sleep. Sleep is executed on
  all valid TX ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device sleep was successful

  @retval FALSE
  False if device sleep failed
*/
bool
rf_cdma_asm::device_sleep_tx( rfm_mode_enum_type mode,
                              rfm_cdma_band_class_type band, 
                              const rf_buffer_intf* rf_script,
                              rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;
  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[TX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[TX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_SLEEP;

      /* Put ASM to sleep */
      result &= rfdevice_asm_sleep( &asm_cfg_s, asm_device, (void*)rf_script,
                                    dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_sleep: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM device Rx enable. Rx enable
  is executed on all valid ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device enable rx was successful

  @retval FALSE
  False if device enable rx failed
*/
bool
rf_cdma_asm::device_enable_rx( rfm_mode_enum_type mode,
                               rfm_cdma_band_class_type band,
                               const rf_buffer_intf* rf_script,
                               rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;

  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[RX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[RX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ENABLE_RX;

      /* Configure ASM for Rx */
      result &= rfdevice_asm_enable( &asm_cfg_s, asm_device, (void*)rf_script,
                                     dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_enable_rx: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM device Tx enable. Tx enable
  is executed on all valid ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device enable tx was successful

  @retval FALSE
  False if device enable tx failed
*/
bool
rf_cdma_asm::device_enable_tx( rfm_mode_enum_type mode,
                               rfm_cdma_band_class_type band,
                               const rf_buffer_intf* rf_script,
                               rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;

  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[TX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[TX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {

      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_ENABLE_TX;

      /* Config ASM for Tx*/
      result &= rfdevice_asm_enable( &asm_cfg_s, asm_device,
                                     (void*)rf_script,
                                     dev_action,
                                     RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_enable_tx: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM device Rx enable. Rx disable
  is executed on all valid ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device disable rx was successful

  @retval FALSE
  False if device disable rx failed
*/
bool
rf_cdma_asm::device_disable_rx( rfm_mode_enum_type mode,
                                rfm_cdma_band_class_type band,
                                const rf_buffer_intf* rf_script,
                                rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;

  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[RX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[RX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_RX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_DISABLE_RX;

      /* Configure ASM for Rx */
      result &= rfdevice_asm_disable( &asm_cfg_s, asm_device, (void*)rf_script,
                                      dev_action, RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_disable_rx: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper object implementation for ASM device Tx disable. Tx disable
  is executed on all valid ASM devices in the wrapper object

  @param rf_script
  Script buffer pointer, which must be populated with the required ASM settings

  @param dev_action
  Script execution type

  @retval TRUE
  If ASM device disable tx was successful

  @retval FALSE
  False if device disable tx failed
*/
bool
rf_cdma_asm::device_disable_tx( rfm_mode_enum_type mode,
                                rfm_cdma_band_class_type band,
                                const rf_buffer_intf* rf_script,
                                rf_device_execution_type dev_action )
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;

  rfdevice_asm_cfg_type asm_cfg_s;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  for ( i = 0; i < this->num_asm_instances[TX_ASM]; i++  )
  {
    /*Retrieve device object*/
    asm_device = this->misc_device_descriptor[TX_ASM][i];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {

      asm_cfg_s.rfm_device = (rfm_device_enum_type)this->rfm_device;
      asm_cfg_s.mode = mode;
      asm_cfg_s.band.cdma_band = band;
      asm_cfg_s.asm_rx_tx = RFDEVICE_ASM_TX_0;
      asm_cfg_s.asm_cell = RFDEVICE_ASM_CELL_0;
      asm_cfg_s.trigger_type = RFDEVICE_ASM_TRIGGER_DISABLE_TX;

      /* Config ASM for Tx*/
      result &= rfdevice_asm_disable( &asm_cfg_s, asm_device,
                                     (void*)rf_script,
                                     dev_action,
                                     RFCOM_INVALID_TIMING_OFFSET );
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::device_disable_tx: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get ASM device ID information

   @params rffe_device_type
  RFFE device type

  @params mfg_id
  Passing in parameter to store manufacture id

  @params prd_id
  Passing in parameter to store product id

  @params rev_id
  Passing in parameter to store revision id

  @retval TRUE
  If get ASM device IDs was successful

  @retval FALSE
  False if get ASM device IDs failed

*/
bool rf_cdma_asm::get_asm_device_info
( 	uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id
)
{
  boolean result = TRUE;
  /* ASM device iterator */
  uint8 i;

  /* Pointer to device class to hold common object */
  void *asm_device = NULL;

  /* This can be Rx or Tx but we shouldnt be even cyclying to this API needs to be fixed. */
  for ( i = 0; i < this->num_asm_instances[RX_ASM]; i++  )
  {
    /*Retrieve device object*/
	asm_device = this->misc_device_descriptor[i][0];

    /* Perform NULL pointer check */
    if ( asm_device != NULL )
    {
      /* Config ASM for Tx*/
      result &= rfdevice_asm_get_device_info( asm_device, &mfg_id[i], &prd_id[i], &rev_id[i]);
      break ;
    }/*if ( asm_device != NULL )*/
    else
    {
      RF_MSG( RF_ERROR, "rf_cdma_asm::get_asm_device_info: "
              "ASM is null" );
    }/*if ( asm_device == NULL )*/
  }

  return result;
}



/*! @} */

