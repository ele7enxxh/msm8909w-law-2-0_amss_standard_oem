#ifndef __DSS_PRIVIPV6ADDRHANDLER_H__
#define __DSS_PRIVIPV6ADDRHANDLER_H__

/*====================================================

FILE:  DSS_PrivIpv6AddrHandler.h

SERVICES:
   Handle private Ipv6 Adress events

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PrivIpv6AddrHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerPrivIPV6Addr.h"

class DSSPrivIpv6AddrHandler : public DSSEventHandlerPrivIpv6Addr
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSPrivIpv6AddrHandler* CreateInstance();
   DSSPrivIpv6AddrHandler();
};

#endif // __DSS_PRIVIPV6ADDRHANDLER_H__
