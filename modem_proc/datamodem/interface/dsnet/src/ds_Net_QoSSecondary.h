#ifndef DS_NET_QOS_SECONDARY_H
#define DS_NET_QOS_SECONDARY_H

/*=========================================================================*/
/*!
  @file
  ds_Net_QoSSecondary.h

  @brief
  This file defines the class that implements the IQoSSecondary interface.

  @details
  The QoSSecondary class (ds::Net::QoSSecondary) implements the following
  interfaces:
  1. IQI
  2. ds::Net::IEventManager
  3. ds::Net::IQoSSecondary
  4. ds::Net::IQoS

  IQoSSecondary instance can be obtained by calling IQoSManager interfaces
  RequestSecondary()/RequestSecondaryEx() methods. There is no
  CreateInstance() method.

  IQoSSecondary object represents the secondary flows that can be created
  by the application. Application can perform secondary flow related IOCTLs
  like:
  1. GetState(): getting the status of the flow
  2. Modify(): modifying the specs of the secondary flow.
  3. Suspend(): Suspend the secondary flow.
  4. Resume(): Resume the suspended secondary flow.
  5. Close(): Releasing the secondary flow.
  6. GetGrantedFlowSpec(): Retrieve the flow specifications granted.
  7. GetUpdatedInfoCode(): Retrieve current info-code for the flow.

  Applications primarily use the IQoSSecondary interface to register for
  events of interests. Supported events include:
  1. ds::Net::QoSSecondaryEvent::STATE_CHANGED
  2. ds::Net::QoSEvent::INFO_CODE_UPDATED
  3. ds::Net::QoSEvent::MODIFY_RESULT

  QoSEvent::STATE_CHANGED:
  After creating a flow, the application should explicitly register for
  this event to monitor the state of the flow. If this event is received,
  application can query the current state of the interface using GetState().
  The set of valid states include: AVAILABLE_MODIFIED, SUSPENDING,
  UNVAILABLE, INVALID, ACTIVATING, DEACTIVATING, CONFIGURING, RELEASING.
  Please see ds::Net::QoSState for more documentation about the
  meaning of these.

  QoSEvent::INFO_CODE_UPDATED and QoSEvent::MODIFY_RESULT are explained in
  ds::Net::IQoS documentation.

  @see
  Interface ds::Net::IQoS
  Interface ds::Net::IQoSManager
  Interface ds::Net::INetwork
  ds::Net::QoSState

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSSecondary.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-25 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IQoSSecondary.h"
#include "ds_Net_IQoSSecondaryPriv.h"
#include "ds_Net_QoS.h"
#include "ds_Net_QoSManager.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"

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

/*lint -esym(1510, IQoSSecondary) */
/*lint -esym(1510, QoS) */
/*lint -esym(1510, IQI) */
class QoSSecondary: public IQoSSecondary,
                    public IQoSSecondaryPriv,
                    public QoS
{
private:
  ISignalBus *       mpSigBusInfoCodeUpdated;
  ISignalBus *       mpSigBusModifyResult;
  ISignalBus *       mpSigBusStateChange;
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
  virtual ~QoSSecondary
  (
    void
  )
  throw();

public:
  /*!
  @brief
  Constructor.

  @details
  Construct the QoSSecondary object. This operation can be performed
  after the lower layers have been issued QOS_SECONDARY request. The flow
  handle used in the constructor is a alias to the ps_iface_flow_ptr
  returned as part of IOCTL call.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  QoSSecondary
  (
    int32 ifaceHandle,
    int32 flowHandle,
    NetworkModeType networkMode
  );

  /*-------------------------------------------------------------------------
    Methods from ds::Net::IQoS and ds::Net::IQoSPriv interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Retrieve the flow spec granted by the network.

  @details
  After requesting a certain secondary flow with a flow-spec, it is not
  necessary that the network honored all of the parameters requested. This
  function can be used to query what was granted by the network. This
  function is applicable for both primary and secondary QoS flows.

  @param[out] pRXFlowSpec - RX Flow granted.
  @param[out] pTXFlowSpec - TX Flow granted.

  @note  This function makes sense after QoS is granted by the network
  (GetState() on the interface returns AVAILABLE_MODIFIED)

  @see   ds::Net::IQoSFlowSpec - Flow specification interface.

  @return AEE_SUCCESS    - if request is received successfully
  @return QDS_EFAULT - Parameters are invalid.
  @return QDS_EINVAL - Invalid state of QoSSecondary object (QoS might
  not be granted yet.)
  */
  virtual ds::ErrorType CDECL GetGrantedFlowSpecPriv
  (
    IQoSFlowPriv** rxFlow,
    IQoSFlowPriv** txFlow
  );

  /*-------------------------------------------------------------------------
    Methods from ds::Net::IQoSSecondary interface.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  Closes the secondary flow.

  @details
  Triggers a message to the network to close the secondary flow. Does
  not Release() the QoSSecondary object. Application should call Close(),
  followed by Release().

  @param  None.
  @see    IQoSManager::RequestSecondary()
  @return     AEE_SUCCESS - if request is received successfully
  */
  virtual ds::ErrorType CDECL Close
  (
    void
  );

  /*!
  @brief
  Modify a granted Qos session.

  @details
              This function issues the following modification requests:
              - Modify an existing QoS Session either in Rx, Tx or both.
              - Add a new QoS in a specific direction.
              - Remove a granted QoS in a specific direction.
  The modification requests can be performed on Rx blocks, Tx blocks or both.
  The specific operations above can be requested via the modifyMask.

  @param[in] requestedSpec - Pointer to the modify QOS spec.
  @param[in] modifyMask  - Specify direction where modify be performed.

  @see       IQoSManager::RequestSecondary()
  @see       ds::Net::QoSSpecType
  @see       ds::Net::QoSModifyMaskType
  @see       ds::Net::IQoSFlowSpec - Flow specification interface.

  @return    AEE_SUCCESS - if request is received successfully
  @return    QDS_EFAULT - If modify spec is invald.
  */
  virtual ds::ErrorType CDECL ModifySecondaryPriv
  (
    const QoSSpecType* requestedSpec,
    QoSModifyMaskType modifyMask
  );

  /*!
  @brief
  Resume a suspended flow.

  @details
  This function issues a message to resume a suspended flow. The function
  only issues the request to resume the flow, AEE_SUCCESS from this function
  does not indicate that the flow has been resumed. The application should
  register for ds::Net::QoSSecondaryEvent::STATE_CHANGED, and verify that
  the state of the interface is ds::Net::QoSState::
  AVAILABLE_MODIFIED to ensure that the operation succeeded.

  @param  None.

  @note    Only makes sense for a suspended flow.

  @return AEE_SUCCESS    - if request is received successfully
  @return QDS_EINVAL - State of flow is not suspended.
  */
  virtual ds::ErrorType CDECL Resume
  (
    void
  );

  /*!
  @brief
  Suspend an active flow.

  @details
  This function issues a message to suspend an active flow. The function
  only issues the request to suspend the flow, AEE_SUCCESS from this function
  does not indicate that the flow has been suspended. The application should
  register for ds::Net::QoSSecondaryEvent::STATE_CHANGED, and verify that
  the state of the interface is ds::Net::QoSState::SUSPENDED
  to ensure that the operation succeeded.

  @param  None.

  @see    Only makes sense for a active flow.

  @return AEE_SUCCESS    - if request is received successfully
  @return QDS_EINVAL - State of flow is not AVAILABLE_MODIFIED.
  */
  virtual ds::ErrorType CDECL Suspend
  (
    void
  );

  /*!
  @brief
  Fetch the status of secondary QoS link.

  @param[out]  pState - State of the interface.

  @see    ds::Net::QoSStateType
  @see    ds::Net::QOSInfoCodeType

  @return AEE_SUCCESS - if request is received successfully
  @return QDS_EFAULT - Invalid arguments.
  */
  virtual ds::ErrorType CDECL GetState
  (
    QoSStateInfoType* pState
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_QOS_SECONDARY)

  /*-------------------------------------------------------------------------
    Definitions of IQI Methods
  -------------------------------------------------------------------------*/
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
    Handle interface forwarders:
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL OnStateChange
  (
    ::ISignal*            signalObj,
    ds::Net::EventType    eventID,
    IQI**                 regObj
  )
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (QoSEvent::QDS_EV_STATE_CHANGED == eventID)
    {
      /* Set the signal and let APP query for QoS status */
      (void) signalObj->AddRef();
      (void) signalObj->Set();
      (void) signalObj->Release();
    }

    return Handle::OnStateChange(signalObj, eventID, regObj);
  }

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );
  /*-------------------------------------------------------------------------
    INode interface methods.
  -------------------------------------------------------------------------*/
  virtual boolean Process
  (
    void* pUserData
  );

  /*-------------------------------------------------------------------------
    IQoS interface forwarders.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetTXPhysLink
  (
    IPhysLink** txPhysLinkObj
  )
  {
    return QoS::GetTXPhysLink(txPhysLinkObj);
  }

  virtual ds::ErrorType CDECL GetRXPhysLink
  (
    IPhysLink** rxPhysLinkObj
  )
  {
    return QoS::GetRXPhysLink(rxPhysLinkObj);
  }

  virtual ds::ErrorType CDECL GetModifyResult
  (
    QoSResultType* result
  )
  {
    return QoS::GetModifyResult(result);
  }

  virtual ds::ErrorType CDECL GetUpdatedInfoCode
  (
    QoSInfoCodeType* infoCode
  )
  {
    return QoS::GetUpdatedInfoCode(infoCode);
  }

  virtual ds::ErrorType CDECL GetTXQueueLevel
  (
    QoSTXQueueLevelType* TXQueueLevel
  )
  {
    return QoS::GetTXQueueLevel(TXQueueLevel);
  }

  virtual ds::ErrorType CDECL GetTechObject
  (
    AEEIID iid,
    void** ppo
  )
  {
    return QoS::GetTechObject(iid, ppo);
  }

  virtual int32 CDECL GetFlowPriv();
  
  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

}; /* class QoSSecondary */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_SECONDARY_H */



