/*==============================================================================

FILE:      CorePool.c

DESCRIPTION: Memory pool implementation

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright ?2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/src/CorePool.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/16/09   yz      Initial import from sg's npa_pool.c

==============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "CorePool.h"
#include "CoreVerify.h"

#include <stdio.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct CorePoolBlockType
{
  struct CorePoolBlockType    *pNext;
  unsigned int                 uSize;
  char                         aData[];
} CorePoolBlockType;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

void Core_PoolLock( CorePoolType *pPool )
{
  DALSYS_SyncEnter(pPool->hLock);
}

void Core_PoolUnlock( CorePoolType *pPool )
{
  DALSYS_SyncLeave(pPool->hLock);
}

static void Core_PoolPush( CorePoolType *pPool, CorePoolElementType *pElem )
{
  CORE_VERIFY_PTR( pElem );

  /* Must be locked */
  Core_PoolLock( pPool );
  pElem->pNext = pPool->pFreeList;
  pPool->pFreeList = pElem;
  Core_PoolUnlock( pPool );
}

static CorePoolElementType *Core_PoolPop( CorePoolType *pPool )
{
  CorePoolElementType *pElem;

  /* Must be locked */
  Core_PoolLock( pPool );
  pElem = pPool->pFreeList;
  if ( NULL != pElem ) 
  { 
    pPool->pFreeList = pElem->pNext;
  }
  Core_PoolUnlock( pPool );
  return pElem;
}

static void *Core_PoolAddBlock( CorePoolType *pPool, void *pData, unsigned int *pBlockSize )
{
  CorePoolBlockType *pBlock = (CorePoolBlockType *)pData;

  CORE_VERIFY_PTR( pBlockSize );
  CORE_VERIFY ( *pBlockSize >= sizeof( CorePoolBlockType ) );
  pBlock->uSize = *pBlockSize;

  Core_PoolLock( pPool );
  pBlock->pNext = pPool->pBlockList;
  pPool->pBlockList = pBlock;
  pPool->uBlockCount++;

  Core_PoolUnlock( pPool );

  *pBlockSize -= sizeof( CorePoolBlockType ); 

  return &pBlock->aData;
}

static void Core_PoolFill( CorePoolType *pPool, void *pData, unsigned int uBlockSize )
{
  pData = Core_PoolAddBlock( pPool, pData, &uBlockSize );
  CORE_VERIFY_PTR( pData );

  while ( uBlockSize >= pPool->uElemSize )
  {
    CorePoolElementType *pElem = (CorePoolElementType *)pData;

    Core_PoolPush( pPool, pElem );

    Core_PoolLock( pPool );
    pPool->uElemCount++;
    Core_PoolUnlock( pPool );

    pData = (char *)pData + pPool->uElemSize;
    uBlockSize -= pPool->uElemSize;
  }
}

/*------------------------------------------------------------------------------
  public functions
------------------------------------------------------------------------------*/

void Core_PoolInit( CorePoolType *pPool, 
                    unsigned int uElemSize, 
                    void *pData, 
                    unsigned int uBlockSize )
{
  CORE_VERIFY_PTR( pPool );
  memset( pPool, 0, sizeof( CorePoolType ) );

  pPool->uElemSize  = MAX( uElemSize, sizeof(CorePoolElementType) );
  CORE_DAL_VERIFY( DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &pPool->hLock, NULL) );

  if ( 0 != uBlockSize )
  {
    CORE_VERIFY_PTR( pData );
    pPool->uBlockSize = uBlockSize;
    Core_PoolFill( pPool, pData, uBlockSize );
  }
}

void Core_PoolCreate( CorePoolType *pPool, 
                      unsigned int uElemNum, 
                      unsigned int uElemSize)
{
  void * pData;
  unsigned int uBlockSize = CORE_POOL_BLOCK_SIZE( uElemNum, uElemSize );

  CORE_VERIFY_PTR( pPool );
  CORE_DAL_VERIFY( DALSYS_Malloc(uBlockSize, &pData) );

  Core_PoolInit(pPool, uElemSize, pData, uBlockSize);
  Core_PoolUseAllocatorFcn(pPool, Core_DALSYSAllocatorFcn);
}

void Core_PoolConfig( CorePoolType *pPool, 
                      const CorePoolConfigType *pConfig )
{
  CORE_VERIFY_PTR( pPool );
  CORE_VERIFY_PTR( pConfig );

  Core_PoolCreate( pPool, pConfig->initial, pConfig->size );
  Core_PoolSetAllocationSize( pPool, pConfig->subsequent );
}

void *Core_PoolAlloc( CorePoolType *pPool )
{
  CorePoolElementType *pElem = Core_PoolPop( pPool );
  CORE_VERIFY_PTR( pPool );

  if ( ( NULL == pElem ) && 
       ( NULL != pPool->pfnAllocatorFcn ) && 
       ( pPool->uBlockSize > 0 ) )
  {
    unsigned int uPoolSize = pPool->uBlockSize;
    void *pData = pPool->pfnAllocatorFcn( uPoolSize );
    if ( NULL != pData )
    {
      Core_PoolFill( pPool, pData, uPoolSize );
      pElem = Core_PoolPop( pPool );
    }
  }
  return pElem;
}

void Core_PoolFree( CorePoolType *pPool, void *pData )
{
  CORE_VERIFY_PTR( pPool );
  CORE_VERIFY_PTR( pData );
  Core_PoolPush( pPool, (CorePoolElementType *)pData );
}

void Core_PoolSetAllocationSize( CorePoolType *pPool, 
                                 unsigned int  uElemNum)
{
  CORE_VERIFY_PTR( pPool );
  pPool->uBlockSize = CORE_POOL_BLOCK_SIZE(uElemNum, pPool->uElemSize);
}

void *Core_DALSYSAllocatorFcn( unsigned int uSize )
{
  void *pData;

  if( DAL_SUCCESS != DALSYS_Malloc( uSize, &pData ) )
    return NULL;

  return pData;
}

void Core_PoolUseAllocatorFcn( CorePoolType *pPool, void *(*pfnAllocatorFcn)( unsigned int uSize ) )
{
  CORE_VERIFY_PTR( pPool );
  pPool->pfnAllocatorFcn = pfnAllocatorFcn;
}
