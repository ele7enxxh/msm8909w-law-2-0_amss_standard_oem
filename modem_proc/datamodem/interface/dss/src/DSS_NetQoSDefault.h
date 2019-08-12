#ifndef __DSS_NETQOSDEFAULT_H__
#define __DSS_NETQOSDEFAULT_H__

/*======================================================

FILE:  DSS_NetQoSDefault.h

SERVICES:
Backward Compatibility Layer Secondary QoS class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetQoSDefault.h#1 $
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
#include "comdef.h"
#include "customer.h"
#include "dssocket.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"

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
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSNetQoS
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

class DSSNetQoSDefault
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:

    DSSNetQoSDefault(ds::Net::IQoS* pNetQoSDefault);
    ~DSSNetQoSDefault();

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------

    inline AEEResult GetNetQoSDefault(ds::Net::IQoS** ppNetQoSDefault) {
       *ppNetQoSDefault = mpNetQoSDefault;
       (void)mpNetQoSDefault->AddRef();
       return AEE_SUCCESS;
    }

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
     ds::Net::IQoS* mpNetQoSDefault;

  public :

   void * operator new (
      unsigned int numBytes
   )  throw();

   void operator delete (
      void *  bufPtr
   );
};

//===================================================================

#endif // __DSS_NETQOSDEFAULT_H__
