/*===========================================================================

                           P S _ I P _ A D D R . C

DESCRIPTION
This file contains IP functions that are common to both IPv4 and IPv6.

Copyright (c) 1995-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_ip_addr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/28/15    skc    Fixed incorrect preferred lifetime for private address
                   when preferred lifetime is infinit
04/09/14    ash    Added Prefix Delegation support.
12/10/12    ssh    NV provisioning for IPv6 MTU
07/27/12    ash    Added support for IPv6 Hybrid Architecture.
03/21/11    op     Data SU API cleanup
03/26/09    pp     CMI De-featurization.
05/07/07    mct    IPv6 Neighbor Discovery
12/06/06    mct    Updated conditions under which to delete priv shared addrs
                   Also priv unique addrs are now application unique.
09/08/06    mct    Created file.

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"

#include "ps_iface.h"
#include "ps_in.h"
#include "ps_route.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6_sm.h"
#include "ps_iface_addr_v6.h"
#include "ps_ifacei_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_iface_ipfltr.h"


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
uint16  ip6_default_mtu = 0;
#ifdef FEATURE_DATA_PS_DHCPV6
boolean dhcp6_prefix_delegation_enable = 0;
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================

                           INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_PS_IPV6


/*===========================================================================
FUNCTION PS_IP_ADDR_IPV6_PRIV_ADDR_INC_REF_CNT_EX()

DESCRIPTION
  This function increments the ref cnt of the IPv6 private address. For
  private unique addresses this ensures that the reference count is currently
  0. If it is greater than 0 it implies another socket is currently bound to
  the address and will fail the subsequent bind. This starts the deprecated
  lifetime timer if the ref_cnt goes from 0 to 1.

PARAMETERS
  ip_addr_ptr:   Pointer to the ip address
  incl_ext_addr: Increment the ref count even if the provided address is
                 external address.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex
(
  struct ps_in6_addr *ip_addr_ptr,
  boolean             incl_ext_addr
)
{
  ps_ifacei_v6_addr_type  *v6_addr_ptr = NULL;
  ps_iface_type           *ps_iface_ptr = NULL;
  int64                   pref_lifetime = 0;
  int64                   lifetime = 0;
  priv_ext_lifetimes_type *priv_ext_lifetimes_ptr = NULL;
  struct ps_in6_addr       v6_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ip_addr_ptr == NULL)
  {
    LOG_MSG_ERROR_1("NULL IPv6 address ptr %p", ip_addr_ptr);
    ASSERT(0);
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if(ps_iface_find_ipv6_addr(ip_addr_ptr, &v6_addr_ptr, &ps_iface_ptr))
  {
    if (NULL == ps_iface_ptr)
    {
      LOG_MSG_ERROR_0 ("Cant find iface for IP addr");
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }

    /*---------------------------------------------------------------------
      Nothing to be done for external address.
    ---------------------------------------------------------------------*/
    if(TRUE == incl_ext_addr && 
       IPV6_ADDR_TYPE_EXTERNAL == v6_addr_ptr->addr_type)
    {    
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return 0;
    } 
    else if( v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PUBLIC &&
       v6_addr_ptr->addr_state == IPV6_ADDR_STATE_VALID)
    {
      /*---------------------------------------------------------------------
        Nothing to be done for public addresses.
      ---------------------------------------------------------------------*/
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return 0;
    }
    else if(v6_addr_ptr->addr_state != IPV6_ADDR_STATE_VALID)
    {
      /*---------------------------------------------------------------------
         Address must be in valid state, don't allow binds to deprecated 
         addresses.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("Can't bind to an IPv6 address in state %d!",
                      v6_addr_ptr->addr_state);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }
    /*-----------------------------------------------------------------------
      Address has been located. Only allow reference counts on privacy 
      addresses.
    -----------------------------------------------------------------------*/ 
    if( (v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_UNIQUE) ||
        (v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_SHARED))
    {
      /*-------------------------------------------------------------------
        Increment the reference count of the address and start the
        preferred lifetime timer. If it's already been started do nothing.
      -------------------------------------------------------------------*/
      if(ps_timer_is_running(v6_addr_ptr->unused_addr_timer_handle))
      {
        /*-----------------------------------------------------------------
          Cancel the unused activity timer if it's running.
        -----------------------------------------------------------------*/
        (void)ps_timer_cancel(v6_addr_ptr->unused_addr_timer_handle);
      }
 
      v6_addr_ptr->ref_cnt++;

     /*---------------------------------------------------------------------
        Add v6 dst based filter associated with private ip address
      ---------------------------------------------------------------------*/    
      if (v6_addr_ptr->ref_cnt == 1)
      {
        v6_addr.ps_s6_addr64[0] = v6_addr_ptr->prefix;
        v6_addr.ps_s6_addr64[1] = v6_addr_ptr->iid;

        v6_addr_ptr->hw_fltr_handle =
          ps_iface_ipfltr_add_v6_dst_fltr(ps_iface_ptr, &v6_addr);
        if (PS_IFACE_IPFLTR_INVALID_HANDLE == v6_addr_ptr->hw_fltr_handle)
        {
          DATA_ERR_FATAL("ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex() : Invalid NULL Hardware hndl");
        }
        LOG_MSG_INFO1_1("ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex : Added fltr "
                        "with handle 0x%x to hw", v6_addr_ptr->hw_fltr_handle);
      }

      if(!ps_timer_is_running(v6_addr_ptr->pref_lifetimer_handle))
      {
        pref_lifetime = ps_timer_remaining(
          ps_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->
            pref_lifetimer_handle);
        priv_ext_lifetimes_ptr = ip6_sm_get_priv_ext_lifetimes();
		
       /*----------------------------------------------------------------
         When the preferred lifetime is infinit, the preferred life timer
         for public address will not be started. So the pref_lietime will 
         always be 0 in that case. Following logic is to ensure that the 
         preferred life timer for private address is always started even 
         when it is infinit
        ---------------------------------------------------------------*/
        lifetime = MIN((pref_lifetime == 0 ? 0xFFFFFFFFUL : pref_lifetime),
            (((int64) priv_ext_lifetimes_ptr->pref_lifetime_timer) * 1000) - 5);

        (void)ps_timer_start(v6_addr_ptr->pref_lifetimer_handle, lifetime);
      }
    }
    else
    {
      IPV6_ADDR_MSG(ip_addr_ptr->ps_s6_addr64);
      LOG_MSG_ERROR_2("BIND FAIL: Incorrect address type %x or state %x",
                      v6_addr_ptr->addr_type, v6_addr_ptr->addr_state);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;

    }
  }
  else
  {  
    IPV6_ADDR_MSG(ip_addr_ptr->ps_s6_addr64);
    LOG_MSG_ERROR_0("BIND FAIL: Failed to find specified IP address"); 
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
}


/*===========================================================================
FUNCTION PS_IP_ADDR_IPV6_PRIV_ADDR_INC_REF_CNT()

DESCRIPTION
  This function increments the ref cnt of the IPv6 private address. For
  private unique addresses this ensures that the reference count is currently
  0. If it is greater than 0 it implies another socket is currently bound to
  the address and will fail the subsequent bind. This starts the deprecated
  lifetime timer if the ref_cnt goes from 0 to 1.

PARAMETERS
  ip_addr_ptr:   Pointer to the ip address

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ip_addr_ipv6_priv_addr_inc_ref_cnt
(
  struct ps_in6_addr *ip_addr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex(ip_addr_ptr, FALSE);
}


/*===========================================================================
FUNCTION PS_IP_ADDR_IPV6_PRIV_ADDR_DEC_REF_CNT()

DESCRIPTION
  This function decrements the ref count of a private IPv6 address. If the 
  reference count goes to 0 the privacy address is deleted.

PARAMETERS
  ip_addr_ptr:   Pointer to the ip address

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ip_addr_ipv6_priv_addr_dec_ref_cnt
(
  struct ps_in6_addr *ip_addr_ptr
)
{
  ps_ifacei_v6_addr_type  *v6_addr_ptr = NULL;
  ps_iface_type           *ps_iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ip_addr_ptr == NULL)
  {
    LOG_MSG_ERROR_1("NULL IPv6 address ptr %p", ip_addr_ptr);
    ASSERT(0);
    return -1;
  }


  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if(ps_iface_find_ipv6_addr(ip_addr_ptr, &v6_addr_ptr, &ps_iface_ptr))
  {
    /*-----------------------------------------------------------------------
      Address has been located. Only allow reference counts on privacy
      addresses. If it is a bind to a public IPv6 address, do nothing.
    -----------------------------------------------------------------------*/
    if(v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PUBLIC ||
       v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_EXTERNAL )
    {
      /*---------------------------------------------------------------------
        Nothing to be done for public and EXTERNAL addresses.
      ---------------------------------------------------------------------*/
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return 0;
    }
    
    if( ((v6_addr_ptr->addr_state == IPV6_ADDR_STATE_VALID) ||
         (v6_addr_ptr->addr_state == IPV6_ADDR_STATE_DEPRECATED)) && 
        ((v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_UNIQUE) ||
        (v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_SHARED)) )
    {
      if(v6_addr_ptr->ref_cnt > 0)
      {
        v6_addr_ptr->ref_cnt--;
      }
      else
      {
        LOG_MSG_ERROR_0("Trying to unbind to an addr that should already be freed.");
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return -1;
      }
    
      if(v6_addr_ptr->ref_cnt == 0)
      {
        /*-------------------------------------------------------------------
          Delete v6 dst based filter associated with private ip address
        -------------------------------------------------------------------*/
        if (PS_IFACE_IPFLTR_INVALID_HANDLE != v6_addr_ptr->hw_fltr_handle)
        {
          ps_iface_ipfltr_delete_v6_dst_fltr(v6_addr_ptr->hw_fltr_handle);
          LOG_MSG_INFO1_1("ps_ip_addr_ipv6_priv_addr_dec_ref_cnt : Deleted fltr "
                          "with handle 0x%x", v6_addr_ptr->hw_fltr_handle);
          v6_addr_ptr->hw_fltr_handle = PS_IFACE_IPFLTR_INVALID_HANDLE;
        }

        /*---------------------------------------------------------------------
          Delete the private IPv6 address. Always delete unique addresses if
          ref_cnt is zero. Only delete shared addresses if both ref_cnt is 0
          and state is deprecated, as multiple apps could have requested the
          addr, but not binded yet.
        ---------------------------------------------------------------------*/
        if( (v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_SHARED &&
             v6_addr_ptr->addr_state == IPV6_ADDR_STATE_DEPRECATED) ||
            (v6_addr_ptr->addr_type == IPV6_ADDR_TYPE_PRIV_UNIQUE))
        {
          (void)ps_iface_delete_priv_ipv6_addr(ps_iface_ptr, v6_addr_ptr);
        }
      }
    }
  }  
  else
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
}


/*===========================================================================
FUNCTION PS_IP_ADDR_GET_IP6_DEFAULT_MTU()

DESCRIPTION
  This function returns the configured default IPv6 MTU

PARAMETERS
  None

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_ip_addr_get_ip6_default_mtu
(
  void
)
{
  uint16 mtu;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mtu = MAX( ip6_default_mtu, DEFAULT_IP6_DATA_UNIT );
  return mtu;
}
#endif /* FEATURE_DATA_PS_IPV6 */
