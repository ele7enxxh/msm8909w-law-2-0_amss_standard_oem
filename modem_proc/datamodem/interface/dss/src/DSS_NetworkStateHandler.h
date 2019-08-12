#ifndef __DSS_NETWORKSTATEHANDLER_H__
#define __DSS_NETWORKSTATEHANDLER_H__

/*====================================================

FILE:  DSS_NetworkStateHandler.h

SERVICES:
   Handle network down event.

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetworkStateHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"

class DSSNetworkStateHandler : public DSSEventHandlerNetApp
{
protected:
  virtual void EventOccurred();
  virtual AEEResult RegisterIDL();

public:
  static DSSNetworkStateHandler* CreateInstance();
  DSSNetworkStateHandler();
};

#endif // __DSS_NETWORKSTATEHANDLER_H__
