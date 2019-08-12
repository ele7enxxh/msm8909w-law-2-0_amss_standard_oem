#ifndef DSPDPREGINT_H
#define DSPDPREGINT_H
/*===========================================================================

                        P D P   R E G I S T R Y   L I B R A R Y 

                          H E A D E R   F I L E

DESCRIPTION
  This file contains common data declarations and function prototypes for 
  accessing pdp profiles stored in EFS.
 
  The data is stored in EFS as packed structures and are acessed 
  as a packed structure. The offsets of structures within the profiles
  and the associated parameters are relative to the other fields.

  If any additions/deletions are made to the profile parameters, care
  should be taken to check that all the offset macros are defined 
  correctly as well.
  
Copyright (c) 2006-2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/dsprofile/inc/dsumtspdpregint.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
05/31/11   rr      Added support for transient profiles
02/28/11   hs      Added changes to support APN class in profile
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/17/11   ss      Added support for Iface linger.
11/22/10   ttv     Added support for DSDS.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
10/12/09   vk      PDP profile changes for IPv4v6 support
09/29/09   asn     Consolidation of Families
09/23/09   vd      Added support for LTE Qos.
09/22/09   vrk     Merged LTE changes.
05/19/09   vk      Defeaturization of PDP registry for CMI
11/15/07   asn     Added support for new feature MT-PDP terminating in TE
06/15/07   aw      Added support for PCSCF Address discovery using DHCP.
05/01/07   aw      Added support for IM CN Flag.
07/17/06   aw      Changed cscf to pcscf.
06/13/06   aw      Added support to store and retrieve cscf address flag.
06/08/06   aw      Converted this file into internal library and added 
                   wrapper functions around it. 
12/06/05   rr      Fixed a typo where ds_umts_pdp_addr_type was defined as 
                   union instead of struct.
12/01/05   rr      Added RMNET Profile Family.
05/12/05   rr      Added IPV6 code.
03/09/05   rc      Added status type DS_UMTS_PDP_READ_ONLY_FLAG_SET.
02/10/05   rc      Changed type (to UTF16 format) and length of profile name.
                   Added fields read_only_flag, otap-enabled_flag and 
                   otap_napid to the profile structure.
                   Added get/set prototype functions for read_only_flag,
                   otap_enabled and otap_napid fields of the profile.
                   All changes were made under FEATURE_PROFILE_OTAP_REQUIREMENTS.
                   Moved version field to be the first field of the structure.                   
11/24/04   rc      Added define for default ATCOP profile.
11/22/04   rc      Added support for separation of ATCOP and sockets 
                   profiles.
06/21/04   snb     Allow 16 PDP profiles under feature definition.
06/04/04   rc      Fixed Lint errors/warnings.
01/27/04   vsk     mainline m-pdp support
06/26/03   ar      Added support for secondary context and traffic flow
                   template paramaters.
02/14/03   vsk     Removed the dependency between context definition and
                   other parameter sets for a profile.
02/10/03   vsk     Added support to store/retrieve minimum UTMS QOS (AT+CGEQMIN)
                   parameters
01/16/03   vsk     creation of file 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsumtspdpreg.h"
#include "dsumts_subs_mgr.h"
#include "ds_profile_db.h"
#include "ds3gcfgmgr.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)

#define DS_FS_DIRECTORY_CHAR ('/')

/* ================================================================= 
       PUBLIC DATA DECLATATIONS FOR MODULE
===================================================================*/
 
/* Fully qualified EFS file name for the storing profile */
extern char ds_umts_pdp_fully_qual_profile_file_name[];


/*---------------------------------------------------------------------------
  Cached value of profile number for embedded calls.  This value is set
  during library initialization and anytime the embedded profile number
  is updated.  Reads for the embedded profile number will return this
  value.
---------------------------------------------------------------------------*/
extern byte  cached_embedded_call_profile_number[];

/*---------------------------------------------------------------------------
  Cached value of profile number for tethered calls.  This value is set
  during library initialization and anytime the tethered profile number
  is updated.  Reads for the tethered profile number will return this
  value.
---------------------------------------------------------------------------*/
extern byte  cached_tethered_call_profile_number[];

extern boolean tethered_call_profile_number_read[];
extern boolean embedded_call_profile_number_read[];

/*===========================================================================
                   EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
FUNCTION DS_UMTS_PDP_PROFILE_REGISTRY_INIT_INTERNAL

DESCRIPTION
  This function initializes the PDP profile registry. It checks the EFS to 
  see if the required profile files are present. If the files are not present,
  it creates the files and initializes them.

PARAMETERS 
  none

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS  - initialization was successfull
  DS_UMTS_PDP_FAIL     - initialization failed

SIDE EFFECTS 
  Creates the profiles in EFS if the profiles are not present at startup
===========================================================================*/
ds_umts_pdp_profile_status_etype ds_umts_pdp_registry_init_internal(void);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ALL_DATA_INTERNAL

DESCRIPTION
  This function retrieves all the parameters associated with a profile number. 
  
  NOTE:
  No check is done to see if a valid context is defined. The caller has to 
  check if the context and associated parameter set is defined and use it
  accordingly.

PARAMETERS
  profile_number  : profile number
  param subs_id   : Subscription id. 
  profile_data    : pointer to store the profile information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_all_data_internal
(
  uint16                   profile_number, 
  ds_umts_pdp_subs_e_type  subs_id,
  ds_umts_pdp_profile_type *profile_data 
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_UTF16_INTERNAL

DESCRIPTION
  This function retrieves the name of the specified profile number . 
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  profile_name_buf    : pointer to store the name string (UTF16 string)
  profile_name_buf_len: length of the profile_name_buf passed (UTF16 words)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of profile_name_buf is shorter
                                      than required

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_utf16_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                *profile_name_buf,
  byte                  profile_name_buf_len
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NAME_INTERNAL

DESCRIPTION
  This function retrieves the name of the specified profile number .
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  profile_name_buf    : pointer to store the name string
  profile_name_buf_len: length of the profile_name_buf passed

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of profile_name_buf is shorter
                                      than required

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  ds_umts_get_pdp_profile_name_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *profile_name_buf,
  byte                  profile_name_buf_len
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function retrieves the context information for a profile number. 
  This information is retrieved only if the context definition is valid
  (valid_flg for context definition is TRUE)

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_context_data: pointer to store the context information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_context_type  *pdp_context_data
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_pcscf_address" flag specified for a
  profile. 
  The "request_pcscf_address" indicates if the pcscf address should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  request_pcscf_address_flag   : pointer to store the request_pcscf_address_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_dhcp_pcscf_address_flag" flag specified
  for a profile. 
  The "request_dhcp_pcscf_address_flag" indicates if the pcscf address should be
  requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  request_pcscf_address_using_dhcp_flag   : pointer to store the 
                                            request_pcscf_address_using_dhcp_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_pcscf_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_pcscf_address_using_dhcp_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_REQUEST_DNS_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "request_dhcp_dns_address_flag" flag specified
  for a profile. 
  The "request_dhcp_dns_address_flag" indicates if the pcscf address should be
  requested for this profile using DHCP or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  request_dns_address_using_dhcp_flag   : pointer to store the 
                                            request_dns_address_using_dhcp_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_request_dns_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *request_dns_address_using_dhcp_flag
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IM_CN_FLAG_INTERNAL

DESCRIPTION
  This function retrieves the "im_cn" flag specified for a
  profile. 
  The "im_cn_flag" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  im_cn_flag   : pointer to store the im_cn_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_im_cn_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *im_cn_flag
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IS_VALID_INTERNAL

DESCRIPTION
  This function retrieves the "valid_flg" specified in the context 
  information for a profile number. The "valid_flg" indicates if the 
  context has been defined (TRUE) or not (FALSE)

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  valid_flg      : pointer to store the valid_flg

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_is_valid_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean                   *valid_flg
);




/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_PDP_ADDRESS_INTERNAL

DESCRIPTION
  This function retrieves the "pdp address" specified in the context 
  information for a profile number. This parameter is retrieved only 
  if a valid context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_address     : pointer to store the pdp address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_pdp_address_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *pdp_address
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_APN_INTERNAL

DESCRIPTION
  This function retrieves the "apn" specified in the context information 
  for a profile number. This parameter is retrieved only if a valid 
  context is defined for the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  apn_buf         : pointer to store the apn string
  apn_buf_len     : length of the buffer pointed by apn_buf

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of apn_buf is shorter than 
                                      required

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_context_info_apn_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                      *apn_buf,
  byte                      apn_buf_len
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the "ipv4
  addr alloc" mechanism specified in the context information for a profile 
  number. This parameter is retrieved only if a valid context is defined for 
  the profile specified.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  ipv4_addr_alloc : pointer to store the ipv4 addr alloc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_context_info_ipv4_addr_alloc_internal
(
  uint16                              profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_ipv4_addr_alloc_e_type *ipv4_addr_alloc
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_DISABLE_FLAG_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_diable_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_disable_flag : pointer to the boolean flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_disable_flag_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean  *apn_disable_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_INACTIVITY_TIMER_VAL_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "inactivity_timer_val" value specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number       : profile number
  subs_id :Subscription id. 
  inactivity_timer_val : pointer to the inactivity_timer_val

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_inactivity_timer_val_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32   *inactivity_timer_val
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_CLASS_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_class        : pointer to the uint8 (to hold APN class)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_class_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_class
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_APN_BEARER_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id :Subscription id. 
  apn_bearer       : pointer to the uint8 (to hold APN class)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_apn_bearer_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *apn_bearer
);


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MCC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mcc              : MCC of the carrier

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_mcc_internal
(
  uint16   profile_number,
  uint16   mcc
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MNC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  mnc              : MNC for the carrier 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_mnc_internal
(
  uint16   profile_number,
  ds_umts_3gpp_mnc_type  mnc
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_OPERATOR_RESERVED_PCO_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the operator reserved 
  "PCO" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  pco              : operator range

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  

SIDE EFFECTS 
  none
===========================================================================*/

ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_operator_reserved_pco_internal
(
  uint16   profile_number,
  uint16   pco
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK_INTERNAL

DESCRIPTION
  This function sets the Max PDN Conn Per Block field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  max_pdn_conn_per_blk : Max PDN Connections Per Blk

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_max_pdn_conn_per_blk_internal
(
  uint16   profile_number,
  uint16   max_pdn_conn_per_blk
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_PDN_REQ_WAIT_TIME_INTERNAL

DESCRIPTION
  This function sets the PDN Req Wait Time field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  pdn_req_wait_time    : PDN Req Wait Time

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_pdn_req_wait_time_internal
(
  uint16   profile_number,
  uint16   pdn_req_wait_time
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_MAX_PDN_CONN_TIME_INTERNAL

DESCRIPTION
  This function sets the Max PDN Conn TIme field in the profile to
  the specified value.

PARAMETERS
  profile_number       : profile number
  pdn_req_wait_time    : Max PDN Conn Time

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_max_pdn_conn_time_internal
(
  uint16   profile_number,
  uint16   max_pdn_conn_time
);
#endif /*FEATURE_DATA_LTE*/

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TRANSIENCE_FLAG
===========================================================================*/

/**
* Calls the registry internal function to retrieve the apn_class
* specified in the profile

  @param profile_number [in]   Profile number.
  @param subs_id [in]  Subscription id. 
* @param transience_flag [out] Pointer to the transience_flag (boolean).
   
  @return  
  DS_UMTS_PDP_SUCCESS -- Operation completed successfully. \n
  DS_UMTS_PDP_NOT_INITIALIZED -- Registry API library is not initialized. \n
  DS_UMTS_PDP_INVALID_PROFILE_NUM -- Invalid profile number. \n
  DS_UMTS_PDP_ACCESS_ERROR -- Error in accessing the file. \n
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED -- Valid context is not defined for the 
                                     profile.

  @dependencies
  ds_umts_pdp_registry_init_flag must be set, i.e., the registry 
  must have been initialized. \n
  A valid context must have been defined for the profile number.
*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_transience_flag
(
  uint16    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean   *transience_flag
);
/*~ FUNCTION ds_umts_get_pdp_profile_persistence_flag
    ONERROR return DS_UMTS_PDP_FAIL */
/*~ PARAM OUT transience_flag POINTER */

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_INTERNAL

DESCRIPTION
  This function retrieves the authentication information for a profile number.

  The auth info is returned ir-respective of the state of the context definition
  for the profile.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_auth_data   : pointer to store the authentication information

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_internal
(
  uint16                    profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_type     *pdp_auth_data
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_TYPE_INTERNAL

DESCRIPTION
  This function retrieves the "authentication type" specified in the
  authentication information for a profile number . This parameter is 
  retrieved ir-respective of the state of the context definition of
  the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  auth_type       : pointer to store the authentication type

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_type_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_auth_enum_type *auth_type
);



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_USERNAME_INTERNAL

DESCRIPTION
  This function retrieves the "username" specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  username_buf    : pointer to store the username string
  username_buf_len: length of the username_buf passed

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of username_buf is shorter than 
                                      required

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_username_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *username_buf,
  byte                       username_buf_len
);



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_AUTH_INFO_PASSWORD_INTERNAL

DESCRIPTION
  This function retrieves the password specified in the authentication 
  information for a profile number . This parameter is retrieved 
  ir-respective of the state of the context definition of the profile

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  password_buf    : pointer to store the password string
  password_buf_len: length of the password_buf passed

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of password_buf is shorter than 
                                      required

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_auth_info_password_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                       *password_buf,
  byte                       password_buf_len
);



/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the UMTS qos parameters (requested) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_umts_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_umts_qos_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the UMTS qos parameters (minimum) specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_umts_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_umts_qos_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_umts_qos_params_type *pdp_umts_qos_data
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  src_stat_desc   : pointer to store the src_stat_desc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the src_stat_desc value specified for a
  profile number .

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  src_stat_desc   : pointer to store the src_stat_desc

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32                       *src_stat_desc
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_INTERNAL

DESCRIPTION
  This function retrieves the "emergency_calls_are_supported" flag specified for a
  profile. 
  This flag indicates if IMS emergency can be made uising this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  emergency_calls_are_supported   : pointer to store the emergency_calls_are_supported flag.


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_emergency_calls_are_supported_flag_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *emergency_calls_are_supported
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number
  src_stat_desc   : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
);

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_SRC_STAT_DESC_REQ_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_src_stat_desc_req_info_internal
(
  uint16                  profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number
  src_stat_desc   : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none

===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                       profile_number,
  const uint32                 src_stat_desc
);

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_SRC_STAT_DESC_MIN_INFO_INTERNAL

DESCRIPTION
  This function calls the internal registry function to update the UMTS
  "requested" src_stat_desc info with the values passed. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_src_stat_desc_min_info_internal
(
  uint16                  profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function retrieves the GPRS "requested" qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_gprs_qos_req_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_gprs_qos_req_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function retrieves the GPRS "minimum" qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_gprs_qos_min_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_gprs_qos_min_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
);

/*===========================================================================
FUNCTION DS_3GPP_GET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the LTE qos parameters specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (valid_flg of QOS is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_lte_qos_data   : pointer to store the QOS parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_get_pdp_profile_lte_qos_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION

  This function retrieves ALL the Traffic Flow Template (TFT)
  parameters specified for a profile number. No check is done to see
  if a valid context is defined.  The caller is responsible for
  allocating sufficent storage for the returned structure.  The caller
  has to check if the context and associated parameter set is defined
  (valid_flg of TFT is set) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  Array of size DS_UMTS_MAX_TFT_PARAM_SETS is returned to caller.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_tft_all_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_tft_params_type      *pdp_tft_data
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_TFT_INFO_INTERNAL

DESCRIPTION
  This function retrieves the Traffic Flow Template (TFT) parameters
  specified for a profile number and filter identifer. No check is
  done to see if a valid context is defined.  The caller has to check
  if the context and associated parameter set is defined (valid_flg of
  TFT is set) and use it accordingly.

PARAMETERS
  profile_number : profile number
  subs_id :Subscription id. 
  filter_id      : filter identifier 
  pdp_tft_data   : pointer to store the TFT parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_INVALID_FILTER_ID    : Invalid filter identifier
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_tft_info_internal
(
  uint16                       profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                         filter_id,
  ds_umts_tft_params_type      *pdp_tft_data
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_LINGER_PARAMS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the linger parameters from the profile.

PARAMETERS
  profile_number     : profile number
  subs_id :Subscription id. 
  linger_params_data : pointer to store the linger parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
   ds_umts_get_pdp_profile_linger_params_info_internal
(
  uint16                     profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_linger_params_type *linger_params_data
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_INTERNAL

DESCRIPTION
  This function retrieves the DNS addresses specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined ( non zero dns address values ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  pdp_dns_addr    : pointer to store the dns addresses

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_internal
(
  uint16                         profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_dns_addresses_type *pdp_dns_addr
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_PRIMARY_INTERNAL

DESCRIPTION
  This function retrieves the primary DNS address specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  primary_dns     : pointer to store the dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_primary_internal
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *primary_dns
);


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_DNS_ADDRESS_INFO_SECONDARY_INTERNAL

DESCRIPTION
  This function retrieves the secondary DNS address specified for a
  profile number . No check is done to see if a valid context is defined. 
  The caller has to check if the context and associated parameter set is 
  defined (non zero dns address ) and use it accordingly.

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  secondary_dns   : pointer to store the dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_dns_address_info_secondary_internal
(
  uint16                  profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_pdp_addr_type   *secondary_dns
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_OTAP_NAPID_INTERNAL

DESCRIPTION
  This function retrieves the NAPID of the specified profile number .
  No check is done to see if a valid context is defined.

PARAMETERS
  profile_number          : profile number
  subs_id                 :subscription_id
  otap_napid_buf          : pointer to store the napid string (UTF8 string)
  otap_napid_buf_len      : length of the otap_napid_buf passed (UTF8 bytes)

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_INSUFFICIENT_OUTPUT_BUFFER : length of otap_napid_buf is shorter
                                      than required

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_otap_napid_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  byte                  *otap_napid_buf,
  byte                  otap_napid_buf_len
);
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_cLAT_ENABLED_INTERNAL

DESCRIPTION
  This function retrieves the "clat_enabled" flag specified for a 
  profile.
  The "im_cn" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  clat_enabled : pointer to store clat_enabled


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_clat_enabled
(
  uint16                   profile_number,
  ds_umts_pdp_subs_e_type  subs_id,
  boolean                  *xlat_enabled
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_READ_ONLY_INTERNAL

DESCRIPTION
  This function retrieves the "read_only_flag" specified in the context 
  information for a profile number. The "read_only_flag" indicates if the 
  context is read_only (TRUE) or not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id                 :subscription_id
  read_only_flag   : pointer to store the read_only_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_is_read_only_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *read_only_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IS_OTAP_ENABLED_INTERNAL

DESCRIPTION
  This function retrieves the "otap_enabled_flag" specified in the context 
  information for a profile number. The "otap_enabled_flag" indicates if the 
  context is otap_enabled (TRUE) or not (FALSE)
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id                 :subscription_id
  otap_enabled_flag   : pointer to store the otap_enabled_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_is_otap_enabled_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *otap_enabled_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ACCESS_CTRL_FLAG

DESCRIPTION
  This function retrieves the "access_ctrl_flag" specified for a PDP context.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  read_only_flag  : pointer to store the read_only_flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype  
  ds_umts_get_pdp_profile_access_ctrl_flag_internal
(
  uint16                          profile_number,
  dsat_pdp_access_control_e_type *access_ctrl_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_INTERNAL

DESCRIPTION
  This function sets the profile number specified to be used for embedded 
  calls. It reads the value from the embedded_call_prof_num file

PARAMETERS 
  subs_id: Subscription ID 
  profile_number  : profile number 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_num_for_embedded_calls_internal
(
  ds_umts_pdp_subs_e_type          subs_id,
  byte*   profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS_INTERNAL

DESCRIPTION
  This function retrieves the profile number specififed for tethered
  data calls. The cached value is return if available; otherwise, 
  the read is done from the EFS file.

PARAMETERS
  subs_id: Subscription ID  
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  The cached sockets call number is updated if necessary.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_num_for_tethered_calls_internal
( 
  ds_umts_pdp_subs_e_type subs_id,
  byte*   profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_NUM_FOR_ATCOP_CALLS_INTERNAL

DESCRIPTION
  This function retrieves the profile number specififed for sockets
  data calls ( sockets calls ).  The cached value is return if available;
  otherwise, the read is done from the EFS file.

PARAMETERS
  profile_number  : pointer to store profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS 
  The cached sockets call number is updated if necessary.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_get_pdp_profile_num_for_atcop_calls_internal
( 
  byte *profile_number
);
/*===========================================================================
FUNCTION DS_UMTS_PDP_SET_QUALIFIED_PROFILE_FILE_NAME

DESCRIPTION
  This function sets the PDP qualified file name.
  
PARAMETERS
  uint16 profile_number
  
DEPENDENCIES 
  None

RETURN VALUE 
  boolean

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_umts_pdp_set_qualified_profile_file_name
(
  uint16 profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_UPDATE_EMBEDDED_CALL_PROF_NUM

DESCRIPTION
  This function updates the profile number in the "embedded_call_prof_num" file
  with the value passed.  The profile number is cached to support reads when
  EFS read operations are disabled.

PARAMETERS 
 subs_id : Subscription ID 
 profile_number : profile number to be written to the file
  
DEPENDENCIES 
  None

RETURN VALUE 
  TRUE       : profile number updated successfully
  FALSE      : Error in updating profile number

SIDE EFFECTS 
  None
===========================================================================*/

boolean ds_umts_update_embedded_call_prof_num
(
  ds_umts_pdp_subs_e_type subs_id,
  uint16  profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME_UTF16_INTERNAL

DESCRIPTION
  This function updates the name of a profile with the value passed.
  NOTE: The name of the profile can be updated at any time ir-respecitive
  of the valididity of the context associated with the profile

PARAMETERS
  profile_number  : profile number
  profile_name_buf : pointer to the name string (UTF16 string)
  profile_name_len : length of the name string (UTF16 words)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of profile name is too long

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_name_utf16_internal
(
  uint16                profile_number,
  const uint16          *profile_name_buf,
  byte                  profile_name_len
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NAME_INTERNAL

DESCRIPTION
  This function updates the name of a profile with the value passed.
  NOTE: The name of the profile can be updated at any time ir-respecitive
  of the valididity of the context associated with the profile

PARAMETERS
  profile_number  : profile number
  profile_name_buf : pointer to the name string
  profile_name_len : length of the name string

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of profile name is too long

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_name_internal
(
  uint16                profile_number,
  const byte            *profile_name_buf,
  byte                  profile_name_len
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_FLAG_INTERNAL

DESCRIPTION
  This function sets the "request_pcscf_address_flag" flag specified for a 
  profile. 
  The "request_pcscf_address_flag" indicates if the pcscf address should be
  requested for this profile using PCO or not.

PARAMETERS
  profile_number  : profile number
  request_pcscf_address_flag   : pointer to store the request_pcscf_address_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_request_pcscf_address_flag_internal
(
  uint16                profile_number,
  boolean               request_pcscf_address_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_REQUEST_PCSCF_ADDRESS_USING_DHCP_FLAG_INTERNAL

DESCRIPTION
  This function sets the "request_dhcp_pcscf_address_flag" flag specified for a 
  profile. 
  The "request_dhcp_pcscf_address_flag" indicates if the pcscf address should be
  requested for this profile using DHCP or not.

PARAMETERS
  profile_number  : profile number
  request_pcscf_address_using_dhcp_flag   : pointer to store the 
                                        request_pcscf_address_flag_using_dhcp


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_request_pcscf_address_using_dhcp_flag_internal
(
  uint16                profile_number,
  boolean               request_pcscf_address_using_dhcp_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_IM_CN_FLAG_INTERNAL

DESCRIPTION
  This function sets the "im_cn" flag specified for a 
  profile. 
  The "im_cn" indicates if the "im_cn" flag should be sent out in PDP
  Context activation message.

PARAMETERS
  profile_number  : profile number
  im_cn_flag   : pointer to store the im_cn_flag


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_im_cn_flag_internal
(
  uint16                profile_number,
  boolean               im_cn_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function updates the context definition  parameters with the
  values passed.

PARAMETERS
  profile_number  : profile number
  pdp_context_data  : pointer to the context definition 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : the valid_flg for the context is not 
                                    set to TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_internal
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_EX_INTERNAL

DESCRIPTION
  This function updates the context definition  parameters with the values
  passed. This is special API called only from ATCOP which will always try to
  create persistent profile. It also updates the cache table with the new
  context information.

PARAMETERS
  profile_number  : profile number
  pdp_context_data  : pointer to the context definition 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : the valid_flg for the context is not 
                                    set to TRUE
  DS_UMTS_PDP_ERR_OUT_OF_PERSISTENT_PROFILES : All persistent profile slots are taken.
                                               Can't create any more persistent profile.

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_ex_internal
(
  uint16                    profile_number,
  const ds_umts_pdp_context_type  *pdp_context_data
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_APN_INTERNAL

DESCRIPTION
  This function updates the APN for the profile specified with the
  value passed. The APN  can be updated ONLY for a valid context as it
  is part of the context definition

PARAMETERS
  profile_number  : profile number
  apn_buf         : pointer to the APN string
  apn_buf_len     : length of the APN string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of APN string is too long

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_apn_internal
(
  uint16                    profile_number,
  const byte                *apn_buf,
  byte                      apn_buf_len
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_ADDRESS_INTERNAL

DESCRIPTION
  This function updates the pdp address for the profile specified with the
  value passed. The address can be updated ONLY for a valid context as it 
  is part of the context definition

PARAMETERS
  profile_number  : profile number
  pdp_address     : ip address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_address_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     pdp_address
);


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_IPV4_ADDR_ALLOC_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the ipv4 addr
  alloc setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number  : profile number
  ipv4_addr_alloc : ipv4 address allocation (mechanism)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number already

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_context_info_ipv4_addr_alloc_internal
(
  uint16                              profile_number,
  ds_umts_pdp_ipv4_addr_alloc_e_type  ipv4_addr_alloc
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_DISABLE_FLAG_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the apn diable
  flag setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number   : profile number
  apn_disable_flag : boolean value to set in the registry

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_disable_flag_internal
(
  uint16   profile_number,
  boolean  apn_disable_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_INACTIVITY_TIMER_VAL_INTERNAL

DESCRIPTION
  This function call the internal registry function to update the inactivity
  timer setting for the profile specified with the value passed. The update 
  can be done ONLY for a valid context as it is part of the context 
  definition. It also updates the cache table with the new value.

PARAMETERS
  profile_number       : profile number
  inactivity_timer_val : boolean value to set in the registry

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_inactivity_timer_val_internal
(
  uint16   profile_number,
  uint32   inactivity_timer_val
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_CLASS_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "apn_class" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  apn_class        : uint8 value being passed in for the APN class

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_class_internal
(
  uint16   profile_number,
  uint8    apn_class
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_APN_BEARER_INTERNAL

DESCRIPTION
  This function calls the registry internal function to set the 
  "apn_bearer" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  apn_bearer       : uint8 value being passed in for the APN class

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_set_pdp_profile_apn_bearer_internal
(
  uint16   profile_number,
  uint8    apn_bearer
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_CONTEXT_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the context information to be defined for the 
  profile number specified and the following values are set for the context
  pdp type : IP
  IP address :0.0.0.0
  APN        : Null string
  data & header compression : OFF

  This function can be called by UI to define a context & hence a profile
  to default and then allow the user to override other fields of the 
  context

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_context_info_to_default_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_EX_INTERNAL

DESCRIPTION 
  This function is similar to DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_INTERNAL.
  It takes an extra parameter, transience_flag - allowing DS_PROFILE_CREATE_EX to
  create a profile in a single EFS update.
 
  This function sets the profile information to default values for a given
  profile number. It defines a valid context for the profile (pdp-ip)or 
  pdp-ipv6  as per ip_vsn and sets all other parameters to the default values
  context : no compression, ip address :0.0.0.0, APN : Null
  Auth    : no auth
  QOS     : no qos params defined ( umts and gprs)
  dns     : primary and secondary set to 0.0.0.0
  TFT     : no tft

  NOTE : The name of the profile is left as-is.
         If the profile was already created (valid flag true),
         the transience_flag is left as-is, otherwise transience_flag
         is set.
 
 
PARAMETERS 
  profile_number  : profile number
  boolean         : transience_flag
DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_to_default_ex_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn,
  boolean                 transience_flag
);

/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_CONTEXT_INFO_INTERNAL

DESCRIPTION
  This function resets the context information to be undefined for the 
  profile number specified. It does not over-write the values of
  other parameter sets (qos, auth etc).

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
 The context definition is reset, but all other parameter sets are left
 as is. Auth, qos, dns address values are not changed.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_context_info_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_INTERNAL

DESCRIPTION
  This function updates the authentication information with the values passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_auth_data   : pointer to authentication information struct

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_internal
(
  uint16                    profile_number,
  const ds_umts_pdp_auth_type           *pdp_auth_data
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TYPE_INTERNAL

DESCRIPTION
  This function updates the authentication type field of authentication 
  information with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  auth_type       : Authentication type

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_type_internal
(
  uint16                     profile_number,
  ds_umts_pdp_auth_enum_type auth_type
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_USERNAME_INTERNAL

DESCRIPTION
  This function updates the username field of authentication information 
  with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  usename_buf     : pointer to the username string
  username_buf_len: length of the username string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : The length of username string is too long

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_username_internal
(
  uint16                    profile_number,
  const byte                *usernme_buf,
  byte                      username_buf_len
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_PASSWORD_INTERNAL

DESCRIPTION
  This function updates the password field of authentication information 
  with the value passed.
  The auth parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  password_buf    : pointer to the password string
  password_buf_len: length of the password string

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : The length of password string is too long

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_password_internal
(  
  uint16                    profile_number,
  const byte                *password_buf,
  byte                      password_buf_len
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_AUTH_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function resets the authentication information for the profile 
  specified. Authentication is set to "none" 
  The auth parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_auth_info_to_default_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function updates the UMTS "requested" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_umts_qos_req_info_internal
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
);


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_REQ_INFO_INTERNAL
DESCRIPTION
  This function updates the UMTS "requested" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the umts qos (requested) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_umts_qos_req_info_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the UMTS "minimum" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_umts_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_umts_qos_min_info_internal
(
  uint16                       profile_number,
  const ds_umts_umts_qos_params_type *pdp_umts_qos_data
);


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_UMTS_QOS_MIN_INFO_INTERNAL
DESCRIPTION
  This function updates the UMTS "minimum" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the umts qos (minimum) information to be undefined and all parameters
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_umts_qos_min_info_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "requested" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_req_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_gprs_qos_req_info_internal
(
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_req_data
);



/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_REQ_INFO_INTERNAL
DESCRIPTION
  This function updates the GPRS "requested" qos parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the gprs requested qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_gprs_qos_req_info_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "minimum" qos parameters with the values
  passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_gprs_qos_min_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_gprs_qos_min_info_internal
(
  uint16                       profile_number,
  const ds_umts_gprs_qos_params_type *pdp_gprs_qos_min_data
);



/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_GPRS_QOS_MIN_INFO_INTERNAL

DESCRIPTION
  This function updates the GPRS "minimum" qos required parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the gprs minimum qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_gprs_qos_min_info_internal
(
  uint16                      profile_number
);


/*===========================================================================
FUNCTION DS_3GPP_SET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function updates the LTE qos parameters with the values passed. 
  The qos parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_lte_qos_data : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_set_pdp_profile_lte_qos_info_internal
(
  uint16                             profile_number,
  const ds_3gpp_lte_qos_params_type *pdp_lte_qos_data
);

/*===========================================================================
FUNCTION DS_3GPP_RESET_PDP_PROFILE_LTE_QOS_INFO_INTERNAL

DESCRIPTION
  This function updates the LTE qos required parameters for the 
  profile indicated. The parameters are updated to be subscribed values
  and the "valid_flg" is set to FALSE indicating that the values are not
  valid.
  The qos parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the lte qos information to be undefined and all parameters 
  are set to subscribed values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_3gpp_reset_pdp_profile_lte_qos_info_internal
(
  uint16                      profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffic Flow Template (TFT) parameters with
  the values passed for the given filter identifier. 
  The TFT parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  filter_id       : filter identifer
  pdp_tft_data    : the values to be updated with 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_INVALID_FILTER_ID    : Invalid fulter identifier
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_tft_info_internal
(
  uint16                       profile_number,
  byte                         filter_id,
  const ds_umts_tft_params_type      *pdp_tft_data
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_LINGER_PARAMS_INFO_INTERNAL

DESCRIPTION
  This function updates the linger parameters in the profile

PARAMETERS
  profile_number     : profile number
  linger_params_data : pointer to the linger parameter data

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype  
   ds_umts_set_pdp_profile_linger_params_info_internal
(
  uint16                           profile_number,
  const ds_umts_linger_params_type *linger_params_data
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffic Flow Template (TFT) parameters
  with the values passed. The data pointer is assumed to be the
  address of an array.
  The TFT parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS
  profile_number  : profile number
  pdp_tft_data    : the values to be updated with;
                    array[DS_UMTS_MAX_TFT_PARAM_SETS] 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  The "valid_flg" in the parameter passed SHOULD be set to TRUE

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_PARMS_VALID_FLAG_NOT_SET : Valid flag for the parameters is not TRUE

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_tft_all_info_internal
(
  uint16                      profile_number,
  const ds_umts_tft_params_type     *pdp_tft_data
);



/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TFT_ALL_INFO_INTERNAL

DESCRIPTION
  This function updates the Traffice Flow Template (TFT) required
  parameters for the profile indicated. The parameters are updated to
  zero values and the "valid_flg" is set to FALSE indicating that the
  values are not valid.

  The TFT parameters are updated even if a valid context defintion 
  is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  sets the TFT information to be undefined and all parameters 
  are set to zero values
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_tft_all_info_internal
(
  uint16                      profile_number
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO_INTERNAL

DESCRIPTION
  This function updates the dns addresses for the profile specified with 
  the values passed
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS 
  profile_number  : profile number
  pds_dns_addr    : pointer to the dns addresses

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_info_internal
(
  uint16                         profile_number,
  const ds_umts_pdp_dns_addresses_type *pdp_dns_addr
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_PRIMARY_INTERNAL

DESCRIPTION
  This function sets the primary dns address for a pdp profile
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS 
  profile_number  : profile number
  primary_dns_addr: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_primary_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     primary_dns_addr
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_SECONDARY_INTERNAL

DESCRIPTION
  This function sets the secondary dns address for a pdp profile
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS 
  profile_number  : profile number
  secondary_dns_addr: dns address

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_secondary_internal
(
  uint16                    profile_number,
  ds_umts_pdp_addr_type     secondary_dns_addr
);


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_DNS_ADDRESS_INFO_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the dns addresses of a pdp profile to default values
  primary, secondary DNS addresses are set to 0.0.0.0
  The dns parameters are updated with the values passed even if a valid 
  context defintion is not present. 

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_dns_address_info_to_default_internal
(
  uint16                      profile_number
);


/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_INTERNAL

DESCRIPTION
  This function is used to set all the parameters associated with a profile
  The profile_data is passed in. The profile_data should atleast have a
  valid context for this function to update the profile parameters 
  
  NOTE : The name of the profile is overwritten with the value passed in 
  the profile

PARAMETERS 
  profile_number  : profile number
  profile_data    : pointer to the profile data parameters

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : The parameter did not contain a valid context

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_internal
(
  uint16                    profile_number, 
  ds_umts_pdp_profile_type  *profile_data 
);



/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_ALL_DATA_TO_DEFAULT_INTERNAL

DESCRIPTION
  This function sets the profile information to default values for a given
  profile number. It defines a valid context for the profile (pdp-ip) and
  sets all other parameters to the default values
  context : no compression, ip address :0.0.0.0, APN : Null
  Auth    : no auth
  QOS     : no qos params defined ( umts and gprs)
  dns     : primary and secondary set to 0.0.0.0

  NOTE : The name of the profile is left as-is.

PARAMETERS 
  profile_number  : profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten.
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_profile_all_data_to_default_internal
(
  uint16                  profile_number,
  ds_ip_version_enum_type ip_vsn
);


/*===========================================================================
FUNCTION DS_UMTS_RESET_PDP_PROFILE_TO_UNDEFINED_INTERNAL

DESCRIPTION
  This function resets the profile specified to undefined values. 
  An undefined profile has the foll : 
  context - not defined
  auth information : none
  QOS params ( umts and gprs ) : not defined
  dns addresses : all addresses are set to 0.0.0.0

  NOTE : The name of the profile is left as-is.

PARAMETERS 
  subs_id                 :subscription_id
  profile_number  : profile number to be set to undefined 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  All the profile parameters are overwritten
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_reset_pdp_profile_to_undefined_internal
(
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                      profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_EMBEDDED_CALLS_INTERNAL

DESCRIPTION
  This function sets the profile number specified to be used for embedded 
  calls. It writes the value specified to the file the embedded_call_prof_num
  file.

PARAMETERS 
  subs_id                 :subscription_id 
  profile_number  : profile number 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
   ds_umts_set_pdp_profile_num_for_embedded_calls_internal
(
    uint16 profile_number,
  ds_umts_pdp_subs_e_type subs_id
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_NUM_FOR_TETHERED_CALLS_INTERNAL

DESCRIPTION
  This function sets the profile number specified to be used for tethered 
  calls. It writes the value specified to the file the tethered_call_prof_num 
  file.

PARAMETERS 
  subs_id                 :subscription_id 
  profile_number  : profile number 

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
   ds_umts_set_pdp_profile_num_for_tethered_calls_internal
( 
  ds_umts_pdp_subs_e_type subs_id,
  uint16 profile_number
);

#ifdef FEATURE_GOBI_MUXD_PDP_PROFILE_DIR
static char* undp_get_muxd_pdp_profile_dirname(void);
#endif /* FEATURE_GOBI_MUXD_PDP_PROFILE_DIR */

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_NAPID_INTERNAL

DESCRIPTION
  This function updates the napid of a profile with the value passed.
  No check is done to see if a valid context is defined.

PARAMETERS
  profile_number : profile number
  otap_napid_buf : pointer to the napid string (UTF8 string)
  otap_napid_len : length of the napid string (UTF8 bytes)

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file
  DS_UMTS_PDP_INPUT_BUFFER_LEN_INVALID : length of otap NAPID is too long

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_otap_napid_internal
(
  uint16                    profile_number,
  byte                      *otap_napid_buf,
  byte                      otap_napid_len
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_READ_ONLY_FLAG_INTERNAL

DESCRIPTION
  This function updates the "read_only_flag" of a profile with the value passed.
  No check is done to see if a valid context is defined.

PARAMETERS
  profile_number : profile number
  read_only_flag : profile is read-only or not

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_read_only_flag_internal
(
  uint16                    profile_number,
  boolean                   read_only_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_OTAP_ENABLED_FLAG_INTERNAL

DESCRIPTION
  This function updates the "otap_enabled_flag" of a profile with the value 
  passed. No check is done to see if a valid context is defined.

PARAMETERS
  profile_number   : profile number
  otap_enabled_flag : profile is otap_enabled or not

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
  ds_umts_set_pdp_otap_enabled_flag_internal
(
  uint16                    profile_number,
  boolean                   otap_enabled_flag
);

/*===========================================================================
FUNCTION DS_UMTS_SET_DEFAULT_PROFILE_NUMBER_PER_SUBS_INTERNAL

DESCRIPTION
  This function sets the profile number for tethered or embedded family for 
  the specified subscription. The value is written to either 
  embedded_call_prof_nums_dsds or tethered_call_prof_nums_dsds file.

PARAMETERS 
  family           : Denotes embedded or tetherd profile
  subs             : Denotes one of two active subscriptions
  profile_number   : Profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : The operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
ds_umts_set_default_profile_number_internal
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                               profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_GET_DEFAULT_PROFILE_NUMBER_INTERNAL

DESCRIPTION
  This function gets the profile number for the specified subscription. 
  It gets the value specified to the embedded_call_prof_nums_dsds file or 
  tethered_call_prof_nums_dsds file.

PARAMETERS 
  family           : Denotes embedded or tethered profile
  subs             : Denotes one of two active subscriptions
  profile_number   : Profile number

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : The operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in updating the file

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype
ds_umts_get_default_profile_number_internal
(
  ds_umts_profile_family_e_type        family,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16                              *profile_number
);

/*===========================================================================
FUNCTION DS_UMTS_SET_PDP_PROFILE_EMERGENCY_CALLS_ARE_SUPPORTED_FLAG_INTERNAL

DESCRIPTION
  This function calls the internal registry function to set the
  "emergency_calls_are_supported" flag specified for a profile. 
  This flag indicates if IMS emergency calls can be made using this profile.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  emergency_calls_are_supported   :value to store in the emergency_calls_are_supported
                                   flag.

DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_set_pdp_profile_emergency_calls_are_supported_flag_internal
(
  uint16                profile_number,
  boolean               emergency_calls_are_supported
);
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MCC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MCC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  mcc              : pointer to the uint16 (to hold MCC)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mcc_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *mcc
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MNC_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  mnc              : pointer to the ds_umts_3gpp_mnc_type (to hold MNC)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_mnc_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  ds_umts_3gpp_mnc_type  *mnc
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_OPERATOR_RESERVED_PCO_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "MNC" for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  pco              : pointer to the uint16 (to hold operator reserved PCO)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_operator_reserved_pco_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16    *pco
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_PER_BLK_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Per Blk for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  max_pdn_conn_per_blk: pointer to the uint16 (to hold max_pdn_conn_per_blk)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_max_pdn_conn_per_blk_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *max_pdn_conn_per_blk
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_REQ_WAIT_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  PDN Req Wait Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  pdn_req_wait_time: pointer to the uint16 (to hold pdn_req_wait_time)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_pdn_req_wait_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *pdn_req_wait_time
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_MAX_PDN_CONN_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  Max PDN Conn Time for a specified profile number. 
  This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number   : profile number
  subs_id                 :subscription_id
  max_pdn_conn_time: pointer to the uint16 (to hold max_pdn_conn_time)

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_max_pdn_conn_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint16   *max_pdn_conn_time
);
#endif /*FEATURE_DATA_LTE*/


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_USER_APP_DATA_INTERNAL
===========================================================================*/
/** 
  
  This function gets the value of parameter
  "user_app_data" for a specified profile number. 

  @profile_number            : profile number
  subs_id                 :subscription_id
  @user_app_data        : uint32 value being passed in for the
                               user_app_data

  DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  
SIDE EFFECTS 
  none

*/
ds_umts_pdp_profile_status_etype 
ds_umts_get_pdp_profile_user_app_data_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint32    *user_app_data
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_ROAMING_DISALLOWED_FLAG_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "roaming_disallowed_flag" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number          : profile number
  subs_id                 :subscription_id
  roaming_disallowed_flag : pointer to the boolean flag

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_roaming_disallowed_flag_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean  *roaming_disallowed_flag
);

/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_PDN_DISCON_WAIT_TIME_INTERNAL

DESCRIPTION
  This function calls the registry internal function to retrieve the 
  "pdn_discon_wait_time" mechanism specified in the context information for a 
  profile number. This parameter is retrieved only if a valid context is 
  defined for the profile specified.

PARAMETERS
  profile_number          : profile number
  subs_id                 :subscription_id
  pdn_discon_wait_time    : pointer to the pdn_discon_wait_time

DEPENDENCIES 
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)
  A valid context must have been defined for the profile number

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system
  DS_UMTS_PDP_CONTEXT_NOT_DEFINED  : valid context not defined for profile

SIDE EFFECTS 
  none
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_pdn_discon_wait_time_internal
(
  uint16   profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  uint8    *pdn_discon_wait_time
);
#if defined(TARGET_UNIX)
/* Restore data structure storage to 32bit word boundaries */
  #pragma pack(4)
#endif /* defined(TARGET_UNIX) */


/*===========================================================================
FUNCTION DS_UMTS_GET_PDP_PROFILE_IPV6_PREFIX_DELEGATION_INTERNAL

DESCRIPTION
  This function retrieves the "ipv6_prefix_delegation" flag specified for a 
  profile.
  The "im_cn" indicates if the dedicated context should be
  requested for this profile or not.
  Note : This information can be retrieved even if the context (profile) has
  not been defined

PARAMETERS
  profile_number  : profile number
  subs_id :Subscription id. 
  ipv6_prefix_delegation : pointer to store ipv6_prefix_delegation


DEPENDENCIES
  ds_umts_pdp_registry_init_flag should be set(we should have been initialized)

RETURN VALUE
  DS_UMTS_PDP_SUCCESS              : the operation completed successfully
  DS_UMTS_PDP_NOT_INITIALIZED      : Registry API library not initialized
  DS_UMTS_PDP_INVALID_PROFILE_NUM  : Invalid profile number
  DS_UMTS_PDP_ACCESS_ERROR         : Error in accessing the file system

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype 
  ds_umts_get_pdp_profile_ipv6_prefix_delegation_internal
(
  uint16                profile_number,
  ds_umts_pdp_subs_e_type              subs_id,
  boolean               *ipv6_delegation_flag
);


/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_CACHE_INDEX

DESCRIPTION
This function loops through the cache and returns the cache index based on 
profile number. 
 
PARAMETERS 
profile_number -   profile number passed
subs_id        -   subscription id 
 
DEPENDENCIES
  None

RETURN VALUE
  int8 cache_index
  Return a cache index of - 1 if no free cache index is found.
 
SIDE EFFECTS
  None
===========================================================================*/
int8 ds_3gpp_profile_cache_get_cache_index
(
	 ds_profile_db_profile_num_type  profile_number,
         ds_umts_pdp_subs_e_type subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_CACHE_INDEX_FOR_DDS

DESCRIPTION
This function loops through the cache and returns the cache index based on 
profile number. 
 
PARAMETERS 
profile_number -   profile number passed
 
DEPENDENCIES
  None

RETURN VALUE
  int8 cache_index
  Return a cache index of - 1 if no free cache index is found.
 
SIDE EFFECTS
  None
===========================================================================*/
int8 ds_3gpp_profile_cache_get_cache_index_for_dds
(
	 ds_profile_db_profile_num_type  profile_number
);



/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_VALID_PROFILE_NUMBERS

DESCRIPTION
This function gets the all valid 3gpp profile_numbers from the cache  
if the context_flag is valid 


PARAMETERS
  subs_id            - ds_umts_pdp_subs_e_type subscription id for profiles 
  profile_number_arr - uint8 array to fill the valid profiles
  arr_size           - uint array size profile_number_arr

RETURN VALUE
  valid number of profile present in the cache

  @dependencies 
  caller function need to make sure the memory is already allocated as described by arr_size
===========================================================================  */

uint8  ds_3gpp_profile_cache_get_valid_profile_numbers
(
  ds_umts_pdp_subs_e_type subs_id,
  uint8  *profile_number_arr, 
  uint8  arr_size
);

/*===========================================================================
FUNCTION DS_3GPP_PROFILE_CACHE_GET_VALID_PROFILE_NUMBERS_FOR_DDS

DESCRIPTION
This function gets the all valid 3gpp profile_numbers from the cache  
if the context_flag is valid 


PARAMETERS
  profile_number_arr - uint8 array to fill the valid profiles
  arr_size           - uint array size profile_number_arr

RETURN VALUE
  valid number of profile present in the cache

  @dependencies 
  caller function need to make sure the memory is already allocated as described by arr_size
===========================================================================  */

uint8  ds_3gpp_profile_cache_get_valid_profile_numbers_for_dds
(
  uint8  *profile_number_arr, 
  uint8  arr_size
);


/*===========================================================================
FUNCTION DS_UMTS_PDP_CHECK_PROFILE_NUMBER

DESCRIPTION
This function checks the validity of profile number. It checks if the profile
number and profile family are in valid range and profile family is defined.
PARAMETERS
  profile_number - profile number passed. This profile number is a two byte
                   number. First byte is the profile family and the second
                   byte is the actual profile number.

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None
===========================================================================*/
ds_umts_pdp_profile_status_etype ds_umts_pdp_check_subscription_id
(
  ds_umts_pdp_subs_e_type              subs_id
);



#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || FEATURE_DATA_LTE */

#endif /* DSPDPREGINT_H */
