/*===========================================================================

                                Boot L2 Cache Controller                                    

GENERAL DESCRIPTION
  This file contains implementation of L2 cache controller operations

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_l2cc.c#1 $
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $

when       who          what, where, why
--------   --------     -----------------------------------------------------
12/14/11   aus          Added support for flush and disable in one function
11/28/11   aus          Initial Creation
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_msm.h"
#include "comdef.h"
#include "boot_target.h"

/*===========================================================================

                           DEFINITIONS

===========================================================================*/
/*
 * Determine if the value 'x' is aligned to a multiple of 'a'.
 * 'a' must be a power of two greater than 0.
 */
#define IS_CACHE_ALIGNED(x, a) (((uint32)(x) & ((a) - 1UL)) == 0UL)

/*===========================================================================

                           FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================

**  Function :  l2_cache_clean_by_pa

** ==========================================================================
*/
/*!
* 
* @brief
*   Clean the given cache line
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void l2_cache_clean_by_pa(uint32 pa)
{
    L2CC_CLN_BY_PA = (uint32)pa & ~(L2CC_LINE_SIZE - 1UL);
}


/*=========================================================================

**  Function :  l2_cache_invalidate_by_pa

** ==========================================================================
*/
/*!
* 
* @brief
*   Invalidate the given cache line
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void l2_cache_invalidate_by_pa(uint32 pa)
{
    L2CC_IVLD_BY_PA = (uint32)pa & ~(L2CC_LINE_SIZE - 1UL);
}


/*=========================================================================

**  Function :  l2_cache_flush_by_pa

** ==========================================================================
*/
/*!
* 
* @brief
*   Flush the given cache line
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void l2_cache_flush_by_pa(uint32 pa)
{
    L2CC_IVLD_CLN_BY_PA = (uint32)pa & ~(L2CC_LINE_SIZE - 1UL);
}

/*=========================================================================

**  Function :  cache_operation

** ==========================================================================
*/
/*!
* 
* @brief
*   Do a full cache flush/clean operation on all ways
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void cache_operation(volatile uint32 *reg)
{
	/*Start background operation*/
	*reg = 0xFFFF;

	/*poll for background operation to complete*/
	while (*reg != 0);
}


/*=========================================================================

**  Function :  l2_cache_enable

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables the L2 cache controller
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_enable(void)
{
    L2CC_CONTROL = 1UL;
}

/*=========================================================================

**  Function :  l2_cache_disable

** ==========================================================================
*/
/*!
* 
* @brief
*   Disables the L2 cache controller
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_disable(void)
{
    L2CC_CONTROL = 0UL;
}

/*=========================================================================

**  Function :  l2_cache_flush_and_disable

** ==========================================================================
*/
/*!
* 
* @brief
*   Flushes the L2 cache and disables it in one function. This prevents caching 
*   of the stack and leaving any valid lines in the cache. 
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_flush_and_disable(void)
{
	/*Start background operation*/
	L2CC_IVLD_CLN_BY_WAY = 0xFFFF;

	/*poll for background operation to complete*/
	while (L2CC_IVLD_CLN_BY_WAY != 0);
	
	/*Disable the cache*/
	L2CC_CONTROL = 0UL;
}

/*=========================================================================

**  Function :  l2_cache_flush

** ==========================================================================
*/
/*!
* 
* @brief
*   Flushes the L2 cache. This includes clean and invalidation of the entire
*   L2 cache.
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_flush(void)
{
    cache_operation(&(L2CC_IVLD_CLN_BY_WAY));
}

/*=========================================================================

**  Function :  l2_cache_clean

** ==========================================================================
*/
/*!
* 
* @brief
*   Cleans the L2 cache
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_clean(void)
{
    cache_operation(&(L2CC_CLN_BY_WAY));
}

/*=========================================================================

**  Function :  l2_cache_invalidate

** ==========================================================================
*/
/*!
* 
* @brief
*   Invalidates the L2 cache
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_cache_invalidate(void)
{    
    cache_operation(&(L2CC_IVLD_BY_WAY));
}

/*=========================================================================

**  Function :  l2_ranged_cache_invalidate

** ==========================================================================
*/
/*!
* 
* @brief
*   Invalidates a range of addresses
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_ranged_cache_invalidate(uint32 pa, uint32 size)
{
    uint32 pa_end, pa_start;
		
	/* Find the first PA, aligned to the cache boundary, to be invalidated. Clean and 
	   invalidate this cache line */
    pa_start = pa & ~(L2CC_LINE_SIZE-1UL);
	l2_cache_flush_by_pa((uint32)pa_start);
	
	/* Find the last PA, aligned to the cache boundary, to be invalidated. Clean and 
	   invalidate this cache line */
    pa_end = (pa + size) & ~(L2CC_LINE_SIZE-1UL);	   
	l2_cache_flush_by_pa((uint32)pa_end);	
	
	/* Invalidate the rest of the cache lines */   
    while(pa_start <= pa_end)
	{
        l2_cache_invalidate_by_pa((uint32)pa_start);
		pa_start += L2CC_LINE_SIZE;
    }
}

/*=========================================================================

**  Function :  l2_ranged_cache_flush

** ==========================================================================
*/
/*!
* 
* @brief
*   Flushes a range of addresses
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_ranged_cache_flush(uint32 pa, uint32 size)
{
    uint32 pa_end, pa_start;
		
	/* Find the first PA, aligned to the cache boundary, to be flushed */
    pa_start = pa & ~(L2CC_LINE_SIZE-1UL);
	
	/* Find the last PA, aligned to the cache boundary, to be flushed */
    pa_end = (pa + size) & ~(L2CC_LINE_SIZE-1UL);	   
	
	/* Flush the cache lines */   
    while(pa_start <= pa_end)
	{
        l2_cache_flush_by_pa((uint32)pa_start);
		pa_start += L2CC_LINE_SIZE;
    }
}

/*=========================================================================

**  Function :  l2_ranged_cache_clean

** ==========================================================================
*/
/*!
* 
* @brief
*   Cleans a range of addresses
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void l2_ranged_cache_clean(uint32 pa, uint32 size)
{
    uint32 pa_end, pa_start;
		
	/* Find the first PA, aligned to the cache boundary, to be cleaned */
    pa_start = pa & ~(L2CC_LINE_SIZE-1UL);
	
	/* Find the last PA, aligned to the cache boundary, to be cleaned */
    pa_end = (pa + size) & ~(L2CC_LINE_SIZE-1UL);	   
	
	/* Clean the cache lines */   
    while(pa_start <= pa_end)
	{
        l2_cache_clean_by_pa((uint32)pa_start);
		pa_start += L2CC_LINE_SIZE;
    }
}

/*=========================================================================

**  Function :  boot_init_l2_cache

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the L2 cache controller
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_init_l2_cache(void)
{
    /* Disable L2 CC */
    l2_cache_disable();

    /* Write to the following registers -
       Auxiliary Control: Associativity & Way Size 
       Tag and Data RAM Latency Control: Latencies for RAM accesses */
    L2CC_AUX_CONTROL = L2CC_AUX_CONTROL_INIT_VALUE;
    L2CC_TAG_RAM_CONTROL = L2CC_TAG_RAM_CONTROL_INIT_VALUE;
    L2CC_DATA_RAM_CONTROL = L2CC_DATA_RAM_CONTROL_INIT_VALUE;

    /* Write to the Invalidate by Way, to invalidate all entries in cache */
    l2_cache_invalidate();

    /* Write to Control Register 1 with the LSB set to 1 to enable the cache. */
    l2_cache_enable();	
}

