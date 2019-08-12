#ifndef PS_IP_NETCONFIG_H
#define PS_IP_NETCONFIG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I P _ N E T C O N F I G . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_ip_netconfig.h
   @brief This is the header file that defines all of the IP addressing types and
  macros. 
 */

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the tcpUdpIpIp6 group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip_addr.h_v   1.5   11 Oct 2002 09:15:46   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ip_netconfig.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
02/03/11    cp     Removed the use of first hop mtu variable.
07/30/10    cp     Propagating the first hop mtu value to TE for v6.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
03/26/09    pp     CMI De-featurization and Removed unused defs.
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.
===========================================================================*/
/* group: tcpUdpIpIp6 */



/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_in.h"
#include "nv_items.h"
#include "ps_ppp_ext.h"

/** @addtogroup datatypes 
  @{
*/

/*---------------------------------------------------------------------------
TYPEDEF IP_V4_CONFIG_TYPE
---------------------------------------------------------------------------*/
/**
  @brief IP configuration information.
*/
typedef struct
{
  /** Gateway address. */
  struct ps_in_addr gateway;
  /** Primary DNS address. */
  struct ps_in_addr primary_dns; 
  /** Secondary DNS address. */
  struct ps_in_addr secondary_dns;  
  /** Net mask. */
  struct ps_in_addr net_mask;       

  /* for WINS support */
  /** Primary NBNS address. */
  struct ps_in_addr primary_nbns; 
  /** Secondary NBNS address. */
  struct ps_in_addr secondary_nbns; 
} ip_v4_net_info_type;

/** 
  @brief IPv6 configuration information.
*/
typedef struct
{
  /** Primary DNS address. */
  struct ps_in6_addr primary_dns; 
  /** Secondary DNS address. */
  struct ps_in6_addr secondary_dns;  
  uint64             gateway_iid;    /**< IID of gateway. */
  uint8              curr_hop_limit; /**< Current hop limit. */

} ip_v6_net_info_type;

/*---------------------------------------------------------------------------
  TYPEDEF NETWORK_INFO_TYPE
---------------------------------------------------------------------------*/
/**
  @brief IP-related network information.
*/
typedef struct
{
  /** IPv4v6 network information union block. */
  union
  {
    ip_v4_net_info_type v4;   /**< IPv4 configuration information. */
    ip_v6_net_info_type v6;   /**< IPv6 configuration information. */
  } net_ip;
  
  uint16 mtu;                 /**< Maximum transmission unit. */

} network_info_type;

/*---------------------------------------------------------------------------
  struct definitions to pass UM config info(ipcp, auth etc) in IFACE bring up command function.
  This replaces the previous struct used by umts (dsumtsps_rm_ppp_info)
  Now defined in a common place as will be used by cdma in future
---------------------------------------------------------------------------*/  

/** 
  @brief Structure definitions used to pass UM configuration 
  information (e.g., ipcp, auth) in IFACE bring up command function.
*/
typedef struct
{
  uint32  ip_address;          /**< IP address requested. */
  uint32  primary_dns;         /**< Primary DNS address. */
  uint32  secondary_dns;       /**< Secondary DNS address. */
  uint32  primary_nbns;        /**< Primary NBNS address. */
  uint32  secondary_nbns;      /**< Secondary NBNS address. */
} ipcp_info_type;

/** Network configuration parameter mask. */
typedef enum
{
  NET_CFG_PRM_PRI_DNS_MASK         =   0x00000001, /**< Primary DNS mask. */
  NET_CFG_PRM_SEC_DNS_MASK         =   0x00000002, /**< Secondary DNS mask. */
  NET_CFG_PRM_PRI_NBNS_MASK        =   0x00000004, /**< Primary NBNS mask. */
  NET_CFG_PRM_SEC_NBNS_MASK        =   0x00000008, /**< Secondary NBNS mask. */ 
  NET_CFG_PRM_IP_ADDR_MASK         =   0x00000010, /**< IP address mask. */
  NET_CFG_PRM_AUTH_PREF_MASK       =   0x00000020, /**< Preference mask. */
  NET_CFG_PRM_AUTH_USERNAME_MASK   =   0x00000040, /**< Username mask. */
  NET_CFG_PRM_AUTH_PASSWORD_MASK   =   0x00000080, /**< Password mask. */
  NET_CFG_PRM_CHAP_CHAL_INFO_MASK  =   0x00000100, /**< Mask that includes chal_info 
                                                        chal_name and auth_id. */
  NET_CFG_PRM_DIAL_STR_MASK        =   0x00000200, /**< Dial string mask. */
  NET_CFG_PRM_MAX_MASK                             /**< Maximum mask. */
} net_cfg_params_mask_e_type;

/**
  @brief Network parameters information for the interface. 
*/
typedef struct
{
  uint32 valid_fields;          /**< Mask to validate the authentication
                                     parameters. */
  /** Structure to hold authentication information. */
  struct
  {
    uint8 auth_type;            /**< Authorization negotiated. Valid values: CHAP, PAP, or 
                                     None. */
    ppp_auth_info_type params;  /**< Pointer to authorization information 
                                     parameters. */
    uint8 auth_id;              /**< ID used for authorization packets. */
  }auth_info;
  byte dial_string[NV_PKT_DIAL_STRING_SIZE]; /**< Dial string passed by ATCOP. */
  ipcp_info_type ipcp_info;     /**< IPCP information. */
} network_params_info_type;

/** @} */ /* end_addtogroup datatypes  */

#endif /* PS_IP_NETCONFIG_H */
