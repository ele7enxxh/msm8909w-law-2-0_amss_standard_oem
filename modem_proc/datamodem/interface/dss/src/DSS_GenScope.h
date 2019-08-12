#ifndef __DSS_GENSCOPE_H__
#define __DSS_GENSCOPE_H__

/*===================================================

FILE:  DSS_GenScope.h

SERVICES:
A utility class to automatically release various variables 
when out of scope

=====================================================

Copyright (c) 2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_GenScope.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

// You should define instance of this class with pointer to some (initialized) object 
// and type of this object , when you wish memory to be automatically freed on scope exit

#include "DSS_Common.h"

class DSSGenScope {
public:
   typedef enum {
      NONE,
      IDSIQI_TYPE,
      DS_Network_DomainName_ARRAY_PTR,
      IDSNetQoSSecondary_ARRAY_PTR,
      IDSNetMCastSession_ARRAY_PTR,
      IDSNetIPFilter_ARRAY_PTR,
      IDSNetQoSFlow_ARRAY_PTR,
      IDSNetFirewallRule_ARRAY_PTR,
      GEN_SCRATCHPAD_ARRAY,
      Scope_ARRAY
   } ScopeVariableType ;


   DSSGenScope(void* var = NULL, ScopeVariableType vType = NONE, int nLen = 0):mpVar(var), mnLen(nLen), mVarType(vType) {};
   void SetParams(void* var = NULL, ScopeVariableType vType = NONE, int nLen = 0);
   ~DSSGenScope();
   void Release();

private:
   void* mpVar;
   int mnLen;
   ScopeVariableType mVarType;
};

inline void DSSGenScope::SetParams(void* val, ScopeVariableType vType, int nLen)
{
   mpVar = val;
   mVarType = vType;
   mnLen = nLen;
}

#endif // __DSS_GENSCOPE_H__
