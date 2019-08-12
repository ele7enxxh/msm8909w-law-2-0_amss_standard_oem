#ifndef AEEISIGNALBUS_H
#define AEEISIGNALBUS_H
/*
=======================================================================
                  Copyright 2008 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary and Confidential
=======================================================================
*/
#include "datamodem_variation.h"
#include "ds_Utils_ISignal.h"

#define AEEIID_ISignalBus  0x0103d14b

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ISignalBus : public IQI
{
  virtual int CDECL Strobe() = 0;
  virtual int CDECL Set() = 0;
  virtual int CDECL Clear() = 0;
  virtual int CDECL OnEnable(ISignal* piSignal) = 0;
  virtual int CDECL Add(ISignal* piSignal) = 0;
  virtual int CDECL Remove(ISignal* piSignal) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ISignalBus(iname) \
   INHERIT_IQI(iname); \
   int (*Strobe)  (iname* pif); \
   int (*Set)     (iname* pif); \
   int (*Clear)   (iname* pif); \
   int (*OnEnable)(iname* pif, ISignal* piSignal); \
   int (*Add)     (iname* pif, ISignal* piSignal); \
   int (*Remove)  (iname* pif, ISignal* piSignal)

AEEINTERFACE_DEFINE(ISignalBus);

static __inline uint32 ISignalBus_AddRef(ISignalBus* pif)
{
   return AEEGETPVTBL(pif, ISignalBus)->AddRef(pif);
}

static __inline uint32 ISignalBus_Release(ISignalBus* pif)
{
   return AEEGETPVTBL(pif, ISignalBus)->Release(pif);
}

static __inline int ISignalBus_QueryInterface(ISignalBus* pif, AEEIID id, void** ppo)
{
   return AEEGETPVTBL(pif, ISignalBus)->QueryInterface(pif, id, ppo);
}

static __inline int ISignalBus_Strobe(ISignalBus* pif)
{
   return AEEGETPVTBL(pif, ISignalBus)->Strobe(pif);
}

static __inline int ISignalBus_Set(ISignalBus* pif)
{
   return AEEGETPVTBL(pif, ISignalBus)->Set(pif);
}

static __inline int ISignalBus_Clear(ISignalBus* pif)
{
   return AEEGETPVTBL(pif, ISignalBus)->Clear(pif);
}

static __inline int ISignalBus_OnEnable(ISignalBus* pif, ISignal* piSignal)
{
   return AEEGETPVTBL(pif, ISignalBus)->OnEnable(pif, piSignal);
}

static __inline int ISignalBus_Add(ISignalBus* pif, ISignal* piSignal)
{
   return AEEGETPVTBL(pif, ISignalBus)->Add(pif, piSignal);
}

static __inline int ISignalBus_Remove(ISignalBus* pif, ISignal* piSignal)
{
   return AEEGETPVTBL(pif, ISignalBus)->Remove(pif, piSignal);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

ISignalBus Interface

Description: 

   ISignalBus is an interface for managing a signal bus.  A signal bus
   manages a set of signal objects.

   Objects that deliver asynchronous notifications typically will accept a
   signal object as a parameter.  Such objects can trigger the signal
   directly via ISignal_Set(), or they may pass the signal to a bus and
   thereafter trigger the signal by calling ISignalBus_Strobe().

   Using a signal bus provides the following advantages:

   1. A signal bus allows an object to manage an arbitrary number of
      signals, and thereby deliver notifications to an arbitrary number of
      clients.  A signal bus acts as a container of signal objects.  Signals
      pre-allocate the memory require for queueing on a signal bus, so any
      number of them can be added to a signal bus.  By contrast, if an
      ordinary object were to hold references to a large number of signals
      it would have to allocate memory, and in the case of a user process it
      would consume remote object references (of which each process has a
      limited budget).

      A related factor is that a signal bus can identify whether two ISignal
      interfaces refer to the same signal object, and thereby avoid
      duplication in its list of signals.  Clients outside the kernel have
      no way of determining whether two signals are identical.

   2. A signal bus can make delivery of notifications more efficient by
      minimizing domain transitions.  When a user process calls
      ISignal_Set() this will require a transition to the kernel and back.
      ISignalBus_Strobe() will also require one transition to kernel and
      back, but in the process can trigger a large number of signals.

   3. A signal bus can manage level-triggered notifications more
      efficiently.  Bus users only need to call ISignalBus_Set() and
      ISignalBus_Clear() on each level transition.  The signal bus handles
      all other interactions with the signals and the recpients of the
      notification (re-registration).

   4. A signal bus will validate that the signal implementation is trusted.
      Trusted implementations, such as those provided by
      AEECLSID_SignalFactory, will never block the caller of ISignal_Set().
      When an object receives an ISignal reference from an untrusted client,
      however, the object has no way of knowing whether the signal will
      block it indefinitely (which would provide a denial of service
      attack).  Th signal bus validates the type of each signal that is
      receives, and rejects signals that it does not deem trustworthy.

   5. Releasing the last reference to the bus will cause all the current
      signals to be strobed.  This is done to notify the signal owners
      that the bus is no longer active.

  When a signal bus is initially created, it is in the "clear" state, as if
  Clear() was called.

============================================================================

ISignalBus_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalBus_Release()

============================================================================

ISignalBus_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalBus_AddRef()

============================================================================

ISignalBus_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

==============================================================================

ISignalBus_Strobe()

Description:

   Atomically set and clear the bus' state, signalling all enabled signals 
   on the bus.

   This is the primary means of delivering edge-triggered notifications.  An
   example of this would be the arrival of a message, or the passing of a
   deadline or timer expiration.

Prototype:

   int ISignalBus_Strobe(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:

   Any signals in the bus are set, and will require ISignal_Enable()
   before being able to be set again.

   If the bus was already set, the bus will end up clear.

See Also:
   ISignal, ISignalBus_Add(), 

==============================================================================

ISignalBus_Set()

Description:

   Changes the state of the bus to "set", and calls ISignal_Set() on all of
   the signals on the bus.

   ISignalBus_Set() and ISignalBus_Clear() allow delivery of level-triggered
   notifications.  Some examples of states that are best communicated via
   level-triggered notifications are presence of data in a receive buffer,
   and availability of space in a send buffer.

   While the bus is in the "set" state, any attached signals will remain
   contunuously in the "set" state.  Signals will repeatedly fire until they
   are left in the disabled state.  (See ISignal.)  When a signal is
   attached to a "set" bus, re-enabling the signal will immediately re-queue
   the signal for delivery.  The recpient of the signal must make use of the
   enabled/disabled state of the signal to request or suspend notifications.
   Requesting and suspending notifications requires no communication with
   the notifying object (the holder of the signal bus).

Prototype:

   int ISignalBus_Set(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:

   Any signals in the bus are set, and will require ISignal_Enable()
   before being able to be set again.

See Also:
   ISignal, ISignalBus_Add()

==============================================================================

ISignalBus_Clear()

Description:

   Set the state of the bus to "clear".

   While the bus is in the "clear" state, re-enabled signals will not
   immediately be re-queued for delivery.

   Note that clearing a bus will not de-queue signals that have already been
   queued.  As is generally the case, signals must be treated as "hints"
   because they are delivered asynchronously and the condition that caused
   them to be queued may not be true when they are eventually delivered.
   
Prototype:

   int ISignalBus_Clear(ISignalBus* pif);

Parameters:
   pif: Pointer to an ISignalBus interface

Return Value:
   Error code.

Side Effects:
   None.

See Also:
   ISignal, ISignalBus_Add()

==============================================================================

ISignalBus_OnEnable()

Description:

   Associates an ISignal to be set when the ISignalBus has been "enabled", 
   i.e. when an ISignal is added or when one of the ISignalBus' ISignals 
   have been re-enabled.

Prototype:

   int ISignalBus_OnEnable(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be Added with the bus

Return Value:
   Error code.
   
Comments:

   This function may only be called once, duplicate calls will return 
   AEE_EALREADY.

See Also:
   ISignalBus_Set()

==============================================================================

ISignalBus_Add()

Description:

   Associates an instance of ISignal with the ISignalBus.

Prototype:

   int ISignalBus_Add(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be added to the bus

Return Value:
   Error code.

Comments:

   An instance of ISignal normally only supports registration with a 
   single instance of ISignalBus.

   This function will fail if the signal has been detached.

Side Effects:

   If the signal has previously been enabled, the OnEnable signal 
   is set.

   If the signal is enabled and the state of the bus is set, the
   signal will be set immediately and disabled.

See Also:
   ISignal, ISignalBus_Set(), ISignalBus_Remove()

==============================================================================

ISignalBus_Remove()

Description:

   Dis-associates an instance of ISignal with the ISignalBus.

Prototype:

   int ISignalBus_Remove(ISignalBus* pif, ISignal* piSignal);

Parameters:
   pif: Pointer to an ISignalBus interface
   piSignal: ISignal to be removed from the bus

Return Value:
   Error code.

Side Effects:
   None.

See Also:
   ISignal, ISignalBus_Set(), ISignalBus_Add()


=============================================================================
*/

#endif /* #ifndef AEEISIGNALBUS_H */
