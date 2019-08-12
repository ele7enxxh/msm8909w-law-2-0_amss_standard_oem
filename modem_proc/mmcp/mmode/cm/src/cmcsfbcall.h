
#ifndef CMCSFBCALL_H
#define CMCSFBCALL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C S F B  C A L L

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling calls through
  IP. IP call reports are received and call object is updated. Call
  Origination, end functions are implemented to call registered
  applications that work using IP.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2005 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmcsfbcall.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/13   dj      Fix to unforce the 1xcsfb call end preferences.
01/14/12   vk      Channges related to handling of RLF cause code in 1XCSFB call.
09/14/11   rk      NV items N_min_MO_call_soft_retry and N_maxSIB8 
09/02/11   rk      S102 less 1XCSFB
06/11/11   rk      Send CSFB abort req command to NAS if user ended the call 
                   while 1XCP is CSFB activated state
04/05/11   rk      Correcting CSFB call silent redial behavior
03/30/11   rk      revising handling of failure cases
03/22/11   rk      Support for handling negative scenarios
12/10/10   rk      Adding support for MT CSFB call
10/22/10   rk      Sucessful MO call initial support

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"
#include "cm.h"
#include "cmi.h"
#include "cmcall.h"


#define CM_CSFB_CALL_MAX_SR_TIME_SOFT_FAIL_RETRIES      20

/* UE will continue using 1X/PP CSFB over the LTE network for up to 20 seconds
** or until the number of short-term failures exceeds N_min_MO_call_soft_retry for 1XCSFB case.
*/ 


#if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) 

/*===========================================================================

FUNCTION cmcsfbcall_reset

DESCRIPTION
  Reset CSFB call fields to default values reflecting
  a CSFB  call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcsfbcall_reset(

     cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

#endif

#if defined (FEATURE_LTE_TO_1X) 

/*===========================================================================

FUNCTION cmcsfbcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. replies to  call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcsfbcall_rpt_proc(

  const cm_esm_cmd_u_type   *cm_rpt_ptr
    /* Pointer to LL report */
);


/*===========================================================================

FUNCTION cmcsfbcall_rpt_proc

DESCRIPTION
  Process srv ind for RLF scenario's.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcsfbcall_handle_srv_ind (

const sys_srv_status_e_type   srv_type

);

/*===========================================================================

FUNCTION cmcsfbcall_send_orig_req

DESCRIPTION
  During LTE to GW or GW to LTE call transfer, map call object info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void cmcsfbcall_send_orig_req (
    
       const cmcall_s_type    *call_ptr
            /* pointer to a call object */
    );



/*===========================================================================

FUNCTION cmcsfbcall_send_1xcsfb_call_clear

DESCRIPTION
  Send 1XCSFB call clear command to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
 void cmcsfbcall_send_1xcsfb_call_clear (
     
         const cmcall_s_type    *call_ptr
             /* pointer to a call object */
     );


/*===========================================================================

FUNCTION cmcsfbcall_send_1xcsfb_call_end_req

DESCRIPTION
  During LTE to GW or GW to LTE call transfer, map call object info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
 void cmcsfbcall_send_1xcsfb_call_end_req (
     
          cmcall_s_type    *call_ptr
             /* pointer to a call object */
     );



/*===========================================================================

FUNCTION cmcsfbcall_send_orig_req_call_obj_not_allocated

DESCRIPTION
  Send ESR for calls , which CM doesnt holds a call object yet.

DEPENDENCIES
  Call object might not had been allocated
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void cmcsfbcall_send_orig_req_call_obj_not_allocated (
    
        const cm_call_id_type                    call_id
            /* pointer to a call object */
    );



/*===========================================================================

FUNCTION cmcsfbcall_1xcsfb_call_end

DESCRIPTION
  Does call end necessary activities (CM side only, no call end
  cmds sent to LL).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcsfbcall_1xcsfb_call_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);


/*===========================================================================

FUNCTION cmcsfbcall_send_csfb_abort_req

DESCRIPTION
  Sends CSFB abort command to NAS

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void cmcsfbcall_send_csfb_abort_req (
    
        const cm_call_id_type                    call_id
            /* pointer to a call object */
    );


/*===========================================================================

FUNCTION cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req

DESCRIPTION
  Send the ps data available request to MM

DEPENDENCIES

RETURN VALUE
  Return TRUE if the command is successfully send. Otherwise FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req(void);
/*===========================================================================

FUNCTION cmcsfbcall_unforce_csfb_call_end_preferences

DESCRIPTION
  Unforces the CSFB call end preferences.

DEPENDENCIES

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcsfbcall_unforce_csfb_call_end_preferences(void);

/*===========================================================================

FUNCTION cmcall_end_csfb_call_with_end_wait_sub_state

DESCRIPTION
  End the calls in END_WAIT substate.

DEPENDENCIES

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/

extern void cmcsfbcall_end_csfb_calls_with_end_wait_sub_state(void);

/*===========================================================================

FUNCTION cmcsfbcall_unforce_hard_failure_preferences

DESCRIPTION
  Unforces the call end preferences on hard failures.

DEPENDENCIES

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcsfbcall_unforce_hard_failure_preferences(void);

void cmcsfbcall_send_esr_req( uint8 call_id );

#endif /* (FEATURE_LTE_TO_1X)  */



#endif /*CMCSFBCALL_H */

