#ifndef DS_NET_QOS_NET_INITIATED_H
#define DS_NET_QOS_NET_INITIATED_H

/*=========================================================================*/
/*!
  @file 
  ds_Net_QoSNetInitiated.h

  @brief
  This file defines the class that implements the IQoSNetInitiated interface.

  @details
  The QoSNetInitiated class (ds::Net::QoSNetInitiated) implements the following
  interfaces:
  1. IQI
  2. ds::Net::IEventManager
  3. ds::Net::IQoSNetInitiated
  4. ds::Net::IQoS

  IQoSNetInitiated instance can be obtained by calling IQoSManager interface
  CreateQoSNetInitiated() method. There is no CreateInstance() method.

  IQoSNetInitiated object represents Network Initiated Flow.
  It can be created upon notification that Network initiates QoS which matches
   application specified filters in an existing IQoSNetInitiatedReq object.

  Applications use the IQoSNetInitiated interface to register for 
  events of interests. Supported events include:
  1. ds::Net::QoSEvent::QDS_EV_STATE_CHANGED
  2. ds::Net::QoSEvent::QDS_EV_MODIFY_RESULT
  3. ds::Net::QoSEvent::QDS_EV_INFO_CODE_UPDATED
  
  QoSEvent::STATE_CHANGED:
  After creating the Network Initiated Qos, the application should explicitly
  register for this event to monitor state change and release of the QoS by the
  Network.
  
  Application can query the current state of the interface using GetState().
  The set of valid states include: AVAILABLE_MODIFIED, SUSPENDING, 
  UNVAILABLE, INVALID, ACTIVATING, DEACTIVATING, CONFIGURING, RELEASING. 
  Please see ds::Net::QoSNetInitiated::GetState for more documentation about the 
  meaning of these.

  @see
  Interface ds::Net::IQoS
  Interface ds::Net::IQoSManager
  Interface ds::Net::INetwork

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSNetInitiated.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-07 sz  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IQoSNetInitiated.h"
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
class QoSNetInitiated: public IQoSNetInitiated,
                       public QoS
{
private:
   ISignalBus *     mpSigBusStateChange;
   ISignalBus *     mpSigBusModifyResult;
   ISignalBus *     mpSigBusInfoCodeUpdated;

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
  virtual ~QoSNetInitiated
  (
    void 
  ) 
  throw();

public:
  /*!
  @brief      
  Constructor. 

  @details
  Construct the QoSNetInitiated object. This operation can be performed
  after the lower layers have notified creation of corresponding
  QOS_NET_INITIATED. The flow handle used in the constructor is a alias to the
  ps_iface_flow_ptr allocated by lower layers and provided to the QoSNetInitiated
  Request object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  QoSNetInitiated 
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
  This function can be used to query the flow specifications of the Network
  Initiated QoS.

  @param[out] pRXFlowSpec - RX Flow granted.
  @param[out] pTXFlowSpec - TX Flow granted.

  @see   ds::Net::IQoSFlowSpec - Flow specification interface.

  @return AEE_SUCCESS    - if request is received successfully
  @return QDS_EFAULT - Parameters are invalid.
  @return QDS_EINVAL - Invalid state of QoSNetInitiated object
  */
  virtual ds::ErrorType CDECL GetGrantedFlowSpecPriv 
  (
    IQoSFlowPriv** rxFlow,
    IQoSFlowPriv** txFlow
  );

  /**
  @brief 
  This function returns the current Rx and Tx filter specification.
  @details
  This function returns the current Rx and Tx filter specification.

  @param[out] rxFilter the current Rx filter specification.
  @param[in]  rxFilterLen Length of sequence
  @param[out] rxFilterLenReq Required length of sequence
  @param[out] txFilter the current Tx filter specification.
  @param[in]  txFilterLen Length of sequence
  @param[out] txFilterLenReq Required length of sequence

  @return AEE_SUCCESS Request received successfully.
  @return ds::EBADOPTS Bad options.
  @return Other ds designated error codes might be returned.

  @see ds_Errors_Def.idl.
  */
  virtual ds::ErrorType CDECL GetGrantedFilterSpec
  (
    IIPFilterPriv** rxFilter,
    int rxFilterLen,
    int* rxFilterLenReq,
    IIPFilterPriv** txFilter,
    int txFilterLen,
    int* txFilterLenReq
  );

  /*!
  @brief      
  Fetch the status of QoS link.

  @param[out]  pState - State of the QoS link.

  @see    ds::Net::QoSStateType
  @see    ds::Net::QOSInfoCodeType

  @return AEE_SUCCESS - if request is received successfully
  @return QDS_EFAULT - Invalid arguments.
  */
  virtual ds::ErrorType CDECL GetState 
  (
    QoSStateInfoType* pState
  );

  /*!
  @brief      
  Returns an identifier which identify this object.

  @return identifier which identify this object.
  */
  virtual int32 CDECL GetQoSNetInitiatedId();

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_QOS_NET_INITIATED)

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
    return AEE_EUNSUPPORTED;
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

}; /* class QoSNetInitiated */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_NET_INITIATED_H */



