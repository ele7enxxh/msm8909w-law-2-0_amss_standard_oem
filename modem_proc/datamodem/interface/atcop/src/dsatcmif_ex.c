/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatcmif_cm_call_cmd_handler_ex
    This function is the handler function for the CM call related commands

  dsatcmif_cm_call_event_handler_ex
    This function is the handler function for the CM call related events

  dsatcmif_cm_ss_event_handler_ex
    This function is the handler function for the CM serving system
    related events

  dsatcmif_end_calls
    This function issues a request to CM to end call(s).

  dsatcmif_signal_handler
    This function processes the asynchronous signals assosiacted with
    messages from Call Manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatcmif_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   skc     Fixed issue with at+chld=2 during MPTY calls
12/16/15   skc     Fixed issue where wrong call id is stored during one 
                   active call and one held call and at+chld=2 is issued.
12/10/15   skc     Adding condition for enabling of dsat_power_off only when
                   operating mode is not online.
05/04/15   sc      Fixed +CHLD command issue to use requesting client id
                   instead of call client id. 
03/05/15   sc      Fixed +CHLD command IP call issue.
02/13/15   sc      Fixed +CHLD AT command issue.
11/24/14   pg      Fixed ws46 featurization issue.
07/29/14   sc      Added support for NV Refresh.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
06/27/14   tk/sc   Initial revision (created file for Dynamic ATCoP).
07/08/14   sc      Fixed SGLTE+G issue on SUB2.

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

/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsictab.h"
#include "dsatetsicmif.h"
#include "dsatetsicall.h"

#include "dstask.h"
#include "queue.h"
#include "stdio.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "dsatetsipkt.h"

/* needed for receiving RAB events and inter RAT HO*/
#include "dsatvend.h"

#include "gsndcp.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "mn_cm_exp.h"

#include "mmgsdilib.h"
#include "mmgsdilib_v.h"

#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_IS707
#include "dsat707ext.h"
#if defined(FEATURE_DSAT_SP_CMDS)
#include "dsat707vend.h"
#endif /* defined(FEATURE_DSAT_SP_CMDS) */
#endif /* FEATURE_DATA_IS707  */

#include "dsatme.h"
#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatcmif.h"
#include "dsatvoice.h"
#include "dsatparm.h"
#include "cm.h"
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "dsm.h"

#include <stringl/stringl.h>
#include "dsatvend.h"
#include "ds3gmshif.h"
#include "ds3gmgr.h"

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* +CGEREP variables */
#ifdef FEATURE_ETSI_ATTACH
dsat_num_item_type     dsat_gprs_attach_status[DSAT_SYS_MODEM_AS_ID_MAX];
#endif /* FEATURE_ETSI_ATTACH */

#ifdef FEATURE_DSAT_ETSI_DATA
boolean                dsat_power_off[DSAT_SYS_MODEM_AS_ID_MAX] = {FALSE};
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_LTE_TO_1X
boolean is_csfb_call_active = FALSE;
#endif /* FEATURE_LTE_TO_1X */
#endif /*FEATURE_DSAT_ETSI_MODE*/

#ifdef FEATURE_TDSCDMA
#define AT_MODE_TDS 4
#endif /* FEATURE_TDSCDMA */
#define AT_MODE_NONE 0

#ifdef FEATURE_DSAT_LTE
typedef enum
{
  DSAT_EPS_PS_MODE_2      = 0,  /* UE registers only to EPS ,UE's usage setting is "data centric" */
  DSAT_EPS_CS_PS_MODE_1   = 1,  /* UE registers only to EPS and Non-EPS ,UE's usage setting is "Voice centric" */
  DSAT_EPS_CS_PS_MODE_2   = 2,  /* UE registers only to EPS and Non-EPS ,UE's usage setting is "Data centric" */
  DSAT_EPS_PS_MODE_1      = 3,  /* UE registers only to EPS ,UE's usage setting is "Voice centric" */
  DSAT_EPS_MODE_MAX
} dsatcmif_eps_mode_e_type;
#endif /*FEATURE_DSAT_LTE  */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    import definitions:
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
/* CM client (ATCOP) Identifier */
cm_client_id_type dsatcm_client_id;

/* Call Manager message watermark queue */
LOCAL dsm_watermark_type  dsat_cm_msg_wm;
LOCAL q_type              dsat_cm_msg_q;
LOCAL boolean             dsat_cm_msg_wm_full;
#ifdef IMAGE_QDSP6_PROC
rex_crit_sect_type dsat_cm_wm_crit_sect;
#endif /* IMAGE_QDSP6_PROC*/
/* Lo Water, Hi Water and Don't Exceed counts for the   */
/* watermark used for select events from Call Manager.   */
#define DSAT_CM_MSG_SIZE            (sizeof(dsat_cm_msg_s_type))
#define DSAT_FROM_CM_LOWATER_CNT          (DSAT_CM_MSG_SIZE * 2)
#define DSAT_FROM_CM_HIWATER_CNT          (DSAT_CM_MSG_SIZE * 9)
#define DSAT_FROM_CM_DONT_EXCEED_CNT      (DSAT_CM_MSG_SIZE * 10)
#define DSAT_RSSI     1
#define DSAT_HDR_RSSI 2

/* Cache of RX signal data */
dsat_num_item_type dsat_rssi_enqueue = 0;
#ifdef IMAGE_QDSP6_PROC
rex_crit_sect_type dsat_rssi_crit_sect;
#endif /* IMAGE_QDSP6_PROC*/

#define DSAT_ASUBS_MASK 7
const LOCAL dsatcmif_ss_mask_map_s_type dsatcmif_ss_subs_mask_map[] =
{
#ifdef FEATURE_LTE_TO_1X
  { CM_SS_EVT_CSFB_CALL_ACTIVE_MASK  , DSAT_SS_CSFB_CALL_ACTIVE_MASK },
#endif /* FEATURE_LTE_TO_1X */
  { CM_SS_EVT_PS_DATA_SUSPEND_MASK  , DSAT_SS_PS_DATA_SUSPEND_MASK }
};

/*Change field 1 mapping*/
const LOCAL dsatcmif_ss_mask_map_s_type dsatcmif_ss_stack_mask_map[] =
{
  { CM_SS_EVT_SRV_STATUS_MASK       , DSAT_SS_SRV_STATUS_MASK       },
  { CM_SS_EVT_SRV_DOMAIN_MASK       , DSAT_SS_SRV_DOMAIN_MASK       },
  { CM_SS_EVT_SYS_MODE_MASK         , DSAT_SS_SYS_MODE_MASK         },
  { CM_SS_EVT_SYS_ID_MASK           , DSAT_SS_SYS_ID_MASK           },
  { CM_SS_EVT_SRV_IND_MASK          , DSAT_SS_SRV_IND_MASK          },
  { CM_SS_EVT_MOBILITY_MGMT_MASK    , DSAT_SS_MOBILITY_MGMT_MASK    },
  { CM_SS_EVT_SIM_STATE_MASK        , DSAT_SS_SIM_STATE_MASK        },
  { CM_SS_EVT_SRV_STATUS_MASK       , DSAT_SS_HDR_SRV_STATUS_MASK   },
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  { CM_SS_EVT_CELL_INFO_MASK        , DSAT_SS_CELL_INFO_MASK        },
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
  { CM_SS_LAC_TAC_RAC_CHGD_MASK     , DSAT_SS_LAC_TAC_RAC_CHGD_MASK },
  { CM_SS_EVT_ROAM_STATUS_MASK      , DSAT_SS_ROAM_STATUS_MASK      }, 
  { CM_SS_EVT_SYS_FORBIDDEN_MASK    , DSAT_SS_SYS_FORBIDDEN_MASK    }, 
  { CM_SS_EVT_SRV_CAPABILITY_MASK   , DSAT_SS_SRV_CAPABILITY_MASK   },
  { CM_SS_EVT_CSG_INFO_CHANGED_MASK , DSAT_SS_CSG_INFO_CHANGED_MASK } 
  /*{ CM_SS_EVT_CQI_INFO_MASK         , DSAT_SS_CQI_INFO_MASK },
  { CM_SS_EVT_CELL_SRV_IND_MASK     , DSAT_SS_CELL_SRV_IND_MASK      },*/  
};

/*Change field 2 mapping - TBD*/

/*Place holder for Second stack information in SGLTE*/
sys_modem_as_id_e_type dsat_hybr_subs_id = SYS_MODEM_AS_ID_3;

dsat_sysinfo_s_type dsat_sysinfo_cmd;

/*-------------------------------------------------------------------------
    Local functions:
-------------------------------------------------------------------------*/

void cmif_call_event_cb_func 
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_ss_event_msim_cb_func 
(
  cm_ss_event_e_type            event,                  /* Event ID              */
  const cm_mm_msim_ss_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_cm_ph_event_cb_func 
(
  cm_ph_event_e_type         event,            /* Event ID              */
  const cm_ph_info_s_type   *event_ptr         /* Pointer to Event info */
);

boolean cmif_is_gw_subscription_available
(
  sys_modem_as_id_e_type     subs_id,
  ds_at_cm_ph_event_type    *ph_event
);

LOCAL dsat_result_enum_type cmif_process_cm_reg_data_ex
(
  const ds_at_cm_ss_event_type * event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id
);

#ifdef FEATURE_SGLTE
LOCAL void cmif_process_cm_hybr_reg_data_ex
(
  const ds_at_cm_ss_event_type *event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type          stack_id,
  sys_modem_as_id_e_type        subs_id
);
#endif /* FEATURE_SGLTE */

LOCAL void dsat_set_net_sel_mode_from_nv
(
  sys_modem_as_id_e_type     subs_id
);

LOCAL void cmif_process_emerg_list_ex
(
  ds_at_ss_info_s_type   *event_info_ptr,        /* DS event  info pointer */
  sys_modem_as_id_e_type  subs_id
);

LOCAL void cmif_report_mode_ex
(
  sys_srv_status_e_type     srv_status,
  sys_srv_domain_e_type     srv_domain,
  sys_sys_mode_e_type       sys_mode,
  sys_modem_as_id_e_type    subs_id
);

#ifdef FEATURE_DSAT_EXTENDED_CMD
LOCAL void cmif_report_mode_3gpp2_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
);
#endif /* FEATURE_DSAT_EXTENDED_CMD */

LOCAL void cmif_process_rssi_info_3gpp
(
  const ds_at_cm_ss_event_type *event_ptr, /* DS event pointer */
  dsat_stack_id_e_type          stack_id,
  sys_modem_as_id_e_type        subs_id
);

LOCAL void cmif_process_rssi_info_3gpp2
(
  ds_at_ss_info_s_type *ss_info_ptr
);

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
void cmif_process_cqi_info_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
);
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
#endif /* FEATURE_DSAT_ETSI_MODE */

LOCAL void cmif_setup_msg_watermarks( void );
LOCAL void cmif_register_cb_func_ex ( void );

#ifdef FEATURE_DSM_WM_CB
LOCAL void cmif_cm_msg_wm_hiwater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
LOCAL void cmif_cm_msg_wm_lowater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
LOCAL void cmif_cm_msg_wm_non_empty_cb
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
#else
LOCAL void cmif_cm_msg_wm_hiwater_cb ( void );
LOCAL void cmif_cm_msg_wm_lowater_cb ( void );
LOCAL void cmif_cm_msg_wm_non_empty_cb ( void );
#endif /* FEATURE_DSM_WM_CB */

LOCAL void cmif_start_s_seven_timer
( 
  sys_modem_as_id_e_type  subs_id,
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         call_id,
  cm_client_type_e_type   client_id
);
LOCAL void cmif_stop_s_seven_timer
( 
  sys_modem_as_id_e_type subs_id,
  ds_profile_num_type    profile_id,
  cm_call_type_e_type    call_type,
  cm_call_id_type        call_id,
  cm_client_type_e_type  client_id
);

LOCAL void cmif_cm_dual_standby_sub_info_cmd_handler
(
  ds3g_cm_sub_info_type *pref_event_ptr
);

/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type            cmd,              /* Command ID            */
  cm_call_cmd_err_e_type        cmd_err           /* Command error code    */
)
{
  ds_cmd_type            *cmd_buf;
  ds_at_cm_call_cmd_type *call_cmd = NULL;

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cm_call_cmd_type), FALSE);

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_CMD;
  call_cmd = cmd_buf->cmd_payload_ptr;
  call_cmd->cmd                    = cmd;
  call_cmd->cmd_err                = cmd_err;
  call_cmd->data_ptr               = data_ptr;
  ds_put_cmd(cmd_buf);
}  /* dsatcmif_call_cmd_cb_func */



/*===========================================================================

FUNCTION DSATCMIF_END_CALLS

DESCRIPTION
  This function issues a request to CM to end call(s).
  Num_call_ids should specify thee size of the call id array pointed to by
  call_id_ptr.

DEPENDENCIES

RETURN VALUE
  TRUE:     The end request is sent, callback function
              will be called.
  FALSE:    The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
boolean dsatcmif_end_calls( 
  const uint8 num_call_ids, 
  const cm_call_id_type * call_id_ptr 
)
{
  /* Hangup the call */
  boolean                 call_status;
  cm_end_params_s_type    *end_params_ptr = NULL;
  uint8 index;
  sys_modem_as_id_e_type   subs_id = dsat_get_current_subs_id(FALSE);  

  ASSERT( call_id_ptr != NULL );

  if ( num_call_ids > CM_CALL_ID_MAX )
  {
    return FALSE;
  }
  end_params_ptr = (cm_end_params_s_type *)dsat_alloc_memory(num_call_ids * sizeof(cm_end_params_s_type),FALSE);

  for ( index = 0; index < num_call_ids; index++ )
  {
    memset( &end_params_ptr[index],
            CM_CALL_CMD_PARAM_DEFAULT_VALUE,
            sizeof ( cm_end_params_s_type ));

    end_params_ptr[index].info_type = voice_state[subs_id][call_id_ptr[index]].call_mode;

#ifdef FEATURE_LTE_TO_1X
    if(is_csfb_call_active == TRUE)
    {
      end_params_ptr[index].info_type = CM_CALL_MODE_INFO_CDMA;
    }
#endif /* FEATURE_LTE_TO_1X */
    end_params_ptr[index].call_id = call_id_ptr[index];
  }

  call_status = cm_mm_call_cmd_end(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   num_call_ids,
                                   end_params_ptr);

  dsatutil_free_memory((void *)end_params_ptr);
  return call_status;
} /* dsatcmif_end_calls */

/*===========================================================================

FUNCTION DSATCMIF_ANSWER_VOICE_CALL

DESCRIPTION
  This command answer a voice call by calling cm_call_cmd_answer

DEPENDENCIES

RETURN VALUE
  TRUE : If the command succeeds.
  FALSE: If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_answer_voice_call
(
  sys_modem_as_id_e_type   subs_id,
  uint8                    call_id_ptr,
  boolean                  answer_call,
  cm_call_mode_info_e_type call_mode
)
{
  boolean                 call_status;
  cm_ans_params_s_type    *answer_params_ptr = NULL;

  answer_params_ptr = (cm_ans_params_s_type *)dsat_alloc_memory(sizeof(cm_ans_params_s_type),FALSE);
  memset( (void *)answer_params_ptr, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));
  
if( call_mode == CM_CALL_MODE_INFO_IP )
  {
    answer_params_ptr->info_type = CM_CALL_MODE_INFO_IP;
    answer_params_ptr->ans_params.ip_ans.accept = answer_call;
    answer_params_ptr->ans_params.ip_ans.call_reject_reason = answer_call ? 
                                                         CM_CALL_END_NONE : 
                                                         CM_CALL_END_SIP_486_BUSY_HERE;
    answer_params_ptr->ans_params.ip_ans.call_type = CM_CALL_TYPE_VOICE;
  }
  else if (call_mode == CM_CALL_MODE_INFO_CDMA)
  {
    answer_params_ptr->info_type = CM_CALL_MODE_INFO_CDMA;
    answer_params_ptr->ans_params.cdma_ans.call_type = CM_CALL_TYPE_VOICE;
  }
  else
  {
    answer_params_ptr->info_type = CM_CALL_MODE_INFO_GW_CS;
    answer_params_ptr->ans_params.gw_cs_ans.accept = answer_call;
    answer_params_ptr->ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_VOICE;
  }

  call_status = cm_mm_call_cmd_answer(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   call_id_ptr,
                                   answer_params_ptr
                                   );
  dsatutil_free_memory((void*)answer_params_ptr);
  return call_status;
} /* dsatcmif_answer_voice_call */

/*===========================================================================

FUNCTION  DSAT_UPDATE_CALL_INFO_GLOBAL

DESCRIPTION
  This function update call related global variables for the CM call-related 
  events.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat_update_call_info_global
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
)
{
  dsati_mode_e_type current_mode;
  cm_call_event_e_type      event;
  ds_at_call_info_s_type   *event_ptr =NULL;
  dsatetsicall_call_state_da_info    *call_da_val = NULL;

  event_ptr = &call_event->event_info;
  event     =  call_event->event;     
  current_mode = dsatcmdp_get_current_mode_per_subs(event_ptr->subs_id);
  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, TRUE);
  if (NULL == call_da_val)
  {
    return ;
  }

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#ifdef FEATURE_DSAT_ETSI_MODE
  if( event == CM_CALL_EVENT_SETUP_IND )
#else
  if( event == CM_CALL_EVENT_CALLER_ID)
#endif /* FEATURE_DSAT_ETSI_MODE */
  {
    /*back up current incoming call info number*/
    if(event_ptr->num.len != 0)
    {
      (void) dsatutil_memscpy((void*)&call_da_val->dsat_cm_curr_incoming_call_num,
              sizeof(cm_num_s_type),(void*)&event_ptr->num,sizeof(cm_num_s_type));     
    }
  }
  else if ( event == CM_CALL_EVENT_END )
  {
    /*reset the call info number when call ended*/
    memset(&call_da_val->dsat_cm_curr_incoming_call_num, 0,sizeof(cm_num_s_type));
  }
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

  if ( dsatcmif_is_voice_call_type(event_ptr->call_type)
      /* outgoing packet data calls(ETSI only), */
#ifdef FEATURE_DSAT_ETSI_DATA
      || ((CM_CALL_TYPE_PS_DATA == event_ptr->call_type) &&
          (CM_CALL_STATE_INCOM != event_ptr->call_state))
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
       /* Incoming, connected or end events for CS data calls */
      || (CM_CALL_TYPE_CS_DATA == event_ptr->call_type &&
         (CM_CALL_STATE_INCOM == event_ptr->call_state ||
          CM_CALL_STATE_CONV  == event_ptr->call_state ||
          CM_CALL_STATE_IDLE  == event_ptr->call_state ) )

       /* CM_CALL_EVENT_MNG_CALLS_CONF
          For ATCOP originated events: 
            Need to check success and update state or give CME ERROR.
          For other client:
            Need to update held calls info as no other events are posted. */
      || event == CM_CALL_EVENT_MNG_CALLS_CONF
      || event == CM_CALL_EVENT_PROGRESS_INFO_IND
      || event == CM_CALL_EVENT_CALL_IS_WAITING
      || event == CM_CALL_EVENT_CALL_BARRED
      || event == CM_CALL_EVENT_CALL_FORWARDED
      || event == CM_CALL_EVENT_CALL_BEING_FORWARDED
      || event == CM_CALL_EVENT_INCOM_FWD_CALL
      || event == CM_CALL_EVENT_CALL_ON_HOLD 
      || event == CM_CALL_EVENT_CALL_RETRIEVED
      || event == CM_CALL_EVENT_ORIG_FWD_STATUS
      || event == CM_CALL_EVENT_ORIG
      || event == CM_CALL_EVENT_ANSWER
#endif /* FEATURE_DSAT_ETSI_MODE */
      || event == CM_CALL_EVENT_INFO
#ifdef FEATURE_DATA_WCDMA_PS
      /* RAB events to support WCDMA-mode $QCDGEN */
      || event == CM_CALL_EVENT_RAB_REESTAB_IND 
#endif /* FEATURE_DATA_WCDMA_PS */
#if defined(FEATURE_HS_USB_SER3_PORT)
      || (CM_CALL_EVENT_CONNECT == event && 
          CM_CALL_TYPE_PS_DATA == event_ptr->call_type)
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
     )
  {
    DS_AT_MSG3_HIGH("ATCOP: cm event cb, event: %d, type: %d, state: %d",
            event, event_ptr->call_type, event_ptr->call_state);
    if( NULL != event_ptr )
    {
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
#ifdef FEATURE_DSAT_ETSI_DATA
        if(CM_CALL_EVENT_END == event)
        {
          call_da_val->dsat_pdp_deactivation_cause = event_ptr->end_status;
        }
#endif /* FEATURE_DSAT_ETSI_DATA */
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
  }
  #ifdef FEATURE_DSAT_ETSI_MODE
  /* Capture call end status */
  if( IS_ETSI_MODE(current_mode) &&
      (
	(CM_CALL_EVENT_END == event) ||
        (CM_CALL_EVENT_CALL_CONF == event) ||
        (CM_CALL_EVENT_PDN_CONN_REJ_IND == event) ||
        (CM_CALL_EVENT_PDN_CONN_FAIL_IND == event)
      ) 
    )
  {
    dsatetsicmif_capture_call_end_status( subs_id, event, event_ptr );
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
}/* dsat_update_call_info_global */


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :       if there was any problem in executing the command
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_call_event_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type            result = DSAT_ASYNC_EVENT;
  dsati_mode_e_type                current_mode;
  ds_at_cm_call_event_type        *call_event = dsat_get_cmd_payload_ptr(cmd_ptr);
  dsatetsicall_call_state_da_info *call_da_val = NULL;
#ifdef FEATURE_DSAT_ETSI_MODE
  sys_modem_as_id_e_type           subs_id;
#endif /*FEATURE_DSAT_ETSI_MODE*/

  current_mode = dsatcmdp_get_current_mode_per_subs(call_event->event_info.subs_id);

  if ( (call_event->event_info.dsat_info_list_avail == TRUE) &&
       (call_event->event ==  CM_CALL_EVENT_INFO) )
  {
    subs_id = dsat_get_current_subs_id(TRUE);
  }
  else
  {
    subs_id = call_event->event_info.subs_id;

    dsat_update_call_info_global(call_event, subs_id);
  }

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, TRUE);
  if (NULL == call_da_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG2_MED("subs_id = %d, etsicall_was_call_active = %d",
                 subs_id, call_da_val->etsicall_was_call_active);

  switch (call_event->event)
  {
    case CM_CALL_EVENT_INCOM:
      /* Assign a sequence number (as described in 3GPP TS 22.030 
         section 6.5.5.1) to this incoming call */
      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) ||
         (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA)||
         (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number(subs_id,
            call_event->event_info.call_id,
            call_event->event,
            call_event->event_info.call_type);
      }
      /* Maintain call state info: add waiting call id and
         pass event to dsatvoice for RINGing or dsatetsicmif for +CCWA 
         unsolicited result code dependent upon active call or not */
      if (call_event->event_info.dsat_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
      {
        result = dsatvoice_call_event_incom(subs_id, &call_event->event_info);
      }
#ifdef FEATURE_DSAT_ETSI_MODE
      else
      {
        result = dsatetsicall_call_event_incom(call_event ,subs_id);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    case CM_CALL_EVENT_CONNECT:
      /* Reset the S7 timer if running  */
      cmif_stop_s_seven_timer(subs_id,
                              call_event->event_info.profile_number,
                              call_event->event_info.call_type,
                              call_event->event_info.call_id,
                              call_event->event_info.call_client_id);
      /* Assign a sequence number (as described in 3GPP TS 22.030 
              section 6.5.5.1) to this connected call unless already done when
              it was incoming */
      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number(subs_id,
                         call_event->event_info.call_id,
                         call_event->event,
                         call_event->event_info.call_type);
      }  
      /* Check for call cmd originator */
#ifdef FEATURE_DSAT_ETSI_MODE
      /* ETSI builds */
      if ( IS_ETSI_MODE(current_mode) )
      {
        result = dsatetsicall_call_event_connect_ex(call_event , subs_id);
      }
      else
#endif /* FEATURE_DSAT_ETSI_MODE */
      {
      /* CDMA-only builds */
        result = dsatvoice_call_event_connect(subs_id,
                   &call_event->event_info);
      }
#if defined(FEATURE_HS_USB_SER3_PORT)
      if (CM_CALL_TYPE_PS_DATA ==
            call_event->event_info.call_type)
      {
        dsatcmif_servs_state_ss_info  *ph_ss_val = NULL;
        ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
        if (NULL == ph_ss_val)
        {
           return DSAT_ASYNC_EVENT;
        }
        ph_ss_val->dsatcmif_dormant_ind_report_enabled = TRUE;
        DS_AT_MSG0_MED("cm call connected");
      }
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
        break;

    case CM_CALL_EVENT_PROGRESS_INFO_IND:
      /* The MO (originating) call moved to alerting state, 
         started ringing on the other side */
      dsat_mo_dailing_state_set(subs_id, FALSE);
#ifdef FEATURE_DSAT_ETSI_MODE 
      /* Update +CIND callsetup indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_CALLSETUP,
                                               DSAT_CIND_CSETUP_MORING, subs_id );
      break;
#endif /* FEATURE_DSAT_ETSI_MODE */

    case CM_CALL_EVENT_END:
      /* As call got ended make dsat_mo_dailing_state true */
      dsat_mo_dailing_state_set(subs_id, TRUE);
      /* Reset the S7 timer if running  */
      cmif_stop_s_seven_timer(subs_id,
                              call_event->event_info.profile_number,
                              call_event->event_info.call_type,
                              call_event->event_info.call_id,
                              call_event->event_info.call_client_id);

      /* Free call sequence number (as described in 3GPP TS 22.030 
               section 6.5.5.1) of this ending call */
      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number( subs_id,
                         call_event->event_info.call_id,
                         call_event->event,
                         call_event->event_info.call_type);
      }
#ifdef FEATURE_DSAT_ETSI_MODE 
      if ( IS_ETSI_MODE(current_mode) )
      {
        result = dsatetsicall_call_event_end_ex(call_event, subs_id);
      }
      else
#endif /* FEATURE_DSAT_ETSI_MODE */
      {
        result = dsatvoice_call_event_end(subs_id,
                                          &call_event->event_info);
      }
      break;

#ifdef FEATURE_DSAT_ETSI_MODE
    case CM_CALL_EVENT_MNG_CALLS_CONF:
      /* Reset the S7 timer if running  */
      cmif_stop_s_seven_timer(subs_id,
                              call_event->event_info.profile_number,
                              call_event->event_info.call_type,
                              call_event->event_info.call_id,
                              call_event->event_info.call_client_id);

      result = dsatetsicall_call_event_mng_calls_conf_ex(call_event, subs_id);
      break;
#endif /* FEATURE_DSAT_ETSI_MODE */
    case CM_CALL_EVENT_ANSWER:
    case CM_CALL_EVENT_ORIG:
      {
        /* Start the S7 timer iff it is ATCoP originated/answered Call */
        if( (dsat_num_item_type)dsatutil_get_val(
                                    DSAT_SREG_S7_IDX,0,0,NUM_TYPE) > 0)
        {
          cmif_start_s_seven_timer(subs_id,
                                   call_event->event_info.profile_number,
                                   call_event->event_info.call_type,
                                   call_event->event_info.call_id,
                                   call_event->event_info.call_client_id);
        }
        if( call_event->event == CM_CALL_EVENT_ORIG )
        {
          result = dsatvoice_call_event_orig(subs_id,
                                             &call_event->event_info);
        }
      }
      break;

    case CM_CALL_EVENT_SETUP_IND:

      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) ||
         (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA)||
         (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {

        dsatact_track_sequence_number(subs_id,
          call_event->event_info.call_id,
          call_event->event,
          call_event->event_info.call_type);
      }
      break;

    case CM_CALL_EVENT_ACT_BEARER_IND:
    case CM_CALL_EVENT_PDN_CONN_REJ_IND:
    case CM_CALL_EVENT_PDN_CONN_FAIL_IND:
    case CM_CALL_EVENT_RES_ALLOC_REJ_IND:
    case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:
      if(call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA)
      {
        dsatact_track_sequence_number(subs_id,
          call_event->event_info.call_id,
          call_event->event,
          call_event->event_info.call_type);
      }
      break;

    case CM_CALL_EVENT_CALL_CONF:
      /* Do nothing, event info captured in callback */
      break;

#ifdef FEATURE_DATA_WCDMA_PS
    case CM_CALL_EVENT_RAB_REESTAB_IND:
      if((call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA) &&
         (call_event->event_info.sys_mode == SYS_SYS_MODE_LTE))
      {
        dsatact_track_sequence_number(subs_id,
          call_event->event_info.call_id,
          call_event->event,
          call_event->event_info.call_type);
      }
      break;
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_DUAL_ACTIVE
    case CM_CALL_EVENT_LOCAL_HOLD:
      if ( IS_ETSI_MODE(current_mode) )
      {
        dsatetsicall_call_event_local_hold(call_event, subs_id);
      }

      dsatvoice_call_event_local_hold(subs_id, &call_event->event_info);
      break;
#endif /* FEATURE_DUAL_ACTIVE */
    case CM_CALL_EVENT_HO_COMPLETE:
    case CM_CALL_EVENT_SRVCC_COMPLETE_IND:
      {
        result = dsatvoice_call_event_ho_complete(subs_id,&call_event->event_info);
      }
      break;
    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_call_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_call_event_handler_fp(call_event, result, subs_id );
  }

  return result;
} /* dsatcmif_cm_call_event_handler_ex */


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related commands

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
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_call_cmd_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type   result = DSAT_NO_RSP;
  ds_at_cm_call_cmd_type *call_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  sys_modem_as_id_e_type  subs_id = dsat_get_current_subs_id(TRUE);

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    return DSAT_ERROR;
  }

  DS_AT_MSG3_HIGH("Call cmd handler cmd_err %d [Applicable for only voice] cause %d alpha len %d",
            call_cmd->cmd_err, call_cmd->err_cause, call_cmd->alpha.len);

  switch (call_cmd->cmd)
  {
    case CM_CALL_CMD_ANSWER:
      result = dsatvoice_cmd_answer_cb_handler(subs_id,
               call_cmd->cmd_err);
      break;

    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp(call_cmd, result);
  }

  return result;
} /* dsatcmif_cm_call_cmd_handler_ex */

/*===========================================================================

FUNCTION   CMIF_DETERMINE_SUBS_MODE

DESCRIPTION
  This function determines mode of subscription in a SS event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_subs_mode_e_type cmif_determine_subs_mode
(
  ds_at_cm_ss_event_type *event_ptr
)
{
  dsat_subs_mode_e_type subs_mode = DSAT_SUB_INACTIVE;

  if(event_ptr->number_of_stacks == 2)
  {

   /*NO Service + NO Service
    Cleared all stored information */
    if((event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_NO_SRV) &&
       (event_ptr->event_info[DSAT_STACK_HYBR].sys_mode == SYS_SYS_MODE_NO_SRV))
    {
      subs_mode = DSAT_SUB_NO_SRV;
    }
  /*
    **SGLTE Cases**
     LTE + G
     LTE + No Service
     No Service + G
 */
    else if((event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_LTE) ||
     (event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_TDS) ||
     (event_ptr->event_info[DSAT_STACK_HYBR].sys_mode == SYS_SYS_MODE_GSM))
   {
     subs_mode = DSAT_SUB_SGLTE;
   }
    /*
    **SVLTE Cases**
    1X + LTE
    1X + No Service 
    NO Service + LTE
    */
    else if(((event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_CDMA) ||
        (event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_NO_SRV)) &&
       ((event_ptr->event_info[DSAT_STACK_HYBR].sys_mode == SYS_SYS_MODE_LTE) ||
        (event_ptr->event_info[DSAT_STACK_HYBR].sys_mode  == SYS_SYS_MODE_NO_SRV)))
    {
      subs_mode = DSAT_SUB_SVLTE;
    }
    
/*
    **SVDO Cases**
    1X + HDR
    1X + No Service
    NO Service + HDR
*/
    else if(((event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_CDMA) ||
        (event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_NO_SRV)) &&
       ((event_ptr->event_info[DSAT_STACK_HYBR].sys_mode == SYS_SYS_MODE_HDR) ||
        (event_ptr->event_info[DSAT_STACK_HYBR].sys_mode  == SYS_SYS_MODE_NO_SRV)))
    {
      subs_mode = DSAT_SUB_SVDO;
    }
    
  }
  else
  {
    subs_mode = DSAT_SUB_STANDALONE;
  }

  DS_AT_MSG5_HIGH("is_main_operational: %d is_hybr_operational: %d Subs Mode: %d Main sys mode %d Hybr sys mode %d", 
    event_ptr->event_info[DSAT_STACK_MAIN].is_operational, event_ptr->event_info[DSAT_STACK_HYBR].is_operational, subs_mode,
    event_ptr->event_info[DSAT_STACK_MAIN].sys_mode,event_ptr->event_info[DSAT_STACK_HYBR].sys_mode);

  return subs_mode;
  
}/*cmif_determine_subs_mode*/

/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM serving system
  related events

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
dsat_result_enum_type dsatcmif_cm_ss_event_handler_ex
(
  dsat_cm_msg_s_type *msg_ptr
)
{
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  dsat_result_enum_type         result_tmp = DSAT_ASYNC_EVENT;
  ds_at_cm_ss_event_type       *event_ptr = NULL;
  dsat_num_item_type            rssi_enqueue = 0;
  dsat_stack_id_e_type          stack_id;
  sys_modem_as_id_e_type        subs_id;
  dsat_subs_mode_e_type         subs_mode;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  if (   (NULL == msg_ptr) 
       ||(msg_ptr->msg.ss_event.number_of_stacks < 1) 
       ||(msg_ptr->msg.ss_event.number_of_stacks > 2)
     )
  {
    DS_AT_MSG0_ERROR("Either msg ptr is NULL or No of Stack is 0: Ignored SS event processing");
    return result;
  }

  /*Assign  SS event info  pointer*/
  event_ptr = &msg_ptr->msg.ss_event;
  /* Calculate subs_id */
  subs_id = event_ptr->subs;

  DS_AT_MSG3_HIGH("Processing CM SS event: %d ATCoP Subs ID %d CM Subs ID %d",
          event_ptr->event,subs_id,event_ptr->subs);

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    DS_AT_MSG1_ERROR("Invalid subs ID %d", subs_id);
    return DSAT_ASYNC_EVENT;
  }
  
  /* Calculate subscription mode  */
  subs_mode = cmif_determine_subs_mode(event_ptr);
  if(subs_mode == DSAT_SUB_SGLTE)
  {
    dsat_subs_info[subs_id].is_sglte_sub = TRUE;
  }

  switch (event_ptr->event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    {
      for ( stack_id = DSAT_STACK_MAIN; stack_id < event_ptr->number_of_stacks; stack_id++ )
      {
        if(DSAT_STACK_MAIN == stack_id)
        { 
             switch(event_ptr->event_info[stack_id].sys_mode)
            {
              /*Process 3GPP information*/
              case SYS_SYS_MODE_NO_SRV:
              case SYS_SYS_MODE_GSM:
              case SYS_SYS_MODE_WCDMA:
              case SYS_SYS_MODE_LTE:
              case SYS_SYS_MODE_TDS:
              case SYS_SYS_MODE_GW:
              case SYS_SYS_MODE_GWL:
                if ( ( event_ptr->event_info[stack_id].changed_fields & 
                        (DSAT_SS_SRV_CHG_MASK 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK 
                         | DSAT_SS_CELL_INFO_MASK 
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
                        )) || 
                      ( DSAT_CM_REQ_ID == event_ptr->event_info[stack_id].cm_req_id ) )
                {
                  /*Process CM registration information */
                  result_tmp = cmif_process_cm_reg_data_ex(event_ptr, stack_id,subs_id);
                }
                if ( event_ptr->event_info[stack_id].changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                {
                  (void)dsatetsicmif_process_cm_mm_info_data_ex( &event_ptr->event_info[stack_id].mm_info,
                  	                                           &event_ptr->event_info[stack_id], subs_id);
                }
              break;
              /*Undefined system mode for ATCoP*/
              default:
              break;
            }
        }	
        else
        {
           switch(subs_mode)
           {
#ifdef FEATURE_SGLTE
              case DSAT_SUB_SGLTE:
                if ( ( event_ptr->event_info[stack_id].changed_fields & 
                      (DSAT_SS_SRV_CHG_MASK 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK 
                       | DSAT_SS_CELL_INFO_MASK 
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
                       )) || 
                    ( DSAT_CM_REQ_ID == event_ptr->event_info[stack_id].cm_req_id ) )
                {
                 cmif_process_cm_hybr_reg_data_ex(event_ptr, stack_id, dsatcmif_get_sglte_hybr_subs_id());
                }
                if ( event_ptr->event_info[stack_id].changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                {
                   dsatetsicmif_process_cm_mm_info_net_reg(
                              &event_ptr->event_info[stack_id].mm_info, dsatcmif_get_sglte_hybr_subs_id());

                }
                break;
#endif /* FEATURE_SGLTE */
              case DSAT_SUB_SVLTE:
                 if ( ( event_ptr->event_info[stack_id].changed_fields & 
                      (DSAT_SS_SRV_CHG_MASK 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK 
                       | DSAT_SS_CELL_INFO_MASK 
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
                       )) || 
                      ( DSAT_CM_REQ_ID == event_ptr->event_info[stack_id].cm_req_id ) )
                 {
                   /*Process LTE information */
                   result_tmp = cmif_process_cm_reg_data_ex(event_ptr, stack_id,
                                                          subs_id);
                 }
                 if ( event_ptr->event_info[stack_id].changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                 {
                   (void)dsatetsicmif_process_cm_mm_info_data_ex( &event_ptr->event_info[stack_id].mm_info,
                   	                                           &event_ptr->event_info[stack_id],subs_id);
                 }
                 break;
              case DSAT_SUB_NO_SRV:
                 if ( ( event_ptr->event_info[stack_id].changed_fields & 
                      (DSAT_SS_SRV_CHG_MASK 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK 
                       | DSAT_SS_CELL_INFO_MASK 
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
                       )) || 
                      ( DSAT_CM_REQ_ID == event_ptr->event_info[stack_id].cm_req_id ) )
                 {
#ifdef FEATURE_SGLTE
                    /* Keep GSM information at MAX -1 index for SGLTE*/
                    cmif_process_cm_hybr_reg_data_ex(event_ptr, stack_id, dsatcmif_get_sglte_hybr_subs_id());
#endif /* FEATURE_SGLTE */
                 }
                 if ( event_ptr->event_info[stack_id].changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                 {
#ifdef FEATURE_SGLTE
                     dsatetsicmif_process_cm_mm_info_net_reg(&event_ptr->event_info[stack_id].mm_info, dsatcmif_get_sglte_hybr_subs_id());
#endif /* FEATURE_SGLTE */
                 }
                 break;
              default:
                 break;
              }
        }
           /*check the result from both the stack */
        if (DSAT_ASYNC_EVENT != result_tmp)
        {
          result = result_tmp;
        }
      }

      switch(event_ptr->event_info[DSAT_STACK_MAIN].sys_mode)
      {
        /*Process 3GPP2 information*/
        case SYS_SYS_MODE_CDMA:
        case SYS_SYS_MODE_HDR:
          if ( event_ptr->event_info[DSAT_STACK_MAIN].changed_fields &
               ( DSAT_SS_SYS_MODE_MASK |
                 DSAT_SS_HDR_SRV_STATUS_MASK ))
          {
            ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,SYS_MODEM_AS_ID_1, TRUE);
            if (NULL != ph_ss_ms_val)
            {
               ph_ss_ms_val->dsat_net_reg_state.sys_mode = event_ptr->event_info[DSAT_STACK_MAIN].sys_mode;
            }
#ifdef FEATURE_DSAT_EXTENDED_CMD
            cmif_report_mode_3gpp2_ex(&event_ptr->event_info[DSAT_STACK_MAIN]);
#endif /* FEATURE_DSAT_EXTENDED_CMD */
          }
          break;
        case SYS_SYS_MODE_NO_SRV:
          if ( (event_ptr->event_info[DSAT_STACK_MAIN].changed_fields &
               ( DSAT_SS_SYS_MODE_MASK |
                 DSAT_SS_HDR_SRV_STATUS_MASK )) && (event_ptr->subs == dsat_get_current_1x_subs_id()) )
          {
            ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,SYS_MODEM_AS_ID_1, TRUE);
            if (NULL != ph_ss_ms_val)
            {
              ph_ss_ms_val->dsat_net_reg_state.sys_mode = event_ptr->event_info[DSAT_STACK_MAIN].sys_mode;
            }
#ifdef FEATURE_DSAT_EXTENDED_CMD
            cmif_report_mode_3gpp2_ex(&event_ptr->event_info[DSAT_STACK_MAIN]);
#endif /* FEATURE_DSAT_EXTENDED_CMD */
          }
          break;

          default:
            break;
      }
      
#ifdef FEATURE_LTE_TO_1X
     if ( event_ptr->event_info[DSAT_STACK_MAIN].changed_fields &
          DSAT_SS_CSFB_CALL_ACTIVE_MASK )
     {
        is_csfb_call_active = event_ptr->event_info[DSAT_STACK_MAIN].is_csfb_call_active;
        DS_AT_MSG1_HIGH("voice_support_on_lte updated %d",is_csfb_call_active);
     }
#endif /* FEATURE_LTE_TO_1X */

#if defined(FEATURE_DSAT_SP_CMDS)
     cmif_determine_spservice(&event_ptr->event_info[DSAT_STACK_MAIN]);
#endif /* defined(FEATURE_DSAT_SP_CMDS) */
    }
    break;

    case CM_SS_EVENT_RSSI:
    {
      rssi_enqueue = DSAT_RSSI;

      /* Pre process RSSI data */
      for ( stack_id = DSAT_STACK_MAIN; stack_id < event_ptr->number_of_stacks; stack_id++ )
      {
        if(DSAT_STACK_MAIN == stack_id)
        {
          switch(event_ptr->event_info[stack_id].sys_mode)
          {
            case SYS_SYS_MODE_NO_SRV:
            case SYS_SYS_MODE_GSM:
            case SYS_SYS_MODE_WCDMA:
            case SYS_SYS_MODE_GW:
            case SYS_SYS_MODE_LTE:
            case SYS_SYS_MODE_TDS:
              cmif_process_rssi_info_3gpp(event_ptr, stack_id, subs_id);
            break;
           default:
             break;
          }
        }
        else
        {
          if(subs_mode == DSAT_SUB_SVLTE)
          {
            cmif_process_rssi_info_3gpp(event_ptr, stack_id, subs_id);
          }
        }
      }
      switch(event_ptr->event_info[DSAT_STACK_MAIN].sys_mode)
      {
        case SYS_SYS_MODE_NO_SRV:
          if(event_ptr->subs == dsat_get_current_1x_subs_id())
          {
            cmif_process_rssi_info_3gpp2(&event_ptr->event_info[DSAT_STACK_MAIN]);
          }
          break;
        case SYS_SYS_MODE_CDMA:
        case SYS_SYS_MODE_HDR:
          cmif_process_rssi_info_3gpp2(&event_ptr->event_info[DSAT_STACK_MAIN]);
          break;
        default:
          break;
      }

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
     /*Command is not DSDS compliant:-  Support for  Primary SUBS only*/
     if(event_ptr->subs == SYS_MODEM_AS_ID_1 )
     {
       cmif_process_cqi_info_ex(&event_ptr->event_info[DSAT_STACK_MAIN]);
     }
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT */
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
    break;

    case CM_SS_EVENT_HDR_RSSI:
    {
      rssi_enqueue = DSAT_HDR_RSSI;

      cmif_process_rssi_info_3gpp2(&event_ptr->event_info[DSAT_STACK_MAIN]);
    }
    break;

    case CM_SS_EVENT_EMERG_NUM_LIST:
    {
      /* We support only main stack emergency list :TBD  Hybrid stack support*/
      event_ptr->event_info[DSAT_STACK_MAIN].report_emerg_num_list = FALSE;

      if (NULL != event_ptr->event_info[DSAT_STACK_MAIN].emerg_num_list)
      {
        /*  +CEN command is 3GPP specific command */
        if (!(event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_CDMA || 
                 event_ptr->event_info[DSAT_STACK_MAIN].sys_mode == SYS_SYS_MODE_HDR))
        {
          cmif_process_emerg_list_ex(&event_ptr->event_info[DSAT_STACK_MAIN], subs_id);
        }
        dsatutil_free_memory(event_ptr->event_info[DSAT_STACK_MAIN].emerg_num_list);
      }
    }
    break;

    case CM_SS_EVENT_REG_REJECT:
    {
      for ( stack_id = DSAT_STACK_MAIN; stack_id < event_ptr->number_of_stacks; stack_id++ )
      {
        if(DSAT_STACK_MAIN == stack_id)
        {
          switch(event_ptr->event_info[stack_id].sys_mode)
          {
            case SYS_SYS_MODE_NO_SRV:
            case SYS_SYS_MODE_GSM:
            case SYS_SYS_MODE_WCDMA:
            case SYS_SYS_MODE_GW:
            case SYS_SYS_MODE_LTE:
            case SYS_SYS_MODE_TDS:
              dsatetsicall_creg_state_machine( event_ptr, stack_id,subs_id);
            break;
           default:
             break;
          }
        }
        else
        {
          if(subs_mode == DSAT_SUB_SGLTE)
          {
#ifdef FEATURE_SGLTE
            subs_id = dsatcmif_get_sglte_hybr_subs_id();
            dsatetsicall_creg_state_machine( event_ptr, stack_id, subs_id );
#endif /*FEATURE_SGLTE*/
          }
          else if(subs_mode == DSAT_SUB_SVLTE)
          {
            dsatetsicall_creg_state_machine( event_ptr, stack_id, subs_id );
          }
          else if(subs_mode == DSAT_SUB_NO_SRV )
          {
            dsatetsicall_creg_state_machine( event_ptr, stack_id, subs_id );
#ifdef FEATURE_SGLTE
            subs_id = dsatcmif_get_sglte_hybr_subs_id();
            dsatetsicall_creg_state_machine( event_ptr, stack_id, subs_id );
#endif /* FEATURE_SGLTE */
          }
        }
      }
    }
    break;

    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp(msg_ptr, result,
                                                           subs_id, subs_mode);
  }

  if ( rssi_enqueue )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    dsat_rssi_enqueue = dsat_rssi_enqueue & ~rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/
  }

  return result;
} /* dsatcmif_cm_ss_event_handler_ex */

/*===========================================================================

FUNCTION   CMIF_MAP_SS_CHANGED_FIELDS

DESCRIPTION
  This function maps CM SS changed fields to ATCoP specific changed fields.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint16 cmif_map_ss_changed_fields
(
  uint64                            cm_changed_fields_per_sub,
  uint64                            cm_changed_fields1,
  uint64                            cm_changed_fields2
)
{
  uint16 dsat_changed_fields = 0;
  uint8  idx;
  const dsatcmif_ss_mask_map_s_type *ss_mask_map;
  size_t                      ss_mask_map_size;

  DS_AT_MSG3_HIGH("CM change fields 0x%X 0x%X 0x%X",cm_changed_fields_per_sub,cm_changed_fields1,cm_changed_fields2);
/*
 Convert and Store  Subs  level changes field 
*/
    ss_mask_map       = &dsatcmif_ss_subs_mask_map[0];
    ss_mask_map_size  = ARR_SIZE( dsatcmif_ss_subs_mask_map );
                 
  for (idx = 0; idx < ss_mask_map_size; idx++)
  {
    if (cm_changed_fields_per_sub & ss_mask_map[idx].cm_ss_mask)
    {
      dsat_changed_fields |= ss_mask_map[idx].dsat_ss_mask;
    }
  }
  
  /*
     Convert and Store per Stack changes field1 
  */
    ss_mask_map       = &dsatcmif_ss_stack_mask_map[0];
    ss_mask_map_size  = ARR_SIZE( dsatcmif_ss_stack_mask_map );
 
    for (idx = 0; idx < ss_mask_map_size; idx++)
    {
      if (cm_changed_fields1 & ss_mask_map[idx].cm_ss_mask)
      {
        dsat_changed_fields |= ss_mask_map[idx].dsat_ss_mask;
      }
    }
    
/*
    Convert and Store per Stack changes field2 -We are not interested right Now.
    New chnage field will be added in change field 2 as change field one got exhausted 
 */
  
  return dsat_changed_fields;
} /* cmif_map_ss_changed_fields */

/*===========================================================================
FUNCTION  CMIF_SS_EVENT_MSIM_CB_FUNC

DESCRIPTION
  CM serving system status event callback function for multi-sim targets.
  CM sends SS_events per subscription 
  Single subscription can have mutiple stacks.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/*ARGSUSED*/
LOCAL void cmif_ss_event_msim_cb_func
(
  cm_ss_event_e_type            event,            /* Event ID              */
  const cm_mm_msim_ss_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  boolean               post_ss_event = FALSE;
  dsat_num_item_type    rssi_enqueue = 0;
  dsat_num_item_type    local_dsat_rssi_enqueue;
  boolean               local_cm_msg_wm_full;
  dsat_cm_msg_s_type   *msg_ptr = NULL;
  ds_at_ss_info_s_type *info_ptr = NULL;
  dsm_item_type        *dsm_item_ptr = NULL;
  dsat_stack_id_e_type  stack_id;

  /* Invalid Number of stacks */
  if( NULL == event_ptr ||
      0 == event_ptr->number_of_stacks)
  {
    return;
  }
  
  DS_AT_MSG3_HIGH("In cmif_ss_event_msim_cb_func: event  %d CM Subs ID  %d number of stack %d ",event, event_ptr->asubs_id ,event_ptr->number_of_stacks);
  /* Allocate memory for the message. This will be freed at the end of the function.
     So there is no return until the end of the function. */
  msg_ptr = dsat_alloc_memory(sizeof(dsat_cm_msg_s_type), FALSE);

  msg_ptr->msg.ss_event.number_of_stacks = event_ptr->number_of_stacks;
  msg_ptr->msg.ss_event.subs             = event_ptr->asubs_id;
  
    switch (event)
    {
      case CM_SS_EVENT_SRV_CHANGED:
  /*Unified CM change fields into ATCoP change fields*/
      DS_AT_MSG1_MED("sys_sel_pref_req_id = %d", event_ptr->sys_sel_pref_req_id);
      for( stack_id = DSAT_STACK_MAIN; (stack_id < event_ptr->number_of_stacks) && (stack_id < DSAT_STACK_MAX); stack_id++ )
      {
        info_ptr = &msg_ptr->msg.ss_event.event_info[stack_id];
        info_ptr->changed_fields =
          cmif_map_ss_changed_fields( event_ptr->changed_fields_subs,
                                      event_ptr->stack_info[stack_id].changed_fields,
                                      event_ptr->stack_info[stack_id].changed_fields2 );
           

        if ( ( 0 != info_ptr->changed_fields ) ||
             ( DSAT_CM_REQ_ID == event_ptr->sys_sel_pref_req_id ) )
        {
          post_ss_event = TRUE;
        }
      }
        break;

      case CM_SS_EVENT_RSSI:
        rssi_enqueue = DSAT_RSSI;
        post_ss_event = TRUE;
        break;

      case CM_SS_EVENT_INFO:
        post_ss_event = TRUE;
        break;

      case CM_SS_EVENT_HDR_RSSI:
        rssi_enqueue = DSAT_HDR_RSSI;
        post_ss_event = TRUE;
        break;

      case CM_SS_EVENT_EMERG_NUM_LIST:
        if ( ( event_ptr->stack_info[DSAT_STACK_MAIN].emerg_num_list.num_list_len > 0 ) &&
             ( event_ptr->stack_info[DSAT_STACK_MAIN].emerg_num_list.num_list_len <= CM_MAX_EMERGENCY_NUM_COUNT ) )
        {
          info_ptr = &msg_ptr->msg.ss_event.event_info[DSAT_STACK_MAIN];
          info_ptr->emerg_num_list =
            dsat_alloc_memory(sizeof(cm_emerg_num_list_s_type), TRUE);

          if (NULL != info_ptr->emerg_num_list)
          {
            dsatutil_memscpy(info_ptr->emerg_num_list,
                             sizeof(cm_emerg_num_list_s_type),
                             &event_ptr->stack_info[DSAT_STACK_MAIN].emerg_num_list,
                             sizeof(cm_emerg_num_list_s_type));

            post_ss_event = TRUE;
          }
        }

        break;

      case CM_SS_EVENT_REG_REJECT:
        post_ss_event = TRUE;
        break;

      default:
        break;
    }

  if ( rssi_enqueue )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    local_dsat_rssi_enqueue = dsat_rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

    /* Do not post multiple RSSI events (only one required) */
    if ( local_dsat_rssi_enqueue & rssi_enqueue )
    {
      DS_AT_MSG0_MED("Dropping redundant RSSI event");
      post_ss_event = FALSE;
    }
  }

  if ( TRUE == post_ss_event )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    local_cm_msg_wm_full = dsat_cm_msg_wm_full;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

    /* Ensure there is capacity in message watermark for new one */
    if( FALSE == local_cm_msg_wm_full )
    {
      /* Populate the message from CM event */
      msg_ptr->cmd = DS_CMD_ATCOP_CM_SS_INFO_CMD;
      msg_ptr->msg.ss_event.event = event;

      for(stack_id=DSAT_STACK_MAIN;(stack_id<event_ptr->number_of_stacks) && (stack_id < DSAT_STACK_MAX) ;stack_id++)
      {
        info_ptr = &msg_ptr->msg.ss_event.event_info[stack_id];
        
        info_ptr->is_operational = event_ptr->stack_info[stack_id].is_operational;
        info_ptr->roam_status = event_ptr->stack_info[stack_id].roam_status;
        info_ptr->hdr_roam_status = event_ptr->stack_info[stack_id].roam_status;
        info_ptr->srv_status = event_ptr->stack_info[stack_id].srv_status;
        info_ptr->srv_domain = event_ptr->stack_info[stack_id].srv_domain;
        info_ptr->rssi = event_ptr->stack_info[stack_id].rssi;
        info_ptr->sys_id = event_ptr->stack_info[stack_id].sys_id;

        info_ptr->mm_info.plmn_avail =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.plmn_avail;
        info_ptr->mm_info.univ_time_and_time_zone_avail =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.univ_time_and_time_zone_avail;
        info_ptr->mm_info.time_zone_avail =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.time_zone_avail;
        info_ptr->mm_info.plmn =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.plmn;
        info_ptr->mm_info.univ_time_and_time_zone =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.univ_time_and_time_zone;
        info_ptr->mm_info.time_zone =
          event_ptr->stack_info[stack_id].mode_info.gw_info.mm_information.time_zone;

        info_ptr->sim_state = event_ptr->stack_info[stack_id].mode_info.gw_info.sim_state;
        dsatutil_memscpy(&info_ptr->cell_srv_ind,
                         sizeof(cm_cell_srv_ind_s_type),
                         &event_ptr->stack_info[stack_id].cell_srv_ind,
                         sizeof(cm_cell_srv_ind_s_type));
        
        info_ptr->sys_mode = event_ptr->stack_info[stack_id].sys_mode;
        switch(info_ptr->sys_mode)
        {
          case SYS_SYS_MODE_WCDMA:
            info_ptr->csg_id = event_ptr->stack_info[stack_id].rat_mode_info.wcdma_mode_info.csg_info.csg_id;
          break;
          case SYS_SYS_MODE_LTE:
            info_ptr->csg_id = event_ptr->stack_info[stack_id].rat_mode_info.lte_mode_info.csg_info.csg_id;
          break;
          default:
            info_ptr->csg_id = SYS_CSG_ID_INVALID;
          break;
        }
        info_ptr->hdr_hybrid = event_ptr->stack_info[stack_id].rat_mode_info.hdr_mode_info.hdr_hybrid;
        info_ptr->hdr_srv_status = event_ptr->stack_info[stack_id].srv_status;
        info_ptr->hdr_active_prot = event_ptr->stack_info[stack_id].rat_mode_info.hdr_mode_info.hdr_active_prot;
        
        info_ptr->ps_data_suspend = event_ptr->ps_data_suspend;
        info_ptr->is_sys_forbidden = event_ptr->stack_info[stack_id].is_sys_forbidden;
        info_ptr->srv_capability = event_ptr->stack_info[stack_id].srv_capability;
        info_ptr->cm_req_id = event_ptr->sys_sel_pref_req_id;
#ifdef FEATURE_LTE_TO_1X
        info_ptr->is_csfb_call_active = event_ptr->is_csfb_call_active;
#endif /* FEATURE_LTE_TO_1X */
#ifdef FEATURE_DSAT_LTE
        info_ptr->lte_tac = event_ptr->stack_info[stack_id].rat_mode_info.lte_mode_info.tac;
       info_ptr->lte_rac_or_mme_code = event_ptr->stack_info[stack_id].rat_mode_info.lte_mode_info.rac_or_mme_code;
#endif /* FEATURE_DSAT_LTE */
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
        dsatutil_memscpy(&info_ptr->cell_info,
                         sizeof(sys_cell_info_s_type),
                         &event_ptr->stack_info[stack_id].cell_info,
                         sizeof(sys_cell_info_s_type));
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
        info_ptr->dsatcmif_signal_reporting.rssi = event_ptr->stack_info[stack_id].rssi;
        info_ptr->dsatcmif_signal_reporting.hdr_rssi = event_ptr->stack_info[stack_id].rssi;
        info_ptr->dsatcmif_signal_reporting.rssi2 = event_ptr->stack_info[stack_id].rssi2;
        info_ptr->dsatcmif_signal_reporting.rscp = event_ptr->stack_info[stack_id].rscp;
        info_ptr->dsatcmif_signal_reporting.bit_err_rate = 
          event_ptr->stack_info[stack_id].mode_info.gw_info.bit_err_rate;
#ifdef FEATURE_UMTS_REPORT_ECIO
        info_ptr->dsatcmif_signal_reporting.ecio = event_ptr->stack_info[stack_id].ecio;
#endif /* FEATURE_UMTS_REPORT_ECIO */
#ifdef FEATURE_UMTS_SIR_PATHLOSS
        info_ptr->dsatcmif_signal_reporting.pathloss = event_ptr->stack_info[stack_id].pathloss;
        info_ptr->dsatcmif_signal_reporting.sir = event_ptr->stack_info[stack_id].sir;
#endif /* FEATURE_UMTS_SIR_PATHLOSS */
        info_ptr->dsatcmif_signal_reporting.sys_mode = event_ptr->stack_info[stack_id].sys_mode;
        info_ptr->dsatcmif_signal_reporting.hdr_hybrid = event_ptr->stack_info[stack_id].rat_mode_info.hdr_mode_info.hdr_hybrid;
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
        dsatutil_memscpy(&info_ptr->dsat_cqi_info,
                         sizeof(cm_cqi_info_s_type),
                         &event_ptr->stack_info[stack_id].cqi_info,
                         sizeof(cm_cqi_info_s_type));
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
#endif /* FEATURE_DSAT_ETSI_MODE */
        dsatutil_memscpy(&info_ptr->reg_reject_info,
                         sizeof(cm_reg_reject_info_s_type),
                         &event_ptr->stack_info[stack_id].reg_reject_info,
                         sizeof(cm_reg_reject_info_s_type));
      }
      
      /* Post message to CM watermark */
      dsm_item_ptr = dsat_dsm_create_packet(msg_ptr, sizeof(dsat_cm_msg_s_type), TRUE);
      if (NULL != dsm_item_ptr)
      {
        /* Add to queue and raise subtask signal */
        dsm_enqueue(&dsat_cm_msg_wm, &dsm_item_ptr);

        if ( rssi_enqueue )
        {
          /* Flag one RSSI event in queue */
#ifdef IMAGE_QDSP6_PROC
          rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
          INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
          dsat_rssi_enqueue = dsat_rssi_enqueue | rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
          rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
          INTFREE();
#endif /* IMAGE_QDSP6_PROC*/
        }
      }
      else
      {
        post_ss_event = FALSE;
      }
    }
    else
    {
      /* Watermark flow control engaged */
      DS_AT_MSG1_ERROR("CM message queue full - dropping event: %d", event);
      post_ss_event = FALSE;
    }
  }

  /* At this point, post_ss_event can be TRUE only when the message is successfully
     posted and in all other cases it will be FALSE. */
  if ( FALSE == post_ss_event )
  {
    for ( stack_id = DSAT_STACK_MAIN; stack_id < event_ptr->number_of_stacks; stack_id++ )
    {
      if (NULL != msg_ptr->msg.ss_event.event_info[stack_id].emerg_num_list)
      {
        dsatutil_free_memory( msg_ptr->msg.ss_event.event_info[stack_id].emerg_num_list );
      }
    }
  }

  dsatutil_free_memory( msg_ptr );

  return;
} /* cmif_ss_event_msim_cb_func */


/*===========================================================================
FUNCTION  DSATCMIF_PH_CMD_CB_FUNC

DESCRIPTION
  CM phone command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
void dsatcmif_ph_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          cmd_err           /* Command error code    */
)
{
  ds_cmd_type               * cmd_buf  = NULL;
  boolean                     post_cmd = FALSE;
  ds_at_cm_ph_cmd_type*       ph_cmd   = NULL;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode       = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */

  DS_AT_MSG3_MED("dsatcmif_ph_cmd_cb_func: data_ptr = %p, cmd = %d, cmd_err = %d",
                  data_ptr, cmd, cmd_err);

  /* Filter supported commands */
  switch (cmd)
  {
  case CM_PH_CMD_SET_PREFERRED_NETWORKS:
#ifdef FEATURE_DSAT_ETSI_MODE
    if ( IS_ETSI_MODE(current_mode) )
    {
      post_cmd = TRUE;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
    break;
  case CM_PH_CMD_INFO_GET:
  case CM_PH_CMD_SYS_SEL_PREF:
  case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
  case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
  case CM_PH_CMD_OPRT_MODE:
  case CM_PH_CMD_GET_NETWORKS:
#ifdef FEATURE_DSAT_LTE
  case CM_PH_CMD_GET_DRX_REQ:
  case CM_PH_CMD_SET_DRX_REQ:
#endif/* FEATURE_DSAT_LTE*/
    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != cmd_err)
    {
      post_cmd = TRUE;
    }
    break;
  case CM_PH_CMD_RESET_ACM:
  case CM_PH_CMD_SET_ACMMAX:
  case CM_PH_CMD_MRU_UPDATE:
  case CM_PH_CMD_READ_MRU:
    post_cmd = TRUE;
    break;
  default:
    /* Do nothing for unsupported commands */
    break;
  }

  if (TRUE == post_cmd)
  {
    /* Send result to DS task */
    cmd_buf    = dsat_get_cmd_buf(sizeof(ds_at_cm_ph_cmd_type), FALSE);
    ph_cmd = cmd_buf->cmd_payload_ptr;

    /* Send the message to ATCOP */
    cmd_buf->hdr.cmd_id  = DS_CMD_ATCOP_CM_PH_CMD;
    ph_cmd->data_ptr = data_ptr;
    ph_cmd->cmd      = cmd;
    ph_cmd->cmd_err  = cmd_err;

    ds_put_cmd(cmd_buf);
  }

  return;
} /* dsatcmif_ph_cmd_cb_func */

/*===========================================================================

FUNCTION  DSATCMIF_CM_PH_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM phone-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_ph_cmd_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type         result = DSAT_ASYNC_CMD;
  ds_at_cm_ph_cmd_type*         ph_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);
  sys_modem_as_id_e_type        subs_id = dsat_get_current_subs_id(TRUE);
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type             current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
  dsatcmif_servs_state_ms_info     *ph_ss_ms_val = NULL;

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);

  switch (ph_cmd->cmd)
  {
  case CM_PH_CMD_SYS_SEL_PREF:

    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err)
    {
      DS_AT_MSG1_ERROR("CM PH command error: %d",ph_cmd->cmd_err);
      
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        /* Clear flags on failure of attach write */
        if ((CM_PH_CMD_SYS_SEL_PREF == ph_cmd->cmd) && (NULL != ph_ss_ms_val))
        {
          if(DSAT_COPS_ASTATE_NULL != ph_ss_ms_val->dsatetsicall_network_list.cmd_state)
          {
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
             (dsat_num_item_type)ph_ss_ms_val->dsatetsicall_network_list.previous_pref.mode,MIX_NUM_TYPE)
          }
          /* Asynch command processing is done */
          ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
          ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        }
      }/* IS_ETSI_MODE() */
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
 
  default:
    result = DSAT_ERROR;
    break;
  }

  if(DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp) )
  {
    result = dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp(ph_cmd, result);
  }

  return result;
} /* dsatcmif_cm_ph_cmd_handler_ex */



/*===========================================================================
FUNCTION  DSATCMIF_UPDATE_NETWORK_INFO

DESCRIPTION


DEPENDENCIES


RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL boolean dsatcmif_update_network_info
(
  ds_at_ph_info_u_type    *event_info_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  dsatcmif_servs_state_ms_info   *ph_ss_ms_val = NULL;

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if (NULL == ph_ss_ms_val)
  {
    return FALSE;
  }
  
  ph_ss_ms_val->dsat_prev_ms_service_preference = ph_ss_ms_val->dsat_ms_service_preference;
  ph_ss_ms_val->dsat_ms_service_preference = event_info_ptr->pref_info.service_domain_pref;

  if( event_info_ptr->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_sel_mode_pref =
                   ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref ;
  }

  if(event_info_ptr->pref_info.network_rat_mode_pref == CM_MODE_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_rat_mode_pref = 
                    ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref ;
  }

  if( event_info_ptr->pref_info.network_roam_pref == CM_ROAM_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_roam_pref =
                    ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_roam_pref ;
  }

  if(event_info_ptr->pref_info.network_band_pref == CM_BAND_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_band_pref = 
                    ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_band_pref ;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION  DSATCMIF_CM_PH_EVENT_HANDLER_EX

DESCRIPTION
  This function is the event handler invoked by CM in response to Phone
  group commands:
    - PS domain attach or detach triggered by +CGATT command
    - functionality level/operating mode (power setting)
    - Available/preferred network reporting
    - Phone preference changes

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_EVENT : if success and no response required.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_ph_event_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  ds_at_cm_ph_event_type*    ph_event = dsat_get_cmd_payload_ptr(cmd_ptr);
  
#ifdef FEATURE_DSAT_LTE
  dsatcmif_eps_mode_e_type      eps_mode = DSAT_EPS_MODE_MAX;
#endif /* FEATURE_DSAT_LTE */  

  sys_modem_as_id_e_type            subs_id;
  dsatcmif_servs_state_ss_info     *ph_ss_val = NULL;
  dsatcmif_servs_state_ms_info     *ph_ss_ms_val = NULL;

  subs_id = ph_event->event_info.subs_id;

  DS_AT_MSG4_HIGH("Processing CM PH event: %d, OPRT mode %d, device mode %d, subs_id:%d",
                  ph_event->event,
                  ph_event->event_info.oprt_mode,
                  ph_event->event_info.device_mode,
                  subs_id);

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    return DSAT_ASYNC_EVENT;
  }

  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
  
  if ((NULL == ph_ss_val) || (NULL == ph_ss_ms_val))
  {
    return DSAT_ASYNC_EVENT;
  }
  
  ph_ss_val->dsat_device_mode = ph_event->event_info.device_mode;

  switch(ph_event->event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    {
      if (ph_ss_val->dsat_oprt_mode != ph_event->event_info.oprt_mode)
      {
        ph_ss_val->dsat_oprt_mode = ph_event->event_info.oprt_mode;
        ph_event->event_info.oprt_mode_changed = TRUE;
      }
      else
      {
        ph_event->event_info.oprt_mode_changed = FALSE;
      }

#ifdef FEATURE_DSAT_ETSI_DATA
      if( ph_event->event_info.oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        dsat_power_off[subs_id] = TRUE; /* Added to indicate power on detach */
      }
      else
      {
        dsat_power_off[subs_id] = FALSE;
      }
#endif /* FEATURE_DSAT_ETSI_DATA */
      break;
    }
    
#ifdef FEATURE_DSAT_ETSI_MODE 
    case CM_PH_EVENT_SYS_SEL_PREF:
    {
      ds_at_cm_ph_pref_type *pref_info_ptr = &ph_event->event_info.pref_info;

      if(FALSE == dsatcmif_update_network_info(&ph_event->event_info,subs_id ))
      { 
        return DSAT_ASYNC_EVENT;
      }

      /* Update network registration state tracking */
      ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref =
      pref_info_ptr->network_sel_mode_pref;
      ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref =
      pref_info_ptr->network_rat_mode_pref;
      
      if( (!(( DSAT_COPS_ASTATE_PREF == ph_ss_ms_val->dsatetsicall_network_list.cmd_state) ||
             ( DSAT_COPS_ASTATE_PREF_ONLY == ph_ss_ms_val->dsatetsicall_network_list.cmd_state))) &&
          (TRUE == cmif_is_gw_subscription_available(subs_id, ph_event) ) )
      {
        /* Client other than ATCOP may have made change so update
         * +COPS mode.  The enum values correspond to the command
         * table list indexes so this simplifies update. */
        switch (ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref)
        {
          case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                     (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
            break;
          case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          case CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV:
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                     (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
            break;
          default:
            DS_AT_MSG1_ERROR(" network_sel_mode_pref = %d not handled",
                                   ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref);
            /* No update required */
            break;
        }
      }
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    if (FALSE == cmif_is_gw_subscription_available(subs_id, ph_event))
    {
     if(FALSE == dsatcmif_update_network_info(&ph_event->event_info,subs_id ))
     { 
       return DSAT_ASYNC_EVENT;
     }
      DS_AT_MSG0_HIGH("CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE event is successful");
      ph_ss_ms_val->net_pref.last_dereg_success = TRUE;
      ph_ss_ms_val->net_pref.mode = DSAT_COPS_MODE_DEREG;
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_DEREG,MIX_NUM_TYPE)
      memset ((void*)&ph_ss_ms_val->dsat_net_reg_state.net_id_info, 0x0,
                               sizeof(ph_ss_ms_val->dsat_net_reg_state.net_id_info));
            
      dsatetsicall_set_net_reg(DSAT_NET_REG_DOMAIN_CS ,DSAT_NET_REG_NONE,subs_id);
      dsatetsicall_set_net_reg(DSAT_NET_REG_DOMAIN_PS ,DSAT_NET_REG_NONE,subs_id);
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    if (TRUE == cmif_is_gw_subscription_available(subs_id, ph_event))
    {
     if(FALSE == dsatcmif_update_network_info(&ph_event->event_info,subs_id ))
     { 
       return DSAT_ASYNC_EVENT;
     }
      ph_ss_ms_val->net_pref.last_dereg_success = FALSE;
      if ( ph_ss_ms_val->net_pref.subscription_invoked == FALSE )
      {
        if(ph_event->event_info.pref_info.network_sel_mode_pref
                                           == CM_NETWORK_SEL_MODE_PREF_MANUAL)
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX, subs_id,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
        }
        else
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
        }
      }
      break;
    }

#ifdef FEATURE_SGLTE 
    case CM_PH_EVENT_SIM_AVAILABLE:
     if(FALSE == dsatcmif_update_network_info(&ph_event->event_info,subs_id ))
     {
       return DSAT_ASYNC_EVENT;
     }

    if (TRUE == cmif_is_gw_subscription_available(subs_id, ph_event))
    {
      ph_ss_ms_val->net_pref.last_dereg_success = FALSE;
      break;
    }
#endif/* FEATURE_SGLTE*/
#endif /* FEATURE_DSAT_ETSI_MODE */

    case CM_PH_EVENT_STANDBY_SLEEP:
    {
      ph_ss_val->cm_ph_deep_sleep = TRUE;
      break;
    }
    
    case CM_PH_EVENT_STANDBY_WAKE:
    {
      ph_ss_val->cm_ph_deep_sleep = FALSE;
      break;
    }
    case CM_PH_EVENT_INFO:
    {
#ifdef FEATURE_DSAT_ETSI_MODE 
      if(FALSE == dsatcmif_update_network_info(&ph_event->event_info,subs_id ))
      { 
        return DSAT_ASYNC_EVENT;
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
 
      if (ph_ss_val->dsat_oprt_mode != ph_event->event_info.oprt_mode)
      {
        ph_ss_val->dsat_oprt_mode = ph_event->event_info.oprt_mode;
        ph_event->event_info.oprt_mode_changed = TRUE;
      }
      else
      {
        ph_event->event_info.oprt_mode_changed = FALSE;
      }
      
#ifdef FEATURE_DSAT_ETSI_MODE      
#ifdef FEATURE_DSAT_LTE
     switch(ph_event->event_info.eps_mode)
     {
       case CM_LTE_UE_MODE_PREF_PS_MODE1:
         eps_mode = DSAT_EPS_PS_MODE_1;
         break;
       case CM_LTE_UE_MODE_PREF_PS_MODE2:  
         eps_mode = DSAT_EPS_PS_MODE_2;
         break;
       case CM_LTE_UE_MODE_PREF_CS_PS_MODE1:
         eps_mode = DSAT_EPS_CS_PS_MODE_1;
         break;
       case CM_LTE_UE_MODE_PREF_CS_PS_MODE2:
         eps_mode = DSAT_EPS_CS_PS_MODE_2;
         break;
       default:
         break;
     }
     if(((dsat_num_item_type)
            dsatutil_get_val( DSATETSI_EXT_CEMODE_IDX,0,0,NUM_TYPE) != eps_mode)&& 
            (eps_mode != DSAT_EPS_MODE_MAX))
     {
       DSATUTIL_SET_VAL(DSATETSI_EXT_CEMODE_IDX,0,0,0,eps_mode,NUM_TYPE);
       DS_AT_MSG1_HIGH(" UPDATED CEMODE as %d ",eps_mode);
     }
#endif /* FEATURE_DSAT_LTE */
        /* This condition is satisfied only during initial phone boot-up.
              Update +COPS <mode> using network_sel_mode_pref. */
      if( (dsat_num_item_type)DSAT_COPS_MODE_MAX == 
                              (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,MIX_NUM_TYPE))
      {
        if(ph_event->event_info.pref_info.network_sel_mode_pref
                                              == CM_NETWORK_SEL_MODE_PREF_MANUAL)
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                            (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
        }
        else
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,subs_id,0,0,
                                            (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
        }
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    }

#ifdef FEATURE_DATA_IS707
    case CM_PH_EVENT_NVRUIM_CONFIG_CHANGED:
    {
      dsat_sysinfo_cmd.dsat_rtre_config = (dsat_rtre_control_e_type)ph_event->event_info.dsat_rtre_config;
      break;
    }
#endif /* FEATURE_DATA_IS707 */

#if defined(FEATURE_HS_USB_SER3_PORT)
    case CM_PH_EVENT_PACKET_STATE:
      /* Send indication to TE for dormant or non-dormant call */
    {
      dsatcmif_call_status_e_type acm_val= DS_CALL_STATUS_NONE;
      cm_packet_state_e_type curr_state;
      result = DSAT_ASYNC_EVENT;
      curr_state = ph_event->event_info.packet_state;
      DS_AT_MSG1_MED("current call state %d",curr_state);
      /* This is for dormancy status report */
      if (curr_state == CM_PACKET_STATE_DORMANT)
      {
        acm_val = DS_CALL_STATUS_DORMANT;
      }
      else
      {
        acm_val = DS_CALL_STATUS_NONDORMANT;
      }
    
      /* need to report the dormancy status*/
      ph_ss_val->dsatcmif_cm_pkt_state = acm_val;
      if( FALSE == ph_ss_val->dsatcmif_dormant_ind_report_enabled )
      {
         DS_AT_MSG0_HIGH("Call has not connected yet");
         dsatutil_free_memory((void*)ph_event);
         return DSAT_ASYNC_EVENT;
      }

       ph_event->event_info.report_dormant = TRUE;
      if( CM_PACKET_STATE_NULL == curr_state )
      {
        ph_ss_val->dsatcmif_dormant_ind_report_enabled = FALSE;
      }
       
      break;
    }
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
    default:
    {
      break;
    }
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp(ph_event, result, subs_id);
  }

  return result;

}/* dsatcmif_cm_ph_event_handler_ex */

/*===========================================================================

FUNCTION DSATCMIF_SIGNAL_HANDLER

DESCRIPTION
  This function processes the asynchronous signals assosiacted with
  messages from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_signal_handler
(
 dsat_mode_enum_type at_mode
)
{
  dsm_item_type         *dsm_item_ptr;
  dsat_cm_msg_s_type    *msg_ptr = NULL;
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;

  msg_ptr = dsat_alloc_memory(sizeof(dsat_cm_msg_s_type), TRUE);
  if (NULL == msg_ptr)
  {
    return result;
  }

  while ( (dsm_item_ptr = dsm_dequeue( &dsat_cm_msg_wm )) != NULL)
  {
    (void)dsm_pullup( &dsm_item_ptr, msg_ptr, sizeof(dsat_cm_msg_s_type) );

    DS_AT_MSG1_MED("ATCOP:Freed 0x%p DSM item, signal handler", dsm_item_ptr);
    dsm_free_packet( &dsm_item_ptr );
    
    /* Dispatch event message to handler */
    switch( msg_ptr->cmd )
    {
      case DS_CMD_ATCOP_CM_SS_INFO_CMD:
        result = dsatcmif_cm_ss_event_handler_ex( msg_ptr );
        break;

    default:
      DS_AT_MSG1_ERROR("Command = %d", msg_ptr->cmd);
    }

    /* Check for response reporting requirement */
    if( result != DSAT_ASYNC_CMD && result != DSAT_ASYNC_EVENT )
    {
      /* If pending msgs in queue, set signal again */
      if( dsat_cm_msg_wm.current_cnt != 0 )
      {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
        (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_CM_MSG_SIG );
#else
        (void)rex_set_sigs( &ds_tcb, DS_AT_CM_MSG_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
      }

      break;
    }
  } /* while */

  dsatutil_free_memory( msg_ptr );

  return result;
} /* dsatcmif_signal_handler */


/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF_EX

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to Call Manager.
  - Setup the Call Manager message watermark.
  - Registers the client call back functions.
  - Requests initial phone info (for preferences).
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif_ex ( void )
{
  /* Register ATCoP as a client to Call Manager */
  cmif_client_init ( );

  /* Setup the Call Manager message watermark */
  cmif_setup_msg_watermarks();
  
  /* Call the registration function to register call back functions */
  cmif_register_cb_func_ex ( );


  /* Initialize RTRE control value */
#ifdef FEATURE_UIM_RUIM
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  switch (nv_rtre_control())
  {
    case NV_RTRE_CONTROL_USE_RUIM:
      dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_RUIM;
      break;
    case NV_RTRE_CONTROL_NO_RUIM:
      dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_NV;
      break;
    #ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
    case NV_RTRE_CONTROL_SIM_ACCESS:
      dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_SIM;
      break;
    #endif  /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
    default:
      DS_AT_MSG0_ERROR("Invalid RTRE control setting from NV");
      dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_NONE;
  }
#else /* FEATURE_UIM_RUN_TIME_ENABLE */
  dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_RUIM;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#else /* FEATURE_UIM_RUIM */
  dsat_sysinfo_cmd.dsat_rtre_config = DSAT_RTRE_CONTROL_NV;
#endif /* FEATURE_UIM_RUIM */


  dsat_mo_dailing_state_init();

  /* Request phone settings for preferences */
  if (TRUE != cm_ph_cmd_get_ph_info( dsatcmif_ph_cmd_cb_func,
                                     NULL,
                                     dsatcm_client_id ))
  {
    DS_AT_MSG0_ERROR("Problem requesting initial CM PH info ");
  }

  return;
}/* dsatcmif_init_cmif_ex */



/************************ LOCAL FUNCTIONS *************************/
/*===========================================================================

FUNCTION   CMIF_CLIENT_INIT

DESCRIPTION
  Initializes ATCOP as a client with Call Manager task.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_client_init ( void )
{
  cm_client_status_e_type status;

  /* Initialize ATCOP as one of the clients of CM */
  status = cm_client_init (
                           CM_CLIENT_TYPE_ATCOP,
                           &dsatcm_client_id 
                          );

  /* This should not happen, raise an error */
  if (status != CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to register as CM client: %d", 
               (int)status, 0, 0 );
  }

  return;
}/* cmif_client_init */



/*===========================================================================
FUNCTION  CMIF_CALL_EVENT_CB_FUNC

DESCRIPTION
  CM Call status event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
void cmif_call_event_cb_func
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  ds_cmd_type * cmd_buf;
  ds_at_cm_call_event_type*  call_event;
  static cm_call_sups_type_e_type  dsat_sups_type;

  ASSERT (event_ptr != NULL);

  if( (event == CM_CALL_EVENT_END_REQ) ||
      (event == CM_CALL_EVENT_MOD_BEARER_IND) ||
      (event == CM_CALL_EVENT_GET_PDN_CONN_IND) ||
      (event == CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND) ||
      (event == CM_CALL_EVENT_SETUP_RES ) ||
      (event == CM_CALL_EVENT_MT_CALL_PAGE_FAIL ) )
  {
    DS_AT_MSG1_LOW("%d Event not required for ATCoP client",event);
    return;
  }
  /* Filter to pass the following events... */
      /* voice/emergency calls,*/
if ( dsatcmif_is_voice_call_type(event_ptr->call_type)

      /* outgoing packet data calls(ETSI only), */
#ifdef FEATURE_DSAT_ETSI_DATA
      || ((CM_CALL_TYPE_PS_DATA == event_ptr->call_type) &&
          (CM_CALL_STATE_INCOM != event_ptr->call_state))
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
       /* Incoming, connected or end events for CS data calls */
      || (CM_CALL_TYPE_CS_DATA == event_ptr->call_type &&
           ( CM_CALL_STATE_INCOM == event_ptr->call_state ||
             CM_CALL_STATE_CONV  == event_ptr->call_state ||
             CM_CALL_STATE_IDLE  == event_ptr->call_state ) )

       /* CM_CALL_EVENT_MNG_CALLS_CONF
          For ATCOP originated events: 
            Need to check success and update state or give CME ERROR.
          For other client:
            Need to update held calls info as no other events are posted. */
      || event == CM_CALL_EVENT_MNG_CALLS_CONF
      || event == CM_CALL_EVENT_PROGRESS_INFO_IND
      || event == CM_CALL_EVENT_ABRV_ALERT
      || event == CM_CALL_EVENT_CALL_IS_WAITING
      || event == CM_CALL_EVENT_CALL_BARRED
      || event == CM_CALL_EVENT_CALL_FORWARDED
      || event == CM_CALL_EVENT_CALL_BEING_FORWARDED
      || event == CM_CALL_EVENT_INCOM_FWD_CALL
      || event == CM_CALL_EVENT_CALL_ON_HOLD 
      || event == CM_CALL_EVENT_CALL_RETRIEVED
      || event == CM_CALL_EVENT_ORIG_FWD_STATUS
      || event == CM_CALL_EVENT_ORIG
      || event == CM_CALL_EVENT_ACT_BEARER_IND
      || event == CM_CALL_EVENT_PDN_CONN_REJ_IND
      || event == CM_CALL_EVENT_PDN_CONN_FAIL_IND
      || event == CM_CALL_EVENT_RES_ALLOC_REJ_IND
      || event == CM_CALL_EVENT_RES_ALLOC_FAIL_IND
      || event == CM_CALL_EVENT_ANSWER
#endif /* FEATURE_DSAT_ETSI_MODE */
      || event == CM_CALL_EVENT_INFO
#ifdef FEATURE_DATA_WCDMA_PS
      || event == CM_CALL_EVENT_RAB_REESTAB_IND 
#endif /* FEATURE_DATA_WCDMA_PS */
#if defined(FEATURE_HS_USB_SER3_PORT)
      || (CM_CALL_EVENT_CONNECT == event && 
          CM_CALL_TYPE_PS_DATA == event_ptr->call_type)
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
#ifdef FEATURE_DSAT_ETSI_MODE
      ||( event == CM_CALL_EVENT_SETUP_IND )
#else
      ||( event == CM_CALL_EVENT_CALLER_ID)
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DUAL_ACTIVE
      ||( event == CM_CALL_EVENT_LOCAL_HOLD )
#endif /* FEATURE_DUAL_ACTIVE */
      ||( event == CM_CALL_EVENT_PS_SIG_REL_CNF)
      ||( event == CM_CALL_EVENT_HO_COMPLETE)
      ||( event == CM_CALL_EVENT_SRVCC_COMPLETE_IND)
     )
  {
    DS_AT_MSG4_MED("ATCOP: cm event cb, event: %d, type: %d, state: %d, "
                   "mode info type: %d",
                   event, event_ptr->call_type, event_ptr->call_state,
                   event_ptr->mode_info.info_type);

     if( CM_CALL_EVENT_SUPS == event)
     {
       if (CM_CALL_MODE_INFO_GW_CS == event_ptr->mode_info.info_type)
       {
         dsat_sups_type = event_ptr->mode_info.info.gw_cs_call.sups_type;
         DS_AT_MSG1_MED("dsat_sups_type: %d", dsat_sups_type);
       }
       return;
     }

    cmd_buf       = dsat_get_cmd_buf(sizeof(ds_at_cm_call_event_type), TRUE);
    
    if ((NULL != cmd_buf) && (cmd_buf->cmd_payload_ptr != NULL))
    {
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_INFO_CMD;
      call_event = cmd_buf->cmd_payload_ptr;

      /* If event_ptr is NULL we shouldn't get this far */
      /* Pleasing lint here                          */
      if( NULL != event_ptr )  /*lint !e774 */
      {
        call_event->event_info.dsat_info_list_avail = FALSE;
        call_event->event_info.call_client_id = (cm_client_type_e_type)event_ptr->call_client_id;
        call_event->event_info.requesting_client_id = (cm_client_type_e_type)event_ptr->requesting_client_id;
        call_event->event_info.call_type = event_ptr->call_type;
        call_event->event_info.call_state = event_ptr->call_state;

        (void) dsatutil_memscpy((void*)&call_event->event_info.num,
                                sizeof(cm_num_s_type),
                                (void*)&event_ptr->num,
                                sizeof(cm_num_s_type));
        call_event->event_info.call_direction =  event_ptr->direction;
        call_event->event_info.sys_mode =  event_ptr->sys_mode;
        call_event->event_info.subs_id = event_ptr->asubs_id;
#ifdef FEATURE_DUAL_ACTIVE
        call_event->event_info.local_hold_state = event_ptr->local_hold_state;
#endif /* FEATURE_DUAL_ACTIVE */

        call_event->event_info.dsat_mode_info.info_type = event_ptr->mode_info.info_type;

        switch (event_ptr->mode_info.info_type)
        {
          case CM_CALL_MODE_INFO_CDMA:
            call_event->event_info.dsat_mode_info.info.cdma_call.srv_opt =
              event_ptr->mode_info.info.cdma_call.srv_opt;
            break;

          case CM_CALL_MODE_INFO_GW_CS:
            call_event->event_info.dsat_mode_info.info.gw_cs_call.bearer_capability_1 =
              event_ptr->mode_info.info.gw_cs_call.bearer_capability_1;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.called_party_bcd_number =
              event_ptr->mode_info.info.gw_cs_call.called_party_bcd_number;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.called_party_subaddress =
              event_ptr->mode_info.info.gw_cs_call.called_party_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.calling_party_bcd_number =
              event_ptr->mode_info.info.gw_cs_call.calling_party_bcd_number;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.calling_party_subaddress =
              event_ptr->mode_info.info.gw_cs_call.calling_party_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cc_cause =
              event_ptr->mode_info.info.gw_cs_call.cc_cause;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cc_reject =
              event_ptr->mode_info.info.gw_cs_call.cc_reject;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.active_calls_list =
              event_ptr->mode_info.info.gw_cs_call.active_calls_list;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.connected_subaddress =
              event_ptr->mode_info.info.gw_cs_call.connected_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cause_of_no_cli =
              event_ptr->mode_info.info.gw_cs_call.cause_of_no_cli;
            call_event->event_info.call_ss_success = event_ptr->mode_info.info.gw_cs_call.call_ss_success;;
            break;

          case CM_CALL_MODE_INFO_IP:
            call_event->event_info.call_ss_success = event_ptr->mode_info.info.ip_call.call_ss_success;
            break;

          case CM_CALL_MODE_INFO_GW_PS:
          case CM_CALL_MODE_INFO_GSM_PS:
          case CM_CALL_MODE_INFO_WCDMA_PS:
            call_event->event_info.dsat_mode_info.info.gw_ps_call.nsapi =
              event_ptr->mode_info.info.gw_ps_call.nsapi;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.pdp_cause_type =
              event_ptr->mode_info.info.gw_ps_call.pdp_cause_type;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.cause =
              event_ptr->mode_info.info.gw_ps_call.cause;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.cause =
              event_ptr->mode_info.info.gw_ps_call.cause;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.ps_sig_rel_status =
              event_ptr->mode_info.info.gw_ps_call.ps_sig_rel_status;

            call_event->event_info.profile_number =
              event_ptr->mode_info.info.gw_ps_call.profile_number;
            break;

          case CM_CALL_MODE_INFO_LTE:
            call_event->event_info.dsat_mode_info.info.lte_call.esm_cause =
              event_ptr->mode_info.info.lte_call.esm_cause;
            call_event->event_info.dsat_mode_info.info.lte_call.esm_local_cause =
              event_ptr->mode_info.info.lte_call.esm_local_cause;

            call_event->event_info.profile_number =
              event_ptr->mode_info.info.lte_call.profile_number;
            break;

          default:
            break;
        }

#ifdef FEATURE_DSAT_ETSI_MODE
        if ( IS_ETSI_MODE(dsatcmdp_get_current_mode_per_subs(event_ptr->asubs_id)) )
        {
          call_event->event_info.end_status = event_ptr->end_status;
          call_event->event_info.call_direction = event_ptr->direction;
          if ( CM_CALL_EVENT_MNG_CALLS_CONF == event )
          {
            call_event->event_info.call_id = event_ptr->call_id;
            /* This occurs if another call is dialed via UI or ATD when a call
               is already up and corresponds to the case
               'Entering "Directory Number"' in 22.030 section 6.5.5.1.
               Enum cm_call_sups_type_e_type has no value to describe this 
               case. Use CM_CALL_SUPS_TYPE_SELECT_CALL_X */
            if ( event_ptr->call_state == CM_CALL_STATE_ORIG )
            {
              call_event->event_info.sups_type = CM_CALL_SUPS_TYPE_SELECT_CALL_X;
            }
            else
            {
              /* Assume that this manage calls confirmation event is for the 
                 supplementary services command */
              if (event_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS)
              {
                call_event->event_info.sups_type = dsat_sups_type;
              }
              else if (event_ptr->mode_info.info_type == CM_CALL_MODE_INFO_IP)
              {
                call_event->event_info.sups_type = event_ptr->mode_info.info.ip_call.sups_type;
              }
              DS_AT_MSG2_HIGH("mode_info:%d, sups_type:%d", event_ptr->mode_info.info_type,
                                                            call_event->event_info.sups_type);
            }
          }
          else
          {
            call_event->event_info.call_id = event_ptr->call_id;
          }
        }
        else
#endif /* FEATURE_DSAT_ETSI_MODE */
        {
          call_event->event_info.call_id = event_ptr->call_id;
        }
      }
      call_event->event      = event;
      ds_put_cmd(cmd_buf);
    }
  }
} /* cmif_call_event_cb_func */

/*===========================================================================
FUNCTION  CMIF_CM_PH_EVENT_CB_FUNC

DESCRIPTION
  CM PH event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
LOCAL void cmif_cm_ph_event_cb_func 
(
  cm_ph_event_e_type         event,            /* Event ID              */
  const cm_ph_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  ds_cmd_type * cmd_buf;
  ds_at_cm_ph_event_type *ph_event = NULL;

  ASSERT (event_ptr != NULL);

  DS_AT_MSG3_HIGH("IN cmif_cm_ph_event_cb_func oprt mode = %d  event = %d Subs=%d",
                  event_ptr->oprt_mode,event,event_ptr->asubs_id);

  /* Filter events to DS task */
  switch (event)
  {
  case CM_PH_EVENT_OPRT_MODE:
  case CM_PH_EVENT_INFO:
  case CM_PH_EVENT_SYS_SEL_PREF:
  case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
  case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
  case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
  case CM_PH_EVENT_FUNDS_LOW:
  case CM_PH_EVENT_TERMINATE_GET_NETWORKS:
#ifdef FEATURE_ETSI_ATTACH
  case CM_PH_EVENT_PS_ATTACH_FAILED:
#endif /* FEATURE_ETSI_ATTACH */
#if defined(FEATURE_HS_USB_SER3_PORT)
  case CM_PH_EVENT_PACKET_STATE:
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
  case CM_PH_EVENT_STANDBY_SLEEP:
  case CM_PH_EVENT_STANDBY_WAKE:
#ifdef FEATURE_DATA_IS707
  case CM_PH_EVENT_NVRUIM_CONFIG_CHANGED:
#endif /* FEATURE_DATA_IS707 */
#ifdef FEATURE_DUAL_SIM
  case CM_PH_EVENT_DUAL_STANDBY_PREF:
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_DSAT_LTE
  case CM_PH_EVENT_GET_DRX_CNF:
  case CM_PH_EVENT_SET_DRX_CNF:
#endif/*FEATURE_DSAT_LTE*/
#ifdef FEATURE_SGLTE
  case CM_PH_EVENT_SIM_AVAILABLE:
#endif/* FEATURE_SGLTE*/
    /* Post event to DSTASK */
    cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_cm_ph_event_type), FALSE);

    /* Send the message to ATCOP */
    cmd_buf->hdr.cmd_id           = DS_CMD_ATCOP_CM_PH_INFO_CMD;
    ph_event = cmd_buf->cmd_payload_ptr;
    ph_event->event = event;
    ph_event->event_info.subs_id = event_ptr->asubs_id;

    switch (event)
    {
#ifdef FEATURE_DSAT_ETSI_MODE
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
      DS_AT_MSG2_MED("available_networks_list_cnf = %d, length = %d",
                     event_ptr->available_networks_list_cnf, 
                     event_ptr->available_networks.length);

     /* Copy the PLMN list only if list_cnf is SYS_PLMN_LIST_SUCCESS else treat the lenght as -1*/        
      if(event_ptr->available_networks_list_cnf == 
          SYS_PLMN_LIST_SUCCESS)
      {
        uint8 i;

        ph_event->event_info.available_networks.length = event_ptr->available_networks.length;

        for (i = 0; (i < event_ptr->available_networks.length) && (i < SYS_PLMN_LIST_MAX_LENGTH); i++)
        {
          ph_event->event_info.available_networks.info[i].plmn =
            event_ptr->available_networks.info[i].plmn;
          ph_event->event_info.available_networks.info[i].rat =
            event_ptr->available_networks.info[i].rat;
          ph_event->event_info.available_networks.info[i].plmn_forbidden =
            event_ptr->available_networks.info[i].plmn_forbidden;
          ph_event->event_info.available_networks.info[i].list_category =
            event_ptr->available_networks.info[i].list_category;
          ph_event->event_info.available_networks.info[i].csg_info.csg_id =
            event_ptr->available_networks.info[i].csg_info.csg_id;
        }
      }
      else
      {
        ph_event->event_info.available_networks.length = INVALID_AVAIL_NET_LENGTH;
      }
      break;
#endif /* FEATURE_DSAT_ETSI_MODE  */
      
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_SYS_SEL_PREF:
#ifdef FEATURE_DSAT_ETSI_MODE 
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
#ifdef FEATURE_SGLTE
  case CM_PH_EVENT_SIM_AVAILABLE:	  
#endif/* FEATURE_SGLTE*/
#ifdef FEATURE_DSAT_LTE
       if( event == CM_PH_EVENT_INFO )
       {
         ph_event->event_info.eps_mode = event_ptr->lte_ue_mode_pref;
         DS_AT_MSG1_HIGH(" EPS UE MODE - %d ",event_ptr->lte_ue_mode_pref);
       }
#endif /* FEATURE_DSAT_LTE */
      ph_event->event_info.pref_info.service_domain_pref= event_ptr->srv_domain_pref;
      ph_event->event_info.pref_info.network_sel_mode_pref = event_ptr->network_sel_mode_pref;
      ph_event->event_info.pref_info.network_rat_mode_pref = event_ptr->mode_pref;
      ph_event->event_info.pref_info.network_roam_pref = event_ptr->roam_pref;
      ph_event->event_info.pref_info.network_band_pref = event_ptr->band_pref;
      ph_event->event_info.pref_info.acq_order_pref = event_ptr->acq_order_pref;
      ph_event->event_info.pref_info.acq_pri_order_pref = event_ptr->acq_pri_order_pref;

#ifdef CM_API_AOC_INFO
      ph_event->event_info.aoc_info = event_ptr->aoc_info;
#endif /* CM_API_AOC_INFO */
#endif /* FEATURE_DSAT_ETSI_MODE */

      ph_event->event_info.oprt_mode = event_ptr->oprt_mode;
      ph_event->event_info.pref_info.network_rat_mode_pref = event_ptr->mode_pref;
      ph_event->event_info.pref_info.network_band_pref = event_ptr->band_pref;

      /* Subscription available info */
      ph_event->event_info.is_gw_subscription_available = 
                                  event_ptr->is_gw_subscription_available;
      ph_event->event_info.is_hybr_gw_subscription_available = 
      	                           event_ptr->is_hybr_gw_subscription_available;
      ph_event->event_info.is_hybr_gw3_subscription_available = 
                                  event_ptr->is_hybr_gw3_subscription_available;
      break;

#if defined(FEATURE_HS_USB_SER3_PORT)
    case CM_PH_EVENT_PACKET_STATE:
      ph_event->event_info.packet_state = event_ptr->packet_state;
      break;
#endif /* defined(FEATURE_HS_USB_SER3_PORT) */
    case CM_PH_EVENT_STANDBY_SLEEP:
    case CM_PH_EVENT_STANDBY_WAKE:
      break;
#ifdef FEATURE_DATA_IS707
    case CM_PH_EVENT_NVRUIM_CONFIG_CHANGED:
      ph_event->event_info.dsat_rtre_config = (uint16) event_ptr->rtre_control;
      break;
    case CM_PH_EVENT_OPRT_MODE:
      ph_event->event_info.oprt_mode = event_ptr->oprt_mode;
      break;
#endif /*FEATURE_DATA_IS707*/

#ifdef FEATURE_DSAT_LTE
    case CM_PH_EVENT_GET_DRX_CNF:
      DS_AT_MSG1_HIGH("drx_coefficient = %d", event_ptr->drx_coefficient);
      ph_event->event_info.drx_coefficient = event_ptr->drx_coefficient;
      break;
    case CM_PH_EVENT_SET_DRX_CNF:
      DS_AT_MSG1_HIGH("set_drx_result = %d", event_ptr->set_drx_result);
      ph_event->event_info.set_drx_result = event_ptr->set_drx_result;
      break;
#endif /*FEATURE_DSAT_LTE*/
    default:  
      ph_event->event_info.oprt_mode = event_ptr->oprt_mode;
      break;
    }

    ph_event->event_info.device_mode = event_ptr->device_mode;

    ds_put_cmd(cmd_buf);
    break;

  default:
    DS_AT_MSG1_ERROR("Unsupported CM PH event: %d", event);
  }
  return;
} /* cmif_cm_ph_event_cb_func */

/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_NET_REG_DATA

DESCRIPTION
  This function processes the Call Manager registration event data to
  set network registration state information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_process_cm_net_reg_data
(
  const ds_at_cm_ss_event_type *event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type          stack_id,
  sys_modem_as_id_e_type        subs_id
)
{
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;
  dsat_net_reg_state_s_type    *net_reg_state_ptr = NULL;

  if ( ( NULL == event_ptr ) ||
       ( FALSE == IS_VALID_STACK_ID(stack_id) ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG3_ERROR("Invalid Arg: event_ptr = %p, stack_id = %d, net_id = %d",
                     event_ptr, stack_id, subs_id);
    return;
  }

  DS_AT_MSG3_HIGH("PLMN structure: 0x%X 0x%X 0x%X",
                  event_ptr->event_info[stack_id].sys_id.id.plmn.identity[0],
                  event_ptr->event_info[stack_id].sys_id.id.plmn.identity[1],
                  event_ptr->event_info[stack_id].sys_id.id.plmn.identity[2]);

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if (NULL == ph_ss_ms_val)
  {
    return;
  }
  net_reg_state_ptr = &ph_ss_ms_val->dsat_net_reg_state;

  /* Check for full service and capture operator information.
     If it is a non-full service, the COPS read command should not show
     the PLMN as it is not registerd so reset the PLMN */
  if ( SYS_SRV_STATUS_SRV == event_ptr->event_info[stack_id].srv_status )
  {
    #define COPS_NULL_PLMN 0xFF

      net_reg_state_ptr->net_id_info.csg_id = event_ptr->event_info[stack_id].csg_id;

    /* Update locally cached network identifier info */
    if( COPS_NULL_PLMN != event_ptr->event_info[stack_id].sys_id.id.plmn.identity[0] )
    {
      net_reg_state_ptr->net_id_info.present = TRUE;
      /* sys_id.id is a union, so plmn and plmn_lac would have valid
         values. for readibility, copy it as they are differnt. */
      net_reg_state_ptr->net_id_info.plmn =
        event_ptr->event_info[stack_id].sys_id.id.plmn;

      /* For now keep it though it is redundant information */
      (void) dsatutil_memscpy((void*)&net_reg_state_ptr->net_id_info.plmn_lac,
                              sizeof(sys_plmn_lac_id_s_type),
                              (void*)&event_ptr->event_info[stack_id].sys_id.id.plmn_lac,
                              sizeof(sys_plmn_lac_id_s_type));
#ifdef FEATURE_DSAT_LTE
      /* In LTE, LAC is TAC */
      if(event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_LTE)
      {
        net_reg_state_ptr->net_id_info.plmn_lac.lac = event_ptr->event_info[stack_id].lte_tac;
      }
#endif /* FEATURE_DSAT_LTE */
    }
    else
    {
      net_reg_state_ptr->net_id_info.present = FALSE;
    }

    net_reg_state_ptr->sys_mode = event_ptr->event_info[stack_id].sys_mode;

    /* Derive AcT based on system mode */
    switch ( event_ptr->event_info[stack_id].sys_mode )
    {
      case SYS_SYS_MODE_WCDMA:
        net_reg_state_ptr->act = DSAT_COPS_ACT_UTRAN;
        net_reg_state_ptr->csg_rat = DSAT_COPS_CSG_RAT_UMTS;
        break;
#ifdef FEATURE_TDSCDMA
      case SYS_SYS_MODE_TDS:
      net_reg_state_ptr->act = DSAT_COPS_ACT_UTRAN;
        net_reg_state_ptr->csg_rat = DSAT_COPS_CSG_RAT_TDS;
      break;
#endif /* FEATURE_TDSCDMA */

    case SYS_SYS_MODE_GSM:
      net_reg_state_ptr->act = DSAT_COPS_ACT_GSM;
        net_reg_state_ptr->csg_rat = DSAT_COPS_CSG_RAT_GSM;
      break;
#ifdef FEATURE_DSAT_LTE
    case SYS_SYS_MODE_LTE:
      net_reg_state_ptr->act = DSAT_COPS_ACT_EUTRAN;
      net_reg_state_ptr->csg_rat= DSAT_COPS_CSG_RAT_LTE;
      break;
#endif /* FEATURE_DSAT_LTE */
    default:
      net_reg_state_ptr->act = DSAT_COPS_ACT_MAX;
      net_reg_state_ptr->csg_rat = DSAT_COPS_CSG_RAT_MAX;
      break;
    }
  }
  /* Clear network identifier on non-full service */
  else
  {
    /* Clean PLMN & network name information */
    memset((void*)&net_reg_state_ptr->net_id_info, 0x0,
           sizeof(net_reg_state_ptr->net_id_info));

    net_reg_state_ptr->sys_mode = SYS_SYS_MODE_NO_SRV;

    net_reg_state_ptr->act = DSAT_COPS_ACT_MAX;
    net_reg_state_ptr->csg_rat = DSAT_COPS_CSG_RAT_MAX;
    net_reg_state_ptr->net_id_info.csg_id = SYS_CSG_ID_INVALID; 
  }

  return;
} /* cmif_process_cm_net_reg_data */


/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_REG_DATA_EX

DESCRIPTION
  This function processes the Call Manager registration event data to
  set PS attach and general network registration state information.

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
LOCAL dsat_result_enum_type cmif_process_cm_reg_data_ex
(
  const ds_at_cm_ss_event_type * event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type           stack_id,
  sys_modem_as_id_e_type         subs_id 
)
{
  #define PLMN_NULL 0xFF
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  sys_srv_domain_e_type          domain;
  sys_roam_status_e_type         roaming;
  if ( ( NULL == event_ptr ) ||
       ( FALSE == IS_VALID_STACK_ID(stack_id) ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG3_ERROR("Invalid Arg: event_ptr = %p, stack_id = %d, subs_id = %d",
                     event_ptr, stack_id, subs_id);
    return result;
  }
  domain = event_ptr->event_info[stack_id].srv_domain;
  roaming = event_ptr->event_info[stack_id].roam_status;

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_DEV_CMDS
  /* Update CNTI technology as NONE as UE has not camped yet*/
  dsat_qccnti_tech = DSAT_CNTI_NONE;
#endif /* FEATURE_DSAT_DEV_CMDS */
#endif /* FEATURE_DSAT_ETSI_MODE */

  DS_AT_MSG4_HIGH("Processing registration update: chgfld=%p %p srv=%d dom=%d",
                  ((event_ptr->event_info[stack_id].changed_fields) >> 32),
                  event_ptr->event_info[stack_id].changed_fields,
                  event_ptr->event_info[stack_id].srv_status, domain);

#ifdef FEATURE_DSAT_UMTS_UNDP 
  /* Update CNTI technology as NONE as it has not camped yet*/
  dsat_qccnti_tech = DSAT_CNTI_NONE;
#endif

#ifdef FEATURE_TDSCDMA
  cmif_report_mode_ex( event_ptr->event_info[stack_id].srv_status,
                       domain,
                       event_ptr->event_info[stack_id].sys_mode,
                       subs_id);
#endif /* FEATURE_TDSCDMA */

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
  if (NULL == ph_ss_ms_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  /* Check for network service. */
  /* Use the registration domain (CS,PS,CS+PS) to set the various
   * indicators within the subsystem. */
  if ( SYS_SRV_STATUS_NO_SRV != event_ptr->event_info[stack_id].srv_status &&
       SYS_SRV_STATUS_PWR_SAVE != event_ptr->event_info[stack_id].srv_status )
  {
    /* Update camped-only state */
    /* DOMAIN_CAMPED is not exactly registration success */
    /* In limited service and no domain, update the net domain. */
    if ( (SYS_SRV_DOMAIN_CAMPED == domain) ||
         (SYS_SRV_DOMAIN_NO_SRV == domain) )
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,0,0,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    }
    /* Update CS+PS registration */
    else if (SYS_SRV_DOMAIN_CS_PS == domain)
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,0,1,NUM_TYPE)
#endif /* FEATURE_ETSI_ATTACH */

      /* Set CS+PS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info[stack_id].srv_status )
      {
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_GSM )
        {
          if ( event_ptr->event_info[stack_id].cell_srv_ind.egprs_supp 
                                          == SYS_EGPRS_SUPPORT_AVAIL )
          {
            dsat_qccnti_tech = DSAT_CNTI_EGPRS;
          }
          else 
          {
            dsat_qccnti_tech = DSAT_CNTI_GPRS;
          }
        }
        else if ( (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_WCDMA) ||
                  (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_TDS) )
        {
          if(event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSDPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSDPA;
          }
          else if (event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSUPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSUPA;
          }
          else if (event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSDPA_HSUPA;
          }
          else
          {
            dsat_qccnti_tech = DSAT_CNTI_UMTS;
          }
        }
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
    }
    /* Update CS only registration */
    else if (SYS_SRV_DOMAIN_CS_ONLY == domain)
    {
      /* Set CS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info[stack_id].srv_status )
      {
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_GSM )
        {
          dsat_qccnti_tech = DSAT_CNTI_GSM;
        }
        else if ( (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_WCDMA)||
                  (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_TDS) )
        {
          dsat_qccnti_tech = DSAT_CNTI_UMTS;
        }
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,0,0,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    }
    /* Update PS only registration */
    else if (SYS_SRV_DOMAIN_PS_ONLY == domain)
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,0,1,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    
      /* Set PS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info[stack_id].srv_status )
      {
      
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_GSM)
        {
          if ( event_ptr->event_info[stack_id].cell_srv_ind.egprs_supp 
                                          == SYS_EGPRS_SUPPORT_AVAIL )
          {
            dsat_qccnti_tech = DSAT_CNTI_EGPRS;
          }
          else 
          {
            dsat_qccnti_tech = DSAT_CNTI_GPRS;
          }
        }
        else if ( (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_WCDMA) ||
                  (event_ptr->event_info[stack_id].sys_mode == SYS_SYS_MODE_TDS) )
        {
          if(event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSDPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSDPA;
          }
          else if (event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSUPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSUPA;
          }
          else if (event_ptr->event_info[stack_id].cell_srv_ind.hs_ind == SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL)
          {
            dsat_qccnti_tech = DSAT_CNTI_HSDPA_HSUPA;
          }
          else
          {
            dsat_qccnti_tech = DSAT_CNTI_UMTS;
          }
        }
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
    }

#ifdef FEATURE_DSAT_ETSI_MODE

    if( SYS_SRV_STATUS_SRV == event_ptr->event_info[stack_id].srv_status )
    {
      #define COPS_NULL_PLMN 0xFF

      if( COPS_NULL_PLMN != event_ptr->event_info[stack_id].sys_id.id.plmn.identity[0] )
      {
         /* Update PLMN for emergency list update*/
         (void) dsatutil_memscpy((void*)&ph_ss_ms_val->dsat_cen_num_list.plmn, 
                                  sizeof(sys_plmn_id_s_type),
                                 (void*)&event_ptr->event_info[stack_id].sys_id.id.plmn,
               sizeof(sys_plmn_id_s_type));
         ph_ss_ms_val->dsat_cen_num_list.plmn_num_flag |= DSAT_CEN_PLMN_PRESENT;
      }

      /* Update service indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE, 
                                               DSAT_CIND_BINARY_SET,subs_id );
    }
    else
    {
      /* Update service indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE,
                                               DSAT_CIND_BINARY_UNSET,subs_id  );
    }

    /* Update +CIND status indicators */
    (void)dsatetsime_change_indicator_state(
              DSAT_CIND_INDEX_ROAM,
              ((SYS_ROAM_STATUS_OFF == roaming)? 
                DSAT_CIND_BINARY_UNSET : DSAT_CIND_BINARY_SET),subs_id  );
    (void)dsatetsime_change_indicator_state(
              DSAT_CIND_INDEX_PACKET,
              ( ( 1 == (dsat_num_item_type)dsatutil_get_val(
                          DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE) )?
                DSAT_CIND_BINARY_SET : DSAT_CIND_BINARY_UNSET ),subs_id  );
    
#endif /* FEATURE_DSAT_ETSI_MODE */
  }
  /* Update no service state, thus no registration */
  else 
  {
#ifdef FEATURE_ETSI_ATTACH
    /* Set PS attach state */
    DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,0,0,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
#ifdef FEATURE_DSAT_ETSI_MODE

    /* Update service indicator */
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE, 0, subs_id );
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_PACKET, 0, subs_id );
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_ROAM, 0, subs_id );

#endif /* FEATURE_DSAT_ETSI_MODE */

  }

#ifdef FEATURE_DSAT_ETSI_MODE

  cmif_process_cm_net_reg_data(event_ptr, stack_id, subs_id);

  /* Process registration data for CREG and generate URC */
  /* Do not report if +COPS is underway as the lower layers do a few
   * transitions before things settle down. */
  if (DSAT_COPS_ASTATE_NULL == ph_ss_ms_val->dsatetsicall_network_list.cmd_state)
  {
    dsatetsicall_creg_state_machine(event_ptr, stack_id, subs_id);
  } /* Update CELL ID ,LAC ID TAC ID when cops is under process */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_ETSI_ATTACH

#ifdef FEATURE_DSAT_ETSI_DATA
  /* Check if there is a change in the PS state */
  if(dsat_gprs_attach_status[subs_id] != (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE))
  {
    if (dsat_gprs_attach_status[subs_id] == 1)
    {
      /* PS detach: report TE depending on +CGEREP buffer mode settings */
      if((ph_ss_ms_val->dsat_prev_ms_service_preference == CM_SRV_DOMAIN_PREF_CS_PS && 
          ph_ss_ms_val->dsat_ms_service_preference == CM_SRV_DOMAIN_PREF_CS_ONLY) ||
         (dsat_power_off[subs_id] == TRUE) ||
         (ph_ss_ms_val->dsat_ms_service_preference == CM_SRV_DOMAIN_PREF_PS_DETACH))
      {
        /* Detach was initiated by MS */
        dsatetsipkt_report_gprs_event_to_te(
                                         DSAT_MS_INITIATED_DETACH, 
                                         (void *)NULL,
                                         subs_id);
        dsat_power_off[subs_id] = FALSE;
      }
      else
      {
        /* Detach was initiated by N/W */
        dsatetsipkt_report_gprs_event_to_te(
                               DSAT_NW_INITIATED_DETACH, 
                               (void *)NULL,
                               subs_id);
      }
    }

    dsat_gprs_attach_status[subs_id] = (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE);
  }
#endif /* FEATURE_DSAT_ETSI_DATA */
#endif /* FEATURE_ETSI_ATTACH */

  return result;
} /* cmif_process_cm_reg_data_ex */

#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_HYBR_REG_DATA_EX

DESCRIPTION
  This function processes the Call Manager registration event data to
  set general network registration state information of hybrid stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_process_cm_hybr_reg_data_ex
(
  const ds_at_cm_ss_event_type *event_ptr,        /* DS event pointer */
  dsat_stack_id_e_type          stack_id,
  sys_modem_as_id_e_type        subs_id
)
{
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  if ( ( NULL == event_ptr ) ||
       ( FALSE == IS_VALID_STACK_ID(stack_id) ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG3_ERROR("Invalid Arg: event_ptr = %p, stack_id = %d, subs_id = %d",
                     event_ptr, stack_id, subs_id);
    return;
  }

  DS_AT_MSG4_HIGH("Processing hybrid registration update: chgfld=%p %p srv=%d dom=%d",
                  ((event_ptr->event_info[stack_id].changed_fields) >> 32),
                  event_ptr->event_info[stack_id].changed_fields,
                  event_ptr->event_info[stack_id].srv_status,
                  event_ptr->event_info[stack_id].srv_domain);

  cmif_process_cm_net_reg_data(event_ptr, stack_id, subs_id);

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if (NULL == ph_ss_ms_val)
  {
    return;
  }

  /* Process registration data for CREG and generate URC */
  /* Do not report if +COPS is underway as the lower layers do a few
   * transitions before things settle down. */
  if (DSAT_COPS_ASTATE_NULL == ph_ss_ms_val->dsatetsicall_network_list.cmd_state)
  {
    dsatetsicall_creg_state_machine(event_ptr, stack_id, subs_id);
  } /* Update CELL ID ,LAC ID TAC ID when cops is under process */


  return;
} /* cmif_process_cm_hybr_reg_data_ex */
#endif /* FEATURE_SGLTE */

/*===========================================================================

FUNCTION  CMIF_REPORT_MODE_EX

DESCRIPTION
  This function Reports System Mode between No Service and Full Service

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_report_mode_ex
(
  sys_srv_status_e_type     srv_status,
  sys_srv_domain_e_type     srv_domain,
  sys_sys_mode_e_type       sys_mode,
  sys_modem_as_id_e_type    subs_id
)
{
  dsat_num_item_type            mode = sys_mode;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if (NULL == ph_ss_ms_val)
  {
    return;
  }

  /* When in FULL_SRV, the Domain should be valid  */
  if( (srv_status == SYS_SRV_STATUS_SRV) &&
     ( srv_domain != SYS_SRV_DOMAIN_CS_ONLY ) &&
     ( srv_domain != SYS_SRV_DOMAIN_CS_PS ) &&
     ( srv_domain != SYS_SRV_DOMAIN_PS_ONLY ) )
  {
    return;
  }
  else
  {
    if( srv_status != SYS_SRV_STATUS_SRV )
    {
      mode = AT_MODE_NONE;
    }
#ifdef FEATURE_TDSCDMA
   /* TDS mode in spec is different from SYS_SYS_MODE_TDS  */
    if((srv_status == SYS_SRV_STATUS_SRV) && 
       (sys_mode == SYS_SYS_MODE_TDS))
    {
      mode = AT_MODE_TDS;
    }
#endif /* FEATURE_TDSCDMA */

    /* Report only the change in the mode. */
    if (ph_ss_ms_val->dsat_mode_urc_info.mode_val != mode)
    {
      ph_ss_ms_val->dsat_mode_urc_info.mode_val    = mode;
      ph_ss_ms_val->dsat_mode_urc_info.report_flag = TRUE;
    }
  }
}/* cmif_report_mode_ex */

#ifdef FEATURE_DSAT_EXTENDED_CMD
/*===========================================================================

FUNCTION  CMIF_REPORT_MODE_3GPP2_EX

DESCRIPTION
  This function reports 3GPP2 system mode when it is changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_report_mode_3gpp2_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  dsm_item_type                *res_buff_ptr = NULL;
  sys_sys_mode_e_type           sys_mode = ss_info_ptr->sys_mode;
  dsatcmif_servs_state_ss_info *ph_ss_val = NULL;

  if ( NULL == ss_info_ptr )
  {
    return;
  }

  /* If hybrid is turned ON and any protcol running 
        on the hybrid stack(Secondary) then the sys_mode would be Hybrid 
        otherwise the sys_mode based on the primary stack. */
  if ( TRUE == ss_info_ptr->hdr_hybrid )
  {
    if ( SYS_SRV_STATUS_SRV == ss_info_ptr->hdr_srv_status )
    {
      /* Include HDR in the system mode if HDR has service */
      if ( SYS_SYS_MODE_CDMA == sys_mode )
      {
        /* Type casting it to sys mode e type as hybrid 
          is not available in sys mode tyep */
        sys_mode = (sys_sys_mode_e_type)DSAT_SYS_HYBRID;
      }
      else
      {
        sys_mode = SYS_SYS_MODE_HDR;
      }
    }
  }
  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
  if (NULL == ph_ss_val)
  {
    return;
  }
  ph_ss_val->dsat_sys_mode = sys_mode;

  return;
} /* cmif_report_mode_3gpp2_ex */
#endif /* FEATURE_DSAT_EXTENDED_CMD */

#if defined(FEATURE_DSAT_SP_CMDS)
/*===========================================================================

FUNCTION  CMIF_DETERMINE_SPSERVICE

DESCRIPTION
  This function determines current service capabilities for $SPSERVICE command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_determine_spservice
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  if ( NULL == ss_info_ptr )
  {
    return;
  }

  switch ( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      dsat_spservice = DSAT_SRV_CDMA;
      break;

    case SYS_SYS_MODE_HDR:
      switch ( ss_info_ptr->hdr_active_prot )
      {
        case SYS_ACTIVE_PROT_HDR_REL0:
          dsat_spservice = DSAT_SRV_EVDO;
          break;

        case SYS_ACTIVE_PROT_HDR_RELA:
          dsat_spservice = DSAT_SRV_EVDO_REV_A;
          break;

        default:
          dsat_spservice = DSAT_SRV_NONE;
          break;
      }
      break;

    default:
      dsat_spservice = DSAT_SRV_NONE;
      break;
  }

  return;
} /* cmif_determine_spservice */
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================

FUNCTION  CMIF_PROCESS_RSSI_INFO_3GPP

DESCRIPTION
  This function processes RSSI information for 3GPP mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmif_process_rssi_info_3gpp
(
  const ds_at_cm_ss_event_type *event_ptr, /* DS event pointer */
  dsat_stack_id_e_type          stack_id,
  sys_modem_as_id_e_type        subs_id
)
{
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if ( ( NULL == event_ptr ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) ||
       ( FALSE == IS_VALID_STACK_ID(stack_id) ) ||
       ( NULL == ph_ss_ms_val ) )
  {
    DS_AT_MSG3_ERROR("Invalid Arg: event_ptr = %p, stack_id = %d, subs_id = %d",
                     event_ptr, stack_id, subs_id);
    return;
  }

  dsatutil_memscpy(&ph_ss_ms_val->dsatcmif_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type),
                   &event_ptr->event_info[stack_id].dsatcmif_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type));

  DS_AT_MSG8_MED("stack_id = %d, rssi = %d, rssi2 = %d, rscp = %d, "
                 "bit_err_rate = %d, ecio = %d, pathloss = %d, sir = %d",
                 stack_id,
                 ph_ss_ms_val->dsatcmif_signal_reporting.rssi,
                 ph_ss_ms_val->dsatcmif_signal_reporting.rssi2,
                 ph_ss_ms_val->dsatcmif_signal_reporting.rscp,
                 ph_ss_ms_val->dsatcmif_signal_reporting.bit_err_rate,
                 ph_ss_ms_val->dsatcmif_signal_reporting.ecio,
                 ph_ss_ms_val->dsatcmif_signal_reporting.pathloss,
                 ph_ss_ms_val->dsatcmif_signal_reporting.sir);

  (void)dsatetsicmif_process_rssi_event(event_ptr, stack_id, subs_id);

  return;
} /* cmif_process_rssi_info_3gpp */

/*===========================================================================

FUNCTION  CMIF_PROCESS_RSSI_INFO_3GPP2

DESCRIPTION
  This function processes RSSI information for 3GPP2 mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_process_rssi_info_3gpp2
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  dsatcmif_servs_state_ss_info *ph_ss_val = NULL;
#ifdef FEATURE_DSAT_EXTENDED_CMD
  uint16                        curr_hdr_rssi_intensity_level = 0;
  static uint16                 prev_hdr_rssi_intensity_level = 0;
  dsm_item_type                *res_buff_ptr = NULL;
#endif /* FEATURE_DSAT_EXTENDED_CMD */

  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
  if ((NULL == ss_info_ptr) || (NULL == ph_ss_val))
  {
    return;
  }

  dsatutil_memscpy(&ph_ss_val->dsatcmif_cdma_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type),
                   &ss_info_ptr->dsatcmif_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type));

  DS_AT_MSG3_MED("rssi = %d, hdr_hybrid = %d, hdr_rssi = %d",
                 ph_ss_val->dsatcmif_cdma_signal_reporting.rssi,
                 ph_ss_val->dsatcmif_cdma_signal_reporting.hdr_hybrid,
                 ph_ss_val->dsatcmif_cdma_signal_reporting.hdr_rssi);

#ifdef FEATURE_DSAT_EXTENDED_CMD
  curr_hdr_rssi_intensity_level = dsatcmif_cm_get_hdr_rssi_intensity_level();

  if ( curr_hdr_rssi_intensity_level != prev_hdr_rssi_intensity_level )
  {
    /* Need to report HDR RSSI intensity level*/
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL != res_buff_ptr)
    {
      res_buff_ptr->used = (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "^HRSSILVL:%d",
                                           curr_hdr_rssi_intensity_level);

      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
    }
    prev_hdr_rssi_intensity_level = curr_hdr_rssi_intensity_level;
  }
#endif /* FEATURE_DSAT_EXTENDED_CMD */

  return;
} /* cmif_process_rssi_info_3gpp2 */

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
/*===========================================================================

FUNCTION  CMIF_PROCESS_CQI_INFO_EX

DESCRIPTION
  This function processes CQI information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmif_process_cqi_info_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  dsatcmif_servs_state_ss_info *ph_ss_val = NULL;


  if(ss_info_ptr->sys_mode != SYS_SYS_MODE_GSM &&
     ss_info_ptr->sys_mode != SYS_SYS_MODE_WCDMA &&
     ss_info_ptr->sys_mode != SYS_SYS_MODE_GW )
  {
    DS_AT_MSG0_ERROR("Unsupported system mode");
    return;
  }
  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
  if ((NULL == ss_info_ptr) || (NULL == ph_ss_val))
  {
    return;
  }

  (void) dsatutil_memscpy((void*)&ph_ss_val->dsat_cqi_info,
                          sizeof(cm_cqi_info_s_type),
                          (void*)&ss_info_ptr->dsat_cqi_info,
                          sizeof(cm_cqi_info_s_type));

  return;
} /* cmif_process_cqi_info_ex */
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION  CMIF_PROCESS_EMERG_LIST_EX

DESCRIPTION
  This function processes the Call Manager registration event data to
  set PS attach and general network registration state information.

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
LOCAL void cmif_process_emerg_list_ex
(
  ds_at_ss_info_s_type   *event_info_ptr,        /* DS event info pointer */
  sys_modem_as_id_e_type  subs_id
)
{
  dsat_num_item_type            emerg_list_len = 0;
  dsat_num_item_type            index =0;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  if ( ( NULL == event_info_ptr ) ||
       ( FALSE == IS_VALID_SUBS_ID(subs_id) ) )
  {
    DS_AT_MSG2_ERROR("Invalid Arg: event_ptr = %p, subs_id = %d",
                     event_info_ptr, subs_id);
    return;
  }

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS,subs_id, TRUE);
  if( NULL == event_info_ptr->emerg_num_list || 0 == event_info_ptr->emerg_num_list->num_list_len ||
      NULL == ph_ss_ms_val )
  {
    DS_AT_MSG0_HIGH("Emergency list is empty ");
    return;
  }
  
  emerg_list_len  =  event_info_ptr->emerg_num_list->num_list_len;
    
  DS_AT_MSG3_HIGH("Emergency New list count %d flag %d old count %d ",
     emerg_list_len,ph_ss_ms_val->dsat_cen_num_list.plmn_num_flag,ph_ss_ms_val->dsat_cen_num_list.num_list_len);
  
  if(ph_ss_ms_val->dsat_cen_num_list.plmn_num_flag & DSAT_CEN_NUMBER_PRESENT)
  {
    /* Previous list is present.Remove old list .Update new list */
    /* browse whole old list and free memory till last index */

    for(index = 0 ;index < ph_ss_ms_val->dsat_cen_num_list.num_list_len && index < CM_MAX_EMERGENCY_NUM_COUNT;index++)
    {
      if (NULL != ph_ss_ms_val->dsat_cen_num_list.cen_num[index])
      {
        dsatutil_free_memory(ph_ss_ms_val->dsat_cen_num_list.cen_num[index]);
      }
    }
    
  }
  /* Re allocate memory and copy number */

  ph_ss_ms_val->dsat_cen_num_list.plmn_num_flag |= DSAT_CEN_NUMBER_PRESENT;
  ph_ss_ms_val->dsat_cen_num_list.num_list_len = MIN (emerg_list_len, CM_MAX_EMERGENCY_NUM_COUNT);
  
   /* Allocate memory for Emergency numbers */
  for(index = 0;(index < ph_ss_ms_val->dsat_cen_num_list.num_list_len) ;index++ )
  {
    ph_ss_ms_val->dsat_cen_num_list.cen_num[index] = (dsat_cen_emerg_num_s_type *)dsat_alloc_memory(sizeof(dsat_cen_emerg_num_s_type), TRUE);
    if (NULL == ph_ss_ms_val->dsat_cen_num_list.cen_num[index])
    {
      return;
    }

    ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->cat = 
                       (byte)event_info_ptr->emerg_num_list->num_list[index].num_type;
    ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->len =
                       MIN(CM_MAX_NUMBER_CHARS,event_info_ptr->emerg_num_list->num_list[index].num.len);

    (void) dsatutil_memscpy((void*)&ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->buf[0], 
                             CM_MAX_NUMBER_CHARS,
                            (void*)&event_info_ptr->emerg_num_list->num_list[index].num.buf[0],
                             MIN(CM_MAX_NUMBER_CHARS,ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->len));
    
    DS_AT_MSG3_HIGH("Rec EMERG num cat %d len %d index %d",ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->cat,
                                ph_ss_ms_val->dsat_cen_num_list.cen_num[index]->len,index);
  }

  event_info_ptr->report_emerg_num_list = TRUE;

  return;
} /*cmif_process_emerg_list_ex*/

/*===========================================================================

FUNCTION DSATCMIF_GET_OPRT_MODE

DESCRIPTION
  This function return cfun operating mode status.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

sys_oprt_mode_e_type dsatcmif_get_oprt_mode(void)
{
  dsatcmif_servs_state_ss_info  *ph_ss_val = NULL;
  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, TRUE);
  if (NULL == ph_ss_val)
  {
    return SYS_OPRT_MODE_ONLINE;
  }
   return ph_ss_val->dsat_oprt_mode;
   
}/*dsatcmif_get_dsat_oprt_mode */

/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_HIWATER_CB

DESCRIPTION
  This is the high watermark function registered with the watermark for 
  flow control.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_hiwater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_hiwater_cb ( void )
#endif
{
#ifdef IMAGE_QDSP6_PROC
  rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  if( FALSE == dsat_cm_msg_wm_full )
  {
    /* Set flag indicating received messages should be dropped */
    dsat_cm_msg_wm_full = TRUE;
    DS_AT_MSG0_HIGH("Enable flow control on CM event queue");
  }  
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

}


/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_LOWATER_CB

DESCRIPTION
  This is the low watermark function registered with the watermark for 
  flow control.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_lowater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_lowater_cb ( void )
#endif
{
#ifdef IMAGE_QDSP6_PROC
  rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  if( TRUE == dsat_cm_msg_wm_full )
  {
    /* Set flag indicating received messages should no longer be dropped */
    dsat_cm_msg_wm_full = FALSE;
    DS_AT_MSG0_HIGH("Disable flow control on CM event queue");
  }
#ifdef IMAGE_QDSP6_PROC
  rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

}


/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the received message watermark goes non-empty.

  This function simply sets the event received subtask signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_non_empty_cb
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_non_empty_cb ( void )
#endif
{
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs( rcinit_lookup_rextask("ds"), DS_AT_CM_MSG_SIG );
#else
  (void)rex_set_sigs( &ds_tcb, DS_AT_CM_MSG_SIG );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* cmif_cm_msg_wm_non_empty_cb */



/*===========================================================================
FUNCTION  CMIF_SETUP_MSG_WATERMARKS

DESCRIPTION
  This function initializes the event watermarks used for receiving event 
  messages from CM. The lowater, hiwater and don't exceed counts are 
  configured. Queue to be used by WM is also initialized

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void cmif_setup_msg_watermarks( void )
{
#ifdef IMAGE_QDSP6_PROC
        rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
        INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  dsat_cm_msg_wm_full = FALSE;
#ifdef IMAGE_QDSP6_PROC
      rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
      INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

  /*-------------------------------------------------------------------------
    Initialize , set Don't Exceed count and register the associated queue
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_cm_msg_wm, DSAT_FROM_CM_DONT_EXCEED_CNT, &dsat_cm_msg_q);

  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water  counts for the watermark.
  -------------------------------------------------------------------------*/
  dsat_cm_msg_wm.lo_watermark    =  DSAT_FROM_CM_LOWATER_CNT;
  dsat_cm_msg_wm.hi_watermark    =  DSAT_FROM_CM_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Register callback functions for the watermark flow control.
  -------------------------------------------------------------------------*/
  /*lint -save -e64 suppress error 64*/
  dsat_cm_msg_wm.lowater_func_ptr      = cmif_cm_msg_wm_lowater_cb;
  dsat_cm_msg_wm.hiwater_func_ptr      = cmif_cm_msg_wm_hiwater_cb;
  dsat_cm_msg_wm.non_empty_func_ptr    = cmif_cm_msg_wm_non_empty_cb;
  /*lint -restore suppress error 64*/


} /* cmif_setup_msg_watermarks */



/*===========================================================================

FUNCTION  CMIF_REGISTER_CB_FUNC_EX

DESCRIPTION
  Registers the callback functions.with other tasks.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_register_cb_func_ex ( void )
{
  cm_client_status_e_type cm_status = CM_CLIENT_OK;

  /* Register the CM call event callback function */
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status | 
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_ORIG,
                                  CM_CALL_EVENT_CONNECT,
                                  NULL ));
    cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_ACT_BEARER_IND,
                                  CM_CALL_EVENT_RES_ALLOC_FAIL_IND,
                                  NULL ));
    cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_ABRV_ALERT,
                                  CM_CALL_EVENT_ABRV_ALERT,
                                  NULL ));

cm_status = (cm_client_status_e_type)
                ((int32)cm_status |
                 (int32)cm_mm_client_call_reg(
                                 dsatcm_client_id,
                                 cmif_call_event_cb_func,
                                 CM_CLIENT_EVENT_REG,
                                 CM_CALL_EVENT_HO_COMPLETE,
                                 CM_CALL_EVENT_SRVCC_COMPLETE_IND,
                                 NULL
                                ));

/*For new interface CM for SGLTE+G*/
cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_ss_reg_msim (
                             dsatcm_client_id,
                             cmif_ss_event_msim_cb_func,
                             CM_CLIENT_EVENT_REG,
                             CM_SS_EVENT_SRV_CHANGED,
                             CM_SS_EVENT_INFO,
                             NULL,
                             DSAT_ASUBS_MASK
                            ));

cm_status = (cm_client_status_e_type)
                ((int32)cm_status |
                   (int32)cm_mm_client_ss_reg_msim (
                             dsatcm_client_id,
                             cmif_ss_event_msim_cb_func,
                             CM_CLIENT_EVENT_REG,
                             CM_SS_EVENT_EMERG_NUM_LIST,
                             CM_SS_EVENT_EMERG_NUM_LIST ,
                             NULL,
                             DSAT_ASUBS_MASK
                            ));
cm_status = (cm_client_status_e_type)
                ((int32)cm_status |
                   (int32)cm_mm_client_ss_reg_msim (
                             dsatcm_client_id,
                             cmif_ss_event_msim_cb_func,
                             CM_CLIENT_EVENT_REG,
                             CM_SS_EVENT_REG_REJECT,
                             CM_SS_EVENT_REG_REJECT ,
                             NULL,
                             DSAT_ASUBS_MASK
                            ));

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  /* Register for CM_CALL_EVENT_CALLER_ID */
  cm_status = (cm_client_status_e_type)
                ((int32)cm_status | 
                (int32)cm_mm_client_call_reg(  dsatcm_client_id,
                                               cmif_call_event_cb_func,
                                               CM_CLIENT_EVENT_REG,
                                               CM_CALL_EVENT_CALLER_ID,
                                               CM_CALL_EVENT_CALLER_ID,
                                               NULL ));
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

cm_status = (cm_client_status_e_type)
                ((int32)cm_status |
                 (int32)cm_client_ph_reg(
                           dsatcm_client_id,
                           cmif_cm_ph_event_cb_func,
                           CM_CLIENT_EVENT_REG,
                           CM_PH_EVENT_STANDBY_SLEEP,
                           CM_PH_EVENT_PS_ATTACH_FAILED,
                           NULL));

  cm_status = (cm_client_status_e_type)
                ((int32)cm_status |
                 (int32)cm_client_ph_reg(
                           dsatcm_client_id,
                           cmif_cm_ph_event_cb_func,
                           CM_CLIENT_EVENT_REG,
                           CM_PH_EVENT_OPRT_MODE,
                           CM_PH_EVENT_SYS_SEL_PREF,
                           NULL));

  cm_status = (cm_client_status_e_type)
                ((int32)cm_status | 
                 (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                cmif_call_event_cb_func,
                                CM_CLIENT_EVENT_REG,
                                CM_CALL_EVENT_PROGRESS_INFO_IND,
                                CM_CALL_EVENT_PROGRESS_INFO_IND,
                                NULL));

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status | 
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_SETUP_IND,
                                  CM_CALL_EVENT_CALL_CONF,
                                  NULL ));
  
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_MNG_CALLS_CONF,
                                  CM_CALL_EVENT_INCOM_FWD_CALL,
                                  NULL));  

    /* Register for blocks of supserv events */
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_client_sups_reg(
                               dsatcm_client_id,
                               dsatetsicmif_sups_event_cb_func,
                               CM_CLIENT_EVENT_REG,
                               CM_SUPS_EVENT_REGISTER,
                               CM_SUPS_EVENT_GET_PASSWORD_RES,
                               NULL
                              ));

  /* This should not happen, raise an error */
  if (cm_status!= CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to register SUPS events: %d",
               (int)cm_status, 0,0 );
  }

#ifdef FEATURE_DATA_WCDMA_PS
  /* If WCDMA PS data available register for RAB events so that dormant
     contexts can be handled by $QCDGEN command */
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_call_reg( dsatcm_client_id,
                                      cmif_call_event_cb_func,
                                      CM_CLIENT_EVENT_REG,
                                      CM_CALL_EVENT_RAB_REESTAB_IND,
                                      CM_CALL_EVENT_RAB_REESTAB_IND,
                                      NULL ));
#endif /* FEATURE_DATA_WCDMA_PS */

#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_DUAL_ACTIVE
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_call_reg(dsatcm_client_id,
                                                cmif_call_event_cb_func,
                                                CM_CLIENT_EVENT_REG,
                                                CM_CALL_EVENT_LOCAL_HOLD,
                                                CM_CALL_EVENT_LOCAL_HOLD,
                                                NULL));
#endif /* FEATURE_DUAL_ACTIVE */

#ifdef FEATURE_DATA_IS707

 cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_mm_client_ss_reg_msim (
                                     dsatcm_client_id,
                                     cmif_ss_event_msim_cb_func,
                                     CM_CLIENT_EVENT_REG,
                                     CM_SS_EVENT_HDR_RSSI,
                                     CM_SS_EVENT_HDR_RSSI,
                                     NULL,
                                     DSAT_ASUBS_MASK
                              ));
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_SGLTE
	cm_status = (cm_client_status_e_type)
		       ((int32)cm_status |
			 (int32)cm_client_ph_reg( dsatcm_client_id,
						cmif_cm_ph_event_cb_func,
						CM_CLIENT_EVENT_REG,
						CM_PH_EVENT_SIM_AVAILABLE,
						CM_PH_EVENT_SIM_AVAILABLE,
						NULL ) );
#endif/* FEATURE_SGLTE*/

  /* This should not happen, raise an error */
  if (cm_status!= CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to register CM events: %d",
               (int)cm_status, 0,0 );
  }

  /*-----------------------------------------------------------------------
    Activate the registered callback functions.
  -----------------------------------------------------------------------*/
  cm_status = cm_client_act( dsatcm_client_id );
  
  if (cm_status!= CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to activate client: %d",
               (int)cm_status, 0,0 );
  }

  return;
}/* cmif_register_cb_func_ex */

/*===========================================================================

FUNCTION  CMIF_START_S_SEVEN_TIMER

DESCRIPTION
  This function Starts S7 timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_start_s_seven_timer
( 
  sys_modem_as_id_e_type  subs_id,
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         cm_call_id,
  cm_client_type_e_type   client_id
)
{
  dsat_timer_s_type *t_ptr = NULL;
  sys_sys_mode_e_type  current_mode = SYS_SYS_MODE_NO_SRV;

  dsatetsicall_call_state_ss_info * call_ss_val = NULL;
  
  call_ss_val = dsat_get_base_addr(DSAT_CALL_SS_VALS, TRUE);
  if (NULL == call_ss_val)
  {
    return;
  }

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    return;
  }

  current_mode = ds3g_get_current_network_mode_ex(subs_id);

  if( ( client_id == (cm_client_type_e_type)dsatcm_client_id ) && 
      ( call_type == CM_CALL_TYPE_VOICE ))
  {
    t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT]; 

#ifdef FEATURE_DUAL_ACTIVE
       if (SYS_MODEM_AS_ID_2 == subs_id)
       {
         t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT_2];
       }
#endif /* FEATURE_DUAL_ACTIVE */

    ASSERT(t_ptr->user_data != NULL);

    if ( t_ptr->is_running == TRUE )
    {
      DS_AT_MSG0_HIGH(" Ignoring Start of new S7 timer ");
      return;
    }

    (void)rex_set_timer( t_ptr->timer_ptr, 
                         (dsat_num_item_type)dsatutil_get_val(
                         DSAT_SREG_S7_IDX,0,0,NUM_TYPE) * 1000 ); 
    t_ptr->is_running = TRUE;
    *(cm_call_id_type *)(t_ptr->user_data) = cm_call_id;
    DS_AT_MSG2_HIGH("VOICE S7 Timer Started for Call ID %d - Duration %d",
              cm_call_id,
              (dsat_num_item_type)dsatutil_get_val(
              DSAT_SREG_S7_IDX,0,0,NUM_TYPE));
  }
  else if( ( call_type == CM_CALL_TYPE_PS_DATA )||
           ( call_type == CM_CALL_TYPE_CS_DATA ))
  {
    if( ( client_id == (cm_client_type_e_type)dsatcm_client_id )||
        (TRUE == ds3g_msh_is_atcop_call( call_type,cm_call_id,
                    current_mode)) )
    {
      t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT];
      if (t_ptr->is_running == FALSE)
      {
        call_ss_val->dsatetsicall_s7_info[0].profile_id = profile_id;
        call_ss_val->dsatetsicall_s7_info[0].subs_id    = subs_id;
      }
      else
      {
        t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT_2];
        if ( t_ptr->is_running == TRUE )
        {
          DS_AT_MSG0_HIGH(" Ignoring Start of new S7 timer ");
          return;
        }
        call_ss_val->dsatetsicall_s7_info[1].profile_id = profile_id;
        call_ss_val->dsatetsicall_s7_info[1].subs_id    = subs_id;
      }
      ASSERT(t_ptr->user_data != NULL);

      (void)rex_set_timer( t_ptr->timer_ptr, 
                          (dsat_num_item_type)dsatutil_get_val(
                           DSAT_SREG_S7_IDX,0,0,NUM_TYPE) * 1000 );
      t_ptr->is_running = TRUE;
      *(cm_call_id_type *)(t_ptr->user_data) = cm_call_id;
      DS_AT_MSG2_HIGH("DATA S7 Timer Started for Call ID %d - Duration %d",
                 cm_call_id,
                (dsat_num_item_type)dsatutil_get_val(
                DSAT_SREG_S7_IDX,0,0,NUM_TYPE));
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Ignoring S7 Start request for call id %d ", cm_call_id);
  }
  return;
}/* cmif_start_s_seven_timer */

/*===========================================================================

FUNCTION  CMIF_STOP_S_SEVEN_TIMER

DESCRIPTION
  This functions clears the S7 timer if running.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_stop_s_seven_timer
( 
  sys_modem_as_id_e_type  subs_id,
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         cm_call_id,
  cm_client_type_e_type   client_id
)
{
  dsat_timer_s_type *t_ptr = NULL;
  dsatetsicall_call_state_ss_info * call_ss_val = NULL;
  uint8 t_idx;

  call_ss_val = dsat_get_base_addr(DSAT_CALL_SS_VALS, TRUE);
  if (NULL  == call_ss_val)
  {
    return;
  }

  if(( call_type == CM_CALL_TYPE_VOICE ) &&
     (voice_state[subs_id][cm_call_id].state == DSAT_VOICE_STATE_CONNECTING ))
  {
    t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT];

#ifdef FEATURE_DUAL_ACTIVE
    if (SYS_MODEM_AS_ID_2 == subs_id)
    {
      t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT_2];
    }
#endif /* FEATURE_DUAL_ACTIVE */

    if(( t_ptr->is_running) && ( *(cm_call_id_type *)(t_ptr->user_data) == cm_call_id))
    {
     (void)rex_clr_timer(t_ptr->timer_ptr);
      t_ptr->is_running = FALSE;
      DS_AT_MSG1_HIGH("VOICE S7 Timer Stopped for Call ID - %d",cm_call_id);
    }
  }
  else if( ( call_type == CM_CALL_TYPE_PS_DATA )||
           ( call_type == CM_CALL_TYPE_CS_DATA ) )
  {
    for (t_idx = 0; t_idx < DSAT_MAX_DATA_CALLS; t_idx++)
    {
      if (call_ss_val->dsatetsicall_s7_info[t_idx].subs_id == subs_id)
      {
        /* Call is profile and subs based */
        if ((FALSE == call_ss_val->dsatetsicall_s7_info[t_idx].is_profile_id_valid) ||
            (call_ss_val->dsatetsicall_s7_info[t_idx].profile_id == profile_id))
        {
          break;
        }
      }
    }
    
    if (t_idx != DSAT_MAX_DATA_CALLS)
    {
      if (0 == t_idx)
      {
        t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT];
      }
      else
      {
        t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT_2];
      }
      call_ss_val->dsatetsicall_s7_info[t_idx].data_s7_abort_handler = NULL;
      call_ss_val->dsatetsicall_s7_info[t_idx].is_profile_id_valid = FALSE;
      call_ss_val->dsatetsicall_s7_info[t_idx].subs_id = DSAT_SYS_MODEM_AS_ID_MAX;
    }

    if ( (NULL != t_ptr) && (NULL != t_ptr->user_data) && 
          (cm_call_id == *(cm_call_id_type *)(t_ptr->user_data)) )
    {
      if( t_ptr->is_running )
      {
        (void)rex_clr_timer(t_ptr->timer_ptr);
        t_ptr->is_running = FALSE;
        DS_AT_MSG1_HIGH("DATA S7 Timer Stopped for Call ID - %d",cm_call_id);
      }
      *(cm_call_id_type *)(t_ptr->user_data) = CM_CALL_ID_INVALID;
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Ignoring S7 stop request for call id %d ",cm_call_id);
  }

  return;
}/* cmif_stop_s_seven_timer */

/*===========================================================================

FUNCTION  DSATCMIF_IS_VOICE_CALL_TYPE

DESCRIPTION
  This function determines if the given call is voice call or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatcmif_is_voice_call_type( cm_call_type_e_type type)
{
  if(   (CM_CALL_TYPE_VOICE == type)
     || (CM_CALL_TYPE_EMERGENCY == type) 
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
     || (CM_CALL_TYPE_STD_OTASP == type) 
     || (CM_CALL_TYPE_NON_STD_OTASP == type) 
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* dsatcmif_is_voice_call_type */
/*===========================================================================
FUNCTION DSATCMIF_INITIALIZE_TO_DEFAULT

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_initialize_to_default( dsat_mem_type mem_type,void* ptr)
{
#define INVALID_TZ 97
  dsat_num_item_type i = 0;
  
  if(NULL == ptr)
  {
    DS_AT_MSG0_ERROR("NULL pointer");
    return;
  }
  switch(mem_type)
  {
    case DSAT_SS_PH_SS_VALS:
     {
       dsatcmif_servs_state_ss_info  *ss = (dsatcmif_servs_state_ss_info  *)ptr;
       ss->cm_ph_deep_sleep = FALSE;
       ss->dsat_oprt_mode = SYS_OPRT_MODE_NONE;
#if defined(FEATURE_HS_USB_SER3_PORT)
       ss->dsatcmif_cm_pkt_state = DS_CALL_STATUS_NONE;
       ss->dsatcmif_dormant_ind_report_enabled = FALSE;
#endif /* FEATURE_HS_USB_SER3_PORT */
       memset ((void *)&ss->dsatcmif_cdma_signal_reporting, 0x0, sizeof(dsatcmif_sigrep_s_type));
       ss->dsatcmif_cdma_signal_reporting.rssi = RSSI_NO_SIGNAL;
       ss->dsatcmif_cdma_signal_reporting.sys_mode = SYS_SYS_MODE_NO_SRV;
       ss->dsatcmif_cdma_signal_reporting.hdr_hybrid = FALSE;
       ss->dsatcmif_cdma_signal_reporting.hdr_rssi = HDR_RSSI_NO_SIGNAL;
       
     }
     break;
    case DSAT_SS_PH_MS_VALS:
     {
       dsatcmif_servs_state_ms_info  *ms = (dsatcmif_servs_state_ms_info  *)ptr;
       ms->net_pref.last_dereg_success = TRUE;
       memset ((void *)&ms->dsat_net_reg_state, 0x0,sizeof(dsat_net_reg_state_s_type));
       memset ((void *)&ms->dsat_cen_num_list,0x0,sizeof(dsat_cen_emerg_num_list_s_type));
       memset ((void *)&ms->dsatcmif_signal_reporting, 0x0,sizeof(dsatcmif_sigrep_s_type));
#ifdef FEATURE_DSAT_ETSI_MODE
       ms->dsatcmif_signal_reporting.rssi = RSSI_NO_SIGNAL;
       ms->dsatcmif_signal_reporting.bit_err_rate = DSAT_BER_NO_SIGNAL;
       ms->dsatcmif_signal_reporting.ecio = DSAT_ECIO_NO_SIGNAL;
       ms->dsatcmif_signal_reporting.pathloss = DSAT_PATHLOSS_NO_SIGNAL;
       ms->dsatcmif_signal_reporting.sir = DSAT_SIR_NO_SIGNAL;
       dsatetsicall_reset_last_reported_creg(ms);
#endif /* FEATURE_DSAT_ETSI_MODE */
  /* HDR CSQ Initailzation */
       ms->dsatcmif_signal_reporting.hdr_hybrid = FALSE;
       ms->dsatcmif_signal_reporting.sys_mode = SYS_SYS_MODE_NO_SRV;
#ifdef FEATURE_DATA_IS707
       ms->dsatcmif_signal_reporting.hdr_rssi = HDR_RSSI_NO_SIGNAL;
#endif /* FEATURE_DATA_IS707 */
       ms->dsat_ws46_res_type = DSAT_WS46_NONE;
       ms->dsat_ws46_curr_val = DSAT_WS46_FUNC_MAX;
       ms->last_reported_tz = INVALID_TZ;
       ms->dsat_mode_urc_info.mode_val    = AT_MODE_NONE;
       ms->dsat_mode_urc_info.report_flag = FALSE;
     }
     break;
    case DSAT_CALL_SS_VALS:
     {
       dsatetsicall_call_state_ss_info *ss = (dsatetsicall_call_state_ss_info *)ptr;
       for(i = 0; i < DSAT_MAX_DATA_CALLS; i++)
       {
         ss->dsatetsicall_s7_info[i].profile_id = 0;
         ss->dsatetsicall_s7_info[i].is_profile_id_valid = FALSE;
         ss->dsatetsicall_s7_info[i].subs_id = DSAT_SYS_MODEM_AS_ID_MAX;
         ss->dsatetsicall_s7_info[i].data_s7_abort_handler = NULL;
       }

     }
     break;
    case DSAT_CALL_DA_VALS:
     {
       dsatetsicall_call_state_da_info  *da = (dsatetsicall_call_state_da_info  *)ptr;
       da->etsicall_was_waiting = FALSE;
       da->etsicall_was_call_active = FALSE;
       da->was_active_call_hold = FALSE;
       da->was_hold_call_retrieved = FALSE;
       da->etsicall_was_mpty_call = FALSE;
         /* Initialize to no valid sequence numbers */
       for(i = 0; i< CM_CALL_ID_MAX ; i++)
       {
         da->dsat_seqnum_callid[i].call_id = CM_CALL_ID_INVALID;
         da->dsat_seqnum_callid[i].call_type= CM_CALL_TYPE_NONE;
       }
         /* Initialize +CEER reporting */
       memset((void*)&da->dsatetsicmif_call_end_status, 0x0, 
                             sizeof(dsatetsicmif_ces_info_s_type));
       
     }
     break;
    case DSAT_CALL_MS_VALS:
     {
       dsatetsicall_call_state_ms_info  *ms = (dsatetsicall_call_state_ms_info  *)ptr;
       ms->etsicall_prev_ccm_val = 0;
     }
     break;
    case DSAT_SUPS_SS_VALS:
     {
       
     }
     break;
    case DSAT_SUPS_MS_VALS:
     {
       dsatetsicmif_sups_state_ms_info  *ms = (dsatetsicmif_sups_state_ms_info  *)ptr;
       ms->dsat_ss_abort_status =  FALSE;
       memset ((void*)&ms->dsat_ss_cm_data, 0x0, sizeof(supserv_cm_op_s_type));
       dsatetsicmif_set_cusd_state(&(ms->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
     }
     break;
   default:
    break;
  }
}

/*===========================================================================
FUNCTION  DSAT_CM_PH_SUBS_PREF_HANDLER

DESCRIPTION
  This function is the event handler invoked by CM for informing subscription
  specific info to ATCoP.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_cm_ph_subs_pref_handler
(
  void         * ds_cmd_ptr              /* DS Command pointer         */
)
{
  ds3g_cm_sub_info_type *pref_event_ptr;
  ds_cmd_type           *cmd_ptr = (ds_cmd_type*)ds_cmd_ptr;

  if( NULL == cmd_ptr || NULL == cmd_ptr->cmd_payload_ptr)
  {
    return;
  }

  pref_event_ptr = (ds3g_cm_sub_info_type*)cmd_ptr->cmd_payload_ptr;
  
  DS_AT_MSG4_MED("session_type_1x = %d, session_type_gw = %d, "
                 "subscription_id = %d, is_operational = %d",
                 pref_event_ptr->session_type_1x,
                 pref_event_ptr->session_type_gw,
                 pref_event_ptr->subscription_id,
                 pref_event_ptr->is_operational);

  DS_AT_MSG4_MED("is_default_data = %d, network_sel_mode_pref = %d, is_available_gw = %d,sub_feature = %d",
                 pref_event_ptr->is_default_data,
                 pref_event_ptr->pref_info.network_sel_mode_pref,
                 pref_event_ptr->is_available_gw,
                 pref_event_ptr->sub_feature_mode);

  switch (cmd_ptr->hdr.cmd_id )
  {
    case DS_CMD_CM_PH_EV_SUB_INFO:
      cmif_cm_dual_standby_sub_info_cmd_handler(pref_event_ptr);
      break;

    default:
      DS_AT_MSG1_ERROR("Unknown event %d",cmd_ptr->hdr.cmd_id);
      break;
  }
}

/*===========================================================================
FUNCTION  DSAT_CM_DUAL_STANDBY_SUB_INFO_CMD_HANDLER

DESCRIPTION
  This function is the event handler invoked by CM for informing
  dual standby subscription specific info to ATCoP.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_cm_dual_standby_sub_info_cmd_handler
(
  ds3g_cm_sub_info_type *pref_event_ptr
)
{
  sys_modem_as_id_e_type        subs_id      = pref_event_ptr->subscription_id;
  dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    return;
  }

  dsat_subs_info[subs_id].is_default_data  = pref_event_ptr->is_default_data;
  dsat_subs_info[subs_id].sub_feature_mode = pref_event_ptr->sub_feature_mode;

  if (SYS_SUBS_FEATURE_MODE_SGLTE == pref_event_ptr->sub_feature_mode)
  {
    dsat_subs_info[subs_id].is_sglte_sub = TRUE;

    /*Update SGLTE second stack place holder */
#ifdef FEATURE_SGLTE
    dsatcmif_set_sglte_hybr_subs_id(SYS_MODEM_AS_ID_3);
#endif /* FEATURE_SGLTE */
  }
  else
  {
    dsat_subs_info[subs_id].is_sglte_sub = FALSE;
  }

  if (TRUE == pref_event_ptr->is_operational)
  {
    if (TRUE == pref_event_ptr->is_default_data)
    {
      dsat_set_qcsimapp_val(subs_id);
    }

    if (TRUE == pref_event_ptr->is_available_gw)
    {
      ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, TRUE);
      if (NULL == ph_ss_ms_val)
      {
        return;
      }

      ph_ss_ms_val->net_pref.last_dereg_success = FALSE;
      if(pref_event_ptr->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
      {
        DSATUTIL_SET_VAL( DSATETSI_EXT_ACT_COPS_ETSI_IDX, subs_id, 0, 0,
                          DSAT_COPS_MODE_MANUAL, MIX_NUM_TYPE );
      }
      else
      {
        DSATUTIL_SET_VAL( DSATETSI_EXT_ACT_COPS_ETSI_IDX, subs_id, 0, 0,
                          DSAT_COPS_MODE_AUTO, MIX_NUM_TYPE );
      }
    }
  }
}

/*===========================================================================
FUNCTION CMIF_IS_GW_SUBSCRIPTION_AVAILABLE 

DESCRIPTION
   This function is used to check if we have subscription available
   for the subs_id passed to this function.

DEPENDENCIES
   None

RETURN VALUE
.  TRUE : if the subscription is available for the subs_id passed.
   FALSE: if the subscription is unavailable for the subs_id passed.

SIDE EFFECTS
   None

===========================================================================*/
boolean cmif_is_gw_subscription_available
(
  sys_modem_as_id_e_type     subs_id,
  ds_at_cm_ph_event_type     *ph_event
)
{
  boolean is_gw_subscription_available = FALSE;
  switch(subs_id)
  {
    case SYS_MODEM_AS_ID_1:
    {
      is_gw_subscription_available = ph_event->event_info.is_gw_subscription_available;
      break;
    }

    case SYS_MODEM_AS_ID_2:
    {
      /* For SGLTE+G, is_hybr_gw_subscription_available is used for stack1, SUB1 
         is_hybr_gw_subscription_available is used for stack2,SUB1
         is_hybr_gw3_subscription_available is used for SUB2 */
      if (SYS_SUBS_FEATURE_MODE_NORMAL != dsat_subs_info[SYS_MODEM_AS_ID_1].sub_feature_mode)
      {
        is_gw_subscription_available = ph_event->event_info.is_hybr_gw3_subscription_available;
      }
      else
      {
        is_gw_subscription_available = ph_event->event_info.is_hybr_gw_subscription_available;
      }
      break;
    }

    case SYS_MODEM_AS_ID_3:
    {
      is_gw_subscription_available = ph_event->event_info.is_hybr_gw3_subscription_available;
      break;
    }

    default:
     is_gw_subscription_available = FALSE;
  }
  
  DS_AT_MSG2_HIGH("subs_id: %d,is_gw_subscription_available:%d",
                  subs_id, is_gw_subscription_available);

  return is_gw_subscription_available;
}/* cmif_is_gw_subscription_available */

#ifdef FEATURE_SGLTE

/*===========================================================================

FUNCTION DSATCMIF_GET_SGLTE_HYBR_SUBS_ID
    
DESCRIPTION
   This function used to get hybrid subs_id.
    
DEPENDENCIES
   None
RETURN VALUE
.  dsat_memory_subs_type
    
SIDE EFFECTS
   None
      
===========================================================================*/

sys_modem_as_id_e_type dsatcmif_get_sglte_hybr_subs_id()
{
  return dsat_hybr_subs_id;
}/* dsatcmif_get_sglte_hybr_subs_id */

/*===========================================================================

FUNCTION DSATCMIF_SET_SGLTE_HYBR_SUBS_ID
    
DESCRIPTION
   This function used to set hybrid subs_id.
    
DEPENDENCIES
   None
RETURN VALUE
.  None
    
SIDE EFFECTS
   None
      
===========================================================================*/

void dsatcmif_set_sglte_hybr_subs_id(sys_modem_as_id_e_type subs_id)
{
  if (subs_id < DSAT_SYS_MODEM_AS_ID_MAX)
  {
    dsat_hybr_subs_id = subs_id;
  }
}/* dsatcmif_set_sglte_hybr_subs_id */

#endif /*FEATURE_SGLTE*/
