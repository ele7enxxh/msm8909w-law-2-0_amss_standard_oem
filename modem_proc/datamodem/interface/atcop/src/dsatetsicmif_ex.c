/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( E T S I  C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatetsicmif_init_cmif
    CM Interface initialization function
    
  dsatetsicmif_initiate_voice_call
    This function requests CM to start a voice call.

  dsatetsicmif_sups_class_action
    This function intiates the action in Call Manager to perform supplemental
    service commands for classes.

  dsatetsicmif_sups_change_password
    This function intiates the change of supplementary services facility
    password.
    
  dsatetsicmif_sups_process_ussd
    This function intiates the action in Call Manager to perform supplemental
    service commands for Unstructured Supplementary Service Data (USSD).

  dsatetsicmif_cm_sups_cmd_handler_ex
    This function is the handler function for the CM supplementary service
    commands.
    
  dsatetsicmif_cm_sups_event_handler_ex
    This function is the handler function for the CM supplementary service
    related events

  dsatetsicmif_report_ccwa_result 
    This function handles the +CCWA unsolicited result triggered by incoming 
    call event

  dsatetsicmif_attach_ps
    Wrapper function to inform CM to attach to a PS domain. Called from CGATT
    command

  dsatetsicmif_detach_ps
    Wrapper function to inform CM to detach from the PS domain. Called from 
    CGATT command

 dsatetsicmif_init_pdp_connect_state
    This function intializes the PDP context profile connection state
    information.

  dsatetsicmif_deactivate_all_contexts
    This function initiates PDP context deactivation for all contexts.

  dsatetsicmif_query_networks
    This function invokes the CM API to get either the list of
    available networks from the access stratum or the preferred
    network list from the SIM/USIM.

  dsatetsicmif_change_network_registration
    This function invokes the CM API to change the network registration
    preference.


INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsicmif_ex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/16   skc     Fixed USSD issue when both MO and MT USSD present.
04/17/15   tk      Fixed issue with unsupported SUPS error codes.
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/26/14   tk      Fixed issues in SUPS events handling.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
07/08/14   sc      Fixed SGLTE+G issue on SUB2.
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_DSAT_ETSI_MODE
/* this file should be included only in GSM or WCDMA mode */

#include <stdio.h>
#include <string.h>
#include "dsati.h"
#include "dsatdl.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#include "dsatetsictab.h"
#include "dsatetsicall.h"
#include "dsatcmif.h"
#include "dsatparm.h"
#include "cm.h"
#include "cmutil.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"

#include "mn_cm_exp.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsumtspspco.h"
#include "ps_ppp_ipcp.h"
#include "dsumtsps.h"
#include "dsatetsipkt.h"
#include "sm.h"
#include "sm_common.h"
#include "dsatvend.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "time_jul.h"
#include "time_svc.h"

extern boolean dsatetsime_set_time_zone_into_nv(sys_time_zone_type time_zone);

uint8 cops_no_mode_change = 0;

/* Cross-reference +COPS AcT to CM mode preference */
/* Table must be terminated with DSAT_COPS_ACT_MAX */
LOCAL const struct act_pref_xref
{
  dsat_cops_act_e_type  cops_act;
  cm_mode_pref_e_type   cm_pref;
} dsatetsicmif_act_pref_xref[] =
{
  { DSAT_COPS_ACT_GSM,    CM_MODE_PREF_GSM_ONLY },
#ifndef FEATURE_TDSCDMA
  { DSAT_COPS_ACT_UTRAN,  CM_MODE_PREF_WCDMA_ONLY },
#else
  { DSAT_COPS_ACT_UTRAN,  CM_MODE_PREF_TDS_WCDMA },
#endif /* FEATURE_TDSCDMA */
#ifdef FEATURE_DSAT_LTE
  { DSAT_COPS_ACT_EUTRAN, CM_MODE_PREF_LTE_ONLY },
#endif /* FEATURE_DSAT_LTE */
  { DSAT_COPS_ACT_AUTO,   CM_MODE_PREF_AUTOMATIC }, 
  { DSAT_COPS_ACT_MAX,    CM_MODE_PREF_NONE }
};

LOCAL dsat_result_enum_type etsicmifi_send_sups_error
(
  dsat_cme_error_e_type    error_code,
  sys_modem_as_id_e_type   subs_id
);

LOCAL dsat_result_enum_type etsicmif_report_sups_error 
(
  const ds_at_cm_sups_event_type * event_ptr,        /* Event info   */
  sys_modem_as_id_e_type            subs_id
);

LOCAL dsat_result_enum_type etsicmif_report_ussd_result_ex
(
  const ds_at_cm_sups_event_type *sups_event_ptr, /* SUPS event pointer */
  const cusd_result_e_type        result_code,    /* Result code */
  sys_modem_as_id_e_type          subs_id
);

/*===========================================================================

FUNCTION DSATETSICMIF_SET_CUSD_STATE

DESCRIPTION
  This function is used to set CUSD state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatetsicmif_set_cusd_state
(
  supserv_cm_op_s_type          *ss_cm_data_ptr,
  cusd_state_e_type              ussd_state,
  uint8                          invoke_id
)
{
  cusd_session_id_e_type    session_id;
  cusd_session_info_s_type *session_info_ptr = NULL;

  if( NULL == ss_cm_data_ptr)
  {
    return;
  }

  switch (ussd_state)
  {
    /* No USSD transaction */
    case DSAT_CUSD_STATE_NULL:
      ss_cm_data_ptr->pending_msg = FALSE;

      /* Clear USSD sessions */
      for ( session_id = DSAT_CUSD_SESSION_ID_MO;
            session_id < DSAT_CUSD_SESSION_ID_MAX;
            session_id++ )
      {
        session_info_ptr =
          &ss_cm_data_ptr->cusd_session_info[session_id];

        session_info_ptr->is_active      = FALSE;
        session_info_ptr->invoke_id      = 0;
        session_info_ptr->ussd_oper_type = processUnstructuredSS_Data;
      }
      break;

    /* MO USSD request */
    case DSAT_CUSD_STATE_MS_SEND:
      ss_cm_data_ptr->pending_msg = TRUE;

      session_info_ptr =
        &ss_cm_data_ptr->cusd_session_info[DSAT_CUSD_SESSION_ID_MO];

      session_info_ptr->is_active      = TRUE;
      session_info_ptr->invoke_id      = invoke_id;
      session_info_ptr->ussd_oper_type = processUnstructuredSS_Request;
      break;

    /* USSD release */
    case DSAT_CUSD_STATE_MS_ABORT:
      break;

    /* MT USSD request */
    case DSAT_CUSD_STATE_SS_REQUEST:
      ss_cm_data_ptr->pending_msg = TRUE;

      session_info_ptr =
        &ss_cm_data_ptr->cusd_session_info[DSAT_CUSD_SESSION_ID_MT];

      session_info_ptr->is_active      = TRUE;
      session_info_ptr->invoke_id      = invoke_id;
      session_info_ptr->ussd_oper_type = unstructuredSS_Request;
      break;

    /* MT USSD notification */
    case DSAT_CUSD_STATE_SS_NOTIFY:
      ss_cm_data_ptr->pending_msg = TRUE;

      session_info_ptr =
        &ss_cm_data_ptr->cusd_session_info[DSAT_CUSD_SESSION_ID_MT];

      session_info_ptr->is_active      = TRUE;
      session_info_ptr->invoke_id      = invoke_id;
      session_info_ptr->ussd_oper_type = unstructuredSS_Notify;
      break;

    /* Clear MT USSD transaction */
    case DSAT_CUSD_STATE_CLEAR_MT_USSD:
      session_info_ptr =
            &ss_cm_data_ptr->cusd_session_info[DSAT_CUSD_SESSION_ID_MT];	  

      session_info_ptr->is_active	   = FALSE;
      session_info_ptr->invoke_id	   = 0;
      session_info_ptr->ussd_oper_type = processUnstructuredSS_Data;			
      break;

    default:
      return;
  }

  ss_cm_data_ptr->ussd_state = ussd_state;

  return;
} /* dsatetsicmif_set_cusd_state */


/*===========================================================================

FUNCTION ETSICMIF_GET_CUSD_INFO

DESCRIPTION
  This function is used to get CUSD info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void etsicmif_get_cusd_info
(
  supserv_cm_op_s_type          *ss_cm_data_ptr,
  uint8                         *invoke_id_ptr,
  uint8                         *oper_type_ptr
)
{
  cusd_session_id_e_type    session_id = DSAT_CUSD_SESSION_ID_MAX;
  cusd_session_info_s_type *session_info_ptr = NULL;

  if( NULL == ss_cm_data_ptr)
  {
    return;
  }

  switch (ss_cm_data_ptr->ussd_state)
  {
    /* MO USSD request */
    case DSAT_CUSD_STATE_MS_SEND:
      session_id = DSAT_CUSD_SESSION_ID_MO;
      break;

    /* USSD release */
    case DSAT_CUSD_STATE_MS_ABORT:
      // Send release request to NAS for first active USSD session.
      // NAS releases the complete USSD transaction. When both MO and MT
      // USSD sessions are active, MO USSD session must be sent to NAS.
      for ( session_id = DSAT_CUSD_SESSION_ID_MO;
            session_id < DSAT_CUSD_SESSION_ID_MAX;
            session_id++ )
      {
        session_info_ptr =
          &ss_cm_data_ptr->cusd_session_info[session_id];

        if (TRUE == session_info_ptr->is_active)
        {
          break;
        }
      }
      break;

    /* MT USSD request */
    case DSAT_CUSD_STATE_SS_REQUEST:
    /* MT USSD notification */
    case DSAT_CUSD_STATE_SS_NOTIFY:
      session_id = DSAT_CUSD_SESSION_ID_MT;
      break;

    default:
      return;
  }

  if (session_id < DSAT_CUSD_SESSION_ID_MAX )
  {
    session_info_ptr =
      &ss_cm_data_ptr->cusd_session_info[session_id];

    if (NULL != invoke_id_ptr)
    {
      *invoke_id_ptr = session_info_ptr->invoke_id;
    }

    if (NULL != oper_type_ptr)
    {
      *oper_type_ptr = session_info_ptr->ussd_oper_type;
    }
  }

  return;
} /* etsicmif_get_cusd_info */


/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM supplementary service
  commands.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :      if there was any problem in execution.
    DSAT_ASYNC_CMD :  if it is a success and asynch cmd continues.
    DSAT_OK :         if it is a success and asynch cmd done. 

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsicmif_cm_sups_cmd_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type            result = DSAT_ASYNC_CMD;
  sys_modem_as_id_e_type           subs_id = dsat_get_current_subs_id(TRUE);
  ds_at_cm_sups_cmd_type          *sups_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  dsatetsicmif_sups_state_ms_info *sups_ms_val = NULL;

  DS_AT_MSG3_HIGH("Received command %d Error status is %d subs_id %d",
                  sups_cmd->cmd, sups_cmd->cmd_err, subs_id);

  sups_ms_val = dsat_get_base_addr_per_subs(DSAT_SUPS_MS_VALS, subs_id, TRUE);
  if (NULL == sups_ms_val)
  {
    return DSAT_ERROR;
  }

  if( DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatetsicmif_cm_sups_cmd_handler_fp))
  {
    result = dsatdl_vtable.dsatetsicmif_cm_sups_cmd_handler_fp(sups_cmd, result, subs_id);
  }

  /* Cleanup on error condition */
  if (DSAT_ASYNC_CMD != result)
  {
    /* Terminate further async messages */
    sups_ms_val->dsat_ss_cm_data.curr_class = (uint16)DSAT_SS_CLASS_MAX;

    /* Cleanup USSD transaction */
    switch (sups_cmd->cmd)
    {
      case CM_SUPS_CMD_PROCESS_USS:
      case CM_SUPS_CMD_ABORT:
      case CM_SUPS_CMD_RELEASE:
      case CM_SUPS_CMD_USS_NOTIFY_RES:
      case CM_SUPS_CMD_USS_RES:
        dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
        break;

      default:
        /* Do nothing */
        break;
    }
  }

  return result;
} /* dsatetsicmif_cm_sups_cmd_handler_ex */

/*===========================================================================

FUNCTION  DSATETSICMIF_CM_SUPS_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM supplementary service
  related events

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :       if there was any problem in execution
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_cm_sups_event_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  ds_at_cm_sups_event_type*  sups_event = dsat_get_cmd_payload_ptr(cmd_ptr);
#ifndef FEATURE_DSAT_CUST
  ds_at_sups_event_info_s_type * einfo_ptr = NULL;
  /* control loop over all +CCFC/+CLCK/+CCWA class parameter values */
  sys_modem_as_id_e_type         subs_id;
  dsatetsicmif_sups_state_ms_info  *sups_ms_val    = NULL;

  einfo_ptr = &sups_event->event_info;


  subs_id = sups_event->event_info.subs_id;
  if (!IS_VALID_SUBS_ID(subs_id))

  {
    DS_AT_MSG1_ERROR("Invalid Subs_id %d in SUPS handler",subs_id);
    return DSAT_ASYNC_EVENT;
  }
  sups_ms_val = dsat_get_base_addr_per_subs(DSAT_SUPS_MS_VALS, subs_id, TRUE);
  if (NULL == sups_ms_val)
  {
     return DSAT_ASYNC_EVENT;
  }
  /* Verify receipt of pending ATCOP message */
  if ((FALSE == dsatme_is_thin_ui()) &&
      (sups_event->event_info.sups_client_id != dsatcm_client_id))
  {
    /* For USSD commands, see if another client responded */
    /* with outgoing response*/
    if ((CM_SUPS_EVENT_USS_NOTIFY_RES == sups_event->event) ||
        (CM_SUPS_EVENT_USS_RES == sups_event->event))
    {
      /* Report event if indications to the TE are not blocked. */
      if ( FALSE == dsatcmdp_block_indications_ex() )
      {
        DS_AT_MSG1_HIGH("Received other client SupS event: %d", sups_event->event);

        /* Generate unsolicited result code */
        result = etsicmif_report_ussd_result_ex(sups_event,
                                                DSAT_CUSD_RESULT_OTHER,subs_id);
      }
    }
    
    /* No further processing for other client events */
    return result;
  }

  DS_AT_MSG3_HIGH("Received CM SupS event %d, for class: %d, conf type %d",
                  sups_event->event,
                  sups_ms_val->dsat_ss_cm_data.curr_class,
                  einfo_ptr->conf_type);

  switch (sups_event->event)
  {
    /* Do nothing for command invocation events */
    case CM_SUPS_EVENT_REGISTER:
    case CM_SUPS_EVENT_ERASE:
    case CM_SUPS_EVENT_ACTIVATE:
    case CM_SUPS_EVENT_DEACTIVATE:
    case CM_SUPS_EVENT_INTERROGATE:
    case CM_SUPS_EVENT_REG_PASSWORD:
    /* Unsupported events for now */
    case CM_SUPS_EVENT_ABORT:
    case CM_SUPS_EVENT_FWD_CHECK_IND:
    case CM_SUPS_EVENT_GET_PASSWORD_RES:
      break;

    case CM_SUPS_EVENT_REGISTER_CONF:
    case CM_SUPS_EVENT_ERASE_CONF:
    case CM_SUPS_EVENT_ACTIVATE_CONF:
    case CM_SUPS_EVENT_DEACTIVATE_CONF:
    case CM_SUPS_EVENT_REG_PASSWORD_CONF:
      /* Process message */
      if (einfo_ptr->ss_success)
      {
        /* Check network response anomaly */
        switch(einfo_ptr->conf_type)
        {
            
          case SS_ERROR_INFO:
            (void)etsicmif_report_sups_error (sups_event, subs_id);
            result = DSAT_CMD_ERR_RSP;
            break;

          default:
            break;
        }
      }
      else
      {
        /* Operation not succesful */
        result = DSAT_CMD_ERR_RSP;
        if (DSAT_ERROR ==
            etsicmif_report_sups_error (sups_event, subs_id))
        {
          result = DSAT_ERROR;
        }
      }
      break;

    case CM_SUPS_EVENT_INTERROGATE_CONF:
      /* Process message */
      if (einfo_ptr->ss_success)
      {
        /* Check network response anomaly */
        switch(einfo_ptr->conf_type)
        {
          case SS_ERROR_INFO:
            if(sups_ms_val->dsat_ss_abort_status == TRUE)
            {
              sups_ms_val->dsat_ss_abort_status = FALSE;
            }
            else
            {
              (void)etsicmif_report_sups_error (sups_event, subs_id);
              result = DSAT_CMD_ERR_RSP;
            }
            break;
          default:
            DS_AT_MSG1_HIGH("Unsupported STATIC CM SupS confirmation type: %d",
                     einfo_ptr->conf_type);
            break;
        }
      }
      else
      {
        if(sups_ms_val->dsat_ss_abort_status == FALSE)
        {
          (void)etsicmif_report_sups_error (sups_event, subs_id);
          result = DSAT_CMD_ERR_RSP;
        }
        else
        {
          sups_ms_val->dsat_ss_abort_status = FALSE;
        }
      }

    case CM_SUPS_EVENT_RELEASE:
    case CM_SUPS_EVENT_USS_NOTIFY_RES:
      /* Release transaction message (from MS) */
      /* Applies to Get Password and USSD operations */
      /* Cleanup USSD transaction */
       dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
      /* Return to command mode */
      result = DSAT_OK;
      break;

    case CM_SUPS_EVENT_USS_RES: 
      /* Cleanup only MT USSD transaction */
      dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_CLEAR_MT_USSD, 0);
      /* Return to command mode */
      result = DSAT_OK;
      break;

    case CM_SUPS_EVENT_PROCESS_USS_CONF:
      /* Process confirmation message (from NW) */
      if (einfo_ptr->ss_success)
      {
        /* Check network response anomaly */
        switch(einfo_ptr->conf_type)
        {
          case USS_DATA_INFO:
          case NO_INFO:
            result = etsicmif_report_ussd_result_ex(sups_event,
                                                    DSAT_CUSD_RESULT_DONE,subs_id);
            break;

          /* Network rejected request */
          case SS_ERROR_INFO:
            /* Explictly handle "not supported" message */
            if ((TRUE == einfo_ptr->ss_error.present) &&
                (ERROR_CODE_TAG == einfo_ptr->ss_error.error_code_tag) &&
                (facilityNotSupported == einfo_ptr->ss_error.error_code))
            {
              result = etsicmif_report_ussd_result_ex(sups_event,
                                                      DSAT_CUSD_RESULT_NOSUP,subs_id);
            }
            else
            {
              /* Check to see if result codes are enabled */
              if (1 == (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_CUSD_ETSI_IDX,0,0,MIX_NUM_TYPE))
              {
                /* General error reporting */
                result = etsicmif_report_sups_error(sups_event, subs_id);
              }
            }
            break;

          case CC_CAUSE_INFO:
            /* Check to see if result codes are enabled */
            if (1 == (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_CUSD_ETSI_IDX,0,0,MIX_NUM_TYPE))
            {
              result = etsicmif_report_sups_error(sups_event, subs_id);
            }
            break;

          default:
            DS_AT_MSG1_HIGH("Unsupported CM SupS confirmation type: %d",
                     einfo_ptr->conf_type);
            break;
        }
      }
      else
      {
        /* Generate +CME error */
        DS_AT_MSG0_HIGH("Unknown USSD process conf msg");
        /* Check to see if result codes are enabled */
        if (1 == (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_CUSD_ETSI_IDX,0,0,MIX_NUM_TYPE))
        {
          if (DSAT_DL_STATUS_UNLOADED == DSAT_DL_GET_STATUS())
          {
            dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
          }
        }
      }
      break;

    case CM_SUPS_EVENT_RELEASE_USS_IND:
      /* Release transaction message (from NW) */
      /* Event received when NW terminates transaction (normal or abort) */
      /* See 3GPP 24.090 section 5 */

      /* If pending opeation, report operation aborted */
      if (TRUE == sups_ms_val->dsat_ss_cm_data.pending_msg)
      {
        result = etsicmif_report_ussd_result_ex(sups_event,
                                                DSAT_CUSD_RESULT_ABORT,subs_id);
      }
      /* Cleanup USSD transaction */
      dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
      break;

    case CM_SUPS_EVENT_USS_NOTIFY_IND:
      /* Notify indication message (from NW) */
      /* Event received when NW initiates data notification */
      /* See 3GPP 24.090 section 5.2 */

      /* Report USSD result code */
      dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_SS_NOTIFY,
                                                          einfo_ptr->invoke_id);
      
      result = etsicmif_report_ussd_result_ex(sups_event,
                                              DSAT_CUSD_RESULT_DONE,subs_id);

      break;
      
    case CM_SUPS_EVENT_USS_IND:
      /* Request indication message (from NW) */
      /* Event received when NW initiates data request */
      /* See 3GPP 24.090 section 5.1 */
      dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_SS_REQUEST,
                                                    einfo_ptr->invoke_id);
      /* Report USSD result code */
      result = etsicmif_report_ussd_result_ex(sups_event,
                                              DSAT_CUSD_RESULT_MORE,subs_id);

      /* See if decoding error in processing message */
      if (DSAT_ERROR == result)
      {
        /* Cleanup USSD transaction */
        dsatetsicmif_set_cusd_state(&(sups_ms_val->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
      }

      break;

    default:
      break;
  }

#endif /* FEATURE_DSAT_CUST */

  if( DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatetsicmif_cm_sups_event_handler_fp))
  {
    result = dsatdl_vtable.dsatetsicmif_cm_sups_event_handler_fp( sups_event, result, subs_id);
  }

  return result;

} /* dsatetsicmif_cm_sups_event_handler_ex */

/*===========================================================================
FUNCTION  DSATETSICMIF_SUPS_EVENT_CB_FUNC

DESCRIPTION
  CM supplementary service event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatetsicmif_sups_event_cb_func 
(
  cm_sups_event_e_type             event,          /* Event ID              */
  const cm_sups_info_s_type       *event_ptr      /* Pointer to Event info */
)
{
  ds_cmd_type       * cmd_buf;

  ASSERT (event_ptr != NULL);

  if ((FALSE == dsatme_is_thin_ui()) &&
      ( event_ptr->sups_client_id != dsatcm_client_id))
  {
    /* In FULL builds, when the client ID is not ATCOP, then only these 
       two EVENTS are handled in the event handler and rest are ignored
    */
    if ((CM_SUPS_EVENT_USS_NOTIFY_RES != event) &&
        (CM_SUPS_EVENT_USS_RES != event))
    {
      DS_AT_MSG2_ERROR("Ignoring received SUPS EVENT %d from another client %d",event,event_ptr->sups_client_id);
      return;
    }
  }

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cm_sups_event_type), TRUE);

  if (NULL != cmd_buf && NULL != cmd_buf->cmd_payload_ptr)
  {
    ds_at_sups_event_info_s_type * einfo_ptr = NULL;
    ds_at_cm_sups_event_type*  sups_event = NULL;

    sups_event = cmd_buf->cmd_payload_ptr;
    einfo_ptr = &sups_event->event_info;

    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_SUPS_INFO_CMD;
    /* Copy fields of interest */
    memset((void*)einfo_ptr, 0x0, sizeof(ds_at_sups_event_info_s_type));
    einfo_ptr->sups_client_id = event_ptr->sups_client_id;
    einfo_ptr->subs_id    = event_ptr->asubs_id;
    einfo_ptr->ss_success = event_ptr->ss_success;      
    einfo_ptr->ss_status = event_ptr->ss_status;
    einfo_ptr->ss_code = event_ptr->ss_code;         
    einfo_ptr->ss_operation = event_ptr->ss_operation;    
    einfo_ptr->ss_error = event_ptr->ss_error;
    einfo_ptr->invoke_id = event_ptr->invoke_id;       
    einfo_ptr->conf_type = event_ptr->conf_type;       
    /*Event CM_SUPS_EVENT_GET_PASSWORD_IND*/
    einfo_ptr->guidance_info = event_ptr->guidance_info;
    /*Event CM_SUPS_EVENT_USS_IND NOTIFY RELEASE PROCESS */
    einfo_ptr->uss_data_type = event_ptr->uss_data_type;
    (void) dsatutil_memscpy((void*)&einfo_ptr->uss_data,
            sizeof(uss_data_s_type),(void*)&event_ptr->uss_data,sizeof(uss_data_s_type));
    /*Event CM_SUPS_EVENT_INTERROGATE_CONF*/
    (void) dsatutil_memscpy((void*)&einfo_ptr->cli_restriction,
            sizeof(cli_restriction_info_s_type),(void*)&event_ptr->cli_restriction,sizeof(cli_restriction_info_s_type));
    (void) dsatutil_memscpy((void*)&einfo_ptr->basic_service,
            sizeof(basic_service_s_type),(void*)&event_ptr->basic_service,sizeof(basic_service_s_type));
    (void) dsatutil_memscpy((void*)&einfo_ptr->bsg_list,
            sizeof(basic_service_group_list_s_type),(void*)&event_ptr->bsg_list,sizeof(basic_service_group_list_s_type));
    (void) dsatutil_memscpy((void*)&einfo_ptr->call_barring_info,
            sizeof(ie_call_barring_info_s_type),(void*)&event_ptr->call_barring_info,sizeof(ie_call_barring_info_s_type));

    /*Store Number details to update CCFC INTERROGATE file */
    einfo_ptr->fwd_feature_list.present = event_ptr->fwd_feature_list.present;
    
    einfo_ptr->fwd_feature_list.forwarding_feature_list_length = 
       event_ptr->fwd_feature_list.forwarding_feature_list_length;
    (void) dsatutil_memscpy((void*)&einfo_ptr->fwd_feature_list.forwarding_feature[0],
            sizeof(forwarding_feature_T)*FWD_FEATURE_LIST_SIZE,
            (void*)&event_ptr->fwd_feature_list.forwarding_feature[0],
             sizeof(forwarding_feature_T)*FWD_FEATURE_LIST_SIZE );
    /* Put event on DS task queue */
    sups_event->event = event;
    ds_put_cmd(cmd_buf);
  }

  return;
}  /* dsatetsicmif_sups_event_cb_func */

/*===========================================================================
FUNCTION  ETSICMIF_SUPS_CMD_CB_FUNC

DESCRIPTION
  CM supplementary services command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void etsicmif_sups_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_sups_cmd_e_type            cmd,              /* Command ID            */
  cm_sups_cmd_err_e_type        cmd_err           /* Command error code    */
)
{
  ds_cmd_type * cmd_buf;
  ds_at_cm_sups_cmd_type *sups_cmd = NULL;
    
  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cm_sups_cmd_type), FALSE);

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id              = DS_CMD_ATCOP_CM_SUPS_CMD;
  sups_cmd                         = cmd_buf->cmd_payload_ptr;
  sups_cmd->cmd                    = cmd;
  sups_cmd->cmd_err                = cmd_err;
  sups_cmd->data_ptr               = data_ptr;
  ds_put_cmd(cmd_buf);

  return;
} /* etsicmif_sups_cmd_cb_func */


/*===========================================================================
FUNCTION  ETSICMIF_REPORT_SUPS_ERROR

DESCRIPTION
  Function to report SS error passed from Call Manager.  Error codes should
  loosly map to responses generated by UI.
  
DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :       if there was any problem in execution
    DSAT_ASYNC_EVENT : if +CME error respose was generated

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsicmif_report_sups_error 
(
  const ds_at_cm_sups_event_type * event_ptr,        /* Event info   */
  sys_modem_as_id_e_type            subs_id
)
{
/* Macro to handle sups error */
#define SEND_SUPS_ERROR( error_code ) \
        result = etsicmifi_send_sups_error(error_code, subs_id);

  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  const ds_at_sups_event_info_s_type * einfo_ptr = &event_ptr->event_info;

  if (einfo_ptr->ss_error.present)
  {
    DS_AT_MSG2_HIGH("CM SupS error tag: %d code: %d",
             einfo_ptr->ss_error.error_code_tag,
             einfo_ptr->ss_error.error_code);
  }
  else
  {
    DS_AT_MSG0_HIGH("Error information not in msg");
    SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
    return result;
  }

  /* Based on the code tag, display a message */
  switch( einfo_ptr->ss_error.error_code_tag )
  {
    case ERROR_CODE_TAG:
      switch( einfo_ptr->ss_error.error_code )
      {
        case unknownSubscriber:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_UNKNOWN_NUMBER);
          break;

        case deflectionToServedSubscriber:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_DFL_SELF_NUMBER);
          break;
                       
        case invalidDeflectedToNumber:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_DFL_INVALID_NUMBER);
          break;

        case bearerServiceNotProvisioned:
        case teleServiceNotProvisioned:
        case callBarred:
        case illegalSS_Operation:
        case ss_ErrorStatus:
        case ss_SubscriptionViolation:
        case ss_Incompatibility:
        case facilityNotSupported:
        case numberOfPW_AttemptsViolation:
        case maxNumberOfMPTY_ParticipantsExceeded:
        case specialServiceCode:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_NETWORK_REJECTED);
          break;
                
        case ss_NotAvailable:
        case resourcesNotAvailable:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_SERVICE_UNAVAIL);
          break;

        case systemFailure:
        case dataMissing:
        case unexpectedDataValue:
        case unknownAlphabet: 
        case ussd_Busy:
        case passwordRegistrationFailure:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;

        case negativePasswordCheck:
          SEND_SUPS_ERROR (DSAT_CME_INCORRECT_PASSWORD);
          break;

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      } /* switch error code  */
      break;

    case GENERAL_PROBLEM:
      switch( einfo_ptr->ss_error.error_code )
      {
        case UNRECOGNISED_COMPONENT:
        case BADLY_STRUCTURED_COMPONENT:
        case MISTYPED_COMPONENT:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      } /* switch general problem code */
      break;

    case INVOKE_PROBLEM:
      switch( einfo_ptr->ss_error.error_code )
      {
        case DUPLICATE_INVOKE_ID:
        case UNRECOGNISED_OPERATION:
        case INITIATING_RELEASE:
        case UNRECOGNISED_LINKED_ID:
        case LINKED_RESPONSE_UNEXPECTED:
        case UNEXPECTED_LINKED_OPERATION:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_NETWORK_REJECTED);
          break;

        case MISTYPED_PARAMETER:
        case RESOURCE_LIMITATION:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      } /* switch invoke problem code */
      break;

    case RETURN_RESULT_PROBLEM:
      switch( einfo_ptr->ss_error.error_code )
      {
        case UNRECOGNISED_INVOKE_ID:
        case RETURN_RESULT_UNEXPECTED:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_NETWORK_REJECTED);
          break;

        case MISTYPED_PARAMETER:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      } /* switch return result problem code  */
      break;

    case RETURN_ERROR_PROBLEM:

      switch( einfo_ptr->ss_error.error_code )
      {
        case UNRECOGNISED_INVOKE_ID:
        case RETURN_ERROR_UNEXPECTED:
        case UNRECOGNISED_ERROR:
        case UNEXPECTED_ERROR:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_NETWORK_REJECTED);
          break;

        case RE_MISTYPED_PARAMETER:
          SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      } /* switch return result problem error code  */
      break;

    case MN_SS_ERROR_TAG:
      switch( einfo_ptr->ss_error.error_code )
      {
        case MN_NETWORK_NOT_RESPONDING:
          /* Timeout error */
          switch(event_ptr->event)
          {
            case CM_SUPS_EVENT_PROCESS_USS:
            case CM_SUPS_EVENT_PROCESS_USS_CONF:
            case CM_SUPS_EVENT_USS_NOTIFY_IND:
            case CM_SUPS_EVENT_USS_NOTIFY_RES:
            case CM_SUPS_EVENT_USS_IND:
            case CM_SUPS_EVENT_USS_RES:
            case CM_SUPS_EVENT_RELEASE:
            case CM_SUPS_EVENT_RELEASE_USS_IND:
              /* Generate timeout unsoliced response for USSD events */
              result = etsicmif_report_ussd_result_ex (event_ptr, DSAT_CUSD_RESULT_TIMEOUT,subs_id);
              break;
  
            default:
              SEND_SUPS_ERROR (DSAT_CME_NETWORK_TIMEOUT);
              break;
          }
          break;

        case MN_INSUFFICIENT_RESOURCES:
            SEND_SUPS_ERROR (DSAT_CME_SUPS_RETRY_OPERATION);
          break;
                  
        case MN_CALL_HOLD_REJ:
        case MN_CALL_RETRIEVE_REJ:
        case MN_SPLIT_MPTY_REJ:
            SEND_SUPS_ERROR (DSAT_CME_SUPS_NETWORK_REJECTED);
          break;                  

        default:
          SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
          break;
      }
      break;


    default:
      DS_AT_MSG2_HIGH("Unsupported error tag: %d  code: %d",
               einfo_ptr->ss_error.error_code_tag,
               einfo_ptr->ss_error.error_code);

      SEND_SUPS_ERROR (DSAT_CME_UNKNOWN);
      break;
  } /* switch code tag . */
  return result;
}  /* etsicmif_report_sups_error */

/*===========================================================================
FUNCTION  ETSICMIFI_SEND_SUPS_ERROR

DESCRIPTION
  Function to send sups error.
  
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsicmifi_send_sups_error
(
  dsat_cme_error_e_type    error_code,
  sys_modem_as_id_e_type   subs_id
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;
  if( DSAT_DL_VALIDATE_SYMBOL_ADDR( dsatdl_vtable.etsicmif_send_sups_error_fp ) )
  {
    result = dsatdl_vtable.etsicmif_send_sups_error_fp(error_code,subs_id);
  }
  else if( dsatcmdp_block_indications_ex() == FALSE )
  {
    dsatetsicmif_sups_state_ms_info  *sups_ms_val = NULL;
    sups_ms_val = dsat_get_base_addr_per_subs(DSAT_SUPS_MS_VALS, subs_id, TRUE);
    if (NULL == sups_ms_val)
    {
      return DSAT_ERROR;
    }

    sups_ms_val->dsat_ss_cm_data.pending_rsp = FALSE;
  }
  return result;
}/* etsicmifi_send_sups_error */

/*===========================================================================
FUNCTION  ETSICMIF_SUPS_SEND_USSD_NOTIFY_RESPONSE

DESCRIPTION
  This function requests Call Manager to send the mobile originated
  USSD notify response to the network.  This response message is sent
  by the MS after an USSD notify indication message has been received.
  If the error present flag is FALSE, no error code is specified in
  the message payload.
  
DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  Send command to Call Manager

===========================================================================*/
dsat_result_enum_type etsicmif_sups_send_ussd_notify_response
(
  boolean                error_present,         /* Error present */
  byte                   error_code,             /* Error code */
  sys_modem_as_id_e_type subs_id
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  cm_uss_notify_res_params_s_type    uss_notify_parms;
  boolean cm_result = TRUE;
  dsatetsicmif_sups_state_ms_info  *sups_ms_val = NULL;

  sups_ms_val = dsat_get_base_addr_per_subs(DSAT_SUPS_MS_VALS, subs_id, TRUE);
  if (NULL == sups_ms_val)
  {
    return DSAT_ERROR;
  }


  /* Initialize CM structure */
  memset( &uss_notify_parms,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_uss_notify_res_params_s_type ));

  /* Setup USS Data structure */
  etsicmif_get_cusd_info(&(sups_ms_val->dsat_ss_cm_data), &uss_notify_parms.invoke_id, NULL);
  uss_notify_parms.ss_error.present = error_present;
  if( TRUE == error_present )
  {
    uss_notify_parms.ss_error.error_code_tag = ERROR_CODE_TAG;
    uss_notify_parms.ss_error.error_code = error_code;
  }

  /* Set indicator for pending messages */
  sups_ms_val->dsat_ss_cm_data.pending_msg = TRUE;
      
  DS_AT_MSG2_HIGH("Sending USSD notify response: err present=%d code=%d",
           error_present, error_code);
  
  /* send the command to the CM command queue */
#ifdef FEATURE_DUAL_SIM
cm_result = cm_sups_cmd_uss_notify_res_per_subs( etsicmif_sups_cmd_cb_func,
                                         &sups_ms_val->dsat_ss_cm_data,
                                         dsatcm_client_id,
                                         &uss_notify_parms,
                                         subs_id);

#else
  cm_result = cm_sups_cmd_uss_notify_res ( etsicmif_sups_cmd_cb_func,
                                           &sups_ms_val->dsat_ss_cm_data,
                                           dsatcm_client_id,
                                           &uss_notify_parms);
#endif /*FEATURE_DUAL_SIM*/
  if (!cm_result)
  {
    DS_AT_MSG0_ERROR("Problem invoking CM API");
    result = DSAT_ERROR;
  }
  return result;
} /* etsicmif_sups_send_ussd_notify_response */

/*===========================================================================
FUNCTION  ETSICMIF_REPORT_USSD_RESULT_EX

DESCRIPTION
  This function generates the unsolicited result code for USSD commands.
  The +CUSD command <n> paramater controls whether results are presented
  or suppressed.

  If TE indications are blocked, the network is sent a busy response. 
  If the data coding scheme is not supported, the network is sent an 
  unknown alphabet response.
  
DEPENDENCIES
  None

RETURNS
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  Send command to Call Manager

===========================================================================*/
LOCAL dsat_result_enum_type etsicmif_report_ussd_result_ex
(
  const ds_at_cm_sups_event_type *sups_event_ptr, /* SUPS event pointer */
  const cusd_result_e_type        result_code,    /* Result code */
  sys_modem_as_id_e_type          subs_id
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  boolean send_ack = (DSAT_CUSD_RESULT_MORE == result_code  || 
                      DSAT_CUSD_RESULT_OTHER == result_code ||
                      DSAT_CUSD_RESULT_ABORT == result_code )? FALSE : TRUE;

  if( DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.etsicmif_report_ussd_result_fp) )
  {
    result = dsatdl_vtable.etsicmif_report_ussd_result_fp( sups_event_ptr,
                                                           result_code,
                                                           subs_id, 
                                                           &send_ack );
  }

  /* Need to send ACK back to NW per 3GPP 24.090 section 5.2.1 */
  if( TRUE == send_ack )
  {
    (void)etsicmif_sups_send_ussd_notify_response (FALSE, 0, subs_id);
  }
  
  return result;
} /* etsicmif_report_ussd_result_ex */


/*===========================================================================

FUNCTION  DSATETSICMIF_PROCESS_CM_MM_INFO_DATA_EX

DESCRIPTION
  This function processes the Call Manager MM information data to display 
  the timezone status to TE

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_process_cm_mm_info_data_ex
(
  const dsat_sys_mm_information_s_type *mm_info_ptr,        /* MM Info pointer */
  ds_at_ss_info_s_type                 *event_info,
  sys_modem_as_id_e_type                subs_id
)
{
/* chosen to be outside the range defined in sys_time_zone_is_valid */
#define DSAT_TIMEZONE_NEG  -96
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;
  sys_time_zone_type            dsat_time_zone;

  /* ME doesn't know which time zone it powered up in */
   ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
   if (NULL == ph_ss_ms_val)
   {
     return DSAT_ERROR;
   }

  if ( ( NULL == mm_info_ptr ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG2_ERROR("Invalid Arg: mm_info_ptr = %p, subs_id = %d",
                     mm_info_ptr, subs_id);
    return result;
  }

  dsatetsicmif_process_cm_mm_info_net_reg(mm_info_ptr, subs_id);

  /* figure out if time zone changed from previously reported and save if so */
  if( mm_info_ptr->time_zone_avail == TRUE )
  {
    dsat_time_zone = mm_info_ptr->time_zone;
    if (( DSAT_TIMEZONE_NEG <= dsat_time_zone )&& (dsat_time_zone <=DSAT_TIMEZONE_HIGH))
    {
      if(ph_ss_ms_val->last_reported_tz != mm_info_ptr->time_zone)
      {
        ph_ss_ms_val->last_reported_tz = mm_info_ptr->time_zone;
        event_info->last_reported_tz_changed = TRUE;
      }
      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,NUM_TYPE) == 1 )
      {
        (void)dsatetsime_set_time_zone_into_nv(dsat_time_zone );
      }
    }
    DS_AT_MSG1_MED("Time Zone Update Info:Local time zone = %d", dsat_time_zone);
  }
  else if ( mm_info_ptr->univ_time_and_time_zone_avail == TRUE)
  {
    dsat_time_zone = mm_info_ptr->univ_time_and_time_zone.time_zone;
    if(( DSAT_TIMEZONE_NEG <= dsat_time_zone )&& (dsat_time_zone <=DSAT_TIMEZONE_HIGH))
    {
      if(ph_ss_ms_val->last_reported_tz != mm_info_ptr->univ_time_and_time_zone.time_zone )
      {
        ph_ss_ms_val->last_reported_tz = mm_info_ptr->univ_time_and_time_zone.time_zone;
        event_info->last_reported_tz_changed = TRUE;
      }
      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,NUM_TYPE) == 1 )
      {
        (void)dsatetsime_set_time_zone_into_nv(dsat_time_zone );
      }
    }
    DS_AT_MSG1_MED("Time Zone Update Info:Univ time zone = %d", dsat_time_zone);
  }
  /* NAS has updated time information*/

  return result;
} /* dsatetsicmif_process_cm_mm_info_data_ex */

/*===========================================================================

FUNCTION  DSATETSICMIF_PROCESS_CM_MM_INFO_NET_REG

DESCRIPTION
  This function processes the Call Manager MM information to set
  network registration information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsicmif_process_cm_mm_info_net_reg
(
  const dsat_sys_mm_information_s_type *mm_info_ptr,        /* MM Info pointer */
  sys_modem_as_id_e_type                subs_id
)
{
#define COPS_NULL_PLMN 0xFF
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  if ( ( NULL == mm_info_ptr ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG2_ERROR("Invalid Arg: mm_info_ptr = %p, net_id = %d",
                     mm_info_ptr, subs_id);
    return;
  }

  DS_AT_MSG4_MED("Processing available mobility info: PLMN=%d, 0x%X, 0x%X, 0x%X",
                 mm_info_ptr->plmn_avail,
                 mm_info_ptr->plmn.identity[0],
                 mm_info_ptr->plmn.identity[1],
                 mm_info_ptr->plmn.identity[2]);

   ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
   if (NULL == ph_ss_ms_val)
   {
     return ;
   }

  /* Update locally cached network identifier info */
  if(( TRUE == mm_info_ptr->plmn_avail )&&
     ( COPS_NULL_PLMN !=mm_info_ptr->plmn.identity[0] ))
  {
    ph_ss_ms_val->dsat_net_reg_state.net_id_info.present = TRUE;
    ph_ss_ms_val->dsat_net_reg_state.net_id_info.plmn = mm_info_ptr->plmn;
  }

  return;
} /* dsatetsicmif_process_cm_mm_info_net_reg */


/*==========================================================================
FUNCTION  DSATETSICMIF_PROCESS_RSSI_EVENT

DESCRIPTION
  This function processes the CM_SS_EVENT_RSSI from Call Manager.  It
  updates the +CIND indicator value.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_process_rssi_event
(
  const ds_at_cm_ss_event_type * event_ptr,     /* Event structure         */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id
)
{
  dsat_result_enum_type result;
  uint8                 siglvl;

  if ( ( NULL == event_ptr ) ||
       ( FALSE == IS_VALID_STACK_ID(stack_id) ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG3_ERROR("Invalid Arg: event_ptr = %p, stack_id = %d, subs_id = %d",
                     event_ptr, stack_id, subs_id);
    return DSAT_ASYNC_EVENT;
  }

  siglvl = dsatetsime_convert_rssi(
             event_ptr->event_info[stack_id].rssi,
             DSAT_CIND_MAX_SIGNAL );
  
  /* Update the signal indicator */
  if (RSSI_TOOHI_CODE == siglvl)
  {
    result = dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SIGNAL,
                                                DSAT_CIND_MAX_SIGNAL , subs_id );
  }
  else if (RSSI_UNKNOWN_CODE == siglvl)
  {
    result = dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SIGNAL, 0 , subs_id);
  }
  else
  {
    result = dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SIGNAL, siglvl,subs_id );
    
  }

  return result;
} /* etsicmif_process_rssi_event */




/*===========================================================================

FUNCTION   DSATETSICMIF_LOOKUP_CM_RAT_PREF

DESCRIPTION
  This function performs lookup to/from the Call Manager radio access
  technology preference based on the input ATCOP AcT parameter value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on sucessful lookup; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsicmif_lookup_cm_rat_pref
(
  boolean                to_cm_pref,
  dsat_cops_act_e_type  *net_act_ptr,
  cm_mode_pref_e_type   *cm_rat_ptr
)
{
  uint8 i = 0;

  ASSERT( NULL != net_act_ptr );
  ASSERT( NULL != cm_rat_ptr );
  
  while ( DSAT_COPS_ACT_MAX != dsatetsicmif_act_pref_xref[i].cops_act )
  {
    if ( to_cm_pref )
    {
      if ( *net_act_ptr == dsatetsicmif_act_pref_xref[i].cops_act )
      {
        *cm_rat_ptr = dsatetsicmif_act_pref_xref[i].cm_pref;
        return TRUE;
      }
    }
    else
    {
      if ( *cm_rat_ptr == dsatetsicmif_act_pref_xref[i].cm_pref )
      {
        *net_act_ptr = dsatetsicmif_act_pref_xref[i].cops_act;
        return TRUE;
      }
    }
    i++;
    if(i >= ARR_SIZE(dsatetsicmif_act_pref_xref))
    {
      DS_AT_MSG0_ERROR("Invalid Access into dsatetsicmif_act_pref_xref");
      return FALSE;
    }
  }

  return FALSE;
} /* dsatetsicmif_lookup_cm_rat_pref */


/*-------------------------------------------------------------------------
        Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION   DSATETSICMIF_CHANGE_NETWORK_REGISTRATION

DESCRIPTION
  This function invokes the CM API to change the network registration
  preference.  The passed in mode, PLMN, and access technology
  parameters are used to configure the call to the CM API.  For manual
  or automatic network registration, a single API call is required.
  For automatic if manual fails, a second call to the CM API is
  required once the NO_SRV indication is reported.  The aborting flag
  indicates whether the state machine should be updated.

  If the requested mode is AUTO and matches the current preference, no
  action is taken as the lower layers may do unnecessary detach &
  attach cycle.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if no action was taken.
    DSAT_ASYNC_CMD : if command was sent to CM API successfully.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsicmif_change_network_registration
(
  dsat_cops_mode_e_type       net_mode,
  sys_plmn_id_s_type *        plmn_ptr,
  dsat_cops_act_e_type        net_act,
  sys_modem_as_id_e_type      subs_id
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  cm_mode_pref_e_type act_mode_pref = CM_MODE_PREF_NO_CHANGE;
  dsat_cops_act_e_type current_act;
  dsati_mode_e_type  current_mode = dsatcmdp_get_current_mode_per_subs(subs_id);
#ifdef FEATURE_DSAT_HIGH_TIER
  cm_cmd_user_pref_update_type user_pref;
  cm_rat_acq_order_pref_s_type  rat_acq_order;
#endif /* FEATURE_DSAT_HIGH_TIER */
  boolean plmn_match = FALSE;
  cm_subscription_status_e_type status_one = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type status_two = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type status_three = CM_SUBSCRIPTION_STATUS_NO_CHANGE;

  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;

  DS_AT_MSG2_MED("ATCOP changing network reg preference: mode=%d act=%d",
            net_mode, net_act);
   ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, FALSE);

  /* Cross-reference CM RAT preference to AcT parameter */
  if (TRUE !=
      dsatetsicmif_lookup_cm_rat_pref( FALSE, &current_act,
                                       &ph_ss_ms_val->dsat_net_reg_state.cmph_pref.
                                         network_rat_mode_pref ))
  {
    current_act = DSAT_COPS_ACT_MAX; /* never match */
  }

  /* See if the Preferred (mode+PLMN+AcT) is same as that requested. If
   * so, abort further processing as there will be no network
   * change.*/
  if( NULL != plmn_ptr &&
      ( TRUE ==
        ( plmn_match = sys_plmn_match (*plmn_ptr,
                        ph_ss_ms_val->dsat_net_reg_state.net_id_info.plmn)) ) &&
      (ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref == 
                   (cm_network_sel_mode_pref_e_type) net_mode) &&
      (net_act == current_act) )
  {
    DS_AT_MSG2_HIGH("Requested (mode+PLMN+AcT) matches current;"
              " no action: %d %d", net_mode, net_act);
    ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
    ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
    return DSAT_OK;
  }
  
  /* Cross-reference AcT parameter to CM RAT preference */
  if (TRUE !=
      dsatetsicmif_lookup_cm_rat_pref( TRUE, &net_act, &act_mode_pref ))
  {
    DS_AT_MSG1_ERROR("Failed on CM RAT pref lookup: %d", net_act);
    ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
    ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
    return DSAT_ERROR;
  }
  
  /* Preserve state for later asynch processing */
  ph_ss_ms_val->dsatetsicall_network_list.requested_pref.mode = net_mode;
  if ( NULL != plmn_ptr )
  {
    ph_ss_ms_val->dsatetsicall_network_list.requested_pref.plmn = *plmn_ptr;
  }
  else
  {
    memset( ph_ss_ms_val->dsatetsicall_network_list.requested_pref.plmn.identity, 0, 
            sizeof(ph_ss_ms_val->dsatetsicall_network_list.requested_pref.plmn.identity) );
  }
  ph_ss_ms_val->dsatetsicall_network_list.requested_pref.act  = net_act;
#ifdef FEATURE_DSAT_HIGH_TIER
  /* fill rat_acq_order if command is $QCCOPS */
      if( ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCOPS ||
          ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCSGCOPS)
  {
    memset(&user_pref,0x0,sizeof(user_pref));
    if ( !cm_user_pref_init( &user_pref ) )
    {
      MSG_ERROR("Failed to initalize user pref structure",0,0,0);
      return DSAT_ERROR;
    }
    user_pref.client_id = dsatcm_client_id;
    user_pref.asubs_id = subs_id;
    user_pref.pref_term = CM_PREF_TERM_PERMANENT;
    rat_acq_order.type = CM_ACQ_ORDER_TYPE_RAT_PRI;
    rat_acq_order.acq_order.rat_acq_pri_order.num_rat =  6;
    switch(net_act)
    {
      case DSAT_COPS_ACT_GSM:
        user_pref.manual_rat = SYS_RAT_GSM_RADIO_ACCESS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[0] = SYS_SYS_MODE_GSM;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[1] = SYS_SYS_MODE_WCDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[2] = SYS_SYS_MODE_TDS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[3] = SYS_SYS_MODE_LTE;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[4] = SYS_SYS_MODE_CDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[5] = SYS_SYS_MODE_HDR;
        break;
      case DSAT_COPS_ACT_UTRAN:
        user_pref.manual_rat = SYS_RAT_UMTS_RADIO_ACCESS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[0] = SYS_SYS_MODE_TDS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[1] = SYS_SYS_MODE_WCDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[2] = SYS_SYS_MODE_GSM;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[3] = SYS_SYS_MODE_LTE;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[4] = SYS_SYS_MODE_CDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[5] = SYS_SYS_MODE_HDR;
        break;
     case DSAT_COPS_ACT_EUTRAN:
        user_pref.manual_rat = SYS_RAT_LTE_RADIO_ACCESS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[0] = SYS_SYS_MODE_LTE;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[1] = SYS_SYS_MODE_GSM;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[2] = SYS_SYS_MODE_WCDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[3] = SYS_SYS_MODE_TDS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[4] = SYS_SYS_MODE_CDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[5] = SYS_SYS_MODE_HDR;
        break;
     case DSAT_COPS_ACT_AUTO:
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[0] = SYS_SYS_MODE_GSM;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[1] = SYS_SYS_MODE_WCDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[2] = SYS_SYS_MODE_TDS;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[3] = SYS_SYS_MODE_LTE;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[4] = SYS_SYS_MODE_CDMA;
        rat_acq_order.acq_order.rat_acq_pri_order.acq_order[5] = SYS_SYS_MODE_HDR;
        break;
     default:
        DS_AT_MSG1_HIGH("Act Not valid %d",net_act);
        break;
    }
    user_pref.rat_acq_order_pref_ptr = &rat_acq_order;
    user_pref.plmn_ptr = plmn_ptr;
    user_pref.req_id = DSAT_CM_REQ_ID;
  }
#endif /* FEATURE_DSAT_HIGH_TIER */
  /* Send CM API the desired command */
  switch (net_mode)
  {
  case DSAT_COPS_MODE_AUTO:
    if(cops_no_mode_change == TRUE)
    {
      act_mode_pref = CM_MODE_PREF_NO_CHANGE;
    }
    DS_AT_MSG2_HIGH("COPS setting Auto network registration %d act_mode_pref %d cops_no_mode_change",
          act_mode_pref,cops_no_mode_change);

#ifdef FEATURE_DSAT_HIGH_TIER
     if( ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCOPS )
     {
       user_pref.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
       DS_AT_MSG0_HIGH("calling cm_user_pref_update_req");
       if(!cm_user_pref_update_req(&user_pref,dsatcmif_ph_cmd_cb_func,NULL))
       {
         DS_AT_MSG0_ERROR("cm_user_pref_update_req failed");
         return DSAT_ERROR;
       }
     }
     else if( ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCSGCOPS )
     {
        (void) cm_ph_cmd_sys_sel_pref_4 ( dsatcmif_ph_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id,
                                          subs_id,
                                          act_mode_pref,
                                          CM_PREF_TERM_PERMANENT,
                                          0,
                                          CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                          CM_BAND_PREF_NO_CHANGE,
                                          CM_BAND_PREF_NO_CHANGE,
                                          CM_BAND_PREF_NO_CHANGE,
                                          CM_PRL_PREF_NO_CHANGE,
                                          CM_ROAM_PREF_NO_CHANGE,
                                          CM_HYBR_PREF_NO_CHANGE,
                                          CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                          CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                          plmn_ptr,
                                          ph_ss_ms_val->net_pref.csg_id,
                                          ph_ss_ms_val->net_pref.csg_rat,
                                          DSAT_CM_REQ_ID);  
     }
     else
     {
#endif /* FEATURE_DSAT_HIGH_TIER */
#ifdef FEATURE_DUAL_SIM
    (void) cm_ph_cmd_sys_sel_pref_per_subs_1( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    subs_id,
                                    act_mode_pref,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    CM_ROAM_PREF_NO_CHANGE,
                                    CM_HYBR_PREF_NO_CHANGE,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                    NULL,
                                    DSAT_CM_REQ_ID);  
#else
    (void) cm_ph_cmd_sys_sel_pref_3 ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    act_mode_pref,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    CM_ROAM_PREF_NO_CHANGE,
                                    CM_HYBR_PREF_NO_CHANGE,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                    NULL,
                                    DSAT_CM_REQ_ID);  
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_DSAT_HIGH_TIER
      }
#endif /* FEATURE_DSAT_HIGH_TIER */
    break;

  case  DSAT_COPS_MODE_MANUAL:
  case  DSAT_COPS_MODE_MANAUTO:
    /* For manual registration with cops command skipping <AcT> parameter, 
       mode change depends on variable 'cops_no_mode_change' assigned to the value 
       @ EFS /ds/atcop/atcop_cops_auto_mode.txt in the UE. Mode change happens when 
       the value @ EFS is 0.
    */    
    
    if( cops_no_mode_change == TRUE )
    {
      if( (ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCSGCOPS && 
           ph_ss_ms_val->net_pref.no_of_args == 5) || (ph_ss_ms_val->net_pref.no_of_args == 3) )
      {
        act_mode_pref = CM_MODE_PREF_NO_CHANGE;
        ph_ss_ms_val->net_pref.no_of_args = 0;
      }
    }

    DS_AT_MSG2_HIGH("COPS setting manual network registration %d act_mode_pref %d cops_no_mode_change",act_mode_pref,cops_no_mode_change);
   
    if( ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCSGCOPS )
    {
      (void) cm_ph_cmd_sys_sel_pref_4 ( dsatcmif_ph_cmd_cb_func,
                                      NULL,
                                      dsatcm_client_id,
                                      subs_id,
                                      act_mode_pref,
                                      CM_PREF_TERM_PERMANENT,
                                      0,
                                      CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                      CM_BAND_PREF_NO_CHANGE,
                                      CM_BAND_PREF_NO_CHANGE,
                                      CM_BAND_PREF_NO_CHANGE,
                                      CM_PRL_PREF_NO_CHANGE,
                                      CM_ROAM_PREF_NO_CHANGE,
                                      CM_HYBR_PREF_NO_CHANGE,
                                      CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                      CM_NETWORK_SEL_MODE_PREF_MANUAL,
                                      plmn_ptr,
                                      ph_ss_ms_val->net_pref.csg_id,
                                      ph_ss_ms_val->net_pref.csg_rat,
                                      DSAT_CM_REQ_ID);  
    }
#ifdef FEATURE_DSAT_HIGH_TIER
    else if( ph_ss_ms_val->dsatetsicall_network_list.cmd_idx == CMD_IDX_QCCOPS )
    {
      user_pref.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_MANUAL;
      DS_AT_MSG0_HIGH("calling cm_user_pref_update_req");
      if(!cm_user_pref_update_req(&user_pref,dsatcmif_ph_cmd_cb_func,NULL))
      {
        DS_AT_MSG0_ERROR("cm_user_pref_update_req failed");
        return DSAT_ERROR;
      }
    }
#endif /* FEATURE_DSAT_HIGH_TIER */
    else
    {
#ifdef FEATURE_DUAL_SIM
    (void) cm_ph_cmd_sys_sel_pref_per_subs_1 ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    subs_id,
                                    act_mode_pref,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    CM_ROAM_PREF_NO_CHANGE,
                                    CM_HYBR_PREF_NO_CHANGE,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    CM_NETWORK_SEL_MODE_PREF_MANUAL,
                                    plmn_ptr,
                                    DSAT_CM_REQ_ID);  
#else
    (void) cm_ph_cmd_sys_sel_pref_3 ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    act_mode_pref,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    CM_ROAM_PREF_NO_CHANGE,
                                    CM_HYBR_PREF_NO_CHANGE,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    CM_NETWORK_SEL_MODE_PREF_MANUAL,
                                    plmn_ptr,
                                    DSAT_CM_REQ_ID);  
#endif /* FEATURE_DUAL_SIM */
    }
    break;

  case  DSAT_COPS_MODE_DEREG:
    DS_AT_MSG0_HIGH("COPS setting deregister");
     dsatetsicall_update_subs_status( &status_one, &status_two, &status_three, subs_id);

     (void) cm_ph_cmd_subscription_not_available3 ( dsatcmif_ph_cmd_cb_func,
                                              NULL,
                                              dsatcm_client_id,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                              status_one,status_two,status_three);
    break;
      
  default:
    DS_AT_MSG1_ERROR("Error: Unsupported network mode: %d", net_mode);
    result = DSAT_ERROR;
  }

  if ( plmn_match == TRUE )
  {
    if ( (( net_act == DSAT_COPS_ACT_GSM ) &&  ( current_mode == DSAT_MODE_GSM )) ||
         ((net_act == DSAT_COPS_ACT_UTRAN ) && (current_mode == DSAT_MODE_WCDMA )) 
#ifdef FEATURE_TDSCDMA
         ||((net_act == DSAT_COPS_ACT_UTRAN ) && (current_mode == DSAT_MODE_TDS )) 
#endif /* FEATURE_TDSCDMA */
#ifdef FEATURE_DSAT_LTE
         ||((net_act == DSAT_COPS_ACT_EUTRAN ) && ((current_mode == DSAT_MODE_LTE ) 
         ||( current_mode == DSAT_MODE_1XLTE )
         ))
#endif /* FEATURE_DSAT_LTE */ 
       )
    {
      DS_AT_MSG0_HIGH("Requested (PLMN+AcT) matches current;"
                " not waiting for ss events: ");
      ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
      ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
      result = DSAT_OK;
    }
  }
  
  return result;
} /* dsatetsicmif_change_network_registration */



/*==========================================================================
FUNCTION  DSATETSICMIF_CAPTURE_CALL_END_STATUS

DESCRIPTION
  This function captures the call end status information from CM
  events.  The events containing the informaiton are CM_CALL_EVENT_END
  and CM_CALL_EVENT_CALL_CONF.  Both CS and PS domain calls are
  monitored.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatetsicmif_capture_call_end_status
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_event_e_type    event,            /* Event ID              */
  ds_at_call_info_s_type *event_ptr         /* Pointer to Event info */
)
{
  dsatetsicall_call_state_da_info  *call_da_val = NULL;
  
  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, TRUE);
  if (NULL == call_da_val)
  {
    return ;
  }

  /* Filter by call type */
  if( (CM_CALL_TYPE_CS_DATA == event_ptr->call_type) ||
      (CM_CALL_TYPE_VOICE == event_ptr->call_type) )
  {
    /* Cache the status codes */
    call_da_val->dsatetsicmif_call_end_status.domain = DSAT_CES_DOMAIN_CS;
    call_da_val->dsatetsicmif_call_end_status.error_info.cs_domain.end_status =
      event_ptr->end_status;
    call_da_val->dsatetsicmif_call_end_status.error_info.cs_domain.cc_cause =
      event_ptr->dsat_mode_info.info.gw_cs_call.cc_cause;
    call_da_val->dsatetsicmif_call_end_status.error_info.cs_domain.cc_reject =
      event_ptr->dsat_mode_info.info.gw_cs_call.cc_reject;
  }
  else if( CM_CALL_TYPE_PS_DATA == event_ptr->call_type )
  {
    /* Cache the status codes */
    call_da_val->dsatetsicmif_call_end_status.domain = DSAT_CES_DOMAIN_PS;
    call_da_val->dsatetsicmif_call_end_status.error_info.ps_domain.sys_mode = 
      event_ptr->sys_mode;
    if(event_ptr->sys_mode == SYS_SYS_MODE_LTE)
    {
      call_da_val->dsatetsicmif_call_end_status.error_info.ps_domain.esm_cause = 
         event_ptr->dsat_mode_info.info.lte_call.esm_cause;
      call_da_val->dsatetsicmif_call_end_status.error_info.ps_domain.esm_local_cause= 
         event_ptr->dsat_mode_info.info.lte_call.esm_local_cause;
    }
    else
    {
     call_da_val->dsatetsicmif_call_end_status.error_info.ps_domain.pdp_cause_type =
      event_ptr->dsat_mode_info.info.gw_ps_call.pdp_cause_type;
     call_da_val->dsatetsicmif_call_end_status.error_info.ps_domain.cause =
      event_ptr->dsat_mode_info.info.gw_ps_call.cause;
    }
  }
} /* dsatetsicmif_capture_call_end_status */


#endif /* FEATURE_DSAT_ETSI_MODE */

