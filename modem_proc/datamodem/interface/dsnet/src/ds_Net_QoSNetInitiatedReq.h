#ifndef DS_NET_QOS_NET_INITIATED_REQ_H
#define DS_NET_QOS_NET_INITIATED_REQ_H

/*=========================================================================*/
/*!
  @file 
  ds_Net_QoSNetInitiatedReqReq.h

  @brief
  This file defines the class that implements the IQoSNetInitiatedReq interface.
  It represents a request object to monitor establishment of network initiated 
  QoS.

  @details
  The QoSNetInitiatedReq class (ds::Net::QoSNetInitiatedReq) implements the following
  interfaces:
  1. IQI
  2. ds::Net::IEventManager
  3. ds::Net::IQoSNetInitiatedReq  

  IQoSNetInitiatedReq instance can be obtained by calling IQoSManager interfaces
  RequestNetInitiatedReq() method. There is no CreateInstance() method.

  IQoSNetInitiatedReq object represents registration of applications to receive
  notification when matching Network Initiated QoS is established. 

  Applications use the IQoSNetInitiatedReq interface to register for 
  events of interests. Supported events include:
  1. ds::Net::QoSNetInitiatedReqEvent::STATE_CHANGED

  QoSNetInitiatedReqEvent::STATE_CHANGED:
  After creating the Network Initiated QoS, the application should explicitly
  register for this event to monitor initiation of matching QoS by the Network.
  
  application can query the current state of the interface using GetState().
  The set of valid states include: AVAILABLE_MODIFIED, SUSPENDING, 
  UNVAILABLE, INVALID, ACTIVATING, DEACTIVATING, CONFIGURING, RELEASING. 
  Please see ds::Net::QoSNetInitiatedReqEvent for more documentation about the 
  meaning of these.

  @see  
  Interface ds::Net::IQoSManager
  Interface ds::Net::INetwork

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSNetInitiatedReq.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2011-07-07 sz  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IQoSNetInitiatedReq.h"
#include "ds_Net_QoS.h"

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
class QoSNetInitiatedReq: public IQoSNetInitiatedReq,
                          public Handle
{
private:
  int32            mIfaceHandle;
  ISignalBus *     mpSigBusStateChange;

  /*-------------------------------------------------------------------------
    Handles queue for network initiated flow that was compatible with this
    request.
    The flow handle is stored in the queue when 
    IFACE_NET_INITIATED_QOS_AVAILABLE_EV event arrives.
    It is popped deleted the queue when app/DSS asks to fetch it in
    order to create a QoSNetInitiated object.
  -------------------------------------------------------------------------*/
  Utils::List * mMatchingFlowHandlesQueue;

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
  virtual ~QoSNetInitiatedReq
  (
    void 
  ) 
  throw();

public:
  /*!
  @brief      
  Constructor. 

  @details
  Construct the QoSNetInitiatedReq object.

  @param      ifaceHandle              - Iface Handle.
  @param      qosNetInitiatedReqHandle - Handle to ps_iface object. Provided
                                         from ps_iface.
  @return     None.
  @see        IQI::Release()
  */
  QoSNetInitiatedReq
  (
    int32 ifaceHandle,
    int32 qosNetInitiatedReqHandle
  );

  /*!
  @brief
  Close

  @details
  Stop monitoring for Network Initiated QoS on this object.

  @param      None.
  @return     None.
  @see        None.
  */
  virtual ds::ErrorType CDECL Close
  (
     void
  );

  /*!
  @brief      
  Fetch the flow handle of last matching
  flow for this Network Initiated QoS Request.

  @param[out]  pFlowHandle - flow handle of matching flow.

  @return AEE_SUCCESS - flow handle provided
  @return QDS_EFAULT - Invalid arguments.
  */
  virtual int CDECL GetLastMatchingFlowHandle 
  (
    int32* pFlowHandle
  );

  /*!
  @brief      
  Indicates if a matching flow is pending for processing.

  @return TRUE - a matching flow is pending for processing.
  @return FALSE - Otherwise.
  */
  virtual boolean CDECL IsPendingMatchingFlow 
  (
    void
  );

  /*!
  @brief
  RealTimeCheck

  @details
  Checks if there is an iface that fulfils QoS requirements. If there is,
  call the callback.
  Should be called for requests that were just created.

  @param      pQoSNetInitiatedSpec - QoS spec.
  @return     AEE_SUCCESS - success.
  @return     Other error values might be returned.
  @see        None.
  */
  virtual ds::ErrorType CDECL RealTimeCheck
  (
    const QoSSpecNetInitiatedType* pQoSNetInitiatedSpec
  );

  /*-------------------------------------------------------------------------
    Method to check for duplicate flows.
    @param      matching queue list.
    @param      flow.
    @return     TRUE/FALSE
    @see        None.
  -------------------------------------------------------------------------*/
  boolean CheckDup
  (
    Utils::List * list,
    int32 *flow
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_QOS_NET_INITIATED_REQ)

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
  );

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
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

}; /* class QoSNetInitiatedReq */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_NET_INITIATED_REQ_H */



