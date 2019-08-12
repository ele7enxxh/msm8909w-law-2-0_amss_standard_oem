#ifndef __DSS_NETWORK_EXT_IPV6_DEL_ADDR_IPHANDLER_H__
#define __DSS_NETWORK_EXT_IPV6_DEL_ADDR_IPHANDLER_H__

/*====================================================

FILE:  DSS_NetworkExtIPv6DelAddrHandler.h.h

SERVICES:
   Handle network external IPv6 address deleted event.

=====================================================

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetworkExtIPv6DelAddrHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-08-07 vm  Created.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSNetworkExtIPv6DelAddrHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSNetworkExtIPv6DelAddrHandler* CreateInstance();
   DSSNetworkExtIPv6DelAddrHandler();
   virtual void Destructor() throw();
};

#endif // __DSS_NETWORK_EXT_IPV6_DEL_ADDR_IPHANDLER_H__
