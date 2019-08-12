/******************************************************************************
  @file    ds_profile_int.h
  @brief   DS PROFILE internal declarations

  DESCRIPTION
  Internal file for ds profile. Depending on tech calls tech specific
  functions

  All function signatures are techology independent. The routines invoke the 
  technology specific routines

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_profilei.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/12   ab	   Added Check for lower bounds in tech value
05/31/11   bvd     Added changes for supporting non-persistent profile and other profile attributes
02/16/11   ttv     Added DSDS changes for consolidated profile family.
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#ifndef DS_PROFILEI_H
#define DS_PROFILEI_H

#include "ds_profile.h"
#include "customer.h"

#define TECH_IS_VALID( tech ) \
  dsi_profile_is_tech_valid(tech)

/*---------------------------------------------------------------------------
                    DSI PROFILE module
---------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION DSI_PROFILE_INIT

DESCRIPTION
  This function initializes the DS profile library. Calls operations and
  access module init functions for all techs

PARAMETERS

DEPENDENCIES 
  
RETURN VALUE 
  mask : ORed value of tech masks
SIDE EFFECTS 
 
===========================================================================*/
uint8 dsi_profile_init( 
  void 
);

/*===========================================================================
FUNCTION DSI_PROFILE_CLOSE_LIB

DESCRIPTION
  This function cleans up open handles if any.

PARAMETERS

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
 
===========================================================================*/
void dsi_profile_close_lib (
  void
);

/*===========================================================================
FUNCTION DS_PROFILE_END_TRANSACTION

DESCRIPTION
  This is internal function which calls tech specific function to commits
  the prefetched modified profile to the persistent storage on the modem. 

PARAMETERS
 hndl  : profile handle

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS        : On successful operation
  DS_PROFILE_REG_RESULT_ERR_LIB_NOT_INITED : Library is not initialized
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL : Invalid handle
  DS_PROFILE_REG_RESULT_FAIL           : On general errors.
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_end_transaction (
  ds_profile_hndl_type hndl
);

/*===========================================================================
FUNCTION DSI_PROFILE_ALLOC_HNDL

DESCRIPTION
  This function calls tech specific function to allocate memory for local
  copy of profile
 

PARAMETERS
  trn  : transaction type requested
  tech : technology type
  num  : profile number
  hndl : pointer to store profile handle returned
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS  : On successful operation
  DS_PROFILE_REG_RESULT_FAIL     : On general errors. This return code 
                                   provides blanket coverage
SIDE EFFECTS 
===========================================================================*/
ds_profile_status_etype dsi_profile_alloc_hndl(
  ds_profile_trn_etype   trn,
  ds_profile_tech_etype  tech, 
  ds_profile_num_type    num,
  ds_profile_hndl_type *hndl,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DSI_PROFILE_DEALLOC_HNDL

DESCRIPTION
  This function cleans up the memory used by the local copy of profile.
  Calls tech specific function to clean up
 
PARAMETERS
 hndl : pointer to handle
 
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS        : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL : Profile handle null or not for 
                                         write transaction
  DS_PROFILE_REG_RESULT_FAIL           : On general errors
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_dealloc_hndl(
  ds_profile_hndl_type *hndl
);

/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM

DESCRIPTION
  This function is used to get profile data element identified by the 
  identifier from the local copy. Calls tech specific funtion

PARAMETERS
  profile_hndl  : handle to profile to set profile data elements
  identifier    : to identify profile data elements
  info          : pointer to value to which data element is to be set
                  (size of buffer passed can atmost be the max size of 
                  the parameter which needs to be set)
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS            : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL     : Invalid handle 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT    : Invalid identifier
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID    : Buffer size more than expected
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type 
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_FAIL               : On general errors 
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_get_param(
  ds_profile_hndl_type        hndl,
  ds_profile_identifier_type  identifier,
  ds_profile_info_type       *info
);

/*===========================================================================
FUNCTION DS_PROFILE_SET_PARAM

DESCRIPTION
  This function is used to set profile data element identified by the 
  identifier in the local copy. Calls tech specific function

PARAMETERS
  profile_hndl  : handle to profile to set profile data elements
  identifier    : to identify profile data elements
  info          : pointer to value to which data element is to be set
                  (size of buffer passed can atmost be the max size of 
                  the parameter which needs to be set)
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS            : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL     : Invalid handle 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT    : Invalid identifier
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID    : Buffer size more than expected
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type 
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_FAIL               : On general errors 
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_set_param(
  ds_profile_hndl_type        hndl,
  ds_profile_identifier_type  identifier,
  const ds_profile_info_type *info
);

/*===========================================================================
FUNCTION DSI_PROFILE_CREATE

DESCRIPTION
  This function calls tech specific create function

PARAMETERS
  tech  : technology type
  config_ptr : config type for persistent property and other profile attributes
  num   : pointer to return profile number of profile created

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS  : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL     : On general errors. This return code 
                                   provides blanket coverage
SIDE EFFECTS 
===========================================================================*/
ds_profile_status_etype dsi_profile_create( 
  ds_profile_tech_etype   tech,
  ds_profile_config_type *config_ptr,
  ds_profile_num_type    *num
);

/*===========================================================================
FUNCTION DSI_PROFILE_DELETE

DESCRIPTION
  This function calls tech specific delete function

PARAMETERS
  tech : technology type
  num  : profile number
  enable_deleting_default_profile : if true, default profile can be deleted


DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS  : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL     : On general errors. This return code 
                                   provides blanket coverage
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_delete( 
  ds_profile_tech_etype  profile_type,
  ds_profile_num_type    profile_number,
  boolean enable_deleting_default_profile,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DSI_PROFILE_RESET_PARAM

DESCRIPTION
  This function calls internal tech specific function to reset parameter value

PARAMETERS
  tech  : technology type
  num   : profile number
  ident : to identify the profile parameter to be set to default
  mark_invalid: to specify if valid flag is marked in valid upon reset

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype dsi_profile_reset_param (  
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident,
  ds_profile_subs_etype subs_id,
  boolean mark_invalid
);

/*===========================================================================
FUNCTION DSI_PROFILE_RESET_PROFILE_TO_DEFAULT

DESCRIPTION
  This function calls internal tech specific function to reset all the
  parameters of the profile to default. 

PARAMETERS
  tech  : technology type
  num   : profile number

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
SIDE EFFECTS 
 
===========================================================================*/
ds_profile_status_etype dsi_profile_reset_profile_to_default (  
  ds_profile_tech_etype  tech,
  ds_profile_num_type num,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DSI_PROFILE_SET_DEFAULT_PROFILE_NUM

DESCRIPTION
  This function calls internal tech specific function to set the given
  profile number as default profile

PARAMETERS
  tech   : technology type
  family : profile family
  num    : profile number

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
SIDE EFFECTS 
 
===========================================================================*/
ds_profile_status_etype dsi_profile_set_default_profile (  
  ds_profile_tech_etype  tech,
  uint32                 family, 
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION DSI_PROFILE_GET_DEFAULT_PROFILE_NUM

DESCRIPTION
  This function calls internal tech specific function to get the default
  profile number for that family

PARAMETERS
  tech   : technology type
  family : profile family
  num    : pointer to store profile number

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
SIDE EFFECTS 
 
===========================================================================*/
ds_profile_status_etype dsi_profile_get_default_profile (  
  ds_profile_tech_etype  tech,
  uint32                 family, 
  ds_profile_num_type   *num
);

/*===========================================================================
FUNCTION DSI_PROFILE_SET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION
  This function calls internal tech specific function to set the given
  profile number as default profile number for the specified tech,
  family and subscription. 

PARAMETERS
  tech    : technology type (3gpp or 3gpp2)
  family  : profile family  (embedded or tethered)
  subs_id : subscription id ( subs_id 1 or subs_id 2)
  num     : profile number

DEPENDENCIES
  None

RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
  DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID : Invalid subscription id passed

SIDE EFFECTS
  None
===========================================================================*/
ds_profile_status_etype dsi_profile_set_default_profile_per_subs (
  ds_profile_tech_etype   tech,
  uint32                  family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type     num
);

/*===========================================================================
FUNCTION DSI_PROFILE_GET_DEFAULT_PROFILE_NUM_PER_SUBS

DESCRIPTION
  This function calls internal tech specific function to get the default
  profile number form the specified tech, family and subscription.

PARAMETERS
  tech    : technology type (3gpp or 3gpp2)
  family  : profile family  (embedded or tethered)
  subs_id : subscription id ( subs_id 1 or subs_id 2)
  num     : pointer to store profile number

DEPENDENCIES
  None

RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Profile type is invalid
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_ERR_INVAL_OP : Operation not supported for tech type
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return 
                                    code provides blanket coverage
  DS_PROFILE_REG_RESULT_ERR_INVAL_SUBS_ID : Invalid subscription id passed
  
SIDE EFFECTS
  None
===========================================================================*/
ds_profile_status_etype dsi_profile_get_default_profile_per_subs (  
  ds_profile_tech_etype   tech,
  uint32                  family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type     *num
);

/*===========================================================================
FUNCTION DSI_PROFILE_GET_LIST_ITR

DESCRIPTION
  This is used to get the list of Profiles of a particular tech type. This 
  function returns an Iterator. The Iterator is traversed using 
  DS_PROFILE_ITR_NEXT. After traversal is complete, the caller is 
  expected to call DS_PROFILE_ITR_DESTROY.

PARAMETERS
  tech  : technology type
  lst   : type of list, (list with all profiles / depending on some search)
  itr   : iterator to traverse through search result

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS  : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type
  DS_PROFILE_REG_RESULT_FAIL     : On general errors. This return code 
                                   provides blanket coverage
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_get_list_itr ( 
  ds_profile_tech_etype  tech,
  ds_profile_list_type  *lst,
  ds_profile_itr_type *itr,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DSI_PROFILE_ITR_NEXT

DESCRIPTION
  This routine advances the Iterator to the next element.  

PARAMETERS
  itr : iterator

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return code 
                                    provides blanket coverage
  DS_PROFILE_REG_RESULT_ERR_INVAL : Invalid argument (iterator)
  DS_PROFILE_REG_RESULT_LIST_END  : End of list
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_itr_next (  
  ds_profile_itr_type   itr
);

/*===========================================================================
FUNCTION DSI_PROFILE_ITR_FIRST

DESCRIPTION
  This routine resets the Iterator to the beginning of the list.  

PARAMETERS
  itr : iterator

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return code 
                                    provides blanket coverage
  DS_PROFILE_REG_RESULT_ERR_INVAL : Invalid argument (iterator)
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_itr_first ( 
  ds_profile_itr_type   itr
);

/*===========================================================================
FUNCTION DSI_PROFILE_GET_INFO_BY_ITR

DESCRIPTION
  This routine gets info stored in that Iterator node.  

PARAMETERS
  itr       : iterator
  list_info : pointer to structure to return profile info

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL : Invalid argument (iterator)
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return code 
                                    provides blanket coverage
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_get_info_by_itr ( 
  ds_profile_itr_type         itr,
  ds_profile_list_info_type  *list_info
);

/*===========================================================================
FUNCTION DSI_PROFILE_ITR_DESTROY

DESCRIPTION
  This routine destroys the Iterator  

PARAMETERS
  itr : iterator

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return code 
                                    provides blanket coverage
  DS_PROFILE_REG_RESULT_ERR_INVAL : Invalid argument (iterator)
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_itr_destroy ( 
  ds_profile_itr_type   itr
);

void dsi_profile_get_profile_num_range (
  ds_profile_tech_etype tech,
  uint16               *min_num,
  uint16               *max_num
);

ds_profile_status_etype dsi_profile_validate_profile_num( 
  ds_profile_tech_etype tech, 
  ds_profile_num_type num,
  ds_profile_subs_etype subs_id
);

ds_profile_status_etype dsi_profile_get_supported_type(  
  uint32                 *num,
  ds_profile_tech_etype  *tech
);

void dsi_profile_get_max_num( 
  ds_profile_tech_etype  tech,
  uint32                *max_num
);

/*===========================================================================
FUNCTION DSI_PROFILE_GET_PROFILE_NUM_FROM_HANDLE

DESCRIPTION
  This function returns profile number associated with handle 

PARAMETERS
  hndl : profile handle

DEPENDENCIES 
  
RETURN VALUE  
  Profile number associated with handle

SIDE EFFECTS 
  none
===========================================================================*/
int16 dsi_profile_get_profile_num_from_handle
(
  ds_profile_hndl_type * hndl
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DSI_PROFILE_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES

DESCRIPTION
  This routine updates the LTE attach PDN list profile parameters

PARAMETERS
  tech  : technology type

DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS   : On successful operation
  DS_PROFILE_REG_RESULT_FAIL      : On general errors. This return code 
                                    provides blanket coverage
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_update_lte_attach_pdn_list_profiles ( 
  ds_profile_tech_etype tech,
  ds_profile_subs_etype subs_id
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_PROFILE_GET_PARAM_IN_USE

DESCRIPTION
  This function is used to get profile data element identified by the 
  identifier from the local copy. Calls tech specific funtion

PARAMETERS
  profile_hndl  : handle to profile to set profile data elements
  identifier    : to identify profile data elements
  info          : pointer to value to which data element is to be set
                  (size of buffer passed can atmost be the max size of 
                  the parameter which needs to be set)
  tech_type     : Tech type for which the parameter is required.
  subs_id       : subscription id
DEPENDENCIES 
  
RETURN VALUE 
  DS_PROFILE_REG_RESULT_SUCCESS            : On successful operation
  DS_PROFILE_REG_RESULT_ERR_INVAL_HNDL     : Invalid handle 
  DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT    : Invalid identifier
  DS_PROFILE_REG_RESULT_ERR_LEN_INVALID    : Buffer size more than expected
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE : Invalid tech type 
  DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM  : Invalid profile number
  DS_PROFILE_REG_RESULT_FAIL               : On general errors 
SIDE EFFECTS 
  none
===========================================================================*/
ds_profile_status_etype dsi_profile_get_param_in_use(
  ds_profile_hndl_type        hndl,
  ds_profile_identifier_type  identifier,
  ds_profile_info_type       *info,
  ds_profile_tech_etype tech_type,
  ds_profile_subs_etype subs_id
);

/*===========================================================================
FUNCTION DSI_PROFILE_IS_TECH_VALID

DESCRIPTION
  This function check if the tech type passed is valid or not.

PARAMETERS 
  ds_profile_tech_etype    tech (ENUM type)

DEPENDENCIES 
NONE 
  
RETURN VALUE 
TRUE: if tech is valid 
FALSE otherwise 
  
SIDE EFFECTS 
NONE 
 
===========================================================================*/
boolean dsi_profile_is_tech_valid
(
  ds_profile_tech_etype    tech
);

#endif /* DS_PROFILEI_H */
