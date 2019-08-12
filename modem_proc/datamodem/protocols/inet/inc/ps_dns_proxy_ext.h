#ifndef __PS_DNS_PROXY_EXT_H_
#define __PS_DNS_PROXY_EXT_H_
/*===========================================================================

                   P S _ D N S _ P R O X Y _ E X T . H

DESCRIPTION
   This is DNS Proxy external header file.


  Copyright (c) 2010-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_dns_proxy_ext.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
05/20/10    rt     Added module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"

/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
typedef void *dns_proxy_handle_type;

/*---------------------------------------------------------------------------
TYPEDEF DNS_PROXY_MODE_ENUM_TYPE

DESCRIPTION
  Enum defining DNS proxy modes
---------------------------------------------------------------------------*/
typedef enum
{
  DNS_PROXY_MODE_V4_V4 = 0,   /* Proxy IPv4 DNS queries to IPv4 (NAT)      */
  DNS_PROXY_MODE_V4_V6 = 1,   /* Proxy IPv4 DNS queries to IPv6 (464XLAT)  */
  DNS_PROXY_MODE_MAX          /* Range checking                            */
} dns_proxy_mode_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DNS_PROXY_RESULT_ENUM_TYPE

DESCRIPTION
  Enum defining possible results from a DNS proxy enable or disable attempt
---------------------------------------------------------------------------*/
typedef enum
{
  DNS_PROXY_ENABLED        = 0,  /* DNS proxy enabled                      */
  DNS_PROXY_ENABLE_FAILED  = 1,  /* DNS proxy enable failed                */
  DNS_PROXY_DISABLED       = 2,  /* DNS proxy disabled                     */
  DNS_PROXY_DISABLE_FAILED = 3,  /* DNS proxy disable failed               */
  DNS_PROXY_RESULT_MAX           /* Range checking                         */
} dns_proxy_result_enum_type;

/*===========================================================================
FUNCTION TYPEDEF DNS_PROXY_RESULT_CBACK_F_TYPE

DESCRIPTION
  Callback function for relaying result from a DNS proxy enable or disable 
  attempt

PARAMETERS
  result : IN : result of the operation
  handle: IN : DNS proxy handle allocated via dns_proxy_enable
  ds_errno : IN : error information

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef void (*dns_proxy_result_cback_f_type)
(
  dns_proxy_result_enum_type result,
  dns_proxy_handle_type      handle,
  sint15                     dss_errno
);

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DNS_PROXY_POWERUP()

DESCRIPTION
  This function is used to do power-up initialization for DNS proxy module. 
  This includes initializing PS mem blocks for the DNS proxy info.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dns_proxy_powerup
(
  void
);

/*===========================================================================
FUNCTION DNS_PROXY_ENABLE()

DESCRIPTION
  This function enables the DNS Proxy. 

DEPENDENCIES

PARAMETERS
  iface_ptr : IN : iface on which to run DNS proxy
  mode      : IN : DNS proxy mode
  result_cb : IN : Callback function for results of operation on this handle
  ds_errno  : IN/OUT : error info

RETURN VALUE
  NULL : Failure
  Other: Success

SIDE EFFECTS
  None
===========================================================================*/
dns_proxy_handle_type
dns_proxy_enable
(
  ps_iface_type                 * iface_ptr,
  dns_proxy_mode_enum_type        mode,
  dns_proxy_result_cback_f_type   result_cb,
  sint15                        * ds_errno 
);

/*===========================================================================
FUNCTION DNS_PROXY_DISABLE()

DESCRIPTION
  This function disables the DNS Proxy. 

DEPENDENCIES

PARAMETERS
  handle : IN : handle returned via earlier call to dns_proxy_enable
  ds_errno  : IN/OUT : error info

RETURN VALUE
  Success  0
  Fail    -1
  
SIDE EFFECTS
  None
===========================================================================*/
int32
dns_proxy_disable
(
  dns_proxy_handle_type   handle,
  sint15                * ds_errno 
);

/*===========================================================================
FUNCTION DNS_PROXY_POWERDOWN()

DESCRIPTION
  This function is used to do power-down reset for DNS proxy module. 

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dns_proxy_powerdown
(
  void
);
#endif /* __PS_DNS_PROXY_EXT_H_ */
