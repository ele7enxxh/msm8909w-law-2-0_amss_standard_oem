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

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ds_rmnet_sio_rt_acl.h"
#include "ps_iface.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds_Utils_DebugMsg.h"

/* <EJECT> */ 
/*===========================================================================

                        FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

/***  WWAN ACL ***/
ACL_DEF( rmnet_sio_rt_acl )
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
FUNCTION RMNET_SIO_RT_ACL_POST_PROC

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
ACL_POST_PROC_DEF( rmnet_sio_rt_acl_post_proc )
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
    LOG_MSG_INFO2_0("Iface pointer NULL!");
  }else
  {
    LOG_MSG_INFO2_2("Iface 0x%x::%d post proc",
                    if_ptr->name, if_ptr->instance);
  }

  /*-------------------------------------------------------------------------
     If an UP interface is selected by routing, this function is anyway
     called even if its already in use
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_PS_IPV6

#if 0
  /*-------------------------------------------------------------------------
    FUTURE:  
      When v6 laptop calls are supported on Rm interface, this function 
      will be called after routing lookup succeeds.  

      So be sure to preclude any invalid combinations (e.g. 
      - if v6 Rm i/f can't be used while v4 Rm up, make routing lookup on v6
        fail by setting v6 iface in use
      - if v6 Rm i/f can be used while v4 Rm up, but only for interface 
        bridged data, make sure lookup fails when source is sockets layer
        (unless 'is_routeable' i.e. routed via IP filtering) 
  -------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v6(if_ptr))
  {
    other_if_ptr = RMNET_GET_PS_IFACE_IPV4_PTR(RMNET_SIO_DEF_PKT_INSTANCE);
  }
  else if(ps_iface_addr_family_is_v4(if_ptr))
  {
    other_if_ptr = RMNET_GET_PS_IFACE_IPV6_PTR(RMNET_SIO_DEF_PKT_INSTANCE);
  }
  else
  {
    ASSERT(0);
  }

  if(!PS_IFACE_IS_IN_USE(other_if_ptr))
  {
    /*-----------------------------------------------------------------------
      If the routing is for lookup only don't block the iface, this allows
      other clients to use the iface if they intend to do so. Else mark the
      iface state appropriately.
    -----------------------------------------------------------------------*/
    if(!(policy_info_ptr->lookup_only))
    {
      PS_IFACE_SET_IN_USE(other_if_ptr);
      PS_IFACE_RESET_LOOKED_UP(other_if_ptr);

      if (policy_info_ptr->is_sock_orig)
      {
        PS_IFACE_SET_SHAREABLE(other_if_ptr);
      }
      else
      {
        PS_IFACE_RESET_SHAREABLE(other_if_ptr);
      }
    } /* if(not just doing lookup */
  }
#endif /* 0 */

#endif /* FEATURE_DATA_PS_IPV6 */
} /* rmnet_sio_rt_acl_post_proc() */

