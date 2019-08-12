#ifndef CM_QSH_EXT_H
#define CM_QSH_EXT_H
/*===========================================================================

     C A L L   M A N A G E R   D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with cmdbg_qsh.c


Copyright (c) 1991 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cm_qsh_ext.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/15   KC      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "qsh.h"

#define CM_QSH_MAJOR_VER 1

#define CM_QSH_MINOR_VER 1


/* Max number of last sent/received commands/reports at CMREGPRX */
#define CM_DBG_MAX_DEBUG_BUFFER_SIZE   20

/* Stack 1 & 2 will be used SUB1 in CSFB & SRLTE respectively
** Stack 3 will be used for Subs 2 */
#define CM_DBG_NUM_OF_GW_STACKS    3



/* Enumeration for the type of Metric */
typedef enum {
  CM_QSH_METRIC_NONE = -1,
  CM_QSH_METRIC_SRV_INFO,
  CM_QSH_METRIC_CALL_END_INFO,
  CM_QSH_METRIC_SERVING_CELL_INFO,
  CM_QSH_METRIC_MAX
}cm_qsh_metric_e;


/* Enumeration for the type of Event */
typedef enum {
  CM_QSH_EVENT_NONE = -1,

  /* 0 */
  CM_QSH_EVENT_CALL_ORIG_FAILED,           /* Denotes call orig failed (deprecated, not sent out) */
  CM_QSH_EVENT_CALL_DROP,                  /* Denotes CS call end */
  CM_QSH_EVENT_FULL_SRV, /* acquired full service */
  CM_QSH_EVENT_NO_SRV, /* Lost service */

  /* 4 */
  CM_QSH_EVENT_LTD_SRV, /* acquired Limited service */
  CM_QSH_EVENT_WCDMA_CS_CALL_DROP,         /* Denotes WCDMA CS call dropped (abnormal call failure after call got connected) */
  CM_QSH_EVENT_WCDMA_CS_CALL_SETUP_FAILED, /* Denotes WCDMA CS setup failed */
  CM_QSH_EVENT_WCDMA_PS_CALL_DROP,         /* Denotes WCDMA PS call dropped (abnormal call failure after call got connected) */

  /* 8 */
  CM_QSH_EVENT_WCDMA_PS_CALL_SETUP_FAILED, /* Denotes WCDMA PS call setup failed */
  CM_QSH_EVENT_TDS_CS_CALL_DROP,           /* Denotes TDS CS call dropped (abnormal call failure after call got connected) */
  CM_QSH_EVENT_TDS_CS_CALL_SETUP_FAILED,   /* Denotes TDS CS call setup failed */
  CM_QSH_EVENT_TDS_PS_CALL_DROP,           /* Denotes TDS PS call dropped (abnormal call failure after call got connected)*/

  /* 12 */
  CM_QSH_EVENT_TDS_PS_CALL_SETUP_FAILED,   /* Denotes TDS PS call setup failed */
  CM_QSH_EVENT_LTE_PS_CALL_DROP,           /* Denotes LTE PS call dropped  (deprecated, not sent out) */
  CM_QSH_EVENT_LTE_PS_CALL_SETUP_FAILED,   /* Denotes LTE PS call setup failed */
  CM_QSH_EVENT_GSM_CS_CALL_DROP,           /* Denotes GSM CS call dropped (abnormal call failure after call got connected) */

  /* 16 */
  CM_QSH_EVENT_GSM_CS_CALL_SETUP_FAILED,   /* Denotes GSM CS call setup failed */
  CM_QSH_EVENT_GSM_PS_CALL_DROP,           /* Denotes GSM PS call dropped (abnormal call failure after call got connected) */
  CM_QSH_EVENT_GSM_PS_CALL_SETUP_FAILED,   /* Denotes GSM PS call setup failed */
  CM_QSH_EVENT_WCDMA_CS_CALL_START,        /* Denotes WCDMA CS call started */

  /* 20 */
  CM_QSH_EVENT_WCDMA_CS_CALL_END,          /* Denotes WCDMA CS call ended (normal call release) */
  CM_QSH_EVENT_WCDMA_PS_CALL_START,        /* Denotes WCDMA PS call started */
  CM_QSH_EVENT_WCDMA_PS_CALL_END,          /* Denotes WCDMA PS call ended (normal call release) */
  CM_QSH_EVENT_TDS_CS_CALL_START,          /* Denotes TDS CS call started */

  /* 24 */
  CM_QSH_EVENT_TDS_CS_CALL_END,            /* Denotes TDS CS call ended (normal call release) */
  CM_QSH_EVENT_TDS_PS_CALL_START,          /* Denotes TDS PS call started */
  CM_QSH_EVENT_TDS_PS_CALL_END,            /* Denotes TDS PS call ended (normal call release) */
  CM_QSH_EVENT_GSM_CS_CALL_START,          /* Denotes GSM CS call started */

  /* 28 */
  CM_QSH_EVENT_GSM_CS_CALL_END,            /* Denotes GSM CS call ended (normal call release) */
  CM_QSH_EVENT_GSM_PS_CALL_START,          /* Denotes GSM PS call started */
  CM_QSH_EVENT_GSM_PS_CALL_END,            /* Denotes GSM PS call ended (normal call release) */
  CM_QSH_EVENT_VOLTE_CALL_START,           /* Denotes VOLTE call started */

  /* 32 */
  CM_QSH_EVENT_VOLTE_CALL_END,             /* Denotes VOLTE call ended (normal call release) */
  CM_QSH_EVENT_LTE_PS_CALL_START,          /* Denotes LTE PS call started */
  CM_QSH_EVENT_LTE_PS_CALL_END,            /* Denotes LTE PS call ended (normal call release) */
  CM_QSH_EVENT_CDMA_CS_CALL_START,         /* Denotes CDMA CS call started */

  /* 36 */
  CM_QSH_EVENT_CDMA_CS_CALL_END,           /* Denotes CDMA CS call ended (normal call release) */
  CM_QSH_EVENT_CDMA_PS_CALL_START,         /* Denotes CDMA PS call started */
  CM_QSH_EVENT_CDMA_PS_CALL_END,           /* Denotes CDMA PS call ended (normal call release) */
  CM_QSH_EVENT_CDMA_CS_CALL_DROP,          /* Denotes CDMA CS call dropped (abnormal call failure after call got connected) */

  /* 40 */
  CM_QSH_EVENT_CDMA_CS_CALL_SETUP_FAILED,  /* Denotes CDMA CS call setup failed */
  CM_QSH_EVENT_CDMA_PS_CALL_DROP,          /* Denotes CDMA PS call dropped (abnormal call failure after call got connected) */
  CM_QSH_EVENT_CDMA_PS_CALL_SETUP_FAILED,  /* Denotes CDMA PS call setup failed */
  CM_QSH_EVENT_HDR_PS_CALL_START,          /* Denotes HDR PS call started */

  /* 44 */
  CM_QSH_EVENT_HDR_PS_CALL_END,            /* Denotes HDR PS call ended (normal call release) */
  CM_QSH_EVENT_HDR_PS_CALL_DROP,           /* Denotes HDR PS call dropped (abnormal call failure after call got connected) */
  CM_QSH_EVENT_HDR_PS_CALL_SETUP_FAILED,   /* Denotes HDR PS call setup failed */
  CM_QSH_EVENT_1XCSFB_CALL_START,          /* Denotes 1XCSFB call started */

  /* 48 */
  CM_QSH_EVENT_PPCSFB_CALL_START,          /* Denotes PPCSFB call started */
  CM_QSH_EVENT_MAX
}cm_qsh_event_e;

/* Enumeration for the type of Dump */
typedef enum {
  CM_QSH_DUMP_TAG_NONE = -1,
  CM_QSH_DUMP_TAG_MINI, /* Denotes Mini dump */
  CM_QSH_DUMP_TAG_MAX
}cm_qsh_dump_tag_e;



/* this structure is evolved from cmregprx_stack_info_s_type */
typedef PACK(struct)
{
  /* Current state of RegProxy.
  */
  uint8 state;
    /* cmregprx_state_e_type */

    /* Current sub-state of RegProxy.
    */
  uint8 substate;
    /* cmregprx_substate_e_type */

    /* Transaction id of the last request to REG.
    */
  uint32 reg_trans_id;
    /* cmregprx_trans_type */

    /* Trans_id for the MMoC's request, can handle only one request
    ** at a time.
    */
  uint8 mmoc_trans_id;
    /* prot_trans_type */

    /* Current GSM/WCDMA/LTE subscription availability status.
    */
  uint8 is_gwl_subs_avail;
    /* cm_subs_avail_e_type */


    /* Flag to indicate if SD20 reselection is allowed or not.
    */
  boolean is_resel_allowed;


  /* Count of CM_SERVICE_REQ during the reference period.
  */
  uint32 cnt_cm_srv_req;
    /* int */

    /** The reason STOP_MODE_REQ was sent to REG. */
  int8 stop_mode_reason;
    /* sys_stop_mode_reason_e_type */

    /* Corresponding SD SS to be used for this stack
    */
  uint8 ss;
    /* sd_ss_e_type */

    /* Assigned Active Subscription ID for this stack
    */
  uint8 as_id;
    /* sys_modem_as_id_e_type */

  int8 nas_stack_id;
    /* sys_modem_stack_id_e_type */


    /*
    ** This flag indicates if the UE is in LTE Connected mode
    */
    boolean                              lte_connected_mode;
  /*
  ** This flag indicates if the UE is in WCDMA Connected mode
  */
  boolean                              wcdma_connected_mode;


  boolean                              is_rlf_reset_delayed;

  /* Req. ID.
  */
  uint16                                sys_sel_pref_req_id;


  /* This variable holding the pending UE_MODE which needs to be sent to NAS
  ** SGLTE/SRLTE: This is used to resolve the scenario of multiple back-2-back ue_mode change
  ** For example, UE_MODE (home) is processed and sys_sel_pref() command is queued
  ** on MMOC.
  ** MMOC starts ue_mode switch, and send STOP_MODE to NAS. NAS has not yet responded.
  ** Meanwhile, UE_MODE (roam) is also processed and queued to MMOC. So now, CM changed UE_mode to ROAM.
  ** NAS responded STOP_MODE_CNF, which triggers ue_mode (roam) to be sent out.
  ** This is wrong, because MMOC is still processing "HOME" preference, while NAS is moving ROAM.
  **
  ** The fix:  Having UE_MODE passing through MMOC per transaction.
  ** That's why we need this variable to hold the 'pending_ue_mode' that is passed back
  ** to CMREGPRX, so CMREGPRX uses this to send to NAS.
  */
  int8 pending_ue_mode;
    /* sys_ue_mode_e_type */

  boolean pending_substate_srlte;


  /* UE mode sent to NAS */
  int8 ue_mode;
    /* sys_ue_mode_e_type */

  boolean is_ue_mode_substate_srlte;
}cm_dbg_cmregprx_stack_info_s_type;



/* this structure is evolved from cmregprx_info_s_type */
typedef PACK(struct)
{

  uint8 ds_pref;
    /* Dual standby preference */
    /* sys_modem_dual_standby_pref_e_type */

  uint8 device_mode;
    /* Device mode, ie. dsds, dsda, etc */
    /* sys_modem_device_mode_e_type */

  cm_dbg_cmregprx_stack_info_s_type stack_info[CM_DBG_NUM_OF_GW_STACKS];
    /* Stack specific information */
    /* cmregprx_stack_info_s_type */

  uint32 ds_stat_chgd_trans_id;
    /* cmregprx_trans_type */

  uint8 mmoc_trans_id;
    /* prot_trans_type */

  uint32 subs_chgd_trans_id;
    /* cmregprx_trans_type */

  uint32 unique_trans_id;
    /* cmregprx_trans_type */

  boolean is_waiting_for_plmn_block_resp;


  boolean is_send_unblock_on_next_flpmn;
}cm_dbg_cmregprx_info_s_type;


/* this structure is evolved from cmregprx_dbg_buf_message_s_type */
typedef PACK(struct)
{
  /* Name of command/report  sent/received by CMREGPRX.Currently uint8 
  ** is sufficient to store all report names received by CMREGPRX.Need 
  ** to be increased in future if any report received from NAS is to be
  ** more than 254 
  */
  uint8 message_name;

  /*SS on which CMD/RPT is received
  */
  uint8 ss;

  /*Indicated whether message received is a command or report
  */
  boolean is_cmd;

  /* Current state of CMREGPRX per stack.
  */
  uint8 cmregprx_state[CM_DBG_NUM_OF_GW_STACKS];

  /* Current substate of CMREGPRX per stack.
  */
  uint8 cmregprx_substate[CM_DBG_NUM_OF_GW_STACKS];

  /* Number of consecutive times this message was sent/received in the debug buffer
  */
  uint16 cnt;

}cm_dbg_cmregprx_buf_message_dbg_s_type;


/* this structure is evolved from cmregprx_debug_buffer_s_type */
typedef PACK(struct)
{
  cm_dbg_cmregprx_buf_message_dbg_s_type dbg_buf[CM_DBG_MAX_DEBUG_BUFFER_SIZE];
    /* cmregprx_dbg_buf_message_s_type */

  uint8 dbg_buf_idx;

  uint8 stack_id_for_current_report;
    /* sys_modem_stack_id_e_type */

  uint8 asubs_id_for_current_report;
    /* sys_modem_as_id_e_type */

}cm_dbg_cmregprx_buffer_s_type;


typedef PACK(struct) cm_qsh_dump_tag_mini_s{
 qsh_dump_tag_hdr_s hdr; /* this header will have a tag and length */


 /* A type for messages [commands/reports] store in cmregprx debug buffer
 */
 cm_dbg_cmregprx_buffer_s_type dbg_buffer;


 /* A type for parameters that holds the state and other information of the
 ** RegProxy.
   */
 cm_dbg_cmregprx_info_s_type cmreg_info;
}cm_qsh_dump_tag_mini_s_type;


/* Below will be used logging into metrics buffer */
typedef struct cm_qsh_metric_srv_info_s
{
  qsh_metric_hdr_s           hdr; /* This header has the timestamp (uint16) */

 uint8 plmn[3];

 uint8 sys_mode;
 /* possible values are defined in sys_sys_mode_e_type */

 uint8 sim_state;
   /* possible values are defined in sys_sim_state_e_type */

 uint8 roam_status;
 /* possible values are defined in sys_roam_status_e_type */

 uint8 srv_status;
 /* possible values are defined in sys_srv_status_e_type */

 boolean dual_sim_state;
 /* tells whether Dual SIM is ON or not */

  uint8 hplmn[3];
  /* the HPLMN */
}cm_qsh_metric_srv_info_s_type;


/* structure for call end info metrics */
typedef struct cm_qsh_metric_call_end_info_s
{
  qsh_metric_hdr_s           hdr; /* This header has the timestamp (uint16) */

  uint8             asubs_id;
  /* the subs ID of the call; possible values are defined in sys_modem_as_id_e_type */

  uint8             srv_status;
  /* possible values are defined in sys_srv_status_e_type */

  uint8             last_system_reported;
  /* the last system in which CM had service; possible values defined in sys_sys_mode_e_type */

  uint8             roam_status;
  /* possible values are defined in sys_roam_status_e_type */

  uint8             srv_domain;
  /* possible values are defined in sys_srv_domain_e_type */
 
  uint8             call_type;
  /* possible values are defined in cm_call_type_e_type */

  uint8             call_state;
  /* state of call before it ended; possible values are defined in cm_call_state_e_type */

  uint8             event_type;
  /* call end event; possible values are defined in sys_call_end_event_e_type */

  uint8             cause;
  /* specific cause of call end; possible values are defined in ie_cm_cc_cause_T.cause_value */

  uint16            call_end_reason;
  /* CM call end reason; possible values are defined in cm_call_end_e_type */
} cm_qsh_metric_call_end_info_s_type;


/* structure for serving cell info metrics */
typedef struct cm_qsh_metric_serving_cell_info_s {
  qsh_metric_hdr_s hdr; /* This header has the timestamp (uint16) */

  uint8            sys_mode;
  /* possible values are defined in sys_sys_mode_e_type */

  uint16           lac_tac;
  /* For GWT, LAC; for L, TAC. possible values defined in sys_lac_type */

  uint8            rac;
  /* Routing area code */

  uint8            plmn[3];
} cm_qsh_metric_serving_cell_info_s_type;


/*===========================================================================

FUNCTION cmdbg_qsh_init

DESCRIPTION
  Initilize CM QSH interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmdbg_qsh_init();


/*===========================================================================

FUNCTION cmdbg_qsh_last_sent_srv_metric_ptr

DESCRIPTION
  Returns a pointer to the last sent srv info metric.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_qsh_metric_srv_info_s_type  *cmdbg_qsh_last_sent_srv_metric_ptr();


/*===========================================================================

FUNCTION cmdbg_qsh_last_sent_serving_cell_metric_ptr

DESCRIPTION
  Returns a pointer to the last sent serving cell metric.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_qsh_metric_serving_cell_info_s_type  *cmdbg_qsh_last_sent_serving_cell_metric_ptr();


#endif /* CM_QSH_EXT_H */

