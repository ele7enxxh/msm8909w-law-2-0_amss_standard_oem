#ifndef DS_UTILS_SIGNAL_CTL_H
#define DS_UTILS_SIGNAL_CTL_H
/*==========================================================================*/
/*!
  @file
  ds_Utils_SignalCtl.h

  @details
  This file provides the ds::Utils::SignalCtl class. This class implements
  the ISignalCtl interface.

  SignalCtl()
    Constructor for the SignalCtl object. This can be called only by
    SignalCBFactory class.

  ~Signal()
    Destructor for the SignalCtl object.

  Enable()
    Enables the SignalCtl object, so that the Signal object associated with
    this SignalCtl object can also be enabled.

  Detach()
    Disables the SignalCtl object. No signals can be set on Signal object
    associated with this SignalCtl object.

  Set()
    Set the signal on the SignalCtl object.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_SignalCtl.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_MemManager.h"

#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_Signal.h"

#include "ps_mem_ext.h"
#include "ps_mem.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
/*lint -esym(1510, ISignalCtl) */
/*lint -esym(1510, ISignal) */
/*lint -esym(1510, IQI) */

/*!
  @class
  ds::Utils::SignalCtl

  @brief
  Implements the ISignalCtl interface.

  @details
  TODO
*/
class SignalCtl : public ISignalCtl
{
private:
  Signal*             sigPtr;

  /*!
    @brief
    Constructor

    @param[in]  pISignal - Signal object for this controller.
    @return     The constructed SignalCtl object.
  */
  SignalCtl
  (
    Signal*     pSignal
  )
  throw();

  /*!
    @brief
    Destructor.

    @param      None.
    @return     None.
  */
  virtual ~SignalCtl
  (
    void
  )
  throw();

public:
  /*!
    @brief
    Detaches the signal.

    @details
    Detach() severs the relationship between a signal and its handler or
    callback.  No further invocations will be initiated, and the observer's
    signal handler (if any) will be released.

    In a multi-threaded environment it cannot be guaranteed that after
    Detach() returns there will be no further calls to a signal's callback or
    to its signal handler's Notify() method because an invocation may already
    be about to commence in another thread when Detach() is called.
    Clients must rely on reference counting and wait for the signal handler
    to be released.

    @param      None.
    @return     AEE_SUCCESS - on success
  */
  virtual ds::ErrorType CDECL Detach
  (
    void
  )
  throw();

  /*!
    @brief
    Enables the underlying signal object.

    @details
    This function enables the signal. If the signal was in the set state
    a callback will be scheduled immediately.

    @param      None.
    @return     AEE_SUCCESS - on success
  */
  virtual ds::ErrorType CDECL Enable
  (
    void
  )
  throw();

  /*!
    @brief
    Sets the signal - the difference between this and Signal::Set is that
    even if the signal is not enabled a callback will be scheduled immediately.

    @param      None.
    @return     AEE_SUCCESS - on success
  */
  virtual ds::ErrorType CDECL Set
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    Defintions of IQI Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(ISignalCtl)

private:
  /* Overloaded new operator */
  void* operator new (unsigned int num_bytes)
  throw()
  {
    (void) num_bytes;
    return ps_mem_get_buf (PS_MEM_DS_UTILS_SIGNAL_CTL);
  }

  /* Overloaded delete operator */
  void operator delete (void* objPtr)
  throw()
  {
    PS_MEM_FREE(objPtr);
  }

  /* Used to create a SignalCtl */
  static SignalCtl *CreateInstance
  (
    Signal*     pSignal
  )
  throw();
  friend class SignalFactory;

}; /* class SignalCtl */

} /* namespace Utils */
}/* namespace ds */

#endif /* DS_UTILS_SIGNAL_CTL_H */

