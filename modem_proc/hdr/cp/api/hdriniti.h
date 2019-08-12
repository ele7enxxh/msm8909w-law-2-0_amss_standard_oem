#ifndef HDRINITI_H
#define HDRINITI_H

/*===========================================================================

          I N I T I A L I Z A T I O N S T A T E  P R O T O C O L
                  I N T E R N A L  D E F I N I T I O N S

DESCRIPTION
  This contains the internal declarations for the Initialization State Protocol (INIT).

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdriniti.h_v   1.17   06 Mar 2003 11:24:52   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdriniti.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/13   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
07/27/11   cnx     Merged Optimized Handoff changes.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
12/02/03   mpa     Removed start_pilot_acq_hold_period()
03/05/03   mpa     Changed timers to use REX callbacks instead of signals
                   Added start_pilot_acq_hold_period()
11/12/02   dna     Moved call to hdrlmac_acquire() to hdrinit.c
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
03/14/01   aaj     Set pilot acq timer back to 60secs
02/05/01   dna     Shortened pilot acq timer from 60 seconds to 1 second until
                   searcher starts reporting acquisition failures.
10/10/00   SH      added HDRINIT_REACQUIRE_CMD
07/29/00   SH      Added PACKED to hdrinit_sync_msg_offset_type
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
#include "hdrind.h"

/*===========================================================================

               DATA DECLARATIONS

===========================================================================*/

/* Commands supported by the initialization state protocol */

/*
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
   HDRINIT_ACTIVATE_CMD      =0,   /* Activates the the initialization
                                      state protocol */
   HDRINIT_DEACTIVATE_CMD    =1,   /* Deactivates the the initialization
                                      state protocol */
   HDRINIT_REACQUIRE_CMD     =2    /* Require the system */

#ifdef FEATURE_LTE_TO_HDR_OH
   ,HDRINIT_CONTINUE_ACQ_CMD =3    /* Continues with acquiring the system */
#endif /* FEATURE_LTE_TO_HDR_OH */

}hdrinit_cmd_enum_type;

typedef struct
{
   hdrinit_cmd_enum_type cmd;             /* command id */
   hdrhai_protocol_name_enum_type sender; /* protocol giving command,
                                             for debug */
}hdrinit_cmd_type;

/*===========================================================================

FUNCTION HDRINIT_START_SYNC_MSG_TIMER

DESCRIPTION
  This function starts the sync msg timer if it is not active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_start_sync_msg_timer ( void );


/*===========================================================================

FUNCTION HDRINIT_STOP_SYNC_MSG_TIMER

DESCRIPTION
  This function stops the sync msg timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_stop_sync_msg_timer(void);

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRINIT_START_LTE_TO_HDR_HO_HYSTR_TIMER

DESCRIPTION
  This function starts the LTE to HDR handoff hystersis timer if it is not 
  active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_start_lte_to_hdr_ho_hystr_timer ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_STOP_LTE_TO_HDR_HO_HYSTR_TIMER

DESCRIPTION
  This function stops the LTE to HDR handoff hystersis timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_stop_lte_to_hdr_ho_hystr_timer(void);
#endif /* FEATURE_LTE_TO_HDR */

/*===========================================================================

FUNCTION HDRINIT_PROCESS_CMD

DESCRIPTION
  This function processes commands given to the initialization state protocol.

DEPENDENCIES
  None

PARAMETERS
  init_cmd_ptr - Pointer to the command for the initialization state protocol
            to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_process_cmd
(
  hdrinit_cmd_type *init_cmd_ptr
);

/*===========================================================================

FUNCTION HDRINIT_PROCESS_IND

DESCRIPTION
  This function processes indications given to INIT.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for INIT to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_process_ind
(
  hdrind_ind_name_enum_type       ind_name,
  hdrind_ind_data_union_type  *ind_data_ptr
);


/*===========================================================================

FUNCTION HDRINIT_PROCESS_MSG

DESCRIPTION
  This function processes messages for INIT.

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
extern void hdrinit_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);


/*===========================================================================
FUNCTION HDRINIT_PROCESS_SIG

DESCRIPTION
  This function processes signals for INIT.

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
extern void hdrinit_process_sig
(
  rex_sigs_type sig
);

/*===========================================================================
FUNCTION HDRINIT_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the init state
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
extern void hdrinit_process_timer
(
  uint32 timer_id
);

/*===========================================================================
FUNCTION HDRINIT_TIMER_CB

DESCRIPTION
  This function is the callback for all init state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrinit_timer_cb
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION HDRINIT_PAUSE_AND_STOP_SYNC_MSG_TIMER

DESCRIPTION
  This function pauses and stops the sync msg timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_pause_and_stop_sync_msg_timer(void);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_START_SYNC_SUSPEND_TIMER

DESCRIPTION
  This function starts the sync suspend timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_start_sync_suspend_timer ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_STOP_SYNC_SUSPEND_TIMER

DESCRIPTION
  This function stops the sync suspend timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_stop_sync_suspend_timer(void);

/*===========================================================================

FUNCTION HDRINIT_RESET_SAVED_SYNC_MSG_TIMER

DESCRIPTION
  This function resets the saved sync msg timer 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_reset_saved_sync_msg_timer(void);

#endif /* HDRINITI_H */

