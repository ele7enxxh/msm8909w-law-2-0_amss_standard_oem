#ifndef RFC_VREG_MGR_H
#define RFC_VREG_MGR_H
/*!
   @file
   rfc_vreg_mgr.h

   @brief


   @details

*/
/*===========================================================================

Copyright (c) 2009 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/common/inc/rfc_vreg_mgr.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager 
07/09/14   aak     Changes for API to control MPP for DAC Vref
07/09/12   aak     Move Quiet mode API from device to RFC  
04/19/12   aak    Changes to change to C++ interface  
04/02/12   sr     Initial revision.

===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
}

#include "rfc_vreg_param.h"

class rfc_vreg_mgr
{
public:

  static rfc_vreg_mgr * get_instance(void);

  virtual void enable_autocal_vregs(boolean on_off) = 0;
  virtual void enable_mpp_dac_vref(boolean on_off) = 0;
  virtual void lte_light_sleep_vregs(void) = 0;
  virtual void lte_light_sleep_wakeup_vregs(void) = 0;
  virtual void cdma_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void gsm_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void wcdma_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void lte_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void tdscdma_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void gps_manage_power(rfc_vreg_param &vreg_param) = 0;
  virtual void get_awake_resource_request(rfm_resource_info* params) = 0;
  virtual void set_quiet_mode(rfm_mode_enum_type mode, boolean on_off) = 0;

  virtual ~rfc_vreg_mgr();

protected:
    rfc_vreg_mgr(void);  // constructor
    static rfc_vreg_mgr *vreg_mgr_ptr;

    rfc_voltage_scaling_type rfc_voltage_scaling_state[RFM_NUM_MODES];
    rf_hw_type rf_hw_id;
private:

};


#endif /* RFC_VREG_MGR_H */



