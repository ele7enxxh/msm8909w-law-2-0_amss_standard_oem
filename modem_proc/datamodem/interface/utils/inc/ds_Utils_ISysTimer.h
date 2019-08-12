#ifndef AEEISYSTIMER_H
#define AEEISYSTIMER_H

/*===========================================================================
        Copyright 2004-2005 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
===========================================================================*/

#include "datamodem_variation.h"
#include "ds_Utils_ISignal.h"

//====================================================================
//   Macro definitions
//====================================================================

#define AEEIID_ISysTimer          0x01028c20

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ISysTimer : public IQI
{
   virtual int AEEINTERFACE_CDECL AssociateSignal(ISignal* piSignal) = 0;
   virtual int AEEINTERFACE_CDECL Cancel() = 0;
   virtual int AEEINTERFACE_CDECL GetTimeRef(uint32* pdwNow) = 0;
   virtual int AEEINTERFACE_CDECL SetExpiry(uint32 dwWhen) = 0;
   virtual int AEEINTERFACE_CDECL GetExpiry(uint32* pdwWhen) = 0;
   virtual int AEEINTERFACE_CDECL SetDelay(uint32 dwDelay) = 0;
   virtual int AEEINTERFACE_CDECL GetDelay(uint32* pdwDelay) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ISysTimer(iname) \
   INHERIT_IQI(iname); \
   int (* AssociateSignal )      (iname* me, ISignal* piSignal); \
   int (* Cancel )    (iname* me); \
   int (* GetTimeRef )(iname* me, uint32* pdwNow); \
   int (* SetExpiry ) (iname* me, uint32 dwWhen); \
   int (* GetExpiry ) (iname* me, uint32* pdwWhen); \
   int (* SetDelay )  (iname* me, uint32 dwDelay); \
   int (* GetDelay )  (iname* me, uint32* pdwDelay)

AEEINTERFACE_DEFINE(ISysTimer);


//====================================================================
//   Accessor functions
//====================================================================

static __inline uint32 ISysTimer_AddRef(ISysTimer *me)
{
   return AEEGETPVTBL(me,ISysTimer)->AddRef(me);
}

static __inline uint32 ISysTimer_Release(ISysTimer *me)
{
   return AEEGETPVTBL(me,ISysTimer)->Release(me);
}

static __inline int ISysTimer_QueryInterface(ISysTimer *me, AEEIID iid,
                                             void **ppo)
{
   return AEEGETPVTBL(me,ISysTimer)->QueryInterface(me, iid, ppo);
}

static __inline int ISysTimer_AssociateSignal(ISysTimer* pif, ISignal *piSignal)
{
   return AEEGETPVTBL(pif,ISysTimer)->AssociateSignal(pif, piSignal);
}

static __inline int ISysTimer_Cancel(ISysTimer *pif)
{
   return AEEGETPVTBL(pif,ISysTimer)->Cancel(pif);
}

static __inline int ISysTimer_GetTimeRef(ISysTimer *pif, uint32* pdwNow)
{
   return AEEGETPVTBL(pif,ISysTimer)->GetTimeRef(pif, pdwNow);
}

static __inline int ISysTimer_SetExpiry(ISysTimer *pif, uint32 dwWhen)
{
   return AEEGETPVTBL(pif,ISysTimer)->SetExpiry(pif, dwWhen);
}

static __inline int ISysTimer_GetExpiry(ISysTimer *pif, uint32* pdwWhen)
{
   return AEEGETPVTBL(pif,ISysTimer)->GetExpiry(pif, pdwWhen);
}

static __inline int ISysTimer_SetDelay(ISysTimer *pif, uint32 dwDelay)
{
   return AEEGETPVTBL(pif,ISysTimer)->SetDelay(pif, dwDelay);
}

static __inline int ISysTimer_GetDelay(ISysTimer *pif, uint32* pdwDelay)
{
   return AEEGETPVTBL(pif,ISysTimer)->GetDelay(pif, pdwDelay);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
 =============================================================================
  DATA STRUCTURE DOCUMENTATION
  ===============================================================================
  ISysTimer Interface ID
  
  Header File:
  	AEEISysTimer.h
  
  Description:  
  Each class and interface is identified by a unique ID.  These IDs are used to 
  distinguish between various types of objects.
  
  The ID for the ISysTimer interface is given below.
  
  Definition:
  ===pre>
  Identifier                  Definition
  ----------                 ----------------------------------------------
  AEEIID_ISysTimer            ISysTimer interface ID
  ===/pre>
  
  Comments:
  None
  
  See Also:
  None.
=============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

Interface Name: ISysTimer

  Header File:
  	AEEISysTimer.h

Description:

   ISysTimer provides methods to query the current time and to request
   signals to be set when a certain time is reached.  The units of time are
   in ticks. The actual tick duration is a property of the implementation
   (e.g. AEECLSID_SysTimer ticks are in milliseconds, AEECLSID_SysTimerUsec
   ticks are in microseconds).

   ISysTimer consumes valuable resources and should be used sparingly.
   In general, a thread should not use more than one of these timers.

   The reference time value exposed by ISysTimer has no absolute meaning.
   It is relative to some arbitrary, fixed point in time, such as
   system startup.  It is steadily increasing and unaffected by
   real-time clock adjustments.

   For absolute timers, use ISysTimer_SetExpiry(), specifying a time
   that is a small offset from ISysTimer_GetTimeRef().

   For relative timers, use ISysTimer_SetDelay(), specifying a small
   delay from "now".

  Interface ID:
  AEEIID_ISysTimer
  
  Usage:
  None.
  
  See also:
  None.
  
  ==============================================================================
  METHOD DOCUMENTATION
  ==============================================================================
  
  ISysTimer_AddRef()
  
  Header File:
  	AEEISysTimer.h
  
  Description:
  	This function is inherited from IQI_AddRef(). 
  
  Prototype:
  	uint32 ISysTimer_AddRef(ISysTimer *me)
  
  Parameters:
  	_pif[in]: Pointer to an ISysTimer interface.
  
  Return Value:
  	Returns the reference count of the ISysTimer interface.
  
  Side Effects:
  	None.
  
  Comments:
  	None.
  
  See Also:
  	ISysTimer_Release()
  
  ==========================================================================
  ISysTimer_Release()
  
  Header File:
  	AEEISysTimer.h
  
  Description:
  	This function is inherited from IQI_Release(). 
  
  Prototype:
  	uint32 ISysTimer_Release(ISysTimer *me)
  
  Parameters:
  	_pif[in]: Pointer to an ISysTimer interface.
  
  Return Value:
  	Returns the reference count of the ISysTimer interface.
  
  Side Effects:
  	None.
  
  Comments:
  	None.
  
  See Also:
  	ISysTimer_AddRef()
  
  ==========================================================================
  ISysTimer_QueryInterface()
  
  Header File:
  	AEEISysTimer.h
  
  Description:
  	This function is inherited from IQI_QueryInterface(). 
  
  Prototype:
  	int ISysTimer_QueryInterface(ISysTimer *me, AEEIID iid,
  void **ppo)
  Parameters:
  	_pif[in]: Pointer to an ISysTimer interface.
  	iid[in]: Interface ID of requested interface
  	ppo[out]: Pointer to pointer of requested interface.
  
  Return Value:
  	Return AEE_SUCCESS if successful, or another appropriate error code if
  	operation was not successful.
  
  Side Effects:
  	None.
  
  Comments:
  	None.
  
  See Also:
  	None.
  
==============================================================================

ISysTimer_AssociateSignal()

Description:

   Associates the given signal with the ISysTimer.  When the specified
   timer expires, this signal will be set.  This method may only
   be called once per instance of ISysTimer.

Prototype:

   int ISysTimer_AssociateSignal(ISysTimer *pif, ISignal *piSignal)

Parameters:
   pif: pointer to an ISysTimer interface
   piSignal: pointer to an ISignal to be associated with the ISysTimer

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None


==============================================================================

ISysTimer_Cancel()

Description:

   Cancel a previously set SetExpiry or SetDelay request.

Prototype:

   int ISysTimer_Cancel(ISysTimer *pif)

Parameters:
   pif: pointer to an ISysTimer interface

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None

==============================================================================

ISysTimer_GetDelay()

Description:

   Request the remaining delay until the associated signal will be set.

Prototype:

   int ISysTimer_GetDelay(ISysTimer *pif, uint32* pdwDelay)

Parameters:
   pif: pointer to an ISysTimer interface
   pdwDelay [out]: filled in with the current delay

Return Value:
   AEE_SUCCESS or error code.

Comments:
   If the originally requested delay has passed, zero will be returned.

Side Effects:
   None

==============================================================================

ISysTimer_GetExpiry()

Description:

   Request the designated time when ISysTimer's associated signal will be set.

Prototype:

   int ISysTimer_GetExpiry(ISysTimer *pif, uint32* pdwWhen)

Parameters:
   pif: pointer to an ISysTimer interface
   pdwWhen [out]: filled in with the current expiry time

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None

==============================================================================

ISysTimer_GetTimeRef()

Description:

   Provides the current time reference.

   Time values are 32 bits, and can eventually wrap around.  As a
   result, the simple comparison "a < b" can fail sometimes even when
   the difference between a and b is small.  Instead, "(signed)(a-b) >
   0" can be used, which will work provided the times a and b are
   within 31 bits of each other.

Prototype:

   int ISysTimer_GetTimeRef(ISysTimer *pif, uint32* pdwNow)

Parameters:
   pif: pointer to an ISysTimer interface
   pdwNow [out]: filled in with the current time reference

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None

==============================================================================

ISysTimer_SetDelay()

Description:

   Request the ISysTimer's associated signal to be set after the
   designated delay passes.

   In order to handle timer value wraparound correctly, SetDelay()
   assumes the given delay is less than 0x80000000 time units; else
   the delay is treated as zero.

Prototype:

   int ISysTimer_SetDelay(ISysTimer *pif, uint32 dwDelay)

Parameters:
   pif: pointer to an ISysTimer interface
   dwDelay: after how long to set the signal

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None

==============================================================================

ISysTimer_SetExpiry()

Description:

   Request the ISysTimer's associated signal to be set when the
   designated time arrives.

   If the specified time has already occurred, the signal will be set
   immediately.

   In order to handle timer value wraparound correctly, SetExpiry()
   assumes the given time is within +/- 31 bits of the current time.
   For example, if the current time is 0xFFFF0000 when a time of
   0x00000000 is specified, a delay of 65536 units will be requested.

Prototype:

   int ISysTimer_SetExpiry(ISysTimer *pif, uint32 dwWhen)

Parameters:
   pif: pointer to an ISysTimer interface
   dwWhen: when to set the signal, relative to GetTimeRef()

Return Value:
   AEE_SUCCESS or error code.

Comments:
   None

Side Effects:
   None

=============================================================================*/

#endif // AEEISYSTIMER_H
