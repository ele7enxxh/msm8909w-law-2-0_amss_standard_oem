/*!
   @file
   rfc_configurable_test_card_cmn_data.cpp

   @brief
   This file contains implementation the rfc_configurable_test_card_cmn_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/rf_card/rfc_configurable_test_card/common/src/rfc_configurable_test_card_cmn_data.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/08/14   vr      Updated with RFC interface changes.
10/08/14   vr      Rumi Changes for Tx devices
08/11/14   vrb     Updated to use physical device structure definitions
06/04/14   Saul    Remove GPS EXT LNA
05/12/14   kab     Remove RFCOM_RECEIVER_DUAL Support
09/05/13   sd      Replace banned APIs
05/01/13   bmg     Make get_logical_path_config() return false if it has never
                   been initialized (for correct no-NV behavior)
02/09/13   sr      Added Destructor, default device and signal list.
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include <stringl/stringl.h>
#include "rfc_msm_signal_info_ag.h" 
#include "rfc_configurable_test_card_cmn_data.h" 
#include "DDITlmm.h" 
#include "rfc_common.h" 
#include "rfcommon_core_sbi.h" 

#ifdef FEATURE_GSM
#include "rfc_configurable_test_card_gsm_data.h" 
#endif 

#ifdef FEATURE_WCDMA
#include "rfc_configurable_test_card_wcdma_data.h" 
#endif 

#ifdef FEATURE_TDSCDMA
#include "rfc_configurable_test_card_tdscdma_data.h" 
#endif 

#ifdef FEATURE_CDMA1X
#include "rfc_configurable_test_card_cdma_data.h" 
#endif 

#ifdef FEATURE_LTE
#include "rfc_configurable_test_card_lte_data.h" 
#endif 

#ifdef FEATURE_CGPS
#include "rfc_configurable_test_card_gnss_data.h" 
#endif 


rfc_phy_device_info_type rfc_configurable_test_card_default_phy_devices_list[] = 
{
  /* Dummy test chip */
  { /*Device: TEST WTR*/ 
    TEST_DEVICE, /* PHY_DEVICE_NAME */ 
    0, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_INVALID, /* PHY_DEVICE_COMM_PROTOCOL */ 
    {    0,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0, /* PHY_DEVICE_PRODUCT_ID */ 
    0, /* PHY_DEVICE_PRODUCT_REV */ 
    0, /* DEFAULT USID RANGE START */
    0, /* DEFAULT USID RANGE END */
    0, /* PHY_DEVICE_ASSIGNED_USID */ 
    0, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    0, /* ASSOCIATED_DAC */ 
  }, /* END - Device: TEST WTR */ 
    /* Dummy test chip */
  { /*Device: TEST PA*/ 
    TEST_DEVICE, /* PHY_DEVICE_NAME */ 
    1, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_INVALID, /* PHY_DEVICE_COMM_PROTOCOL */ 
    {    0,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0, /* PHY_DEVICE_PRODUCT_ID */ 
    0, /* PHY_DEVICE_PRODUCT_REV */ 
    0, /* DEFAULT USID RANGE START */
    0, /* DEFAULT USID RANGE END */
    0, /* PHY_DEVICE_ASSIGNED_USID */ 
    0, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    0, /* ASSOCIATED_DAC */ 
  }, /* END - Device: TEST PA */
    /* Dummy test chip */
  { /*Device: TEST PAPM */ 
    TEST_DEVICE, /* PHY_DEVICE_NAME */ 
    2, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_INVALID, /* PHY_DEVICE_COMM_PROTOCOL */ 
    {    0,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0, /* PHY_DEVICE_PRODUCT_ID */ 
    0, /* PHY_DEVICE_PRODUCT_REV */ 
    0, /* DEFAULT USID RANGE START */
    0, /* DEFAULT USID RANGE END */
    0, /* PHY_DEVICE_ASSIGNED_USID */ 
    0, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    0, /* ASSOCIATED_DAC */ 
  }, /* END - Device: TEST PAPM */
  /* Array terminator */
  { /*Device: RFDEVICE_INVALID */ 
    RFDEVICE_INVALID, /* PHY_DEVICE_NAME */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_INSTANCE */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_INVALID, /* PHY_DEVICE_COMM_PROTOCOL */ 
    {    0 /* 0 not specified */,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_PRODUCT_ID */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_PRODUCT_REV */ 
    0 /*Warning: Not specified*/, /* DEFAULT USID RANGE START */
    0 /*Warning: Not specified*/, /* DEFAULT USID RANGE END */
    0 /*Warning: Not specified*/, /* PHY_DEVICE_ASSIGNED_USID */ 
    0 /*Warning: Not specified*/, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    RFC_INVALID_PARAM, /* ASSOCIATED_DAC */ 
  }, /* END - Device: RFDEVICE_INVALID */  
};


rfc_logical_device_info_type rfc_configurable_test_card_default_logical_devices_list[] = 
{
  { /*Device: TEST WTR */ 
    RFDEVICE_TRANSCEIVER, /* DEVICE_MODULE_TYPE */ 
    TEST_DEVICE, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    0, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: TEST WTR */ 
  { /*Device: TEST PA */ 
    RFDEVICE_PA, /* DEVICE_MODULE_TYPE */ 
    TEST_DEVICE, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    1, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: TEST PA */ 
  { /*Device: TEST PAPM */ 
    RFDEVICE_PAPM, /* DEVICE_MODULE_TYPE */ 
    TEST_DEVICE, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    2, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: TEST PAPM */ 
  { /*Device: RFDEVICE_INVALID */ 
    RFDEVICE_TYPE_INVALID, /* DEVICE_MODULE_TYPE */ 
    RFDEVICE_INVALID, /* DEVICE_MODULE_NAME */ 
    0 /*Warning: Not specified*/, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    0 /*Warning: Not specified*/, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: RFDEVICE_INVALID */ 
};


rfc_signal_info_type rfc_configurable_test_card_default_sig_info[RFC_CONFIGURABLE_TEST_CARD_SIG_NUM + 1] = 
{
  { RFC_MSM_TIMING_PA_CTL      , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_TIMING_PA_CTL */
  { RFC_MSM_TIMING_PA_RANGE    , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_TIMING_PA_RANGE */
  { RFC_MSM_TIMING_ASM_CTL     , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_TIMING_ASM_CTL */
  { RFC_MSM_TIMING_TX_TX_RF_ON0, RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_TIMING_TX_TX_RF_ON0 */
  { RFC_MSM_TIMING_TX_RX_RF_ON0, RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_TIMING_TX_RX_RF_ON0 */
  { RFC_MSM_PA_ON_04           , RFC_LOW, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_PA_ON_04 */ 
  { RFC_MSM_RX_ON0             , RFC_LOW, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RX_ON0 */ 
  { RFC_MSM_RF_ON0             , RFC_LOW, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RF_ON0 */ 
  { RFC_MSM_PA0_R0             , RFC_LOW, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_PA0_R0 */ 
  { RFC_MSM_RF_PATH_SEL_00     , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RF_PATH_SEL_00 */ 
  { RFC_MSM_RF_PATH_SEL_01     , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RF_PATH_SEL_01 */ 
  { RFC_MSM_RF_PATH_SEL_02     , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RF_PATH_SEL_02 */ 
  { RFC_MSM_RF_PATH_SEL_03     , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RF_PATH_SEL_03 */ 
  { RFC_MSM_GPDATA0_0          , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_GPDATA0_0 */ 
  { RFC_MSM_GPDATA0_1          , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_GPDATA0_1 */ 
  { RFC_MSM_RFFE1_CLK          , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RFFE1_CLK */ 
  { RFC_MSM_RFFE1_DATA         , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RFFE1_DATA */ 
  { RFC_MSM_RFFE2_CLK          , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RFFE2_CLK */
  { RFC_MSM_RFFE2_DATA         , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioSignalType)NULL }, /* RFC_CONFIGURABLE_TEST_RFFE2_DATA */
  { (rfc_msm_signal_type)RFC_SIG_LIST_END, (rfc_logic_type)0x02040028, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioSignalType)NULL } /* LAST SIG INDICATOR */ 
};


static rfc_phy_device_info_type *rfc_configurable_test_card_phy_devices_list = NULL; 
static rfc_logical_device_info_type *rfc_configurable_test_card_log_devices_list = NULL; 
static rfc_signal_info_type rfc_configurable_test_card_sig_info[RFC_CONFIGURABLE_TEST_CARD_SIG_NUM + 1];
static int32 rfc_configurable_test_card_sig_info_num = 0;

rfc_common_data* rfc_configurable_test_card_cmn_data::get_instance(rf_hw_type rf_hw)
{
  if (rfc_common_data_ptr == NULL)
  {
    rfc_common_data_ptr = (rfc_common_data *)new rfc_configurable_test_card_cmn_data(rf_hw);
  }
  return( (rfc_common_data *)rfc_common_data_ptr);
}

//constructor
rfc_configurable_test_card_cmn_data::rfc_configurable_test_card_cmn_data(rf_hw_type rf_hw)
:rfc_common_data(rf_hw)
{
  // initialize the sig_info array with RFC_SIG_LIST_END.
  memset(rfc_configurable_test_card_sig_info, RFC_SIG_LIST_END, sizeof(rfc_signal_info_type)*(RFC_CONFIGURABLE_TEST_CARD_SIG_NUM+1));

  // initialize the cmn signal info with default signal info table
  sig_info_table_set(&rfc_configurable_test_card_default_sig_info[0]);

  // initialize the cmn device info with default device info table
  phy_device_cfg_set(&rfc_configurable_test_card_default_phy_devices_list[0]);
  log_device_cfg_set(&rfc_configurable_test_card_default_logical_devices_list[0]);

}

// Destructor
rfc_configurable_test_card_cmn_data::~rfc_configurable_test_card_cmn_data()
{
  // nothing to do for now
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the common rfc data. And creates the supported techs rfc data objects.

  @details
  This function returns the common rfc data. And creates the supported techs rfc data objects.
 
  @param
  rfc_info_table : common rfc_info_table ptr. 
 
  @retval
  num_rfc_signals: number of rf signals.
*/
uint32 rfc_configurable_test_card_cmn_data::sig_info_table_get(rfc_signal_info_type **rfc_info_table)
{
  if (NULL == rfc_info_table)
  {
    return 0;
  }

  *rfc_info_table = rfc_configurable_test_card_sig_info;

  #ifdef FEATURE_GSM
  // Create GSM RFC AG Data Object
  rfc_gsm_data *rfc_gsm_data = rfc_configurable_test_card_gsm_data::get_instance(); 
  if (rfc_gsm_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_GSM */

  #ifdef FEATURE_WCDMA
  // Create WCDMA RFC AG Data Object
  rfc_wcdma_data *rfc_wcdma_data = rfc_configurable_test_card_wcdma_data::get_instance(); 
  if (rfc_wcdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_WCDMA */

  #ifdef FEATURE_CDMA1X
  // Create CDMA RFC AG Data Object
  rfc_cdma_data *rfc_cdma_data = rfc_configurable_test_card_cdma_data::get_instance(); 
  if (rfc_cdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_CDMA */

  #ifdef FEATURE_LTE
  // Create LTE RFC AG Data Object
  rfc_lte_data *rfc_lte_data = rfc_configurable_test_card_lte_data::get_instance(); 
  if (rfc_lte_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_TDSCDMA
  // Create TDSCDMA RFC AG Data Object
  rfc_tdscdma_data *rfc_tdscdma_data = rfc_configurable_test_card_tdscdma_data::get_instance(); 
  if (rfc_tdscdma_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_TDSCDMA */

  #ifdef FEATURE_CGPS
  // Create GNSS RFC AG Data Object
  rfc_gnss_data *rfc_gnss_data = rfc_configurable_test_card_gnss_data::get_instance(); 
  if (rfc_gnss_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GNSS Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_GNSS */

  return rfc_configurable_test_card_sig_info_num;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the physical device configuration table.

  @details
  This function returns the physical device configuration table.
 
  @param None
 
  @retval
  rfc_phy_device_info_type: physical rf device configuration list.
*/
rfc_phy_device_info_type* rfc_configurable_test_card_cmn_data::get_phy_device_cfg( void )
{
  return (rfc_configurable_test_card_phy_devices_list);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the logical device configuration table.

  @details
  This function returns the logical device configuration table.
 
  @param None
 
  @retval
  rfc_logical_device_info_type: logical rf device configuration list.
*/
rfc_logical_device_info_type* rfc_configurable_test_card_cmn_data::get_logical_device_cfg( void )
{
  return (rfc_configurable_test_card_log_devices_list);
}

rfm_concurrency_restriction_type rfc_configurable_test_card_restriction_table;
rfm_devices_configuration_type rfc_configurable_test_card_logical_device_properties_data =
{
  253, /* HWID */
  { /* Mode Concurrencies: Bit0 represents Rx concurrency and Bit1 represents Tx concurrency */
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
  },
  { /* device_support */ 
    { /* Logical Device 0 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          ( ( (uint64)1 << (SYS_BAND_GSM_850 - 0) ) | ( (uint64)1 << (SYS_BAND_GSM_EGSM_900 - 0) ) | ( (uint64)1 << (SYS_BAND_GSM_DCS_1800 - 0) ) | ( (uint64)1 << (SYS_BAND_GSM_PCS_1900 - 0) ) | ( (uint64)1 << (SYS_BAND_BC0 - 0) ) | ( (uint64)1 << (SYS_BAND_BC1 - 0) ) | ( (uint64)1 << (SYS_BAND_BC6 - 0) ) | ( (uint64)1 << (SYS_BAND_BC10 - 0) ) | ( (uint64)1 << (SYS_BAND_BC14 - 0) ) | ( (uint64)1 << (SYS_BAND_BC15 - 0) ) 
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          ( ( (uint64)1 << (SYS_BAND_WCDMA_I_IMT_2000 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_II_PCS_1900 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_III_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_IV_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_V_850 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_VI_800 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_VIII_900 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_IX_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_XI_1500 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_XIX_850 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND1 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND2 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND3 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND4 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND5 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND7 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND8 - 64) ) 
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND11 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND12 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND13 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND17 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND18 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND19 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND20 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND21 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND25 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND26 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND28 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND30 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND34 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND38 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND39 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND40 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND41 - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDA - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDF - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDE - 128) ) 
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( RFM_DEVICE_RX_SUPPORTED | RFM_DEVICE_TX_SUPPORTED),
      RFM_DEVICE_1, /* Associated Slave Device for RFM_DEVICE_0 */
    }, /* End Logical Device 0 */ 
    { /* Logical Device 1 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          ( ( (uint64)1 << (SYS_BAND_BC0 - 0) ) | ( (uint64)1 << (SYS_BAND_BC1 - 0) ) | ( (uint64)1 << (SYS_BAND_BC6 - 0) ) | ( (uint64)1 << (SYS_BAND_BC10 - 0) ) | ( (uint64)1 << (SYS_BAND_BC14 - 0) ) | ( (uint64)1 << (SYS_BAND_BC15 - 0) ) 
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          ( ( (uint64)1 << (SYS_BAND_WCDMA_I_IMT_2000 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_II_PCS_1900 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_III_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_IV_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_V_850 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_VI_800 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_VIII_900 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_IX_1700 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_XI_1500 - 64) ) | ( (uint64)1 << (SYS_BAND_WCDMA_XIX_850 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND1 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND2 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND3 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND4 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND5 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND7 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND8 - 64) ) 
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND11 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND12 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND13 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND17 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND18 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND19 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND20 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND21 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND25 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND26 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND28 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND30 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND34 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND38 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND39 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND40 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND41 - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDA - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDF - 128) ) | ( (uint64)1 << (SYS_BAND_TDS_BANDE - 128) ) 
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( RFM_DEVICE_RX_SUPPORTED),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_1 */
    }, /* End Logical Device 1 */ 
    { /* Logical Device 2 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
           SYS_BM_64BIT(SYS_BAND_GSM_850) | SYS_BM_64BIT(SYS_BAND_GSM_EGSM_900) | SYS_BM_64BIT(SYS_BAND_GSM_DCS_1800) | SYS_BM_64BIT(SYS_BAND_GSM_PCS_1900)
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND1 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND2 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND3 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND4 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND5 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND7 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND8 - 64) ) 
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND11 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND12 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND13 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND17 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND18 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND19 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND20 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND21 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND25 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND26 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND28 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND30 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND34 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND38 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND39 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND40 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND41 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND29 - 128) ) 
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          ( ( (uint64)1 << (SYS_BAND_BC0 - 0) ) | ( (uint64)1 << (SYS_BAND_BC1 - 0) ) | ( (uint64)1 << (SYS_BAND_BC6 - 0) ) | ( (uint64)1 << (SYS_BAND_BC10 - 0) ) | ( (uint64)1 << (SYS_BAND_BC14 - 0) ) | ( (uint64)1 << (SYS_BAND_BC15 - 0) ) 
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( RFM_DEVICE_RX_SUPPORTED | RFM_DEVICE_TX_SUPPORTED),
      RFM_DEVICE_3, /* Associated Slave Device for RFM_DEVICE_2 */
    }, /* End Logical Device 2 */ 
    { /* Logical Device 3 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
           SYS_BM_64BIT(SYS_BAND_GSM_850) | SYS_BM_64BIT(SYS_BAND_GSM_EGSM_900) | SYS_BM_64BIT(SYS_BAND_GSM_DCS_1800) | SYS_BM_64BIT(SYS_BAND_GSM_PCS_1900)
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND1 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND2 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND3 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND4 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND5 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND7 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND8 - 64) ) 
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND11 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND12 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND13 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND17 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND18 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND19 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND20 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND21 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND25 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND26 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND28 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND30 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND34 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND38 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND39 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND40 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND41 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND29 - 128) ) 
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( RFM_DEVICE_RX_SUPPORTED),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_3 */
    }, /* End Logical Device 3 */ 
    { /* Logical Device 4 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_4 */
    }, /* End Logical Device 4 */ 
    { /* Logical Device 5 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_5 */
    }, /* End Logical Device 5 */ 
    { /* Logical Device 6 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },        
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_6 */
    }, /* End Logical Device 6 */ 
    { /* Logical Device 7 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* rfm_bands_bitmask ho_rxd_bands_supported */
      {
        {
          /* Bit mask element 0 */ 
          (
           0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
           0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
           0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_7 */
    }, /* End Logical Device 7 */ 
  },
  {
    0, /* num_concurrency_restrictions */ 
    NULL, /* No Concurrency Restrictions */ 
  },
  {
    0, /* num_3mode_entries */ 
    NULL, /* No 3Mode Support */ 
  },
  FALSE, /* gps_has_independent_receive_chain */ 
  FALSE, /* antenna_swap_supported */ 
};
rfm_devices_configuration_type *rfc_configurable_test_card_logical_device_properties = &rfc_configurable_test_card_logical_device_properties_data;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the logical path configuration table.

  @details
  This function returns the logical path configuration table.
 
  @param
  dev_cfg : ptr to the logical path configuration data to be set.
 
  @retval
  boolean: TRUE if successful, else FALSE.
*/
boolean rfc_configurable_test_card_cmn_data::get_logical_path_config(rfm_devices_configuration_type* dev_cfg)
{
  if ( dev_cfg == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfm_get_devices_configuration: Invalid Container", 0);
    return FALSE;
  }

  if (rfc_configurable_test_card_logical_device_properties != NULL)
  {
    memscpy(dev_cfg,
            sizeof(rfm_devices_configuration_type),
            rfc_configurable_test_card_logical_device_properties,
            sizeof(rfm_devices_configuration_type));
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the common rfc data.

  @details
  Set the common rfc data.
 
  @param
  rfc_info_table : common rfc_info_table ptr. 
 
  @retval
  num_rfc_signals: number of rf signals.
*/
uint32 rfc_configurable_test_card_cmn_data::sig_info_table_set(rfc_signal_info_type *rfc_info_table)
{
  uint8 i = 0;
  rfc_configurable_test_card_sig_info_num = 0;
  if (NULL != rfc_info_table)
  {
    // loop through the signal info table and count the number of signals in the rf-card.
    while (rfc_info_table[i].msm_sig_name != RFC_SIG_LIST_END && i < RFC_CONFIGURABLE_TEST_CARD_SIG_NUM)
    {
      memscpy(&rfc_configurable_test_card_sig_info[i], sizeof(rfc_signal_info_type), &rfc_info_table[i], sizeof(rfc_signal_info_type));
      rfc_configurable_test_card_sig_info_num++;
      i++;
    }
  }

  return rfc_configurable_test_card_sig_info_num;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the phy device configuration table.

  @details
  This function sets the phy device configuration table.
 
  @param
  device_config : phy device configuration table.
 
  @retval
  boolean: TRUE if sucessful.
*/
boolean rfc_configurable_test_card_cmn_data::phy_device_cfg_set( rfc_phy_device_info_type *device_config )
{
  rfc_configurable_test_card_phy_devices_list = device_config;
  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the logical device configuration table.

  @details
  This function sets the logical device configuration table.
 
  @param
  device_config : logical device configuration table.
 
  @retval
  boolean: TRUE if sucessful.
*/
boolean rfc_configurable_test_card_cmn_data::log_device_cfg_set( rfc_logical_device_info_type *device_config )
{
  rfc_configurable_test_card_log_devices_list = device_config;
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the logical path configuration table.

  @details
  This function sets the logical path configuration table.
 
  @param
  dev_cfg : ptr to the logical path configuration data to be set.
 
  @retval
  boolean: TRUE if successful, else FALSE.
*/
boolean rfc_configurable_test_card_cmn_data::logical_path_config_set(rfm_devices_configuration_type* dev_cfg)
{
  rfc_configurable_test_card_logical_device_properties = dev_cfg;

  return TRUE;
}

