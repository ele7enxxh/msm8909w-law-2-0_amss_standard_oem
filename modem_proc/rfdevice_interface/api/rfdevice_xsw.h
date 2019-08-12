#ifndef RFDEVICE_XSW_H
#define RFDEVICE_XSW_H
/*!
   @file
   rfdevice_xsw.h

   @brief
   Declaration of the interface class for the cross switch

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_xsw.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:12 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
09/19/14   ndb     Remove the un-used interfaces init()
08/14/14   dbc   Add support for PRX_DRX_PATH_TX  
03/27/14   dbc   Created
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"
#include "rfdevice_cmn_type_defs.h"

/*!
  @brief
  Cross switch operation mode type

  @details
*/
typedef enum
{
  /* Possible cross switch modes */
  RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT,
  RFDEVICE_XSW_PRX_DRX_PATH_SWAP,
  RFDEVICE_XSW_PRX_ONLY_PATH_DEFAULT,
  RFDEVICE_XSW_PRX_ONLY_PATH_SWAP,
  RFDEVICE_XSW_SRS_ONLY_PATH_DEFAULT,
  RFDEVICE_XSW_SRS_ONLY_PATH_SWAP,
  RFDEVICE_XSW_PRX_DRX_PATH_TX_DEFAULT,
  RFDEVICE_XSW_PRX_DRX_PATH_TX_SWAP,
} rfdevice_xsw_modes_type;

/*!
  @brief
  Cross switch configuration type

  @details
*/
typedef struct 
{
   rfdevice_xsw_modes_type   xsmode;
   rfcom_band_type_u         band;
   rfcom_mode_enum_type      mode;
} rfdevice_xsw_config_type;


class rfdevice_xsw:public rfdevice_class
{
public:
 rfdevice_xsw();

 virtual ~rfdevice_xsw();
/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function programming the cross switch PRX and DRX paths to the desired state

  @param xsconfig
  structure containing configuration data
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts  

  @param script_timing
  timing offset for CCS scripts   
  
  @return
  boolean indicating pass or fail  
*/	
  virtual boolean set_cross_switch_config
  (
    rfdevice_xsw_config_type* xsconfig,
    rf_buffer_intf*           buff_obj_ptr, 
    rf_device_execution_type  dev_action, 
    int16 script_timing       = RFCOM_INVALID_TIMING_OFFSET
  )  = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for populating the XSW band/port map for the default cross
  switch position

  @param mode
  RF mode
  
  @param band
  RF Band

  @param data_ptr
  pointer to data containing the port information  

  @param size
  size of dat array pointed to by data_ptr   
  
  @return
  boolean indicating pass or fail  
*/	

  virtual boolean set_band_map_default
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for populating the XSW band/port map for the swap cross
  switch position

  @param mode
  RF mode
  
  @param band
  RF Band

  @param data_ptr
  pointer to data containing the port information  

  @param size
  size of dat array pointed to by data_ptr   
  
  @return
  boolean indicating pass or fail  
*/	
  virtual boolean set_band_map_swap
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  )= 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for populating the XSW band/port map for the SRS default cross
  switch position

  @param mode
  RF mode
  
  @param band
  RF Band

  @param data_ptr
  pointer to data containing the port information  

  @param size
  size of dat array pointed to by data_ptr   
  
  @return
  boolean indicating pass or fail  
*/	

  virtual boolean set_srs_band_map_default
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for populating the XSW band/port map for the SRS swap cross
  switch position

  @param mode
  RF mode
  
  @param band
  RF Band

  @param data_ptr
  pointer to data containing the port information  

  @param size
  size of dat array pointed to by data_ptr   
  
  @return
  boolean indicating pass or fail  
*/	
  virtual boolean set_srs_band_map_swap
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  )= 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function programming the cross switch in the desired state

  @param mode
  Enum indicating the current mode for the cross switch
    
  @return
  boolean indicating pass or fail  
*/	
  virtual boolean get_cross_switch_config
  (
     rfdevice_xsw_config_type* xsconfig
  );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for cross switch init

  @details
  API used to config the cross switch for use

  @param rfc_data
  ASIC data capturing port info and default functionality provided by RFC

  @return
  boolean indicating pass or fail   
*/
  virtual boolean init 
  (
    int32* rfc_data
  ) = 0;

/*!
  @brief
  Interface function for cross switch trigger, to latch the pending settings.

  @details
  trigger script
  
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
    rf_buffer_intf*           buff_obj_ptr, 
    rf_device_execution_type  dev_action, 
    int16 script_timing       = RFCOM_INVALID_TIMING_OFFSET
  );

};

#endif /*RFDEVICE_XSW_H*/
