#ifndef __DSS_EVENTHANDLERQOSNETINITIATEDREQ_H__
#define __DSS_EVENTHANDLERQOSNETINITIATEDREQ_H__

/*====================================================

FILE:  DSS_EventHandlerQoSNetInitiatedReq.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerQoSNetInitiatedReq.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-05 sz  Created.

===========================================================================*/

#include "DSS_EventHandler.h"
#include "DSS_QoSNetInitiatedReq.h"

class DSSEventHandlerQoSNetInitiatedReq : public DSSEventHandler
{
public:
   // This class uses two-phase construction: the constructor is empty and the
   // actual initialization is done through the Init() function, so it can return
   // an error value.
   AEEResult Init(DSSQoSNetInitiatedReq* parentNetAppParam);
   virtual void Destructor() throw();

protected:
   void DispatchCB(dss_iface_ioctl_event_enum_type event, 
                   EventData* ped,
                   dss_iface_ioctl_event_info_union_type* eventInfo);

   DSSQoSNetInitiatedReq* mpParent;
};

#endif // __DSS_EVENTHANDLERQOSNETINITIATEDREQ_H__
