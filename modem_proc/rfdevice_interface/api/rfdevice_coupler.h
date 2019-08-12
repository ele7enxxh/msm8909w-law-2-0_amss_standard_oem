#ifndef RFDEVICE_COUPLER_H
#define RFDEVICE_COUPLER_H
/*!
   @file
   rfdevice_coupler.h

   @brief

*/

/*===========================================================================

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_coupler.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:12 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
01/07/14   ndb   Remove the Pure Virtual function API's (Make them as only Virtual functions)
04/08/14   yb    Coupler driver integration with RFC and AG files
                 Added tech band information to config API
04/07/14   yb    Support for Coupler RFC changes
03/31/14   yb    Fixed compilation error
03/17/14   vv    Compile fix
08/22/12   aca   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"
#include "rfdevice_cmn_type_defs.h"

/*==========================================================================*/
/* DEFINES and ENUMS  */
/*==========================================================================*/

/*!
  @brief
  Modes of coupler operation

  @details
  OPEN indicates open position
  FWD indicates forward coupling used to route the incident power (for FbRx) 
  REV indicates reverse coupling used to route the reflected power (for Tuning)
  INTERNAL indicates set towards tuner internal pids/tdet
*/
typedef enum
{
  RFDEVICE_COUPLER_DIRECTION_OPEN = 1,
  RFDEVICE_COUPLER_DIRECTION_FWD,
  RFDEVICE_COUPLER_DIRECTION_REV,
  RFDEVICE_COUPLER_DIRECTION_INTERNAL,
  RFDEVICE_COUPLER_DIRECTION_INVALID = 0xFF
} rfdevice_coupling_direction_type;

/*! @brief Coupler Attenuation type
  0_DB -  0  dB Attenuation
  3_DB -  3  dB Attenuation
  6_DB -  6  dB Attenuation
  9_DB -  9  dB Attenuation
  12_DB - 12 dB Attenuation
  15_DB - 15 dB Attenuation
*/
typedef enum
{
RFDEVICE_COUPLER_ATTENUATION_DEFAULT = 1,
RFDEVICE_COUPLER_ATTENUATION_0_DB,
RFDEVICE_COUPLER_ATTENUATION_3_DB,
RFDEVICE_COUPLER_ATTENUATION_6_DB,
RFDEVICE_COUPLER_ATTENUATION_9_DB,
RFDEVICE_COUPLER_ATTENUATION_12_DB,
RFDEVICE_COUPLER_ATTENUATION_15_DB,
RFDEVICE_COUPLER_ATTENUATION_MAX,
RFDEVICE_COUPLER_ATTENUATION_INVALID = 0xFF
}rfdevice_coupler_attenuation_type;


/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

/*! @brief Structure containing tech specific configuration for coupler

*/
typedef struct
{
  /* enum indicating the radio access technology*/
  rfcom_mode_enum_type mode;
  /* enum indicating the band number */
  rfcom_band_type_u band;
  /* Enum indicating the required state of the coupler */
  rfdevice_coupling_direction_type direction;
  /* Coupler port for corresponding antenna */
  uint8 port;
} rfdevice_coupler_tech_cfg_type;


#ifdef __cplusplus

/* Coupler interface class */
class rfdevice_coupler:public rfdevice_class
{
public:
 rfdevice_coupler();

 virtual ~rfdevice_coupler();
/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function programming the coupler in the desired state

  @param tech_cfg
  Structure containing tech specific configuration for coupler

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts  

  @param script_timing
  timing offset for CCS scripts   

  @return
  boolean indicating pass or fail  
*/	
  virtual boolean config_coupler
  (
    rfdevice_coupler_tech_cfg_type tech_cfg,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for coupler init

  @details
  API used to config the coupler for use

  @param rfc_data
  ASIC data capturing port info and default functionality provided by RFC

  @return
  boolean indicating pass or fail   
*/
  virtual boolean init 
  (
    int32* rfc_data
  );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for coupler sleep

  @details
  API used to place the chip in low power state
  
  @param rfc_data
  ASIC data capturing port info and default functionality provided by RFC
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts   
  
  @param script_timing
  timing offset for CCS scripts     

  @return
  boolean indicating pass or fail  
*/
  virtual boolean sleep
  (
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  ) ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for coupler wakeup

  @details
  API used to wake up the chip from low power state
  
  @param rfc_data
  ASIC data capturing port info and default functionality provided by RFC

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts   

  @param script_timing
  timing offset for CCS scripts     

  @return
  boolean indicating pass or fail  
*/
  virtual boolean wakeup
  (
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  ) ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function for coupler trigger, to latch the pending settings.

  @details
  trigger script
  
  @param rfc_data
  ASIC data capturing port info and default functionality provided by RFC

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
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface function to set band config mapping per tech per band

  @details
  To set band config mapping from rfc to driver

  @param mode
  enum indicating the radio access technology

  @param band
  enum indicating the band number

  @param data_ptr
  ASIC data capturing default functionality provided by RFC

  @return
  boolean indicating pass or fail  
*/
virtual boolean set_band_config_map
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  int32 *data_ptr
);

};

#endif

#endif /*RFDEVICE_COUPLER_H*/
