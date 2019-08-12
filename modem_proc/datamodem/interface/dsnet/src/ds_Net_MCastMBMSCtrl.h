#ifndef MCAST_MBMS_CTRL_H
#define MCAST_MBMS_CTRL_H
#ifdef FEATUTE_DATA_PS_MCAST
/*==========================================================================*/
/*!
  @file
  ds_Net_MCastMBMSCtrl.h

  @brief
  This file defines the class that implements the IMCastMBMSCtrl 
  interface.

  @details
  TODO

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastMBMSCtrl.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-09-12 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"

namespace ds
{
namespace Net
{
/*lint -esym(1510, IMCastMBMSCtrlPriv) */
/*lint -esym(1510, IQI) */
class MCastMBMSCtrl : public IMCastMBMSCtrlPriv,
                      public Handle

{

private:
  int32                    mMBMSHandle;
  ISignalBus *             mpSigBusStateChange;

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
  virtual ~MCastMBMSCtrl
  (
    void
  )
  throw();


public:
  MCastMBMSCtrl(int32 ifaceHandle, int32 mbmsHandle);

  /* Functions derived from IMCastMBMSCtrlPriv interface */
  virtual ds::ErrorType CDECL DeActivate (void);
  virtual ds::ErrorType CDECL GetState (MBMSStateType* status);

  /* Methods from Handle class */
  virtual ds::ErrorType CDECL OnStateChange 
  (
    ::ISignal*         signalObj,
    ds::Net::EventType eventID,
    IQI**              regObj
  )
  {
    return Handle::OnStateChange(signalObj, eventID, regObj);
  }

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );

  /*  Definitions of IQI Methods */
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_NETWORK_MBMS)

  /*-------------------------------------------------------------------------
    Macro to define IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()

};/* Class MCastMBMSCtrl */

} /* namespace Net */
} /* namespace ds */

#endif // FEATUTE_DATA_PS_MCAST
#endif /* MCAST_MBMS_CTRL_H */
