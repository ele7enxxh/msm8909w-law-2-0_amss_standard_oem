#ifndef TLE_API_H
#define TLE_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE Header
GENERAL DESCRIPTION
   This file contains constants, structures and functions prototypes
   related to TLE (e.g Terrestrial Location Manager).


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/tle/1_0/inc/tle_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
10/15/2010  atien Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_variation.h"
#include "gps_types.h"
#include "cgps_api.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "mgp_api.h"
#ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
#include "rrclsmif.h"
#endif /* FEATURE_WCDMA_GPS_CELL_ID_READING */
#include "loc_wwan_me_api.h"
#include "tdp_common.h"
#include "gnss_gdt.h"
#include "rtpersrc/asn1per.h"
#include "location_service_v02.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/
#define TLE_POS_UNC_MAX   (25000)  /* [meter] 
                                      Maximum position uncertainty threshold 
                                      if equal or exceeding then TLE triggers a
                                      XTRA-T Session. 
                                   */

#define TLE_INVALID_PHY_ID  (0xFFFF)


/* for XTM_SESSION_REQUEST_START */
#define TLE_DATA_XFR_UPLOAD_MASK   (0x01)  /* for Upload with free Download */
#define TLE_DATA_XFR_DNLOAD_MASK   (0x02)  /* for Upload with free Download */
#define TLE_DATA_XFR_DN_UP_MASK    (0x04)  /* for Download only followed by upload only */
#define TLE_DATA_XFR_DEFAULT_MASK  (TLE_DATA_XFR_UPLOAD_MASK | TLE_DATA_XFR_DNLOAD_MASK)
#define TLE_DATA_XFR_ALL_MASK      (TLE_DATA_XFR_UPLOAD_MASK | TLE_DATA_XFR_DNLOAD_MASK | TLE_DATA_XFR_DN_UP_MASK)

/*  Guard time interval (in ms) after a diag delete cmd during which pos inj are rejected */
#define C_TLE_POS_INJ_DELETE_GUARD_TIME_INTERVAL 0

/*=============================================================================

       Macros

=============================================================================*/

/*==============================================================================

                                  VARIABLES

==============================================================================*/
/* This variable indicates whether external TLE APIs should be disabled */
extern boolean tle_bDisableAllAPIs;


/*=============================================================================

       Structures and enumerated types

=============================================================================*/
typedef struct
{
  boolean               b_Valid;
  gnss_ClockStructType  z_GnssClk;
} tle_ClockStructType;

typedef struct
{
  FLT f_PosUnc;
  boolean u_Valid;
  boolean b_XtaExist;
} tle_PosUncStructType;

/* xtm_SendE911Events */
typedef enum 
{
  C_TLE_EVENT_E911_NONE,
  C_TLE_EVENT_E911_CALL_ORIG,
  C_TLE_EVENT_E911_CALL_END,
  C_TLE_EVENT_E911CB_ENTER,
  C_TLE_EVENT_E911CB_EXIT
} tle_e911_event_e_type;

/* xtm_SendAGPSEvents */
typedef enum 
{
  C_TLE_EVENT_AGPS_NONE,
  C_TLE_EVENT_AGPS_PDCOMM_CONNECTED,
  C_TLE_EVENT_AGPS_PDCOMM_DISCONNECTED
} tle_agps_pdcomm_event_e_type;

typedef enum 
{
  TLE_SESSION_REQUEST_START,
  TLE_SESSION_REQUEST_STOP
} tle_request_e_type;

/* xtm_RequestSession() - request a data transfer session in XTM Task */
typedef enum 
{
  TLE_SESSION_SOURCE_NONE = 0,
  TLE_SESSION_SOURCE_TEST,             /* SMART XTRA-T or gpsOneDiag */
  TLE_SESSION_SOURCE_TLM,
  TLE_SESSION_SOURCE_SELF,             /* XTM background timers      */
  TLE_SESSION_SOURCE_XTRA,             /* XTRA session               */
  TLE_SESSION_SOURCE_AGPS,             /* AGPS session               */
  TLE_SESSION_SOURCE_XTM_PUNC,         /* PUNC exceeding threshold   */
  TLE_SESSION_SOURCE_SIZE = 0x100000   /* To fix enum size as int    */
} tle_session_source_e_type;

/*---------------------------*/
/* XTRA-T Session End Reason   */
/*---------------------------*/
typedef enum xtm_sess_end_e 
{
  TLE_SESS_END_SESS_NONE = -1,           /* For internal use of XTM             */ 
  TLE_SESS_END_NO_CONN_HANDLE,           /* Cannot get handle                   */ 
  TLE_SESS_END_NO_CONN,                  /* No connecntion with XTS             */ 
  TLE_SESS_END_DUP_CONN,                 /* Duplicate connection request        */
  TLE_SESS_END_XTS_ADDR_INVALID,         /* No valid XTS address                */   
  TLE_SESS_END_XTS_CONN_TIMER_FAILED,    /* XTS Connection Timer failed         */ 
  TLE_SESS_END_XTS_TLM_INIT_FAILED,      /* Cannot init TLM                     */ 
  TLE_SESS_END_XTS_CONN_TIMER_EXPIRED,   /* XTS Connection Timer expired        */ 
  TLE_SESS_END_XTS_DISCONN_TIMER_EXPIRED,/* XTS Disconnect Timer expired        */ 
  TLE_SESS_END_UDM_DATA_TIMER_FAILED,    /* UDM Data Timer expired              */ 

  TLE_SESS_END_MEAS_FAILED,              /* Session abort due to TLM Measurement encode failure  */
  TLE_SESS_END_MSD_FAILED,               /* Session abort due to Mobile Status Data encode failure  */
  TLE_SESS_END_MSH_FAILED,               /* Session abort due to Mobile Status Header encode failure  */
  TLE_SESS_END_CLIENT_INFO_FAILED,       /* Session abort due to CLIENT_INFO encode failure  */
  TLE_SESS_END_XTA_PROT_FAILED,          /* Session abort due to XTA Protocol init failure */
  TLE_SESS_END_ABORT_XTA_ENCODE,         /* Session abort due to XTA encode failure */
  TLE_SESS_END_ABORT_XTA_DECODE,         /* Session abort due to XTA decode failure */
  TLE_SESS_END_ABORT_E911,               /* Session abort due to E911           */
  TLE_SESS_END_ABORT_COMM_FAILURE,       /* Session abort due to PDCOMM failure */
  TLE_SESS_END_ABORT_AGPS,               /* Session abort due to active AGPS session */ 

  TLE_SESS_END_INVALID_STATE,            /* Session end due to unexpected state */
  
  TLE_SESS_END_TLM_BEGIN,                /* TLM "initiated" related end reason */      
  TLE_SESS_END_TLM_INVALID_MSG = TLE_SESS_END_TLM_BEGIN,
  TLE_SESS_END_TLM_INVALID_SESSION,
  TLE_SESS_END_TLM_ACK_TIME_OUT,
  TLE_SESS_END_TLM_INVALID_PARTITION,
  TLE_SESS_END_TLM_INVALID_MOBILE_STATUS, 
  TLE_SESS_END_TLM_UPLOAD_FAILED, 
  TLE_SESS_END_TLM_OUT_OF_MEMORY,

  /* NOTE: Update this when adding new TLM "initiated" end reason*/
  TLE_SESS_END_TLM_END = TLE_SESS_END_TLM_OUT_OF_MEMORY, 
  
  TLE_SESS_END_TLM_IPC_FAILED,           /* This error is not TLM "initiated". */

  TLE_SESS_END_MAX,                      /* Maximum for XTM internal use   */
  TLE_SESS_END_E_SIZE = 0x10000000       /* To fix enum Size as int        */
} tle_sess_end_e_type;

/* tle_XTRAEventNotify */
typedef enum 
{
  TLE_EVENT_XTRA_NONE = 0,
  TLE_EVENT_XTRA_START,
  TLE_EVENT_XTRA_STOP
} tle_xtra_event_e_type;

typedef enum 
{
  TLE_API_E_USER_SESS_CONTROL = 0,
  TLE_API_E_CLIENT_TOKEN,
  TLE_API_E_END_SIZE = 0x100000        /* To fix enum size as int */
} tle_api_info_e_type;


typedef enum 
{
  TLE_API_E_GTP_DISABLED = 0,
  TLE_API_E_GTP_BASIC,
  TLE_API_E_GTP_POWER_DISABLED,
  TLE_API_E_GTP_PREMIUM,
  TLE_API_E_GTP_MAX
} tle_api_gtp_config_e_val;
typedef uint8 tle_api_gtp_config_e_type;

typedef enum
{
  TLE_API_E_SERV_GTP = 0,
  TLE_API_E_SERV_TDP, 
  TLE_API_E_SERV_MAX
} tle_api_service_e_val;
typedef uint16 tle_api_service_e_type;

typedef struct
{
  tle_api_service_e_type    e_Service;
  tle_api_gtp_config_e_type e_Config;
} tle_api_gtp_config_s_type;

/*---------------------------*/
/* IPC Message Payload       */
/*---------------------------*/
/* xtm_SendXTRATApiInfo */
typedef union xtm_api_data_type 
{
  uint8    session_control;          /* User session control */
  uint32   client_token;             /* Client token         */
} tle_api_data_u_type;

typedef struct xtm_api_info_type 
{
  tle_api_data_u_type   data;
  tle_api_info_e_type   type;
  int                   client_id;
} tle_api_info_s_type;

typedef struct 
{
  tle_sess_end_e_type    e_end_reason;  /* XTM_SESSION_REQUEST_STOP  */
  uint8                  data_xfr_type; /* XTM_SESSION_REQUEST_START */
  boolean                b_deferred; /* XTM_SESSION_REQUEST_START */
} tle_request_s_type;

typedef struct 
{
  tle_request_e_type              e_req_type;
  tle_session_source_e_type       e_source;
  tle_request_s_type              u_req_data;
} tle_request_session_s_type;


/* Structure for the ALE input position */
typedef struct 
{
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  DBL     d_PosLLa[3];    
  FLT     f_PosUnc99;       /* Horizontal Uncertainty in meters (circular) */
  FLT     f_AltUnc68;       /* Altitude Uncertainty in meters */
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  uint16 w_TechContributions[TECHNOLOGIES_MAX]; /* Technology contributions.  */
  boolean b_PropagatedPosition; /*Flag to indicate if PE position is propagated */ 
  boolean b_SpeedAndHeadingValid;  /* Flag to indicate if speed and heading information is valid */
  FLT f_HorSpeedMps; /*Horizontal speed in m/s*/
  FLT f_HorSpeedUnc99Mps; /*Horizontal speed uncertainty in m/s*/
  /* The horSpeedUncertainty default confidence interval is 99%.*/
  FLT f_HeadingRad; /* Heading in units of radian */
  FLT f_HeadingUnc99Rad; /* Heading uncertainty in units of radian */
  /* The headingUncertainty default confidence interval is 99%.*/
} tle_InputPosStructType; 

typedef struct
{
  qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 z_ClienDownloadedReq;
  int             l_Client_handle;
  uint32          q_TransactionId;
} tle_GtpClientDownloadedReqType;

/*=============================================================================

       Functions

=============================================================================*/

/*=============================================================================

  FUNCTION      tle_RequestSessionByXtra

  DESCRIPTION 	This function is called when XTRA session ends successfully.
                This event triggers a XTRAT upload/download session.
                
  DEPENDENCIES  None

  PARAMETERS    std_active - TRUE if active Standalone session

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
void tle_RequestSessionByXtra(boolean std_active);

/*=============================================================================

  FUNCTION      tle_GetTerrPosStatus

  DESCRIPTION 	This function is called to report current cell db position
                uncertainty to XTM. If exceeding a threshold then
                XTRA-T session will be triggered for download.
                
  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_GetTerrPosStatus(void);

/*=============================================================================

  FUNCTION      tle_XTRAEventNotify

  DESCRIPTION 	This function is called to notify XTM a XTRA session start/stop event.

                
  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  TRUE - IPC sent successfully; FALSE - otherwise
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_XTRAEventNotify(tle_xtra_event_e_type xtra_event);

/*==============================================================================
  FUNCTION      tle_LteCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new LTE cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_LteCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_LteCellChange(uint8 b_SubscrID, const cgps_LteCellInfoType * const cpz_LteCell);

/*==============================================================================
  FUNCTION      tle_TDScdmaCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new TDScdma cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_TdsCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_TDScdmaCellChange(uint8 b_SubscrID, const cgps_TDScdmaCellInfoType * const cpz_TdsCell);

/*==============================================================================
  FUNCTION      tle_SSChange

  DESCRIPTION 	
    Alert tle of a change in serving cell type or in 1x cell information (SID,
    NID, bsid, lat, lon, search window size).
 
  DEPENDENCIES
    None

  PARAMETERS
    p_SSInfo - The type must be:
               CGPS_SSINFO_SRV_SYS - Contains the serving system type.
               CGPS_SSINFO_CELLINFO - Contains the complete 1x serving cell
                info.  If position is not valid, the lat and lon shall be set
                to 0.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_SSChange(const cgps_SSInfoType * const p_SSInfo);


/*==============================================================================
FUNCTION
  tle_DeleteTime

DESCRIPTION
  Deletes time in TLE.  
  
PARAMETERS
   
DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   The requested data is deleted from the database as well as PTM.

==============================================================================*/
void tle_DeleteTime(void);

/*==============================================================================
FUNCTION
  tle_AlterTimetagTime

DESCRIPTION
  Called to alter the time tag.  This is either deleting the time tag, or it
  could entail changing the offset &/or the uncertainty.
  
PARAMETERS 
  u_Delete - TRUE if the timetag should be deleted.
 
  f_TimeOffsetMs - Time in ms to adjust the timetag offset (only if u_Delete is
   FALSE).
 
  f_TimeUncMs - Time in ms to adjust the timetag uncertainty (only if u_Delete
   is FALSE).
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The DB will be updated as well.

==============================================================================*/
void tle_AlterTimetagTime(boolean u_Delete, FLT f_TimeOffsetMs, FLT f_TimeUncMs);

/*==============================================================================
  FUNCTION      tle_UpdateSlowClkTime

  DESCRIPTION 	
    The callback to register when trying to get the current gps time.

  DEPENDENCIES
    The signature of this function needs to conform to gnss_SlowClkCbType.

  PARAMETERS
    cpz_GnssClk - constant pointer to the clock structure that contains the
                  current gps time.
 
    b_Valid     - TRUE if the data pointed to by cpz_GnssClk is valid.
                  FALSE if the data pointed to by cpz_GnssClk is not valid.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_UpdateSlowClkTime(const gnss_ClockStructType * cpz_GnssClk,
                           boolean b_Valid);

/*==============================================================================
  FUNCTION      tle_WcdmaCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new WCDMA cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_WcdmaCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_WcdmaCellChange(uint8 b_SubscrID,
 const cgps_WcdmaCellInfoType * const cpz_WcdmaCell);

/*==============================================================================
  FUNCTION      tle_GsmCellChange

  DESCRIPTION 	
    Alert tle of a change in serving cell to a new GSM cell.

  DEPENDENCIES
    None

  PARAMETERS
    b_SubscrID - The subscription of the cell change.
 
    cpz_GsmCell - The cell info of the new cell.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_GsmCellChange(uint8 b_SubscrID,
                       const cgps_GsmCellInfoType * const cpz_GsmCell);

/*===========================================================================
FUNCTION
  tle_UpdateTimeTag

DESCRIPTION
  This function is used to push a time tag update into the TLM.
  
PARAMETERS 
  b_SubscrID - The subscription that the timetag belongs to.
 
  cpz_TimeTag - Pointer to the new time-tag information
 
DEPENDENCIES
   None

RETURN VALUE
   TRUE - 

SIDE EFFECTS
   None

===========================================================================*/
void tle_UpdateTimeTag(uint8 b_SubscrID,
                       const gnss_TimeTagStructType * cpz_TimeTag);

/*===========================================================================
FUNCTION
  tle_DeleteData

DESCRIPTION
  Deletes terrestrial data.  This includes timetag, position, and cellular data.
  Deleting cells causes timetag and position to be deleted as well.  This is
  the tle version of mc_PrMsgDeleteCellDBInfo which handles the delete all
  command.
  
PARAMETERS
   q_DeleteMask - The celldb delete mask from the delete all command.  This mask
   contains the components to delete.
   
DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   The requested data is deleted from the database as well as PTM.

===========================================================================*/
void tle_DeleteData(uint32 q_DeleteMask);

/*===========================================================================
FUNCTION
  tle_DeletePosition

DESCRIPTION
  This function processes the Position Estimate delete message received from PE.
  It should be called whenever PE gets a delete command.
  All position estimates are deleted (including EFS copies).
  
PARAMETERS
   None
   
DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
boolean tle_DeletePosition(void);

/*==============================================================================
  FUNCTION      tle_TriggerTerPosUpdate

  DESCRIPTION 	
 	This function is called to initiate a terrestrial position update request.
 	The computed position will be shipped to all the subscribers asynchronously.

  DEPENDENCIES

  PARAMETERS

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void tle_TriggerTerPosUpdate(void);

#if defined(FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH) || defined(FEATURE_WCDMA_GPS_CELL_ID_READING)
/*==============================================================================
FUNCTION
  tle_rrc_cell_info_dch_report

DESCRIPTION
  This function is registered as a callback function with RRC module and
  called whenever RRC responds to cell id request sent by TLE. If the cell
  ID's are available the function sends the cell ID info to TLM task, to
  update the current serving cells.
  
PARAMETERS 
  z_CellInfo :  Cell ID information
  e_Status   :(RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)  
 
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS 
  Note : This function is called in the context of RRC, so no local function
         call should be added in this function.
==============================================================================*/
void tle_rrc_cell_info_dch_report(rrcmeas_plmn_cell_info_struct_type z_WcdmaCell,
                                  rrc_cell_id_status_type e_Status );
#endif /* FEATURE_WCDMA_GPS_CELL_ID_READING || FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH */


/*===========================================================================

FUNCTION tle_SendE911Events

DESCRIPTION
  This function routes E911 events sent by CM and received by SM to XTM task.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tle_SendE911Events(tle_e911_event_e_type e911_event);

/*===========================================================================

FUNCTION tle_SendAGPSEvents

DESCRIPTION
  This function routes AGPS pdcomm event (connected, disconnected) to XTM.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tle_SendAGPSEvents(tle_agps_pdcomm_event_e_type agps_event);

/*===========================================================================

FUNCTION tle_SendXTRATApiInfo

DESCRIPTION
  This function processes XTRA-T APIs.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tle_SendXTRATApiInfo(tle_api_info_s_type *p_xtrat_api_info);

/*===========================================================================
FUNCTION       tle_RequestSession

DESCRIPTION    This function is called to send IPC message to 
               request XTM task to perform the following actions:
                - start/stop XTRA-T data transfer session

INPUT PARAMS

OUTPUT PARAMS

RETURN VALUE   FALSE if failing to create/send IPC message to XTM task
===========================================================================*/
boolean tle_RequestSession(const tle_request_session_s_type *p_req);

/*===========================================================================
FUNCTION
  tle_InjectPosition

DESCRIPTION
  This function is used by PE to inject a computed or externally injected
  current position estimate to TLM. 
  
PARAMETERS
   p_PosEst - Pointer to the new Position Estimate
   
DEPENDENCIES
   None

RETURN VALUE
   TRUE - if the position is successfuly accepeted by TLM

SIDE EFFECTS
   None

===========================================================================*/
boolean tle_InjectPosition( const tle_InputPosStructType * const p_PosEst);

/*=============================================================================

  FUNCTION      tle_StartKeepWarmPeriod

  DESCRIPTION
    This function is called to start or restart the keep warm timer
                
  DEPENDENCIES  None

  PARAMETERS  None

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
void tle_StartKeepWarmPeriod(void);


/*=============================================================================

  FUNCTION      tle_StartPuncBasedSession

  DESCRIPTION 	This function is called to initiate a punc based xtra-t session.
                If f_PosUnc >  TLE_POS_UNC_MAX a new session will be initiated.
                
  DEPENDENCIES  None

  PARAMETERS
    f_PosUnc - Current Position uncertainty

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_StartPuncBasedSession(FLT f_PosUnc);

/*=============================================================================

  FUNCTION      tle_isEnabled

  DESCRIPTION 	This function returns if TLE is enabled or not
                
  DEPENDENCIES  None

  PARAMETERS
    void

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_isEnabled(void);

/*=============================================================================

  FUNCTION      tle_getAccuracyCapbility

  DESCRIPTION 	This function returns TLE accuracy capability
                
  DEPENDENCIES  None

  PARAMETERS
    void

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
uint32 tle_getAccuracyCapbility(void);

/*=============================================================================

  FUNCTION      tle_WWanMeasBlkPut

  DESCRIPTION 	This function injects a Wwan measurement block into TLE
                
  DEPENDENCIES  None

  PARAMETERS
    cpz_WwanMeasBlk - Points to the measurement block to be injected

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_WWanMeasBlkPut(const loc_wwan_me_WwanMeasSetType * cpz_WwanMeasBlk);

/*=============================================================================

  FUNCTION      tle_SetEnhancedCellIdCfg

  DESCRIPTION
    This function Enables/Disables enhanced cell Id positioning in TLE
                
  DEPENDENCIES  None

  PARAMETERS
    b_Enable - TRUE : Enables enahnced cell ID positioning in TLE
    (input)    FALSE: Disables enahnced cell ID positioning in TLE

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_SetEnhancedCellIdCfg(boolean b_Enable);

/*=============================================================================

  FUNCTION      tle_DSPrefUpdate

  DESCRIPTION
    This function is called when Dual Standby preference is updated
    or the preferred data subscription is changed.
                
  DEPENDENCIES  None

  PARAMETERS
    p_DSPref - Pointer to the DS Preference data.

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_DSPrefUpdate(const cgps_DSPrefType * const p_DSPref);

/*=============================================================================

  FUNCTION      xtm_MMGSDIReady

  DESCRIPTION
    This function is called when TM has completed MMGSDI registration/initiation.
                
  DEPENDENCIES  None

   PARAMETERS

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_MMGSDIReady(void);

/*=============================================================================

  FUNCTION      tle_UpdateIMSI

  DESCRIPTION
    This function is registered as a callback to deliver the requested IMSI for the DDS
                
  DEPENDENCIES  None

  PARAMETERS
     p_IMSI - The retrieved IMSI information

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_UpdateIMSI(const cgps_IMSIType * const p_IMSI);

/*=============================================================================

  FUNCTION      tle_SetGTPConfiguration

  DESCRIPTION
    This function allows HLOS to change GTP settings in modem processror 
    (Disable/Basic/Premium) 
                
  DEPENDENCIES  None

  PARAMETERS
     cpz_GtpConfig - Points to the new GTP setting

  RETURN VALUE  
 
  SIDE EFFECTS  

=============================================================================*/
void tle_SetGTPConfiguration(const tle_api_gtp_config_s_type * cpz_GtpConfig);

/*=============================================================================

  FUNCTION      tle_RcvdSendAck

  DESCRIPTION
    This function is called by GDT as an ACK for a received record.
 
  PARAMETERS
    z_Ack  - The ack received
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     
  RETURN VALUE
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_RcvdSendAck(gdt_SendAckArgumentType z_Ack);


/*=============================================================================

  FUNCTION      tle_ProcSendEndResponse

  DESCRIPTION
    This API is called by GDT to confirm the session end.
 
   PARAMETERS
    z_EndStatus  - Indicates whether the session succeeded or not
    (input)
                
  DEPENDENCIES  None

  PARAMETERS
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_ProcSendEndResponse(gdt_EndArgumentType z_EndStatus);

/*=============================================================================

  FUNCTION      tle_InjectGtpWwanClientData

  DESCRIPTION
    This API is called by loc_mw to inject GTP Wwan ClienDownloaded data.
 
  PARAMETERS
    cpz_Data - Points to the received encoded WwanClientDownloadedData
    (input)    in raw format 
 
  DEPENDENCIES  None

  PARAMETERS
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to XTM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_InjectGtpWwanClientData(const tle_GtpClientDownloadedReqType * cpz_Data);

/*=============================================================================

  FUNCTION      tle_InjectTdpMeasBlk

  DESCRIPTION
    This API is used by WWAN ME to inject TDP measurement block to TLE
                
  DEPENDENCIES  None

  PARAMETERS
    cpz_TdpMeasBlk - Points to the measuremnet block to be injected
    (input)
     

  RETURN VALUE
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_InjectTdpMeasBlk(const tdp_MeasAndCellDataBlkStructType * cpz_TdpMeasBlk);

/*=============================================================================

  FUNCTION      tle_ProcSendBeginResponse

  DESCRIPTION
    This function is called by GDT to send a upload session request status.
                
  DEPENDENCIES  None

  PARAMETERS
    z_Response - The permission status for the requested upload session
    (input)
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_ProcSendBeginResponse(gdt_StartArgumentType z_Response);

/*=============================================================================

  FUNCTION      tle_ProcSendOpenResponse

  DESCRIPTION
    This function is called by GDT to send an open request status.
                
  DEPENDENCIES  None

  PARAMETERS
    z_Response - The status of the requested open request
    (input)
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_ProcSendOpenResponse(gdt_OpenArgumentType z_Response);

/*=============================================================================

  FUNCTION      tle_ProcSendCloseResponse

  DESCRIPTION
    This function is called by GDT to send a close request status.
                
  DEPENDENCIES  None

  PARAMETERS
    z_Response - The status for the requested close request
    (input)
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to TLM
    FALSE - Otherwise
 
  SIDE EFFECTS  

=============================================================================*/
boolean tle_ProcSendCloseResponse(gdt_CloseArgumentType z_Response);

/*===========================================================================
FUNCTION tle_InjectInsCapability

DESCRIPTION
  This function is used by SM to inject INS capability into TLE
 
PARAMETERS 
  q_InsCapability - INS capability as reported by SM
  (input)           
 
DEPENDENCIES

RETURN VALUE 
  TRUE:  The IPC is sent to XTM
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tle_InjectInsCapability(uint32 q_InsCapability);

/*===========================================================================
FUNCTION tle_InjectPcid

DESCRIPTION
  This function sends the PCID to TLE module. 
   
PARAMETERS 
  t_Pcid - PCID
  (Input)

DEPENDENCIES

RETURN VALUE 
  TRUE  - If the API info is sent to TLE
  FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tle_InjectPcid(uint64 t_Pcid);

/*===========================================================================

FUNCTION tle_ReadPcid

DESCRIPTION
  This function reads the PCID from TLE module. 
 
PARAMETERS 
  t_Pcid - PCID
  (Input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  - If the PCID is available to be read.
  FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tle_ReadPcid(uint64 * pt_Pcid);

#endif /* TLE_API_H */