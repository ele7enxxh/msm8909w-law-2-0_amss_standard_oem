
/*===================================================

FILE:  DSS_QoSRequestExScope.cpp

SERVICES:
A utility class to automatically release various variables 
when out of scope

=====================================================

Copyright (c) 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSRequestExScope.cpp#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2011-02-23 aa  History added.

===========================================================================*/

#include "customer.h"
#include "DSS_MemoryManagement.h"
#include "DSS_QoSRequestExScope.h"

using namespace ds::Net;

// You should define instance of this class with pointer to some (initialized) object 
// and type of this object , when you wish memory to be automatically freed on scope exit


DSSQoSRequestExScope::DSSQoSSpecScope::DSSQoSSpecScope():
   mrxFilter(NULL), mtxFilter(NULL), mrxFlows(NULL), mtxFlows(NULL),
   mnRxFilterLen(0), mnTxFilterLen(0), mnRxFlowsLen(0), mnTxFlowsLen(0){}

void 
DSSQoSRequestExScope::DSSQoSSpecScope::Init
(
 IIPFilterPriv** rxFilter,
 int nRxFilterLen,
 IIPFilterPriv** txFilter,
 int nTxFilterLen,
 IQoSFlowPriv** rxFlows,
 int nRxFlowsLen,
 IQoSFlowPriv** txFlows,
 int nTxFlowsLen
)
{
   mrxFilter = rxFilter;
   mnRxFilterLen = nRxFilterLen;
   
   mtxFilter = txFilter;
   mnTxFilterLen = nTxFilterLen;

   mrxFlows = rxFlows;
   mnRxFlowsLen = nRxFlowsLen;

   mtxFlows = txFlows;
   mnTxFlowsLen = nTxFlowsLen;
}

DSSQoSRequestExScope::DSSQoSSpecScope::~DSSQoSSpecScope()
{
   int i;

   for(i = 0; i < mnRxFilterLen; i++)
   {
      DSSCommon::ReleaseIf((IQI**)&mrxFilter[i]);
   }

   PS_SYSTEM_HEAP_MEM_FREE(mrxFilter);

   for(i = 0; i < mnTxFilterLen; i++)
   {
      DSSCommon::ReleaseIf((IQI**)&mtxFilter[i]);
   }

   PS_SYSTEM_HEAP_MEM_FREE(mtxFilter);

   for(i = 0; i < mnRxFlowsLen; i++)
   {
      DSSCommon::ReleaseIf((IQI**)&mrxFlows[i]);
   }

   PS_SYSTEM_HEAP_MEM_FREE(mrxFlows);

   for(i = 0; i < mnTxFlowsLen; i++)
   {
      DSSCommon::ReleaseIf((IQI**)&mtxFlows[i]);
   }

   PS_SYSTEM_HEAP_MEM_FREE(mtxFlows);

}

void* DSSQoSRequestExScope::DSSQoSSpecScope::operator new[] 
(
 unsigned int size
) throw ()
{
   void * buf = NULL;
   PS_SYSTEM_HEAP_MEM_ALLOC(buf, size, void*);
   return buf;
}

void DSSQoSRequestExScope::DSSQoSSpecScope::operator delete[] 
(
 void* ptr
)
{
   PS_SYSTEM_HEAP_MEM_FREE(ptr);
}

DSSQoSRequestExScope::DSSQoSRequestExScope
(
 int nQoSSpecScopeLen
 ) : mnQoSSpecScopeLen(0)
{
   maQoSSpecScope = new DSSQoSSpecScope[nQoSSpecScopeLen];

   if (NULL != maQoSSpecScope)
   {
      mnQoSSpecScopeLen = nQoSSpecScopeLen;
   }
}

DSSQoSRequestExScope::~DSSQoSRequestExScope()
{
   delete[] maQoSSpecScope;
}

AEEResult DSSQoSRequestExScope::SetNthQoSSpec
(
 int n,
 IIPFilterPriv** rxFilter,
 int nRxFilterLen,
 IIPFilterPriv** txFilter,
 int nTxFilterLen,
 IQoSFlowPriv** rxFlows,
 int nRxFlowsLen,
 IQoSFlowPriv** txFlows,
 int nTxFlowsLen
)
{
   if (n < 0 || n >= mnQoSSpecScopeLen)
   {
      return AEE_EBADPARM;
   }

   maQoSSpecScope[n].Init(rxFilter,
                          nRxFilterLen,
                          txFilter,
                          nTxFilterLen,
                          rxFlows,
                          nRxFlowsLen,
                          txFlows,
                          nTxFlowsLen);

   return AEE_SUCCESS;

}



