#ifndef SM_DM_EVENT_H
#define SM_DM_EVENT_H
/*===========================================================================

              PDSMLOG Header File.

DESCRIPTION
   This file contains log types used internally by the PDSM task.

  
  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/inc/sm_dm_event.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/10   atien   Remove older XTRA-T event implementation.  The new events
                   should be part of TLE.
03/11/09   ns      QWiP Sysd Transition Event
01/14/09   atien   XTRA-T logs and events
06/12/08    lt     Support for external coarse position injection.
06/09/08   gk      On-Demand Changes to add some logs and events
4/2/07     gk      Modified BEGIN Event APi.
12/27/06   ank     Added support for IS801 Rx and Tx events.
12/04/06   gk      Updated headers
11/8/06    gk      Initial checkin for converged GPS

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */

#include "pdapi.h"      /* External interface to pd.c */

/*===========================================================================

                     DEBUGGING RELATED DEFINITIONS 
                      (MUST DELETE THEM)

*===========================================================================*/



/*===========================================================================

                     DEFINES FOR MODULE

===========================================================================*/



/*==========================================================================
                     
                     MACRO DEFINITIONS
                     
===========================================================================*/

                     
                     
/*===========================================================================

                     DATA TYPE DEFINITIONS FOR MODULE

                        !!!!!!! ATTENTION !!!!!!!!
Size/Order of data fields in the following data types should not
be changed as they are synchronized with external tool software.
===========================================================================*/

#include "tm_pdapi_iface.h"

typedef enum
{
  MOBILE_INIT_SESSION,
  NETWORK_INIT_SESSION
}sm_event_pd_session_start_source_e_type;

typedef enum
{ 
  PD_GET_POS,
  PD_END_SESS
} sm_event_pd_session_type;

typedef enum
{
  MS_ASSISTED_OP,
  MS_BASED_OP
} sm_event_pd_fix_start_payload_e_type;

typedef enum
{
  IS801_REQ,
  IS801_RESP
} sm_event_is801_msg_e_type;

typedef enum
{
  DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_DONE,
  DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_FAILED,
  DM_EVT_EXTERN_COARSE_POS_INJ_END_REASON_ERROR
} sm_event_extern_coarse_pos_inj_end_reason;

typedef PACKED struct PACKED_POST
{
  uint8  u_StartSrc_and_OpMode;            /* Session start source and Operation mode */
  uint8  u_SessType_and_Privacy;            /* Session type and Privacy level */
  uint8  u_DatadlType_and_TransType;      /* Data Download type and transport type */
  uint16 w_NumFixes;                        /* Number of fixes requested */
  uint16 w_FixPeriod;                       
  uint8  u_Prq;
  uint16 w_Threshold;
}sm_event_pd_session_start_payload_s_type;


typedef PACKED struct PACKED_POST
{  
  uint8 u_CmdType;
  uint8 u_CmdErr;
} sm_event_pd_cmd_err_callback_s_type;     
                     
                     
typedef PACKED struct PACKED_POST
{  
  uint8 u_CmdType;
  uint8 u_CmdErr;
} sm_event_pa_cmd_err_callback_s_type;     

typedef PACKED struct PACKED_POST
{
  uint8  data_type;
  uint32 sv_mask;
}sm_event_gps_data_download_payload_s_type;

typedef PACKED struct PACKED_POST
{
  uint16 w_LogCnt; 
  uint8  u_OperationMode;
} sm_event_gps_pd_fix_start_payload_s_type;


typedef PACKED struct PACKED_POST
{
  uint16 ref_bit_num;
  uint8  dr_size;
} sm_event_gps_sa_rcv_payload_s_type;

#define EVENT_PDSM_NOTIFY_VERIFY_REQ 0x07

typedef PACKED struct PACKED_POST
{
  uint32 client_id;
  byte  u_EventName;
  byte  u_EventData;
} sm_report_event_type;


/* gpsOneXTRA Download Session Triggers
*/
typedef enum {
    XTRA_DOWNLOAD_TRIGGER_NONE             = -1,      /* NONE                              */
    XTRA_DOWNLOAD_TRIGGER_AUTO             = 0,       /* Automatic Download Timer expired  */
    XTRA_DOWNLOAD_TRIGGER_RETRY,                      /* Retry Download Timer expired      */
    XTRA_DOWNLOAD_TRIGGER_XTRA_CLIENT_REQ,            /* gpsOneXTRA Client dload request   */
    XTRA_DOWNLOAD_TRIGGER_FIX_REQUEST,                /* Standalone Fix Request            */
    XTRA_DOWNLOAD_TRIGGER_TIME_INJECTION,             /* gpsOneXTRA Time Injection         */
    XTRA_DOWNLOAD_TRIGGER_MAX,                        /* For internal use                  */
    XTRA_DOWNLOAD_TRIGGER_E_SIZE = 0x10000000         /* Fix enum size as int              */
} sm_event_xtra_download_trigger_e_type;

/* gpsOneXTRA DM event (EVENT_GPSONEXTRA_START_DOWNLOAD) payload */
typedef PACKED struct PACKED_POST
{
  sm_event_xtra_download_trigger_e_type  trigger_type;
} sm_event_xtra_start_download_s_type;

/* gpsOneXTRA DM event (EVENT_GPSONEXTRA_END_DOWNLOAD) payload */
typedef PACKED struct PACKED_POST
{
  pdsm_xtra_status_e_type  end_reason;  
} sm_event_xtra_end_download_s_type;

/* ON DEMAND events */
typedef PACKED struct PACKED_POST
{
   uint8    mode_prev;
   uint8    mode_current;
   uint8    mode_change_reason;
} sm_event_on_demand_mode_change_payload_s_type;


/* ON DEMAND events */
typedef PACKED struct PACKED_POST
{
  uint8    mode;
  uint8    gps_session_timeout;
  uint32    accuracy;
} sm_event_on_demand_begin_payload_s_type;


/* QWiP Sysd Transition DM event  
  - EVENT_CGPS_QWIP_SYSD_TRANSITION - 
  One of sysd State machines receives an event */
typedef PACKED struct PACKED_POST
{
  uint8  sysd_sm_type;  /* State machine type. Matches 
                           tm_sysd_loc_engine_cfg_e_type */
  uint8 event_type;    /* Event received by this state machine */
  uint8 current_state; /* Current state of this machine */
  uint8 next_state;   /* Next state for this state machine for this event */
}sm_event_qwip_sysd_transition_s_type;


/*  Fast TCal Event - EVENT_GNSS_FAST_TCAL_END */
typedef PACKED struct PACKED_POST
{
  int16 TCal;              /* TCAL value in nano seconds */
  uint8 TCalUncertainty;   /* TCAL uncertainity in nano seconds */
  uint8 ErrorCode;         /* Error code (ErrorCode == 0 – no error) */
}sm_event_gnss_fast_tcal_end_s_type;


/*===========================================================================

                     EXTERNALISED DATA/FUNCTION
                     
===========================================================================*/

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_SESSION_START

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_session_start
(
  void *pd_info_ptr, 
  uint8 start_source
);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_SESSION_START

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_session_done (void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_START

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_comm_start (uint8 u_ProtocolType);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_CONNECTED

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_comm_connected (void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_FAILURE

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_comm_failure (uint8 u_Reason);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_DONE

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_comm_done (void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_POSITION

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_position (void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_EVENT_END

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_event_end (uint8 u_Reason);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PA_EVENT_CALLBACK

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pa_event_callback (pdsm_pa_e_type e_PaEvent);

/*
 ******************************************************************************
 * FUNCTION: SM_REPORT_EVENT_PA_CMD_ERR 
 *
 * Function description:
 *
 * This function generates a DM event when a PA Cmd error is generated
 *
 * Parameters: 
 * pointer to pdsm_dm_event_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************/
extern void sm_report_event_pa_cmd_err(uint8 u_CmdType, pdsm_pa_cmd_err_e_type e_CmdErr);

/*
 ******************************************************************************
 * FUNCTION: SM_REPORT_EVENT_PD_CMD_ERR 
 *
 * Function description:
 *
 * This function generates a DM event when a PD Cmd error is generated
 *
 * Parameters: 
 * pointer to pdsm_dm_event_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************/
extern void sm_report_event_pd_cmd_err(uint8 u_CmdType, pdsm_pd_cmd_err_e_type e_CmdErr);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_EVENT_DOWNLOAD_START

DESCRIPTION
  Report events GPS PD Download end.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_download_start (uint8  data_type,uint32 sv_mask);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_EVENT_DOWNLOAD_END

DESCRIPTION
  Report events GPS PD Download end.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_pd_download_end(sint31 status);

/*===========================================================================

FUNCTION sm_report_event_gps_is801_rx

DESCRIPTION
  Report events IS801 RX event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_is801_rx
(
  sm_event_is801_msg_e_type  msg_type,
  uint32                     mask
);

/*===========================================================================

FUNCTION sm_report_event_gps_is801_tx

DESCRIPTION
  Report events IS801 TX event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_is801_tx
(
  sm_event_is801_msg_e_type  msg_type,
  uint32                     mask
);

/*===========================================================================

FUNCTION sm_report_event_gps_fix_start

DESCRIPTION
  Report EVENT_GPS_PD_FIX_START 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_fix_start
(
  uint16                                log_cnt,
  sm_event_pd_fix_start_payload_e_type  op_mode
);

/*===========================================================================

FUNCTION sm_report_event_gps_fix_end

DESCRIPTION
  Report EVENT_GPS_PD_FIX_END

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_fix_end
(
  sint31 status
);

/*===========================================================================

FUNCTION sm_report_event_gps_sa_received

DESCRIPTION
  Report EVENT_GPS_SA_RECEIVED

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_gps_sa_received
(
  uint16 ref_bit_num,
  uint8  dr_size
);


/*===========================================================================

FUNCTION sm_report_event_ui_req

DESCRIPTION
  This function generates a DM message when a UI request is genreated by the
  PDAPI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_ui_req
(
  uint32 client_id,
   byte priv_type
);


/*===========================================================================

FUNCTION sm_report_event_xtra_start_dload

DESCRIPTION
  This function generates a DM message when the gpsOneXTRA sends a start
  download request to the clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_xtra_start_dload
(
  sm_event_xtra_download_trigger_e_type trigger
);


/*===========================================================================

FUNCTION sm_report_event_xtra_end_dload

DESCRIPTION
  This function generates a DM message when the gpsOneXTRA sends a end
  download request to the clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void sm_report_event_xtra_end_dload
(
  pdsm_xtra_status_e_type reason 
);

/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_change(uint8 mode_prev, uint8 mode_next, uint32 reason_for_mode_change);

/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_begin(uint8 mode);


/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_done(uint8 reason);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_COARSE_POS_INJ_START

DESCRIPTION
  Report DM event EVENT_COARSE_POS_INJ_START.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_coase_pos_inj_start (void);


/*===========================================================================

FUNCTION SM_REPORT_EVENT_COARSE_POS_INJ_END

DESCRIPTION
  Report DM event EVENT_COARSE_POS_INJ_END.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_coase_pos_inj_end (sm_event_extern_coarse_pos_inj_end_reason 
                                        u_reason);


/*===========================================================================

FUNCTION SM_REPORT_EVENT_QWIP_SYSD_TRANSITION

DESCRIPTION
  Report event: QWiP SYSD State transitions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_qwip_sysd_transition (uint8 sysd_sm_type,uint8 event,
                                           uint8 current_state, uint8 next_state);


/*===========================================================================

FUNCTION SM_REPORT_EVENT_PREEMPTION_FLAG

DESCRIPTION
  Report event: Reports an event when GSM preemption happens in E911

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_preemption_flag (void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GNSS_FAST_TCAL_END 

DESCRIPTION
  Report event: Fast TCal Completed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gnss_fast_tcal_end 
(
  int16 TCal, 
  uint8 TCalUncertainty,
  uint8 ErrorCode
);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_RESET_LOCATION_SERVICE_RECEIVED 

DESCRIPTION
  Report event:Reset Location Service Request Received

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_reset_location_service_received(void);

/*===========================================================================

FUNCTION SM_REPORT_EVENT_RESET_LOCATION_SERVICE_DONE 

DESCRIPTION
  Report event:Reset Location Service Request Done

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_reset_location_service_done(uint32 status);


#endif /* SM_DM_EVENT_H */

