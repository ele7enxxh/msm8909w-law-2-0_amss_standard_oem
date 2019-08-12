#ifndef QSEE_MAP_H
#define QSEE_MAP_H

#include "lktypes.h"
#include "lkthread.h"
#include "tzbsp_shim.h"
#include "mmu.h"


/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

void qsee_map_thread_regions(thread_t * pthread, boolean map);

#endif /* MMU_H */
