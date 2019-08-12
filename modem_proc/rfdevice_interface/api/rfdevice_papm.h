#ifndef RFDEVICE_PAPM_H
#define RFDEVICE_PAPM_H
/*!
   @file
   rfdevice_papm.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_papm.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
10/23/14   vv    Added QFE3100 support
10/21/14   px    Added RFM Device support
06/13/13   vbh   Added explicit trigger for the PAPM device
03/23/13   sr    changes to support script timing.
03/22/13   sr    changes to make the interfaces consistent with other devices.
03/07/13   sr    changed the "pwr_tracker" name to "papm".
01/17/13   shb   Added RFDEVICE_STANDBY_MODE support
08/25/12   shb   Featurize class defn with __cplusplus
08/23/12   shb   Moved rfdevice_papm_mode_type to 
                 rfdevice_interface/api/rfdevice_papm.h 
07/26/12   bn    Initial Release
============================================================================*/




/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfdevice_cmn_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#define SET_MODE_SCRIPT_SIZE 5
#define RFDEVICE_PAPM_INVALID_ET_VOLTAGE 0

typedef enum
{
  RFDEVICE_BYPASS_MODE,
  RFDEVICE_APT_MODE,
  RFDEVICE_ET_MODE,
  RFDEVICE_MAX_MODE,
  RFDEVICE_NO_PT_MODE,
  RFDEVICE_STANDBY_MODE
} rfdevice_papm_mode_type;

typedef enum
{
  RFDEVICE_PAPM_BW_NOT_APPLICABLE = 0,
  RFDEVICE_PAPM_BW_5MHZ = 0,
  RFDEVICE_PAPM_BW_SC = 0,
  RFDEVICE_PAPM_BW_MCDO_1X = 0,
  RFDEVICE_PAPM_BW_10MHZ = 1,
  RFDEVICE_PAPM_BW_DC = 1,
  RFDEVICE_PAPM_BW_MCDO_3X = 1,
  RFDEVICE_PAPM_BW_15MHZ = 2,
  RFDEVICE_PAPM_BW_MCDO_5X = 2,
  RFDEVICE_PAPM_BW_20MHZ = 3,
  RFDEVICE_PAPM_BW_MCDO_7X = 3,
  RFDEVICE_PAPM_BW_INVALID,
} rfdevice_papm_bw_enum_type;

typedef struct
{
  
  uint16 ET_Vmin;  /* ET voltage minimum*/
  uint16 ET_Vmax;  /* ET voltage maximum*/  

}rfdevice_papm_misc_info;

#ifdef __cplusplus
class rfdevice_papm:public rfdevice_class
{

public:

rfdevice_papm();

virtual ~rfdevice_papm();


/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for papm enable

  @details
  rfdevice_papm_enable()

  @param tech
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number  

  @param buff_obj_ptr
  void pointer to CCS buffer object
 
  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event

  @return
  boolean indicating pass or fail  
*/	
  virtual boolean enable
  (
    rfm_device_enum_type rfm_device, 
    rfcom_mode_enum_type tech, 
    rfcom_band_type_u band, 
    rfdevice_papm_bw_enum_type bw, 
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type execution_type, 
    int16 script_timing = 0
  ) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for papm tx config

  @details
  rfdevice_papm_config_tx()

  @param tech
  enum indicating the Radio Access Technology

  @param buff_obj_ptr
  void pointer to CCS buffer object
 
  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event

  @return
  boolean indicating pass or fail  
*/
  virtual boolean disable
  (
    rfm_device_enum_type rfm_device, 
    rfcom_mode_enum_type tech, 
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type exec_type, 
    int16 script_timing = 0
  ) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function to set mode bias

  @details

  @param tech
  enum indicating the Radio Access Technology

  @param xpt_mode
  enum indicating xpt mode

  @param bias
  bias value that needs to be programmed

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param execution_type
  boolean to indicate write to hw or prepare script

  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event
 
  @return
  boolean indicating pass or fail  
*/	
  virtual boolean set_mode_bias
  (
    rfm_device_enum_type rfm_device, 
    rfcom_mode_enum_type tech, 
    rfcom_band_type_u band, 
    rfdevice_papm_mode_type xpt_mode, 
    uint16 bias, 
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type execution_type, 
    int16 script_timing = 0
  ) =0;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function to get xpt mode settings for give bias

  @details

  @param device_ptr
  void pointer that contains the papm device ptr 
  
  @param tech
  enum indicating the Radio Access Technology

  @param xpt mode
  enum indicating xpt mode

  @param bias
  bias value

  @param *settings
  register settings for bias value

  @return
  boolean indicating pass or fail  
*/

  virtual boolean xpt_linearizer_settings
  (
    rfm_device_enum_type rfm_device, 
    rfcom_mode_enum_type tech, 
    rfdevice_papm_mode_type xpt_mode, 
    uint16 bias, uint8 *settings
  ) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  populates the band map information from rfc

  @details
  rfdevice_papm_set_band_map

  @param mode
  enum indicating the Radio Access Technology

  @param band
  enum indicating band number  

  @param *data_ptr
  band mapping information passed as int32 pointer
*/
  virtual boolean set_band_map
  (
    rfm_device_enum_type rfm_device, 
    rfcom_mode_enum_type mode, 
    rfcom_band_type_u band, 
    int32 *data_ptr, uint8 size
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  provide the ET Vmin and Vmax voltage for the QPOET

  @details
  provide the ET Vmin and Vmax voltage for the QPOET.
 
*/
  virtual boolean get_et_voltage_min_max(rfdevice_papm_misc_info* et_voltage_min_max) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for asm trigger, to latch the pending settings.

  @details
  rfdevice_asm_trigger()

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts  
    
  @param script_timing
  time offset for the script where a negative value refers to the time BEFORE the event

  @return
  boolean indicating pass or fail  
*/
  virtual boolean trigger
  (
    rf_buffer_intf *buff_obj_ptr, 
    rfm_device_enum_type rfm_device,
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  virtual boolean get_max_bw_supported
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    uint32 *max_bw_khz
  );
	
};

#endif /* #ifdef __cplusplus */
#endif