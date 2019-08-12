/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I P  C A L L

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling calls through
  IP. IP call reports are received and call object is updated. Call
  Origination/end functions are implemented to call, registered
  applications that work using IP.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2005 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipcall.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/14   xs      Add per call hold/resume support
05/29/14   xs      VOLTE call handling during L->D IRAT
04/23/14   sm      Wait for IMS call cleanup during user ending
09/25/13   xs      Hold VOPS indication when moving from VOLTE area to non-VOLTE area
08/14/13   sm      End IMS call in ORIG upon normal call release 
08/08/13   sm      SKT req FR changes
07/03/13   dk      KW error fixed in cmipcall_client_cmd_forward_ll()
06/24/13   xs      VT answered as voice
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
05/08/13   sm      Removed check for ss during mng_state_update
04/08/13   xs      Notify NAS of hold call end after srvcc
01/15/13   xs      Added video share support
01/04/13   th      Support error code CM_CALL_END_UPGRADE_DOWNGRADE_CANCELLED and
                   return error code type to clients.
01/04/13   th      Mapping new end cause CMIPAPP_END_CAUSE_RETRY_AFTER
11/02/12   xs      Fix VOLTE call alllow with modification issue
10/18/12   ak/xs   Fix not sending MNG_CNF issue
08/21/12   xs      Add debug msg
08/21/12   xs      Fix VT call waiting issue
07/31/12   xs      Send MNG_CNF without considering the report order
07/31/12   xs      Fix issue of VT call on hold
05/05/12   xs      Support sending 486 busy here end cause to client
04/25/12   gm      Refactor IPAPP callback interface 
03/27/12   xs      Add call attribute info in connected_ind
03/27/12   xs      VoLTE Remove of user from Conf-call
03/08/12   xs      Added codec info support for VOLTE
02/13/12   xs      VT upgrade/downgrade support
02/07/12   xs      VOLTE silent redial support
01/11/12   xs      Send remote party retrieve event without checking call status
01/06/12   gm      Data synchronization changes for freefloating platform
01/05/12   xs      Added support of remote party hold notification
01/04/12   xs      Add new IMS call end error codes
12/21/11   xs      L->W SRVCC support
11/18/11   xs      Need to set VOLTE call flag for MT call as well
10/10/11   xs      Fix CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB reject issue
10/05/11   xs      Fix 2 sups call issue
09/23/11   xs      Change data block sending mechanism
09/19/11   xs      Fix invalid data block sent in call orig.
08/25/11   xs      Add user data block support for some APIs
08/05/11   xs      VOLTE phase 1 support
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/29/20   jqi     VoIP early media support 
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
12/02/10   gm      Fix to compiler warning- naming conflicts with VC
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/23/10   xs      Superlint critical/high error fixed
08/17/10   gm      Fix to KW Error
06/10/10   ak      Mainlining memory optimization changes.
05/26/10   jqi     EHRPD VOIP silent redial support.
01/26/10   np      Added LTE IP Call sys mode  
01/19/10   aj      Added VOIP support for emergency call
12/23/09   sg      Deallocate the memory after processing call info.
11/05/09   mp      Added support to remove the last party added for VOIP calls
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to 
                   allocate memory dynamically.
09/01/09   sv      Moving from clk API to time API
04/23/09   aj      Prevent referencing deallocated memory
12/03/08   cl      Added support for CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL
10/30/08   jd      Added support for IMS registration 
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
08/20/08   st      Klockwork Fixes
06/12/08   sv      Resolve Klocwork defects
01/25/08   vs      Added 0-send call reject support to VOIP call.
12/07/07   ra      Added support for IMS VCC in HOMER files 
08/07/07   ic      Lint clean up
06/22/07   sk      Added support for ATOM.
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
03/15/07   ka      Adding INFO_IP to CM_CALL_EVENT_MNG_CALLS_CONF.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/09/07   ka      Replacing call to cmipcall_end with cmcall_end to make 
                   sure GW connection id is unreserved.  
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   ka      Removing the check in Hold Call for call id passed to be 
                   in CONV state.
10/03/06   ka      Adding support for IP call on WCDMA
09/18/06   ka      Moving data_block copy to within switch case block. 
                   Primarily done for MT_INVITE but changed others to make
                   it uniform.
08/08/06   sk      Mapped all VoIP errors to CM_CALL_END_IP_FAIL.
                   Removed unnecessary call end event when processing IP call
                   end reports.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type 
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
05/07/06   ka      Adding support for activating/deactivating answer hold on 
                   VoIP calls.
02/17/06   ka      Adding support for videoshare
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
01/05/06   ka      Adding support for multi-call hold/activate.         
12/02/05   ka      Fixing compilation warnings.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/16/05   ka      User used to be alerted at the receipt of 
                   CM_CALL_EVENT_SETUP_IND, this behavior is changed to wait 
                   for seperate ringing indication from network to alert user.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/21/05   sk      Removed all unnecessary checks on digit_mode.
10/08/05   ic      Added Header: field 
08/16/05   pk      Added support for priority Queues for origination 
                   parameters
08/16/05   ka      Added mapping for SYS_SYS_MODE_HDR to CMIPAPP enum.
08/02/05   ic      Print value that caused error in default switch case
04/11/05   ka      Changing return type for function
                   cmipcall_map_sys_mode_to_ip_sys_mode ()
04/06/05   ka      Passing system mode to call origination.
03/28/05   ka      Fixed Lint errors.
03/15/05   ka      Fixing RVCT compiler warning, conversion from CM call type
                   to IP call type
03/14/05   ka      cleaning up IP rpt and adding handling call cmd for IP
                   in forward_ll, reply_ll and cmd_check functions.
02/09/05   ic      Lint cleanup
02/03/05   ka      Lint fixes
02/02/05   dk/ka   Initial release
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_IP_CALL)

#include "cmipappi.h"   /* Includes definitions for VoIP app */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmdbg.h"     /* CM debug macros */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmipcall.h"
#include "cmph.h"      /* Includes phone object */
#include "cmss.h"      /* Includes SS object */
#include "time_svc.h"
#include "cmtaski.h"
#include "cmxcall.h" 
#ifdef CM_GW_SUPPORTED
#include "cmwcall.h"
#endif
#include "cmll.h"
#ifdef CM_DEBUG
#error code not present
#endif
#include "cmsds.h"
#include "cmmsc.h"
#include "cmsimcoord.h"
#include "cmaccessctrl.h"
#include "cmdrvcc.h"

typedef struct
{
  cm_call_sups_type_e_type ss_cmd;
    /* sups command type */
  cmipcall_sups_state_e_type cmipcall_mng_call_ids[CMCALL_MAX_CALL_OBJ];
    /* each call's state regarding the sups service */
}cmipcall_mng_call_s_type;

static cmipcall_mng_call_s_type cmipcall_mng_call_cnf;


/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
                         TUNABLE CALL PARAMETERS
---------------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions - internal:
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmipcall_update_mng_state_per_call

DESCRIPTION
  Update mng_cnf state per call_id

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_update_mng_state_per_call(cm_call_id_type call_id,cmipcall_sups_state_e_type sup_state)
{
  CM_MSG_MED_2("mng state update, call=%d, state=%d", call_id, sup_state);
  cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id] = sup_state;
  return;
}

/*===========================================================================

FUNCTION cmipcall_end_cause_to_cmcall_end_status

DESCRIPTION
  Map app end status to CM call end enum.

DEPENDENCIES
  None

RETURN VALUE
  CM call end status that corresponds to App end status

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_end_e_type cmipcall_end_cause_to_cmcall_end_status
(
       cmipapp_end_cause_e_type  end_cause,
        /* ipcall cause that needs to be mapped to CM end cause */
       cm_call_type_e_type       call_type,

       sys_modem_as_id_e_type    asubs_id

)
{
  cm_call_end_e_type    call_end = CM_CALL_END_REL_NORMAL;
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return CM_CALL_END_NONE;
  }

  switch (end_cause)
  {
    case CMIPAPP_END_CAUSE_NORMAL:             

      call_end = CM_CALL_END_REL_NORMAL;
      break;

    case CMIPAPP_END_CAUSE_TEMPORARY_FAILURE:  

      call_end = CM_CALL_END_IP_RETRY;
      break;

    case CMIPAPP_END_CAUSE_FALLBACK_TO_CS:     
        /* these changes needs to be removed once 
        IMS is ready with the changes of sending CMIPAPP_END_CAUSE_HARD_FAILURE
        instead of fallback_to_cs */
      if(call_type == CM_CALL_TYPE_EMERGENCY &&
        ph_sub_config_ptr->is_alternate_redial_algorithm)
      { 
        call_end = CM_CALL_END_IP_HARD_FAILURE;
      }
      else
      {
        /* for rest of the cases, 
                  voice calls and E911 when alternate redial is OFF
                  use behavior similar to FALLBACK_TO_CS */
        call_end = CM_CALL_END_IP_FAIL;
      }
      break;

    case CMIPAPP_END_CAUSE_HARD_FAILURE:
      if(call_type == CM_CALL_TYPE_EMERGENCY &&
        ph_sub_config_ptr->is_alternate_redial_algorithm)
      { 
        call_end = CM_CALL_END_IP_HARD_FAILURE;
      }
      else
      {
        /* for rest of the cases, 
                  voice calls and E911 when alternate redial is OFF
                  use behavior similar to FALLBACK_TO_CS */
      call_end = CM_CALL_END_IP_FAIL;
      }
      break;

    case CMIPAPP_END_CAUSE_PERMANENT_FAILURE:  

      call_end = CM_CALL_END_IP_END;
      break;

    case CMIPAPP_END_CAUSE_RETRY_AFTER:

      call_end = CM_CALL_END_IP_RETRY_AFTER_FAIL;
      break;
	  
	case CMIPAPP_END_CAUSE_IP_ORIGINATION_FAIL:

      call_end = CM_CALL_END_IP_FAIL;
      break;

    case CMIPAPP_END_CAUSE_BUSY_HERE:

      call_end = CM_CALL_END_SIP_486_BUSY_HERE;  
	  break;

	case CMIPAPP_END_CAUSE_SERVER_INTERNAL_ERROR:
	case CMIPAPP_END_CAUSE_REQUEST_TIMEOUT:

	  call_end = CM_CALL_END_NETWORK_NO_RESP_TIME_OUT; 
	  break;

	case CMIPAPP_END_CAUSE_NOT_FOUND:

      call_end = CM_CALL_END_INVALID_REMOTE_URI;
	  break;

	case CMIPAPP_END_CAUSE_DECLINE:

      call_end = CM_CALL_END_IP_USER_REJECT;
	  break;

    case CMIPAPP_END_CAUSE_TEMPORARILY_UNAVAILABLE:
      call_end = CM_CALL_END_IP_LOW_BATTERY;
	  break;

    case CMIPAPP_END_CAUSE_SRVCC_LTE_TO_CS:

      call_end = CM_CALL_END_IP_UNSUPPORTED_USER_TERMINAL;
	  break;

    case CMIPAPP_END_CAUSE_SERVER_UNAVAILABLE:  

      call_end = CM_CALL_END_NO_NETWORK_RESP;
	  break;
	case CMIPAPP_END_CAUSE_FORBIDDEN:

	  call_end = CM_CALL_END_SIP_403_FORBIDDEN;    
	  break;
	  
	case CMIPAPP_END_CAUSE_ANSWERED_ELSEWHERE: 
	  call_end = CM_CALL_END_SIP_200_ANSWERED_ELSEWHERE;    
	  break;
    case CMIPAPP_END_CAUSE_BUSY_EVERYWHERE:
		 call_end = CM_CALL_END_SIP_600_BUSY_EVERYWHERE;    
	  break; 
  
   case CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL:
   	  call_end = CM_CALL_END_IP_FAIL;
	  break;

   case CMIPAPP_END_CAUSE_HOLD_RESUME_FAILED:
      call_end = CM_CALL_END_IP_HOLD_RESUME_FAILED;
      break;

   case CMIPAPP_END_CAUSE_HOLD_RESUME_CANCELED:
      call_end = CM_CALL_END_IP_HOLD_RESUME_CANCELED;
      break;

   case CMIPAPP_END_CAUSE_REINVITE_COLLISION:
      call_end = CM_CALL_END_IP_REINVITE_COLLISION;
      break;

    default:
      call_end = CM_CALL_END_IP_FAIL;
      break;
  }

  CM_MSG_HIGH_2("VOIP SR ipapp end cause %d call end %d", end_cause, call_end );

  return call_end;

} /* cmipcall_end_cause_to_cmcall_end_status () */


/*===========================================================================

FUNCTION cmipcall_should_call_redial_over_ip()

DESCRIPTION
  Checks if call will get redialed over IP

DEPENDENCIES
  None

RETURN VALUE
  TRUE if call will be redialed over IP

SIDE EFFECTS
  none

===========================================================================*/

boolean cmipcall_should_call_redial_over_ip
(
  cmcall_s_type   *call_ptr, 
  cmipapp_end_cause_e_type ims_end_cause
)
{
  boolean ret_val = FALSE;

  switch(ims_end_cause)
  {
    case CMIPAPP_END_CAUSE_TEMPORARY_FAILURE:
    case CMIPAPP_END_CAUSE_LOOP_BACK:
      {
        ret_val = TRUE;
      }
      break;
    default:
      {
        if(cmipcall_end_cause_to_cmcall_end_status( 
             ims_end_cause, 
             call_ptr->call_type,call_ptr->asubs_id) == CM_CALL_END_IP_FAIL &&
           cmcall_is_volte_restricted_call(call_ptr))
        {
          ret_val = TRUE;
        }
      }
      break;
  } /* ims_end_cause */

  return ret_val;
} /* cmipcall_should_call_redial_over_ip ()*/


/*===========================================================================

FUNCTION cmcall_end_cause_to_cmipcall_rej_status

DESCRIPTION
  Map CM call end enum to app incoming rej status 

DEPENDENCIES
  None

RETURN VALUE
  APP end status 

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_incom_rej_cause_e_type cmcall_end_cause_to_cmipcall_rej_status
(
       cm_call_end_e_type  end_cause
        /* CM end cause to be mapped to ipcall incoming rej */

)
{
  cmipapp_incom_rej_cause_e_type    incom_rej = CMIPAPP_INCOM_REJ_CAUSE_NONE;

  switch (end_cause)
  {
    case CM_CALL_END_SIP_486_BUSY_HERE:             
      incom_rej = CMIPAPP_INCOM_REJ_CAUSE_BUSY;
      break;

    case CM_CALL_END_IP_USER_REJECT:             
      incom_rej = CMIPAPP_INCOM_REJ_CAUSE_USER_REJECT;
      break;

    case CM_CALL_END_IP_LOW_BATTERY:  
      incom_rej = CMIPAPP_INCOM_REJ_CAUSE_LOW_BATTERY;
      break;
	  
    case CM_CALL_END_BLACKLISTED_CALL_ID:  
      incom_rej = CMIPAPP_INCOM_REJ_CAUSE_BLACKLISTED_CALL_ID;
      break;
    
    default:
      incom_rej = CMIPAPP_INCOM_REJ_CAUSE_UNAVAILABLE;
      break;
    
  }

  CM_MSG_HIGH_2("VOIP SR ipapp rej cause %d client call end %d", incom_rej, 
              end_cause );

  return incom_rej;

} /* cmcall_end_cause_to_cmipcall_rej_status () */


/*===========================================================================

FUNCTION cmipcall_map_client_end_status

DESCRIPTION
  Map app end status to CM call end enum.

DEPENDENCIES
  None

RETURN VALUE
  CM call end status that corresponds to App end status

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_end_e_type cmipcall_map_client_end_status
(
       cmipapp_client_end_cause_e_type  end_cause
        /* ipcall cause that needs to be mapped to CM end cause */

)
{
  cm_call_end_e_type    call_end = CM_CALL_END_NONE;

  call_end = (cm_call_end_e_type)end_cause;
  if(!INRANGE(call_end,CM_CALL_END_NONE, (CM_CALL_END_MAX - 1)))
  {
    call_end = CM_CALL_END_NONE;
  }

  CM_MSG_HIGH_2("VOIP client end cause %d, call_end %d", end_cause, call_end );

  return call_end;

} /* cmipcall_end_cause_to_cmcall_end_status () */


/*===========================================================================

FUNCTION cmipcall_map_modify_cause_code

DESCRIPTION
  Map IP APP modify cause code to CM Modify cause code.

DEPENDENCIES
  None

RETURN VALUE
  CM modify cause code

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_modify_cause_e_type cmipcall_map_modify_cause_code
(
  cmipapp_client_modify_cause_e_type  modify_cause_code
  /* ipcall cause that needs to be mapped to CM modify cause */
)
{
  cm_call_modify_cause_e_type    cm_modify_cause_code = CM_CALL_MODIFY_CAUSE_NONE;

  cm_modify_cause_code = (cm_call_modify_cause_e_type) modify_cause_code;
  if(!INRANGE(cm_modify_cause_code,CM_CALL_MODIFY_CAUSE_NONE, (CM_CALL_MODIFY_CAUSE_MAX - 1)))
  {
    cm_modify_cause_code = CM_CALL_MODIFY_CAUSE_NONE;
  }

  CM_MSG_HIGH_2("VOIP client modify_cause_code %d, cm_modify_cause_code %d", 
    modify_cause_code, cm_modify_cause_code);

  return cm_modify_cause_code;
} /* cmipcall_map_modify_cause_code () */


/*===========================================================================

FUNCTION cmipcall_map_calltype_to_cm_calltype

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  None.

RETURN VALUE
  CM call type that corresponds to app call type

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_type_e_type cmipcall_map_calltype_to_cm_calltype(

    cmipapp_call_type_e_type      call_type
        /* call type */
)
{
  switch (call_type)
  {
    case CMIPAPP_CALL_TYPE_VOICE:   return CM_CALL_TYPE_VOICE;

    case CMIPAPP_CALL_TYPE_VT:      return CM_CALL_TYPE_VT;

    case CMIPAPP_CALL_TYPE_VS:      return CM_CALL_TYPE_VS;

    case CMIPAPP_CALL_TYPE_SMS:     return CM_CALL_TYPE_SMS;

    case CMIPAPP_CALL_TYPE_EMERGENCY:     return CM_CALL_TYPE_EMERGENCY;

    default:
       CM_ERR_1( "CMIPAPP call type %d not mapped to CM call type !!!",call_type);
       CM_ASSERT(FALSE);
       return CM_CALL_TYPE_MAX;
  }

} /* cmipcall_map_calltype_to_cm_calltype () */


/*===========================================================================

FUNCTION cmipcall_map_cm_calltype_to_ip_calltype

DESCRIPTION
  Maps CM call type to CM app call type

DEPENDENCIES
  None.

RETURN VALUE
  app call type that corresponds to CM call type

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_call_type_e_type cmipcall_map_cm_calltype_to_ip_calltype(

    cm_call_type_e_type      call_type
        /* call type */
)
{
  switch (call_type)
  {
    case CM_CALL_TYPE_VOICE:       return CMIPAPP_CALL_TYPE_VOICE;

    case CM_CALL_TYPE_EMERGENCY:   return CMIPAPP_CALL_TYPE_EMERGENCY;

    case CM_CALL_TYPE_VT:          return CMIPAPP_CALL_TYPE_VT;

    case CM_CALL_TYPE_VS:          return CMIPAPP_CALL_TYPE_VS;

    case CM_CALL_TYPE_SMS:         return CMIPAPP_CALL_TYPE_SMS;

    default:
       CM_ERR_1( "CM call type %d not mapped to CMIPAPP call type !!!", call_type);
       CM_ASSERT(FALSE);
       return CMIPAPP_CALL_TYPE_MAX; /*lint !e641 */   
  }

} /* cmipcall_map_cm_calltype_to_ip_calltype () */


/*===========================================================================

FUNCTION cmipcall_map_sys_mode_to_ip_sys_mode

DESCRIPTION
  Maps system mode to IP system mode

DEPENDENCIES
  None.

RETURN VALUE
  sys mode that corresponds to ip system mode

SIDE EFFECTS
  none

===========================================================================*/
static cmipapp_sys_mode_e_type cmipcall_map_sys_mode_to_ip_sys_mode(

    sys_sys_mode_e_type  sys_mode
        /* call type */
)
{
  CM_ASSERT (BETWEEN(sys_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  switch (sys_mode)
  {
    case SYS_SYS_MODE_NO_SRV:  return CMIPAPP_SYS_MODE_NO_SRV;

    case SYS_SYS_MODE_CDMA:    return CMIPAPP_SYS_MODE_CDMA;

    case SYS_SYS_MODE_GSM:     return CMIPAPP_SYS_MODE_GSM;

    case SYS_SYS_MODE_WCDMA:   return CMIPAPP_SYS_MODE_WCDMA;

    case SYS_SYS_MODE_HDR:     return CMIPAPP_SYS_MODE_HDR;

    case SYS_SYS_MODE_LTE:     return CMIPAPP_SYS_MODE_LTE;

    case SYS_SYS_MODE_WLAN:    return CMIPAPP_SYS_MODE_WLAN;

    default:
       CM_ERR_1( "sys mode %d not supported in IP call !!!", sys_mode);
       CM_ASSERT(FALSE);
       return CMIPAPP_SYS_MODE_MAX;
  }

} /* cmipcall_map_sys_mode_to_ip_sys_mode () */


/*===========================================================================

FUNCTION cmipcall_reoriginate_call_as_emergency

DESCRIPTION
  Change ordinary VOLTE call into an emergency call
 
DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_reoriginate_call_as_emergency
(
  cmcall_s_type           *call_ptr,
    /* call object
    */
  boolean fallback_to_cs
  /* if IMS has already send fallback to CS for this */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* update call object - call type , srv type*/
  call_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
  call_ptr->srv_type = CM_SRV_TYPE_AUTOMATIC;

  /* update the volte indicator*/
  call_ptr->is_volte = FALSE;
  call_ptr->is_vowlan = FALSE;
  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_NONE;

  /* Reset the silent redial component.
  */
  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp)
  {
    dword curr_wlan_fallback_timer = call_ptr->sr.wlan_fallback_timer;
    cm_emerg_call_attempt_state_e_type curr_emerg_call_attempt_state = call_ptr->sr.emerg_call_attempt_state;
  sr_reset( &call_ptr->sr );
    call_ptr->sr.wlan_fallback_timer = curr_wlan_fallback_timer;
    call_ptr->sr.emerg_call_attempt_state = curr_emerg_call_attempt_state;
  }
  else
  {
    sr_reset(&call_ptr->sr);
  }

  /* update favored mode/hybr prefs for call*/
  cmcall_set_favored_mode_pref_for_origination ( CM_SS_MAX, // this param is not used for non DSDS
                                                 call_ptr->call_type,
                                                 call_ptr->srv_type ,
                                                 TRUE, //drs bit - not used
                                                 #ifdef FEATURE_IS2000
                                                 call_ptr->cmcall_mode_info.info.cdma_info.srv_opt,//not used,
                                                 #else
                                                 TRUE,
                                                 #endif
                                                 &call_ptr->favored_mode_pref,
                                                 &call_ptr->favored_hybr_pref );
  /* Find the SS to use for call origination */
  call_ptr->ss = cmcall_determine_ss_for_origination(
                                                 call_ptr->favored_mode_pref,
                                                 call_ptr->favored_hybr_pref,
                                                 CM_CC_MODE_PREF_ANY,
                                                 call_ptr );
  CM_MSG_HIGH_3( "Using ss=%d for emerg fav mode_pref %d,fav hybr_pref =%d",
                                                       call_ptr->ss,
                                                       call_ptr->favored_mode_pref,
                                                       call_ptr->favored_hybr_pref );
  /*
  ** Force top pref is set to TRUE, if the current call is possible over IP
  ** If there is no ipapp (on HDR )for voice origination, remove HDR
  ** component from favored mode preference.
  ** Both these steps are not needed for emergency call over EHRPD case.
  ** Hybr pref will not be OFF during 911 call only for emerg call over ehrpd case.
  */
  if(! ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
         call_ptr->favored_hybr_pref != CM_HYBR_PREF_NONE)
    )
  {
    cmcall_process_ipcall_params (call_ptr);
  }
  /* Run domain selection */
  #if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)  || \
      defined (FEATURE_DOMAIN_SELECTION)
  call_ptr->csfb_type = cmcall_process_dom_sel_params(call_ptr);
  #endif
  
  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->wifi_e911_support && !fallback_to_cs)
  {
    cmcall_process_custom_wifi_settings(call_ptr);
  }
  #endif

  /* update orig mode */
  (void)cmcall_process_orig_mode(CM_CC_MODE_PREF_ANY,call_ptr);
  
  #if defined (FEATURE_CM_LTE) && defined(FEATURE_IP_CALL)
  /* End existing VOLTE call if any */
  cmcall_end_concurrent_volte_call(call_ptr->asubs_id);
  #endif

   /* Update priority Q and send call orig event */
  cmcall_orig_start (call_ptr);
  return;

} /* cmipcall_reoriginate_call_as_emergency() */

/*===========================================================================

FUNCTION cmipcall_sups_cmd_check

DESCRIPTION
  Check the supplementary service command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no errors found or an error as appropriate

SIDE EFFECTS
  None

==========================================================================*/
static cm_call_cmd_err_e_type cmipcall_sups_cmd_check(

     cm_call_cmd_s_type *call_cmd_ptr
       /* SUPS command to be checked */
)
{

  cmss_s_type                *ss_ptr              = cmss_ptr();
    /* Pointer to the serving system object */

  cmcall_s_type              *call_ptr            = NULL;
    /* Pointer to call object */  

  cm_call_cmd_err_e_type      cmd_err             = CM_CALL_CMD_ERR_NOERR;
    /* Call command error to return */

  cm_call_cmd_info_s_type    *cmd_info_ptr        = CMD_INFO_PTR( call_cmd_ptr );
      /* Pointer to call cmd info */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr       != NULL );
  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( cmd_info_ptr != NULL );

 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We have to have a call up for all SUPS commands  
  **
  ** - This also checks for Phone being online, since call obj
  **   does not get alloc without phone being online.     
  **
  */
  call_ptr = cmcall_ptr( cmd_info_ptr->call_id );

  if(call_ptr == NULL)
  {
    return CM_CALL_CMD_ERR_CALL_ID_P;
  }  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /* Phone has to have service on the ss pointed by call OR
  ** It is a WLAN call and no service is req on MAIN
  */
  if(!call_ptr->is_vowlan && !( ( (call_ptr->ss == CM_SS_MAIN || call_ptr->ss == CM_SS_MAX) && 
                        sys_srv_status_is_srv (ss_ptr->main_srv_status))
         ||
         ( (call_ptr->ss == CM_SS_HDR) && 
                        sys_srv_status_is_srv (ss_ptr->hybr_1_srv_status))
         ||
         ( (call_ptr->ss == CM_SS_HYBR_2) && 
                        sys_srv_status_is_srv (ss_ptr->hybr_gw_srv_status))
         ||
         ( (call_ptr->ss == CM_SS_HYBR_3) && 
                        sys_srv_status_is_srv (ss_ptr->hybr_3_gw_srv_status))
       )
    )
  {
    CM_MSG_HIGH_1 ("No service on ss %d", call_ptr->ss);

    return (CM_CALL_CMD_ERR_SRV_STATE_S );
  }   

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(cmd_info_ptr->sups_cmd_type)
  {    

    /* Hold call is used either to 
    ** - Hold the current active call
    ** - Hold current active call and accept incoming call.    
    **  call id passed for HOLD_CALL can be in INCOM or CONV state.
	**  Reject hold operation when 1xSRVCC in progress
    */
    case CM_CALL_SUPS_TYPE_HOLD_CALL:
	
	  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)

	   if( call_ptr->srvcc_prog_state == CM_SRVCC_STATE_HO_EUTRA_RSP_SENT || 
		  call_ptr->srvcc_prog_state == CM_SRVCC_STATE_HO_EUTRA_REQ_RCVD )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
       #endif
      #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
      {
        CM_MSG_HIGH_0("DRVCC: drvcc is in progress, reject hold cmd");
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      #endif
		
      break;

    /* Operation of multiparty requires both calls
    ** to be connected. State of either call will
    ** be in conversation.
    */    
    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:
      if( call_ptr->call_state != CM_CALL_STATE_CONV )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;
    
    /* ECT can be performed only when a call is 
    ** in INCOM/CONV state.
    */
    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:
      if( !( (call_ptr->call_state == CM_CALL_STATE_CONV) ||
             (call_ptr->call_state == CM_CALL_STATE_INCOM)
           )
        )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;


    /* Allow Act Answer hold only when call is in incoming state.
    */
    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
      if ( ! ( ( call_ptr->call_state == CM_CALL_STATE_INCOM) &&
               (call_ptr->call_subst.incom == 
                                      CM_CALL_INCOM_SUBST_ALERTING)
             )
         )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;


    /* Allow De act Answer hold only when call is in CONV state.
    */
    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:
      if( call_ptr->call_state != CM_CALL_STATE_CONV )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

  /* Allow UDUB (0-Send) only when call is in INCOM state */
  case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:
      if( call_ptr->call_state != CM_CALL_STATE_INCOM &&
         !(call_ptr->call_state == CM_CALL_STATE_CONV &&
       call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD))
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

  /* Allow only flash to go through if IP Call is in conversation */
  case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
      if (call_ptr->call_state != CM_CALL_STATE_CONV)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break; 

  /* Allow active Call Release only when Call is in conversation state */
  case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL:
      if (call_ptr->call_state != CM_CALL_STATE_CONV)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      break;

  case CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE:
     if (call_ptr->call_state != CM_CALL_STATE_CONV)
     {
       cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
     }
     break;
	 
  case CM_CALL_SUPS_TYPE_ADD_PARTICIPANT:
     if (call_ptr->call_state != CM_CALL_STATE_CONV)
     {
       cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
     }
     break;	 

  case CM_CALL_SUPS_TYPE_CALL_DEFLECTION:
    if (call_ptr->call_state != CM_CALL_STATE_INCOM)
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
    }
    break;
	
  case CM_CALL_SUPS_TYPE_HOLD_PER_CALL:
    if(!( call_ptr->call_state == CM_CALL_STATE_CONV && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE))
    {
     cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
    }
    break;

  case CM_CALL_SUPS_TYPE_RESUME_PER_CALL:
    if( !(call_ptr->call_state == CM_CALL_STATE_CONV && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD))
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
    }
    break;
	  
  case CM_CALL_SUPS_TYPE_SWITCH_SPEECH_CODEC:
  case CM_CALL_SUPS_TYPE_IP_EXPLICIT_CALL_TRANSFER:
		/* no error checks required as of now */
	  break;		

	  
    default:
      cmd_err = CM_CALL_CMD_ERR_CALL_SUPS_P;
      break;
  }

  return cmd_err;

} /* cmipcall_sups_cmd_check () */


/*===========================================================================

FUNCTION cmipcall_process_hold_call

DESCRIPTION
  Sends request to answer a waiting call or to put a conencted call 
  on hold/active sub-state.

  Sequence to be followed
  -----------------------
  1> First put all active calls on hold.
  2> Make held calls active or accept an incoming call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_hold_call
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */
  cm_call_sups_type_e_type       sups_cmd
)
{
  cm_iterator_type       call_itr;
    /* iterates through call call objects in CM 
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmcall_s_type           *call_ptr_tmp = NULL;
  cmcall_s_type           *call_ptr_tmp1 = NULL;
  
  cm_ip_call_sip_reject_code_s_type      sip_reject_code;
    /* SIP code sent for rejecting incoming call*/  


  CM_ASSERT (call_ptr != NULL);

  memset( &sip_reject_code, 0, sizeof( sip_reject_code ) );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sups_cmd == CM_CALL_SUPS_TYPE_HOLD_CALL)
  {
    cmcall_obj_iterator_init(&call_itr);

    call_ptr_tmp = cmcall_obj_get_next(&call_itr);  
    
    /* Active calls should be put on hold first
    */
    while(call_ptr_tmp != NULL)
    {
    
      if (call_ptr_tmp->call_state == CM_CALL_STATE_CONV &&
          call_ptr_tmp->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE &&
           (call_ptr_tmp->call_type == CM_CALL_TYPE_VOICE || call_ptr_tmp->call_type == CM_CALL_TYPE_SUPS || 
            call_ptr_tmp->call_type == CM_CALL_TYPE_VT || call_ptr_tmp->call_type == CM_CALL_TYPE_VS ||
            call_ptr_tmp->call_type == CM_CALL_TYPE_EMERGENCY)
         )
      {           
        if (!cmipapp_send_hold_cmd(call_ptr_tmp->ipapp_id, call_ptr_tmp->call_id, call_ptr_tmp->asubs_id))
        {
          CM_ERR_1 ("Request to put call %d on hold failed",
                                                    call_ptr_tmp->call_id);
          return;
        }
        cmipcall_mng_call_cnf.ss_cmd = sups_cmd;
        cmipcall_update_mng_state_per_call(call_ptr_tmp->call_id, CMIPCALL_SUPS_ST_WAIT_HOLD_CNF);
           
      } /* if ((call_ptr->call_state == CM_CALL_STATE_CONV) &&
          (call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE))
        */
      call_ptr_tmp = cmcall_obj_get_next(&call_itr);

    } /* while(call_ptr != NULL) */
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  /* Make held calls active or accept an incoming call
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr_tmp1 = cmcall_obj_get_next(&call_itr);  
  

  while(call_ptr_tmp1 != NULL)
  {
    switch (call_ptr_tmp1->call_state)
    {

      case CM_CALL_STATE_INCOM:

        CM_MSG_HIGH_3 ("Answering call %d, is_call_modified %d, call_type_mod_to %d", call_ptr_tmp1->call_id,
            call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.is_call_modified,
            call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.call_type_mod_to);

        if((call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.is_call_modified) && 
            (call_ptr_tmp1->call_id == call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.waiting_call_id))
        {
          call_ptr_tmp1->call_type = call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.call_type_mod_to;
          call_ptr_tmp1->cmcall_mode_info.info.ip_info.ipcall_attrib_info.audio_attrib = call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.audio_attrib;
          call_ptr_tmp1->cmcall_mode_info.info.ip_info.ipcall_attrib_info.video_attrib = call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param.video_attrib;
        }

        /* Accept incoming call with TRUE 
        */
        sip_reject_code.is_reject_code_valid = FALSE;
        cmipcall_answer (call_ptr_tmp1, TRUE, CMIPAPP_INCOM_REJ_CAUSE_NONE,sip_reject_code);
        cmipcall_mng_call_cnf.ss_cmd = sups_cmd;
        cmipcall_update_mng_state_per_call(call_ptr_tmp1->call_id, CMIPCALL_SUPS_ST_WAIT_ANS_CNF);
        return;


      case CM_CALL_STATE_CONV:
        
        /* If Call is currently on hold, invoke active callback
        */
        if (call_ptr_tmp1->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD)
        {       
          call_ptr_tmp = call_ptr_tmp1;
        }
        
        break;

      case CM_CALL_STATE_NONE:
      case CM_CALL_STATE_MAX:
        CM_ERR_2("Invalid call state %d for call id %d", 
                 call_ptr_tmp1->call_state, 
                 call_ptr_tmp1->call_id);
        return;

      case CM_CALL_STATE_IDLE:
      case CM_CALL_STATE_ORIG:
      case CM_CALL_STATE_CC_IN_PROGRESS:
      case CM_CALL_STATE_RECALL_RSP_PEND:
      default:
        break;

    } /* switch (call_ptr->call_state) */

    call_ptr_tmp1 = cmcall_obj_get_next(&call_itr);
   
  } /* while(call_ptr != NULL) */

  if (call_ptr_tmp != NULL)
  {
    CM_MSG_HIGH_1 ("Requesting call %d to be made active", 
    			 call_ptr_tmp->call_id);
    
    if (!cmipapp_send_resume_cmd(call_ptr_tmp->ipapp_id, call_ptr_tmp->call_id, call_ptr_tmp->asubs_id))
    {
      CM_ERR_0 ("Request to make call active failed");
      return;
    } 		
    cmipcall_mng_call_cnf.ss_cmd = sups_cmd;
    cmipcall_update_mng_state_per_call(call_ptr_tmp->call_id, CMIPCALL_SUPS_ST_WAIT_RETRIEVE_CNF);
  }

} /* cmipcall_process_hold_call () */


/*===========================================================================

FUNCTION cmipcall_process_act_answer_hold

DESCRIPTION
  Sends a request for activating answer hold.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_act_answer_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  if (!cmipapp_send_activate_answer_hold_cmd(call_ptr->ipapp_id,
                                                          call_ptr->call_id,
                                                          call_ptr->asubs_id))
  {
    CM_ERR_1 ("Request to put call on answer-hold failed, call_id %d",
                                                    call_ptr->call_id);
    return;
  }

} /* cmipcall_process_act_answer_hold () */



/*===========================================================================

FUNCTION cmipcall_process_deact_answer_hold

DESCRIPTION
  Sends a request for de-activating answer hold.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_deact_answer_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  if (!cmipapp_send_deactivate_answer_hold_cmd(call_ptr->ipapp_id,
                                                          call_ptr->call_id,
                                                          call_ptr->asubs_id))
  {
    CM_ERR_1 ("Request to deact answer-hold failed, call_id %d",
             call_ptr->call_id);
    return;
  }
          
} /* cmipcall_process_act_answer_hold () */



/*===========================================================================

FUNCTION cmipcall_process_ect

DESCRIPTION
  Sends a request to perform explicit call transfer on the calls.  

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_ect 
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  if (!cmipapp_send_ect_on_calls_cmd(call_ptr->ipapp_id, call_ptr->asubs_id))
  {
    CM_ERR_1 ("Request to perform ECT failed, call_id %d",
                                                    call_ptr->call_id);
    return;
  }
          
} /* cmipcall_process_ect () */

cmipapp_ip_expicit_call_transfer_e_type cmipcall_map_ect_type_to_ip_ect_type (
  cm_call_ip_expicit_call_transfer_e_type ect_type
  )
{
  cmipapp_ip_expicit_call_transfer_e_type ip_ect_type;
  switch(ect_type)
  {
  case CM_EXPLICIT_CALL_TRANSFER_TYPE_BLIND:
    ip_ect_type = CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_BLIND;
    break;
  case CM_EXPLICIT_CALL_TRANSFER_TYPE_ASSURED:
    ip_ect_type = CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_ASSURED;
    break;
  case CM_EXPLICIT_CALL_TRANSFER_TYPE_CONSULTATIVE:
    ip_ect_type = CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_CONSULTATIVE;
    break;
  default:
    ip_ect_type = CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_NONE;
    break;
  }
  return ip_ect_type;
}

/*===========================================================================

FUNCTION cmipcall_process_ip_explicit_call_transfer

DESCRIPTION
  Sends a request to perform explicit call transfer for IP Calls.  

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_ip_explicit_call_transfer
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */
  const cm_call_cmd_info_s_type    *cmd_info_ptr
)
{
  CM_ASSERT (call_ptr != NULL);
  CM_ASSERT (cmd_info_ptr != NULL);

  CM_MSG_HIGH_1 ("Requesting ECT for Call id %d", call_ptr->call_id);
  
  if (!cmipapp_send_ip_explict_call_transfer_on_calls_cmd(
    call_ptr->ipapp_id, 
    call_ptr->asubs_id,
    call_ptr->call_id,
    cmipcall_map_ect_type_to_ip_ect_type(cmd_info_ptr->ip_call.ip_sups_ect_params.ect_type),
    cmd_info_ptr->ip_call.ip_sups_ect_params.transfer_target_call_num,
    cmd_info_ptr->ip_call.ip_sups_ect_params.transfer_target_call_id))
  {
    CM_ERR_1 ("Request to perform ECT failed, call_id %d",
                                                    call_ptr->call_id);
    return;
  }
          
} /* cmipcall_process_ect () */

/*===========================================================================

FUNCTION cmipcall_process_mpty

DESCRIPTION
  Sends a request to perform multiparty on given calls.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_process_mpty
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  if (!cmipapp_send_mpty_cmd( call_ptr->ipapp_id, call_ptr->asubs_id))
  {
    CM_ERR_1 ("Request to perform MPTY failed, call_id %d",
             call_ptr->call_id);
    return;
  }
          
} /* cmipcall_process_mpty () */

/*===========================================================================

FUNCTION cmipcall_proc_sups_cmd_type

DESCRIPTION
  Branches sups request to hold, activ answer-hold or deact answer-hold
  functions. 

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmipcall_proc_sups_cmd_type
(
  const cm_call_cmd_info_s_type    *cmd_info_ptr,
    /* Pointer to cmd
    */

  const cmcall_s_type              *call_ptr
    /* Pointer to a call object
    */
)
{

  cmipapp_incom_rej_cause_e_type   incoming_rej = CMIPAPP_INCOM_REJ_CAUSE_NONE;
  
  if( call_ptr == NULL )
  {
    CM_ERR_0 ("parameter call_ptr = NULL");
    return;
  }
  
  switch (cmd_info_ptr->sups_cmd_type)
  {
    case CM_CALL_SUPS_TYPE_HOLD_CALL:                 

      /* Call is put into hold or active depending on its current
      ** active status.
      **
      ** SUPS_TYPE_HOLD_CALL toggles a call between held and active
      ** states.
      */
      cmipcall_process_hold_call (call_ptr, CM_CALL_SUPS_TYPE_HOLD_CALL);

      break;

    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:

      /* Call is being put on answer hold.
      */
      cmipcall_process_act_answer_hold (call_ptr);

      break;


    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:

      /* Call is being pulled out of answer hold.
      */
      cmipcall_process_deact_answer_hold (call_ptr);

      break;

    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:                 

      /* Explicit call transfer request being processed
      */
      cmipcall_process_ect (call_ptr);

      break;


    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:                 

      /* Request being sent to put call in MPTY
      */
      cmipcall_process_mpty (call_ptr);

      break;

    case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:

      /* Answer the call and send reject.
      */
      incoming_rej = cmcall_end_cause_to_cmipcall_rej_status(cmd_info_ptr->ip_call.call_reject_reason);
      
      if(call_ptr->call_state == CM_CALL_STATE_INCOM)
      {
        cmipcall_answer (call_ptr, FALSE, incoming_rej,cmd_info_ptr->ip_call.sip_reject_code);

        cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB;
        cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_ANS_CNF);
      }
      /* Release the held call */
      else if (call_ptr->call_state == CM_CALL_STATE_CONV &&
               call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD)
      {
        cmipcall_send_end (call_ptr);

        cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB;
        cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_END_CNF);
      }
      else /*Adding this F3 to catch more call states */
      {
        CM_MSG_MED_1("ERR: Not inform IMS in call_state %d",call_ptr->call_state);
      }

      break;

    case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
      /* Answer the Call Waiting Indication/Delete last party added to the conference
      */
      cmipcall_simple_flash(call_ptr);
      break;

    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL: 

      /* Holded call should not be ended, End call only if active */
      if (call_ptr->call_state      == CM_CALL_STATE_CONV &&
          call_ptr->call_subst.conv != CM_CALL_CONV_CALL_ON_HOLD)
      {
        cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL;
        cmipcall_send_end (call_ptr);
        cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_END_CNF);
      }

      /* Do retreive only. If there is call on hold, automatically retrieve it.
      */
      cmipcall_process_hold_call (call_ptr, CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL);
      break;

    case CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE:
      cmipcall_remove_last_party(call_ptr);
      break;
	  
	case CM_CALL_SUPS_TYPE_ADD_PARTICIPANT:
	  cmipcall_add_new_party(call_ptr);
	  break;  

    case CM_CALL_SUPS_TYPE_CALL_DEFLECTION:
	  cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_CALL_DEFLECTION;
      cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_CALL_DEFLECTION_CNF);
	  
      cmipcall_process_call_deflection(call_ptr, cmd_info_ptr);
      break;
	  
	case CM_CALL_SUPS_TYPE_HOLD_PER_CALL:
      cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_HOLD_PER_CALL;
      cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_HOLD_CNF);
  
      cmipapp_send_hold_cmd(call_ptr->ipapp_id, call_ptr->call_id, call_ptr->asubs_id);
      break;
	  
    case CM_CALL_SUPS_TYPE_RESUME_PER_CALL:
      cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_RESUME_PER_CALL;
      cmipcall_update_mng_state_per_call(call_ptr->call_id, CMIPCALL_SUPS_ST_WAIT_RETRIEVE_CNF);
  
      cmipapp_send_resume_cmd(call_ptr->ipapp_id, call_ptr->call_id, call_ptr->asubs_id);
      break;

	case CM_CALL_SUPS_TYPE_SWITCH_SPEECH_CODEC:
	  {
	  	if (!cmipapp_send_switch_speech_codec_cmd(
                                call_ptr->ipapp_id,
                                call_ptr->call_id,
                                cmd_info_ptr->ip_call.ip_sups_param.switch_to_codec,
                                call_ptr->asubs_id))
		{
		  CM_ERR_1 ("Sending switch speech codec to app failed, call_id %d", 
		         call_ptr->call_id);
		  return;
		}
	  }
	  break;

    case CM_CALL_SUPS_TYPE_IP_EXPLICIT_CALL_TRANSFER:
    {
      /* IP Explicit call transfer request being processed */
      cmipcall_process_ip_explicit_call_transfer (call_ptr, cmd_info_ptr);
    }
    break;

    default:
      CM_ERR_1 ("Unknown command type received = %d", 
                  cmd_info_ptr->sups_cmd_type);
      break;
  }   

} /* cmipcall_proc_sups_cmd_type () */

/**--------------------------------------------------------------------------
** Functions - external:
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmipcall_reset_mng_call_cnf

DESCRIPTION
  reset MNG call cnf structure to default values

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmipcall_reset_mng_call_cnf(void)
{
  CM_MSG_MED_0("reset_mng_cnf");
  memset(&cmipcall_mng_call_cnf,0,sizeof(cmipcall_mng_call_s_type));
  cmipcall_mng_call_cnf.ss_cmd = CM_CALL_SUPS_TYPE_NONE;
}

/*===========================================================================

FUNCTION cmipcall_reset

DESCRIPTION
  Reset IP call fields to default values reflecting
  a IP call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_ptr->ipapp_id          = CMIPAPP_ID_INVALID;

  call_ptr->ipapp_data_block  = 0;

  call_ptr->is_volte          = FALSE;

  call_ptr->end_cause_to_client = CM_CALL_END_NONE;

  call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type = CM_CALL_TYPE_NONE;
  
  call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason = NULL;

  call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.vs_ver = CM_IP_VS_RCSE;
  REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
  memset(&call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info,0, sizeof(cm_ip_call_attrib_info_s_type));
  memset(&call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib,0, sizeof(cm_ip_call_attrib_info_s_type));
  
   #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  call_ptr->is_srvcc_call                    = FALSE;
  call_ptr->is_int_ims                       = TRUE;
  call_ptr->mid_call_supported               = FALSE;
  call_ptr ->ims_call_type					 = CM_CALL_TYPE_NONE;
  call_ptr->parent_call_id                   = CM_CALL_ID_INVALID;
  call_ptr->alert_media_type                 = CM_SRVCC_ALERTING_NONE;
  call_ptr->is_conf_call					 = FALSE;
  call_ptr->srvcc_prog_state 				 = CM_SRVCC_STATE_NONE;
    
  if(call_ptr->srvcc_caller_info.caller_name != NULL)
  {
	cm_mem_free(call_ptr->srvcc_caller_info.caller_name);
  }

  call_ptr->srvcc_caller_info.caller_name     = NULL;
  call_ptr->srvcc_caller_info.caller_len      = 0;
  call_ptr->srvcc_caller_info.caller_name_pi   = CM_PRESENTATION_ALLOWED;   
  call_ptr->orm_sent_status = FALSE;
  call_ptr->drvcc_parent_call_id = CM_CALL_ID_INVALID;
  #endif

}

/*===========================================================================

FUNCTION cmipcall_send_mng_cnf

DESCRIPTION
  send mng_cnf to client

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmipcall_send_mng_cnf(boolean call_ss_status, cm_ipapp_cause_e_type failure_cause)
{

  cmipapp_ind_s_type           ipapp_ind;
  uint8                        i;
  sys_modem_as_id_e_type       as_id = cmph_get_sub_with_lte_cap();

  if(cmipcall_mng_call_cnf.ss_cmd == CM_CALL_SUPS_TYPE_NONE)
  {
    return;
  }

  /* if there is no others calls waiting for cnf, send mng_cnf with ss_status. otherwise,
  ** continue waiting for cnf 
  */
  for(i=0;i<CMCALL_MAX_CALL_OBJ;i++)
  {
    if(cmipcall_mng_call_cnf.cmipcall_mng_call_ids[i] != CMIPCALL_SUPS_ST_NONE)
    {
      CM_MSG_HIGH_3("Not send mng cnf, cmd=%d, call=%d, state=%d",
                     cmipcall_mng_call_cnf.ss_cmd,i,
                     cmipcall_mng_call_cnf.cmipcall_mng_call_ids[i]);
      return;
    }
  }

  /* send MNG_CNF to client */
  memset(&ipapp_ind,
         0,
         sizeof(cmipapp_ind_s_type));

  CM_MSG_HIGH_3("CM->CM: CM_IP_CALL_IND_SUPS_CONF, call_success %d,sups_type %d,lte as_id %d",
                 call_ss_status, 
                 ipapp_ind.ind.sups_conf.sups_type, 
                 as_id);
  
  ipapp_ind.ind.sups_conf.call_success = call_ss_status; 
  ipapp_ind.ind.sups_conf.sups_type= cmipcall_mng_call_cnf.ss_cmd;
  ipapp_ind.ind.sups_conf.failure_cause = failure_cause;
  ipapp_ind.hdr.asubs_id = as_id;
  
  cmipapp_rpt_ind (CM_IP_CALL_IND_SUPS_CONF, ipapp_ind, 0);

  cmipcall_reset_mng_call_cnf();
}
/*===========================================================================

FUNCTION cmipcall_mng_state_update

DESCRIPTION
  Update mng_call_cnf states

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

 void cmipcall_mng_state_update(cm_name_type ip_ind_type, boolean ss_status, cm_call_id_type call_id, cm_ipapp_cause_e_type failure_cause)
{

  CM_MSG_MED_6("cmipcall_mng_state_update:ind_type %d, status %d, call_id %d call mng state %d, ss_cmd %d, failure_cause %d",
  				ip_ind_type, ss_status, call_id,
 				cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id],cmipcall_mng_call_cnf.ss_cmd, failure_cause);
   
  switch(ip_ind_type)
  {
  case CM_IP_CALL_IND_CALL_HOLD:

    /* check if call id is in valid state to receive call hold cnf */
    if(CMIPCALL_SUPS_ST_WAIT_HOLD_CNF == cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id])
    {
      cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
    }
    else
    {
      return;
    }
    break; //CM_IP_CALL_IND_CALL_HOLD

  case CM_IP_CALL_IND_CALL_DEFLECTION:
  
  /* check if call id is in valid state to receive call deflection cnf */
  if(CMIPCALL_SUPS_ST_WAIT_CALL_DEFLECTION_CNF == cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id])
  {
    cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
  }
  else
  {
    return;
  }
  break; //CM_IP_CALL_IND_CALL_DEFLECTION

  case CM_IP_CALL_IND_CONNECTED:
    /* check if call id is in valid state to receive call connected */
    if(CMIPCALL_SUPS_ST_WAIT_ANS_CNF == cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id])
    {
      cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
    }
    else
    {
      return;
    }
    break; //CM_IP_CALL_IND_CONNECTED

  case CM_IP_CALL_IND_CALL_RETRIEVE:
    if(CMIPCALL_SUPS_ST_WAIT_RETRIEVE_CNF == cmipcall_mng_call_cnf.cmipcall_mng_call_ids[call_id])
    {
      cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
    }
    else
    {
      return;
    }
    break; //CM_IP_CALL_IND_CALL_RETRIEVE

  case CM_IP_CALL_IND_CALL_END:
  case CM_IP_CALL_IND_ANSWER_REJECT: // This is specific only to the case when INCOM call is rejected without processing by IMS
    cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
    break; //CM_IP_CALL_IND_CALL_END | CM_IP_CALL_IND_ANSWER_REJECT

  default:
    CM_ERR_1( "wrong ip_ind_type =%d",
                         ip_ind_type);
    break;
  }

  cmipcall_send_mng_cnf(ss_status, failure_cause);
}

/*===========================================================================

FUNCTION cmipcall_rpt_proc

DESCRIPTION
  Process app reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
)
{

  const cmipapp_rpt_s_type  *cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) rpt_ptr;
    /* Pointer to a IP app report */

  cmcall_s_type             *call_ptr = NULL;
    /* Point at call object */

  cm_mm_call_info_s_type    *call_info_ptr = NULL;
    /* Structure to send call event */

  ip_cmcall_type            *ip_ptr = NULL;
    /* type to point to ipcall specific info */

  boolean                    ss_status = FALSE;
    /* supplement service status */

  cmph_s_type                  *ph_ptr         = cmph_ptr();
  
  cmph_sub_config_s_type     *cmph_sub_info = cmph_get_sub_config_ptr(cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmipapp_rpt_ptr != NULL );

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */
  switch( cmipapp_rpt_ptr->hdr.cmd )
  {
    case CM_IP_CALL_IND_MT_INVITE:

      CM_MSG_HIGH_3("IP RXD: MT_INVITE sys_mode=%d, as_id=%d, lte as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.mt_invite.sys_mode,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id,
                   cmph_get_sub_with_lte_cap());
      /*
      ** Call id should be requested by IP app before sending any reports
      ** to CM.
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.mt_invite.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT( call_ptr != NULL );
        CM_ERR_0 ( "ERROR: Unknown call ID in MT invite");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
      

     /*------------------------------------------------------------------*/


      /* Record app that is placing the call
      */
      call_ptr->ipapp_id     = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.app_id;
      

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /* Copy system mode for the call
      */
      call_ptr->sys_mode   = cmipapp_map_sysmode_to_cm_sysmode (
                                cmipapp_rpt_ptr->ip_ind.ind.mt_invite.sys_mode);


      /*------------------------------------------------------------------*/

      /* Determine ss for this call
      */
      call_ptr->asubs_id = cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;
      call_ptr->ss         = cmcall_det_ss_from_sys_mode (call_ptr);

      CM_ASSERT (call_ptr->ss != CM_SS_NONE);

      if (call_ptr->ss == CM_SS_NONE)
      {
        CM_ERR_1 (" ss for incom call id %d returned NONE", call_ptr->call_id);
      }

      /*------------------------------------------------------------------*/

      /* Call type is carried by invite indication
      */
      call_ptr->call_type  = cmipcall_map_calltype_to_cm_calltype (
                               cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_type);


      if((call_ptr->sys_mode == SYS_SYS_MODE_LTE) && 
         (call_ptr->call_type == CM_CALL_TYPE_VOICE ||
          call_ptr->call_type == CM_CALL_TYPE_VT ||
          call_ptr->call_type == CM_CALL_TYPE_VS))
      {
        call_ptr->is_volte = TRUE;
        CM_MSG_HIGH_0("is_volte is TRUE for MT");
      }
      else if((call_ptr->sys_mode == SYS_SYS_MODE_WLAN) && 
         (call_ptr->call_type == CM_CALL_TYPE_VOICE ||
          call_ptr->call_type == CM_CALL_TYPE_VT))
      {
        call_ptr->is_vowlan = TRUE;
        CM_MSG_HIGH_0("is_vowlan is TRUE for MT");
      }
      else
      {
        call_ptr->is_ip_call = TRUE;
        CM_MSG_HIGH_0("is_ip_call is TRUE for MT");
      }


      /* Initialize number buffer in call object
      */
      memset((byte *)&call_ptr->num, 0, sizeof(call_ptr->num));

      if ( cmipapp_rpt_ptr->ip_ind.ind.mt_invite.num.len != 0 )
      {
          call_ptr->num.len = memscpy( call_ptr->num.buf,
                                       sizeof(call_ptr->num.buf),
                                       cmipapp_rpt_ptr->ip_ind.ind.mt_invite.num.buf,
                                       cmipapp_rpt_ptr->ip_ind.ind.mt_invite.num.len ) ;
      }
	  
	  /* Initialize pi in call object
	  */
	  call_ptr->num.pi = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.num.pi;

	  /*Intialize child nuber */

      memset((byte *)&call_ptr->child_num, 0, sizeof(call_ptr->child_num));


	   if ( cmipapp_rpt_ptr->ip_ind.ind.mt_invite.child_num.len != 0   )
       {
          call_ptr->child_num.len = memscpy( call_ptr->child_num.buf,
                                             sizeof(call_ptr->child_num.buf),
                                             cmipapp_rpt_ptr->ip_ind.ind.mt_invite.child_num.buf,
                                             cmipapp_rpt_ptr->ip_ind.ind.mt_invite.child_num.len ) ;
       }

      /* Initialize redirecting number buffer in call object
      */
      memset((byte *)&call_ptr->redirecting_number.num, 0, sizeof(call_ptr->redirecting_number.num));

      if ( cmipapp_rpt_ptr->ip_ind.ind.mt_invite.redirecting_number.len != 0 )
      {
          call_ptr->redirecting_number.num.len = memscpy( call_ptr->redirecting_number.num.buf,
                                       sizeof(call_ptr->redirecting_number.num.buf),
                                       cmipapp_rpt_ptr->ip_ind.ind.mt_invite.redirecting_number.buf,
                                       cmipapp_rpt_ptr->ip_ind.ind.mt_invite.redirecting_number.len ) ;

		  call_ptr->redirecting_number.num.pi = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.redirecting_number.pi;
      }

      /*------------------------------------------------------------------*/

      /* Set up ind is not presented to the user and is only used by UI
      ** code to check if an incoming call can be allowed. User is alerted
      ** after CM_IP_CALL_IND_MT_RING_CNF is received. 
      */      
      call_ptr->call_state                     = CM_CALL_STATE_INCOM;

      call_ptr->call_subst.incom               = CM_CALL_INCOM_SUBST_SETUP;

      /* Set call info type to IP
      */
      call_ptr->cmcall_mode_info.info_type     = CM_CALL_MODE_INFO_IP;
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info;
      CM_MSG_HIGH_1("is_secure_call %d", cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info.is_secure_call);
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_peer_cap_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.peer_call_cap_attrib;
      if(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param != NULL)
      {
        ref_cnt_obj_add_ref(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
      }
      
      if(cmipapp_rpt_ptr->ip_ind.ind.mt_invite.caller_info != NULL)
      {
        call_ptr->cmcall_mode_info.info.ip_info.caller_info = cmipapp_rpt_ptr->ip_ind.ind.mt_invite.caller_info;
        CM_MSG_HIGH_1("caller_info, info_text %s", call_ptr->cmcall_mode_info.info.ip_info.caller_info->info_text);
      }
      
      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);  


      /*If the UE is in an emergency call, any incoming call must be provided the busy treatment, irrespective of the CW setting.
      */
      if(cmcall_is_there_a_call_type(call_ptr->ss, CM_CALL_TYPE_EMERGENCY, call_ptr->call_id) != CM_CALL_ID_INVALID)
      {
        cmipcall_invite_res (call_ptr, FALSE, CMIPAPP_INCOM_REJ_CAUSE_BUSY);
      }

      /*If the UE is in a voice or VT call, any incoming call must be provided the busy treatment, 
      ** if the CW is set to be UE based and it s disabled.
      */
       else if(cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->ue_based_cw.use_ue_based_cw &&
               !cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->ue_based_cw.is_enabled &&
        ((cmcall_is_there_a_call_type(call_ptr->ss, CM_CALL_TYPE_VOICE, call_ptr->call_id) != CM_CALL_ID_INVALID) ||
        (cmcall_is_there_a_call_type(call_ptr->ss, CM_CALL_TYPE_VT, call_ptr->call_id) != CM_CALL_ID_INVALID))
        )
      {
        cmipcall_invite_res (call_ptr, FALSE, CMIPAPP_INCOM_REJ_CAUSE_BUSY);
        CM_MSG_HIGH_0("ue_based-cw, reject incoming call");
      }

      /* Check if GW CS call is in origination state then reject the vowlan call 
               This is required to handle the race cond where CM is processing the GW CS call(is in orig state) 
               and IMS is not aware of this call . So if CM receives any MT call from IMS in this duration, 
                it needs to be rejected.
          */
       else if(call_ptr->is_vowlan == TRUE && !cmcall_is_no_call_in_gw_cs(CM_SS_MAX))
      {
        /* check any GW CS call in any of the state exists */
           /* voice call exists , reject MT call */
          CM_MSG_HIGH_0("GW CS voice call already exists.");
          cmipcall_invite_res (call_ptr, FALSE, CMIPAPP_INCOM_REJ_CAUSE_BUSY);
      }      /*
       * If there is no existing CDMA call, accept the VoLTE MT call
       */
      else if (cmcall_is_there_a_cdma_call() == CM_CALL_ID_INVALID)
      {
        /*------------------------------------------------------------------*/      

        /* Set up indication to UI followed by setup response from UI was 
        ** designed to mirror GW way of handling incoming calls. With 2 clients
        ** ICM and ITelephone being able to handle setup indications it can
        ** happen that ITel sends setup response with FALSE even before ICM
        ** can respond with TRUE. Final Verdict is to remove sending INVITE
        ** to CM from ip app. Intermediate fix is to make CM send setup 
        ** response.
        */

        /* cmcall_event( call_ptr, CM_CALL_EVENT_SETUP_IND ); */

        /* Call the IP call funtion for set up response
        */
        cmipcall_invite_res (call_ptr, TRUE, CMIPAPP_INCOM_REJ_CAUSE_NONE);

        /* Change incom substate to setup processed.
        */
        call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_SETUP_PROCESSED;  
        cmph_send_trm_priority_req(LTE_TRM_PRIORITY_HIGH, call_ptr->asubs_id);	
        cmcall_check_and_send_mmtel_status(call_ptr,TRUE);		
      }
      else
      {
        /* 1x call existing. Reject this VoLTE MT */

        /* Call the IP call funtion for set up response
        */
        cmipcall_invite_res (call_ptr, FALSE, CMIPAPP_INCOM_REJ_CAUSE_1X_COLLISION);
      }

     
      break;


    case CM_IP_CALL_IND_MT_RING_CNF:
      {

      /*
      ** Call id should be requested by IP app before sending any reports
      ** to CM.
      */
        cm_call_type_e_type new_call_type;
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.id );

      CM_MSG_HIGH_3("IP RXD: MT_RING_CNF, id=%d, sys_mode=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.id,
                   cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.sys_mode,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      if (call_ptr == NULL)
      {
        CM_ASSERT( call_ptr != NULL );
        CM_ERR_0 ( "ERROR: Unknown call ID in MT_RING_CNF");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
     
      /*------------------------------------------------------------------*/

      /* Call state should be incoming and subst should be setup_processed
      ** for alerting to be processed.
      */
      if( (call_ptr->call_state       == CM_CALL_STATE_INCOM) &&
         (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP_PROCESSED)
       )
      {

        call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_ALERTING;

        call_ptr->sys_mode   = cmipapp_map_sysmode_to_cm_sysmode (
                              cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.sys_mode);

        cmph_call_start( call_ptr );

      }
      else
      {      

        CM_ERR_0( "Rcvd CM_IP_CALL_IND_MT_RING_CNF out of seq.");
        call_ptr->end_status = CM_CALL_END_INCOM_CALL;
        cmcall_end( call_ptr );
        return;

      }           

      /*------------------------------------------------------------------*/

          /* Call type is carried by MT_RING_CNF as well.
          this is required as call_type may change between MT_INVITE and MT_RING_CNF
          It is IMS responsibility to sedn the correct call type always
        */
        new_call_type = cmipcall_map_calltype_to_cm_calltype (
                                 cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.call_type);
        if(call_ptr->call_type != new_call_type)
        {
          CM_MSG_HIGH_2("call type varied between MT_INVITE %d and MT_RING_CNF %d",call_ptr->call_type, new_call_type);
          call_ptr->call_type  = new_call_type;
		 
        }
     
        call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.is_secure_call = 
          cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.is_secure_call;

        CM_MSG_HIGH_1("is_secure_call %d", cmipapp_rpt_ptr->ip_ind.ind.mt_ring_cnf.is_secure_call);

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);      

      /*------------------------------------------------------------------*/
	  
        if(call_ptr->is_vowlan == FALSE)
        {
	  (void)cmph_update_orig_param_incoming_call(call_ptr);
        }

      cmcall_event( call_ptr, CM_CALL_EVENT_INCOM );

      /* delete the reference which is stored at MT invite after sending INCOM event. 
      ** QMI should add reference to the buf when receive INCOM event.
      ** answer cmd will pass in new ip_gen_param
      */
      REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);

	  if (call_ptr->cmcall_mode_info.info.ip_info.caller_info != NULL)
	  {
	    CM_MSG_HIGH_0("Released ref for caller_info");
        REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.caller_info);
	  }
      }
	  
      break;

    case CM_IP_CALL_IND_CALL_UPDATE_INFO:

      CM_MSG_HIGH_2("IP RXD: CALL_UPDATE_INFO, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_update.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* get the call pointer from the call ID and check for its validity */
      call_ptr = cmcall_ptr(cmipapp_rpt_ptr->ip_ind.ind.call_update.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call update info");

        return;
      }

      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.caller_name = 
        cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name;

      /* For adversarial scenarios, logging caller name len/text*/
      if(cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name == NULL)
      {
        CM_MSG_HIGH_0("caller_name not allocated. Something Wrong!");
      }
      else
      {
        if((cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name->caller_name_len == 0) ||
          (cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name->caller_name_text == NULL))
        {
          CM_MSG_HIGH_2("caller_name_len %d and caller_name_text %d. Something Wrong!",
            cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name->caller_name_len,
            cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name->caller_name_text);
        }
      }

      /* Processing of Caller Info and then sending this Info to registered clients (e.g. QMI-V) */
      cmcall_event( call_ptr, CM_CALL_EVENT_CNAP_INFO_RECEIVED);

      /* Since the Event has been sent, we are not storing the Caller Name in Call Object */
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.caller_name = NULL;

      break;

    case CM_IP_CALL_IND_BARRING:

      CM_MSG_HIGH_2("IP RXD: BARRING_IND, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_barring.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* get the call pointer from the call ID and check for its validity */
      call_ptr = cmcall_ptr(cmipapp_rpt_ptr->ip_ind.ind.call_barring.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in barring ind");

        return;
      }

      /* Processing of Caller Info and then sending this Info to registered clients (e.g. QMI-V) */
      cmcall_event( call_ptr, CM_CALL_EVENT_CALL_BARRED);

      break;

    case CM_IP_CALL_IND_FORWARD_INFO:

      CM_MSG_HIGH_2("IP RXD: FORWARD_INFO, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_forward.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
      
      /* get the call pointer from the call ID and check for its validity */
      call_ptr = cmcall_ptr(cmipapp_rpt_ptr->ip_ind.ind.call_forward.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in fwd info");

        return;
      }

      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.forward_info = 
        cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info;

      /* For adversarial scenarios, logging caller name len/text*/
      if(cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info == NULL)
      {
        CM_MSG_HIGH_0("forward_info not allocated. Something Wrong!");
      }
      else
      {
        if((cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info->hist_info_len == 0) ||
          (cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info->hist_info_text == NULL))
        {
            CM_MSG_HIGH_2("hist_info_len %d and hist_info_text %d. Something Wrong!",
            cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info->hist_info_len,
            cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info->hist_info_text);
        }
      }

      /* Sending this Forward Info to registered clients (e.g. QMI-V) */
      cmcall_event( call_ptr, CM_CALL_EVENT_CALL_BEING_FORWARDED);

      /* Since the Event has been sent, we are not storing the Forward Info in Call Object */
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.forward_info = NULL;

      break;

    case CM_IP_CALL_IND_MO_PROGR_INFO:

      CM_MSG_HIGH_2("IP RXD: MO_PROGR_INFO, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr (cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in progr info");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);
      /*------------------------------------------------------------------*/

      /* Copy the number, call is being made from
      */
      if ( cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.num.buf[0] != 0 )
      {
          call_ptr->num.len = memscpy( call_ptr->num.buf,
                                       sizeof(call_ptr->num.buf),
                                       cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.num.buf,
                                       cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.num.len ) ;
      }

      /* Check for the call state to be in origination
      */
      CM_ASSERT (call_ptr->call_state == CM_CALL_STATE_ORIG);

      if (call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        CM_ERR_0 ("Progress info for call not in CM_CALL_STATE_ORIG");
      }
      else
      {
        if(call_ptr->is_vowlan)
        {
          CM_MSG_HIGH_2("sys_mode Change from %d to %d",call_ptr->sys_mode, SYS_SYS_MODE_WLAN);
          call_ptr->sys_mode = SYS_SYS_MODE_WLAN;
        }
      }
      
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.is_secure_call = 
        cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.is_secure_call;

      CM_MSG_HIGH_2("is_secure_call %d is_mo_prog_ind_for_180_ringing %d", 
                       cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.is_secure_call,
                       cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.is_mo_prog_ind_for_180_ringing);

      /*------------------------------------------------------------------*/

      switch (cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.progr)
      {
        case CMIPAPP_PROG_SESSION:
         cmcall_event( call_ptr, CM_CALL_EVENT_PROGRESS_EARLY_MEDIA );

         break;

        case CMIPAPP_PROG_FORWARD:
          cmcall_event( call_ptr, CM_CALL_EVENT_CALL_FORWARDED );
          break;

        case CMIPAPP_PROG_REDIRECT:
          cmcall_event( call_ptr, CM_CALL_EVENT_REDIRECTING_NUMBER );
          break;

       case CMIPAPP_PROG_MO_RINGBACK:
          ip_ptr                 = &call_ptr->cmcall_mode_info.info.ip_info;
          ip_ptr->call_prog_info = CM_CALL_PROG_INFO_REMOTE_ALERT;
          ip_ptr->ipcall_peer_cap_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.peer_call_cap_attrib;
          cmcall_event( call_ptr, CM_CALL_EVENT_PROGRESS_INFO_IND );
          break;

       case CMIPAPP_PROG_MO_CALL_WAITING:
          cmcall_event( call_ptr, CM_CALL_EVENT_CALL_IS_WAITING);
          break;

        default:
          CM_MSG_HIGH_1 ("mo_prog_info default case %d",
                        cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.progr);
          break;

      } /* switch */

      if(cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.is_mo_prog_ind_for_180_ringing)
      {
          CM_MSG_HIGH_1 ("Rxd is_mo_prog_ind_for_180_ringing %d in MO_PROG_INFO_IND",
                        cmipapp_rpt_ptr->ip_ind.ind.mo_progr_info.is_mo_prog_ind_for_180_ringing);          
          call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.is_180_ringing_rxd_in_mo_prog_ind = TRUE;

          cmcall_event( call_ptr, CM_CALL_EVENT_ABRV_ALERT);
          
          /* Need to inform RIL about call_alert indication only once, when 180 ring is receieved
             clearing call_ptr flag imemdiately to avoid sending the flag as TRUE in subsequent call events*/   
          call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.is_180_ringing_rxd_in_mo_prog_ind = FALSE;
      }
      break;


    case CM_IP_CALL_IND_CONNECTED:
      {
        sys_modem_as_id_e_type rpt_asubs_id = cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;
        cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(rpt_asubs_id);
        cmph_sub_config_s_type   *ph_sub_config_ptr;
        
        CM_MSG_HIGH_2("IP RXD: CONNECTED, id=%d, as_id=%d",
                     cmipapp_rpt_ptr->ip_ind.ind.connected.id,
                     cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
        
        /* Get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.connected.id );
      
        if (call_ptr == NULL)
        {
          CM_ASSERT ( call_ptr != NULL );
          CM_ERR_0    ("ERROR: Unknown call ID in connected");
      
          return;
        }

        ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);
        if(ph_sub_config_ptr == NULL)
        {
          CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",call_ptr->asubs_id);
          return;
        }

        /* sometimes MMODE might have switched from SRLTE -> SingleMode ater
        ** MT INVITE was sent and before CM_IP_CALL_IND_CONNECTED
        ** hence for such cases make sure we move the call to CM_SS_MAIN
        */
        if(cmph_is_subs_feature_mode_1x_sxlte(rpt_asubs_id) &&
           state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED &&
           cmss_is_hybr2_operational() == FALSE)
        {
          call_ptr->ss         = cmcall_det_ss_from_sys_mode (call_ptr);
        }    
      
        /*------------------------------------------------------------------*/
      
        /* Copy the data block value from application
        */
        call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
      
        cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                                 cmipapp_rpt_ptr->data_block);
      
        /*------------------------------------------------------------------*/
      
        /* Get the system call is connected through
        */
        call_ptr->sys_mode =  cmipapp_map_sysmode_to_cm_sysmode (
                                cmipapp_rpt_ptr->ip_ind.ind.connected.sys_mode);
      
      
      
        CM_ASSERT( call_ptr != NULL );
      
        call_ptr->call_state        = CM_CALL_STATE_CONV;
      
        call_ptr->call_subst.conv   = CM_CALL_CONV_CALL_ACTIVE;
      
        call_ptr->call_connect_time = time_get_uptime_secs();     
      
        call_ptr->call_type = cmipcall_map_calltype_to_cm_calltype(cmipapp_rpt_ptr->ip_ind.ind.connected.call_type);
      
        call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.connected.call_attrib;
      
        CM_MSG_HIGH_1("is_secure_call %d", cmipapp_rpt_ptr->ip_ind.ind.connected.call_attrib.is_secure_call);

        call_ptr->cmcall_mode_info.info.ip_info.ipcall_local_cap_attrib_info= cmipapp_rpt_ptr->ip_ind.ind.connected.local_call_cap_attrib;
      
        call_ptr->cmcall_mode_info.info.ip_info.ipcall_peer_cap_attrib_info= cmipapp_rpt_ptr->ip_ind.ind.connected.peer_call_cap_attrib;
      

        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
        {
          // EMVT: Write a utility fn to convert and update is_em_vt;
          if(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type == CM_CALL_EM_VOICE)
          {
            call_ptr->is_em_vt = FALSE;
          }
          else if(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type == CM_CALL_EM_VT)
          {
            call_ptr->is_em_vt = TRUE;
          }
          else
          {
            CM_MSG_HIGH_1("Invalid em_type %d from IMS!",cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type);
          }
        }

      
        /*------------------------------------------------------------------*/
      
        cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );
      
        cmipcall_mng_state_update(CM_IP_CALL_IND_CONNECTED, TRUE, call_ptr->call_id, CM_IPAPP_CAUSE_NORMAL);
        
#ifdef  FEATURE_VOIP_E911_SUPPORT
        /* If the call is a mobile originated emergency call over EHRPD, queue an indication
        ** to CM to enter emergency callback mode
        */
        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
           call_ptr->direction == CM_CALL_DIRECTION_MO &&
           call_ptr->sys_mode  == SYS_SYS_MODE_HDR)
        {
          emerg_callbck_mode_cb( SD_EMERG_CB_MODE_HYBR_ENTER);
        }
#endif
      
        /* After sending the CALL CONNECT event to clients
        ** if the call is a mobile originated emergency call over LTE IMS,
        ** enter emergency callback mode ( if Emergency call customization so indicates) 
        */
        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
           call_ptr->direction == CM_CALL_DIRECTION_MO &&
           call_ptr->sys_mode  == SYS_SYS_MODE_LTE &&
           ph_sub_config_ptr->is_ecbm_required)
        {
          cmph_s_type  * ph_ptr = cmph_ptr();
      
          /* Store the RAT on which CM enters callback mode
          */
          ph_ptr->ecbm_rat = call_ptr->sys_mode;
      
          if (call_ptr->ss == CM_SS_MAX) 
          {
            cmph_enter_emergency_cb( ph_ptr, CM_SS_MAIN, TRUE );
          }
          else
          {
            cmph_enter_emergency_cb( ph_ptr, call_ptr->ss, TRUE );
          }
          CM_MSG_HIGH_2("LTE IMS ECBM rat%d,ss %d",ph_ptr->ecbm_rat,call_ptr->ss);
      
        }

        /* WLAN ECBM */
        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
           call_ptr->direction == CM_CALL_DIRECTION_MO &&
           cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp &&
           call_ptr->sys_mode  == SYS_SYS_MODE_WLAN)
        {
          cmph_enter_emergency_cb_wlan();
          CM_MSG_HIGH_1("WLAN ECBM rat,ss %d",call_ptr->ss);
        }
      
        /* - - - - - - - - - for WLAN <-> LTE HO - - - - - - - - - */
        /*If call is moving from WLAN -> LTE. set/reset corresponding flags*/
        if(call_ptr->is_vowlan && call_ptr->sys_mode == SYS_SYS_MODE_LTE)
        {
          call_ptr->is_vowlan = FALSE;
          switch(call_ptr->call_type)
          {
          case CM_CALL_TYPE_VOICE:
          case CM_CALL_TYPE_VT:
            call_ptr->is_volte = TRUE;
            break;
          default:
            break;
          }
          cmph_update_orig_param_incoming_call(call_ptr);
        }
        
        /*If call is moving from LTE -> WLAN set/reset corresponding flags and run dom sel if there is pending one*/
        if((cmcall_is_there_volte_call_obj(SYS_BM_64BIT(call_ptr->call_type)) != CM_CALL_ID_INVALID)&& 
           call_ptr->sys_mode == SYS_SYS_MODE_WLAN)
        {
          call_ptr->is_volte = FALSE;
          call_ptr->is_vowlan = TRUE;
          #ifdef FEATURE_DOMAIN_SELECTION
          if(cmsds_ptr()->srv_ind_buf_ptr != NULL)
          {
            cm_ss_e_type         ss            = CM_SS_NONE;
            ss = cmph_determine_ss_per_rat(rpt_asubs_id, cmsds_ptr()->srv_ind_buf_ptr->mode );
            cmsds_process_srv_ind(ss, cmsds_ptr()->srv_ind_buf_ptr);
      }
          #endif

          cmph_remove_act_id_and_force_orig_mode(call_ptr->ss,(cm_act_id_type)call_ptr->call_id, FALSE);
        }

      }
      break;


    case CM_IP_CALL_IND_CALL_END:
      {
        cmph_sub_config_s_type   *ph_sub_config_ptr;
        cmipapp_end_cause_e_type  ims_end_cause = cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause;
      CM_MSG_HIGH_3("IP RXD: CALL_END, end_cause=%d, client_end_cause=%d, call_id=%d",
                     cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause,
                     cmipapp_rpt_ptr->ip_ind.ind.call_end.client_end_cause,
                     cmipapp_rpt_ptr->ip_ind.ind.call_end.id);    

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr (cmipapp_rpt_ptr->ip_ind.ind.call_end.id);

      if (call_ptr == NULL)
      {
        CM_MSG_MED_0("call not exist when receiving IMS END IND, ignore!");

        return;
      }

      ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);
      if(ph_sub_config_ptr == NULL)
      {
        CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",call_ptr->asubs_id);
        return;
      }	  
	    /* If received IMS call end due to SRVCC drop the indication
      */
      if(cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause == CMIPAPP_END_CAUSE_SRVCC_LTE_TO_CS)
      {
	      cmipapp_release_reference_end_reason(call_ptr);
        break;
      }


      #ifdef FEATURE_IP_CALL
        // For VoWLAN call if device is in LPM, 
        // then call should be cleared instead of redialing over CSFB
        if (cmcall_vowlan_call_not_in_online_oprt_mode(call_ptr)          == TRUE &&
            cmipcall_should_call_redial_over_ip(call_ptr, ims_end_cause)  == FALSE)
        {
          ims_end_cause = CMIPAPP_END_CAUSE_NORMAL;
        }
      
        if(ims_end_cause == CMIPAPP_END_CAUSE_FALLBACK_TO_CS)
      {
        call_ptr->sr.ignore_sr_max_uptime = TRUE;
        CM_MSG_HIGH_0("Set ignore_sr_max_uptime due to CSFB");
      }
      #endif

	  /* If received IMS call end due to CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL, change the call_type to VOICE
	   **  or FALLBACK_TO_CS for call_type VT. 
	   */
	  if(ims_end_cause == CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL ||
	    (ims_end_cause == CMIPAPP_END_CAUSE_FALLBACK_TO_CS && 
	  	 call_ptr->call_type == CM_CALL_TYPE_VT))
	  {
	    call_ptr->call_type = CM_CALL_TYPE_VOICE;
	  }

	  /* If received loopback end cause just send it back to IMS
	  */
	  if(ims_end_cause == CMIPAPP_END_CAUSE_LOOP_BACK)
	  {
	    cmipapp_release_reference_end_reason(call_ptr);
		cmipcall_send_orig(call_ptr);
		break;
	  }

      /* If received IMS call end due to it being identified as an emergency call - reoriginate the call
      */
        if(ims_end_cause == CMIPAPP_END_CAUSE_ALTERNATE_EMERGENCY_CALL)
      {
        /*
        ** Update the emerg_srv_categ as the call may originate as EMERG over CS.
        */
        if(call_ptr->call_type == CM_CALL_TYPE_VT)
        {
          CM_MSG_HIGH_0("Due to 380 rsp, set is_em_vt to TRUE!");
          call_ptr->is_em_vt = TRUE;
        }

        call_ptr->num.emerg_srv_categ = cmipapp_rpt_ptr->ip_ind.ind.call_end.emerg_srv_categ;
		cmipapp_release_reference_end_reason(call_ptr);
        cmipcall_reoriginate_call_as_emergency(call_ptr, FALSE);
        break;
      }     
      

      /* 1) If emergency call originated over WLAN fails and IMS returns a FALLBACK_TO_CS end reason,
                      the call is reattempted over CS domain
                  2) If emergency call over LTE fails, and IMS returns FALLBACK_TO_CS end reason,
                      the call needs to be reattempted over CS domain
                      All the above when the emergency calling over wifi feature is ON
             */
      #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
      if((cmph_sub_info->wifi_e911_support) && (!cmph_sub_info->wifi_e911_wwan_full_srv) &&
        (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) &&
        (call_ptr->is_vowlan == TRUE) &&
        (ims_end_cause == CMIPAPP_END_CAUSE_FALLBACK_TO_CS|| 
           ims_end_cause == CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL))
      {
        /*
              ** Update the emerg_srv_categ as the call may originate as EMERG over CS.
              */
        call_ptr->num.emerg_srv_categ = cmipapp_rpt_ptr->ip_ind.ind.call_end.emerg_srv_categ;
        call_ptr->is_vowlan = FALSE;
		cmipapp_release_reference_end_reason(call_ptr);
        cmipcall_reoriginate_call_as_emergency(call_ptr, TRUE);
        break;
      }     
      #endif
      if((cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp) && 
        (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) &&
        (call_ptr->is_vowlan == TRUE) &&
        (cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause == CMIPAPP_END_CAUSE_FALLBACK_TO_CS || 
        cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause == CMIPAPP_END_CAUSE_HARD_FAILURE||
         cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause == CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL))
      {        
        cm_call_end_e_type curr_end_status;
        call_ptr->num.emerg_srv_categ = cmipapp_rpt_ptr->ip_ind.ind.call_end.emerg_srv_categ;
        call_ptr->is_vowlan = FALSE;
        cmipapp_release_reference_end_reason(call_ptr);
        curr_end_status = call_ptr->end_status;
        call_ptr->end_status = cmipcall_end_cause_to_cmcall_end_status
                              (cmipapp_rpt_ptr->ip_ind.ind.call_end.end_cause,
                                call_ptr->call_type, call_ptr->asubs_id);
        cmcall_check_and_update_wlan_e911_settings(call_ptr);
        call_ptr->end_status = curr_end_status;
        cmipcall_reoriginate_call_as_emergency(call_ptr, TRUE);
        break;
      }     

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
      
      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

      call_ptr->end_cause_to_client = cmipcall_map_client_end_status
                              (cmipapp_rpt_ptr->ip_ind.ind.call_end.client_end_cause);
							  
	  if(cmipapp_rpt_ptr->ip_ind.ind.call_end.end_reason != NULL)
          {
		call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason = cmipapp_rpt_ptr->ip_ind.ind.call_end.end_reason;
		CM_MSG_HIGH_1("end reason len %d", call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason->end_reason_len);
	  }

      switch( call_ptr->call_state )
      {

        case CM_CALL_STATE_IDLE:
          /* Client released already */
          if ( call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT )
          {
            #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
            if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
            {
              cmcall_drvcc_check_and_abort_drvcc_setup(call_ptr);            
            }
            #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */
            cmcall_end(call_ptr);
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_ORIG:

          /* If we are trying to originate, indicate a RELEASE
          ** end status and call the origination function to decide
          ** whether to continue or abort the origination process.
          */
          
          if(call_ptr->sr.voip_sr.is_lte_hard_failure) 
          {
            if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY && 
              ph_sub_config_ptr->is_alternate_redial_algorithm)
            {
              call_ptr->end_status = CM_CALL_END_IP_HARD_FAILURE;
            }
            else
            {
            call_ptr->end_status = CM_CALL_END_IP_FAIL;
            }
            call_ptr->sr.voip_sr.is_lte_hard_failure = FALSE;
          }
          else
          {
            call_ptr->end_status = cmipcall_end_cause_to_cmcall_end_status
                                   (ims_end_cause,
                                    call_ptr->call_type,
                                    call_ptr->asubs_id);
             if((call_ptr->favored_mode_pref == CM_MODE_PREF_WLAN_ONLY) && 
              ((CMPH_SS_MODE_PREF(call_ptr->asubs_id,cmph_ptr())) != CM_MODE_PREF_NONE))
             {
                call_ptr->favored_mode_pref = CMPH_SS_MODE_PREF(call_ptr->asubs_id,cmph_ptr());
             }
  
            /* End call without silent Redial when IMS sends normal call release */
            if((call_ptr->end_status == CM_CALL_END_REL_NORMAL) ||(call_ptr->favored_mode_pref == CM_MODE_PREF_WLAN_ONLY) ||
               call_ptr->is_special_pref_volte_redial)
            {
               cmcall_end(call_ptr);
               break;
            }
          }  
          
           /* IMS will send end reason string only for these two causes in ORIG, if IMS send endreason text
             for any other reason, release reference */
            if(ims_end_cause != CMIPAPP_END_CAUSE_PERMANENT_FAILURE &&
               ims_end_cause != CMIPAPP_END_CAUSE_NORMAL)
          {
            cmipapp_release_reference_end_reason(call_ptr);
          }
          /* Even when IMS sent normal call end release, cmph would not send LOW again */
    

          if(call_ptr->is_vowlan && 
           (ims_end_cause == CMIPAPP_END_CAUSE_FALLBACK_TO_CS ||
              ims_end_cause == CMIPAPP_END_CAUSE_ALTERNATE_VOICE_CALL)
            )
          {
            call_ptr->is_vowlan = FALSE;
          }

          cmph_send_trm_priority_req(LTE_TRM_PRIORITY_LOW, call_ptr->asubs_id);


          if((call_ptr->is_em_vt) && ((call_ptr->end_status == CM_CALL_END_IP_FAIL) || (call_ptr->end_status == CM_CALL_END_IP_END)))
          {
            call_ptr->is_em_vt = FALSE;
            CM_MSG_HIGH_1("Reset is_em_vt to FALSE for end_cause %d",call_ptr->end_status);
          }
           #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
          /* if wifi_e911_wwan_full_srv is enabled, and UE is in full service then all call end reasons are to be changed to 
            CM_CALL_END_IP_FAIL to trigger forcing preferences to SD */
          if((ph_sub_config_ptr->wifi_e911_wwan_full_srv == TRUE) && 
            (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) && 
            cmss_is_ss_in_full_srv(call_ptr->ss))
          {
            call_ptr->end_status = CM_CALL_END_IP_FAIL;
          }
          #endif
          cmcall_orig_proc(call_ptr);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_INCOM:
          /* Call was never established -
          ** simply end the call.
          */
          /* This covers scenario where incoming call can be in SETUP/ ALERTING */		
            if(ims_end_cause == CMIPAPP_END_CAUSE_ANSWERED_ELSEWHERE || 
			   ims_end_cause == CMIPAPP_END_CAUSE_BUSY_EVERYWHERE )
		  {
            call_ptr->end_status = cmipcall_end_cause_to_cmcall_end_status
                                   (ims_end_cause,
                                    call_ptr->call_type,call_ptr->asubs_id);
		  }
		  else
		  {
            call_ptr->end_status = CM_CALL_END_REL_NORMAL;
		  }
          cmcall_end( call_ptr );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_CONV:
            if(ims_end_cause == CMIPAPP_END_CAUSE_DRVCC_WIFI_TO_CS)
            {
              /* call end due to this cause are to be supressed */
              call_ptr->end_status = CM_CALL_END_DRVCC_WIFI_TO_CS;
              call_ptr->end_cause_to_client = CM_CALL_END_NONE;
            }
            else
            {
            call_ptr->end_status = CM_CALL_END_REL_NORMAL;
            }
            #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
            if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
            {
              cmcall_drvcc_check_and_abort_drvcc_setup(call_ptr);
            }
            #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */
            cmcall_end(call_ptr);            
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:
          CM_ERR_1( "Default switch in CALL_EVENT_END , call_state=%d",
                         call_ptr->call_state);
        }      
      } /* CM_IP_CALL_IND_CALL_END*/
      break;

    /*------------------------------------------------------------------*/

    case CM_IP_CALL_IND_CALL_HOLD:
      
      CM_MSG_HIGH_2("IP RXD: CALL_HOLD, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.hold.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
      
      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.hold.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call hold");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

	  /* If the call is put to hold by remote party, send indication to client without
      ** changing call status
      */
    call_ptr->cmcall_mode_info.info.ip_info.held_call_audio_attrib = 
          cmipapp_rpt_ptr->ip_ind.ind.hold.held_call_audio_attrib;
      if(cmipapp_rpt_ptr->ip_ind.ind.hold.is_call_held_by_remote)
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_CALL_ON_HOLD_BY_REMOTE);
        break;
      }

      /* Call has to satisfy following conditions to be put on hold
      ** Call type is not checked since multiple call types can use
      ** cmipcall
      ** 
      ** - Call state has to be in conversation.      
      ** - Call substate has to be Active.
      */
      if ((call_ptr->call_state      != CM_CALL_STATE_CONV) ||          
          (call_ptr->call_subst.conv != CM_CALL_CONV_CALL_ACTIVE)
         )
      {
        CM_ERR_1 ("Call %d not in right state for hold", call_ptr->call_id);
        break;
      }    
      
      /*------------------------------------------------------------------*/

      /* call is not put to hold by remote party
      */
      CM_MSG_HIGH_2 ("Call hold status for call %d cause %d", 
                     call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.ind.hold.cause);
      if (cmipapp_rpt_ptr->ip_ind.ind.hold.cause == CM_IPAPP_CAUSE_NORMAL)
      {
        call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ON_HOLD; 
        ss_status = TRUE;
      }          
      else
      {
         ss_status = FALSE;
      }

      /*------------------------------------------------------------------*/

      /*
      ** 10/31/05 - According to UI MANAGE_CALLS_CONF way of supplying
      ** active call list in a table is a legacy implementation. UI is
      ** more comfortable handling call hold/retrieve events on a per 
      ** call basis.
      */
      if(ss_status)
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_CALL_ON_HOLD);
      }

      cmipcall_mng_state_update(CM_IP_CALL_IND_CALL_HOLD, ss_status, call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.ind.hold.cause);

      break;

    case CM_IP_CALL_IND_CALL_LOCAL_HOLD:
    {
      sys_modem_as_id_e_type rpt_asubs_id = cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;
      sys_modem_as_id_e_type other_subs = (rpt_asubs_id == SYS_MODEM_AS_ID_1)?
        SYS_MODEM_AS_ID_2:SYS_MODEM_AS_ID_1;

      CM_MSG_HIGH_2("IP RXD: CALL_LOCAL_HOLD, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.local_hold.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.local_hold.id );
      
      /*------------------------------------------------------------------*/
      /* update phone voice sub to the other sub
      */
      if(ph_ptr->curr_voice_subs == rpt_asubs_id)
      {
        ph_ptr->curr_voice_subs = other_subs;
        cmph_sp_subs_info_update();
      }

      /* Copy the data block value from application */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/
      cmcall_event (call_ptr, CM_CALL_EVENT_LOCAL_HOLD);

      break;
    }
    case CM_IP_CALL_IND_CALL_LOCAL_RETRIEVE:
    {
      sys_modem_as_id_e_type rpt_asubs_id = cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;
      
      CM_MSG_HIGH_2("IP RXD: CALL_LOCAL_RETRIEVE, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.local_retrieve.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.local_retrieve.id );

      /*------------------------------------------------------------------*/
      /* update phone voice sub to the current sub
      */
      if(ph_ptr->curr_voice_subs != rpt_asubs_id)
      {
        ph_ptr->curr_voice_subs = rpt_asubs_id;
        cmph_sp_subs_info_update();
      }

      /* Copy the data block value from application */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/
      cmcall_event (call_ptr, CM_CALL_EVENT_LOCAL_HOLD);

      break;
    }

    case CM_IP_CALL_IND_CALL_DEFLECTION:

      CM_MSG_HIGH_2("IP RXD: CALL_DEFLECTION, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_deflection.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
	  
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.call_deflection.id );
      
      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0("ERROR: Unknown call ID in call hold");
      
        return;
      }
      
      /*------------------------------------------------------------------*/
      
      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
      
      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                         cmipapp_rpt_ptr->data_block);
		
      CM_MSG_HIGH_2 ("Call deflection sattus for call %d cause %d", 
                   call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.ind.call_deflection.cause);
      
      if (cmipapp_rpt_ptr->ip_ind.ind.call_deflection.cause == CM_IPAPP_CAUSE_NORMAL)
      {
        ss_status = TRUE;
      }          
      
      /* Copy the cause to call object.		
      */
      call_ptr->end_params.end_params.ip_cause.cause = 
      			 cmipapp_rpt_ptr->ip_ind.ind.call_deflection.cause;
      
      cmipcall_mng_state_update(CM_IP_CALL_IND_CALL_DEFLECTION, ss_status, call_ptr->call_id, CM_IPAPP_CAUSE_NORMAL);
      break;

    case CM_IP_CALL_IND_CALL_RETRIEVE:

      CM_MSG_HIGH_2("IP RXD: CALL_RETRIEVE, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.retrieve.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.retrieve.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call hold");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;
      
      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /* If the call is retrieved by remote party, send indication to client without
      ** changing call status
      */
      if(cmipapp_rpt_ptr->ip_ind.ind.retrieve.is_call_retrieve_by_remote)
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_CALL_RETRIEVE_BY_REMOTE);
        break;
      }
      /*------------------------------------------------------------------*/

      /* Call has to satisfy following conditions to be put on hold
      ** Call type is not checked since multiple call types can use
      ** cmipcall.
      ** 
      ** - Call state has to be in conversation.      
      ** - Call substate has to be on hold.
      */
      if ((call_ptr->call_state      != CM_CALL_STATE_CONV) ||          
          (call_ptr->call_subst.conv != CM_CALL_CONV_CALL_ON_HOLD)
         )
      {
        CM_ERR_1 ("Call %d not in right state for retrieve", call_ptr->call_id);
        break;
      }          

      /*------------------------------------------------------------------*/
      CM_MSG_HIGH_2 ("Call retrieve status for  call %d cause %d", 
                       call_ptr->call_id,
                       cmipapp_rpt_ptr->ip_ind.ind.retrieve.cause);

      if (cmipapp_rpt_ptr->ip_ind.ind.retrieve.cause == CM_IPAPP_CAUSE_NORMAL)
      {
        call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE; 
        ss_status = TRUE;
      }          
      else
      {
        ss_status = FALSE;
      }

      /*------------------------------------------------------------------*/

      /* 
      ** 10/31/05 - According to UI MANAGE_CALLS_CONF way of supplying
      ** active call list in a table is a legacy implementation. UI is
      ** more comfortable handling call hold/retrieve events on a per 
      ** call basis.
      */
     
      if(ss_status)
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_CALL_RETRIEVED);
      }
      cmipcall_mng_state_update(CM_IP_CALL_IND_CALL_RETRIEVE, ss_status, call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.ind.retrieve.cause);
      break;            

    case CM_IP_CALL_IND_VS_STATUS:
      CM_MSG_HIGH_2("IP RXD: VS_STATUS, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.vs_status.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.vs_status.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0 ("ERROR: Unknown call ID in call hold");

        return;
      }      

      /* Voice call needs to be in conversation for videoshare to be possible
      */
      if( !((call_ptr->call_state == CM_CALL_STATE_CONV) &&                
            (call_ptr->call_type == CM_CALL_TYPE_VOICE)
           )
        )
      {
        CM_ERR_1("Call %d in incorrect state for videoshare", 
                  call_ptr->call_id );
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

      /* Event sent to clients informing videoshare availability
      */
      CM_MSG_HIGH_2 ("Vidoeshare status on call %d status %d", 
                      call_ptr->call_id, cmipapp_rpt_ptr->ip_ind.ind.vs_status.is_vs_poss );
      
      if (cmipapp_rpt_ptr->ip_ind.ind.vs_status.is_vs_poss == TRUE)
      {     
        cmcall_event (call_ptr, CM_CALL_EVENT_VS_AVAIL);
      }
      else
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_VS_NOT_AVAIL);
      }

      break;

    case CM_IP_CALL_IND_SUPS_CONF:
      
      
      /* Call dependant supp service works on more
      ** than one call. Conf is not sent with respect 
      ** to a particular call id.
      */      

      /*------------------------------------------------------------------*/

      CM_MSG_HIGH_6("IP RXD: SUPS_CONF, sups_type=%d, num_part=%d, participant=0x%x id=%d, rpt asubs_id=%d, lte asubs_id=%d",
                    cmipapp_rpt_ptr->ip_ind.ind.sups_conf.sups_type,
                    cmipapp_rpt_ptr->ip_ind.ind.sups_conf.number_of_participants_add_removed,
                    	cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants,
                       cmipapp_rpt_ptr->ip_ind.ind.sups_conf.id,
                       cmipapp_rpt_ptr->ip_ind.hdr.asubs_id,
                       cmph_get_sub_with_lte_cap());

      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();
	  
	   /* Call Id for sups */
	  call_info_ptr->call_id = cmipapp_rpt_ptr->ip_ind.ind.sups_conf.id;

	  call_info_ptr->asubs_id= cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;

	  call_info_ptr->mode_info.info.ip_call.modify_error_code =
          cmipcall_end_cause_to_cmcall_end_status((cmipapp_end_cause_e_type)cmipapp_rpt_ptr->ip_ind.ind.sups_conf.failure_cause,
                                              CM_CALL_TYPE_NONE,
                                              call_info_ptr->asubs_id);

      /* Success of supp service req */
      call_info_ptr->mode_info.info.ip_call.call_ss_success =
                    cmipapp_rpt_ptr->ip_ind.ind.sups_conf.call_success;      
         
      /* Sups type that is generating ss_success */
      call_info_ptr->mode_info.info.ip_call.sups_type =
                    cmipapp_rpt_ptr->ip_ind.ind.sups_conf.sups_type;      

      call_info_ptr->mode_info.info.ip_call.number_of_participants_add_removed =
                    cmipapp_rpt_ptr->ip_ind.ind.sups_conf.number_of_participants_add_removed;

      /* List of active calls is not sent as in GW because IP call
      ** sends individual hold/retrieve for each call.
      */      

      /* Set info_type to IP
      */
      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_IP;

       /* CM pass the participant pointer to QMi directly without CM buffer allocation
      ** This buffer will be released by QMi after use
      */
      call_info_ptr->mode_info.info.ip_call.participant_uri = cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants;

      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_MNG_CALLS_CONF, 
                                      call_info_ptr );


      if(cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants != NULL)
      {
        ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants);
      }

      /* Deallocate the memory after sending the call event. */
      cm_mem_free(call_info_ptr);

      break;


    case CM_IP_CALL_IND_TRANSFER:
      
      CM_MSG_HIGH_2("IP RXD: CM_IP_CALL_IND_TRANSFER, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_transfer.id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.call_transfer.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call hold");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

      cmcall_event (call_ptr, CM_CALL_EVENT_TRANSFERRED_CALL);
      
      break;


    case CM_IP_CALL_IND_CONNECTED_NUM:
      
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.connected_num.id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call hold");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

      /* Copy the new connected number from ip app */
      call_ptr->num  = cmipapp_rpt_ptr->ip_ind.ind.connected_num.num;
      call_ptr->cmcall_mode_info.info.ip_info.connected_num_is_ect_trig = 
        cmipapp_rpt_ptr->ip_ind.ind.connected_num.is_ect_triggered;
      /*------------------------------------------------------------------*/

      cmcall_event (call_ptr, CM_CALL_EVENT_CONNECTED_NUM);

      break;

    case CM_IP_CALL_IND_MODIFY_COMPLETE:
      
      CM_MSG_HIGH_2("IP RXD: MODIFY_COMPLETE, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
      
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call modify");
        return;
      }

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /* store the call modify status
      */
      call_ptr->cmcall_mode_info.info.ip_info.call_modif_err_status = cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.modify_error_status;
      call_ptr->cmcall_mode_info.info.ip_info.modify_error_code = cmipcall_map_client_end_status(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.modify_error_code);
      call_ptr->cmcall_mode_info.info.ip_info.retry_timer_val = cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.retry_timer_val;
      call_ptr->cmcall_mode_info.info.ip_info.modify_cause_code     = cmipcall_map_modify_cause_code(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.modify_cause_code);
      
      /* store the new call type and call attrib */
      if(!cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.modify_error_status)
      {
        call_ptr->call_type = cmipcall_map_calltype_to_cm_calltype(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_type);
        call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib;
        
        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
        {
          if(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type == CM_CALL_EM_VOICE)
          {
            call_ptr->is_em_vt = FALSE;
          }
          else if(cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type == CM_CALL_EM_VT)
          {
            call_ptr->is_em_vt = TRUE;
          }
          else
          {
            CM_MSG_HIGH_1("Invalid em_type %d from IMS!",cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.em_type);
          }
        }
        CM_MSG_MED_2("Modify cnf success with audio=%d, video = %d",
                           cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.audio_attrib,
                           cmipapp_rpt_ptr->ip_ind.ind.call_modify_cnf.call_attrib.video_attrib);
      }
      
      CM_MSG_MED_2("Modify cnf err status = %d, err_code=%d ",
                           call_ptr->cmcall_mode_info.info.ip_info.call_modif_err_status,
                           call_ptr->cmcall_mode_info.info.ip_info.modify_error_code);
      
      cmcall_event(call_ptr, CM_CALL_EVENT_MODIFY_COMPLETE_CONF);

      /* Reset new call type field
      */
      call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type = CM_CALL_TYPE_NONE;
      memset(&(call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib),0,sizeof(cm_ip_call_attrib_info_s_type));
      break;

    case CM_IP_CALL_IND_MODIFY_IND:      
      CM_MSG_HIGH_2("IP RXD: MODIFY_IND, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_modify_ind.call_id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
      
      call_ptr = cmcall_ptr(cmipapp_rpt_ptr->ip_ind.ind.call_modify_ind.call_id);
      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call modify");
        return;
      }

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmipapp_rpt_ptr->ip_ind.ind.call_modify_ind.call_attrib;
      call_ptr->call_type = cmipcall_map_calltype_to_cm_calltype(cmipapp_rpt_ptr->ip_ind.ind.call_modify_ind.req_call_type);

      cmcall_event(call_ptr, CM_CALL_EVENT_MODIFY_IND);
      break;

    case CM_IP_APP_AUDIO_CODEC_INFO:
      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.audio_codec.call_id);

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in audio codec info");

        return;
      }

      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      call_ptr->speech_codec_info.network_mode = SYS_NETWORK_TYPE_LTE;

      if(call_ptr->is_vowlan)
      {
        CM_MSG_HIGH_0("Set network_mode as WLAN for Audio Codec Indo");
        call_ptr->speech_codec_info.network_mode = SYS_NETWORK_TYPE_WLAN;
      }

      // For normal VT calls over IMS, we need to make the change and we need to derive it based on sys_mode
      if(call_ptr->sys_mode == SYS_SYS_MODE_WCDMA)
      {
        CM_MSG_HIGH_0("Set network_mode as WCDMA for Audio Codec Indo");
        call_ptr->speech_codec_info.network_mode = SYS_NETWORK_TYPE_WCDMA;
      }      

      call_ptr->speech_codec_info.speech_codec_type = 
              cmipapp_rpt_ptr->ip_ind.ind.audio_codec.speech_codec_type;
      call_ptr->speech_codec_info.speech_enc_samp_freq = 
              cmipapp_rpt_ptr->ip_ind.ind.audio_codec.speech_enc_samp_freq;
      
      cmcall_event (call_ptr, CM_CALL_EVENT_SPEECH_CODEC_INFO);
      
      break;

    case CM_IP_CALL_START_DTMF_CONF:
      cminband_ip_rpt_proc(CM_IP_CALL_START_DTMF_CONF, cmipapp_rpt_ptr->ip_ind.ind.dtmf_start_conf.dtmf_success);
      break;

    case CM_IP_CALL_STOP_DTMF_CONF:
      cminband_ip_rpt_proc(CM_IP_CALL_STOP_DTMF_CONF, cmipapp_rpt_ptr->ip_ind.ind.dtmf_stop_conf.dtmf_success);
      break;

    case CM_IP_CALL_START_DTMF_IND:
      cminband_ip_ind_proc(CM_IP_CALL_START_DTMF_IND,cmipapp_rpt_ptr->ip_ind.ind.start_dtmf_ind);
      break;
      
    case CM_IP_CALL_STOP_DTMF_IND:
      cminband_ip_ind_proc(CM_IP_CALL_STOP_DTMF_IND,cmipapp_rpt_ptr->ip_ind.ind.stop_dtmf_ind);
      break;

    case CM_IP_CALL_JOIN_CONF_CALL:

      CM_MSG_MED_4("IP RXD: JOIN_CONF, id=%d, paticipant_uri=0x%x rpt asubs_id=%d, lte asubs_id=%d",
                    cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.id,
                    	cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri,
                       cmipapp_rpt_ptr->ip_ind.hdr.asubs_id,
                       cmph_get_sub_with_lte_cap());
                       
      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();
      call_info_ptr->call_id = cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.id;
      call_info_ptr->asubs_id= cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;

      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_IP;
      call_info_ptr->mode_info.info.ip_call.participant_uri = cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri;

      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_JOIN_CONFERENCE, 
                                      call_info_ptr );

      if(cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri != NULL)
      {
        ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri);
      }

      /* Deallocate the memory after sending the call event. */
      cm_mem_free(call_info_ptr);
      
      break;

    case CM_IP_CALL_CONF_INFO:
      CM_MSG_HIGH_3("IP RXD: CONF_INFO, id=%d, rpt asubs_id=%d, lte asubs_id=%d",
                       cmipapp_rpt_ptr->ip_ind.ind.conf_info.id,
                       cmipapp_rpt_ptr->ip_ind.hdr.asubs_id,
                       cmph_get_sub_with_lte_cap());

      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();
      call_info_ptr->call_id = cmipapp_rpt_ptr->ip_ind.ind.conf_info.id;
      call_info_ptr->asubs_id= cmipapp_rpt_ptr->ip_ind.hdr.asubs_id;

      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_IP;

      call_info_ptr->mode_info.info.ip_call.conference_info = cmipapp_rpt_ptr->ip_ind.ind.conf_info.conf;

      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_CONFERENCE_INFO, 
                                      call_info_ptr );

      if(cmipapp_rpt_ptr->ip_ind.ind.conf_info.conf != NULL)
      {
        ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.conf_info.conf);
      }

      /* Deallocate the memory after sending the call event. */
      cm_mem_free(call_info_ptr);
      
      break;

    /* Receive rat change from IMS
      */
    case CM_IP_IND_CALL_RAT_CHANGE:
      cmipcall_rat_change_rpt_proc(cmipapp_rpt_ptr);
      break;

    /* ===This indication is obsoleted for now. No one use it 
    ** CM_CALL_EVENT_CALL_IS_WAITING will be sent when is_alert_info_present is set in progress info ind
    ** It is used to notify calling party call is waiting. For called party, QMI send call waiting upon
    ** CM_CALL_EVENT_INCOMING  ===*/
    case CM_IP_CALL_IND_CALL_WAIT:
      #ifdef FEATURE_IP_CALL
      
      CM_MSG_HIGH_2("IP RXD: CALL_WAIT, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_waiting.call_id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
      
      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.call_waiting.call_id );

      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0    ("ERROR: Unknown call ID in call hold");
        return;
      }

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);      

      /* Copy the new waiting num from ip */
      call_ptr->waiting_num = cmipapp_rpt_ptr->ip_ind.ind.call_waiting.num;

      /* Notify clients list of call event CM_CALL_EVENT_CALL_IS_WAITING. */
      cmcall_event ( call_ptr, CM_CALL_EVENT_CALL_IS_WAITING );
      break;
      #endif
      
	case CM_IP_CALL_IND_ANSWER_REJECT:
    
    CM_MSG_HIGH_2("IP RXD: ANSWER_REJECT, id=%d, as_id=%d",
                 cmipapp_rpt_ptr->ip_ind.ind.answer_reject.id,
                 cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
    
	  /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.answer_reject.id );
      
      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0 ("ERROR: Unknown call ID in ANSWER reject");
        return;
      }
	  
      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

	  CM_MSG_HIGH_1("ANSWER reject from IMS for INCOM, cause %d", cmipapp_rpt_ptr->ip_ind.ind.answer_reject.cause);
	  
	  cmipcall_mng_state_update(CM_IP_CALL_IND_ANSWER_REJECT, ss_status, call_ptr->call_id, CM_IPAPP_CAUSE_NORMAL);
	  break;

    case CM_IP_APP_AUDIO_SESSION_REL_CNF:
      CM_MSG_HIGH_0("IP RXD: CM_IP_APP_AUDIO_SESSION_REL_CNF");
      #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      /* audio session released confirmation received from IMS 
            can send the srvcc call cache context to NAS now */
      if(cmph_sub_info->srvcc_sync_support)
      {
        /* srvcc_sync: post the waiting command here so that cache context can be sent to NAS */
        /* and set the flag to False that cache context has been sent */
        cm_set_wait_for_ims_audio_rel(FALSE, cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);
        cmwcall_process_ims_audio_rel_cnf();
      }
      else
      {
        CM_MSG_HIGH_0("srvcc_sync_support is not defined, wrong report received");
      }
      #else
      CM_MSG_HIGH_0("FEATURE_IP_CALL or FEATURE_CM_LTE is not defined");
      #endif
      break;

    case CM_IP_APP_CALL_SWITCH_CODEC_CNF:
    {
      CM_MSG_HIGH_2("IP RXD: SWITCH_CODEC_CNF, id=%d, as_id=%d",
                   cmipapp_rpt_ptr->ip_ind.ind.call_switch_codec_cnf.call_id,
                   cmipapp_rpt_ptr->ip_ind.hdr.asubs_id);

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( cmipapp_rpt_ptr->ip_ind.ind.call_switch_codec_cnf.call_id );
      
      if (call_ptr == NULL)
      {
        CM_ASSERT ( call_ptr != NULL );
        CM_ERR_0 ("ERROR: Unknown call ID in switch codec cnf");
        return;
      }
	  
      /*------------------------------------------------------------------*/

      /* Copy the data block value from application
      */
      call_ptr->ipapp_data_block = cmipapp_rpt_ptr->data_block;

      cmipapp_set_user_cb_data(call_ptr->ipapp_id,
                               cmipapp_rpt_ptr->data_block);

      /*------------------------------------------------------------------*/

	  CM_MSG_HIGH_2("switch codec cnf from IMS for call id %d, status %d",
	                 cmipapp_rpt_ptr->ip_ind.ind.call_switch_codec_cnf.call_id,
	                 cmipapp_rpt_ptr->ip_ind.ind.call_switch_codec_cnf.status);
	  
	  /* Copy the status of switch codec cnf in call_ptr */
      call_ptr->cmcall_mode_info.info.ip_info.switch_codec_cmd_status = cmipapp_rpt_ptr->ip_ind.ind.call_switch_codec_cnf.status;

      /* Notify clients list of call event CM_CALL_EVENT_SWITCH_CODEC_CNF. */
      cmcall_event ( call_ptr, CM_CALL_EVENT_SWITCH_CODEC_CNF );
	  break;
    }

    default:
      CM_MSG_MED_1 ("Unkwn report from IP app %d", cmipapp_rpt_ptr->hdr.cmd);
      break;

  } /* switch (cmipapp_rpt_ptr->hdr.cmd) */
  

} /* cmipcall_rpt_proc () */

/*===========================================================================

FUNCTION  cmipcall_process_audio_release

DESCRIPTION
  This function handles the Audio Rel Indication from IMS.
  Basically, based on Sync flag, CM will send out the IMS Cache to NAS

DEPENDENCIES

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void cmipcall_process_audio_release(cm_audio_session_rel_msg_type *ipapp_audio_rel)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

  if( (ipapp_audio_rel->ip_audio_rel_type == CM_IPAPP_AUDIO_REL_3GPP_SRVCC) )
  {
    CM_MSG_HIGH_0("Audio_rel from IMS for 3gpp srvcc sync");
  /* audio session released confirmation received from IMS 
          can send the srvcc call cache context to NAS now */
  if(cmph_get_sub_config_ptr(SYS_MODEM_AS_ID_1)->srvcc_sync_support)
  {
    /* srvcc_sync: post the waiting command here so that cache context can be sent to NAS */
    /* and set the flag to False that cache context has been sent */
    cm_set_wait_for_ims_audio_rel(FALSE,SYS_MODEM_AS_ID_1);
    cmwcall_process_ims_audio_rel_cnf();
  }
  else
  {
    CM_MSG_HIGH_0("srvcc_sync_support is not defined, wrong report received");
  }
  }
  else if(ipapp_audio_rel->ip_audio_rel_type == CM_IPAPP_AUDIO_REL_3GPP_DRVCC)
  {
    /* 3gpp DRVCC handling */
    CM_MSG_HIGH_0("DRVCC: Audio Rel from IMS for pp");
  }
  else if(ipapp_audio_rel->ip_audio_rel_type == CM_IPAPP_AUDIO_REL_3GPP2_DRVCC)
  {
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    CM_MSG_HIGH_0("DRVCC: Audio Rel from IMS for pp2");
    cmcall_drvcc_send_x_audio_rel_ind();
    #endif
  }
  else
  {
    CM_MSG_HIGH_1("Audio_rel from IMS for %d",ipapp_audio_rel->ip_audio_rel_type);
  }
  #else
  CM_MSG_HIGH_0("FEATURE_IP_CALL or FEATURE_CM_LTE is not defined");
  #endif

  return;
}

/*===========================================================================

FUNCTION  cmipcall_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified call command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None

===========================================================================*/
cm_call_cmd_err_e_type  cmipcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
)
{

  cmcall_s_type               *call_ptr      = NULL;
    /* Pointer to the call object */

  cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  int                          i;
    /* For loop counter */
  cmph_s_type           *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  CM_MSG_HIGH_1( "IPCALL cmd check %d",call_cmd_ptr->cmd_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* check cmd is already in error or not
  */
  if ( call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA )
  {
    if (call_cmd_ptr->info.cmd_err != CM_CALL_CMD_ERR_NOERR)
    {
      return call_cmd_ptr->info.cmd_err;
    }

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /* Check whether command is possible.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*---------------------------*/
                       /* Response to a setup message */
                       /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:


      call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );

      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
      }
      else if ( call_ptr->call_state != CM_CALL_STATE_INCOM )
      {
        /* Makes sense only in incoming state
        */
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }
      /*lint -restore */
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_ANSWER:

      /* check command parameter errors */
      if((cmd_err = cmcall_call_cmd_answer_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
      {
          /* stop further processing if any errors found in the command */
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );
      if(call_ptr==NULL)
      {
          CM_MSG_HIGH_0( "call_ptr_NULL");
          break; 
      }

      if (call_ptr->call_state != CM_CALL_STATE_INCOM)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }

      /*lint -restore */
      break; /* CM_CALL_CMD_ANSWER */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_END:

      /* CALL_CMD_END can carry calls in multiple technologies. Earlier it
      ** was either xcall_end, wcall_end or ipcall_end being called. Currently
      ** all three get called one after the other and overall call state can
      ** go to IDLE in any of these calls. So not returning error any more.            
      */
      if( (cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_IDLE ) )
      {                    
        CM_MSG_MED_0 ("overall call state in idle");         
      }


      /* check command parameter errors */
      /* none */
      for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
               != CM_CALL_MODE_INFO_IP)         
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        call_ptr = cmcall_ptr ( call_cmd_ptr->info.end_params[i].call_id );

        if (call_ptr == NULL )
        {
          cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
          break;
        }

        if((ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE) 
        #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
        && !(cmph_get_sub_config_ptr(call_ptr->asubs_id)->voice_in_wifi_only && call_ptr->is_vowlan)
        #endif
        )
        {
          cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
          break;
        }

        if ( call_ptr->call_state == CM_CALL_STATE_IDLE )
        {
          /* In idle this command makes no sense.
          */
          cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
          break;
        }

        /* Need not check for call info type, since CM internally
        ** determines from call object
        */
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


    case CM_CALL_CMD_SUPS:
      
      /* Check cmd parameters for call related sups
      */
      cmd_err = cmipcall_sups_cmd_check (call_cmd_ptr);

    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_MO_MODIFY_REQ:
    case CM_CALL_CMD_MODIFY_RES:
    
      call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );
      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
        CM_ASSERT(call_ptr != NULL);
        break;
      }

      if(call_ptr->call_state != CM_CALL_STATE_CONV)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
      }

      /* Call modification is possible on only VOIP, VS, VT calls
      */
      if ( !((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
             (call_ptr->call_type == CM_CALL_TYPE_VT) ||
             (call_ptr->call_type == CM_CALL_TYPE_VS)
            )
         )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_S;
      }

      /* Call can be modified to only VOIP, VS, VT calls
      */
      if ( !((call_cmd_ptr->info.ip_call.new_call_type_req == CM_CALL_TYPE_VOICE) ||
             (call_cmd_ptr->info.ip_call.new_call_type_req == CM_CALL_TYPE_VT) || 
             (call_cmd_ptr->info.ip_call.new_call_type_req == CM_CALL_TYPE_VS)
            )
         )
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
      }

      break;

    default:

      cmd_err = CM_CALL_CMD_ERR_OTHER;
      CM_ERR_1( "Bad call command %d", call_cmd_ptr->cmd);

  }

  return cmd_err;
} /* cmipcall_client_cmd_check */


/*===========================================================================

FUNCTION cmipcall_end_req

DESCRIPTION
  Does call end necessary activities.
  Sends end_req event.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmipcall_end_req(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0( "cmipcall_end_req()");
  CM_ASSERT( call_ptr != NULL );

  /* call state IDLE means call end request already sent
     just return. no need for another call end request event*/
  if(call_ptr->call_state      == CM_CALL_STATE_IDLE)
  {
    return;
  }

  /* save the previous call state */
  call_ptr->prev_call_state = call_ptr->call_state;

  /* Change the call state to idle.*/
  call_ptr->call_state      = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;

  if(call_ptr->cc_object_ptr != NULL)
  {
    return;
  }
  /* Notify clients of call end req event.*/
  cmcall_update_call_end_info( call_ptr );
  cmcall_event( call_ptr, CM_CALL_EVENT_END_REQ );

  /*
  ** Tell phone that Call End procedure has been
  ** started.
  */
  cmph_call_end_req(call_ptr);


}


/*===========================================================================

FUNCTION cmipcall_client_cmd_forward_ll

DESCRIPTION
  Forward a client call commands to Lower Layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a reply from IP App before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmipcall_client_cmd_forward_ll(

    const cm_call_cmd_s_type    *call_cmd_ptr
        /* Pointer to a CM command */
)
{
  const cm_call_cmd_info_s_type    *cmd_info_ptr     = NULL;
      /* Pointer to call cmd info */

  cmipapp_incom_rej_cause_e_type    incom_rej        = CMIPAPP_INCOM_REJ_CAUSE_NONE;
      /* Reject reason for call set up and call answer */

  const cm_call_cmd_info_ip_s_type  *ip_cmd_info_ptr = NULL;


  cmcall_s_type                     *call_ptr        = NULL;
      /* Pointer to a call object */

  int                                i;

  #ifdef  FEATURE_VOIP_E911_SUPPORT
  cmph_s_type                       *ph_ptr          = cmph_ptr();
  #endif

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
  mc_msg_type             *mc_ptr;
  #endif

  cm_ip_call_sip_reject_code_s_type      sip_reject_code ;
      /* Reject reason for call set up and call answer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  memset( &sip_reject_code, 0, sizeof( sip_reject_code ) );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ip_cmd_info_ptr = IPCALL_CMD_INFO_PTR(call_cmd_ptr);
  cmd_info_ptr    = CMD_INFO_PTR(call_cmd_ptr);

  CM_MSG_HIGH_3("IP CALLCMD: cmd=%d, as_id=%d, lte as_id %d",
                 call_cmd_ptr->cmd,
                 call_cmd_ptr->cmd_subs,
                 cmph_get_sub_with_lte_cap());

  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* Response to a setup message*/
                     /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id);

      if (ip_cmd_info_ptr->accept)
      {
        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_NONE;
      }
      else
      {
        incom_rej = CMIPAPP_INCOM_REJ_CAUSE_BUSY;
      }

      if(call_ptr == NULL)
      {
        CM_ERR_1( "Invalid callid: %d", call_cmd_ptr->info.call_id);
      }
      else
      {
        /* Call the IP call funtion for set up response
        */
        cmipcall_invite_res (call_ptr, ip_cmd_info_ptr->accept, incom_rej);
      }

      /* CM_CALL_EVENT_INCOM gets sent at the receipt of a seperate 
      ** indication.
      */      

      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id);

      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      if(ip_cmd_info_ptr->accept)
	  {
        incom_rej= CMIPAPP_INCOM_REJ_CAUSE_NONE;
		/* update call type with user accepted call type
        */
        call_ptr->call_type = call_cmd_ptr->info.call_type;
	  }
	  else
	  {
	  incom_rej = cmcall_end_cause_to_cmipcall_rej_status(ip_cmd_info_ptr->call_reject_reason);
          sip_reject_code = ip_cmd_info_ptr->sip_reject_code;
	  }
	  
      call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmd_info_ptr->ip_call.ipcall_attrib_info;
      if(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param != NULL)
      {
        ref_cnt_obj_add_ref(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
      }
      /* Update called number PI with value received in ANSWER command */
      call_ptr->num.pi = call_cmd_ptr->info.ip_call.pi;

      cmipcall_answer (call_ptr, ip_cmd_info_ptr->accept, incom_rej, sip_reject_code);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*---------------------------*/
                     /* Ending a call              */
                     /*--------------------------- */

    case CM_CALL_CMD_END:
      for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_IP)        
        {
          continue;
        }       
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        call_ptr = cmcall_ptr( call_cmd_ptr->info.end_params[i].call_id);

        if (call_ptr == NULL)
        {
          CM_ERR_1("Invalid call_ptr from call_id %d",
          call_cmd_ptr->info.end_params[i].call_id);

          continue;
        }

        /* If the origination has not been sent to the lower layer,
        ** do not send a call end request to the lower layer.
        */
        if ((call_ptr->call_state == CM_CALL_STATE_ORIG) &&
            (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE))
        {
          break;
        }
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        #ifdef  FEATURE_VOIP_E911_SUPPORT
        /* If the call is an emergency call in origination state 
        ** start the cancel retry timer
        */
        {
          uint8 voip_cancel_retry_period = cmph_get_sub_config_ptr(call_ptr->asubs_id)->voip_cancel_retry_period;
          if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY && 
              call_ptr->call_state == CM_CALL_STATE_ORIG    &&
              voip_cancel_retry_period != 0)
          {
            ph_ptr->voip_cancel_retry_uptime = time_get_uptime_secs() + 
                                      voip_cancel_retry_period;
            CM_MSG_MED_2("voip911: retry period %d, retry timer expiry %d", 
                        voip_cancel_retry_period,
                        ph_ptr->voip_cancel_retry_uptime);
          }
        }
        #endif
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* End the call
        */
		
		#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
         defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)

		/* handle call end after sending HO_EUTRA RSP to 1x */
		if(call_ptr->srvcc_prog_state == CM_SRVCC_STATE_HO_EUTRA_RSP_SENT)
		{
		  mc_ptr  = cm_mc_get_buf_else_err_fatal();

		  mc_ptr->hdr.cmd = MC_SRVCC_CALL_END_F;
	      mc_ptr->srvcc_call_end.call_id = call_ptr->call_id;
		  mc_ptr->srvcc_call_end.end_reason = (byte)(call_ptr->end_params.end_params.ip_cause.cause);

	      cm_mc_send_cmd( mc_ptr );

		}
        #endif
		
        /* To CM cleints, the end_reason would still go as CLIENT_END */
        call_ptr->end_status = (cm_call_end_e_type) call_cmd_ptr->info.end_params[i].end_params.ip_cause.cause;

        /* Inform IMS to end the Call */
        cmipcall_send_end (call_ptr);
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /*---------------------------*/
                      /*   Call related SUPS CMD   */
                      /*---------------------------*/

    case CM_CALL_CMD_SUPS:

      call_ptr = cmcall_ptr (cmd_info_ptr->call_id);          
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "cmd sups: call ptr null");
        break;
      }

      /* ASSERT ON call_ptr not required because cmipcall_sups_cmd_check()
      ** already does the checking !!!!!
      */
     
       if(cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE 
	      || cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_ADD_PARTICIPANT) 
      {
        *(&(call_ptr->cmcall_mode_info.info.ip_info.num)) = *(&(cmd_info_ptr->num));
      }

      if(((cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_HOLD_CALL) ||
        (cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL)) && 
        cmd_info_ptr->is_ip_call_info_present)
      {
        call_ptr->cmcall_mode_info.info.ip_info.ip_sups_param = cmd_info_ptr->ip_call.ip_sups_param;
      }
      cmipcall_proc_sups_cmd_type (cmd_info_ptr, call_ptr);            

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_MO_MODIFY_REQ:
      call_ptr = cmcall_ptr (cmd_info_ptr->call_id); 
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "modify req: call ptr null");
        break;
      }
      call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib = cmd_info_ptr->ip_call.ipcall_attrib_info;
      call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type = cmd_info_ptr->ip_call.new_call_type_req;
      cmipcall_modify_req( call_ptr);     
      break;

    case CM_CALL_CMD_MODIFY_RES:
      call_ptr = cmcall_ptr (cmd_info_ptr->call_id); 
      if(call_ptr == NULL)
      {
        CM_MSG_HIGH_0( "modify rsp: call ptr null");
        break;
      }
      call_ptr->cmcall_mode_info.info.ip_info.call_modify_response = cmd_info_ptr->ip_call.modify_response;
      call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib = cmd_info_ptr->ip_call.ipcall_attrib_info;
      call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type = cmd_info_ptr->ip_call.new_call_type_req;
      cmipcall_modify_rsp( call_ptr);
      break;

    default:

      CM_ERR_1( "Bad call command %d", call_cmd_ptr->cmd);


  }

  return FALSE;
} /* cmipcall_client_cmd_forward_ll () */



/*===========================================================================

FUNCTION cmipcall_client_cmd_ll_reply

DESCRIPTION
  Forward a client call commands to Lower Layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmipcall_client_cmd_ll_reply(

    const cm_call_cmd_s_type    *call_cmd_ptr
        /* Pointer to a CM command */
)
{
  cmcall_s_type                        *call_ptr      = NULL;
      /* Pointer to a call object */

  const cm_call_cmd_info_s_type    *cmd_info_ptr     = NULL;
      /* Pointer to call cmd info */

  int                                   i             = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr  = CMD_INFO_PTR(call_cmd_ptr);

  if (call_cmd_ptr->cmd != CM_CALL_CMD_END)
  {     
    if ((call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id)) == NULL)
    {
      sys_err_fatal_null_ptr_exception();
       /*
       ** 527: Unreachable
       ** This is because for some targets _ERR_FATAL may evaluate with sth
       ** with exit(1) in it but on others error fatals may be turned off
       */
       /*lint -save -e527 */
       return;
       /*lint -restore */
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*---------------------------*/
                     /* Response to a setup message*/
                     /*---------------------------*/

    case CM_CALL_CMD_SETUP_RES:
      /* No fields to copy into call object as of now
      */

      /* if setup was rejected, send end to clients else no event is sent
      */
      if (call_cmd_ptr->info.end_status == CM_CALL_END_SETUP_REJ){

        /* Send call end request for the call
        */
        cmipcall_send_end (call_ptr);
      }

      /* Change incom substate to setup processed.
      */
      call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_SETUP_PROCESSED;      

      cmcall_event (call_ptr, CM_CALL_EVENT_SETUP_RES);
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* Answer an incoming call */
                     /*-------------------------*/

    case CM_CALL_CMD_ANSWER:

      /* Change incom substate to answer processed      
      */
      call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_ANSWERED;


      /* No fields to copy into call object as of now
      */

      if (call_cmd_ptr->info.end_status == CM_CALL_END_INCOM_REJ){

        /* Send call end request for the call
        */
        cmipcall_send_end (call_ptr);

      }
      else
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*-------------------------*/
                     /* End the call            */
                     /*-------------------------*/

    case CM_CALL_CMD_END:

      for ( i=0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* cmipcall should end calls of info_type IP only
        */
        if (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
             != CM_CALL_MODE_INFO_IP)         
        {
          continue;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        
        call_ptr = cmcall_ptr(call_cmd_ptr->info.end_params[i].call_id);

        if (call_ptr == NULL)
        {
          CM_MSG_HIGH_0( "call_ptr_NULL");
        }
        else
        {

          call_ptr->end_status = CM_CALL_END_CLIENT_END;

          *(&(call_ptr->end_params)) = *(&(call_cmd_ptr->info.end_params[i]));

          if( (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE) &&
               (call_ptr->call_state == CM_CALL_STATE_ORIG))
          {
            cmipcall_end_req( call_ptr );   
            cmcall_end( call_ptr );  
          }
          else
          {
            cmipcall_end_req(call_ptr);
          }
        }

      }      

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/         

    case CM_CALL_CMD_SUPS:

      call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id); 
      if(call_ptr==NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        break;
      }

      switch (cmd_info_ptr->sups_cmd_type)
      {         
           
        case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
          
          /* Change incom substate to answered
          */
          call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_ANSWERED;

          cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );

          break;


        default:
          CM_MSG_MED_1("Unknown command type received = %d", 
                     cmd_info_ptr->sups_cmd_type );
          break;
      }

      /* Update call fields by copying relevant
      ** command fields into call struct.
      */
      cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );

      /* Send CM_CALL_EVENT_SUPS to clients
      ** IP call specific fields have not been added to 
      ** call object. sups_type and sups_cmd_params
      ** for ip call are not sent to clients in 
      ** CALL_EVENT_SUPS.
      */
      cmcall_event( call_ptr, CM_CALL_EVENT_SUPS );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/         

    case CM_CALL_CMD_MO_MODIFY_REQ:
      
      cmcall_event( call_ptr, CM_CALL_EVENT_MODIFY_REQ);
      break;

    case CM_CALL_CMD_MODIFY_RES:

      cmcall_event (call_ptr, CM_CALL_EVENT_MODIFY_RES);
      break;

    default:

      CM_ERR_1( "Bad call command %d", call_cmd_ptr->cmd);



  } /* switch () */
} /* cmipcall_client_cmd_ll_reply () */


/*===========================================================================

FUNCTION cmipcall_client_cmd_proc

DESCRIPTION
 Handles call commands for CM IP call interface

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmipcall_client_cmd_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr
    /* pointer to a CM command */

)
{
  cm_call_cmd_err_e_type    call_cmd_err;

  boolean                   wait_for_reply;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_ASSERT( call_cmd_ptr != NULL );

  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Start by checking whether this is a valid command
  ** relative to the current state of the call/phone.
  */
  call_cmd_err = cmipcall_client_cmd_check( call_cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  wait_for_reply = cmipcall_client_cmd_forward_ll( call_cmd_ptr );


  /* If NO need to wait for reply from IP App,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    /* Comes here only when cmd_err is NO_ERR, so need not send
    ** cmd_err as an argument
    */
    cmipcall_client_cmd_ll_reply(call_cmd_ptr);

  }

} /* cmipcall_client_cmd_proc () */

/*===========================================================================

FUNCTION cmipcall_send_orig

DESCRIPTION
  Directs a call origination to a VoIP app after matching an app with the
  right system mode and capability,

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  TRUE : If origination command was succesfully sent to IP APP.
  FALSE: If origination command was not sent to IP APP.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipcall_send_orig
(
  cmcall_s_type           *call_ptr
    /* Call object for the originating call
    */
)
{
  cmipapp_id_type app_id = CMIPAPP_ID_INVALID;
  
  CM_ASSERT (call_ptr != NULL);

  app_id = cmipapp_find_active_app(call_ptr->sr.orig_sent_on_mode,
                                   call_ptr->call_type);
  if(app_id == CMIPAPP_ID_INVALID)
  {
    CM_MSG_HIGH_2("CMIPAPP: No app matching call %d sys_mode %d",
                                          call_ptr->call_type,
                                          call_ptr->sr.orig_sent_on_mode);
    return FALSE;
  }



  if(call_ptr->is_em_vt)
  {
    CM_MSG_HIGH_1("For call_type %d, set em_type to VT",call_ptr->call_type);
    call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.em_type = CM_CALL_EM_VT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Give the origination to an active app
  */
  if(!cmipapp_send_orig_cmd(app_id,
         #ifdef CM_GW_SUPPORTED
         call_ptr->gw_cc_modified_info.num, 
         #else
         call_ptr->cdma_cc_modified_info.num, 
         #endif
         call_ptr->call_id,
         cmipcall_map_cm_calltype_to_ip_calltype(call_ptr->call_type),
         cmipcall_map_sys_mode_to_ip_sys_mode(call_ptr->sr.orig_sent_on_mode),
         &(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info),
         call_ptr->calling_num.pi,
         call_ptr->asubs_id))
  {
    CM_ERR_0 ("Sending origination to app failed");
    return FALSE;
  }
  
  cmcall_check_and_send_mmtel_status(call_ptr,TRUE);

  cmph_send_trm_priority_req(LTE_TRM_PRIORITY_HIGH, call_ptr->asubs_id);

  call_ptr->ipapp_id = cmipapp_find_active_app(call_ptr->sr.orig_sent_on_mode,
                                               call_ptr->call_type);
  return TRUE;
  
} /* cmipcall_send_orig () */


/*===========================================================================

FUNCTION cmipcall_invite_res

DESCRIPTION
  Sends client response to invite received from network.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_invite_res
(
  const cmcall_s_type            *call_ptr,
    /* call object
    */

  boolean                         is_accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for is_accept = TRUE
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!cmipapp_send_invite_response(call_ptr->ipapp_id, call_ptr->call_id,
                                   is_accept, cause, call_ptr->asubs_id))
  {
    CM_ERR_0 ("Sending invite response to app failed");
    return;
  }

} /* cmipcall_invite_res () */


/*===========================================================================

FUNCTION cmipcall_answer

DESCRIPTION
  Sends user response to an incoming call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_answer
(
  const cmcall_s_type                   *call_ptr,
    /* call object
    */

  boolean                         is_accept,
    /* TRUE or FALSE depending on if call is accepted or rejected
    */

  cmipapp_incom_rej_cause_e_type  cause,
    /* causes for rejecting an incoming call. Can be set to
    ** CMIPAPP_INCOM_REJ_CAUSE_NONE for is_accept = TRUE
    */
    
  cm_ip_call_sip_reject_code_s_type      sip_reject_code
    /* SIP code sent for rejecting incoming call*/
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call
  */
  if(!cmipapp_send_answer_cmd(
                 call_ptr->ipapp_id, call_ptr->call_id, is_accept, cause, 
                 cmipcall_map_cm_calltype_to_ip_calltype(call_ptr->call_type),
                 call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info,
                 call_ptr->num.pi,sip_reject_code,
                 call_ptr->asubs_id))

  {
    CM_ERR_2 ("Sending call answer to app failed, call_id %d, cm_call_type=%d", 
              call_ptr->call_id, call_ptr->call_type);
    return;
  }

  /* If sending answer cmd to IMS success process answer cmd on ims Cache.
  */
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)  
    cm_process_answer_cmd_on_ims_cache();
  #endif

} /* cmipcall_answer () */


/*===========================================================================

FUNCTION cmipcall_simple_flash

DESCRIPTION
  Sends user response to an call waiting indication or request by the user to
  remove the last party added.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_simple_flash
(
  const cmcall_s_type                   *call_ptr
    /* call object
    */
)
{
  cm_mm_call_info_s_type     *call_info_ptr = NULL;
    /* Call information pointer */

  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call waiting or
   * user request to remove the last party added.
   */

  if (!cmipapp_send_answer_call_waiting_response(call_ptr->ipapp_id,
                                                 call_ptr->call_id,
                                                 call_ptr->asubs_id))
  {
    CM_MSG_MED_0 ("Sending call wait answer to app failed");

    call_info_ptr = cmcall_info_alloc();

    (void) cmcall_info_get( call_ptr, call_info_ptr );

    if (!cmipapp_send_simple_flash_cmd(call_ptr->ipapp_id, call_ptr->call_id,
                                       call_info_ptr, call_ptr->asubs_id))
    {
      CM_ERR_0 ("Answer to Call wait or remove last party to app failed");
    }

    /* Deallocate the memory after processing call info. */
    cm_mem_free(call_info_ptr);
  }

} /* cmipcall_simple_flash () */


/*===========================================================================

FUNCTION cmipcall_send_end

DESCRIPTION
  Ends a call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_send_end
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to end the call */
  if(!cmipapp_send_call_end_cmd(call_ptr->ipapp_id,
                                call_ptr->call_id,
                                call_ptr->end_status,
                                call_ptr->asubs_id))
  {
    CM_MSG_MED_1 ("Call end request failed, call_id %d", call_ptr->call_id);
    return;
  }

} /* cmipcall_send_end () */


/*===========================================================================

FUNCTION cmipcall_send_abort

DESCRIPTION
  Ends a call

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_send_abort
(
  const cmcall_s_type                  *call_ptr,
    /* call object
    */

  const cmipapp_abort_cause_e_type     abort_cause
    /* The call abort cause enum type.
    */
)
{
    
  CM_ASSERT (call_ptr != NULL);

  /* Request to abort the call
  */
  cmph_send_trm_priority_req(LTE_TRM_PRIORITY_LOW, call_ptr->asubs_id);

  if (!cmipapp_send_abort_call_cmd(call_ptr->ipapp_id, call_ptr->call_id,
                                        abort_cause, call_ptr->asubs_id))
  {
    CM_MSG_MED_0 ("Call abort request failed");
  }
  return;

} /* cmipcall_send_abort() */


/*===========================================================================

FUNCTION cmipcall_end

DESCRIPTION
  Deallocates call object and informs phone of a call end

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Call object gets deallocated.

===========================================================================*/
void cmipcall_end
(
  cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  cm_act_update_reas_e_type update_reason = CM_ACT_UPDATE_REAS_ACT_END;
  boolean mt_call_in_setup = FALSE;
  
  cm_ss_e_type         ss            = CM_SS_NONE;
  sys_modem_as_id_e_type asubs_id    = SYS_MODEM_AS_ID_NONE;

  CM_ASSERT (call_ptr != NULL);

  if ( call_ptr->call_state == CM_CALL_STATE_INCOM &&
  	  (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP ||
  	   call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP_PROCESSED) )
  {
	mt_call_in_setup = TRUE;
  }
  
  cmcall_check_and_send_mmtel_status(call_ptr,FALSE);
	
  /* Change the call state to idle.
  */
  call_ptr->call_state       = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle  = CM_CALL_IDLE_SUBST_NONE;

  /* We need to clear the state to CMIPCALL_SUPS_ST_NONE for the call to be ended.
  */
  cmipcall_mng_state_update(CM_IP_CALL_IND_CALL_END, TRUE, call_ptr->call_id, CM_IPAPP_CAUSE_NORMAL);

  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone now.
  */

  if(call_ptr->end_status == CM_CALL_END_EMERGENCY_ONLY ||
    call_ptr->end_status == CM_CALL_END_THERMAL_EMERGENCY)
  {
    update_reason = CM_ACT_UPDATE_REAS_EMERG_ENTER;
  }

  if(call_ptr->end_cause_to_client != CM_CALL_END_NONE)
  {
    call_ptr->end_status = call_ptr->end_cause_to_client;
    CM_MSG_MED_1("cmipcall_end, reas=%d",call_ptr->end_status);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the queues for this activity
  ** serach the Queues and if the activity is found, update its uptime a
  ** and the update reason
  */


  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id, 
                                 CM_SS_MAIN, 
                                 update_reason );

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id, 
                                 CM_SS_HDR, 
                                 update_reason );
  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_2,
                                 update_reason );
  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_3,
                                 update_reason );


  if(cm_number_of_allocated_calls() == 1) {
    cmph_call_end( call_ptr );
  }

  if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmac_enable_emergency_if_pending();
  }
  /* Notify clients of call end event.
  */
  cmcall_update_call_end_info( call_ptr );
  
  /* Donot send end event, when call waiting for reorig phase2
  ** Also, if we are in INCOM; SETUP/SETUP_PROCESSED substate then we should not send END EVENT
  ** as MT_INCOM event is also not sent
  */
  if(call_ptr->cc_object_ptr == NULL && !mt_call_in_setup)
  {
    cmcall_event( call_ptr, CM_CALL_EVENT_END );
  }

  cmipapp_release_reference_end_reason(call_ptr);
  asubs_id = call_ptr->asubs_id;
  
  cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );

  /* update DDTM status */
  cmph_update_ddtm_status(cmph_ptr());
  
  if(CM_CALL_ID_INVALID == cmcall_is_there_volte_call(CM_CALL_TYPE_VOICE |CM_CALL_TYPE_VT ) &&
     CM_CALL_ID_INVALID == cmcall_is_there_vowlan_call())
  {
    cmph_send_trm_priority_req(LTE_TRM_PRIORITY_LOW, asubs_id);
  }

  /* Process buffered VOPS ind if any */
  #ifdef FEATURE_DOMAIN_SELECTION
  if(cmsds_ptr()->srv_ind_buf_ptr != NULL)
  {
    ss = cmph_determine_ss_per_rat(asubs_id, cmsds_ptr()->srv_ind_buf_ptr->mode );
    CM_MSG_MED_1("cmipcall_end,Process buffered srv ind,SS =%d",ss);
    cmsds_process_srv_ind(ss, cmsds_ptr()->srv_ind_buf_ptr);
  }
  #endif

} /* cmipcall_end */

/*===========================================================================

FUNCTION cmipcall_hold

DESCRIPTION
  Request to put call on hold

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_hold
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to put the call on hold
  */
  if (!cmipapp_send_hold_cmd(call_ptr->ipapp_id, call_ptr->call_id, call_ptr->asubs_id))
  {
    CM_ERR_1 ("Call hold request failed, call_id %d", 
             call_ptr->call_id);
    return;
  }

} /* cmipcall_hold () */


/*===========================================================================

FUNCTION cmipcall_active

DESCRIPTION
  Request to put the call active

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_active
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to make the call active
  */

  if (!cmipapp_send_resume_cmd(call_ptr->ipapp_id, call_ptr->call_id, call_ptr->asubs_id))
  {
    CM_ERR_1 ("Call active request failed, call_id %d", call_ptr->call_id);
  }

  return;

} /* cmipcall_active () */

/*===========================================================================

FUNCTION cmipcall_start_cont_dtmf

DESCRIPTION
  Sends the dtmf for the requested digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_start_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint8                    digit
    /* DTMF digit
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to make the call active
  */
  if(!cmipapp_send_start_inband_dtmf_cmd(call_ptr->ipapp_id,
                                         call_ptr->call_id, digit,
                                         call_ptr->asubs_id))
  {
    CM_ERR_1 ("start dtmf request failed, call_id %d, data_block %d", 
             call_ptr->call_id );
  }

  return;

} /* cmipcall_start_cont_dtmf () */

/*===========================================================================

FUNCTION cmipcall_stop_cont_dtmf

DESCRIPTION
  Sends request to stop dtmf operation for the digit

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_stop_cont_dtmf
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */

  uint16                         duration
    /* DTMF digit duration
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Request to stop dtmf.
  */
  if(!cmipapp_send_stop_inband_dtmf_cmd(call_ptr->ipapp_id, call_ptr->call_id,
                                        duration, call_ptr->asubs_id))
  {
    CM_ERR_1 ("stop dtmf request failed, call_id %d", 
             call_ptr->call_id );
  }

  return;

} /* cmipcall_stop_cont_dtmf () */


/*===========================================================================

FUNCTION cmipcall_cmd_copy_fields

DESCRIPTION
  Copy the active IP fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmipcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

)
{ 
  /* Copy the presentation indicator value */
  call_ptr->calling_num.pi = cmd_ptr->info.calling_num.pi;

  call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info = cmd_ptr->info.ip_call.ipcall_attrib_info;
  if(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param != NULL)
  {
    ref_cnt_obj_add_ref(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
  }

}


/*===========================================================================

FUNCTION cmipcall_modify_req

DESCRIPTION
  send call modify request to IMS

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_modify_req
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call
  */
  if (!cmipapp_send_modify_call_cmd(
         call_ptr->ipapp_id, call_ptr->call_id, 
         cmipcall_map_cm_calltype_to_ip_calltype(
           call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type),
         call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib,
         call_ptr->asubs_id))
  {
    CM_ERR_2 ("Sending call modify req to app failed, call_id %d, cm_call_type=%d", 
              call_ptr->call_id,
              call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type);
    return;
  }
  
  CM_MSG_HIGH_3 ("Sending IP modify req, call_id  %d, req_call_type %d,\
                current_call_type = %d", call_ptr->call_id,
                 call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type,
                 call_ptr->call_type);

} /* cmipcall_modify_req () */



/*===========================================================================

FUNCTION cmipcall_modify_rsp

DESCRIPTION
  send call modify response to IMS

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_modify_rsp
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call
  */
 if ( !cmipapp_send_modify_response(
        call_ptr->ipapp_id, call_ptr->call_id, 
        call_ptr->cmcall_mode_info.info.ip_info.call_modify_response,
        cmipcall_map_cm_calltype_to_ip_calltype(
          call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type),
        call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib,
        call_ptr->asubs_id))
  {
    CM_ERR_1("Sending call modify rsp to app failed, call_id %d", 
                                                   call_ptr->call_id);
    return;
  }

} /* cmipcall_modify_rsp () */

/*===========================================================================

FUNCTION cmipcall_remove_last_party

DESCRIPTION
  send request to IMS to remove last party from conference

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_remove_last_party
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the user response about answering the call
  */
  if (!cmipapp_send_remove_last_party_cmd(
                                call_ptr->ipapp_id,
                                call_ptr->cmcall_mode_info.info.ip_info.num,
                                call_ptr->asubs_id))
  {
    CM_ERR_1("Sending remove last party to app failed, call_id %d", 
             call_ptr->call_id);
    return;
  }

} /* cmipcall_remove_last_party () */


/*===========================================================================

FUNCTION cmipcall_add_new_party

DESCRIPTION
  send request to IMS to add a new party to conference

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_add_new_party
(
  const cmcall_s_type           *call_ptr
    /* call object
    */
)
{
  CM_ASSERT (call_ptr != NULL);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the client cmd to add a new party
  */
  if (!cmipapp_send_add_new_party_cmd(
                                call_ptr->ipapp_id,
								call_ptr->call_id,
                                call_ptr->cmcall_mode_info.info.ip_info.num,
                                call_ptr->asubs_id))
  {
    CM_ERR_1 ("Sending add new party to app failed, call_id %d", 
             call_ptr->call_id);
    return;
  }

} /* cmipcall_add_new_party () */

/*===========================================================================

FUNCTION cmipcall_rat_change_rpt_proc

DESCRIPTION
  Process rat change indication from IMS and notify clients.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmipcall_rat_change_rpt_proc (
  const cmipapp_rpt_s_type  *rpt_ptr
)
{
  cmcall_s_type *call_ptr = NULL;
  
  sys_sys_mode_e_type curr_sys_mode;
  cm_ss_e_type         ss            = CM_SS_NONE;
  
  CM_ASSERT( rpt_ptr != NULL );
  if (rpt_ptr == NULL)
  {
    CM_ERR_0("rpt_ptr null");
    return;
  }
  
  CM_MSG_MED_3("IP RXD: RAT_CHANGE id=%d, sys_mode=%d, as_id=%d",
              rpt_ptr->ip_ind.ind.call_rat_change.id,
              rpt_ptr->ip_ind.ind.call_rat_change.sys_mode,
              rpt_ptr->ip_ind.hdr.asubs_id);

  /* Get the call pointer from the call ID 
   */
  call_ptr = cmcall_ptr ( rpt_ptr->ip_ind.ind.call_rat_change.id);
  
  if (call_ptr == NULL)
  {
    CM_ERR_0    ("ERROR: Unknown call ID in call_rat_change");    
    CM_ASSERT ( call_ptr != NULL );
    return;
  }

  /* Indicate call end in previous sys mode first */
  cmcall_check_and_send_mmtel_status(call_ptr,FALSE);

  /* Update sys_mode based on the rpt
   */
   
  curr_sys_mode = call_ptr->sys_mode ;
  
  call_ptr->sys_mode 
      = cmipapp_map_sysmode_to_cm_sysmode (
          rpt_ptr->ip_ind.ind.call_rat_change.sys_mode);
		  
  /* Update VOWLAN flag when LTE to WLAN HO happens*/
  if(curr_sys_mode ==  SYS_SYS_MODE_LTE && call_ptr->sys_mode == SYS_SYS_MODE_WLAN)
  {
    call_ptr->is_vowlan = TRUE;
	call_ptr->is_volte  = FALSE;

  /*If VOLTE call HO to WLAN, no need to buffer VOPS. kick start domain sel*/
    #ifdef FEATURE_DOMAIN_SELECTION
    if(cmsds_ptr()->srv_ind_buf_ptr != NULL)
    {
      ss = cmph_determine_ss_per_rat(rpt_ptr->ip_ind.hdr.asubs_id, cmsds_ptr()->srv_ind_buf_ptr->mode );
      cmsds_process_srv_ind(ss, cmsds_ptr()->srv_ind_buf_ptr);
    }
    #endif

  }
  /*  WLAN to LTE HO, update volte flag */
  else if (curr_sys_mode ==  SYS_SYS_MODE_WLAN && call_ptr->sys_mode == SYS_SYS_MODE_LTE)
  {
    call_ptr->is_vowlan = FALSE;
    call_ptr->is_volte  = TRUE;
  }
  
  /* Now, indicate call start on the latest handed over sys mode */
  cmcall_check_and_send_mmtel_status(call_ptr,TRUE);

  CM_MSG_MED_3("call id %d moved from %d to  %d",
                call_ptr->call_id , curr_sys_mode,call_ptr->sys_mode );
  

  /* Notify clients 
   */
  cmcall_event ( call_ptr, CM_CALL_EVENT_RAT_CHANGE);

  return;  
}



/*===========================================================================

FUNCTION cmipcall_process_call_deflection

DESCRIPTION
  send request to IMS to handle communication deflection

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmipcall_process_call_deflection
(
  const cmcall_s_type           *call_ptr,
    /* call object
    */
  const cm_call_cmd_info_s_type    *cmd_info_ptr
)
{
  CM_ASSERT (call_ptr != NULL);
  CM_ASSERT (cmd_info_ptr != NULL);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send command to IMS to handle communication deflection on
  ** incoming call
  */
  if (!cmipapp_send_call_deflection_cmd(
                                call_ptr->ipapp_id,
                                call_ptr->call_id,
                                cmd_info_ptr->num,
                                call_ptr->asubs_id))
  {
    CM_ERR_1 ("Sending communication deflection to app failed, call_id %d", 
             call_ptr->call_id);
    return;
  }

} /* cmipcall_process_call_deflection () */



#else /* FEATURE_IP_CALL */

/*===========================================================================
FUNCTION cmipcall_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmipcall_dummy( void )
/*lint -esym(714,cmipcall_dummy) */
/*lint -esym(765,cmipcall_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif /* FEATURE_IP_CALL */

