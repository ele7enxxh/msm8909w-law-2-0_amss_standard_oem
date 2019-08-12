/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S
                
                A T   C O M M A N D

                V O I C E  I N T E R F A C E

GENERAL DESCRIPTION
  This Module has the Voice command related State Machine.
  It has Call Back and event handlers. It also contains internal
  State machine
  
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatvoice_answer_cmd
    Handle ATA voice answer commands.  This function is called by 
    dsatact_exec_ata_cmd when the current call's call type is VOICE.

  dsatvoice_end_voice_call
    This function is called by dsat707_exec_qcchv_cmd to end current
    active call if it is a voice call.

  dsatvoice_cmd_end_cb_handler
    This function is the handler for CM_CALL_CMD_END cb cmd.

  dsatvoice_is_voice_call
    Test if there is voice call for call ID.

  dsatvoice_go_to_connecting
    Change a call ID to DSAT_VOICE_STATE_CONNECTING state, 
    assumes call orig type command active.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Copyright (c) 2001 - 2016 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatvoice.c_v   1.7   12 Jul 2002 10:05:08   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatvoice.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/16   skc     Fixed $QCCHV issue
12/10/15   skc     Fix added to handle aborting of ATD when there
                   is no network service
02/02/15   sc      Fixed +CHV call end issue.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
03/27/14   pg      Fixed NOCARRIER response for command issue.
03/11/14   tk      Optimized debug macros usage in ATCoP.
09/13/13   sc      Fixed KW errors.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
02/20/13   tk      Fixed CLCC issue between PS data call ORIG and CONNECT.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
03/07/12   sk      Made CLCC common for all modes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/08/12   sk      Fixed User busy issue CR:333043.
01/19/12   sk      Feature cleanup.
04/20/11   bs      NIKEL Phase I support.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
06/14/10   ad      Fixed Compilation errors.
05/31/10   ad      Added support for S7 command.
02/08/10   ua      Removing support for CLVL and CMUT. 
12/15/09   nc      Featurisation changes for LTE.
06/11/09   nc      Modified the waiting call end using +CHUP to 
                   reflect the correct cause code.
04/29/09   sa      Fixed NO CARRIER response for MT terminated active calls.
04/20/09   ua      Fixed lint errors.
03/04/09   sa      AU level CMI modifications.
01/19/09   bs      Fixed +CHV command for MT calls.
12/17/08   ua      Removing SND support for 8k targets.
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
05/18/07   sa      Corrected dsatvoice_call_event_end for voice call abort.
03/02/07   sa      Added outgoing voice call support when there is an active call  
                   for ATD command.
02/23/07   sa      Correction to dsatvoice_call_event_end when COLP is enabled.
08/03/06   ua      If dsat_s0_val is set  then sending VOICE_CMD_ATSO instead of 
                   VOICE_CMD_ATA in voice_ring_te.
04/06/06   snb     ETSI feature wrap +COLP reference and use
                   dsatvoice_go_to_connecting_cmd_ext()/dsatvoice_is_voice_call()
                   interfaces locally but maintain others for AT707 use.
01/16/06   ua      Added support for +CDIP command.
11/10/05   ar      Return to command mode after voice call dialed.
05/25/05   gr      Set the ATA owned parameter for CDV initiated calls so
                   that NO CARRIER is generated during CHV hangup
05/05/05   iv      Added call id to sequence number mapping for +CLCC
04/14/05   sb      Changes for incoming CS call when serializer is enabled
04/01/05   tkk     removed the featurized code for vibrator and ringer. This is
                   handled in UI code now.
03/03/05   snb     Allow auto-answer to return OK result code.
02/24/05   sb      Reintroduce a bug fix that was overwritten by version #15 
                   check-in
02/15/05   snb     Fix CDMA-only build issues.
02/14/05   tkk     Added support for +CVIB command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/25/05   tkk     Added support for sound AT commands.
01/05/05   snb     Correct RINGing behavior.
12/15/04   snb     Save info for UI originated voice calls, do not give 
                   NO CARRIER on end of UI voice call not yet connected.
21/12/04   tkk     Added support of reporting CLI during call ringing phase.
10/18/04   ar      Add generic timer support
09/15/04   snb     Corrections For 6500 build, make aborted voice call behavior
                   match aborted CSD call's returning OK.
09/03/04   snb     Add +CHLD support.
09/24/03   sb      CDMA result code handler cannot handle CRC result code for
                   voice RING indication. Use basic result code instead.
09/04/03   ar      Adjust interface for dsatvoice_call_event_*().
02/26/03   wx      Put dialed number into last dial ph book.
10/28/02   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "dsatparm.h"
#include "ds3gsiolib.h"

#include "rex.h"

#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "dsat_v.h"

#ifdef FEATURE_ETSI_PBM
#include "dsatetsime.h"
#endif /* FEATURE_ETSI_PBM */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsipkt.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#include "cm_v.h"


extern voice_state_type voice_state[DSAT_SYS_MODEM_AS_ID_MAX][CM_CALL_ID_MAX];

LOCAL void voice_send_result_code
(
  sys_modem_as_id_e_type    subs_id,
  dsat_result_enum_type     result_code
);

/*===========================================================================

FUNCTION DSATVOICE_ANSWER_CMD

DESCRIPTION
  Handle ATA voice answer commands.  This function is called by 
  dsatact_exec_ata_cmd when the current call's call type is VOICE.

  This function sends request to CM to answer current incoming call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : If the command succeeds.
  DSAT_NO_CARRIER: If the request is rejected by CM.
  DSAT_ERROR     : If ATA is not expected.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_answer_cmd
(
  sys_modem_as_id_e_type subs_id
)
{
  /* Default result returned if no DSAT_VOICE_STATE_WAITING call is 
        found after ATA command  was issued */
  dsat_result_enum_type result = DSAT_ERROR;
  cm_call_id_type call_id;

#ifdef FEATURE_DUAL_ACTIVE
  if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_ANSWER))
  {
    return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }
#endif /* FEATURE_DUAL_ACTIVE */

  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    /* If waiting call ID is found... */
    if ( voice_state[subs_id][call_id].state == DSAT_VOICE_STATE_WAITING )
    {
#ifndef FEATURE_DSAT_CUST
      voice_stop_ring(subs_id);
#endif /* FEATURE_DSAT_CUST */

      if ( dsatcmif_answer_voice_call(subs_id,call_id,TRUE,voice_state[subs_id][call_id].call_mode) )
      {
        voice_state[subs_id][call_id].state = DSAT_VOICE_STATE_CONNECTING;
        voice_state[subs_id][call_id].cmd_active = VOICE_CMD_ATA;
        voice_state[subs_id][call_id].cmd_pending = TRUE;

        /* the command callback function will be called */
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DS_AT_MSG0_ERROR("ATCOP's answer-call is denied by CM");

        /* reset voice flag */
        dsatvoice_go_to_idle_state( subs_id, call_id );
        result = DSAT_NO_CARRIER;
      }
    }
  }

  return result;
} /* dsatvoice_answer_cmd */


/*===========================================================================

FUNCTION DSATVOICE_CMD_ORIG_CB_HANDLER

DESCRIPTION
  This function is the handler for a CM_CALL_CMD_ORIG cb cmd.

  This function handles cm_call_cmd_orig call status.  It returns
  dsat_result based on err conditions.  Go to IDLE_STATE if there is
  error.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK        : if no error reported by CM and +COLP disabled
  DSAT_ASYNC_CMD : if no error reported by CM and +COLP enabled
  DSAT_NO_CARRIER: if the one of the following errors is reported by CM,
                   CM_CALL_CMD_ERR_OFFLINE_S, 
                   CM_CALL_CMD_ERR_ORIG_RES_S,
                   CM_CALL_CMD_ERR_EMERGENCY_MODE_S, 
                   CM_CALL_CMD_ERR_SRV_TYPE_S,
                   CM_CALL_CMD_ERR_CALL_STATE_S,
                   CM_CALL_CMD_ERR_SRV_STATE_S
  DSAT_BUSY      : if the following error is reported by CM,
                   CM_CALL_CMD_ERR_IN_USE_S
  DSAT_ERROR     : other CM errors

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_orig_cb_handler
(
  sys_modem_as_id_e_type   subs_id,
  cm_call_cmd_err_e_type   call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  /* Default returned if command error and no 
     DSAT_VOICE_STATE_CONNECTING call ID found 
     (other client initiated origination) */
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  cm_call_id_type       call_id;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type     current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
  dsatetsicall_call_state_da_info  *call_da_val = NULL;

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS,subs_id, TRUE);
  if (NULL == call_da_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG1_MED("Orig cb called: %d.", call_cmd_err);

  /* Find the active call instance */
  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    /* If DSAT_VOICE_STATE_CONNECTING call ID is found (there should be only one)... */
    if ( voice_state[subs_id][call_id].state == DSAT_VOICE_STATE_CONNECTING )
    {
      break;
    }
  }
  if(call_id == CM_CALL_ID_MAX)
  {
    DS_AT_MSG0_ERROR("Could not find DSAT_VOICE_STATE_CONNECTING voice call");
    return result;
  }

  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /* When +COLP is enabled, it must be reported before returning to
     * command mode. Otherwise, return immediately to command mode per
     * 3GPP 27.007 section 6.2 & Annex G. */
    if( (voice_state[subs_id][call_id].call_mode == CM_CALL_MODE_INFO_CDMA)
        || ((dsat_num_item_type)dsatutil_get_val(
           DSATETSI_EXT_ACT_COLP_ETSI_IDX,0,0,NUM_TYPE) != DSAT_COLP_ENABLED) )
#endif /* FEATURE_DSAT_ETSI_MODE */
    {
      voice_state[subs_id][call_id].cmd_pending = FALSE;
      result = DSAT_OK;
    }
  }
  else
  {
    /* Return NO CARRIER or BUSY only if ATD initiated connect attempt */
    if ( VOICE_CMD_ATD == voice_state[subs_id][call_id].cmd_active || 
         VOICE_CMD_ABORT == voice_state[subs_id][call_id].cmd_active )
    {
      if (call_cmd_err == CM_CALL_CMD_ERR_OFFLINE_S ||
          call_cmd_err == CM_CALL_CMD_ERR_ORIG_RES_S ||
          call_cmd_err == CM_CALL_CMD_ERR_EMERGENCY_MODE_S ||
          call_cmd_err == CM_CALL_CMD_ERR_SRV_TYPE_S ||
          call_cmd_err == CM_CALL_CMD_ERR_CALL_STATE_S ||
          call_cmd_err == CM_CALL_CMD_ERR_SRV_STATE_S)
      {
        result = DSAT_NO_CARRIER;
      }
      else if (call_cmd_err == CM_CALL_CMD_ERR_IN_USE_S)
      {
        result = DSAT_BUSY;
      }
      else
      {
        result = DSAT_ERROR;
      }
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        /* clear the flag for ATD origination in case of failure*/
        call_da_val->etsicall_was_call_active = FALSE;
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
    else
    {
      DS_AT_MSG1_MED("Ignoring call orig error: %d",call_cmd_err);
    }

    /* Initialize state */
    dsatvoice_go_to_idle_state( subs_id, call_id );
  }

  return result;
} /* dsatvoice_cmd_orig_cb_handler */

/*===========================================================================

FUNCTION DSATVOICE_END_VOICE_CALL

DESCRIPTION
  This function is called by dsat707_exec_qcchv_cmd to end current
  incoming call if it is a voice call.

  This function issues a request to CM to end a voice call.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_CMD : The end request is sent, callback function
                   will be called.
  DSAT_ERROR     : The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_end_voice_call
(
  sys_modem_as_id_e_type subs_id
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  cm_call_id_type call_id;

#ifdef FEATURE_DUAL_ACTIVE
  if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_END))
  {
    return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }
#endif /* FEATURE_DUAL_ACTIVE */

  /* If error on attempting to answer look for DSAT_VOICE_STATE_WAITING call ID... */
  for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
  {
    /* If DSAT_VOICE_STATE_WAITING or DSAT_VOICE_STATE_ACTIVE call ID is found (there should be only one).*/
    if (( voice_state[subs_id][call_id].state == DSAT_VOICE_STATE_WAITING )||
       ( voice_state[subs_id][call_id].state == DSAT_VOICE_STATE_ACTIVE ))
    {
      if ( dsatcmif_end_calls(1, &call_id) )
      {
        /* the command callback function will be called */
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        /* no buffer to send the request */
        DS_AT_MSG0_ERROR("ATCOP's end call request is denied by CM");
        result = DSAT_ERROR;
      }
    } 
  }

  return result;
} /* dsatvoice_end_voice_call */

/*===========================================================================

FUNCTION DSATVOICE_CMD_END_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_END cb cmd.

  If status shows cm_call_cmd_end succeeded, return OK to TE and 
  go to IDLE_STATE. 
  If status shows cm_call_cmd_end failed, return ERROR to TE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : if no error.
  DSAT_ERROR: if there is error.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_end_cb_handler
(
  sys_modem_as_id_e_type    subs_id,
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  dsat_result_enum_type result;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */

  DS_AT_MSG1_MED("End cb called: %d.", call_cmd_err);
  
  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /* Any call that can be ended will give an end event at which time
       state will be updated  */

#ifdef FEATURE_DSAT_ETSI_MODE
    if ( (IS_ETSI_MODE(current_mode)) && 
         (dsatutil_strncmp_ig_sp_case((const byte *)dsat_curr_cmd_var.parse_table->name,(const byte *)("+CHV"),4) != 0) &&
         (dsatutil_strncmp_ig_sp_case((const byte *)dsat_curr_cmd_var.parse_table->name,(const byte *)("$QCCHV"),6) != 0) )
    {
      /* Wait for END events for all active calls */
      result = DSAT_ASYNC_CMD;
    }
    else
#endif /* FEATURE_DSAT_ETSI_MODE */
    {
      result = DSAT_OK;
    }
  }
  else
  {
    /* we could not end the call */
    DS_AT_MSG0_ERROR("End call failed");
#ifdef FEATURE_DSAT_ETSI_MODE
    if ( IS_ETSI_MODE(current_mode) )
    {
      etsicall_cmd_pending[subs_id] = CMD_NONE;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatvoice_cmd_end_cb_handler */

/*===========================================================================

FUNCTION DSATVOICE_IS_VOICE

DESCRIPTION
  Test if there is any incoming voice call.
  This function used by AT707 code.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if there is any incoming voice call.
  FALSE: otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_is_voice
(
  sys_modem_as_id_e_type subs_id
)
{
  return dsatvoice_is_voice_call( subs_id, DSAT_CALLID_ALL );
}


/*===========================================================================

FUNCTION DSATVOICE_IS_VOICE_CALL

DESCRIPTION
  Test if there is voice call for given call ID.  All call IDs will be
  checked if DSAT_CALLID_ALL is passed in, reporting those in DSAT_VOICE_STATE_WAITING
  state.  For specific call ID, returns true if not DSAT_VOICE_STATE_IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if there is voice call.
  FALSE: otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_is_voice_call
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
)
{
  cm_call_id_type index;

  /* Check for case to look at all call ID's */
  if( DSAT_CALLID_ALL == call_id )
  {
    /* look for DSAT_VOICE_STATE_WAITING or DSAT_VOICE_STATE_ACTIVE call ID. */
    for ( index = 0; index < CM_CALL_ID_MAX; index++ )
    {
      /* If DSAT_VOICE_STATE_WAITING or DSAT_VOICE_STATE_ACTIVE call ID is found (there should be only one).*/
      if (( voice_state[subs_id][index].state == DSAT_VOICE_STATE_WAITING )||
         ( voice_state[subs_id][index].state == DSAT_VOICE_STATE_ACTIVE ))
      {
        return TRUE;
      } 
    }
  }
  else
  {
    /* See if call ID is not idle */
    if ( ( call_id < CM_CALL_ID_MAX ) && 
         (voice_state[subs_id][call_id].state != DSAT_VOICE_STATE_IDLE) )
    {
      return TRUE;
    } 
  }

  return FALSE;
}


/*===========================================================================

FUNCTION DSATVOICE_GO_TO_CONNECTING

DESCRIPTION
  Change a call ID to DSAT_VOICE_STATE_CONNECTING state, assumes call orig type command active.
  AT707 code uses this interface only for CDV command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_connecting
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
)
{
  dsatvoice_go_to_connecting_cmd_ext( subs_id, call_id, VOICE_CMD_ATD );
}/* dsatvoice_go_to_connecting */

/*===========================================================================

FUNCTION VOICE_SEND_RESULT_CODE

DESCRIPTION
  Send result code to TE by appending subscription ID at the end.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_send_result_code
(
  sys_modem_as_id_e_type subs_id,
  dsat_result_enum_type  result_code
)
{
  dsm_item_type *fmt_rsp_ptr;

  /* Get buffer to hold formatted response. */
  fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != fmt_rsp_ptr)
  {
    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    if ( fmt_rsp_ptr != NULL )
    {
#ifdef FEATURE_DUAL_ACTIVE
      /* Appending QCSIMAPP index to distinguish the URC */
      fmt_rsp_ptr->used += (word) snprintf(
                           (char*)(fmt_rsp_ptr->data_ptr + fmt_rsp_ptr->used),
                           (fmt_rsp_ptr->size - fmt_rsp_ptr->used),
                           "##%d", subs_id);
#endif /* FEATURE_DUAL_ACTIVE */

      /* Send the formatted result code. */
      dsatcmdp_send_urc(fmt_rsp_ptr, DSAT_COMPLETE_RSP);
    }
  }
  return;
}

/*===========================================================================

FUNCTION VOICE_SEND_RING_RESULT

DESCRIPTION
  Sends ring result code.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_send_ring_result
(
  sys_modem_as_id_e_type      subs_id,
  cm_call_mode_info_e_type    call_mode
)
{
  if (call_mode == CM_CALL_MODE_INFO_CDMA)
  {
    /* CDMA result code handler cannot handle CRC result code */
    voice_send_result_code(subs_id, DSAT_RING);
  }
  else
  {
    voice_send_result_code(subs_id, DSAT_CRC_CODE_VOICE);
#ifdef FEATURE_DSAT_ETSI_MODE
    if((dsat_num_item_type)dsatutil_get_val(
                DSATETSI_EXT_ACT_CLIP_ETSI_IDX,0,0,NUM_TYPE) == DSAT_CLIP_ENABLED)
    {
       dsatetsicall_report_clip_result_ring(subs_id);
    }
    if((dsat_num_item_type)dsatutil_get_val(
                DSATETSI_EXT_ACT_CDIP_ETSI_IDX,0,0,NUM_TYPE) == DSAT_CDIP_ENABLED)
    {
      dsatetsicall_report_cdip_result_ring(subs_id);
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }
}

