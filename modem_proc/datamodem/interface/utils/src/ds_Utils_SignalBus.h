#ifndef DS_UTILS_SIGNAL_BUS_H
#define DS_UTILS_SIGNAL_BUS_H

/*===========================================================================
  @file DSSignalBus.h

  This file provides the ds::Utils::SignalBus library. It is currently
  defined to be a wrapper around the ISignalBus interface.

  SignalBus()
    Constructor for the signal bus object.

  ~SignalBus()
    Destructor for the signal bus object.

  Add()
    Add a signal to the signal bus.

  Remove()
    Remove a signal object from the signal bus.

  Set()
    Set the SignalBus - all existing signals and future signals added
    to the bus will be set.

  Clear()
    Clear the SignalBus - future signals added to the bus will not be set.

  Strobe()
    Set all existing signals on the SignalBus. Future signals added
    to the bus will not be set.

  OnEnable()
    A signal passed to this function will be set if any signals
    in the SignalBus become enabled.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_SignalBus.h#1 $
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
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_ISignalBus.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_SignalCtl.h"
#include "ds_Utils_CSignalBus.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Utils
  {

    /*!
      @class
      ds::Utils::SignalBus

      @brief
      Implementation of ISignalBus interface.

      @details
      This class provides an implementation of ISignalBus interface. Please
      refer to AEEISignalBus.h for interface documentation.

      @see AEEISignalBus.h
      @see AEEISignal.h
    */
     /*lint -esym(1510, ISignalBus) */
     /*lint -esym(1510, IQI) */
    class SignalBus : public ISignalBus,
                      public Factory
    {

    private:
      Signal*  mpOnEnableSig;
      bool     isSet;

    public:
      /*!
        @brief
        Constructor of the SignalBus object.

        @params     None.
        @see        None.
        @return     The constructed SignalBus object.
      */
      SignalBus();

      /*!
        @brief
        Destructor of the SignalBus object.

        @details
        Only releases the memory allocated to SignalBus object
        
        @params     None.
        @see        None.
        @return     None.
      */
      virtual ~SignalBus() throw();

      /*!
        @brief
        Destructor of the SignalBus object.

        @details
        Removes all the Signal objects from the SignalBus. Before doing this
        SignalBus is SET, informing individual signals that signal bus is
        going away.

        @params     None.
        @see        None.
        @return     None.
      */
      virtual void Destructor
      (
        void 
      ) 
      throw();

      /*!
        @brief
        Adds a ISignal object to the signal bus.

        @details
        This method is used to add an ISignal object to the signal bus.
        Adding of ISignalCtl objects is not supported (CS does support
        this, but is not recommended).

        ds::Utils::SignalBus only allows addition of ds::Utils::Signal
        objects. CS signals/any other implementations are not supported.

        A signal cannot be added to a SignalBus in Detached state. That is,
        if the signal's ISignalCtl facet has detached the signal (permanantly
        disabled the signal), then adding to signal bus is not supported. In
        this condition, EINVAL error code is returned.

        A signal can be added to one SignalBus only. If the signal is tried
        to be added to more than 1 buses, then EINVAL is returned.

        @side_effects  The signal's internal state is changed, so it cannot
                       be added to any other signal buses.

        @params pISignal - Pointer to ISignal object to be added.
        @see    AEEISignalBus.h::ISignalBus_Add()
        @return SUCCESS - On successful addition
                EFAULT  - If invalid/unsupported signal is tried to be added.
                EINUSE  - If signal object is already part a bus
                EINVAL  - If signal has been detached.
      */
      virtual ds::ErrorType CDECL Add
      (
        ISignal* piSignal
      )
      throw();

      /*!
        @brief
        Remove a signal from the signal bus.

        @details
        The signal is removed from the signal bus. The signal may not be
        reused with other signal buses (reuse is not recommended by CS).

        @params   pISignal - Signal object.
        @see      AEEISignalBus.h::ISignalBus_Remove()
        @return   SUCCESS - on success
        @return   EFAULT  - Invalid arguments.
      */
      virtual ds::ErrorType CDECL Remove
      (
        ISignal* piSignal
      )
      throw();
      /*!
        @brief
        Move signal bus to SET state.

        @details
        This method is used to deliver level-triggered notifications.

        If the bus is in CLEAR state:
        1. SignalBus is moved to SET state.
        2. Any enabled signals in the bus would be set.
        3. If any new signals are added during SET state, they would
           be set depending upon whether they are enabled.
        4. The SignalBus would remain in SET state until Clear/Strobe
           is called.
        5. Until the SignalBus' state is SET, any attached signals
           would remain in SET state. That is, Signals will repeatedly
           fire until they are left in the disabled state.Re-enabling
           signal would immediately requeue them for delivery.

        If the bus is in SET state:
        1. Nothing happens, SignalBus would remain in SET state.

        @params   None.
        @see      AEEISignalBus.h::ISignalBus_Set()
        @return   SUCCESS - Always
      */
      virtual ds::ErrorType CDECL Set
      (
        void
      )
      throw();

      /*!
        @brief
        Move signal bus to CLEAR state.

        @details
        Clears the state of the signal bus.

        If the bus is in SET state:
        1. SignalBus is moved to CLEAR state. When the bus is in
        CLEAR state, re-enabled signals would not be requeued for
        delivery until the state changes.

        If the bus is in CLEAR state:
        1. Nothing happens, SignalBus would remain in CLEAR state.

        @params   None.
        @see      AEEISignalBus.h::ISignalBus_Clear()
        @return   SUCCESS - Always
      */
      virtual ds::ErrorType CDECL Clear
      (
        void
      )
      throw();

      /*!
        @brief
        Set and clear the SignalBus state.

        @details
        This method is used to deliver edge-triggered notification.

        If the bus is in CLEAR state:
        Any enabled signals in the bus are set.

        If the bus is in SET state:
        Nothing happens.

        @params   None.
        @see      AEEISignalBus.h::ISignalBus_Strobe()
        @return   SUCCESS - Always
      */
      virtual ds::ErrorType CDECL Strobe
      (
        void
      )
      throw();

      /*!
        @brief
        Associate with an ISignal that would be set when any signals in the bus
        are enabled.

        @details
        Associate with an ISignal that would be set when any signals in the bus
        are enabled.

        The associated signal would be set, if:
        1. Any existing signals in the bus are re-enabled.
        2. Any new enabled signals are added to the bus.

        @params   None.
        @see      AEEISignalBus.h::ISignalBus_OnEnable()
        @return   SUCCESS - on success
        @return   EFAULT - Invalid/unsupported signal is tried to be registered.
      */
      virtual ds::ErrorType CDECL OnEnable
      (
        ISignal* piSignal
      )
      throw();

      /*!
        @brief
        Method that signal uses to notify its parent bus about being enabled.

      */
      void CDECL SignalEnabled
      (
        ds::Utils::Signal *pSignal
      )
      throw();

      /*!
        @brief
        Initializes the command handler, must be called at powerup.
      */
      static void RegisterCmdHandler
      (
        void
      )
      throw();

      /*!
        @brief
        Overloaded new operator.

        The current implementation assumes PS Mem is used for memory allocation
        for signals, signal ctls and signal bus. If this is removed/changed,
        implementation needs to be updated accordingly (see SignalBus::Add).
        This is because PS mem is used to validating if the signal objects added
        to the bus are valid objects.
      */
      void* operator new (unsigned int num_bytes)
      throw()
      {
        (void) num_bytes;
        return ps_mem_get_buf (PS_MEM_DS_UTILS_SIGNAL_BUS);
      }

      /* Overloaded delete operator */
      /*lint -e{1511} */
      void operator delete (void* objPtr)
      throw()
      {
        PS_MEM_FREE(objPtr);
      }

      /*-------------------------------------------------------------------------
        Defintions of IQI Methods
      -------------------------------------------------------------------------*/
      DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(ISignalBus)

      /*-------------------------------------------------------------------------
        Overloaded Factory methods
      -------------------------------------------------------------------------*/
      virtual int32 AddItem(INode* item) throw();

      /*-------------------------------------------------------------------------
        CreateInstance method for SignalBus.
      -------------------------------------------------------------------------*/
      static int CreateInstance (void* env, AEECLSID clsid,
                                 void** newObj);
    private:
      /*!
        @brief
        Checks for validity of signal object passed in.
      */
      ds::ErrorType ValidateSignal
      (
        ISignal * pISignal
      )
      throw();

  /*!
    @brief
    Command handler for signal bus dispatch command.
  */
  static void DispatchCmdHandler
  (
    ds_sig_cmd_enum_type    cmd,
    void                   *pUserData
  )
  throw();
    }; /* class SignalBus */

  } /* namespace Utils */
}/* namespace ds */


#endif /* DS_UTILS_SIGNAL_BUS_H */

