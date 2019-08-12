#ifndef SM_CM_H
#define SM_CM_H

/*===========================================================================
                        S M - C M  Header File

DESCRIPTION

   Definition of Primitive message types between Session Management Entity
   and Call Manager layer (SM <-> CM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm_cm.h_v   1.6   21 Nov 2001 15:56:16   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_cm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   am      Back off timer code
07/31/12   us      NAS should always send negotiated length of OTA message, MAX_OCTETS_L3_MESSAGE to CM 
                   when NAS rejects any PDP activation/modification request, & this field is supposed to
                   be looked at only when the reject cause is SM_MESSAGE_EXCEED_MAX_L2_LIMIT.
07/30/08   rm      CMI defeaturisation
08/04/06   ss      Implemented changes for ROHC and IP header compression.
07/03/06   ss      Added GPRS call control fixes
04/04/06  kvk     Add HTORPC meta-comments to export CM.
11/11/05   ss      Release 5 SM PCO and TFT changes put in.
09/13/05   kvk    MO PDP Modify supported. Added new enum PDP_MODIFY_COLLISION to
                  sm_cm_status_T staructure and req_minqos to smreg_pdp_modify_req structure
09/02/04   kvk     Added new CM primitives SMREG_PDP_DEACTIVATE_PROMOTE_IND, 
                  SMREG_PDP_DEACTIVATE_PROMOTE_CNF to promote SEC pdp to pri.
                  Added group_id in smreg_pdp_activate_cnf to pass group id to CM.
06/30/04   kvk     New primitive smreg_ps_sig_rel_ind_T added.
06/03/04   ks      Added SNDCP h_comp and d_comp params to PDP context info.
06/03/04   kvk     Added new error codes in enum sm_cm_cause_T for SM internal error failures
08/05/03   mks     Added minimum_qos field in smreg_pdp_activate_req and 
                   smreg_pdp_activate_sec_req data types. 
11/20/01   ks      Added new enum pdp_cause_T and also added connection_id
                   in the response messages from SM to CM.
08/01/01   ks      Added PDP_INACTIVE_MAX_TIMEOUT in sm_cm_status_T enum
06/04/01   ks      removed the unnecesary fields APN and PDP_ADDR from the
                   primitive smreg_pdp_activate_rej_resp_T.
05/23/01   ks      Added smreg_pdp_deactivate_rej_T primitive.
04/30/01   ks      Added more comments to the functions.
01/26/01   ks      Created file.
            
===========================================================================*/

#include "ms.h"
#include "sys.h"      /* system wide common types */

#include "sys_v.h"
#include "cm_gw.h"    /* Defintions from NAS required by clients */ 
#include "sm_common.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between SM and CM (SM-CM) at the service SMREG-SAP.
* These are as defined in Table 6.5.1 of TR 24.007.
* --------------------------------------------------------------------------*/

/* Note: Messages from CM to SM are defined in smtask.h*/

/* Messages from SM to CM */                                 
/* Note: This Enum will be removed once CM defines in one of its headers */
/*  now in CM */
typedef enum 
{
  SMREG_PDP_ACTIVATE_CNF,
  SMREG_PDP_ACTIVATE_REJ,
  SMREG_PDP_ACTIVATE_IND,
  SMREG_PDP_DEACTIVATE_CNF,
  SMREG_PDP_DEACTIVATE_IND,
  SMREG_PDP_DEACTIVATE_PROMOTE_IND,
  SMREG_PDP_DEACTIVATE_PROMOTE_CNF,
  SMREG_PDP_DEACTIVATE_REJ,
  SMREG_PDP_MODIFY_IND,
  SMREG_PDP_MODIFY_CNF,
  SMREG_PDP_MODIFY_REJ,
  SMREG_PDP_ACTIVATE_SEC_CNF,  
  SMREG_PDP_ACTIVATE_SEC_REJ,
  SMREG_PDP_SIG_CON_REL_IND,

  SMREG_PDP_SIG_CON_REL_CNF,


  SMREG_MBMS_ACTIVATE_IND,
  SMREG_MBMS_ACTIVATE_CNF,
  SMREG_MBMS_ACTIVATE_REJ,


  SMREG_PDP_ACTIVATE_SEC_IND,
  SMREG_PDP_NW_MODIFY_REQ,

  SMREG_PDP_UNKNOWN_REQ
}cm_sm_cmd_type;


/* --------------------------------------------------------------------------
 Enumeration that identifies the status of ue initiated ps signal release
** ------------------------------------------------------------------------*/
typedef enum
{
  SM_PS_SIG_REL_SUCCESS,
  SM_PS_SIG_REL_FAILURE
} ps_sig_rel_status_T;


/**************************************************************************
      MESSAGES FROM CM TO SM , DEFINED IN Table 6.5.1 in TS 24.007
**************************************************************************/ 

/* --------------------------------------------------------------------------
** NOTE: All the data types inside these primitives are described in 
**      SM_COMMON.H file.  
** ------------------------------------------------------------------------*/                    

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ACTIVATE_REQ, 
** Description: Mobile originated (MO) PDP context activation Req.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_req
{ 
  qos_T                  qos;
  qos_T                  minimum_qos;
  pdp_address_T          pdp_addr;
  apn_T                  apn_name;
  protocol_cfg_options_T config_options;

  sys_pdp_header_comp_e_type    h_comp;          /* SNDCP IP Header Compression reqd */
  sys_pdp_data_comp_e_type       d_comp;          /* SNDCP Data Compression reqd */

  llc_sapi_T             llc_sapi;
  nsapi_T                nsapi;
  byte                    trans_id;
  byte                    sm_pd;
  byte                    msg_type;
  uint16                    context_id;
  sys_modem_as_id_e_type    as_id;
  pdp_request_type_T        request_type;

} smreg_pdp_activate_req_T;

/* ------------------------------------------------------------------------
** Primitive  : SMREG-PDP-MODIFY-REQ,
** Description: Mobile Originated PDP context Modification request.
** -----------------------------------------------------------------------*/
typedef struct smreg_pdp_modify_req
{

  nsapi_T             nsapi;

  qos_T                  qos;
  qos_T                  minimum_qos;
  tft_type_T             tft;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;

} smreg_pdp_modify_req_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG_PDP_DEACTIVATE_REQ,
** Description: Mobile Originated PDP context deactivation Request.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deactivate_req
{
  nsapi_T            nsapi;
  ti_T               tear_down_ind;
  sm_status_T        cause;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;

} smreg_pdp_deactivate_req_T;

/* ------------------------------------------------------------------------
** Primitive  : SMREG-PDP-ACTIVATE-SEC-REQ,
** Description: Mobile Originated Secondary PDP activation Req.
** ----------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_sec_req
{
  nsapi_T                nsapi;
  qos_T                  qos;
  qos_T                  minimum_qos;
  tft_type_T             tft;
  nsapi_T                primary_nsapi;
  byte                   group_id;

  sys_pdp_header_comp_e_type    h_comp;          /* SNDCP IP Header Compression reqd */
  sys_pdp_data_comp_e_type       d_comp;          /* SNDCP Data Compression reqd */


  protocol_cfg_options_T config_options;


  llc_sapi_T             llc_sapi;
  byte                    trans_id;
  byte                    sm_pd;
  byte                    msg_type;

  uint16                    context_id;
  uint16                    assoc_context_id;

  sys_modem_as_id_e_type    as_id;

} smreg_pdp_activate_sec_req_T;

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ACTIVATE_REJ_RESP,
** Description: Mobile sends the PDP activation failed response to the 
                (Mobile Terminated) PDP activation request.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_rej_resp
{
  sm_status_T            cause;    

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;

} smreg_pdp_activate_rej_resp_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-NW-MODIFY-CNF
** Description: DS accepting MT PDP modification request.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_nw_modify_cnf
{
  sys_modem_as_id_e_type    as_id;
}smreg_pdp_nw_modify_cnf_T;


/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-NW-MODIFY-REJ
** Description: DS rejected MT PDP modification request based on BCM constaints.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_nw_modify_rej
{
  sm_status_T            cause;
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_nw_modify_rej_T;




/**************************************************************************
*           MESSAGES FROM SM TO CM , DEFINED IN Table 6.5.1 in TS 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ACTIVATE_CNF, 
** Description: Confirmation for the MO PDP activation request message.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_cnf
{ 
  byte                   connection_id;
  nsapi_T                nsapi;
  byte                   group_id;
  qos_T                  qos;
  pdp_address_T          pdp_addr;
  protocol_cfg_options_T config_options;
  boolean                is_net_alloc_qos_diff_from_req;

  sys_modem_as_id_e_type    as_id;


  byte                  esm_conn_id;


  cause_enum_type_T  cause_type;
  pdp_cause_T cause;

} smreg_pdp_activate_cnf_T;

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ACTIVATE_REJ, 
** Description: MO PDP activation request rejected by the network.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_rej
{ 
  byte                   connection_id;
  cause_enum_type_T  cause_type;
  pdp_cause_T            cause;
  nsapi_T                nsapi;
  protocol_cfg_options_T config_options;

  sys_modem_as_id_e_type    as_id;
  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;
  
  pdp_reject_ind_params_s_type pdp_reject_param;
  
} smreg_pdp_activate_rej_T;

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ACTIVATE_IND,
** Description: Mobile Terminated (MT) PDP activation indication. Network
                has initiated a PDP activation request.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_ind
{ 
  byte                   connection_id;
  byte                   group_id;
  pdp_address_T          pdp_addr;
  apn_T                  apn_name;

  protocol_cfg_options_T config_options;


  llc_sapi_T             llc_sapi;
  nsapi_T                nsapi;
  byte                    sm_pd;
  byte                    msg_type;


  sys_modem_as_id_e_type    as_id;

} smreg_pdp_activate_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-DEACTIVATE-CNF
** Description: Confirmation for the MO PDP deactivation request.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deactivate_cnf
{
  byte                 connection_id;
  nsapi_T              nsapi;

  protocol_cfg_options_T config_options;



  mbms_prot_config_options_T mbms_config_options;



  sys_modem_as_id_e_type    as_id;

} smreg_pdp_deactivate_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-DEACTIVATE-IND
** Description: Mobile Terminated (MT) PDP deactivation indication.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deactivate_ind
{
  byte                  connection_id;
  nsapi_T               nsapi;
  ti_T                  tear_down_ind;
  cause_enum_type_T     cause_type;
  pdp_cause_T           cause;

  protocol_cfg_options_T config_options;


  mbms_prot_config_options_T mbms_config_options;

  timer_3gpp_type3    backoff_timer;

  sys_modem_as_id_e_type    as_id;


} smreg_pdp_deactivate_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG_PDP_DEACT_PROMOTE_IND
** Description: Mobile Terminated (MT) PDP deactivation indication
                and prmotion of secondary to primary.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deact_promote_ind
{
  byte                  connection_id;
  nsapi_T               nsapi;
  byte                  promote_con_id;//For promoting sec to primary
  cause_enum_type_T     cause_type;
  pdp_cause_T           cause;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;

}smreg_pdp_deact_promote_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG_PDP_DEACT_PROMOTE_CNF
** Description: Mobile Terminated (MT) PDP deactivation indication
                and prmotion of secondary to primary.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deact_promote_cnf
{
  byte                  connection_id;
  nsapi_T               nsapi;
  byte                  promote_con_id;//For promoting sec to primary

  protocol_cfg_options_T config_options;
  sys_modem_as_id_e_type    as_id;
}smreg_pdp_deact_promote_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-MODIFY-IND
** Description: Indication of a successful MT PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_modify_ind
{
  byte              connection_id;
  nsapi_T           nsapi;
  qos_T             qos;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;


  tft_type_T             tft;

} smreg_pdp_modify_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-MODIFY-CNF,
** Description: Confirmation for MO PDP modification request, Network 
                has accepted the Mobile proposed PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_modify_cnf
{
  byte              connection_id;
  nsapi_T           nsapi;
  qos_T             qos;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;

} smreg_pdp_modify_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-MODIFY-REJ
** Description: Rejection of MO PDP modification request, Network 
                has rejected the Mobile proposed PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_modify_rej
{
  byte                 connection_id;
  cause_enum_type_T    cause_type;
  pdp_cause_T          cause;
  nsapi_T                nsapi;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;
  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;

} smreg_pdp_modify_rej_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-DEACTIVATE-REJ
** Description: Rejection of MO PDP modification request, Network 
                has rejected the Mobile proposed PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_deactivate_rej
{
  byte                 connection_id;
  cause_enum_type_T    cause_type;
  pdp_cause_T          cause;
  nsapi_T                nsapi;

  sys_modem_as_id_e_type    as_id;

} smreg_pdp_deactivate_rej_T;


/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-ACTIVATE-SEC-CNF  
** Description: Confirmation for MO SEC PDP modification request, Network 
                has accepted the Mobile proposed SEC PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_sec_cnf
{
  byte                   connection_id;
  nsapi_T                nsapi;
  qos_T                  qos;
  boolean                is_net_alloc_qos_diff_from_req;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;


  byte                      esm_conn_id;

} smreg_pdp_activate_sec_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-ACTIVATE-SEC-REJ  
** Description: Rejection of MO SEC PDP modification request, Network 
                has rejected the Mobile proposed SEC PDP modification.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_sec_rej
{
  byte                   connection_id;
  nsapi_T                nsapi;
  cause_enum_type_T      cause_type;
  pdp_cause_T            cause;

  protocol_cfg_options_T config_options;


  sys_modem_as_id_e_type    as_id;
  byte nas_max_ota_negotiated_len;

  timer_3gpp_type3    backoff_timer;   

  pdp_reject_ind_params_s_type pdp_reject_param;

} smreg_pdp_activate_sec_rej_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG_PS_SIG_REL_IND  
** Description: Indication of SIG connection release.
** -------------------------------------------------------------------*/
typedef struct smreg_ps_sig_rel_ind
{
   cause_enum_type_T      cause_type;

  sys_modem_as_id_e_type    as_id;

} smreg_ps_sig_rel_ind_T;


/* ---------------------------------------------------------------------
** Primitive  : SMREG_PS_SIG_REL_CNF  
** Description: Indication of SIG connection release.
** -------------------------------------------------------------------*/
typedef struct smreg_ps_sig_rel_cnf
{
  ps_sig_rel_status_T status;

  sys_modem_as_id_e_type    as_id;

} smreg_ps_sig_rel_cnf_T;




/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_ABORT_REQ, 
** Description: Locally deactivate all the PDPs
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_abort_req
{ 
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_abort_req_T;



/* --------------------------------------------------------------------------
** Primitive  : SMREG_PS_SIGNALING_REL_REQ, 
** Description: Locally deactivate all the PDPs
** ------------------------------------------------------------------------*/
typedef enum
{
  SMREG_PS_SIG_REL_REQ_CAUSE_MIN,
  SMREG_UE_INITIATED_DORMANCY,
  SMREG_DUAL_STANDBY_CS_MO_CALL,
  SMREG_DUAL_TRIPLE_STANDBY_CHANGE,
  SMREG_PS_SIG_REL_REQ_CAUSE_MAX
} smreg_ps_signalling_rel_req_cause_T;

typedef struct smreg_ps_signalling_rel_req
{ 
  sys_modem_as_id_e_type               as_id;
  smreg_ps_signalling_rel_req_cause_T  cause;
} smreg_ps_signalling_rel_req_T;

/* --------------------------------------------------------------------------
** Primitive  : SMREG_PDP_GRACEFUL_ABORT_REQ, 
** Description: Handling graceful PDP abort request
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_graceful_abort_req
{ 
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_graceful_abort_req_T;



/* --------------------------------------------------------------------------
** Primitive  : SMREG_SEC_PDP_ACTIVATE_IND,
** Description: Mobile Terminated (MT) Secondary PDP activation indication. Network
                has initiated a Secondary PDP activation request.
** ------------------------------------------------------------------------*/
typedef struct smreg_pdp_activate_sec_ind
{ 
  byte                   connection_id;
  byte                   group_id;
  tft_type_T             tft;
  protocol_cfg_options_T config_options;
  qos_T                  qos;
  llc_sapi_T             llc_sapi;
  nsapi_T                nsapi;
  byte                    sm_pd;
  byte                    msg_type;
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_activate_sec_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-NW-MODIFY-REQ
** Description: Request for DS to validate TFT params.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_nw_modify_req
{
  byte              connection_id;
  nsapi_T           nsapi;   
  tft_type_T             tft;
  protocol_cfg_options_T config_options;
  qos_T              qos;
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_nw_modify_req_T;


/* ---------------------------------------------------------------------
** Primitive  : SMREG-PDP-NW-MODIFY-REQ
** Description: Request for DS to validate TFT params.
** -------------------------------------------------------------------*/
typedef struct smreg_pdp_graceful_abort_cnf
{
  sys_modem_as_id_e_type    as_id;
} smreg_pdp_graceful_abort_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : CM-UNBLOCK-ALL-APN-IND
** Description: Request for DS to unblock all APNs.
** -------------------------------------------------------------------*/
typedef struct smreg_unblock_all_apn
{
  sys_modem_as_id_e_type    as_id;
} smreg_unblock_all_apn_ind_T;

/* ---------------------------------------------------------------------
** Primitive  : SMREG-BLOCKED_APN-LIST
** Description: List of all blocked APN from DS
** -------------------------------------------------------------------*/
typedef struct smreg_blocked_apn_list
{
  apn_T                  apn_name[MAX_BLOCKED_APN];
  uint16                  length;
  sys_modem_as_id_e_type    as_id;
} smreg_blocked_apn_list_ind_T;

#endif
