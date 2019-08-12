/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

SUPPLEMENTARY S E R V I CE    ROUTING   M O D U L E

GENERAL DESCRIPTION
This module contains the Call Manager supplementary service domain selection and routing functionality.

The supplementary service domain routing is responsible for:
1. Routing IP PS supp service command



INITIALIZATION AND SEQUENCING REQUIREMENTS
cmsups_init() must be called to initialize this module before any other
function declared in this module is being called.


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/14   sm      Sups fallback from IMS to CS
12/12/13   sm      Add error string for IP SUPS CONF 
11/22/12   xs      Added PS supplemetary service support
===========================================================================*/

#include "cmipsups.h"
#include "cmregprxi.h"
#include "cmipappi.h"
#include "cmidbg.h"
#include "cmtaski.h"
#include "cmll_v.h"
#include "cmsds.h"
#include "cmph.h"
#include <stringl/stringl.h>
#include "cmwsups.h"
#include "cmefs.h"
#ifdef FEATURE_IP_CALL
/*===========================================================================
FUNCTION cmipsups_is_ussd_sups_cmd

DESCRIPTION
to check if the command is to be send on USSD interface. 

RETURN VALUE
boolean
===========================================================================*/

boolean cmipsups_is_ussd_sups_cmd( cm_sups_cmd_e_type cmd )
{
  if(cmd == CM_SUPS_CMD_PROCESS_USS ||
     cmd == CM_SUPS_CMD_USS_RES ||
     cmd == CM_SUPS_CMD_USS_NOTIFY_RES ||
     cmd == CM_SUPS_CMD_RELEASE)
  {
    return TRUE;  
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================
FUNCTION cmipsups_is_ussd_sups_rpt

DESCRIPTION
to check if the command is to be send on USSD interface. 

RETURN VALUE
boolean
===========================================================================*/

static boolean cmipsups_is_ussd_sups_rpt( cm_name_type rpt )
{
  if(rpt == CM_IP_SUPS_UNSTRUCTURED_SS_IND ||
     rpt == CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND ||
     rpt == CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF)
  {
    return TRUE;  
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION cmipsups_is_cs_only_sups_cmd

DESCRIPTION
to check if the command is to be sent on CS domain always 

RETURN VALUE
boolean
===========================================================================*/

boolean cmipsups_is_cs_only_sups_cmd( cm_sups_cmd_e_type cmd )
{
  if(cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES ||
     cmd == CM_SUPS_CMD_LCS_MOLR ||
     cmd == CM_SUPS_CMD_LCS_MOLR_COMPLETED)
  {
    return TRUE;  
  }

  return FALSE;
} /* cmipsups_is_cs_only_sups_cmd () */


/*===========================================================================

FUNCTION cmipsups_route_check

DESCRIPTION
Check if routing supps command to IMS possible

DEPENDENCIES
none

RETURN VALUE
cmsups_cmd_route_e_type

SIDE EFFECTS
none

===========================================================================*/
cmsups_cmd_route_e_type cmipsups_route_check(
  cm_sups_cmd_e_type sups_cmd,
  sys_modem_as_id_e_type sub_id
)
{
  
  CM_MSG_MED_1("cmipsups_route: cmd=%d",sups_cmd);

  if(cmipsups_is_ussd_sups_cmd(sups_cmd))
  {
    cmsups_cmd_route_e_type sups_route;
    /* If MT USSD is on going, send the MO USS request to same route as MT USS*/
    if ( cmsups_get_mt_uss_routing_context_info( sub_id, &sups_route) == TRUE ) {
      return sups_route;
    }
    /* Check USSD IP call back registration status, if not, force to CS */
    else if(cmipapp_find_ussd_support_app() == CMIPAPP_ID_INVALID)
    {
      /* This API should cover all VoLTE (both Normal/Emergency) Calls but not VoWLAN Emergency Call.
          During VoWLAN (both Normal/Emergency) Call, USSD will always be attempted either on IMS or CS i.e. not rejected here */
      if(cmcall_is_present_ip_call() != CM_CALL_ID_INVALID)
      {
        if(cmcall_is_there_vowlan_call() != CM_CALL_ID_INVALID)
        {
          return CMSUPS_CMD_ALLOW_CS;
        }
        else
        {
          return CMSUPS_CMD_REJECT;
        }
      }
      else
      {
        return CMSUPS_CMD_ALLOW_CS;
      }
    }
    else
    {
      return CMSUPS_CMD_ALLOW_IMS;
    }
  }
  else if (cmipsups_is_cs_only_sups_cmd(sups_cmd))
  {
    return CMSUPS_CMD_ALLOW_CS;
  }
  else
  {
    boolean is_ics_support = (cmipapp_find_ics_support_app() != CMIPAPP_ID_INVALID);
    boolean is_sups_cmd_supported_by_ims = TRUE;

    if(sups_cmd == CM_SUPS_CMD_REG_PASSWORD ||
      sups_cmd == CM_SUPS_CMD_GET_PASSWORD_RES)
    {
      is_sups_cmd_supported_by_ims = FALSE;
    }

    switch(cmsups_get_srv_domain_pref(sub_id))
    {
    case SYS_SUPP_SRV_DOMAIN_PREF_PS_ONLY:
      if(is_ics_support && is_sups_cmd_supported_by_ims)
      {
        return CMSUPS_CMD_ALLOW_IMS;
      }
      else
      {
        return CMSUPS_CMD_REJECT;
      }
    case SYS_SUPP_SRV_DOMAIN_PREF_CS_ONLY:
      return CMSUPS_CMD_ALLOW_CS;

    case SYS_SUPP_SRV_DOMAIN_PREF_PS_PREF:
      if(is_ics_support && is_sups_cmd_supported_by_ims)
      {
        return CMSUPS_CMD_ALLOW_IMS;
      }
      else
      {
        return CMSUPS_CMD_ALLOW_CS;
      }
    case SYS_SUPP_SRV_DOMAIN_PREF_AUTO:

      if( 
        #ifdef FEATURE_LTE
        cmsds_ptr()->curr_voice_dom_selected == SYS_CM_DOMAIN_SEL_DOMAIN_IMS &&
        #endif
        is_ics_support && is_sups_cmd_supported_by_ims
        )
      {
        return CMSUPS_CMD_ALLOW_IMS;
      }
      else
      {
        return CMSUPS_CMD_ALLOW_CS;
      }
      break;
    default:
      return CMSUPS_CMD_ALLOW_CS;
    }
  }

}

static void cmipsups_common_cmd_proc(cmipapp_id_type   app_id, cm_sups_cmd_s_type    *in_sups_cmd_ptr)
{
  cmipsups_cmd_s_type      *ip_cmd_ptr;
  cmipsups_register_ss_req_e_type  *ip_register_cmd_ptr;
  cmipsups_common_ss_req_e_type    *ip_common_cmd_ptr;
  cmipsups_process_unstructured_ss_req_s_type *ip_process_uss_cmd_ptr;
  cmipsups_unstructured_ss_res_s_type *ip_uss_res_cmd_ptr;
  cmipsups_unstructured_ss_notify_res_s_type *ip_uss_notify_res_cmd_ptr;
  cmipsups_release_ss_req_s_type *ip_sups_release_cmd_ptr;

  cm_sups_cmd_info_s_type  *in_sups_cmd_info_ptr;
  uint8                    num_bytes = 0;
  /* Number of actual bytes required for USSD data */

  in_sups_cmd_info_ptr = SUPS_CMD_INFO_PTR( in_sups_cmd_ptr );

  switch(in_sups_cmd_ptr->cmd)
  {
    case CM_SUPS_CMD_ACTIVATE:
    case CM_SUPS_CMD_DEACTIVATE:
      {
        ip_common_cmd_ptr = (cmipsups_common_ss_req_e_type*)cm_mem_malloc(sizeof(cmipsups_common_ss_req_e_type));
        ip_common_cmd_ptr->pi = in_sups_cmd_info_ptr->pi;
        ip_cmd_ptr = (cmipsups_cmd_s_type*)ip_common_cmd_ptr;
      }
      break;

    case CM_SUPS_CMD_ERASE:
    case CM_SUPS_CMD_INTERROGATE:
      /* parmeet: add commands here for the newly added ones */
      {
        ip_cmd_ptr = (cmipsups_cmd_s_type*)cm_mem_malloc(sizeof(cmipsups_cmd_s_type));
      }
      break;

    case CM_SUPS_CMD_REGISTER:
      {
        ip_register_cmd_ptr = (cmipsups_register_ss_req_e_type*)cm_mem_malloc(sizeof(cmipsups_register_ss_req_e_type));

        ip_register_cmd_ptr->forwarded_to_number = in_sups_cmd_info_ptr->forwarded_to_number;
        ip_register_cmd_ptr->forwarded_to_subaddress = in_sups_cmd_info_ptr->forwarded_to_subaddress;
        ip_register_cmd_ptr->cm_no_reply_condition_timer = in_sups_cmd_info_ptr->nr_timer;
        ip_register_cmd_ptr->sups_time_info = in_sups_cmd_info_ptr->sups_time_info;

        ip_cmd_ptr = (cmipsups_cmd_s_type*)ip_register_cmd_ptr;
      }
      break;

    case CM_SUPS_CMD_PROCESS_USS:
      {
        ip_process_uss_cmd_ptr = (cmipsups_process_unstructured_ss_req_s_type *)cm_mem_malloc(sizeof(cmipsups_process_unstructured_ss_req_s_type));

        ip_process_uss_cmd_ptr->ss_operation = in_sups_cmd_info_ptr->ss_operation;

        CM_MSG_MED_2( "CM_SUPS_CMD_PROCESS_USS Size: %d bytes DCS: %d",
                      in_sups_cmd_info_ptr->uss_data.size,
                      in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme );

        /*lint -save -e641 Converting enum to int */
        if(((in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
             CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_7_BIT)
            ||
           ((in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
             CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UNSPECIFIED)
          )
        {
          byte num_chars = in_sups_cmd_info_ptr->uss_data.size;
          /* MAX_USS_CHAR is what MN can support */
          if(num_chars > MAX_USS_CHAR_ASCII)
          {
            num_chars = MAX_USS_CHAR_ASCII;
            CM_MSG_HIGH_1("Truncating USSD msg to %d bytes", num_chars);
          }

          /* convert to the default alphabet */
          num_bytes = cm_util_ascii_to_gsm_alphabet(
            ip_process_uss_cmd_ptr->uss_data.ussData,
            in_sups_cmd_info_ptr->uss_data.ussData,
            num_chars);

          ip_process_uss_cmd_ptr->uss_data.size = num_bytes;
        }

        /* Usage of CM_SUPS_CMD_PROCESS_USS has been to encode ussd data in CM
        ** and then send. EXCEPTION is being made to UCS2 where data being
        ** received is in encoded form. It is a TEMPORARY fix and needs to be
        ** changed with a new field that says if encoding has already been done.
        */
        else if(((in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
          CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_8_BIT)
          ||
          ((in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
          CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UCS2)
          )
        {
          byte uss_data_size = in_sups_cmd_info_ptr->uss_data.size;
          if(uss_data_size > MAX_USS_BYTES)
          {
            uss_data_size = MAX_USS_BYTES;
            CM_MSG_HIGH_1("Truncating USSD msg to %d bytes", uss_data_size);
          }

          memscpy( ip_process_uss_cmd_ptr->uss_data.ussData,
                   sizeof(ip_process_uss_cmd_ptr->uss_data.ussData),
                   in_sups_cmd_info_ptr->uss_data.ussData,
                   uss_data_size );

          ip_process_uss_cmd_ptr->uss_data.size = uss_data_size;
        }
        else
        {
          CM_ERR_0(" USSD Data coding scheme error");
        }
        /*lint -restore 641 Converting enum to int */

        ip_process_uss_cmd_ptr->uss_data.uss_data_coding_scheme = 
          in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme;

        ip_process_uss_cmd_ptr->uss_data.present = TRUE;
        in_sups_cmd_info_ptr->uss_data.present = TRUE;

        CM_MSG_HIGH_4( "uss data sent to IP USSD present:%d dcs: %d size: %d uss_data: %s",
                      in_sups_cmd_info_ptr->uss_data.present,
                      in_sups_cmd_info_ptr->uss_data.uss_data_coding_scheme,
                      in_sups_cmd_info_ptr->uss_data.size,
                      in_sups_cmd_info_ptr->uss_data.ussData );
        ip_cmd_ptr = (cmipsups_cmd_s_type*)ip_process_uss_cmd_ptr;
      }
      break;

    case CM_SUPS_CMD_USS_RES:
      {
        ip_uss_res_cmd_ptr = (cmipsups_unstructured_ss_res_s_type *)cm_mem_malloc(sizeof(cmipsups_unstructured_ss_res_s_type));

        ip_uss_res_cmd_ptr->invoke_id =   in_sups_cmd_info_ptr->invoke_id;

        if(in_sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR)
        {
          ip_uss_res_cmd_ptr->uss_res_data.data.cm_ss_error =
            in_sups_cmd_info_ptr->ss_error;
          ip_uss_res_cmd_ptr->uss_res_data.data_type = SS_ERROR_INFO;
        }
        else if(in_sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_DATA)
        {
          ip_uss_res_cmd_ptr->uss_res_data.data.uss_data =
            in_sups_cmd_info_ptr->uss_data;
          ip_uss_res_cmd_ptr->uss_res_data.data_type = USS_DATA_INFO;
        }

        ip_cmd_ptr = (cmipsups_cmd_s_type*)ip_uss_res_cmd_ptr;
      }
      break;

    case CM_SUPS_CMD_USS_NOTIFY_RES:
      {
        ip_uss_notify_res_cmd_ptr = (cmipsups_unstructured_ss_notify_res_s_type *)cm_mem_malloc(sizeof(cmipsups_unstructured_ss_notify_res_s_type));
      
        /* Copy command information fields. */
        ip_uss_notify_res_cmd_ptr->invoke_id = in_sups_cmd_info_ptr->invoke_id;
    
        if( in_sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR)
        {
          ip_uss_notify_res_cmd_ptr->cm_ss_error = in_sups_cmd_info_ptr->ss_error;
        }
        ip_cmd_ptr = (cmipsups_cmd_s_type*)ip_uss_notify_res_cmd_ptr;
      }
      break;


    case CM_SUPS_CMD_RELEASE:
      ip_sups_release_cmd_ptr = (cmipsups_release_ss_req_s_type *)cm_mem_malloc(sizeof(cmipsups_release_ss_req_s_type));
      ip_sups_release_cmd_ptr->invoke_id = in_sups_cmd_info_ptr->invoke_id;
      ip_sups_release_cmd_ptr->ss_operation = in_sups_cmd_info_ptr->ss_operation;
      ip_sups_release_cmd_ptr->release_cause.present = FALSE;

      /* Reset the MT USSD route context stored in global*/
      cmsups_reset_mt_uss_context_info(in_sups_cmd_ptr->cmd_info.cmd_subs_id);
      ip_cmd_ptr = (cmipsups_cmd_s_type *)ip_sups_release_cmd_ptr;
      break;

    default:
      CM_MSG_HIGH_1("cmipsups_common_cmd_proc: unsupported cmd %d", in_sups_cmd_ptr->cmd);
      return;
  }

  ip_cmd_ptr->cmd_type = in_sups_cmd_ptr->cmd;
  ip_cmd_ptr->cm_ss_ref = in_sups_cmd_info_ptr->ss_ref;
  ip_cmd_ptr->ss_code = in_sups_cmd_info_ptr->ss_code;
  ip_cmd_ptr->basic_service = in_sups_cmd_info_ptr->basic_service;

	if ( ip_cmd_ptr->ss_code == (byte)bil && 
		 ip_cmd_ptr->cmd_type != CM_SUPS_CMD_INTERROGATE )
	{
	  ip_cmd_ptr->call_barring_num_list = in_sups_cmd_info_ptr->call_barring_num_list;
	}

  cmipapp_send_sups_cmd(app_id, (cmipsups_cmd_s_type*)ip_cmd_ptr, in_sups_cmd_info_ptr->cmd_subs_id);

  cm_mem_free(ip_cmd_ptr);
  cmsups_cmd_copy_fields( cmsups_ptr(), in_sups_cmd_ptr );
  return;
}

/*===========================================================================

FUNCTION CMSUPS_CONF_COPY

DESCRIPTION
Copy the confirmation from MN to the SUPS object.

DEPENDENCIES
SUPS object must have already been initialized with
cmsups_init().

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
static void cmipsups_conf_copy(
  const cmipsups_ss_info_s_type   *ss_info,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cm_sups_info_s_type  *sups_conf_ptr;

  int                   i;
  int                   j;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_info           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sups_conf_ptr = cmsups_get_sups_info_ptr_asubs_id(asubs_id);

  /* save the confirmation type */
  sups_conf_ptr->conf_type = ss_info->info_type;

  if( sups_conf_ptr->conf_type == NO_INFO)
    /* confirmation with no parameter information*/
    /* no further processing is required */
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (ss_info->info_type)
  {
  case FWD_INFO:
    sups_conf_ptr->forwarding_info = ss_info->info.forwarding_info;
    if( sups_conf_ptr->forwarding_info.forwarding_feature_list.
      forwarding_feature_list_length != 0 )

    {
      for( i = 0;
        i < sups_conf_ptr->forwarding_info.forwarding_feature_list.
        forwarding_feature_list_length;
      i++ )
      {
        for ( j = 0; j < MAXNO_FORWARD_TO_NUMBER; j++ )
        {
          if( sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j].present )
          {
            memscpy(
              &sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
              sizeof(sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]),
              &ss_info->info.forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
              sizeof( ss_info->info.forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]));
          }
        }
      }
    }
    break;

  case CALL_BARRING_INFO:
    sups_conf_ptr->call_barring_info = ss_info->info.call_barring_info;
    break;

  case SS_DATA_INFO:
    sups_conf_ptr->ss_data = ss_info->info.ss_data;
    break;

  case SS_ERROR_INFO:
    sups_conf_ptr->ss_error = ss_info->info.cm_ss_error;
    break;

  default:
    CM_MSG_MED_1( "ERR: Unknown Confirmation type %d received", ss_info->info_type);
    sups_conf_ptr->conf_type = UNKNOWN_CONF_INFO;
    break;
  }
}



static void cmipsups_common_conf_proc(
  const cmipsups_common_ss_conf_type *common_cnf,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmsups_s_type               *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type         *sups_info_ptr;

  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ; 
    
  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);
  sups_info_ptr->ss_ref = common_cnf->cm_ss_ref;
 
  sups_info_ptr->retry_duration = common_cnf->retry_duration;
  sups_info_ptr->sups_err_code =  (cm_ipsups_err_code_e_type)common_cnf->sups_ss_info.info.cm_ss_error.error_code;
  sups_info_ptr->ss_error.error_code_tag = common_cnf->sups_ss_info.info.cm_ss_error.error_code_tag;
  sups_info_ptr->ss_error.present = common_cnf->sups_ss_info.info.cm_ss_error.present;
  CM_MSG_HIGH_3("cmipsups_common_conf_proc: retry duration: %d , error code: %d, error tag : %d" ,sups_info_ptr->retry_duration,\
                   common_cnf->sups_ss_info.info.cm_ss_error.error_code,common_cnf->sups_ss_info.info.cm_ss_error.error_code_tag);

  if ( common_cnf->success )
  {
    cmipsups_conf_copy( &common_cnf->sups_ss_info, asubs_id);
    sups_info_ptr->ss_success = TRUE;
  }
  else
  {  
    /* error in the message received from the network */
    sups_info_ptr->ss_success = FALSE;
  }

  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)  

  if(common_cnf->sups_ss_error_text!= NULL)
  {
    sups_info_ptr->sups_ss_error_text = common_cnf->sups_ss_error_text;
    CM_MSG_HIGH_2("cmipsups_common_conf_proc:err_length %d , text : %s",sups_info_ptr->sups_ss_error_text->err_text_length,
	  (char*)sups_info_ptr->sups_ss_error_text->err_text);
  }
  else
  {
     sups_info_ptr->sups_ss_error_text = NULL;
	 CM_MSG_HIGH_0("cmipsups_common_conf_proc: IMS err string absent");
  }
  
  #endif
  
  return;
}

static void cmipsups_interrogate_conf_proc
(
  const cmipsups_interrogate_ss_conf_type *interrogate_cnf,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  const cmipsups_interrogatess_res_s_type *interr_res_ptr;
  cmsups_s_type               *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type         *sups_info_ptr;
  uint32 i,j;

  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ;  

  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);

  sups_info_ptr->ss_ref = interrogate_cnf->cm_ss_ref;
  sups_info_ptr->retry_duration = interrogate_cnf->retry_duration;
  sups_info_ptr->sups_err_code = interrogate_cnf->interrogate_ss_res.cm_ss_error.error_code;
  sups_info_ptr->ss_error.error_code_tag =interrogate_cnf->interrogate_ss_res.cm_ss_error.error_code_tag;
  sups_info_ptr->sups_time_info =interrogate_cnf->interrogate_ss_res.sups_time_info;

  CM_MSG_HIGH_3("cmipsups_interrogate_conf_proc: retry duration: %d , error code: %d, error tag : %d" ,interrogate_cnf->retry_duration, \
		 interrogate_cnf->interrogate_ss_res.cm_ss_error.error_code,interrogate_cnf->interrogate_ss_res.cm_ss_error.error_code_tag);
		 
  if( interrogate_cnf->success )
  {
    sups_info_ptr->ss_success = TRUE;

    interr_res_ptr = &interrogate_cnf->interrogate_ss_res;

    /* confirmation type */
    sups_info_ptr->conf_type = interrogate_cnf->res_type;

	/* presentation indicator */
	sups_info_ptr->pi = interr_res_ptr->pi;

    /* save the interrogate SS response */
    switch( sups_info_ptr->conf_type )
    {
      /* no information */
    case NO_INFO:
      break;
      /* ss status */
    case SS_STATUS_INFO:
      sups_info_ptr->ss_status = interr_res_ptr->cm_ss_status;
      break;

      /* cause */
    case CC_CAUSE_INFO:
      sups_info_ptr->cc_cause = interr_res_ptr->cm_cc_cause;
      break;

      /* ss error */
    case SS_ERROR_INFO:
      sups_info_ptr->ss_error = interr_res_ptr->cm_ss_error;
      break;

      /* CLI restriction */
    case CLI_RESTRICTION_INFO:
      sups_info_ptr->cli_restriction = interr_res_ptr->cli_restriction_info;
      break;

      /* Forwarding Feature List */
    case FWD_FEATURE_LIST_INFO:
      sups_info_ptr->fwd_feature_list =
        interr_res_ptr->forwarding_feature_list;
      if( sups_info_ptr->fwd_feature_list.forwarding_feature_list_length != 0 )
      {
        for( i = 0; i < sups_info_ptr->fwd_feature_list.forwarding_feature_list_length; i++)
        {
          for ( j = 0; j < MAXNO_FORWARD_TO_NUMBER; j++ )
          {
            if ( sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j].present )
            {
              memscpy(
                &sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j],
                sizeof(sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j]),
                &interr_res_ptr->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
                sizeof( interr_res_ptr->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]) );
            }
          }
        }
      }
      break;         

		case CALL_BARRING_NUM_LIST_INFO:
		  
		  if (interr_res_ptr->call_barring_num_info_list->present)
		  {
		  	  sups_info_ptr->call_barring_num_info_list = interr_res_ptr->call_barring_num_info_list;
                    CM_MSG_HIGH_0( "Copied call barring num info list to sups_info_ptr");
		  }
		  
		  break;
			
      /* Basic Service Group List */
    case BSG_LIST_INFO:
      sups_info_ptr->bsg_list = interr_res_ptr->basic_service_group_list;
      break;

      /* Forwarded to Number */
    case FWD_TO_NUM_INFO:
    case CALLED_PARTY_NUM_INFO:
      break;

    default:
      CM_MSG_MED_1( "ERR: Unknown Confirmation type %d received", sups_info_ptr->conf_type);
      /* required if lower layer does not set the unknown info type */
      sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
      break;  
    }
  }
  else
  {
    /* error in the message received from the network */
    sups_info_ptr->ss_success = FALSE;

  }

  return;
}

static void cmipsups_unstructured_ss_ind_proc(
  const cmipsups_unstructured_ss_ind_s_type *unstructured_ss_ind,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmsups_s_type               *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type         *sups_info_ptr;

  CM_MSG_MED_0("RXD: CM_IP_SUPS_UNSTRUCTURED_SS_IND");
  
  /* Update the number of pending sups requests */
  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ; 

  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);

  /* USS invoke id */
  sups_info_ptr->invoke_id = unstructured_ss_ind->invoke_id;

  /* USS data */
  sups_info_ptr->uss_data = unstructured_ss_ind->uss_data;

}

static void cmipsups_unstructured_ss_notify_ind_proc(
  const cmipsups_unstructured_ss_notify_ind_s_type *unstructured_ss_notify_ind,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmsups_s_type               *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type         *sups_info_ptr;

  CM_MSG_MED_0("RXD: CM_IP_SUPS_UNSTRUCTURED_SS_NOTIFY_IND");
  
  /* Update the number of pending sups requests */
  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ; 

  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);

  /* USS invoke id */
  sups_info_ptr->invoke_id = unstructured_ss_notify_ind->invoke_id;

  /* USS data */
  sups_info_ptr->uss_data = unstructured_ss_notify_ind->uss_data;

}


static void cmipsups_process_unstructured_ss_conf(
  const cmipsups_process_unstructured_ss_conf_s_type *process_unstructured_ss_conf,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmsups_s_type				*sups_ptr = cmsups_ptr();
  const cmipsups_process_unstructured_ss_res_s_type *ss_conf_res_ptr;
  cm_sups_info_s_type		  *sups_info_ptr;

  CM_MSG_MED_0("RXD: CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF");

  /* Update the number of pending sups requests */
  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ; 
    
  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);

  sups_info_ptr->ss_ref = process_unstructured_ss_conf->cm_ss_ref;

  sups_info_ptr->sups_err_code = (cm_ipsups_err_code_e_type)process_unstructured_ss_conf->ss_data_info.data.cm_ss_error.error_code;
  sups_info_ptr->ss_error.error_code_tag = process_unstructured_ss_conf->ss_data_info.data.cm_ss_error.error_code_tag;
  sups_info_ptr->ss_error.present = process_unstructured_ss_conf->ss_data_info.data.cm_ss_error.present;
  CM_MSG_HIGH_3( "cmipsups_common_conf_proc: error code: %d, error tag: %d, present: %d",
                 sups_info_ptr->sups_err_code,
                 sups_info_ptr->ss_error.error_code_tag,
                 sups_info_ptr->ss_error.present);

  if ( process_unstructured_ss_conf->success)
  {
    ss_conf_res_ptr = &process_unstructured_ss_conf->ss_data_info;
    sups_info_ptr->ss_success = TRUE;

    /* confirmation type */
    sups_info_ptr->conf_type = ss_conf_res_ptr->data_type;       

    sups_info_ptr->uss_data_type = CM_USS_PHASE2_TYPE;

    switch( sups_info_ptr->conf_type )
    {
      /* no information */
    case NO_INFO:
      break;

      /* USS data */
    case USS_DATA_INFO:
      sups_info_ptr->uss_data =
        ss_conf_res_ptr->data.uss_data;
      break;

      /* SS error */
    case SS_ERROR_INFO:
      sups_info_ptr->ss_error =
        ss_conf_res_ptr->data.cm_ss_error;
      break;

      /* Cause */
    case CC_CAUSE_INFO:
      sups_info_ptr->cc_cause =
        ss_conf_res_ptr->data.cm_cc_cause;
      break;

    default:
      CM_MSG_MED_1( "ERR: Unknown Confirmation type %d received", 
        sups_info_ptr->conf_type );
      /* required if lower layer does not set the unknown info type */
      sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
      break;
    }

  }
  else
  {
    sups_info_ptr->ss_success = FALSE;
  }

}


static void cmipsups_rel_unstructured_ss_ind(
  const cmipsups_release_unstructured_ss_ind_s_type  *rel_unstructured_ss_ind,

  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmsups_s_type               *sups_ptr = cmsups_ptr();
  cm_sups_info_s_type         *sups_info_ptr;

  CM_MSG_MED_0("RXD: CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND");

  /* Update the number of pending sups requests */
  if (sups_ptr->num_pending_sups_cmds != 0)
    sups_ptr->num_pending_sups_cmds-- ; 

  /* Get sups_info_ptr from global sups object */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(asubs_id);

  /* clear the invocation ID */
  sups_info_ptr->invoke_id = 0;

  /* Reset the MT USSD route context stored in global*/
  cmsups_reset_mt_uss_context_info( asubs_id );
}

/*===========================================================================

FUNCTION cmsups_client_cmd_forward_ip

DESCRIPTION
Forward a client sups commands to IMS.

DEPENDENCIES
SUPS object must have already been initialized with
cmsups_init().

RETURN VALUE
TRUE if CM needs to wait for a replay from MN before continuing
processing this command, FALSE otherwise.

SIDE EFFECTS
none

===========================================================================*/
void  cmipsups_client_cmd_forward_ip(

                                     cmsups_s_type  *sups_ptr,
                                     /* Pointer to the sups object */

                                     cm_sups_cmd_s_type    *in_sups_cmd_ptr
                                     /* Pointer to a CM command */
                                     )
{
  cmipapp_id_type   app_id = CMIPAPP_ID_INVALID; 
  cm_sups_event_e_type     sups_event_type = CM_SUPS_EVENT_NONE;
  cm_sups_cmd_info_s_type  *in_sups_cmd_info_ptr;

  in_sups_cmd_info_ptr = SUPS_CMD_INFO_PTR( in_sups_cmd_ptr );

  /* parmeet: need to change this logic, as new commands would only go on USSD interface now */
  if(cmipsups_is_ussd_sups_cmd(in_sups_cmd_ptr->cmd))
  {
    app_id = cmipapp_find_ussd_support_app();
    if(app_id == CMIPAPP_ID_INVALID)
    {
      CM_MSG_HIGH_0("cmipsups: No USSD app found");
      return;
    }
  }
  else
  {	
    app_id = cmipapp_find_ics_support_app();
    if(app_id == CMIPAPP_ID_INVALID)
    {
      CM_MSG_HIGH_0("cmipsups: No ICS app found");
      return;
    }
  }

  switch( in_sups_cmd_ptr->cmd )
  {
  case CM_SUPS_CMD_ACTIVATE:
  case CM_SUPS_CMD_DEACTIVATE:
  case CM_SUPS_CMD_ERASE:
  case CM_SUPS_CMD_INTERROGATE:
  case CM_SUPS_CMD_REGISTER:
  case CM_SUPS_CMD_PROCESS_USS:
  case CM_SUPS_CMD_USS_RES:
  case CM_SUPS_CMD_USS_NOTIFY_RES:
  case CM_SUPS_CMD_RELEASE:

    cmipsups_common_cmd_proc(app_id, in_sups_cmd_ptr);
    break;

  default:
    CM_MSG_MED_1("cmipsups, cmd_type %d not supprted",in_sups_cmd_ptr->cmd);
    return;

  }

  /* send sups event */
  sups_event_type = cmsups_cmd_to_event_map(in_sups_cmd_ptr->cmd);
  if(sups_event_type != CM_SUPS_EVENT_NONE)
  {
    cmsups_event (cmph_map_subs_to_ss(in_sups_cmd_info_ptr->cmd_subs_id),
                                                               sups_event_type);
  }

  
  if(in_sups_cmd_info_ptr->ss_code == (byte)bil && 
  	 in_sups_cmd_ptr->cmd != CM_SUPS_CMD_INTERROGATE )
  {		  
    ref_cnt_obj_release(in_sups_cmd_info_ptr->call_barring_num_list);
        CM_MSG_HIGH_1("Released ref for in_sups_cmd_info_ptr->call_barring_num_list cmd: %d",in_sups_cmd_ptr->cmd);
  }
  
  return;
}


/*===========================================================================

FUNCTION cmipsups_map_conf_with_request

DESCRIPTION
MAPS SUPS confirmation with the request 

DEPENDENCIES
Call object must have already been initialized with
cmcall_init().

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
cm_sups_cmd_e_type cmipsups_map_conf_with_request(

 cm_name_type   cmd
 /* sups cnf cmd  */
)

{
  cm_sups_cmd_e_type ss_req_cmd =CM_SUPS_CMD_NONE;
  switch(cmd)
  {
	case CM_IP_SUPS_ACTIVATE_SS_CONF:
      ss_req_cmd = CM_SUPS_CMD_ACTIVATE;
      break;

    case CM_IP_SUPS_DEACTIVATE_SS_CONF:
      ss_req_cmd = CM_SUPS_CMD_DEACTIVATE;
      break;

    case CM_IP_SUPS_ERASE_SS_CONF:
      ss_req_cmd =CM_SUPS_CMD_ERASE;
      break;

    case CM_IP_SUPS_REGISTER_SS_CONF:
      ss_req_cmd =CM_SUPS_CMD_REGISTER;
      break;

    case CM_IP_SUPS_INTERROGATE_SS_CONF:
      ss_req_cmd = CM_SUPS_CMD_INTERROGATE;
      break;

    case CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF:
      ss_req_cmd = CM_SUPS_CMD_PROCESS_USS;
      break;

    case CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND:
      ss_req_cmd = CM_SUPS_CMD_PROCESS_USS;
      break;

    default:
      CM_MSG_HIGH_1("No matching sups request found : %d",cmd );
      break;

  }
 
  return ss_req_cmd;
}

/*===========================================================================

FUNCTION cmipsups_reset_sups_info_error

DESCRIPTION
Resets sups info error fields upon CS fallback reason from IMS

DEPENDENCIES
Call object must have already been initialized with
cmcall_init().

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipsups_reset_sups_info_error(cm_sups_info_s_type* sups_info_ptr)
{
  if(sups_info_ptr == NULL)
  {
    CM_MSG_HIGH_0("cmipsups_reset_sups_info_error: sups_info_ptr is NULL");
    return;
  }

  CM_MSG_MED_0("resetting sups info error");
  sups_info_ptr->sups_err_code = CM_IPSUPS_ERR_NONE;
  sups_info_ptr->ss_error.error_code_tag =0;
  sups_info_ptr->ss_error.error_code = 0;
  sups_info_ptr->conf_type = 0;
  sups_info_ptr->ss_error.present = FALSE;
  sups_info_ptr->ss_error.error_type = NO_ERR;

}
/*===========================================================================

FUNCTION cmsups_is_wlan_sups_rpt

DESCRIPTION
check if the rpt is on wlan

DEPENDENCIES
Call object must have already been initialized with
cmcall_init().

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/

static boolean cmsups_is_wlan_sups_rpt(cm_name_type rpt)
{
#ifdef FEATURE_IP_CALL
  /*USSD go with voice RAT*/
  if(cmipsups_is_ussd_sups_rpt(rpt))
  {
    if(cmipapp_find_ussd_support_app() != CMIPAPP_ID_INVALID &&
      cmipapp_find_active_app_sys_mode(CM_CALL_TYPE_VOICE) == SYS_SYS_MODE_WLAN)
    {
      return TRUE;
    }
  }
  /*IMS do cmipapp_cap_reg for sups command over UT, not for USSD*/
  else
  {
    if(cmipapp_find_active_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_SUPS) != CMIPAPP_ID_INVALID)
    {
      return TRUE;
    }
  }
#endif
  return FALSE;
}
/*===========================================================================

FUNCTION cmipsups_convert_to_ue_based_cw

DESCRIPTION
  convert NW based cw to ue based. If IMS Ut client receives 403 response on any XCAP Ut request, 
  it will be determined that it is a Non-VoLTE subscriber and CW setting request will be sent to CS network.
DEPENDENCIES
Call object must have already been initialized with
cmcall_init().

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
static void cmipsups_convert_to_ue_based_cw(const cmipapp_rpt_s_type *cmipapp_sups_rpt_ptr)
{
#ifdef CM_GW_SUPPORTED
  cm_sups_cmd_info_s_type sups_cmd_info;
  sys_modem_as_id_e_type      sub_id = SYS_MODEM_AS_ID_1;
  volte_sub_info_s_type* volte_sub_info_ptr = NULL;
  boolean current_is_enabled;

  CM_MSG_MED_0("cmipsups_convert_to_ue_based_cw");

  sub_id = cmipapp_sups_rpt_ptr->ip_ind.hdr.asubs_id;
  volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(sub_id);
  sups_cmd_info.cmd_subs_id = sub_id;
  current_is_enabled = volte_sub_info_ptr->ue_based_cw.is_enabled;

  switch(cmipapp_sups_rpt_ptr->hdr.cmd)
  {
  case CM_IP_SUPS_ACTIVATE_SS_CONF:
    sups_cmd_info.ss_ref = cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.cm_ss_ref;
    cmsups_create_activate_cnf_internal(&sups_cmd_info);
    break;
  case CM_IP_SUPS_DEACTIVATE_SS_CONF:
    sups_cmd_info.ss_ref = cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.cm_ss_ref;
    cmsups_create_deactivate_cnf_internal(&sups_cmd_info);
    break;
  case CM_IP_SUPS_INTERROGATE_SS_CONF:
  {
    cm_interrogate_ss_conf_T  *interrogate_ss_conf = NULL;
    cm_rpt_type               *cm_rpt_ptr          = NULL;

    cm_rpt_ptr = cm_get_cmd_buf ();
    if(cm_rpt_ptr == NULL)
    {
      CM_ERR_FATAL_0("cannot get buffer");
    }

    CM_MSG_MED_2("CM_IP_SUPS_INTERROGATE_SS_CONF, bsg_present %d, ss_status present %d", 
                    cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.basic_service_group_list.present,
                    cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_status.present);

    
    interrogate_ss_conf = &cm_rpt_ptr->cmd.interrogate_conf;
    cm_rpt_ptr->hdr.cmd                        = CM_INTERROGATE_SS_CONF;
    interrogate_ss_conf->as_id                 = sub_id;
    interrogate_ss_conf->cm_ss_ref             = cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.cm_ss_ref;
    interrogate_ss_conf->success               = TRUE;
    if(cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.basic_service_group_list.present)
    {
      /*cw enabled*/ 
      interrogate_ss_conf->res_type              = BSG_LIST_INFO;
      interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list = cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.basic_service_group_list;
      volte_sub_info_ptr->ue_based_cw.is_enabled = TRUE;

    }
    else if(cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_status.present)
    {
      /*cw disabled*/
     interrogate_ss_conf->res_type              = SS_STATUS_INFO;
     interrogate_ss_conf->InterrogateSS_Res.cm_ss_status = cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_status;
     volte_sub_info_ptr->ue_based_cw.is_enabled = FALSE;
    }
    cm_put_cmd (cm_rpt_ptr);
    break;
  }
  default:
    break;
  }

  if(volte_sub_info_ptr->ue_based_cw.use_ue_based_cw != TRUE ||
    volte_sub_info_ptr->ue_based_cw.is_enabled != current_is_enabled)
  {
    volte_sub_info_ptr->ue_based_cw.use_ue_based_cw = TRUE;
    cmefs_fs_write(CMEFS_UE_BASED_CW,
                     (byte*)&(volte_sub_info_ptr->ue_based_cw),
                     sizeof(cm_ue_based_cw_s_type),
                     sub_id);
  }
#else
  CM_ERR_0("CM_GW_SUPPORTED not defined!");
#endif
  return;
}

/*===========================================================================

FUNCTION cmipsups_send_release_ss_req

DESCRIPTION
Send Release request to IMS with release cause

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void cmipsups_send_release_ss_req(
  byte invoke_id,
  byte ss_operations,
  cmipsups_ussd_reject_cause_s_type release_cause,
  sys_modem_as_id_e_type              asubs_id
  )
{
  cmipsups_release_ss_req_s_type *ip_sups_release_cmd_ptr;
  cmipsups_cmd_s_type            *ip_cmd_ptr;
  cmipapp_id_type                 app_id = CMIPAPP_ID_INVALID; 

  ip_sups_release_cmd_ptr = (cmipsups_release_ss_req_s_type *)cm_mem_malloc(sizeof(cmipsups_release_ss_req_s_type));
  
  ip_sups_release_cmd_ptr->invoke_id = invoke_id;
  ip_sups_release_cmd_ptr->ss_operation = ss_operations;
  ip_sups_release_cmd_ptr->release_cause = release_cause;
  ip_cmd_ptr = (cmipsups_cmd_s_type *)ip_sups_release_cmd_ptr;

  ip_cmd_ptr->cmd_type = CM_SUPS_CMD_RELEASE;
  ip_cmd_ptr->cm_ss_ref = invoke_id;
  ip_cmd_ptr->ss_code = 0;
  ip_cmd_ptr->basic_service.present = FALSE;

  app_id = cmipapp_find_ussd_support_app();
  if(app_id != CMIPAPP_ID_INVALID)
  {
    cmipapp_send_sups_cmd(app_id, (cmipsups_cmd_s_type*)ip_cmd_ptr, asubs_id);
  }

  cm_mem_free( ip_cmd_ptr );
  return;  
} /* cmipsups_send_release_ss_req () */


/*===========================================================================

FUNCTION cmipsups_rpt_proc

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
void cmipsups_rpt_proc(

                       const cm_hdr_type   *rpt_ptr
                       /* Pointer to a IP app report */
                       )
{
  const cmipapp_rpt_s_type  *cmipapp_sups_rpt_ptr = (cmipapp_rpt_s_type *) rpt_ptr;
  cm_sups_event_e_type       sups_event_type = CM_SUPS_EVENT_NONE;
  sd_ss_mode_pref_e_type     comm_mode=SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type     comm_band=SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type     comm_tds_band=SD_SS_BAND_PREF_NONE;
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  sys_lte_band_mask_e_type  comm_lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
  #endif
  cmsups_s_type             *sups_ptr = cmsups_ptr();
  sys_modem_as_id_e_type        asubs_id      = SYS_MODEM_AS_ID_1;
  cm_ss_e_type                  ss            = CM_SS_MAIN;
  cm_sups_info_s_type          *sups_info_ptr = NULL;
  cm_sups_uss_data_info_s_type *uss_info_ptr  = NULL;
  if(cmipapp_sups_rpt_ptr == NULL) 
  {
    CM_MSG_HIGH_0("cmipsups_rpt_proc: NULL cmipapp_sups_rpt_ptr");
    return;
  }

  asubs_id      = cmipapp_sups_rpt_ptr->ip_ind.hdr.asubs_id;
  if (asubs_id < SYS_MODEM_AS_ID_1 || asubs_id >= MAX_SIMS)
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  ss            = cmph_map_subs_to_ss(asubs_id);
  sups_info_ptr = cmsups_get_sups_info_ptr_asubs_id(asubs_id);
  uss_info_ptr  = cmsups_get_uss_info_ptr(asubs_id, cmipsups_map_conf_with_request(cmipapp_sups_rpt_ptr->hdr.cmd));


  if((cmph_ptr()->oprt_mode != SYS_OPRT_MODE_ONLINE) && 
    (!cmsups_is_wlan_sups_rpt(cmipapp_sups_rpt_ptr->hdr.cmd))
   )
  {
    CM_MSG_MED_1("cmipsups_rpt_proc: ignoring sups rsp %d as ph not online", cmipapp_sups_rpt_ptr->hdr.cmd);
    return;
   
  } 
  else if ((uss_info_ptr != NULL && uss_info_ptr->is_fallback_to_cs == TRUE) && 
             cmipapp_sups_rpt_ptr->hdr.cmd   == CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND)
  {

    CM_MSG_HIGH_0("Ignore RELEASE_UNSTRUCTURED_SS_IND since USS command has fallback to CS");
    uss_info_ptr->is_fallback_to_cs = FALSE;
    return;
  }

  switch(cmipapp_sups_rpt_ptr->hdr.cmd)
  {
  case CM_IP_SUPS_ACTIVATE_SS_CONF:
    CM_MSG_MED_4("act_cnf, type %d, err_present %d, tag %d, code %d", 
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info_type,
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.present,
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.error_code_tag,
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.error_code);
    if(cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info_type == SS_ERROR_INFO &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.present == TRUE &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.error_code_tag == CMIPSUPS_SS_ERROR_CODE_TAG &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_info.info.cm_ss_error.error_code == CM_IPSUPS_FAILURE_CAUSE_200_UE_BASED_CW)
     {
       cmipsups_convert_to_ue_based_cw(cmipapp_sups_rpt_ptr);
       return;
     }
    cmipsups_common_conf_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.activate_ss_conf,
                                 asubs_id);
    break;

  case CM_IP_SUPS_DEACTIVATE_SS_CONF:
    CM_MSG_MED_4("deact_cnf, type %d, err_present %d, tag %d, code %d", 
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info_type,
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.present,
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.error_code_tag,
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.error_code);
      
     if(cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info_type == SS_ERROR_INFO &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.present == TRUE &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.error_code_tag == CMIPSUPS_SS_ERROR_CODE_TAG &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_info.info.cm_ss_error.error_code == CM_IPSUPS_FAILURE_CAUSE_200_UE_BASED_CW)
     {
       cmipsups_convert_to_ue_based_cw(cmipapp_sups_rpt_ptr);
       return;
     }
    cmipsups_common_conf_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.deactivate_ss_conf,
                                 asubs_id);
    break;

  case CM_IP_SUPS_ERASE_SS_CONF:
    cmipsups_common_conf_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.erase_ss_conf,
                                 asubs_id);
    break;

  case CM_IP_SUPS_REGISTER_SS_CONF:
    cmipsups_common_conf_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.register_ss_conf,
                                 asubs_id);
    break;

  case CM_IP_SUPS_INTERROGATE_SS_CONF:
   CM_MSG_MED_4("intg_cnf, type %d, err_present %d, tag %d, code %d", 
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.res_type,
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.present,
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.error_code_tag,
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.error_code);
     if(cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.res_type == SS_ERROR_INFO &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.present == TRUE &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.error_code_tag == CMIPSUPS_SS_ERROR_CODE_TAG &&
      cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.cm_ss_error.error_code == CM_IPSUPS_FAILURE_CAUSE_200_UE_BASED_CW)
     {
       cmipsups_convert_to_ue_based_cw(cmipapp_sups_rpt_ptr);
       return;
     }
    cmipsups_interrogate_conf_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.interrogate_ss_conf,
                                   asubs_id);
    break;

  case CM_IP_SUPS_UNSTRUCTURED_SS_IND:
    {
      cmsups_cmd_route_e_type sups_route;
      if ( cmsups_get_mt_uss_routing_context_info( asubs_id, &sups_route) == TRUE &&
           sups_route == CMSUPS_CMD_ALLOW_CS ) 
      {
        cmipsups_ussd_reject_cause_s_type release_cause;
        CM_MSG_HIGH_0("Reject cmd, MT USS on CS domain is ongoing");
        release_cause.present = TRUE;
        release_cause.cause_value = USS_REJECT_REASON_USSD_BUSY;
        cmipsups_send_release_ss_req( cmipapp_sups_rpt_ptr->ip_ind.ind.unstructuresd_ss_ind.invoke_id,
                                      unstructuredSS_Request,
                                      release_cause,
                                      asubs_id );
        return;
      }
      cmipsups_unstructured_ss_ind_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.unstructuresd_ss_ind,
                                         asubs_id);
      cmsups_set_mt_uss_context_info(asubs_id, CMSUPS_CMD_ALLOW_IMS);
    }
    break;

  case CM_IP_SUPS_UNSTRUCTURED_SS_NOTIFY_IND:
    {
      cmsups_cmd_route_e_type sups_route;
      if ( cmsups_get_mt_uss_routing_context_info( asubs_id, &sups_route) == TRUE &&
           sups_route == CMSUPS_CMD_ALLOW_CS ) 
      {
        cmipsups_ussd_reject_cause_s_type release_cause;
        CM_MSG_HIGH_0("Reject cmd, MT USS on CS domain is ongoing");
        release_cause.present = TRUE;
        release_cause.cause_value = USS_REJECT_REASON_USSD_BUSY;
        cmipsups_send_release_ss_req( cmipapp_sups_rpt_ptr->ip_ind.ind.unstructured_ss_notify_ind.invoke_id,
                                      unstructuredSS_Notify,
                                      release_cause,
                                      asubs_id );
        return;
      }
      cmipsups_unstructured_ss_notify_ind_proc(&cmipapp_sups_rpt_ptr->ip_ind.ind.unstructured_ss_notify_ind,
                                               asubs_id);
      cmsups_set_mt_uss_context_info(asubs_id, CMSUPS_CMD_ALLOW_IMS);
    }
    break;

  case CM_IP_SUPS_RELEASE_UNSTRUCTURED_SS_IND:
    cmipsups_rel_unstructured_ss_ind(&cmipapp_sups_rpt_ptr->ip_ind.ind.rel_unstructured_ss_ind,
                                     asubs_id);
    break;

  case CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF:
    cmipsups_process_unstructured_ss_conf(&cmipapp_sups_rpt_ptr->ip_ind.ind.process_unstructured_ss_conf,
                                           asubs_id);
    break;

  default:
    CM_MSG_MED_1("cmipsups:  type %d not applicable",cmipapp_sups_rpt_ptr->hdr.cmd);
    return;
  }
  
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  if(sups_info_ptr != NULL)
  {
    if( (sups_info_ptr->sups_err_code == CM_IPSUPS_FAILURE_CAUSE_PDP_FAILURE
	  && sups_info_ptr->ss_error.error_code_tag == CMIPSUPS_SS_ERROR_CODE_TAG
	  && cmsups_get_srv_domain_pref(asubs_id) == SYS_SUPP_SRV_DOMAIN_PREF_PS_PREF)
	    || 
       (sups_info_ptr->sups_err_code             == CM_IPSUPS_FAILURE_CAUSE_USS_FALLBACK_TO_CS
       && sups_info_ptr->ss_error.error_code_tag == CMIPSUPS_USS_ERROR_CODE_TAG
       && (cmipapp_sups_rpt_ptr->hdr.cmd         == CM_IP_SUPS_PROCESS_UNSTRUCTURED_SS_CONF))
	    )
    {
      cmsups_s_type             *sups_ptr = cmsups_ptr();
      /* Check if the command is required to be routed over CS path */
            /* get mode and band capabilities of PM policy via state machine.  */
      cmph_get_comm_mode_band_capab_with_pm(&comm_mode, &comm_band, &comm_lte_band, &comm_tds_band, asubs_id);/* ASID to confirm */
      if((cmregprx_get_stack_info_ptr(cmph_map_cm_ss_type_to_sd_ss_type(ss))->state != CMREGPRX_STATE_ACTIVE) || 
	  			((comm_mode & SD_SS_MODE_PREF_TDS_GSM_WCDMA ) == SD_SS_MODE_PREF_NONE )||
                 cmcall_is_lte_only_mode(cmph_map_sd_mode_pref_to_cm_mode_pref(comm_mode),\
                 cmph_map_sd_band_pref_to_cm_band_pref(comm_band),\
                 comm_lte_band,\
                 cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(comm_tds_band)))
      {
        CM_MSG_MED_3("cmipsups: sups shouldnt be routed comm_mode=%d,comm_band=0x%08x %08x, comm_lte_band",\
                       comm_mode,QWORD_HIGH(comm_band),QWORD_LOW(comm_band));
        cm_print_lte_band_mask(comm_lte_band);
      }
      else
      {
        CM_MSG_MED_2("cmipsups: fallback reason: %d , sups_cnf: %d",sups_info_ptr->sups_err_code ,cmipapp_sups_rpt_ptr->hdr.cmd );
        cmwsups_client_cmd_forward_gw(asubs_id, cmipsups_map_conf_with_request(cmipapp_sups_rpt_ptr->hdr.cmd));        
        
        if (cmipsups_is_ussd_sups_rpt(cmipapp_sups_rpt_ptr->hdr.cmd) && uss_info_ptr != NULL)
        {
          /* Setting this global to reject Release IND from IMS for USS cmd.*/
          uss_info_ptr->is_fallback_to_cs = TRUE;
        }
        /* Increase the pending sups count */
        sups_ptr->num_pending_sups_cmds++;
        cmipsups_reset_sups_info_error(sups_info_ptr);
        return;
      }
    }
  }
  else
  {
    CM_MSG_HIGH_0("sups info NULL, UT Fallback ignored" );
  }
  #endif
  

  sups_event_type = cmsups_conf_to_event_map(cmipapp_sups_rpt_ptr->hdr.cmd);
  if(sups_event_type != CM_SUPS_EVENT_NONE)
  {
    cmsups_event( ss, sups_event_type);      
  }
 
  if (sups_info_ptr                             != NULL      &&
      sups_info_ptr->ss_code                    == (byte)bil &&
  	  sups_info_ptr->call_barring_num_info_list != NULL)
    {
	  sups_info_ptr->call_barring_num_info_list = NULL;
    }
    if ( sups_info_ptr                             != NULL &&
          sups_info_ptr->sups_ss_error_text != NULL) 
    {
      sups_info_ptr->sups_ss_error_text = NULL;
    }

#if defined (FEATURE_IP_CALL)
  cmipapp_release_reference(cmipapp_sups_rpt_ptr);
  #endif 
  return;
}
#endif

