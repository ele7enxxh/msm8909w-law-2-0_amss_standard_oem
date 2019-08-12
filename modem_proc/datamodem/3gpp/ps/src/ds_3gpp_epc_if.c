
/*!
  @file
  ds_3gpp_if.c

  @brief
  This is the module that acts as the interface between the EPC and the 3GPP
  mode handlers.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_epc_if.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/09   hs      Initial version created
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#if defined(FEATURE_EPC_HANDOFF)

#include "ds_3gpp_epc_if.h"
#include "ds_3gpp_pdn_context.h"

#include "ds_eps_pdn_context.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_IF_GET_CONFIG_INFO

DESCRIPTION
  This function returns the PDP type and the APN of the call, the Iface is 
  bound to. 

PARAMETERS
  ps_iface_type*            - Pointer to the iface
  ds3gpp_epc_if_cfg_info_s* - Pointer to the config info

DEPENDENCIES
  None.

RETURN VALUE  
  boolean - TRUE if the parameters were found on the iface
            FALSE if the iface / params are not valid

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_epc_if_get_config_info
(
  ps_iface_type* ps_iface_p, 
  ds3gpp_epc_if_cfg_info_s* config_info_p
)
{
  ds_3gpp_iface_s  *ds_iface_p = NULL;
  ds_pdn_context_s *pdn_cntx_p = NULL;
  boolean          retVal = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ps_iface_p != NULL)
  {
    /*------------------------------------------------------------------------
      Get the DS Iface from the PS Iface
    ------------------------------------------------------------------------*/
    ds_iface_p = (ds_3gpp_iface_s*)ps_iface_p->client_data_ptr;
  }

  if(ds_iface_p != NULL)
  {
    /*----------------------------------------------------------------------
      Get the PDN context from the DS Iface
    ----------------------------------------------------------------------*/
    pdn_cntx_p = (ds_pdn_context_s*)ds_iface_p->client_data_ptr;
  }

  if((pdn_cntx_p != NULL) && (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p)))
  {
    /*----------------------------------------------------------------------
      Set the return value to TRUE since we have located the PDN context
    ----------------------------------------------------------------------*/
    retVal = TRUE;

    /*----------------------------------------------------------------------
      Store the APN name and the PDN type in the structure passed into this
      function
    ----------------------------------------------------------------------*/
    memscpy(config_info_p->apn, (DS_UMTS_MAX_APN_STRING_LEN+1),
	     pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
             (DS_UMTS_MAX_APN_STRING_LEN+1));
    config_info_p->pdn_type = pdn_cntx_p->ds_pdn_context_dyn_p->
	   pdp_profile.context.pdp_type;
  }

  return retVal;
} /* ds_3gpp_if_get_config_info */

/*===========================================================================
FUNCTION DS_3GPP_IF_REG_EMM_ATTACH_HDLR

DESCRIPTION
  This function is used to register the EMM Attach handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler. Whenever an attach
  indication is received in the LTE mode, we check if the EPC mode handler
  has registered with LTE MH. If it has, the attach handler provided by EPC
  MH is invoked.

PARAMETERS
  emm_attach_hdlr_f - Pointer to the EMM attach handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_if_reg_emm_attach_hdlr
(
  ds_3gpp_if_emm_attach_hdlr_f_type emm_attach_hdlr_f
)
{
#if defined(FEATURE_DATA_LTE)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_eps_pdn_cntxt_reg_emm_attach_hdlr(emm_attach_hdlr_f);

#endif
} /* ds_3gpp_if_reg_emm_attach_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_IF_REG_ATTACH_TYPE_EX_HDLR

DESCRIPTION
  This function is used to register the attach type with the LTE
  Mode Handler. This API is called by EPC Mode Handler.
 
  MH will call this registered function during the Attach Process to find out
  whether to use Initial Attach or Handover Attach.

PARAMETERS
  attach_type_hdlr_f - Pointer to the attach type handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_if_reg_attach_type_ex_hdlr
(
  ds_3gpp_if_call_attach_type_hdlr_ex_f_type attach_type_hdlr_f
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_3gpp_pdn_cntxt_reg_attach_type_ex_hdlr(attach_type_hdlr_f);
}

/*===========================================================================
FUNCTION DS_3GPP_IF_REG_CALL_ALLOWED_EX_HDLR

DESCRIPTION
  This function is used to register the call allowed handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler. 

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_if_reg_call_allowed_ex_hdlr
(
  ds_3gpp_if_call_allowed_hdlr_ex_f_type call_allowed_hdlr_f
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_3gpp_pdn_cntxt_reg_call_allowed_ex_hdlr(call_allowed_hdlr_f);
}

#endif /* FEATURE_EPC_HANDOFF */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

