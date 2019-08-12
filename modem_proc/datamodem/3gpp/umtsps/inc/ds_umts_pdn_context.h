#ifndef DSUMTSPDNCONTEXT_H
#define DSUMTSPDNCONTEXT_H
/*===========================================================================

            E P S   S P E C I F I C   M O D E   H A N D L E R

===========================================================================*/
/*!
  @file
  ds_pdn_context.h

  @brief
  This header file defines the externalized function that interface DSEPS 
  and ESM.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/inc/ds_umts_pdn_context.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/07/13   nd      Added support for filling PCO of MT calls(primary/secondary)
                   from incoming call request and not from profile(See 23.060).
03/24/11   sa      Added MT PDP Auto answer support.
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)

#include "ds_3gpp_pdn_context.h"
#include "ps_iface_defs.h"
#include "ds3gmgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief UMTS PDN context table */
extern ds_umts_pdn_context_s umts_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT];

/*===========================================================================

 
                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


#define DS_UMTS_PDP_TYPE_IPV4    0x21
#define DS_UMTS_PDP_TYPE_IPV6    0x57
#define DS_UMTS_PDP_TYPE_IPV4V6  0x8D
 
/*===========================================================================
FUNCTION DS_PDN_CONTEXT_UMTS_INIT

DESCRIPTION
  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsPdnCntx_umtsInit( void );

/*===========================================================================
FUNCTION dsUmtsPdnCntx_alloc_dyn_mem

DESCRIPTION
  This function allocates the  dyn memory associated with the index in umts_pdn_context_tbl
 
PARAMETERS 
  int loop_index
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean 
 
SIDE EFFECTS
  None

=============================================================================*/

boolean dsUmtsPdnCntx_alloc_dyn_mem
(
  int loop_index
);
/*===========================================================================
FUNCTION dsUmtsPdnCntx_free_dyn_mem

DESCRIPTION
  This function frees the  dyn memory associated with the index in umts_pdn_context_tbl
 
PARAMETERS 
  int loop_index
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean 
 
SIDE EFFECTS
  None

=============================================================================*/


boolean dsUmtsPdnCntx_free_dyn_mem
(   
   int loop_index
);

/*===========================================================================
FUNCTION dsUmtsPdnCntx_validate_pdn_context

DESCRIPTION
  This function checks if the umts_pdn_cntxt_p has been allocated dynamic memory

PARAMETERS
  umts_pdn_cntxt_p      - umts pdn context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/


boolean dsUmtsPdnCntx_validate_pdn_context
(
   ds_umts_pdn_context_s *umts_pdn_cntxt_p
);

/*===========================================================================
FUNCTION dsUmtsPdnCntx_incomingCall

DESCRIPTION
  This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.

DEPENDENCIES
   None

RETURN VALUE
   ds3g_incom_e_type value

SIDE EFFECTS
   None

===========================================================================*/
ds3g_incom_e_type dsUmtsPdnCntx_incomingCall
( 
  cm_call_id_type             call_id,
  const ds_cmd_type          *cmd_ptr,
  cm_ans_params_s_type       *ans_params,
  cm_end_params_s_type       *end_params,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION dsUmtsPdnCntx_getAnswerParams

DESCRIPTION
  This function provides call origination paramters.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration.

SIDE EFFECTS  

===========================================================================*/
boolean dsUmtsPdnCntx_getAnswerParams
(
  ds_pdn_context_s              *pdn_cntx_p,
  cm_ans_params_s_type          *answer_params,
  ds_3gpp_pdn_call_info_type    *call_info
);

/*===========================================================================
  FUNCTION DSUMTSPS_CONFIGURE_PDP_PPP

  DESCRIPTION
    Gets PPP defaults and configures PPP for a PDP type PPP call.
    Based on the current mode calls function to configure PPP in that mode.

  PARAMETERS
    iface_ptr - Interface ptr for the call

  DEPENDENCIES
    None.

  RETURN VALUE
    0 - if ppp_start succeeds.

  SIDE EFFECTS
    None.

===========================================================================*/
int dsUmtsPdnCntx_configurePdpPpp
(
  ps_iface_type     *iface_p,
  ppp_dev_enum_type  ppp_dev
);

/*===========================================================================
FUNCTION dsUmtsPdnCntx_bQOSIsSupported

DESCRIPTION
   Accessor function that returns information about whether QOS is supported on
   UMTS. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
  TRUE is QOS is supported.
  FALSE otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsPdnCntx_bQOSIsSupported(void);

/*===========================================================================

FUNCTION ds_umts_pdn_cntxt_hndl_unkown_cause

DESCRIPTION
   This function is used to handle cases for CAUSE CODE set to UNKNOWN

PARAMETERS
  Pdp_type_ctx_req: IP type requested to N/W
  pdp_type_num : IP type returned by N/W
  Pdn_cntxt_p  : PDN context pointer
  ip_support   : Updated IP type

DEPENDENCIES

  None.

RETURN VALUE 

  TRUE  DS3G_SUCCESS.
  FALSE   DS3G_FAILURE. 

SIDE EFFECTS

  None.

===========================================================================*/

boolean ds_umts_pdn_cntxt_hndl_unknown_cause
( 
  ds_umts_pdp_type_enum_type    pdp_type_ctx_req,
  byte                          pdp_type_num,
  ds_pdn_context_s              *pdn_cntx_p,
  ds_apn_ip_support_type_e      *ip_support
);


#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
#endif    /* DSUMTSPDNCONTEXT_H */

