#ifndef MTPD_REG_H
#define MTPD_REG_H

/*===========================================================================
  @file ds_Net_MTPDReg.h

  This file defines the class that implements the MTPDReg interface.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MTPDReg.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-10-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Handle.h"

namespace ds
{
namespace Net
{
/*lint -esym(1510, IQI) */
class MTPDReg : public Handle
{
private:
  int32 mMTPDHandle;
  IQI*  mRegObj;
  ISignalBus* mpSigBusMTPD;  

public:
  MTPDReg
  (
    int32 ifaceHandle,
    int32 mtpdHandle
  )
  throw();

  /*!
  @brief
  Destructor of MTPD reg object.

  @param      None.
  @return     None.
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
  virtual ~MTPDReg
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    INode interface Methods
  -------------------------------------------------------------------------*/
  virtual boolean Process
  (
    void*                                   userDataPtr
  );

  /*-------------------------------------------------------------------------
    Forwarders for Handle.
  -------------------------------------------------------------------------*/
  virtual ds::ErrorType CDECL OnStateChange 
  (
    ISignal*                                signalObj,
    ds::Net::EventType                      eventID,
    IQI**                                   regObj
  )
  {
    return Handle::OnStateChange(signalObj, eventID, regObj);
  }

  inline void SetRegObj(IQI* regObj)
  {
      mRegObj = regObj;
  }

  inline IQI* GetRegObj()
  {
     return mRegObj;
  }

  virtual ds::ErrorType GetSignalBus
  (
    ds::Net::EventType  eventID,
    ISignalBus **       ppISigBus
  );

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_MTPD_REG)

  /*-------------------------------------------------------------------------
    Macros to define IWeakRef/IQI methods
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS()


}; /* class MTPDSpec */

} /* namespace Net */
} /* namespace ds */

#endif /* MTPD_REG_H */
