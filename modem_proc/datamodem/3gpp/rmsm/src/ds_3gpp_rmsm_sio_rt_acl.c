/*===========================================================================

                   R M   N E T W O R K   I N T E R F A C E   

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
       F O R   R O U T I N G   V I A   T H E   R M N E T   I N T E R F A C E

GENERAL DESCRIPTION
  The Rm network interface specific routing ACL is used to enable policy and address
  based routing across the 1x interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   pp      CMI De-featurization.
03/14/05   jd      Commented out v6 post-processing logic for now
03/14/05    ks     Removed featurization
03/08/05   ks      Changed name from ds_netsio_rt_acl.c
08/30/02   usb/jd      Created Module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_3gpp_rmsm_sio_rt_acl.h"
#include "ds_3gppi_utils.h"
#include "ps_iface.h"


/* <EJECT> */ 
/*===========================================================================

                        FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

/***  WWAN ACL ***/
ACL_DEF( ds_3gpp_rmsm_sio_rt_acl )
ACL_START
  PASS_QUICK( ACL_DEFAULT_CLASS, IS_POL_INFO_NULL() );
  DENY( DIFFERENT_IFACE_NAME_REQUIRED() );
  DENY( DIFFERENT_IFACE_ID_REQUIRED() );
  // DENY( IFACE_NOT_AVAILABLE(wwan_rt_is_iface_available) );  // fix later
  DENY( IFACE_UP_REQUIRED_BUT_NOT_UP() );
  DENY( NOT( IFACE_AND_POL_FAMILY_MATCH() ) );
  DENY( NOT( IS_PKT_INFO_NULL() OR IFACE_AND_PKT_FAMILY_MATCH() ) );
  PASS( ACL_IFNAME_CLASS, IFACE_ID_MATCH() );
  // PASS( ACL_IFNAME_CLASS, REQUIRED_IFACE_TYPE_IS( SIO_IFACE ) );
  // PASS( ACL_UP_PREF_CLASS, IFACE_UP_PREFERRED_AND_UP() );
  // PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( RM_GROUP ) );
  // PASS( ACL_IFGROUP_CLASS, REQUIRED_IFACE_TYPE_IS( ANY_DEFAULT_GROUP ) );
  PASS_DEFAULT( ACL_DEFAULT_CLASS );
ACL_END


/*===========================================================================
FUNCTION DS_3GPP_RMSM_SIO_RT_ACL_POST_PROC

DESCRIPTION
   This function performs the post routing lookup actions.  This funtion is 
   called for the interface which is picked up as the most preferred interface 
   during lookup based upon client requirements (policy) and other system 
   parameters.
   For SIO network interface there is nothing to do right now.
  
PARAMETERS  
  *ipfilter_info_ptr  - Ip filter information
  *policy_info_ptr    - Policy information
  *this_if_ptr        - Ptr to the interface
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
ACL_POST_PROC_DEF( ds_3gpp_rmsm_sio_rt_acl_post_proc )
{
  ps_iface_type *if_ptr;
#ifdef FEATURE_DATA_PS_IPV6
#if 0
  ps_iface_type *other_if_ptr=NULL;
#endif
#endif /* FEATURE_DATA_PS_IPV6 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if_ptr = (ps_iface_type *) this_if_ptr;

  if( NULL == if_ptr )
  {
    DS_3GPP_MSG0_ERROR("Iface pointer NULL!");
  }else
  {
    DS_3GPP_MSG2_MED("Iface 0x%x::%d post proc", if_ptr->name, if_ptr->instance);
  }

} /* ds_3gpp_rmsm_sio_rt_acl_post_proc() */


