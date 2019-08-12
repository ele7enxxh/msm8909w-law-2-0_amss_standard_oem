
/*!
  @file
  esm_context_state.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_state.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
07/31/09   MNK     Add handlers for modify bearer context to the context state machine
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/22/09   MNK     Changes for compiling on Linux
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/05/09   MNK     Add Dormancy support
==============================================================================*/

#ifndef ESM_CONTEXT_STATE_H
#define ESM_CONTEXT_STATE_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
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
//MNK
/*! @brief External type for my clients
*/

/* Forword declaration */
struct esm_bcm;

typedef enum
{
  ESM_BC_INACTIVE,  /*!< ESM Bearer Context In-Active State */
  ESM_BC_ACTIVE_PENDING,  /*!<  ESM Context Procedure Active State */
  ESM_BC_ACTIVE,  /*!< ESM Bearer Context Abort State  */
  ESM_BC_MODIFY_PENDING  /*!< ESM Context Procedure Abort State  */
} esm_bc_state_T;

typedef struct esm_ctxt_state_s* esm_ctxt_state_ptr_type ;

struct esm_eps_bearer_context;

/*
** Function pointers used in the definition of esm_state_type.
*/
typedef void esm_ctxt_act_default_bearer_ctxt_req_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr
);
typedef void esm_ctxt_act_dedicated_bearer_ctxt_req_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr /* over the air message structure */
);
typedef void esm_ctxt_act_default_bearer_ctxt_accept_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_default_bearer_context_accept_T* msg_ptr /* over the air message structure */
);

typedef void esm_ctxt_act_dedicated_bearer_ctxt_accept_fp        
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_dedicated_bearer_context_accept_T* msg_ptr, /* over the air message structure */
  struct esm_bcm* mgr_ptr
);

typedef void esm_ctxt_act_default_bearer_ctxt_reject_fp        
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_default_bearer_context_rej_T* msg_ptr /* over the air message structure */
);

typedef void esm_ctxt_act_dedicated_bearer_ctxt_reject_fp        
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr /* over the air message structure */
);

typedef void esm_ctxt_sig_conn_released_ind_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_sig_connection_released_ind_T* msg_ptr
);

typedef void esm_ctxt_active_eps_ind_fp
(
  struct esm_eps_bearer_context  *ctxt_ptr, 
  const esm_active_eps_ind_T     *msg_ptr,
  struct esm_bcm                 *mgr_ptr
);                                                                                      

typedef void esm_ctxt_deact_bearer_ctxt_req_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const  lte_nas_esm_deact_eps_bearer_context_req* msg_ptr/* over the air message structure */
);

typedef void esm_ctxt_failure_ind_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_failure_ind_T* msg_ptr  /* over the air message structure */
);

typedef esm_bc_state_T esm_ctxt_state_enum_fp (void);
typedef void esm_ctxt_state_name_fp (void);
typedef char* esm_ctxt_get_state_name_fp (void);

typedef void esm_ctxt_change_state_fp         
(
  struct esm_eps_bearer_context* ctxt_ptr,
  esm_ctxt_state_ptr_type state
);

typedef void esm_ctxt_modify_bearer_ctxt_req_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const  lte_nas_esm_modify_eps_bearer_ctxt_req* msg_ptr
);

typedef void esm_ctxt_modify_bearer_ctxt_rej_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const  esm_modify_bearer_context_rej_T* msg_ptr
);

typedef void esm_ctxt_modify_bearer_ctxt_accept_fp
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const  esm_modify_bearer_context_accept_T* msg_ptr,
  struct esm_bcm* mgr_ptr
);

typedef struct esm_ctxt_state_s
{
  esm_ctxt_act_default_bearer_ctxt_req_fp*  ctxt_process_act_default_bearer_ctxt_req;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_act_dedicated_bearer_ctxt_req_fp*  ctxt_process_act_dedicated_bearer_ctxt_req;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_act_default_bearer_ctxt_accept_fp*  ctxt_process_act_default_bearer_ctxt_accept;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_act_default_bearer_ctxt_reject_fp*  ctxt_process_act_default_bearer_ctxt_reject;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_act_dedicated_bearer_ctxt_accept_fp*  ctxt_process_act_dedicated_bearer_ctxt_accept;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_act_dedicated_bearer_ctxt_reject_fp* ctxt_process_act_dedicated_bearer_ctxt_reject;

  esm_ctxt_deact_bearer_ctxt_req_fp*  ctxt_process_deact_bearer_ctxt_req;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */

  esm_ctxt_sig_conn_released_ind_fp* ctxt_process_sig_conn_released_ind;
  esm_ctxt_active_eps_ind_fp*        ctxt_process_active_eps_ind;

  esm_ctxt_state_enum_fp* state;  /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */

  esm_ctxt_state_name_fp* state_name;   /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_get_state_name_fp* get_state_name;   /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */
  esm_ctxt_failure_ind_fp* ctxt_process_esm_failure_ind;   /*!< Brief description of this member, which can wrap around to the next line/s if necessary. */

  esm_ctxt_change_state_fp* ctxt_change_state;

  esm_ctxt_modify_bearer_ctxt_req_fp* ctxt_process_modify_bearer_ctxt_req;
  esm_ctxt_modify_bearer_ctxt_rej_fp* ctxt_process_modify_bearer_ctxt_rej;
  esm_ctxt_modify_bearer_ctxt_accept_fp* ctxt_process_modify_bearer_ctxt_accept;

} esm_ctxt_state_type;

/* function declarations These need to be defined in the generic state file*/

extern void esm_ctxt_discard_act_default_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_default_bearer_context_accept_T* msg_ptr
) ;
extern void esm_ctxt_discard_act_dedicated_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_dedicated_bearer_context_accept_T* msg_ptr,
  struct esm_bcm* mgr_ptr
) ;

extern void esm_ctxt_discard_act_default_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr
) ;
extern void esm_ctxt_discard_act_dedicated_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr
) ;
extern void esm_ctxt_discard_sig_conn_released_ind
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_sig_connection_released_ind_T* msg_ptr
);
extern void esm_ctxt_discard_active_eps_ind
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  const esm_active_eps_ind_T     *msg_ptr,
  struct esm_bcm                 *mgr_ptr
);

extern void esm_ctxt_discard_deact_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description may wrap to the next lines if necessary. */
  const  lte_nas_esm_deact_eps_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_ctxt_discard_default_bearer_ctxt_reject        
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_default_bearer_context_rej_T* msg_ptr /* over the air message structure */
);

extern void esm_ctxt_process_sig_conn_released_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_sig_connection_released_ind_T* msg_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_ctxt_process_active_eps_ind
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  const esm_active_eps_ind_T     *msg_ptr,
  struct esm_bcm                 *mgr_ptr
);

extern void esm_ctxt_discard_esm_failure_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_failure_ind_T* msg_ptr    /*!< REQUIRED short parameter description */
);

extern void      esm_ctxt_discard_dedicated_bearer_ctxt_reject        
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_ctxt_change_state         
(
  struct esm_eps_bearer_context* ctxt_ptr,
  esm_ctxt_state_ptr_type new_state_ptr
);

extern void esm_ctxt_discard_modify_bearer_ctxt_req             
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_modify_eps_bearer_ctxt_req* msg_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_ctxt_discard_modify_bearer_ctxt_rej             
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_modify_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
);

extern void  esm_ctxt_discard_modify_bearer_ctxt_accept            
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_modify_bearer_context_accept_T* msg_ptr,   /*!< REQUIRED short parameter description */
  struct esm_bcm* mgr_ptr
);

extern esm_ctxt_state_type esm_ctxt_state_inactive;
extern esm_ctxt_state_type esm_ctxt_state_active_pending;
extern esm_ctxt_state_type esm_ctxt_state_active;                
extern esm_ctxt_state_type esm_ctxt_state_modify_pending;

/*===========================================================================

FUNCTION   ESM_DEACT_CTXT_UPON_SYNC_UP

DESCRIPTION
  This function deactivates the given bearer context and its assiciated dedicated
  bearer contexts if it is a default one. It also aborts related ESM procedure
  before the deactivation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_deact_ctxt_upon_sync_up
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  lte_nas_local_cause_T           local_cause,
  esm_cause_T                     ota_cause,
  struct esm_bcm                 *mgr_ptr
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* ESM_CONTEXT_STATE_H */

#endif /*FEATURE_LTE*/
