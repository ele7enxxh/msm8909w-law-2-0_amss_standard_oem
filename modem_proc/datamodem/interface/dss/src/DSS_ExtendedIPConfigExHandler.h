#ifndef __DSS_EXTENDEDIPCONFIGEXHANDLER_H__
#define __DSS_EXTENDEDIPCONFIGEXHANDLER_H__

/*====================================================

FILE:  DSS_ExtendedIPConfigExHandler.h

SERVICES:
   Handle mtu propagated event.

=====================================================

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_ExtendedIPConfigExHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-07-20 sb  Created module.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSExtendedIPConfigExHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSExtendedIPConfigExHandler* CreateInstance();
   DSSExtendedIPConfigExHandler();
};

#endif // __DSS_EXTENDEDIPCONFIGEXHANDLER_H__
