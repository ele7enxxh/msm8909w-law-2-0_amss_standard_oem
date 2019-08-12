#ifndef DS_UTILS_SIGNAL_FACTORY_H
#define DS_UTILS_SIGNAL_FACTORY_H

/*==========================================================================*/
/*!
  @file
  ds_Utils_SignalFactory.h

  @brief
  This file prvovides the ds::Utils::SignalFactory class.

  @details
  This file provides the ds::Utils::SignalFactory implementation. This
  class implements the ISignalFactory interface for REX/L4 environment.

  SignalFactory()
    Create a SignalFactory object.

  ~SignalFactory()
    Destroy the SignalFactory object and all the signals associated with
    the signal factory.

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_SignalFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-11-26 mt  Created module based on SignalCBFactory.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_MemManager.h"

#include "ds_Utils_ISignalFactory.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_CSignalFactory.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Class ds::Utils::SignalFactory
  --------------------------------

  Implements interface      : ISignalFactory
  Derives from classes      : ISignalFactory
                                -->IQI
                              ds::Utils::Factory
                                -->ITraverser
  Abstract class?           : No.
---------------------------------------------------------------------------*/
namespace ds
{
namespace Utils
{
/*lint -esym(1510, ISignalFactory) */
/*lint -esym(1510, IQI) */
class SignalFactory : public ISignalFactory,
                      public Factory
{

private:
 /**
  @brief
  This is the constructor of the SignalFactory object.

  @details
  Use the static method provided by this class instead to create instances
  of this object.

  @param      None.
  @see        None.
  @return     The constructed object.
  */
  SignalFactory
  (
    void
  );

public:

  /**
  @brief
  Destructor.

  @param      None.
  @see        None.
  @return     The constructed object.
  */
  virtual ~SignalFactory
  (
    void
  )
  throw();

  /**
  @brief
  Creates the Signal/SignalCtl objects.

  @details
  The SignalFactory class provides a factory method to create
  instances of Signal and SignalCtl objects. This method takes a
  SignalHandler object and two parameters that the signal object
  should be created with.

  @param[in]  piSignalHandler - Pointer to a SignalHandler class.
  @param[in]  pSignalHandlerCallBack - Pointer to the client callback function.
  @param[in]  pSignalHandlerBaseParam - Pointer to client data, the parameter to the callback SignalHandlerCallBack.
  @param[out] ppISig - Returned ISignal interface pointer.
  @param[out] ppISigCtl - Returned ISignalCtl object.

  @see        None.
  @return     AEE_SUCCESS - On success.
  @return     AEE_ENOMEMORY - no memory to construct object.
  @note       uArgA function cannot be NULL.
  */
  virtual int CDECL CreateSignal
  (
    ISignalHandler * piHandler,
    SignalHandlerCallBack pSignalHandlerCallBack,
    SignalHandlerBase* pSignalHandlerBaseParam,
    ISignal**        ppISig,
    ISignalCtl**     ppiSigCtl
  )
  throw();

  /* Overloaded new operator */
  void* operator new (unsigned int num_bytes)
  throw()
  {
    (void) num_bytes;
    return ps_mem_get_buf (PS_MEM_DS_UTILS_SIGNAL_FACTORY);
  }

  /* Overloaded delete operator */
  void operator delete (void* objPtr)
  throw()
  {
    PS_MEM_FREE(objPtr);
  }

  /*-------------------------------------------------------------------------
    Defintions of IQI Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS_SINGLETON(ISignalFactory)

  /*-------------------------------------------------------------------------
    CreateInstance method for SignalFactory.
  -------------------------------------------------------------------------*/
  static int CreateInstance
  (
    void*    env,
    AEECLSID clsid,
    void**   newObj
  );

}; /* class SignalFactory */
}  /* namespace Utils */
}  /* namespace ds */


#endif /* DS_UTILS_SIGNAL_CB_FACTORY_H */
