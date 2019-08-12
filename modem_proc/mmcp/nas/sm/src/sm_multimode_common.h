#ifndef SM_MULTIMODE_COMMON_H
#define SM_MULTIMODE_COMMON_H
/*===========================================================================

        S M   MULTIMODE I N F O R M A T I O N   H E A D E R   F I L E

DESCRIPTION
  This file contains types, function prototypes and data declarations  
  required for MM Information procedure

Copyright (c) 2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/11   rajesh  Added prototype for function "sm_esm_isr_status_set"
02/16/10   MNK     First cut of i-RAT between LTE and GW

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "mmcp_variation.h"
#include <stdio.h>
#include <string.h>
#include "customer.h"

#ifdef FEATURE_LTE
#include "comdef.h"
#include "esm_bcm.h"
#include "sm_v.h"
#include "esm_bpm.h"

extern sm_deact_isr_on_interrat_status_T sm_deact_isr_on_interrat_status;

/*===========================================================================

FUNCTION  SM_UPDATE_SM_CONTEXT_FROM_ESM

DESCRIPTION
  This function update corresponding SM PDP context upon bearer context 
  activation and deactivation in ESM

DEPENDENCIES
  None

RETURN VALUE
  SM Group ID 

SIDE EFFECTS
  None

===========================================================================*/
byte sm_update_sm_context_from_esm
(
  const esm_eps_bearer_context_T  *esm_bc,
  esm_bc_state_T                   bc_state 
);

byte esm_update_esm_context_from_sm(const pdp_context_info* pdp_ctxt_ptr);

void esm_deactivate_bearer_ctxts_irat_to_GW(void);
void sm_esm_isr_status_set(boolean isr_status);

/*==============================================================================

  FUNCTION:  sm_deact_isr_on_interrat_status_set

==============================================================================*/
extern void sm_deact_isr_on_interrat_status_set
(
  sm_deact_isr_on_interrat_status_T deact_isr_on_interrat_status
);


/*==============================================================================

  FUNCTION:  sm_deact_isr_on_interrat_status_get

==============================================================================*/
extern sm_deact_isr_on_interrat_status_T sm_deact_isr_on_interrat_status_get
(
  void
);



/*===========================================================================

FUNCTION  ESM_BC_DEACT_PENDING_UPDATE_SM

DESCRIPTION
  This function updates SM to reset corresponding PDP contexts when bearer 
  context(s) is(are) going to be deactivated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_bc_deact_pending_update_sm
(
  eps_bearer_id_T  bearer_id
);

/*===========================================================================

FUNCTION  SM_ABORT_PENDING_ESM_PROC

DESCRIPTION
  This function aborts all pending ESM procedures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  sm_abort_pending_esm_proc(void);

#ifdef FEATURE_DUAL_SIM

/*==============================================================================

  FUNCTION:  esm_validate_and_set_sm_as_id

  DESCRIPTION
  This function checks the as_id received if it is correct value and also sets sm_as_id

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if as_id received is a valid one.

SIDE EFFECTS
  None

==============================================================================*/
extern boolean esm_validate_and_set_sm_as_id
(
  esm_bcm_T             *mgr_ptr,
  esm_bpm_T             *proc_mgr,
  esm_cmd_type  *msg_ptr
);


/*==============================================================================

  FUNCTION:  esm_get_sm_as_id

    DESCRIPTION
  This function returns the as id value in SM.

DEPENDENCIES
  None

RETURN VALUE
  Returns sm_as_id.

SIDE EFFECTS
  None

==============================================================================*/
extern sys_modem_as_id_e_type esm_get_sm_as_id
(
  void
);

#endif

/*===========================================================================
FUNCTION SM_IS_MULTIMODE_SUB

DESCRIPTION
  This function tells if current sub is multimode sub.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS

===========================================================================*/
boolean sm_is_multimode_sub(void);


#endif /* FEATURE_LTE*/

/*===========================================================================
FUNCTION SM_GET_PRIMARY_INDEX 

DESCRIPTION
  This function gets the index of primary PDP
  
DEPENDENCIES
  None.
  
RETURN VALUE
  index of pri pdp

SIDE EFFECTS
   None

===========================================================================*/

byte sm_get_nsapi_from_lti(byte ti);

#ifdef FEATURE_DUAL_DATA
sys_modem_as_id_e_type sm_get_multimode_sub(void);
#endif

#endif /* #ifndef SM_MULTIMODE_COMMON_H */
