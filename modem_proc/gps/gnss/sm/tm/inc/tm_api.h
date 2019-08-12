#ifndef TM_API_H
#define TM_API_H


/*===========================================================================
  
                            TM_API
                   
DESCRIPTION:
  This header file contains TM API interface related constants, variables
  and function protocols.

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/inc/tm_api.h#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  10/26/15   ss       Supporting Geofence Client Config indication
  05/01/15    rh      Added get gps lock TM API
  04/28/14    ah      Added handling of Set Premium Services indication status
  01/28/13    mj      Modified tm_api_reg_task_init_notifier to add new param Client thread id
  01/10/13    gk      LPP ECID changes
  11/06/12    mj      Updated NV_CGPS_UTC_GPS_TIME_OFFSET_DEFAULT to 16
  08/01/12    mjohn   Added TM_CORE_MSG_ID_LSMP_GET_PARAM and TM_CORE_MSG_ID_LSMP_SET_PARAM msg id
  04/18/12    ssu     Adding sm_PositionReportStructType for GPS state information.
  03/01/12    rh      Added TM API tm_api_send_engine_error_recovery_report()
  10/10/11    rh      Changed XTRA SNTP servers to time.gpsonextra.net
  06/10/09    gk      ODP 2.0 support
  06/10/09    gk      Added Get UTC offset from NV
  01/12/09    atien   XTRA-T support    
  09/15/08    gk      Rename the on-demand struct to sm_ from cgps_
  04/25/08    gk      on-demand changes
  07/10/07    LT      Initial check-in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "gnss_consts.h"
#include "gnss_nv_loader.h"
#include "sm_api.h"
#include "sm_loc_lte_otdoa.h"
#include "gnss_lpp_ecid.h"
#ifdef FEATURE_CGPS_WCDMA
#include "rrccmd.h"
#endif
#include "wl_common.h"
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif
	 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               DEFINES                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*--------------------------------------------------

           IPC MSG ID definition

---------------------------------------------------*/
/* TM-Core MSG Definition Section */
#define TM_CORE_MSG_ID_DIAG                   0x1027   /* Forward to Diag Module */
/* TM-Core TM-LM API MSG */
#define TM_CORE_MSG_ID_SESS_REQ_API           0x1028   /* IPC MSG for TM-LM Session Request API */
#define TM_CORE_MSG_ID_SESS_UPDATE_API_0      0x1029   /* IPC MSG for TM-LM Session Update API */
#define TM_CORE_MSG_ID_SESS_INFO_API_0        0x1030   /* IPC MSG for TM-LM Session Info API */
#define TM_CORE_MSG_ID_PDAPI                  0x1031   /* IPC MSG for PD API */
/* TM-Core PDCOMM MSG */
#define TM_CORE_PDCOMM_MSG_ID_BASE            0x1032   /* IPC ID base for PD COMM */

#define TM_CORE_PDCOMM_MSG_ID_WRITE_ACK       TM_CORE_PDCOMM_MSG_ID_BASE
#define TM_CORE_PDCOMM_MSG_ID_READ_DATA       0x1033
#define TM_CORE_PDCOMM_MSG_ID_NOTIFY          0x1034
#define TM_CORE_PDCOMM_MSG_ID_IOCTL_ACK       0x1035
#define TM_CORE_PDCOMM_MSG_ID_HASH_RESP       0x1036
#define TM_CORE_PDCOMM_MSG_ID_HANDLE          0x1037
#define TM_CORE_PDCOMM_MSG_ID_SMS_ACK_REPORT  0x1038  
#define TM_CORE_ON_DEMAND_CHARGER_CONNECTED   0x1039   /* IPC from on demand charger poll */
#define TM_CORE_ON_DEMAND_CHARGER_REMOVED     0x1040   /* IPC from on demand charger poll */
#define TM_CORE_ON_DEMAND_BATTERY_FULLY_CHARGED 0x1041 /* IPC from on demand charger poll */
  
#define TM_CORE_CM_MSG_ID_CMD_NTFY            0x1051
#define TM_CORE_CM_MSG_ID_EVENT_NTFY          0x1052

/*RDM callbacks*/
#ifdef FEATURE_RUNTIME_DEVMAP
  #define TM_CORE_NMEA_MSG_ID_SIO_OPEN_CB     0x1053
  #define TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB    0x1054
#endif /* FEATURE_RUNTIME_DEVMAP */ 

#ifdef FEATURE_CGPS_UMTS_CP
  #define TM_CORE_MSG_ID_MM                   0x1055
#endif

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  #define  TM_CORE_MSG_ID_RRC                 0x1056
#endif   

#define TM_CORE_MSG_ID_MC_CELLDB_UPDATE       0x1057  /* IPC from MC updating Cell DB data base */  

#define TM_CORE_MSG_ID_L1                     0x1058  /* IPC from L1 */

#define TM_CORE_MSG_ID_GPS_STATE_INFO         0x1059

#define TM_CORE_ON_DEMAND_MSG_ID_CHARGER_CONNECTED 0x105A

#define TM_CORE_ON_DEMAND_MSG_ID_CHARGER_NOT_CONNECTED 0x105B

#define TM_CORE_ON_DEMAND_MSG_ID_BATTERY_FULLY_CHARGED 0x105C

/* This id is now available for use with new XTRA-T implementation :  TLE sends CellDB to XTM 
#define TM_CORE_MSG_ID_REPORT_CELL_DATA       0x105D  * IPC from MC sending segmented Cell DB data 
*/

#define TM_CORE_MSG_ID_LTE_L1                 0x105E  /* IPC from LTE L1 */


/*
 *  TM_RR_IFACE message(s)
 */
#define TM_CORE_RR_MSG_ID_EVENT_NTFY          0x1060

#define TM_CORE_MSG_ID_SIM_INIT_COMPLETED     0x1061


#define TM_CORE_MSG_ID_ODP_SESS_START           0x1066


#define TM_CORE_MSG_ID_SIM_CARD_INSERTED             0x1067
#define TM_CORE_MSG_ID_SIM_SUBSCRIPTION_READY        0x1068
#define TM_CORE_MSG_ID_SIM_SESSION_CHANGED           0x1069
#define TM_CORE_MSG_ID_SIM_SESSION_CLOSED            0x106A
#define TM_CORE_MSG_ID_MMGSDI_CLIENT_REG             0x106B
#define TM_CORE_MSG_ID_MMGSDI_LBS_SRV_AVAILABLE      0x106C
#define TM_CORE_MSG_ID_MMGSDI_CDMA_SVC_INITIALIZED   0x106D
#define TM_CORE_MSG_ID_MMGSDI_CMD_READ_CDMA_SVC_TBL  0x106E

#define TM_CORE_MSG_ID_LSMP_GET_PARAM                0x106F  /* IPC MSG ID for GET PARAM request from LSMP API */
#define TM_CORE_MSG_ID_LSMP_SET_PARAM                0x1070  /* IPC MSG ID for SET PARAM request from LSMP API */
#define TM_CORE_MSG_ID_LSMP_SEND_PREM_SVC_IND_STATUS 0x1071

#ifdef FEATURE_TM_SECURITY
#define TM_CORE_SEC_MSG_ID_CLNT_ACTIVATED     0x1080
#endif /* FEATURE_TM_SECURITY */

#define TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST   0x1090

/*****************************************************/
/*********** GEN8 Message ID Range Start *************/
/*****************************************************/
/* IPC MSG for TM-LM Session Update API */
#define TM_CORE_MSG_ID_SESS_UPDATE_API        SM_GEN8_IPC_ID_START_RANGE
/* IPC MSG for TM-LM Session Info API */
#define TM_CORE_MSG_ID_SESS_INFO_API            (SM_GEN8_IPC_ID_START_RANGE + 1)

/* IPC MSG for XTM (xtrat) - TM to report success of NV write*/
#define TM_CORE_MSG_ID_XTM_CLIENT_TOKEN         (SM_GEN8_IPC_ID_START_RANGE + 2)
#define TM_CORE_MSG_ID_XTM_SESSION_CONTROL      (SM_GEN8_IPC_ID_START_RANGE + 3)

/* SIO Close IPC */
#define TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB_2      (SM_GEN8_IPC_ID_START_RANGE + 4)
/* SIO Diag Close IPC */
#define TM_CORE_NMEA_MSG_ID_DIAG_SIO_CLOSE_CB_2 (SM_GEN8_IPC_ID_START_RANGE + 5)

/* TM-Core Geofence MSG */
#define TM_CORE_GM_MSG_ID_BASE                           (SM_GEN8_IPC_ID_START_RANGE + 6) 
#define TM_CORE_GM_MSG_ID_ADD_RESPONSE                   (TM_CORE_GM_MSG_ID_BASE) 
#define TM_CORE_GM_MSG_ID_EDIT_NOTIFY                    (SM_GEN8_IPC_ID_START_RANGE + 7)
#define TM_CORE_GM_MSG_ID_DELETE_RESPONSE                (SM_GEN8_IPC_ID_START_RANGE + 8) 
#define TM_CORE_GM_MSG_ID_PURGE_RESPONSE                 (SM_GEN8_IPC_ID_START_RANGE + 9) 
#define TM_CORE_GM_MSG_ID_BREACH_NOTIFY                  (SM_GEN8_IPC_ID_START_RANGE + 10) 
#define TM_CORE_GM_MSG_ID_POS_REQ                        (SM_GEN8_IPC_ID_START_RANGE + 11) 
#define TM_CORE_GM_MSG_ID_CLIENT_BLOB_NOTIFY             (SM_GEN8_IPC_ID_START_RANGE + 12)
#define TM_CORE_GM_MSG_ID_CLIENT_GEOFENCE_HANDELS_NOTIFY (SM_GEN8_IPC_ID_START_RANGE + 13)
#define TM_CORE_GM_MSG_ID_GEOFENCE_HANDELS_NOTIFY        (SM_GEN8_IPC_ID_START_RANGE + 14)
#define TM_CORE_GM_MSG_ID_QUERY_NOTIFY                   (SM_GEN8_IPC_ID_START_RANGE + 15)
#define TM_CORE_GM_MSG_ID_UNSOL_EVENT_NOTIFY             (SM_GEN8_IPC_ID_START_RANGE + 16) 
#define TM_CORE_GM_MSG_ID_PRIO_POS_REQ_INDICATION_NOTIFY (SM_GEN8_IPC_ID_START_RANGE + 17)
#define TM_CORE_GM_MSG_ID_SET_ENGINE_CONFIG_NOTIFY       (SM_GEN8_IPC_ID_START_RANGE + 18) 
#define TM_CORE_GM_MSG_ID_ABORT_POS_REQ                  (SM_GEN8_IPC_ID_START_RANGE + 19) 
#define TM_CORE_GM_MSG_ID_BATCH_BREACH_NOTIFY            (SM_GEN8_IPC_ID_START_RANGE + 20) 
#define TM_CORE_GM_MSG_ID_PROXIMITY_NOTIFY               (SM_GEN8_IPC_ID_START_RANGE + 21) 
#define TM_CORE_GM_MSG_ID_REPLACE_NOTIFY                 (SM_GEN8_IPC_ID_START_RANGE + 22)
#define TM_CORE_GM_MSG_ID_BATCH_DWELL_NOTIFY             (SM_GEN8_IPC_ID_START_RANGE + 23)
#define TM_CORE_GM_MSG_ID_TZ_REQ_NOTIFY                  (SM_GEN8_IPC_ID_START_RANGE + 24) 
#define TM_CORE_GM_MSG_ID_SET_CLIENT_CONFIG_NOTIFY       (SM_GEN8_IPC_ID_START_RANGE + 25) 

#define TM_CORE_GM_MSG_ID_LAST  (SM_GEN8_IPC_ID_START_RANGE + 50)

#define TM_CORE_MGP_WBIQ_INFO                   (SM_GEN8_IPC_ID_START_RANGE + 51) 

#define TM_CORE_SM_LOC_LTE_OTDOA_MEASUREMENTS    (SM_GEN8_IPC_ID_START_RANGE + 52)

/*RRC cell info update*/
#define TM_CORE_MSG_ID_RRC_CELL_INFO_DCH_REPORT (SM_GEN8_IPC_ID_START_RANGE + 53)

/* Fast TCal Data Report Messages */
#define TM_CORE_MSG_ID_FTCAL_TT_DATA            (SM_GEN8_IPC_ID_START_RANGE + 54)
#define TM_CORE_MSG_ID_FTCAL_PPM_DATA           (SM_GEN8_IPC_ID_START_RANGE + 55)

/* Fast TCal Enable/Disable Request */
#define TM_CORE_MSG_ID_FTCAL_ENABLE             (SM_GEN8_IPC_ID_START_RANGE + 56)
#define TM_CORE_MSG_ID_FTCAL_DISABLE            (SM_GEN8_IPC_ID_START_RANGE + 57)

/* Engine Error Recovery Report */
#define TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT  (SM_GEN8_IPC_ID_START_RANGE + 58)



/* Griffon App Track Test Message (to be removed shortly) */
#define TM_CORE_DUMMY_GRIFFON_MSG                  (SM_GEN8_IPC_ID_START_RANGE + 59)

/*Reset Location Service Done*/
#define TM_CORE_MSG_ID_RESET_LOCATION_SERVICE_DONE (SM_GEN8_IPC_ID_START_RANGE + 60)

/* Best available position Report */
#define TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT  (SM_GEN8_IPC_ID_START_RANGE + 61)

/* Engine DPO status Report */
#define TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT  (SM_GEN8_IPC_ID_START_RANGE + 62)

/* LPP Control Plane Protocol Related */
#define TM_CORE_MSG_ID_NAS_EVENT_BASE           (SM_GEN8_IPC_ID_START_RANGE + 63)

#define TM_CORE_MSG_ID_NAS_DL_DATA_EVENT        (TM_CORE_MSG_ID_NAS_EVENT_BASE)
#define TM_CORE_MSG_ID_NAS_FAILURE_EVENT        (SM_GEN8_IPC_ID_START_RANGE + 64)
#define TM_CORE_MSG_ID_NAS_EVENT_LAST           (SM_GEN8_IPC_ID_START_RANGE + 69)

#define TM_CORE_GNSS_LPP_ECID_MEASUREMENTS    (SM_GEN8_IPC_ID_START_RANGE + 70)

/* Request to retrieve IMSI */
#define TM_CORE_MSG_ID_RETRIEVE_IMSI           (SM_GEN8_IPC_ID_START_RANGE + 71)

/* TM API registration MSG ID */
#define TM_CORE_MSG_ID_API_REGISTRATION			(SM_GEN8_IPC_ID_START_RANGE + 72)

/*Wifi Fix Response*/
#define TM_CORE_MSG_ID_WIFI_FIX_RESP			  (SM_GEN8_IPC_ID_START_RANGE + 73)




/* TM API to receive LPPM Status Report  */
#define TM_CORE_MSG_ID_LPPM_STATUS_REPORT			(SM_GEN8_IPC_ID_START_RANGE + 75)


/* TM IPC to re-read carrier-specific NV  */
#define TM_CORE_MSG_ID_REFRESH_CARRIER_SPECIFIC_NV	(SM_GEN8_IPC_ID_START_RANGE + 76)


/*
 * Default NV Item Value for XTRA
 */
#define NV_AAGPS_DEFAULT_XTRA_DOWNLOAD_INTERVAL         (48)     // hour
#define NV_AAGPS_DEFAULT_XTRA_NUM_DOWNLOAD_ATTEMPTS     (3)
#define NV_AAGPS_DEFAULT_XTRA_TIME_BETWEEN_ATTEMPTS     (10)     // minute
 
#define NV_AAGPS_DEFAULT_XTRA_SERVER_URL_SIZE           (128)

/* Default NV Item Value for XTRA Time Information */
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_ENABLED                (1)
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DISABLED               (0)
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_UNC_THRESH             (12500)   // ms
#define  NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DELAY_THRESH           (1000)    // ms

/* Default NV Item Value for Sensors configuration parameters */
#define  NV_CGPS_DEFAULT_SENSORS_SPI_OPINTERVAL                 (5000)    /* msec */
#define  NV_CGPS_DEFAULT_SENSORS_SPI_DETECTTHRESHOLD            (47)      /* milliG */


/* MSBased aiding throttling feature. This feature throttles MS-PDE 
** connections to download Aiding data, AFLT seed, Eph & Almanac.
** MS starts with a min back-off and goes up to max back-off, increasing
** the interval by back off factor. If the requests shows up on time 
** after the safe duration, then the back off time is reset back to min.
*/
#define GPSONE_MSB_THROTTLE_ENABLE TRUE
#define GPSONE_MSB_THROTTLE_DISABLE FALSE
#define BACK_OFF_FACTOR 4             /* Exp back-off by a factor of 4 */
#define BACK_OFF_MIN_DURATION 60      /* Min duration is 1 min */
#define BACK_OFF_MAX_DURATION 900     /* Max is 15 min's */
#define BACK_OFF_RESET_DURATION 900   /* Safe value is 15 min's */

/* Default NV Item Value for XTRA-T */
#define NV_AAGPS_DEFAULT_XTRA_T_ENABLED                   (1) 
#define NV_AAGPS_DEFAULT_XTRA_T_DISABLED                  (0) 

/* Bit definitions for the NV_AAGPS_DEVELOPMENT_TEST_CONTROL5 word */
#define NV_AAGPS_TEST5_EMERGENCY_CALL_SIM     ( 1 << 2 )  /* simulated e-call */


/* Maximum number of callbacks that can be registered. The callbacks are
   for potential clients to be notified of the fact that engine is ready to
   accept incoming clients

   Current clients (Please increase limit and add clients here):
   [0]                     - QMI LOC
   [1]                     - SDP
   [2]                     - LBS
*/

#define TM_MAX_NOTIFIERS                      3

/* Future new clients that register callbacks with TM will have to add their indexes, and update the TM_MAX_NOTIFIERS */
#define TM_LOC_MW_CLIENT_INDEX	(0)
#define TM_SDP_CLIENT_INDEX		(1)
#define TM_LBS_CLIENT_INDEX		(2)

#define TM_UNSUPPORTED_CLIENT_INDEX (TM_MAX_NOTIFIERS)

/* Enum for XTM set nv items */
typedef enum
{
  TM_XTM_SET_CLIENT_TOKEN = 0x0001,     /* sets the client token NV*/
  TM_XTM_SESSION_CONTROL  = 0x0002,     /* sets the session control NV*/
} tm_xtm_nv_e_options;

typedef union
{
  uint32 client_token;
  uint8 session_control;
} tm_xtm_nv_u_data;

typedef struct
{
  tm_xtm_nv_e_options xtm_nv_item_set;     /*Type of NV item set by XTRAT */
  tm_xtm_nv_u_data xtm_nv_data_set;        /*Contains the data that is being set*/
  int client_id;                           /*Type of client */
} tm_xtm_struct_nv_write_status;

#ifdef FEATURE_CGPS_WCDMA
typedef struct {
  rrcmeas_plmn_cell_info_struct_type cell_id_info;
  rrc_cell_id_status_type            status;
} tm_rrc_cell_info_dch_report_type;
#endif

/* -----------------------------------------------------------------------*//**
  This callback function is called by GPS tasks once they are up and initialized. 
*//* ------------------------------------------------------------------------*/
typedef void (*tm_TaskInitNotifierCb)
(
  uint32 taskId
);


/* -----------------------------------------------------------------------*//**
  This callback function is called to deliver IMSI information when it has been retrieved. 
*//* ------------------------------------------------------------------------*/
typedef void (*tm_RetrieveIMSICb)
(
  const cgps_IMSIType * const pz_IMSIInfo
);

typedef struct
{
  uint8 sub;
  tm_RetrieveIMSICb pz_RetrieveIMSICb;
} tm_RetrieveIMSIType;


typedef struct {
  cgps_ActiveRATEnumType e_TriggerRAT;
  cgps_CellInfoCachedStruct cellInfoCached;
} tm_cell_info_update_type;

typedef struct {
  sm_ApiInitNotifierCb pNotifierCb; /* Callback registered with TM */
  uint8 clientIndex;				/* Index into the CB array, unique for each clients */
  boolean broadcast;				/* Go through the CB array and call all CBs if registered */
} tm_api_notifier_type;

typedef struct {
  sm_GnssFixRptStructType z_PosFixRpt;
  wl_ResultFlagsType z_wifi_result;
  wl_PosReqSrcType e_wifi_pos_src;
} tm_wifi_fix_response_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Array to hold the callbacks for notification */
extern sm_ApiInitNotifierCb   pz_SmNotifierCb[TM_MAX_NOTIFIERS];


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
 * @brief Converts CDMA timestamp into milliseconds
 *
 * This function takes a timestamp in the format generated by time_get()
 * and converts it to the implied total number of gps milliseconds.
 *
 * @param[in]  pt_CmdaTimestamp   Quadword with CDMA timestamp.
 * @param[out] pt_MillisecondsOut Quadword with timestamp in milliseconds.
 *
 * @return None.
 */
void gps_total_msec
(
  const qword pt_CdmaTimestamp,
  qword       pt_MillisecondsOut
);

/**
 * @brief Convert quadword timestamp into GPS week number and milliseconds.
 *
 * This function takes a total number of gps milliseconds and returns the
 * week and the number of milliseconds in the week.
 *
 * @param[in]  pt_Milliseconds Quadword with milliseconds.
 * @param[out] pw_GpsWeekOut   GPS week number.
 * @param[out] pq_GpsMsOut     GPS millisecond since week start.
 *
 * @return None
 */
void gps_week_ms
(
  const qword  pt_Milliseconds,
  uint16      *pw_GpsWeekOut,
  uint32      *pq_GpsMsOut
);

/**
 * @brief Convert GPS time into CDMA system time
 *
 * This function takes time stored in GPS Time and converts it into
 * CDMA system time.
 *
 * @param[in]  pz_Clock        GPS time structure.
 * @param[out] pt_TimestampOut Quadword pointer for CDMA time.
 *
 * @return None
 */
void gps_to_cdma_time
(
  const gps_TimeStructType *pz_Clock,
  qword                     pt_TimestampOut
);

/*******************************************************************************
 * gps_MsecWeekLimit
 *
 * Function description:
 *
 *  me_GpsMsecWeekLimit is a helper function used to perform the 
 *  fairly common check to see if a msec is indeed with the allowable
 *  range of 0 thru WEEK_MSECS-1. Only values within a single week
 *  are entertained. (ie- Multiple week adjustments are not performed
 *
 *  Assigning a NULL pointer to p_GpsWeek will disable the week 
 *  adjustment logic ... ie - The p_GpsMsec will only be affected.
 *
 * Parameters: 
 *
 *  p_GpsMsecs - Pointer to the msec value under test
 *  p_GpsWeek - Pointer to week number which may be adjusted
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void gps_MsecWeekLimit(int32 *p_GpsMsecs, uint16 *p_GpsWeek);

/*******************************************************************************
 * tm_api_store_ftcal_tt_data
 *
 * Function description:
 *
 * Parameters: 
 *
 *  
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_tt_data (cgps_FtcalTTStruct* p_TTData);

/*******************************************************************************
 * tm_api_store_ftcal_ppm_data
 *
 * Function description:
 *
 * Parameters: 
 *
 *  
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_ppm_data (cgps_FtcalPpmStruct* p_PpmData);

/*******************************************************************************
 * tm_api_update_cell_info
 *
 * Function description:
 *
 * Parameters: 
 *
 *  p_CellInfoCached - Pointer to Cell ID information
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_update_cell_info ( const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct * const p_CellInfoCached );


/*******************************************************************************
 * tm_api_update_pos_punc_tunc
 *
 * Function description:
 *
 * Parameters: 
 *
 *  p_PosInfo - Pointer to the best available position fix info.
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_update_gps_state_info ( const sm_PositionReportStructType* const p_PosInfo );

/*******************************************************************************
 * tm_api_report_nv_utc_offset
 *
 * Function description:
 *
 * Parameters: 
 *
 *  p_UtcOffset - Pointer to NV UTC offset information
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/
void tm_api_report_nv_utc_offset(uint8 *p_UtcOffset);


/*===========================================================================
FUNCTION tm_xtra_copy_data

DESCRIPTION
This function is called by PD API at client's context. Be sure only to perform
simple operations in this function.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA command status
===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_xtra_copy_data( uint8 part_num, byte *xtra_part_ptr, uint32 xtra_part_length );




/* Enum for Sensor Data Streams */
typedef enum
{
  /* Client 1 data streams */
  TM_SENSOR_DATA_MOTION_LOW_POWER = 0x0001,     /* low power stationary pos indicator*/
  TM_SENSOR_DATA_VEL_1HZ_1SEC     = 0x0002,     /* velocity process noise 1Hz operating interval 1s */
  TM_SENSOR_DATA_JERK_1HZ_1SEC    = 0x0004,     /* jerk variance 1Hz operating interval 1s */

  /* Client 2 data streams */
  TM_SENSOR_DATA_MOTION_1HZ       = 0x0008,     /* stationary pos indicator 1Hz */
  TM_SENSOR_DATA_VEL_1HZ_100MS    = 0x0010,     /* velocity process noise 1Hz operating interval 100ms */
  TM_SENSOR_DATA_JERK_1HZ_100MS   = 0x0020      /* jerk variance 1Hz operating interval 100ms */
} tm_sensor_data_streams_bitmask;


/*******************************************************************************
 * tm_api_XTM_NV_status_report
 *
 * Function description:
 *  Called by XTRAT module to report the success of NV write to TM thread.
 *
 * Parameters:
 * which nv item has been set 
 * 
 * Return value: 
 * none
 *
 *******************************************************************************/

extern boolean tm_api_xtm_nv_status_report(tm_xtm_struct_nv_write_status* xtm_nv_set );

/*===========================================================================

FUNCTION tm_cm_map_sys_mode

DESCRIPTION
  This function maps a sys_sys_mode_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  sys_mode - The sys_sys_mode_e_type variable to be mapped
 
DEPENDENCIES
  None.

RETURN VALUE
  cgps_SrvSystemType

SIDE EFFECTS
  None

===========================================================================*/
cgps_SrvSystemType tm_cm_map_sys_mode(sys_sys_mode_e_type sys_mode);

/**
@brief dispatches an asynchronous request for TM to raise the external spi 
       injection event and request SPI injection
@param
@return
*/
void tm_api_xspi_start_injection( void );

/**
@brief dispatches an asynchronous request for TM to raise the external spi 
       injection event and stop SPI injection
@param
@return
*/
void tm_api_xspi_stop_injection( void );

/*=============================================================================

FUNCTION
  tm_api_reg_task_init_notifier

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called
  inline.  The caller should be ready to handle this and queue an IPC to itself.

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered
  clientThreadId - Client Thread ID to identify the index within the array

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_reg_task_init_notifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId);

/**
@brief Calls the tm_xtra function for indicating xtra injection 
       has completed
@param None
@return None
*/
void tm_api_xtra_inj_done(void);

/*******************************************************************************
 * tm_api_report_wbiq_info
 *
 * Function description:
 *  Called by LM to update TM with the latest WBIQ FFT info
 *
 * Parameters: 
 *
 *  p_ReportWBIQInfo - WBIQ Info
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_wbiq_info (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo);


/*===========================================================================
  tm_loc_lte_otoda_recv_measurements

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_loc_lte_otdoa_recv_measurements(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info);



/*===========================================================================
  tm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean tm_loc_lte_ecid_recv_measurements(gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info);

#ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
/*******************************************************************************
*   tm_rrc_iface_cell_info_dch_report_callback
* 
* Function description:
*   This function is registered as a callback function with RRC module and
*   called whenever RRC responds to cell id request sent by TM.
* 
* Parameters:
*   Cell ID information: 
*      rrcmeas_plmn_cell_info_struct_type cell_id_info
*   Status: (RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)  
*      rrc_cell_id_status_type            status 
* 
* RETURN VALUE
*   None 
*
 *******************************************************************************/

void tm_rrc_iface_cell_info_dch_report_callback(
    rrcmeas_plmn_cell_info_struct_type cell_id_info,
    rrc_cell_id_status_type            status );

#endif /*FEATURE_WCDMA_GPS_CELL_ID_READING */

/*******************************************************************************
 * tm_api_ftcal_request
 *
 * Function description:
 *  Called by LM to update TM that FTCal is enabled/disabled 
 *
 * Parameters: 
 *
 *  b_Enable - TRUE to enable FTCal 
 *             FALSE to disable FTCal
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_ftcal_request (boolean b_Enable);


/*=============================================================================

FUNCTION
 tm_api_reset_location_service_done

DESCRIPTION
  Indication from MGP that location service completed.

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_reset_location_service_done(pdsm_pd_ext_reset_location_service_status_e_type status);

/*=============================================================================

FUNCTION
  tm_api_send_engine_error_recovery_report

DESCRIPTION
  TM API to send engine error recovery report to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_send_engine_error_recovery_report( gnss_engine_err_recovery_rpt *pErrReport );

/*
******************************************************************************
* tm_read_qmi_services_configuration
*
* Function description:
*
*  This function is called by QMI services to read which QMI services are 
*  enabled. This function can be called only after SM has been initialized. 
*
* Parameters:
*  output_cfg : QMI Configuration read from NV
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_read_qmi_services_configuration( pdsm_QMIServiceConfig* output_cfg);

/*
******************************************************************************
* tm_retrieve_IMSI
*
* Function description:
*
*  This function is called by XTM to request TM core to retrieve the IMSI
*
* Parameters:
*  NONE
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_retrieve_IMSI( uint8 sub, tm_RetrieveIMSICb p_RetrieveImsiInfoCb );


/*=============================================================================

FUNCTION
  tm_api_send_dpo_status_report

DESCRIPTION
  TM API to send DPO status report to TM which inturn sends it to QMI.

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_send_dpo_status_report( sm_DpoStatusReportStructType *p_DpoStatusReport );

/*===========================================================================
FUNCTION tm_core_get_param_msg_handler

DESCRIPTION
  This function handles the get param ipc msg, and sends an ipc msg as a response
  with the param value retrieved
  
DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS


===========================================================================*/
extern boolean tm_core_get_param_msg_handler(void *p_Data);

/*===========================================================================
FUNCTION tm_core_handle_prem_svc_status

DESCRIPTION
  This function handles the send premium service indication status ipc msg, 
  and calls tm_pdapi_pa_event_callback to send event to registered clients.

DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_prem_svc_status(void *p_Data);

/*===========================================================================

FUNCTION tm_api_get_gps_lock_status()

DESCRIPTION
  TM module can call this function to get current GPS lock status
 
  This is a synchronous API, will return immediately
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
extern pdsm_gps_lock_e_type tm_api_get_gps_lock_status(void);

/*===========================================================================
  tm_handle_wifi_fix_response

  Description:

     This function is used by SM to provide Wifi fix response to TM
 Parameters:
   sm_GnssFixRptStructType: Pointer to Position Report
                                            Note that this may be non Wifi position 
                                            if there was a better
                                            position available in ALE.
    wifi_ResultFlagsType: Wifi Results Flag
          The result flag indicates any error during Wifi pos fix calculation
          like no modem cache, no measurements, etc. It also indicates 
          whether a better non Wifi fix was sent instead. 
    sm_WifiPosReqSrcType: Wifi Position fix source (originiator)
  Return value: 
   VOID
=============================================================================*/
void tm_handle_wifi_fix_response(const sm_GnssFixRptStructType* pz_PosFixRpt,
                                 wl_ResultFlagsType z_wifi_result,
                                 wl_PosReqSrcType e_wifi_pos_src);

#endif /* TM_IFACE_H */
