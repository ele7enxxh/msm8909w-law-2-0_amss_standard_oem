#ifndef DS_NET_NETWORK_1X_H
#define DS_NET_NETWORK_1X_H

/*===========================================================================
  @file Network1X.h

  This file defines the class that implements the INetwork1x interface.

  TODO: Detailed explaination about the class here.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Network1X.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_INetwork1x.h"
#include "ds_Net_INetwork1xPriv.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Platform.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Network.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, INetwork1x) */
/*lint -esym(1510, INetwork1xPriv) */
/*lint -esym(1510, IQI) */
class Network1X : public INetwork1x,
                  public INetwork1xPriv,
                  public Handle
{
private:
  Network*                 mpParent;
  ISignalBus *             mpSigBusHDRRev0RateInertia;
  ISignalBus *             mpSigBusHDRSlottedMode;
  ISignalBus *             mpSigBusHDRChangeSlottedMode;
  Network1xPrivResultCodeType slotModeOpResult;
  boolean                     rev0InertiaOpSuccess;
  Network1xPrivHDRRev0RateInertiaFailureCodeType rev0InertiaFailResult;
  
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
  virtual ~Network1X
  (
    void
  )
  throw();

public:
  Network1X
  (
    Network* pParent, 
    int32 objHandle
  );

  /*!
   @brief
   Construction code that can fail. You must call this function and verify
   it succeeds when creating a Network1X.

   @params     None.
   @return     AEE_SUCCESS or other error code.
   */
  ds::ErrorType Init(void);

  /*---------------------------------------------------------------------
    Inherited functions from INetwork1x.
  ---------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL GetHDRRev0RateInertiaResult (boolean* result);
  virtual ds::ErrorType CDECL QueryDoSSupport (Network1xDoSFlagsType flags, boolean* dosSupported);
  virtual ds::ErrorType CDECL GetMDR (int* MDR);
  virtual ds::ErrorType CDECL SetMDR (int MDR);
  virtual ds::ErrorType CDECL GetQoSNAPriority (int* QoSNAPriority);
  virtual ds::ErrorType CDECL SetQoSNAPriority (int QoSNAPriority);
  virtual ds::ErrorType CDECL GetRLPAllCurrentNAK (Network1xRLPOptionType* RLPAllCurrentNAK);
  virtual ds::ErrorType CDECL SetRLPAllCurrentNAK (const Network1xRLPOptionType* RLPAllCurrentNAK);
  virtual ds::ErrorType CDECL GetRLPDefCurrentNAK (Network1xRLPOptionType* RLPDefCurrentNAK);
  virtual ds::ErrorType CDECL SetRLPDefCurrentNAK (const Network1xRLPOptionType* RLPDefCurrentNAK);
  virtual ds::ErrorType CDECL GetRLPNegCurrentNAK (Network1xRLPOptionType* RLPNegCurrentNAK);
  virtual ds::ErrorType CDECL GetHysteresisTimer (int* HysteresisTimer);
  virtual ds::ErrorType CDECL SetHysteresisTimer (int HysteresisTimer);
  virtual ds::ErrorType CDECL GetSIPMIPCallType(Network1xSIPMIPCallType* callType);


  /*---------------------------------------------------------------------
    Inherited functions from INetwork1xPriv.
  ---------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL EnableHoldDown (boolean enable);
  virtual ds::ErrorType CDECL EnableHDRRev0RateInertia (boolean enable);
  virtual ds::ErrorType CDECL EnableHDRSlottedMode (const Network1xPrivHDRSlottedModeArg* arg);
  virtual ds::ErrorType CDECL GetHDRSlottedModeResult (Network1xPrivResultCodeType* resultCode);
  virtual ds::ErrorType CDECL GetHDRSlottedModeCycleIndex (uint32 * sci);
  virtual ds::ErrorType CDECL EnableHDRHPTMode (boolean enable);
  virtual ds::ErrorType CDECL GetDormancyTimer (int* DormancyTimer);
  virtual ds::ErrorType CDECL SetDormancyTimer (int DormancyTimer);
  virtual ds::ErrorType CDECL GetSessionTimer (Network1xPrivSessionTimerType* SessionTimer);
  virtual ds::ErrorType CDECL SetSessionTimer (const Network1xPrivSessionTimerType* SessionTimer);
  virtual ds::ErrorType CDECL GetHDR1xHandDownOption (boolean* HDR1xHandDownOption);
  virtual ds::ErrorType CDECL SetHDR1xHandDownOption (boolean HDR1xHandDownOption);
  virtual ds::ErrorType CDECL GetHDRRev0RateInertiaResultInfoCode (Network1xPrivHDRRev0RateInertiaFailureCodeType* failureCode);


  virtual boolean Process
  (
    void     *pUserData
  );
  
  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  /**
  @brief      This method implements the QueryInterface() method of the
              IQI interface.

              Supported interfaces:
              AEEIID_INetwork
              AEEIID_INetwork1x
              AEEIID_IQI

  @param[in]  iid - Interface ID.
  @param[out] ppo - Out interface.

  @see        No external dependencies

  @return     AEE_SUCCESS: On Success
              AEE_ECLASSNOSUPPORT: If IID is not supported by this object.
  */
  virtual ds::ErrorType CDECL QueryInterface
  (
    AEEIID iid,
    void **ppo
  )
  throw();

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
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_NETWORK_1X)

  /*-------------------------------------------------------------------------
    Macros to implement IWeakRef/IQI methods.
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

}; /* class Network1X */
} /* namespace Net */
} /* namespace ds */

#endif

