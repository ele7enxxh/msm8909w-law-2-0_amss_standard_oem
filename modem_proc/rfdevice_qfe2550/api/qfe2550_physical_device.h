#ifndef QFE2550_PHYSICAL_DEVICE_AG_H
#define QFE2550_PHYSICAL_DEVICE_AG_H
/*
Generated using: qfe_physical_device_autogen.pl 
Generated from-  

	File: QFE2550_RFFE_Settings.xlsx 
	Released: 
	Author: 
	Revision: 
	Change Note: 

*/

/*=============================================================================

          RF DEVICE QFE2550 PHYSICAL DEVICE

GENERAL DESCRIPTION
  This file captures the configuration of QFE2550 CHIP.

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2550/api/qfe2550_physical_device.h#1 $
  $DateTime: 2016/12/13 07:59:31 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
10/17/14   adk   Removed device calibration code
08/21/14   vv    Clean-up old constructor and device creation APIs()
07/01/14   yb    Add support to make tuner transactions non-extended via rfc
05/22/14   vv    Added Overloaded APIs  create_tuner_object() 
05/12/14   vv    Physical Device support for Bolt  
04/09/14   ndb   Added Extended Cmd interface
03/25/14   yb    Initial File
============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rfdevice_antenna_tuner.h"
#include "rfdevice_qtuner_api.h"
#include "rfc_common.h"

#include "rfdevice_physical_device.h"


class qfe2550_physical_device : public rfdevice_physical_device
{
public:

  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

  qfe2550_physical_device* qfe2550_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  qfe2550_physical_device(rfc_phy_device_info_type* cfg_req,
  	                  boolean ext_cmd = TRUE );

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);

private:
  /* Device config */
  rfc_device_cfg_info_type* cfg;
  boolean extended_cmd;

  /* QTUNER */
  void create_tuner_object( rfc_logical_device_info_type *logical_device_info ); 

  rfdevice_antenna_tuner* qfe2550_qtuner_obj_ptr;
  #define QFE2550_QTUNER_INSTANCE 0

};
#endif