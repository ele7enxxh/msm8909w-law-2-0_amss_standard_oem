#ifndef QSEE_MMU_H
#define QSEE_MMU_H

#include "tzbsp_shim.h"
#include "tt_tz.h"
#include "tzbsp_mmu_cache.h"

/*Bit mask of permissions*/
#define PERM_R 04
#define PERM_W 02
#define PERM_X 01

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

void qsee_map_region(unsigned int vaddr, unsigned int paddr, unsigned int size, unsigned int perms, unsigned int attr, unsigned int user);

#endif /* MMU_H */
