
/*            C A L L   M A N A G E R   CSFB C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for LTE CSFB call.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmcall_init
    Initializing the call object.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmcall_client_cmd_proc
    Process clients' call commands.

  cmcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmcsfbcall.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/13   fj        Add support for DSDX L+G
05/16/13   dj      Fix to unforce the 1xcsfb call end preferences.
09/21/12   vk      FIX to unreserve connection id in CSFB mode.
05/21/12   vk      FIX to clear call context at 1XCP upon hard failure.
05/10/12   vk      FIX to disbale LTE, if CSFB status is negative for CS_PREF
                   voice config.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/02/12   rk      correcting the debug statements
01/24/12   vk      Channges related to handling of RLF cause code in 1XCSFB call.
09/15/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_PROT_CMD" related changes.
09/14/11   rk      NV items N_min_MO_call_soft_retry and N_maxSIB8
09/08/11   vk      Treating "ONEX_REJ_1XCSFB_HO_FAIL_ACQ_FAIL" as SOFT failure
09/02/11   rk      S102 less 1XCSFB
06/11/11   rk      Send CSFB abort req command to NAS if user ended the call
                   while 1XCP is CSFB activated state
05/26/11   rk      Treat HO fail with reason CALL_REL_REORDER as hard failure
05/10/11   sv      In case of 1XCSFB call silent redial fix to hold the call
                   when SRV is not full service
04/05/11   rk      Support for CSFB abort on getting release order/reorder
04/05/11   rk      Correcting CSFB call silent redial behavior
03/31/11   mp      Made esm_msg buffer to use from heap to avoid stack overflow
03/31/11   rk      Handle CSFB response messages received after call end
                   gracefully.
03/30/11   rk      revising handling of failure cases
03/22/11   rk      Support for handling negative scenarios
12/10/10   rk      Adding support for MT CSFB call
10/22/10   rk      Sucessful MO call initial support

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cmcsfbcall.h"
#if defined(FEATURE_CM_LTE)
#include "cm_esm.h"                /* Inteface to ESM */
#include "cmidbg.h"                /* Interfce for CM_ASSERT */
#include "esm.h"                   /* Interface to NAS-ESM */
#include "esm_cm_msgr_enum_msg.h"  /* Interface to UMID */

#if defined (FEATURE_LTE_TO_1X)
#include "lte_irat_types.h"
#include "cmsds.h"
#include "mmtask_v.h"
#endif

#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */

#include <string.h>    /* Interface to string library */
#include "rex.h"       /* Interface to rex services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */

#include "cmcall.h"    /* Interface to CM call object */
#include "cmwcall.h"   /* Interface to GW info in call object */

#include "cmss.h"
#include "cmutil.h"

#include "mmoc.h"
#include "mmoc_v.h"

#include "cmltecall.h"
#include "cmregprx.h"
#include "cmi.h"
#include "cmcc.h"
#include "cmtaski.h"
#include "sys_gw_v.h"
#include "cmlog_v.h"
#include "cmmmgsdi.h"
#include "cmxcall.h"

#if (defined(T_PC) && defined(CM_UNIT_TEST))
#error code not present
#endif /* (defined(T_PC) && defined(CM_UNIT_TEST)) */

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmmsc.h"
#include "cmsimcoord.h"
#include "cmdrvcc.h"
#include "cmemgext.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions - external:
** --------------------------------------------------------------------------
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
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_ptr->csfb_type = CM_CALL_CSFB_TYPE_NONE;
  #if defined (FEATURE_LTE_TO_1X)
  call_ptr->sr.esr_retry_counter = 0;
  #endif

}

#endif /* ( FEATURE_3GPP_CSFB) || (FEATURE_LTE_TO_1X)  */

#if defined (FEATURE_LTE_TO_1X)

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
void cmcsfbcall_send_orig_req (

        const cmcall_s_type    *call_ptr
            /* pointer to a call object */
    )
{
  esm_msg                      *client_request;


  CM_ASSERT( call_ptr != NULL );


  /*Below Assert is a defensive check for handling 1xcsfb cases only .
  ** Below other cases doesnt need this check , since we didnt SD get ok-to-orig yet .
  ** and we didnt have updated   orig_sent_on_mode yet .
  ** 1) 1X voice/911 call originated when UE operating 1xsrllte mode .
  ** 2) WPS voice/911 call originated in hvolte mode .
  */
  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)      
  if ((cm_is_esr_allowed_in_srlte() == FALSE
      || cmph_is_oprting_in_1xsrlte_mode(call_ptr->asubs_id) == FALSE
      || (call_ptr->call_type != CM_CALL_TYPE_VOICE
          && call_ptr->call_type != CM_CALL_TYPE_EMERGENCY))
      &&
      cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr) == FALSE)
  #endif      
  {
    CM_ASSERT( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  client_request = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  memset(client_request,0,sizeof(esm_msg));

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->esm_1xCSFB_call_req.csfb_call_id = call_ptr->call_id;
  if ( (call_ptr->direction == CM_CALL_DIRECTION_MO) && 
       ((call_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_ptr->call_type == CM_CALL_TYPE_PD)) 
      )
  {
    
    #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    if(cm_is_esr_allowed_in_srlte())
    {
      /* for 1XSRLTE SVLTE mode  */
      client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_1xCSFB;
    } 
    else if(cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr)
            && cmcall_is_esr_allowed_in_hvolte())
    {
       /*send ESR when WPS call orig while operating in single mode (or)
       **or call failed on Volte , silent redial on 1x
       */
       client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_1xCSFB;
       client_request->esm_1xCSFB_call_req.partial_esr = TRUE;
    } 
    else 
    #endif      
    if(cmsds_is_dual_rx_1xcsfb_enabled())
    {
       /* If dual Rx 1XCSFB enabled send new type to NAS  */
       client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_S102_LESS_1XCSFB;
    } 
    else
    {
      /*MO CSFB normal voice call */
      client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_1xCSFB;
    }
  }
  else if ( (call_ptr->direction == CM_CALL_DIRECTION_MT) && 
            ((call_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_ptr->call_type == CM_CALL_TYPE_PD))
    	  )
  {
    /*MT CSFB normal voice call */
    client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MT_CSFB_1XCSFB;
  }
  else if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    /*If CSFB Type is not none , that means 
    ** we will send the call as legacy 1xcsfb call , Hence partial_esr is not needed .
    ** Also partial_esr is needed , only when UE is in single mode .
    */
    if(call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE
       && cmph_is_oprting_in_csfb_pref_csfb_sub_mode(call_ptr->asubs_id))
    {
        /*send ESR when WPS call orig while operating in single mode (or)
        **or call failed on Volte , silent redial on 1x
        */
        client_request->esm_1xCSFB_call_req.partial_esr = TRUE;
    }      
    /* CSFB emergency  call */
    client_request->esm_1xCSFB_call_req.csfb_service_type =
                                      LTE_MO_CSFB_ECALL_1XCSFB_ECALL;
  }

  CM_MSG_HIGH_3( "CM->MSGR: NAS_ESM_1XCSFB_CALL_REQ. Call ID %d  \
                 CSFB Srv Type %d \
                 partial ESR %d",
                 client_request->esm_1xCSFB_call_req.csfb_call_id,
                 client_request->esm_1xCSFB_call_req.csfb_service_type,
                 client_request->esm_1xCSFB_call_req.partial_esr);

  (void) cm_msgr_send( NAS_ESM_1XCSFB_CALL_REQ, MSGR_MM_CM,
                       (msgr_hdr_s *)client_request, sizeof(esm_msg) );

  /* Free allocated ESM message buffer */
  cm_mem_free (client_request);

}/* cmcsfbcall_send_orig_req */


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
void cmcsfbcall_send_orig_req_call_obj_not_allocated (

        const cm_call_id_type                    call_id
            /* pointer to a call object */
    )
{
  esm_msg                      *client_request;

  CM_MSG_LOW_0("START cmcsfbcall_send_orig_req_call_obj_not_allocated()" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  client_request = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->esm_1xCSFB_call_req.csfb_call_id = call_id;

  if(call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB1 ||
     call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB2)
  {
    /*CMEMG E911 call */
    client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_ECALL_1XCSFB_ECALL;

    /*if UE is operating in CSFB mode , then mark it as partial esr */    
    if(cmph_is_oprting_in_csfb_pref_csfb_sub_mode(SYS_MODEM_AS_ID_1))
    {
      client_request->esm_1xCSFB_call_req.partial_esr = TRUE;      
    } 
  } 
  else if(call_id == CM_MT_UNALLOCATED_CALL_ID)
  {
  /*MT CSFB normal voice call */
  client_request->esm_1xCSFB_call_req.csfb_service_type = LTE_MT_CSFB_1XCSFB;
  } 


  CM_MSG_HIGH_1( "CM->MSGR: SEND: NAS_ESM_1XCSFB_CALL_REQ. CSFB Call ID %d",
                 client_request->esm_1xCSFB_call_req.csfb_call_id );
  (void) cm_msgr_send( NAS_ESM_1XCSFB_CALL_REQ, MSGR_MM_CM,
                       (msgr_hdr_s*)client_request, sizeof(esm_msg) );

  /* Free allocated ESM message buffer */
  cm_mem_free (client_request);

}/* cmcsfbcall_send_orig_req_call_obj_not_allocated */

/*===========================================================================

FUNCTION cmcsfbcall_send_redial_req_for_mt_call

DESCRIPTION
  This API is used to send command to MC to retain MT call context
  before CM dactivates 1x from 1xCSFB mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcsfbcall_send_redial_req_for_mt_call (

        const cm_call_id_type                    call_id
            /* pointer to a call object */
    )
{
  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
  /* allocate a MC command buffer */

  CM_ASSERT( mc_ptr != NULL );

  mc_ptr->hdr.cmd = MC_CSFB_RETAIN_MT_PAGE_CXT_F;
  mc_ptr->csfb_call_clear.call_id = call_id;

  /* Send the command to MC to retain MT call context
  */
  CM_MSG_HIGH_1( "1XCSFB:Send MC_CSFB_RETAIN_MT_PAGE_CXT_F to 1XCP for call id =%d",\
                 call_id );
  cm_mc_send_cmd( mc_ptr );

}/* cmcsfbcall_send_redial_req_for_mt_call */



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
 void cmcsfbcall_send_csfb_abort_req (

        const cm_call_id_type                    call_id
            /* pointer to a call object */
    )
{
  esm_msg                      *client_request;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  client_request = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  client_request->esm_1xCSFB_abort_req.csfb_call_id = call_id;

  CM_MSG_HIGH_1("CM->MSGR: SEND: NAS_ESM_1XCSFB_ABORT_REQ for call_id = %d ",call_id);
  (void) cm_msgr_send( NAS_ESM_1XCSFB_ABORT_REQ, MSGR_MM_CM,
                       (msgr_hdr_s*)client_request, sizeof(esm_msg) );

  /* Free allocated ESM message buffer */
  cm_mem_free (client_request);

}/* cmcsfbcall_send_csfb_abort_req */


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
void cmcsfbcall_1xcsfb_call_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( call_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if(call_ptr->call_state == CM_CALL_STATE_ORIG)
  {
    cmlog_call_orig_end(call_ptr->end_status);
  }


  /* Call is ending due to acquisition failure,
  ** call was waiting for sim cc till now,
  ** so calling callback here with no srv error*/
  if(call_ptr->cc_object_ptr != NULL)
  {
    cm_cmd_type              *cmd_ptr;
    cmd_ptr = (cm_cmd_type *) call_ptr->cc_object_ptr->user_data;

    /* freeing call cmd here, if ending from call control state*/
    if(cmd_ptr != NULL)
    {
      cmcall_client_cmd_err( CALL_CMD_PTR(cmd_ptr), CM_CALL_CMD_ERR_SRV_STATE_S );
      CM_MSG_HIGH_0("cmcall_end, CM_CALL_CMD_ERR_SRV_STATE_S");
      cm_cmd_dealloc( cmd_ptr );
    }
  }



  /* Change the call state to idle.
  */
  call_ptr->call_state = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;
  call_ptr->end_req_wait_uptime  = CMCALL_UPTIME_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the priority queue for this activity
  */

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_MAIN,
                                 CM_ACT_UPDATE_REAS_ACT_END );

  (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HDR,
                                 CM_ACT_UPDATE_REAS_ACT_END );


  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone
     now. */
  if(cm_number_of_allocated_calls() == 1) {
     cmph_call_end( call_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Notify clients of call end event for MO call.
  */
  if ( call_ptr->direction == CM_CALL_DIRECTION_MO )
  {
    if(call_ptr->cc_object_ptr == NULL)
    {
    cmcall_update_call_end_info( call_ptr );
    cmcall_event( call_ptr, CM_CALL_EVENT_END );
    }
  }

  /* Unreserve the connection ID */
  cmcall_unreserve_connection_id(call_ptr);

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
  {
    cmcall_drvcc_check_and_abort_drvcc_setup(call_ptr);
  }
  #endif /* defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) */

  /* Deallocate the call object
  */
  cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );


  #ifdef FEATURE_MMGSDI_CDMA
  /* If there are no more calls, then the traffic channel must be down */
  if (cmcall_is_there_a_cdma_call_in_conv () == CM_CALL_ID_INVALID)
  {
    cmcall_update_gsdi_and_log_call_state(CM_SS_MAIN);
  }
  #endif

} /* cmcsfbcall_1xcsfb_call_end() */


/*===========================================================================

FUNCTION cmcsfbcall_send_1xcsfb_call_clear

DESCRIPTION
  Sends CSFB CALL CLEAR command to MC

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
    )
{


  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
      /* allocate a MC command buffer */

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( mc_ptr != NULL );

  CM_ASSERT( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA);

  mc_ptr->hdr.cmd = MC_CSFB_CALL_CLEAR_F;
  mc_ptr->csfb_call_clear.call_id = call_ptr->call_id;

  /* Send the origination command to MC and set orig_sent_on_mode to TRUE
  */

  CM_MSG_HIGH_1( "1XCSFB:Send MC_CSFB_CALL_CLEAR_F to 1XCP for call id =%d",\
                 call_ptr->call_id );
  cm_mc_send_cmd( mc_ptr );

}/* cmcsfbcall_send_1xcsfb_call_clear */



/*===========================================================================

FUNCTION cmcsfbcall_send_1xcsfb_call_end_req

DESCRIPTION
  Sends CSFB CALL END command to MC

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
    )
{


  mc_msg_type    *mc_ptr  = cm_mc_get_buf_else_err_fatal();
      /* allocate a MC command buffer */

  if (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA)
  {
    /* Update the call state */
    call_ptr->call_state = CM_CALL_STATE_IDLE;
    call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;
    cmcall_event( call_ptr, CM_CALL_EVENT_END_REQ );

    CM_ASSERT( call_ptr != NULL );
    CM_ASSERT( mc_ptr != NULL );

    mc_ptr->hdr.cmd = MC_CSFB_CALL_END_F;
    mc_ptr->csfb_call_end.call_id = call_ptr->call_id;
 
    /* Send the origination command to MC and set orig_sent_on_mode to TRUE
    */

    CM_MSG_HIGH_1( "1XCSFB:Send MC_CSFB_CALL_END_F to 1XCP for call id =%d",\
                 call_ptr->call_id );
    cm_mc_send_cmd( mc_ptr );
  }
  else
  {
    cmcsfbcall_1xcsfb_call_end(call_ptr);
  }

}/* cmcsfbcall_send_1xcsfb_call_end_req */



/*===========================================================================

FUNCTION cmcall_map_onexrej_cc_rej

DESCRIPTION
  map the reject cause

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static byte cmcall_map_onexrej_cc_rej (byte onex_rej_value)
{
  switch ((lte_irat_1xcsfb_ho_failure_cause_e)onex_rej_value)
  {

    case LTE_IRAT_1XCSFB_HO_FAILURE_MSG_IGNORE:
     return ONEX_REJ_1XCSFB_HO_FAIL_MSG_IGNORE;

    case LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL:
     return ONEX_REJ_1XCSFB_HO_FAIL_ACQ_FAIL;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REL_ORDER:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_REL_ORDER;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REORDER:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_REORDER;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_INTERCEPT_ORDER:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_INTERCEPT_ORDER;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_NORMAL;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_SO_REJ:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_SO_REJ;

    case LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_OTASP_SPC_ERR:
     return ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_OTASP_SPC_ERR;

    case LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD:
     return ONEX_REJ_1XCSFB_HO_FAIL_MSG_INVAILD;
    default:
     CM_MSG_HIGH_1("Invalid onex rej_value %d",onex_rej_value);
     return ONEX_REJ_1XCSFB_HO_FAIL_MSG_INVAILD;
  }
} /*cmcall_map_onexrej_cc_rej */


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
void cmcsfbcall_handle_srv_ind (

const sys_srv_status_e_type   srv_type

)
{
  cmcall_s_type   *call_ptr = NULL;
  cm_call_id_type call_id = CM_CALL_ID_INVALID;
  dword           uptime    = time_get_uptime_secs();

  call_id = cmcall_is_there_csfb_call(CM_SS_MAIN,
               CM_CALL_CSFB_TYPE_1XCSFB, CM_CALL_ID_INVALID);

  if (call_id != CM_CALL_ID_INVALID)
  {
    call_ptr = cmcall_ptr(call_id);

    if(call_ptr == NULL)
    {
      CM_MSG_HIGH_1("Unable to retrive the call object for CALL_ID %d", call_id);
      return;
    }

    if(call_ptr->is_rlf_in_1xcsfb_call == TRUE)
    {
      CM_MSG_HIGH_1("RLF: Handle SRV_IND/CNF for 1XCSFB call,srv_type=%d",srv_type);

      if (srv_type == SYS_SRV_STATUS_NO_SRV)
      {
        /* Update call end status to hard failure so that call can be
        ** originated in legacy CDMA mode if SR period is not yet expired.
        */

        /* Deactivate 1x protocol from 1XCSFB mode*/
        cmsds_check_and_deact_csfb_mode ();

        call_ptr->end_status = CM_CALL_END_1XCSFB_HARD_FAILURE;
        call_ptr->sr.hold_orig_uptime = uptime;

        cmcall_orig_proc(call_ptr);
      }

      call_ptr->is_rlf_in_1xcsfb_call = FALSE;
    }
  }
}

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
)
{
  cmcall_s_type            *call_ptr;
      /* Point at call object */

  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
      /* Call ID */

   cdma_cmcall_type        *call_mode_info_ptr;
      /* Pointer to call mode info */
   boolean  is_soft_failure = FALSE;

   dword                  uptime = time_get_uptime_secs() ;

   cmsds_s_type           *sds_ptr = cmsds_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */

  switch( cm_rpt_ptr->msg_hdr.id )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_1XCSFB_CALL_RSP:
    {

      /* 1XCSFB call confirmation received */
      CM_MSG_HIGH_4("1XCSFB_CALL_RSP:call_id = %d rej present = %d,rej_type = %d,rej_value = %d",\
                   cm_rpt_ptr->cm_1xcsfb_call_rsp.csfb_call_id,
                   cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.present,\
                   cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                   cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

      call_id = cm_rpt_ptr->cm_1xcsfb_call_rsp.csfb_call_id;

      /* call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB1 or
      ** call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB2 will be used by cmcsfbcall_rpt_proc  ,
      ** to determine if the ESR request was due to CMEMG E911 call .
      ** we needed this since we didnt have call obj when CMEMG E911 call is originated */
      if ( call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB1 ||
           call_id == CM_UNALLOCATED_CALL_ID_CMEMG_SUB2)
      {
        cmemg_handle_esr_rsp(call_id);
        return;
      }

      #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)

      /* CM_MT_UNALLOCATED_CALL_ID is used in 1xSRLTE where a temp. call id
         is used to do ESR before accepting 1x MT call pages. So irrespective
         of type of 1XCSFB_CALL_RSP, 1x MT pages are given go-ahead */
      if ( call_id == CM_MT_UNALLOCATED_CALL_ID &&
           cm_is_esr_allowed_in_srlte() &&
           cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1) &&
           cmph_ptr()->is_1x_mt_call_pending)
      {
        /*MT normal voice call made 1xsrlte-svlte mode */
        cmxcall_mc_incoming_page_proceed(TRUE);
        return;
      }
      #endif

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );
      
      if( call_ptr == NULL )
      {
        CM_MSG_HIGH_0("1XCSFB:so skip processing  CSFB RSP as call ended");
        return;
      }

      if(cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1) &&
         call_ptr->csfb_type != CM_CALL_CSFB_TYPE_1XCSFB &&
         call_ptr->is_waiting_on_esr_rsp)
      {
        /* process ESR response for case when 
        ** 1. camped on 1x+LTE dual stack in SRLTE 
        ** 2. 1x voice call is originated.
        ** 3. ESR request sent to NAS
        ** 4. ESR response ts recieved , send call origination request to 1x.
        */
        if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
        {
          cmxcall_send_orig(call_ptr);
          call_ptr->is_waiting_on_esr_rsp = FALSE;
        }
        /* process ESR response for case when 
        ** 1. camped on 1x+LTE dual stack in SRLTE , no E911 support on LTE
        **       (i.e call not being dialed on hybr2)
        ** 2. E911 call is originated.
        ** 3. ESR request sent to NAS
        ** 4. ESR response is recieved , force the preferences.
        */
        else if(call_ptr->asubs_id == SYS_MODEM_AS_ID_1
                && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY
                && call_ptr->ss != CM_SS_HYBR_2)
        {
          call_ptr->is_waiting_on_esr_rsp = FALSE;
          cmcall_force_mode_and_send_event(call_ptr);
        }
        /* process ESR response for case when 
        ** 1. camped on 1x+LTE dual stack in SRLTE 
        ** 2. GSM voice call is originated on sub2.
        ** 3. ESR request sent to NAS
        ** 4. ESR response is recieved , send call origination request to NAS.
        */
        else
        {
        cmmsimc_proc_rpt_esr_rsp(cm_rpt_ptr->cm_1xcsfb_call_rsp.csfb_call_id);               
        }
        return;
      }	  
        
      /* process ESR response for case when 
      ** 1.camped on LTE in single stack in SRLTE-HVOLTE
      ** 2. voice call fails over IMS and being redialled over 1x, or a WPS call is originated.
      ** 3. ESR request sent to NAS
      ** 4. ESR response ts recieved , process and force 1x.
      */
      if( cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr))
      {
        call_ptr->is_waiting_on_esr_rsp = FALSE;
        CM_MSG_HIGH_1("e911_esr %d ",call_ptr->sr.orig_sent_on_mode);
        //silent redial case
        if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
        {
          if(cmcall_act_get(call_ptr) == CMCALL_ACT_FORCE_NON_IP)
          {
            cmcall_remove_mode_and_force_and_sched_sr(call_ptr);
          }
          else if(cmcall_act_get(call_ptr) == CMCALL_ACT_HARD_FAILURE)
          {
            cmcall_redial_after_hard_failure_and_sched_sr(call_ptr);
          }
        }       
        //user sends WPS call case
        else
        {
          cmcall_force_mode_and_send_event(call_ptr);
        }
        return;
      }
      
      if ((call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)&&
          (call_ptr->call_state == CM_CALL_STATE_IDLE))
      {
        CM_MSG_HIGH_0("1XCSFB:Ignore CSFB call response due CALL_END substate");
        return;
      }

      CM_MSG_HIGH_2("call_ptr->as_id=%d,cm_1xcsfb_call_rsp.msg_hdr.inst_id=%d",
                 call_ptr->asubs_id, cm_rpt_ptr->cm_1xcsfb_call_rsp.msg_hdr.inst_id);

      call_ptr->asubs_id = INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_1xcsfb_call_rsp.msg_hdr.inst_id);

      call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );

      call_mode_info_ptr->cc_reject.rej_type = cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type;

      if(call_mode_info_ptr->cc_reject.rej_type == ONEX_CSFB_REJECT_CAUSE)
      {
        call_mode_info_ptr->cc_reject.rej_value =
        cmcall_map_onexrej_cc_rej (cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);
      } else
      {
        call_mode_info_ptr->cc_reject.rej_value = cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value;
      }

      if(call_ptr->direction == CM_CALL_DIRECTION_MT)
      {
        CM_MSG_HIGH_3("MT_REDIAL:rej_cause:%d rej_value:%d is_mtcsfb_redial:%d", 
		 	                 call_mode_info_ptr->cc_reject.rej_type, 
		 	                 call_mode_info_ptr->cc_reject.rej_value,
		 	                 sds_ptr->is_mtcsfb_call_redial_supported);
        /* for MT 1xCSFB call, when ESR is rejected with rej_value as 
        ** AS_REJ_LRRC_LOWER_LAYER_FAILURE, do the following:
        ** 1.send MC_CSFB_RETAIN_MT_PAGE_CXT_F to 1x so that 1x.CP will not clear
        **   the call context.
        ** 2.Deactivate 1x from CSFB mode.
        ** 3.Remove G/W/L/T from call's orig_mode and force preferences to SD so 
        **   that SD will deactivate LTE stack and activate 1x in native mode.
        ** Once,we activate 1x in native mode, 1x will send MT call indication again, 
        ** with same call_id, to redial the call
        */
        if( sds_ptr->is_mtcsfb_call_redial_supported == TRUE &&
            call_mode_info_ptr->cc_reject.rej_type == AS_REJECT_CAUSE &&
            call_mode_info_ptr->cc_reject.rej_value == AS_REJ_LRRC_LOWER_LAYER_FAILURE)
        {
          cmph_s_type* ph_ptr = cmph_ptr();
          cm_orig_q_s_type *ph_orig_ptr    = NULL;
          cmmsc_proc_cmd_pref_sys_chgd_s_type  cmd;

          CM_MSG_MED_3("MT_REDIAL:call_id:%d call_state:%d, call_subst:%d", 
                    call_ptr->call_id,call_ptr->call_state, call_ptr->call_subst.idle);
          ph_orig_ptr = cmtask_orig_para_search_act_id(call_ptr->ss,
                                                (cm_act_id_type) call_ptr->call_id );
		  
	  cmcsfbcall_send_redial_req_for_mt_call(call_ptr->call_id);
 
          call_ptr->call_state = CM_CALL_STATE_IDLE;
          call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;
          call_ptr->end_req_wait_uptime  = CMCALL_UPTIME_NONE;;
		   
          /* Deactivate 1x protocol from 1XCSFB mode*/
          cmsds_check_and_deact_csfb_mode ();

          if(ph_orig_ptr != NULL)
          {
            ph_orig_ptr->orig->orig_mode_pref = cm_remove_mode_pref_components(
                                   ph_ptr->main_stack_info.pref_info.mode_pref, 7,
                                   SD_SS_MODE_PREF_TDS,
                                   SD_SS_MODE_PREF_GSM,
                                   SD_SS_MODE_PREF_WCDMA,
                                   SD_SS_MODE_PREF_LTE,
                                   SD_SS_MODE_PREF_GW,
                                   SD_SS_MODE_PREF_GWL,
                                   SD_SS_MODE_PREF_AMPS
                                   );
            ph_orig_ptr->orig->orig_mode = SD_SS_ORIG_MODE_NORMAL;

            CM_MSG_HIGH_2("call fav mode pref %d call_type =%d ", ph_orig_ptr->orig->orig_mode_pref, call_ptr->call_type);
            cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue (
                                                   call_ptr->ss,
                                                   SD_SS_PREF_REAS_ORIG_START_CS,
                                                   ph_orig_ptr->orig->orig_mode,
                                                   FALSE,
                                                   ph_orig_ptr,
                                                   CM_ACT_UPDATE_REAS_NONE,
                                                   &cmd
                                                  );

            cmmsimc_proc_cmd_pref_sys_chgd(&cmd);
          }/* if(ph_orig_ptr != NULL) */

          return;
        }/* if( sds_ptr->is_mtcsfb_call_redial_supported == TRUE ...*/
        cmcsfbcall_send_1xcsfb_call_clear( call_ptr);

        if (call_mode_info_ptr->cc_reject.rej_value != ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_SO_REJ)
        {
          cmcsfbcall_send_csfb_abort_req(call_ptr->call_id);
        }

        switch ( call_mode_info_ptr->cc_reject.rej_type)
        {
        case EMM_REJECT_CAUSE:
          {
            switch (call_mode_info_ptr->cc_reject.rej_value)
            {
            default:
              is_soft_failure = FALSE;
              break;
            } /* switch ((lte_nas_local_cause_enum_T)call_mode_info_ptr->cc_reject.rej_value) */
          }
          break;
        case ONEX_CSFB_REJECT_CAUSE:
          {
            switch (call_mode_info_ptr->cc_reject.rej_value)
            {
            case ONEX_REJ_1XCSFB_HO_FAIL_ACQ_FAIL:
            case ONEX_REJ_1XCSFB_HO_FAIL_MSG_INVAILD:
              is_soft_failure = TRUE;

              break;
            default:
              is_soft_failure = FALSE;
              break;

            } /* switch ((lte_irat_1xcsfb_ho_failure_cause_e)call_mode_info_ptr->cc_reject.rej_value) */
          }
          break;

        case AS_REJECT_CAUSE:

          switch (call_mode_info_ptr->cc_reject.rej_value)
          {
          case AS_REJ_LRRC_RADIO_LINK_FAILURE:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
          case AS_REJ_LRRC_CONN_EST_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL:
          case AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE:
          case AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH:
          case AS_REJ_LRRC_CONN_REL_NORMAL:
          case AS_REJ_LRRC_CONN_REL_RLF:
          case AS_REJ_LRRC_CONN_REL_CRE_FAILURE:
          case AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE:
          case AS_REJ_LRRC_CONN_REL_ABORTED:
          case AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR:
          case AS_REJ_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS:
            {
              /* Treat is as soft failure */
              is_soft_failure = TRUE;
            }
            break;

          default:
            {
              CM_ERR_2("1XCSFB:Received unexpected rej_type = %d,rej_value = %d",\
                cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

              is_soft_failure = FALSE;
            }
            break;

          }
          break;
        case OTA_REJECT_CAUSE:
        case MM_REJECT_CAUSE:
        case CNM_MN_REJECT_CAUSE:
        default:
          {
            CM_ERR_2("1XCSFB:Received unexpected rej_type = %d,rej_value = %d",\
              cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
              cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

            /* Check if the call is MT call*/
            is_soft_failure = FALSE;

          }
          break;
        }
        if(is_soft_failure == FALSE)
        {
          call_ptr->end_status=CM_CALL_END_1XCSFB_HARD_FAILURE;
        }
        else
        {
          call_ptr->end_status=CM_CALL_END_1XCSFB_SOFT_FAILURE;
        }
        call_ptr->cmcall_mode_info.info_type=CM_CALL_MODE_INFO_CDMA;
        cmcall_event( call_ptr, CM_CALL_EVENT_MT_CALL_PAGE_FAIL);
        cmcsfbcall_1xcsfb_call_end(call_ptr);
        return;
      }

      switch ( call_mode_info_ptr->cc_reject.rej_type)
      {
        case EMM_REJECT_CAUSE:
        {
          switch (call_mode_info_ptr->cc_reject.rej_value)
          {
            case EMM_REJ_TIMER_T3417_EXP:
            case EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT:
            {
              is_soft_failure = FALSE;
            }
            break;

            default:
            {
              CM_ERR_2("1XCSFB:Received unexpected rej_type = %d,rej_value = %d",\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

              is_soft_failure = FALSE;
            }
            break;

          } /* switch ((lte_nas_local_cause_enum_T)call_mode_info_ptr->cc_reject.rej_value) */
        }
        break;
        case ONEX_CSFB_REJECT_CAUSE:
        {
          switch (call_mode_info_ptr->cc_reject.rej_value)
          {

            case ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_NORMAL:
            {
              /* Release order is received with NORMAL reason
              ** send call clear to 1XCP and end the call
              */
              cmcsfbcall_send_1xcsfb_call_clear( call_ptr);
              cmcsfbcall_send_csfb_abort_req(call_ptr->call_id);
              cmcsfbcall_1xcsfb_call_end(call_ptr);
            }
            return;

            case ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_SO_REJ:
            {
              if (call_ptr->direction == CM_CALL_DIRECTION_MO &&
                  call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
              {
                /* Release order is received with SO REJ
                ** treat it as hard failure.TBD: is rety count needed?.
                */
                is_soft_failure = TRUE;
                call_ptr->sr.esr_retry_counter++;
              } else
              {
                cmcsfbcall_send_1xcsfb_call_clear( call_ptr);
                cmcsfbcall_1xcsfb_call_end(call_ptr);
                return;
              }
            }
            break;

            case ONEX_REJ_1XCSFB_HO_FAIL_ACQ_FAIL:
            case ONEX_REJ_1XCSFB_HO_FAIL_MSG_INVAILD:
            {
              cmcsfbcall_send_csfb_abort_req(call_ptr->call_id);
              is_soft_failure = TRUE;
              call_ptr->sr.esr_retry_counter++;
            }
            break;
            case ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_REORDER:
            {
              is_soft_failure = FALSE;
            }
            break;

            case ONEX_REJ_1XCSFB_HO_FAIL_MSG_IGNORE:
            case ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_OTASP_SPC_ERR:
            case ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_INTERCEPT_ORDER:
            default:
            {
              CM_ERR_2("1XCSFB:Received unexpected/not supported rej_type = %d,rej_value = %d",\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

              is_soft_failure = FALSE;
            }
            break;

          } /* switch ((lte_irat_1xcsfb_ho_failure_cause_e)call_mode_info_ptr->cc_reject.rej_value) */
        }
        break;

       case AS_REJECT_CAUSE:

        switch (call_mode_info_ptr->cc_reject.rej_value)
        {
          case AS_REJ_LRRC_LOWER_LAYER_FAILURE:
          {
            /* Check if the call is MO call*/
            is_soft_failure = FALSE;
          }
          break;

          case AS_REJ_LRRC_RADIO_LINK_FAILURE:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF:
          case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
          case AS_REJ_LRRC_CONN_EST_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL:
          case AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE:
          case AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE:
          case AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH:
          {
            is_soft_failure = TRUE;
            call_ptr->sr.esr_retry_counter++;
            /* NAS will perform ATTACH till then CM has to wait so
            ** making ok to orig to FALSE
            */
            cmcall_update_ok_to_orig (call_ptr, FALSE);
            CM_MSG_HIGH_0("1XCSFB: Updating ok_to_orig to FALSE");
          }
          break;

          case AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED:
          case AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
          case AS_REJ_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH:
          case AS_REJ_LRRC_CONN_EST_FAILURE_CONN_REJECT:
          {
            /* Check if the call is MO call*/
            is_soft_failure = FALSE;
          }
          break;

          case AS_REJ_LRRC_CONN_REL_NORMAL:
          case AS_REJ_LRRC_CONN_REL_RLF:
          case AS_REJ_LRRC_CONN_REL_CRE_FAILURE:
          case AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE:
          case AS_REJ_LRRC_CONN_REL_ABORTED:
          case AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR:
          case AS_REJ_LRRC_CONN_REL_ABORTED_IRAT_SUCCESS:
          {
            /* Treat is as soft failure */
            is_soft_failure = TRUE;
            call_ptr->sr.esr_retry_counter++;
          }

          break;

          default:
           {
            CM_ERR_2("1XCSFB:Received unexpected rej_type = %d,rej_value = %d",\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

             is_soft_failure = FALSE;
           }
           break;

        }
        break;
       case OTA_REJECT_CAUSE:
       case MM_REJECT_CAUSE:
       case CNM_MN_REJECT_CAUSE:
       default:
        {
          CM_ERR_2("1XCSFB:Received unexpected rej_type = %d,rej_value = %d",\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_type,\
                     cm_rpt_ptr->cm_1xcsfb_call_rsp.cm_cc_reject.rej_value);

          /* Check if the call is MT call*/
          is_soft_failure = FALSE;

        }
        break;


      } /*switch ( call_mode_info_ptr->cc_reject.rej_type) */

      /* Process soft and hard CSFB call origination failures */
      if ( call_ptr->call_state == CM_CALL_STATE_ORIG )
      {
        /* If we are trying to originate, and CSFB origination failed
        ** update the end status depending on if the failure is soft or hard
        ** and if ESR retry counter threshold is reached or not.
        */

        CM_MSG_HIGH_3("1XCSFB:esr_retry_count=%d,sr_start_uptime=%d,uptime=%d",\
                      call_ptr->sr.esr_retry_counter,
                      call_ptr->sr.sr_start_uptime,uptime );

        if( (call_ptr->sr.esr_retry_counter <
                             sds_ptr->n_min_MO_call_soft_retry)  &&
           (( uptime < call_ptr->sr.sr_start_uptime +
            CM_CSFB_CALL_MAX_SR_TIME_SOFT_FAIL_RETRIES ) ) &&
            is_soft_failure == TRUE )
         {
           /* Update call end status to soft failure so that ESR can be retried
           ** if SR period is not yet expired.
           */
           cmcsfbcall_send_1xcsfb_call_clear( call_ptr);

           call_ptr->end_status = CM_CALL_END_1XCSFB_SOFT_FAILURE;

           CM_MSG_HIGH_1("1XCSFB: Start processing SOFT_FAILURE  n_min_MO_call_soft_retry = %d",
                         sds_ptr->n_min_MO_call_soft_retry);
         }
         else
         {
           /* Update call end status to hard failure so that call can be
           ** originated in legacy CDMA mode if SR period is not yet expired.
           */
           CM_MSG_HIGH_1("1XCSFB: Start processing HARD_FAILURE n_min_MO_call_soft_retry = %d",
                        sds_ptr->n_min_MO_call_soft_retry);
           cmcsfbcall_send_1xcsfb_call_clear( call_ptr );

           /* Deactivate 1x protocol from 1XCSFB mode*/
           cmsds_check_and_deact_csfb_mode ();

           call_ptr->end_status = CM_CALL_END_1XCSFB_HARD_FAILURE;
           call_ptr->sr.hold_orig_uptime = time_get_uptime_secs();
         }
         call_ptr->sr.orig_fail_layer = CM_CALL_ORIG_ERR_LAYER_1XCSFB;
         call_ptr->prot_end_status = call_ptr->end_status; /* report the latest end status from N/W */
         cmcall_orig_proc(call_ptr);
      }

    } /* case MM_CM_1XCSFB_CALL_RSP */
    break;

    case MM_CM_1XCSFB_ABORT_RSP:
     CM_MSG_HIGH_2("1XCSFB: Received MM_CM_1XCSFB_ABORT_RSP for call_id=%d,inst_id%d",
                 cm_rpt_ptr->cm_1xcsfb_abort_rsp.csfb_call_id,
                 cm_rpt_ptr->cm_1xcsfb_abort_rsp.msg_hdr.inst_id);

     #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
     if ( cm_is_esr_allowed_in_srlte() &&
        cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1))
     {
       CM_MSG_HIGH_0("clear 1XSRLTE call");
       cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();
     }
     else if(cmph_is_msim_standby_esr_enabled())
     {
       cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req();      
     }
     #endif
     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR_1( "Wrong msg id = %d",cm_rpt_ptr->msg_hdr.id);
      break;

  } /* switch */


} /* cmcsfbcall_rpt_proc */


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
boolean cmcsfbcall_send_mm_dual_rx_1xcsfb_tau_req(void)

{

  #ifdef FEATURE_CM_LTE
  mm_cmd_type                   *mm_ptr;
      /* MM command pointer */

    mm_ptr = mm_cm_get_cmd_buf(MMCM_DUAL_RX_1XCSFB_TAU_REQ);
    if (mm_ptr == NULL)
    {
      CM_ERR_0("MMCM_DUAL_RX_1XCSFB_TAU_REQ: out of MM cmd buffer");
      return FALSE;
    }

    mm_put_cmd( mm_ptr );

    CM_MSG_HIGH_0("Send MMCM_DUAL_RX_1XCSFB_TAU_REQ to MM");

    return TRUE;
  #endif
}

/*===========================================================================

FUNCTION cmcsfbcall_unforce_csfb_call_end_preferences

DESCRIPTION
  Unforces the CSFB call preferences.

DEPENDENCIES

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
void cmcsfbcall_unforce_csfb_call_end_preferences(void)
{

  cm_orig_q_s_type      *q_ptr;
  cm_orig_q_s_type      *q_next_ptr;
  dword                  uptime         = time_get_uptime_secs();
  cm_mode_pref_e_type    call_mode_pref = CM_MODE_PREF_NONE;
  boolean                is_unforce_req = FALSE;
  q_ptr = cmtask_orig_para_get_first(CM_SS_MAIN);

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next(q_ptr, CM_SS_MAIN);
    call_mode_pref = q_ptr->orig->orig_mode_pref;

    CM_MSG_HIGH_3(" Orig params act_update_reas=%d, orig_mode=%d mode_pref=%d",\
                  q_ptr->orig->act_update_reas, q_ptr->orig->orig_mode,call_mode_pref);

    if ((q_ptr->orig->act_update_reas == CM_ACT_UPDATE_REAS_ACT_END)&&
        ((q_ptr->orig->orig_mode == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG) ||
         (q_ptr->orig->orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG))&&
        (cmcall_misc_is_mode_pref(call_mode_pref,CM_MODE_PREF_LTE_ONLY) == FALSE)
       )
    {
      q_ptr->orig->orig_mode_uptime = uptime;
      is_unforce_req                = TRUE;
      CM_MSG_HIGH_0(" Setting uptime to current time");
    }

    q_ptr = q_next_ptr;
  }

  if (is_unforce_req == TRUE)
  {
    cmph_check_and_unforce_orig_mode(CM_SS_MAIN);
  }
}

/*===========================================================================

FUNCTION cmcsfbcall_unforce_hard_failure_preferences

DESCRIPTION
  Unforces the 1X CSFB call preferences.

DEPENDENCIES

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
void cmcsfbcall_unforce_hard_failure_preferences(void)
{

  cm_orig_q_s_type      *q_ptr;
  cm_orig_q_s_type      *q_next_ptr;
  dword                  uptime         = time_get_uptime_secs();
  cm_mode_pref_e_type    call_mode_pref = CM_MODE_PREF_NONE;
  boolean                is_unforce_req = FALSE;
  q_ptr = cmtask_orig_para_get_first(CM_SS_MAIN);

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next(q_ptr, CM_SS_MAIN);
    call_mode_pref = q_ptr->orig->orig_mode_pref;

    CM_MSG_HIGH_3(" Orig params act_update_reas=%d, orig_mode=%d mode_pref=%d",\
                  q_ptr->orig->act_update_reas, q_ptr->orig->orig_mode,call_mode_pref);

    if ((q_ptr->orig->act_update_reas == CM_ACT_UPDATE_REAS_ACT_END)&&
        ((q_ptr->orig->orig_mode == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG) ||
         (q_ptr->orig->orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG))
       )
    {
      q_ptr->orig->orig_mode_uptime = uptime;
      is_unforce_req                = TRUE;
      CM_MSG_HIGH_0(" Setting uptime to current time");
    }

    q_ptr = q_next_ptr;
  }

  if (is_unforce_req == TRUE)
  {
    cmph_check_and_unforce_orig_mode(CM_SS_MAIN);
  }
}

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

void cmcsfbcall_end_csfb_calls_with_end_wait_sub_state(void)
{

  cm_iterator_type      call_itr;
  cmcall_s_type         *call_ptr;
  /* Point to call object */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /* Ending the call with substate 'end-wait' */
  while(call_ptr != NULL)
  {
    if ( (call_ptr->call_id != CM_CALL_ID_INVALID) &&
         (call_ptr->call_state == CM_CALL_STATE_IDLE) &&
         (call_ptr->csfb_type  == CM_CALL_CSFB_TYPE_1XCSFB) &&
         (call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
       )
    {
      cmcsfbcall_1xcsfb_call_end(call_ptr);
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

}

/*===========================================================================

  This is utility function to send NAS_ESM_1XCSFB_CALL_REQ for ESR. Call id in
  the argument has id of the call because of which ESR on LTE being done

===========================================================================*/

void cmcsfbcall_send_esr_req( uint8 call_id )
{
  esm_msg *req;

  req = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));
  req->esm_1xCSFB_call_req.csfb_call_id = call_id;
  req->esm_1xCSFB_call_req.csfb_service_type = LTE_MO_CSFB_1xCSFB;

  CM_MSG_HIGH_1("CM->MSGR: 1XCSFB_CALL_REQ call_id = %d",call_id);

 (void) cm_msgr_send( NAS_ESM_1XCSFB_CALL_REQ, MSGR_MM_CM,
                     (msgr_hdr_s *)req, sizeof(esm_msg) );

  /* Free allocated ESM message buffer */
  cm_mem_free(req);
  
}

#endif /* FEATURE_LTE_TO_1X  */

#endif /* FEATURE_CM_LTE */
