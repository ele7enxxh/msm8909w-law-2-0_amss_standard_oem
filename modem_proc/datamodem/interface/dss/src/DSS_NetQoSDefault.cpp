/*======================================================

FILE:  DSS_NetQoSDefault.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSNetQoSDefault class

=====================================================

Copyright (c) 2008-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetQoSDefault.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "DSS_Common.h"
#include "dserrno.h"

#include "DSS_NetQoSDefault.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"

using namespace ds::Net;
//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================

//===================================================================
//            DSSNetQoSDefault Functions Definitions
//===================================================================

// TODO: documentation
DSSNetQoSDefault::DSSNetQoSDefault(IQoS* pNetQoSDefault):
   mpNetQoSDefault(pNetQoSDefault)
{
   if (NULL != pNetQoSDefault) {
      (void)mpNetQoSDefault->AddRef();
   }
}



//===================================================================
//  FUNCTION:   DSSNetQoSDefault::~DSSNetQoSDefault
//
//  DESCRIPTION:
//  Destructor of the DSSNetQoSDefault class.
//===================================================================
/*lint -e{1551} */
DSSNetQoSDefault::~DSSNetQoSDefault() 
{
   // release the Default QoS Session
   DSSCommon::ReleaseIf((IQI**)&mpNetQoSDefault);
}
/*lint –restore */

//===================================================================


void * DSSNetQoSDefault::operator new
(
   unsigned int numBytes
)  throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_QOS_DEFAULT_TYPE);
} /* DSSNetQoSDefault::operator new() */


void DSSNetQoSDefault::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSNetQoSDefault::operator delete() */


