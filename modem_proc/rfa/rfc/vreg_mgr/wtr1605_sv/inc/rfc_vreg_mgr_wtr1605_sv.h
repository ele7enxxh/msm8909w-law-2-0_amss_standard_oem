#ifndef RFC_VREG_MGR_WTR1605_SV_H
#define RFC_VREG_MGR_WTR1605_SV_H
/*!
   @file
   rfc_vreg_mgr_wtr1605_sv.h

   @brief


   @details

*/
/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/wtr1605_sv/inc/rfc_vreg_mgr_wtr1605_sv.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
09/18/14   vv      API support to detect rf sleep status 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager 
07/09/14   aak     Changes for API to control MPP for DAC Vref
08/03/13   aak     Changes for voting for resources for concurrent techs
05/07/13   aak     Replace RFM_EGSM_MODE_2 with RFM_NUM_MODES 
04/30/13   aak    Changes for second GSM client for DSDS/DSDA 
08/24/12   aak    Fixes for sending NPA requests from RF SW on Dime 
07/23/12   aak    Initial check-in for Dime 
07/09/12   aak    Move Quiet mode API from device to RFC  
04/02/12   sr     Initial revision.

===========================================================================
                           INCLUDE FILES
===========================================================================*/


#ifdef __cplusplus

#include "rfc_vreg_mgr.h"
#include "rfc_vreg_param.h"

class rfc_vreg_mgr_wtr1605_sv : public rfc_vreg_mgr
{
public:

  static rfc_vreg_mgr * get_instance(void);

  virtual ~rfc_vreg_mgr_wtr1605_sv();

  virtual void enable_autocal_vregs(boolean on_off);
  virtual void enable_mpp_dac_vref(boolean on_off);
  virtual void lte_light_sleep_vregs(void);
  virtual void lte_light_sleep_wakeup_vregs(void);
  virtual void cdma_manage_power(rfc_vreg_param &vreg_param);
  virtual void gsm_manage_power(rfc_vreg_param &vreg_param);
  virtual void wcdma_manage_power(rfc_vreg_param &vreg_param);
  virtual void lte_manage_power(rfc_vreg_param &vreg_param);
  virtual void tdscdma_manage_power(rfc_vreg_param &vreg_param);
  virtual void gps_manage_power(rfc_vreg_param &vreg_param);
  virtual void get_awake_resource_request(rfm_resource_info* params);
  virtual void set_quiet_mode(rfm_mode_enum_type mode, boolean on_off);
  boolean rf_vreg_is_asleep();
protected:
    rfc_vreg_mgr_wtr1605_sv();  // constructor

    uint8 npa_rxtx_table[RFM_NUM_MODES][RFC_VOLTAGE_SCALE_MAX];

   npa_client_handle    rf_wan_gps_npa_handle[RFM_NUM_MODES];

private:

  void manage_power(rfc_vreg_param &vreg_param);
  void manage_sv_power(rfc_vreg_param &vreg_param);
  rfc_voltage_scaling_type get_voltage_scaling_state(rfc_vreg_param &vreg_param);
  void send_npa_rf_id(rfcom_mode_enum_type mode, uint8 npa_id);
  void send_npa_rf_gps_id(rfcom_mode_enum_type mode, uint8 npa_id);
  void send_sleep_npa_id(rfc_vreg_param &vreg_param);
  void save_mode_info(rfc_vreg_param &vreg_param);
  void init_rf_npa_handle(void);
  void init_chip_info(void);
  void init_npa_rxtx_table(void);
  void print_npa_rf_id_msg(uint8 npa_id);
  void print_npa_rf_gps_id_msg(uint8 npa_id);

  //rfc_rf_chip_info     rf_chip_info[RF_CHIP_MAX];
  uint8                last_rf_wan_gps_npa_id[RFM_NUM_MODES];

  rfm_path_state rfc_state_info[RFM_NUM_MODES][2]; 

  uint16               vreg_por_wait;
};

extern "C" 
{
#endif /* #ifdef __cplusplus */
extern  boolean rf_is_asleep(void);
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* RFC_VREG_MGR_WTR1605_SV_H */



