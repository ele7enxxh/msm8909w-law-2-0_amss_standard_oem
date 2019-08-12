#ifndef RFDEVICE_CLASS_H
#define RFDEVICE_CLASS_H
/*!
  @file rfdevice_class.h 

  @brief
  This is the rf device factory which creates all devices.



*/

/*===========================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_class.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
11/19/14   vv      rfdevice_reg_dump() API  for debug
09/19/14   ndb     Remove the un-used interfaces init()
09/22/14   rsr     Added support to initialize manufacture/product and revision ID of transceiver.
07/28/14   vv      Added mutex protection support for all logical devices
05/12/14   vv      Added rfdevice_rev_id_get() API
05/05/14   aks     Spur mitigation changes
02/06/14   vb      Added rfdevice_instance_num_get() API
12/30/13   vv      Adding a shim layer APIs to read/write device calibration state info in device driver space
10/31/13   shb     Converted TRX common device interface to C++
08/26/13   shb     Added rfdevice_id_get() and a default implementation for 
                   rfdevice_class::init() making it non-pure virtual
03/21/13  yzw     Add device driver support to query RFFE device information {mfg_id, prd_id, rev_id}
03/05/13   sr      updated self_test api parameter types.
03/05/13   sr      Added self_test support.
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
07/27/12   sr      Added methods to get the device_id and device_type.
07/11/12   vss     Add change to inherit rfa class 
05/24/12   vss     Initial revision.


===========================================================================*/

#ifdef __cplusplus
extern "C" {
#include "rfcommon_locks.h"
}
#endif

#include "rfdevice_logical_component_types.h"
#include "rfdevice_chip_ids.h"

#ifdef __cplusplus
#include "rfa.h"

#define INVALID_DEVICE_INFO_ID 0xFFFF

class rfdevice_class: public rfa
{
  protected:
    rfdevice_id_enum_type rf_device_id;
    rfdevice_type_enum_type rf_device_type;
    uint16 mfg_id; /*! RFFE Manafacturer ID */
    uint16 prd_id; /*! RFFE Product ID */
    uint16 rev_id; /*! Revision ID */
    static boolean device_cal_state; /*! Device calibration state info */
    uint32 instance_num; /*! Instance number */

  public: 
    rfdevice_class();
    rfdevice_class(uint16 mfg_id, uint16 prd_id, uint16 revision_id);
    rfdevice_class(rfdevice_type_enum_type dev_type, rfdevice_id_enum_type dev_id);
    rfdevice_id_enum_type rfdevice_id_get(void);
    rfdevice_type_enum_type rfdevice_type_get(void);
    uint32 rfdevice_instance_num_get(void);
    uint16 rfdevice_rev_id_get(void);
    virtual boolean rfdevice_reg_dump(void);
    virtual boolean self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id);
	
    boolean get_device_info(uint16 *mfg_id, uint16 *prd_id, uint16 *rev_id);
		

    /*!Updates the static variable "device_cal_state" with device calibration info for use in the device driver space*/
    static void rfdevice_set_cal_state(boolean cal_state);
    static boolean rfdevice_get_cal_state(void);
    virtual ~rfdevice_class();

    /*! rfdevice global mutex - one per instance.Can be used by all rfdevices to protect critical section code. 
    Should be initialized during device configuration before any other usage */
    rf_lock_data_type rfdevice_critical_sect;
};

extern "C" 
{

#endif /* #ifdef __cplusplus */
/*----------------------------------------------------------------------------*/
extern rfdevice_id_enum_type rfdevice_id_get(void*);
extern void rfdevice_set_cal_state(boolean);
#ifdef __cplusplus
}
#else
/* Type def below is for Compilation purposes only */ 
typedef int rfdevice_class;
#endif

#endif
