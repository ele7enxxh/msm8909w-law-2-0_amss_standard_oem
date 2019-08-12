#ifndef HDRIDLE_H
#define HDRIDLE_H

/*===========================================================================

          I D L E S  T A T E  P R O T O C O L
                 D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the idle protocol (IDLE).

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$PVCSPath: L:/src/asw/COMMON/vcs/hdridle.h_v   1.16   14 Mar 2003 11:10:10   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdridle.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
04/14/14   vko     Reset idle_reacq_suspend_to_reacq if LMAC AWAKE is recieved when IDLE is not in MONITOR
04/07/14   vko     Fixed IDLE giving SLEEP command to LMAC, when SRCH moves from 3a->3f
02/21/14   vko     FR2399 - improved cc handling
08/24/12   sju     Fix to Re-try open conn cmd after resume if hdrhmp_pending_queue
                   is having pending RTC MSG from HDRHAI_IDLE_STATE_PROTOCOL
08/07/12   cnx     Abort Active Handoff if redirection is received.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
08/01/11   cnx     Merged Optimized Handoff changes.
07/28/11   cnx     Supported force long sleep. 
09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
05/28/10   pba     Added eHRPD VOIP Silent redial feature.
03/24/10   pxu     Removed FEATURE_HDR_QSM features 
12/01/09   suren   Added QChat optimization changes.
02/10/09   sju     Added support for intra scc sleep.
12/23/08   pba     Support for Quick Idle Protocol
06/08/08   wsh     Fixed DCM can't turn on when in ConSetup
04/30/08   wsh     Moved code to check if data session is on HDR to ARM9
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
11/27/06   pba     Added hdridle_is_suspended()
10/10/06   hal     Added support to throttle 1x acquisitions if AT negotiates low SCI
08/03/06   etv     (for Sarika, QCHAT). Exported hdridle_get_current_sci.
05/10/06   yll/dna Don't allow new AC messages to be sent while waiting for
                   TCA
05/09/06   yll     Added support for Minimal UATI Mode.
04/26/06   hal     Renamed hdridle_con_is_for_signaling() to 
                   hdridle_con_is_for_session_config()
07/07/05   dna     Added support for Enhanced Idle State protocol
11/09/04   dna     Allow for ConnectionRequest to be aborted.
09/29/03   mpa     Added hdridle_is_sleeping()
07/30/03   mpa     Merged in 6500 baseline
03/12/03   mpa     Fixed attribute_id_length
12/02/02   vas     Added hdridle_ready_for_long_sleep()
05/06/02   dna     Tell others whether connection is opening to send signaling
01/21/02   sh      Added hdridle_update_pref_cntl_chan_cycle and removed
                   hdridle_mode_change
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
07/11/01   SH      Added connect reason to the open connection command
04/17/01   SH      Renamed hdrilde_pccc_reconfig to hdridle_mode_change
03/19/01   dna     Added define for config attribute ID length
03/10/01   SH      Added PCCC related command function prototypes.
09/21/00   SH      Added slotted operation
05/29/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "hdrind.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define HDRIDLE_MAX_PREF_CHANNEL 6

/* Connection request reason for the ConnectionRequest message */
#define HDRIDLE_AT_INITIATED  0x00
#define HDRIDLE_AN_INITIATED  0x01

/* Attribute ID length in bytes */                                   
#define HDRIDLE_ATTRIBUTE_ID_LENGTH 1

/* Bit Mask for Connection Setup Fail Reason */
#define HDRIDLE_CONN_FAIL_CONN_DENY_MASK      HDRHAI_CONN_FAIL_IDLE_CONN_DENY
#define HDRIDLE_CONN_FAIL_CR_ROR_MASK         HDRHAI_CONN_FAIL_IDLE_CR_ROR
#define HDRIDLE_CONN_FAIL_SAP_L2_FAILED_MASK  HDRHAI_CONN_FAIL_SAP_L2_FAILED
#define HDRIDLE_CONN_FAIL_SAP_L3_FAILED_MASK  HDRHAI_CONN_FAIL_SAP_L3_FAILED

typedef enum                       /* Connection setup failure enum        */
{                                  /* -------------------------------------*/
  HDRIDLE_CF_SYS_LOST,             /* Call Fade                            */
  HDRIDLE_CF_TCA_TIMEOUT,          /* state timeout - TCA                  */
  HDRIDLE_CF_RTCACK_TIMEOUT,       /* state timeout - RTCAck               */
  HDRIDLE_CF_MAX_ACC_PROBE,        /* access failure                       */
  HDRIDLE_CF_REDIRECT_CHG_HDR,     /* redirect to another HDR              */
  HDRIDLE_CF_REDIRECT_EXIT_HDR,    /* redirection causes HDR to exit       */
  HDRIDLE_CF_PRL_NOT_PREF_CHG_HDR,  /* chaneg to another HDR due to not
                                       preferred by PRL                    */
  HDRIDLE_CF_PRL_NOT_PREF_EXIT_HDR, /* Exit HDR due to not preferred by PRL*/

  HDRIDLE_CF_USER,                 /* user abort                           */
  HDRIDLE_CF_CD_GENERAL,           /* Rcv Conn deny msg - reason:general   */
  HDRIDLE_CF_CD_NET_BUSY,          /* Rcv Conn deny msg - reason:net busy  */
  HDRIDLE_CF_CD_AUTH_BILL_FAIL,    /* Rcv Conn deny msg - reason: auth or  */
                                   /* billing failure                      */
  HDRIDLE_CF_ORIG_ERR_ABORT,       /* needless enum duplication            */
  HDRIDLE_CF_TCA_FAILED,  
  HDRIDLE_CF_CD_PREF_CHAN_NOT_AVAIL/* Rcv Conn deny msg - reason: Pref ch 
                                      not available                        */
} hdridle_conn_fail_enum_type;
/* Message structure */

/* Connection Request message */
typedef struct
{
   uint8                            msg_id;
   uint8                            trans_id;
   uint8                            req_reason;
   uint8                            preferred_channel_cnt;
   sys_hdr_channel_record_s_type    preferred_channel[HDRIDLE_MAX_PREF_CHANNEL];                            
   boolean                          awaiting_slp_callback;
}hdridle_conn_req_msg_type;

/* Connection Deny message */
typedef struct
{
   uint8      trans_id;
   uint8      deny_reason;
}hdridle_conn_deny_msg_type;

/* Intra CC indication structure */
typedef struct
{
   boolean      continued_scc_pkt_expected;
     /* CONTINUED_SCC_PKT_EXPECTED indication rcvd */
   uint16       cc_start;
     /* CC start in slots */
   uint8        sci;
     /* SCI value that AT used between Control Channel Packet */
}hdridle_intra_scc_ind_type;

typedef struct
{
  rex_timer_type                suspend_timer;
    /* suspend timer */
  rex_timer_type                state_timer;
    /* idle state timer */
#ifdef FEATURE_LTE_TO_HDR_OH
  rex_timer_type                tunnel_conn_timer;
    /* tunnel connection timer */
#endif /* FEATURE_LTE_TO_HDR_OH */
#ifdef FEATURE_HDR_LONG_SLEEP
  rex_timer_type                long_sleep_delay_timer;
    /* long sleep timer. If this timer is on, long sleep is not allowed */
#endif /* FEATURE_HDR_LONG_SLEEP */
  hdridle_conn_req_msg_type     conn_req_msg;
    /* connection request message */
  hdridle_conn_deny_msg_type    conn_deny_msg;
    /* connection deny message    */
  boolean                       rcv_end_of_sync_cap_ind;
    /* End of sync capsule rcvd or missed since wakeup? */
  boolean                       sleep_capsule_done;
    /* SLEEP_CAPSULE_DONE indication rcvd since wakeup? */
  boolean                       first_sleep;
  /* If idle has entered sleep since it was activated */
  boolean                       lmac_suspend_or_resume_received;
    /* If idle has received an indication from LMAC to either suspend or 
     * resume since it was activated */
  uint8                         connect_reason;
    /* reason for opening a connection               */
  uint64                        slot_cycle1_timeout;
    /* System time at which to change from SlotCcyle1 to SlotCycle2 */
  uint64                        slot_cycle2_timeout;
    /* System time at which to change from SlotCcyle2 to SlotCycle3 */
  boolean                       access_since_entering_monitor;
    /* If an access has attempted after since last time 
     * AT enters Monitor state. */
  rex_timer_type                imo_timer;
#ifdef FEATURE_HDR_DDARF
  int                           num_reacq_failed;
    /* Number of consecutive reacquisition failures each after
       waking from sleep. */
  uint8                         last_sci;
    /* SCI value that AT used when it last went to sleep */
#endif
#ifdef FEATURE_HDR_CC_DECODE_FAIL
  int                           num_cc_reacq_failed;
#endif
  boolean                       throttling_1x_acq;
    /* If IDLE is throttling 1x acquisition because of a low SCI */
  uint32                        sci_override;
    /* Holds the override SCI value read from HDR L1 debug NV item */
  boolean                       cr_ror_in_progress;
    /* Whether CR+RoR is being sent */
  boolean                       data_session_is_on_hdr;

  uint8                         config_change;

  hdridle_intra_scc_ind_type    intra_scc;
  boolean                       qc_is_rxed_in_this_scc;
    /* If QC msg has been received in this sync control channel capsule*/
#ifdef FEATURE_HDR_TO_LTE
  hdrsrch_wakeup_reason_enum_type last_wakeup_reason;
#endif /* FEATURE_HDR_TO_LTE */

  boolean                       force_long_sleep;
    /* If this flag is true, AT uses 40 seconds as sleep period in SLEEP
     * state. */ 

#ifdef FEATURE_HDR_REVC
  boolean                       multi_at_page_msg_rcvd;
    /* Whether AT has received a MultiATPage message in the current sector */

  boolean                       multi_at_page_attrib_rejected;
    /* Whether MultiATPageMessageSupported attribute proposed has been
       rejected by the current serving sector */
#endif
  boolean                       retry_open_conn_cmd;
    /* Whether to retry open conn cmd*/ 
  boolean                       idle_suspend_to_reacq;
    /* Check whether SRCH moved to reacq from suspend in Idle state */
  boolean                       first_sleep_for_meas;
}hdridle_type;

extern hdridle_type hdridle;
/*===========================================================================

FUNCTION HDRIDLE_ABORT_OPEN_CONNECTION

DESCRIPTION
  This function commands the idle state protocol to abort
  open connection request.

DEPENDENCIES
  None.

PARAMETERS
  reason  - reason for aborting the open connection
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_abort_open_connection 
( 
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);

 
/*===========================================================================

FUNCTION HDRIDLE_ACTIVATE

DESCRIPTION
  This function commands the idle state protocol to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_activate 
( 
  hdrhai_protocol_name_enum_type sender 
);


/*===========================================================================

FUNCTION HDRIDLE_DEACTIVATE

DESCRIPTION
  This function commands the idle state protocol to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  reason  - reason for the deactivation
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_deactivate 
( 
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);


/*===========================================================================

FUNCTION HDRIDLE_OPEN_CONNECTION

DESCRIPTION
  This function commands the idle state protocol to open a connection.

DEPENDENCIES
  None.

PARAMETERS
  reason - reason to open a connection
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_open_connection 
(
  hdrhai_open_reason_enum_type   reason,
  hdrhai_protocol_name_enum_type sender 
);



/*===========================================================================

FUNCTION HDRIDLE_CONFIGURE

DESCRIPTION
  This function queries the Idle State Protocol whether configuration
  negotiation is needed or not.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridle_configure 
( 
  hdrhai_protocol_name_enum_type sender 
);


/*===========================================================================

FUNCTION HDRIDLE_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization of the idle state protocol.

DEPENDENCIES
  This function must called only once after powerup 

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_powerup_init ( void );

/*===========================================================================

FUNCTION HDRIDLE_INIT

DESCRIPTION
  This function initializes the idle state protocol.

DEPENDENCIES
  This function must called every time when the acess terminal enters the HDR mode
  and before the idle state protocol can be activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_init ( void );


/*===========================================================================

FUNCTION HDRIDLE_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRIDLE.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdridle_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void                      *ind_data_ptr
);

/*===========================================================================

FUNCTION HDRIDLE_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRIDLE.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_msg_cb
( 
  dsm_item_type *item_ptr 
);

/*===========================================================================

FUNCTION HDRIDLE_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRIDLE in-config instance.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_config_msg_cb
(
  dsm_item_type *item_ptr
);

/*===========================================================================

FUNCTION HDRIDLE_RESELECT_IS_ALLOWED

DESCRIPTION
  This function processes an abort open connection command given to the
  the idle state protocol

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)
  
RETURN VALUE
  TRUE        - Relection is allowed.
  FALSE       - Relection is not allowed.       
SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridle_reselect_is_allowed 
( 
  hdrhai_protocol_name_enum_type caller 
); 

/*===========================================================================

FUNCTION HDRIDLE_CON_IS_FOR_SESSION_CONFIG

DESCRIPTION
  Return whether the current connection is being opened in order to send 
  signaling information for session configuration

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the current connection is being opened in order to send 
  signaling information for session configuration
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdridle_con_is_for_session_config (void);

/*===========================================================================

FUNCTION HDRIDLE_UPDATE_PREF_CNTL_CHAN_CYCLE

DESCRIPTION
  This function commands the idle state protocol tto update 
  the preferred control channel cycle.
 

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_update_pref_cntl_chan_cycle 
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRIDLE_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This function sets force_long_sleep flag.
 
  When force_long_sleep flag is set, AT uses 40 seconds as sleep period when
  it is ready to go to sleep state.

DEPENDENCIES
  None

PARAMETERS
  force_long_sleep - turn on/off force_long_sleep flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_set_force_long_sleep
(
  boolean force_long_sleep
);

/*===========================================================================

FUNCTION HDRIDLE_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This function gets the current value of force_long_sleep flag.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - force_long_sleep flag is set. AT uses 40 seconds as sleep period in
         SLEEP state. 
  FALSE - force_long_sleep flag is not set.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_get_force_long_sleep( void );

/* <EJECT> */
#ifdef FEATURE_HDR_LONG_SLEEP
/*===========================================================================

FUNCTION HDRIDLE_READY_FOR_LONG_SLEEP

DESCRIPTION
  This function checks for if AT were to go to sleep, would it do long sleep.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT were to do long sleep
  FALSE - Otherwise
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_ready_for_long_sleep( void );

#endif /* FEATURE_HDR_LONG_SLEEP */

/*===========================================================================

FUNCTION HDRIDLE_SEND_CALL_CONNECTED_TO_CM

DESCRIPTION
  This function sends an connected notification to the Call Manager

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The alert ans bypass field is always set to TURE

===========================================================================*/
void hdridle_send_call_connected_to_cm( void );

/*===========================================================================

FUNCTION HDRIDLE_IS_SLEEPING

DESCRIPTION
  This function checks if AT is sleeping

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AT is sleeping
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_sleeping( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_SUSPENDED

DESCRIPTION
  This function checks if IDLE state protocol is in suspended state

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IDLE state protocol is in suspended state
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_suspended( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTION_NEEDED

DESCRIPTION
  This function tells if a connection is needed by the protocol


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if a connection is needed, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_connection_needed ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_ALMP_LEAVING_IDLE

DESCRIPTION
  This function processes when ALMP leaves the Idle, specifically it resets
  the lmac_suspend_or_resume_received flag. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None  

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_almp_leaving_idle ( void );

/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTING

DESCRIPTION
  This function tells if a idle is attempting to open a connection.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if performing connection setup, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridle_is_connecting ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_GET_CURRENT_SCI

DESCRIPTION
  This function computes the current slot cycle based on the Enhanced Idle 
  State Protocol slotted sleep schedule.

DEPENDENCIES
  None

PARAMETERS
  gear_ptr - Return which SlotCycle value is chosen, for debug

RETURN VALUE
  The proper slot cycle to use

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdridle_get_current_sci ( int *gear_ptr );

/*===========================================================================

FUNCTION HDRIDLE_STOP_1X_ACQ_THROTTLING

DESCRIPTION
  This function stops 1x acquisition throttling if needed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdridle_stop_1x_acq_throttling( void );

/*===========================================================================

FUNCTION HDRIDLE_GET_CR_ROR_PROGRESS

DESCRIPTION
  This function gets ConReq + RoR progress.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  True - if CR+RoR is in progress
  False - if CR+RoR has ended.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdridle_get_cr_ror_progress( void );

/*===========================================================================

FUNCTION HDRIDLE_SET_CR_ROR_PROGRESS

DESCRIPTION
  This function sets ConReq + RoR progress.

DEPENDENCIES
  None.

PARAMETERS
  progress - indicates if CR+RoR is in progress

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdridle_set_cr_ror_progress
(
  boolean progress
);

/*===========================================================================

FUNCTION HDRIDLE_IS_PKT_DATA_SESS_ON_HDR

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - Whether the data session is on hdr

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdridle_is_pkt_data_sess_on_hdr(void);

/*===========================================================================

FUNCTION HDRIDLE_SET_PKT_DATA_SESS_STATUS

DESCRIPTION
  Sets the current packet data session status - whether it is on HDR
  or not. This function is called everytime the session status changes.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  is_session_on_hdr - TRUE if whether data session is on HDR or 1x
                    - FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_set_pkt_data_sess_status
(
  boolean is_session_on_hdr
);

/*===========================================================================

FUNCTION HDRIDLE_IS_CONNECTING_BEFORE_ACK

DESCRIPTION
  Checks if IDLE is in connection set up, and the ConRequest
  has not been ACKed

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  True - If IDLE is connecting AND ConReq is ACKed
  False - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdridle_is_connecting_before_ack( void );

/*===========================================================================

FUNCTION HDRIDLE_GET_CURRENT_CONFIG_CHANGE

DESCRIPTION
  This function returns the configuration change values associated with the 
  current sector.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Value of the configuration change field for current sector.

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 hdridle_get_current_config_change ( void );

/*===========================================================================

FUNCTION HDRIDLE_SLEEP_BASED_ON_QUICKPAGE

DESCRIPTION
  This function sets the sleep flag to indicate whether AT can go 
  to sleep based on quick page message.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdridle_sleep_based_on_quickpage( boolean sleep_flag );

/*===========================================================================

FUNCTION HDRIDLE_RESET_QC_IS_RECEIVED_FLAG

DESCRIPTION
  This function set the flag that indicates if the QC is received in current 
  Sync Capsule 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_reset_qc_is_received_flag( void );

#ifdef FEATURE_HDR_REVC
/*===========================================================================

FUNCTION HDRIDLE_IS_MULTI_AT_PAGE_MSG_RCVD

DESCRIPTION
  Check whether any MultiATPage message is received from current
  serving sector.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE  - A MultiATPage message has been received from current
          serving sector in the CONNECTED or IDLE state
  FALSE - None of loading information message has been received.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean hdridle_is_multi_at_page_msg_rcvd ( void );

/*===========================================================================
 
FUNCTION HDRIDLE_SET_MULTI_AT_PAGE_MSG_RCVD

DESCRIPTION
  This function resets the indicator which shows whether MultiATPage
  message is received in the CONNECTED OR IDLE state from the current serving
  sector.
 
DEPENDENCIES
  None.

PARAMETERS
  multi_at_page_msg_rcvd - TRUE, if MultiATPage message has been received
                           in the CONNECTED OR IDLE state from the current 
                           serving sector. FALSE, if not.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 
===========================================================================*/
extern void hdridle_set_multi_at_page_msg_rcvd
(
  boolean multi_at_page_msg_rcvd 
);

/*===========================================================================
 
FUNCTION HDRIDLE_IS_MULTI_AT_PAGE_ATTRIB_REJECTED

DESCRIPTION
  This function tells whether MultiATPageMessageSupported attribute has been
  rejected by the current serving sector or not. 

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  TRUE  - MultiATPageMessageSupported attribute request has been
          rejected by the current serving sector.
  FALSE - MultiATPageMessageSupported attribute request has not
          been rejected by the current serving sector yet.

SIDE EFFECTS
  None.
 
===========================================================================*/
extern boolean hdridle_is_multi_at_page_attrib_rejected( void );

/*===========================================================================
 
FUNCTION HDRIDLE_SET_MULTI_AT_PAGE_ATTRIB_REJECTED

DESCRIPTION
  This function resets the indicator which shows whether
  MultiATPageMessageSupported attribute proposed by AT is rejected by the 
  current serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  multi_at_page_attrib_rejected - TRUE, if MultiATPageMessageSupported 
                                  attribute proposed or GAUPed has been 
                                  rejected by the current serving sector. 
                                  FALSE, if not.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdridle_set_multi_at_page_attrib_rejected
(
  boolean multi_at_page_attrib_rejected 
);
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_TUNNEL_CONN_TIMEOUT

DESCRIPTION
  This function processes the tunnel connection timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridle_process_tunnel_conn_timeout ( void );
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_CC_DECODE_FAIL
/*===========================================================================
 
FUNCTION HDRIDLE_RESET_CC_REACQ_FAIL_COUNTER

DESCRIPTION
  This function resets the cc/re-acq fail counter.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdridle_reset_cc_reacq_fail_counter(void);
#endif /* FEATURE_HDR_CC_DECODE_FAIL */

/*===========================================================================

FUNCTION HDRIDLE_RESET_IDLE_SUSPEND_TO_REACQ_FLAG

DESCRIPTION
  This function set the flag that indicates if idle suspend to reacq occured

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_reset_idle_suspend_to_reacq_flag( void );

/*=============================================================================

FUNCTION HDRIDLE_SET_FIRST_HDR_TO_LTE_REDIR_MEAS

DESCRIPTION
  This function sets if LTE measurements for redirection are in progress (or) not

DEPENDENCIES
  None.

PARAMETERS
  value - value to be set TRUE (or) FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdridle_set_first_hdr_to_lte_redir_meas
(
   boolean value
);

#endif /* HDRIDLE_H */
