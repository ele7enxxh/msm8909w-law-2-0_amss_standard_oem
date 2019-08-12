
/*!
  @file
  ds_eps_bearer_context.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_bearer_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/12   ss      Fixed issue of RAB_REESTAB_IND received before resume ind
                   resulting in error in data path setup.
01/02/12   msh     Coan: Feature cleanup
03/10/11   ttv     Added support for LTE watermark tracing.
===========================================================================*/

#ifndef DSEPSBEARERCONTEXT_H
#define DSEPSBEARERCONTEXT_H

#ifndef UINT64_MAX
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */
#endif

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_LTE

#include "ds_3gpp_bearer_context.h"
#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef byte ds_rb_id_T;
/*-----------------------------------------------------------------------------
  This structure contains all info needed in the EPS bearer context.
-----------------------------------------------------------------------------*/
typedef struct
{  
  /*! Cookie to ensure dyn mem is valid */
  uint32               cookie;   
  eps_bearer_id_T      eps_bearer_id; /* eps bearer id */
  eps_bearer_id_T      lbi;      /* linked bearer id */
  ds_rb_id_T           rb_id;    /* RBID corresponding to this EPS bearer */
  boolean              is_in_tlb; /* Indicates if this bearer is in TLB mode */
  boolean              is_registered_pdcp;
  boolean              dl_wm_reg_with_a2; /*Indicates if DL wm reg'ed with A2*/
  ds_bearer_context_s *bearer_context_p; /* corresponding bearer context */
  int                  index;
} ds_eps_bearer_context_dyn_s;


typedef struct
{
  ds_eps_bearer_context_dyn_s *ds_eps_bearer_context_dyn_p;
} ds_eps_bearer_context_s;

/*-----------------------------------------------------------------------------
  EPS bearer context table exposed to other files through header
-----------------------------------------------------------------------------*/
extern ds_eps_bearer_context_s eps_bearer_context_tbl[DS_3GPP_MAX_BEARER_CONTEXT];

/*-----------------------------------------------------------------------------
  enum to indentify the flow direction
-----------------------------------------------------------------------------*/
typedef enum
{
  DS_EPS_FLOW_DIR_INVALID        = 0,
  DS_EPS_FLOW_DIR_DOWNLINK       = 1,
  DS_EPS_FLOW_DIR_UPLINK         = 2,
  DS_EPS_FLOW_DIR_BIDIRECTIONAL  = 3
} ds_eps_bearer_context_flow_dir_e;

#ifndef UINT32_MAX
#define UINT32_MAX (4294967295U)
#endif

/*-------------------------------------------------------------------------
Cookie to ensure that dynamic memory allocated is valid
---------------------------------------------------------------------------*/
#define EPS_BEARER_CNTXT_VALID_COOKIE  0xebeaebea

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DSEPSBEARERCNTXT_INIT

DESCRIPTION
  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_cntxt_init( void );

/*===========================================================================
FUNCTION DSEPSBEARERCNTXT_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection
  This function will decide if it has to send out a PDN Abort request or a 
  PDN Disconnect request 

PARAMETERS
  bearer_context_p  - pointer to the bearer context that is to be used
                      to end the call
  call_info_p       - call info structure
  info_ptr          - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS is all is successful, DS3G_FAILURE otherwise. 
  When this function returns failure, the bearer and the PDN contexts are
  cleaned up from the calling function. 
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_eps_bearer_cntxt_tear_down_pdn_connection
(
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_pdn_call_info_type *call_info_p,
  void                       *info_ptr
);

/*===========================================================================
FUNCTION DSEPSBEARERCNTXT_GETFLOWDIRECTION

DESCRIPTION
  This function returns the direction of the flow in a particular bearer.

PARAMETERS
  bearer_context_p - pointer to the bearer context
DEPENDENCIES
  None.

RETURN VALUE
ds_eps_bearer_context_flow_dir_e - Flow direction for this bearer
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_eps_bearer_context_flow_dir_e ds_eps_bearer_cntxt_getFlowDirection
(
  ds_bearer_context_s*  bearer_context_p
);

/*===========================================================================
FUNCTION DSEPSBEARERCNTX_PS_TO_PDCP_FLOW_ENABLE_CB

DESCRIPTION
  This function sends a flow enable command to ds task
  
PARAMETERS  
  bearer_context_p   - bearer context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_cntxt_ps_to_pdcp_flow_enable_cb
(
 dsm_watermark_type *wm,
 /*lint -esym(715,wm) */ 
 /*lint -esym(818,wm) */ 
 void*              callback_data
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_ISIM_REFERSH_VOTING_HDLR

DESCRIPTION
  This function handles isim refresh voting event for LTE mode.

PARAMETERS
  bearer_context_p - pointer to the bearer context
  subs_id          - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
boolean - Whether ISIM fresh can proceed or not
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_cntxt_isim_refresh_voting_hdlr
(
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_REDIAL_HDLR

DESCRIPTION
  This function is used to redial  PDN connectivity request.
  This function will be invoked both during Attach as well as during addnl
  PDN connection requests. 

PARAMETERS
  bearer_context_p  - pointer to the bearer context that is to be used
                      to bring up the call  
  call_info_p       - structure passed from the calling function. Used to
                      identify a call until CM Call Id gets assigned  

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS is all is successful, DS3G_FAILURE otherwise. 
  When this function returns failure, the bearer and the PDN contexts are
  cleaned up from the calling function. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_cntxt_redial_hdlr
(  
  ds_bearer_context_s           *bearer_context_p
);

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_REPORT_WM_STATS

DESCRIPTION
  This function stores the watermark status in wm_stat_ptr.

PARAMETERS
  bearer_instance - bearer context instance 
  wm_stat_ptr     - where the wm values are stored

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_bearer_cntxt_report_wm_stats
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_REQ_DEACT_FOR_PENDING_BEARERS

DESCRIPTION
  This function initiates call end for bearers that are in COMING_UP state. 
  
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if call end was initiated with CM for any pending bearers.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_eps_bearer_cntxt_req_deact_for_pending_bearers
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
STATIC FUNCTION DS_EPS_BEARER_CONTEXT_MAP_CAUSE_CODES

DESCRIPTION
  

PARAMETERS
  map_rule     - Rule for mapping cause-codes
  *output_ptr  - ptr to location holding converted cause-code
  *input_ptr   - ptr to location with cause-codes to be converted

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_eps_bearer_cntxt_map_cause_codes
(
  ds3gpp_inter_layer_map_e map_rule,
  void *output_ptr,
  const void *input_ptr
);

/*===========================================================================
FUNCTION ds_eps_bearer_cntxt_send_QOS_indication

DESCRIPTION
  This function sends the QCI information of a particular bearer to interested
  parties.
  
PARAMETERS  
  bearer_context_p - Bearer Context.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_cntxt_send_QOS_indication(ds_bearer_context_s *bearer_context_p);

/*===========================================================================
FUNCTION ds_eps_bearer_cntxt_validate_bearer_cntxt

DESCRIPTION
  This function checks if the eps bearer contextv has been allocated dynamic
  memory

PARAMETERS
  eps_bearer_context_p   -  eps bearer context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_bearer_cntxt_validate_bearer_cntxt
(
   ds_eps_bearer_context_s *eps_bearer_context_p
);

/*===========================================================================
FUNCTION ds_eps_bearer_cntxt_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
  index   - index where the memory would be allocated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
boolean ds_eps_bearer_cntxt_alloc_dyn_mem
(
  int index
);

/*===========================================================================
FUNCTION ds_eps_bearer_cntxt_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS
  bearer_cntxt_p
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/

boolean ds_eps_bearer_cntxt_free_dyn_mem
(
   ds_bearer_context_s *bearer_context_p
);

/*==========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_EXAMINE_IF_EMM_CAUSE_THROT_NEEDED

DESCRIPTION 
  This function checks whether EMM cause throttling is needed or not.
    
PARAMETERS
  LTE Call Info Ptr
  
DEPENDENCIES
  None.
                  
RETURN VALUE
  TRUE, if throttling needed
  FALSE, otherwise

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_eps_bearer_cntxt_examine_if_emm_cause_throt_needed
(
  cm_lte_call_info_s_type  *lte_call_info_ptr,
  sys_modem_as_id_e_type    subs_id
);
/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_GET_LTE_DIV_DUPLEX

DESCRIPTION
  This function returns the LTE div duplex reported by CM.

PARAMETERS
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  Last div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
sys_div_duplex_e_type ds_eps_bearer_cntxt_get_div_duplex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_SET_DIV_DUPLEX

DESCRIPTION
  This function returns the previous lte div duplex reported by CM.

PARAMETERS
  new_div_duplex: Div Duplex 
  subs_id:        Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Last lte div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_cntxt_set_div_duplex
(
  sys_div_duplex_e_type   new_div_duplex,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_IS_ANY_BEARER_NOT_DOWN

DESCRIPTION
  This function checks if any of the bearer is not in DOWN state
  
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if any bearer is not in down state
  FALSE - Otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_eps_bearer_cntxt_is_any_bearer_not_down
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_RE_REGISTER_WM_WITH_PDCP

DESCRIPTION
  This function is used to reestablish the Data path when Flow direction has got modified due to Bearer Modify indication
  from NW

PARAMETERS
  flow_dir: previous firection
  modified_flow_dir :new direction after modify
  call_id: Callid for which modify occured

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is all is successful, 
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_cntxt_re_register_wm_with_pdcp
(
  ds_eps_bearer_context_flow_dir_e		   bearer_flow_dir,
  ds_eps_bearer_context_flow_dir_e		   modified_bearer_flow_dir,
  cm_call_id_type                                  call_id
);


/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_GET_TOTAL_NEW_DL_BYTE_COUNT

DESCRIPTION
  This function is used to sum up all the bytes received for particular
  subscription accross all bearer

PARAMETERS
  requested_subs_id - CM Subscription ID for the request

DEPENDENCIES
  None.

RETURN VALUE
  Returns all total number of bytes in all UP eps bearers 
  
SIDE EFFECTS
  None.

===========================================================================*/
uint64 ds_eps_bearer_cntxt_get_total_new_dl_byte_count
(
 sys_modem_as_id_e_type  requested_subs_id
);


/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_RESET_TO_CURRENT_DL_BYTE_COUNT

DESCRIPTION
  This function is used to reset all the bytes received for particular 
  subscription accross all bearer with current pdcp values

PARAMETERS
  requested_subs_id - CM Subscription ID for the request

DEPENDENCIES
  None.

RETURN VALUE
  Returns all total number of bytes in all UP eps bearers 
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_cntxt_reset_to_current_dl_byte_count
(
 sys_modem_as_id_e_type  requested_subs_id
);


/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_RESET_DL_BYTE_COUNT

DESCRIPTION
  This function is used to clear bearer dl byte count with current pdcp values

PARAMETERS
  bearer_context_p - bearer context pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_cntxt_reset_dl_byte_count
(
 ds_bearer_context_s  *bearer_context_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_CHECK_IF_ANY_LTE_IS_BEAR_UP

DESCRIPTION
  This function is used to look if any LTE bearer is in UP state

PARAMETERS
  requested_subs_id - CM Subscription ID for the request

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any lte bearer is up , FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_cntxt_check_if_any_lte_is_bear_up
(
 sys_modem_as_id_e_type  requested_subs_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_CNTXT_GET_UE_CAT

DESCRIPTION
  This function returns the LTE UE cateogry for the pass subs
  
PARAMETERS 
  subs_id - subscription for the ue cat
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ue category number
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint8 ds_eps_bearer_cntxt_get_ue_cat
(
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_IS_SILENT_REDIAL_ALLOWED

DESCRIPTION
  This function determines whether we can allow silent redial based on pdp_type_to_use.

PARAMETERS
  pdn_cntxt_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_cntxt_is_silent_redial_allowed
(
  ds_pdn_context_s *pdn_cntxt_ptr
);

#endif /* FEATURE_DATA_LTE */
#endif /* DSEPSBEARERCONTEXT_H */

