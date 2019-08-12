/*!
  @file
  ds_3gpp_dyn_mem_hdlr.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_dyn_mem_hdlr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/12   vb      Initialize IRAT variables, function pointers. This file
    			   ensures bearer context avoids including umts/eps bearer context
 
===========================================================================*/
#ifndef DS_3GPP_IRAT_HDLR_H
#define DS_3GPP_IRAT_HDLR_H
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "dstask_v.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_umts_bearer_context.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_umts_pdn_context.h"
#include "cm.h"
#include "ps_iface.h"
#include "ds_3gppi_utils.h"
#include "sys.h"
#include "ps_iface_defs.h"
#include "cm_gw.h"
#include "ds_3gpp_apn_table.h"

#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#endif /*FEATURE_DATA_LTE*/

#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

#include "pstimer.h"


/*===========================================================================
FUNCTION DS_3GPP_DYN_MEM_HDLR_INIT

DESCRIPTION
 This function registers the necessary function pointers.
 It is invoked during DS Task power-up.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_init( void );

/*===========================================================================
FUNCTION DS_3GPP_DYN_MEM_HDLR_ALLOC_DYN_MEM

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
 bearer context and eps bearer context dynamically.
 
PARAMETERS
  Index.
  Subs_Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_alloc_bearers_dyn_mem
(
   int                    index,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_DYN_MEM_HDLR_FREE_DYN_MEM

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
 bearer context and eps bearer context dynamically.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
  Subs_Id             - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_bearers_dyn_mem
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_umts_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
 bearer flow manager dynamically.
 
PARAMETERS
  int                 - Index
  Subs_Id             - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_umts_bearer_flow_manager
(
   int                    index,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_dyn_mem_umts_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
 bearer flow manager dynamically.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
  Subs_Id             - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_flow_mgr_dyn_mem

DESCRIPTION
This function frees umts/eps bearer context, bearer context
apn tbl and pdn context in that order.
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
 
  3gpp bearer context memory is not freed here

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_flow_mgr_dyn_mem
(
   ds_pdn_context_s *pdn_context_p
);

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem

DESCRIPTION
This function frees umts/eps bearer flow managers, bearer flow manager,
bearers and then if the bearer is default also frees apn and pdn
 
PARAMETERS
  ds_bearer_context_p - Bearer context pointer
 
  3gpp bearer context memory is not freed here

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem
(
	 ds_bearer_context_s *bearer_context_p
);


/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_eps_bearer_flow_manager

DESCRIPTION
 This function frees eps bearer flow maanger
 
PARAMETERS
  index
  Subs_Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_eps_bearer_flow_manager
(
   int                    index,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_eps_bearer_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate eps
 bearer flow manager dynamically.
 
PARAMETERS
  int       Index
  Subs_Id   Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_eps_bearer_flow_manager
(
   int                    alloc_index,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate umts
pdn context dynamically.
 
PARAMETERS
  int       Index
  Subs_Id   Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context

DESCRIPTION
 This function dispatches the appropriate function pointers to free umts
pdn context dynamically
 
PARAMETERS
  int       Index
  Subs_Id   Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_dyn_mem_hdlr_alloc_dyn_mem_eps_flow_manager

DESCRIPTION
 This function dispatches the appropriate function pointers to allocate eps
 flow manager dynamically.
 
PARAMETERS
  int       Index
  Subs_Id   Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_dyn_mem_hdlr_allocate_eps_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_dyn_mem_hdlr_free_eps_flow_manager

DESCRIPTION
 This function frees eps flow maanger
 
PARAMETERS
  int       Index
  Subs_Id   Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_dyn_mem_hdlr_free_eps_flow_manager
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);
#endif /* FEATURE_DATA_3GPP */
#endif /* DS_3GPP_IRAT_HDLR_H */ 
