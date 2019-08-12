#ifndef DS_EPC_PDN_THROTTLE_MGR_H
#define DS_EPC_PDN_THROTTLE_MGR_H
/*===========================================================================

                      DS_EPC_PDN_THROTTLE_MGR. H
                   
DESCRIPTION
  This module handles PDN Throttling info. When a PDN is throttled on the trat,
  no handoff for this PDN.
 
  PDN throttle info is received from MH via PS events. This module maintains
  the PDN throttle info and provide service for handoff.
 
EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $Author: 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/19/11    jy    Created Module
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "sys.h"
#include "rex.h"

#include "list.h"

#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ps_sys.h"

//jyang01todo remove
#include "ds707_pdn_context.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define   DS_EPC_PDN_THRTL_IP_V4           0x00000001
#define   DS_EPC_PDN_THRTL_IP_V6           0x00000002

typedef struct ds_epc_pdn_thrtl_info_s ds_epc_pdn_thrtl_info_type;
struct ds_epc_pdn_thrtl_info_s
{
  list_link_type                 link;
  uint32                         thrtl_info_mask;
  ds_profile_info_type           apn_name;
};

/*===========================================================================
FUNCTION      DS_EPC_PDN_THROTTLE_MGR_IS_PDN_THROTTLED

DESCRIPTION   Check if a given PDN is a throttled PDN

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_throttle_mgr_is_pdn_throttled
(
  ds_profile_tech_etype            tech_type,
  ds_epc_pdn_thrtl_info_type      *pdn_thrtl_info_item_ptr,
  uint32                           subs_id
);

/*===========================================================================
FUNCTION      ds_epc_pdn_throttle_mgr_store_3gpp2_thrtl_info

DESCRIPTION   Store PDN throttle info in a list

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
list_type *ds_epc_pdn_throttle_mgr_store_3gpp2_thrtl_info
(
 ps_sys_pdn_throttle_info_type * thrtl_info
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_THROTTLE_MGR_CREATE_TEMP_LIST

DESCRIPTION   Get the pointer to a list.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
list_type *ds_epc_pdn_throttle_mgr_create_temp_list(void);

/*===========================================================================
FUNCTION      DS_EPC_PDN_THROTTLE_MGR_REFRESH_LIST_PTR

DESCRIPTION   Set the pointer of the throttled PDN list.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_throttle_mgr_refresh_list_ptr
(
  ds_profile_tech_etype  tech_type,
  list_type             *list_ptr,
  uint32                 subs_id
);


/*===========================================================================
FUNCTION      DS_EPC_PDN_THROTTLE_MGR_EMPTY_LIST

DESCRIPTION   Delete all throttle info in the list.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_throttle_mgr_empty_list
(
  list_type               *list_ptr
);
#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_PDN_THROTTLE_MGR__H */
