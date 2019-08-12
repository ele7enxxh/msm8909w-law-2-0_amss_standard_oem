#ifndef TM_LM_IFACE_H
#define TM_LM_IFACE_H
/*=============================================================================

                        Transport Manager API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the Transport Manager software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/inc/tm_lm_iface.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)  
01/22/13   ah      Support extraction of MPG event from union tm_info_s_type
08/01/07   rw      Add support for GPS time info report
06/12/06   mr      Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"
#include "sm_api.h"
#include "aries_os_globals.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/
#define TM_MAX_NMEA_SENTENCE_LEN  MAX_NMEA_SENTENCE_LEN

#define SM_GNSS_GLO_AA_INJECTED       0x01   /* LM injected Glonass Acquistion Assistance Data into ME */
#define SM_GNSS_LS_REQ_GLO_MEAS       0x02   /* Location Server (LS) requested Glonass measurement report from UE */
#define SM_GNSS_EXPECT_GLO_MEAS       (SM_GNSS_GLO_AA_INJECTED | SM_GNSS_LS_REQ_GLO_MEAS) /* Instruct LM to expect Glonass meas from ME */

/*=============================================================================

       Typedefs

=============================================================================*/
typedef enum
{
  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_SM_TM_MSG_FIRST = C_USR_MSG_ID_FIRST,  

  C_SM_TM_MSG_SESSION_REQUEST,    /* LM-TM session related request from TM to LM */
  C_SM_TM_MSG_SESSION_UPDATE,     /* LM-TM session independent request from TM to LM */
  C_SM_TM_MSG_INFO,             /* LM-TM session independent info from TM to LM */

  C_SM_TM_MSG_END 
} tm_ipc_message_type;

typedef enum {
  TM_REQUEST_ERROR_NOTIFY,
  /* Asynchronous error notification from LM because of error, etc */
  TM_REQUEST_SA_DATA
  /*Request to get SA data from network*/    
}tm_session_request_e_type;

typedef enum {
  TM_ERROR_CAUSE_GENERAL_ERROR,
  TM_ERROR_CAUSE_PREVIOUS_SESSION_ACTIVE,
  TM_ERROR_CAUSE_PRM_TIMEOUT,
  TM_ERROR_CAUSE_FIX_TIMEOUT,
  TM_ERROR_CAUSE_PPM_TIMEOUT,
  TM_ERROR_MGP_STATUS_TIMEOUT
} tm_session_error_cause_e_type;

typedef union {
  /* TM_REQUEST_ERROR_NOTIFY */
  tm_session_error_cause_e_type    e_errorCause;
}tm_session_request_u_type;

typedef struct {
  uint32                             q_handle;
  tm_session_request_e_type          e_request_type;
  tm_session_request_u_type          z_request;
} tm_session_request_s_type;


typedef enum {
  TM_INFO_STATUS_REPORT,
    /* Status */
  TM_INFO_PPM_REPORT,
    /* Actual PPM results in MGP format */
  TM_INFO_PRM_REPORT,
    /* PRM data in MGP format */
  TM_INFO_FINAL_FIX_REPORT,
    /* QoS qualified or final Position fix in MGP format */
  TM_INFO_INTERMEDIATE_FIX_REPORT,
    /* Non QoS qualified fix in MGP format */
  TM_INFO_LATE_PRM_REPORT,
    /* Late PRM data in MGP format */

  TM_INFO_SV_POLY_REPORT,
    /* Satellite polynomial report */

} tm_session_update_info_e_type;

typedef struct
{
  sm_GpsMeasRptStructType       z_GpsMeasRpt;
} tm_measurement_report_s_type;


typedef struct
{
  sm_SvPolyReportType       z_SvPolyRpt;
} tm_sv_poly_report_s_type;

typedef struct
{
  srch_ppm_rpt_type             z_PpmRpt;
} tm_ppm_report_s_type;

typedef struct
{
  sm_GnssFixRptStructType       z_GpsFixRpt;
  uint32                        q_FixTimeRemainingMSec;
} tm_fix_report_s_type;

typedef enum
{
  TM_STATUS_ACK_OK,
  TM_STATUS_NACK_CAUSE_UNKNOWN_ERROR,
  TM_STATUS_NACK_CAUSE_INVALID_PARAM,
  TM_STATUS_NACK_PREVIOUS_SESSION_ACTIVE,
  TM_STATUS_NACK_NO_SESSION_ACTIVE
} tm_status_cause_e_type;

typedef enum
{
  TM_STATUS_ACK,
  TM_STATUS_NACK
} tm_status_report_e_type;

typedef struct
{
  tm_status_report_e_type e_status;
  tm_status_cause_e_type  e_cause;
} tm_status_report_s_type;

typedef union {
  tm_measurement_report_s_type  z_measure_rpt; 
  tm_fix_report_s_type          z_fix_rpt;
  tm_ppm_report_s_type          z_ppm_rpt;
  tm_status_report_s_type       z_status_rpt;
  tm_sv_poly_report_s_type		z_sv_poly_rpt;
} tm_session_update_info_u_type;

typedef struct {
  uint32                           q_handle;
  tm_session_update_info_e_type    e_update_type;
  tm_session_update_info_u_type    z_update;
} tm_session_update_info_s_type;

typedef enum
{
  TM_INFO_AIDING_DATA_WISHLIST,
  /* Status */
  TM_INFO_PD_API_EVENT,
  /* Event to be delivered to PD-API */
  TM_NMEA_DATA,
  /* NMEA update */
  TM_INFO_POS_ESTIMATE,
  /* position estimate from PE */
  TM_RF_INFO,
  /* RF Info from MGP */
  TM_GPS_TIME,
  /* GPS Time info from MGP */
  TM_INFO_MGP_EVENT,
  /* MGP EVENT info */
  TM_INFO_OEMDRE_TD_CTRL,
  /* OEM DRE Tunnel Detection on/off control */
} tm_info_e_type;

typedef struct
{
  sm_InternalAssistDataStatusStructType z_assist_data_status;
  boolean for_keepwarm;
} tm_aiding_data_wishlist_s_type;

typedef enum
{
  TM_PD_EVENT_WARM,
  TM_PD_EVENT_NOT_WARM,
  TM_PD_EVENT_GPS_BEGIN,
  TM_PD_EVENT_GPS_DONE,
  /*This is not a PDAPI event currently.
    Only needed by TM to maintian proper MGP state*/  
  TM_PD_EVENT_GPS_IDLE 
} tm_pd_api_event_e_type;

typedef enum
{
  TM_NMEA_GGA,
  TM_NMEA_RMC,
  TM_NMEA_GSV,
  TM_NMEA_GSA,
  TM_NMEA_VTG,
  TM_NMEA_DEBUG
} tm_nmea_data_sentence_type_e_type;

typedef struct
{
  tm_nmea_data_sentence_type_e_type e_sentence_type;
  uint32 q_sentence_len;
  char p_nmea_sentence[TM_MAX_NMEA_SENTENCE_LEN];
} tm_nmea_data_s_type;

typedef struct
{
  sm_ReportRfInfoStructType z_rf_info_struct; 
} tm_rf_info_s_type;

typedef struct
{
  sm_ReportGpsTimeStructType z_gps_time_info_struct; 
} tm_gps_time_info_s_type;

typedef enum
{
  TM_OEMDRE_TD_CTRL_OFF,
  TM_OEMDRE_TD_CTRL_ON
} tm_oemdre_td_ctrl_e_type;

typedef union
{
  /* TM_INFO_AIDING_DATA_WISHLIST */
  tm_aiding_data_wishlist_s_type z_wishlist;
  /* TM_INFO_PD_API_EVENT */
  tm_pd_api_event_e_type         e_pdapi_event;
  /* TM_NMEA_DATA */
  tm_nmea_data_s_type            z_nmea_data;
  /* TM_INFO_POS_ESTIMATE */
  tm_fix_report_s_type           z_pos_estimate;
  /* TM_RF_INFO */
  tm_rf_info_s_type              z_rf_info;
  /* TM_GPS_INFO */
  tm_gps_time_info_s_type        z_gps_time_info;
  /* TM_INFO_MGP_EVENT: see tm_info_mgp_event_s_type */
  /* TM_INFO_OEMDRE_TD_CTRL */
  tm_oemdre_td_ctrl_e_type       e_oemdre_td_ctrl;
} tm_info_u_type;

typedef struct {
  tm_info_e_type          e_info_type;
  tm_info_u_type          z_info;
} tm_info_s_type;

/* Struct for msgid = TM_INFO_MGP_EVENT */
typedef struct
{
  tm_info_e_type           e_info_type;
  sm_ReportEventStructType z_mgp_event;
} tm_info_mgp_event_s_type;

/*=============================================================================

       Functions

=============================================================================*/

/*=============================================================================

FUNCTION 
  tm_session_request

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_req - Pointer to the structure containing session related request from 
  LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern boolean tm_session_request
(
  const tm_session_request_s_type *p_req
);

/*=============================================================================

FUNCTION 
  tm_session_update

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_update - Pointer to the structure containing the session related update 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern boolean tm_session_update
(
  const tm_session_update_info_s_type   *p_update
);

/*=============================================================================

FUNCTION 
  tm_info

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_info - Pointer to the structure containing the session independent info 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern boolean tm_info
(
  const tm_info_s_type   *p_info 
);

/*=============================================================================

FUNCTION 
  tm_info_mgp_event

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  p_info - Pointer to the structure containing the session independent info 
  from LM to TM

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern boolean tm_info_mgp_event
(
  const tm_info_mgp_event_s_type   *p_info 
);

/*===========================================================================

FUNCTION tm_core_is_periodic_ppm_session_on

DESCRIPTION
  This function determines if periodic PPM session is ongoing

RETURN VALUE
  TRUE if periodic PPM is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_periodic_ppm_session_on(void);

/*===========================================================================

FUNCTION tm_core_get_periodic_ppm_session_handle

DESCRIPTION
  This function provides the special P.PPM session handle.

RETURN VALUE
  TM_SESS_HANDLE_NONE         : If P.PPM is not ON.
  TM_SESS_HANDLE_PERIODIC_PPM : Periodic PPM Session handle if P.PPM is on

  
  if P.PPM is OFF.
DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
uint32 tm_core_get_periodic_ppm_session_handle(void);


#endif /* TM_LM_IFACE_H */
