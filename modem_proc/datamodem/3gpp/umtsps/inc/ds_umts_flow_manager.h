
/*!
  @file
  ds_umts_flow_manager.h

  @brief
  Manages Flow contexts and provides interface to PDN and bearer

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/inc/ds_umts_flow_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/10   vrk     Initial version.
===========================================================================*/

#ifndef DSUMTSFLOWMANAGER_H
#define DSUMTSFLOWMANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)

#include "ds_3gpp_flow_manager.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_INIT

DESCRIPTION
  Register UMTS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_umts_flow_manager_init(void);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CHECK_NW_INPUT

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
boolean ds_umts_flow_manager_check_nw_input
(
  cm_call_mode_info_s_type* mode_info_ptr,
  boolean                   default_bearer,
  boolean                   bearer_alloc
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CONVERT_CAUSE_CODE

DESCRIPTION
  Convert the generic cause codes to UMTS specific cause code.

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
boolean ds_umts_flow_manager_convert_cause_code
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type *ds_int_cause_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_REJ

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
void ds_umts_flow_manager_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_FAIL

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
void ds_umt_flow_manager_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_CNF

DESCRIPTION
  Handle the bearer modification Indication.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_flow_manager_bearer_modify_cnf
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_REJ

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
boolean ds_umts_flow_manager_bearer_modify_rej
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_IND

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
boolean ds_umts_flow_manager_bearer_modify_ind
(  
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOCATION_IND

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
boolean ds_umts_flow_manager_bearer_allocation_ind
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr
);


/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_REQ

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
int ds_umts_flow_manager_bearer_modify_req
(
  ds_flow_context_s             *flow_context_p,
  tft_type_T                    *nw_tft_p,
  qos_spec_type                 *app_qos,
  int                           flow_state
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_REQ

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
int ds_umts_flow_manager_bearer_alloc_req
(
  ds_pdn_context_s    *pdn_context_p,
  ds_flow_context_s   *flow_context_p,
  qos_spec_type       *app_qos,
  tft_type_T          *nw_tft_p
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_ABORT_REQ

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
int ds_umts_flow_manager_bearer_alloc_abort_req
(
  ds_flow_context_s             *flow_context_p
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

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
boolean ds_umts_flow_manager_nw_bearer_modify_ind
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
);

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_INCOMING_CALL

DESCRIPTION
  Handle the incoming secondary PDP context.

PARAMETERS
  pdn_context_p    -  Pointer to PDN context.
  flow_context_p   -  Pointer to Flow context.
  mode_info_ptr    -  Mode specific info ptr.


DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_flow_manager_incoming_call
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr,
  cm_end_params_s_type*     end_params,  
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_UMTS_BEARER_FLOW_MANAGER_STORE_NW_DATA_RATE
DESCRIPTION
  Converts the APP passed data rate to UMTS data rate and store in bearer
  flow manager
  orig_call_hdlr converts the UMTS values to NW format.
  In UMTS, except data rates and guaranteed data rates, other parameters
  are common across UL and DL. 

PARAMETERS
nw_qos          -  NW qos to be converted
bflow_manager_p -  UMTS bearer flow manager 

DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.
===========================================================================
boolean ds_umts_bearer_flow_manager_store_nw_data_rate
(
   const qos_T     *nw_qos_ptr,  
   void            *bflow_manager_p
);
*/
#endif /* if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
#endif /* DSUMTSFLOWMANAGER_H */

