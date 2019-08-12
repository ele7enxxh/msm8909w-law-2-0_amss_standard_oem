/*===============================================================================
#
# CORE Image overide custfile
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/core/pkg/wcnss/pr/rel/1.0/Pronto/bsp/prontoimg/inc/cust_pronto_img.h#2 $
#  $DateTime: 2013/03/08 15:27:59 $
#  $Author: coresvc $
#  $Change: 3457537 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================*/
#ifndef CUST_WCNSS_IMG_H
#define CUST_WCNSS_IMG_H

/* Include the target cust file for this build flavor */
#include AMSS_CUST_H

/*===============================================================================
hack until we get the builds file FIXED!!!!
#===============================================================================*/
//#undef IMAGE_QDSP6_PROC
//#undef FEATURE_QDSP6
//#undef ARCH_QDSP6
//#undef FEATURE_QUBE
//#undef FEATURE_REXL4_CRIT_SECT_TO_QUBE_MUTEX
//#undef FEATURE_DSM_QUBE
//#undef FEATURE_L4_BETA_P6
//#undef STANDALONE_Q6_BUILD

#undef FEATURE_KEYPAD_OKTS
#undef FEATURE_TCXO_WARMUP_IN_SLEEP_TASK
//#undef FEATURE_DIAG_MP_MASTER_MODEM

#define T_RUMI_9X00

#define TRAMP_SLP_TICK_ISR       TRAMP_DEBUG_TIMER_IRQ
#define TMC_SLEEP

#define FEATURE_CORE_SERVICE_ONLY   //needed for diag
// remove nvdiag_init from ARM9, as NV is remoted to QDSP6. 
#undef FEATURE_NVDIAG_INIT         //needed for nvdiag to work
//#define FEATURE_DIAG_MP_MODEM_ONLY
#undef FEATURE_PREFERRED_ROAMING

#define FEATURE_EFS_HOTPLUG

//needed for hsu
//#define FEATURE_HS_USB
#define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB
#define FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB
//#define FEATURE_USB_DIAG_NMEA
//#define FEATURE_HS_USB_CORE_RESET_ON_DISC
#define SIO_USES_HS_USB
//#define FEATURE_HS_USB_CHG_REMOTE_API
//#define FEATURE_HS_USB_MS_FD

/*===============================================================================
hack until we get the builds file FIXED!!!! hack end
#===============================================================================*/


#define FEATURE_TOOLSDIAG_INIT
#define DSMI_DIAG_SIO_RX_ITEM_SIZ 2048
#define FEATURE_DISABLE_CGPS

/* Time services */
#define FEATURE_TIME_NO_DB
#define FEATURE_TIME_DEM_SYNCHRONIZE

/* ==================================================================== */
/* QCoreImage                                                           */
/*     black list of feature flags which must be turned off             */
/*                                                                      */
/* These features when enabled bring in code/functionality from outside */
/* CoreBSP and thus can not be used for coreimage builds                */
/* ==================================================================== */



#undef FEATURE_APP_CATAPP
#undef FEATURE_APP_TEST_AUTOMATION
#undef FEATURE_AUDIOSETTINGS_TEST_AUTOMATION
#undef FEATURE_AUDIO_CONFIGURATION_MINIMAL
#undef FEATURE_AUTH
#undef FEATURE_AUTH_DIGITS
#undef FEATURE_BCMCS
#undef FEATURE_BREW_USB_HID
#undef FEATURE_BT
#undef FEATURE_BT_EXTPF
#undef FEATURE_BT_QSOC_ALL
#undef FEATURE_BT_TEST_API
#undef FEATURE_CDMA
#undef FEATURE_CDMA1X
#undef FEATURE_CDMA_1900
#undef FEATURE_CDMA_800
#undef FEATURE_CDSMS
#undef FEATURE_CGPS
#undef FEATURE_CGPS_USES_CDMA
#undef FEATURE_CGPS_USES_UMTS
#undef FEATURE_CM
#undef FEATURE_COMP_TASK
#undef FEATURE_CPU_BASED_FLOW_CONTROL
#undef FEATURE_DATA
#undef FEATURE_DATA_GCSD
#undef FEATURE_DATA_PDCP
#undef FEATURE_DATA_PS
#undef FEATURE_DATA_PS_DCC
#undef FEATURE_DATA_PS_IPHC
#undef FEATURE_DATA_PS_ROHC
#undef FEATURE_DATA_S2_REG
#undef FEATURE_DATA_WCDMA_CS
#undef FEATURE_DBM
#undef FEATURE_DH
#undef FEATURE_DH_EXP
#undef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#undef FEATURE_DS
#undef FEATURE_DS_DEFAULT_BITRATE_NV
#undef FEATURE_DS_UI_BAUD
#undef FEATURE_ERR_DEBUG_HANDLER
//#undef FEATURE_ERR_EXTENDED_STORE
#undef FEATURE_ERR_LCD
#undef FEATURE_EXPORT_MCCMEID
#undef FEATURE_FACTORY_TESTMODE
#undef FEATURE_FTM_HWTC
#undef FEATURE_FTM_TASK
#undef FEATURE_FULL_TIME_SHDR_3
#undef FEATURE_GPSONE
#undef FEATURE_GPSONE_SMS_CLIENT
#undef FEATURE_GPSONE_STANDALONE
#undef FEATURE_GPSONE_XTRA_BREW_EXT
#undef FEATURE_GPSSRCH_TASK
#undef FEATURE_GRAPH_TASK
#undef FEATURE_GSM
#undef FEATURE_GSM_CB
#undef FEATURE_GSM_EGPRS
#undef FEATURE_GSM_GPRS
#undef FEATURE_GSM_L1_IMPLEMENT_LPM
#undef FEATURE_GSM_SNDCP_IPHC_ROHC
#undef FEATURE_GSTK
#undef FEATURE_GWSMS
#undef FEATURE_HDR
#undef FEATURE_HDRSRCH_POWER_PROFILE
#undef FEATURE_HDR_BCMCS
#undef FEATURE_HDR_CUSTOM_CONFIG
#undef FEATURE_HS
#undef FEATURE_HSDPA
#undef FEATURE_HSDPA_ACCESS_STRATUM
#undef FEATURE_HYBRID_HDR
#undef FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL
#undef FEATURE_IMS_FW_API
#undef FEATURE_IS2000
#undef FEATURE_IS2000_REV_FCH_GATING
#undef FEATURE_IS2000_SLOTTED_TIMER
#undef FEATURE_IS2000_TDSO
#undef FEATURE_IS683C_OTASP
#undef FEATURE_IS683C_PRL
#undef FEATURE_IS95B_MDR
#undef FEATURE_IXFILESVC
#undef FEATURE_IXTIMESVC
#undef FEATURE_KEYPAD
#undef FEATURE_MDSP_TASK
#undef FEATURE_MEDIAPLAYER_TEST_AUTOMATION
#undef FEATURE_MEID_SUPPORT
#undef FEATURE_MMGSDI
#undef FEATURE_MODEM_1X_DIAG
#undef FEATURE_MODEM_PDCOMM
#undef FEATURE_MULTIMODE_RFAPI
#undef FEATURE_NAS
#undef FEATURE_NV_CNV
#undef FEATURE_ONCRPC_SM
#undef FEATURE_OTASP
#undef FEATURE_OTASP_OTAPA
#undef FEATURE_PBM_TASK
#undef FEATURE_PBM_USE_EFS_PB
#undef FEATURE_PBM_USIM_SUPPORT
#undef FEATURE_PDAPI
#undef FEATURE_PORTMAP_TEST_AUTOMATION
#undef FEATURE_QCAMERA_TEST_AUTOMATION
#undef FEATURE_QDSP_RTOS
#undef FEATURE_QDSP_TASK
#undef FEATURE_REX_CS
#undef FEATURE_RUIM
//#undef FEATURE_SAVE_DEBUG_TRACE
//#undef FEATURE_SAVE_TRACE_EXTENDED
#undef FEATURE_SDCC_8BIT_BUS_SETTING
#undef FEATURE_SEC
#undef FEATURE_SECOND_UART
#undef FEATURE_SECRND
#undef FEATURE_SEC_ARM_CRYPTO
#undef FEATURE_SEC_USFS
#undef FEATURE_SENSORS
#undef FEATURE_SND_TASK
#undef FEATURE_SPLIT_SCAN_LINES
#undef FEATURE_SUPPORT_CDMA_GPS_PROTOCOLS
#undef FEATURE_SUPPORT_UMTS_GPS_PROTOCOLS
#undef FEATURE_TC
#undef FEATURE_TIMETEST
#undef FEATURE_TMC_TCXOMGR
#undef FEATURE_TMC_TRM
#undef FEATURE_UI
#undef FEATURE_UIM
#undef FEATURE_UIM_RUIM 
#undef FEATURE_UIM_DRIVER
#undef FEATURE_UMTS_NEGATE_ALL_TCXOMGR_VALUES
#undef FEATURE_UMTS_PDCP
#undef FEATURE_VIRTUAL_SIM
#undef FEATURE_VOC_TASK
#undef FEATURE_WCDMA
#undef FEATURE_WCDMA_800
#undef FEATURE_WCDMA_850
#undef FEATURE_WCDMA_900
#undef FEATURE_WCDMA_BC4
#undef FEATURE_WCDMA_BC9
#undef FEATURE_WCDMA_MULTIBAND
#undef FEATURE_WCDMA_PLT
#undef FEATURE_WCDMA_RXD_DEBUG_CHAIN_STUCK
#undef FEATURE_WLAN_WPS
#undef RF_MULTIMODE
#undef SIRIUS_PLATFORM
#undef TMC_HW_IMPLEMENT_LPM



/* ==================================================================== */
/* remote APIs stuff that it is assumed to be on all the time but it    */
/* shouldn't for CBSP Image                                             */
/* ==================================================================== */
#undef FEATURE_EXPORT_ADSPRTOSATOM
#undef FEATURE_EXPORT_ADSPRTOSMTOA
#undef FEATURE_EXPORT_AUDMGR
#undef FEATURE_EXPORT_AUTH
#undef FEATURE_EXPORT_BT
#undef FEATURE_EXPORT_BTPF
#undef FEATURE_EXPORT_CM
#undef FEATURE_EXPORT_DATA_ON_APPS_ATOM_APIS
#undef FEATURE_EXPORT_DATA_ON_MODEM_MTOA_APIS
#undef FEATURE_EXPORT_DB
#undef FEATURE_EXPORT_DS707_APIS
#undef FEATURE_EXPORT_DSATAPI
#undef FEATURE_EXPORT_DSBC_MDM_APIS
#undef FEATURE_EXPORT_DSHDR_APIS
#undef FEATURE_EXPORT_DSHDR_JCDMA_APIS
#undef FEATURE_EXPORT_DSHDR_MDM_APIS
#undef FEATURE_EXPORT_DSMP_UMTS_APPS_APIS
#undef FEATURE_EXPORT_DSMP_UMTS_MODEM_APIS
#undef FEATURE_EXPORT_DSRLP_APIS
#undef FEATURE_EXPORT_DSUCSDAPPIF_APIS
#undef FEATURE_EXPORT_DSUCSDMPSHIM
#undef FEATURE_EXPORT_DS_MP_SHIM_APPS
#undef FEATURE_EXPORT_DS_MP_SHIM_APPS_ASYNC
#undef FEATURE_EXPORT_DS_MP_SHIM_MODEM
#undef FEATURE_EXPORT_FTM_BT
#undef FEATURE_EXPORT_FTM_WLAN
#undef FEATURE_EXPORT_GSDI_EXP
#undef FEATURE_EXPORT_GSTK_EXP
#undef FEATURE_EXPORT_HDRMC_APIS
#undef FEATURE_EXPORT_HDRMC_MRLP_APIS
#undef FEATURE_EXPORT_HDRMRLP_APPS_APIS
#undef FEATURE_EXPORT_HDRMRLP_MDM_APIS
#undef FEATURE_EXPORT_KEYPAD
#undef FEATURE_EXPORT_MC
#undef FEATURE_EXPORT_MCCAP
#undef FEATURE_EXPORT_MCCDMA
#undef FEATURE_EXPORT_MCCDS
#undef FEATURE_EXPORT_MCCSCH
#undef FEATURE_EXPORT_MCCSRID
#undef FEATURE_EXPORT_MCCSYOBJ
#undef FEATURE_EXPORT_MISC_APPS_APIS_NONWINMOB
#undef FEATURE_EXPORT_MISC_MODEM_APIS
#undef FEATURE_EXPORT_MISC_MODEM_APIS_NONWINMOB
#undef FEATURE_EXPORT_MMGSDILIB
#undef FEATURE_EXPORT_MMOC
#undef FEATURE_EXPORT_MVS
#undef FEATURE_EXPORT_PBMLIB
#undef FEATURE_EXPORT_PDAPI
#undef FEATURE_EXPORT_PDSM_ATL
#undef FEATURE_EXPORT_PD_COMMS_API
#undef FEATURE_EXPORT_PD_COMMS_CLIENT_API
#undef FEATURE_EXPORT_PM_STROBE_FLASH
#undef FEATURE_EXPORT_RFM
#undef FEATURE_EXPORT_RLP_APIS
#undef FEATURE_EXPORT_SD
#undef FEATURE_EXPORT_SECUTIL
#undef FEATURE_EXPORT_SND
#undef FEATURE_EXPORT_SNM
#undef FEATURE_EXPORT_UIM
#undef FEATURE_EXPORT_UIUTILS
#undef FEATURE_EXPORT_UI_CALLCTRL
#undef FEATURE_EXPORT_WLAN_ADP_FTM
#undef FEATURE_EXPORT_WLAN_CP_CM
#undef FEATURE_EXPORT_WLAN_TRP_UTILS
#undef FEATURE_EXPORT_WMS
#undef FEATURE_EXPORT_WMSPM
#undef FEATURE_REMOTE_SND


#endif /* CUST_WCNSS_IMG_H */
