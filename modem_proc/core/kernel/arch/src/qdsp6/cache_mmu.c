 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              A R M   C A C H E   F U N C T I O N S   O N   QDSP6 

GENERAL DESCRIPTION
  This module contains the implementation of the AMSS cache operation APIs.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/arch/src/qdsp6/cache_mmu.c#1 $
$DateTime: 2016/12/13 07:59:23 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "assert.h"
#include "cache_mmu.h"
#include "qurt.h"

/*===========================================================================

                     DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                     Undefined Cache functions

===========================================================================*/

/*===========================================================================

                     MODULE FUNCTIONS

===========================================================================*/
void mmu_invalidate_data_cache_lines(void *start_ptr, unsigned int byte_cnt)
{
   int ret = qurt_mem_cache_clean( (qurt_addr_t)start_ptr, byte_cnt, 
         QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE );
   ASSERT(QURT_EOK == ret);
}

void dcache_inval_region(void *addr, unsigned int length)
{
   int ret = qurt_mem_cache_clean( (qurt_addr_t)addr, length, 
         QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE );
   ASSERT(QURT_EOK == ret);
}

void dcache_clean_region(void *addr, unsigned int length)
{
   int ret = qurt_mem_cache_clean( (qurt_addr_t)addr, length, 
         QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE );
   ASSERT(QURT_EOK == ret);
}

void dcache_clean_line(void *addr)
{
   int ret = qurt_mem_cache_clean( (qurt_addr_t)addr, 32, QURT_MEM_CACHE_FLUSH, 
         QURT_MEM_DCACHE );
   ASSERT(QURT_EOK == ret);
}

void dcache_clean_inval_region(void *addr, unsigned int length)
{
   int ret = qurt_mem_cache_clean( (qurt_addr_t)addr, length, 
         QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE );
   ASSERT(QURT_EOK == ret);
}

void memory_barrier(void)
{
   __asm__ __volatile__( "barrier\n" );
}
