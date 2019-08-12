#ifndef DS_NET_PHYS_LINK_H
#define DS_NET_PHYS_LINK_H

/*=========================================================================*/
/*!
  @file 
  ds_Net_PhysLink.h

  @brief
  This file defines the class that implements the IPhysLink and IPhysLinkPriv
  interfaces.

  @details
  The PhysLink class (ds::Net::PhysLink) implements the following
  interfaces:
  1. IQI
  2. ds::Net::IEventManager
  3. ds::Net::IPhysLink
  4. ds::Net::IPhysLinkPriv

  IPhysLink interface can be obtained via INetwork interface's GetTXPhysLink/
  GetRXPhysLink() methods. There is no CreateInstance() method for this.

  PhysLink objects represent the physical channel associated with the 
  network object. The operations supported on IPhysLink interface include:
  1. GetState(): Return the state of the physical channel.
  2. GoDormant(): Force the phys link to go dormant.
  3. GoActive(): Force a dormant to become active.

  PhysLink object also implements the IPhysLinkPriv interface. This interface
  is for backward DSS compatibility only. The exported method from this 
  interface is:
  1. GetPreviousState(): After a state change event happens on phys link,
     this method can be used to query the previous state to populate the
     DSS event info.

  IPhysLink interface also supports registering for 
  ds::Net::PhysLinkEvent::STATE_CHANGED to monitor the state changes on
  the PhysLink object.

  @see Interface ds::Net::IPhysLink
  @see ds::Net::INetwork::GetTXPhysLink()
  @see ds::Net::INetwork::GetRXPhysLink()
  @see ds::Net::PhysLinkEvent::STATE_CHANGED

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_PhysLink.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-04-07 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Handle.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_INetworkFactory.h"

/*===========================================================================
                     FORWARD DECLERATION
===========================================================================*/

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, IPhysLink) */
/*lint -esym(1510, IQI) */
class PhysLink : public IPhysLink,
                 public Handle
{
private:

  ISignalBus *               mpSigBusStateChange;
  NetworkModeType            mNetworkMode;
  ds::Net::PhysLinkStateType mCachedPreviousState;

  /*!
  @brief
  Private destructor.

  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual void Destructor
  (
    void 
  )
  throw();
  
  /*!
  @brief
  Dummy destructor. Only used to free memory. 
  
  @details
  For objects that support weak references, destruction of the object and
  freeing up of memory can happen at different times. Hence Destructor()
  is the one which performs actual destruction of the object, where as 
  ~Object() would perform memory cleanup. 
  @params     None.
  @return     None.
  */
  virtual ~PhysLink
  (
    void
  )
  throw();

public:
  /*!
  @brief
  Constructor.

  @details
  Construct the PhysLink object. This is a privileged operation and cannot
  be directly used by applications. Applications should use INetwork
  methods: GetTXPhysLink() and GetRXPhysLink().

  @param[in]  physLinkHandle - Underlying phys link pointer.
  @return     None.
  @see        IQI::Release()
  @see        ds::Net::INetwork::GetTXPhysLink()
  @see        ds::Net::INetwork::GetRXPhysLink()
  */
  PhysLink 
  (
    int32 physLinkHandle,
    NetworkModeType networkMode
  );

  /*---------------------------------------------------------------------
    Inherited functions from IPhysLink.
  ---------------------------------------------------------------------*/
  /*!
  @brief
  Force a dormant phys link to go dormant (similar to power save mode).

  @details
  This function causes the underlying physical channel to go dormant.
  For most applications, this function is not needed. This is only for
  system applications that want to preserve battery performance and
  other resources by explicitly inducing dormancy.

  Dormancy is a technology-specific behavior. If data transfer is
  requested by any applications during dormancy, the physical channel
  is brought out of dormancy to perform the requested operation. In
  707/1X there is a concept of a linger timer per iface. If the linger
  timer expires, then dormancy is induced automatically until further
  data transfer happens.

  Applications should register for the STATE_CHANGED event on the
  PhysLink object to monitor the state of the phys link. AEE_SUCCESS from
  this function does not indicate that phys link dormancy was accepted.
  The state of the PhysLink object changes to ds::Net::PhysLinkState::
  DOWN after going dormant.

  @param[in]  dormantReason - Reason for forcing dormancy.

  @see     ds::Net::IPhysLink::GoActive()

  @return  AEE_SUCCESS - if request is received successfully
  */
  virtual ds::ErrorType CDECL GoDormant 
  (
    DormantReasonType  dormantReason
  );

  /*!
  @brief
  Force a dormant phys link to go active.

  @details
  For most application, this operation is not needed. This operation is
  for system applications that need to force the phys link out of
  dormancy induced by those applications.

  Applications should register for the STATE_CHANGED event on the
  PhysLink object to monitor the state of the phys link. AEE_SUCCESS from
  this function does not indicate that GoActive() was accepted.
  The state of the PhysLink object changes to ds::Net::PhysLinkState::UP
  after coming out of dormancy.

  @param   None.
  @see     ds::Net::IPhysLink::GoDormant()

  @return  AEE_SUCCESS - if request is received successfully
  */
  virtual ds::ErrorType CDECL GoActive 
  (
    void
  );

  /*!
  @brief
  Fetch the state of the PhysLink object.

  @param[out]  pState - Returned state.
  @see         ds::Net::PhysLinkStateType.
  @return      AEE_SUCCESS - on success.
  @return      QDS_EFAULT - Invalid args.
  */
  virtual ds::ErrorType CDECL GetState 
  (
    PhysLinkStateType*    pState
  );

  /*!
  @brief      
  Fetch the dormancy info code. 
 
  @param[out]  pDormancyInfoCode - Returned info code.
  @see         ds::Net::DormancyInfoCodeType.
  @return      AEE_SUCCESS - on success.
  @return      QDS_EFAULT - Invalid args.
  */
  virtual ds::ErrorType CDECL GetDormancyInfoCode
  (
    ds::Net::DormancyInfoCodeType   *pDormancyInfoCode
  );

  /*-------------------------------------------------------------------------
    Methods from IPhysLinkPriv interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Fetch the previous state of the PhysLink object.

  This is a privileged operation to fetch the previous state of the
  interface. Currently used to populate event info field while posting
  DSS layer event.

  @param[out]  pState - Returned state.
  @see         ds::Net::PhysLinkStateType.
  @return      AEE_SUCCESS - on success.
  @return      QDS_EFAULT - Invalid args.
  */
  virtual ds::ErrorType CDECL GetPreviousState 
  (
    PhysLinkStateType*    pState
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_PHYS_LINK)

  /*-------------------------------------------------------------------------
    Handle interface forwarders:
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL OnStateChange 
  (
    ::ISignal*            signalObj,
    ds::Net::EventType    eventID,
    IQI**                 regObj
  );

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );


  /*-------------------------------------------------------------------------
    Process method for event handling.
  -------------------------------------------------------------------------*/
  virtual boolean Process
  (
    void* pUserData
  );

  /*-------------------------------------------------------------------------
    Macro to implmenet IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(IPhysLink)

}; /* class PhysLink */
} /* Namespace Net */
} /* Namespace ds */

#endif /* DS_NET_PHYS_LINK_H */
