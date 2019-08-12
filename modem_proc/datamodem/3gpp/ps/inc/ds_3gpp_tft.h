
/*!
  @file
  ds_3gpp_tft.h

  @brief
  Manages Flow contexts and provides interface to PDN and bearer

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_tft.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/10   vd       Initial version.
===========================================================================*/

#ifndef DS3GPPTFT_H
#define DS3GPPTFT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_flow_context.h"
#include "ps_flow.h"
#include "ds3gmgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DS_MAX_FI_ID             16
#define DS_MAX_FI_PRECED         255
#define DS_MIN_FI_PRECED         1
#define DS_FI_PRECED_ARR_SIZE    8


/*-----------------------------------------------------------------------------
  Book keeping structure when processing App modify Qos.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32                  match_filter_id; /* Bit mask of match filter id*/
  uint32                  rx_mod_non_match_id; /* bit mask of rx mod index*/
  uint32                  tx_mod_non_match_id; /* bit mask of tx mod index*/
  uint32                  replaced_filter_id; /* bit mask of filters to replace*/
  uint8                   rx_match_cnt; /* filters matching in rx spec*/
  uint8                   tx_match_cnt; /* filters match in tx spec */
  uint8                   rx_non_match_cnt; /* filters not matching in rx spec*/
  uint8                   tx_non_match_cnt;/* filters not matching in tx spec*/
  uint8                   rx_curr_fltr_cnt; /* current rx fltr cnt */
  uint8                   tx_curr_fltr_cnt; /* current tx fltr cnt */
  ds_3gpp_nw_filter_op    network_op;       /* Network operation*/
  ds_3gpp_app_filter_op   app_op;           /* app requested operation */
} ds_3gpp_app_modify_qos_bk_s;

/*-----------------------------------------------------------------------------
  Book keeping structure when processing NW modification indication
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32   flow_cntx_fltr_id;/* Bitmask of FLTR ID matching flow             */
  uint32   bearer_fltr_id;/* Bit mask of FLTR ID matching bearer,not flow    */
  uint32   non_match_fltr_id;/* Bit mask of FLTR non matching                */
  uint32   preced_conflict[DS_FI_PRECED_ARR_SIZE];/* bit mask of precedence  */
  uint32   preced_conflict_cnt;
  qos_spec_type   curr_qos; /* current qos flow spec */
  ip_filter_type  curr_tx_fltr[MAX_PACKET_FILTERS];/* current tx fltrs*/
  ip_filter_type  curr_rx_fltr[MAX_PACKET_FILTERS];/* current rx fltrs*/
  qos_spec_type   mod_qos; /* modified qos flow spec */
  ip_filter_type  mod_tx_fltr[MAX_PACKET_FILTERS];/* modified tx fltrs*/
  ip_filter_type  mod_rx_fltr[MAX_PACKET_FILTERS];/* modified rx fltrs*/
  void     *conflict_bearer[7];/*pointers to bearers with 
                                                          conflicting preced */
  uint32   conflict_bearer_cnt; /* counter to keep track of above array      */
  boolean  new_flow;            /* track if new flow needs to be created.    */ 
  boolean  is_add;          /* track if this is replace or add           */
  boolean  prec_conf_on_same_bearer_create;
  boolean  prec_conf_on_same_bearer_modify;
} ds_3gpp_nw_modify_qos_bk_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_3GPP_TFT_SET_FILTER_PRECED_BIT

DESCRIPTION
  Set the filter precedence bit in the given precedence bit mask.

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_set_filter_preced_bit
(
  uint16     fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_CLEAR_FILTER_PRECED_BIT

DESCRIPTION
  Clear the filter precedence bit in the given precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_clear_filter_preced_bit
(
  uint16     fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION ds_3gpp_tft_is_filter_preced_bit_set

DESCRIPTION
  Return whether the filter precedence is in use for a give precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_is_filter_preced_bit_set
(
  uint16     fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_IS_FILTER_IDENTIFIER_REQUESTED

DESCRIPTION
  Return whether this filter identifier is already in use.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter identifier is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_is_filter_identifier_requested
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_SET_FILTER_IDENTIFIER_BIT

DESCRIPTION
  Set the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_set_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
);

/*===========================================================================
FUNCTION DS_3GPP_CLEAR_FILTER_IDENTIFIER_BIT

DESCRIPTION
  unset the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_clear_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
);

#if 0
/*===========================================================================
FUNCTION DS_3GPP_TFT_IS_FILTER_PRECED_REQUESTED

DESCRIPTION
  Return whether this filter precedence is used for this flow manager context

PARAMETERS
  fi_preced     - Filter precedence.
  flow_mgr_ptr  - Pointer to the flow manager context.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_is_filter_preced_requested
(
  uint16             fi_preced,
  ds_flow_manager_s  *flow_mgr_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_FIND_AND_SET_FILTER_PRECED

DESCRIPTION
  Returns the first available preced value for flow manager. 

PARAMETERS
  flow_mgr_ptr     - Pointer to the flow manager context.
  lower_limit      - Assigned precedence should be >=  lower limit.
  upper_limit      - Assigned precedence should be <=  upper limit.
DEPENDENCIES
  None.

RETURN VALUE
  int           - Returns -1 if the precedence cannot be assigned.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_tft_find_and_set_filter_preced
(
  ds_flow_manager_s     *flow_mgr_ptr,
  uint16                lower_limit,
  uint16                upper_limit
);

#endif

/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_ADD_REPALCE_FLTR_SEMANTIC_ERROR

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_mod_fltr_semantic_error
(
  tft_type_T         *nw_tft  
);
/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_FOR_REPEATED_PRECEDENCE

DESCRIPTION
  Verify if the input TFT has precedence repeated.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_for_repeated_precedence
(
  tft_type_T         *nw_tft  
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_CONFIGURE_NW_BK_FLOW_SPEC

DESCRIPTION
  Configure the flow spec and filters in the Network book keeping 
  data strutcure.

PARAMETERS
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_configure_nw_bk_flow_spec
(
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_NW_FLTR_ID_PRECED

DESCRIPTION
  Verify the filter ID and precedence for each filter passed by the network.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information
  bearer_context_p            -  Bearer information
  flow_context_p              -  Flow context
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.
  del_filter                  -  Indicate delete filter operation
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_nw_fltr_id_preced
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p,
  tft_type_T                      *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk,
  boolean                          del_filter
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_FIND_BEARERER_WITH_FLTR_PRECED_CONFLICT

DESCRIPTION
  Find the bearers on which the precedence conflict occurs.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information
  bearer_context_p            -  Bearer information
  flow_context_p              -  Flow context
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_find_bearer_with_preced_conflict
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p,
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_PRECED_CONFLICT_ON_BEARER

DESCRIPTION
  Verifies if the input bearer is in the conflict bearer array.

PARAMETERS
  nw_bk                       -  Boook keeping info.
  input                       -  bearer_ptr
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_preced_conflict_on_bearer
(
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk,
  void                            *input
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_VERIFY_NW_TFT

DESCRIPTION
  Verifies if the NW response addresses the UE request.

PARAMETERS
  ue_tft                      -  UE requested TFT.
  nw_tft                      -  NW responsed TFT.
  flow_context_p              -  Flow context on which to perform operation.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_verify_nw_tft
(
  tft_type_T                    *ue_tft,
  tft_type_T                    *nw_tft,
  ds_flow_context_s             *flow_context_p
);

/*===========================================================================
FUNCTION DS_3GPP_TFT_UPDATE_NW_DELETE_TFT

DESCRIPTION
  Update the NW TFT request to delete filters from a given flow.

PARAMETERS
  nw_tft                      -  NW responsed TFT.
  flow_context_p              -  Flow context on which to perform operation.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_update_nw_delete_tft
(
  tft_type_T                    *nw_tft,
  ds_flow_context_s             *flow_context_p
);

#endif /* FEATURE_DATA_3GPP */
#endif /* DS3GPPTFT_H */

