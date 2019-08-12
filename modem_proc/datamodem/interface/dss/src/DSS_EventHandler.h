#ifndef __DSS_EVENTHANDLER_H__
#define __DSS_EVENTHANDLER_H__

/*====================================================

FILE:  DSS_EventHandler.h

SERVICES:
   Provide a common base class for event handling.

=====================================================

Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandler.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "ds_Utils_ICritSect.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ps_mem.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_SignalHandler.h"
#include "ds_Utils_SignalHandlerBase.h"
#include "dss_iface_ioctl.h"

class DSSEventHandler: public ds::Utils::SignalHandlerBase
{
public:
   AEEResult Register(dss_iface_ioctl_event_enum_type event,
                      dss_iface_ioctl_event_cb userCB,
                      void* userData,
                      dss_iface_id_type ifaceId);
   AEEResult DeRegister(dss_iface_ioctl_event_enum_type event);
   DSSEventHandler();
   virtual void Destructor(void) throw();

protected:

   // This destructor is not used.
   //
   // In Weak Ref-using classes, function Destructor() does the cleanup,
   // and the real destructor is a no-op.
   virtual ~DSSEventHandler() throw(){ /* NO-OP */ };
   struct EventData {
      bool bReg;
      dss_iface_ioctl_event_cb userCB;
      void* userData;

      // Either app ID or -1 for policy bit on
      // This number is used to generate ifaceId number to be used when calling
      sint15 appId;
   };

   struct HandlerData {
      EventData* mpEventData;
   };

   // This enum is used by each concrete handler to specify of what type the
   // handler is. The reason is ability to distinguish the different objects on the target
   // due to the fact that they are all allocated in the same ps_mem array
   typedef enum {
      EVENT_HANDLER_UNDEFINED        = 0,
      EVENT_HANDLER_EXT_IP_CONFIG    = 1,
      EVENT_HANDLER_RATE_INTERIA     = 2,
      EVENT_HANDLER_MCAST_MBSCTRL    = 3,
      EVENT_HANDLER_BEARER_TECH      = 4,
      EVENT_HANDLER_SLOTT_SESS_CHNG  = 5,
      EVENT_HANDLER_SLOTT_RES        = 6,
      EVENT_HANDLER_RFC_COND         = 7,
      EVENT_HANDLER_QOS_PROF_CHNG    = 8,
      EVENT_HANDLER_QOS_MODIFY       = 9,
      EVENT_HANDLER_QOS_INF_CODE_UPD = 10,
      EVENT_HANDLER_QOS              = 11,
      EVENT_HANDLER_QOS_DEL_ON_IFACE = 12,
      EVENT_HANDLER_QOS_AW_UNAW      = 13,
      EVENT_HANDLER_QOS_ADD_ON_IFACE = 14,
      EVENT_HANDLER_QOS_PRI_MOD_STAT = 15,
      EVENT_HANDLER_QOS_PRI_MOD      = 16,
      EVENT_HANDLER_PHYS_LINK_STATE  = 17,
      EVENT_HANDLER_OUTAGE           = 18,
      EVENT_HANDLER_NETWORK_STATE    = 19,
      EVENT_HANDLER_NETWORK_IP       = 20,
      EVENT_HANDLER_MTPD_REQUEST     = 21,
      EVENT_HANDLER_MCAST_STATUS     = 22,
      EVENT_HANDLER_MCAST_REGISTER   = 23,
      EVENT_HANDLER_IP6PRIV_ADDR     = 24,
      EVENT_HANDLER_IPV6_PREFIX      = 25,
      EVENT_HANDLER_FAST_DORM_STATUS = 26,
      EVENT_HANDLER_EXT_IPV6_DEL_ADDR = 27,
      EVENT_HANDLER_HANDOFF_INIT      = 28,
      EVENT_HANDLER_HANDOFF_SUCCESS   = 29,
      EVENT_HANDLER_HANDOFF_FAILURE   = 30,
      EVENT_HANDLER_BEARER_TECH_EX    = 31,
      EVENT_HANDLER_EXT_IP_CONFIG_EX  = 32,
      EVENT_HANDLER_APN_PARAM_CHANGE  = 33,
   }EventType;

   // Set the event data for specific event
   // The default implementation should be overridden if there are several events
   // handled by the specific event handler
   virtual AEEResult SetEventData(dss_iface_ioctl_event_enum_type event,
                                  bool bReg,
                                  dss_iface_ioctl_event_cb userCB,
                                  void* userData,
                                  sint15 appId);

   // Fills a single event data structure
   // pEd - output param : the event data structure to be filled
   // bReg - input param : if true - we are trying to register, if false - deregister
   // userCB, userData - input params : user callback and data
   // 2 errors can be returned :
   //   QDS_EINPROGRESS - if we are trying to register and there is already data inside data structure, meaning we've already registered in the past
   //   QDS_ENOTCONN - if we are trying to deregister and there is no data inside the data structure, meaning we didn't register
   AEEResult InitEventData(EventData* pEd,
                           bool bReg,
                           dss_iface_ioctl_event_cb userCB,
                           void* userData,
                           sint15 appId);

   virtual void EventOccurred() = 0;

   // Register to events in the IDL layer.
   virtual AEEResult RegisterIDL() = 0;

   static void SignalCB(SignalHandlerBase* pSignalHandlerBaseParam);
   void CBHandler();

   ISignal* mpSignal;

   // this member is holding the Handlers Data. It holds EventData but there are some classes such as
   // DSSIPv6PrefixChangedStateHandler that hold extra data.
   // This change is done in order to make all the EventHandlers the same size
   // to avoid memory management.
   // the Handlers Data is allocated on a SCRATCHPAD.
   HandlerData* mpHandlerData;
   EventType mEventType;

   // during event processing, mpHandlerData is copied to mpHandlerDataClone and only the data there is used for the processing.
   // The data in mpHandlerData may be changed by a new registration operation triggered by application.
   HandlerData* mpHandlerDataClone;

   ICritSect* mpCritSect;
   ISignalCtl* mpSignalCtl;

   // The event this event handler registers to
   int mRegisteredIDLEvent;

   // The event handled by this event handler
   dss_iface_ioctl_event_enum_type mEvent;

protected:
   void * operator new (
      unsigned int numBytes
   ) throw();
};

#endif // __DSS_EVENTHANDLER_H__
