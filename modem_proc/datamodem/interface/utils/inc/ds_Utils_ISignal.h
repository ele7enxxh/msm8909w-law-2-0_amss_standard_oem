#ifndef AEEISIGNAL_H
#define AEEISIGNAL_H
/*=======================================================================
            Copyright 2005-2008 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                  QUALCOMM Confidential and Proprietary
=======================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_IQI.h"

#define AEEIID_ISignal 0x010285f5

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ISignal : public IQI
{
   virtual int CDECL Set() = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ISignal(iname) \
   INHERIT_IQI(iname);\
   int (*Set)   (iname*  pif)

AEEINTERFACE_DEFINE(ISignal);

static __inline uint32 ISignal_AddRef(ISignal* pif)
{
   return AEEGETPVTBL(pif,ISignal)->AddRef(pif);
}

static __inline uint32 ISignal_Release(ISignal* pif)
{
   return AEEGETPVTBL(pif,ISignal)->Release(pif);
}

static __inline int ISignal_QueryInterface(ISignal* pif, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(pif,ISignal)->QueryInterface(pif, cls, ppo);
}

static __inline int ISignal_Set(ISignal* pif)
{
   return AEEGETPVTBL(pif,ISignal)->Set(pif);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*======================================================================
INTERFACE DOCUMENTATION
========================================================================

ISignal Interface

Description: 

   This interface provides a method, Set, which is used to enqueue a signal
   object.  Signal objects are used to notify other objects of events,
   changes in state, or the presence of a condition.

   ISignal is typically used with an asynchronous signal framework.  See the
   usage discussion in ISignalFactory for an overview of this framework and
   the role of ISignal.

See Also:

   ISignalCtl, AEECLSID_SignalFactory, AEECLSID_SignalCBFactory

==============================================================================

ISignal_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignal_Release()

==============================================================================

ISignal_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignal_AddRef()

==============================================================================

ISignal_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

==============================================================================

ISignal_Set()

Description:

   This function marks a signal as ready, queueing it on its assigned signal
   queue or causing associated signal handler to be invoked.
   
   If the signal has been detached, this call has no effect.

   If the signal is not enabled, the set will be recorded and dispatched
   when the signal becomes enabled.

Prototype:

   int ISignal_Set(ISignal* pif)

Parameters:
   pif: Pointer to an ISignal interface

Return Value:
   Error code.

Comments:

   ISignal_Set() will trigger the associated signal dispatcher to process
   the signal. The signal is dispatched when it is marked ready and enabled.
   The signal can be enabled by the client using
   ISignalCtl_Enable(). Signals just created are enabled.  Signal
   dispatching is asynchronous to ISignal_Set() operation. The signal when
   set will remain ready until it is enabled and dispatched. Signal
   dispatching involves notifying the client.

   Similarly, if the thread is created from ISignalFactory the associated
   signal handler will be notified when a thread in the apartment becomes
   available.

See Also:

=============================================================================
*/
#endif /* #ifndef AEEISIGNAL_H */

