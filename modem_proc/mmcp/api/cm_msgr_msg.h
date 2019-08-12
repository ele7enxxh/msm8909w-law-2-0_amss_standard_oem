#ifndef CM_MSGR_MSG_H
#define CM_MSGR_MSG_H
/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cm_msgr_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/06/13   xs      FR3799: Broadcasting IMS E911 capability
05/15/13   xs      Enable emergency API
01/07/12   xs      FR 2972: Enhance QMI to support domain selection preference status 
                   updates 
05/01/12   gm      Use separate sub-module IDs between CM and MMOC.
02/23/12   vk      Provided config request support to CMSDS
12/01/11   vk      Added PS voice only support to domain selection module.
10/06/11   xs      Add SRVCC cancel notification definition between CM and NAS
02/28/11   gm      Changes for Thermal emergency FR
09/21/10   rk      Added support for CSFB
06/21/10   fj      Add support for LTE call control.
11/13/09   fj      Reverted the change from #6.
10/15/09   np      Added $header
10/14/09   sv      Updating UMID def's to use MM tech area.
08/06/09   fj      Added UMID for PDN_DISCONNECT_REJECT_IND.
06/18/09   fj      Added UMID for DRB_REESTABLISH_REJECT_IND.
05/08/09   fj      Added UMID for BEARER_RESOURCE_ALLOC_REJECT_IND and 
                   BEARER_RESOURCE_ALLOC_ABORT_RSP.
03/11/09   fj      Added UMID for ACT_DEDICATED_BEARER_CONTEXT_REQUEST.
01/05/09   fj      Initial Version.
==============================================================================*/


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "sys.h"
#include "sys_v.h"
#include "cm.h"
#include <msgr.h>
#include <msgr_umid.h>


/*==============================================================================
                           Module definitions
==============================================================================*/
/* CM should modules between 0x01 to 0x50 */

#define MSGR_MODULE_CM        0x01
#define MSGR_MM_CM            MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_CM )

#define MSGR_MODULE_MM_DOM_SEL   0x02
#define MSGR_MM_DOM_SEL      MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_MM_DOM_SEL)

#define MSGR_MODULE_DOM_SEL_CONFIG    0x03
#define MSGR_MM_DOM_SEL_CONFIG       MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_DOM_SEL_CONFIG)

#define MSGR_MODULE_CM_AC             0x04
#define MSGR_CM_AC                    MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_CM_AC )

#define MSGR_MODULE_CM_IMS             0x05
#define MSGR_CM_IMS_SRVCC_CNTXT         MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_CM_IMS )

#define MSGR_MODULE_CM_TRM_PRIORITY_REQ  0x06
#define MSGR_CM_TRM_PRIORITY_REQ        MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_CM_TRM_PRIORITY_REQ )

#define MSGR_MODULE_EXT_VOLTE_STATE           0x07
#define MSGR_MM_EXT_VOLTE         MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_EXT_VOLTE_STATE

#define MSGR_CM_VOLTE_STATUS          MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_CM )
/** @brief Different configuration params managed by CNE */
typedef enum
{
  CM_DOM_SEL_CONFIG_TYPE_NONE = -1,             
  /**< Internal use.  */
  CM_DOM_SEL_CONFIG_TYPE_LTE_VOICE_DOM_PREF,  
  /**< OMA-DM configuration for voice domain preference over LTE 
       Data type: cne_config_lte_voice_domain_pref_e_type
  */
                              
  CM_DOM_SEL_CONFIG_TYPE_LTE_SMS_DOM_PREF,
  /**< OMA-DM configuration for sms domain preference over LTE 
       Data type: cne_config_lte_sms_domain_pref_e_type
  */
  CM_DOM_SEL_CONFIG_TYPE_MAX                  
  /**< Internal use. */
} cmsds_config_e_type;

/* Return value */
typedef enum
{
  CM_DOM_SEL_FAILURE = -1,

  CM_DOM_SEL_SUCCESS = 1
}cmsds_ret_e_type;

typedef enum cm_ipapp_audio_rel_e {
  CM_IPAPP_AUDIO_REL_NONE,
    /**< @internal */
  CM_IPAPP_AUDIO_REL_3GPP_SRVCC,

  CM_IPAPP_AUDIO_REL_3GPP2_SRVCC,

  CM_IPAPP_AUDIO_REL_3GPP_DRVCC,

  CM_IPAPP_AUDIO_REL_3GPP2_DRVCC,

  CM_IPAPP_AUDIO_REL_MAX
  /**< @internal */
}cm_ipapp_audio_rel_e_type;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** @brief Event information to be sent when the selected domain output
     from domain selection changes. 
     For LTE: 
     When call_type is CM_CALL_TYPE_VOICE the possible domains are: 
      - SYS_CM_DOMAIN_SEL_DOMAIN_NONE
      - SYS_CM_DOMAIN_SEL_DOMAIN_IMS
      - SYS_CM_DOMAIN_SEL_DOMAIN_3GPP
      - SYS_CM_DOMAIN_SEL_DOMAIN_1X
      - SYS_CM_DOMAIN_SEL_DOMAIN_NO_VOICE
      - 
     When call_type is CM_CALL_TYPE_SMS the possible domains are:
      - SYS_CM_DOMAIN_SEL_DOMAIN_NONE
      - SYS_CM_DOMAIN_SEL_DOMAIN_IMS
      - SYS_CM_DOMAIN_SEL_DOMAIN_3GPP
      - SYS_CM_DOMAIN_SEL_DOMAIN_1X
      - SYS_CM_DOMAIN_SEL_DOMAIN_NO_SMS */
typedef struct 
{
  sys_sys_mode_e_type         system; 
  /**< Currently only support SYS_SYS_MODE_LTE **/ 
  cm_call_type_mask_type   call_type_mask; 
  /** Call types associated with this domain. Currently only 
      support voice and sms */
  sys_cm_domain_sel_domain_e_type domain; 
/** selected domain info for the system and call_type */
} cm_domain_sel_domain_info_s_type;

/** @brief Structure containing IMS registration status data. */
typedef struct 
{
  sys_sys_mode_e_type         system; 
  /**< Currently only support SYS_SYS_MODE_LTE **/ 

  cm_call_type_mask_type         call_type_mask; 
  /** Call types associated with this domain. Currently only 
      support voice and sms */

  boolean is_registered;
  /** Whether or not IMS is registered */

  sys_cm_domain_sel_ims_reg_end_cause_e_type end_cause;
  /** If registration failed or ended what was the cause  */

} cm_domain_sel_ims_reg_status_s_type;


/** @brief Struct containing IMS domain pref info for
  different call types.
*/
typedef struct 
{
  sys_sys_mode_e_type         system; 
  /**< Currently only support SYS_SYS_MODE_LTE **/ 

  cm_call_type_mask_type   call_type_mask; 
  /**< Call types for which change in IMS_PREF is communicated */

  cm_call_type_mask_type   ims_pref_mask;
  /**< For voice, if IMS is PREF then BIT 0 will be set otherwise it will be unset 
       For SMS if IMS is PREF, then BIT 3 will set otherwise it will be unset */

  boolean       lte_ims_voice_avail;
  /**< True VOPS info*/

} cm_domain_sel_ims_pref_info_s_type;


/** @brief Message structure for the data associated 
  with a domain changed ind
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_domain_info_s_type data;
  /**< domain info that has changed. */
} cm_domain_sel_domain_chg_ind_msg_type;

/** @brief Message structure for the data associated 
  with a domain selected req
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_domain_info_s_type data;
  /**< domain info that has changed. */
} cm_domain_sel_domain_get_req_msg_type;

/** @brief Message structure for the data associated with
  a domain selected response
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_domain_info_s_type data;
  /** domain info associtated with the request. Valid only if
      request was successfull */
} cm_domain_sel_domain_get_rsp_msg_type;

/** @brief Message structure for the IMS preferred indication.
  This struct provides info required by IMS to perform
  required registrations for different services.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_ims_pref_info_s_type data;
  /**< IMS domain preference for various services. */
} cm_domain_sel_ims_pref_chg_ind_msg_type;

/** @brief Message structure for the IMS preferrence request.
  CMDS will provide required info to IMS to perform required
  registrations for different services.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */
} cm_domain_sel_ims_pref_req_msg_type;

/** @brief Message structure for the IMS preferred response.
  This struct provides info required by IMS to perform
  required registrations for different services.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_ims_pref_info_s_type data;
  /**< IMS domain preference for various services. */
} cm_domain_sel_ims_pref_rsp_msg_type;

/** @brief Message structure for the IMS preferred response.
  This struct provides info required by IMS to perform
  required registrations for different services.
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  sys_sys_mode_mask_e_type  sys_mode_mask;
  /** valid systems which carry the ims voice/sms pref */

  sys_sys_mode_mask_e_type     ims_voice_pref_mask;
  /** TRUE if IMS voice is preferred over system mode set in sys_mode_mask, FALSE otherwise */

  sys_sys_mode_mask_e_type     ims_sms_pref_mask;
  /** TRUE if IMS SMS is preferred over system mode set in sys_mode_mask, FALSE otherwise */
} cm_domain_sel_ims_pref_rsp_msg_type_ext;


/** @brief Message structure for the IMS registration status
  indication.
*/ 
typedef struct
{
  msgr_hdr_s  msg_hdr;
  /**< Message header */

  cm_domain_sel_ims_reg_status_s_type data;
  /**< IMS registration status info */
} cm_domain_sel_update_ims_reg_status_req_msg_type;


/** @brief configuration info struct */
typedef struct
{
  cmsds_config_e_type config;
  /** config type */
  union
  {
    sys_voice_domain_pref_e_type lte_voice_dom_pref;
    /**< voice domain preferenc for lte @sa cne_config_e_type 
      for possible values. */
    sys_sms_domain_pref_e_type lte_sms_dom_pref;
    /**< voice domain preferenc for lte @sa cne_config_e_type 
      for possible values. */
  } value;
  /** config value */
} cmsds_config_info_s_type;

/** @brief Message structure for configurations req
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;     
  /**< Message header */
  
}cm_domain_sel_get_config_req_msg_type;

/** @brief Message structure for configurations req
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /**< Message header */
  cm_ipapp_audio_rel_e_type ip_audio_rel_type;  
}cm_audio_session_rel_msg_type;


/** @brief Message structure for configuration update reqs
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;     
  /**< Message header */
  
  cmsds_config_info_s_type data;
  /**< config value */
} cm_domain_sel_config_update_req_msg_type;

/** @brief Message structure for configurations rsp
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /**< Message header */
  
  cmsds_ret_e_type         status;
  /**< Clients must check status prior to using
       the config value to ensure expected behavior.  
       When returned status is CNE_RET_FAILURE  
       a pre-defined default value is returned 
       in config value field.
  */
  cmsds_config_info_s_type data;
  /**< config value */
} cm_domain_sel_get_config_rsp_msg_type;

/** @brief Message structure for configurations ind
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;     
  /**< Message header */
  
  cmsds_config_info_s_type data;
  /**< config value */
} cm_domain_sel_config_chg_ind_msg_type;

/** @brief Message structure for configurations ind
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;     
  /**< Message header */
  
} cm_domain_sel_get_ims_pref_msg_type;

/* Payload for Access Control Events */
typedef struct 
{
  msgr_hdr_struct_type hdr;


  sys_modem_as_id_e_type   asubs_id;
    /**< For DSDS consideration */

  cm_ac_reason_e_type reason;
    /**< Reason for restricting certain services */

  void  *user_data;

} cm_ac_info_ind_s_type;

typedef struct
{
  msgr_hdr_struct_type hdr;
  sys_modem_as_id_e_type                      as_id;
}cm_csg_srch_trigger_ind_s_type;


typedef  union cm_sds_cmd_u
{  

  msgr_hdr_s        msg_hdr; /*!< Message router header */

  /*! @brief Messages sent by the IMS to CM. */ 
  cm_domain_sel_ims_pref_req_msg_type              cm_domain_sel_ims_pref_get_req;
  cm_domain_sel_domain_get_req_msg_type            cm_domain_sel_domain_get_req_msg;
  cm_domain_sel_update_ims_reg_status_req_msg_type cm_domain_sel_update_ims_reg_status_Ind;
  cm_domain_sel_get_config_req_msg_type                cm_domain_sel_config_req;
  cm_domain_sel_config_update_req_msg_type         cm_domain_sel_config_update_req;
  cm_domain_sel_get_ims_pref_msg_type              cm_domain_sel_get_ims_pref_req_msg;
} cm_sds_cmd_u_type;

typedef union cm_sds_rsp_u
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */

  /* Messages sent by  CMSDS to IMS */
  cm_domain_sel_ims_pref_rsp_msg_type        cm_domain_sel_ims_pref_get_rsp;
  cm_domain_sel_domain_get_rsp_msg_type      cm_domain_sel_domain_get_rsp;
  cm_domain_sel_domain_chg_ind_msg_type      cm_domain_sel_domain_chg_ind;
  cm_domain_sel_ims_pref_chg_ind_msg_type    cm_domain_sel_ims_pref_chg_ind;
  cm_domain_sel_get_config_rsp_msg_type      cm_domain_sel_config_resp;
  cm_domain_sel_config_chg_ind_msg_type      cm_domain_sel_config_chg_ind;
  cm_domain_sel_ims_pref_rsp_msg_type_ext    cm_domain_sel_ims_pref_get_rsp_ext;
}cm_sds_rsp_u_type;


typedef struct 
{
  msgr_hdr_struct_type hdr;

  sys_modem_as_id_e_type   asubs_id;
  /**< For DSDS consideration */

  void * user_data;

  boolean is_redial;
  /**< Used when AP want to include LTE for e911
       when CS e911 fails */

} cm_ac_emergency_enter_req_s_type;

typedef struct 
{
  msgr_hdr_struct_type hdr;

  sys_modem_as_id_e_type   asubs_id;
  /**< For DSDS consideration */

  boolean is_ecbm_required;
  /**< Client requests ecbm mode */

  void* user_data;

} cm_ac_emergency_exit_req_s_type;

typedef union 
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  cm_ac_emergency_enter_req_s_type                     cm_ac_emergency_enter_request;
  cm_ac_info_ind_s_type                                cm_ac_emergency_enter_ind;
  cm_ac_info_ind_s_type                                cm_ac_emergency_ready_ind;
  cm_ac_emergency_exit_req_s_type                      cm_ac_emergency_exit_request;
  cm_ac_info_ind_s_type                                cm_ac_emergency_exit_ind;

} cm_emg_u_type;

/* Emergency mode status information will be sent as
** 1. EMERGENCY_MODE_STARTED on emergency call origination 
**    if UE is not in emergency mode already
** 2. EMERGENCY_MODE_STARTED on third party e911 call origination
**    if UE is not in emergency mode already
** 3. EMERGENCY_MODE_ENDED on emergency call end if 
**    UE is not in ECBM.
** 4. EMERGENCY_MODE_ENDED on ECBM exit. If ECBM exit is due to second
**    emergency call origination, EMERGENCY_MODE_ENDED will not be sent 
** 5. EMERGENCY_MODE_ENDED on third party e911 call end.
** 6. EMERGENCY_MODE_ENDED for the above cases 3,4,5 is sent only if UE is 
**    in emergency mode.
*/
typedef struct
{

  msgr_hdr_s		 msg_hdr;
  /* Message header */

  sys_modem_as_id_e_type   asubs_id;  
  /* Active Subscription */

  cm_emerg_mode_status_e_type 	 emerg_mode_status; 
  /* Emergency mode start and end status*/
  
}cm_emerg_mode_status_ind_s_type;


/* Sends updated voice/video status during start and end of
   Volte/VT calls
*/
typedef struct
{

  msgr_hdr_s		 msg_hdr;
  /* Message header */

  boolean voice_call_status;/*0-inactive 1-active*/
		 
  boolean video_call_status;/*0-inactive 1-active*/
		 
  boolean sms_status;/*0-inactive 1-active*/

sys_sys_mode_e_type  sys_mode;

  
}cm_volte_call_status_ind_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  -*/
/* SRVCC mid-call context message router definitions CM <--> IMS*/

/* Call info needed from IMS for non-conference calls */
typedef struct 
{
  cm_call_id_type             call_id;
  cm_call_type_e_type         call_type;
  cm_call_state_e_type        call_state;
  cm_call_substate_type       call_sub_state;
  cm_num_s_type               call_num;
  cm_call_direction_e_type    call_direction;
  cm_srvcc_alert_media_type   call_alert_media_type;
  cm_srvcc_caller_name_s_type srvcc_caller_info;
  /* caller name passed on from AP during HO response */
}cm_mid_srvcc_ip_call_info;


/* call info needed from IMS for conference call */
typedef struct
{
  cm_call_id_type        participant_id;
  cm_call_type_e_type    call_type;
  cm_call_state_e_type   call_state;
  cm_call_substate_type  call_sub_state;
  cm_num_s_type          participant_num;
  cm_call_direction_e_type    call_direction;
}cm_mid_srvcc_participant_info;
typedef struct 
{
  cm_call_id_type                 call_id;
  uint8                           num_of_participants;
  cm_call_state_e_type			  conf_call_state;
  cm_call_substate_type           conf_call_substate;
  cm_mid_srvcc_participant_info   participant_list[MAX_CONF_CALL_NUM_SRVCC];
}cm_mid_srvcc_ip_conf_call_info;

typedef union
{
  cm_mid_srvcc_ip_call_info           ip_call;
  cm_mid_srvcc_ip_conf_call_info      ip_conf_call;

}cm_mid_srvcc_call_info_u_type;

typedef struct
{
  boolean                        is_conf_call;
  cm_mid_srvcc_call_info_u_type  call_info;
}cm_mid_srvcc_ip_context_s_type;

/* structure for IMS return call context info after SRVCC */
typedef struct
{
  msgr_hdr_s                      msg_hdr; /*!< Message router header */
  uint8                           num_of_calls;
  cm_mid_srvcc_ip_context_s_type  call_context[MAX_CALL_NUM_SRVCC]; 
  sys_modem_as_id_e_type as_id;
} cm_srvcc_call_context_rsp_s_type;


/* Structure for CM sending call info list after SRVCC HO complete */
typedef struct{
  msgr_hdr_s                      msg_hdr; /*!< Message router header */
  byte                            number_calls;
  boolean                         is_int_ims; 
  cm_mm_srvcc_call_info_s_type    info[MAX_CALL_NUM_SRVCC];  
} cm_mid_srvcc_ho_comp_list_s_type;


/* Structure for CM notifying IMS to process PH cmd*/
typedef struct cm_subsc_chgnd_ind_msg
{
  msgr_hdr_s              msg_hdr;
 
  cm_ph_cmd_e_type  subs_cmd;

  sys_modem_as_id_e_type as_id;
 
} cm_subsc_chgnd_ind_msg_type;
 
/* Structure for CM recieving response from IMS for PH cmd*/
typedef struct cm_subsc_chgnd_rsp_s
{
  msgr_hdr_s              msg_hdr;
 
  sys_modem_as_id_e_type  as_id;

} cm_subsc_chgnd_rsp_s_type;

/* Structure for CM to broadcast IMS e911 capability support*/
typedef struct cm_ims_emerg_cap_ind_s
{
  msgr_hdr_s              msg_hdr;

  boolean                 ims_emergency_supp;
 
} cm_ims_emerg_cap_ind_s_type;

/* Structure for CM to notify timer T3346 information */
typedef struct cm_t3346_info_s
{

  msgr_hdr_s              msg_hdr;

  sys_modem_as_id_e_type          as_id;
    /** < Subscription id
    */

  sys_nas_t3346_status_e_type     timer_status;
    /** < Indicate the status of timer T3346
    */

  sys_sys_mode_e_type             active_rat;
    /** < Indicates the RAT for which timer t3346 status is sent
    */

} cm_t3346_info_s_type;

typedef struct cm_plmn_list_chngd_ind_s
{                                                                
  msgr_hdr_s              msg_hdr;
  sys_plmn_list_type_e_type changed_list_type;
  sys_modem_as_id_e_type as_id;
} cm_plmn_list_chngd_ind_s_type;

typedef struct 
{
  msgr_hdr_struct_type hdr;

  boolean volte_mode_enable; 

  sys_modem_as_id_e_type  as_id;

} cm_volte_state_notification_cmd_s_type;


typedef struct 
{
  msgr_hdr_struct_type hdr;

  sys_call_mode_t call_mode; 
  /* Updated Call Mode from CM/PM to AP*/
  
  sys_modem_as_id_e_type  as_id;

} cm_call_mode_ind_s_type;

/* Structure for CM receiving volte call status from ext IMS */
typedef struct cm_ext_volte_call_state_ind
{
  msgr_hdr_s              msg_hdr;

  sys_modem_as_id_e_type   asubs_id;

  cm_call_type_mask_type   call_type; /* bit mask for call type*/
  
  cm_volte_call_state_e_type      call_state; /* state of call */
 
} cm_ext_volte_call_state_ind_s_type;


/** @ Message structure for IMS to let IMS know about LTE active status
 */
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /**< Message header */
  boolean status;
  /** status = False : LTE is not active due to CS calls in SRLTE or DSDS
  ** status = TRUE : Call is ended, LTE is active now
  */
}cm_lte_active_status_ind_s_type;

/* Structure for lte rpm parameters */
typedef struct cm_mm_lte_rpm_parameters_s
{
  uint8                   max_event_counter;
  uint16                  max_event_timer;
} cm_mm_lte_rpm_parameters_s_type;


/* Structure for set rpm parameters request*/
typedef struct cm_mm_set_rpm_parameters_req_s
{
  msgr_hdr_s              msg_hdr;
  cm_mm_lte_rpm_parameters_s_type lte_rpm_params;

} cm_mm_set_rpm_parameters_req_s_type;

/* Structure for get rpm parameters request*/
typedef struct cm_mm_get_rpm_parameters_req_s
{
  msgr_hdr_s              msg_hdr;

} cm_mm_get_rpm_parameters_req_s_type;

/* RPM response return value */
typedef enum
{
  CM_RPM_NONE = -1,

  CM_RPM_SUCCESS_RPM_ENABLED = 1,

  CM_RPM_FAILED_RPM_DISABLED = 2,

  CM_RPM_MAX

}cm_rpm_ret_e_type;

/* Structure for set rpm parameters response to cm clients*/
typedef struct cm_mm_set_rpm_parameters_rsp_s
{
  msgr_hdr_s              msg_hdr;

  cm_rpm_ret_e_type       rpm_rsp_ret_val;

} cm_mm_set_rpm_parameters_rsp_s_type;


/* Structure for reporting rpm parameters to cm clients*/
typedef struct cm_mm_get_rpm_parameters_rsp_s
{
  msgr_hdr_s              msg_hdr;

  cm_rpm_ret_e_type       rpm_rsp_ret_val;
  cm_mm_lte_rpm_parameters_s_type lte_rpm_params;

} cm_mm_get_rpm_parameters_rsp_s_type;


/* Structure to set rpm configuration request*/
typedef struct cm_mm_set_rpm_config_req_s
{
  msgr_hdr_s       msg_hdr;
  boolean          lte_rpm_config;

} cm_mm_set_rpm_config_req_s_type;


typedef struct cm_manual_scan_fail_ind_s
{
  msgr_hdr_s             msg_hdr;              /* Message router header */

} cm_manual_scan_fail_ind_s_type;

typedef struct cm_trm_priority_ind_s
{
  msgr_hdr_s             msg_hdr;              /* Message router header */

  boolean                trm_priority_high;

} cm_trm_priority_ind_s_type;

typedef struct
{
  msgr_hdr_struct_type   hdr;     
  /**< Message header */
  uint16                 raw_rssi_1x;
  unsigned char          raw_ecio_1x;
}cm_raw_rssi_msg_type;

/** @brief Message structure for the PSM_ENABLE request.
*/
typedef struct 
{
  msgr_hdr_s     msg_hdr;       /* Message router header */

} cm_psm_get_cfg_params_req_s_type;

/** @brief Message structure for the PSM_ENABLE responce.
*/
typedef struct 
{
  msgr_hdr_s     msg_hdr;       /* Message router header */

  boolean        enabled;       /* PSM is enabled or not */

  uint32         threshold;    /* Duration threshold to enter PSM, in seconds */

  uint16         psm_duration_due_to_oos;     /* PSM duration due to outage, in seconds */

  uint16         psm_randomization_window;    /* PSM wakeup randomization window, in seconds */

} cm_psm_get_cfg_params_rsp_s_type;

/** @brief Message structure for the PSM_ENTER request.
*/
typedef struct 
{
  msgr_hdr_s     msg_hdr;       /* Message router header */

  uint32         apps_time;     /* Apps time in seconds */

} cm_psm_enter_req_s_type;


/** Enumeration of PSM mode status. Clients will be notified about 
    PSM status through MM_CM_PSM_STATUS_IND.
*/
typedef enum
{
  /** @cond
  */
  CM_PSM_STATUS_NONE=-1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_PSM_STATUS_COMPLETED,
  /**< Entering into PSM  is done */

  CM_PSM_STATUS_REJECTED,
  /**< Entering PSM  is rejected */

  CM_PSM_STATUS_READY,
  /**< MODEM is ready for entering into PSM  */

  CM_PSM_STATUS_OOS,
  /**< UE not able acquired service even after few full scans */

  CM_PSM_STATUS_LIMITED_SERVICE,
  /**< UE acquired limited service but not full service */

  /** @cond
  */
  CM_PSM_STATUS_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_psm_status_e_type;

/** Enumeration of PSM mode reject reason. Reason for entering PSM mode 
    failure or rejection will be notified to clients through 
    MM_CM_PSM_STATUS_IND.
*/
typedef enum
{
  /** @cond
  */
  CM_PSM_REJECT_REASON_NONE=-1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_PSM_REJECT_REASON_PSM_NO_ERROR,
  /**< PSM No Error */

  CM_PSM_REJECT_REASON_PSM_NOT_ENABLED,
  /**< PSM is not enabled */

  CM_PSM_REJECT_REASON_MODEM_NOT_READY,
  /**< Modem is not in ready state, ex: MT call is in progress */

  CM_PSM_REJECT_REASON_DURATION_TOO_SHORT,
  /**< if apps duration is less than the PSM mode threshold */

  /** @cond
  */
  CM_PSM_REJECT_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_psm_reject_reason_e_type;

/** @brief Message structure for the PSM status indication.
  This struct provides the status of the PSM.
*/

typedef struct cm_psm_mode_status_ind_s
{
  msgr_hdr_s                       msg_hdr;       /* Message router header */

  cm_psm_status_e_type             status;        /* status */

  cm_psm_reject_reason_e_type      reject_reason; /* reject reason */

} cm_psm_status_ind_s_type;

typedef union 
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  cm_psm_get_cfg_params_req_s_type       cm_psm_get_cfg_params_req;
  cm_psm_enter_req_s_type                cm_psm_enter_req;
} cm_psm_u_type;

/* Structure for set Twwan-911 timer request*/
typedef struct cm_mm_set_t_wwan_911_req_s
{
  msgr_hdr_s              msg_hdr;

  uint16                  t_wwan_911_val;
 
} cm_mm_set_t_wwan_911_req_s_type;

/* Structure for get Twwan-911 timer request*/
typedef struct cm_mm_get_t_wwan_911_req_s
{
  msgr_hdr_s              msg_hdr;
 
} cm_mm_get_t_wwan_911_req_s_type;

/* Structure for reporting Twwan-911 timer to cm clients  
** Rsp corresponding to get request*/
typedef struct cm_mm_get_t_wwan_911_rsp_s
{
  msgr_hdr_s              msg_hdr;

  uint16                  t_wwan_911_val;
 
} cm_mm_get_t_wwan_911_rsp_s_type;


typedef struct cm_3gpp_service_status_s
{
  msgr_hdr_s                 msg_hdr;          
  sys_srv_status_e_type      srv_status;
  sys_srv_domain_e_type      srv_domain;
  sys_sys_mode_e_type        rat;
}cm_3gpp_service_status_s_type;

typedef struct cm_tau_complete_ind_s
{
  msgr_hdr_s             msg_hdr;              /* Message router header */

} cm_tau_complete_ind_s_type;

/* Structure to indicate csfb->srlte swtich*/
typedef struct cm_srlte_switch_s
{
  msgr_hdr_s              msg_hdr;
 
} cm_srlte_switch_s_type;

/*==============================================================================
                           Msg UMID definitions
==============================================================================*/

/*! @brief UMIDs of the messages sent by the other modules to CM
*/
enum
{
  MM_CM_IND_FIRST = MSGR_UMID_BASE(MSGR_MM_CM, MSGR_TYPE_IND),
  
  MSGR_DEFINE_UMID(MM, CM, IND, ACT_DEFAULT_BEARER_CONTEXT_REQUEST, 0x01, \
                   cm_act_default_bearer_context_request_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, ACT_DEDICATED_BEARER_CONTEXT_REQUEST,0x02, \
                   cm_act_dedicated_bearer_context_request_ind_s_type ),
  MSGR_DEFINE_UMID(MM, CM, IND, DEACT_BEARER_CONTEXT_REQUEST,0x03, \
                   cm_deact_bearer_context_request_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, BEARER_RESOURCE_ALLOC_REJECT,0x04, \
                   cm_bearer_resource_alloc_reject_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, MODIFY_BEARER_CONTEXT_REQUEST,0x05, \
                   cm_modify_bearer_context_request_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, PDN_CONNECTIVITY_REJECT,0x06, \
                   cm_pdn_connectivity_reject_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, PDN_DISCONNECT_REJECT,0x07, \
                   cm_pdn_disconnect_reject_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, ACT_DRB_RELEASED,0x08, cm_drb_released_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, DRB_SETUP,0x09, cm_drb_setup_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, DRB_REESTABLISH_FAILED, 0x0a, \
                   cm_drb_reestablish_failed_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, PDN_CONNECTIVITY_FAILURE, 0x0b, \
                   cm_pdn_connectivity_failure_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, BEARER_RESOURCE_ALLOC_FAILURE, 0x0c, \
                   cm_bearer_resource_alloc_failure_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, DRB_REESTABLISH_REJECT, 0x0d, \
                   cm_drb_reestablish_reject_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, GET_PDN_CONNECTIVITY_REQUEST, 0x0e, \
                   cm_get_pdn_connectivity_request_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, BEARER_CONTEXT_MODIFY_REJECT, 0x0f, \
                   cm_bearer_context_modify_reject_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, PDN_CONNECTIVITY_PACKED_REQUEST,0x10, \
                   cm_pdn_connectivity_packed_request_ind_s_type ),
  MSGR_DEFINE_UMID(MM, CM, IND, ESM_NOTIFICATION, 0x11, \
                   cm_esm_notification_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, RSP, 1XCSFB_ABORT, 0x12, \
                   cm_1xCSFB_abort_rsp),
  MSGR_DEFINE_UMID(MM, CM, RSP, 1XCSFB_CALL, 0x13, \
                   cm_1xCSFB_call_rsp),
  MSGR_DEFINE_UMID(MM, CM, IND, CFCM_MONITOR_THERMAL_PA_EM, 0x14, \
                   cm_cfcm_fc_cmd_msg_s),

  /* MM_CM_AC_1XPPP_CLEAR_START_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, AC_1XPPP_CLEAR_START, 0x15, \
                   cm_ac_info_ind_s_type),
  
  /* MM_CM_AC_1XPPP_CLEAR_END_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, AC_1XPPP_CLEAR_END, 0x16, \
                   cm_ac_info_ind_s_type),
  
  /* MM_CM_AC_EMERGENCY_ENTER_REQ - Emergency enter req from client */
  MSGR_DEFINE_UMID(MM, CM, REQ, AC_EMERGENCY_ENTER, 0x17, \
                   cm_ac_emergency_enter_req_s_type),

  /* MM_CM_AC_EMERGENCY_ENTER_IND - Emergency enter rsp to client */
  MSGR_DEFINE_UMID(MM, CM, IND, AC_EMERGENCY_ENTER, 0x18, \
                   cm_ac_info_ind_s_type),

  /* MM_CM_AC_EMERGENCY_READY_IND - Emergency ready state sent to client*/
  MSGR_DEFINE_UMID(MM, CM, IND, AC_EMERGENCY_READY, 0x19, \
                   cm_ac_info_ind_s_type),

  /* MM_CM_AC_EMERGENCY_EXIT_REQ - Emergency exit req from client */
  MSGR_DEFINE_UMID(MM, CM, REQ, AC_EMERGENCY_EXIT, 0x20, \
                   cm_ac_emergency_exit_req_s_type),

  /* MM_CM_AC_EMERGENCY_EXIT_IND - Emergency exit rsp to client */
  MSGR_DEFINE_UMID(MM, CM, IND, AC_EMERGENCY_EXIT, 0x21, \
                   cm_ac_info_ind_s_type),

	/* MM_CM_SRVCC_CONTEXT_RSP - IMS return SRVCC context info to CM */
  MSGR_DEFINE_UMID(MM, CM, RSP, SRVCC_CONTEXT, 0x22, cm_srvcc_call_context_rsp_s_type),
	  
  /* MM_CM_SRVCC_HO_COMPLETE_IND  */
  MSGR_DEFINE_UMID(MM, CM, IND, SRVCC_HO_COMPLETE, 0x23, cm_mid_srvcc_ho_comp_list_s_type),


  /* MM_CM_SUBSC_CHGND_IND  */
  MSGR_DEFINE_UMID(MM, CM, IND, SUBSC_CHGND, 0x24,cm_subsc_chgnd_ind_msg_type),
  
  /* MM_CM_SUBSC_CHGND_RSP  */
  MSGR_DEFINE_UMID(MM, CM, RSP, SUBSC_CHGND, 0x25,cm_subsc_chgnd_rsp_s_type),

   /**< MM_CM_IMS_EMERG_CAP_SUPP_IND Message sent from CM to L-RRC  */
  MSGR_DEFINE_UMID(MM, CM, IND, IMS_EMERG_CAP_SUPP, 0x28,\
                   cm_ims_emerg_cap_ind_s_type),

 /* MM_CM_CSG_SEARCH_TRIGGER_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, CSG_SEARCH_TRIGGER, 0x29, \
                   cm_csg_srch_trigger_ind_s_type),


  /* MM_CM_EMERGENCY_MODE_STATUS_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, EMERGENCY_MODE_STATUS, 0x30, \
                   cm_emerg_mode_status_ind_s_type),

    /* MM_CM_VOLTE_STATUS_IND*/
  MSGR_DEFINE_UMID(MM, CM, IND, VOLTE_STATUS, 0x31, \
                   cm_volte_call_status_ind_s_type),

   /* MM_CM_T3346_INFO_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, T3346_INFO, 0x32, \
                   cm_t3346_info_s_type),

    /* MM_CM_PLMN_LIST_CHANGE_IND*/
  MSGR_DEFINE_UMID(MM, CM, IND, PLMN_LIST_CHANGE, 0x33, \
                   cm_plmn_list_chngd_ind_s_type),

  /* MM_CM_VOLTE_STATE_NOTIFICATION_CMD - Volte Mode Change indication from AP */
  MSGR_DEFINE_UMID(MM, CM, CMD, VOLTE_STATE_NOTIFICATION, 0x34, \
                   cm_volte_state_notification_cmd_s_type),

  /* MM_CM_CALL_MODE_IND - Volte Mode Change Indication from CM 
  to QMI-V
  */
  MSGR_DEFINE_UMID(MM, CM, IND, CALL_MODE, 0x35, \
                   cm_call_mode_ind_s_type),

  MSGR_DEFINE_UMID(MM, CM, IND, AUDIO_RELEASE,0x36, \
                   cm_audio_session_rel_msg_type),

  MSGR_DEFINE_UMID(MM, CM, IND, LTE_ACTIVE_STATUS,0x37, \
                   cm_lte_active_status_ind_s_type),

/* MM_CM_EXT_VOLTE_CALL_STATE_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, EXT_VOLTE_CALL_STATE, 0x38, \
                   cm_ext_volte_call_state_ind_s_type),

  MSGR_DEFINE_UMID(MM, CM, IND, MANUAL_SCAN_FAIL, 0x39, \
                   cm_manual_scan_fail_ind_s_type),
  MSGR_DEFINE_UMID(MM, CM, IND, TRM_PRIORITY, 0x40, \
                   cm_trm_priority_ind_s_type),
   /* MM_CM_RAW_RSSI_IND*/
  MSGR_DEFINE_UMID(MM, CM, IND, RAW_RSSI,0x43, \
                   cm_raw_rssi_msg_type),
                  
  /* MM_CM_PSM_ENTER_REQ - Enter PSM req from client */
  MSGR_DEFINE_UMID(MM, CM, REQ, PSM_ENTER, 0x44, \
                   cm_psm_enter_req_s_type),
  /* MM_CM_PSM_STATUS_IND - PSM status to client */
  MSGR_DEFINE_UMID(MM, CM, IND, PSM_STATUS, 0x45, \
                   cm_psm_status_ind_s_type),

  /* MM_CM_PSM_GET_CFG_PARAMS_REQ - Get PSM config data */
  MSGR_DEFINE_UMID(MM, CM, REQ, PSM_GET_CFG_PARAMS, 0x46, \
                   cm_psm_get_cfg_params_req_s_type),
  /* MM_CM_PSM_GET_CFG_PARAMS_RSP -  Provided PSM config data to client */
  MSGR_DEFINE_UMID(MM, CM, RSP, PSM_GET_CFG_PARAMS, 0x47, \
                   cm_psm_get_cfg_params_rsp_s_type),

  /* MM_CM_SET_T_WWAN_911_REQ*/
  MSGR_DEFINE_UMID(MM, CM, REQ, SET_T_WWAN_911, 0x48, \
                   cm_mm_set_t_wwan_911_req_s_type),

  /* MM_CM_GET_T_WWAN_911_REQ*/
  MSGR_DEFINE_UMID(MM, CM, REQ, GET_T_WWAN_911, 0x49, \
                   cm_mm_get_t_wwan_911_req_s_type),

  /* MM_CM_GET_T_WWAN_911_RSP*/
  MSGR_DEFINE_UMID(MM, CM, RSP, GET_T_WWAN_911, 0x50, \
                   cm_mm_get_t_wwan_911_rsp_s_type),


   /**< MM_CM_3GPP_SRV_IND Message sent from CM to IMS  */
  MSGR_DEFINE_UMID(MM, CM, IND, 3GPP_SRV, 0x52,\
                   cm_3gpp_service_status_s_type),
  
  /* MM_CM_DRVCC_HO_COMPLETE_IND  */
  MSGR_DEFINE_UMID(MM, CM, IND, DRVCC_HO_COMPLETE, 0x53, \
                   cm_mid_srvcc_ho_comp_list_s_type), 

   /* MM_CM_TAU_COMPLETE_IND */
  MSGR_DEFINE_UMID(MM, CM, IND, TAU_COMPLETE, 0x54, \
    cm_tau_complete_ind_s_type),

      /* MM_CM_SET_RPM_PARAMETERS_REQ*/
  MSGR_DEFINE_UMID(MM, CM, REQ, SET_RPM_PARAMETERS, 0x41, \
                   cm_mm_set_rpm_parameters_req_s_type),

      /* MM_CM_GET_RPM_PARAMETERS_REQ*/
  MSGR_DEFINE_UMID(MM, CM, REQ, GET_RPM_PARAMETERS, 0x42, \
                   cm_mm_get_rpm_parameters_req_s_type),

      /* MM_CM_SET_RPM_PARAMETERS_RSP*/
  MSGR_DEFINE_UMID(MM, CM, RSP, SET_RPM_PARAMETERS, 0x43, \
                   cm_mm_set_rpm_parameters_rsp_s_type),

      /* MM_CM_GET_RPM_PARAMETERS_RSP*/
  MSGR_DEFINE_UMID(MM, CM, RSP, GET_RPM_PARAMETERS, 0x44, \
                   cm_mm_get_rpm_parameters_rsp_s_type),

     /* MM_CM_SET_RPM_CONFIG_REQ*/
  MSGR_DEFINE_UMID(MM, CM, REQ, SET_RPM_CONFIG, 0x45, \
                   cm_mm_set_rpm_config_req_s_type),

  /**< MM_CM_SRLTE_SWITCH_IND Message sent from CM to IMS  */
  MSGR_DEFINE_UMID(MM, CM, IND, SRLTE_SWITCH, 0x46,\
                   cm_srlte_switch_s_type),


  MM_CM_IND_MAX,

  MM_CM_IND_LAST = MM_CM_IND_MAX - 1
};



enum
{
    /*=========================================================================*/
    /*            Domain selection output messages                             */
    /*=========================================================================*/

  /**< Message to request the selected domain, requester would fill in the 
       cm_domain_sel_domain_get_req_msg_type struct with the system and the
       call type for which the selected domain info is requested.
  */
  MSGR_DEFINE_UMID(MM, DOM_SEL, REQ, DOMAIN_SELECTED_GET, 0x01,
                   cm_domain_sel_domain_get_req_msg_type),

  /**< Message in response to the selected domain request.
      cm_domain_sel_domain_get_rsp_msg_type will contain the
      requested domain info
   */ 
  MSGR_DEFINE_UMID(MM, DOM_SEL, RSP, DOMAIN_SELECTED_GET, 0x02,
                   cm_domain_sel_domain_get_rsp_msg_type),

  /**< Message indication that the domain selected has changed 
       cm_domain_sel_domain_chg_ind_msg_type will contain the
       changed domain info
   */
  MSGR_DEFINE_UMID(MM, DOM_SEL, IND, DOMAIN_SELECTED_CHANGED, 0x03,
                   cm_domain_sel_domain_chg_ind_msg_type),

/*=========================================================================*/
/*            Domain selection trigger messages                             */
/*=========================================================================*/

  /**< Message sent by CMSDS to indicate when service(s)IMS domain preference 
       has changed. Note that this is just a preference not the selected
       domain yet. This indication should trigger IMS to do the required
       registrations/deregistrations and provide status so that domain
       selection can run and select a domain for a service.
       cm_domain_sel_ims_pref_ind_msg_type contains the info required by IMS
       to do the required registrations/deregistrations.
       */
  MSGR_DEFINE_UMID(MM, DOM_SEL, IND, DOMAIN_SELECTED_IMS_PREF_CHANGED, 0x04,
                   cm_domain_sel_ims_pref_chg_ind_msg_type),

  /**< Message request to get IMS domain preference for a service(s).
       Note that this is just a preference not the selected
       domain yet.
       cm_domain_sel_ims_pref_ind_msg_type contains the info required by IMS
       to do the required registrations/deregistrations.
       */
  MSGR_DEFINE_UMID(MM, DOM_SEL, REQ, DOMAIN_SELECTED_LTE_IMS_PREF_GET, 0x05,
                   cm_domain_sel_ims_pref_req_msg_type),

  /**< Message response in request to get IMS domain preference for a service(s).
       Note that this is just a preference not the selected
       domain yet.
       cm_domain_sel_ims_pref_rsp_msg_type contains the info required by IMS
       to do the required registrations/deregistrations.
       */
  MSGR_DEFINE_UMID(MM, DOM_SEL, RSP, DOMAIN_SELECTED_LTE_IMS_PREF_GET, 0x06,
                   cm_domain_sel_ims_pref_rsp_msg_type),


  /**< Message indication from IMS to CMSDS to update the IMS registration status
       has changed. IMS should fill in cm_domain_sel_update_ims_reg_status_req_msg_type
       with the registration status.
   */
  MSGR_DEFINE_UMID(MM, DOM_SEL, IND, UPDATE_IMS_REG_STATUS,0x07,
                   cm_domain_sel_update_ims_reg_status_req_msg_type),

  /*=========================================================================*/
  /*            Configuration req/rsp/ind messages                           */
  /*=========================================================================*/

  /* Message to CM to request the voice preference  over LTE */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, REQ, LTE_VOICE_DOMAIN_PREF_GET, 0x08,
                   cm_domain_sel_get_config_req_msg_type),
  
  /* Message to CM to request the sms preference over LTE */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, REQ, LTE_SMS_DOMAIN_PREF_GET,0x09,
                   cm_domain_sel_get_config_req_msg_type),
  
  /* Message CM sends in response to a voice preference request*/
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, RSP, LTE_VOICE_DOMAIN_PREF_GET, 0x0a,
                   cm_domain_sel_get_config_rsp_msg_type),
  
  /* Message CM sends in response to a sms preference  request*/
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, RSP, LTE_SMS_DOMAIN_PREF_GET,0x0b,
                   cm_domain_sel_get_config_rsp_msg_type),
  
  /* Message to CM to update the voice preference over LTE */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, REQ, UPDATE_LTE_VOICE_DOMAIN_PREF,0x0c,
                   cm_domain_sel_config_update_req_msg_type),
  
  /* Message to CM to update the sms preference over LTE */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, REQ, UPDATE_LTE_SMS_DOMAIN_PREF, 0x0d,
                   cm_domain_sel_config_update_req_msg_type),
  
  /* Message CM  sends when the preferance for voice has changed */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, IND, LTE_VOICE_DOMAIN_PREF_CHANGED,0x0e,
                   cm_domain_sel_config_chg_ind_msg_type),
  
  /* Message CM sends when the preferance for sms has changed */
  MSGR_DEFINE_UMID(MM, DOM_SEL_CONFIG, IND, LTE_SMS_DOMAIN_PREF_CHANGED,0x0f,
                   cm_domain_sel_config_chg_ind_msg_type),

  /**< Message request to get IMS domain preference for a service(s).
       Note that this is just a preference not the selected
       domain yet.
       This is the extention of DOMAIN_SELECTED_LTE_IMS_PREF_GET_REQ. pair with  
       DOMAIN_SELECTED_IMS_PREF_GET_EXT_RSP
       */
  MSGR_DEFINE_UMID(MM, DOM_SEL, REQ, DOMAIN_SELECTED_IMS_PREF_GET_EXT, 0x10,
                   cm_domain_sel_get_ims_pref_msg_type),

  /**< Message response in request to get IMS domain preference for a service(s).
       Note that this is just a preference not the selected
       domain yet.
       This is the extention of DOMAIN_SELECTED_LTE_IMS_PREF_GET_RSP with reconstructed
       payload data. 
       */
  MSGR_DEFINE_UMID(MM, DOM_SEL, RSP, DOMAIN_SELECTED_IMS_PREF_GET_EXT, 0x11,
                   cm_domain_sel_ims_pref_rsp_msg_type_ext),

};

#endif /* CM_MSGR_MSG_H */

