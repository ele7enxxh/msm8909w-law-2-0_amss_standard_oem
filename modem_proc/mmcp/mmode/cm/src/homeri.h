#ifndef HOMERI_H
#define HOMERI_H
/*===========================================================================

                      H A N D O F F   M A N A G E R
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  handoff between different protocols. for Internal use of CM only
  
Copyright (c) 1999 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/homeri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   ra      Added support for IMS VCC in HOMER files 
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/16/06   pk      Lint Fixes
10/10/06   pk      Initial Revision

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "cmdbg.h"
#include "sys.h"
#include "sys_v.h"

#ifdef  FEATURE_HDR_CSNA
#include "caix.h"
#include "homer.h"
#endif


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Macro to correctly align function definitions (cosmetic).
*/
#define EXTERN

/* Invalid timer.
*/
#define HOMER_INVALID_TIMER                (dword)(-1)

/* Service lost timer T3.
*/
#define HOMER_UMTS_SRV_LOST_T3_TIMER       (dword)(75)

/*
*/
#define HOMER_1X_PSEUDO_ONLINE_T1_TIMER    (dword)(75)

/* 1x -> UMTS reselection hold period.
*/
#define HOMER_1X_UMTS_RESEL_HOLD_T4_TIMER  (dword)(30)

/* UMTS sr. acquire timer after IX to UMTS handover.
*/
#define HOMER_UMTS_SRV_ACQ_TIMER           (dword)(30)

#endif


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Homer state
*/
typedef enum homer_state_e {

  HOMER_STATE_NONE = -1,                /* INTERNAL CM use ONLY             */

  HOMER_STATE_VOIP_HDR_1X_NONE,         /* Not in VoIP on HDR to 1X handoff */

  HOMER_STATE_VOIP_HDR_1X_WAIT_SRDM,    /* Waiting for SRDM from HDR        */

  HOMER_STATE_VOIP_HDR_1X_WAIT_MC_ORIG, /* Waiting for MC origination       */

  HOMER_STATE_VOIP_HDR_1X_WAIT_UHDM,    /* Waiting for UHDM from HDR        */

  HOMER_STATE_VOIP_HDR_1X_WAIT_VOIP_CALL_END, /* Waiting for VoIP call end from CP */

  HOMER_STATE_VOIP_HDR_1X_WAIT_VOIP_CALL_END_CNF, /* Waiting for VoIP call end from CP */

  HOMER_STATE_VOIP_HDR_1X_WAIT_1X_CALL_CONNECT,/* Waiting for confirmation from MC */

  HOMER_STATE_VOIP_HDR_1X_MAX,          /* INTERNAL CM use ONLY             */

  HOMER_STATE_UMTS_1X_NONE,             /* Not in UMTS 1X handover          */

  HOMER_STATE_UMTS_1X_WAIT_PSEUDO_ONLINE,
                                        /* Waiting for 1X to be pseudo 
                                        ** online                           */

  HOMER_STATE_UMTS_1X_WAIT_HANDOFF_REQ, /* Wait for handoff req. from UMTS  */

  HOMER_STATE_UMTS_1X_HANDOFF_RETRY,    /* Re-dialing the call on 1x        */

  HOMER_STATE_UMTS_1X_WAIT_HANDOFF_END, /* Wait for handoff to end          */

  HOMER_STATE_MAX                       /* INTERNAL CM use ONLY             */


} homer_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration that describes whether the event (command or report) was
** processed to completion or not.
*/
typedef enum {

  HOMER_EVT_STATUS_CONSUMED,            /* The event was processed to
                                       ** completion.
                                       */

  HOMER_EVT_STATUS_NOT_CONSUMED,        /* The event was not processed, the
                                       ** CMHM should call the transaction
                                       ** handlers again.
                                       */

  HOMER_EVT_STATUS_MAX                  /* For internal use. */

} homer_evt_status_e_type;


/*----------------------------------------------------------------------*/

/** Enum that defines the handoff call ids   */
typedef enum {

    HOMER_SWITCH_OUT_CALL_ID,
    /**< Switch out call id*/

    HOMER_SWITCH_IN_CALL_ID,
    /**< Switch in call id*/

    HOMER_CALL_ID_MAX
    /**< @Internal use only*/

} homer_handoff_call_id_e_type;



/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Homer state
*/
typedef struct homer_s {

  homer_state_e_type state;
    /* The current state of the handoff manager
    */

  cm_call_id_type   call_id;
    /**< The call is associated with the call.  
    ** Also denotes switch_out call ID from HOM  
    */
   
  cm_call_id_type   switch_in_call_id;
    /* switch_in call ID from HOM  
    */

  boolean           ho_req_rcvd;
    /* Handoff request received
    */

  boolean           end_cnf_rvcd;
    /* Call end confirmation already received
    */


} homer_s_type;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef  FEATURE_HDR_CSNA
/* CSNA configuration
*/
typedef struct homer_csna_msg_config_tbl_s {

  int                           num_valid_cch_fwd_csna_msg;
    /* Number of Valid cch fwd csna messages
    */

  homer_csna_msg_config_s_type  cch_fwd_link_msg[HOMER_MAX_CSNA_MSG];
    /* Common Channel forward link messages
    */

  int                           num_valid_ded_fwd_csna_msg;
    /* Number of Valid ded fwd csna messages
    */

  homer_csna_msg_config_s_type  ded_fwd_link_msg[HOMER_MAX_CSNA_MSG];
    /* Dedicated Channel forward link messages
    */

  int                           num_valid_cch_rev_csna_msg;
    /* Number of Valid cch rev csna messages
    */

  homer_csna_msg_config_s_type  cch_rev_link_msg[HOMER_MAX_CSNA_MSG];
    /* Common Channel reverse link messages
    */

  int                           num_valid_ded_rev_csna_msg;
    /* Number of Valid ded rev csna messages
    */

  homer_csna_msg_config_s_type  ded_rev_link_msg[HOMER_MAX_CSNA_MSG];
    /* Dedicated Channel reverse link messages
    */

} homer_csna_msg_config_tbl_s_type;

#endif

/*===========================================================================

@FUNCTION homer_umts_1x_handover_proc

@DESCRIPTION
  This function handles the Traffic handover for UMTS to 1X.
  In this state, the state handler will

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern  void                 homer_umts_1x_handover_proc(

        cmcall_s_type        *call_ptr,
            /* The pointer to the call object
            */

        cm_call_event_e_type  call_event,
            /* The call event that generated this request
            */

        cm_ss_event_e_type    ss_event
            /* The SS event that generated this request
            */
);


/*===========================================================================

FUNCTION homer_get_state

DESCRIPTION
  Retruns the current homer state
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  homer_state_e_type   homer_get_state( void );


/*===========================================================================

FUNCTION homer_init

DESCRIPTION
  Initializes HOMER
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void                 homer_init( void );





#ifdef FEATURE_HDR_CSNA

/*===========================================================================

FUNCTION homer_csna_msg_config_tbl_ptr

DESCRIPTION
  Retruns the current homer state
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern homer_csna_msg_config_tbl_s_type* homer_csna_msg_config_tbl_ptr( void );
/*===========================================================================

FUNCTION homer_csna_msg_config_tbl_init

DESCRIPTION
  Initialize the CSNA message configuration status
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void                 homer_csna_msg_config_tbl_init( void );

/*===========================================================================

FUNCTION homer_csna_config_msg_update_proc

DESCRIPTION
  Handles the update to the CSNA configuration message table
  
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None
 
SIDE EFFECTS
  None
  
===========================================================================*/

extern  void                 homer_csna_config_msg_update_proc(

        const cm_mc_rpt_type       *cm_mc_rpt_ptr
            /* Handoff message from HDR
            */

);
#endif /* FEATURE_HDR_CSNA */

#endif /* HOMERI_H */
