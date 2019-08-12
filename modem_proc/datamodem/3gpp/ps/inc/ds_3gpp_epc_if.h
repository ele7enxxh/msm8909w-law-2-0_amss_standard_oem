/*!
  @file
  ds_3gpp_if.h

  @brief
  REQUIRED brief Interface between 3GPP and EPC mode handler.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_epc_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/09   hs      Initial version created
===========================================================================*/

#ifndef DS_3GPP_EPC_IF_H
#define DS_3GPP_EPC_IF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#if defined(FEATURE_EPC_HANDOFF)

#include "dsumtspdpreg.h"
#include "ps_iface.h"
#include "ps_sys.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


typedef boolean (*ds_3gpp_if_emm_attach_hdlr_f_type)
(
  boolean op, 
  uint16  prof_id,
  ps_iface_type *v4_iface,
  ps_iface_type *v6_iface
);

typedef boolean (*ds_3gpp_if_call_allowed_hdlr_f_type)
(
  uint16  prof_id
);

typedef boolean (*ds_3gpp_if_call_attach_type_hdlr_f_type)
(
  uint16  prof_id
);

typedef boolean (*ds_3gpp_if_call_allowed_hdlr_ex_f_type)
(
  uint16                         prof_id,
  ps_sys_subscription_enum_type  subs_id
);

typedef boolean (*ds_3gpp_if_call_attach_type_hdlr_ex_f_type)
(
  uint16                         prof_id,
  ps_sys_subscription_enum_type  subs_id
);

typedef struct
{
  char                       apn[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_umts_pdp_type_enum_type pdn_type;
} ds3gpp_epc_if_cfg_info_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

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
  ps_iface_type*, 
  ds3gpp_epc_if_cfg_info_s*
);

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
);

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
);

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
);

#endif /* FEATURE_EPC_HANDOFF */

#endif /* DS_3GPP_EPC_IF_H */

