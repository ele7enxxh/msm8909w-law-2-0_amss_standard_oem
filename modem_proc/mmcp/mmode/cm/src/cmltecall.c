/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   LTE   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for LTE.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.


EXTERNALIZED FUNCTIONS

  cmltecall_info_get
    Copy the current call state information into a specified buffer.

  cmltecall_client_cmd_proc
    Process clients' call commands.

  cmltecall_rpt_proc
    Process Lower Layer call commands.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmltecall.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/22/13   fj        Add support for DSDX L+G
08/29/13   jvo     Added check for NULL to fix static analysis error
08/13/13   sm      Midcall SRVCC feature
06/25/13   ss      Added condition to NOT transfer call if in call control
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
03/29/13   fj      End call on GW during LTE->GW IRAT. 
02/14/13   th      Ext IMS SRVCC cancel notification ind support
02/02/13   xs      Delete priority queue without forcing after LTe call end if there
                   is 1 more data call in queue
01/02/13   xs      Fix issue of not sending IMS SRVCC complete ind when alerting call
12/21/12   xs      Fix issue of not sending IMS SRVCC failure ind
12/03/12   xs      Added SRVCC Start indication support
11/18/12   aj      Pass emerg bearer info from DS to NAS
09/28/12   fj      Added a new call end_status.
09/26/12   skk     Changes of ESM SM backoff timer requirements
08/03/12   xs      Use correct ss info when notify GSDI of call event
04/03/12   sg      Updated cmltecall_map_lte_info_to_gw_info for TDS <-> LTE IRAT
03/22/12   sg      Enable WRLF functionality for TD-SCDMA RAT
12/21/11   xs      L->W SRVCC support
11/16/11   jh      Replace cmcall CTXT_TRANSFER event to cmss event
10/14/11   jh      Handling DEACT_BEARER_CONTEXT_REQUEST after IRAT to GW
09/09/11   jh      Handling DRB_RELEASE_IND after IRAT to GW
09/09/11   xs      Notify client of LTE call connect event after DS accepts ACT BEARER
                   request
08/25/11   jh      Do not transfer the GW call if it's pending call.
07/26/11   jh      Marking GW connection IDs on LTE Activate bearer request
                   Adding additional logs for connection_ids
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/19/11   jh      Revert previous changes for reserve conn id on ACT_BEARER_REQ
07/12/11   fj      Don't end LTE call when sys_sel_pref change
06/30/11   fj      Add support to reserve local GW connection id if
                   ACT_BEARER_REQ contains valid gsm_umts_connection_id.
06/24/11   rk      Unreserve the GW connection ID as part of  LTE PS call end
                   if GW connection was reserved.
06/10/11   fj      Set srv_type to LTE when LTE call starts.
05/17/11   fj      Update connection_id in lte_info during GWL IRAT.
04/13/11   fj      During GWL IRAT, if GW call orig is not sent to NAS, end
                   the call when processing call object transfer indication.
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/23/11   mp      Made esm_msg to use from heap to avoid stack overflow
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
03/01/11   fj      Update esm cause when receives deactivate request.
03/01/11   gm      Thermal emergency support
02/25/11   fj      Check original info_type before do call object transfer.
02/04/11   fj      Update eps_bearer_id and rb_id based on DRB_REESTAB cmd.
02/03/11   fj      During IRAT, search both old RAT and target RAT connection
                   id for reject responses.
02/01/11   fj      Add F3 messages.
01/27/11   fj      After sending the call object transfer event to DS,
                   change the info_type back to GW_PS.
01/27/11   jqi     Fixed the compilation errors.
01/12/11   fj      Add new indication for call object transfer during IRAT.
12/23/10   sv      Integrate DSDS feature
12/17/10   gm      Changed user_data parameter to const type in CC callback.
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/02/10   gm      Fix to KW errors.
10/28/10   gm      Fix to compilation warning.
09/23/10   xs      Superlint critical/high error fixed
09/14/10   np      Added LTE mode for gsdi in traffic channel
08/30/10   fj      Add support for LTE call control.
08/27/10   fj      Set connection_id to 0xFF for UE requests.
08/02/10   fj      Add call state CM_CALL_STATE_CONV.
07/12/10   fj      Keep the connection_id not resetting in IRAT mapping functions.
07/06/10   gm      Fix for Klocwork deferred critical errors.
07/01/10   fj      Revert temp solution in CL#1330955.
06/14/10   gm      Fix to KW/Lint errors.
06/01/10   fj      Temp solution for handling DRB_SETUP_IND before SERVICE_IND
                   during W to L redirection.
06/09/10   gm      Fix for Klocwork errors.
05/27/10   fj      Added esm_local_cause in cm_res_mod_s_type.
05/26/10   fj      Add cmltecall_call_end().
05/26/10   fj      Map primary_pdp and group_id in GW to L and L to GW mapping
                   functions.
05/21/10   fj      Modify reset in GW to L and L to GW mapping functions.
05/20/10   fj      Remove call_id from cm_lte_call_info_s_type.
05/17/10   fj      Add support for GW to LTE call transfer.
05/12/10   fj      Enhance info copy.
05/10/10   fj      Fixed TFT copy for Bearer Modification Reqeust.
05/06/10   fj      Modified function using call by reference instead of call
                   by value.
04/29/10   fj      Copy apn_ambr in cmltecall_info_get().
04/27/10   fj      Temp change. Will be removed when DS stops checking for
                   call_id in lte_info.
04/16/10   fj      Added support for LTE to GW call object transfer.
02/19/10   fj      Removed act_bearer_rsp from cm_lte_call_info_s_type.
02/05/10   fj      Added apn_ambr in act_default_bearer_context request and
                   modify_bearer_context_request.
01/20/10    fj     Merged changelist 987678.
12/17/09    fj     Merged change from LTE only branch.
12/16/09    fj     Merged change from LTE only branch.
11/03/09    np     Added esm local cause for PDN Disconnect Request
11/12/09    fj     Fixes for CFG_LTE.
11/05/09    fj     For LTE calls, set connection id src to ESM in cmltecall.c
10/28/09    np     Removed lte_assert.h
10/16/09    np     Replace LTE_ASSERT with CM_ASSERT
10/15/09    np     Added $Header: in history
10/14/09    sv     Updating UMID def's to use MM tech area.
08/31/09    fj     Added support for LTE UE initiate call functionality.
09/11/09    fj     Added rb_id in DRB_SETUP_IND.
08/10/09    fj     Added handling for MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND
07/27/09    fj     Modified cmltecall_send_act_bearer_context_rsp(), since
                   added esm_cause and esm_local_cause in the response.
06/19/09    np     Added DRB Reestablisment reject indication processing
06/17/09    fj     Added cmltecall_reset();
                   added update for sdf_id, lbi, bearer_type fields in
                   cmltecall_info_get();
                   added MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND
                   handling in cmltecall_update_call_ptr();
                   added dedicated bearer handling in
                   cmltecall_send_act_bearer_context_rsp();
                   added MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND
                   handling in cmltecall_rpt_proc().
06/02/09    fj     Modified cmltecall_update_call_ptr(), since ESM moves lbi
                   into esm_hdr.
05/28/09    fj     Added lbi copy in cmltecall_update_call_ptr();
                   modified eps_bearer_id, connection_id, and sdf_id copy in
                   cmltecall_update_call_ptr().
04/29/09    fj     Modified ASSERT value.
04/17/09    fj     Fixed lint warnings.
03/25/09    fj     Added utility functions to copy pdn address, apn address,
                   tft and protocol config options.
03/02/09    np     Added LTE Network Initiated Dormancy

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_CM_LTE)
#include "cm_esm.h"                /* Inteface to ESM */
#include "cmidbg.h"                /* Interfce for CM_ASSERT */
#include "esm.h"                   /* Interface to NAS-ESM */
#include "esm_cm_msgr_enum_msg.h"  /* Interface to UMID */

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

#include "cmltecall.h"
#include "cmregprx.h"

#include "cmcc.h"
#include "cmtaski.h"

#if (defined(T_PC) && defined(CM_UNIT_TEST))
#error code not present
#endif /* (defined(T_PC) && defined(CM_UNIT_TEST)) */

#ifdef CM_DEBUG
#error code not present
#endif


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

/*===========================================================================

FUNCTION cmltecall_copy_pdn_address

DESCRIPTION
  Copy the pdn_address from source to destination structure.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_copy_pdn_address(

    pdn_address_T                 *dest_pdn_address,
        /* Pointer to the destination PDN_address */

    const pdn_address_T           *src_pdn_address
        /* Pointer to the source PDN_address  */
)
{
  dest_pdn_address->valid          = src_pdn_address->valid;

  if (src_pdn_address->valid)
  {
    *dest_pdn_address = *src_pdn_address;
  }

} /* cmltecall_copy_pdn_address */



/*===========================================================================

FUNCTION cmltecall_copy_protocol_config_options

DESCRIPTION
  Copy the protocol_config_options from source to destination structure.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void cmltecall_copy_protocol_config_options(

    protocol_cfg_options_T                 *dest_cfg_options,
        /* Pointer to the destination cfg_options */

    const protocol_cfg_options_T           *src_cfg_options
        /* Pointer to the source cfg_options  */
)
{
  dest_cfg_options->valid              = src_cfg_options->valid;

  if (src_cfg_options->valid)
  {
    *dest_cfg_options = *src_cfg_options;
  }

} /* cmltecall_copy_protocol_config_options */



/*===========================================================================

FUNCTION cmltecall_copy_apn_address

DESCRIPTION
  Copy the apn_address from source to destination structure.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_copy_apn_address(

    apn_T                 *dest_apn_address,
        /* Pointer to the destination apn_address */

    const apn_T           *src_apn_address
        /* Pointer to the source apn_address  */
)
{
  dest_apn_address->valid          = src_apn_address->valid;

  if (src_apn_address->valid)
  {
    *dest_apn_address = *src_apn_address;
  }

} /* cmltecall_copy_apn_address */




/*===========================================================================

FUNCTION cmltecall_copy_tft_type

DESCRIPTION
  Copy the tft_type from source to destination structure.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_copy_tft_type(

    tft_type_T                 *dest_tft,
        /* Pointer to the destination tft */

    const tft_type_T           *src_tft
        /* Pointer to the source tft  */
)
{
  dest_tft->valid              = src_tft->valid;

  if (src_tft->valid)
  {
    *dest_tft = *src_tft;
  }

} /* cmltecall_copy_tft_type */



/*===========================================================================

FUNCTION cmltecall_reset

DESCRIPTION
  Reset LTE-specific call fields to default values reflecting
  a LTE call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_reset(

    lte_cmcall_type              *lte_mode_ptr
        /* pointer to the LTE mode info */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( lte_mode_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(lte_mode_ptr, 0, sizeof(lte_cmcall_type));

  lte_mode_ptr->bearer_type           = CM_LTE_BEARER_TYPE_NONE;

}



/*===========================================================================

FUNCTION cmltecall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmltecall_info_get(

    cm_mm_call_info_s_type        *call_info_ptr,
        /* Copy call state info into this buffer */

    const cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
)
{
  const lte_cmcall_type        *lte_mode_ptr = NULL;
  cm_lte_call_info_s_type      *lte_info_mode_ptr  = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  lte_mode_ptr          = &call_ptr->cmcall_mode_info.info.lte_info;
  lte_info_mode_ptr     = &call_info_ptr->mode_info.info.lte_call;

  call_info_ptr->asubs_id = call_ptr->asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_ptr->cmcall_mode_info.info_type )
    {
    case CM_CALL_MODE_INFO_LTE:
      lte_info_mode_ptr->eps_bearer_id    = lte_mode_ptr->eps_bearer_id;
      lte_info_mode_ptr->rb_id            = lte_mode_ptr->rb_id;
      lte_info_mode_ptr->connection_id    = lte_mode_ptr->connection_id;
      lte_info_mode_ptr->sdf_id           = lte_mode_ptr->sdf_id;
      lte_info_mode_ptr->lbi              = lte_mode_ptr->lbi;
      lte_info_mode_ptr->bearer_type      = lte_mode_ptr->bearer_type;

      *(&(lte_info_mode_ptr->sdf_qos)) = *(&(lte_mode_ptr->sdf_qos));

      lte_info_mode_ptr->neg_qos.valid    = lte_mode_ptr->neg_qos.valid;
      if (lte_mode_ptr->neg_qos.valid)
      {
        *(&(lte_info_mode_ptr->neg_qos)) = *(&(lte_mode_ptr->neg_qos));
      }

      cmltecall_copy_pdn_address(&(lte_info_mode_ptr->pdn_addr),
                                 &(lte_mode_ptr->pdn_addr));

      cmltecall_copy_protocol_config_options(
        &(lte_info_mode_ptr->config_options),
        &(lte_mode_ptr->config_options));

      cmltecall_copy_apn_address(&(lte_info_mode_ptr->apn_name),
                                 &(lte_mode_ptr->apn_name));

      lte_info_mode_ptr->esm_cause.valid  = lte_mode_ptr->esm_cause.valid;
      if (lte_mode_ptr->esm_cause.valid)
      {
        lte_info_mode_ptr->esm_cause.esm_cause  =
          lte_mode_ptr->esm_cause.esm_cause;
      }

      lte_info_mode_ptr->esm_local_cause.valid=
                                  lte_mode_ptr->esm_local_cause.valid;
      if (lte_mode_ptr->esm_local_cause.valid)
      {
        lte_info_mode_ptr->esm_local_cause.local_cause=
                                  lte_mode_ptr->esm_local_cause.local_cause;
      }
      memscpy(&lte_info_mode_ptr->emm_failure_cause, 
              sizeof(cm_emm_failure_type),
              &lte_mode_ptr->emm_failure_cause,
              sizeof(cm_emm_failure_type));

      lte_info_mode_ptr->profile_number   = lte_mode_ptr->profile_number;
      lte_info_mode_ptr->pdp_group_id     = lte_mode_ptr->pdp_group_id;

      cmltecall_copy_tft_type(&(lte_info_mode_ptr->ul_dl_tft),
                              &(lte_mode_ptr->ul_dl_tft));

      lte_info_mode_ptr->neg_llc_sapi.valid   =
        lte_mode_ptr->neg_llc_sapi.valid;
      if (lte_mode_ptr->neg_llc_sapi.valid)
      {
        lte_info_mode_ptr->neg_llc_sapi.sapi  =
          lte_mode_ptr->neg_llc_sapi.sapi;
      }

      lte_info_mode_ptr->apn_ambr.valid=lte_mode_ptr->apn_ambr.valid;
      if (lte_mode_ptr->apn_ambr.valid)
      {
        *(&(lte_info_mode_ptr->apn_ambr)) = *(&(lte_mode_ptr->apn_ambr));
      }

      /* Update gsm_umts_connection_id for LTE to GW call object
      ** transfer.
      */
      lte_info_mode_ptr->gsm_umts_connection_id.valid = \
        lte_mode_ptr->gsm_umts_connection_id.valid;
      if (lte_mode_ptr->gsm_umts_connection_id.valid)
      {
        lte_info_mode_ptr->gsm_umts_connection_id.connection_id = \
          lte_mode_ptr->gsm_umts_connection_id.connection_id;
      }

      lte_info_mode_ptr->backoff_timer.active = \
	  	  lte_mode_ptr->backoff_timer.active;
      lte_info_mode_ptr->backoff_timer.timer_count= \
				lte_mode_ptr->backoff_timer.timer_count;

      break;

    default:
      CM_ERR_1("Wrong call mode = %d",call_ptr->cmcall_mode_info.info_type);
      break;
    }

  return ((boolean)( CM_INIT_CHECK(call_ptr->init_mask) ));
}

/*===========================================================================

FUNCTION cmltecall_update_call_ptr

DESCRIPTION
  Copy the current call state information into the call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_update_call_ptr(

    cmcall_s_type           *call_ptr,
        /* Pointer to the call object  */

    const cm_esm_cmd_u_type *cm_rpt_ptr
        /* Pointer to the report message */

)
{
  lte_cmcall_type        *lte_mode_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_ptr == NULL )
  {
    CM_ERR_0 ("parameter call_ptr = NULL");
    return;
  }

  lte_mode_ptr    =  &(call_ptr->cmcall_mode_info.info.lte_info);

  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_LTE;
  call_ptr->call_type                  = CM_CALL_TYPE_PS_DATA;
  call_ptr->direction                  = CM_CALL_DIRECTION_MT;
  /* Set the connection id src to ESM */
  cm_set_connection_id_src(call_ptr,CONN_ID_ESM);

  switch( cm_rpt_ptr->msg_hdr.id )
  {
    case MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND:
      
    CM_MSG_MED_5("DEFAULT_BEARER_CONTEXT_REQ: eps_bearer_id %d conn_id %d call_id %d eps_bearer_id %d bearer_type %d",
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.eps_bearer_id,
                 cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.connection_id,
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

      call_ptr->asubs_id             = 
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
        cm_act_default_bearer_context_request_ind.msg_hdr.inst_id);

      /* Determine cm_ss base on LTE rat and asubs_id */
      call_ptr->ss = cmph_determine_ss_per_rat(call_ptr->asubs_id, SYS_SYS_MODE_LTE);

      lte_mode_ptr->eps_bearer_id    =
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.eps_bearer_id;
      lte_mode_ptr->connection_id    =
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.\
        connection_id;
      lte_mode_ptr->sdf_id    =
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.sdf_id;
      lte_mode_ptr->lbi    =
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.lbi;

      *(&(lte_mode_ptr->sdf_qos)) = *(&(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.sdf_qos));

      if (cm_rpt_ptr->cm_act_default_bearer_context_request_ind.neg_qos.valid)
      {
        *(&(lte_mode_ptr->neg_qos)) = *(&(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.neg_qos));
      }
      else
      {
        lte_mode_ptr->neg_qos.valid = FALSE;
      }

      cmltecall_copy_pdn_address(
        &(lte_mode_ptr->pdn_addr),
        &(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.pdn_address));

      cmltecall_copy_apn_address(
        &(lte_mode_ptr->apn_name),
        &(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.apn_name));

      cmltecall_copy_protocol_config_options(
        &(lte_mode_ptr->config_options),
        &(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.\
          protocol_config_options));

      lte_mode_ptr->esm_cause.valid =
    cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_cause.\
    valid;
      if (cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_cause.\
          valid)
      {
        lte_mode_ptr->esm_cause.esm_cause =
          cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_cause.\
          esm_cause;
      }

      if (cm_rpt_ptr->cm_act_default_bearer_context_request_ind.ul_tft.valid)
      {
        *(&(lte_mode_ptr->ul_dl_tft)) = *(&(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.ul_tft));
      }
      else
      {
        lte_mode_ptr->ul_dl_tft.valid = FALSE;
      }

      if (cm_rpt_ptr->cm_act_default_bearer_context_request_ind.neg_llc_sapi.\
          valid)
      {
        *(&(lte_mode_ptr->neg_llc_sapi)) = *(&(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.neg_llc_sapi));
      }
      else
      {
        lte_mode_ptr->neg_llc_sapi.valid = FALSE;
      }

      if (cm_rpt_ptr->cm_act_default_bearer_context_request_ind.apn_ambr.\
          valid)
      {
        *(&(lte_mode_ptr->apn_ambr)) = *(&(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.apn_ambr));
      }
      else
      {
        lte_mode_ptr->apn_ambr.valid = FALSE;
      }

      /* Update gsm_umts_connection_id for LTE to GW call object
      ** transfer.
      */
      lte_mode_ptr->gsm_umts_connection_id.valid = \
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.gsm_umts_connection_id.valid;
      if (lte_mode_ptr->gsm_umts_connection_id.valid)
      {
        lte_mode_ptr->gsm_umts_connection_id.connection_id = \
        cm_rpt_ptr->cm_act_default_bearer_context_request_ind.gsm_umts_connection_id.connection_id;
      }

      /* Update bearer_type, group_id, preparing for LTE to GW call object transfer
      */
      lte_mode_ptr->bearer_type  = CM_LTE_BEARER_TYPE_DEFAULT;
      lte_mode_ptr->pdp_group_id = cm_rpt_ptr->cm_act_default_bearer_context_request_ind.group_id;
    break;

    case MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND:


      CM_MSG_MED_4("MM_CM_DEACT_BEARER_CONTEXT_REQUEST inst_id %d rpt eps_bearer_id %d, connection_id %d, pco valid %d",
	  	cm_rpt_ptr->cm_deact_bearer_context_request_ind.msg_hdr.inst_id,
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.eps_bearer_id,
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.\
        connection_id,
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.protocol_config_options.valid);
      
      CM_MSG_MED_3("current call_ptr call_id %d, eps_bearer_id %d, bearer type %d",
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

      lte_mode_ptr->backoff_timer.active  = \
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.backoff_timer.active;
      lte_mode_ptr->backoff_timer.timer_count  = \
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.backoff_timer.timer_count;
      lte_mode_ptr->eps_bearer_id    =
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.eps_bearer_id;
      lte_mode_ptr->connection_id    =
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.\
        connection_id;
      lte_mode_ptr->sdf_id    =
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.sdf_id;

      lte_mode_ptr->esm_cause.valid =
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_cause.valid;

      if (cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_cause.valid)
      {
        lte_mode_ptr->esm_cause.esm_cause =
          cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_cause.esm_cause;
      }
      lte_mode_ptr->esm_local_cause.valid =
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_local_cause.valid;
      if (cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_local_cause.valid)
      {
        lte_mode_ptr->esm_local_cause.local_cause =
          cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_local_cause.local_cause;
      }

      lte_mode_ptr->config_options.valid = 
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.protocol_config_options.valid;
      if (cm_rpt_ptr->cm_deact_bearer_context_request_ind.protocol_config_options.valid)
      {
        cmltecall_copy_protocol_config_options(
          &(lte_mode_ptr->config_options),
          &(cm_rpt_ptr->cm_deact_bearer_context_request_ind.protocol_config_options));
      }
      
    break;

    case MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND:
      
      CM_MSG_MED_5("DEDICATED_BEARER_CONTEXT_REQ: eps_bearer_id %d conn_id %d call_id %d eps_bearer_id %d bearer_type %d",
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.eps_bearer_id,
                   cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.connection_id,
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

      call_ptr->asubs_id             = 
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
        cm_act_dedicated_bearer_context_request_ind.msg_hdr.inst_id);

      /* Determine cm_ss base on LTE rat and asubs_id */
      call_ptr->ss = cmph_determine_ss_per_rat(call_ptr->asubs_id, SYS_SYS_MODE_LTE);

      lte_mode_ptr->eps_bearer_id    =
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.eps_bearer_id;
      lte_mode_ptr->connection_id    =
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.\
        connection_id;
      lte_mode_ptr->sdf_id    =
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.sdf_id;
      lte_mode_ptr->lbi    =
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.lbi;

      *(&(lte_mode_ptr->sdf_qos)) = *(&(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.sdf_qos));

      if (cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.tft.\
        valid)
      {
        *(&(lte_mode_ptr->ul_dl_tft)) = *(&(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.tft));
      }
      else
      {
        lte_mode_ptr->ul_dl_tft.valid = FALSE;
      }

      if (cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.neg_qos.\
        valid)
      {
        *(&(lte_mode_ptr->neg_qos)) = *(&(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.neg_qos));
      }
      else
      {
        lte_mode_ptr->neg_qos.valid = FALSE;
      }

      cmltecall_copy_protocol_config_options(
      &(lte_mode_ptr->config_options),
      &(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.\
        protocol_config_options));

      if (cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.\
        neg_llc_sapi.valid)
      {
        *(&(lte_mode_ptr->neg_llc_sapi)) = *(&(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.neg_llc_sapi));
      }
      else
      {
        lte_mode_ptr->neg_llc_sapi.valid = FALSE;
      }

      /* Update gsm_umts_connection_id for LTE to GW call object
      ** transfer.
      */
      lte_mode_ptr->gsm_umts_connection_id.valid = \
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.gsm_umts_connection_id.valid;
      if (lte_mode_ptr->gsm_umts_connection_id.valid)
      {
        lte_mode_ptr->gsm_umts_connection_id.connection_id = \
        cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.gsm_umts_connection_id.connection_id;
      }

      /* Update bearer_type, group_id, preparing for LTE to GW call object transfer
      */
      lte_mode_ptr->bearer_type  = CM_LTE_BEARER_TYPE_DEDICATED;
      lte_mode_ptr->pdp_group_id = cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.group_id;

      break;

    case MM_CM_ACT_DRB_RELEASED_IND:

      CM_MSG_HIGH_1("MM_CM_ACT_DRB_RELEASED_IND inst_id=%d",
        cm_rpt_ptr->cm_act_drb_released_ind.msg_hdr.inst_id);

      lte_mode_ptr->esm_cause        =
        cm_rpt_ptr->cm_act_drb_released_ind.esm_cause;

    /* Need to verify if this is needed */
    // lte_nas_local_cause_T esm_local_cause ;

      break;

    case MM_CM_DRB_SETUP_IND:
    
      CM_MSG_MED_4("M_CM_DRB_SETUP_IND inst_id=%d rpt eps_bearer_id %d, connection_id %d, rb_id %d",
        cm_rpt_ptr->cm_drb_setup_ind.msg_hdr.inst_id,
        cm_rpt_ptr->cm_drb_setup_ind.eps_bearer_id,
        cm_rpt_ptr->cm_drb_setup_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_drb_setup_ind.rb_id);
      CM_MSG_MED_3("current call_ptr call_id %d, eps_bearer_id %d, bearer type %d",
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

      lte_mode_ptr->rb_id           =
        cm_rpt_ptr->cm_drb_setup_ind.rb_id;
      lte_mode_ptr->eps_bearer_id   =
        cm_rpt_ptr->cm_drb_setup_ind.eps_bearer_id;

      break;

    case MM_CM_DRB_REESTABLISH_FAILED_IND:
     
      CM_MSG_MED_6("DRB_REESTAB_FAIL: inst_id %d eps_bearer_id %d conn_id %d call_id %d eps_bearer_id %d bearer type %d",
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.msg_hdr.inst_id,
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.eps_bearer_id,
                   cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.connection_id,
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

     lte_mode_ptr->eps_bearer_id    =
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.eps_bearer_id;
     lte_mode_ptr->connection_id    =
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.connection_id;
     lte_mode_ptr->sdf_id    =
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.sdf_id;

     lte_mode_ptr->esm_local_cause.valid  =
             cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_local_cause.valid;


     lte_mode_ptr->esm_local_cause.local_cause=
             cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_local_cause.local_cause;


      break;

     case MM_CM_DRB_REESTABLISH_REJECT_IND:
      
       CM_MSG_MED_6("DRB_REESTAB_REJ: inst_id %d eps_bearer_id %d conn_id %d call_id %d eps_bearer_id %d bearer_type %d",
	  	 cm_rpt_ptr->cm_drb_reestablish_reject_ind.msg_hdr.inst_id,
         cm_rpt_ptr->cm_drb_reestablish_reject_ind.eps_bearer_id,
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

       lte_mode_ptr->eps_bearer_id    =
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.eps_bearer_id;
       lte_mode_ptr->connection_id    =
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id;
       lte_mode_ptr->sdf_id    =
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.sdf_id;

       lte_mode_ptr->esm_local_cause.valid  =
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause.valid;

       lte_mode_ptr->esm_local_cause.local_cause=
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause.local_cause;

       /* copy emm cause code*/
       lte_mode_ptr->emm_failure_cause.cause_type =
         cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause_type;
       memscpy(&(lte_mode_ptr->emm_failure_cause.detailed_cause),sizeof(uint16),
                &(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause),
                sizeof(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause));
       lte_mode_ptr->emm_failure_cause.access_barring_rem_time =
         cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.access_barring_rem_time;
       CM_MSG_MED_3("emm cause %d, detailed cause %d, acc_barr_time %d",
                  lte_mode_ptr->emm_failure_cause.cause_type,
                  lte_mode_ptr->emm_failure_cause.detailed_cause,
                  lte_mode_ptr->emm_failure_cause.access_barring_rem_time);


      break;

    case MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND:
     
      CM_MSG_MED_6("MODIFY_BEARER_REQ: inst_id %d eps_bearer_id %d conn_id %d call_id %d eps_bearer_id %d bearer_type %d",
	  	cm_rpt_ptr->cm_modify_bearer_context_request_ind.msg_hdr.inst_id,
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.eps_bearer_id,
                   cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.connection_id,
                  call_ptr->call_id,
                  lte_mode_ptr->eps_bearer_id,
                  lte_mode_ptr->bearer_type);

      lte_mode_ptr->eps_bearer_id    =
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.eps_bearer_id;
      lte_mode_ptr->connection_id    =
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.\
        connection_id;
      lte_mode_ptr->sdf_id       =
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.sdf_id;
      lte_mode_ptr->lbi              =
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.lbi;

      *(&(lte_mode_ptr->sdf_qos)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.sdf_qos));

      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_qos.valid)
      {
        *(&(lte_mode_ptr->neg_qos)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_qos));
      }
      else
      {
        lte_mode_ptr->neg_qos.valid = FALSE;
      }

      cmltecall_copy_protocol_config_options(
        &(lte_mode_ptr->config_options),
        &(cm_rpt_ptr->cm_modify_bearer_context_request_ind.\
        protocol_config_options));

      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.ul_dl_tft.valid)
      {
        *(&(lte_mode_ptr->ul_dl_tft)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.ul_dl_tft));
      }
      else
      {
        lte_mode_ptr->ul_dl_tft.valid = FALSE;
      }

      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_llc_sapi.\
          valid)
      {
        *(&(lte_mode_ptr->neg_llc_sapi)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_llc_sapi));
      }
      else
      {
        lte_mode_ptr->neg_llc_sapi.valid = FALSE;
      }

      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.apn_ambr.\
          valid)
      {
        *(&(lte_mode_ptr->apn_ambr)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.apn_ambr));
      }
      else
      {
        lte_mode_ptr->apn_ambr.valid = FALSE;
      }

      break;

    default:
      CM_ERR_1("Wrong call mode = %d",call_ptr->cmcall_mode_info.info_type);
      break;
  }


}


#if defined(CM_GW_SUPPORTED)
/*===========================================================================

FUNCTION cmltecall_update_call_ptr_lte_to_gw_irat

DESCRIPTION
  Update the call object with the GW common info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_update_call_ptr_lte_to_gw_irat(

    cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_ptr == NULL )
  {
    CM_ERR_0 ("parameter call_ptr = NULL");
    return;
  }

  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;

  /* Set the connection id src to ESM */
  cm_set_connection_id_src(call_ptr,CONN_ID_SM);

}
#endif


/*===========================================================================

FUNCTION cmltecall_rpt_info_get

DESCRIPTION
  Copy the report information into the call_info_ptr.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None.

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_rpt_info_get(

    const cm_esm_cmd_u_type *cm_rpt_ptr,
        /* Pointer to the report message */

    cm_call_event_e_type    call_event,
        /* notify client list of this call event */

    cm_mm_call_info_s_type     *call_info_ptr
        /* call info pointer to be copied to */

)
{

  CM_ASSERT(call_info_ptr != NULL);

  if( cm_rpt_ptr == NULL )
  {
    CM_MSG_HIGH_0("NULL ptr recvd for cm_rpt_ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_LTE;
  call_info_ptr->call_type = CM_CALL_TYPE_PS_DATA;
  call_info_ptr->sys_mode = SYS_SYS_MODE_LTE;

  CM_MSG_MED_1("SEND: CM CALL EVENT %d",call_event );
  
  switch (call_event)
  {
    case CM_CALL_EVENT_GET_PDN_CONN_IND:

      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_get_pdn_connectivity_request_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.seq_num = \
        cm_rpt_ptr->cm_get_pdn_connectivity_request_ind.seq_num;

      call_info_ptr->mode_info.info.lte_call.responding_to_page = \
        cm_rpt_ptr->cm_get_pdn_connectivity_request_ind.responding_to_page;


      CM_MSG_MED_1("CM_CALL_EVENT_GET_PDN_CONN_IND, rtp = %d", \
                        cm_rpt_ptr->cm_get_pdn_connectivity_request_ind.responding_to_page);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_MOD_BEARER_IND:


      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_modify_bearer_context_request_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.lbi;
      call_info_ptr->mode_info.info.lte_call.eps_bearer_id = \
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.eps_bearer_id;

      *(&(call_info_ptr->mode_info.info.lte_call.sdf_qos)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.sdf_qos));

      call_info_ptr->mode_info.info.lte_call.neg_qos.valid = \
      cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_qos.valid;
      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_qos.valid)
      {
        *(&(call_info_ptr->mode_info.info.lte_call.neg_qos)) = *(&(cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_qos));
      }

      call_info_ptr->mode_info.info.lte_call.neg_llc_sapi.valid = \
        cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_llc_sapi.valid;
      if (cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_llc_sapi.valid)
      {
        call_info_ptr->mode_info.info.lte_call.neg_llc_sapi.sapi =
          cm_rpt_ptr->cm_modify_bearer_context_request_ind.neg_llc_sapi.sapi;
      }

      cmltecall_copy_tft_type(
        &(call_info_ptr->mode_info.info.lte_call.ul_dl_tft),
        &(cm_rpt_ptr->cm_modify_bearer_context_request_ind.ul_dl_tft));

      cmltecall_copy_protocol_config_options(
        &(call_info_ptr->mode_info.info.lte_call.config_options),
        &(cm_rpt_ptr->cm_modify_bearer_context_request_ind.protocol_config_options));

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_PDN_CONN_REJ_IND:


      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_pdn_connectivity_reject_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_hdr.lbi;

      call_info_ptr->mode_info.info.lte_call.esm_cause.valid  = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_cause.valid;
      if (cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_cause.esm_cause  = \
          cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_cause.esm_cause;
      }

      call_info_ptr->mode_info.info.lte_call.esm_local_cause.valid = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause.valid;
      if (cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_local_cause.local_cause =\
          cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause.local_cause;
      }

      call_info_ptr->mode_info.info.lte_call.emm_failure_cause.cause_type =
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause_type;
       memscpy(&(call_info_ptr->mode_info.info.lte_call.emm_failure_cause.detailed_cause),
               sizeof(uint16),
              &(cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause),
              sizeof(cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause));
       call_info_ptr->mode_info.info.lte_call.emm_failure_cause.access_barring_rem_time =
         cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.access_barring_rem_time;
       CM_MSG_MED_3("emm cause %d, detailed cause %d, acc_barr_time %d",
                  call_info_ptr->mode_info.info.lte_call.emm_failure_cause.cause_type,
                  call_info_ptr->mode_info.info.lte_call.emm_failure_cause.detailed_cause,
                  call_info_ptr->mode_info.info.lte_call.emm_failure_cause.access_barring_rem_time);


      cmltecall_copy_protocol_config_options(
        &(call_info_ptr->mode_info.info.lte_call.config_options),
        &(cm_rpt_ptr->cm_modify_bearer_context_request_ind.protocol_config_options));

      call_info_ptr->mode_info.info.lte_call.backoff_timer.active  = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.backoff_timer.active;
      call_info_ptr->mode_info.info.lte_call.backoff_timer.timer_count  = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.backoff_timer.timer_count;

      call_info_ptr->mode_info.info.lte_call.pdn_reject_ind.pdn_type  = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.pdn_reject_ind.pdn_type ;

      call_info_ptr->mode_info.info.lte_call.pdn_reject_ind.access_point_name = \
        cm_rpt_ptr->cm_pdn_connectivity_reject_ind.pdn_reject_ind.access_point_name;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_PDN_CONN_FAIL_IND:

      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_pdn_connectivity_failure_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_pdn_connectivity_failure_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_pdn_connectivity_failure_ind.esm_hdr.lbi;
      call_info_ptr->mode_info.info.lte_call.result = \
        cm_rpt_ptr->cm_pdn_connectivity_failure_ind.result;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND:

      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_pdn_disconnect_reject_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_hdr.lbi;
      call_info_ptr->mode_info.info.lte_call.eps_bearer_id = \
        cm_rpt_ptr->cm_pdn_disconnect_reject_ind.eps_bearer_id;

      call_info_ptr->mode_info.info.lte_call.esm_cause.valid  = \
        cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_cause.valid;
      if (cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_cause.esm_cause  = \
          cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_cause.esm_cause;
      }

      call_info_ptr->mode_info.info.lte_call.esm_local_cause.valid = \
        cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_local_cause.valid;
      if (cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_local_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_local_cause.local_cause =\
          cm_rpt_ptr->cm_pdn_disconnect_reject_ind.esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(call_info_ptr->mode_info.info.lte_call.config_options),
        &(cm_rpt_ptr->cm_pdn_disconnect_reject_ind.protocol_config_options));

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_RES_ALLOC_REJ_IND:


      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_hdr.lbi;

      call_info_ptr->mode_info.info.lte_call.esm_cause.valid  = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_cause.valid;
      if (cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_cause.esm_cause  = \
          cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_cause.esm_cause;
      }

      call_info_ptr->mode_info.info.lte_call.esm_local_cause.valid = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_local_cause.valid;
      if (cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_local_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_local_cause.local_cause =\
          cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(call_info_ptr->mode_info.info.lte_call.config_options),
        &(cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.protocol_config_options));

      call_info_ptr->mode_info.info.lte_call.backoff_timer.active  = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.backoff_timer.active;
      call_info_ptr->mode_info.info.lte_call.backoff_timer.timer_count  = \
        cm_rpt_ptr->cm_bearer_resource_alloc_reject_ind.backoff_timer.timer_count;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:


      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_bearer_resource_alloc_failure_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_bearer_resource_alloc_failure_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_bearer_resource_alloc_failure_ind.esm_hdr.lbi;
      call_info_ptr->mode_info.info.lte_call.result = \
        cm_rpt_ptr->cm_bearer_resource_alloc_failure_ind.result;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_EVENT_BEARER_MOD_REJ_IND:

     

      call_info_ptr->asubs_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_bearer_context_modify_reject_ind.msg_hdr.inst_id);
      call_info_ptr->mode_info.info.lte_call.sdf_id = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_hdr.sdf_id;
      call_info_ptr->mode_info.info.lte_call.lbi = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_hdr.lbi;

      call_info_ptr->mode_info.info.lte_call.esm_cause.valid  = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_cause.valid;
      if (cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_cause.esm_cause  = \
          cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_cause.esm_cause;
      }

      call_info_ptr->mode_info.info.lte_call.esm_local_cause.valid = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_local_cause.valid;
      if (cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_local_cause.valid)
      {
        call_info_ptr->mode_info.info.lte_call.esm_local_cause.local_cause =\
          cm_rpt_ptr->cm_bearer_context_modify_reject_ind.esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(call_info_ptr->mode_info.info.lte_call.config_options),
        &(cm_rpt_ptr->cm_bearer_context_modify_reject_ind.protocol_config_options));

      call_info_ptr->mode_info.info.lte_call.backoff_timer.active  = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.backoff_timer.active;
      call_info_ptr->mode_info.info.lte_call.backoff_timer.timer_count  = \
        cm_rpt_ptr->cm_bearer_context_modify_reject_ind.backoff_timer.timer_count;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_MSG_HIGH_1("Wrong call event %d in cmltecall_rpt_info_get() ",(uint16)call_event);

      break;

  } // end of switch

}



/*===========================================================================

FUNCTION cmltecall_send_act_bearer_context_rsp

DESCRIPTION
  Send the response to ESM about ACT_BEARER_CONTEXT_REQUEST.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_send_act_bearer_context_rsp(

    const cmcall_s_type                 *call_ptr,
        /* Pointer to the call object  */

    const cm_call_cmd_info_lte_s_type   *lte_info
        /* Pointer to the info need to be passed to NAS */
)
{
  esm_msg                      *act_bearer_context_rsp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_5("CM->MSGR bearer_context_rsp: is_accept %d is_emerg_bearer %d as_id %d, call_id %d eps_bearer_id %d",
                lte_info->is_accept,
                lte_info->is_emergency_bearer,
                call_ptr->asubs_id,
              call_ptr->call_id,
              call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  act_bearer_context_rsp = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send LTE_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF to ESM */
  if (lte_info->is_accept)
  {
    if (call_ptr->cmcall_mode_info.info.lte_info.bearer_type == \
        CM_LTE_BEARER_TYPE_DEFAULT)
    {
      act_bearer_context_rsp->esm_act_default_bearer_context_accept.esm_hdr.\
        connection_id =
      call_ptr->cmcall_mode_info.info.lte_info.connection_id;
      act_bearer_context_rsp->esm_act_default_bearer_context_accept.\
        eps_bearer_id =
      call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id;
      act_bearer_context_rsp->esm_act_default_bearer_context_accept.emc_bc =
                                              lte_info->is_emergency_bearer;
      act_bearer_context_rsp->esm_act_default_bearer_context_accept.msg_hdr.inst_id =
                                              SYS_AS_ID_TO_INST_ID(lte_info->as_id);

      cmltecall_copy_protocol_config_options(
        &(act_bearer_context_rsp->esm_act_default_bearer_context_accept.\
          protocol_config_options),
        &(lte_info->config_options));

      act_bearer_context_rsp->esm_act_default_bearer_context_accept.h_comp =\
        lte_info->h_comp;

      act_bearer_context_rsp->esm_act_default_bearer_context_accept.d_comp =\
        lte_info->d_comp;

      (void) cm_msgr_send( NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF, MSGR_MM_CM,
                          (msgr_hdr_s *)act_bearer_context_rsp, sizeof(esm_msg) );
    }
    else
    {
      /* Send NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF to ESM */

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.\
        esm_hdr.connection_id =
        call_ptr->cmcall_mode_info.info.lte_info.connection_id;
      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.\
        eps_bearer_id =
        call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id;

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.emc_bc =
                                              lte_info->is_emergency_bearer;
      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.msg_hdr.inst_id =
                                              SYS_AS_ID_TO_INST_ID(lte_info->as_id);
      cmltecall_copy_protocol_config_options(
        &(act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.\
          protocol_config_options),
        &(lte_info->config_options));

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.h_comp =\
        lte_info->h_comp;

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_accept.d_comp =\
        lte_info->d_comp;

      (void) cm_msgr_send( NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF, MSGR_MM_CM,
                           (msgr_hdr_s *)act_bearer_context_rsp, sizeof(esm_msg) );
    }
  }
  else
  /* Send NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND to ESM */
  {
    if(call_ptr->cmcall_mode_info.info.lte_info.bearer_type == \
       CM_LTE_BEARER_TYPE_DEFAULT)
    {
      act_bearer_context_rsp->esm_act_default_bearer_context_rej.esm_hdr.\
        connection_id =
      call_ptr->cmcall_mode_info.info.lte_info.connection_id;
      act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
        eps_bearer_id =
      call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id;

      act_bearer_context_rsp->esm_act_default_bearer_context_rej.msg_hdr.inst_id =
         SYS_AS_ID_TO_INST_ID(lte_info->as_id);

      act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
        esm_cause.valid = lte_info->esm_cause.valid;
      if (lte_info->esm_cause.valid)
      {
        act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
          esm_cause.esm_cause = lte_info->esm_cause.esm_cause;
      }

      act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
        esm_local_cause.valid = lte_info->esm_local_cause.valid;
      if (lte_info->esm_local_cause.valid)
      {
        act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
          esm_local_cause.local_cause =
          lte_info->esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(act_bearer_context_rsp->esm_act_default_bearer_context_rej.\
          protocol_config_options),
        &(lte_info->config_options));

      act_bearer_context_rsp->esm_act_default_bearer_context_rej.emc_bc
         = lte_info->is_emergency_bearer;

      (void) cm_msgr_send( NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND, MSGR_MM_CM,
                          (msgr_hdr_s *)act_bearer_context_rsp, sizeof(esm_msg) );
    }
    else
    {
      /* Send NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND to ESM */

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.esm_hdr.\
        connection_id =
        call_ptr->cmcall_mode_info.info.lte_info.connection_id;
      act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
        eps_bearer_id =
        call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id;
      act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(lte_info->as_id);

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
        esm_cause.valid = lte_info->esm_cause.valid;
      if (lte_info->esm_cause.valid)
      {
        act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
          esm_cause.esm_cause = lte_info->esm_cause.esm_cause;
      }

      act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
        esm_local_cause.valid = lte_info->esm_local_cause.valid;
      if (lte_info->esm_local_cause.valid)
      {
        act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
          esm_local_cause.local_cause =
          lte_info->esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(act_bearer_context_rsp->esm_act_dedicated_bearer_context_rej.\
          protocol_config_options),
        &(lte_info->config_options));

      (void) cm_msgr_send( NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND, MSGR_MM_CM,
                          (msgr_hdr_s *)act_bearer_context_rsp, sizeof(esm_msg) );
    }
  }

  /* Free allocated ESM message buffer */
  cm_mem_free (act_bearer_context_rsp);

}


/*===========================================================================

FUNCTION cmltecall_send_mod_bearer_context_rsp

DESCRIPTION
  Send the response to ESM about MOD_BEARER_CONTEXT_REQUEST.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_send_mod_bearer_context_rsp(

    const cmcall_s_type                 *call_ptr,
        /* Pointer to the call object  */

    const cm_call_cmd_info_lte_s_type   *lte_info
        /* Pointer to the info need to be passed to NAS */
)
{
  esm_msg                      *mod_bearer_context_rsp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM->MSGR: mod_bearer_context_rsp is_accept=%d, asubs_id=%d",
                 lte_info->is_accept,
                 call_ptr->asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  mod_bearer_context_rsp = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF to ESM */
  if (lte_info->is_accept)
  {
    mod_bearer_context_rsp->esm_modify_bearer_context_accept.esm_hdr.\
      sdf_id = lte_info->sdf_id;
    mod_bearer_context_rsp->esm_modify_bearer_context_accept.esm_hdr.\
      connection_id = call_ptr->cmcall_mode_info.info.lte_info.connection_id;
    mod_bearer_context_rsp->esm_modify_bearer_context_accept.\
      eps_bearer_id = lte_info->eps_bearer_id;
    mod_bearer_context_rsp->esm_modify_bearer_context_accept.msg_hdr.inst_id = \
      SYS_AS_ID_TO_INST_ID(lte_info->as_id);

    cmltecall_copy_protocol_config_options(
      &(mod_bearer_context_rsp->esm_modify_bearer_context_accept.\
        protocol_config_options),
      &(lte_info->config_options));

    (void) cm_msgr_send( NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF, MSGR_MM_CM,
                         (msgr_hdr_s*)mod_bearer_context_rsp, sizeof(esm_msg));
  }
  else
  /* Send NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND to ESM */
  {
      mod_bearer_context_rsp->esm_modify_bearer_context_rej.esm_hdr.\
        sdf_id = lte_info->sdf_id;
      mod_bearer_context_rsp->esm_modify_bearer_context_rej.esm_hdr.\
        connection_id = call_ptr->gw_connection_id;
      mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
        eps_bearer_id = lte_info->eps_bearer_id;
      mod_bearer_context_rsp->esm_modify_bearer_context_rej.msg_hdr.inst_id = \
	  	SYS_AS_ID_TO_INST_ID(lte_info->as_id);

      mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
        esm_cause.valid = lte_info->esm_cause.valid;
      if (lte_info->esm_cause.valid)
      {
        mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
          esm_cause.esm_cause = lte_info->esm_cause.esm_cause;
      }

      mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
        esm_local_cause.valid = lte_info->esm_local_cause.valid;
      if (lte_info->esm_local_cause.valid)
      {
        mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
          esm_local_cause.local_cause = lte_info->esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
        &(mod_bearer_context_rsp->esm_modify_bearer_context_rej.\
          protocol_config_options),
        &(lte_info->config_options));

      (void) cm_msgr_send( NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND, MSGR_MM_CM,
                           (msgr_hdr_s *)mod_bearer_context_rsp, sizeof(esm_msg));
  }

  /* Free allocated ESM message buffer */
  cm_mem_free (mod_bearer_context_rsp);

}


/*===========================================================================

FUNCTION cmltecall_send_drb_reestablish_req

DESCRIPTION
  Send the request to ESM about DRB_REESTABLISHMENT_REQ.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None
  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_send_drb_reestablish_req(

    const cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
)
{
  esm_msg               *drb_reestablish_req; /* ESM message buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM->MSGR: drb_reestablish_req : Bearer ID =%d, asubs_id=%d",
              call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id,
              call_ptr->asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  drb_reestablish_req = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send NAS_ESM_DRB_REESTABLISH_REQ to ESM */

   /* ESM DRB Reestablishment ESM header connection id*/
   drb_reestablish_req->esm_drb_reestablish_req.esm_hdr.connection_id =
           call_ptr->cmcall_mode_info.info.lte_info.connection_id;

   /* ESM DRB Reestablishment EPS bearer  id*/
   drb_reestablish_req->esm_drb_reestablish_req.eps_bearer_id =
              call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id;
   drb_reestablish_req->esm_drb_reestablish_req.msg_hdr.inst_id =
              SYS_AS_ID_TO_INST_ID(call_ptr->cmcall_mode_info.info.lte_info.as_id);
   drb_reestablish_req->esm_drb_reestablish_req.call_type =
              call_ptr->cmcall_mode_info.info.lte_info.sys_lte_reestab_calltype;

   (void) cm_msgr_send( NAS_ESM_DRB_REESTABLISH_REQ, MSGR_MM_CM,
                        (msgr_hdr_s*)drb_reestablish_req, sizeof(esm_msg) );

   /* Free allocated ESM message buffer */
   cm_mem_free (drb_reestablish_req);

}


/*===========================================================================

FUNCTION cmltecall_send_requests

DESCRIPTION
  Send the request to ESM.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None
  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_send_requests (
  const cm_call_cmd_s_type        *call_cmd_ptr
    /* The call command pointer with the request-related info */
)
{
  esm_msg                      *client_request;

  CM_ASSERT( call_cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Dynamically allocate ESM message buffer
  */
  client_request = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_MED_2("CM->NAS: LTE req %d with as_id %d", call_cmd_ptr->cmd,
                                        call_cmd_ptr->info.lte_info.as_id);
  switch( call_cmd_ptr->cmd )
  {
    case CM_CALL_CMD_PDN_CONNECTIVITY_REQ:

      /*
      **  Ensure that the conn_id that ESM expects does not have
      **  garbage value in this message. So CM sets 0xff, Need to discuss.
      */
      client_request->esm_pdn_connectivity_req.esm_hdr.connection_id = \
        CM_UNKNOWN_CONN_ID;

      client_request->esm_pdn_connectivity_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;

      client_request->esm_pdn_connectivity_req.request_type = \
        call_cmd_ptr->info.lte_info.request_type;

      client_request->esm_pdn_connectivity_req.msg_hdr.inst_id= \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      *(&client_request->esm_pdn_connectivity_req.pdn_data) = \
         *(&(call_cmd_ptr->info.lte_info.pdn_data));

      CM_MSG_MED_1("CM_CALL_CMD_PDN_CONNECTIVITY_REQ, lps = %d", \
                  call_cmd_ptr->info.lte_info.pdn_data.low_priority_signaling);


      (void) cm_msgr_send( NAS_ESM_PDN_CONNECTIVTY_REQ, MSGR_MM_CM,
                           (msgr_hdr_s*)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ:

      client_request->esm_pdn_connectivity_abort_req.esm_hdr.connection_id =\
        CM_UNKNOWN_CONN_ID;
      client_request->esm_pdn_connectivity_abort_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;
      client_request->esm_pdn_connectivity_abort_req.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      client_request->esm_pdn_connectivity_abort_req.esm_cause.valid = \
        call_cmd_ptr->info.lte_info.esm_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_cause.valid)
      {
        client_request->esm_pdn_connectivity_abort_req.esm_cause.esm_cause =
          call_cmd_ptr->info.lte_info.esm_cause.esm_cause;
      }

      client_request->esm_pdn_connectivity_abort_req.esm_local_cause.valid =
        call_cmd_ptr->info.lte_info.esm_local_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_local_cause.valid)
      {
        client_request->esm_pdn_connectivity_abort_req.esm_local_cause.local_cause =
          call_cmd_ptr->info.lte_info.esm_local_cause.local_cause;
      }

      
      (void) cm_msgr_send( NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ, MSGR_MM_CM,
                           (msgr_hdr_s*)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_PDN_DISCONNECT_REQ:

      client_request->esm_pdn_disconnect_req.esm_hdr.connection_id = \
        CM_UNKNOWN_CONN_ID;
      client_request->esm_pdn_disconnect_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;

      client_request->esm_pdn_disconnect_req.eps_bearer_id = \
        call_cmd_ptr->info.lte_info.eps_bearer_id;
      client_request->esm_pdn_disconnect_req.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      cmltecall_copy_protocol_config_options(
        &(client_request->esm_pdn_disconnect_req.protocol_config_options),
        &(call_cmd_ptr->info.lte_info.config_options) );

      client_request->esm_pdn_disconnect_req.esm_local_cause.valid =
        call_cmd_ptr->info.lte_info.esm_local_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_local_cause.valid)
      {
        client_request->esm_pdn_disconnect_req.esm_local_cause.local_cause =
          call_cmd_ptr->info.lte_info.esm_local_cause.local_cause;
      }

      
      (void) cm_msgr_send( NAS_ESM_PDN_DISCONNECT_REQ, MSGR_MM_CM,
                           (msgr_hdr_s*)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_RES_ALLOC_REQ:

      client_request->esm_bearer_resource_alloc_req.esm_hdr.connection_id = \
        CM_UNKNOWN_CONN_ID;
      client_request->esm_bearer_resource_alloc_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;
      client_request->esm_bearer_resource_alloc_req.esm_hdr.lbi = \
        call_cmd_ptr->info.lte_info.lbi;
	  client_request->esm_bearer_resource_alloc_req.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      client_request->esm_bearer_resource_alloc_req.sdf_qos.valid = \
        call_cmd_ptr->info.lte_info.sdf_qos.valid;
      if (call_cmd_ptr->info.lte_info.sdf_qos.valid)
      {
        *(&(client_request->esm_bearer_resource_alloc_req.sdf_qos)) = \
           *(&(call_cmd_ptr->info.lte_info.sdf_qos));
      }

      cmltecall_copy_tft_type(
        &(client_request->esm_bearer_resource_alloc_req.ul_dl_tft),
        &(call_cmd_ptr->info.lte_info.ul_dl_tft));

      cmltecall_copy_protocol_config_options(
        &(client_request->esm_bearer_resource_alloc_req.protocol_config_options),
        &(call_cmd_ptr->info.lte_info.config_options) );

      client_request->esm_bearer_resource_alloc_req.low_priority_signaling = \
                           call_cmd_ptr->info.lte_info.pdn_data.low_priority_signaling ;

      CM_MSG_MED_1("CM_CALL_CMD_RES_ALLOC_REQ, lps = %d", \
                        call_cmd_ptr->info.lte_info.pdn_data.low_priority_signaling);

      (void) cm_msgr_send( NAS_ESM_BEARER_RESOURCE_ALLOC_REQ, MSGR_MM_CM,
                           (msgr_hdr_s *)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_RES_ALLOC_ABORT_REQ:

      client_request->esm_bearer_resource_alloc_abort_req.esm_hdr.connection_id = \
        CM_UNKNOWN_CONN_ID;
      client_request->esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;
      client_request->esm_bearer_resource_alloc_abort_req.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      client_request->esm_bearer_resource_alloc_abort_req.esm_cause.valid = \
        call_cmd_ptr->info.lte_info.esm_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_cause.valid)
      {
        client_request->esm_bearer_resource_alloc_abort_req.esm_cause.esm_cause =
          call_cmd_ptr->info.lte_info.esm_cause.esm_cause;
      }

      client_request->esm_bearer_resource_alloc_abort_req.esm_local_cause.valid =
        call_cmd_ptr->info.lte_info.esm_local_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_local_cause.valid)
      {
        client_request->esm_bearer_resource_alloc_abort_req.esm_local_cause.local_cause =
          call_cmd_ptr->info.lte_info.esm_local_cause.local_cause;
      }

      (void) cm_msgr_send( NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ, MSGR_MM_CM,
                           (msgr_hdr_s *)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_BEARER_RES_MOD_REQ:

      client_request->esm_bearer_resource_modification_req.esm_hdr.connection_id = \
        CM_UNKNOWN_CONN_ID;
      client_request->esm_bearer_resource_modification_req.esm_hdr.sdf_id = \
        call_cmd_ptr->info.lte_info.sdf_id;
      client_request->esm_bearer_resource_modification_req.esm_hdr.lbi = \
        call_cmd_ptr->info.lte_info.lbi;
      client_request->esm_bearer_resource_modification_req.msg_hdr.inst_id = \
        SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

      *(&(client_request->esm_bearer_resource_modification_req.sdf_qos)) = \
         *(&(call_cmd_ptr->info.lte_info.sdf_qos));

      cmltecall_copy_tft_type(&(client_request->esm_bearer_resource_modification_req.tad),
                              &(call_cmd_ptr->info.lte_info.ul_dl_tft));

      client_request->esm_bearer_resource_modification_req.esm_cause.valid = \
        call_cmd_ptr->info.lte_info.esm_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_cause.valid)
      {
        client_request->esm_bearer_resource_modification_req.esm_cause.esm_cause =
          call_cmd_ptr->info.lte_info.esm_cause.esm_cause;
      }

      client_request->esm_bearer_resource_modification_req.esm_local_cause.valid = \
        call_cmd_ptr->info.lte_info.esm_local_cause.valid;
      if (call_cmd_ptr->info.lte_info.esm_local_cause.valid)
      {
        client_request->esm_bearer_resource_modification_req.esm_local_cause.local_cause =
          call_cmd_ptr->info.lte_info.esm_local_cause.local_cause;
      }

      cmltecall_copy_protocol_config_options(
          &(client_request->esm_bearer_resource_modification_req.protocol_config_options),
          &(call_cmd_ptr->info.lte_info.config_options) );

      client_request->esm_bearer_resource_modification_req.low_priority_signaling = \
        call_cmd_ptr->info.lte_info.pdn_data.low_priority_signaling;

      CM_MSG_MED_1("CM_CALL_CMD_BEARER_RES_MOD_REQ, lps = %d", \
                        call_cmd_ptr->info.lte_info.pdn_data.low_priority_signaling);

      (void) cm_msgr_send( NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ, MSGR_MM_CM,
                           (msgr_hdr_s *)client_request, sizeof(esm_msg) );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_MOD_BEARER_RSP:

      if (call_cmd_ptr->info.lte_info.is_accept)
      {
        client_request->esm_modify_bearer_context_accept.esm_hdr.connection_id\
          = call_cmd_ptr->info.lte_info.connection_id;
        client_request->esm_modify_bearer_context_accept.esm_hdr.sdf_id = \
          call_cmd_ptr->info.lte_info.sdf_id;

        client_request->esm_modify_bearer_context_accept.eps_bearer_id = \
          call_cmd_ptr->info.lte_info.eps_bearer_id;
        client_request->esm_modify_bearer_context_accept.msg_hdr.inst_id = \
          SYS_AS_ID_TO_INST_ID(call_cmd_ptr->info.lte_info.as_id);

        cmltecall_copy_protocol_config_options(
          &(client_request->esm_modify_bearer_context_accept.protocol_config_options),
          &(call_cmd_ptr->info.lte_info.config_options) );

        CM_MSG_MED_0("CM->MSGR: SEND: NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_IND ");
        (void) cm_msgr_send( NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF, MSGR_MM_CM,
                             (msgr_hdr_s *)client_request, sizeof(esm_msg) );
      }
      else
      {
        client_request->esm_modify_bearer_context_rej.esm_hdr.connection_id\
          = call_cmd_ptr->info.lte_info.connection_id;
        client_request->esm_modify_bearer_context_rej.esm_hdr.sdf_id = \
          call_cmd_ptr->info.lte_info.sdf_id;

        client_request->esm_modify_bearer_context_rej.eps_bearer_id = \
          call_cmd_ptr->info.lte_info.eps_bearer_id;

        client_request->esm_modify_bearer_context_rej.esm_cause.valid = \
        call_cmd_ptr->info.lte_info.esm_cause.valid;
        if (call_cmd_ptr->info.lte_info.esm_cause.valid)
        {
          client_request->esm_modify_bearer_context_rej.esm_cause.esm_cause =
            call_cmd_ptr->info.lte_info.esm_cause.esm_cause;
        }

        client_request->esm_modify_bearer_context_rej.esm_local_cause.valid =
          call_cmd_ptr->info.lte_info.esm_local_cause.valid;
        if (call_cmd_ptr->info.lte_info.esm_local_cause.valid)
        {
          client_request->esm_modify_bearer_context_rej.esm_local_cause.local_cause =
            call_cmd_ptr->info.lte_info.esm_local_cause.local_cause;
        }

        cmltecall_copy_protocol_config_options(
          &(client_request->esm_modify_bearer_context_rej.protocol_config_options),
          &(call_cmd_ptr->info.lte_info.config_options) );

        CM_MSG_MED_0("CM->MSGR: SEND: NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND ");
        (void) cm_msgr_send( NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND, MSGR_MM_CM,
                             (msgr_hdr_s *)client_request, sizeof(esm_msg) );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR_1( "call cmd %d", (int)(call_cmd_ptr->cmd));
  } /* end of switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Free allocated ESM message buffer */
  cm_mem_free (client_request);

}


/*===========================================================================

FUNCTION cmltecall_call_end

DESCRIPTION
  Clean up call object and ph object when LTE call ends.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_call_end(

  cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
)
{
  cm_act_update_reas_e_type update_reason = CM_ACT_UPDATE_REAS_ACT_END;
  const lte_cmcall_type     *lte_mode_ptr = NULL;
  cm_ss_e_type              ss = CM_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_ptr == NULL )
  {
    CM_ERR_0( "Parameter call_ptr = NULL");
    return;
  }

  if (cmph_is_msim())
  {
    ss = cmph_map_as_id_to_cm_ss(call_ptr->asubs_id);
  }
  
  /* Change the call state to idle.
  */
  call_ptr->call_state = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_NONE;

  cmcall_update_gsdi_and_log_call_state(call_ptr->ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify clients of call end event.
  */
  cmcall_update_call_end_info( call_ptr );
  cmcall_event( call_ptr, CM_CALL_EVENT_END );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_MMODE_DUAL_SIM
    if (cmph_ptr()->dds_status == CMPH_DDS_STATUS_REMOVE_DATA_OBJ)
    {
      /* Delete the proprity queue's for this activity end
       */
      cmph_delete_orig_param( (cm_act_id_type) call_ptr->call_id,
                       CM_SS_MAIN);
      cmph_delete_orig_param( (cm_act_id_type) call_ptr->call_id,
                       CM_SS_HYBR_2);
      cmph_delete_orig_param( (cm_act_id_type) call_ptr->call_id,
                       CM_SS_HDR);
      cmph_delete_orig_param( (cm_act_id_type) call_ptr->call_id,
                       CM_SS_HYBR_3);

      /* Reset the dds_status */
    cmph_ptr()->dds_status = CMPH_DDS_STATUS_NONE;

    }
    else
    #endif
    {
      /* Update the proprity queue's for this activity end
      */
      (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                   CM_SS_MAIN,
                                   update_reason );
      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                   CM_SS_HYBR_2,
                                   update_reason );
      (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                   CM_SS_HDR,
                                   update_reason );
      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                   CM_SS_HYBR_3,
                                   update_reason );
    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmcall_is_no_call_in_gw_ps(ss) && cmcall_is_no_call_in_lte_ps() )
  {
    cmregprx_proc_gw_resel_ok(ss);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there are no calls active, reset the phone variables.
  */
  /* Tell phone that call is over.
  Number of allocated calls equal to 1 means that this is the
  last call that is being ended, so we need to notify the phone
  now. 
  */
  cmph_call_end_process(call_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Unreserve GW connection_id */

  lte_mode_ptr          = &call_ptr->cmcall_mode_info.info.lte_info;

  if(lte_mode_ptr->gsm_umts_connection_id.valid)
  {
    if (!cmcall_unreserve_sm_connection_id(
        lte_mode_ptr->gsm_umts_connection_id.connection_id,call_ptr->asubs_id ))
    {
      CM_ERR_1( "Could not unreserve connection id.",
              lte_mode_ptr->gsm_umts_connection_id.connection_id);
    }
  }
  cm_print_connection_ids();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* free the call ID */
  cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update DDTM status to take care of emerg call end.
  */
  cmph_update_ddtm_status(cmph_ptr());

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if this is the last ps domain call being ended
  ** and update ciphering indication accordingly.
  */
  if (cmcall_is_no_call_in_gw_ps(ss) && cmcall_is_no_call_in_lte_ps())
  {
    cmss_rel_cipher_ind (SYS_SRV_DOMAIN_PS_ONLY);
  }

  /* Check if this is the last EPS call being ended
  ** and updates the GSDI status about traffic/non-traffic.
  */
  if(cm_number_of_allocated_calls() == 0)
  {    
    cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
  }

}



#ifdef FEATURE_CM_LTE_CC
/*===========================================================================

FUNCTION cmltecall_call_control_complete_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765, cmltecall_call_control_complete_cb)
** Can't be static, unit test uses it
*/
void cmltecall_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                              *user_data
    /* data passed from original call */

)
{
  esm_msg          *pdn_packed_cnf; /* ESM message buffer */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_data                  != NULL);
  if( gw_call_control_info_ptr == NULL )
  {
    CM_MSG_HIGH_0("NULL ptr recvd for gw_call_control_info_ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("cmltecall_call_control_complete_cb(), call_id=%d,status=%d,call_type=%d ",
                 call_id,status,call_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Dynamically allocate ESM message buffer
  */
  pdn_packed_cnf = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send NAS_ESM_PDN_CONNECT_PACKED_CNF to ESM */

  /* sdf id */
  pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.esm_hdr.sdf_id =
    gw_call_control_info_ptr->lte_ps_cc_params.sdf_id;

  /* connection_id */
  pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.esm_hdr.connection_id = 0xFF;

  /* as_id */
  pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.msg_hdr.inst_id =
    SYS_AS_ID_TO_INST_ID(gw_call_control_info_ptr->lte_ps_cc_params.as_id);

  CM_MSG_HIGH_3("lt cc cb: as_id=%d, ps_cc_sys_mode=%d, call_control_result=%d", 
                gw_call_control_info_ptr->lte_ps_cc_params.as_id, 
                gw_call_control_info_ptr->ps_cc_sys_mode, 
                gw_call_control_info_ptr->call_control_result);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  if (status == CM_CC_STATUS_SUCCESS   &&
      gw_call_control_info_ptr != NULL &&
      gw_call_control_info_ptr->ps_cc_sys_mode == SYS_SYS_MODE_LTE)
  /*lint -restore */
  {

    /* call control status */
    if ((gw_call_control_info_ptr->call_control_result >=
         CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS) &&
        (gw_call_control_info_ptr->call_control_result <=
         CM_CC_RESULT_ALLOWED_BUT_MODIFIED))
    {
      if((gw_call_control_info_ptr->call_control_result ==
          CM_CC_RESULT_NOT_ALLOWED) &&
         (gw_call_control_info_ptr->error_cause ==
          CM_CALL_CMD_ERR_CAUSE_ACL_NO_MATCH))
      {
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status =
        ESM_MSG_REJECTED_ACL_FAILURE;
      }
      else
      {
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status =
        (pdn_conn_req_status_T)gw_call_control_info_ptr->call_control_result;
      }
    }
    else
    {
      pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status=ESM_MSG_REJECTED;

      CM_MSG_HIGH_1("Invalid call_control_result %d",
                  gw_call_control_info_ptr->call_control_result);
    }

    /* packed message length */
    pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len =
      gw_call_control_info_ptr->lte_ps_cc_params.length;

    if(pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len >
       PDN_CONN_REQ_SIZE)
    {
      pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len =
        PDN_CONN_REQ_SIZE;
    }

    /* copy packed message */
    memscpy( pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg,
             sizeof(pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg),
             gw_call_control_info_ptr->lte_ps_cc_params.pkd_msg,
             pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len);

  }
  else
  {

    /* call control status */
    pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status =
      ESM_MSG_REJECTED;

    /* packed message length */
     pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len = 0;

  }

  /* transaction id */
  pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pti =
    gw_call_control_info_ptr->lte_ps_cc_params.pti;

  CM_MSG_MED_2("CM->MSGR: Send: NAS_ESM_PDN_CONNECT_PACKED_CNF, status=%d, pti=%d",
                pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status,
                pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pti);

  (void) cm_msgr_send( NAS_ESM_PDN_CONNECT_PACKED_CNF, MSGR_MM_CM,
                       (msgr_hdr_s *)pdn_packed_cnf, sizeof(esm_msg));

  /* Free allocated ESM message buffer */
  cm_mem_free (pdn_packed_cnf);

  SYS_ARG_NOT_USED(cdma_call_control_info_ptr);
  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(call_type);
  SYS_ARG_NOT_USED(user_data);

}  /* cmltecall_call_control_complete_cb */
#endif

/*===========================================================================

FUNCTION cmltecall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_rpt_proc(

  const cm_esm_cmd_u_type   *cm_rpt_ptr
    /* Pointer to LL report */
)
{
  cmcall_s_type            *call_ptr;
      /* Point at call object */

  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
      /* Call ID */


  boolean                  is_found = TRUE;

  #ifdef FEATURE_CM_LTE_CC
  boolean                  ret_val = FALSE;
  esm_msg          *pdn_packed_cnf;

  cm_cc_subscription_source_e_type     cdma_subscription_source;
  cm_cc_subscription_source_e_type     gwl_subscription_source;
  cm_lte_ps_cc_params_s_type           *lte_ps_cc_params_ptr = NULL;
#ifdef CM_DEBUG
  #error code not present
#endif
  #endif /* FEATURE_CM_LTE_CC */

 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

  cm_iterator_type        call_itr;
   /* Call Object Iterator */
   
  cm_call_id_type ims_call_id ; 

  #endif

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

                     /*------------------------------*/
                     /*      Incoming call           */
                     /*------------------------------*/

    case MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND:
    case MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND:

      /* Check whether this request is for the existing Originating call */
      if (cm_rpt_ptr->msg_hdr.id ==
          MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND)
      {
        /* find the call_id for the ACT_BEARER_REQUEST_IND */
        is_found = cm_find_call_id_per_subs(
          CONN_ID_ESM,
          cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.\
          connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
          cm_act_default_bearer_context_request_ind.msg_hdr.inst_id));

        /* Mark gw connection id */
        if(cm_rpt_ptr->cm_act_default_bearer_context_request_ind.gsm_umts_connection_id.valid)
        {
          cmcall_mark_sm_connection_id_reserved(
            cm_rpt_ptr->cm_act_default_bearer_context_request_ind.gsm_umts_connection_id.connection_id, 
			INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
            cm_act_default_bearer_context_request_ind.msg_hdr.inst_id));
        }

        CM_MSG_HIGH_3("ACT DEFAULT BEARER req call_id=%d, esm conn_id=%d, sm_conn_id=%d",call_id,
          cm_rpt_ptr->cm_act_default_bearer_context_request_ind.esm_hdr.connection_id,
          cm_rpt_ptr->cm_act_default_bearer_context_request_ind.gsm_umts_connection_id.connection_id);

      }
      else
      {
        /* find the call_id for the ACT_BEARER_REQUEST_IND */
        is_found = cm_find_call_id_per_subs(
          CONN_ID_ESM,
          cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.\
          connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
          cm_act_dedicated_bearer_context_request_ind.msg_hdr.inst_id));

        /* Mark gw connection id */
        if(cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.gsm_umts_connection_id.valid)
        {
          cmcall_mark_sm_connection_id_reserved(
            cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.gsm_umts_connection_id.connection_id,
			INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->\
            cm_act_dedicated_bearer_context_request_ind.msg_hdr.inst_id)
			);
        }

        CM_MSG_HIGH_3("ACT DEDICATED BEARER req call_id=%d, esm conn_id=%d, sm_conn_id=%d",call_id,
          cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.esm_hdr.connection_id,
          cm_rpt_ptr->cm_act_dedicated_bearer_context_request_ind.gsm_umts_connection_id.connection_id);
      }

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        /* mark that the act bearer req is for existing call */
        if( call_ptr != NULL )
        {
          call_ptr->call_debug_info |= BM(CMCALL_DEBUG_INFO_ACT_BEARER_CALL_ID_FOUND);
        } 
      }
      else
      {
        /* Generate call id for the incoming call */
        if( !cm_new_incom_call_id ( CM_CALL_TYPE_PS_DATA, &call_id,
            cm_rpt_ptr->cm_act_default_bearer_context_request_ind.\
                                    esm_hdr.connection_id) )
        {
          CM_ERR_0( "ERROR: No call id for BEARER_CONTEXT_REQUEST_IND");
          break;
        }

        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        if( call_ptr == NULL )
        {
          CM_ERR_1( "No call object for call_id=%d exists", call_id);
          break;
        }

        /* Check service */
        call_ptr->call_state = CM_CALL_STATE_INCOM;
        call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_ACT_BEARER_REQUEST;

        call_ptr->sys_mode = SYS_SYS_MODE_LTE;
        call_ptr->srv_type = CM_SRV_TYPE_NONE;

        /* mark that the act bearer req is for a new call */
        call_ptr->call_debug_info &= (~BM(CMCALL_DEBUG_INFO_ACT_BEARER_CALL_ID_FOUND));

        cmph_call_start( call_ptr );
      }

      if( call_ptr != NULL )
      {
        /* Set default/dedicated bearer flag*/
        if (cm_rpt_ptr->msg_hdr.id ==
            MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND)
        {
          call_ptr->cmcall_mode_info.info.lte_info.bearer_type = \
            CM_LTE_BEARER_TYPE_DEFAULT;
        }
        else
        {
          call_ptr->cmcall_mode_info.info.lte_info.bearer_type = \
            CM_LTE_BEARER_TYPE_DEDICATED;
        }

        /* send call event to inform DS about ACT_BEARER_IND*/
        cmcall_event( call_ptr, CM_CALL_EVENT_ACT_BEARER_IND );
      }


      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND:
       CM_MSG_MED_2("RXD: DEACT_BEARER_CONTEXT_REQUEST_IND: %d, inst_id=%d",
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_deact_bearer_context_request_ind.msg_hdr.inst_id);

      /* find the call_id for the DEACT_BEARER_REQUEST_IND */
      is_found = cm_find_call_id_per_subs(
      CONN_ID_ESM,
      cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.connection_id,
      &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_deact_bearer_context_request_ind.\
      msg_hdr.inst_id));

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmltecall_call_end( call_ptr );
      }
      else
      {
        #if defined(CM_GW_SUPPORTED)
        /* try to find the call which might already transfer to GW. */
        is_found = cm_find_call_id_gwl(
          cm_rpt_ptr->cm_deact_bearer_context_request_ind.esm_hdr.connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_deact_bearer_context_request_ind.\
          msg_hdr.inst_id));

        if (is_found)
        {
          /* get the call pointer from the call ID */
          call_ptr = cmcall_ptr ( call_id );

          if (call_ptr == NULL)
          {
             CM_ERR_1( "cmcall_ptr NULL for call id %d",
                     call_id);
             return;
          }

          /*Update GW end cause */
          call_ptr->end_status= CM_CALL_END_LL_CAUSE;
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause_type =
                                                            SM_INTERNAL_CAUSE;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause.int_cause =
                                                              NO_GPRS_CONTEXT;
        
          cmcall_end( call_ptr );

        }
        else
        #endif
        {
           CM_ERR_0( "No call id for MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND");
        }
      }

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  case MM_CM_ACT_DRB_RELEASED_IND:
      CM_MSG_MED_2("RXD: CM_ACT_DRB_RELEASED_IND: %d, inst_id=%d",
        cm_rpt_ptr->cm_act_drb_released_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_act_drb_released_ind.msg_hdr.inst_id);
        
      /* find the call_id for the DRB_RELEASED_IND */
      is_found = cm_find_call_id_per_subs(
      CONN_ID_ESM,
      cm_rpt_ptr->cm_act_drb_released_ind.esm_hdr.connection_id,
      &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_act_drb_released_ind.msg_hdr.inst_id)
      );

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REL_IND );

        /* free the call ID */
        /* UMTS RAB Relese does not dealloate call id*/
        //cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );

      }
      else
      {
        #if defined(CM_GW_SUPPORTED)
        /* try to find the call which might already transfer to GW. */
        is_found = cm_find_call_id_gwl(
          cm_rpt_ptr->cm_act_drb_released_ind.esm_hdr.connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_act_drb_released_ind.msg_hdr.inst_id));

        if (is_found)
        {
          /* get the call pointer from the call ID */
          call_ptr = cmcall_ptr ( call_id );

          /*Update call_ptr */
          cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

          cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REL_IND );

          cmltecall_update_call_ptr_lte_to_gw_irat( call_ptr );
        }
        else
        #endif
        {
           CM_ERR_0( "No call id for MM_CM_ACT_DRB_RELEASED_IND");
        }
      }

      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  case MM_CM_DRB_SETUP_IND:
      CM_MSG_MED_2("RXD: CM_DRB_SETUP_IND: %d, inst_id=%d",
        cm_rpt_ptr->cm_drb_setup_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_drb_setup_ind.msg_hdr.inst_id);

      /* find the call_id for the DRB_SETUP_IND */
      is_found = cm_find_call_id_per_subs(
      CONN_ID_ESM,
      cm_rpt_ptr->cm_drb_setup_ind.esm_hdr.connection_id,
      &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_drb_setup_ind.msg_hdr.inst_id));

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REESTAB_IND );

      }
      else
      {
         CM_ERR_0( "No call id for MM_CM_DRB_SETUP_IND");
      }
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  case MM_CM_DRB_REESTABLISH_FAILED_IND:
      CM_MSG_MED_2("RXD: CM_DRB_REESTABLISH_FAILED_IND: %d, inst_id=%d",
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_drb_reestablish_failed_ind.msg_hdr.inst_id);

      /* find the call_id for the REESTABLISH_FAILED_IND */
      is_found = cm_find_call_id_per_subs(
      CONN_ID_ESM,
      cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.connection_id,
      &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_drb_reestablish_failed_ind.msg_hdr.\
      inst_id));

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REESTAB_FAIL );

      }
      else
      {
        #if defined(CM_GW_SUPPORTED)
        /* try to find the call which might already transfer to GW. */
        is_found = cm_find_call_id_gwl(
          cm_rpt_ptr->cm_drb_reestablish_failed_ind.esm_hdr.connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_drb_reestablish_failed_ind.\
          msg_hdr.inst_id));

        if (is_found)
        {
          /* get the call pointer from the call ID */
          call_ptr = cmcall_ptr ( call_id );

          /*Update call_ptr */
          cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

          cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REESTAB_FAIL );

          cmltecall_update_call_ptr_lte_to_gw_irat( call_ptr );
        }
        else
        #endif
        {
          CM_ERR_0( "No call id for MM_CM_DRB_REESTABLISH_FAILED_IND");
        }

      }
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  case MM_CM_DRB_REESTABLISH_REJECT_IND:
      CM_MSG_MED_2("RXD: CM_DRB_REESTABLISH_REJECT_IND: %d, inst_id=%d",
        cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
        cm_rpt_ptr->cm_drb_reestablish_reject_ind.msg_hdr.inst_id);

      /* find the call_id for the REESTABLISH_REJECT_IND */
      is_found = cm_find_call_id_per_subs(
      CONN_ID_ESM,
      cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
      &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_drb_reestablish_reject_ind.msg_hdr.inst_id));

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REESTAB_REJ );

      }
      else
      {
        #if defined(CM_GW_SUPPORTED)
        /* try to find the call which might already transfer to GW. */
        is_found = cm_find_call_id_gwl(
          cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
          &call_id,
          INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_drb_reestablish_reject_ind.\
          msg_hdr.inst_id));

        if (is_found)
        {
          /* get the call pointer from the call ID */
          call_ptr = cmcall_ptr ( call_id );

          /*Update call_ptr */
          cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

          cmcall_event( call_ptr, CM_CALL_EVENT_RAB_REESTAB_REJ );

          cmltecall_update_call_ptr_lte_to_gw_irat( call_ptr );
        }
        else
        #endif
        {
          CM_ERR_0( "No call id for CM_DRB_REESTABLISH_REJECT_IND");
        }

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_GET_PDN_CONN_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_PDN_CONNECTIVITY_REJECT_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_PDN_CONN_REJ_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_PDN_CONNECTIVITY_FAILURE_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_PDN_CONN_FAIL_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_PDN_DISCONNECT_REJECT_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_RES_ALLOC_REJ_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_RES_ALLOC_FAIL_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND:
      cmcall_event_2( NULL,
                      CM_CALL_EVENT_BEARER_MOD_REJ_IND,
                      cm_rpt_ptr);


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND:
      /* find the call_id for the MODIFY_BEARER_CONTEXT_REQUEST_IND */
      is_found = cm_find_call_id_per_subs(CONN_ID_ESM,
      cm_rpt_ptr->cm_modify_bearer_context_request_ind.esm_hdr.\
      connection_id, &call_id,
      INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_modify_bearer_context_request_ind.\
      msg_hdr.inst_id));

      if (is_found)
      {
        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        /*Update call_ptr */
        cmltecall_update_call_ptr( call_ptr, cm_rpt_ptr);

        cmcall_event( call_ptr, CM_CALL_EVENT_MOD_BEARER_IND );
      }
      else
      {
        CM_ERR_0( "No call id for MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND");
      }


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND:
    {
      cm_ss_e_type cm_ss = CM_SS_MAIN;
      
      #ifdef FEATURE_CM_LTE_CC
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /*
      ** Dynamically allocate a call control params object
      */
      lte_ps_cc_params_ptr=(cm_lte_ps_cc_params_s_type *)cm_mem_malloc(
          sizeof(cm_lte_ps_cc_params_s_type));

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* sdf id */
      lte_ps_cc_params_ptr->sdf_id =
        cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.esm_hdr.sdf_id;

      /* transaction id */
      lte_ps_cc_params_ptr->pti =
        cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.pti;

      /* as_id */
      lte_ps_cc_params_ptr->as_id =
        INST_ID_TO_SYS_AS_ID(cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.msg_hdr.inst_id);

      CM_MSG_MED_3("RXD: PDN_CONNECITIVITY_PACKED_REQUEST_IND, inst_id=%d, asubs_id=%d, pti=%d",
                    cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.msg_hdr.inst_id,
                    lte_ps_cc_params_ptr->as_id, lte_ps_cc_params_ptr->pti);

      if (cmph_is_ssim())
      {
        lte_ps_cc_params_ptr->as_id = SYS_MODEM_AS_ID_1;
      }

      /* copy packed message and set packed message length */
      if( cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.pkd_msg_len >= CM_MAX_PDN_PACKED_SIZE )
      {
         lte_ps_cc_params_ptr->length = memscpy( lte_ps_cc_params_ptr->pkd_msg,
                                                 sizeof(lte_ps_cc_params_ptr->pkd_msg),
                                                 cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.pkd_msg,
                                                 CM_MAX_PDN_PACKED_SIZE );
      }
      else
      {
         lte_ps_cc_params_ptr->length = memscpy( lte_ps_cc_params_ptr->pkd_msg,
                                                 sizeof(lte_ps_cc_params_ptr->pkd_msg),
                                                 cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.pkd_msg,
                                                 cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.pkd_msg_len );
      }

      /* copy apn_name */
      *(&(lte_ps_cc_params_ptr->apn_name)) = *(&(cm_rpt_ptr->cm_pdn_connectivity_packed_request_ind.apn_name));

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef CM_DEBUG
      #error code not present
#endif
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Determine cm_ss base on asubs_id and rat */
      cm_ss = cmph_determine_ss_per_rat(lte_ps_cc_params_ptr->as_id, 
                                                      SYS_SYS_MODE_LTE);

      cdma_subscription_source = cmph_get_cdma_subscription_source();
      gwl_subscription_source = cmph_get_gw_subscription_source(cm_ss);

      CM_MSG_MED_3("map to cm_ss=%d, cdma_subscription_source=%d,gwl_subscription_source=%d", 
                    cm_ss, cdma_subscription_source, gwl_subscription_source);

      /* Start call control */
      ret_val = cmcc_call_control_processing_lte (cdma_subscription_source,
                                                   gwl_subscription_source,
                                        cmltecall_call_control_complete_cb,
                                              lte_ps_cc_params_ptr,
                                              cm_ss,
                                              lte_ps_cc_params_ptr->as_id
                                              );

      CM_MSG_HIGH_1("cmcc_call_control_processing_lte return %d",ret_val);

      if (!ret_val)
      {

        CM_MSG_HIGH_0("cmcc_call_control_processing_lte return FALSE");

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*
         ** Dynamically allocate ESM message buffer
         */
        pdn_packed_cnf = (esm_msg *)cm_mem_malloc(sizeof(esm_msg));

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* Send NAS_ESM_PDN_CONNECT_PACKED_CNF to ESM */

        /* call control status */
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.status =
          ESM_MSG_REJECTED;

        /* packed message length */
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pkd_msg_len = 0;

        /* connection_id */
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.esm_hdr.connection_id = 0xFF;

        /* transaction id */
        pdn_packed_cnf->esm_pdn_connectivity_pkd_cnf.pti = 
          lte_ps_cc_params_ptr->pti;

        CM_MSG_HIGH_0("CM->MSGR: PDN_CONNECT_PACKED_CNF");
        
        (void) cm_msgr_send( NAS_ESM_PDN_CONNECT_PACKED_CNF, MSGR_MM_CM,
                             (msgr_hdr_s *)pdn_packed_cnf, sizeof(esm_msg) );

        /* Free allocated ESM message buffer */
        cm_mem_free (pdn_packed_cnf);
      }

      /* Deallocate the memory after sending the call control cnf. */
      cm_mem_free(lte_ps_cc_params_ptr);

      #endif /* FEATURE_CM_LTE_CC */
     }
     break;
    
    /* SRVCC cancel notification ind
    */
    case MM_CM_ESM_NOTIFICATION_IND:
    {
      sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
      CM_MSG_HIGH_3("RXD: MM_CM_ESM_NOTIFICATION_IND %d, inst_id %d, lte sub %d",
                    cm_rpt_ptr->cm_esm_notif_ind.notification_ind,
                    cm_rpt_ptr->cm_esm_notif_ind.msg_hdr.inst_id,
                    cmph_get_sub_with_lte_cap());

      as_id = cmph_get_as_id_from_ll_ind(INST_ID_TO_SYS_AS_ID(
                            cm_rpt_ptr->cm_esm_notif_ind.msg_hdr.inst_id));

     #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if ( cm_rpt_ptr->cm_esm_notif_ind.notification_ind != 
           SRVCC_HO_CANCELLED)
           break;   
   
    CM_MSG_HIGH_0("RECVD CM_MM_SRVCC_HANDOVER_CANCEL_IND");
    ims_call_id= cmcall_get_active_srvcc_volte_call(CM_CALL_TYPE_VOICE_MASK |
                                                    CM_CALL_TYPE_EMERG_MASK |
                                                    CM_CALL_TYPE_VT_MASK,
                                                    as_id);
    
    cmcall_obj_iterator_init(&call_itr);

    call_ptr = cmcall_obj_get_next(&call_itr);
  
    if(cm_get_srvcc_ho_error_state() != CM_SRVCC_HO_ERR_UNMANAGED_CALLS 
     && cm_get_srvcc_ho_error_state() != CM_SRVCC_HO_UNMANAGED_CALL_COMBINATION  )
    {
     while(call_ptr != NULL)
     { 
         
      /* End ext IMS participant calls , both single/conf participants*/
      if ( call_ptr->is_srvcc_call && !cmwcall_is_int_ims())
      {
       cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );
      }
       /* Retain conf call  id of internal ims conf  calls */
      else if ( call_ptr->is_srvcc_call && cmwcall_is_int_ims())
      {
        if(call_ptr->is_mpty )
        {
          cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );
        }
        /* single ims calls , Conf ims calls wont be touched */
        else
        {
          CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_CANCEL_IND, retain IMS conn id");
        
          if (!cm_unreserve_connection_id(call_ptr->gw_connection_id, call_ptr->asubs_id ))
          {
            CM_ERR_2( "Could not unreserve W connection id %d for %d.",
                       call_ptr->gw_connection_id, call_ptr->call_id);
          }
  
          cm_set_connection_id(call_ptr,call_ptr->ims_connection_id);
             
          cm_set_connection_id_src(call_ptr,0);
             
          call_ptr->parent_call_id = CM_CALL_ID_UNASSIGNED;
  
          call_ptr->sys_mode = SYS_SYS_MODE_LTE;
              
          call_ptr->is_srvcc_call = FALSE;
   
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
		  
		  if(call_ptr->call_state == CM_CALL_STATE_ORIG)
		  {
		    call_ptr->sr.orig_sent_on_mode =SYS_SYS_MODE_LTE;
		  }

          /* reassingn VT call type upon Cancel*/
          if(call_ptr->ims_call_type == CM_CALL_TYPE_VT)
          {
            call_ptr->call_type = call_ptr->ims_call_type;
            call_ptr->ims_call_type = CM_CALL_TYPE_NONE;
          } 
        }    
             
      }

      call_ptr = cmcall_obj_get_next(&call_itr);
        
    }//while

    /* if no ims call present, we have to send fail with empty call */
    if(!cmwcall_is_int_ims())
    {
      cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_CANCEL,CM_CALL_ID_INVALID,ims_call_id, as_id);

    }
        /* we have ims single/conf call, send ipapp notify */
    else
    {
      cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_CANCEL,CM_CALL_ID_INVALID,ims_call_id, as_id);

      cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_CANCEL_NOTIFY, as_id);

      CM_MSG_HIGH_0("IPAPP HO CANCEL sent");
     }
    }
    /* case where IMS fills in unmanageble calls in cache */
    else
    {
      cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_CANCEL,CM_CALL_ID_INVALID,ims_call_id, as_id);
    }
    /* HO processing complete, reset the IMS SRVCC cache */
    cm_ims_cache_reset();
    #endif
  }
  break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR_2("RXD: Wrong msg_id %d, inst_id %d",
               cm_rpt_ptr->msg_hdr.id, cm_rpt_ptr->msg_hdr.id);
      break;

  } /* switch */

  #ifdef FEATURE_CM_LTE_CC
  SYS_ARG_NOT_USED(lte_ps_cc_params_ptr);
  #endif

}



/*===========================================================================

FUNCTION  cmltecall_client_cmd_check

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
  If originating dial-string matches one of the special numbers that is
  stored in the phone-book, this function changes the call type to E911,
  OTASP, etc.

===========================================================================*/
static cm_call_cmd_err_e_type  cmltecall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
)
{

  cmcall_s_type               *call_ptr = NULL;
    /* Pointer to the call object */

  cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  CM_MSG_HIGH_1( "cmd check %d",call_cmd_ptr->cmd_type);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* check cmd is already in error or not */
  if (call_cmd_ptr->info.cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    return call_cmd_ptr->info.cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (call_cmd_ptr->cmd == CM_CALL_CMD_ACT_BEARER_RSP)
  {
    call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );

    /* Check for call id errors */
    if(call_ptr == NULL)
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    }
  }

  return cmd_err;

} /* cmltecall_client_cmd_check() */



/*===========================================================================

FUNCTION cmltecall_client_cmd_forward_ll

DESCRIPTION
  Forward a client call commands to Lower Layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a reply from MN before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_client_cmd_forward_ll(

    cm_call_cmd_s_type    *call_cmd_ptr
        /* Pointer to a CM command */
)
/*lint -esym(818, cmd_ptr) */
{
  cmcall_s_type                        *call_ptr;
        /* Pointer to a call object */

  byte                                 connection_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_cmd_ptr->cmd )
  {
    case CM_CALL_CMD_ACT_BEARER_RSP:

      if (!cm_find_connection_id (call_cmd_ptr->info.call_id,
                     &connection_id))
      {
        CM_ERR_0 ("err in getting connection id");
        break;
      }

      if ( (call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id )) == NULL)
      {
        CM_ERR_0 ("err in getting call id");
        break;
      }

      if (call_ptr->asubs_id != call_cmd_ptr->info.lte_info.as_id)
      {
        CM_MSG_MED_1 ("err in ACT_BEARER_RSP CMD as_id=%d",
                call_cmd_ptr->info.lte_info.as_id);
      }

      CM_MSG_MED_3("eps_bearer_id in DS cmd is %d eps_bearer_id in call_id %d is %d",
                  call_cmd_ptr->info.lte_info.eps_bearer_id,
                  call_ptr->call_id,
                  call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id);

      /* Ending all stale GW calls as device moved to LTE now*/
      cmcall_end_stale_gw_ps_calls(call_cmd_ptr->info.lte_info.as_id);

      /* Send ACT_BEARER_CONTEXT_REQUEST Response */
      cmltecall_send_act_bearer_context_rsp (call_ptr,
        &(call_cmd_ptr->info.lte_info));

      /* If DS accepts ACT BEARER request, set call state to CONV. */
      if (call_cmd_ptr->info.lte_info.is_accept)
      {
        /* Update call_state */
        call_ptr->call_state       = CM_CALL_STATE_CONV;
        call_ptr->call_subst.conv  = CM_CALL_CONV_CALL_ACTIVE;
	/* store the is_emergency_bearer_info in call_ptr */
	call_ptr->cmcall_mode_info.info.lte_info.is_emergency_bearer = call_cmd_ptr->info.lte_info.is_emergency_bearer;
        cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
        call_ptr->call_connect_time = time_get_uptime_secs();
        cmcall_event(call_ptr, CM_CALL_EVENT_CONNECT);
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_RAB_REESTAB:

      if (!cm_find_connection_id (call_cmd_ptr->info.call_id,
                                  &connection_id))
      {
        CM_ERR_0 ("CM_CALL_CMD_RAB_REESTAB: err in getting connection id");
        break;
      }

      if ( (call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id )) == NULL)
      {
        CM_ERR_0 ("CM_CALL_CMD_RAB_REESTAB: err in getting call id");
        break;
      }

      CM_MSG_MED_3("CM_CALL_CMD_RAB_REESTAB call_id=%d, connection_id=%d, bearer_id=%d",
                  call_cmd_ptr->info.call_id,connection_id,
                  call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id);

      if (call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id != \
          call_cmd_ptr->info.lte_info.eps_bearer_id)
      {
        CM_MSG_HIGH_3("eps_bearer_id in call_id %d is %d. eps_bearer_id in cmd_ptr is %d",
                    call_ptr->call_id,
                    call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id,
                     call_cmd_ptr->info.lte_info.eps_bearer_id);

        call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id = \
          call_cmd_ptr->info.lte_info.eps_bearer_id;

        call_ptr->cmcall_mode_info.info.lte_info.rb_id = \
          call_cmd_ptr->info.lte_info.rb_id;
      }
      call_ptr->cmcall_mode_info.info.lte_info.sys_lte_reestab_calltype =
                   call_cmd_ptr->info.lte_info.sys_lte_reestab_calltype;

      /* unlike other req asubs_id isn't recvd in CM_CALL_CMD_RAB_REESTAB 
         get it from call ptr, which should master value */
      call_ptr->cmcall_mode_info.info.lte_info.as_id = call_ptr->asubs_id;

      /* Send DRB_REESTABLISH_REQ */
      cmltecall_send_drb_reestablish_req (call_ptr);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_MOD_BEARER_RSP:

      if (!cm_find_connection_id (call_cmd_ptr->info.call_id,
                     &connection_id))
      {
        CM_ERR_0 ("err in getting connection id");
        break;
      }

      if ( (call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id )) == NULL)
      {
        CM_ERR_0 ("err in getting call id");
        break;
      }

      if (call_ptr->asubs_id != call_cmd_ptr->info.lte_info.as_id)
      {
        CM_ERR_1 ("err in MOD_BEARER_RSP CMD as_id=%d",
                call_cmd_ptr->info.lte_info.as_id);
      }

      /* Send MOD_BEARER_CONTEXT_REQUEST Response */
      cmltecall_send_mod_bearer_context_rsp (call_ptr,
        &(call_cmd_ptr->info.lte_info));

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CALL_CMD_PDN_CONNECTIVITY_REQ:
    case CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ:
    case CM_CALL_CMD_PDN_DISCONNECT_REQ:
    case CM_CALL_CMD_RES_ALLOC_REQ:
    case CM_CALL_CMD_RES_ALLOC_ABORT_REQ:
    case CM_CALL_CMD_BEARER_RES_MOD_REQ:

      cmltecall_send_requests (call_cmd_ptr);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_ERR_1( "call cmd %d", (int)(call_cmd_ptr->cmd));
  } //end of switch


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmltecall_client_cmd_forward_ll() */



/*===========================================================================

FUNCTION cmltecall_client_cmd_proc

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmltecall_client_cmd_proc(

    cm_call_cmd_s_type  *call_cmd_ptr
        /* pointer to a CM command */
)
{

  cm_call_cmd_err_e_type    call_cmd_err;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_err = cmltecall_client_cmd_check( call_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

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
  cmltecall_client_cmd_forward_ll( call_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmltecall_client_cmd_proc() */



/*===========================================================================

FUNCTION cmltecall_map_lte_info_to_gw_info

DESCRIPTION
  During LTE to GW call transfer, map LTE info to GW info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_map_lte_info_to_gw_info(

    cmcall_s_type                             *call_ptr,
        /* pointer to the call object */
    sys_radio_access_tech_e_type              target_rat
        /* the active service system */
)
{
  gsm_wcdma_cmcall_ps_type *ps_wcall_info_ptr;
    /* Point at wcdma call info */
  cm_lte_call_info_s_type  *lte_info_ptr;
    /* Point at LTE call info */
  byte                     esm_connection_id;

  ps_wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info;
  lte_info_ptr = &call_ptr->cmcall_mode_info.info.lte_info;
  esm_connection_id = call_ptr->gw_connection_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set the GW connection id and SRC for GW call
  */
  call_ptr->gw_connection_id = call_ptr->cmcall_mode_info.info.lte_info.\
    gsm_umts_connection_id.connection_id;
  call_ptr->gw_conn_id_src   = CONN_ID_SM;

  CM_MSG_HIGH_3("IRAT to GW: call_id:%d, sm_conn_id:%d, esm_conn_id:%d",
            call_ptr->call_id,
            call_ptr->gw_connection_id,
            esm_connection_id);

  /* Reset the GW PS info
  */
  cmwcall_reset_gw_wcdma_ps_info(ps_wcall_info_ptr);

  /* Map LTE info to GW PS call info */
  if (target_rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    call_ptr->cmcall_mode_info.info_type   = CM_CALL_MODE_INFO_GSM_PS;
  }
  else if ((target_rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
           (target_rat == SYS_RAT_TDS_RADIO_ACCESS))
  {
    call_ptr->cmcall_mode_info.info_type   = CM_CALL_MODE_INFO_WCDMA_PS;
  }
  else
  {
    CM_ERR_1("Wrong RAT %d", target_rat);
  }

  ps_wcall_info_ptr->nsapi.valid         = TRUE;
  ps_wcall_info_ptr->nsapi.nsapi         = lte_info_ptr->eps_bearer_id;
  ps_wcall_info_ptr->qos                 = lte_info_ptr->neg_qos;
  ps_wcall_info_ptr->h_comp              = lte_info_ptr->h_comp;
  ps_wcall_info_ptr->d_comp              = lte_info_ptr->d_comp;
  ps_wcall_info_ptr->apn_name            = lte_info_ptr->apn_name;
  ps_wcall_info_ptr->llc_sapi            = lte_info_ptr->neg_llc_sapi;
  ps_wcall_info_ptr->minimum_qos.valid   = FALSE;
  ps_wcall_info_ptr->trans_id            = call_ptr->gw_connection_id;
  ps_wcall_info_ptr->esm_conn_id         = esm_connection_id;

  /* TBD: PDP Address (pdp_addr) */

  /* TBD: config_options */

  /* TBD: tft */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map default_bearer to primary_pdp
  **  map dedicated bearer to secondary_pdp
  */
  if (lte_info_ptr->bearer_type == CM_LTE_BEARER_TYPE_DEFAULT)
  {
    ps_wcall_info_ptr->primary_pdp = TRUE;
  }
  else if (lte_info_ptr->bearer_type == CM_LTE_BEARER_TYPE_DEDICATED)
  {
    ps_wcall_info_ptr->primary_pdp = FALSE;
  }
  else
  {
    CM_MSG_HIGH_1("Unexpected LTE bearer type %d",
                (int)lte_info_ptr->bearer_type);
  }

  /*
  ** group_id is returned at the activation of Primary context and
  ** is used for secondary activation
  */
  ps_wcall_info_ptr->pdp_group_id    = lte_info_ptr->pdp_group_id;

  /* TBD: ps_wcall_info_ptr->is_net_alloc_qos_diff_from_req */
  ps_wcall_info_ptr->is_net_alloc_qos_diff_from_req = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_ptr->call_state     = CM_CALL_STATE_CONV;
  call_ptr->call_subst.conv  = CM_CALL_CONV_CALL_ACTIVE;
  
  cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
  call_ptr->call_connect_time = time_get_uptime_secs();

}



/*===========================================================================

FUNCTION cmltecall_map_gw_info_to_lte_info

DESCRIPTION
  During GW to LTE call transfer, map GW PS info to LTE info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmltecall_map_gw_info_to_lte_info(

    cmcall_s_type     *call_ptr
        /* pointer to the call object */
)
{
  gsm_wcdma_cmcall_ps_type *ps_wcall_info_ptr;
    /* Point at wcdma call info */
  cm_lte_call_info_s_type  *lte_info_ptr;
    /* Point at LTE call info */
  gsm_umts_connection_id_T gw_connection_id;

  ps_wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info;
  lte_info_ptr = &call_ptr->cmcall_mode_info.info.lte_info;
  gw_connection_id.valid = TRUE;
  gw_connection_id.connection_id = call_ptr->gw_connection_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set the ESM connection id and SRC for LTE call
  */
  call_ptr->gw_connection_id = ps_wcall_info_ptr->esm_conn_id;
  call_ptr->gw_conn_id_src   = CONN_ID_ESM;

  /* Map LTE info to GW PS call info */
  call_ptr->cmcall_mode_info.info_type   = CM_CALL_MODE_INFO_LTE;

  /* Reset LTE info */
  cmltecall_reset(lte_info_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  lte_info_ptr->gsm_umts_connection_id = gw_connection_id;

  CM_MSG_HIGH_3("IRAT to LTE: call_id:%d, sm_conn_id:%d, esm_conn_id:%d",
            call_ptr->call_id,
            lte_info_ptr->gsm_umts_connection_id.connection_id,
            ps_wcall_info_ptr->esm_conn_id);

 (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                CM_SS_MAIN,
                                CM_ACT_UPDATE_REAS_ACT_END );
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || \
      defined FEATURE_MMODE_SC_SGLTE
 (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                CM_SS_HYBR_2,
                                CM_ACT_UPDATE_REAS_ACT_END );
  #endif

 (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                CM_SS_HDR,
                                CM_ACT_UPDATE_REAS_ACT_END );

 #if defined FEATURE_MMODE_SXLTE_G || defined FEATURE_TRIPLE_SIM
  (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_3,
                                 CM_ACT_UPDATE_REAS_ACT_END );
 #endif

  lte_info_ptr->apn_name           = ps_wcall_info_ptr->apn_name;
  lte_info_ptr->eps_bearer_id      = ps_wcall_info_ptr->nsapi.nsapi;
  lte_info_ptr->neg_llc_sapi     = ps_wcall_info_ptr->llc_sapi;
  lte_info_ptr->neg_qos      = ps_wcall_info_ptr->qos;
  lte_info_ptr->pdn_addr.valid     = ps_wcall_info_ptr->pdp_addr.valid;
  lte_info_ptr->connection_id      = ps_wcall_info_ptr->esm_conn_id;

  /* PDN address in LTE is 12 bytes. The lower order 4 bytes corresponds to
  IPv4 an higher order 8 bytes corresponds to
  the IID of IPv6. In LTE the lower order 8 bytes of IPv6 are ignored
  +-------------------------------------+
  |   (8  +  8)IPv6  |    4(IPv4)    | = 20 bytes in UMTS
  +------------------------------------+
  */
  memset(&lte_info_ptr->pdn_addr.address[0],0x0,MAX_PDN_ADR_LEN);
  if(ps_wcall_info_ptr->pdp_addr.pdp_type_num == 33)
  {
     lte_info_ptr->pdn_addr.pdn_addr_len = memscpy( &lte_info_ptr->pdn_addr.address[0],
                                                    sizeof(lte_info_ptr->pdn_addr.address),
                                                    &ps_wcall_info_ptr->pdp_addr.address[0],4 );

  lte_info_ptr->pdn_addr.pdn_type_val = NAS_ESM_IPV4;
  }

  if(ps_wcall_info_ptr->tft.valid == TRUE)
  {
  lte_info_ptr->ul_dl_tft     = ps_wcall_info_ptr->tft;
  }
  else
  {
  lte_info_ptr->ul_dl_tft.valid   = FALSE;
  }

  lte_info_ptr->h_comp = ps_wcall_info_ptr->h_comp;
  lte_info_ptr->d_comp = ps_wcall_info_ptr->d_comp;

  /* Map primary pdp to default bearer,
  **  map secondary pdp to dedicated bearer
  */
  if (ps_wcall_info_ptr->primary_pdp)
  {
    lte_info_ptr->bearer_type = CM_LTE_BEARER_TYPE_DEFAULT;
  }
  else
  {
    lte_info_ptr->bearer_type = CM_LTE_BEARER_TYPE_DEDICATED;
  }

  lte_info_ptr->pdp_group_id  = ps_wcall_info_ptr->pdp_group_id;


  //lte_info_ptr->packet_flow_id   = pdp_ctxt_ptr->flow_id;
  //lte_info_ptr->radio_pri      = ps_wcall_info_ptr->radio_pri;
  //lte_info_ptr->ti.connection_id = ps_wcall_info_ptr->trans_id;
  /* Convert QoS. DS will provide a function to be dropped into nas\services
  and we should call that here*/
  /* TBD: lte_info_ptr->sdf_qos */

}



/*===========================================================================

FUNCTION cmltecall_irat_call_object_transfer

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
EXTERN void cmltecall_irat_call_object_transfer(

    sys_radio_access_tech_e_type              target_rat
        /* the active service system */

)
{
  cmcall_s_type     *call_ptr = NULL;
  cm_iterator_type  call_itr;
  gsm_wcdma_cmcall_ps_type *ps_wcall_mode_ptr = NULL;

  cmcall_obj_iterator_init(&call_itr);

  if ((target_rat == SYS_RAT_GSM_RADIO_ACCESS) ||
      (target_rat == SYS_RAT_TDS_RADIO_ACCESS) ||
      (target_rat == SYS_RAT_UMTS_RADIO_ACCESS))
  {
    while( (call_ptr = cmcall_obj_get_next(&call_itr)) != NULL)
    {

      /* Only the call objects with valid gsm_umts_connection_id are transferable.
      */
      if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
         (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.valid))
      {
        /* Map LTE info to GW PS info */
        cmltecall_map_lte_info_to_gw_info(call_ptr, target_rat);

        /* After sending the call object transfer event to DS,
        ** change the mode back to GW
        */
        if ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GSM_PS)||
            (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_WCDMA_PS))
        {
          CM_MSG_MED_0("Change info_type from GSM_PS/WCDMA_PS to GW_PS");

          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
        }

        CM_MSG_HIGH_0("Finishing mapping LTE info to GW PS info.");
      }

    } /* while */

  }
  else if (target_rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    while( (call_ptr = cmcall_obj_get_next(&call_itr)) != NULL)
    {
      if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)
      {
        if(call_ptr->call_state == CM_CALL_STATE_ORIG)
        {
          if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
          {
            /* End the GW call which hasn't been sent to NAS.
            */
            call_ptr->end_status = CM_CALL_END_IRAT_PENDING_CALL;

            cmcall_end(call_ptr);

            CM_MSG_HIGH_1("Call Orig not sent to NAS:%d, End the call",
                       call_ptr->call_id);
          }
          else
          {
            /* For pending call, do not end the call,do not transfer the call.
            ** If the call orig is sent to NAS but still haven't received
            ** ACTIVATE_CNF, call is still in ORIG state.
            */
            CM_MSG_HIGH_1("Call Pending:%d, Do not transfer",
                        call_ptr->call_id);
          }
        }
        else if( call_ptr->call_state == CM_CALL_STATE_CC_IN_PROGRESS )
        {
          /* For call in Call Control, do not end the call, do not transfer the call.
          ** If the call orig is sent to NAS but still haven't received
          ** ACTIVATE_CNF, call is still in ORIG state or Call Control .
          */
          CM_MSG_HIGH_1( "Call Pending in Call Control:%d, Do not transfer",
                       call_ptr->call_id);
        }
        else
        {
          /* All the GW call objects are transferable to LTE.
           */
          /* Map GW PS info to LTE info */
          cmltecall_map_gw_info_to_lte_info(call_ptr);
          CM_MSG_HIGH_0("Finishing mapping GW PS info to LTE info.");
        }
      }

    } /* while */

  }
  else
  {
    CM_MSG_HIGH_1("Unexpected target_rat %d", target_rat);
  }

  /* Print all connection ids */
  cm_print_connection_ids();

  SYS_ARG_NOT_USED(ps_wcall_mode_ptr);

}

/*===========================================================================

FUNCTION cmltecall_irat_call_object_transfer_per_subs

DESCRIPTION
  During LTE to GW or GW to LTE call transfer, map call object info.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmltecall_irat_call_object_transfer_per_subs(

    sys_radio_access_tech_e_type              target_rat,
        /* the active service system */

    sys_modem_as_id_e_type             asubs_id
	    /* subs identifier */
)
{
  cmcall_s_type     *call_ptr = NULL;
  cm_iterator_type  call_itr;
  gsm_wcdma_cmcall_ps_type *ps_wcall_mode_ptr = NULL;

  /* Indicate if the call object is transfered */
  boolean is_obj_transfed = TRUE;

  cmcall_obj_iterator_init(&call_itr);

  if ((target_rat == SYS_RAT_GSM_RADIO_ACCESS) ||
      (target_rat == SYS_RAT_TDS_RADIO_ACCESS) ||
      (target_rat == SYS_RAT_UMTS_RADIO_ACCESS))
  {
    while( ((call_ptr = cmcall_obj_get_next(&call_itr)) != NULL))
    {
      /* Only the call objects with valid gsm_umts_connection_id are transferable.
      */
      if((call_ptr->asubs_id == asubs_id) && \
           (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
         (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.valid))
      {
        /* Map LTE info to GW PS info */
        cmltecall_map_lte_info_to_gw_info(call_ptr, target_rat);

        /* After sending the call object transfer event to DS,
        ** change the mode back to GW
        */
        if ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GSM_PS)||
            (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_WCDMA_PS))
        {
          CM_MSG_MED_0("Change info_type from GSM_PS/WCDMA_PS to GW_PS");

          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
        }

        CM_MSG_HIGH_0("Finishing mapping LTE info to GW PS info.");
      }

    } /* while */

  }
  else if (target_rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    while( ((call_ptr = cmcall_obj_get_next(&call_itr)) != NULL))
    {
      if((call_ptr->asubs_id == asubs_id) && (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS))
      {
        if(call_ptr->call_state == CM_CALL_STATE_ORIG)
        {
          if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
          {
            /* End the GW call which hasn't been sent to NAS.
            */
            call_ptr->end_status = CM_CALL_END_IRAT_PENDING_CALL;

            cmcall_end(call_ptr);

            CM_MSG_HIGH_1("IRAT GW->L: Call Orig not sent to NAS:%d, End the call",
                       call_ptr->call_id);
          }
          else
          {
            /* For pending call, do not end the call,do not transfer the call.
            ** If the call orig is sent to NAS but still haven't received
            ** ACTIVATE_CNF, call is still in ORIG state.
            */
            CM_MSG_HIGH_1("IRAT GW->L: Call Pending:%d, Do not transfer",
                        call_ptr->call_id);
          }
          /* GW call object is not transferred to LTE since the call is not connected */
          is_obj_transfed = FALSE;
        }
        else if( call_ptr->call_state == CM_CALL_STATE_CC_IN_PROGRESS )
        {
          /* For call in Call Control, do not end the call, do not transfer the call.
          ** If the call orig is sent to NAS but still haven't received
          ** ACTIVATE_CNF, call is still in ORIG state or Call Control .
          */
          CM_MSG_HIGH_1( "IRAT GW->L: Call Pending in Call Control:%d, Do not transfer",
                       call_ptr->call_id);
          /* GW call object is not transferred to LTE since the call is not connected */
          is_obj_transfed = FALSE;
        }
        else
        {
          /* All the GW call objects are transferable to LTE.
           */
          /* Map GW PS info to LTE info */
          cmltecall_map_gw_info_to_lte_info(call_ptr);
          CM_MSG_HIGH_0("IRAT GW->L: Finishing mapping GW PS info to LTE info.");
        }
      }

    } /* while */

  }
  else
  {
    CM_MSG_HIGH_1("IRAT: Unexpected target_rat %d", target_rat);
    is_obj_transfed = FALSE;
  }

  /* Print all connection ids */
  cm_print_connection_ids();

  SYS_ARG_NOT_USED(ps_wcall_mode_ptr);
  return is_obj_transfed;

}

#endif /* defined(FEATURE_CM_LTE) */
