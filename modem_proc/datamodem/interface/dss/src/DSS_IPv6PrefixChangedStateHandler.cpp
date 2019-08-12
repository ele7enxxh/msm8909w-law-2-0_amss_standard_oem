/*======================================================

FILE:  DSS_PhysLinkStateHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_PhysLinkStateHandler functions

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_IPv6PrefixChangedStateHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_IPv6PrefixChangedStateHandler.h"
#include <stringl/stringl.h>
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_GenScope.h"
#include "ps_in.h"
#include "ps_ip6_addr.h"
#include "ps_system_heap.h"

using namespace ds::Net;

DSSIPv6PrefixChangedStateHandler::DSSIPv6PrefixChangedStateHandler()
{
   mEventType = EVENT_HANDLER_IPV6_PREFIX;
}

void DSSIPv6PrefixChangedStateHandler::Destructor() throw()
{
   PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes);
   PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes);
   DSSEventHandlerNetApp::Destructor();
}

AEEResult DSSIPv6PrefixChangedStateHandler::SetEventData(dss_iface_ioctl_event_enum_type event,
                                  bool bReg,
                                  dss_iface_ioctl_event_cb userCB,
                                  void* userData,
                                  sint15 appId)
{
   if (NULL == mpHandlerData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerData, sizeof(DSSIPv6PrefixHndlData), 
                               DSSIPv6PrefixHndlData*);
      if (NULL == mpHandlerData) {
         return AEE_ENOMEMORY;
      }
      
      mpHandlerData->mpEventData = NULL;
      ((DSSIPv6PrefixHndlData *)mpHandlerData)->mPrevPrefixes = NULL;
      ((DSSIPv6PrefixHndlData *)mpHandlerData)->mNumPrevPrefixes = 0;
   }

   if (NULL == mpHandlerDataClone) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone, sizeof(DSSIPv6PrefixHndlData),
                               DSSIPv6PrefixHndlData*);
      if (NULL == mpHandlerDataClone) {
         return AEE_ENOMEMORY;
      }
      
      mpHandlerDataClone->mpEventData = NULL;
      ((DSSIPv6PrefixHndlData *)mpHandlerDataClone)->mPrevPrefixes = NULL;
      ((DSSIPv6PrefixHndlData *)mpHandlerDataClone)->mNumPrevPrefixes = 0;
   }

   
   PS_SYSTEM_HEAP_MEM_FREE(((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes);
   ((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes = 0;
   PS_SYSTEM_HEAP_MEM_FREE(((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes);
   ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes = 0;

   if (NULL == mpHandlerData->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC( mpHandlerData->mpEventData, sizeof(EventData), EventData*);
      if (NULL == mpHandlerData->mpEventData) {
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerData->mpEventData, 0, sizeof (EventData));
   }

   if (NULL == mpHandlerDataClone->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone->mpEventData, sizeof(EventData), EventData*);
      if (NULL == mpHandlerDataClone->mpEventData) {
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerDataClone->mpEventData, 0, sizeof (EventData));
   }

   IDS_ERR_RET(InitEventData(mpHandlerData->mpEventData,bReg,userCB,userData,appId));

   return AEE_SUCCESS;
}

AEEResult DSSIPv6PrefixChangedStateHandler::GetNetworkIPv6Priv(INetworkIPv6Priv** ppiNetworkIPv6Priv)
{
   DSSIDSNetworkScope IDSNetworkScope;
   IDS_ERR_RET(IDSNetworkScope.Init(mpParent));
   IDS_ERR_RET(IDSNetworkScope.Fetch()->GetTechObject(AEEIID_INetworkIPv6Priv, (void**)ppiNetworkIPv6Priv));
   return AEE_SUCCESS;
}


void DSSIPv6PrefixChangedStateHandler::EventOccurred()
{
   INetworkIPv6Priv* piNetworkIPv6Priv;

   AEEResult res;

   {
      // for DSSIPv6PrefixChangedStateHandler, event data has different size, so it should copy
      // all the data to mpHandlerDataClone
      DSSCritScope cs(*mpCritSect);
      if ((((DSSIPv6PrefixHndlData*)mpHandlerData)->mpEventData)->bReg) {

        memscpy(((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mpEventData,
           sizeof(DSSEventHandler::EventData),
           ((DSSIPv6PrefixHndlData*)mpHandlerData)->mpEventData,
           sizeof(DSSEventHandler::EventData));

        if (0 != ((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes) {
           PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes);

           ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes = 
              ((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes;

         ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes = ((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes;
            ((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes = NULL;
            ((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes = 0;
         }
         else {
           ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes = NULL;
           ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes = 0;
         }

      }
      else {
         return;
      }
   } // release lock

   DSSWeakRefScope WeakRefScope;

   if(!WeakRefScope.Init(mpParent)) {
      return;
   }

   res = GetNetworkIPv6Priv(&piNetworkIPv6Priv);
   DSSGenScope scopeNetworkIPv6Priv(piNetworkIPv6Priv,DSSGenScope::IDSIQI_TYPE);

   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
                      "GetNetworkIPv6Priv() failed: %d", res);
      return;
   }

   IPv6PrivPrefixInfoType* prevPrefixes = ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes;
   int numCurrPrefixes = 0;
   IPv6PrivPrefixInfoType* currPrefixes = NULL;
   res = GetAllPrefixes(piNetworkIPv6Priv,&currPrefixes,&numCurrPrefixes);
   LOG_MSG_INFO1_1 ("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
     "numCurrPrefixes %d", numCurrPrefixes);
   if (AEE_SUCCESS != res || NULL == currPrefixes) {
      LOG_MSG_ERROR_0("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
                      "GetAllPrefixes failed ");
      return;
   }

   // now we need to compare current and previous prefixes arrays
   // stage 1 , pass the currPrefix array and locate all prefixes appearing also inside prevPrefix array but with a different state
   int currInd = 0;  int prevInd = 0, prevIndBack = 0;
   bool nextCurr = false;

   int numPrevPrefixes = (((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes);
   IPv6PrivPrefixInfoType prev,curr;

   while (currInd<numCurrPrefixes) {
      curr = currPrefixes[currInd];
      LOG_MSG_INFO1_2 ("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
        "curr.prefixType %d, currInd %d", curr.prefixType, currInd);
      nextCurr = false;
      while ((prevInd< numPrevPrefixes) && (!nextCurr)){
         prev = prevPrefixes[prevInd];
         prevIndBack = prevInd;
         // we've already found this one , skip it
         if (!prev.prefixLen) {
            prevInd++ ;
            continue ;
         }

         IPV6PrefixCompareResult resPrefix = ComparePrefixes(&prev,&curr);
         LOG_MSG_INFO1_3 ("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
           "CMP resPrefix %d, prev.prefixType %d, curr.prefixType %d",
           resPrefix, prev.prefixType, curr.prefixType);

         switch (resPrefix) {
            case PREFIXES_EQUAL:
               // we have a match , now mark the prefix in prevPrefix array , so that we skip it in the future
               prev.prefixLen = 0;
               prevPrefixes[prevIndBack].prefixLen = 0;
               // handle the case when prefix was available when registered and prefix is available now,
               // since application expects Prefix update with kind DSS_IFACE_IOCTL_PREFIX_ADDED
               if ( (IPv6AddrState::PRIV_ADDR_AVAILABLE == prev.prefixType) &&
                    (IPv6AddrState::PRIV_ADDR_AVAILABLE == curr.prefixType))
                 ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_ADDED);
               // continue to the next currInd
               nextCurr = true;
               continue;
            case PREFIXES_DIFFERENT:
               // nothing to do here , continue
               prevInd++;
               continue;
            case PREFIXES_STATE_CHANGED:
               // we have a match , now mark the prefix in prevPrefix array , so that we skip it in the future
               prev.prefixLen = 0;
               prevPrefixes[prevIndBack].prefixLen = 0;
               // fire callback and continue to the next currInd
               if ( (IPv6AddrState::PRIV_ADDR_AVAILABLE == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_WAITING == curr.prefixType)) {
                  LOG_MSG_ERROR_2("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
                                  "Wrong prefix state sequence:%d %d",
                                  prev.prefixType, curr.prefixType);
                  goto bail;
               }
               if ( (IPv6AddrState::PRIV_ADDR_AVAILABLE == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_DEPRECATED == curr.prefixType)) {
                  ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_DEPRECATED);
               }
               if ( (IPv6AddrState::PRIV_ADDR_WAITING == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_AVAILABLE == curr.prefixType)) {
                  ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_ADDED);
               }
               if ( (IPv6AddrState::PRIV_ADDR_WAITING == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_DEPRECATED == curr.prefixType)) {
                  ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_ADDED);
                  ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_DEPRECATED);
               }
               if ( (IPv6AddrState::PRIV_ADDR_DEPRECATED == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_AVAILABLE == curr.prefixType)) {
                  ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_UPDATED);
               }
               if ( (IPv6AddrState::PRIV_ADDR_DEPRECATED == prev.prefixType) && (IPv6AddrState::PRIV_ADDR_WAITING == curr.prefixType)) {
                  LOG_MSG_ERROR_2("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
                                  "Wrong prefix state sequence:%d %d",
                                  prev.prefixType, curr.prefixType);
                  goto bail;
               }
               nextCurr = true;
               continue;
            default:
               continue;
         } // switch
      } // while (prevInd< numPrevPrefixes)
      currInd++; prevInd=0;
      if (nextCurr) {
         continue;
      }

      // if we got here , it means that we could not find current prefix inside previous prefixes array
      // it's a new one , fire callback and advance to next currInd
      if (IPv6AddrState::PRIV_ADDR_AVAILABLE == curr.prefixType) {
         ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_ADDED);
      }
      if (IPv6AddrState::PRIV_ADDR_DEPRECATED == curr.prefixType) {
         ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_ADDED);
         ProcessCB(&curr,DSS_IFACE_IOCTL_PREFIX_DEPRECATED);
      }
   } //  while (currInd<numCurrPrefixes)

   // stage 2
   // pass the prevPrefix array and dispatch callback with DSS_IFACE_IOCTL_PREFIX_REMOVED event for all unmarked prefixes
   for (int i=0; i < (((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes); i++) {
      if (!prevPrefixes[i].prefixLen) {
         continue;
      }
      if ((IPv6AddrState::PRIV_ADDR_AVAILABLE == prevPrefixes[i].prefixType) || (IPv6AddrState::PRIV_ADDR_DEPRECATED == prevPrefixes[i].prefixType)) {
         LOG_MSG_INFO1_2 ("DSSIPv6PrefixChangedStateHandler::EventOccurred(): "
           "prevPrefixes[i].prefixType %d, i %d", prevPrefixes[i].prefixType, i);
         ProcessCB(&prevPrefixes[i],DSS_IFACE_IOCTL_PREFIX_REMOVED);
      }
   }
   
bail:
   PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mPrevPrefixes);
   ((DSSIPv6PrefixHndlData*)mpHandlerDataClone)->mNumPrevPrefixes = 0;
   
   // copy all back to mpHandlerData.   
   DSSCritScope cs(*mpCritSect);
   if ((((DSSIPv6PrefixHndlData*)mpHandlerData)->mpEventData)->bReg) {
      PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes);
      ((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes = currPrefixes;
      ((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes = numCurrPrefixes;
   }
}


AEEResult DSSIPv6PrefixChangedStateHandler::RegisterIDL()
{
   INetworkIPv6Priv* piNetworkIPv6Priv = NULL;
   
   DSSWeakRefScope WeakRefScope;

   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   // DSSIPv6PrefixChangedStateHandler will cache the current prefixes
   // this is done since dsNet posts an event to the Handler via raising a signal &
   // signal cant pass event info. Handler needs to query dsNet and figure out the
   // event info which is prone to race conditions.
   IDS_ERR_RET(GetNetworkIPv6Priv(&piNetworkIPv6Priv));
   DSSGenScope scopeNetworkIPv6Priv(piNetworkIPv6Priv,DSSGenScope::IDSIQI_TYPE);

   PS_SYSTEM_HEAP_MEM_FREE (((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes);
   AEEResult nRet = GetAllPrefixes(piNetworkIPv6Priv,&(((DSSIPv6PrefixHndlData*)mpHandlerData)->mPrevPrefixes),&(((DSSIPv6PrefixHndlData*)mpHandlerData)->mNumPrevPrefixes));

   if (AEE_SUCCESS != nRet) {
      LOG_MSG_ERROR_1("DSSIPv6PrefixChangedStateHandler::RegisterIDL(): "
                      "Could not get all prefixes while registering: %d",
                      nRet);
      return nRet;
   }

   LOG_MSG_INFO1_1("DSSIPv6PrefixChangedStateHandler::RegisterIDL(): "
                   "Registering to QDS_EV_PREFIX_CHANGED, NetworkIPv6Priv obj 0x%p",
                   piNetworkIPv6Priv);
   nRet = piNetworkIPv6Priv->OnStateChange(mpSignal, IPv6PrivEvent::QDS_EV_PREFIX_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   if (AEE_SUCCESS != nRet) {
      LOG_MSG_ERROR_1("DSSIPv6PrefixChangedStateHandler::RegisterIDL(): "
                      "Could not register for PREFIX_CHANGED event: %d", nRet);
      return nRet;
   }

   return nRet;
}


AEEResult DSSIPv6PrefixChangedStateHandler::GetAllPrefixes(INetworkIPv6Priv* piNetworkIPv6Priv ,IPv6PrivPrefixInfoType** prefixes,int* num)
{
   int temp;
   AEEResult nRet = AEE_SUCCESS;
   IDS_ERR_RET(piNetworkIPv6Priv->GetAllIPv6Prefixes(NULL,0,num));
   // In case there are no prefixes, just return success
   if (0 == *num) {
      return AEE_SUCCESS;
   }

   PS_SYSTEM_HEAP_MEM_ALLOC(*prefixes, sizeof(IPv6PrivPrefixInfoType)*(*num),
                             IPv6PrivPrefixInfoType*);

   if (NULL == *prefixes) {
      return AEE_ENOMEMORY;
   }
   BAIL_ERR(piNetworkIPv6Priv->GetAllIPv6Prefixes(*prefixes,*num,&temp));

   return nRet;

bail:
   PS_SYSTEM_HEAP_MEM_FREE (*prefixes);

   return nRet;
}


DSSIPv6PrefixChangedStateHandler::IPV6PrefixCompareResult
DSSIPv6PrefixChangedStateHandler::ComparePrefixes(IPv6PrivPrefixInfoType* source, IPv6PrivPrefixInfoType* dest)
{
   ps_in6_addr srcAddr;
   ps_in6_addr dstAddr;

   if (source->prefixLen != dest->prefixLen) {
      return PREFIXES_DIFFERENT;
   }
   memset(&srcAddr, 0, sizeof(ps_in6_addr));
   memset(&dstAddr, 0, sizeof(ps_in6_addr));

   memscpy(&srcAddr, sizeof(ps_in6_addr),
     source->prefix, sizeof(ps_in6_addr));
   memscpy(&dstAddr, sizeof(ps_in6_addr),
     dest->prefix, sizeof(ps_in6_addr));

   if (!IN6_ARE_PREFIX_EQUAL(&srcAddr, &dstAddr, source->prefixLen)) {
      return PREFIXES_DIFFERENT;
   }

   if (source->prefixType != dest->prefixType) {
      return PREFIXES_STATE_CHANGED;
   } else {
      return PREFIXES_EQUAL;
   }
}


void DSSIPv6PrefixChangedStateHandler::ProcessCB(IPv6PrivPrefixInfoType* prefixStruct,dss_iface_ioctl_prefix_update_enum_type event)
{
   dss_iface_ioctl_event_info_union_type eventInfo;
   dss_iface_ioctl_event_enum_type eventStatus = DSS_IFACE_IOCTL_PREFIX_UPDATE_EV;
   eventInfo.prefix_info.kind = event;
   eventInfo.prefix_info.prefix_len = (uint8) prefixStruct->prefixLen;
   memscpy(&eventInfo.prefix_info.prefix, sizeof(ps_in6_addr),
     prefixStruct->prefix, sizeof(ps_in6_addr));

   DSSEventHandlerNetApp::DispatchCB(eventStatus, ((DSSIPv6PrefixHndlData*)mpHandlerData)->mpEventData, &eventInfo);

}

DSSIPv6PrefixChangedStateHandler* DSSIPv6PrefixChangedStateHandler::CreateInstance()
{
   return new DSSIPv6PrefixChangedStateHandler;
}
