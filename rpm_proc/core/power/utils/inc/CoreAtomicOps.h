#ifndef COREATOMICOPS_H
#define COREATOMICOPS_H
/*==============================================================================

FILE:      CoreAtomicOps.h

DESCRIPTION: Atomic increment and decrement

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
 			QUALCOMM Proprietary/GTDR
 
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreAtomicOps.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
08/20/10   ddh     added Q6 support
04/05/10   yz      Initial version
 
==============================================================================*/

#include <stdint.h>

static inline uint32_t Core_AtomicInc(volatile uint32_t * pCount) 
{
  uint32_t new_val;
  do
  {
    new_val = __ldrex(pCount) + 1;
  } while(__strex(new_val, pCount));

  return new_val;
}

static inline uint32_t Core_AtomicDec(volatile uint32_t * pCount) 
{
  uint32_t new_val;
  do
  {
    new_val = __ldrex(pCount) - 1;
  } while(__strex(new_val, pCount));

  return new_val;
}

#endif /* COREATOMICOPS_H */

