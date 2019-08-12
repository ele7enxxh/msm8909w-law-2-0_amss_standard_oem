/*==============================================================================

                    GPS NV   R E G I S T R Y

  DESCRIPTION
 
    The GPS Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2016 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
==============================================================================*/

/*----------------------------------------------------------------------------
 *  Copyright (c) 2013-2015 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*
 *  Copyright (c) 2015 Qualcomm Technologies, Inc. 
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gps_nv_efs.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/30/16   skm     Added NV for default LPP12 2 timer feature config 
04/15/16   ld      Added NHz NV support
08/13/15   sjk     Added default configuration of GM from NV for AON
06/11/15   sjk     Adding NV for location batching -radius & config 
06/09/15   ssu      Adding NV for GM filtering of WLAN freq. 
02/26/15   ssu      Adding new NV items for GM LOWI integration.
02/26/15   tf      Added MCFG API support for SIM specific NVs
11/25/14   rk      LPP Rel10 24 AD NV
09/24/14   rh      Added new GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY NV item. 
02/21/14   ssu   Adding NV for configuring motion sensing distance in case of challenging GPS env. 
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config. 
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/10/14  ss       Added new nv item to support High confidence Geofence configuration
06/11/12   rk      Added new GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT NV item. 
07/17/12   mjohn   Added new GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES NV for Enhanced Cell ID positioning


=============================================================================*/


#include "gps_variation.h"
#ifdef FEATURE_CGPS

#ifndef _GPSNVEFS_H
#define _GPSNVEFS_H


/*==============================================================================
  To add new items to the registry:
  1. Add entry to the end of existing enum cgps_nv_efs_reg_item_type. Don't insert,
     always append.
  2. Add entry in table sm_nv_efs.c for SM related NV items
     Make sure the order of the items is the same as in enum cgps_nv_efs_reg_item_type.
     Create a new table in the subsystem modules if the a new subsystem is added.
  3. Add a default value in sm_nv_efs_get_default() in sm_nv_efs.c for SM related
     items.
  4. Create a new enum in cgps_nv_efs_subsys_type for other subsystems and create
     new MIN and MAX items for the subsystem in cgps_nv_efs_reg_item_type.
  
==============================================================================*/

/* these are the new NV items that are using the EFS APIs. this is the master list
   of all GPS EFS items . If using a for loop make sure the loop checks for <=MAX
    to make sure all items are accessed. */
typedef enum
{
  CGPS_NV_EFS_MIN_NUM_ITEMS = 0,
  /* ------------------------------------------------------------------
     SM items
     ------------------------------------------------------------------ */
  CGPS_NV_EFS_SM_MIN_ITEM = CGPS_NV_EFS_MIN_NUM_ITEMS,
  CGPS_NV_EFS_SM_XTRA_PREFERRED_MAX_VALID_AGE = CGPS_NV_EFS_SM_MIN_ITEM, /* XTRA Data Validity */
  GNSS_NV_EFS_SM_SEND_NO_FIX_REPORT, /* TM will send out PE's no_fix report if set */
  GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE, /*To enable/disable SUPL on 1x UP*/
  GNSS_NV_EFS_SM_TM_LPP_ENABLE, /* To enable or disable LPP protocol */
  GNSS_NV_EFS_SM_AGPS_RAT_PREF_CONFIG, /* Agps rat preference*/
  GNSS_NV_EFS_CONFIG_QMI_SERVICE_EXTERNAL, /* Enable/Disable QMI-PDS/QMI-LOC services */
  GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT, /* Control selection of positioning protocols 
                                                     between RRLP, RRC,& LPP for user & control plane */
  GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES, /* Enable/Disable Premium services (Enhanced Cell ID positioning) */                                              
  GNSS_NV_EFS_SM_TM_LPP_CP_TRANSPORT_TIMEOUT_SECS, /* LPP CP transport TX timeout secs */
  GNSS_NV_EFS_SM_TM_EMERGENCY_PROTOCOL_SELECT, /* If E911 session in W/G/1x is UP or CP */
  GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD, /* HEPE threshold value */
  GNSS_NV_EFS_SM_GM_GNSS_UNAVAIL_IND_TIMEOUT, /* Timeout after which GNSS unavail indication is sent out from GM in a bad GNSS environment */ 
  GNSS_NV_EFS_SM_GM_MOTION_DETECTION_SOURCES, /*Geofencing Motion Detection sources*/
  GNSS_NV_EFS_SM_GM_GNSS_POS_QOS_SESS_TIMEOUT, /*Geofencing GNSS Pos Qos Session Timeout*/
  GNSS_NV_EFS_SM_GM_POSITION_SOURCES, /*Geofencing Position Sources*/
  GNSS_NV_EFS_SM_XTRA_LAST_XTRA3_KEY_INDEX,  /* last key index used in XTRA3 */
  GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC, /* number of seconds to wait for WIFI scan result */
  GNSS_NV_EFS_SM_GM_GNSS_MAX_POS_UNC_ACCEPTED, /* Geofencing GNSS max pos unc accepted */
  GNSS_NV_EFS_SM_GM_USE_NW_ASST_FIXES,         /* Geofencing use NW assisted fixes */
  GNSS_NV_EFS_SM_GM_HIGH_RESPONSIVENESS_CONFIG, /*High Responsiveness Config*/
  GNSS_NV_EFS_SM_E911_CONFIG,  /* Configureable E911 behavior */  
  GNSS_NV_EFS_SM_GM_CPI_REQUEST_RATE,  /* GM CPI request rate */
  GNSS_NV_EFS_SM_GM_MED_RESP_BACKOFF,  /*Medium Responsiveness Backoff in seconds */
  GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_BO_CONFIG, /* Backoff config when in Challenging GPS Env in seconds */  
  GNSS_NV_EFS_SM_GM_CHAL_GPS_ENV_MS_DIST_CONFIG, /* The motion sensing distance for GPS in case of challenging GPS env */
  GNSS_NV_EFS_SM_GM_MS_SPEED_CONFIG, /*Motion Sensing Speed config for Geofencing*/
  GNSS_NV_EFS_SM_TM_MEAS_QMI_REPORT_CONTROL, /*GNSS measurement report QMI control*/  
  GNSS_NV_EFS_SM_TM_CONFIG_AGNSS_TRANSMISSION_DELAY, /* Config Transmission Delay for Protocols */
  GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_SUPPORT, /* UE support for processing greater than 24 OTDOA AD with same frequency/earfcn */
  GNSS_NV_EFS_SM_GM_LOWI_REGISTER_ENABLE, /* Enable GM registration with LOWI service. */
  GNSS_NV_EFS_SM_GM_LOWI_SUBSCRIPTION_MASK, /* Subscribe to the different services supported on LOWI from GM  */
  GNSS_NV_EFS_SM_GM_LOWI_REG_CONFIG, /* Configure how the GM registers with the LOWI   */
  GNSS_NV_EFS_SM_GM_COMP_LOGGING_RATE, /*Geofencing comprehensive logging rate*/  
  GNSS_NV_EFS_SM_GM_IOD_CONFIG_SWITCHES_MASK, /* GeoFencing IOD config mask */
  GNSS_NV_EFS_SM_GM_FILTER_WLAN_FREQ_MASK, /* Filter and accept WLAN frequencies */
  GNSS_NV_EFS_SM_BATCHING_GF_RADIUS, /* Geofence radius used in batching */
  GNSS_NV_EFS_SM_BATCHING_CONFIG_MASK, /* bitmask for controlling batching behavior */  
  GNSS_NV_EFS_SM_AON_GNSS_POS_QOS_SESSION_TIMEOUT, /*AlwaysOn Service default GNSS Pos Qos Session Timeout*/
  GNSS_NV_EFS_SM_AON_CPI_REQUEST_RATE, /*AlwaysOn Service default CPI request rate */
  GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_BO_CONFIG, /*AlwaysOn Service default backoff config when in Challenging GPS Env in seconds */
  GNSS_NV_EFS_SM_AON_CHAL_GPS_ENV_MS_DIST_CONFIG, /*AlwaysOn Service default for motion sensing distance for GPS in case of challenging GPS env */  
  GNSS_NV_EFS_SM_NHZ_CONFIG, /* Configuration for NHz mode of operation */
  GNSS_NV_EFS_SM_LPP_2TIMER_SUPPORT, /*AlwaysOff 2Timer LPP rel12 feature config*/
  CGPS_NV_EFS_SM_MAX_ITEM = GNSS_NV_EFS_SM_LPP_2TIMER_SUPPORT,
  /* ------------------------------------------------------------------
     ME items
     ------------------------------------------------------------------ */
  GNSS_NV_EFS_ME_MIN_ITEM,
  GNSS_NV_EFS_ME_NV_ACCESS_CONTROL = GNSS_NV_EFS_ME_MIN_ITEM,
  GNSS_NV_EFS_ME_EXTERNAL_LNA_STATE,
  GNSS_NV_EFS_ME_CLOCK_SOURCE,
  GNSS_NV_EFS_ME_NOTCH_FILTER_CONTROL,
  GNSS_NV_EFS_ME_GSM_BLANKING_CONTROL,
  GNSS_NV_EFS_ME_ADDED_FREQUENCY_OFFSET,
  GNSS_NV_EFS_PP_DISABLE_LTE_DETECTION_INCREASE,
  GNSS_NV_EFS_ME_CELLDB_MAX_TIMETAG_AGE_SEC,
  GNSS_NV_EFS_ME_RFGD_GLO_NS,
  GNSS_NV_EFS_ME_RFGD_GLO_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_TAUGPS_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_MAIN_WWAN_TX_ANTENNA_IM_JAMMING_POWER,
  GNSS_NV_EFS_ME_AUX_WWAN_TX_ANTENNA_IM_JAMMING_POWER,
  GNSS_NV_EFS_ME_WLAN_BT_DELTA_TX_ANTENNA_IM_JAMMING_POWER,
  GNSS_NV_EFS_ME_GPS_5MS_IQSUM_LOGGING,
  GNSS_NV_EFS_ME_PEAK_ANTENNA_GAIN,
  GNSS_NV_EFS_ME_LTE_B13_TX_GPS_XCORR_MASK_INCRESAE,
  GNSS_NV_EFS_ME_SPAN_CONTROL,
  GNSS_NV_EFS_ME_SPAN_TIMERS,
  GNSS_NV_EFS_ME_GNSS_CONFIG,
  GNSS_NV_EFS_ME_GNSS_BDS_PATH_LOSS,
  GNSS_NV_EFS_ME_APQ_NO_WWAN_TX_INFO_PGA_CAL,
  GNSS_NV_EFS_ME_RFGD_BDS_NS,
  GNSS_NV_EFS_ME_RFGD_BDS_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_RFGD_BDS2GLO_NS,
  GNSS_NV_EFS_ME_RFGD_BDS2GLO_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_SP_ON_HEAP_ALLOCATION_MODE,
  GNSS_NV_EFS_ME_SP_ON_HEAP_ALLOC_FAIL_TEST_ID,
  GNSS_NV_EFS_ME_SP_ON_HEAP_ALLOC_FAIL_BUFFER_NUM,
  GNSS_NV_EFS_ME_DYNAMIC_PGA_CONFIG,
  GNSS_NV_EFS_ME_PGA_BACKOFF_CONFIG,
  GNSS_NV_EFS_ME_GIT_CPU_BUSY_THRESHOLD_MS,
  GNSS_NV_EFS_ME_GIT_IPC_SLEEP_MS,
  GNSS_NV_EFS_ME_WWAN_ME_CONFIG,
  GNSS_NV_EFS_ME_5GHZ_WLAN_MAIN_WWAN_TX_ANTENNA_IM_JAMMING_POWER, 
  GNSS_NV_EFS_ME_5GHZ_WLAN_AUX_WWAN_TX_ANTENNA_IM_JAMMING_POWER,
  GNSS_NV_EFS_ME_SPAN_TIMERS_EXTENDED,
  GNSS_NV_EFS_ME_WLAN_TX_INDICATION_MODE,
  GNSS_NV_EFS_ME_GPM_CONFIG1,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_ACI_MASK_INCRESAE,
  GNSS_NV_EFS_ME_LTE_B13_TX_GPS_SBAS_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_SEVEN_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_SIX_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_FIVE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_FOUR_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_THREE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_TWO_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_NEG_ONE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_ZERO_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_ONE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_TWO_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_THREE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_FOUR_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_FIVE_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_FREQUENCY_PLUS_SIX_CNO_THRESHOLD,
  GNSS_NV_EFS_ME_LTE_B13_TX_GPS_CNO_OFFSET,
  GNSS_NV_EFS_ME_LTE_B13_TX_GLO_CNO_OFFSET,
  GNSS_NV_EFS_ME_LTE_B13_TX_PWR_THRESHOLD_dBm, 
  GNSS_NV_EFS_ME_GNSS_5MS_IQSUM_LOGGING,
  GNSS_NV_EFS_ME_AUTO_MIN_GPS_WEEK,
  GNSS_NV_EFS_ME_MC_MEMALLOC_FAILURE,
  GNSS_NV_EFS_ME_GNSS_GAL_PATH_LOSS,
  GNSS_NV_EFS_ME_RFGD_GPS2GAL_NS,
  GNSS_NV_EFS_ME_RFGD_GPS2GAL_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_RFGD_GAL2BDS_NS,
  GNSS_NV_EFS_ME_RFGD_GAL2BDS_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_RFGD_GAL2GLO_NS,
  GNSS_NV_EFS_ME_RFGD_GAL2GLO_UNCERTAINTY_NS,
  GNSS_NV_EFS_ME_ENHANCED_FITNESS_TRACKING_CONTROL,
  GNSS_NV_EFS_ME_MULTIPATH_CN0_WEIGHING_FACTOR,
  GNSS_NV_EFS_ME_AUTOMOTIVE_CONFIG,
  GNSS_NV_EFS_ME_AGNSS_ACQ_OPTIMIZATION,

  /* add other ME items here. Make sure to update GNSS_NV_EFS_ME_MAX_ITEM */
  GNSS_NV_EFS_ME_MAX_ITEM = GNSS_NV_EFS_ME_AGNSS_ACQ_OPTIMIZATION,


  /* ------------------------------------------------------------------
   PE items
   ------------------------------------------------------------------ */
  GNSS_NV_EFS_PE_MIN_ITEM,  
  GNSS_NV_EFS_PE_CLOCK_FREQ_PROCESS_NOISE = GNSS_NV_EFS_PE_MIN_ITEM,
  GNSS_NV_EFS_PE_CLOCK_PHASE_PROCESS_NOISE,
  GNSS_NV_EFS_PE_2D_SET_OPT,
  GNSS_NV_EFS_PE_POSPINNING_REPIN_THRESH,
  GNSS_NV_EFS_PE_XTRA_MEM_REL_TIME_INTERVAL,
  GNSS_NV_EFS_PE_REPORT_FULL_MEAS_BLK,
  GNSS_NV_EFS_PE_ADVANCED_FDIC,
  GNSS_NV_EFS_PE_PED_DR_TIMEOUT,
  GNSS_NV_EFS_PE_PDR_SUBFEATURE_CONTROL,
  GNSS_NV_EFS_PE_SFS_WRITE_FACTOR,
  GNSS_NV_EFS_PE_NF_CD_MEMALLOC_FAILURE,

  /* add other PE items here. Make sure to update GNSS_NV_EFS_PE_MAX_ITEM */
  GNSS_NV_EFS_PE_MAX_ITEM = GNSS_NV_EFS_PE_NF_CD_MEMALLOC_FAILURE,


  /* ------------------------------------------------------------------
   PDCOMMS items
   ------------------------------------------------------------------ */
  CGPS_NV_EFS_PDCOMMS_MIN_ITEM,
  CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES = CGPS_NV_EFS_PDCOMMS_MIN_ITEM, 
  CGPS_NV_EFS_PDCOMM_XTRA_APN_PROFILES, 
  CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES, 
  CGPS_NV_EFS_PDCOMM_SSL_CERTIFICATE_KEY_LENGTH, 
  CGPS_NV_EFS_PDCOMMS_MAX_ITEM = CGPS_NV_EFS_PDCOMM_SSL_CERTIFICATE_KEY_LENGTH,
  /* ------------------------------------------------------------------
     TLE items
     ------------------------------------------------------------------ */
  GNSS_NV_EFS_TLE_XTM_MIN_ITEM,
  GNSS_NV_EFS_TLE_XTRAT_PRIMARY_SVR_ADDRESS = GNSS_NV_EFS_TLE_XTM_MIN_ITEM,
  GNSS_NV_EFS_TLE_XTRAT_PRIMARY_SVR_PORT,
  GNSS_NV_EFS_TLE_XTRAT_SECONDARY_SVR_ADDRESS,
  GNSS_NV_EFS_TLE_XTRAT_SECONDARY_SVR_PORT,
  GNSS_NV_EFS_TLE_XTRAT_USER_SESSION_CONTROL,
  GNSS_NV_EFS_TLE_XTRAT_CLIENT_TOKEN,
  GNSS_NV_EFS_TLE_XTRAT_DATAXFR_MAX_ELAPSED_THRESHOLD,
  GNSS_NV_EFS_TLE_XTRAT_PERIODIC_UPLOAD_LIVECOUNT,
  GNSS_NV_EFS_TLE_XTRAT_DATAXFR_NUM_MAX_ELAPSED_RETRY,
  GNSS_NV_EFS_TLE_XTRAT_LAST_DOWNLOAD_GPS_WEEK,
  GNSS_NV_EFS_TLE_XTRAT_LAST_DOWNLOAD_GPS_SEC,
  GNSS_NV_EFS_TLE_XTRAT_MIN_TIME_SERVER_ACCESS,
  GNSS_NV_EFS_TLE_XTRAT_UPLOAD_SESS_CONTROL,
  GNSS_NV_EFS_TLE_XTRAT_FEATURE_CONTROL,
  GNSS_NV_EFS_TLE_XTRAT_PRIMARY_REQUEST_URI,
  GNSS_NV_EFS_TLE_XTRAT_SECONDARY_REQUEST_URI,
  GNSS_NV_EFS_TLE_XTRAT_USE_TRANSPORT_SECURITY,
  GNSS_NV_EFS_TLE_XTRAT_ENABLE_SIMULTANEOUS_XTRA_XTRAT,
  GNSS_NV_EFS_TLE_XTRAT_CONN_FAILURE_RETRY_PERIOD,
  CGPS_NV_EFS_TLE_XTM_MAX_ITEM = GNSS_NV_EFS_TLE_XTRAT_CONN_FAILURE_RETRY_PERIOD,

  GNSS_NV_EFS_TLE_TLM_MIN_ITEM,
  GNSS_NV_EFS_TLE_TLM_DATABASE_AGE_THRESHOLD = GNSS_NV_EFS_TLE_TLM_MIN_ITEM,
  GNSS_NV_EFS_TLE_TLM_DATABASE_AGE_THRESHOLD_UNIT,
  GNSS_NV_EFS_TLE_TLM_CELLID_L2_DFT_RADIUS,
  GNSS_NV_EFS_TLE_TLM_CELLID_L3_DFT_RADIUS,
  GNSS_NV_EFS_TLE_TLM_CELLID_L4_DFT_RADIUS,
  GNSS_NV_EFS_TLE_XTRAT_TIME_INJECTION_CONTROL,
  GNSS_NV_EFS_TLE_XTRAT_TIME_INJECT_TUNC,
  GNSS_NV_EFS_TLE_XTRAT_KEEP_WARM_TIMEOUT,
  GNSS_NV_EFS_TLE_TLM_DOWNLOADED_CELLDB_NUM_PAGES,
  GNSS_NV_EFS_TLE_TLM_DOWNLOADED_REGIONDB_NUM_PAGES,
  GNSS_NV_EFS_TLE_TLM_SELFLEARNED_POSDB_NUM_PAGES,
  GNSS_NV_EFS_TLE_TLM_ENABLE_DB_WRITE_TO_STORAGE,
  GNSS_NV_EFS_TLE_TLM_TEST_CONTROL,
  GNSS_NV_EFS_TLE_TLM_UPLOAD_CHECK_TIMER_PERIOD,
  GNSS_NV_EFS_TLE_TLM_UPLOAD_CELLCOUNT_THRESHOLD,
  GNSS_NV_EFS_TLE_TLM_UPLOAD_FORCE_TIMER_PERIOD,
  GNSS_NV_EFS_TLE_TLM_TDP_MEM_CHECK_THRESHOLD,
  GNSS_NV_EFS_TLE_TLM_OPERATIONAL_USER_CELL_HISTORY_SIZE,
  CGPS_NV_EFS_TLE_TLM_MAX_ITEM = GNSS_NV_EFS_TLE_TLM_OPERATIONAL_USER_CELL_HISTORY_SIZE,

  /* ------------------------------------------------------------------
  SDP items
  ------------------------------------------------------------------ */
  GNSS_NV_EFS_SDP_MIN_ITEM,  
  GNSS_NV_EFS_SDP_CHARGER_CTRL_ENABLED = GNSS_NV_EFS_SDP_MIN_ITEM,  
  /* add other SDP items here. Make sure to update GNSS_NV_EFS_SDP_MAX_ITEM */
  GNSS_NV_EFS_SDP_MAX_ITEM = GNSS_NV_EFS_SDP_CHARGER_CTRL_ENABLED,

  /* ------------------------------------------------------------------
  Common items
  ------------------------------------------------------------------ */
  GNSS_NV_EFS_COMMON_MIN_ITEM,
  GNSS_NV_EFS_COMMON_FP_EXCEPTION_CONFIG = GNSS_NV_EFS_COMMON_MIN_ITEM, 
  /* Tasks for which FP exceptions (Overflow, Invalid and Div By Zero) are enabled */
  GNSS_NV_EFS_COMMON_MAX_ITEM =  GNSS_NV_EFS_COMMON_FP_EXCEPTION_CONFIG,

  /* ------------------------------------------------------------------
  WLE items
  ------------------------------------------------------------------ */
 
  GNSS_NV_EFS_WLE_WLM_MIN_ITEM,
  GNSS_NV_EFS_WLE_WLM_ASYNC_SCAN_CONTROL = GNSS_NV_EFS_WLE_WLM_MIN_ITEM,
  CGPS_NV_EFS_WLE_WLM_MAX_ITEM = GNSS_NV_EFS_WLE_WLM_ASYNC_SCAN_CONTROL,

  /* ------------------------------------------------------------------
  SAML items
  ------------------------------------------------------------------ */
  GNSS_NV_EFS_SAML_MIN_ITEM,
  GNSS_NV_EFS_SAML_FEATURE_CONTROL = GNSS_NV_EFS_SAML_MIN_ITEM,
  GNSS_NV_EFS_SAML_MAX_ITEM = GNSS_NV_EFS_SAML_FEATURE_CONTROL,

  /* ------------------------------------------------------------------ 
  LOC_MW items  
  ------------------------------------------------------------------ */  
  GNSS_NV_EFS_LOC_MIN_ITEM,  
  GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL = GNSS_NV_EFS_LOC_MIN_ITEM,                   /*Minimum supported mininterval for start fix request in milli seconds */
  GNSS_NV_EFS_LOC_MAX_ITEM =  GNSS_NV_EFS_LOC_SUPPORTED_MIN_INTERVAL,


  CGPS_NV_EFS_MAX_NUM_ITEMS /* This should be the last item. Add all entries above this line */
} cgps_nv_efs_reg_item_type;


/* other structs and constants */

    #define CGPS_MAX_NV_EFS_FILEPATH_SIZE 120 /* max file path size */
    #define CGPS_MAX_NV_EFS_FILENAME_SIZE 60 /* max file name size */

typedef enum
{
  CGPS_NV_EFS_REG_OP_GET_ITEM,
  CGPS_NV_EFS_REG_OP_SET_ITEM
}cgps_nv_efs_operation_type;

typedef enum
{
  CGPS_NV_EFS_REG_ST_ITEM_INTERNAL,
  CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL
} cgps_nv_efs_item_usage_type;

typedef enum
{
  CGPS_NV_EFS_REG_TYPE_INTEGRAL,
  CGPS_NV_EFS_REG_TYPE_FLOAT,
  CGPS_NV_EFS_REG_TYPE_DOUBLE
  /* add new data types here if needed */
}cgps_nv_efs_data_type;

typedef enum
{
  CGPS_NV_EFS_REG_RW_STAT_OK,
  CGPS_NV_EFS_REG_RW_STAT_RD_ERR,
  CGPS_NV_EFS_REG_RW_STAT_WR_ERR,
  CGPS_NV_EFS_REG_RW_STAT_INVALID
} cgps_nv_efs_reg_rw_stat_enum_type;

typedef struct
{
  cgps_nv_efs_item_usage_type data_src; /* location of item data */
  char file_name[CGPS_MAX_NV_EFS_FILENAME_SIZE]; /* name of item file */
  uint32 size; /* size of item in bytes */
  uint8 precision_digits; /* number of digits of precision for float/double */
  cgps_nv_efs_data_type type_data; /* if integer/float/double */
} cgps_nv_efs_reg_item_struct;

typedef enum 
{
  CGPS_NV_EFS_SUBSYS_MIN = -1,
  CGPS_NV_EFS_SM,
  CGPS_NV_EFS_ME,
  CGPS_NV_EFS_PE,
  CGPS_NV_EFS_PDCOMM,
  CGPS_NV_EFS_TLE_TLM,
  CGPS_NV_EFS_TLE_XTM,
  CGPS_NV_EFS_SDP,
  CGPS_NV_EFS_COMMON,
  CGPS_NV_EFS_WLE_WLM,
  CGPS_NV_EFS_SAML,
  CGPS_NV_EFS_LOC,
  /* if you add a new type, make sure you upate the cgps_nv_efs_init() function with the
     appropriate config file name for your subsystem*/
  CGPS_NV_EFS_INVALID,
  CGPS_NV_EFS_SUBSYS_MAX = CGPS_NV_EFS_INVALID
} cgps_nv_efs_subsys_type;

/* Enum to specify the File System Type.  
** Values are explicitly set to match enum "mcfg_fs_e_type" */
typedef enum
{
  CGPS_NV_FS_TYPE_MIN = -1,
  CGPS_NV_FS_TYPE_EFS = 0,
  CGPS_NV_FS_TYPE_SFS = 1,
  CGPS_NV_FS_TYPE_INVALID,
  CGPS_NV_FS_TYPE_MAX = CGPS_NV_FS_TYPE_INVALID,
} cgps_nv_efs_fs_type;

/* Enum to specify the Subscription ID
** Values are explicitly set to match enum "mcfg_fs_sub_id_e_type" 
** CGPS_NV_SUBID_NO_PREF should be used if NV item is subscription independent. */
typedef enum
{
  CGPS_NV_SUBID_MIN = -1,
  CGPS_NV_SUBID_NO_PREF = 0,
  CGPS_NV_SUBID_0    = CGPS_NV_SUBID_NO_PREF,
  CGPS_NV_SUBID_1    = 1,
  CGPS_NV_SUBID_2    = 2,
  CGPS_NV_SUBID_MAX  = CGPS_NV_SUBID_2,
} cgps_nv_efs_sub_id_type;

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init

  DESCRIPTION
    Performs initialization required for CGPS NV items in the EFS.
    Currently this means generating the config file for diag tools.

    Requires a table of the NV items that need to be input to the config file.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean cgps_nv_efs_init (cgps_nv_efs_subsys_type type, 
                          cgps_nv_efs_reg_item_type table_start,
                          cgps_nv_efs_reg_item_type table_end,
                          const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items
                         ); 

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init_subsid

  DESCRIPTION
    Performs initialization required for CGPS NV items in the EFS.
    Currently this means generating the config file for diag tools.

    Requires a table of the NV items that need to be input to the config file.

    This function is subscription ID aware and assumes EFS file system.  

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean cgps_nv_efs_init_subsid (cgps_nv_efs_subsys_type type, 
                                 cgps_nv_efs_reg_item_type table_start,
                                 cgps_nv_efs_reg_item_type table_end,
                                 const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                 cgps_nv_efs_sub_id_type subid
                                ); 

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_read

  DESCRIPTION
    Read the specified item from EFS2 special items file.
    The function logs the read write activity. This is a generic read function.
    If this function returns anything other than CGPS_NV_EFS_REG_RW_STAT_OK,
    the caller must set this item to default value if needed.
 
  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item, 
                                                            uint32 data_size,
                                                            void *data_ptr,
                                                            cgps_nv_efs_reg_item_type table_start,
                                                            cgps_nv_efs_reg_item_type table_end,
                                                            const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items
                                                           );

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_read_subsid

  DESCRIPTION
    Read the specified item from EFS2 special items file.
    The function logs the read write activity. This is a generic read function.
    If this function returns anything other than CGPS_NV_EFS_REG_RW_STAT_OK,
    the caller must set this item to default value if needed.
  
    This function is subscription ID aware and assumes EFS file system.  

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_read_subsid(cgps_nv_efs_reg_item_type const item, 
                                                                   uint32 data_size,
                                                                   void *data_ptr,
                                                                   cgps_nv_efs_reg_item_type table_start,
                                                                   cgps_nv_efs_reg_item_type table_end,
                                                                   const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                                                   cgps_nv_efs_sub_id_type subid
                                                                  );

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity. This is a generic write function.
 
  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item, 
                                                             uint32 data_size,
                                                             const void *data_ptr,
                                                             cgps_nv_efs_reg_item_type table_start,
                                                             cgps_nv_efs_reg_item_type table_end,
                                                             const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items
                                                            );

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write_subsid

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity. This is a generic write function.
  
    This function is subscription ID aware and assumes EFS file system. 
 
  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write_subsid(cgps_nv_efs_reg_item_type const item, 
                                                                    uint32 data_size,
                                                                    const void *data_ptr,
                                                                    cgps_nv_efs_reg_item_type table_start,
                                                                    cgps_nv_efs_reg_item_type table_end,
                                                                    const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                                                    cgps_nv_efs_sub_id_type subid
                                                                   );

/*==============================================================================
  FUNCTION
    gps_nv_efs_init

  DESCRIPTION
    Performs initialization required for NV items in the EFS.
    Currently this means generating the config file for diag tools.
    Requires a table of the NV items that need to be input to the config file.
 
    Unlike cgps_nv_efs_init() which uses the hardcoded "cgps" in the NV item's
    pathnames, the input parameter psz_pathname_prefix in this function allows
    caller to specify the name to be used as the pathname prefix.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean gps_nv_efs_init (cgps_nv_efs_subsys_type type, 
                         cgps_nv_efs_reg_item_type table_start,
                         cgps_nv_efs_reg_item_type table_end,
                         const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                         const char* psz_pathname_prefix
                         ); 

/*==============================================================================
  FUNCTION
    gps_nv_efs_reg_item_read

  DESCRIPTION
    Reads the specified item from EFS2 special items file.
 
    The subsystem that the given item belongs to must have already been
    initialized, or this function will return CGPS_NV_EFS_REG_RW_STAT_INVALID.
 
    If the item EFS file is not found, the default value for the item is returned.
 
  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type gps_nv_efs_reg_item_read(
  cgps_nv_efs_reg_item_type const item_id, 
  uint32 data_size,  /* number of bytes of data in *data_ptr */
  void* data_ptr    /* source of data */
);

/*==============================================================================
  FUNCTION
    gps_nv_efs_reg_item_write

  DESCRIPTION
    Write the specified item value to EFS2 special items file.
 
    The subsystem that the given item belongs to must have already been
    initialized, or this function will return CGPS_NV_EFS_REG_RW_STAT_INVALID.
 
  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type gps_nv_efs_reg_item_write(
  cgps_nv_efs_reg_item_type const item_id, 
  uint32 data_size,      /* number of bytes of data *data_ptr can hold*/
  const void* data_ptr  /* destination for data */
);

#endif /* _GPSNVEFS_H */
  
#endif /* FEATURE_CGPS */
