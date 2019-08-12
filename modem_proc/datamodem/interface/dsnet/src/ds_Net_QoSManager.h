#ifndef DS_NET_QOS_MANAGER_H
#define DS_NET_QOS_MANAGER_H

/*===========================================================================
  @file QoSManager.h

  This file defines the class that implements the IQoSManager interface.

  The QoSManager class (ds::Net::QoSManager) implements the following interfaces:
  IQI
  IEventManager
  IQoSManager

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-12-22 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_Factory.h"
#include "ds_Net_IQoSManager.h"
#include "ds_Net_IQoSManagerPriv.h"
#include "ds_Net_Handle.h"
#include "ds_Net_QoSDefault.h"
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
/*lint -esym(1510, IQoSManager) */
/*lint -esym(1510, IQI) */
class QoSManager : public IQoSManager,
                   public IQoSManagerPriv, 
                   public Handle
{
private:
  QoSDefault*             mpQoSDefault;
  ISignalBus *            mpSigBusProfilesChanged;
  NetworkModeType         mNetworkMode;

public:
  QoSManager
  (
    int32           ifaceHandle,
    NetworkModeType networkMode
  );

  /*! 
  @brief
  Destructor of QoSManager object. 
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
  virtual ~QoSManager() throw();

  virtual boolean Process
  (
    void* userDataPtr
  );

  /*-------------------------------------------------------------------------
    Inherited functions from IQoSManager and IQoSManagerPriv.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL RequestSecondary
  (
    const QoSSpecType*             requestedQoSSpec,
    IQoSSecondary**                    session
  );

  virtual ds::ErrorType CDECL RequestBundle 
  (
    const QoSSpecType* specs, 
    int specsLen, 
    QoSRequestOpCodeType opCode,
    IQoSSecondariesOutput** sessions
  );

  virtual ds::ErrorType CDECL GetQosDefault
  (
    IQoS**                      qoSDefaultObj
  );

  virtual ds::ErrorType CDECL RequestNetInitiatedReq
  (
    const QoSSpecNetInitiatedType* pQoSNetInitiatedSpec,  
    IQoSNetInitiatedReq** ppIDSNetQoSInitiatedReq
  );

  virtual ds::ErrorType CDECL RequestFiltrMatch
  (
    void * fltr_info_ptr
  );

  virtual ds::ErrorType CDECL CreateQoSNetInitiated
  (    
    IQoSNetInitiatedReq* pQoSNetInitiatedReq,  
    IQoSNetInitiated**   ppiQoSNetInitiated
  );

  virtual ds::ErrorType CDECL Close
  (
    IQoSSecondariesInput* qosSessions
  );

  virtual ds::ErrorType CDECL Resume
  (
    IQoSSecondariesInput* qosSessions
  );

  virtual ds::ErrorType CDECL Suspend
  (
    IQoSSecondariesInput* qosSessions
  );

  virtual ds::ErrorType CDECL GetSupportedProfiles
  (
    QoSProfileIdType                        *profiles,
    int                                     profilesLen,
    int*                                    profilesLenReq
  );
  
  virtual ds::ErrorType CDECL CreateQoSSecondariesInput 
  (
    IQoSSecondariesInput** newQoSSecondariesInput
  );

  /*-------------------------------------------------------------------------
    Forwarders for Handle.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL OnStateChange
  (
    ISignal*                                signalObj,
    ds::Net::EventType                      eventID,
    IQI**                                   regObj
  );

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_QOS_MANAGER)

  /*-------------------------------------------------------------------------
    Macro to implement IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()
    
    virtual int CDECL QueryInterface (AEEIID iid, void **ppo);                                                                      

};/* class QoSManager */
} /* namespace Net */
} /* namespace ds */

#endif /* DS_NET_QOS_MANAGER_H */

