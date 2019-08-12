#ifndef HDRIDLEI_H
#define HDRIDLEI_H

/*===========================================================================

          I N I T I A L I Z A T I O N S T A T E  P R O T O C O L
                  I N T E R N A L  D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the Initialization State 
  Protocol (IDLE).

Copyright (c) 2000 - 2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdridlei.h_v   1.32   29 Jul 2002 12:34:46   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdridlei.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/24/10   pxu     Removed FEATURE_HDR_QSM features 
12/01/09   suren   Added QChat optimization changes.
09/23/09   sju     Added support for SVDO.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
09/05/06   mpa     Added support for FEATURE_HDR_QSM.
04/03/06   yll     Added hdridle_enter_monitor() with parameter of whether
                   this state transition was from Sleep state.
11/09/04   dna     Allow for ConnectionRequest to be aborted.
07/17/02   dna     Queue timer expiration events on hdrmc.event_q because timer
                   callback runs as a separate thread and causes problems.
06/07/02   dna     Make Connection Setup Timeout standard compliant
04/22/02   sh      Added Long Sleep timer
03/29/02   dna     Changed timers to use REX callback instead of indications
03/29/02   dna     Only give SDSS_EVT_HDR_OPR_RESEL_OK on first sleep
01/21/02   sh      Replaced mode change cmd with update pccc cmd
11/17/01   sh      Added 1xhdr data session handoff support
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
07/11/01   SH      Added more connect reasons 
03/27/01   SH      Removed trans_id array in the control block
03/23/01   SH      Added the cnt argument for start conn setup timer
03/21/01   SH      Added trans_id array for storing SLP callback info
03/10/01   SH      Added support for PCCC configuration negotiation
09/21/00   SH      Added slotted operation
05/29/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "qw.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "hdrbit.h"
#include "hdrmc_v.h"
#include "hdrovhd.h"
#include "hdrcmac.h"
#include "hdridle.h"
#include "cai.h"

#ifdef FEATURE_CMI
#include "cai_v.h"
#endif


/*===========================================================================

          DEFIDLEIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRIDLE_PROT_SUBTYPE_DEFAULT    0x0000   /* default IDP subtype*/

#define HDRIDLE_CONN_SETUP_TIMER1_MSECS  2500  /* connection setup state 
                                                  duration timer is started 
                                                  after rcv successful SLP
                                                  conntion request tx outcome
                                                  callback  */

/* This timer duplicates T_RTCMPATSetup (See section 8.5.8) to avoid an
 * extra 1 sec. stuck in ConnectionSetup if quick connect fails
 */
#define HDRIDLE_CONN_SETUP_TIMER2_MSECS  1500  /* connection setup state 
                                                  duration timer is started 
                                                  upon entering the conn 
                                                  setup state and the
                                                  rtcmac is active */
#ifdef FEATURE_LTE_TO_HDR_OH
/* Connection set up timer for tunnel mode */
#define HDRIDLE_CONN_SETUP_TIMER3_MSECS  2000  /* Maximum time to wait for 
                                                  transition to Connection 
                                                  Setup State when Connection
                                                  Request is tunneled through
                                                  the other radio access 
                                                  technology */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* number of control channel cycles consituting a sleep cycle */
#define HDRIDLE_NUM_CC_CYCLES              0x0c

/* Address of the TCB for the task in which this protocol runs */
#define HDRIDLE_TASK_PTR hdrmc_tcb_ptr

/*===========================================================================

               DATA DECLARATIONS

===========================================================================*/

/* connect reason bit pattern */
#define HDRIDLE_CR_USER         0x01      /* user requests to open a 
                                             connection               */
#define HDRIDLE_CR_MSG          0x02      /* open a connection for HMP 
                                             to send RTC messages     */
#define HDRIDLE_CR_AN           0x04      /* AN requests to open a 
                                             connection               */ 
#define HDRIDLE_CR_ABORT        0x08      /* user abort the open conn
                                             request                  */
#ifdef FEATURE_HDR_HANDOFF
#define HDRIDLE_CR_NO_DATA      0x10      /* open conn but no data to be sent
                                          */
#endif  /* FEATURE_HDR_HANDOFF */

#ifdef FEATURE_LTE_TO_HDR_OH
#define HDRIDLE_CR_ACTIVE_HO    0x20
#endif /* FEATURE_LTE_TO_HDR_OH */
  
/* Commands supported by the idle state protocol */

/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
   HDRIDLE_ACTIVATE_CMD=0,        /* Activates the the idle  state protocol */
   HDRIDLE_DEACTIVATE_CMD=1,      /* Deactivates the the idle state protocol*/
   HDRIDLE_OPEN_CONN_CMD=2,       /* Open connection request */
   HDRIDLE_ABORT_OPEN_CONN_CMD=3, /* Abort open connection request */
   HDRIDLE_CONFIG_CMD=4,          /* Start config negotiation if needed */
   HDRIDLE_UPDATE_PCCC_CMD=5,     /* Update the preferred control channel cycle*/
   HDRIDLE_FINISH_ABORT_CON_SETUP_CMD=6,  /* Return to Monitor state */
   HDRIDLE_CONN_REQ_CANCELED_CMD=7

}hdridle_cmd_enum_type;


typedef enum
{
  HDRIDLE_STATE_TIMER,      /* Idle state timer */
  HDRIDLE_SUSPEND_TIMER,    /* Idle suspend timer */
  HDRIDLE_CONFIG_RES_TIMER, /* Idle Config Response timer */
  HDRIDLE_LONG_SLEEP_DELAY_TIMER,  /* Long sleep timer */
  HDRIDLE_QSM_TIMER         /* QSM timer */
#ifdef FEATURE_HDR_QCHAT
  ,HDRIDLE_QCHAT_TIMER      /* QChat timer */
#else
  ,HDRIDLE_RESERVED_1       /* Reserved */
#endif /*FEATURE_HDR_QCHAT*/ 
#ifdef FEATURE_LTE_TO_HDR_OH
  ,HDRIDLE_TUNNEL_CONN_TIMER /* tunnel connection timer */
#else
  ,HDRIDLE_RESERVED_2      /* Reserved */
#endif /*FEATURE_LTE_TO_HDR_OH*/ 
} hdridle_timer_enum_type;

typedef struct
{
   hdridle_cmd_enum_type          cmd;    /* command id */
   hdrhai_protocol_name_enum_type sender; /* protocol giving command, 
                                             for debug */
   union {
   hdrhai_abort_reason_enum_type          abort_reason;
   hdrhai_open_reason_enum_type           open_reason;
   } data;
}hdridle_cmd_type;
      
/*===========================================================================

FUNCTION HDRIDLE_START_SUSPEND_TIMER

DESCRIPTION
  This function starts the suspend timer if the advertised suspend time
  is greater than the current time.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_start_suspend_timer ( void );


/*===========================================================================

FUNCTION HDRIDLE_STOP_SUSPEND_TIMER

DESCRIPTION
  This function stops the suspend timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_stop_suspend_timer ( void );

/*===========================================================================

FUNCTION HDRIDLE_START_CONN_SETUP_TIMER

DESCRIPTION
  This function starts the connection setup timer

DEPENDENCIES
  None

PARAMETERS
  cnt - timer count in millisecond

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_start_conn_setup_timer 
( 
  rex_timer_cnt_type cnt 
);


/*===========================================================================

FUNCTION HDRIDLE_STOP_CONN_SETUP_TIMER

DESCRIPTION
  This function stops the connection setup timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_stop_conn_setup_timer ( void );



/*===========================================================================

FUNCTION HDRIDLE_SEND_CONN_REQ_MSG

DESCRIPTION
  This function sends the connection request message to the access
  network

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_send_conn_req_msg ( void );


/*===========================================================================

FUNCTION HDRIDLE_CHECK_OK_TO_SLEEP

DESCRIPTION
     This function checks whether all requirements for transition to
     the Sleep State are met or not. If they are met, then change the state
     to the Sleep State.

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_check_ok_to_sleep(void);


/*===========================================================================

FUNCTION HDRIDLE_ENTER_MONITOR

DESCRIPTION
  This function processes the entering of Monitor state. 

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  come_from_sleep - Whether the state transition is from Sleep state.

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_enter_monitor
(
  boolean come_from_sleep
);


/*===========================================================================

FUNCTION HDRIDLE_SET_FIRST_SLEEP_FLAG

DESCRIPTION
  This function set the flag that indicates if the idle state protocol has
  entered sleep since it last entered monitor state, either from some state
  other than sleep.

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  flag   - TRUE or FALSE

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_set_first_sleep_flag(boolean flag);

/*===========================================================================

FUNCTION HDRIDLE_PROCESS_IND

DESCRIPTION
  This function processes indications given to IDLE.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for IDLE to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_process_ind
(
  hdrind_ind_name_enum_type         ind_name,
  hdrind_ind_data_union_type        *ind_data_ptr
);

/*===========================================================================
FUNCTION HDRIDLE_PROCESS_SIG

DESCRIPTION
  This function processes signals for IDLE.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdridle_process_sig
(
  rex_sigs_type sig
);

/*===========================================================================

FUNCTION HDRIDLE_PROCESS_MSG

DESCRIPTION
  This function processes messages for IDLE.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
);


/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the idle state protocol.

DEPENDENCIES
  None

PARAMETERS
  idle_cmd_ptr - Pointer to the command for the idle state protocol
            to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_process_cmd
(
     hdridle_cmd_type *idle_cmd_ptr
);

/*===========================================================================
FUNCTION HDRIDLE_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the idle state 
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdridle_process_timer
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION HDRIDLE_SET_CONNECT_REASON

DESCRIPTION
  This function is used to set the connection setup reason

DEPENDENCIES
  hdridle_init must have been called before calling this function.

PARAMETERS
  reason   - connection setup reason 

RETURN VALUE:
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_set_connect_reason
(
  uint8 reason
);

/*===========================================================================
FUNCTION HDRIDLE_TIMER_CB

DESCRIPTION
  This function is the callback for all idle state protocol timers.
  This function is called by REX when the timer expires, and runs 
  in the context of the task that defined the timer (HDRMC).

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdridle_timer_cb
(
  uint32 timer_id
);

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRIDLE_STOP_TUNNEL_CONN_TIMER

DESCRIPTION
  This function stops the tunnel connection timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdridle_stop_tunnel_conn_timer ( void );
#endif /* FEATURE_LTE_TO_HDR_OH */

#endif /* HDRIDLEI_H */

