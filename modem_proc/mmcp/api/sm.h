#ifndef SM_SM_H
#define SM_SM_H

/*===========================================================================
                        S M  Header File

DESCRIPTION

   Contains the definitions and enumerations used in the SM.  This includes
   the SM-SM peer messages, Various SM states, Timers and external functions.
   This file is used internally among the SM related source files.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm.h_v   1.11   18 Jul 2002 15:43:26   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
08/18/06   rk       Added Extended TI Feature Code
08/04/06   ss      Implemented changes for ROHC and IP header compression.
07/17/06   ss       Delaying Local deactivation for 2 secs to avoid RAB setup failure.
07/03/06   ss       Added GPRS call control fixes
06/07/06   ss       Added prototype for is_WtoGCCO_in_prgress
01/04/06   kvk     Defined new variable ps_signalling_conn_state to store the status of 
                   PS siganlling connection
11/22/05  kvk/hj   GCF12.9.9: Defined EXTENDED_ESTABLISH_PENDING_TIME TO 8s for GCF
11/09/05   kvk     Added prototype for sm_force_gmmsm_close_session_req()
09/20/05   kvk     Added prototype sm_get_most_demanding_traffic_class() to get
                   the most demanding traffic class for all PDPs.
09/13/05   kvk     MO PDP Modify supported. Adedd new member req_qos,req_minqos and req_tft
                   to the pdp_context_info structure.
09/12/05   kvk     Added extern variable gmm_anite_gcf_flag to check for the
                   GCF flag.
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
05/16/05   ss      Added qos_ie_length as global to check for R99 QoS parameters
01/04/05   kvk     Changed TIMER ESTABLISH PENDING  from 3 sec to 6 sec
12/06/04   kvk     New prototype GET_NSAPI() is added to get NSAPI from connection id.
11/30/04   kvk     Added prototype SM_GET_PDP_CONTEXT_STATUS() for get pdp state for
                   NSAPIs 
10/18/04   kvk     MT PDP support added.
10/07/04   kvk     Added new enum  pdp_addr_alloc_type_T for PDP address
                    allocation type.
09/02/04  kvk      Added members grou_id and is_net_alloc_qos_diff_from_req to 
                   pdp_list state object. Added new funtion prototypes 
                   SM_CM_DEACT_GROUP(), SM_PROMOTE_SEC_PRI() and SM_PICK_BEST_QOS()
06/07/04  kvk      Added sm_process_local_pdp_deactivation() function
06/03/04   ks      Added SNDCP h_comp and d_comp params to PDP context info.
02/28/04   ks      Added new enumeration sig_conn_state_T for Connection mgmt
                   of PDP context. Added pdp_con_state in pdp_context_info(). 
11/24/03   ks      Added support for TIMER_ESTABLISH_PENDING for handling the
                   GMM connection before starting the PDP activation request.
08/25/03   ks      Corrected the LLC_SAPI_9 value from 7 to 9
08/05/03   mks     Added minimum_qos and qos_valid fields in pdp_context_info
                   data type. Added function prototypes sm_validate_qos() and
                   sm_handle_qos_validation_failure.
05/09/03   ks      Added extern variable sm_crit_sect for critical section 
                   handling
03/18/03   mks     Added inter_system_change_pending to indicate if an inter
                   system change is pending (waiting for SNDCP response).     
02/28/03   ks      Added new function convert_to_rrc_est_cause() to convert
                   Traffic class into RRC connection cause.
02/24/03   ks      Removed redundant declaration of exterm sm_tcb type. Added
                   extern is_gsm_mode() function; cleanred Lint errors
01/16/03   ks      Added send_gmm_close_session_req() primitive, removed the
                   extern mm_abort_rr() function.
12/17/02   ks      Added cn_domain_id enumeration the domain ID during 
                   CLOSE session request.  
12/02/02   ks      Added new enum values in llc_state_T
07/18/02   ks      Added is_umts_mode() extern function
07/01/02   ks      added llc_state in pdp_context_info and corresponding
                   enum values.
06/13/02   ks      defined max_timer_count variable and mm_abort_rr
                   as extern
08/24/01   ks      Added extern variables: pdp_active_timer,pdp_modify_timer
                   pdp_inactive_timer,rabm_resp_timer & sm_tcb.
08/01/01   ks      Added sm_set_pdp_timer() extern function to set a
                   PDP context timer
06/27/01   ks      Added sig_connection_available flag to indicate if the
                   RRC signaling connection is established/released.
06/04/01   ks      Added send_gmm_establish_req() function.
05/24/01   ks      Added Packet Flow Identifier 'IE' in the pdp_context_info
                   structure.  Removed redundant send_gmm_sm_pdu() definition.
05/17/01   ks      Added all the external functions used in the SM source
                   files.
04/30/01   ks      Added a Macro GET_TRANSID to get transaction ID when an
                   NSAPI is given.
01/26/01   ks      Created file.
            
===========================================================================*/


/* <EJECT> */
/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "err.h"
#include "sm_common.h"
#include "sys_stru.h"
#include "rabmtask.h"
#include "naslog.h"
#include "sys.h"
#include "sys_v.h"


#include "modem_mem.h"


/*=========================================================================
                 DEFINITIONS AND DECLARATIONS FOR THE MODULE
===========================================================================*/

extern word sm_pd;  // Defines the SM protocol discriminator

#define INVALID_ID    0xff
#define UNKNOWN_CMD   0
#define TIMER_PENDING 0

#define MAX_PDP_NUM_NORMAL_TI         16

/* ---------------------------------------------------------------------------
* PDP ISR status
* --------------------------------------------------------------------------*/
typedef enum
{
  CTXT_ACT_BEFORE_ISR,
  CTXT_ACT_AFTER_ISR_IN_GW,
  CTXT_ACT_AFTER_ISR_IN_LTE
} isr_ctxt_act_status_T;


/* CN domain identifier */
typedef enum
{ SM_CS_DOMAIN_CN_ID,
  SM_PS_DOMAIN_CN_ID
} cn_domain_id;

typedef enum
{
  SM_DEACT_ISR_ON_INTERRAT_NONE,
  SM_DEACT_ISR_ON_INTERRAT_TO_LTE,
  SM_DEACT_ISR_ON_INTERRAT_TO_GW
} sm_deact_isr_on_interrat_status_T;



/* ---------------------------------------------------------------------------
* LLC-SAPI definitions for valid LLC_SAPIs for GPRS.
* --------------------------------------------------------------------------*/
#define LLC_SAPI_UNASSIGNED   0
#define LLC_SAPI_3            3
#define LLC_SAPI_5            5
#define LLC_SAPI_9            9
#define LLC_SAPI_11           11


/*PDP address allocation types*/
typedef enum
{
   PDP_ADDR_STATIC,
   PDP_ADDR_DYNAMIC
}pdp_addr_alloc_type_T;

/* -----------------------------------------------------------------------------
* Message Types for sm to sm peer messages, defined in Table 10.4a of TS 24.008.
* ----------------------------------------------------------------------------*/
typedef enum
{
  ACTIVATE_PDP_REQUEST        = 0x41,
  ACTIVATE_PDP_ACCEPT         ,
  ACTIVATE_PDP_REJECT         ,
  REQUEST_PDP_ACTIVATION      ,
  REQUEST_PDP_ACTIVATION_REJ  ,   
  DEACTIVATE_PDP_REQUEST      ,
  DEACTIVATE_PDP_ACCEPT       ,
  NETWORK_MODIFY_PDP_REQUEST  ,
  MS_MODIFY_PDP_ACCEPT        ,
  MS_MODIFY_PDP_REQUEST       ,
  MODIFY_PDP_ACCEPT           ,
  MODIFY_PDP_REJECT           ,
  ACTIVATE_SEC_PDP_REQUEST    ,
  ACTIVATE_SEC_PDP_ACCEPT     ,
  ACTIVATE_SEC_PDP_REJECT     ,
  SM_STATUS = 0x55
 
  ,ACTIVATE_MBMS_REQUEST,
  ACTIVATE_MBMS_ACCEPT,
  ACTIVATE_MBMS_REJECT,
  REQUEST_MBMS_ACTIVATION,
  REQUEST_MBMS_ACTIVATION_REJ
  
 
  , REQUEST_SEC_PDP_ACTIVATION = 0x5B,
  REQUEST_SEC_PDP_ACTIVATION_REJ
 
} sm_layer_msg_T;

/*=========================================================================
                 DEFINITIONS OF STRUCTURES OF MESSAGES
===========================================================================*/

/* ---------------------------------------------------------------------------
* PDP Context state machine enumeration.
* --------------------------------------------------------------------------*/
typedef enum
{
  PDP_INACTIVE,
  PDP_ACTIVE_PENDING,
  PDP_ACTIVE,
  PDP_MODIFY_PENDING,
  PDP_INACTIVE_PENDING
  
  ,MBMS_ACTIVE_PENDING,
  MBMS_ACTIVE
 
} pdp_state_T;

/* ---------------------------------------------------------------------------
* PDP Context state machine enumeration.
* --------------------------------------------------------------------------*/
typedef enum
{
  SIG_CONNECTION_NONE,
  SIG_CONNECTION_PENDING,
  SIG_CONNECTION_AVAILABLE
} sig_conn_state_T;
  
/* ---------------------------------------------------------------------------
* State machine enumeration for RABM responses.
* --------------------------------------------------------------------------*/
/* Enum for the RAB states */
typedef enum
{
  RABM_INITIAL,
  RABM_ACTIVE_PENDING,
  RABM_ACTIVATED,
  RABM_MODIFY_PENDING,
  RABM_MODIFIED,
  RABM_INACTIVE_PENDING,
  RABM_DEACTIVATED
}rab_state_T;

/* ---------------------------------------------------------------------------
* State machine enumeration for LLC confirmation responses.
* --------------------------------------------------------------------------*/
/* Enum for the RAB states */
typedef enum
{
  LLC_NOT_CONFIRMED,
  LLC_MO_ACTIVATE_CNF_PENDING,
  LLC_MT_ACTIVATE_CNF_PENDING,
  LLC_MO_MODIFY_CNF_PENDING,
  LLC_MT_MODIFY_CNF_PENDING,
  LLC_MO_DEACTIVATE_CNF_PENDING,
  LLC_MT_DEACTIVATE_CNF_PENDING,
  LLC_ACTIVE
}llc_state_T;


/* ---------------------------------------------------------------------------
* Enumeration for encode/decode L3 message return status.
* --------------------------------------------------------------------------*/
typedef enum
{
  L3_PARSE_SUCCESS,
  L3_PARSE_FAIL,
  L3_PARSE_INVALID_PD_TI,
  L3_INVALID_MANDATORY_INFO,
  L3_PARSE_INVALID,
  L3_UNSUPPORTED_MSG,
  L3_UNSUPPORTED_IE
} xlate_status_T;


/* ---------------------------------------------------------------------------
* This structure defines the characteristis of a PDP context.
* --------------------------------------------------------------------------*/
typedef struct 
{
  byte                   trans_id;        /* Transaction ID */
  pdp_state_T            pdp_state;       /* PDP state */
  sig_conn_state_T       pdp_con_state;    /* PDP signaling connection state */
  rab_state_T            rab_state;       /* RABM state indicator */
  pdp_address_T          pdp_addr;        /* PDP Address */
  llc_state_T            llc_state;       /* LLC state info during GPRS */
  boolean                mt_orig_type;    /* Mobile Terminated call Ind */
  pdp_addr_alloc_type_T  pdp_addr_alloc_type; /*PDP address allocation type(static/dynamic)*/
  nsapi_T                nsapi;           /* NSAPI value */
  byte                   group_id;        /* Group id fro group of pri and sec*/
  llc_sapi_T             llc_sapi;        /* LLC_SAPI value */
  //llc_sapi_T             req_llc_sapi;    /* Requested LLC SAPI value */
  qos_T                  qos;             /* Quality of service */
  qos_T                  req_qos;         /* QoS requested with PDP Modify*/
  qos_T                  minimum_qos;     /* Minimum Quality of service */
  qos_T                  req_minqos;      /* Min Qos with Modify*/
  boolean                is_net_alloc_qos_diff_from_req;/*To indicate 
                                           Requested is from allocated*/
  apn_T                  apn_name;        /* APN name */
  rad_priority_T         radio_pri;       /* Radio Priority */

  rad_priority_T     req_radio_pri;
  llc_sapi_T            req_llc_sapi; 
  pfi_T                    req_flow_id;  

  boolean                is_primary_pdp;  /* indicates if primary context */
  tft_type_T             tft;             /* Traffic Flow template */
  tft_type_T             req_tft;         /* TFT requested with PDP Modify*/
  byte                   linked_ti;       /* Identifies a linked TI */
  protocol_cfg_options_T config_options;  /* Configuration options */
  cc_cause_T             current_cause;   /* Identifies current cause */
  cause_enum_type_T      cause_type;      /* Identifies cause type */
  ti_T                   teardown_ind;    /* Teardown indicator */
  pfi_T                  flow_id;         /* Packet Flow Identifier */
  int                    cm_pending_msg;  /* Current CM pending request */
  boolean                qos_valid;       /* Network offered QoS is acceptable or not */

  sys_pdp_header_comp_e_type    h_comp;          /* SNDCP IP Header Compression reqd */
  sys_pdp_data_comp_e_type       d_comp;          /* SNDCP Data Compression reqd */

  boolean                       is_mbms;                        /*This field determines whether this context is MBMS or not*/
  pdp_address_T                 multicast_addr;
  mbms_prot_config_options_T        mbms_prot_cnfg; /*MBMS protocol configuration*/
  mbms_bearer_cap_T             mbms_bear_cap;  /*MBMS bearer Capabilities*/
  tmgi_T                        tmgi;                               /*TMGI*/
  sys_ip_address_s_type        mip;
 
  cc_cause_T         status_message_cause; /* temp storage for cause received in status message*/
 
  bearer_ctrl_mode_T      bcm;  /*Bearer control mode */
  

  sm_layer_msg_T sm_msg_to_resend;
  boolean status_ind_flag;


  byte esm_conn_id;

  isr_ctxt_act_status_T isr_pdp_act_status;

  uint16                    context_id;
  uint16                    assoc_context_id;


  sys_modem_as_id_e_type    as_id;

  timer_3gpp_type3  sm_gprs_type3_timer;
  pdp_request_type_T     request_type;       /* To store the PDP request type*/

} pdp_context_info;

/* ---------------------------------------------------------------------------
* Data type for list of PDP contexts SM manages.
* --------------------------------------------------------------------------*/
extern pdp_context_info  *pdp_list[];

extern pdp_context_info  pdp_list_static[];

extern pdp_context_info  *pdp_list_sim[MAX_AS_IDS][MAX_PDP_NUM_NORMAL_TI];
extern pdp_context_info  pdp_list_static_sim[MAX_AS_IDS][MAX_PDP_NUM_NORMAL_TI];

extern boolean sm_ps_vote_for_reselection;

/* -----------------------------------------------------------------------
FUNCTION SM_RESERVE_NSAPI

DESCRIPTION
  This function reserves a NSAPI for a given connection_id. The validity of
  connection_id is not checked.
-------------------------------------------------------------------------*/
extern byte sm_reserve_nsapi ( byte connection_id );

/* -----------------------------------------------------------------------
FUNCTION SM_RESERVE_NSAPI

DESCRIPTION
  This function reserves a NSAPI for a given connection_id. The validity of
  connection_id is not checked.
-------------------------------------------------------------------------*/
extern byte sm_per_subs_reserve_nsapi ( byte connection_id,
                                        sys_modem_as_id_e_type as_id );

/* -----------------------------------------------------------------------
FUNCTION SM_RELEASE_NSAPI

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
-------------------------------------------------------------------------*/
extern int sm_release_nsapi ( byte connection_id );

/* -----------------------------------------------------------------------
FUNCTION SM_RELEASE_NSAPI

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
-------------------------------------------------------------------------*/
extern int sm_per_subs_release_nsapi ( byte connection_id,
                                       sys_modem_as_id_e_type as_id );

/* --------------------------------------------------------------------------
**FUNCTION    :SM_FORCE_GMMSM_CLOSE_SESSION_REQ
**DESCRIPTION :Sends GMMSM_CLOSESSION_REQ unconditionally
** ------------------------------------------------------------------------*/
extern void sm_force_gmmsm_close_session_req(void);


/*===========================================================================
FUNCTION SM_OK_FOR_TUNEAWAY

DESCRIPTION
  This function checks if
  any of the existing PDPs are of CONVERSATION or STREAMING class traffic type.
  PDP that are getting deactivated are exluded as they are going to be cleared.
  This indicates, if the lower layers can tuneaway.
  
DEPENDENCIES
  
  
RETURN VALUE

  return FALSE, if the above condition passes. Implies NOT to tuneaway

SIDE EFFECTS

===========================================================================*/

extern boolean sm_ok_for_tuneaway(void);

/*--------------------------------------------------------------------------------------------------
** Function used by RRC to know if PS call establishment is on going and RAB is not established yet.
** [ As per FR 19450 : When only signalling bearers are established and PS call is being activated, this API is used by TRRC ]
**   to block Reselections ]
**---------------------------------------------------------------------------------------------------*/
extern nas_vote_for_reselection_e_type sm_ps_call_vote_for_reselection(sys_modem_as_id_e_type asid);


#endif /* SM_SM_H */
