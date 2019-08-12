#ifndef AEEATOMIC_H
#define AEEATOMIC_H
/*
=======================================================================

FILE:         AEEatomic.h

SERVICES:     atomic

DESCRIPTION:  Fast Atomic ops

=======================================================================
        Copyright 2005, 2007 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
=======================================================================
*/

#include "ds_Utils_StdDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

uint32 atomic_Add(uint32* puDest, int nAdd);
uint32 atomic_Exchange(uint32* puDest, uint32 uVal);
uint32 atomic_CompareAndExchange(uint32 *puDest, uint32 uExchange, 
                                 uint32 uCompare);
uint32 atomic_CompareOrAdd(uint32* puDest, uint32 uCompare, int nAdd);
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*=====================================================================
INTERFACE DOCUMENTATION
=======================================================================
atomic Interface

  The atomic interface provides fast "atomic" operations.  The 
   operations are defined to be atomic with respect to each other.

=======================================================================

=======================================================================

atomic_Add()

Description:

   Performs an atomic sum operation.

Prototype:

   uint32 atomic_Add(uint32* puDest, int nInc);

Parameters:
   puDest [in|out] : Points to unsigned number to add nInc and save
   nInc : increment

Return Value:
   result.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

atomic_Exchange()

Description:

   Atomic exchange of 32bit value. Performs an atomic operation of :
      write uVal to *puDest 
      return the previous value in *puDest

Prototype:

   uint32 atomic_Exchange(uint32* puDest, uint32 uVal);

Parameters:
   puDest [in|out] : Points to unsigned number to be exchanged
   uVal : new value to write.

Return Value:
   previous value at *puDest.

Comments:
   None

Side Effects:
   May cause exception if puDest is not a 32 bit aligned address.

See Also:
   None
=======================================================================

atomic_CompareAndExchange()

Description:

   Performs an atomic operation of :
      if (*puDest == uCompare) {
         *puDest = uExchange;
      }

   returns the previous value in *puDest

Prototype:

   uint32 atomic_CompareAndExchange(uint32 *puDest, uint32 uExchange, 
                                    uint32 uCompare);

Parameters:
   puDest [in|out] : Points to unsigned number.
   uExchange : A new value to write to *puDest
   uCompare : Comparand

Return Value:
   previous value at *puDest.

Comments:
   None

Side Effects:
   May cause exception if puDest is not a 32 bit aligned address.

See Also:
   None

=======================================================================
atomic_CompareOrAdd()

Description:

   Performs an atomic operation of :
      if (*puDest != uCompare) {
         *puDest += nAdd;
      }

   returns the new value in *puDest

Prototype:

   uint32 atomic_CompareOrAdd(uint32 *puDest, uint32 uCompare, int nAdd);

Parameters:
   puDest [in|out] : Points to unsigned number.
   uCompare : Comparand
   nAdd : Add to *puDest

Return Value:
   new value at *puDest.

Comments:
   None

Side Effects:
   May cause exception if puDest is not a 32 bit aligned address.

See Also:
   None
=======================================================================*/

#endif /* #ifndef AEEATOMIC_H */

