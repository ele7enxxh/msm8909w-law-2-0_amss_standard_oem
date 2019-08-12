#include "mmcp_variation.h"
#ifdef FEATURE_LTE

 /*!
   @file
   esm_proc_state.h
 
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
 
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_proc_state.h#1 $
 
when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/08/09   MNK     Added support for bearer resource allocation procedures
04/22/09   MNK     Changes for compiling on Linux
 ==============================================================================*/
 
 #ifndef ESM_PROC_H
 #define ESM_PROC_H
 
 /*==============================================================================
 
                            INCLUDE FILES
 
 ==============================================================================*/
 
#include "comdef.h"
#include "esm_cm_msg_if.h"
#include "esm_emm_msg_if.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef enum
{
  ESM_BP_INACTIVE,  /*!< ESM Bearer Procedure In-Active State */
  ESM_BP_ACTIVE,  /*!<  ESM Bearer Procedure Active State */
  ESM_BP_ABORT  /*!< ESM Bearer Procedure Abort State  */                                
} esm_bp_state_T;


typedef struct esm_proc_state* esm_proc_state_ptr_type ;

struct esm_bearer_resource_proc;

typedef void  esm_proc_pdn_connect_req_fp 
(
  struct esm_bearer_resource_proc* proc_ptr,  
  const esm_pdn_connectivity_req_T* msg_ptr 
);

typedef void  (esm_proc_bearer_resource_alloc_req_fp)
(
 struct esm_bearer_resource_proc* proc_ptr, 
 const esm_bearer_resource_alloc_req_T* msg_ptr
);

typedef void  (esm_proc_act_default_bearer_context_req_fp)
(
 struct esm_bearer_resource_proc* proc_ptr, 
 const byte* msg_ptr
);

typedef void  (esm_proc_bearer_resource_allocation_abort_fp)
(
 struct esm_bearer_resource_proc* proc_ptr 
);

typedef boolean esm_proc_process_OTA_message_fp
(
  struct esm_bearer_resource_proc* proc_ptr, 
  lte_nas_esm_incoming_msg_type* data_ptr
);
typedef esm_bp_state_T esm_proc_state_enum_fp (void);
typedef void esm_proc_state_name_fp (void);
typedef char* esm_proc_get_state_name_fp (void);

typedef boolean esm_proc_process_timer_expiry_or_lower_layer_failure_fp
(
  struct esm_bearer_resource_proc  *proc_ptr,
  esm_failure_ind_T                *failure_ind_ptr
);

typedef void esm_proc_process_pdn_disconnect_req_fp
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* dat_ptr
);

typedef void  (esm_proc_pdn_connectivity_abort_fp)
(
 struct esm_bearer_resource_proc* proc_ptr 
);

typedef void  (esm_proc_clean_up_fp)
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_T             local_cause
);

typedef void  (esm_proc_process_bearer_resrc_mod_req_fp)
(
  struct esm_bearer_resource_proc                 *proc_ptr, 
  const  esm_bearer_resource_modification_req_T   *msg_ptr
);

typedef struct esm_proc_state
{
  esm_proc_pdn_connect_req_fp                              *esm_proc_process_pdn_connect_req; 
  esm_proc_bearer_resource_alloc_req_fp                    *esm_proc_process_bearer_resource_alloc_req;
  esm_proc_act_default_bearer_context_req_fp               *esm_proc_process_act_default_bearer_ctxt_req;
  esm_proc_process_OTA_message_fp                          *esm_proc_process_OTA_message;
  esm_proc_state_enum_fp                                   *state;
  esm_proc_state_name_fp                                   *state_name;
  esm_proc_get_state_name_fp                               *get_state_name;
  esm_proc_bearer_resource_allocation_abort_fp             *esm_proc_process_bearer_resource_alloc_abort;
  esm_proc_process_timer_expiry_or_lower_layer_failure_fp  *esm_proc_process_timer_expiry_or_lower_layer_failure;
  esm_proc_process_pdn_disconnect_req_fp                   *esm_proc_process_pdn_disconnec_req;
  esm_proc_pdn_connectivity_abort_fp                       *esm_proc_process_pdn_connectivity_abort;
  esm_proc_clean_up_fp                                     *esm_proc_clean_up;
  esm_proc_process_bearer_resrc_mod_req_fp                 *esm_proc_process_bearer_resrc_mod_req;

} esm_proc_state_type;

extern esm_proc_state_type esm_proc_state_inactive;
extern esm_proc_state_type esm_proc_state_active;
extern esm_proc_state_type esm_proc_state_abort;

void   esm_proc_discard_pdn_connect_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const esm_pdn_connectivity_req_T* msg_ptr
) ;
extern void   esm_proc_discard_bearer_resource_alloc_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const esm_bearer_resource_alloc_req_T* msg_ptr
) ;

extern void   esm_proc_discard_act_default_bearer_ctxt_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const byte* msg_ptr
) ;

extern void    esm_proc_discard_bearer_resource_abort_req
(
  struct esm_bearer_resource_proc* proc_ptr 
);

boolean esm_proc_discard_timer_expiry_or_lower_layer_failure
(
  struct esm_bearer_resource_proc  *proc_ptr,
  esm_failure_ind_T                *failure_ind_ptr
);

extern void esm_proc_send_cm_autonomous_reject
(
  struct esm_bearer_resource_proc*  proc_ptr,
  lte_nas_local_cause_enum_T        nas_local_cause,
  emm_failure_type                 *emm_failure_ptr
);

extern void esm_proc_discard_pdn_disconnect_req
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* dat_ptr
);

extern void    esm_proc_discard_pdn_connectivity_abort_req
(
  struct esm_bearer_resource_proc* proc_ptr 
);

extern void    esm_proc_discard_detach_ind
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_T             local_cause
);

void    esm_proc_discard_bearer_resrc_mod_req
(
  struct esm_bearer_resource_proc                 *proc_ptr, 
  const  esm_bearer_resource_modification_req_T   *msg_ptr
);

extern void esm_proc_clean_up
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_T             local_cause
);

void  esm_proc_process_pdn_connect_req
(
  struct esm_bearer_resource_proc        *proc_ptr, 
  const esm_pdn_connectivity_req_T       *msg_ptr
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void get_pdn_connectivity_msg
(
  byte   *ptr2esm_msg
);

 #endif /* ESM_PROC_H */

#endif /*FEATURE_LTE*/
