/*===========================================================================

                      P S _ I F A C E _ I O C T L. C

DESCRIPTION
  This file contains functions used by various modules to access
  network interface.

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ioctl.c_v   1.8   13 Feb 2003 14:15:20   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_ioctl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/09/14    ash    Added Prefix Delegation support.
06/21/13    ash    Support MTU upto 2000
10/03/11    ash    Added new ioctl PS_IFACE_GET_IPV6_ROUTER_LIFETIME
08/31/11    hs     Added new ioctl PS_IFACE_GET_SUBNET_MASK.
08/30/11    hs     Added new ioctl PS_IFACE_GET_GATEWAY_ADDR.
08/20/08    hm     Moved many DSS ioctls from dss_iface_ioctl to here.
08/15/05    mct    Updated ps_iface_ioctl to be used only as pipe for mode
                   handler specific operations.
07/28/05    rt     Added new ioctl's DSS_IFACE_IOCTL_GET_IFACE_NAME and
                   DSS_IFACE_IOCTL_GET_IP_FAMILY.
05/12/05    ks     fixed lint errors
04/16/05    ks     Added support for new ioctl, DSS_IFACE_GET_HW_ADDR
11/19/04    msr    Using internal ps_iface macros instead of external ones.
10/30/04    msr    Moved handling of DSS_IFACE_IOCTL_GET_ALL_IFACES to
                   dss_iface_ioctl().
11/02/04    vp     Removed the handling of DSS_IFACE_GET_PHYS_LINK_STATE and
                   placed in dss_iface_ioctl.c.
10/13/04    vp     Removed the byte order conversion of v4 addresses.
08/02/04    ak     When getting DNS addrs, call functions in ps_iface to
                   retrieve.
07/30/04    vp     Changes due to consolidation of MTU and net_info structure
                   in ps_iface.
07/11/04    aku    Moved processing of REG and DEREV iface events to
                   dss_iface_ioctl.c
06/14/04    mct    Added IOCTL support for getting V6 primary and secondary DNS
                   addresses and for retrieving the v6 and generic ip address.
06/11/04    vp     Changes for representation of IP addresses as struct ps_in_addr
                   or struct ps_in6_addr in ps_ip_addr_type.
04/30/04    mct    Fixed some lint errors.
02/03/04    aku    Added support for Logical ifaces.
10/03/03    jd     enabled IP filtering ioctl's
08/28/03    ss     Updated to use ps_iface_id_type instead of name, instance
05/05/03    aku    Aded support for DSS_IFACE_IOCTL_GET_ALL_IFACES
03/31/03    aku    Added NULL checks for argval_ptr before deferencing it.
03/19/03    aku    Set the IP address type to IPV4 for secondary DNS address.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
02/07/03    usb    Added ioctls for reg and dereg inbound IP filters.
01/17/03    atp    Added argument ps_iface_ptr to iface_ioctl_f_ptr.
01/17/03    usb    Added validation of iface_ptr in ps_iface_ioctl(),
                   included file dssocki.h to remove compiler warnings
12/24/02    aku    Added support for reg/dereg event callbacks
11/24/02    aku    Removed interface statistics IOCTL.
11/19/02    aku    Added interface statisitics IOCTL
11/11/02    aku    Initial version.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dserrno.h"
#include "dss_iface_ioctl.h" //need to re-visit this inclusion!
#include "ps_iface_ioctl.h"
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_ifacei_utils.h"
#include "ps_aclrules.h"
#include "ps_in.h"
#include "ps_ifacei.h"
#include "ps_qos_net_initiated_req.h"

#ifdef FEATURE_DATA_PS_QOS
#include "ps_qos_spec_logging.h"
#endif /* FEATURE_DATA_PS_QOS */

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#include "ps_ifacei_addr_v6.h"
#include "ps_ip6_addr.h"
#include "dhcp6_client.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "dhcp_client.h"


#include "ds_Utils_DebugMsg.h"

#include <stringl/stringl.h>

#include "ps_dpm.h"

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
static int ps_iface_ioctli_get_all_ifaces
(
  ps_iface_type            *ps_iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  uint8                                    iface_index;
  dss_iface_ioctl_all_ifaces_type         *all_iface_info    = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argval_ptr)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  all_iface_info = (dss_iface_ioctl_all_ifaces_type *) argval_ptr;
  all_iface_info->number_of_ifaces = 0;

  /*---------------------------------------------------------------------
    Search the global iface array
  ---------------------------------------------------------------------*/
  for (iface_index = 0; iface_index < MAX_SYSTEM_IFACES; iface_index++)
  {
    /*-------------------------------------------------------------------
      Since ifaces are added sequentially and they are never deleted, as
      soon as we get a NULL ptr there is no more iface pointers in the
      list.
    -------------------------------------------------------------------*/
    if (global_iface_ptr_array[iface_index] == NULL)
    {
      break;
    }
    all_iface_info->ifaces[iface_index] =
      PS_IFACE_GET_ID(global_iface_ptr_array[iface_index]);
    all_iface_info->number_of_ifaces++;
  }

  return 0;

}

static int ps_iface_ioctli_mode_handler_cback
(
  ps_iface_type            *ps_iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_type                           *assoc_iface_ptr = NULL;
  int                                      ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Call the mode handler registered ioctl callback.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Handle logical ifaces.
  -------------------------------------------------------------------------*/
  if (PS_IFACEI_IS_LOGICAL(ps_iface_ptr))
  {
    /*-------------------------------------------------------------------
      Go through the chain of ifaces and execute the first one which
      has registered an ioctl hdlr
    -------------------------------------------------------------------*/
    assoc_iface_ptr = ps_iface_ptr;
    while(assoc_iface_ptr != NULL)
    {
      if (assoc_iface_ptr->iface_ioctl_f_ptr != NULL)
      {
        ret_val = assoc_iface_ptr->iface_ioctl_f_ptr(assoc_iface_ptr,
                                                     ioctl_name,
                                                     argval_ptr,
                                                     ps_errno);
        break;
      }

      assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(assoc_iface_ptr);
    }

    /*-------------------------------------------------------------------
      If after looping through the chain of associated ifaces, we do not
      find an appropriate ioctl function handler for the specified IOCTL,
      return DS_EINVAL error code.
    -------------------------------------------------------------------*/
    if (assoc_iface_ptr == NULL)
    {
      LOG_MSG_ERROR_3("ps_iface_ioctli_mode_handler_cback(): "
                      "Unsupported IOCTL 0x%x on iface 0x%x:%d",
                      ioctl_name, ps_iface_ptr->name, ps_iface_ptr->instance);
      *ps_errno = DS_EINVAL;
      ret_val = -1;
    }

    return ret_val;
  }

  /*-------------------------------------------------------------------------
    Handle non-logical ifaces.
  -------------------------------------------------------------------------*/
  if (ps_iface_ptr->iface_ioctl_f_ptr != NULL &&
      !PS_IFACEI_IS_LOGICAL(ps_iface_ptr))
  {
    return ps_iface_ptr->iface_ioctl_f_ptr(ps_iface_ptr,
                                           ioctl_name,
                                           argval_ptr,
                                           ps_errno);
  }

  /*-------------------------------------------------------------------------
    Iface is neither logical nor has an associated IOCTL callback.
  -------------------------------------------------------------------------*/
  LOG_MSG_ERROR_3("ps_iface_ioctli_mode_handler_cback(): "
                  "Inv ioctl %d, iface 0x%x:%d",
                  ioctl_name, ps_iface_ptr->name, ps_iface_ptr->instance);

  *ps_errno = DS_EINVAL;
  return -1;

} /* ps_iface_ioctli_mode_handler_cback() */



/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_IOCTL()

DESCRIPTION
  This function performs various operations on the given ps interface.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  ps_iface_type             - ps_iface on which the specified operations
                              is to be performed

  dss_iface_ioctl_type      - The operation name

  void*                     - Pointer to operation specific structure

  sint15*                   - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EBADF - Returned by dss_iface_ioctl() if the
                              specified id_ptr is invalid (i.e. id_ptr does
                              not map to a valid ps_iface_ptr).

                              DS_EINVAL - Returned by dss_iface_ioctl() when
                              the specified IOCTL does not belong to the
                              common set of IOCTLs and there is no IOCTL mode
                              handler registered for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific, for example,
                              GO_DORMANT).

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but dss_iface_ioctl() or a mode handler
                              encounters an error while executing the IOCTL..
                              For instance, if the 1X interface cannot
                              "GO_DORMANT" it would return this error.

                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int ps_iface_ioctl
(
  ps_iface_type            *ps_iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ip_addr_type                              *ip_addr                = NULL;
  ip_addr_type                              *gateway_ip_addr        = NULL;
  ip_addr_type                              *ipv4_subnet_mask       = NULL;
  ps_ip_addr_type                            ret_ip_addr;
  uint32                                    *mtu                    = NULL;
  ps_iface_state_enum_type                  *iface_state_ptr        = NULL;
  ps_iface_state_enum_type                   iface_state            = IFACE_STATE_INVALID;
  ip_addr_type                               prim_dns_addr, sec_dns_addr;
  ps_iface_ioctl_hw_addr_type               *iface_hw_addr          = NULL;
  uint8                                      hw_addr_len;
  ps_iface_name_enum_type                   *iface_name_ptr         = NULL;
  ps_iface_addr_family_type                 *family_ptr             = NULL;
  boolean                                   *laptop_call_status_ptr = NULL;
  uint8					                       *hw_addr;

  void                                    *dhcp_client_handle;

#ifdef FEATURE_DATA_PS_IPV6
  dss_iface_ioctl_priv_ipv6_addr_type     *priv_addr;
  ps_ipv6_iid_params_type                  iid_params;
  dss_iface_ioctl_get_all_v6_prefixes_type *all_v6_prefixes = NULL;
  ps_iface_prefix_info_type                prefix_info[MAX_IPV6_PREFIXES];
  int                                      result;
#endif /* FEATURE_DATA_PS_IPV6 */
  uint8                                    index;
  dss_iface_ioctl_get_all_dns_addrs_type  *get_dns_addrs_ptr = NULL;
  dss_iface_ioctl_domain_name_search_list_type *domain_name_search_list_info = NULL;

  dss_iface_ioctl_sip_serv_addr_info_type         *sip_serv_addr_info;
  dss_iface_ioctl_sip_serv_domain_name_info_type  *sip_serv_domain_name_info;

  ps_iface_ioctl_data_path_info_type   * data_path_info_ptr;
  int                                    is_data_path_software;
  ps_iface_op_reserved_pco_type        * operator_reserved_pco_info_ptr = NULL;
  uint8                                  unbridge_mode_ref_cnt = 0;
  uint8                                  comp_iface_ref_cnt    = 0;
  ps_iface_type                         * comp_iface_ptr    = NULL;
  ps_iface_type                        * base_iface_ptr    = NULL;
  ps_iface_ioctl_pdn_teardown_config_type * pdn_teardown_config_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_ioctl(): NULL args");
    return -1;
  }

  *ps_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    GET_ALL_IFACES is a special case, iface pointer need not be valid here.
  -------------------------------------------------------------------------*/
  if (PS_IFACE_IOCTL_GET_ALL_IFACES == ioctl_name)
  {
    return ps_iface_ioctli_get_all_ifaces (ps_iface_ptr, argval_ptr, ps_errno);
  }

  if ( !(PS_IFACE_IS_VALID(ps_iface_ptr)) )
  {
    LOG_MSG_ERROR_2("ps_iface_ioctl(): "
                    "IOCTL %d failed, invalid iface 0x%p",
                    ioctl_name, ps_iface_ptr);
    *ps_errno = DS_EBADF;
    goto bail;
  }

  LOG_MSG_INFO2_3 ("ps_iface_ioctl(): "
                   "IOCTL %d, iface 0x%x:%d",
                   ioctl_name, ps_iface_ptr->name, ps_iface_ptr->instance);

  /*-------------------------------------------------------------------------
    More special case: For the followign IOCTLs the iface addr family
    needs to verified.
  -------------------------------------------------------------------------*/
  switch(ioctl_name)
  {
    case PS_IFACE_IOCTL_GET_IPV4_ADDR:
    case PS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR:
    case PS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR:

      if ( !(ps_iface_addr_family_is_v4(ps_iface_ptr)) )
      {
        LOG_MSG_ERROR_0("ps_iface_ioctl(): "
                        "v4 iface required");
        *ps_errno = DS_EINVAL;
        goto bail;
      }
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case PS_IFACE_IOCTL_GET_IPV6_ADDR:
    case PS_IFACE_IOCTL_GET_ALL_V6_PREFIXES:
    case PS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR:
    case PS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR:
    case PS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR:
    case PS_IFACE_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME:

      if ( !(ps_iface_addr_family_is_v6(ps_iface_ptr)) )
      {
        LOG_MSG_ERROR_0("ps_iface_ioctl(): "
                        "v6 iface required");
        *ps_errno = DS_EINVAL;
        goto bail;
      }
    break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      break;

  } /* switch (ioctl_name) */

  /*-------------------------------------------------------------------------
    Service the IOCTL based on the ioctl name
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    case PS_IFACE_IOCTL_GET_DATA_PATH_BRIDGE_MODE:
    {
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      } 

      data_path_info_ptr = (ps_iface_ioctl_data_path_info_type *)argval_ptr;
      is_data_path_software = ps_dpm_is_data_path_software
                               (PS_IFACE_GET_BASE_IFACE(ps_iface_ptr));
                               
      LOG_MSG_INFO1_1("ps_iface_ioctl(): Data path softwrae %d",
                       is_data_path_software);                                
      if (1 == is_data_path_software)
      {
        data_path_info_ptr->data_path = IFACE_DATA_PATH_SOFTWARE;
      }
      else if (0 == is_data_path_software)
      {
        data_path_info_ptr->data_path = IFACE_DATA_PATH_HARDWARE;
      }
      else
      {
        *ps_errno = DS_EINVAL;
        goto bail;
      }
      break;
    }
    
    case PS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE:
    {
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }    
      
      iface_state = ps_iface_state(ps_iface_ptr);
            
      if (IFACE_DOWN == iface_state)
      {
        LOG_MSG_ERROR_4("ps_iface_ioctl(): IOCTL %d is not supported in "
                        "state 0x%x iface 0x%x:%d ",
                         ioctl_name,
                         iface_state,
                         ps_iface_ptr->name,
                         ps_iface_ptr->instance);
          
          *ps_errno = DS_ENETDOWN;
          goto bail;
      }
      
      data_path_info_ptr = (ps_iface_ioctl_data_path_info_type *)argval_ptr;

      #ifdef FEATURE_DATA_PS_464XLAT
        base_iface_ptr = PS_DPM_GET_BASE_IFACE(ps_iface_ptr);
      #else
        base_iface_ptr = PS_IFACE_GET_BASE_IFACE(ps_iface_ptr); 
      #endif /* FEATURE_DATA_PS_464XLAT */
      
      unbridge_mode_ref_cnt = PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT(base_iface_ptr);

      if (IFACE_DATA_PATH_HARDWARE == data_path_info_ptr->data_path)
      {
        LOG_MSG_INFO1_2("ps_iface_ioctl(): Resuming hardware path 0x%p unbridge_mode_ref_cnt %d",
                         base_iface_ptr,  unbridge_mode_ref_cnt);

        /* Check companion iface Ref Count before invoking resume API 
        1. Get companion iface from DPM 
        2. Check the companion iface ref count if it is zero then allow to resume PDN
           Note:  CR 899598 is applicable to LTE only. EHRPD/WLAN has data path SW (by default) 
           and after every HO we invoke IOCTL based on unbridge flag which makes ref count in sync*/

        comp_iface_ptr = ps_dpm_get_companion_iface(base_iface_ptr);

        if ( PS_IFACE_IS_VALID(comp_iface_ptr) )
        {
          comp_iface_ref_cnt =  PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT(comp_iface_ptr);
        }

        /* Ref count indicates number of unbridge request made on that iface.
         If it is 1 then we can allow to resume if companion iface  doesn't 
         have any obligation (ref count other then 0) */

        if ( 1 == unbridge_mode_ref_cnt )
        {
           LOG_MSG_INFO1_2("ps_iface_ioctl():comp_iface_ref_cnt %d unbridge_mode_ref_cnt %d",
                         comp_iface_ref_cnt,  unbridge_mode_ref_cnt);

          if ( 0 == comp_iface_ref_cnt )
          {
            ps_dpm_resume_hw_path_per_pdn(base_iface_ptr);
          }
 
          PS_IFACEI_SET_UNBRIDGE_MODE(ps_iface_ptr, FALSE)
        }
 
        PS_IFACEI_DEC_UNBRIDGE_MODE_REF_CNT(base_iface_ptr);        
      }
      
      if (IFACE_DATA_PATH_SOFTWARE == data_path_info_ptr->data_path)
      {
        LOG_MSG_INFO1_2("ps_iface_ioctl(): Suspending hardware path 0x%p unbridge_mode_ref_cnt %d",
                         ps_iface_ptr,unbridge_mode_ref_cnt);        
        
        if (FALSE == ps_dpm_data_path_switch_allowed(PS_DPM_DISABLE_HW_PATH_IFACE_IOCTL))
        {
          *ps_errno = DS_EINVAL;
          goto bail; 
        }
        /*if reference count is not zero data path is already in software mode
          hence no need to suspend it agian on same iface*/
        if(unbridge_mode_ref_cnt == 0)
        {
          ps_dpm_suspend_hw_path_per_pdn(base_iface_ptr);
          PS_IFACEI_SET_UNBRIDGE_MODE(ps_iface_ptr, TRUE)
        }      
        PS_IFACEI_INC_UNBRIDGE_MODE_REF_CNT(base_iface_ptr);
      }      
      break;
    }    
    /*-----------------------------------------------------------------------
      Get IPV4 addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV4_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->type = IPV4_ADDR;
      ret_ip_addr.type = IPV4_ADDR;
      ps_iface_get_addr(ps_iface_ptr,&ret_ip_addr);
      ip_addr->addr.v4 = ret_ip_addr.addr.v4.ps_s_addr;

      IPV4_ADDR_MSG(ip_addr->addr.v4);
      break;

    /*-----------------------------------------------------------------------
      Get IPV4 Primary DNS addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      ps_iface_get_v4_dns_addrs (ps_iface_ptr,&prim_dns_addr, &sec_dns_addr);
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v4 = prim_dns_addr.addr.v4;
      ip_addr->type = prim_dns_addr.type;

      IPV4_ADDR_MSG(ip_addr->addr.v4);
      break;

    /*-----------------------------------------------------------------------
      Get IPV4 Secondary DNS addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      ps_iface_get_v4_dns_addrs (ps_iface_ptr,&prim_dns_addr, &sec_dns_addr);
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v4 = sec_dns_addr.addr.v4;
      ip_addr->type = sec_dns_addr.type;

      IPV4_ADDR_MSG(ip_addr->addr.v4);
      break;

    case PS_IFACE_IOCTL_GET_ALL_DNS_ADDRS:
      get_dns_addrs_ptr = (dss_iface_ioctl_get_all_dns_addrs_type *)argval_ptr;

      if (NULL == get_dns_addrs_ptr ||
          0    == get_dns_addrs_ptr->num_dns_addrs ||
          NULL == get_dns_addrs_ptr->dns_addrs_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      ps_iface_get_all_dns_addrs (ps_iface_ptr,
                                  get_dns_addrs_ptr->dns_addrs_ptr,
                                  &(get_dns_addrs_ptr->num_dns_addrs));

      /*---------------------------------------------------------------------
        Print retrieved DNS addresses.
      ---------------------------------------------------------------------*/
      if(ps_iface_addr_family_is_v4 (ps_iface_ptr))
      {
        for(index = 0; index < get_dns_addrs_ptr->num_dns_addrs; index++)
        {
          IPV4_ADDR_MSG(get_dns_addrs_ptr->dns_addrs_ptr[index].addr.v4);
        }

      }
#ifdef FEATURE_DATA_PS_IPV6
      else if(ps_iface_addr_family_is_v6(ps_iface_ptr))
      {
        for(index = 0; index < get_dns_addrs_ptr->num_dns_addrs; index++)
        {
          IPV6_ADDR_MSG(get_dns_addrs_ptr->dns_addrs_ptr[index].addr.v6);
        }
      }
#endif /* FEATURE_DATA_PS_IPV6 */
      break;

#ifdef FEATURE_DATA_PS_IPV6

    /*------------------------------------------------------------------------
      Get IPV6 addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV6_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->type = IPV6_ADDR;
      ret_ip_addr.type = IPV6_ADDR;
      ps_iface_get_addr (ps_iface_ptr,&ret_ip_addr);
      ip_addr->addr.v6[0] = ret_ip_addr.addr.v6.ps_s6_addr64[0];
      ip_addr->addr.v6[1] = ret_ip_addr.addr.v6.ps_s6_addr64[1];

      IPV6_ADDR_MSG(ip_addr->addr.v6);
      break;

    /*------------------------------------------------------------------------
      Get all IPV6 prefixes of the interface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_ALL_V6_PREFIXES:
      all_v6_prefixes = (dss_iface_ioctl_get_all_v6_prefixes_type *)argval_ptr;
      if (all_v6_prefixes == NULL || 0 == all_v6_prefixes->num_prefixes)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      ps_iface_get_all_v6_prefixes (ps_iface_ptr,
                                    &prefix_info[0],
                                    &all_v6_prefixes->num_prefixes);

      for(index = 0; index < all_v6_prefixes->num_prefixes; index++)
      {
        memscpy(&all_v6_prefixes->prefix_info_ptr[index].prefix,
                sizeof(struct ps_in6_addr),
               &prefix_info[index].prefix,
               sizeof(struct ps_in6_addr));

        all_v6_prefixes->prefix_info_ptr[index].prefix_state =
          (dss_iface_ioctl_ipv6_addr_state_enum_type) prefix_info[index].prefix_state;
        all_v6_prefixes->prefix_info_ptr[index].prefix_len =
          prefix_info[index].prefix_len;
      }

      break;

    /*-----------------------------------------------------------------------
      Get IPV6 Primary DNS addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      ps_iface_get_v6_dns_addrs (ps_iface_ptr,&prim_dns_addr, &sec_dns_addr);
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v6[0] = prim_dns_addr.addr.v6[0];
      ip_addr->addr.v6[1] = prim_dns_addr.addr.v6[1];
      ip_addr->type = prim_dns_addr.type;

      IPV6_ADDR_MSG(ip_addr->addr.v6);
      break;

    /*-----------------------------------------------------------------------
      Get IPV6 Secondary DNS addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      ps_iface_get_v6_dns_addrs (ps_iface_ptr,&prim_dns_addr, &sec_dns_addr);
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v6[0] = sec_dns_addr.addr.v6[0];
      ip_addr->addr.v6[1] = sec_dns_addr.addr.v6[1];
      ip_addr->type = sec_dns_addr.type;

      IPV6_ADDR_MSG(ip_addr->addr.v6);
      break;

    case PS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR:
      //TODO: Register for events ???
      priv_addr = (dss_iface_ioctl_priv_ipv6_addr_type *) argval_ptr;

      if (NULL == priv_addr ||
          NULL == priv_addr->ip_addr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      /*---------------------------------------------------------------------
        This is an application initiated request to generate a new privacy
        address.
      ---------------------------------------------------------------------*/
      iid_params.app_request = TRUE;
      iid_params.is_unique   = priv_addr->iid_params.is_unique;

      result = ps_iface_generate_priv_ipv6_addr (ps_iface_ptr,
                                                 &ret_ip_addr,
                                                 &iid_params,
                                                 ps_errno);
      if (0 != result)
      {
        goto bail;
      }

      priv_addr->ip_addr->type       = ret_ip_addr.type;
      priv_addr->ip_addr->addr.v6[0] = ret_ip_addr.addr.v6.ps_s6_addr64[0];
      priv_addr->ip_addr->addr.v6[1] = ret_ip_addr.addr.v6.ps_s6_addr64[1];
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Get MTU of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_MTU:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      mtu = (uint32 *)argval_ptr;
      *mtu = PS_IFACE_GET_MTU(ps_iface_ptr);
      LOG_MSG_INFO2_1("ps_iface_ioctl(): "
                      "MTU is %d", *mtu);
      break;

    /*-----------------------------------------------------------------------
      Get op res pco of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_OP_RES_PCO:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      operator_reserved_pco_info_ptr = 
        (ps_iface_op_reserved_pco_type *)argval_ptr;
      if( 0 != ps_iface_get_operator_reserved_pco
                (ps_iface_ptr,operator_reserved_pco_info_ptr) )
      {
        *ps_errno = DS_EINVAL;
        goto bail;
      }

      break;
    
    /*-----------------------------------------------------------------------
      Get IP addr of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IP_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->type = ps_iface_get_addr_family(ps_iface_ptr);
      ret_ip_addr.type = ip_addr->type;
      ps_iface_get_addr (ps_iface_ptr, &ret_ip_addr);

      switch(ip_addr->type)
      {
        case IPV4_ADDR:
          ip_addr->addr.v4 = ret_ip_addr.addr.v4.ps_s_addr;
          IPV4_ADDR_MSG(ip_addr->addr.v4);
          break;

#ifdef FEATURE_DATA_PS_IPV6
        case IPV6_ADDR:
          ip_addr->addr.v6[0] = ret_ip_addr.addr.v6.ps_s6_addr64[0];
          ip_addr->addr.v6[1] = ret_ip_addr.addr.v6.ps_s6_addr64[1];
          IPV6_ADDR_MSG(ip_addr->addr.v6);
          break;
#endif

        default:
          ip_addr->type = IP_ADDR_INVALID;
          *ps_errno = DS_EFAULT;
          break;
      }
      break;

    /*-----------------------------------------------------------------------
      Get gateway's IP address.
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_GATEWAY_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      gateway_ip_addr = (ps_iface_ioctl_gateway_addr_type *)argval_ptr;
      ret_ip_addr.type = IP_ANY_ADDR;
      ps_iface_get_peer_addr (ps_iface_ptr, &ret_ip_addr);
      gateway_ip_addr->type = ret_ip_addr.type;

      switch(gateway_ip_addr->type)
      {
        case IPV4_ADDR:
          gateway_ip_addr->addr.v4 = ret_ip_addr.addr.v4.ps_s_addr;
          IPV4_ADDR_MSG(gateway_ip_addr->addr.v4);
          break;

#ifdef FEATURE_DATA_PS_IPV6
        case IPV6_ADDR:
          gateway_ip_addr->addr.v6[0] = ret_ip_addr.addr.v6.ps_s6_addr64[0];
          gateway_ip_addr->addr.v6[1] = ret_ip_addr.addr.v6.ps_s6_addr64[1];
          IPV6_ADDR_MSG(gateway_ip_addr->addr.v6);
          break;
#endif

        default:
          gateway_ip_addr->type = IP_ADDR_INVALID;
          *ps_errno = DS_EFAULT;
          break;
      }
      break;

      /*-----------------------------------------------------------------------
      Get IPv4 subnet mask
      -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_SUBNET_MASK:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        LOG_MSG_ERROR_0("NULL agval_ptr");
        goto bail;
      }

      ipv4_subnet_mask = (ps_iface_ioctl_subnet_mask_type *)argval_ptr;

      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

      if (!PS_IFACE_IS_VALID(ps_iface_ptr))
      {
        ASSERT(0);
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        *ps_errno = DS_EFAULT;
        LOG_MSG_ERROR_0("Invalid iface");
        goto bail;
      }

      switch(ps_iface_ptr->iface_private.state)
      {
        /*-------------------------------------------------------------------
          Return the invalid address, the subnet mask is invalid in this
          state
        -------------------------------------------------------------------*/
      case IFACE_DISABLED:
      case IFACE_DOWN:
      case IFACE_COMING_UP:
          memset(ipv4_subnet_mask, 0, sizeof(ip_addr_type));
          ipv4_subnet_mask->type = IP_ADDR_INVALID;

          LOG_MSG_ERROR_0("Subnet mask is invalid due to IFACE state");
        break;

        /*-------------------------------------------------------------------
          In following states, iface has a valid IP address, and hence a
          valid subnet mask
        -------------------------------------------------------------------*/
      case IFACE_ROUTEABLE:
      case IFACE_UP:
      case IFACE_GOING_DOWN:
      case IFACE_CONFIGURING:
          if (ps_iface_addr_family_is_v4(ps_iface_ptr))
        {
            ipv4_subnet_mask->type = IPV4_ADDR;
            ipv4_subnet_mask->addr.v4 =
              ps_iface_ptr->net_info.net_ip.v4.net_mask.ps_s_addr;
          }
          else
        {
            PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
            LOG_MSG_ERROR_2("PS_IFACE_IOCTL_GET_SUBNET_MASK is not supported "
                            "on IPv6 iface 0x%x:%d",
                            ps_iface_ptr->name, ps_iface_ptr->instance);
          *ps_errno = DS_EAFNOSUPPORT;
            goto bail;
        }

        break;

      default:
        {
        ASSERT(0);
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          LOG_MSG_ERROR_3("Unknown state %d, iface 0x%x:%d",
                          ps_iface_ptr->iface_private.state,
                          ps_iface_ptr->name,
                          ps_iface_ptr->instance);
          *ps_errno = DS_EINVAL;
          goto bail;
        }
      } /* switch(iface state) */

      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    /*-----------------------------------------------------------------------
      Get state of iface
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_STATE:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      iface_state_ptr = (ps_iface_state_enum_type *)argval_ptr;
      *iface_state_ptr = ps_iface_state (ps_iface_ptr);
      LOG_MSG_INFO2_1 ("ps_iface_ioctl(): "
                       "Iface state is %d", *iface_state_ptr);
      break;

    /*-----------------------------------------------------------------------
      Get the address of the hardware.
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_HW_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      iface_hw_addr = (ps_iface_ioctl_hw_addr_type *)argval_ptr;
      hw_addr_len   = PS_IFACE_HW_ADDR_LEN(ps_iface_ptr);

      /*-----------------------------------------------------------------------
        if HW lenght is 0, iface does not support HW Address
      -----------------------------------------------------------------------*/
      if (0 == hw_addr_len)
      {
        *ps_errno = DS_EINVAL;
        goto bail;
      }

      /*---------------------------------------------------------------------
        Validate if the size of buffer provided is enough for the hw address
      ---------------------------------------------------------------------*/
      if (iface_hw_addr->hw_addr_len < hw_addr_len)
      {
        iface_hw_addr->hw_addr_len = hw_addr_len;
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      hw_addr = PS_IFACE_HW_ADDR_PTR(ps_iface_ptr);
      if (NULL == hw_addr)
      {
        iface_hw_addr->hw_addr_len = hw_addr_len;
        *ps_errno = DS_EINVAL;
        goto bail;
      } 

      memscpy(iface_hw_addr->hw_addr, hw_addr_len, hw_addr, hw_addr_len);
      iface_hw_addr->hw_addr_len = hw_addr_len;
      break;

    /*-----------------------------------------------------------------------
      Get iface name
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IFACE_NAME:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      iface_name_ptr = (ps_iface_name_enum_type *)argval_ptr;
      if (ps_iface_ptr->name == IWLAN_3GPP_IFACE)
      {
        *iface_name_ptr = UMTS_IFACE;
      }
      else if (ps_iface_ptr->name == IWLAN_3GPP2_IFACE)
      {
        *iface_name_ptr = CDMA_SN_IFACE;
      }
      else
      {
        *iface_name_ptr = ps_iface_ptr->name;
      }
      break;

    /*-----------------------------------------------------------------------
      Get iface IP family
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_IP_FAMILY:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      family_ptr = (ps_iface_addr_family_type *) argval_ptr;
      *family_ptr = ps_iface_get_addr_family (ps_iface_ptr);
      LOG_MSG_INFO2_1("ps_iface_ioctl(): "
                      "IFACE family is %d", *family_ptr);
      break;

    /*-----------------------------------------------------------------------
      Check if iface is in laptop call
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      laptop_call_status_ptr = (boolean *) argval_ptr;
      *laptop_call_status_ptr = PS_IFACE_IS_IN_LAPTOP_CALL(ps_iface_ptr);
      break;

    /*-----------------------------------------------------------------------
      Refresh DHCP config info
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO:

      if (NULL != argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      dhcp_client_handle = 
                ps_iface_get_dhcp_stateless_client_handle (ps_iface_ptr);
      if (0 == dhcp_client_handle)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }


      if (TRUE == ps_iface_addr_family_is_v4 (ps_iface_ptr))
      {
        /* Get already configured ip_addr */
        ret_ip_addr.type = IPV4_ADDR;
        ps_iface_get_addr (ps_iface_ptr, &ret_ip_addr);
        ASSERT(IPV4_ADDR == ret_ip_addr.type);

        /* Trigger non-address config refresh */
        dhcp_client_inform(dhcp_client_handle, ret_ip_addr.addr.v4.ps_s_addr);
        break;
      }

#ifdef FEATURE_DATA_PS_IPV6
      if (TRUE == ps_iface_addr_family_is_v6 (ps_iface_ptr))
      {
        dhcp6_client_inform(dhcp_client_handle);
        break;
      }
#endif /* FEATURE_DATA_PS_IPV6 */

      LOG_MSG_ERROR_0("ps_iface_ioctl(): "
                      "Unsupported addr family");
      *ps_errno = DS_EAFNOSUPPORT;
      goto bail;


    /*-----------------------------------------------------------------------
      Get domain name search list
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      domain_name_search_list_info =
        (dss_iface_ioctl_domain_name_search_list_type *)argval_ptr;
      ps_iface_get_domain_name_search_list
      (
        ps_iface_ptr,
        domain_name_search_list_info->name_array,
        &domain_name_search_list_info->count
      );

      break;

    /*-----------------------------------------------------------------------
      Get SIP address list.
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_SIP_SERV_ADDR:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      sip_serv_addr_info =
        (dss_iface_ioctl_sip_serv_addr_info_type *) argval_ptr;

      if(0 != ps_iface_get_sip_serv_addr_ex(ps_iface_ptr,
                                            sip_serv_addr_info->addr_array,
                                            &sip_serv_addr_info->count))
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      break;

    /*-----------------------------------------------------------------------
      Get SIP domain name list.
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        break;
      }
      sip_serv_domain_name_info =
        (dss_iface_ioctl_sip_serv_domain_name_info_type *)argval_ptr;
      ps_iface_get_sip_domain_names( ps_iface_ptr,
                                     sip_serv_domain_name_info->name_array,
                                     &sip_serv_domain_name_info->count);
      break;

    case PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST:
      if (NULL == argval_ptr)
      {
         *ps_errno = DS_EFAULT;
         goto bail;
      }

      iface_state = ps_iface_state(ps_iface_ptr);

      if (IFACE_UP != iface_state && IFACE_ROUTEABLE != iface_state)
      {
        *ps_errno = DS_ENETDOWN;
        goto bail;
      }
      
      if(-1 == ps_iface_create_qos_net_initiated_request(ps_iface_ptr,
                                                         argval_ptr,
                                                         ps_errno))
      {
         return -1;
      }
      break;

    case PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE:
      if (NULL == argval_ptr)
      {
         *ps_errno = DS_EFAULT;
         goto bail;
      }

      if(-1 == ps_iface_release_qos_net_initiated(ps_iface_ptr,
                                                  argval_ptr,
                                                  ps_errno))
      {
         return -1;
      }
      break;

    case PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK:
      if (NULL == argval_ptr)
      {
         *ps_errno = DS_EFAULT;
         goto bail;
      }              

      if(-1 == ps_iface_qos_net_initiated_real_time_check(ps_iface_ptr,
                                                          argval_ptr,
                                                          ps_errno))
      {
         return -1;
      }
      break;

      case PS_IFACE_IOCTL_KEEP_ALIVE:
      if (NULL == argval_ptr)
      {
         *ps_errno = DS_EFAULT;
         goto bail;
      }

      /*---------------------------------------------------------------------
        Setting to FALSE is not supported for now.
      ---------------------------------------------------------------------*/	
      if (*((ps_iface_ioctl_keep_alive_type *)argval_ptr) == FALSE)
      {
        *ps_errno = DS_EOPNOTSUPP;
        goto bail;
      }

      iface_state = ps_iface_state(ps_iface_ptr);

      if (IFACE_DOWN == iface_state)
      {
        *ps_errno = DS_ENETDOWN;
         goto bail;
      }
      /*---------------------------------------------------------------------
        Set the flag on both physical and logical iface.
      ---------------------------------------------------------------------*/	
      PS_IFACE_SET_PROPERTY( ps_iface_ptr, 
                             PS_IFACE_SUPPORTED_FLAG_KEEP_ALIVE );
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(ps_iface_ptr);
      PS_IFACE_SET_PROPERTY( base_iface_ptr, 
                             PS_IFACE_SUPPORTED_FLAG_KEEP_ALIVE );

    break;

    case PS_IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      
      pdn_teardown_config_ptr =
        (ps_iface_ioctl_pdn_teardown_config_type *)argval_ptr;
      
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(ps_iface_ptr);
      if (PS_IFACE_IS_VALID(base_iface_ptr))
      {
        base_iface_ptr->iface_private.pdn_teardown_config.local_abort = 
          pdn_teardown_config_ptr->local_abort;
      }
      else
      {
        *ps_errno = DS_EINVAL;
        goto bail;
      }

      break;   

      
    case PS_IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG:
      if (NULL == argval_ptr)
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }

      pdn_teardown_config_ptr =
        (ps_iface_ioctl_pdn_teardown_config_type *)argval_ptr;

      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(ps_iface_ptr);
      if (PS_IFACE_IS_VALID(base_iface_ptr))
      {
        pdn_teardown_config_ptr->local_abort =
          base_iface_ptr->iface_private.pdn_teardown_config.local_abort;
      }
      else
      {
        *ps_errno = DS_EINVAL;
        goto bail;
      }
      
      break; 
 
    case PS_IFACE_IOCTL_FILTER_MATCH_REQUEST:
      if (NULL == argval_ptr)
      {
         *ps_errno = DS_EFAULT;
         goto bail;
      }
      if(0 != ps_iface_check_fltr_match(ps_iface_ptr,
                                            argval_ptr))
      {
        *ps_errno = DS_EFAULT;
        goto bail;
      }
      break;  

    default:
      /*---------------------------------------------------------------------
        Unless iface is UP, OTA signalling can't be done and hence the IOCTLs 
        are rejected"
      ---------------------------------------------------------------------*/	
      if (ioctl_name == PS_IFACE_IOCTL_QOS_REQUEST || 
          ioctl_name == PS_IFACE_IOCTL_QOS_REQUEST_EX ||
          ioctl_name == PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE ||
          ioctl_name == PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE ||
          ioctl_name == PS_IFACE_IOCTL_SET_SILENT_REDIAL ||
          ioctl_name == PS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT)
      {
        iface_state = ps_iface_state(ps_iface_ptr);

        if (IFACE_UP != iface_state && IFACE_ROUTEABLE != iface_state)
        {
          LOG_MSG_ERROR_4("ps_iface_ioctl(): IOCTL %d is not supported in "
                          "state 0x%x iface 0x%x:%d ",
                          ioctl_name,
                          iface_state,
                          ps_iface_ptr->name,
                          ps_iface_ptr->instance);

          *ps_errno = DS_ENETDOWN;
          goto bail;
        } 
      }
	  
      /*---------------------------------------------------------------------
        Interface specific operation. Call the registered callback.
      ---------------------------------------------------------------------*/
      if (-1 == ps_iface_ioctli_mode_handler_cback (ps_iface_ptr,
                                                    ioctl_name,
                                                    argval_ptr,
                                                    ps_errno))
      {
        goto bail;
      }
      break;
  } /* switch */

  /*-------------------------------------------------------------------------
    In success case, we break out of the switch. Please be aware of this
    if you are adding any code after the above switch statement.
  -------------------------------------------------------------------------*/
  return 0;

bail:
  LOG_MSG_ERROR_1 ("ps_iface_ioctl err %d", *ps_errno);
  return -1;

} /* ps_iface_ioctl() */

