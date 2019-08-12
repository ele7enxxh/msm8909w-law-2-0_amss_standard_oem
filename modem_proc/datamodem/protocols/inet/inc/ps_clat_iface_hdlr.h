#ifndef __PS_CLAT_IFACE_HDLR_H_
#define __PS_CLAT_IFACE_HDLR_H_
/*===========================================================================

                 P S _ C L A T _ I F A C E _ H D L R . H

DESCRIPTION
   This is CLAT iface mode handler header file.

Copyright (c) 2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_clat_iface_hdlr.h#1 $ 
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $ 

--------    ---    ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT  
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_route.h"
#include "dsm.h"
#include "ps_sys.h"
#include "ps_iface.h"
#include "ps_aclrules.h"

/*===========================================================================
                               CONSTANTS
===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum number of CLAT Ifaces (PDNs) supported
---------------------------------------------------------------------------*/
#define CLAT_MAX_IFACES PS_SYS_MAX_AVAIL_PDNS 

/*---------------------------------------------------------------------------
  IPv4 Well-Known addresses (WKA) for use in PLAT Discovery. RFC 7050.
---------------------------------------------------------------------------*/
#define CLAT_IPV4_WKA_ONE  IPV4(192, 0, 0, 170)
#define CLAT_IPV4_WKA_TWO  IPV4(192, 0, 0, 171)

/*---------------------------------------------------------------------------
  IPv4 Service Continuity Prefix. 
  RFC 7335 recommends this to be 192.0.0.0/29 (mask 255.255.255.248), but 
  that only allows 6 usable addresses total (192.0.0.1 - 192.0.0.6). 
  We need a total of 16 (2 per PDN), so we use 192.0.0.0/27 
  (mask 255.255.255.224), which allows 30 addresses, 192.0.0.1 - 192.0.0.30.
  11111111.11111111.11111111.11100000b
---------------------------------------------------------------------------*/
#define CLAT_ADDR_SUBNET_MASK         IPV4(255, 255, 255, 224) 
#define CLAT_SUBNET_ADDR              IPV4(192, 0, 0, 0)
#define CLAT_ADDR_SUBNET_COMMON_BITS  27   

/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/
typedef struct clat_iface_addr_s
{
  ps_ip_addr_type own_ip_addr;
  ps_ip_addr_type te_ip_addr;
} ps_clat_iface_addr_type;

/*===========================================================================

                          EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_CLAT_IFACE_INIT() 

DESCRIPTION
  Performs iface related initialization for a CLAT instance.

PARAMETERS
  iface_ptr       : IN  : CLAT iface to be initialized.
  rt_acl_ptr      : IN  : Pointer to memory for the routing ACL
  inbound_acl_ptr : IN  : Pointer to memory for the inbound ACL
clat_instance     : IN  : Pointer to the CLAT instance memory (user_data)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ps_clat_iface_init
(
  ps_iface_type * iface_ptr,
  acl_type      * rt_acl_ptr,
  acl_type      * inbound_acl_ptr, 
  void          * clat_instance
);

/*===========================================================================
FUNCTION  PS_CLAT_IFACE_GET_ADDRS() 

DESCRIPTION
  Returns addresses from the IPv4 Service Continuity pool for the CLAT PDN.

PARAMETERS
  index : IN : index into the CLAT PDN array to return info from
  addr_ptr : OUT : Pointer to write to

DEPENDENCIES
  None.

RETURN VALUE
   0: Success
  -1: Failure

SIDE EFFECTS
  None
===========================================================================*/
int ps_clat_iface_get_addrs
(
  uint8 index,
  ps_clat_iface_addr_type * addr_ptr
);
#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
#endif /* __PS_CLAT_IFACE_HDLR_H_ */
