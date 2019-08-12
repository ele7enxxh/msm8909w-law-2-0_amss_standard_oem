/*!
   @file rfc_configurable_test_vreg_mgr.cpp

   @brief


   @details
   This file implements the VREG Mgr functionality for GENERIC rf-cards.

*/
/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/rfc_configurable_test_vreg_mgr/src/rfc_configurable_test_vreg_mgr.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager
07/09/14   aak     Changes for API to control MPP for DAC Vref
10/03/13   aak    Remove FEATURE_DIME_MODEM from vreg_mgr files
11/09/11   sr     Initial revision.
============================================================================

                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfa_variation.h"
#include "msg.h"
#include "pm.h"
#include "npa.h"
#include "pmapp_npa.h"
#include "DALSys.h"
#include "mcpm_api.h"
#ifdef FEATURE_RF_HAS_NPA_SCHEDULED_REQUEST_API
#include "npa_scheduler.h"
#endif
#include "rfm_internal.h"
}

#include "rfc_configurable_test_vreg_mgr.h" 
#include "rfcommon_nv_mm.h"

static rfm_mode_enum_type tech_resource_data[RFM_NUM_MODES]; 
static volatile boolean rfc_sv_npa_voting = TRUE;

/*===========================================================================
                           Functions
===========================================================================*/
rfc_vreg_mgr * rfc_configurable_test_vreg_mgr::get_instance()
{
  if (vreg_mgr_ptr == NULL)
  {
    vreg_mgr_ptr = (rfc_vreg_mgr *)new rfc_configurable_test_vreg_mgr();
  }
  return(vreg_mgr_ptr);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_configurable_test_vreg_mgr::rfc_configurable_test_vreg_mgr(void)
: rfc_vreg_mgr()
{
  uint8 tech = 0;

  //Wait in case needed after command send to NPA for vregs/clocks to settle. Set to 650us right now.
  vreg_por_wait = 650;

  /* initialize NPA ID for all techs and all voltage scaling  */
  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
  {
    tech_resource_data[tech] = (rfm_mode_enum_type)tech;
    rf_wan_gps_npa_handle[tech] = 0;
    last_rf_wan_gps_npa_id[tech] = PMIC_NPA_MODE_ID_RF_SLEEP;
  }

  init_rf_npa_handle();
  init_chip_info();
  init_npa_rxtx_table();

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Destructor.

   @details

*/
rfc_configurable_test_vreg_mgr::~rfc_configurable_test_vreg_mgr()
{
  // Do nothing
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_configurable_test_vreg_mgr::enable_autocal_vregs(boolean on_off)
{

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Entering rfc_configurable_test_vreg_mgr::enable_autocal_vregs(%d)", on_off);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_configurable_test_vreg_mgr::enable_mpp_dac_vref(boolean on_off)
{

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Entering rfc_configurable_test_vreg_mgr::enable_mpp_dac_vref(%d)", on_off);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::lte_light_sleep_vregs(void)
{

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Entering rfc_configurable_test_vreg_mgr::lte_light_sleep_vregs", 0);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::lte_light_sleep_wakeup_vregs(void)
{

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Entering rfc_configurable_test_vreg_mgr::lte_light_sleep_wakeup_vregs", 0);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_configurable_test_vreg_mgr::manage_power(rfc_vreg_param &vreg_param)
{

  MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,"Enter manage_power: mode=%d, band=%d, chip_num=%d,path=%d, rf state=%d", 
        vreg_param.get_mode(), vreg_param.get_band(), vreg_param.get_rfchip_number(),
        vreg_param.get_path(), vreg_param.get_rf_state());

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_configurable_test_vreg_mgr::manage_sv_power(rfc_vreg_param &vreg_param)
{

  MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,"Enter manage_sv_power: mode=%d, band=%d, chip_num=%d,path=%d, rf state=%d", 
        vreg_param.get_mode(), vreg_param.get_band(), vreg_param.get_rfchip_number(),
        vreg_param.get_path(), vreg_param.get_rf_state());

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_configurable_test_vreg_mgr::cdma_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for GSM mode

   @details

*/
void rfc_configurable_test_vreg_mgr::gsm_manage_power (rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for WCDMA mode

   @details

*/
void rfc_configurable_test_vreg_mgr::wcdma_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for TDSCDMA mode

   @details

*/
void rfc_configurable_test_vreg_mgr::tdscdma_manage_power (rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for GPS mode

   @details

*/
void rfc_configurable_test_vreg_mgr::gps_manage_power(rfc_vreg_param &vreg_param)
{

  MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,"Enter gps_manage_power: mode=%d, band=%d, chip_num=%d,path=%d, rf state=%d", 
        vreg_param.get_mode(), vreg_param.get_band(), vreg_param.get_rfchip_number(),
        vreg_param.get_path(), vreg_param.get_rf_state());

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for LTE mode

   @details

*/
void rfc_configurable_test_vreg_mgr::lte_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_configurable_test_vreg_mgr::get_awake_resource_request(rfm_resource_info* params)
{

  MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"Enter GENERIC Awake Resource Request(): techA:mode=%d,state=%d,device=%d,"
                                    "vol_scaling=%d", params->tech_A.mode, params->tech_A.path_state, 
                                     params->tech_A.device_num, rfc_voltage_scaling_state[params->tech_A.mode]);
  MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"techB:mode=%d,state=%d,device=%d,vol_scaling=%d", 
                                       params->tech_B.mode, params->tech_B.path_state, params->tech_B.device_num,
                                       rfc_voltage_scaling_state[params->tech_B.mode]);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_configurable_test_vreg_mgr::set_quiet_mode(rfm_mode_enum_type mode, boolean enable)
{
  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC set_quiet_mode(%d, %d) ", mode, enable);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
rfc_voltage_scaling_type rfc_configurable_test_vreg_mgr::get_voltage_scaling_state(rfc_vreg_param &vreg_param)
{
  MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC get_voltage_scaling_state() "
                                    "rf_chip_info.path_info[%d].state = %d"
                                    "mode=%d, band=%d",vreg_param.get_path(),vreg_param.get_rf_state(),
                                    vreg_param.get_mode(), vreg_param.get_band());


  rfc_voltage_scaling_type voltage_scaling = RFC_VOLTAGE_SCALE_ON;

  return (voltage_scaling);  
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::send_npa_rf_id(rfcom_mode_enum_type mode, uint8 npa_id)
{
  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC send_npa_rf_id(%d, %d) ", mode, npa_id);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::send_npa_rf_gps_id(rfcom_mode_enum_type mode, uint8 npa_id)
{
  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC send_npa_rf_gps_id(%d, %d) ", mode, npa_id);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::send_sleep_npa_id(rfc_vreg_param &vreg_param)
{

  MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC send_sleep_npa_id() "
                                    "rf_chip_info.path_info[%d].state = %d"
                                    "mode=%d, band=%d",vreg_param.get_path(),vreg_param.get_rf_state(),
                                    vreg_param.get_mode(), vreg_param.get_band());

}


/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::save_mode_info(rfc_vreg_param &vreg_param)
{

  MSG_4(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC save_mode_info() "
                                    "rf_chip_info.path_info[%d].state = %d"
                                    "mode=%d, band=%d",vreg_param.get_path(),vreg_param.get_rf_state(),
                                    vreg_param.get_mode(), vreg_param.get_band());

}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::init_rf_npa_handle(void)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC init_rf_npa_handle() ", 0);
} 

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::init_chip_info(void)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC init_chip_info() ", 0);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::init_npa_rxtx_table(void)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC init_npa_rxtx_table() ", 0);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::print_npa_rf_id_msg(uint8 npa_id)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC print_npa_rf_id_msg(%d) ", npa_id);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_configurable_test_vreg_mgr::print_npa_rf_gps_id_msg(uint8 npa_id)
{
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"GENERIC print_npa_rf_gps_id_msg(%d) ", npa_id);
}

