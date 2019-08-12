
/*===================================================

FILE:  DSS_GenScope.cpp

SERVICES:
A utility class to automatically release various variables 
when out of scope

=====================================================

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_GenScope.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "customer.h"
#include "DSS_MemoryManagement.h"
#include "DSS_GenScope.h"

using namespace ds::Net;

// You should define instance of this class with pointer to some (initialized) object 
// and type of this object , when you wish memory to be automatically freed on scope exit



/*lint -e{1551} */
DSSGenScope::~DSSGenScope()
{
   Release();
}


void DSSGenScope::Release()
{
   if ( NULL == mpVar ){
      LOG_MSG_ERROR_1("DSSGenScope::Release trying to release NULL pointer, this: 0x%x",
                      this);
      return ;
   }

   switch (mVarType) {
   case(IDSIQI_TYPE): 
      DSSCommon::ReleaseIf((IQI**)&mpVar);
      break;
   case(GEN_SCRATCHPAD_ARRAY):
         PS_SYSTEM_HEAP_MEM_FREE(mpVar);
         break;
   case(Scope_ARRAY): {  
         DSSGenScope* temp = (DSSGenScope*)mpVar;
         for (int i = 0; i < mnLen; i++) { 
           temp[i].Release();
         }
         PS_SYSTEM_HEAP_MEM_FREE(temp);
       }
      break;
   case(DS_Network_DomainName_ARRAY_PTR): {  
         DomainName* temp = (DomainName*)mpVar;
         PS_SYSTEM_HEAP_MEM_FREE(temp);
      }
      break;
   case (IDSNetQoSSecondary_ARRAY_PTR): {
         IQoSSecondary** temp = (IQoSSecondary**)mpVar;
         for (int i=0 ; i<mnLen; i++) {
            if (NULL != temp[i]) {
               DSSCommon::ReleaseIf((IQI**)&(temp[i]));
            }
         }
         PS_SYSTEM_HEAP_MEM_FREE(temp);
      }
      break;
#ifdef FEATUTE_DATA_PS_MCAST
   case (IDSNetMCastSession_ARRAY_PTR): {
         IMCastSessionPriv** temp = (IMCastSessionPriv**)mpVar;
         for (int i=0; i<mnLen; i++) {
            if (NULL != temp[i]) {
               DSSCommon::ReleaseIf((IQI**)&(temp[i]));
            }
         }
         PS_SYSTEM_HEAP_MEM_FREE(temp);
      }
      break;
#endif // FEATUTE_DATA_PS_MCAST
   case (IDSNetIPFilter_ARRAY_PTR): {
         IIPFilterPriv** temp = (IIPFilterPriv**)mpVar;
         for (int i=0; i<mnLen; i++) {
            if (NULL != temp[i]) {
               DSSCommon::ReleaseIf((IQI**)&(temp[i]));
            }

         }
         PS_SYSTEM_HEAP_MEM_FREE(temp);
      }
      break;

   case (IDSNetQoSFlow_ARRAY_PTR): {
         IQoSFlowPriv** temp = (IQoSFlowPriv**)mpVar;
         for (int i=0; i < mnLen; i++) {
            if (NULL != temp[i]) {
               DSSCommon::ReleaseIf((IQI**)&(temp[i]));
            }
         }
         PS_SYSTEM_HEAP_MEM_FREE(temp);
      }
      break;
   default:
      LOG_MSG_ERROR_2("DSSGenScope::Release switch is in default, this: 0x%x, mVarType: %d",
                      this, mVarType);
      break;
   }
}
/*lint –restore */
