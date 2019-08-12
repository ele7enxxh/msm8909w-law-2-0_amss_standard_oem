#ifndef DS_NET_QOS_DEFAULT_H
#define DS_NET_QOS_DEFAULT_H
/*===========================================================================
  @file QoSDefault.h

  This file defines the class that also implements the IQoS 
  interface.

  TODO: Detailed explaination about the class here.

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSDefault.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-25 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IQoSDefaultPriv.h"
#include "ds_Net_QoS.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_INetworkFactory.h"

/*===========================================================================
                           FORWARD DECLERATION
===========================================================================*/

/*---------------------------------------------------------------------------
  Default Qos interface
---------------------------------------------------------------------------*/

namespace ds
{
namespace Net
{

/*lint -esym(1510, IQoS) */
/*lint -esym(1510, IQI) */
class QoSDefault: public IQoSDefaultPriv, 
                  public QoS
{
private:
  ISignalBus *      mpSigBusInfoCodeUpdated;
  ISignalBus *      mpSigBusModifyResult;
  ISignalBus *      mpSigBusDefaultModify;

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
  virtual ~QoSDefault
  (
    void 
  ) 
  throw();

public:
  QoSDefault
  (
    int32 ifaceHandle,
    int32 flowHandle,
    NetworkModeType networkMode
  );


  /* Inherited from IQoSPriv */
  virtual ds::ErrorType CDECL GetGrantedFlowSpecPriv 
  (
    IQoSFlowPriv** rxFlow, 
    IQoSFlowPriv** txFlow
  );

  /* Inherited functions from IQoSDefaultPriv */
  virtual ds::ErrorType CDECL ModifyDefaultPriv 
  (
    const QoSSpecPrimaryType* requestedSpec, 
    QoSModifyMaskType mask
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_QOS_DEFAULT)

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

  /*---------------------------------------------------------------------
    Inherited functions from IQoS. Call QoS class forwarders.
  ---------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetTXPhysLink 
  (
    IPhysLink** txPhysLinkObj
  );

  virtual ds::ErrorType CDECL GetRXPhysLink 
  (
    IPhysLink** rxPhysLinkObj
  );

  virtual ds::ErrorType CDECL GetModifyResult 
  (
    QoSResultType* result
  );
  
  virtual ds::ErrorType CDECL GetUpdatedInfoCode 
  (
    QoSInfoCodeType* infoCode
  );

  virtual ds::ErrorType CDECL GetTXQueueLevel 
  (
    QoSTXQueueLevelType* TXQueueLevel
  );

  virtual ds::ErrorType CDECL GetTechObject 
  (
    AEEIID iid,
    void** ppo
  );

  virtual ds::ErrorType CDECL GetState 
  (
    QoSStateInfoType* state
  );

  virtual int32 CDECL GetFlowPriv();
  
  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI interface
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

}; /* class QoSDefault */

} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_DEFAULT_H */

