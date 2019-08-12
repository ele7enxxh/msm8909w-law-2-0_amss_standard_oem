#ifndef __DSS_HANDOFFINITHANDLER_H__
#define __DSS_HANDOFFINITHANDLER_H__

/*====================================================

FILE:  DSS_HandoffInitHandler.h

SERVICES:
   Handle handoff events.

=====================================================

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_HandoffInitHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-13 da  Created module.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSHandoffInitHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSHandoffInitHandler* CreateInstance();
   DSSHandoffInitHandler();
};

#endif // __DSS_HANDOFFINITHANDLER_H__
