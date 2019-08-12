
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/rf_card/rfc_wtr4905_na_dlca/lte/src/rfc_wtr4905_na_dlca_lte_config_data_ag.c#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_wtr4905_na_dlca_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 
#include "wtr4905_typedef_ag.h" 
#include "rfdevice_qtuner_api.h" 

rfc_device_info_type rf_card_wtr4905_na_dlca_init_lte_rx_device_info = 
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_init_lte_rx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_init_lte_tx_device_info = 
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_init_lte_tx_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND1_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (7)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND1_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND1_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (28)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (21)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (17)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (7)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b2_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND2_PMB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      8,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b2_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b2_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND2_DMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      11,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b2_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b2_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND2_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (29)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (22)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (20)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b2_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b2_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND2_PMB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      8,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b2_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b2_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND2_DMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      11,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b2_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b2_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  2,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND2_PMB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      8,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b2_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b2_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  3,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND2_DMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      11,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b2_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b4_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND4_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b4_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b4_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND4_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b4_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b4_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND4_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (31)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (21)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (19)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b4_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b4_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND4_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b4_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b4_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND4_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b4_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b4_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  2,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND4_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b4_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b4_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  3,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND4_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b4_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b5_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND5_PLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b5_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_17,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b5_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND5_DLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b5_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b5_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND5_THMLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (24)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (8)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b5_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b5_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND5_PLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b5_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_17,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b5_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND5_DLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b5_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b7_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND7_PHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b7_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b7_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND7_DHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b7_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b7_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND7_THMLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE2340, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (25)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b7_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b8_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND8_PLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b8_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_04,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b8_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND8_DLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (7)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b8_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b8_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND8_THMLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (7)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (24)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b8_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b10_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND10_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b10_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b10_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND10_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b10_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b10_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND10_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (37)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (21)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (19)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b10_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b12_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND12_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b12_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b12_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND12_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b12_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b12_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND12_THMLB5,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      GEN_PA /* B17_PA */, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x1B << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (12)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b12_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b12_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND12_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b12_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b12_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND12_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b12_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b13_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND13_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b13_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_15,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b13_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND13_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b13_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b13_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND13_THMLB4,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      GEN_PA /* B13_PA */, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x19 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (12)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b13_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b13_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND13_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b13_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_15,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b13_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND13_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b13_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b17_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND17_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b17_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b17_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND17_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b17_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b17_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND17_THMLB5,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_LUT_PWR_INVALID << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      GEN_PA /* B17_PA */, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x1B << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (12)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b17_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b17_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND17_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b17_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_07,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b17_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND17_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b17_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b23_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND23_PMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      9,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b23_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b23_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND23_DMB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      12,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x1B0 << 22)/*mfg_id*/ | (0x2 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b23_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b23_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND23_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (32)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (21)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (16)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b23_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b25_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND25_PMB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      8,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b25_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b25_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND25_DMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      11,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b25_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b25_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND25_THMLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (33)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (22)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (20)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b25_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_01,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b25_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND25_PMB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_MB, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      8,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b25_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_11,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b25_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND25_DMB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_MB_W0, /* NAME */
      5,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      11,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x83 << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b25_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b26_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND26_PLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b26_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_17,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b26_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND26_DLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b26_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b26_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  7,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND26_THMLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE3320_EPT, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (13)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (23)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_TX, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (8)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
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
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b26_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b26_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND26_PLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b26_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_17,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b26_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND26_DLB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (6)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b26_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b29_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND29_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b29_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_15,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b29_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND29_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b29_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b29_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  0,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND29_PLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE3320_LB, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x26 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_PRX */, /* NAME */
      10,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (0)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b29_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_15,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b29_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  1,   /* NV Container */
  1,   /* Antenna */
  4,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND29_DLB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_LB_W0, /* NAME */
      4,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP4T_DRX */, /* NAME */
      13,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x134 << 22)/*mfg_id*/ | (0x2C << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b29_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b30_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND30_PHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b30_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b30_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND30_DHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b30_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b30_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_TX_BAND30_THMLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE2340, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (25)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (4)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b30_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b30_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND30_PHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b30_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b30_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND30_DHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b30_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx2_lte_b30_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_0,   /* Modem Chain */
  2,   /* NV Container */
  2,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_PRXLGY1_BAND30_PHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (3)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx2_lte_b30_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx3_lte_b30_alt1_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */
  3,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTEFDD_DRXLGY1_BAND30_DHB1,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (2)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx3_lte_b30_alt1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_PRXLGY1_BAND41_PHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_DRXLGY1_BAND41_DHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_TX_BAND41_THMLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE2340, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (25)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_b_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_PRXLGY1_BAND41_PHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_b_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_b_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_DRXLGY1_BAND41_DHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_b_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_b_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_TX_BAND41_THMLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE2340, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (25)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_b_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_c_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_2,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  3,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_PRXLGY1_BAND41_PHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_c_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_09,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_20,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_c_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */
  1,   /* NV Container */
  3,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      1,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_DRXLGY1_BAND41_DHB2,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE1040_HB_W0, /* NAME */
      6,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x10 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_c_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_c_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_TX_MODEM_CHAIN_0,   /* Modem Chain */
  0,   /* NV Container */
  2,   /* Antenna */
  6,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        (int)WTR4905_LTETDD_TX_BAND41_THMLB3,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_VALID | WTR4905_LP_LUT_TYPE << RFDEVICE_PA_STATE_0_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_1_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_2_BSHFT | WTR4905_HP_LUT_TYPE << RFDEVICE_PA_STATE_3_BSHFT  ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_LOW_ATTN_MODE,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_PA,
      QFE2340, /* NAME */
      2,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
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
        (0x217 << 22)/*mfg_id*/ | (0x31 << 14)/*prd_id*/ | (25)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      QFE2340, /* NAME */
      3,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x217 << 22)/*mfg_id*/ | (0x21 << 14)/*prd_id*/ | (5)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* ASM_SP6T */, /* NAME */
      7,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0x20C << 22)/*mfg_id*/ | (0x20 << 14)/*prd_id*/ | (1)/*port_num*/, /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_HDET,
      TRX_HDET, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
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


rfc_sig_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_c_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_TX_GTR_TH,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_PA_IND,   { RFC_CONFIG_ONLY, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal0_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
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
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal0_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal0_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
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
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal0_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal0_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
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
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal0_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal0_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_1,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
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
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal0_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal1_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal1_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal1_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal1_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal1_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal1_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal1_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal1_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_08,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal2_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal2_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal2_fbrx_path_internal_adc_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_INTERNAL_ADC,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal2_fbrx_path_internal_adc_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal2_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_et_cal2_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal2_fbrx_path_drx_iq_device_info = 
{
  RFC_ENCODED_REVISION, 
  RFC_RX_MODEM_CHAIN_3,   /* Modem Chain */        
  0 /* Warning: Not specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not specified */,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      WTR4905, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0,   /* PHY_PATH_NUM */            
      {
        0 /* Warning: Not specified */,   /* INTF_REV */                  
        RFDEVICE_FBRX_PATH_DRX_IQ,   /* PORT */
        ( RFDEVICE_PA_LUT_MAPPING_INVALID ),   /* RF_ASIC_BAND_AGC_LUT_MAPPING */        
        FALSE,   /* TXAGC_LUT */
        WTR4905_FBRX_ATTN_DEFAULT,   /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_et_cal2_fbrx_path_drx_iq_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_05,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_ant_swap0_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */
  0 /* Warning: Not specified */,   /* NV Container */
  1,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      1,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_ant_swap0_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_06,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_14,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_ant_swap0_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */
  0 /* Warning: Not specified */,   /* NV Container */
  0,   /* Antenna */
  1,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TUNER,
      QFE2550, /* NAME */
      0,   /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */,   /* PHY_PATH_NUM */
      {
        0  /* Orig setting:   */,  /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID,  /* DISTORTION_CONFIG */  
        0  /* Orig setting: */,  /* NOTES (REFERENCE ONLY) */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_ant_swap0_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_06,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_14,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_en_ant_swap1_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */
  0 /* Warning: Not specified */,   /* NV Container */
  1,   /* Antenna */
  0,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    /* The following is dummy FILLER information, to keep from having an empty array.
     An empty array causes compilation issues on certain compilers. */ 
    {
      RFDEVICE_TUNER,
      DEFAULT_RFDEVICE,  /* NAME */
      0 /* Warning: Not specified */, /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */, /* PHY_PATH_NUM */
      {
    0 /* Orig setting: */, /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID /* Orig setting: */, /* DISTORTION_CONFIG */
        0 /* Orig setting: HB PRX */, /* NOTES (REFERENCE ONLY) */
        0, /* Array Filler */
        0, /* Array Filler */
        0, /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_en_ant_swap1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_06,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_14,   { RFC_HIGH, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_device_info_type rf_card_wtr4905_na_dlca_lte_dis_ant_swap1_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not specified */,   /* Modem Chain */
  0 /* Warning: Not specified */,   /* NV Container */
  0,   /* Antenna */
  0,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    /* The following is dummy FILLER information, to keep from having an empty array.
     An empty array causes compilation issues on certain compilers. */ 
    {
      RFDEVICE_TUNER,
      DEFAULT_RFDEVICE,  /* NAME */
      0 /* Warning: Not specified */, /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /* Warning: Not specified */, /* PHY_PATH_NUM */
      {
    0 /* Orig setting: */, /* INTF_REV */
        (int)RFDEVICE_QTUNER_IM_DISTORTION_INVALID /* Orig setting: */, /* DISTORTION_CONFIG */
        0 /* Orig setting: HB PRX */, /* NOTES (REFERENCE ONLY) */
        0, /* Array Filler */
        0, /* Array Filler */
        0, /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_wtr4905_na_dlca_lte_dis_ant_swap1_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_06,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_WTR4905_NA_DLCA_RF_PATH_SEL_14,   { RFC_LOW, 0/*Warning: Not specified*/ }, {RFC_LOW, 0/*Warning: Not specified*/ }  },
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


rfc_band_split_info_type rf_card_wtr4905_na_dlca_rx0_lte_b41_split_cfg = 
{
  RFC_ENCODED_REVISION, 
      {
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
    { RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM }
  }
};

rfc_band_split_info_type rf_card_wtr4905_na_dlca_rx1_lte_b41_split_cfg = 
{
  RFC_ENCODED_REVISION, 
      {
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
    { RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM }
  }
};

rfc_band_split_info_type rf_card_wtr4905_na_dlca_tx0_lte_b41_split_cfg = 
{
  RFC_ENCODED_REVISION, 
      {
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 39650, 40040, (int)RFCOM_BAND_LTE_B41, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40041, 40815, (int)RFCOM_BAND_LTE_B41_B, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_3MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_5MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_10MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_15MHZ, 255 },
        { 40816, 41589, (int)RFCOM_BAND_LTE_B41_C, RFC_FILTER_CFG_NOT_APP, RFC_BW_20MHZ, 255 },
    { RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM, RFC_INVALID_PARAM }
  }
};

























