
/*!
  @file
  ds_3gpp_flow_context.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_flow_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/10   sa      Added support for dynamic cid for flow context.
01/20/10   vd      Changes to support UE Qos modification.
07/17/09   vd      Initial version.
===========================================================================*/

#ifndef DS3GPPFLOWCONTEXT_H
#define DS3GPPFLOWCONTEXT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_rmsm_atif.h"
#include "cm_gw.h"
#include "ps_flow.h"
#include "ds3gmgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define         DS_3GPP_NW_FLOW_TYPE                  0x1
#define         DS_3GPP_V4_FLOW_TYPE                  0x2
#define         DS_3GPP_V6_FLOW_TYPE                  0x4

typedef enum
{
  DS_3GPP_NW_FLTR_INVALID_OP              = 0x0,
  DS_3GPP_NW_FLTR_ADD_OP                  = 0x1,
  DS_3GPP_NW_FLTR_DEL_OP                  = 0x2,
  DS_3GPP_NW_FLTR_NO_OP                   = 0x4,
  DS_3GPP_NW_FLTR_REPLACE_OP              = 0x8,
  DS_3GPP_NW_DATA_RATE_OP                 = 0x10,
  DS_3GPP_NW_FLTR_UNSUPP_OP               = 0x20
}ds_3gpp_nw_filter_op;

typedef enum
{
  DS_3GPP_APP_FLTR_INVALID_OP             = 0x0,
  DS_3GPP_APP_RX_FLTR_OP                  = 0x1,
  DS_3GPP_APP_TX_FLTR_OP                  = 0x2,
  DS_3GPP_APP_RX_DATA_RATE_OP             = 0x4,
  DS_3GPP_APP_TX_DATA_RATE_OP             = 0x8,
  DS_3GPP_APP_FLTR_UNSUPP_OP              = 0x10
}ds_3gpp_app_filter_op;


typedef enum
{
  DS_FLOW_CONTEXT_STATE_DOWN                  = 0x0,
  DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP        = 0x1,
  DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP  = 0x2,
  DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP       = 0x4,
  DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP          = DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP,
  DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP          = 0x8,
  DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP     = 0x10,
  DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP      = 0x20,
  DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP        = 0x40,
  DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP = 0x80,
  DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP_DEL = 0x100, 
  DS_FLOW_CONTEXT_STATE_COMING_UP             = 0x200,
  DS_FLOW_CONTEXT_STATE_UP                    = 0x400,
  DS_FLOW_CONTEXT_STATE_PENDING_LL_CONFIRM    = 0x800,
  DS_FLOW_CONTEXT_STATE_MAX                   = 0x1000
} ds_flow_context_state_e;

/*-----------------------------------------------------------------------------
  This structure contains all info needed in the Flow Context.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32				           cookie; 		/* Cookie to ensure dynamic allocation is successful*/
  ps_flow_type            *ps_flow_ptr;/* PS flow pointer */
  ds_3gpp_iface_s         *ds_iface_ptr;/* 3GPP iface associated with the flow*/
  tft_type_T              tft_mod;     /* TFT Modify request sent to NW.*/
  void                    *bearer_flow_mgr_ptr;/* Bearer Flow manager Pointer */
  uint32                  cid;  /* static or dynamic profile no */
  uint32                  fi_identifier; /*bitmask of filter identifiers*/  
  uint32                  ignore_fi_id;  /* bitmask of filter id to ignore.*/ 
  ds_flow_context_state_e state;     /* Flow Context State */
  boolean                 granted_flow_set; /* Has the granted flow set*/
  uint8                   flow_type; /* Indicates if it is a NW Flow,v4,v6 */
               /*bits 2-4 identify v4 or V6 flow 4, 6)  bit 1 is for NW FLOW */
  int                     index;

} ds_flow_context_dyn_s;

typedef struct
{
  list_link_type          link; /* Ptr to next node */
  ds_flow_context_dyn_s *ds_flow_context_dyn_p;
  word 					 sdf_id;
} ds_flow_context_s;

typedef struct {
  ps_flow_type      *ps_flow_ptr;
  uint32             p_cid;  /* associated profile number */
  byte               bearer_id;
  uint8              flow_type;
  qos_spec_type      qos_spec;
}ds_flow_dynamic_info_s;

/*-----------------------------------------------------------------------------
  Flow Context table exposed to other files through header
-----------------------------------------------------------------------------*/
extern ds_flow_context_s ds_flow_context_tbl[DS_3GPP_MAX_FLOW_CONTEXT];

#define FLOW_CONTEXT_VALID_COOKIE  0xaa7e7e55

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ALLOCATE_FLOW_CONTEXT

DESCRIPTION
  Allocate a new flow context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Pointer to empty flow context.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_context_allocate_flow_context(void);


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_RELEASE_FLOW_CONTEXT

DESCRIPTION
  Release the flow context.

PARAMETERS
  flow_context_p  - Pointer to the flow context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_release_flow_context
(
  ds_flow_context_s *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_ACTIVATE_CMD

DESCRIPTION
  Invoke the activate command on the PS Flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  int              - Indicates sucess/ failure
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_context_ps_flow_activate_cmd
( 
  ds_flow_context_s  *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_CONFIGURE_FLOW_CONTEXT

DESCRIPTION
  Configure the IP type and state for the flow context.

PARAMETERS
  flow_context_p      - Flow Context information.
  pdn_context_p       - PDN context information.
  bearer_context_p    - Bearer context information.
  flow_type           - flow flag information.
  ip_type             - V4 or V6 ip type.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_configure_flow_context
(
  ds_flow_context_s     *flow_context_p,
  ds_pdn_context_s      *pdn_context_p,
  ds_bearer_context_s   *bearer_context_p,
  uint8                 flow_type,
  uint8                 ip_type
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GO_NULL_CMD

DESCRIPTION
  Invoke the go null command on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
   int              - Indicates sucess/ failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_context_ps_flow_go_null_cmd
( 
  ds_flow_context_s  *flow_context_p
);


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GO_NULL_IND

DESCRIPTION
  Post the go null indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  ps_eic           - Error code indicating reason for reject.
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_go_null_ind
( 
  ds_flow_context_s               *flow_context_p,
  ps_extended_info_code_enum_type ps_eic
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_ACTIVATE_IND

DESCRIPTION
  Post the activate indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  ps_eic           - Error code indicating reason for reject.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_activate_ind
( 
  ds_flow_context_s               *flow_context_p,
  ps_extended_info_code_enum_type ps_eic
);


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_SUSPEND_IND

DESCRIPTION
  Post the suspend indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_suspend_ind
( 
  ds_flow_context_s *flow_context_p
);


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_FILTERING_CONTROL

DESCRIPTION
  Disable or Enbale flow filtering control based on the input flag.

PARAMETERS
  flow_context_p   - Flow Context information.
  enable          - TRUE or FALSE

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE if the operation is successful.
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_filtering_control
( 
  ds_flow_context_s *flow_context_p,
  boolean           enable
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_MODIFY_RESULT

DESCRIPTION
  Notify if the PS flow modify has been accepted or rejected.

PARAMETERS
  flow_context_p    - Flow Context information.
  result            - TRUE or FALSE
  ps_eic            - Error code indicating reason for reject.

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE if successful.
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_modify_result
( 
  ds_flow_context_s      *flow_context_p,
  boolean                 result,
  ps_extended_info_code_enum_type ps_eic
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_BIND_PHYS_LINK

DESCRIPTION
 Bind the PS flow associated with the flow context with the phys_link

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_bind_phys_link
( 
  ds_flow_context_s     *flow_context_p
);


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_SET_GRANTED_FLOW

DESCRIPTION
 Set the granted flow.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_set_granted_flow
( 
  ds_flow_context_s    *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GET_FILTERS

DESCRIPTION
  Retrieve the current filters assigned to the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  modify           - boolean indicates current copy or modify set.
  app_fltr_spec    - Application filter spec to be populated.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_get_filters
( 
 ds_flow_context_s    *flow_context_p,
 boolean               modify,
 qos_spec_type        *app_fltr_spec
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Removes the filter from the passed ps qos spec.

PARAMETERS
  flow_context_p    - Flow Context information.
  filter_id         - Filter to be removed.
  app_qos_spec      - filter spec from which the filter has to be deleted.
  modified_qos_spec  - filter spec which has the modified filter changes.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_delete_filter_from_flow
( 
  ds_flow_context_s    *flow_context_p,
  uint32               filter_id,
  qos_spec_type        *app_qos_spec,
  qos_spec_type        *modified_qos_spec
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_FIND_FLOW_CONTEXT

DESCRIPTION
 Find the flow context associated with the PS FLOW

PARAMETERS
  ps_flow_p     -  PS Flow information.

DEPENDENCIES
  None.

RETURN VALUE
  Flow context Ptr

SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_context_find_flow_context
( 
  ps_flow_type     *ps_flow_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update requested filter identifier bit mask.

PARAMETERS
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_update_filter_identifier_bit_mask
( 
  ds_flow_context_s    *flow_context_p,
  boolean              set_identifier
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_COMPARE_FILTERS

DESCRIPTION
  Compares the two input filters

PARAMETERS
  filter1             -  Filter1 to be compared.
  filter2             -  Filter2 to be compared.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_context_compare_filters
(
  ip_filter_type            *filter1,
  ip_filter_type            *filter2
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_FILTER_IDENTIFIER

DESCRIPTION
  Compare the App filter spec to NW returned filter spec.
  This will also update the filter identifier and filter precedence.

PARAMETERS
 app_filter_spec       -  App requested filter spec.
 nw_filter_spec        -  NW granted filter speco.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_context_update_filter_identifier(
  ip_filter_spec_type            *app_filter_spec,
  ip_filter_spec_type            *nw_filter_spec
);

/*===========================================================================
FUNCTION ds_flow_context_retrieve_iface_ip_type

DESCRIPTION
  Retrieve the PS iface and PS IP Type from the flow context.

PARAMETERS
 flow_context_p        - Pointer to flow context.
 ip_type               - Pointer to IP type. Filled in by function.
 iface_ptr             - Pointer to the Ps iface. filled in by function.

DEPENDENCIES
  None.

RETURN VALUE
 iface_p               - Pointer to PS iface.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_context_retrieve_iface_ip_type
(
  ds_flow_context_s      *flow_context_p,
  ip_version_enum_type   *ip_type,
  ps_iface_type          **iface_ptr
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_SET_FLOW_STATE

DESCRIPTION
 Set the flow state.

PARAMETERS
  flow_context_p   - Flow Context information.
  new_state            - New Flow State.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_set_flow_state
( 
  ds_flow_context_s        *flow_context_p,
  ds_flow_context_state_e  new_state
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ALLOC_DYNAMIC_CID

DESCRIPTION
  This function is invoked to allocate a dynamic profile number for a 
  flow request.

PARAMETERS
  prof_num_p    -  ptr to dynamic profile number

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns TRUE if success, FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_alloc_dynamic_cid
(
  uint32 * cid_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_DYNAMIC_CID

DESCRIPTION
  This function is invoked to update a dynamic profile number for a 
  flow request.

PARAMETERS
  flow_p      - flow pointer whose dynamic profile needs to be updated.
  cid    -  cid number associated with the flow p

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns TRUE if success, FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_update_dynamic_cid
(
  ps_flow_type *            flow_p,
  uint32                     cid
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ALL_CIDS

DESCRIPTION
  This function returns the list of all flow cid's that are 
  currently active.
  
PARAMETERS   
  cid_list_ptr  - list of cid's which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_flow_context_get_all_cids
(
  ds_3gpp_atif_flow_cid_list_type*   cid_list_ptr
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ATI_DYNAMIC_INFO

DESCRIPTION
  This function returns the state of the flow 
  a. flow pointer.
  b. associated primary cid
  c. bearer id (to do send nsapi )
  d. flow type
  e. qos
PARAMETERS   
  cid  - secondary cid number whose state info needs to retrieved.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- if valid flow state is found
  FALSE -- if no valid flow is present.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_get_ati_dynamic_info
(
  uint32                  cid,
  sys_modem_as_id_e_type  subs_id,
  ds_flow_dynamic_info_s*   flow_state_info_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ATI_DYNAMIC_INFO_FOR_DDS

DESCRIPTION
  This function returns the state of the flow 
  a. flow pointer.
  b. associated primary profile
  c. bearer id (to do send nsapi )
  
PARAMETERS   
  profile_num  - secondary profile number whose state info needs to retrieved.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- if valid flow state is found
  FALSE -- if no valid flow is present.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_get_ati_dynamic_info_for_dds
(
  uint32                        cid,
  ds_flow_dynamic_info_s*   flow_ati_info_p
);

/*===========================================================================
FUNCTION ds_flow_context_validate_index

DESCRIPTION
  This function checks if an index in the Flow context tbl has been allocated
  memory

PARAMETERS
  index        - index to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_validate_flow
(
  ds_flow_context_s *flow_context_p
);

/*===========================================================================
FUNCTION ds_flow_context_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
void ds_flow_context_free_dyn_mem
(
  ds_flow_context_s *flow_context_p
);

#endif /* FEATURE_DATA_3GPP */
#endif /* DS3GPPFLOWCONTEXT_H */

