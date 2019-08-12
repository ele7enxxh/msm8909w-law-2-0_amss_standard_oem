#ifndef RFC_CONFIGURABLE_TEST_VREG_MGR_H
#define RFC_CONFIGURABLE_TEST_VREG_MGR_H
/*!
   @file
   rfc_configurable_test_vreg_mgr.h

   @brief


   @details

*/
/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/rfc_configurable_test_vreg_mgr/inc/rfc_configurable_test_vreg_mgr.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager 
07/09/14   aak     Changes for API to control MPP for DAC Vref
11/09/12   sr     Initial revision.

===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
}

#include "rfc_vreg_mgr.h"
#include "rfc_vreg_param.h"


class rfc_configurable_test_vreg_mgr : public rfc_vreg_mgr
{
public:

  static rfc_vreg_mgr * get_instance(void);

  virtual ~rfc_configurable_test_vreg_mgr();

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

protected:
    rfc_configurable_test_vreg_mgr();  // constructor

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

  rfc_rf_chip_info     rf_chip_info[RF_CHIP_MAX];
  uint8                last_rf_wan_gps_npa_id[RFM_NUM_MODES];

  uint16               vreg_por_wait;
};


#endif /* RFC_CONFIGURABLE_TEST_VREG_MGR_H */



