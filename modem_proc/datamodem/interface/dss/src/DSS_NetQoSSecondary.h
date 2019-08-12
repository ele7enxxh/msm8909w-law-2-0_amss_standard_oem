#ifndef __DSS_NETQOSSECONDARY_H__
#define __DSS_NETQOSSECONDARY_H__

/*======================================================

FILE:  DSS_NetQoSSecondary.h

SERVICES:
Backward Compatibility Layer Secondary QoS class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetQoSSecondary.h#1 $
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
#include "DSS_QoS.h"

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
//  CLASS:      DSSNetQoSSecondary
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

// Forward declarations to prevent circular inclusion of DSSNetQoSSecondary.
class DSSEventHandler;
class DSSQoSHandler;
class DSSQoSModifyHandler;
class DSSQoSInfoCodeUpdatedHandler;

class DSSNetQoSSecondary : public DSSQoS
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  public:

    DSSNetQoSSecondary(ds::Net::IQoSSecondary* pNetQoSSecondary, uint32 flowID);
    virtual ~DSSNetQoSSecondary() throw() {}
    void InsertToList(DSSNetQoSSecondary* pDSSQoS);
    virtual void Destructor() throw();
    void GetFlowID(uint32* pFlowID);
    virtual void GetHandle(dss_qos_handle_type* pHandle);

    /*-------------------------------------------------------------------------
    Defintions of IQI and IWeakRef Methods
    -------------------------------------------------------------------------*/
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------


    // Set() and Get() of parent DSSNetActive object.
    // GetDSSNetActive() returns NetActive with storng ref, or NULL if
    // NetActive freed already. who calls GetDSSNetActive(), should call
    // Release() on NetActive when done.
    virtual AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive);

    virtual void SetDSSNetActive(DSSNetActive* pDSSNetActive);

    AEEResult GetNext(DSSNetQoSSecondary** ppDSSNetQoS) throw(){
       *ppDSSNetQoS = mNext;
       return AEE_SUCCESS;
    }

    virtual AEEResult GetQoS(ds::Net::IQoS** ppQoS);
    AEEResult GetNetQoSSecondary(ds::Net::IQoSSecondary** ppNetQoSSecondary); 

    AEEResult RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                         dss_iface_id_type ifaceId);

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------
  protected:
    template<typename HandlerType>
    AEEResult FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit);

    AEEResult GetEventHandler(dss_iface_ioctl_event_enum_type event, DSSEventHandler** ppEventHandler, bool bInit);

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
     ds::Net::IQoSSecondary* mpNetQoSSecondary;
     DSSNetQoSSecondary* mNext;     // DSSNetQoS list
     uint32 mFlowID;
     DSSQoSHandler* mpNetQoSHandler;
     DSSQoSModifyHandler* mpNetQoSModifyHandler;
     DSSQoSInfoCodeUpdatedHandler* mpQoSInfoCodeUpdatedHandler;
     DSSNetActive* mparentNetActive;

  public :

     void * operator new (
        unsigned int numBytes
     )  throw();

     void operator delete (
        void *  bufPtr
     );

};

//===================================================================

#endif // __DSS_NETQOSSECONDARY_H__
