#ifndef __DSS_EVENTHANDLERNETACTIVE_H__
#define __DSS_EVENTHANDLERNETACTIVE_H__

/*====================================================

FILE:  DSS_EventHandlerNetActive.h

SERVICES:
   Provide a common base class for event handling.

=====================================================

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerNetActive.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-12-01 yl  File created.

===========================================================================*/

#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "DSS_EventHandler.h"
#include "DSS_NetActive.h"

class DSSEventHandlerNetActive: public DSSEventHandler
{
public:
  // This class uses two-phase construction: the constructor is empty and the
  // actual initialization is done through the Init() function, so it can return
  // an error value.
  AEEResult Init(DSSNetActive* net);
  virtual void Destructor(void) throw();

protected:
  void DispatchCB(
    dss_iface_ioctl_event_enum_type event,
    EventData* ped,
    dss_iface_ioctl_event_info_union_type* eventInfo);

  DSSNetActive* mpDSSNetActive;
};

#endif // __DSS_EVENTHANDLERNETACTIVE_H__
