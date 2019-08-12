#ifndef __DSS_HANDOFFFAILURETHANDLER_H__
#define __DSS_HANDOFFFAILURETHANDLER_H__

/*====================================================

FILE:  DSS_HandoffFailureHandler.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_HandoffFailureHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-13 da  Created module.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSHandoffFailureHandler : public DSSEventHandlerNetApp
{
protected:
   virtual void EventOccurred();
   virtual AEEResult RegisterIDL();

public:
   static DSSHandoffFailureHandler* CreateInstance();
   DSSHandoffFailureHandler();
};

#endif // __DSS_HANDOFFFAILURETHANDLER_H__
