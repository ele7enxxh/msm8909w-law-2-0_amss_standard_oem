
/*!
  @file
  esm_bcm.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_bcm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/23/12   us      Removing unsused static buffer from esm_bcm_T.
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
10/21/10   MNK     Deactivation of bearer contexts not transferable to GW upon IRAT 
06/01/10   zren    Updated ESM to sync bearer contexts status with EMM when SM 
                   updated ESM
03/26/10   MNK     Add more i-RAT(GSM) field support
03/11/10   MNK     Changes for transaction_id optional param in def & ded bearer context requests
02/17/10   MNK     Add support for context updates when in UMTS
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
02/22/09   MNK     Added support for Dormancy

==============================================================================*/

#ifndef ESM_BCM_H
#define ESM_BCM_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "esm_context_state.h"
#include "lte_nas_local_cause.h"
#include "lte_nas_esm_message_description.h"
#include "esm.h"
#include "esm_v.h"
#include "esm_common.h"
#include "cm_gw.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define ESM_MAX_EPS_BEARERS     11
#define ESM_STARTING_BEARER_ID  5
#define ESM_ENDING_BEARER_ID    15

/* We are assuming here that atmost 2 messages can be pending per bearer 
when on attach default bearer context is being setup and ESM needs to 
buffer the dedicated bearer setups received while the on attach default 
bearer is being setup. Those 2 messages are dedicated bearer setup and 
it's corresponding ACTIVE_EPS_IND(DRB_SETUP_IND). One EPS_IND has all 
EPSbearer/DRB info. This one also includes the EPS_IND for the default 
bearer as well*/
#define ESM_MAX_PENDING_MSGS 2*ESM_MAX_EPS_BEARERS

#define ESM_UNDEFINED_RB_ID  0

typedef void esm_emm_context_status_update_func_fp(void);

typedef void esm_bcm_deactivate_associated_ded_ctxts_fp
(
  lte_nas_local_cause_T       local_cause,
  esm_cause_T                 ota_cause,
  eps_bearer_id_T             bearer_id
);

/* Function pointer to deactivate bearer contexts */
typedef void esm_bcm_deactivate_ctxts_fp
(
  lte_nas_local_cause_T       local_cause,
  esm_cause_T                 ota_cause,
  eps_bearer_id_T             lbi
);

/* Structure used to store bearer context modification data */
typedef struct esm_bearer_context_modify_data
{
  sdf_qos_T                sdf_qos;
  tft_type_T               tft;
  qos_T                    neg_qos;
  llc_sapi_T               neg_llc_sapi;
  rad_priority_T           radio_pri;
  pfi_T                    packet_flow_id;
  apn_ambr_T               apn_ambr;
  protocol_cfg_options_T   config_options;

}esm_bearer_context_modify_data_T;

typedef void esm_bcm_abort_proc_fp(byte bearer_id);

/*! @brief This represents a bearer context in the ESM
*/
typedef struct esm_eps_bearer_context
{
  byte connection_id; /*!< Connection id of the context which is the key into the context list */
  eps_bearer_id_T eps_bearer_id;  /*!< eps bearer id */
  sdf_qos_T sdf_qos;  /*Qos*/
  gsm_umts_connection_id_T ti;/*Equivalent transaction id for GSM/UMTS*/
  /* Group ID used for primary PDP */
  byte  sm_group_id; 
  pdn_address_T  pdn_address; /*!< PDN Address i.e IP address */
  apn_T apn_name; /*!< Access point name  */
  qos_T  neg_qos;  /*!< Qos */
  llc_sapi_T neg_llc_sapi; 
  rad_priority_T radio_pri;
  pfi_T packet_flow_id;  /*!< Packet Flow Id */
  apn_ambr_T   apn_ambr;  /* APN aggregate max bit rate */
  protocol_cfg_options_T config_options;  /*!< Configuration options  PCO */
  esm_cause_T esm_cause; /*!< Brief description of this member */
  lte_nas_local_cause_T esm_local_cause; /*!< ESM local cause value for the message */
  byte pti; /*PTI used when context was created*/
  byte lbi;  /*!< Linked EPS bearer Id */
  esm_ctxt_state_ptr_type  eps_bearer_state_ptr;  /*!< Current state of the bearer context */
  tft_type_T tft;  /*!< Uplink TFT */
  word sdf_id;
  msgr_tech_module_type con_id_creator;/* Connection Id creator*/
  //pending_msg_T pending_msg ; /*!< The message pending currently  that needs to be sent out */
  lte_nas_message_id last_msg_sent;
  boolean on_attach_default_bearer; /* This flag is used to decide if the bearer need to be deactivated if a 
                                               failure happens, because of lower layer couldn't send the default bearer 
                                               context accept */
  boolean default_bearer_ctxt;
  esm_emm_context_status_update_func_fp* context_status_update_func_ptr;
  esm_bcm_deactivate_associated_ded_ctxts_fp* deactivate_associated_dedicated_ctxts_func_ptr;
  //byte associated_dedicated_bearers[ESM_MAX_EPS_BEARERS];
  byte rb_id; /* Added for Test Loop back functionality and hence the for logging. 
                Just pass it on to CM/DS. Not to be used for any other purposes*/
  /* Structure used to store bearer context modification data */
  esm_bearer_context_modify_data_T        bc_modify_data;
  sys_pdp_header_comp_e_type      h_comp;
  sys_pdp_data_comp_e_type        d_comp;
  isr_ctxt_act_status_T                  isr_bearer_ctxt_act_status;
#ifdef FEATURE_LTE_REL9
  boolean    emc_bc;
#endif
  bearer_ctrl_mode_T      bcm;
  byte       unclaimed_rb_id;
  timer_3gpp_type3         backoff_timer;
}esm_eps_bearer_context_T;

typedef struct
{
  esm_cmd_type pending_cmd;
  boolean valid;
}bcm_pending_msg;

struct esm_bcm;

typedef void handle_bcm_cm_message_fp ( struct esm_cmd* cmd, struct esm_bcm* mgr, byte con_id);
typedef void handle_bcm_emm_message_fp ( struct esm_cmd* cmd, struct esm_bcm* mgr);


typedef struct esm_bcm
{
  esm_eps_bearer_context_T  *esm_bc[ESM_MAX_EPS_BEARERS];  /*!<  */
  handle_bcm_cm_message_fp* handle_cm_message_ptr ;
  handle_bcm_emm_message_fp* handle_emm_message_ptr ;
  esm_bcm_deactivate_ctxts_fp    *deact_ctxts_ptr;
  esm_bc_on_attach_def_bearer_state_T on_attach_def_bearer_pending;
  boolean isr_status;
  boolean first_def_bearer_ctxt_received;
  esm_bcm_abort_proc_fp  *abort_proc_ptr;
} esm_bcm_T;


typedef  esm_eps_bearer_context_T* (esm_bcm_allocate_ctxt_fp_type)(esm_bcm_T* mgr_ptr,byte bearer_id);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void init_esm_bcm
(
  esm_bcm_T                                    *mgr_ptr,
  esm_emm_context_status_update_func_fp        *func_ptr,
  esm_bcm_deactivate_associated_ded_ctxts_fp   *deact_func_ptr, 
  esm_bcm_deactivate_ctxts_fp                  *deact_ctxts_ptr,
  esm_bcm_allocate_ctxt_fp_type                **bcm_allocate_ctxt_fp,
  esm_bcm_abort_proc_fp                        *abort_proc_ptr
);

extern void handle_bearer_context_message
(
 esm_bcm_T   *mgr_ptr
);

struct esm_eps_bearer_context* esm_bcm_find_bearer_context
(
  esm_bcm_T* mgr_ptr,  /*!< REQUIRED short parameter description */
  eps_bearer_id_T bearer_id    /*!< REQUIRED short parameter description */
);

void esm_bcm_reset_ctxt_data
(
  struct esm_eps_bearer_context  *ctxt_ptr
);

void esm_bcm_reset_ctxt
(
  struct esm_eps_bearer_context* ctxt_ptr
);


void esm_bcm_deactivate_non_transferable_ctxts_to_GW(esm_bcm_T* mgr_ptr);

void send_esm_ctxt_info_log(esm_eps_bearer_context_T *ctxt_ptr);

void esm_init_bcm_context_fn_ptr(esm_eps_bearer_context_T *esm_bc);


void esm_init_bcm_context_fn(esm_eps_bearer_context_T *esm_bc,
	                              esm_emm_context_status_update_func_fp *update_func_ptr,
                                  esm_bcm_deactivate_associated_ded_ctxts_fp *deact_func_ptr);


#endif /* ESM_BCM_H */

#endif /*FEATURE_LTE*/
