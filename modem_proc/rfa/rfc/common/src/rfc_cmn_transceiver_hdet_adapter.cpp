/*!
   @file
   rfc_cmn_transceiver_hdet_adapter.cpp

   @brief
   Definition of the Transceiver device to Common HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2012,2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_cmn_transceiver_hdet_adapter.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/16/13   hm      passing in wcdma rx device in constructor, 
                   to enable the hdet adapter call C-function pointers directly
09/16/13   zg      Updates to support C++ class based TDSCDMA RX and TX interface
03/29/13   adk     Read and store temp (K-sensor) values for QFE TDET
03/21/13   ms      Tuner TDET support
03/13/13   aca     Tuner power characterization
03/11/13   sar     Updated file for APQ, GNSS only target.
01/22/13   adk     Added support for TDET gated mode
12/06/12   pv      LTE support for HDET CMN class.
12/06/12   bmg     Initial version.

==============================================================================*/

#include "comdef.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfc_cmn_transceiver_hdet_adapter.h"
#include "rfdevice_cmn_intf.h"
#include "rfcommon_msg.h"

rfc_cmn_transceiver_hdet_adapter::rfc_cmn_transceiver_hdet_adapter
(
  rfdevice_cmn_type* rfdevice_cmn_ptr
) :
   #ifdef FEATURE_CDMA1X
   cdma_adapter(NULL), 
   #endif
   wcdma_adapter(NULL)
{
#ifdef FEATURE_CDMA1X
  cdma_adapter =
    new rfc_cdma_transceiver_hdet_adapter(
          (rfdevice_cdma_tx_type *)rfdevice_cmn_get_tech_instance(rfdevice_cmn_ptr,
                                                                  RFM_1X_MODE,
                                                                  RFDEVICE_TX_CHAIN));
#endif /* FEATURE_CDMA1X */
  wcdma_adapter = 
    new rfc_wcdma_transceiver_hdet_adapter(
          (rfwcdma_device_tx_type *)rfdevice_cmn_get_tech_instance(rfdevice_cmn_ptr,
                                                                  RFM_IMT_MODE,
                                                                  RFDEVICE_TX_CHAIN));

#ifdef FEATURE_LTE
  /* LTE Support */
  lte_adapter =
    new rfc_lte_transceiver_hdet_adapter(
          (rfdevice_lte_tx_type *)rfdevice_cmn_get_tech_instance( rfdevice_cmn_ptr,
                                                                  RFM_LTE_MODE,
                                                                  RFDEVICE_TX_CHAIN));
#endif

#ifdef FEATURE_TDSCDMA
  /* TDSCDMA Support */
  tdscdma_adapter =
    new rfc_tdscdma_transceiver_hdet_adapter(
          (rftdscdma_device_tx_type *)rfdevice_cmn_get_tech_instance(rfdevice_cmn_ptr,
                                                                     RFM_TDSCDMA_MODE,
                                                                     RFDEVICE_TX_CHAIN));
#endif

}

rfc_cmn_transceiver_hdet_adapter::~rfc_cmn_transceiver_hdet_adapter()
{
#ifdef FEATURE_CDMA1X
  delete cdma_adapter;
#endif
  delete wcdma_adapter;
#ifdef FEATURE_LTE
  delete lte_adapter;
#endif

#ifdef FEATURE_TDSCDMA
  delete tdscdma_adapter;
#endif
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for Common HDET auto-cal interface.

  @details
  Auto-cal is always done through the tranceiver object.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::auto_cal
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for Common HDET disable interface.

  @details
  HDET on tranceivers can only be enabled and disabled through the tech-
  specific interface.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::disable_hdet
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for Common HDET DC calibration interface.

  @details
  Self-calibration is always done through the tranceiver object.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::do_hdet_dc_cal
(
  void
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for Common HDET DC calibration query interface.

  @details
  Transceivers do not support DC calibration query.

  @param hdet_inc_dc_offset_val
  Unused.

  @param hdet_ref_dc_offset_val
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::get_dc_offset_val
(
  uint16* hdet_inc_dc_offset_val,
  uint16* hdet_ref_dc_offset_val
)
{
  return FALSE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets for power read.
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param num_avg
  Averaging

  @wait_time
  Time to wait between trigger and read

  @return
  boolean indicating pass (TRUE) or fail (FALSE)   
*/

boolean
rfc_cmn_transceiver_hdet_adapter::setup_power_read
(
  rf_buffer_intf* buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint8 num_avg,
  uint16 wait_time

)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported incident power query.

  @details
  Transceiver devices only support power readings through tech-specific
  interfaces.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @param hdet_inc_pwr
  Unused.

  @param trigger_flag
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::get_incident_val
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16* hdet_inc_pwr,
  boolean trigger_flag
)
{
  return FALSE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function reads the power of the pid of interest. 

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @param hdet_pwr
  pointer to hdet power value
  
  @param hdet_pid
  Pid of interest

  @return
  boolean indicating pass or fail  
*/

boolean
rfc_cmn_transceiver_hdet_adapter::get_power
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint8 hdet_pid,
  uint16* hdet_inc_pwr
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported common HDET interface.

  @details
  Transceivers do not support the "K sensor."

  @param k_val
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::get_k_sensor_val
(
  uint16* k_val
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported common HDET interface.

  @details
  Transceivers do not support the "K sensor."

  @param rf_buff
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to HW or build scripts
 
  @return
  Always returns TRUE
*/
boolean
rfc_cmn_transceiver_hdet_adapter::update_temp_comp_param
(
  rf_buffer_intf* rf_buff,
  rf_device_execution_type dev_action
)
{
    RF_MSG( RF_LOW,
            "rfc_cmn_transceiver_hdet_adapter::update_temp_comp_param:"
            " API not supported for this HDET device");
      return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported common HDET interface.

  @details
  Transceivers do not support the "K sensor."

  @param temp_comp_data_ptr
  ptr to the temp comp data. This data will be stored in the TDET object.

  @return
  Always returns TRUE
*/
boolean
rfc_cmn_transceiver_hdet_adapter::update_temp_comp_param_cb
(
   qfe_script_type* temp_comp_data_ptr
)
{
  RF_MSG( RF_LOW,
          "rfc_cmn_transceiver_hdet_adapter::update_temp_comp_param_cb:"
          " API not supported for this HDET device");
    return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported reflected power query.

  @details
  Transceivers do not support reading reflected power.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @param hdet_ref_pwr
  Unused.

  @param trigger_flag
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::get_ref_val
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16* hdet_ref_pwr,
  boolean trigger_flag
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET path selection.

  @details
  Tranceivers do not support setting HDET measurement incident/reflected
  configuration.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::hkadc_set_incident_path
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET path selection.

  @details
  Tranceivers do not support setting HDET measurement incident/reflected
  configuration.

  @param buff_obj_ptr
  Unused.

  @param dev_action
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::hkadc_set_ref_path
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET interface.

  @details
  Transceivers do not support the "K value."

  @param k0_val
  Unused.

  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::program_k0_val
(
  uint16 k0_val
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for Common HDET Set Gain State interface.
 
  @details
  Transceivers do not support a technology-independent interface to set the
  HDET gain state.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Unused.
 
  @param hdet_gain_state
  Unused.

  @hdet_pid
  PID of interest. Defalted to 0.
 
  @return
  Always returns FALSE.
*/
boolean
rfc_cmn_transceiver_hdet_adapter::set_gain_state
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  rfdevice_hdet_gain_state_type hdet_gain_state,
  uint8 hdet_pid
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET interface.

  @details
  Transceivers do not support the "K value."

  @return
  Always returns FALSE.
*/
boolean rfc_cmn_transceiver_hdet_adapter::update_k_comp(void)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET interface.

  @details
  Transceivers do not support the "K value."

  @return
  Always returns 0.
*/
int16 rfc_cmn_transceiver_hdet_adapter::get_k_comp(void)
{
  return 0;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for unsupported HDET interface.

  @details
  Transceivers do not support gated mode.

  @return
  Always returns FALSE
*/
boolean rfc_cmn_transceiver_hdet_adapter::get_gated_incident_val
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint16 *hdet_inc_pwr,
  boolean trigger_flag,
  int delay,
  int number_of_samples
)
{
  return FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns a pointer to the tech specific HDET interface adapter for this
  tranceiver.
 
  @return
  void pointer to tech specific hdet instance
*/
void* rfc_cmn_transceiver_hdet_adapter::get_hdet_tech_object(rfm_mode_enum_type tech)
{
  switch ( tech )
  {
#ifdef FEATURE_CDMA1X
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
    return cdma_adapter;
#endif

  case RFM_IMT_MODE:
    return wcdma_adapter;

#ifdef FEATURE_LTE
  case RFM_LTE_MODE:
    return lte_adapter;
#endif
    
#ifdef FEATURE_TDSCDMA
  case RFM_TDSCDMA_MODE:
   return tdscdma_adapter;
#endif

  default:
    RF_MSG_1(RF_ERROR,
             "rfc_cmn_transceiver_hdet_adapter::get_hdet_tech_object: "
             "Unsupported tech (%d).", tech);
    return NULL;
  }
} /* rfc_cmn_transceiver_hdet_adapter::get_hdet_tech_object() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Required implementation of the common init() function required by all
  classes derived from rfdevice_class.
 
  @details
  No operations are needed to initialize this adapter class.
*/
void rfc_cmn_transceiver_hdet_adapter::init()
{
}

#endif
