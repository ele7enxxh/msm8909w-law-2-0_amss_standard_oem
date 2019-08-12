#ifndef RFDEVICE_QTHERM_H
#define RFDEVICE_QTHERM_H


/*===========================================================================

Copyright (c) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qtherm/api/rfdevice_qtherm.h#1 $
  $DateTime: 2016/12/14 01:58:16 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
09/05/14   vv    Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/


#include "rfc_common.h"
#include "rfdevice_therm.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qtherm_typedef.h"


class rfdevice_qtherm: public rfdevice_therm
{
  public:
 
    boolean setup_therm (rfdevice_therm_setup_cfg* therm_setup_cfg_p);

	
    boolean clean_up_therm (rfdevice_therm_clean_up_cfg* therm_clean_up_cfg_p);

 
    boolean read_therm_immediate (rfdevice_read_imm_cfg* therm_read_cfg_p);

    
    boolean read_therm_script (rfdevice_read_script_cfg* therm_read_cfg_p);
	
    
    boolean process_therm_read_script (rfdevice_process_read_script_cfg* therm_read_cfg_p);
	
  
    virtual ~rfdevice_qtherm();


  /* Overloaded constructor for physical device support */
    rfdevice_qtherm(
                  rfdevice_physical_device* phy_obj_ptr,
                  rfc_phy_device_info_type *phy_device_info, 
                  rfc_logical_device_info_type *logical_device_info,
                  rfdevice_qtherm_settings_type *qtherm_settings_p,
                  rfdevice_qtherm_func_tbl_type *qtherm_fn_ptrs
                  ); 

    rfdevice_qtherm(void);

  protected:

   rfdevice_qtherm *rfdevice_qtherm_p;

   /*To store the parent physical device object*/
   rfdevice_physical_device* rfdevice_phy_obj_ptr;

   rfc_device_cfg_info_type qtherm_device_info;

   rfdevice_qtherm_ag_cfg_type qtherm_device_cfg;

   rfdevice_qtherm_settings_type qtherm_settings;

   rfdevice_qtherm_func_tbl_type qtherm_fn_ptrs;

private:


  boolean setup_therm_internal (rfdevice_therm_setup_cfg* therm_setup_cfg_p);

  boolean clean_up_therm_internal (rfdevice_therm_clean_up_cfg* therm_clean_up_cfg_p);

  boolean read_therm_immediate_internal (rfdevice_read_imm_cfg* therm_read_cfg_p);


};

void qtherm_therm_setup(uint8 bus_id, uint8 slave_id);

void qtherm_read_therm(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);

void qtherm_therm_cleanup(uint8 bus_id, uint8 slave_id);


extern "C"
{
  boolean rfdevice_qtherm_rffe_in_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 *value
  );

  boolean rfdevice_qtherm_rffe_out_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 value
  );
}



#endif /* RFDEVICE_QTHERM_H */
