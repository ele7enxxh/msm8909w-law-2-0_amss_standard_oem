#ifndef _DS_QMI_WDS_PROFILE_REG_H
#define _DS_QMI_WDS_PROFILE_REG_H
/*===========================================================================

                         D S _ Q M I _ W D S _ P R O F I L E _ R E G . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface header file.

EXTERNALIZED FUNCTIONS
  
  QMI_WDS_PROFILE_MODIFY_SETTINGS()
    Modify profile settings 
  
  QMI_WDS_PROFILE_GET_SETTINGS()
    Get the settings for a given profile.
  
  QMI_WDS_PROFILE_GET_DEFAULT_SETTINGS()
    Get the settings of default profile.
  
  QMI_WDS_PROFILE_GET_LIST()
    Get the Profile list
  
  QMI_WDS_PROFILE_GET_DEFAULT_PROFILE_NUM()
    Get the default profile number.
  
  QMI_WDS_PROFILE_SET_DEFAULT_PROFILE_NUM()
    Set the default profile number.
  
  QMI_WDS_PROFILE_RESET_PROFILE_TO_DEFAULT()
    Reset the profile parameters to default.
  
  QMI_WDS_PROFILE_RESET_PARAM_TO_INVALID()
    Reset a given profile parameter to invalid.
  
  QMI_WDS_PROFILE_CREATE()
    Create a new Profile.
  
  QMI_WDS_PROFILE_DELETE()
    Delete a profile.
  
  QMI_WDS_PROFILE_UMTS_VALIDATE_PDP_TYPE()
    Validate PDP Type for a UMTS Profile.
  
Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ds_qmi_defs.h"
#include "ds_rmnet_meta_sm.h"
#include "qmi_si.h"


/*===========================================================================
                               MACROS
===========================================================================*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_MODIFY_SETTINGS()
 
  DESCRIPTION
    Modify an existing profile 
 
  PARAMETERS
    sdu_in       : incoming request
    profile_type : profile type (3GPP/3GPP2)
    profile_index: profile index to be modified.
    subs_id      : subscription identifier  
    error_val_ptr: Pointer containing the error code in case this API fails 
 
  RETURN VALUE
    response
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_modify_settings
(
  dsm_item_type **    sdu_in,
  uint8               profile_type,
  uint8               profile_index,
  uint32              subs_id,  
  qmi_error_type_v01   *error_val_ptr,
  ds_profile_status_etype *modify_status
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_SETTINGS()
 
  DESCRIPTION
    Get the settings for a given profile.
 
  PARAMETERS
    profile_type          : profile type
    profile_index         : profile whose settings are to be retrieved
 
  RETURN VALUE
    None
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ===========================================================================*/
dsm_item_type * qmi_wds_profile_get_settings
(
  uint8                        profile_type,
  uint8                        profile_index,
  uint32                       subs_id
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_DEFAULT_SETTINGS()
 
  DESCRIPTION
    For a given technology get the settings for default profile.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_get_default_settings
(
  uint8           profile_type,
  uint32          subs_id
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_LIST()

  DESCRIPTION
    Get a list of profiles.

  PARAMETERS
    profile_type   : profile type
    subs_id        : subs identifier

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_get_list
(
  uint8               profile_type,
  uint32              subs_id
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_GET_DEFAULT_PROFILE_NUM()
 
  DESCRIPTION
    For a given technology get the default profile number.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    subs_id        : subs identifier
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_get_default_profile_num
(
  uint8           profile_type,
  uint8           profile_family,
  uint32          subs_id
);


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_SET_DEFAULT_PROFILE_NUM()
 
  DESCRIPTION
    For a given technology set the default profile number.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index      : profile number that should be set to default
    subs_id          : subs identifier
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_set_default_profile_num
(
  uint8           profile_type,
  uint8           profile_family,
  uint8           profile_index,
  uint32          subs_id
);


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RESET_PROFILE_TO_DEFAULT()
 
  DESCRIPTION
    For a given technology resets the specified profile to default values.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be reset to use 
                       default values
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_reset_profile_to_default
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          subs_id
);


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RESET_PARAM_TO_INVALID()
 
  DESCRIPTION
    Set the specified parameter in the specified profile to invalid
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be reset to use 
                       default values
    param_id         : param id that should be reset to invalid
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_reset_param_to_invalid
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          param_id,
  uint32          subs_id
);


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_CREATE()
 
  DESCRIPTION
    Create a new profile.
 
  PARAMETERS
    sdu_in       : incoming request
    subs_id      : subscription identifier
 
  RETURN VALUE
    response
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_profile_create
(
  dsm_item_type **    sdu_in,
  uint32              subs_id
);


/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_DELETE()
 
  DESCRIPTION
  Deletes a specified profile for the specified technology.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
    profile_index    : profile number that should be deleted.
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_delete
(
  uint8           profile_type,
  uint8           profile_index,
  uint32          subs_id
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_UMTS_VALIDATE_PDP_TYPE()

  DESCRIPTION
    Validate the pdp_type set in the profile is IP

  PARAMETERS
    profile_num

  RETURN VALUE
    TRUE   - pdp_type is PDP-IP
    FALSE  - otherwise
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_wds_profile_umts_validate_pdp_type
(
  uint8  profile_num,
  uint32 subs_id
);

/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_RETRIEVE_DEFAULT_PROFILE_NUM()
 
  DESCRIPTION
  Get the default profile number for tech type.
 
  PARAMETERS
   profile_type (IN)  : Profile Type (3GPP/3GPP2)
   profile_family (INI) : Profile Family (RMNET/SOCKETs/ATCOP)
   profile_num  (OUT) : Default Profile Number
 
  RETURN VALUE
    None
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wds_profile_retrieve_default_profile_num
(
  uint8                        profile_type,
  uint8                        profile_family,
  uint16                      *default_profile_num,
  uint32                       subs_id
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
  FUNCTION QMI_WDS_PROFILE_UPDATE_LTE_ATTACH_PDN_LIST_PARAMS ()
 
  DESCRIPTION
  Updates the profile parameters in the lte attach pdn list of the 
  specified technology.
 
  PARAMETERS
    profile_type     : profile type (3GPP/3GPP2)
 
  RETURN VALUE
    ptr to response
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
 ==========================================================================*/
dsm_item_type * qmi_wds_profile_update_lte_attach_pdn_list_params
(
  uint8           profile_type,
  uint32          subs_id
);
#endif /* FEATURE_DATA_LTE */

#endif /* _DS_QMI_WDS_PROFILE_REG_H */
