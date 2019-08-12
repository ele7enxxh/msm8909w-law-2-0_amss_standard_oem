
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr2965_qrd_non_ca_4373_1/gsm/src/rfc_wtr2965_qrd_non_ca_4373_1_gsm_config_data_ag.c#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_wtr2965_qrd_non_ca_4373_1_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 
#include "wtr2965_typedef_ag.h" 
#include "qfe4373fc_pa_port_typedef_ag.h" 
#include "rfdevice_coupler.h" 

rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_init_gsm_rx_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not Specified */,   /* Modem Chain */
  0 /* Warning: Not Specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not Specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
{
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0 /* Warning: Not specified */,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0 /* Warning: Not specified */,   /* INTF_REV */
        0 /* Invalid TRx port */,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_init_gsm_rx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_init_gsm_tx_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not Specified */,   /* Modem Chain */
  0 /* Warning: Not Specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not Specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
{
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0 /* Warning: Not specified */,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0 /* Warning: Not specified */,   /* INTF_REV */
        0 /* Invalid TRx port */,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_init_gsm_tx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g850_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_PRXLGY1_BAND850_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (8)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g850_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_20,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g850_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  5,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_TX_BAND850_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE4373FC_GSM, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x45 << 14)/*prd_id*/ | (QFE4373FC_GSM_GSM_BAND850_PORT_ANT_LB)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PAPM,
      QFE2101, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (27)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (9)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_COUPLER,
      QFE2085FC, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */            
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */
        (int)RFDEVICE_COUPLER_DIRECTION_OPEN,   /* POSITION */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g850_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_RANGE,   { RFC_CONFIG_ONLY, -1 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -150 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_MULTISLOT_CTL,   { RFC_CONFIG_ONLY, -6 }, {RFC_CONFIG_ONLY, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_11,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_GPDATA0_0,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_INTERNAL_GNSS_BLANK,   { RFC_HIGH, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 66 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g900_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_PRXLGY1_BAND900_PLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (7)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g900_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g900_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  5,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_TX_BAND900_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE4373FC_GSM, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x45 << 14)/*prd_id*/ | (QFE4373FC_GSM_GSM_BAND900_PORT_ANT_LB)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PAPM,
      QFE2101, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (27)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (9)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_COUPLER,
      QFE2085FC, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */            
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */
        (int)RFDEVICE_COUPLER_DIRECTION_OPEN,   /* POSITION */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g900_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_RANGE,   { RFC_CONFIG_ONLY, -1 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -150 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_MULTISLOT_CTL,   { RFC_CONFIG_ONLY, -6 }, {RFC_CONFIG_ONLY, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_11,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_GPDATA0_0,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_INTERNAL_GNSS_BLANK,   { RFC_HIGH, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 66 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g1800_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_PRXLGY1_BAND1800_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g1800_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_06,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_14,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g1800_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  5,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_TX_BAND1800_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE4373FC_GSM, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x45 << 14)/*prd_id*/ | (QFE4373FC_GSM_GSM_BAND1800_PORT_ANT_MB)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PAPM,
      QFE2101, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (27)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (10)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_COUPLER,
      QFE2085FC, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */            
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */
        (int)RFDEVICE_COUPLER_DIRECTION_OPEN,   /* POSITION */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g1800_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_RANGE,   { RFC_CONFIG_ONLY, -1 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -150 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_MULTISLOT_CTL,   { RFC_CONFIG_ONLY, -6 }, {RFC_CONFIG_ONLY, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_11,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_GPDATA0_0,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_INTERNAL_GNSS_BLANK,   { RFC_HIGH, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 66 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g1900_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_PRXLGY1_BAND1900_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_rx0_gsm_g1900_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_06,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_14,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g1900_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  5,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR2965, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR2965_GSM_TX_BAND1900_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR2965_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE4373FC_GSM, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x45 << 14)/*prd_id*/ | (QFE4373FC_GSM_GSM_BAND1900_PORT_ANT_MB)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PAPM,
      QFE2101, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (27)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2085FC, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (10)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_COUPLER,
      QFE2085FC, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        (0x217 << 22)/*mfg_id*/ | (0x17 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */            
        (int)RFDEVICE_COUPLER_ATTENUATION_INVALID,   /* ATTEN_FWD */
        (int)RFDEVICE_COUPLER_DIRECTION_OPEN,   /* POSITION */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr2965_qrd_non_ca_4373_1_tx0_gsm_g1900_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PA_RANGE,   { RFC_CONFIG_ONLY, -1 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_ASM_CTL,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_TUNER_CTL,   { RFC_CONFIG_ONLY, -150 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_CTL,   { RFC_CONFIG_ONLY, -100 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TIMING_PAPM_MULTISLOT_CTL,   { RFC_CONFIG_ONLY, -6 }, {RFC_CONFIG_ONLY, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_11,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_07,   { RFC_LOW, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_RF_PATH_SEL_16,   { RFC_HIGH, -40 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_GPDATA0_0,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_INTERNAL_GNSS_BLANK,   { RFC_HIGH, -10 }, {RFC_LOW, 0 }  },
    { (int)RFC_WTR2965_QRD_NON_CA_4373_1_TX_GTR_TH,   { RFC_CONFIG_ONLY, -10 }, {RFC_LOW, 66 }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_gsm_properties_type rfc_wtr2965_qrd_non_ca_4373_1_gsm_properties =
{
  RFC_ENCODED_REVISION,
  /* GSM_DESENSE_CHANNEL (Max: 25 per band) */
  {
    { (uint32)RFCOM_BAND_GSM850 ,  198}, 
    { (uint32)RFCOM_BAND_GSM850 ,  203}, 
    { (uint32)RFCOM_BAND_GSM900 ,  29}, 
    { (uint32)RFCOM_BAND_GSM900 ,  60}, 
    { (uint32)RFCOM_BAND_GSM900 ,  62}, 
    { (uint32)RFCOM_BAND_GSM1800 ,  606}, 
    { (uint32)RFCOM_BAND_GSM1800 ,  654}, 
    { (uint32)RFCOM_BAND_GSM1800 ,  702}, 
    { (uint32)RFCOM_BAND_GSM1800 ,  798}, 
    { (uint32)RFCOM_BAND_GSM1900 ,  653}, 
    { (uint32)RFCOM_BAND_GSM1900 ,  749}, 
    { (uint32)RFCOM_NUM_GSM_BANDS, RFC_INVALID_PARAM },
  },
};

























