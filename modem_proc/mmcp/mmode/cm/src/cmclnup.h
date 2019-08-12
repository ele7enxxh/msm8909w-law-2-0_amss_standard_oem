#ifndef CMCLNUP_H
#define CMCLNUP_H
/*===========================================================================

                    C L N U P    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Call Manager ( CM )
  to call the Cleanup module.

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmclnup.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   ak      Create file.

===========================================================================*/

#include "cmi.h"

#define SUBS_CLEANUP_REASON_MASK_NONE (uint8) 0

/* Identify which stage of cleanup is currently going on. IMS deregistration
** is done prior to DS cleanup.
**
** Future expectation is to have the IMS deregistration move to DS.
*/
typedef enum cm_clnup_stage_e {

  CM_CLNUP_STAGE_NONE,        /* No cleanup activity initiated. */

  CM_CLNUP_STAGE_IMS_DEREG,   /* Currently awaiting IMS deregistration */

  CM_CLNUP_STAGE_DS_CLNUP,    /* Currently awaiting DS cleanup */

  CM_CLNUP_STAGE_DONE,        /* Current cleanup is complete. */

  CM_CLNUP_MAX

} cm_clnup_stage_e_type;

/* This is to determine what kind of cleanup is needed. In case of a multi-sim
** device, the subscription change related cleanup would be restricted to a
** single asubs_id. However the operating mode change would impact entire UE
** which is greater impact operation
**
** So if operating mode change is received while processing subscription_chg,
** the field will be updated as the operating mode has wider impact.
**
** If subscription change occurs while operating mode is being changed, then
** field will not be updated as it has a narrower impact.
**
** The subscription ids run from 0. Whereas the below field is to be used in
** Bitmask. So if needed, use the OPER_SUBS values directly. But for OPER_OPRT
** always convert to bitmask first.
*/
typedef enum cm_clnup_oper_e {

  CM_CLNUP_OPER_NONE ,        /* Internal bound checking. */

  CM_CLNUP_OPER_SUBS_ID_1 = SYS_MODEM_AS_ID_1_MASK,    /* 1 */

  CM_CLNUP_OPER_SUBS_ID_2 = SYS_MODEM_AS_ID_2_MASK,    /* 2 */

  CM_CLNUP_OPER_OPRT_MODE,    /* 3. Cleanup needed for operating mode change */

  CM_CLNUP_OPER_SUBS_ID_3 = SYS_MODEM_AS_ID_3_MASK,    /* 4 */

  CM_CLNUP_OPER_MAX           /* Internal bound checking. */

} cm_clnup_oper_e_type;

typedef struct cm_clnup_curr_oper_payload_s
{
  uint8                   subs_id_bitmask;
    /* bitmask needed by DS callback.
    */
  sys_oprt_mode_e_type    oprt_mode;
    /* operating mode checked before calling the IMS callback.
    */
  uint8       sub_cleanup_reason[MAX_AS_IDS];
  /* Reason for cleaning up sub */

}cm_clnup_curr_oper_payload_s_type;

typedef struct cm_clnup_state_s cm_clnup_state_s_type;

/*===========================================================================

FUNCTION cmclnup_init

DESCRIPTION
  Initialize the Clean up state machine.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclnup_st_mc_init( void );

/*===========================================================================

FUNCTION cmclnup_is_ongoing_subsc

DESCRIPTION
  Let the calling entity know if cleanup is currently in progress.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up currently in progress.
  FALSE: Cleanup operation not currently underway.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                cmclnup_is_ongoing_subsc(

       uint8                  subs_bm

);

/*===========================================================================

FUNCTION cmclnup_is_ongoing_subsc

DESCRIPTION
  Let the calling entity know if cleanup is currently in progress.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up currently in progress.
  FALSE: Cleanup operation not currently underway.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean                cmclnup_is_ongoing_oprt_mode( void );

/*===========================================================================

FUNCTION cmclnup_is_clnup_needed

DESCRIPTION
  If the device is currently in Online operating mode and has subscription
  available, cleanup operation might be needed.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up might be needed.
  FALSE: Cleanup operation wont be needed.

SIDE EFFECTS
  none

===========================================================================*/
boolean                cmclnup_is_clnup_needed
(
  cm_subscription_not_avail_cause_e_type sub_not_avail_cause,
  sys_oprt_mode_e_type                   cmd_oprt_mode
);

/*===========================================================================

FUNCTION cm_buff_cmd_q_cnt

DESCRIPTION
  Determine if there are any commands in the buffer command queue.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  TRUE: Buffer Queue is empty
  FALSE: Buffer Queue contains commands.

SIDE EFFECTS
  none

===========================================================================*/
boolean                cmclnup_is_buff_q_empty( void );

/*===========================================================================

FUNCTION cmclnup_initiate_cleanup

DESCRIPTION
  Initiate the cleanup operation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
       void                   cmclnup_initiate_cleanup(

       cm_ph_cmd_s_type const *      ph_cmd_ptr
);

#ifdef FEATURE_IP_CALL
/*===========================================================================

FUNCTION cmclnup_ip_rpt_proc

DESCRIPTION
  Update the current cleanup operation in the Cleanup state machine.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void                          cmclnup_ip_rpt_proc(

       cm_name_type                  rpt_name,

       uint8                         subs_bm
);
#endif

/*===========================================================================

FUNCTION cmclnup_ds_pkt_rpt_proc

DESCRIPTION
  Process the DS cleanup confirmation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void                   cmclnup_ds_pkt_rpt_proc( uint8 );

/*===========================================================================

FUNCTION cmclnup_timer_proc

DESCRIPTION
  Process the DS cleanup confirmation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void                   cmclnup_timer_proc( void );

/*===========================================================================

FUNCTION cmclnup_is_cmd_tobe_buff

DESCRIPTION
  Put the Phone command on to the buffer queue if the cleanup related to it is
  already underway.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  TRUE: Command was placed on the buffer queue.
  False: Command was not placed on the buffer queue.

SIDE EFFECTS
  none

===========================================================================*/
boolean                       cmclnup_is_cmd_tobe_buff(

       cm_ph_cmd_s_type const *      ph_cmd_ptr

);

#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================

FUNCTION cmclnup_get_sub_reason_mask

DESCRIPTION
  Get the cleanup reason mask 

DEPENDENCIES
  none

RETURN VALUE
  cm_clnup_stage_e_type

SIDE EFFECTS
  none

===========================================================================*/
uint8* cmclnup_get_sub_reason_mask( void );


#endif  /* CMCLNUP_H */

