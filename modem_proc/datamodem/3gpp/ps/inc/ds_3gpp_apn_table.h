#ifndef DS_3GPP_APN_TABLE_H
#define DS_3GPP_APN_TABLE_H
/*===========================================================================

            A P N  T A B L E

===========================================================================*/
/*!
  @file
  ds_3gpp_apn_table.h

  @brief
  This header file defines the externalized function for apn table

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_apn_table.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/27/10     az      Initial version
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_context.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef enum 
{
  DS_3GPP_PDN_IP_TYPE_V4 = 0,
  DS_3GPP_PDN_IP_TYPE_V6
} ds_3gpp_throt_ip_type_e;


typedef struct 
{
  uint32						   cookie;
  byte                             apn[DS_UMTS_MAX_APN_STRING_LEN+1];  /* APN string */
  byte                             resolved_apn[DS_UMTS_MAX_APN_STRING_LEN+1];  
                                   /* APN string as resolved by NW. Filled up if apn 
                                      requested OTA is NULL */
  ds_apn_ip_support_type_e         ip_support;                     /* PDN IP support */
  ds_pdn_context_s*                v4_pdn_ptr;
  ds_pdn_context_s*                v6_pdn_ptr;
} ds_pdn_context_apn_table_dyn_s;

typedef struct
{
  ds_pdn_context_apn_table_dyn_s *ds_pdn_context_apn_table_dyn_p[DS_MAX_APN_NUM];
} ds_pdn_context_apn_table_s;

extern ds_pdn_context_apn_table_s  ds_apn_table[DS3GSUBSMGR_SUBS_ID_MAX];

#define APN_VALID_COOKIE  0xa6a6a6a6

/*===========================================================================
FUNCTION DSPDNCNTX_GET_APN_IP_SUPPORT_INFO

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular APN

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_apn_ip_support_info
(
  byte                      *apn,
  ds_apn_ip_support_type_e  *ip_support,
  sys_modem_as_id_e_type     subs_id
);


/*===========================================================================
FUNCTION DSPDNCNTX_UPDATE_APN_TABLE

DESCRIPTION
  This function updates the APN table with the supplied info

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_update_apn_table
(
  byte                      *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_context_s          *pdn_ptr
);

/*===========================================================================
FUNCTION DSPDNCNTX_REMOVE_ENTRY_IN_APN_TABLE

DESCRIPTION
  This function removes the corresponding entry in the APN table for a
  specific APN

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully removed this entry
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_remove_entry_in_apn_table
(
  ds_pdn_context_s   *pdn_cntx_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_IS_APN_TYPE_SUPPORTED

DESCRIPTION
  This function checks if APN can support the given iface

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - APN can support the given iface
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_apn_type_supported
(
  byte           *apn, 
  ps_iface_type  *iface_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_IP_SUPPORT

DESCRIPTION
  This function gets the ip support type of the given pdn context pointer

PARAMETERS   
    
DEPENDENCIES
  None.

RETURN VALUE
  IP support type of the pdn context

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_type_enum_type ds_pdn_cntxt_get_pdn_context_ip_support
(
  ds_pdn_context_s*           pdn_cntx_p
);

/*===========================================================================
FUNCTION DSPDNCNTX_IS_PDN_ASSOCIATED_FOR_APN

DESCRIPTION
  This function searchs the APN table and returns whether pdn context exists
  for a given apn

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If there is a pdn context associated with this apn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_pdn_associated_for_apn
(
  byte                      *apn,
  sys_modem_as_id_e_type     subs_id
);



/*===========================================================================
FUNCTION DSPDNCNTX_GET_IP_SUPPORT_INFO_FOR_PDN

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular PDN. Note that DS_IPV4V6_SINGLE_BEARER is not a valid support
  type for a particular PDN since there would be 2 PDN connectivity to the 
  same APN and this particular PDN can only be one of them.

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this pdn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_ip_support_info_for_pdn
(
  ds_pdn_context_s          *pdn_cntx_ptr,
  ds_apn_ip_support_type_e  *ip_support
);
/*===========================================================================
FUNCTION ds_pdn_cntxt_get_v4_and_v6_pdn_context

DESCRIPTION
  This function returns the v4 and v6 PDN pointers corresponding to 
  any given PDN entry in APN table. One(or both) of the PDN returned could be 
  same as the input PDN. This API is used to clearly diff bet v4 and v6 PDN.

PARAMETERS   :
  input_pdn_cntx_p*                 - input PDN pointer 
  v4_pdn_cntx_p** (OUT)-               Pointer to the v4 context 
  v6_pdn_cntx_p** (OUT)-               Pointer to the v6 context
      
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_v4_and_v6_pdn_context
(
  ds_pdn_context_s          *input_pdn_cntx_p,
  ds_pdn_context_s          **v4_pdn_cntx_p,
  ds_pdn_context_s          **v6_pdn_cntx_p
);
/*===========================================================================
FUNCTION DSPDNCNTX_GET_V4_PDN_CONTEXT

DESCRIPTION
  This function returns the pointer to the V4 PDN context

PARAMETERS   :
  byte*              - APN name
  ds_pdn_context_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_v4_pdn_context
(
  byte                      *apn,
  ds_pdn_context_s          **pdn_cntx_p,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DSPDNCNTX_GET_V6_PDN_CONTEXT

DESCRIPTION
  This function returns the pointer to the V6 PDN context

PARAMETERS   :
  byte*              - APN name
  ds_pdn_context_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_v6_pdn_context
(
  byte                      *apn,
  ds_pdn_context_s          **pdn_cntx_p,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_PDN_CONTXT_UPDATE_ALLOWED_IP_TYPE

DESCRIPTION
  This function blocks a specific IP type due to address configuration 
  failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_update_allowed_ip_type
(
  ds_pdn_context_s        *pdn_context_ptr,
  ds_apn_ip_support_type_e allowed_ip_type
);

/*===========================================================================
FUNCTION DS_PDN_CONTXT_UPDATE_RESOLVED_APN_NAME

DESCRIPTION
  This function updates an entry in the APN table with the resolved APN name.
  This is used only if the APN name in the profile was NULL

PARAMETERS   :
  pdn_cntx_ptr* - Pointer to PDN context
  byte* - APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_cntxt_update_resolved_apn_name 
(
  ds_pdn_context_s* pdn_cntx_ptr,
  byte* resolved_apn_name
);

/*==============================================================================
FUNCTION DS_PDN_CONTXT_GET_RESOLVED_APN_NAME

DESCRIPTION
  This function is used to return the resolved name given 
  the pdn context

PARAMETERS   :

  IN
  pdn_cntx_ptr* - Pointer to PDN context

  OUT
  byte** - Resolved APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_pdn_cntxt_get_resolved_apn_name
(
  ds_pdn_context_s* pdn_cntx_ptr, 
  byte** res_apn_name
);

/*===========================================================================
FUNCTION ds_apn_table_add_pdn

DESCRIPTION
  This function adds an entry to the APN table with the supplied info

PARAMETERS
  apn        - name of the APN
  ip_support - IP support for the APN 
  pdn_ptr    - Pointer to the PDN context


    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_add_pdn
(
  byte                      *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_context_s          *pdn_ptr
);

/*===========================================================================
FUNCTION ds_apn_table_free_dyn_mem

DESCRIPTION
  This function frees dynamic memory based on the index passed. 

PARAMETERS
   index       - Index to be freed
    
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None

===========================================================================*/
void ds_apn_table_free_dyn_mem
(
   int                       free_index,
   sys_modem_as_id_e_type    subs_id
);
#endif    /* DS_3GPP_APN_TABLE_H */



