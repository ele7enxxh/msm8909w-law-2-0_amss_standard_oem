#ifndef DS3GPP_RT_ACL_H
#define DS3GPP_RT_ACL_H
/*===========================================================================


            U M T S   S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
     F O R   R O U T I N G   V I A   T H E   U M T S   I N T E R F A C E

GENERAL DESCRIPTION
  The UMTS/GPRS specific routing ACL is used to enable policy and address
  based routing across the UMTS interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10    asn    Support for Arbitration
07/20/03    usb    Added new function declarations used by ACL.
11/13/02    rc     Added multimode support for GPRS.
08/30/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_acl.h"
#include "ds3gmgr.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ds3gpp_rt_acl );

ACL_POST_PROC_DEF( ds3gpp_rt_acl_post_proc );

ACL_IS_AVAILABLE_DEF2( ds3gpp_rt_is_pref_sys );

ACL_IS_AVAILABLE_DEF( ds3gpp_rt_is_iface_available );

ACL_CFG_SUPPORTED_DEF( ds3gpp_rt_is_cfg_supported );
ACL_CFG_SUPPORTED_DEF2( ds3gpp_rt_is_cfg_supported2 );
ACL_CFG_SHAREABLE_DEF( ds3gpp_rt_is_cfg_shareable );

ACL_DEF( ds3gpp_rt_acl_emergency_calls );
ACL_POST_PROC_DEF( ds3gpp_rt_acl_emergency_calls_post_proc );

/*===========================================================================
FUNCTION ds3gpp_rt_is_profile_in_use

DESCRIPTION
  Check if profile number passed in is already in a call.
  
PARAMETERS  
  uint32                   pdp_profile_num
  acl_policy_info_type   * policy_info_ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE/FALSE  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds3gpp_rt_is_profile_in_use
( 
  uint32                   pdp_profile_num,     /* PDP profile number*/
  acl_policy_info_type   * policy_info_ptr      /* Policy Information */
);

/*===========================================================================
FUNCTION DS3GPP_GET_POLICY_APN_NAME

DESCRIPTION
  This function gets APN name from the given acl policy.

PARAMETERS  
  *policy_info_ptr    - Policy information
  *apn_name_ptr       - ptr to the apn name holder for output

DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : APN name is successfully retrieved
  FALSE: Failed to get APN name
  
SIDE EFFECTS 
  None.
 ===========================================================================*/
boolean ds3gpp_get_policy_apn_name
( 
  acl_policy_info_type       *policy_info_ptr,
  ds3g_apn_name_type         *apn_name_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_RT_ACL_IS_CLAT_SUPPORTED

DESCRIPTION
  This function returns the CLAT_SUPPORTED field for the passed profile or
  resolved profile passed in Policy

PARAMETERS
  arg_val_ptr - IOCTL payload pointer
  subs_id     - subscription id
  ps_error    - incase any error occured, details of it

DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
       DS_EFAULT   if NULL pointers passed.
       DS_ENETDOWN if PDN contexts are not valid.

SIDE EFFECTS
  None.
===========================================================================*/
int ds_3gpp_rt_acl_is_clat_supported
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
);

#endif    /* DSUMTS_PS_RT_ACL_H */



