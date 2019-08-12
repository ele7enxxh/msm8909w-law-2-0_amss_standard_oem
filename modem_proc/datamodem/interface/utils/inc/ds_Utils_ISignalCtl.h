#ifndef AEEISIGNALCTL_H
#define AEEISIGNALCTL_H
/*======================================================================
        Copyright 2006 - 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
========================================================================

DESCRIPTION:  Definition of ISignalCtl interface.

======================================================================*/

#include "datamodem_variation.h"
#include "ds_Utils_ISignal.h"

#define AEEIID_ISignalCtl 0x01041079

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ISignalCtl : public ISignal
{
   virtual int CDECL Detach() = 0;
   virtual int CDECL Enable() = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ISignalCtl(iname) \
   INHERIT_ISignal(iname); \
   int (*Detach)(iname *); \
   int (*Enable)(iname *)

AEEINTERFACE_DEFINE(ISignalCtl);

static __inline uint32 ISignalCtl_AddRef(ISignalCtl *pif)
{
   return AEEGETPVTBL(pif,ISignalCtl)->AddRef(pif);
}

static __inline uint32 ISignalCtl_Release(ISignalCtl *pif)
{
   return AEEGETPVTBL(pif,ISignalCtl)->Release(pif);
}

static __inline int ISignalCtl_QueryInterface(ISignalCtl *pif, AEEIID iid, void * *ppOut)
{
   return AEEGETPVTBL(pif,ISignalCtl)->QueryInterface(pif, iid, ppOut);
}

static __inline int ISignalCtl_Set(ISignalCtl *pif)
{
   return AEEGETPVTBL(pif,ISignalCtl)->Set(pif);
}

static __inline int ISignalCtl_Detach(ISignalCtl *pif)
{
   return AEEGETPVTBL(pif,ISignalCtl)->Detach(pif);
}

static __inline int ISignalCtl_Enable(ISignalCtl *pif)
{
   return AEEGETPVTBL(pif,ISignalCtl)->Enable(pif);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

ISignalCtl Interface

Description:

   ISignalCtl is the controlling interface of signal object.  ISignalCtl
   inherits methods from ISignal.

   ISignalCtl can be used to enable notifications via a signal object, or to
   permanently prevent notifications from that object.

   Refer to the ISignal usage discussion for an overview of the signal
   framework.
  
===============================================================================

ISignalCtl_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalCtl_Release()

===============================================================================

ISignalCtl_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalCtl_AddRef()

===============================================================================

ISignalCtl_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================

ISignalCtl_Set()

Description:

   This function is inherited from ISignal_Set(), but has slightly different
   behavior:  ISignalCtl_Set() causes the signal to be dispatched even if 
   the signal is not enabled.

===============================================================================

ISignalCtl_Detach()

Description:

   Detach() severs the relationship between a signal and its handler or
   callback.  No further invocations will be initiated, and the observer's
   signal handler (if any) will be released.

   In a single-threaded dispatch environment, it is guaranteed that after
   Detach() returns there will be no further calls to a signal's callback or
   to its signal handler's Notify() method.

   In a multi-threaded environment this cannot be guaranteed, because an
   invocation may already be about to commence in another thread when
   Detach() is called.  In multi-threaded environments clients must rely on
   reference counting and wait for the signal handler to be released.

Prototype:

   int ISignalCtl_Detach(ISignalCtl* pif)

Parameters:
   pif: Pointer to an ISignalCtl interface

Return Value:
   Error code.

===============================================================================

ISignalCtl_Enable()

Description:

   Enables the signal to be dispatched when it becomes ready.

   New signals are enabled when created.  Once a signal has been queued and
   dispatched, the signal is disabled.  While a signal is disabled, calls to
   ISignal_Set() are recorded, but not dispatched until the next call to
   ISignalCtl_Enable().

   ISignalCtl_Enable() will typically be called while the signal's callback
   is executing, after the callback has inspected the current state and
   performed all necessary processing and determined that it is ready for
   more notifications.  If the object does not require notifications it will
   refrain from calling ISignalCtl_Enable() during the callback, and instead
   call it at some other time when it regains interest in notifications.
   Allowing a signal to remain disabled is particularly important when
   dealing with level-triggered notifications.  When a level-triggered
   condition is true, re-enabling a signal will immediately trigger the
   callback or handler to fire again.

   ISignalHandler_Notify() provides an easy way to re-enable a signal:
   simply returning AEE_SUCCESS from Notify() will cause the signal to be
   re-enabled.  Still, ISignalCtl_Enable() may be called within
   ISignalHandler_Notify().  This allows other threads to awaken and invoke
   the signal handler even before the current thread returns from Notify().

Prototype:

   int ISignalCtl_Enable(ISignalCtl* pif)

Parameters:
   pif: Pointer to an ISignalCtl interface

Return Value:
   Error code.

Comments:

   If the signal has been added to a signal bus, ISignalCtl_Enable() will
   also enable the signal bus if it is not already in enabled state.  A
   signal bus is enabled when at least one signal on the bus is enabled.

See Also:
   ISignalBus, ISignalCtl_Set(), ISignal_Set()

===============================================================================

*/

#endif /* AEEISIGNALCTL_H */

