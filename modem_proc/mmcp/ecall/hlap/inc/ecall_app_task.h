#ifndef ECALL_APP_TASK_H_
#define ECALL_APP_TASK_H_
/*===========================================================================

                           E C A L L _ T A S K . H

DESCRIPTION
  eCall task source file

REFERENCES
  3GPP TS 26.267

EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/inc/ecall_app_task.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $
===========================================================================*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "queue.h"
#include "ecall_app.h"
#include "ecall_modem_apis.h"
#include "cm.h"
#include "pbmlib.h"
#include "sys.h"

#ifndef FEATURE_ECALL_HAS_QMI_LOC
#include "pdapi.h"
#endif /* FEATURE_ECALL_HAS_QMI_LOC */

#ifndef FEATURE_ECALL_LCU45
#include "cm_dualsim.h"
#endif /* FEATURE_ECALL_LCU45 */

#ifndef FEATURE_ECALL_HAS_V1_HEADER
#include "aprv2_packet.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#endif /* FEATURE_ECALL_HAS_V1_HEADER */

#define ECALL_GPS_FIX_TIMEOUT          (80*1000) /* GPS Timer of 80 secs */
#define ECALL_START_TIMEOUT            (5*1000) /* CEN EN 16062:2015 T5: 5 secs */
#define ECALL_AL_ACK_TIMEOUT           (5*1000)        /* CEN EN 16062 T6: 5 secs */
/* HLAP EN 16062 7.12.4, redial attempts should timeout in 2 minutes   */
#define ECALL_REDIAL_TIMEOUT           (2*60*1000)     /* 2 minutes = 120,000 msecs */
#define ECALL_UNMUTE_TIMEOUT           (3000)       /* 3 secs */

#ifdef FEATURE_ECALL_HAS_MMOC
/*============================================================================== 
 
                 M S G R   M o d u l e   D E F I N I T I O N S
 
==============================================================================*/
#include "mmoc_msgr.h"
#define MSGR_MODULE_APP        0x01
#define MSGR_ECALL_APP         MSGR_TECH_MODULE( MSGR_TECH_ECALL, MSGR_MODULE_APP )

typedef struct
{
  mmoc_offline_ind_s_type   offline_ind;
}ecall_app_msgr_cmd_type;

typedef struct
{
  q_link_type          link;         /* Queue link */
  byte                 reserved[4];
  ecall_app_msgr_cmd_type  cmd;
} ecall_app_msgr_cmd_s_type;
#endif /* FEATURE_ECALL_HAS_MMOC */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
typedef enum
{
  ECALL_APP_TASK_OFFLINE_SIG             = 0x2000, /* TASK_OFFLINE_SIG */ 
  ECALL_APP_TASK_STOP_SIG                = 0x4000, /* TASK_STOP_SIG */ 
  ECALL_APP_TASK_START_SIG               = 0x8000, /* TASK_START_SIG */ 

  ECALL_APP_CMD_Q_SIG                    = 0x0001,
  ECALL_APP_GPS_TIMER_SIG                = 0x0002,
  ECALL_APP_GPS_DURING_CALL_TIMER_SIG    = 0x0004,
  ECALL_APP_START_TIMER_SIG              = 0x0008,

  ECALL_APP_SESSION_TIMER_SIG            = 0x0010,
  ECALL_APP_CALLBACK_TIMER_SIG           = 0x0020,
  ECALL_APP_AL_ACK_TIMER_SIG             = 0x0080,

  ECALL_APP_RETRY_TIMER_SIG              = 0x0200,
  ECALL_APP_CALL_CLEARDOWN_TIMER_SIG     = 0x0400,
  ECALL_APP_DOG_RPT_TIMER_SIG            = 0x0800,

  ECALL_APP_UNMUTE_TIMER_SIG             = 0x10000,
  ECALL_APP_NOTIFY_LOC_SVC_SIG           = 0x20000,
  ECALL_APP_SEND_MSG_WAIT_SIG            = 0x40000,
  ECALL_APP_MSGR_Q_SIG                   = 0x80000
} ecall_app_sig_enum_type;

typedef enum {
  ECALL_APP_CMD_GPS_FIX_RECVD = 0,
  ECALL_APP_CMD_GPS_ERROR_RECVD,
  ECALL_APP_CMD_SESSION_START,
  ECALL_APP_CMD_ECALL_CONNECTED,
  ECALL_APP_CMD_END_ECALL,
  ECALL_APP_CMD_INCOMING_ECALL,
  ECALL_APP_CMD_STOP_ECALL,
  ECALL_APP_CMD_TX_STATUS,
  ECALL_APP_CMD_GET_CALL_INFO,
  ECALL_APP_CMD_PBM_EVENT_UPDATE,
  ECALL_APP_CMD_PBM_SIM_FILE_INFO,
  ECALL_APP_CMD_APR_PACKET,
  ECALL_APP_CMD_UPDATE_ECALL_CALL_ID,
  ECALL_APP_CMD_UPDATE_CALL_STATUS,
  ECALL_APP_CMD_SETUP_CALL,
  ECALL_APP_CMD_MT_ECALL_END_START_MO_ECALL,
  ECALL_APP_CMD_UPDATE_ECALL_PROFILE,
  ECALL_APP_CMD_CM_SUBS_EVENT_INFO,
  ECALL_APP_CMD_UPDATE_MSD_SETTING
} ecall_app_task_cmd_id_e_type;

typedef enum {
  ECALL_APP_ECALL_INACTIVE = 0,
  ECALL_APP_ORIGINATING_CALL,
  ECALL_APP_IN_CALL_TRANSMITTING,
  ECALL_APP_WAITING_FOR_AL_ACK,
  ECALL_APP_IN_CALL,
  ECALL_APP_IDLE_ALLOW_MT_ECALL
} ecall_app_state_type;

typedef enum
{
  ECALL_MO_CALL = 0,
  ECALL_MT_CALL
}call_direction_type;

typedef enum {
  ECALL_PROFILE_TYPE_REGULAR   = 0,
  ECALL_PROFILE_TYPE_EMERGENCY = 1
}ecall_profile_type_enum_type;

typedef struct
{
  ecall_type_of_call type_of_call;
  ecall_activation_type activation_type;
}ecall_start_config_type;

typedef struct {
  q_link_type           link;
  ecall_app_task_cmd_id_e_type   cmd_id;
  union
  {
    ecall_session_call_info_cb_type cmd_call_info_cb;

    /*IVS status info: ECALL_APP_CMD_TX_STATUS */
    struct 
    {
      void              *tx_user_data;
      ecall_cb_data_type ivs_status;
    }ivs;

    /* PBM info: ECALL_APP_CMD_PBM_SIM_FILE_INFO
                 ECALL_APP_CMD_PBM_EVENT_UPDATE */
    struct
    {
      pbm_phonebook_type pb_id;
      pbm_extended_fileinfo_s_type info;
    }pbm;

    /* start session config info: ECALL_APP_CMD_SESSION_START */
    struct
    {
      ecall_start_config_type start_config;
      boolean qmi_start;
      ecall_start_session_cb_type start_session_cb;
      ecall_call_orig_info_cb_f_type call_orig_info_cb;
      ecall_call_orig_exten_info_cb_f_type call_orig_exten_info_cb;
      void *data_block_ptr;
      sys_modem_as_id_e_type as_id;
      boolean is_external_msd;
      uint8 external_msd[ECALL_MSD_MAX_LENGTH];
    }start_session_config;

    /* incoming call info: ECALL_APP_CMD_INCOMING_ECALL */
    struct
    {
      uint8 call_id;
      cm_call_type_e_type ecall_incom_call_type;
    }incom_call_config;

    /* eCall connected info: ECALL_APP_CMD_ECALL_CONNECTED */
    struct
    {
      call_direction_type   call_direction;
      ecall_tx_mode_type tx_mode; /* PUSH or PULL */
    }ecall_connected_config;

    /* end call info: ECALL_APP_CMD_END_ECALL */
    struct
    {
      boolean redial_pending;
      boolean is_call_fail_at_orig;
      boolean is_mo_ecall_normal_clearing;
      call_direction_type   call_direction;
      cm_call_end_e_type end_status;
    }end_call_config;

    /* GPS info: ECALL_APP_CMD_GPS_ERROR_RECVD
                 ECALL_APP_CMD_GPS_FIX_RECVD */
    struct
    {
      #ifdef FEATURE_ECALL_HAS_QMI_LOC
      boolean         latitude_valid;
      boolean         longitude_valid;
      boolean         timestamp_valid;
      boolean         vehicledirection_valid;
      boolean         horConfidence_valid;
      boolean         horUncCircular_valid;
      double          latitude;
      double          longitude;
      uint64_t        timestamp;
      float           vehicledirection;
      uint8_t         horConfidence;
      float           horUncCircular;
      #else /* PDAPI */
      pdsm_pd_data_type   ecall_cmd_gps_data;
      #endif /* FEATURE_ECALL_HAS_QMI_LOC */
    }gps_data;
    
    #if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
    /* APR packet: ECALL_APP_CMD_APR_PACKET */
    aprv2_packet_t         *apr_packet;
    #endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

    /* eCall Call ID: ECALL_APP_CMD_UPDATE_ECALL_CALL_ID */
    /* eCall Call ID: ECALL_APP_CMD_SETUP_CALL */
    cm_call_id_type    call_id;

    /* Call Status: ECALL_APP_CMD_UPDATE_CALL_STATUS */
    struct
    {
      cm_call_event_e_type           call_event;
      cm_call_id_type                call_id;
      cm_call_type_e_type            call_type;
    }call_status_data;

    /* eCall Profile: ECALL_APP_CMD_UPDATE_ECALL_PROFILE */
    ecall_profile_type_enum_type     ecall_profile;

    /* CM Subs Info: ECALL_APP_CMD_CM_SUBS_INFO */
    struct
    {
      cm_ph_event_e_type               subs_event;
      cm_subs_info_s_type              subs_info;
    }cm_subs_event_info;

    /* eCall MSD Setting: ECALL_APP_CMD_UPDATE_MSD_SETTING */
    ecall_session_update_msd_setting_s_type msd_setting;

  }data;
  
} ecall_app_task_cmd_type;

/*===========================================================================

                  F U N C T I O N   D E C L A R A T I O N S

===========================================================================*/
/*==========================================================================

  FUNCTION ECALL_APP_TASK

  DESCRIPTION
  Initializes the ECALL app task, defines timers, initialize DOG timer, etc.
    
  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*lint -esym(715,ignored) -e762 */
extern void ecall_app_task
(
  dword ignored
);
/*lint +esym(715,ignored) +e762 */

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_CMD_BUF

  DESCRIPTION
    Returns a command pointer off of the free queue, or NULL on failure.

  DEPENDENCIES
    None.

  RETURN VALUE
    Command pointer or NULL

  SIDE EFFECTS
    None

===========================================================================*/
ecall_app_task_cmd_type *ecall_app_task_get_cmd_buf
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_PUT_CMD

  DESCRIPTION
    Adds a command pointer to the command queue and signals the eCall task
    to process it.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    eCall task is signaled that there is a pending command

===========================================================================*/
void ecall_app_task_put_cmd(
  ecall_app_task_cmd_type  *cmd_ptr
  );


/*==========================================================================

  FUNCTION ECALL_GET_CM_CLIENT_ID

  DESCRIPTION
    This api returns the CM client ID for ecall App task

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
cm_client_id_type ecall_get_cm_client_id
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_NV

  DESCRIPTION
    This function will return the value of NV item 65589 for ECALL 

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

boolean ecall_app_task_get_nv
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_DEREGISTER_MODEM_CLIENT_IF_REGISTERED

  DESCRIPTION
    This function will de-register eCall modem client if one is registered.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_deregister_modem_client_if_registered
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_CALL_CMD_CB_FUNC

  DESCRIPTION
        This Callback Function is called when a call origination command
        is issued.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_call_cmd_cb_func
(
    void                      *data_block_ptr,
    cm_call_cmd_e_type        call_cmd,
    cm_call_cmd_err_e_type    call_cmd_err
);

/*==========================================================================

  FUNCTION ECALL_APP_CALL_CMD_EXTEN_CB_FUNC

  DESCRIPTION
        This Callback Function is called when a call origination command
        is issued.

  DEPENDENCIES
    None.

  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_call_cmd_exten_cb_func
(
    void                          *data_block_ptr,
    cm_call_cmd_e_type            call_cmd,
    cm_call_cmd_err_e_type        call_cmd_err,
    cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
    cm_alpha_s_type               alpha
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_GET_STATE

  DESCRIPTION
    This function will return the state of the ecall App - waiting for GPS, in call, idle, etc.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

ecall_app_state_type ecall_app_task_get_state
(
  void
);

/*==========================================================================

  FUNCTION ecall_app_task_init_before_task_start

  DESCRIPTION
    Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    Initialization of internal data is done.
 
===========================================================================*/
void ecall_app_task_init_before_task_start
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_INIT

  DESCRIPTION
    Initializes queues and defualt values for the global data.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_init
(
  void
);

/*==========================================================================
  FUNCTION ECALL_APP_MUTE_RX_OUT

  DESCRIPTION
    This function will mute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_app_apr_dest_port.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_mute_rx_out
(
  void
);

/*==========================================================================
  FUNCTION ECALL_APP_UNMUTE_RX_OUT

  DESCRIPTION
    This function will unmute the path from vocoder RX output to loudspeaker.

  DEPENDENCIES
    ecall_modem_apr_dest_port.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_unmute_rx_out
(
  void
);

/*==========================================================================

  FUNCTION ECALL_GET_CALL_ID

  DESCRIPTION
    This api returns the current call ID for ecall App task

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
===========================================================================*/
cm_call_id_type ecall_get_call_id
(
  void
);

/*==========================================================================

  FUNCTION ECALL_APP_TASK_START_SESSION_STATUS_UPDATE

  DESCRIPTION
    This function report the start session status to registered callbacks.

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_task_start_session_status_update
(
  ecall_session_status_type  session_status,
  cm_call_id_type            call_id,
  cm_client_id_type          client_id,
  sys_modem_as_id_e_type     as_id,
  void                      *data_block_ptr
);

/*==========================================================================
  FUNCTION ECALL_APP_PROCESS_GPS_TIMER_SIGNAL

  DESCRIPTION
    This function will process the GPS_TIMER expire signal.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_process_gps_timer_signal
(
  void
);

/*===========================================================================

  FUNCTION ECALL_APP_TASK_GET_IS_ERA_GLONASS_ENABLED

  DESCRIPTION
    This function will return the value of is_era_glonass_enabled.

  DEPENDENCIES
    None

  RETURN VALUE
    boolean: 0 - ERA-GLONASS not enabled
             1 - ERA-GLONASS enabled

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_task_get_is_era_glonass_enabled
(
  void
);

/*===========================================================================

  FUNCTION ECALL_APP_TASK_GET_ECALL_USIM_SLOT_ID

  DESCRIPTION
    This function will return the value of ecall_usim_slot_id configured
    in EFS.

  DEPENDENCIES
    None

  RETURN VALUE
    mmgsdi_slot_id_enum_type

  SIDE EFFECTS
    None

===========================================================================*/
mmgsdi_slot_id_enum_type ecall_app_task_get_ecall_usim_slot_id
(
  void
);

/*===========================================================================

  FUNCTION ECALL_APP_TASK_IS_ASID_VALID

  DESCRIPTION
    This function will check if the Modem as_id valid or not.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_task_is_asid_valid
(
  sys_modem_as_id_e_type asid
);

#if ( (defined FEATURE_ECALL_HAS_VOICE_A) || (defined FEATURE_ECALL_HAS_VOICE_B) )
/*===========================================================================

     A - F a m i l y   &   B - F a m i l y   C o m m o n   F u n c t i o n

===========================================================================*/
/*===========================================================================

  FUNCTION ECALL_APP_APR_INIT

  DESCRIPTION
    This function will initialize the APR interface for eCall APP

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void ecall_app_apr_init 
( 
  void
);

/*==========================================================================
  FUNCTION ECALL_APP_PROCESS_APR_PACKET

  DESCRIPTION
    This function will process the incoming APR packet.

  DEPENDENCIES
    FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_app_process_apr_packet
(
  aprv2_packet_t *packet
);

#endif /* FEATURE_ECALL_HAS_VOICE_A || FEATURE_ECALL_HAS_VOICE_B */

#else

void ecall_app_task_dummy_func(void);

#endif //End Featurisation

#endif /* ECALL_APP_TASK_H_ */

