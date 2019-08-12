#ifndef QFE2520_PHYSICAL_DEVICE_AG_H
#define QFE2520_PHYSICAL_DEVICE_AG_H
/*!
  @file
  qfe2520_physical_device.h 

  @brief
  Software abstraction of a QFE2520 physical device.
*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2520/api/qfe2520_physical_device.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/17/14   adk     Removed device calibration code
08/20/14   vv      Added support to decouple qcoupler factory to chip specific drivers   
				   Clean-up unused constructor and APIs()
07/01/14   yb      Add support to make tuner transactions non-extended via rfc
05/22/14    vv     Added Overloaded APIs  create_tuner_object()  , create_coupler_object() &  create_hdet_object() 
05/12/14    vv     Physical Device support for Bolt 
04/04/14   ndb     Added TDET support
12/26/13   ndb     Initial version

===========================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rfdevice_antenna_tuner.h"
#include "rfdevice_qtuner_api.h"

#include "rfdevice_coupler.h"
//#include "rfdevice_qcoupler_api.h"

#include "rfc_common.h"
#include "rfdevice_comm_intf.h"

#include "rfdevice_physical_device.h"


#include "qfe2520_coupler_config_ag.h"

class qfe2520_physical_device : public rfdevice_physical_device
{
public:

  qfe2520_physical_device(rfc_phy_device_info_type* cfg,
  	                  boolean ext_cmd = TRUE);

  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

  qfe2520_physical_device* qfe2520_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  ~qfe2520_physical_device();

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);

private:
  /* Device config */
  rfc_device_cfg_info_type* cfg;

  /*comm_object*/
  rfdevice_comm_intf* comm_object;
  boolean extended_cmd;
  
  /* QTUNER */
  void create_tuner_object( rfc_logical_device_info_type *logical_device_info ); 

  /* QCOUPLER */
  void create_coupler_object( rfc_logical_device_info_type *logical_device_info ); 

  rfdevice_antenna_tuner* qfe2520_qtuner_obj_ptr;

  rfdevice_coupler* qfe2520_qcoupler_obj_ptr;

  
  #define QFE2520_QTUNER_INSTANCE 0

  #define QFE2520_QCOUPLER_INSTANCE 0
};
#endif