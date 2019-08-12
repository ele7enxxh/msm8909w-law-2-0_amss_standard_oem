#ifndef DS_UTILS_SIGNAL_H
#define DS_UTILS_SIGNAL_H

/*==========================================================================*/
/*!
  @file
  ds_Utils_Signal.h

  @details
  This file provides the ds::Utils::Signal class. This class implements
  the AEEISignal interface. The following methods are exported.

  Signal()
    Constructor for the signal object.

  ~Signal()
    Destructor for the signal object.

  Set()
    Set the signal.


  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_Signal.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_List.h"
#include "ds_Utils_INode.h"
#include "ds_sig_svc.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"
#include "ds_Utils_SignalHandler.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Utils
{
  /* Possible commands for Process() */
  typedef enum
  {
    SIGNAL_PROCESS_ENUM_MIN,
    SIGNAL_PROCESS_ENUM_SET,
    SIGNAL_PROCESS_ENUM_DISCONNECT_SIGNAL_BUS,
    SIGNAL_PROCESS_ENUM_MAX
  } SignalProcessEnumType;

  /* Forward declarations */
  class SignalBus;

  /*! Magic number for checking signal validity */
  #define DS_UTILS_SIGNAL_MAGIC_NUMBER    (0xABC0FFEE)

/*lint -esym(1510, ISignal) */
/*lint -esym(1510, IQI) */

/*!
  @class
  ds::Utils::Signal

  @brief
  Implements the ISignal interface.

  @details
  TODO
*/
class Signal : public ISignal,
               public INode
{
private:
  ISignalHandler *mpiSignalHandler;
  SignalHandlerCallBack mpSignalHandlerCallBack;
  SignalHandlerBase* mpSignalHandlerBaseParam;

  uint32 mMagic; /* Stores the magic number for checking signal validity */
  SignalBus *mpSignalBus;
  uint8 mSigState; /* Signal state - see the .cpp file */

private:
  /* Overloaded new operator */
  void* operator new (unsigned int num_bytes)
  throw()
  {
    (void) num_bytes;
    return ps_mem_get_buf (PS_MEM_DS_UTILS_SIGNAL);
  }

  /* Overloaded delete operator */
  void operator delete (void* objPtr)
  throw()
  {
    PS_MEM_FREE(objPtr);
  }

  /*!
    @brief
    Constructor used by SignalFactory to construct a Signal.

    Since Signals can only be created via a Factory, the constructor is
    private.

    @param[in]  pSignalHandlerCallBack - Registered callback function.
    @param[in]  pSignalHandlerBaseParam - The "me" object.

    @return     The constructed signal object.
  */
private:
  Signal
  (
    ISignalHandler *piSignalHandler,
    SignalHandlerCallBack pSignalHandlerCallBack,
    SignalHandlerBase* pSignalHandlerBaseParam
  ) throw();
  friend class SignalFactory;

  /*!
   * Since operator new is private it can't be used to create a Signal.
   *
   * @return Signal*
   */
  static Signal *CreateInstance(
    ISignalHandler *piSignalHandler,
    SignalHandlerCallBack pSignalHandlerCallBack,
    SignalHandlerBase* pSignalHandlerBaseParam
  )
  throw();

  /*!
    @brief Check if the signal is eligible to be scheduled for future
    execution, and if it is, prepare it for scheduling
  */
  bool Check
  (
    void
  )
  throw();

  /*!
    @brief Schedule the signal for future execution
  */
  void Schedule
  (
    void
  )
  throw();

public: /* Used by SignalCtl */
  /*!
    @brief
    Permanently disable the signal.

    @param      None.
    @return     None.
    @see        ISignalCtl::Detach()
  */
  void Detach
  (
    void
  )
  throw();

  /*!
    @brief
    This method is used by SignalCtl object to enable the signal.

    @param      None.
    @return     None.
    @see        ISignalCtl::Enable()
    @see        ISignalCtl::Set()
  */
  void Enable
  (
    void
  )
  throw();

  /*!
    @brief
    This method is used by SignalBus.

    @param      None.
    @return     None.
  */
  bool IsEnabled
  (
    void
  )
  throw();

  /*!
    @brief
    This method sets the signal for the object.

    @details
    A special version of Set for SignalCtl that will queue a callback
    regardless of the signal's Enabled state.

    @param      None.
    @return     None.
    @see        ISignalCtl::Set()
    @see        Isignal::Set()
  */
  ds::ErrorType SignalCtlSet
  (
    void
  )
  throw();

  /*!
    @brief
    Destructor of the signal object.

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
  virtual ~Signal
  (
    void
  )
  throw();

  /* Derived from ISignal interface */
  /*!
    @brief
    This method sets the signal for the object.

    @details
    The registered callback is called once the signal is set and if the signal
    was enabled. If the signal was not enabled the callback will be called
    when the signal gets enabled. Once callback is called the signal moves
    to disabled state. To re-enable the signal ISignalCtl->Enable()
    should be called.

    @param      None.
    @return     None.
    @see        ISignalCtl::Enable()
    @see        ISignalCtl::Set()
  */
  virtual ds::ErrorType CDECL Set
  (
    void
  )
  throw();

  /* Derived from INode interface */
  /*!
    This method implements Process() function of INode interface. Signal
    objects can be part of a signal bus (which acts as a parent container).
    SignalBus::Traverse() method triggers Process() on individual Signal
    objects. This method is used to set all signals in a SignalBus etc.
    See ds::Utils::SignalProcessEnumType for various activities that can
    be performed using this.

    @param      pUserData - User data for traversal.
    @return     TRUE - if traversal can continue.
    @return     FALSE - If traversal has to halt.
    @see        ISignalBus
    @see        ITraverser
    @see        INode
  */
  virtual boolean Process
  (
    void* pUserData
  )
  throw();

  /*!
    @brief
    Associate a signal bus with this signal.

    @param      pSignalBus - Signal bus to be associated with.

    @note A signal can be associated with only one bus. CS allows re-use
    of a signal with a different signal bus, but this is not recommended.
    This implementation does not allow re-use.
  */
  ds::ErrorType AssociateSignalBus
  (
    SignalBus *pSignalBus
  )
  throw();

  /*!
    @brief
    Disconnect from the associated SignalBus.

    @note Since Signal does not hold a strong reference to the SignalBus,
    this function must be used on all associated signals when the SignalBus
    is destroyed.
  */
  void DisconnectSignalBus
  (
    void
  )
  throw();

  /*!
    @brief
    Checks for Signal object validity.

    @details
    Assumes memory for the SignalBus object is already validated.
    Checks if a certain magic number is present in the object and
    refCnt is valid.

    @return     TRUE if valid.
    @return     FALSE if no valid.
  */
  inline boolean IsValid
  (
    void
  )
  throw()
  {
    return (boolean) (DS_UTILS_SIGNAL_MAGIC_NUMBER == mMagic && refCnt > 0);
  }

  /* Signal dispatch command handler */
  /*!
    This method is the command handler registered with the DS_SIG task to
    handle the DS_SIG_SIGNAL_DISPATCH_CMD. This function is guaranteed to
    run in DS_SIG task context.

    @param[in]  cmd - Command to process (DS_SIG_SIGNAL_DISPATCH_CMD)
    @param[in]  pUserData - User data pointer. Points to the signal object.

    @return     None.
    @see        ds_sig_task.h
    @see        ds_sig_svc.h
  */
  static void SignalDispatcher
  (
    ds_sig_cmd_enum_type    cmd,
    void                   *user_data_ptr
  );

  /* Perform the application registered callback. */
  /*!
    This method notifies the user of the signal of the event of interest.
    The user specified callback is invoked with the corresponding user data.

    @param      None.
    @return     None.
  */
  void Callback
  (
    void
  )
  throw();

  /*-------------------------------------------------------------------------
    Macro to define IWeakRef/IQI methods.
  -------------------------------------------------------------------------*/
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(ISignal)

}; /* class Signal */

} /* namespace Utils */
} /* namespace ds */

#endif /* DS_UTILS_SIGNAL_H */

