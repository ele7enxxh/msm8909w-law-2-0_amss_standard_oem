#ifndef PMAPP_NPA_H
#define PMAPP_NPA_H
/*===========================================================================


                  P M I C    N P A    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs using NPA framework.

Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pmapp_npa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/*
 * Generic mode IDs that can be used for clients that only use ON / OFF or
 * ACTIVE / STANDBY states.
 */
#define PMIC_NPA_GROUP_ID_ANT1 "/pmic/client/ant1"
#define PMIC_NPA_GROUP_ID_AUDIO "/pmic/client/audio"
#define PMIC_NPA_GROUP_ID_AUDIO2 "/pmic/client/audio2"
#define PMIC_NPA_GROUP_ID_BT "/pmic/client/bt"
#define PMIC_NPA_GROUP_ID_CAMERA_BACK "/pmic/client/camera_back"
#define PMIC_NPA_GROUP_ID_CAMERA_FRONT "/pmic/client/camera_front"
#define PMIC_NPA_GROUP_ID_CLK_REGIME_DIG "/pmic/client/clk_regime_dig"
#define PMIC_NPA_GROUP_ID_CLK_REGIME_DIG2 "/pmic/client/clk_regime_dig2"
#define PMIC_NPA_GROUP_ID_CODEC "/pmic/client/codec"
#define PMIC_NPA_GROUP_ID_CODEC_ANC "/pmic/client/codec_anc"
#define PMIC_NPA_GROUP_ID_DIG_MIC "/pmic/client/dig_mic"
#define PMIC_NPA_GROUP_ID_DISP_EXT_DSUB "/pmic/client/disp_ext_dsub"
#define PMIC_NPA_GROUP_ID_DISP_EXT_HDMI "/pmic/client/disp_ext_hdmi"
#define PMIC_NPA_GROUP_ID_DISP_EXT_LVDS "/pmic/client/disp_ext_lvds"
#define PMIC_NPA_GROUP_ID_DISP_EXT_TVOUT "/pmic/client/disp_ext_tvout"
#define PMIC_NPA_GROUP_ID_DISP_PRIM "/pmic/client/disp_prim"
#define PMIC_NPA_GROUP_ID_DISP_PRIM_DSI "/pmic/client/disp_prim_dsi"
#define PMIC_NPA_GROUP_ID_DISP_SEC "/pmic/client/disp_sec"
#define PMIC_NPA_GROUP_ID_EMMC "/pmic/client/emmc"
#define PMIC_NPA_GROUP_ID_FM "/pmic/client/fm"
#define PMIC_NPA_GROUP_ID_FUSE "/pmic/client/fuse"
#define PMIC_NPA_GROUP_ID_GFX "/pmic/client/gfx"
#define PMIC_NPA_GROUP_ID_GPS1 "/pmic/client/gps1"
#define PMIC_NPA_GROUP_ID_GPS2 "/pmic/client/gps2"
#define PMIC_NPA_GROUP_ID_GPS3 "/pmic/client/gps3"
#define PMIC_NPA_GROUP_ID_HFPLL1 "/pmic/client/hfpll1"
#define PMIC_NPA_GROUP_ID_HKADC "/pmic/client/hkadc"
#define PMIC_NPA_GROUP_ID_INVALID "/pmic/client/invalid"
#define PMIC_NPA_GROUP_ID_MCPM "/pmic/client/mcpm"
#define PMIC_NPA_GROUP_ID_MCPM_DIG "/pmic/client/mcpm_dig"
#define PMIC_NPA_GROUP_ID_MCPM_MEM "/pmic/client/mcpm_mem"
#define PMIC_NPA_GROUP_ID_MCPM1 "/pmic/client/mcpm1"
#define PMIC_NPA_GROUP_ID_MCPM2 "/pmic/client/mcpm2"
#define PMIC_NPA_GROUP_ID_MEM "/pmic/client/mem"
#define PMIC_NPA_GROUP_ID_NFC "/pmic/client/nfc"
#define PMIC_NPA_GROUP_ID_PA1 "/pmic/client/pa1"
#define PMIC_NPA_GROUP_ID_PA2 "/pmic/client/pa2"
#define PMIC_NPA_GROUP_ID_RF "/pmic/client/rf"
#define PMIC_NPA_GROUP_ID_RF_GPS "/pmic/client/rf_gps"
#define PMIC_NPA_GROUP_ID_RF1 "/pmic/client/rf1"
#define PMIC_NPA_GROUP_ID_RF1_GPS "/pmic/client/rf1_gps"
#define PMIC_NPA_GROUP_ID_RF1_GPS1 "/pmic/client/rf1_gps1"
#define PMIC_NPA_GROUP_ID_RF1_GPS2 "/pmic/client/rf1_gps2"
#define PMIC_NPA_GROUP_ID_RF1_GPS3 "/pmic/client/rf1_gps3"
#define PMIC_NPA_GROUP_ID_RF1_NO_SV "/pmic/client/rf1_no_sv"
#define PMIC_NPA_GROUP_ID_RF1_SV "/pmic/client/rf1_sv"
#define PMIC_NPA_GROUP_ID_RF1_TECH_1X "/pmic/client/rf1_tech_1x"
#define PMIC_NPA_GROUP_ID_RF1_TECH_DO "/pmic/client/rf1_tech_do"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GPS "/pmic/client/rf1_tech_gps"
#define PMIC_NPA_GROUP_ID_RF1_TECH_GSM "/pmic/client/rf1_tech_gsm"
#define PMIC_NPA_GROUP_ID_RF1_TECH_LTE "/pmic/client/rf1_tech_lte"
#define PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA "/pmic/client/rf1_tech_tdscdma"
#define PMIC_NPA_GROUP_ID_RF1_TECH_WCDMA "/pmic/client/rf1_tech_wcdma"
#define PMIC_NPA_GROUP_ID_RF2 "/pmic/client/rf2"
#define PMIC_NPA_GROUP_ID_RF2_2 "/pmic/client/rf2_2"
#define PMIC_NPA_GROUP_ID_RF2_GPS1 "/pmic/client/rf2_gps1"
#define PMIC_NPA_GROUP_ID_RF2_GPS2 "/pmic/client/rf2_gps2"
#define PMIC_NPA_GROUP_ID_RF2_GPS3 "/pmic/client/rf2_gps3"
#define PMIC_NPA_GROUP_ID_RF2_NO_SV "/pmic/client/rf2_no_sv"
#define PMIC_NPA_GROUP_ID_RF2_SV "/pmic/client/rf2_sv"
#define PMIC_NPA_GROUP_ID_RF3_2 "/pmic/client/rf3_2"
#define PMIC_NPA_GROUP_ID_RF3_GPS1 "/pmic/client/rf3_gps1"
#define PMIC_NPA_GROUP_ID_RF3_NO_SV "/pmic/client/rf3_no_sv"
#define PMIC_NPA_GROUP_ID_RPM_INIT "/pmic/client/rpm_init"
#define PMIC_NPA_GROUP_ID_SD "/pmic/client/sd"
#define PMIC_NPA_GROUP_ID_SENSOR_DSP_PROXY "/pmic/client/sensor_dsp_proxy"
#define PMIC_NPA_GROUP_ID_SENSOR_DSP_PROXY "/pmic/client/sensor_dsp_proxy"
#define PMIC_NPA_GROUP_ID_TEST_CLIENT1 "/pmic/client/test_client1"
#define PMIC_NPA_GROUP_ID_TEST_CLIENT2 "/pmic/client/test_client2"
#define PMIC_NPA_GROUP_ID_TSCREEN "/pmic/client/tscreen"
#define PMIC_NPA_GROUP_ID_UIM1 "/pmic/client/uim1"
#define PMIC_NPA_GROUP_ID_UIM1_ACTIVITY "/pmic/client/uim1_activity"
#define PMIC_NPA_GROUP_ID_UIM1_ENABLE "/pmic/client/uim1_enable"
#define PMIC_NPA_GROUP_ID_UIM2 "/pmic/client/uim2"
#define PMIC_NPA_GROUP_ID_UIM2_ACTIVITY "/pmic/client/uim2_activity"
#define PMIC_NPA_GROUP_ID_UIM2_ENABLE "/pmic/client/uim2_enable"
#define PMIC_NPA_GROUP_ID_USB_CHG "/pmic/client/usb_chg"
#define PMIC_NPA_GROUP_ID_USB_FS1 "/pmic/client/usb_fs1"
#define PMIC_NPA_GROUP_ID_USB_FS2 "/pmic/client/usb_fs2"
#define PMIC_NPA_GROUP_ID_USB_HS1 "/pmic/client/usb_hs1"
#define PMIC_NPA_GROUP_ID_USB_HS2 "/pmic/client/usb_hs2"
#define PMIC_NPA_GROUP_ID_USB_HS3 "/pmic/client/usb_hs3"
#define PMIC_NPA_GROUP_ID_USB_HSIC "/pmic/client/usb_hsic"
#define PMIC_NPA_GROUP_ID_WCN1 "/pmic/client/wcn1"
#define PMIC_NPA_GROUP_ID_WCN1 "/pmic/client/wcn1"
#define PMIC_NPA_GROUP_ID_WCN2 "/pmic/client/wcn2"
#define PMIC_NPA_GROUP_ID_WCN2 "/pmic/client/wcn2"
#define PMIC_NPA_GROUP_ID_WLAN "/pmic/client/wlan"
#define PMIC_NPA_GROUP_ID_XO_SHUTDOWN "/pmic/client/xo_shutdown"

enum
{
   PMIC_NPA_MODE_ID_ANT_MODE_0 = 0,
   PMIC_NPA_MODE_ID_ANT_MODE_1 = 1,
   PMIC_NPA_MODE_ID_ANT_MODE_2 = 2,
   PMIC_NPA_MODE_ID_ANT_MODE_3 = 3,
};

enum
{
   PMIC_NPA_MODE_ID_AUDIO_STANDBY = 0,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_LOW = 1,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED = 2,
   PMIC_NPA_MODE_ID_AUDIO_ACTIVE_HIGH = 3,
   PMIC_NPA_MODE_ID_AUDIO_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_BT_STANDBY = 0,
   PMIC_NPA_MODE_ID_BT_ACT_SOC = 1,
   PMIC_NPA_MODE_ID_BT_ACT_ALL = 2,
   PMIC_NPA_MODE_ID_BT_MAX = 3,
};

enum
{
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_LOW_DIG = 0,
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NORMAL_DIG = 1,
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_HIGH_DIG = 2,
   PMIC_NPA_MODE_ID_CLK_REGIME_VDD_DIG_MIN = 3,
   PMIC_NPA_MODE_ID_CLK_REGIME_DIG_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_0 = 0,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_1 = 1,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_2 = 2,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_3 = 3,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_4 = 4,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_5 = 5,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_6 = 6,
   PMIC_NPA_MODE_ID_CLK_REGIME_MODE_7 = 7,
};

enum
{
   PMIC_NPA_MODE_ID_CODEC_PATH_OFF = 0,
   PMIC_NPA_MODE_ID_CODEC_1PATH_ON = 1,
   PMIC_NPA_MODE_ID_CODEC_2PATH_ON = 2,
   PMIC_NPA_MODE_ID_CODEC_3PATH_ON = 3,
   PMIC_NPA_MODE_ID_CODEC_4PATH_ON = 4,
   PMIC_NPA_MODE_ID_CODEC_MAX = 5,
};

enum
{
   PMIC_NPA_MODE_ID_CORNER_LEVEL_NO_VOTE = 0,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_1 = 1,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_2 = 2,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_3 = 3,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_4 = 4,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_5 = 5,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_6 = 6,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_7 = 7,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_8 = 8,
   PMIC_NPA_MODE_ID_CORNER_LEVEL_MAX = 9,
};

enum
{
   PMIC_NPA_GROUP_ID_eMMC_OFF = 0,
   PMIC_NPA_GROUP_ID_eMMC_ACTIVE = 1,
   PMIC_NPA_GROUP_ID_eMMC_SLEEP = 2,
   PMIC_NPA_GROUP_ID_eMMC_MAX = 3,
};

enum
{
   PMIC_NPA_MODE_ID_GENERIC_STANDBY = 0,
   PMIC_NPA_MODE_ID_GENERIC_ACTIVE = 1,
   PMIC_NPA_MODE_ID_GENERIC_MAX = 2,
};

enum
{
   PMIC_NPA_MODE_ID_OFF = 0,
   PMIC_NPA_MODE_ID_STANDBY = 1,
   PMIC_NPA_MODE_ID_ACTIVE_NOM_LOW = 2,
   PMIC_NPA_MODE_ID_ACTIVE_SS_LOW = 3,
   PMIC_NPA_MODE_ID_ACTIVE_NOM_HIGH = 4,
   PMIC_NPA_MODE_ID_ACTIVE_SS_HIGH = 5,
   PMIC_NPA_MODE_ID_NOM_HIGH_RX = 6,
   PMIC_NPA_MODE_ID_MAX = 7,
};

enum
{
   PMIC_NPA_MODE_ID_GPS_MODE_0 = 0,
   PMIC_NPA_MODE_ID_GPS_MODE_1 = 1,
   PMIC_NPA_MODE_ID_GPS_MODE_2 = 2,
   PMIC_NPA_MODE_ID_GPS_MODE_3 = 3,
   PMIC_NPA_MODE_ID_GPS_MODE_4 = 4,
   PMIC_NPA_MODE_ID_GPS_MODE_5 = 5,
   PMIC_NPA_MODE_ID_GPS_MODE_6 = 6,
   PMIC_NPA_MODE_ID_GPS_MODE_7 = 7,
   PMIC_NPA_MODE_ID_GPS_MODE_8 = 8,
   PMIC_NPA_MODE_ID_GPS_MODE_MAX = 9,
};

enum
{
   PMIC_NPA_MODE_ID_HKADC_OFF = 0,
   PMIC_NPA_MODE_ID_HKADC_ACTIVE = 1,
   PMIC_NPA_MODE_ID_HKADC_MAX = 2,
};

enum
{
   PMIC_NPA_MODE_ID_INVALID = 0,
};

enum
{
   PMIC_NPA_MODE_ID_MCPM_LOW_DIG = 0,
   PMIC_NPA_MODE_ID_MCPM_NORMAL_DIG = 1,
   PMIC_NPA_MODE_ID_MCPM_HIGH_DIG = 2,
   PMIC_NPA_MODE_ID_MCPM_DIG_MIN = 3,
   PMIC_NPA_MODE_ID_MCPM_DIG_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_MCPM_MODE_0 = 0,
   PMIC_NPA_MODE_ID_MCPM_MODE_1 = 1,
   PMIC_NPA_MODE_ID_MCPM_MODE_2 = 2,
   PMIC_NPA_MODE_ID_MCPM_MODE_3 = 3,
   PMIC_NPA_MODE_ID_MCPM_MODE_4 = 4,
   PMIC_NPA_MODE_ID_MCPM_MODE_5 = 5,
   PMIC_NPA_MODE_ID_MCPM_MODE_6 = 6,
   PMIC_NPA_MODE_ID_MCPM_MODE_7 = 7,
   PMIC_NPA_MODE_ID_MCPM_MODE_8 = 8,
   PMIC_NPA_MODE_ID_MCPM_MODE_MAX = 9,
};

enum
{
   PMIC_NPA_MODE_ID_MEM_DEEP_STANDBY = 0,
   PMIC_NPA_MODE_ID_MEM_SELF_REFRESH = 1,
   PMIC_NPA_MODE_ID_MEM_ACTIVE = 2,
   PMIC_NPA_MODE_ID_MEM_MAX = 3,
};

enum
{
   PMIC_NPA_MODE_ID_PA_MODE_0 = 0,
   PMIC_NPA_MODE_ID_PA_MODE_1 = 1,
   PMIC_NPA_MODE_ID_PA_MODE_2 = 2,
   PMIC_NPA_MODE_ID_PA_MODE_3 = 3,
};

enum
{
   PMIC_NPA_MODE_ID_PLL_MODE_0 = 0,
   PMIC_NPA_MODE_ID_PLL_MODE_1 = 1,
   PMIC_NPA_MODE_ID_PLL_MODE_2 = 2,
   PMIC_NPA_MODE_ID_PLL_MODE_3 = 3,
   PMIC_NPA_MODE_ID_PLL_MODE_4 = 4,
   PMIC_NPA_MODE_ID_PLL_MODE_5 = 5,
   PMIC_NPA_MODE_ID_PLL_MODE_MAX = 6,
};

enum
{
   PMIC_NPA_MODE_ID_RF_SLEEP = 0,
   PMIC_NPA_MODE_ID_RF_UMTS_NOMINAL = 1,
   PMIC_NPA_MODE_ID_RF_AUTOCAL = 1,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_C2K_RX = 2,
   PMIC_NPA_MODE_ID_RF_1X_NOMINAL = 2,
   PMIC_NPA_MODE_ID_RF_UMTS_SS = 3,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_C2K_RX = 3,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_C2K_RXTX = 4,
   PMIC_NPA_MODE_ID_RF_1X_SS = 4,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_C2K_RXTX = 5,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_UMTS_RX = 6,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_UMTS_RX = 7,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_UMTS_RXTX = 8,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_UMTS_RXTX = 9,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_LTE_EVDO_RX = 10,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_LTE_EVDO_RX = 11,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_LTE_EVDO_RXTX = 12,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_LTE_EVDO_RXTX = 13,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_SV_LTE_EVDO_RX = 14,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_SV_LTE_EVDO_RX = 15,
   PMIC_NPA_MODE_ID_RF_DVS_HIGH_SV_LTE_EVDO_RXTX = 16,
   PMIC_NPA_MODE_ID_RF_DVS_LOW_SV_LTE_EVDO_RXTX = 17,
   PMIC_NPA_MODE_ID_RF_MAX = 18,
};

enum
{
   PMIC_NPA_MODE_ID_RF_GPS_EXIT = 0,
   PMIC_NPA_MODE_ID_RF_GPS_SLEEP = 1,
   PMIC_NPA_MODE_ID_RF_GPS_ON_NOMINAL = 2,
   PMIC_NPA_MODE_ID_RF_GPS_ON_SS = 3,
   PMIC_NPA_MODE_ID_RF_GPS_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_RF_MODE_0 = 0,
   PMIC_NPA_MODE_ID_RF_MODE_1 = 1,
   PMIC_NPA_MODE_ID_RF_MODE_2 = 2,
   PMIC_NPA_MODE_ID_RF_MODE_3 = 3,
   PMIC_NPA_MODE_ID_RF_MODE_4 = 4,
   PMIC_NPA_MODE_ID_RF_MODE_5 = 5,
   PMIC_NPA_MODE_ID_RF_MODE_6 = 6,
   PMIC_NPA_MODE_ID_RF_MODE_7 = 7,
   PMIC_NPA_MODE_ID_RF_MODE_8 = 8,
   PMIC_NPA_MODE_ID_RF_MODE_9 = 9,
   PMIC_NPA_MODE_ID_RF_MODE_10 = 10,
   PMIC_NPA_MODE_ID_RF_MODE_11 = 11,
   PMIC_NPA_MODE_ID_RF_MODE_12 = 12,
   PMIC_NPA_MODE_ID_RF_MODE_13 = 13,
   PMIC_NPA_MODE_ID_RF_MODE_14 = 14,
   PMIC_NPA_MODE_ID_RF_MODE_15 = 15,
   PMIC_NPA_MODE_ID_RF_MODE_16 = 16,
   PMIC_NPA_MODE_ID_RF_MODE_17 = 17,
   PMIC_NPA_MODE_ID_RF_MODE_18 = 18,
   PMIC_NPA_MODE_ID_RF_MODE_19 = 19,
   PMIC_NPA_MODE_ID_RF_MODE_20 = 20,
   PMIC_NPA_MODE_ID_RF_MODE_21 = 21,
   PMIC_NPA_MODE_ID_RF_MODE_22 = 22,
   PMIC_NPA_MODE_ID_RF_MODE_23 = 23,
   PMIC_NPA_MODE_ID_RF_MODE_24 = 24,
   PMIC_NPA_MODE_ID_RF_MODE_25 = 25,
   PMIC_NPA_MODE_ID_RF_MODE_26 = 26,
   PMIC_NPA_MODE_ID_RF_MODE_27 = 27,
   PMIC_NPA_MODE_ID_RF_MODE_28 = 28,
   PMIC_NPA_MODE_ID_RF_MODE_29 = 29,
   PMIC_NPA_MODE_ID_RF_MODE_30 = 30,
   PMIC_NPA_MODE_ID_RF_MODE_31 = 31,
   PMIC_NPA_MODE_ID_RF_MODE_32 = 32,
   PMIC_NPA_MODE_ID_RF_MODE_33 = 33,
   PMIC_NPA_MODE_ID_RF_MODE_34 = 34,
   PMIC_NPA_MODE_ID_RF_MODE_35 = 35,
   PMIC_NPA_MODE_ID_RF_MODE_36 = 36,
   PMIC_NPA_MODE_ID_RF_MODE_37 = 37,
   PMIC_NPA_MODE_ID_RF_MODE_38 = 38,
   PMIC_NPA_MODE_ID_RF_MODE_39 = 39,
   PMIC_NPA_MODE_ID_RF_MODE_MAX = 40,
};

enum
{
   PMIC_NPA_MODE_ID_RF1_GPS_EXIT = 0,
   PMIC_NPA_MODE_ID_RF1_GPS_SLEEP = 1,
   PMIC_NPA_MODE_ID_RF1_GPS_DVS_HIGH_RX = 2,
   PMIC_NPA_MODE_ID_RF1_GPS_DVS_LOW_RX = 3,
   PMIC_NPA_MODE_ID_RF1_GPS_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_0 = 0,
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_1 = 1,
   PMIC_NPA_MODE_ID_RPM_INIT_MODE_MAX = 2,
};

enum
{
   PMIC_NPA_GROUP_ID_SD_OFF = 0,
   PMIC_NPA_GROUP_ID_SD_ACTIVE = 1,
   PMIC_NPA_GROUP_ID_SD_MAX = 2,
};

enum
{
   PMIC_NPA_MODE_ID_SD_MODE_OFF = 0,
   PMIC_NPA_MODE_ID_SD_MODE_1 = 1,
   PMIC_NPA_MODE_ID_SD_MODE_2 = 2,
   PMIC_NPA_MODE_ID_SD_MODE_MAX = 3,
};

enum
{
   PMIC_NPA_MODE_ID_SENSOR_POWER_OFF = 0,
   PMIC_NPA_MODE_ID_SENSOR_LPM = 1,
   PMIC_NPA_MODE_ID_SENSOR_POWER_ON = 2,
   PMIC_NPA_MODE_ID_SENSOR_MAX = 3,
};

enum
{
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_0 = 0,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_1 = 1,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2 = 2,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_3 = 3,
   PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_MAX = 4,
};

enum
{
   PMIC_NPA_MODE_ID_UIM_STANDBY = 0,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW = 1,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C = 2,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH = 3,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW = 4,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B = 5,
   PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH = 6,
   PMIC_NPA_MODE_ID_UIM_MAX = 7,
};

enum
{
   PMIC_NPA_MODE_ID_UIM_MODE_1 = 0,
   PMIC_NPA_MODE_ID_UIM_MODE_2 = 1,
   PMIC_NPA_MODE_ID_UIM_MODE_3 = 2,
   PMIC_NPA_MODE_ID_UIM_MODE_4 = 3,
   PMIC_NPA_MODE_ID_UIM_MODE_5 = 4,
   PMIC_NPA_MODE_ID_UIM_MODE_6 = 5,
   PMIC_NPA_MODE_ID_UIM_MODE_7 = 6,
   PMIC_NPA_MODE_ID_UIM_MODE_8 = 7,
   PMIC_NPA_MODE_ID_UIM_MODE_9 = 8,
   PMIC_NPA_MODE_ID_UIM_MODE_10 = 9,
   PMIC_NPA_MODE_ID_UIM_MODE_11 = 10,
   PMIC_NPA_MODE_ID_UIM_MODE_12 = 11,
   PMIC_NPA_MODE_ID_UIM_MODE_13 = 12,
   PMIC_NPA_MODE_ID_UIM_MODE_MAX = 13,
};

enum
{
   PMIC_NPA_MODE_ID_USB_CHG_DETACHED = 0,
   PMIC_NPA_MODE_ID_USB_CHG_MAX = 1,
};

enum
{
   PMIC_NPA_MODE_ID_USB_FS_POWER_OFF = 0,
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_0 = 1,
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_1 = 2,
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_2 = 3,
   PMIC_NPA_MODE_ID_USB_FS_PERI_LPM_3 = 4,
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_0 = 5,
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_1 = 6,
   PMIC_NPA_MODE_ID_USB_FS_HOST_LPM_2 = 7,
   PMIC_NPA_MODE_ID_USB_FS_MAX = 8,
};

enum
{
   PMIC_NPA_MODE_ID_USB_HS_POWER_OFF = 0,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0 = 1,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1 = 2,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_2 = 3,
   PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_3 = 4,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_0 = 5,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_1 = 6,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_2 = 7,
   PMIC_NPA_MODE_ID_USB_HS_HOST_LPM_3 = 8,
   PMIC_NPA_MODE_ID_USB_HS_MAX = 9,
};

enum
{
   PMIC_NPA_MODE_ID_WCN_MODE_0 = 0,
   PMIC_NPA_MODE_ID_WCN_MODE_1 = 1,
   PMIC_NPA_MODE_ID_WCN_MODE_2 = 2,
   PMIC_NPA_MODE_ID_WCN_MODE_3 = 3,
   PMIC_NPA_MODE_ID_WCN_MODE_4 = 4,
   PMIC_NPA_MODE_ID_WCN_MODE_5 = 5,
   PMIC_NPA_MODE_ID_WCN_MODE_MAX = 6,
};

enum
{
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_0 = 0,
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_1 = 1,
   PMIC_NPA_MODE_ID_XO_SHUTDOWN_MODE_MAX = 2,
};
#endif // PMAPP_NPA_H

