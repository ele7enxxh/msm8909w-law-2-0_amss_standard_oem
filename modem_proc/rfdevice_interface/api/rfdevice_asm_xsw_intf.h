#ifndef RFDEVICE_ASM_XSW_INTF_H
#define RFDEVICE_ASM_XSW_INTF_H
/*!
   @file
   rfdevice_asm_xsw_intf.h

   @brief
   Declaration of the interface class for the cross switch

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_asm_xsw_intf.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:12 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
10/01/14   dbc   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rfdevice_logical_component.h"
#include "rfc_common.h"


typedef struct
{
  uint8 xsw_port;
  rfm_device_enum_type rfm_device;
  rfcom_mode_enum_type mode;
  rfcom_band_type_u band;
} asm_xsw_port_config_info_type;

class rfdevice_asm_xsw_intf
{
public:
 rfdevice_asm_xsw_intf();

 virtual ~rfdevice_asm_xsw_intf();

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for asm to get the cross switch position

  @param logical_device_cfg
  pointer to structure containing logical device info
  
  @param xsw_mode
  pointer used to return cross switch position
  
  @return
  boolean indicating pass or fail  
*/	
  virtual bool get_asm_xsw_config
  (
    rfc_logical_device_info_type *logical_device_cfg, uint8 *xsw_mode
  )  = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for xsw to save the cross switch position

  @param logical_device_cfg
  pointer to structure containing logical device info
  
  @param xsw_mode
  cross switch mode
  
  @return
  boolean indicating pass or fail  
*/  
  virtual bool save_asm_xsw_config
  (
    rfc_logical_device_info_type *logical_device_cfg, uint8 xsw_mode
  )  = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for asm to set the cross switch port to use

  @param logical_device_cfg
  pointer to structure containing logical device info
  
  @param xsw_port_info
  cross switch port info
  
  @return
  boolean indicating pass or fail  
*/  
  virtual bool set_xsw_port_config
  (
    rfc_logical_device_info_type *logical_device_cfg, 
    asm_xsw_port_config_info_type *xsw_port_info
  )  = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  interface function for asm to get the cross switch position

  @param logical_device_cfg
  pointer to structure containing logical device info
  
  @param xsw_port_info
  pointer used to return cross switch port info
  
  @return
  boolean indicating pass or fail  
*/	
  virtual bool get_xsw_port_config
  (
    rfc_logical_device_info_type *logical_device_cfg, 
    asm_xsw_port_config_info_type *xsw_port_info
  )  = 0;


};


#endif /* RFDEVICE_ASM_XSW_INTF_H */
