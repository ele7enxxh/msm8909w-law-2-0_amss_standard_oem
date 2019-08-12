
/*!
  @file
  ds_eps_flow_manager.h

  @brief
  Manages Flow contexts and provides interface to PDN and bearer

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_flow_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/10   vd      Changed function naming format.
01/20/10   vd      Changes to support UE Qos modification.
07/17/09   vd      Initial version.
===========================================================================*/

#ifndef DSEPSFLOWMANAGER_H
#define DSEPSFLOWMANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_LTE

#include "ds_3gpp_flow_manager.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define EPS_FLOW_MGR_VALID_COOKIE 0xef9ef9ef

/*-----------------------------------------------------------------------------
  This structure contains EPS specific info for the Flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32 			   cookie;
  apn_ambr_T           eps_pdn_ambr; /* PDN AMBR*/ 
  int 				   index; 
} ds_eps_flow_manager_dyn_s;

typedef struct
{
  ds_eps_flow_manager_dyn_s *eps_flow_manager_dyn_p;
}ds_eps_flow_manager_s;

/*-----------------------------------------------------------------------------
  EPS Flow manager table exposed to other files through header
-----------------------------------------------------------------------------*/
extern ds_eps_flow_manager_s ds_eps_flow_manager_tbl[DS_3GPP_MAX_PDN_CONTEXT];


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_INIT

DESCRIPTION
  Register EPS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_init(void);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CHECK_NW_INPUT

DESCRIPTION
  Handle the bearer allocation failure.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the NW params are valid.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_check_nw_input
(
  cm_call_mode_info_s_type* mode_info_ptr,
  boolean                   default_bearer,
  boolean                   bearer_alloc
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CONVERT_CAUSE_CODE

DESCRIPTION
  Convert the generic cause codes to EPS specific cause code.

PARAMETERS
  mode_info_ptr      -  CM passed mode information.
  ds_int_cause_ptr   -  DS generic cause  

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the cause code conversion is successful.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_convert_cause_code
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type *ds_int_cause_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_UPDATE_APN_AMBR

DESCRIPTION
  Update the APN AMBR with the network request.

PARAMETERS
  lte_info_ptr    -  LTE information.
  pdn_context_p   -  PDN context pointer

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if successfully updated.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_update_apn_ambr
(
  cm_lte_call_info_s_type  *lte_info_ptr,
  ds_pdn_context_s         *pdn_context_p
);


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_TEAR_DOWN_SEC_BEARER

DESCRIPTION
  Handle secondary bearer tear down

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_tear_down_sec_bearer
(
  ds_flow_context_s   *flow_context_p
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_REJ

DESCRIPTION
  Handle the bearer allocation rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_FAIL

DESCRIPTION
  Handle the bearer allocation failure.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_REJ

DESCRIPTION
  Handle the bearer modificaiion rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_modify_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_IND

DESCRIPTION
  Handle the bearer modification Indication.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOCATION_IND

DESCRIPTION
  Handle the bearer allocation Indication.

PARAMETERS
  pdn_context_p         -  Ptr to the PDN context.
  bearer_context_p      -  Ptr to the bearer context.
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_bearer_allocation_ind
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  Handle the bearer modificaiion rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_check_sdf_id
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_REQ

DESCRIPTION
  Handle the bearer modificaiion request.

PARAMETERS
  flow_context_p   -  Pointer to Flow context to be modified.
  nw_tft_p         -  Pointer to NW TFT
  app_qos          -  Pointer to App Qos.
  flow_state        -  flow state when the command was posted

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_modify_req
(
  ds_flow_context_s             *flow_context_p,
  tft_type_T                    *nw_tft_p,
  qos_spec_type                 *app_qos,
  int                           flow_state
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_REQ

DESCRIPTION
  Handle the bearer allocation request.

PARAMETERS
  pdn_context_p    -  Pointer to PDN context to be modified.
  flow_context_p   -  Pointer to Flow context to be modified.
  nw_tft_p         -  Pointer to NW TFT
  app_qos          -  Pointer to App Qos.

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_alloc_req
(
  ds_pdn_context_s    *pdn_context_p,
  ds_flow_context_s   *flow_context_p,
  qos_spec_type       *app_qos,
  tft_type_T          *nw_tft_p
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_ABORT_REQ

DESCRIPTION
  Send out the bearer allocation abort request.

PARAMETERS
  flow_context_p   -  Pointer to Flow context to be aborted.

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_alloc_abort_req
(
  ds_flow_context_s             *flow_context_p
);

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

DESCRIPTION
  Handle the NW requested bearer modificaiion Indication.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCEESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_nw_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
);

/*===========================================================================
FUNCTION ds_eps_flow_manager_alloc_eps_flow_manager

DESCRIPTION
  This function allocates dynamic memory for the index in the eps flow manager
  table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_alloc_eps_flow_manager
(
   int alloc_index
);

/*===========================================================================
FUNCTION ds_eps_flow_manager_validate_eps_flow_manager

DESCRIPTION
  This function validates dynamic memory for the index in the eps 
  flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_validate_flow_manager
(
   ds_eps_flow_manager_s *eps_flow_manager_p
);

/*===========================================================================
FUNCTION ds_eps_flow_manager_free_eps_flow_manager

DESCRIPTION
  This function frees dynamic memory for the index in the
  eps flow manager table

PARAMETERS
int index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_free_eps_flow_manager
(
   int index
);


#endif /* FEATURE_DATA_LTE */
#endif /* DSEPSFLOWMANAGER_H */

