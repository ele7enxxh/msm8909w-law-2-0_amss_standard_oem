#include "mmcp_variation.h"
#ifdef FEATURE_LTE
/*!
  @file
  esm_bpm.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_bpm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/03/10   zren    Added ESM new timer machanism
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/21/09   MNK     Changes due to IRAT design
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
06/02/09   MNK     Added LBI to the procedure 
05/08/09   MNK     Added support for bearer resource allocation procedures
02/24/09   MNK     Added RESERVED PTI value
==============================================================================*/

#ifndef ESM_BPM_H
#define ESM_BPM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "esm_v.h"       
#include "lte_nas_esm_message_description.h"
#include "esm_proc_state.h"
#include "emm_esm_if_msg.h"
#include "esm_common.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define BPM_PTI_START 1  
#define ESM_MAX_BP_PROCS 44 
#define ESM_RESERVED_PTI 0xff
#define ESM_BPM_MAX_ATTEMPTS 4

typedef void esm_bpm_deactivate_pdn_ctxts_fp(lte_nas_local_cause_T local_cause,esm_cause_T ota_cause,eps_bearer_id_T bearer_id);
typedef boolean esm_bpm_chk_on_attach_def_bc_setup_fp(void);

typedef struct esm_bearer_resource_proc
{
  boolean inuse;
  byte pti; /*!< Brief description of this member */
  word sdf_id ; /*!< Brief description of this member */
  byte lbi;
  byte instance_id;/* Initialized at the tartup and Instance Id should never be reset*/               

  esm_proc_state_ptr_type esm_proc_state_ptr;      

  /* Pointer to the message that is pending the response from the network */
  emm_data_req_T  *pending_msg_ptr;

  byte retransmit_counter;
  esm_bpm_deactivate_pdn_ctxts_fp    *request_esm_to_deactivate_ctxts_func_ptr;
  esm_bpm_chk_on_attach_def_bc_setup_fp    *chk_on_attach_def_bc_setup_ptr;

  /* Flag to indicate whether to send PCO in ESM INFO RSP message or not */
  boolean                 pco_ciphering_flag;
  /* Pointer to the timer for proc instance */
  rex_timer_type         *timer_ptr;
  /* Local cause may be used by DS to indicate special operation 
     e.g. DS_EXPLICIT_DEACT in UE initiated modfication req means deactivate 
     the bearer no matter what is the NW response */
  lte_nas_local_cause_T   esm_local_cause;  

} esm_bearer_resource_proc_T;        

struct esm_bpm;

typedef boolean handle_bpm_message_fp ( struct esm_cmd* cmd, struct esm_bpm* mgr);
typedef boolean esm_check_if_detach_to_be_triggered_fp(byte bearer_id);
typedef void esm_find_bearer_context_fp
(
  byte                                bearer_id, 
  esm_bearer_context_info_shared_T   *bc_info_shared_ptr
);

typedef struct esm_bpm
{
  esm_bearer_resource_proc_T                esm_bp[ESM_MAX_BP_PROCS]; 
  handle_bpm_message_fp                    *handle_message_ptr ;
  boolean                                   pti_list[255];
  esm_check_if_detach_to_be_triggered_fp   *check_detach_trigger;
  esm_find_bearer_context_fp               *find_bearer_context_ptr;

  byte                                     cur_pti_index;
  uint8                                    esm_emm_attach_seq_num;
} esm_bpm_T;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void init_esm_bpm
(
 esm_bpm_T                                *mgr_ptr,
 esm_check_if_detach_to_be_triggered_fp   *chk_trigger_ptr,
 esm_bpm_deactivate_pdn_ctxts_fp          *deact_func_ptr,
 esm_find_bearer_context_fp               *find_bearer_context_ptr,
 esm_bpm_chk_on_attach_def_bc_setup_fp    *chk_on_attach_def_bc_setup_ptr
);

extern byte esm_generate_pti
(
  void 
);    
void esm_bpm_process_timer_expiry
(
  struct esm_cmd* cmd_ptr, 
  struct esm_bpm* mgr_ptr
);

boolean esm_validate_pdn_disconnect_req_bearer_id
(
  byte             bearer_id,
  esm_bpm_T       *mgr_ptr
);

/*===========================================================================

FUNCTION  ESM_BPM_ABORT_PROC_BY_BID

DESCRIPTION
  This function aborts the ongoing ESM procedure which is associated to the
  given bearer context 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_bpm_abort_proc_by_bid
(
  byte        bearer_id,
  esm_bpm_T  *mgr_ptr
);

/*===========================================================================

FUNCTION  ESM_BPM_CLEAN_UP

DESCRIPTION
  This function aborts all pending ESM procedures and releases all resources

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_bpm_clean_up
(
  struct esm_bpm        *mgr_ptr,
  lte_nas_local_cause_T  local_cause
);

#endif /* ESM_BPM_H */

#endif /*FEATURE_LTE*/
