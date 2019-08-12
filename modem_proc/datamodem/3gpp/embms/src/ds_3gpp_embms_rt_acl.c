/*==============================================================================

    E M B M S S P E C I F I C   M O D E   H A N D L E R

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E  e M B M S   I N T E R F  A C E

GENERAL DESCRIPTION
  The embms specific routing ACL is used to enable policy and address
  based routing across the embms interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/src/ds_3gpp_embms_rt_acl.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/11    hs     Created file/Initial version.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_EMBMS
#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_iface.h"
#include "ds_3gppi_utils.h"

#include "ds_3gpp_embms_rt_acl.h"
/*===========================================================================

                        FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_EMBMS_RT_IS_CFG_SUPPORTED2

DESCRIPTION
   This function checks to see if the configuration requested can be 
   supported by the specified interface ie can the interface be aciveted
   using the specified configuration.
  
PARAMETERS  
  *this_if_ptr        - Ptr to the interface
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information

   
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE : config can be supported
  FALSE: config cannot be supported
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_CFG_SUPPORTED_DEF2( ds_embms_rt_is_cfg_supported2 )
{
  ps_iface_type *if_ptr = (ps_iface_type *) this_if_ptr;
  boolean        ret_val = TRUE;
  /*----------------------------------------------------------------------*/

  if( (ps_iface_state(if_ptr) != IFACE_DOWN) ||
      PS_IFACE_IS_IN_USE(if_ptr) )
  {
    if (!PS_IFACE_IS_SUBS_ID_MATCH(if_ptr))
    {
      *reason_for_deny = ACL_DENY_REASON_IFACE_AND_POL_FAMILY_MISMATCH;
       DS_EMBMS_MSG2_HIGH("Requested ps subs id: %d and "
                         "iface subs id: %d do not match", 
                         policy_info_ptr->subs_id, 
                         PS_IFACE_GET_SUBS_ID(if_ptr));
       ret_val = FALSE;
    }
    else
    {
       DS_EMBMS_MSG2_HIGH("Requested ps subs id: %d and "
                         "iface subs id: %d match", 
                         policy_info_ptr->subs_id, 
                         PS_IFACE_GET_SUBS_ID(if_ptr));
    }
  }
  /* EMBMS IFACE should be denied if it is not requested specifically in policy info  */

  if (policy_info_ptr->iface.info.name != (dss_iface_name_enum_type)EMBMS_IFACE)
  {
    DS_EMBMS_MSG0_HIGH("Requested IFACE name is not EMBMS, deny");
    *reason_for_deny = ACL_DENY_REASON_IFACE_MISMATCH;
    ret_val =  FALSE;
  }
  return ret_val;  
} /* ACL_CFG_SUPPORTED_DEF2( ds_embms_rt_is_cfg_supported2 ) */


/***  eMBMS ACL ***/
ACL_DEF( ds_3gpp_embms_rt_acl )
ACL_START
  PASS_QUICK( ACL_DEFAULT_CLASS, IS_POL_INFO_NULL() );
  DENY( DIFFERENT_IFACE_NAME_REQUIRED() );
  DENY( DIFFERENT_IFACE_ID_REQUIRED() );
  PASS( ACL_UP_PREF_CLASS, IFACE_UP_PREFERRED_AND_UP() );
  DENY_WITH_ANY_REASON( SPECIFIED_CONFIG_NOT_SUPPORTED2(ds_embms_rt_is_cfg_supported2) );
  PASS( ACL_IFNAME_CLASS, REQUIRED_IFACE_TYPE_IS( EMBMS_IFACE ) );
  PASS( ACL_DEFAULT_CLASS, ALL );
ACL_END

#endif /* FEATURE_DATA_EMBMS */
