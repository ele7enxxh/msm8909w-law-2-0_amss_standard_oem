#ifndef __DSS_QOSNETINITIATEDREQ_H__
#define __DSS_QOSNETINITIATEDREQ_H__

/*======================================================

FILE:  DSS_NetQoSNetInitiatedReq.h

SERVICES:
Backward Compatibility Layer QoS Net Initiated Request class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSNetInitiatedReq.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2011-06-07 sz  Created.

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
#include "dssocket_defs.h"
#include "ds_Utils_IWeakRef.h"
#include "ds_Utils_IWeakRefSupport.h"
#include "ds_Net_IQoSNetInitiatedReq.h"

#include "dss_iface_ioctl.h"

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
//  CLASS:      DSSQoSNetInitiatedReq
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

// Forward declarations to prevent circular inclusion of DSSQoSNetInitiatedReq.
class DSSEventHandler;
class DSSQoSNetInitiatedHandler;
class DSSQoSNetInitiatedReqHandler;
class DSSNetActive;

using ds::Utils::IWeakRef;

class DSSQoSNetInitiatedReq : public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:

    DSSQoSNetInitiatedReq(ds::Net::IQoSNetInitiatedReq* pQoSNetInitiatedReq, 
                          dss_qos_net_initiated_req_handle_type handle,
                          dss_iface_id_type ifaceId);
    virtual ~DSSQoSNetInitiatedReq() throw() {}
    void InsertToList(DSSQoSNetInitiatedReq* pDSSQoSNetInitiated);
    virtual void Destructor() throw();    

    /*-------------------------------------------------------------------------
    Defintions of IQI and IWeakRef Methods
    -------------------------------------------------------------------------*/
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:

    AEEResult Close();
//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------


    // Set() and Get() of parent DSSNetActive object.
    // GetDSSNetActive() returns NetActive with storng ref, or NULL if
    // NetActive freed already. who calls GetDSSNetActive(), should call
    // Release() on NetActive when done.
    AEEResult GetDSSNetActive(DSSNetActive** pDSSNetActive);

    void SetDSSNetActive(DSSNetActive* pDSSNetActive);
    
    void GetHandle(dss_qos_net_initiated_req_handle_type* pHandle);
    void SetHandle(dss_qos_net_initiated_req_handle_type handle);  

    inline void GetNetHandle(sint15* pNetHandle)
    {
      *pNetHandle = mNetHandle;
    }
    inline void SetNetHandle(sint15 netHandle)
    {
      mNetHandle = netHandle;
    }

    void GetIfaceId(dss_iface_id_type* pIfaceId);

    AEEResult GetNext(DSSQoSNetInitiatedReq** ppDSSQoSNetInitiated) throw(){
       *ppDSSQoSNetInitiated = mNext;
       return AEE_SUCCESS;
    }

    AEEResult GetQoSNetInitiatedReq(ds::Net::IQoSNetInitiatedReq** ppQoSNetInitiatedReq); 

    AEEResult RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                         dss_iface_id_type ifaceId);
    
    AEEResult RegCB(dss_iface_ioctl_ev_cb_type* pEvArg);
    
    AEEResult CreateDSSQoSNetInitiated(dss_qos_handle_type qos_handle);
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
     ds::Net::IQoSNetInitiatedReq* mpQoSNetInitiatedReq;
     DSSQoSNetInitiatedReq* mNext;     // DSSQoSNetInitiatedReq list
     
     // handle identifying the application request object in dss
     dss_qos_net_initiated_req_handle_type  mHandle;  
     dss_iface_id_type mIfaceId;
     DSSQoSNetInitiatedReqHandler* mpQoSNetInitiatedReqHandler;
     DSSNetActive* mParentNetActive;

     // Application CB is stored in DSSQoSNetInitiatedReq so it can be provided
     // to newly created DSSQoSNetInitiated corresponding to this request.
     // QoS events on those objects are provide to app via the same CB.
     dss_iface_ioctl_ev_cb_type mCbType;
     sint15 mNetHandle;  //netHandle of application


  public :

     void * operator new (
        unsigned int numBytes
     )  throw();

     void operator delete (
        void *  bufPtr
     );

};

//===================================================================

#endif // __DSS_QOSNETINITIATEDREQ_H__
