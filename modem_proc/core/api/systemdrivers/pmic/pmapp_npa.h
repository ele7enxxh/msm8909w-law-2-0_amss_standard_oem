#ifndef PMAPP_NPA_H
#define PMAPP_NPA_H

/*! \file  pmapp_npa.h
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: MSM8916_PM8916_NPA_4p01_2014_07_04 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                  P M    H E A D E R    F I L E

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pmapp_npa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/14   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)

===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** @addtogroup pmapp_npa 
@{ */
/*
 * Generic mode IDs that can be used for clients that only use ON /OFF
 * or ACTIVE / STANDBY states.
 */
#define PMIC_NPA_GROUP_ID_5V_SUPPLY "/pmic/client/5v_supply"    /*!< Group ID 5v_supply. */
#define PMIC_NPA_GROUP_ID_ANT1 "/pmic/client/ant1"    /*!< Group ID ant1. */
#define PMIC_NPA_GROUP_ID_AUDIO "/pmic/client/audio"    /*!< Group ID audio. */
#define PMIC_NPA_GROUP_ID_AUDIO_RX "/pmic/client/audio_rx"    /*!< Group ID audio_rx. */
#define PMIC_NPA_GROUP_ID_AUDIO_SPEAKER "/pmic/client/audio_speaker"    /*!< Group ID audio_speaker. */
#define PMIC_NPA_GROUP_ID_AUDIO2 "/pmic/client/audio2"    /*!< Group ID audio2. */
#define PMIC_NPA_GROUP_ID_BT "/pmic/client/bt"    /*!< Group ID bt. */
#define PMIC_NPA_GROUP_ID_CAMERA_BACK "/pmic/client/camera_back"    /*!< Group ID camera_back. */
#define PMIC_NPA_GROUP_ID_CAMERA_FRONT "/pmic/client/camera_front"    /*!< Group ID camera_front. */
#define PMIC_NPA_GROUP_ID_CLK_REGIME_DIG "/pmic/client/clk_regime_dig"    /*!< Group ID clk_regime_dig. */
#define PMIC_NPA_GROUP_ID_CLK_REGIME_DIG2 "/pmic/client/clk_regime_dig2"    /*!< Group ID clk_regime_dig2. */
#define PMIC_NPA_GROUP_ID_CODEC "/pmic/client/codec"    /*!< Group ID codec. */
#define PMIC_NPA_GROUP_ID_CODEC_ANC "/pmic/client/codec_anc"    /*!< Group ID codec_anc. */
#define PMIC_NPA_GROUP_ID_DIG_MIC "/pmic/client/dig_mic"    /*!< Group ID dig_mic. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_DSUB "/pmic/client/disp_ext_dsub"    /*!< Group ID disp_ext_dsub. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_HDMI "/pmic/client/disp_ext_hdmi"    /*!< Group ID disp_ext_hdmi. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_HDMI "/pmic/client/disp_ext_hdmi"    /*!< Group ID disp_ext_hdmi. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_LVDS "/pmic/client/disp_ext_lvds"    /*!< Group ID disp_ext_lvds. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_MHL "/pmic/client/disp_ext_mhl"    /*!< Group ID disp_ext_mhl. */
#define PMIC_NPA_GROUP_ID_DISP_EXT_TVOUT "/pmic/client/disp_ext_tvout"    /*!< Group ID disp_ext_tvout. */
#define PMIC_NPA_GROUP_ID_DISP_PRIM "/pmic/client/disp_prim"    /*!< Group ID disp_prim. */
//#define PMIC_NPA_GROUP_ID_DISP_PRIM "/pmic/client/disp_prim"    /*!< Group ID disp_prim.*/
#define PMIC_NPA_GROUP_ID_DISP_PRIM_DSI "/pmic/client/disp_prim_dsi"    /*!< Group ID disp_prim_dsi. */
#define PMIC_NPA_GROUP_ID_DISP_PRIM_EDP "/pmic/client/disp_prim_edp"    /*!< Group ID disp_prim_edp. */
#define PMIC_NPA_GROUP_ID_DISP_SEC "/pmic/client/disp_sec"    /*!< Group ID disp_sec. */
#define PMIC_NPA_GROUP_ID_DISP_SEC "/pmic/client/disp_sec"    /*!< Group ID disp_sec. */
#define PMIC_NPA_GROUP_ID_EMMC "/pmic/client/emmc"    /*!< Group ID emmc. */
#define PMIC_NPA_GROUP_ID_FM "/pmic/client/fm"    /*!< Group ID fm. */
#define PMIC_NPA_GROUP_ID_FUSE "/pmic/client/fuse"    /*!< Group ID fuse. */
#define PMIC_NPA_GROUP_ID_GPS1 "/pmic/client/gps1"    /*!< Group ID gps1. */
#define PMIC_NPA_GROUP_ID_GPS2 "/pmic/client/gps2"    /*!< Group ID gps2. */
#define PMIC_NPA_GROUP_ID_GPS3 "/pmic/client/gps3"    /*!< Group ID gps3. */
#define PMIC_NPA_GROUP_ID_HFPLL1 "/pmic/client/hfpll1"    /*!< Group ID hfpll1. */
#define PMIC_NPA_GROUP_ID_HKADC "/pmic/client/hkadc"    /*!< Group ID hkadc. */
#define PMIC_NPA_GROUP_ID_INVALID "/pmic/client/invalid"    /*!< Group ID invalid. */
#define PMIC_NPA_GROUP_ID_MEM_UVOL "/pmic/client/vdd_mem_uvol"    /*!< Group ID vdd_mem_uvol. */
#define PMIC_NPA_GROUP_ID_MCPM "/pmic/client/mcpm"    /*!< Group ID mcpm. */
#define PMIC_NPA_GROUP_ID_MCPM_DIG "/pmic/client/mcpm_dig"    /*!< Group ID mcpm_dig. */
#define PMIC_NPA_GROUP_ID_MCPM_MEM "/pmic/client/mcpm_mem"    /*!< Group ID mcpm_mem. */
#define PMIC_NPA_GROUP_ID_MCPM1 "/pmic/client/mcpm1"    /*!< Group ID mcpm1. */
#define PMIC_NPA_GROUP_ID_MCPM2 "/pmic/client/mcpm2"    /*!< Group ID mcpm2. */
#define PMIC_NPA_GROUP_ID_MEM "/pmic/client/mem"    /*!< Group ID mem. */
#define PMIC_NPA_GROUP_ID_NFC "/pmic/client/nfc"    /*!< Group ID nfc. */
#define PMIC_NPA_GROUP_ID_PA1 "/pmic/client/pa1"    /*!< Group ID pa1. */
#define PMIC_NPA_GROUP_ID_PA2 "/pmic/client/pa2"    /*!< Group ID pa2. */
#define PMIC_NPA_GROUP_ID_PLL_HF "/pmic/client/pll_hf"    /*!< Group ID pll_hf. */
#define PMIC_NPA_GROUP_ID_PLL_SR2 "/pmic/client/pll_sr2"    /*!< Group ID pll_sr2. */
#define PMIC_NPA_GROUP_ID_PLL_TS "/pmic/client/pll_ts"
#define PMIC_NPA_GROUP_ID_QFPROM "/pmic/client/qfprom"    /*!< Group ID qfprom. */
#define PMIC_NPA_GROUP_ID_QFPROM1 "/pmic/client/qfprom1"    /*!< Group ID qfprom1. */
#define PMIC_NPA_GROUP_ID_RAIL_CX "/pmic/client/rail_cx"    /*!< Group ID rail_cx. */
#define PMIC_NPA_GROUP_ID_RAIL_GFX "/pmic/client/rail_gfx"    /*!< Group ID rail_gfx. */
#define PMIC_NPA_GROUP_ID_RAIL_MSS "/pmic/client/rail_mss"    /*!< Group ID rail_mss. */
#define PMIC_NPA_GROUP_ID_RAIL_MX "/pmic/client/rail_mx"    /*!< Group ID rail_mx. */
#define PMIC_NPA_GROUP_ID_RF "/pmic/client/rf"    /*!< Group ID rf. */
#define PMIC_NPA_GROUP_ID_RF_GPS "/pmic/client/rf_gps"    /*!< Group ID rf_gps. */
#define PMIC_NPA_GROUP_ID_RF1 "/pmic/client/rf1"    /*!< Group ID rf1. */
#define PMIC_NPA_GROUP_ID_RF1_GPS "/pmic/client/rf1_gps"    /*!< Group ID rf1_gps. */
#define PMIC_NPA_GROUP_ID_RF1_GPS1 "/pmic/client/rf1_gps1"    /*!< Group ID rf1_gps1. */
#define PMIC_NPA_GROUP_ID_RF1_GPS2 "/pmic/client/rf1_gps2"    /*!< Group ID rf1_gps2. */
#define PMIC_NPA_GROUP_ID_RF1_GPS3 "/pmic/client/rf1_gps3"    /*!< Group ID rf1_gps3. */
#define PMIC_NPA_GROUP_ID_RF1_NO_SV "/pmic/client/rf1_no_sv"    /*!< Group ID rf1_no_sv. */
#define PMIC_NPA_GROUP_ID_RF1_SV "/pmic/client/rf1_sv"    /*!< Group ID rf1_sv. */
#define PMIC_NPA_GROUP_ID_RF1_MBB "/pmic/client/rf1_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_1X "/pmic/client/rf1_tech_1x"    /*!< Group ID rf1_tech_1x. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB "/pmic/client/rf1_tech_1x_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_DO "/pmic/client/rf1_tech_do"    /*!< Group ID rf1_tech_do. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_DO_MBB "/pmic/client/rf1_tech_do_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GPS "/pmic/client/rf1_tech_gps"    /*!< Group ID rf1_tech_gps. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_GPS_ELNA "/pmic/client/rf1_tech_gps_elna"    /*!< Group ID rf1_tech_gps_elna. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_GPS_ELNA2 "/pmic/client/rf1_tech_gps_elna2"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GPS_ELNA3 "/pmic/client/rf1_tech_gps_elna3"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GSM "/pmic/client/rf1_tech_gsm"    /*!< Group ID rf1_tech_gsm. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_GSM_MBB "/pmic/client/rf1_tech_gsm_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GSM2 "/pmic/client/rf1_tech_gsm2"    /*!< Group ID rf1_tech_gsm2. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_GSM3 "/pmic/client/rf1_tech_gsm3"    /*!< Group ID rf1_tech_gsm3. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_INIT "/pmic/client/rf1_tech_init"
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE "/pmic/client/rf1_tech_lte"    /*!< Group ID rf1_tech_lte. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE_MBB "/pmic/client/rf1_tech_lte_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE2 "/pmic/client/rf1_tech_lte2"
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE2_MBB "/pmic/client/rf1_tech_lte2_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA "/pmic/client/rf1_tech_tdscdma"    /*!< Group ID rf1_tech_tdscdma. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA_MBB "/pmic/client/rf1_tech_tdscdma_mbb"
#define PMIC_NPA_GROUP_ID_RF1_TECH_WCDMA "/pmic/client/rf1_tech_wcdma"    /*!< Group ID rf1_tech_wcdma. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_WCDMA_MBB "/pmic/client/rf1_tech_wcdma_mbb"
#define PMIC_NPA_GROUP_ID_RF2_TECH_1X "/pmic/client/rf2_tech_1x"    /*!< Group ID rf2_tech_1x. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_DO "/pmic/client/rf2_tech_do"    /*!< Group ID rf2_tech_do. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_GPS "/pmic/client/rf2_tech_gps"    /*!< Group ID rf2_tech_gps. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_GSM "/pmic/client/rf2_tech_gsm"    /*!< Group ID rf2_tech_gsm. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_GSM2 "/pmic/client/rf2_tech_gsm2"    /*!< Group ID rf2_tech_gsm2. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_GSM3 "/pmic/client/rf2_tech_gsm3"    /*!< Group ID rf2_tech_gsm3. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_LTE "/pmic/client/rf2_tech_lte"    /*!< Group ID rf2_tech_lte. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_LTE2 "/pmic/client/rf2_tech_lte2"
#define PMIC_NPA_GROUP_ID_RF2_TECH_TDSCDMA "/pmic/client/rf2_tech_tdscdma"    /*!< Group ID rf2_tech_tdscdma. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_WCDMA "/pmic/client/rf2_tech_wcdma"    /*!< Group ID rf2_tech_wcdma. */
#define PMIC_NPA_GROUP_ID_RF2 "/pmic/client/rf2"    /*!< Group ID rf2. */
#define PMIC_NPA_GROUP_ID_RF2_2 "/pmic/client/rf2_2"    /*!< Group ID rf2_2. */
#define PMIC_NPA_GROUP_ID_RF2_GPS1 "/pmic/client/rf2_gps1"    /*!< Group ID rf2_gps1. */
#define PMIC_NPA_GROUP_ID_RF2_GPS2 "/pmic/client/rf2_gps2"    /*!< Group ID rf2_gps2. */
#define PMIC_NPA_GROUP_ID_RF2_GPS3 "/pmic/client/rf2_gps3"    /*!< Group ID rf2_gps3. */
#define PMIC_NPA_GROUP_ID_RF2_NO_SV "/pmic/client/rf2_no_sv"    /*!< Group ID rf2_no_sv. */
#define PMIC_NPA_GROUP_ID_RF2_SV "/pmic/client/rf2_sv"    /*!< Group ID rf2_sv. */
#define PMIC_NPA_GROUP_ID_RF3_2 "/pmic/client/rf3_2"    /*!< Group ID rf3_2. */
#define PMIC_NPA_GROUP_ID_RF3_GPS1 "/pmic/client/rf3_gps1"    /*!< Group ID rf3_gps1. */
#define PMIC_NPA_GROUP_ID_RF3_NO_SV "/pmic/client/rf3_no_sv"    /*!< Group ID rf3_no_sv. */
#define PMIC_NPA_GROUP_ID_RPM_INIT "/pmic/client/rpm_init"    /*!< Group ID rpm_init. */
#define PMIC_NPA_GROUP_ID_SD "/pmic/client/sd"    /*!< Group ID sd. */
#define PMIC_NPA_GROUP_ID_SENSOR_DSP_PROXY "/pmic/client/sensor_dsp_proxy"    /*!< Group ID sensor_dsp_proxy. */
//#define PMIC_NPA_GROUP_ID_SENSOR_DSP_PROXY "/pmic/client/sensor_dsp_proxy"    /*!< Group ID sensor_dsp_proxy. */
#define PMIC_NPA_GROUP_ID_SENSOR_DSP_VDD_MODE "/pmic/client/sensor_dsp_vdd_mode"    /*!< Group ID sensor_dsp_vdd_mode. */
#define PMIC_NPA_GROUP_ID_SENSOR_VDD "/pmic/client/sensor_vdd"    /*!< Group ID sensor_vdd. */
#define PMIC_NPA_GROUP_ID_SENSOR_VDDIO "/pmic/client/sensor_vddio"    /*!< Group ID sensor_vddio. */
#define PMIC_NPA_GROUP_ID_SLIMBUS "/pmic/client/slimbus"    /*!< Group ID slimbus. */
#define PMIC_NPA_GROUP_ID_TEST_CLIENT1 "/pmic/client/test_client1"    /*!< Group ID test_client1. */
#define PMIC_NPA_GROUP_ID_TEST_CLIENT2 "/pmic/client/test_client2"    /*!< Group ID test_client2. */
#define PMIC_NPA_GROUP_ID_TEST_CLIENT3 "/pmic/client/test_client3"    /*!< Group ID test_client3. */
#define PMIC_NPA_GROUP_ID_TSCREEN "/pmic/client/tscreen"    /*!< Group ID tscreen. */
#define PMIC_NPA_GROUP_ID_TSCREEN1 "/pmic/client/tscreen1"    /*!< Group ID tscreen1. */
#define PMIC_NPA_GROUP_ID_TSCREEN2 "/pmic/client/tscreen2"    /*!< Group ID tscreen2. */
#define PMIC_NPA_GROUP_ID_TSCREEN3 "/pmic/client/tscreen3"    /*!< Group ID tscreen3. */
#define PMIC_NPA_GROUP_ID_UIM1_ACTIVITY "/pmic/client/uim1_activity"    /*!< Group ID uim1_activity. */
#define PMIC_NPA_GROUP_ID_UIM1_ENABLE "/pmic/client/uim1_enable"    /*!< Group ID uim1_enable. */
#define PMIC_NPA_GROUP_ID_UIM2_ACTIVITY "/pmic/client/uim2_activity"    /*!< Group ID uim2_activity. */
#define PMIC_NPA_GROUP_ID_UIM2_ENABLE "/pmic/client/uim2_enable"    /*!< Group ID uim2_enable. */
#define PMIC_NPA_GROUP_ID_UIM3_ACTIVITY "/pmic/client/uim3_activity"    /*!< Group ID uim3_activity. */
#define PMIC_NPA_GROUP_ID_UIM3_ENABLE "/pmic/client/uim3_enable"    /*!< Group ID uim3_enable. */
#define PMIC_NPA_GROUP_ID_RF "/pmic/client/rf"    /*!< Group ID rf. */
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE2 "/pmic/client/rf1_tech_lte2"    /*!< Group ID rf1_tech_lte2. */
#define PMIC_NPA_GROUP_ID_RF2_TECH_LTE2 "/pmic/client/rf2_tech_lte2"    /*!< Group ID rf1_tech_lte2. */
#define PMIC_NPA_GROUP_ID_UIM1 "/pmic/client/uim1"    /*!< Group ID uim1. */
#define PMIC_NPA_GROUP_ID_UIM2 "/pmic/client/uim2"    /*!< Group ID uim2. */
#define PMIC_NPA_GROUP_ID_USB_CHG "/pmic/client/usb_chg"    /*!< Group ID usb_chg. */
#define PMIC_NPA_GROUP_ID_USB_FS1 "/pmic/client/usb_fs1"    /*!< Group ID usb_fs1. */
#define PMIC_NPA_GROUP_ID_USB_FS2 "/pmic/client/usb_fs2"    /*!< Group ID usb_fs2. */
#define PMIC_NPA_GROUP_ID_USB_HS1 "/pmic/client/usb_hs1"    /*!< Group ID usb_hs1. */
#define PMIC_NPA_GROUP_ID_USB_HS2 "/pmic/client/usb_hs2"    /*!< Group ID usb_hs2. */
#define PMIC_NPA_GROUP_ID_USB_HS3 "/pmic/client/usb_hs3"    /*!< Group ID usb_hs3. */
#define PMIC_NPA_GROUP_ID_USB_HSIC "/pmic/client/usb_hsic"    /*!< Group ID usb_hsic. */
#define PMIC_NPA_GROUP_ID_USB_SS1 "/pmic/client/usb_ss1"    /*!< Group ID usb_ss1. */
#define PMIC_NPA_GROUP_ID_WCN1 "/pmic/client/wcn1"    /*!< Group ID wcn1. */
//#define PMIC_NPA_GROUP_ID_WCN1 "/pmic/client/wcn1"    /*!< Group ID wcn1. */
#define PMIC_NPA_GROUP_ID_WCN2 "/pmic/client/wcn2"    /*!< Group ID wcn2. */
//#define PMIC_NPA_GROUP_ID_WCN2 "/pmic/client/wcn2"    /*!< Group ID wcn2. */
#define PMIC_NPA_GROUP_ID_WGR1 "/pmic/client/wgr1"    /*!< Group ID wgr1. */
#define PMIC_NPA_GROUP_ID_WLAN "/pmic/client/wlan"    /*!< Group ID wlan. */
#define PMIC_NPA_GROUP_ID_MEM_UVOL "/pmic/client/vdd_mem_uvol"
#define PMIC_NPA_GROUP_ID_XO_SHUTDOWN "/pmic/client/xo_shutdown"    /*!< Group ID xo_shutdown. @newpage */
#define PMIC_NPA_GROUP_ID_SENSOR_VDD "/pmic/client/sensor_vdd"
#define PMIC_NPA_GROUP_ID_SENSOR_VDD_2 "/pmic/client/sensor_vdd_2"
#define PMIC_NPA_GROUP_ID_SENSOR_VDDIO "/pmic/client/sensor_vddio"
#define PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2 "/pmic/client/sensor_vddio_2"

/**
 * @anonenum{Generic}
 * Generic mode.
 */
enum
{
   PMIC_NPA_MODE_ID_GENERIC_STANDBY, /**< Standby. */
   PMIC_NPA_MODE_ID_GENERIC_ACTIVE,  /**< Active. */
   PMIC_NPA_MODE_ID_GENERIC_MAX,     /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{GPS Operating}
 * GPS operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_GPS_MODE_0,   /**< Mode 0. */
   PMIC_NPA_MODE_ID_GPS_MODE_1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_GPS_MODE_2,   /**< Mode 2. */
   PMIC_NPA_MODE_ID_GPS_MODE_3,   /**< Mode 3. */
   PMIC_NPA_MODE_ID_GPS_MODE_4,   /**< Mode 4. */
   PMIC_NPA_MODE_ID_GPS_MODE_5,   /**< Mode 5. */
   PMIC_NPA_MODE_ID_GPS_MODE_6,   /**< Mode 6. */
   PMIC_NPA_MODE_ID_GPS_MODE_7,   /**< Mode 7. */
   PMIC_NPA_MODE_ID_GPS_MODE_8,   /**< Mode 8. */
   PMIC_NPA_MODE_ID_GPS_MODE_MAX, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Core Rail Operating}
 * Core rail operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_CORE_RAIL_OFF,          /**< OFF. */
   PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION,    /**< Retention. */
   PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION_PLUS, /**< Retention PLus. */
   PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS,    /**< Low minus. */
   PMIC_NPA_MODE_ID_CORE_RAIL_LOW,          /**< Low. */
   PMIC_NPA_MODE_ID_CORE_RAIL_LOW_PLUS,       /**< Low Plus. */
   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL,      /**< Nominal. */
   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS, /**< Nominal plus. */
   PMIC_NPA_MODE_ID_CORE_RAIL_TURBO,        /**< Turbo. */
   PMIC_NPA_MODE_ID_CORE_RAIL_MAX,          /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{UIM Operating}
 * UIM operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_UIM_MODE_1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_UIM_MODE_2,   /**< Mode 2. */
   PMIC_NPA_MODE_ID_UIM_MODE_3,   /**< Mode 3. */
   PMIC_NPA_MODE_ID_UIM_MODE_4,   /**< Mode 4. */
   PMIC_NPA_MODE_ID_UIM_MODE_5,   /**< Mode 5. */
   PMIC_NPA_MODE_ID_UIM_MODE_6,   /**< Mode 6. */
   PMIC_NPA_MODE_ID_UIM_MODE_7,   /**< Mode 7. */
   PMIC_NPA_MODE_ID_UIM_MODE_8,   /**< Mode 8. */
   PMIC_NPA_MODE_ID_UIM_MODE_9,   /**< Mode 9. */
   PMIC_NPA_MODE_ID_UIM_MODE_10,  /**< Mode 11. */
   PMIC_NPA_MODE_ID_UIM_MODE_11,  /**< Mode 12. */
   PMIC_NPA_MODE_ID_UIM_MODE_12,  /**< Mode 13. */
   PMIC_NPA_MODE_ID_UIM_MODE_13,  /**< Mode 14. */
   PMIC_NPA_MODE_ID_UIM_MODE_MAX, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Antenna}
 * Antenna mode.
 */
enum
{
   PMIC_NPA_MODE_ID_ANT_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_ANT_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_ANT_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_ANT_MODE_3 = 3, /**< Mode 3. */
   PMIC_NPA_MODE_ID_ANT_MAX  = 3    /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Audio Activity}
 * Audio activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_AUDIO_STANDBY = 0,     /**< Standby. */
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_LOW = 1,  /**< Active low. */
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED = 2,  /**< Active medium. */
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_HIGH = 3, /**< Active high. */
   PMIC_NPA_MODE_ID_AUDIO_MAX = 4,         /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Bluetooth Activity}
 * Bluetooth\reg activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_BT_STANDBY = 0, /**< Standby. */
   PMIC_NPA_MODE_ID_BT_ACT_SOC = 1, /**< SOC activity. */
   PMIC_NPA_MODE_ID_BT_ACT_ALL = 2, /**< All activity. */
   PMIC_NPA_MODE_ID_BT_MAX = 3,     /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Clock Regime Diagnostic}
 * Clock regime diagnostic mode.
 */
enum
{
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_LOW_DIG = 0,    /**< Digital low digital. */
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NORMAL_DIG = 1, /**< Digital normal digital. */
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_HIGH_DIG = 2,   /**< Digital high digital. */
   PMIC_NPA_MODE_ID_CLK_REGIME_VDD_DIG_MIN = 3,    /**< Vdd minimum digital. */
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_MAX = 4,        /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Clock Regime Operating}
 * Clock regime operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_3 = 3, /**< Mode 3. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_4 = 4, /**< Mode 4. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_5 = 5, /**< Mode 5. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_6 = 6, /**< Mode 6. */
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_7 = 7, /**< Mode 7. */
};

/**
 * @anonenum{Codec Path}
 * Codec path.
 */
enum
{
   PMIC_NPA_MODE_ID_CODEC_PATH_OFF = 0, /**< OFF. */
   PMIC_NPA_MODE_ID_CODEC_1PATH_ON = 1, /**< Path 1 ON. */
   PMIC_NPA_MODE_ID_CODEC_2PATH_ON = 2, /**< Path 2 ON. */
   PMIC_NPA_MODE_ID_CODEC_3PATH_ON = 3, /**< Path 3 ON. */
   PMIC_NPA_MODE_ID_CODEC_4PATH_ON = 4, /**< Path 4 ON. */
   PMIC_NPA_MODE_ID_CODEC_MAX = 5,      /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Corner level}
 * Corner level.
 */
enum
{
   PMIC_NPA_MODE_ID_CORNER_LEVEL_NO_VOTE = 0, /**< No vote. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_1 = 1,       /**< Level 1. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_2 = 2,       /**< Level 2. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_3 = 3,       /**< Level 3. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_4 = 4,       /**< Level 4. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_5 = 5,       /**< Level 5. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_6 = 6,       /**< Level 6. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_7 = 7,       /**< Level 7. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_8 = 8,       /**< Level 8. */
   PMIC_NPA_MODE_ID_CORNER_LEVEL_MAX = 9,     /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{eMMC Activity}
 * eMMC activity mode.
 */
enum
{
   PMIC_NPA_GROUP_ID_eMMC_OFF = 0,    /**< OFF. */
   PMIC_NPA_GROUP_ID_eMMC_ACTIVE = 1, /**< Active. */
   PMIC_NPA_GROUP_ID_eMMC_SLEEP = 2,  /**< Sleep. */
   PMIC_NPA_GROUP_ID_eMMC_MAX = 3,    /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Activity}
 * Activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_OFF = 0,             /**< OFF. */
   PMIC_NPA_MODE_ID_STANDBY = 1,         /**< Standby. */
   PMIC_NPA_MODE_ID_ACTIVE_NOM_LOW = 2,  /**< Active nominal low. */
   PMIC_NPA_MODE_ID_ACTIVE_SS_LOW = 3,   /**< Active SS low. */
   PMIC_NPA_MODE_ID_ACTIVE_NOM_HIGH = 4, /**< Active nominal high. */
   PMIC_NPA_MODE_ID_ACTIVE_SS_HIGH = 5,  /**< Active SS high. */
   PMIC_NPA_MODE_ID_NOM_HIGH_RX = 6,     /**< Nominal high Rx. */
   PMIC_NPA_MODE_ID_MAX = 7,             /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{HKADC Activity}
 * HKADC activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_HKADC_OFF = 0,    /**< OFF. */
   PMIC_NPA_MODE_ID_HKADC_ACTIVE = 1, /**< Active. */
   PMIC_NPA_MODE_ID_HKADC_MAX = 2,    /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Activity Operating}
 * Activity operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_MODE_3 = 3, /**< Mode 3. */
   PMIC_NPA_MODE_ID_MODE_4 = 4, /**< Mode 4. */
   PMIC_NPA_MODE_ID_MODE_5 = 5, /**< Mode 5. */
   PMIC_NPA_MODE_ID_MODE_6 = 6, /**< Mode 6. */
   PMIC_NPA_MODE_ID_MODE_MAX = 7, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Mode Invalid}
 * Mode invalid.
 */
enum
{
   PMIC_NPA_MODE_ID_INVALID = 0, /**< Mode is invalid. */
};

/**
 * @anonenum{MCPM Diagnostic}
 * MCPM diagnostic mode.
 */
enum
{
   PMIC_NPA_MODE_ID_MCPM_LOW_DIG = 0,    /**< Low digital. */
   PMIC_NPA_MODE_ID_MCPM_NORMAL_DIG = 1, /**< Nominal digital. */
   PMIC_NPA_MODE_ID_MCPM_HIGH_DIG = 2,   /**< High digital. */
   PMIC_NPA_MODE_ID_MCPM_DIG_MIN = 3,    /**< Minimum digital. */
   PMIC_NPA_MODE_ID_MCPM_DIG_MAX = 4,    /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{MCPM Operating}
 * MCPM operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_MCPM_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_MCPM_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_MCPM_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_MCPM_MODE_3 = 3, /**< Mode 3. */
   PMIC_NPA_MODE_ID_MCPM_MODE_4 = 4, /**< Mode 4. */
   PMIC_NPA_MODE_ID_MCPM_MODE_5 = 5, /**< Mode 5. */
   PMIC_NPA_MODE_ID_MCPM_MODE_6 = 6, /**< Mode 6. */
   PMIC_NPA_MODE_ID_MCPM_MODE_7 = 7, /**< Mode 7. */
   PMIC_NPA_MODE_ID_MCPM_MODE_8 = 8, /**< Mode 8. */
   PMIC_NPA_MODE_ID_MCPM_MODE_MAX = 9, /**< Maximum value for this enumeration. @newpage */
};

/**
 * @anonenum{Memory Activity}
 * Memory activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_MEM_DEEP_STANDBY = 0, /**< Deep standby. */
   PMIC_NPA_MODE_ID_MEM_SELF_REFRESH = 1, /**< Self refresh. */
   PMIC_NPA_MODE_ID_MEM_ACTIVE = 2,       /**< Active. */
   PMIC_NPA_MODE_ID_MEM_MAX = 3,          /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{PA}
 * PA mode.
 */
enum
{
   PMIC_NPA_MODE_ID_PA_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_PA_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_PA_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_PA_MODE_3 = 3, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{PLL Operating}
 * PLL operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_PLL_MODE_0 = 0, /**< Mode 0. */
   PMIC_NPA_MODE_ID_PLL_MODE_1 = 1, /**< Mode 1. */
   PMIC_NPA_MODE_ID_PLL_MODE_2 = 2, /**< Mode 2. */
   PMIC_NPA_MODE_ID_PLL_MODE_3 = 3, /**< Mode 3. */
   PMIC_NPA_MODE_ID_PLL_MODE_4 = 4, /**< Mode 4. */
   PMIC_NPA_MODE_ID_PLL_MODE_5 = 5, /**< Mode 5. */
   PMIC_NPA_MODE_ID_PLL_MODE_MAX = 6, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{RF}
 * RF mode.
 */
enum
{
   PMIC_NPA_MODE_ID_RF_SLEEP = 0,                      /**< Sleep. */
   PMIC_NPA_MODE_ID_RF_UMTS_NOMINAL = 1,               /**< UMTS nominal. */
   PMIC_NPA_MODE_ID_RF_AUTOCAL = 1,                    /**< Autocalibration. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_C2K_RX = 2,             /**< DVS low C2K Rx. */
   PMIC_NPA_MODE_ID_RF_1X_NOMINAL = 2,                 /**< 1X nominal. */
   PMIC_NPA_MODE_ID_RF_UMTS_SS = 3,                    /**< UMTS SS. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_C2K_RX = 3,            /**< DVS high C2K Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_C2K_RXTX = 4,           /**< DVS low C2K RxTx. */
   PMIC_NPA_MODE_ID_RF_1X_SS = 4,                      /**< 1X SS. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_C2K_RXTX = 5,          /**< DVS high C2K RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_UMTS_RX = 6,            /**< DVS low UMTS Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_UMTS_RX = 7,           /**< DVS high UMTS Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_UMTS_RXTX = 8,          /**< DVS low UMTS RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_UMTS_RXTX = 9,         /**< DVS high UMTS RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_LTE_EVDO_RX = 10,      /**< DVS high LTE EVDO Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_LTE_EVDO_RX = 11,       /**< DVS low LTE EVDO Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_LTE_EVDO_RXTX = 12,    /**< DVS high LTE EVDO RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_LTE_EVDO_RXTX = 13,     /**< DVS low LTE EVDO RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_SV_LTE_EVDO_RX = 14,   /**< DVS high SV LTE EVDO Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_SV_LTE_EVDO_RX = 15,    /**< DVS low SV LTE EVDO Rx. */
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_SV_LTE_EVDO_RXTX = 16, /**< DVS high SV LTE EVDO RxTx. */
   PMIC_NPA_MODE_ID_RF_DVS_LOW_SV_LTE_EVDO_RXTX = 17,  /**< DVS low SV LTE EVDO RxTx. */
   PMIC_NPA_MODE_ID_RF_MAX = 18,                      /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{GPS}
 * GPS mode.
 */
enum
{
   PMIC_NPA_MODE_ID_RF_GPS_EXIT = 0,       /**< Exit. */
   PMIC_NPA_MODE_ID_RF_GPS_SLEEP = 1,      /**< Sleep. */
   PMIC_NPA_MODE_ID_RF_GPS_ON_NOMINAL = 2, /**< ON nominal. */
   PMIC_NPA_MODE_ID_RF_GPS_ON_SS = 3,      /**< ON SS. */
   PMIC_NPA_MODE_ID_RF_GPS_MAX = 4,        /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{RF Operating}
 * RF operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_RF_MODE_0 = 0,    /**< Mode 0. */
   PMIC_NPA_MODE_ID_RF_MODE_1 = 1,    /**< Mode 1. */
   PMIC_NPA_MODE_ID_RF_MODE_2 = 2,    /**< Mode 2. */
   PMIC_NPA_MODE_ID_RF_MODE_3 = 3,    /**< Mode 3. */
   PMIC_NPA_MODE_ID_RF_MODE_4 = 4,    /**< Mode 4. */
   PMIC_NPA_MODE_ID_RF_MODE_5 = 5,    /**< Mode 5. */
   PMIC_NPA_MODE_ID_RF_MODE_6 = 6,    /**< Mode 6. */
   PMIC_NPA_MODE_ID_RF_MODE_7 = 7,    /**< Mode 7. */
   PMIC_NPA_MODE_ID_RF_MODE_8 = 8,    /**< Mode 8. */
   PMIC_NPA_MODE_ID_RF_MODE_9 = 9,    /**< Mode 9. */
   PMIC_NPA_MODE_ID_RF_MODE_MAX = 10, /**< Maximum value for this enumeration. @newpage */
};

/**
 * @anonenum{RF1 GPS}
 * RF1 GPS mode.
 */
enum
{
   PMIC_NPA_MODE_ID_RF1_GPS_EXIT = 0,        /**< Exit. */
   PMIC_NPA_MODE_ID_RF1_GPS_SLEEP = 1,       /**< Sleep. */
   PMIC_NPA_MODE_ID_RF1_GPS_DVS_HIGH_RX = 2, /**< DVS high Rx. */
   PMIC_NPA_MODE_ID_RF1_GPS_DVS_LOW_RX = 3,  /**< DVS low Rx. */
   PMIC_NPA_MODE_ID_RF1_GPS_MAX = 4,         /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{RPM Initialization}
 * RPM initialization mode.
 */
enum
{
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_0 = 0,   /**< Mode 0. */
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_1 = 1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_MAX = 2, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{SD Activity}
 * SD activity mode.
 */
enum
{
   PMIC_NPA_GROUP_ID_SD_OFF = 0,    /**< OFF. */
   PMIC_NPA_GROUP_ID_SD_ACTIVE = 1, /**< Active. */
   PMIC_NPA_GROUP_ID_SD_MAX = 2,    /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{SD Operating}
 * SD operating mode.
 */
enum
{
   PMIC_NPA_MODE_ID_SD_MODE_OFF = 0, /**< OFF. */
   PMIC_NPA_MODE_ID_SD_MODE_1 = 1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_SD_MODE_2 = 2,   /**< Mode 2. */
   PMIC_NPA_MODE_ID_SD_MODE_MAX = 3, /**< Maximum value for this enumeration. @newpage */
};

/**
 * @anonenum{Sensor Power}
 * Sensor power mode.
 */
enum
{
   PMIC_NPA_MODE_ID_SENSOR_POWER_OFF = 0, /**< OFF. */
   PMIC_NPA_MODE_ID_SENSOR_LPM = 1,       /**< LPM. */
   PMIC_NPA_MODE_ID_SENSOR_POWER_ON = 2,  /**< ON. */
   PMIC_NPA_MODE_ID_SENSOR_MAX = 3,       /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{Touchscreen}
 * Touchscreen mode.
 */
enum
{
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_0 = 0,   /**< Mode 0. */
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_1 = 1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2 = 2,   /**< Mode 2. */
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_3 = 3,   /**< Mode 3. */
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_MAX = 4, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{UIM activity}
 * UIM activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_UIM_STANDBY = 0,             /**< Standby. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW = 1,  /**< Class C low. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C = 2,      /**< Class C. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH = 3, /**< Class C high. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW = 4,  /**< Class B low. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B = 5,      /**< Class B. */
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH = 6, /**< Class B high. */
   PMIC_NPA_MODE_ID_UIM_MAX = 7,                 /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{USB Charger}
 * USB charger mode.
 */
enum
{
   PMIC_NPA_MODE_ID_USB_CHG_DETACHED = 0, /**< Detached. */
   PMIC_NPA_MODE_ID_USB_CHG_MAX = 1       /**< Maximum value for this enumeration. @newpage */,
};

/**
 * @anonenum{USB Full-speed Host/peripheral Power}
 * USB full-speed host/peripheral power mode.
 */
enum
{
   PMIC_NPA_MODE_ID_USB_FS_POWER_OFF = 0,  /**< OFF. */
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_0 = 1, /**< Peripheral LPM 0. */
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_1 = 2, /**< Peripheral LPM 1. */
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_2 = 3, /**< Peripheral LPM 2. */
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_3 = 4, /**< Peripheral LPM 3. */
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_0 = 5, /**< Host LPM 0. */
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_1 = 6, /**< Host LPM 1. */
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_2 = 7, /**< Host LPM 2. */
   PMIC_NPA_MODE_ID_USB_FS_MAX = 8,        /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{USB High-speed Host/peripheral Power}
 * USB high-speed host/peripheral power mode.
 */
enum
{
   PMIC_NPA_MODE_ID_USB_HS_POWER_OFF = 0,  /**< OFF. */
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0 = 1, /**< Peripheral LPM 0. */
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1 = 2, /**< Peripheral LPM 1. */
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_2 = 3, /**< Peripheral LPM 2. */
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_3 = 4, /**< Peripheral LPM 3. */
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_0 = 5, /**< Host LPM 0. */
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_1 = 6, /**< Host LPM 1. */
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_2 = 7, /**< Host LPM 2. */
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_3 = 8, /**< Host LPM 3. */
   PMIC_NPA_MODE_ID_USB_HS_MAX = 9, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{USB Host/peripheral Power}
 * USB host/peripheral activity mode.
 */
enum
{
   PMIC_NPA_MODE_ID_USB_OFF = 0, /**< OFF. */
   PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND = 1, /**< Peripheral suspend. */
   PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE = 2,  /**< Peripheral active. */
   PMIC_NPA_MODE_ID_USB_HOST_SUSPEND = 3,  /**< Host suspend. */
   PMIC_NPA_MODE_ID_USB_HOST_ACTIVE = 4,   /**< Host active. */
   PMIC_NPA_MODE_ID_USB_MAX = 5,           /**< Maximum value for this enumeration. @newpage */
};

/**
 * @anonenum{WCN}
 * WCN mode.
 */
enum
{
   PMIC_NPA_MODE_ID_WCN_MODE_0 = 0,   /**< Mode 0. */
   PMIC_NPA_MODE_ID_WCN_MODE_1 = 1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_WCN_MODE_2 = 2,   /**< Mode 2. */
   PMIC_NPA_MODE_ID_WCN_MODE_3 = 3,   /**< Mode 3. */
   PMIC_NPA_MODE_ID_WCN_MODE_4 = 4,   /**< Mode 4. */
   PMIC_NPA_MODE_ID_WCN_MODE_5 = 5,   /**< Mode 5. */
   PMIC_NPA_MODE_ID_WCN_MODE_MAX = 6, /**< Maximum value for this enumeration. */
};

/**
 * @anonenum{XO Shutdown}
 * XO shutdown mode.
 */
enum
{
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_0 = 0,   /**< Mode 0. */
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_1 = 1,   /**< Mode 1. */
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_MAX = 2, /**< Maximum value for this enumeration. */
};
/** @} */ /* end_addtogroup pmapp_npa */

#endif    /* PMAPP_NPA_H */
