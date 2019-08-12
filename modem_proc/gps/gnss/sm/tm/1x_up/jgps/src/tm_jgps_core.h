#ifndef TM_JGPS_CORE_H
#define TM_JGPS_CORE_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM JGPS Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for Japan 
  GPS specification for KDDI.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/src/tm_jgps_core.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*=============================================================================

                                Include Files

=============================================================================*/
#ifdef FEATURE_JCDMA_GPSONE
#include "tm_common.h"
#include "pdapi.h"
#include "tm_prtl_iface.h"

#include "tm_1x_up.h"
#include "tm_jgps_msg.h"
#include "pd_comms_api.h"


/*=============================================================================

                                Data Structures                              

=============================================================================*/

/* Data pipe request flags */
#define TM_JGPS_DATA_PIPE_REQ_FAIL    0
#define TM_JGPS_DATA_PIPE_REQ_GRANT   1

  
/* JGPS module should use timer ID range from 0x02020000 - 0x0202FFFF*/
typedef enum
{
  TM_JGPS_TIMER_ID_MIN                   = TM_1X_UP_KDDI_TIMER_ID_BASE,
  TM_JGPS_TIMER_ID_T02,
  TM_JGPS_TIMER_ID_T04, 
  TM_JGPS_TIMER_ID_RF_INFO_REQ,
  TM_JGPS_TIMER_ID_MS_IP_REQ, 
  TM_JGPS_TIMER_ID_MAX                   = TM_1X_UP_KDDI_TIMER_ID_BASE + TM_1X_UP_KDDI_TIMER_ID_RANGE
} tm_jgps_timer_id_e_type;

/* state of PM while in session.
*/
typedef enum {
  TM_JGPS_SESS_STATE_INIT,                
    /* Init state */
  TM_JGPS_SESS_STATE_START_REQ,           
    /* Request from TM-Core to start session */
  TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP,
    /* Waiting for resp from SCU due to POS req or ACQ req */
  TM_JGPS_SESS_STATE_START_REQ_GRANT,
    /* Grant request from IS-801 Driver */
  TM_JGPS_SESS_STATE_ABORT_RECOVER,          
    /* Abort and Recover */
  TM_JGPS_SESS_STATE_STOP_REQ,
    /* Request from IS-801 Driver to stop session */
  TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP,
    /* Request from IS-801 Driver to stop session */
  TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT
    /* Wait for TCP disconnection complete */
} tm_jgps_sess_state_e_type;

/* Structure for JPGS mode processing.
*/
typedef enum {
  TM_JGPS_SESS_REQ_MODE_NONE,       /* Not in any substate */
  TM_JGPS_SESS_REQ_MODE_MSA,        /* MSAssisted processing */
  TM_JGPS_SESS_REQ_MODE_MSB_DLOAD,   /* MSBased processing */
  TM_JGPS_SESS_REQ_MODE_MAX
} tm_jgps_sess_req_mode_e_type;

/* Structure for SCU communication TCP state */
typedef enum {
  TM_JGPS_COMM_STATE_INIT = 0,
  TM_JGPS_COMM_STATE_CONNECTING,
  TM_JGPS_COMM_STATE_CONNECTED,
  TM_JGPS_COMM_STATE_DISCONNECTING,
  TM_JGPS_COMM_STATE_DISCONNECTED
} tm_jpgs_comm_state_e_type;

/* Structure for recieve state, header or payload */
typedef enum 
{
  TM_JGPS_COMM_RECV_STATE_HEADER = 0,
  TM_JGPS_COMM_RECV_STATE_PAYLOAD,
} tm_jgps_comm_recv_state;



/* Structure for request (RF info or MS IP addr) info state
*/
typedef enum{
  TM_JGPS_REQ_INFO_STATE_INIT = -1,
  TM_JGPS_REQ_INFO_STATE_WAIT,
  TM_JGPS_REQ_INFO_STATE_GRANT
}tm_jgps_req_info_state_e_type;


/* Data structure for app info added to URL data 
*/
typedef struct{
  boolean  app_url_enable;        /* Data is active or not */
  char     url[TM_JGPS_MAX_URL];  /* the url string */
  byte     ver;
  byte     datum;    
  byte     unit;
  byte     acry;
  byte     num;
} tm_jgps_url_info_s_type;

/* Enum for restarting the session
*/
typedef enum{
 TM_JGPS_RESTART_INIT,
 TM_JGPS_RESTART_TM_CORE  
}tm_jgps_restart_state_e_type;


/* Data structre for dynamic JGPS session information 
*/
typedef struct
{
  /*---------------------------------------------------------------------------
     JGPS Module information
  ---------------------------------------------------------------------------*/
  /* Session state */
  tm_jgps_sess_state_e_type         state;    

  /* Start Session request parameters */
  prtl_start_sess_req_param_s_type  smod_req_param;
  
  /* Session request state */
  tm_jgps_sess_req_mode_e_type      req_mode;
    
  /* Current TM-Core session handle */
  tm_sess_handle_type               tm_core_sess_handle;

  /* URL information sent during POS req or ACQ req */
  tm_jgps_url_info_s_type           url_info;

  /* Restart the session */
  tm_jgps_restart_state_e_type      restart;
    
  /*---------------------------------------------------------------------------
     JGPS packet data
  ---------------------------------------------------------------------------*/
  /* Packet Header Data (MS->SCU)*/
  tm_jgps_i_hdr_s_type            pkt_hdr;
    
  /* Request to SCU (POS or ACQ) (MS->SCU) */
  tm_jgps_i_ms_req_s_type         req_param;

  /* Response packet from SCU (SCU->MS)*/
  tm_jgps_packet_s_type           resp_pkt;
  
  /*---------------------------------------------------------------------------
     LR information
  ---------------------------------------------------------------------------*/
  /* Used to save PD info from 1x801 driver module for position report */
  pdsm_pd_info_s_type          lr_resp;
  qword                        lr_time;
  pm_scu_location_resp_type    scu_lr_resp;

  /*---------------------------------------------------------------------------
     Request information
  ---------------------------------------------------------------------------*/  
  tm_sess_req_param_u_type       tm_sess_req_param;
  tm_jgps_req_info_state_e_type  rf_info_req_state;

  tm_jgps_req_info_state_e_type  ms_ip_req_state;
  
  /*---------------------------------------------------------------------------
     Timers
  ---------------------------------------------------------------------------*/
  /* From sending of SPPReq to receipt of SPPResp (5 seconds) */
  os_TimerMsgType              *t02_timer;

  /* From sending of Pos Report to receipt of Pos Report Ack (120 seconds) */
  os_TimerMsgType              *t04_timer;

  /* Request RF info timer with TM-Core */
  os_TimerMsgType              *rf_info_req_timer;
  
  /* Request MS IP addr timer with PD-Comm */
  os_TimerMsgType              *ms_ip_req_timer;  

} tm_jgps_sess_info_s_type;

/* Data structure for client application information */
typedef struct
{  
  /* Application Type: BREW, JAVA, BROWSER, ... etc */
  pdsm_client_type_e_type              app_type;

  /* TM-Core Start Request Action */
  prtl_start_sess_req_param_s_type    tm_core_start_req;
      
} tm_jgps_app_info_s_type;

/* Data structure for PDCOMM interface */
typedef struct {
  boolean                      close_flag;
  pd_comms_link_type           link_type;
  tm_jpgs_comm_state_e_type    comm_state;
  tm_jgps_comm_recv_state      recv_state;  
  uint16                       incoming_data_length;  
} tm_jgps_comm_info_s_type;

/* 
  Data structure for general JGPS protocol module inforamtion
  1. sess_info contains dynamic session information, session state, active client, ...etc
  2. app_info contains the application request information from TM-Core
*/
typedef struct 
{

  /* sess_info contains session related information (Dynamic) */
  tm_jgps_sess_info_s_type       sess_info;

  /* Structure for applcition related information */
  tm_jgps_app_info_s_type        app_info;

  /* Communication link information */
  tm_jgps_comm_info_s_type       comm_info;
   
} tm_jgps_gen_info_s_type;

/*=============================================================================

                                Extern Data Structures

=============================================================================*/
extern tm_jgps_gen_info_s_type   tm_jgps_gen_info;


/*=============================================================================

                                Macros                             

=============================================================================*/

/* General Information */
#define TM_JGPS_GEN_INFO_PTR() &tm_jgps_gen_info

/* Session Information */
#define TM_JGPS_GEN_INFO_SESS_PTR() (&((TM_JGPS_GEN_INFO_PTR())->sess_info))

/* Application Information */
#define TM_JGPS_GEN_INFO_APP_PTR() (&((TM_JGPS_GEN_INFO_PTR())->app_info))


/*===========================================================================

FUNCTION tm_jgps_core_translate_end_status

DESCRIPTION
  This function translate external end status to internal abort reason.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
tm_sess_stop_reason_e_type tm_jgps_core_translate_end_status(pdsm_pd_end_e_type end_status);

/*=============================================================================

                                Extern Functions

=============================================================================*/
/*=============================================================================
===============================================================================

         Extern Functions: 1x UP (IS-801 Drv) => TM-JGPS-Core

===============================================================================
=============================================================================*/


/*===========================================================================

FUNCTION   tm_jgps_core_start_sess_req_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  TRUE - JGPS core grants the session start request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_start_sess_req_handler( void );


/*===========================================================================

FUNCTION   tm_jgps_core_stop_sess_req_handler

DESCRIPTION
  This function handles the request from IS-801 driver to stop the session

RETURN VALUE
  TRUE - JGPS core grants the session stop request
  FALSE - otherwise
============================================================================*/
extern boolean tm_jgps_core_stop_sess_req_handler
(
  pdsm_pd_end_e_type            status,
  pdsm_pd_dload_data_s_type     *dload_ptr
);


/*===========================================================================

FUNCTION   tm_jgps_core_info_available_handler

DESCRIPTION
  This function is used to format the PD data to clients or save the data
  until SCU returns a completion reponse packet at the end of MS-A.

RETURN VALUE
  TRUE - JGPS core services the info available request
  FALSE - otherwise
============================================================================*/
extern boolean tm_jgps_core_info_available_handler
(
  pdsm_pd_info_s_type   *pd_info_ptr
);


/*===========================================================================

FUNCTION: tm_jgps_core_data_pipe_req_handler

DESCRIPTION:
  This function is called by TM-JGPS module to append KDDI specfic header
  to outgoing IS-801 data.

   
PARAMETERS:

RETURN VALUES:
  0 - Failure
  NUM - number of bytes sent
===========================================================================*/
extern int tm_jgps_core_data_pipe_req_handler
(
  boolean   close_flag,
  byte      *data_ptr,
  int16     data_ptr_len
);


/*=============================================================================
===============================================================================

              Extern Functions: TM-Core => TM-JGPS-Core
              
===============================================================================
=============================================================================*/

/*===========================================================================
FUNCTION: tm_jgps_init

DESCRIPTION:
  This function is called at boot up time to initialize TM JGPS module.

 
PARAMETERS:

===========================================================================*/
extern void tm_jgps_core_init( void );


/*===========================================================================

FUNCTION   tm_jgps_core_reinit

DESCRIPTION
  This function is used to abort the current sesssion and notify the 
  proper sub-modules (i.e. IS-801 driver), TM-Core, and client

RETURN VALUE
  None

============================================================================*/
extern void tm_jgps_core_reinit( void );


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_start_sess_req_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
extern prtl_start_sess_req_result_e_type 
tm_jgps_core_prtl_start_sess_req_handler
(
  tm_sess_handle_type                  sess_handle,
  prtl_start_sess_req_param_s_type     *start_sess_req_param,
  tm_sess_req_start_param_s_type       *actions
);


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_stop_sess_req_handler

DESCRIPTION
  This function is used to handle user terminated stop session from
  TM-Core.

RETURN VALUE
  None

============================================================================*/
extern boolean tm_jgps_core_prtl_stop_sess_req_handler
(
  tm_sess_stop_reason_e_type    reason,
  prtl_sess_stop_type           stop_type
);


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_save_app_info

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
extern boolean tm_jgps_core_prtl_save_app_info
(  
  pdsm_pa_info_type  *pa_info_ptr
);


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_sess_info_handler

DESCRIPTION
  This function handles the session information data returned by TM-Core

RETURN VALUE
  None

============================================================================*/
extern boolean tm_jgps_core_prtl_sess_info_handler
(
  prtl_sess_info_e_type         sess_info_type,
  prtl_sess_info_param_u_type   *sess_info_param
);


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_data_pipe_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
extern boolean tm_jgps_core_prtl_data_pipe_handler
(
  pd_comms_client_ipc_message_type   msg_type,  
  void                               *msg_data_ptr
);


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_timer_handler

DESCRIPTION
  This function handles the timer call back.

RETURN VALUE
  None

============================================================================*/
void tm_jgps_core_prtl_timer_handler
(
  const os_TimerExpiryType *timer_param
);


#endif /* FEATURE_JCDMA_GPSONE */
  
#ifdef __cplusplus
}
#endif

#endif /* TM_JGPS_CORE_H */

