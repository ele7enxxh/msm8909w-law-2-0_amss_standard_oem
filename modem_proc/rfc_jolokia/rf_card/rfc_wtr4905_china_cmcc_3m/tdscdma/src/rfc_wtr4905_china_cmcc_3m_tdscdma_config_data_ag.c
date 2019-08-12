
/*
WARNING: This file is auto-generated.

Generated using: xmlautogen.exe
Generated from: V5.9.321 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr4905_china_cmcc_3m/tdscdma/src/rfc_wtr4905_china_cmcc_3m_tdscdma_config_data_ag.c#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_wtr4905_china_cmcc_3m_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 
#include "wtr4905_typedef_ag.h" 

rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_init_tdscdma_rx_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not Specified */,   /* Modem Chain */
  0 /* Warning: Not Specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not Specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
{
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0 /* Warning: Not specified */,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0 /* Warning: Not specified */,   /* INTF_REV */
        0 /* Invalid TRx port */,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_init_tdscdma_rx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_init_tdscdma_tx_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not Specified */,   /* Modem Chain */
  0 /* Warning: Not Specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not Specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
{
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0 /* Warning: Not specified */,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0 /* Warning: Not specified */,   /* INTF_REV */
        0 /* Invalid TRx port */,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_init_tdscdma_tx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_rx0_tdscdma_b34_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_TDSCDMA_PRXLGY1SAWLESS_BAND34_PMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_rx0_tdscdma_b34_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_01,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_09,   { RFC_HIGH, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_02,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_11,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_tx0_tdscdma_b34_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_TDSCDMA_TX_BAND34_THMLB5,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_tx0_tdscdma_b34_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_CHINA_CMCC_3M_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -3 }, {RFC_LOW, -3 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_01,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_09,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_02,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_PA_ON_01,   { RFC_HIGH, 0 }, {RFC_LOW, -2 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_PA0_R0_ALT1,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_INTERNAL_GNSS_BLANK_CONCURRENCY,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_rx0_tdscdma_b39_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_TDSCDMA_PRXLGY1SAWLESS_BAND39_PMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_rx0_tdscdma_b39_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_01,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_09,   { RFC_HIGH, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_02,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_11,   { RFC_LOW, -1 }, {RFC_LOW, -6 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_china_cmcc_3m_tx0_tdscdma_b39_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_TDSCDMA_TX_BAND39_THMLB5,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_china_cmcc_3m_tx0_tdscdma_b39_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_CHINA_CMCC_3M_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -3 }, {RFC_LOW, -3 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_01,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_09,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_RF_PATH_SEL_02,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_PA_ON_01,   { RFC_HIGH, 0 }, {RFC_LOW, -2 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_PA0_R0_ALT1,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_INTERNAL_GNSS_BLANK_CONCURRENCY,   { RFC_HIGH, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_WTR4905_CHINA_CMCC_3M_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, -1 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


























