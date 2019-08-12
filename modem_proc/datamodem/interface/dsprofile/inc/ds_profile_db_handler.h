/******************************************************************************
  @file    ds_profile_db_handler.h
  @brief   A handler to Profiles DB API

  DESCRIPTION
  This is a glue layer to Profiles DB. DB handler implements tech_fntbl_type interface.
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/12   ez      Created the module. First version of the file.
===========================================================================*/

#ifndef DS_PROFILE_DB_HANDLER_H
#define DS_PROFILE_DB_HANDLER_H

#include "ds_profile.h"
#include "ds_util.h"
#include "ps_sys.h"

/*===========================================================================
FUNCTION ds_profile_db_handler_dealloc
 
DESCRIPTION
  This function is used to free memory which was allocated for the local copy
  of the profile

PARAMETERS 
  prf_ptr - pointer to local copy of profile

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_dealloc(
  void *prf_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_set_param
 
DESCRIPTION
  This function is used to set parameter value in the local copy
  of the profile

PARAMETERS 
  tech     - technology 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified

DEPENDENCIES 
  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_param( 
  ds_profile_tech_etype       tech,
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  const ds_profile_info_type  *info_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_param
 
DESCRIPTION
  This function is used to get parameter value from the local copy
  of the profile

PARAMETERS 
  tech     - technology 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified

DEPENDENCIES  
  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_param( 
  ds_profile_tech_etype       tech,
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  ds_profile_info_type        *info_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_create

DESCRIPTION
  This function sends the request to create a new profile
PARAMETERS 
  tech       - technology 
  config_ptr - various configuration parameters for the profile creation
  num_ptr    - pointer to store the returned profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_create(
  ds_profile_tech_etype  tech,
  ds_profile_config_type *config_ptr,
  ds_profile_num_type    *num_ptr         
);

/*===========================================================================
FUNCTION ds_profile_db_handler_delete

DESCRIPTION
  This function sends the request to release the profile
PARAMETERS 
  tech - technology
  num  - profile number
  boolean - if true, default profile can be deleted

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_delete(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  boolean enable_deleting_default_profile,
  ds_profile_subs_etype subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_profile_read

DESCRIPTION
  This function is used to read a profile from modem to the local copy

PARAMETERS 
  tech    - technology
  num     - profile number
  prf_ptr - pointer to profile blob
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_profile_read(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  void                  *prf_ptr,
  ds_profile_subs_etype subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_profile_write

DESCRIPTION
  This function is used to write a profile to modem from the local copy

PARAMETERS 
  tech    - technology
  num     - profile number
  prf_ptr - pointer to profile blob
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_profile_write(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  void                  *prf_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_profile_to_default

DESCRIPTION
  This function is used to send a request to reset profile to default values.

PARAMETERS 
  tech - technology
  num  - profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_profile_to_default(  
  ds_profile_tech_etype  tech,
  ds_profile_num_type    num,
  ds_profile_subs_etype  subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_param

DESCRIPTION
  This function is used to send a request to reset a particular identifier 
  value to default.

PARAMETERS 
  tech  - technology
  num   - profile number
  ident - identifier for the parameter
  mark_invalid - mark the param invalid upon reset

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_param(
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_identifier_type  ident,
  ds_profile_subs_etype       subs,
  boolean mark_invalid
);

/*===========================================================================
FUNCTION ds_profile_db_handler_reset_pdp_context

DESCRIPTION
  This function is used to send a request to reset a pdp context 
  value to default.

PARAMETERS 
  tech - technology
  num  - profile number


DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_reset_pdp_context(
  ds_profile_tech_etype       tech,
  ds_profile_num_type         num,
  ds_profile_subs_etype       subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_set_default_profile

DESCRIPTION
  This function is used to set default profile number for a particular
  family

PARAMETERS 
  tech   - technology
  family - type of profile (socket, rmnet, atcop)
  num    - profile number

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_default_profile(  
  ds_profile_tech_etype  tech,
  uint32                 family, 
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_default_profile

DESCRIPTION
  This function is used to get default profile number for a particular
  family

PARAMETERS 
  tech    - technology
  family  - type of profile (socket, rmnet, atcop)
  num_ptr - profile number pointer

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_default_profile(
  ds_profile_tech_etype tech,
  uint32                family, 
  ds_profile_num_type   *num_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_set_default_profile_per_subs

DESCRIPTION
  This function is used to set default profile number for a particular
  family and subscription.

PARAMETERS
  tech    - technology
  family  - profile family  (embedded or tethered)
  subs_id - subscription id ( subs 1 or subs 2)
  num     - profile number

DEPENDENCIES 
  None
  
RETURN VALUE 
  
SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_set_default_profile_per_subs(
  ds_profile_tech_etype  tech,
  uint32                 family,
  ds_profile_subs_etype  subs_id,
  ds_profile_num_type    num
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_default_profile_per_subs

DESCRIPTION
  This function is used to get default profile number for a particular
  family and subscription.

PARAMETERS
  tech    - technology
  family  - profile family  (embedded or tethered)
  subs_id - subscription id ( subs 1 or subs 2)
  num_ptr - profile number pointer

DEPENDENCIES 
  None
  
RETURN VALUE 
  
SIDE EFFECTS 
  None
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_default_profile_per_subs(
  ds_profile_tech_etype   tech,
  uint32                  family,
  ds_profile_subs_etype   subs_id,
  ds_profile_num_type     *num_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_validate_profile_num

DESCRIPTION
  This function is used to validate a profile number.

PARAMETERS 
  tech - technology
  num  - profile number
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_validate_profile_num(
  ds_profile_tech_etype tech,
  ds_profile_num_type   num,
  ds_profile_subs_etype subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_num_range

DESCRIPTION
  This function is used to get the range of profile numbers

PARAMETERS 
  tech             - technology
  min_num, max_num - pointers to store range (min & max profile numbers)

DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
void ds_profile_db_handler_get_num_range(
  ds_profile_tech_etype tech,
  uint16                *min, 
  uint16                *max
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_list

DESCRIPTION
  This function is used to form a list of profile numbers depending on input
  value. (All profile numbers or search according to <key, value> pair)

PARAMETERS 
  tech     - technology
  hndl     - list handle
  lst_ptr  - pointer to return list
  
DEPENDENCIES 
  
RETURN VALUE 
  
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_list(
  ds_profile_tech_etype  tech,
  ds_util_list_hndl_type hndl,
  ds_profile_subs_etype  subs,
  ds_profile_list_type   *lst_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_list_node

DESCRIPTION
  This function is used to get info from a particular node in the list

PARAMETERS 
  tech          - technology
  hndl          - iterator handle
  list_info_ptr - pointer to store information to be returned
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_list_node(
  ds_profile_tech_etype      tech,
  ds_util_itr_hndl_type      hndl,
  ds_profile_list_info_type  *list_info_ptr
);

/*===========================================================================
FUNCTION ds_profile_db_handler_cleanup

DESCRIPTION
  This function is used for the final cleanup

PARAMETERS 
  tech          - technology
  
DEPENDENCIES 
  
RETURN VALUE 

SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_cleanup(
  ds_profile_tech_etype      tech
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_tech_type

DESCRIPTION
  This function gets the tech type for the given profile num

PARAMETERS
  profile_num : profile number

DEPENDENCIES 
  
RETURN VALUE 
  ds_profile_tech_etype
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_tech_etype ds_profile_db_handler_get_tech_type(
  ds_profile_num_type profile_num,
  ds_profile_subs_etype  subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_persistence

DESCRIPTION
  This function gets the persistence for the given profile num

PARAMETERS
  profile_num : profile number
  is_persistenct: boolean for persistency.

DEPENDENCIES 
  
RETURN VALUE 
  ds_profile_status_etype
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_persistence(
  ds_profile_num_type profile_num,
  boolean *is_persistent,
  ds_profile_subs_etype  subs
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_param_in_use
 
DESCRIPTION
  This function is used to get parameter value from the local copy
  of the profile

PARAMETERS 
  prf_ptr  - pointer to local copy of profile
  ident    - identifier whose value is to be set
  info_ptr - pointer to store value of identifier to be modified
  tech     - technology 

DEPENDENCIES  
  
RETURN VALUE 
 
SIDE EFFECTS 
  
===========================================================================*/
ds_profile_status_etype ds_profile_db_handler_get_param_in_use( 
  void                        *prf_ptr,
  ds_profile_identifier_type  ident,
  ds_profile_info_type        *info_ptr,
  ds_profile_tech_etype       tech_type,
  ds_profile_subs_etype       subs
);

ds_profile_status_etype ds_profile_db_handler_is_valid_profile(
  ds_profile_num_type profile_num, 
  ds_profile_subs_etype subs_id, 
  ds_profile_tech_etype tech_type, 
  boolean *is_valid
);

/*===========================================================================
FUNCTION ds_profile_db_handler_get_epc_support_val
 
DESCRIPTION
  This function is used to get the value if EPC profiles are supported
  by the modem

PARAMETERS 

DEPENDENCIES  
  
RETURN VALUE 
   TRUE:  if supported
   FALSE: otherwise
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_profile_db_handler_get_epc_support_val
(
  void 
);

#endif /* DS_PROFILE_DB_HANDLER_H */
