/*============================================================================
  FILE:         DALHeap.c

  OVERVIEW:     Provides Memory services

  DEPENDENCIES: None
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/dal/src/DALHeap.c#1 $
  $$DateTime: 2015/09/01 00:30:35 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  09-05-2012  aa  Initial Version
============================================================================*/
/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include "stdlib.h"
#include "comdef.h"

// Use one heap even for physical mem and enforce all mem at 32 byte boundaries
#define MEM_ALIGN_MASK  0x00000001f
#define ALIGN_HEAP( s ) ( ( (uint32)(s) + 0x01f) & ~0x01f)

/*
* Heap is initialized in two stages. At Intial stage dal heap is set
* to immem heap and latter on switched to DDR when available.
* "Free" or "Destroy" do nothing and memory can not be reclaimed.
*/
typedef struct {
    uint32 pHeapBase;
    uint32 pHeapFree;
    uint32 pHeapEnd;
    uint32 iHeapSzFree;
}DALSYSMemState;

typedef struct {
    DALSYSMemState Imem;
    DALSYSMemState Ddr;
}DALSYSHeapState;

static DALSYSHeapState gDALSYSHeap = {{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0},
                                      {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0}
									 } ;
								
void DALSYS_HeapInit(void *pHeapBase, uint32 dwHeapSz, uint32 dwInitType ) 
{  
  
   /* Initialize internal mem if dwInitType is 0 */  
   if ( 0 == dwInitType)
   {
      gDALSYSHeap.Imem.pHeapBase = ALIGN_HEAP((byte *)pHeapBase);
      gDALSYSHeap.Imem.pHeapFree = gDALSYSHeap.Imem.pHeapBase;
      gDALSYSHeap.Imem.pHeapEnd = ALIGN_HEAP(gDALSYSHeap.Imem.pHeapBase + dwHeapSz - MEM_ALIGN_MASK);
      gDALSYSHeap.Imem.iHeapSzFree = gDALSYSHeap.Imem.pHeapEnd - gDALSYSHeap.Imem.pHeapFree;
	}
	/* Initialize external memory otherwise */  
	else 
	{
	  gDALSYSHeap.Ddr.pHeapBase = ALIGN_HEAP((byte *)pHeapBase);
      gDALSYSHeap.Ddr.pHeapFree = gDALSYSHeap.Ddr.pHeapBase;
      gDALSYSHeap.Ddr.pHeapEnd = ALIGN_HEAP(gDALSYSHeap.Ddr.pHeapBase + dwHeapSz - MEM_ALIGN_MASK);
      gDALSYSHeap.Ddr.iHeapSzFree = gDALSYSHeap.Ddr.pHeapEnd - gDALSYSHeap.Ddr.pHeapFree;
	}
}

uint32 IsExternalMemReady(void)
{
   /* DDR not available */
   if(0xFFFFFFFF == gDALSYSHeap.Ddr.pHeapBase)
      return 0;
	  
   return 1;
}


void *malloc(size_t dwSize)
{  
   void *pMem = NULL;
   dwSize = ALIGN_HEAP(dwSize);
   
   if(IsExternalMemReady())
   {
      if (gDALSYSHeap.Ddr.pHeapFree + dwSize > gDALSYSHeap.Ddr.pHeapEnd)
         return pMem;
   
      pMem = (void *)gDALSYSHeap.Ddr.pHeapFree;
      gDALSYSHeap.Ddr.pHeapFree += dwSize;
      gDALSYSHeap.Ddr.iHeapSzFree -=dwSize;
   }
   else 
   {
      if(gDALSYSHeap.Imem.pHeapFree + dwSize > gDALSYSHeap.Imem.pHeapEnd)
	     return pMem;
    
      pMem = (void *)gDALSYSHeap.Imem.pHeapFree;
      gDALSYSHeap.Imem.pHeapFree += dwSize;
      gDALSYSHeap.Imem.iHeapSzFree -=dwSize;
   }
   
   return pMem;
}

void free(void *ptr)
{
   /*do nothing for now */
   return;
}

