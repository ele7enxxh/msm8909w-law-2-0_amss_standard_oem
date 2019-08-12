#ifndef MCCCCIM_H
#define MCCCCIM_H

/*===========================================================================

M A I N   C O N T R O L   C D M A   C A L L   C O N T R O L
              I N S T A N C E    M A N A G E R

DESCRIPTION
  This file contains declarations for use with the call control instance
  manager.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccccim.h_v   1.13   07 Oct 2002 11:59:18   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccccim.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
12/5/10    jj      CSFB: Added support for Rel8/Rel9 CSFB.
06/03/09   ag      Added support for displaying otasp session status.
                   Merged OTASP fixes for SPC validation failure.
05/13/09   ag      CMI Phase2 changes.
12/14/07   an      Added Passport Mode
06/11/07   pg      Lint Cleanup: Remove the first 2 arguments from
                   mccccim_is_clam_so_compatible().  They are not used.
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
11/06/06   rsj     Added MCCCCIM_LOW_BATTERY to mccccim_rel_or_del_reason_type
                   enum to support JCDMA Low Battery event as
                   end_reason.
06/12/06   fc      Added support for service status order.
06/02/06   fc      Cleanup umts interface.
07/21/04   sj      Added support for umts to 1x handover.
06/16/04   fc      Release C addendum changes for Sync ID.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
09/16/03   ph      Added mccccim_is_any_cc_in_wfo() to check before gps tuneaway.
03/31/03   va      Added alert_sent_to_cm boolean to mccccim_cc_info_type
10/07/02   va      More lint cleanup.
10/04/02   va      Added prototype for mccccim_get_cc_substate
09/06/02   va      Cleaned up lint errors.
04/12/02   lh      Added support for subaddress in Orig_C.
02/11/02   va      Added Support for flashing emergency call
02/07/02   lh      Added function mccccim_call_id_is_in_rel_substate().
01/23/02   ph      Changed prototype of mccccim_is_eom_so_compatible()
                   to allow SO to be changed.
11/30/01   va      Added new delete reason MCCCCIM_HHO_FAIL
10/04/01   kk      QoS Parameters added to mccccim_cc_info_type.
08/28/01   lh      Support for concurrent service.
08/24/01   ph      Added new func mccccim_con_is_in_rel_substate().
08/23/01   ph      Using CAI constant for max number of connections.
07/06/01   ph      Added support for Callback Timers
06/22/01   va      Added rel_reason is substate timer expired primitive also.
06/15/01   va      Added more comments, added call_id in AHDM primitive.
06/14/01   va      Initial Revision. Header file for MC CDMA Call Control
                   Instance Manager ( MCCCCIM).

===========================================================================*/

/* Include Files */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "caii.h"
#include "mccdma.h"
#include "mclog_v.h"
#include "rxtx_v.h"
#include "mc_v.h"


 /*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/

/* Maximum number of Call Control Instances supported */

#define MCCCCIM_NUM_MAX_CCS CAI_MAX_NUM_CONNECTIONS

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* List of interface primitive events that can be received from MCCTC (Also
   called Lower (L) L3) to MCCCCIM */

typedef enum
{
  MCCCCIM_RESET_WFO,       /* Reset timers for CCs in WFO substate */
  MCCCCIM_CREATE_CC,       /* Create a new Call Control Instance */
  MCCCCIM_UPDATE_CON_RECS, /* Update CC database based on new SCR */
  MCCCCIM_UPDATE_SO,       /* Update CC based on new SO connected thro' SO
                              negotiation */
  MCCCCIM_DELETE_ALL_CC,   /* Delete all Call Control Instances */
  MCCCCIM_DELETE_CC,       /* Delete a particular CC */
  MCCCCIM_RELEASE_ALL_CC,  /* Release all CCs */
  MCCCCIM_CC_MSG,          /* Process a Call Control Specific Message */
  MCCCCIM_ANALOG_HO,       /* Perform processing related to Analog HO */
  MCCCCIM_HANDLE_EMER_CALL,/* Handle emergency orig from CM */
  MCCCCIM_SO_NEG,          /* Do processing needed for change for service to SO
                              negotiation */
  MCCCCIM_NDSS,            /* Do processing related to NDSS */

  MCCCCIM_EVENTS_COUNT     /* This is always the last item on the enum */

} mccccim_event_code_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* List of reasons for releasing or deleting a CC */
typedef enum
{
  MCCCCIM_BS_END,                 /* BS Initiated release */
  MCCCCIM_USER_END,               /* User initiated Release */
  MCCCCIM_USER_POWERDOWN,         /* User trigerred power down, Reset or LPM */
  MCCCCIM_USER_SRV_INACTIVE,      /* User trigerred end with service inactive Ind */
  MCCCCIM_STATE_TIMER_EXP,        /* Substate timer expiration */
  MCCCCIM_STATE_TIMER_EXP_NO_ANS, /* Substate timer expiration due to no answer */
  MCCCCIM_FADE,                   /* Fade due to L2 ack timeout or bad frames */
  MCCCCIM_LOCK,                   /* BS ordered a lock */
  MCCCCIM_REDIRECTION,            /* BS redirected the MS */
  MCCCCIM_SO_REJ,                 /* Requested SO has been rejected by BS using SO neg */
  MCCCCIM_HHO_FAIL,               /* Returning to old system, hence delete new CCs */
  MCCCCIM_REL_TO_DEL,             /* Deleting a CC which should already in Release state */
  MCCCCIM_LOW_BATTERY,            /* Releasing a call with CAI_REL_PWDN for JCDMA Low battery event processing */
  MCCCCIM_REL_OTASP_ERR           /* Release initiated because of OTASP error */
} mccccim_rel_or_del_reason_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* List of status codes that can be returned to MCCTC in response to an
interface primitive */
typedef enum
{
  MCCCCIM_DONE,              /* Event Processing was successful */
  MCCCCIM_CC_NOT_FOUND,      /* No siutable CC found to process event */
  MCCCCIM_MSG_REJECT_STATE,  /* Message rejected by CC due to wrong state */
  MCCCCIM_MSG_REJECT_CAP,    /* Message rejected by CC due to CAPABILITY */
  MCCCCIM_MSG_REJECT_STRUCT, /* Message rejected by CC due to bad fields */
  MCCCCIM_ERR

} mccccim_event_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* For events w/o any payload */
typedef struct
{
  mccccim_event_code_type event_code; /* Event Code for the event to be
                                         processed */
  mccccim_event_status_type status;   /* Return status for the event populated
                                         by CCIM */
} mccccim_gen_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_CREATE_CC */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */

  uint8 call_id;                      /* ID for this call as provided by CM */
  boolean mob_orig;                   /* Indicates if this is a MS originated call */
  uint8 con_ref;                      /* con_ref given in SCR/CLAM, null if first call */
  uint8 sr_id;                        /* sr_id sent in orig/EOM, 0 if MS terminated call */
  word temp_so;                       /* SO sent in orig or page resp  or EOM */
  boolean is_null_cc;                 /* Identifies if this is the first CC instance */
  boolean bypass_alert_ans;           /* Indicates if alerting canbe bypassed */
  boolean orig_c_required;            /* Origination continuation required */
                                      /* Relevant only for mob origi calls */
  /* Note, The actual digits themselves are not given here, We will use them
     from the "cdma" struct, No need to waste memory for this again */

} mccccim_create_cc_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_UPDATE_CON_RECS */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code;    /* Event Code */
  mccccim_event_status_type status;      /* Return Status populated by CCIM */

  const caii_srv_cfg_type * new_cfg_ptr; /* Pointer to current config */

} mccccim_update_conrec_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_UPDATE_SO */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */
  word so;                            /* SO for the only existing CC, used only for SO negotiation
                                         when there is only one CC instance */
} mccccim_update_so_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_DELETE_ALL_CC*/
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */
  mccccim_rel_or_del_reason_type del_reason;

} mccccim_del_all_cc_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_DELETE_CC */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */
  byte con_ref;                       /* con_ref of the CC to be deleted */
  mccccim_rel_or_del_reason_type del_reason;

} mccccim_delete_cc_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_RELEASE_ALL_CC*/
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */
  mccccim_rel_or_del_reason_type rel_reason;

} mccccim_rel_all_cc_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_CC_MSG */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */

  caii_rx_msg_type * msg_ptr;         /* CC message to be processed */
  byte con_ref;
   /* If the msg is rejected by CC, this is the con_ref of
      the CC for which the msg was intended, populated by MCCCCIM */

} mccccim_cc_msg_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_ANANLOG_HO */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */

  caii_fm_ho_type * msg_ptr;          /* AHDM message to be processed */
  cdma_cc_state_type substate;        /* Substate of the "preserved" con_ref,
                                         Populated by MCCCCIM */
  byte call_id;                       /* Call_id of the call to be handed off,
                                         populated by MCCCCIM */

} mccccim_ahdm_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_HANDLE_EMER_CALL*/
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */
  mc_origination_type * cm_msg_ptr;   /* Orig mesg received from CM */

} mccccim_emer_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Payload for MCCCCIM_CC_NDSS */
typedef struct
{
  /* Don't add any new member before mccccim_event_status_type */

  mccccim_event_code_type event_code; /* Event Code */
  mccccim_event_status_type status;   /* Return Status populated by CCIM */

} mccccim_ndss_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Union of all event payload types */
typedef union
{
  mccccim_gen_event_type gen;
  mccccim_create_cc_event_type create_cc;         // MCCCCIM_CREATE_CC
  mccccim_update_conrec_event_type update_conrec; // MCCCCIM_UPDATE_CON_REC
  mccccim_update_so_event_type update_so;         // MCCCCIM_UPDATE_SO
  mccccim_delete_cc_event_type del_cc;            // MCCCCIM_DELETE_CC
  mccccim_del_all_cc_event_type del_all_cc;       // MCCCCIM_DELETE_ALL_CC
  mccccim_rel_all_cc_event_type rel_all_cc;       // MCCCCIM_RELEASE_ALL_CC
  mccccim_cc_msg_event_type cc_msg;               // MCCCCIM_CC_MSG
  mccccim_ahdm_event_type ahdm_msg;               // MCCCCIM_ANALOG_HO
  mccccim_emer_event_type emer;                   // MCCCCIM_HANDLE_EMER_CALL
  mccccim_gen_event_type so_neg;                  /* MCCCCIM_SO_NEG */
  mccccim_ndss_event_type ndss;                   // MCCCCIM_CC_NDSS

} mccccim_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure describing parameters of a Call Control Instance */
typedef struct
{
  boolean active;                /* Indicates if this CC entry in table is
                                    even valid */
  uint8 cc_index;                /* Index of this CC in the array */
  uint8 call_id;                 /* ID for this call as provided by CM */
  uint8 con_ref;                 /* con_ref given in SCR, null if SCR is not received */
  word so;                       /* SO that has been assigned to this con_ref,
                                    NULL_SO if SCR has not been received */
  byte sr_id;                    /* sr_id sent in origination, set it to 0 if
                                    this is a MS terminated call */
  word temp_so;                  /* SO sent in orig or page resp  or EOM */
  boolean bypass_alert_ans;      /* Indicates if alerting canbe bypassed */
  cdma_cc_state_type substate;   /* Substate for this CC */
  boolean is_null_cc;            /* Indicates if this was the NULL CC i.e the
                                    one responsible for transition from idle to
                                    TC state */
  boolean mob_orig;              /* Indicates if this is a MS originated call */
  mccccim_rel_or_del_reason_type rel_del_reason;
                                 /* Reason why this CC is in release or deleted
                                    state */
  boolean orig_c_required;       /* Orig Cont msg required */

  /* The following variables are associated with DTMF processing */

  boolean dtmf_stop;             /* Indicates that dtmf_timer expires a
                                    DTMF_STOP order must be transmitted */
  boolean dtmf_ack_wait;         /* Indicator that we are waiting for an ack to
                                    a Start or Stop Continous DTMF order or a
                                    Burst DTMF message */
  byte cont_dtmf;                /* Pending continous DTMF */
  rex_timer_type substate_timer; /* timer for substate expiration */
  rex_timer_type dtmf_timer;     /* timer for DTMF operation */

  boolean alert_sent_to_cm;

  #ifdef FEATURE_IS2000_REL_A
  /* In case Base Station had assigned a QoS, that needs to conveyed to CM/UI */

  boolean qos_parms_incl;        /* Qos Parameters indicator */
  byte qos_parms_len;
  byte qos_parms[CAI_ORIG_QOS_LEN_MAX];
  #endif /* FEATURE_IS2000_REL_A */

} mccccim_cc_info_type;

/* CC Info table */
extern mccccim_cc_info_type mccccim_cc_info[MCCCCIM_NUM_MAX_CCS];
/* externalizing this because for CSFB post processing after
 * CSFB -> CDMA handover
 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Interface definition data structs between MCCTC (LL3) and MCCCCIM */

/* List of interface primitives that can be sent from MCCCCIM to MCCTC */
typedef enum
{
  MCCTC_SUBSTATE_TIMER_EXP, /* Substate Timer for a CC has expired */
  MCCTC_CALL_END,           /* User has released a particular call */
  MCCTC_ENTER_TC_STATE,     /* The CC has come out of Release substate */
  MCCTC_EVENTS_END

} mccccim_tc_event_code_type;

typedef struct
{
  /* Don't add any new member before evt_code */

  mccccim_tc_event_code_type evt_code; /* Event Code */

} mccccim_tc_gen_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Data struct for MCCTC_SUBSTATE_TIMER_EXP */
typedef struct
{
  /* Don't add any new member before evt_code */

  mccccim_tc_event_code_type evt_code; /* Event Code */
  byte con_ref;                        /* con_ref of CC whose timer expired */
  mccccim_rel_or_del_reason_type rel_reason;
                                       /* Indicates in which substate it expired*/

} mccccim_tc_timer_exp_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Data struct for MCCTC_CALL_END */
typedef struct
{
  /* Don't add any new member before evt_code */

  mccccim_tc_event_code_type evt_code; /* Event Code */

  byte con_ref;                        /* con_ref of CC which is being released */
  mccccim_rel_or_del_reason_type rel_reason;
                                       /* Reason why this was released*/

} mccccim_tc_call_end_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Union of all primitives between MCCTC and MCCCCIM */
typedef union
{
  mccccim_tc_gen_type gen;
  mccccim_tc_timer_exp_event_type timer_exp; /* For MCCTC_SUBSTATE_TIMER_EXP */
  mccccim_tc_call_end_event_type call_end;   /* For MCCTC_CALL_END */

} mccccim_tc_event_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  /* CallId for an emergency call that was flashed
  on top of an existing call */
  byte emer_call_id;

  /* Boolean indicating if ack for emergency
  flash was received */
  boolean waiting_for_ack;

  /* ccindex on which this call was flashed */
  byte cc_index;

} mccccim_emer_flash_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Function Prototypes */

/*===========================================================================

FUNCTION MCCCCIM_INITIALIZE

DESCRIPTION
  This function resets the CCIM database. Good to call this as soon
  MCCTC enters TC init.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mccccim_initialize( void );

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_EVENT

DESCRIPTION
  This function processes primitives received from LL3 (MCCTC).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mccccim_process_event( mccccim_event_type * event_ptr);

/*===========================================================================
FUNCTION MCCCCIM_PROCESS_HS_CMDS

DESCRIPTION
  This function processes commands received from CM. The CC this
  commands is intended for is derived from the call_id in the
  command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mccccim_process_hs_cmds
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
);

/*===========================================================================

FUNCTION MCCCCIM_GET_NUM_CCS_IN_NON_REL_STATE

DESCRIPTION
  This function returns the number of Call Control Instances
  which are active but not in "Release" substate. Used by MCCTC
  to determine if MCCTC can go to the "Release" substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 mccccim_get_num_ccs_in_non_rel_state( void );

/*===========================================================================

FUNCTION MCCCCIM_CAN_SO_BE_CONNECTED

DESCRIPTION
  This function determines if the TXC can "connect" this SO.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if it can be connected, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_can_so_be_connected
(
  byte con_ref,
  byte con_ref_index,
  word so
);

/*===========================================================================

FUNCTION mccccim_is_any_cc_in_wfo

DESCRIPTION
  This function checks if any Call Control instance is in WFO.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if any active CC is in WFO, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_is_any_cc_in_wfo(void);

/*===========================================================================

FUNCTION MCCCCIM_CON_IS_IN_REL_SUBSTATE

DESCRIPTION
  This function determines if a connection is in release substate and
  is used when the SCR action packet is prepared.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if con_ref is present and in release substate, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_con_is_in_rel_substate
(
  byte con_ref
);

/*===========================================================================

FUNCTION MCCCCIM_TC_EXIT

DESCRIPTION
  This function does the necessary clean up when the MS exits the
  TC state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccccim_tc_exit( void );

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_ACK

DESCRIPTION
  This function does processing related to receiving L2 acks for
  specific CC messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccccim_process_ack
(
  rxtx_cmd_type *cmd_ptr
  /* Pointer to rxtx command taken from mc_rxtx_ack_q */
);

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_TIMER_EXP

DESCRIPTION
  This function does processing associated with a Call Control
  Instance Timer expiration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccccim_process_timer_exp( unsigned long timer_id);

/*===========================================================================

FUNCTION MCCCCIM_INITIALIZE_TIMERS

DESCRIPTION
  This function initializes timers that are call-control specific.
  This should only be called once and is invoked from mcc_powerup_init()
  in mccdma.c

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccccim_initialize_timers( void );

extern const  byte MCCCCIM_CALL_ID_NOT_FOUND;

/*===========================================================================

FUNCTION MCCCCIM_GET_CC_INDEX_FROM_CALL_ID

DESCRIPTION
  This function seracges the Call Control Instance data base and retrives
  the entry that matches the given call_id

DEPENDENCIES
  None.

RETURN VALUE
  index of the corresponding CC in the array, if no such call_id is
  found, MCCCCIM_CALL_ID_NOT_FOUND is returned.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 mccccim_get_cc_index_from_callid( byte call_id );

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION MCCCCIM_IS_EOM_SO_COMPATIBLE

DESCRIPTION
  This function checks if an SO is compatible with the existing ones. It is
  called to see whether an EOM can be sent.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if compatible.
  FALSE if the proposed SO does not, and has no alternative to be compatible.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_is_eom_so_compatible(word *so);

/*===========================================================================

FUNCTION MCCCCIM_IS_CLAM_SO_COMPATIBLE

DESCRIPTION
  This function checks if an SO from CLAM is compatible with the existing ones,
  or if it is compatible with the one that MS proposed in EOM.

DEPENDENCIES
  This function can only be called when SO is included in CLAM.

RETURN VALUE
  TRUE if compatible.
  FALSE if the proposed SO does not, and has no alternative to be compatible.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_is_clam_so_compatible
(
  word so_clam
    /* SO supplied in CLAM */
);

/*===========================================================================

FUNCTION MCCCCIM_UPDATE_TEMP_SO

DESCRIPTION
  This function updates the temp_so field in CCI.

DEPENDENCIES
  This function shall only be called if CCI identified by CON_REF exists.

RETURN VALUE
  TRUE if successful.
  FALSE if not.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_update_temp_so
(
  byte con_ref,
    /* CON_REF to identify CCI */
  word temp_so
    /* Temp SO */
);

/*===========================================================================

FUNCTION MCCCCIM_IS_ABLE_TO_CREATE_CC

DESCRIPTION
  This function check to see if a CC can be created. This function does not
  actually create a CC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a CC can be created.
  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mccccim_is_able_to_create_cc( void );

/*===========================================================================

FUNCTION MCCCCIM_IS_CON_REF_EXIST

DESCRIPTION
  This function check to see if a CC has the specified con_ref.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a CC with matching con_ref is found.
  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mccccim_is_con_ref_exist( byte con_ref );

/*===========================================================================

FUNCTION MCCCCIM_NUM_AVAILABLE_CC

DESCRIPTION
  This function reports the maximum number of CCIs that can be initiated.

DEPENDENCIES
  None.

RETURN VALUE
  Number of CCIs that can be initiated.

SIDE EFFECTS
  None.
===========================================================================*/
extern byte mccccim_num_available_cc(void);

/*===========================================================================

FUNCTION MCCCCIM_IS_ORIG

DESCRIPTION
  This function check if any call is MS originated.

DEPENDENCIES
  None.

RETURN VALUE
  True if any call is MS originated.
  False if no call is MS originated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mccccim_is_orig(void);

/*===========================================================================

FUNCTION MCCCCIM_CALL_ID_IS_IN_REL_SUBSTATE

DESCRIPTION
  This function determines if a call_id is in release substate.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if call_id is present and in release substate, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccccim_call_id_is_in_rel_substate
(
  byte call_id
);
#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION MCCCCIM_GET_CC_SUBSTATE

DESCRIPTION
  This function returns the substate of a CC instance.

DEPENDENCIES
  None.

RETURN VALUE
  substate of the corresponding CC in the array, if no such con_ref is
  found, CDMA_CC_RELEASE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
cdma_cc_state_type mccccim_get_cc_substate( byte con_ref);

/*===========================================================================

FUNCTION MCCCCIM_DIAG_GET_STATE

DESCRIPTION
  This function returns state info for all CCs active.

DEPENDENCIES
  None.

RETURN VALUE
  The argument ptr is filled with CC info and returned to caller.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mccccim_diag_get_state( mclog_diag_state_query_rsp_type * mc_state_ptr);

#endif //MCCCCIM

