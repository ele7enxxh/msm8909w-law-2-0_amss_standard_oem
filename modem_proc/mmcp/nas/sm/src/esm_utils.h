
/*!
  @file
  esm_utils.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_utils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Modified input parameters esm_build_and_send_pdn_connectivity_reject_ind to 
                   support Rel-10 ESM backoff timer (T3396) requirement
03/28/12   us       Compilation warnings fixed 
10/07/10   MNK     1XCSFB introduction 
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
10/14/09   MNK     Interface change for building and sending the detach cmd
09/25/09   MNK     Fix warnings
09/21/09   MNK     Changes due to IRAT design
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
07/30/09   MNK     Changes for multiple PDN connectivity support
06/22/09   zren    Updated to use LTE_CM_DRB_REESTABLISH_REJECT_IND for service
                   request procedure failures
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
05/19/09   MNK     Moved timer values to new header file esm_ext_constants.h
05/08/09   MNK     Store the transaction id on retransmission;Removed const on the function parameter 
05/08/09   MNK     Added support for bearer resource allocation procedures
05/01/09   MNK     Added support for N/W initiated bearer context deactivation
04/24/09   MNK     Added detach_cmd message
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/05/09   MNK     Add support for dormancy
==============================================================================*/

#ifndef ESM_UTILS_H
#define ESM_UTILS_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "gs_v.h"
#include "esm_bpm.h"

#define ESM_PRINT_LOW(str, a, b, c) MSG_LOW_DS(SM_SUB,str,a,b,c) 

struct esm_bcm;
struct esm_bpm;
struct esm_eps_bearer_context;
struct esm_bearer_resource_proc;
struct esm_1xcsfbm;

typedef enum 
{
  ESM_BCM = 1,
  ESM_BPM = 2
}esm_module_id_T;

#define ESM_SEQ_NUM_MASK 0x000000ff
#define ESM_INSTANCE_ID_MASK 0x0000ff00
#define ESM_MODULE_MASK 0x00ff0000

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief External type for my clients
*/

/*==============================================================================

  FUNCTION:  esm_build_and_send_pkd_pdn_connectivity_req_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of 
    what esm_build_and_send_pkd_pdn_connectivity_req_msg() does goes here.

    @detail
    Builds, packs and sends the PDN Connectivity message to CM, 
    for verifying this PKD data.
*/
/*============================================================================*/

void esm_build_and_send_pkd_pdn_connectivity_req_msg
(  
  esm_bearer_resource_proc_T          *proc_ptr, 
  const esm_pdn_connectivity_req_T    *msg_ptr
);

boolean esm_validate_OTA_message
(
  lte_nas_esm_incoming_msg_type          *msg_ptr,
  struct esm_bcm                         *mgr_ptr,
  word                                    sdf_id,
  byte                                    lbi,
  lte_nas_message_id                      trigger_msg_id,
  struct esm_eps_bearer_context          *ctxt_ptr,
  lte_nas_esm_hdr_type                   *esm_hdr
);

void esm_build_and_send_autonomous_act_def_ctxt_reject 
(
  const lte_nas_esm_act_default_bearer_context_req* ota_msg_ptr,
  const esm_act_default_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_service_request
( 
  struct esm_eps_bearer_context* ctxt_ptr ,
  sys_lte_reestab_calltype_e call_type_ptr
);

extern void esm_build_and_send_cm_drb_reestablish_reject_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_failure_ind_T* msg_ptr,   /*!< REQUIRED short parameter description */
  byte con_id /*!< Ideally to be used only for case where existing con_id does not exist with ESM */
);

extern byte esm_get_instance_id_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
);


extern lte_nas_esm_hdr_type* esm_get_esm_hdr_from_esm_incoming_msg
(
  lte_nas_esm_incoming_msg_type* in_msg   /*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_cm_act_dedicated_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_act_dedicated_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_accept_T* msg_ptr/*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_act_dedicated_ctxt_reject 
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
);

extern void esm_copy_dedicated_ctxt_data
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
);


extern void esm_bcm_deactivate_all_contexts
(
  struct esm_bcm* mgr_ptr,   
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause
);

extern void esm_build_and_send_autonomous_act_dedicated_ctxt_reject
(
  const lte_nas_esm_act_dedicated_bearer_context_req* ota_msg_ptr,
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr 
);

extern void esm_build_and_send_emm_detach_cmd(lte_nas_local_cause_enum_T local_cause);
extern void esm_build_and_send_deactivate_bearer_context_accept
(
  byte bearer_id,
  protocol_cfg_options_T* prot_ptr
);

extern void esm_build_and_send_cm_bearer_resource_allocation_reject
(
  //const struct esm_bearer_resource_proc* proc_ptr,
  word sdf_id,
  const lte_nas_esm_bearer_resource_allocation_rej* msg_ptr,
  lte_nas_local_cause_enum_T  local_cause,
  byte lbi
);

extern void esm_build_and_send_bearer_resource_alloc_abort_rsp(word sdf_id, boolean result);
extern void esm_send_retransmission(struct esm_bearer_resource_proc* proc_ptr);

boolean esm_start_timer
(
  esm_timer_type                    timer_id,
  struct esm_bearer_resource_proc  *proc_ptr 
);

void esm_stop_timer
( 
  esm_timer_type                    timer_id,
  struct esm_bearer_resource_proc  *proc_ptr
);

void esm_clear_timer
( 
  struct esm_bearer_resource_proc  *proc_ptr
);

extern void esm_build_and_send_bearer_resource_allocation_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const esm_bearer_resource_alloc_req_T* msg_ptr 
);

extern void esm_bcm_deactivate_associated_ded_ctxts
(
  struct esm_bcm* mgr_ptr,   
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause,
  byte linked_bearer_id
);

extern void esm_build_and_send_pdn_connectivity_req_msg
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const esm_pdn_connectivity_req_T* msg_ptr  
);


extern boolean esm_is_detach_required(byte bearer_id, struct esm_bcm* mgr_ptr);
extern void esm_build_and_send_pdn_disconnect_req
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* msg_ptr
);

void esm_build_and_send_cm_pdn_disconnect_reject
(
  byte   lbi,
  const  lte_nas_esm_pdn_disconnect_rej* msg_ptr,
  lte_nas_local_cause_enum_T local_cause
);

extern void esm_build_and_send_deactivate_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,   /*!< REQUIRED short parameter description */
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause
);

extern void esm_build_and_send_pdn_connectivity_abort_rsp(word sdf_id,boolean result);

extern void esm_build_and_send_pdn_connectivity_reject_ind
(
  esm_bearer_resource_proc_T *proc_ptr,
  word sdf_id,
  const lte_nas_pdn_connectivity_rej  *msg_ptr,
  lte_nas_local_cause_enum_T           local_cause,
  emm_failure_type                    *emm_failure_ptr
);

void esm_build_and_send_get_pdn_connectivity_req_ind
(
  uint8 seq_num
);

void esm_temp_pdn_connectivity_req_rsp(void);

void esm_build_and_send_bearer_resrc_mod_req
(
  struct esm_bearer_resource_proc                 *proc_ptr,
  const  esm_bearer_resource_modification_req_T   *msg_ptr
);

void esm_save_modify_ctxt_data
(
  struct esm_eps_bearer_context                     *ctxt_ptr,
  const  lte_nas_esm_modify_eps_bearer_ctxt_req     *msg_ptr
);

void esm_apply_modify_ctxt_data
(
  struct esm_eps_bearer_context                     *ctxt_ptr,
  const  esm_modify_bearer_context_accept_T         *msg_ptr
);

void esm_build_and_send_cm_modify_ctxt_req
(
  struct esm_eps_bearer_context   *ctxt_ptr
);

void esm_build_and_send_modify_ctxt_accept
(
  struct esm_eps_bearer_context               *ctxt_ptr,  
  const  esm_modify_bearer_context_accept_T   *msg_ptr
);

void esm_build_and_send_modify_ctxt_rej
(
  struct esm_eps_bearer_context             *ctxt_ptr,  
  const  esm_modify_bearer_context_rej_T    *msg_ptr
);

void esm_build_and_send_autonomous_modify_ctxt_reject 
(
  const lte_nas_esm_modify_eps_bearer_ctxt_req      *modify_req_ptr,
  const esm_modify_bearer_context_rej_T             *modify_rej_ptr
);

void esm_bpm_build_and_send_autonomous_reject
(
  const lte_nas_esm_incoming_msg_type   *data_ptr,
  esm_cause_enum_T                       esm_cause
);

void esm_build_and_send_cm_bearer_resrc_mod_reject
(
  word                                              sdf_id,
  const  lte_nas_esm_bearer_resource_modifi_rej    *msg_ptr,
  lte_nas_local_cause_enum_T                        local_cause,
  byte                                              lbi
);

void esm_build_and_send_esm_info_rsp 
(
  struct esm_bearer_resource_proc   *proc_ptr
);

void esm_send_esm_status 
(
  const lte_nas_esm_hdr_type        *esm_hdr_ptr,
  esm_cause_enum_T                   esm_cause
);

boolean esm_validate_bearer_resrc_mod_req_bearer_id
(
  byte                   bearer_id,
  const struct esm_bpm  *mgr_ptr
);

void esm_chk_ds_explicit_deact
(
  struct esm_bearer_resource_proc  *proc_ptr
);

void esm_build_and_send_eps_bearer_status_update_ind
(
  const struct esm_bcm* mgr_ptr,
  lte_nas_isr_status_type isr_status
);

#define esm_modem_mem_alloc(x)  esm_modem_mem_alloc_debug(x, #x)

/*===========================================================================

FUNCTION   ESM_MODEM_MEM_ALLOC_DEBUG

DESCRIPTION
  This function allocates dynamic memory using modem heap. This function simply
  calls modem_mem_alloc and returns pointer to the allocated memory.  
  The memory allocated is initialized with zeros.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None

===========================================================================*/
extern void *esm_modem_mem_alloc_debug
(
  size_t               size,  /* size of memory to be allocated */
  char                *objname /* Used to see the name of the object being freed if "sizeof" operator is used. */
);

#define esm_modem_mem_free(x) esm_modem_mem_free_debug(x, #x)

/*===========================================================================

FUNCTION   ESM_MODEM_MEM_FREE_DEBUG

DESCRIPTION
   This function returuns the specified memory back to the Heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void esm_modem_mem_free_debug
(
  void                *ptr,
  char                *objname
);

/*===========================================================================

FUNCTION   ESM_BUILD_AND_SEND_ESM_NOTIFICATION

DESCRIPTION
   This function builds and sends the ESM notification message to CM
   with the Notification Indicator as SYS_SRVCC_HO_CANCELLED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_build_and_send_esm_notification(void);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern void esm_post_timer_expired(unsigned long param);
#ifdef FEATURE_DUAL_SIM
void esm_post_timer_expired_sub2(unsigned long param);
#endif

void esm_build_and_send_1xcsfb_call_req(esm_1xCSFB_call_req_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr);

void esm_build_and_send_1xcsfb_abort_req(esm_1xCSFB_abort_req_T* msg_ptr);

void esm_build_and_send_1xcsfb_call_rsp(emm_1xCSFB_esr_call_rsp_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr);

void esm_build_and_send_1xcsfb_call_abort_rsp(emm_1xCSFB_esr_call_abort_rsp_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr);

#ifdef FEATURE_LTE_REL9 
/*===========================================================================

FUNCTION  ESM_CHK_PDN_CONN_ALLOWANCE

DESCRIPTION
  This function checks if PDN CONN procedure is allowed

DEPENDENCIES
  None

RETURN VALUE
  TRUE   PDN CONN procedure is allowed
  FALSE  PDN CONN procedure is not allowed

SIDE EFFECTS
  None

===========================================================================*/
boolean  esm_chk_pdn_conn_allowance
(
  esm_pdn_connectivity_req_T  *msg_ptr,
  const struct esm_bpm        *mgr_ptr
);

/*===========================================================================

FUNCTION  ESM_PDN_CONN_FAILURE_UPDATE_EMC_SRV_STATUS

DESCRIPTION
  This function updates emergency service status upon PDN CONN failure

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void  esm_pdn_conn_failure_update_emc_srv_status
(
  struct esm_bearer_resource_proc  *proc_ptr
);

/*===========================================================================

FUNCTION  ESM_BEARER_SETUP_FAILURE_UPDATE_EMC_SRV_STATUS

DESCRIPTION
  This function updates emergency service status upon emergency bearer setup failure

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
extern void esm_bearer_setup_failure_update_emc_srv_status
(
  const esm_act_default_bearer_context_rej_T* msg_ptr 
);

#endif

/*===========================================================================

FUNCTION  ESM_GET_CONNECTION_ID

DESCRIPTION
  This function calcuates the connection ID using bearer ID passed as param

DEPENDENCIES
  None

RETURN VALUE
  byte
 
SIDE EFFECTS
  None

===========================================================================*/
byte  esm_get_connection_id
(
  byte bearer_id
);


/*===========================================================================

FUNCTION  ESM_MSGR_SEND

DESCRIPTION
  This function logs the message before calling msgr_send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

errno_enum_type esm_msgr_send
(
  msgr_hdr_struct_type*     msg_ptr,
  uint32                     msg_len
);


/*===========================================================================

FUNCTION  ESM_COPY_ESM_CMD_MSG

DESCRIPTION
  This function copies the individual struct member in the source esm_msg union
  pointer to the destination pointer by checking the message id in the header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void esm_copy_esm_cmd_msg
(
  esm_msg*     srcPtr,
  esm_msg*     destPtr
);

extern lte_3gpp_release_ver_e emm_get_3gpp_rel_ver_per_sub
(
  sys_modem_as_id_e_type as_id
);

#endif /*FEATURE_LTE*/

#endif /* ESM_UTILS_H */


