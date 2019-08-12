#ifndef AEEISIGNALFACTORY_H
#define AEEISIGNALFACTORY_H
/*======================================================================
        Copyright 2006-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
========================================================================

DESCRIPTION:  Definition of ISignalFactory interface.

======================================================================*/

#include "datamodem_variation.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_ISignalHandler.h"

#define AEEIID_ISignalFactory 0x01041077

#if defined(AEEINTERFACE_CPLUSPLUS)

struct ISignalFactory : public IQI
{
   virtual int CDECL CreateSignal(ISignalHandler * piHandler,
                                  SignalHandlerCallBack pSignalHandlerCallBack,
                                  ds::Utils::SignalHandlerBase* pSignalHandlerBaseParam,
                                  ISignal * *ppiSig,
                                  ISignalCtl * *ppiSigCtl) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ISignalFactory(iname) \
   INHERIT_IQI(iname); \
   int (*CreateSignal)(iname *, \
      ISignalHandler * piHandler, \
      uint32 uArgA, \
      uint32 uArgB, \
      ISignal * *ppiSig, \
      ISignalCtl * *ppiSigCtl \
   )

AEEINTERFACE_DEFINE(ISignalFactory);

static __inline uint32 ISignalFactory_AddRef(ISignalFactory *pif)
{
   return AEEGETPVTBL(pif,ISignalFactory)->AddRef(pif);
}

static __inline uint32 ISignalFactory_Release(ISignalFactory *pif)
{
   return AEEGETPVTBL(pif,ISignalFactory)->Release(pif);
}

static __inline int ISignalFactory_QueryInterface(ISignalFactory *pif, AEEIID iid, void * *ppOut)
{
   return AEEGETPVTBL(pif,ISignalFactory)->QueryInterface(pif, iid, ppOut);
}

static __inline int ISignalFactory_CreateSignal(ISignalFactory *pif, ISignalHandler * piHandler, uint32 uArgA, uint32 uArgB, ISignal * *ppiSig, ISignalCtl * *ppiSigCtl)
{
   return AEEGETPVTBL(pif,ISignalFactory)->CreateSignal(pif, piHandler, uArgA, uArgB, ppiSig, ppiSigCtl);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

ISignalFactory Interface

Description:

	 This interface allows creation of signal objects that are associated
	 with an ISignalHandler instance.  These signals, when "set", will cause
	 their associated handler's Notify() method to be invoked.

    ISignalFactory can be used in single-threaded or multi-threaded
    environments.
    
    Signal objects enable an asynchronous form of communication.  Signal
	 objects can be used within the local domain (e.g. applet or process) or
	 can be passed to other domains.

Usage:

    For an introduction to the signal framework, see the usage note for
    ISignalCBFactory.  Here we will discuss how and why ISignalFactory
    differs from ISignalCBFactory.

    The most serious problem that arises from using ISignalCBFactory in a
    multi-threaded environment is that of cleanup.  In a single-threaded
    environment this is relatively simple because the client can call
    ISignalCtl_Detach() and be guaranteed that no further callback calls
    will occur.  In a multi-threaded environment no such guarantee exists.
    When Detach() is called, a callback may already be in progress on
    another thread, or it may be in the process of being dispatched.  In
    order to ensure that a callback is not called with a dangling pointer a
    new strategy is needed.

    ISignalFactory uses reference counting to solve this problem.  The
    signal object holds a reference to a signal handler object.  During each
    call to ISignalHandler_Notify() it AddRefs the object reference,
    preventing deletion of the handler when a notification is in progress or
    pending.  When ISignalCtl_Detach() is called, the signal zeroes its
    pointer to that object and releases its reference count.  After Detach()
    and after all pending callbacks occur, the handler's reference count
    will go to zero, at which point the client can free the memory use by
    the handler.

    Reference counting neatly avoids this thread safety issue without
    introducing locks and the potential for deadlock, but it does introduce
    one complication: circular reference counts.  The discussion so far
    describes the following configuration:

 ===pre>
            +-----------+   
            | Observer  |
            +-----------+   
               |   ^
               v   |        
            +-----------+   
            |  Signal   |<--- Observed object
            +-----------|   
===/pre>

    Such a situation would be a problem, because with the signal and
    observer both referencing each other, the observer's count will never go
    to zero, so it will never know when to call ISignalCtl_Detach().  Note
    that this was not an issue with signals created via ISignalCBFactory,
    because those signals hold an *uncounted* reference (which essentially
    is the root of the tread safety issue).  With signals that count the
    reference, we need to distinguish the reference held by the signal from
    other references.  In order to do this, we distinguish the handler
    from the observer, as in this diagram:
    
===pre>
            +-----------+
            | Observer  |
            +-----------+
               |   ^
               |   :
               |   :
               | +---------+
               | | Handler |
               | +---------+
               |   ^
               |   |
               v   |
            +-----------+   
            |  Signal   |<--- Observed object
            +-----------|   
===/pre>

    The dotted line between the handler and the observer represents a "weak"
    reference.  A "weak" reference is one that does not prevent the
    referenced object from being deleted, but which can be used to obtain a
    "strong" (ordinarily counted) reference.  See IWeakRef for more
    information on weak references.

    The handler holds a weak reference to the observer all the time, and
    holds a strong reference only during notifications.  Before it delivers
    a notification, it uses the weak reference to obtain a strong reference,
    and it releases the strong reference after the notification function
    returns.

    Incorporating all of these factors, the MyClass_Register() example from
    the ISignalCBFactory usage discussion would be modified to look like
    this:
    
===pre>
   void MyClass_Register(MyClass *me)
   {
      ISignalFactory *piSignalFactory;
      ISignalHandler *piSignalHandler;
      ISignal *piSignalCtl;
      int nErr;

      nErr = MyClass_CreateSignalHandler(me, &piSignalHandler);
      if (AEE_SUCCESS == nErr) {
         nErr = IEnv_CreateInstance(me->piEnv, AEECLSID_SignalFactory,
                                    (void**)&piSignalFactory);
      }
      if (AEE_SUCCESS == nErr) {
         nErr = ISignalFactory_CreateSignal(piSignalFactory,
                   piSignalHandler, BATTERY_LEVEL_CHANGE, 0,
                   &piSignal, &me->piSignalCtl);
      }
      if (AEE_SUCCESS == nErr) {
         nErr = IBatteryLevel_OnLevelChange(me->piBL, piSignal);
      }
      IQI_RELEASEIF(piSignal);
      IQI_RELEASEIF(piSignalFactory);
      IQI_RELEASEIF(piSignalHandler);
   }
===/pre>

   The function "CreateSignalHandler()" is assumed to create a signal
   handler object that maintains a weak reference to the object.  There
   are different ways to achieve this.  See IWeakRef for more information.
    
 See also:

   ISignalCBFactory
   AEECLSID_SignalFactory
       
===============================================================================

ISignalFactory_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ISignalFactory_Release()

===============================================================================

ISignalFactory_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ISignalFactory_AddRef()

===============================================================================

ISignalFactory_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================

ISignalFactory_CreateSignal()

Description:
         Create a signal object that invokes a handler object when set.  The
         new signal will AddRef the handler object.  The handler object will
         be released after the signal is deleted (asynchronously, not
         synchronously with the last Release of the signal).
      
Prototype:

   int ISignalFactory_CreateSignal(ISignalFactory* pif, 
                                   ISignalHandler* piHandler, 
                                   uint32 uArgA, 
                                   uint32 uArgB, 
                                   ISignal** ppiSig, 
                                   ISignalCtl** ppiSigCtl)

Parameters:
   pif: Pointer to an ISignalFactory interface
   piHandler : handler object to be invoked when signal is set.
   uArgA : Parameters to ISignalHandler_Notify()
   uArgB : Parameters to ISignalHandler_Notify()
   ppiSig : [OUT] Optional. resulting signal object's ISignal interface.
   ppiSigCtl : [OUT] resulting signal object's ISignalCtl interface.

Return Value:

   Error code.

Comments:
   None

Side Effects:
   None

===============================================================================
*/

#endif /* AEEISIGNALFACTORY_H */

