/*==============================================================================
@file  DALSysMem_static.c

DALSYS static memory region implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALSysInt.h"
#include <qurt.h>
#include <qurt_memory.h>

/*==============================================================================
          Structures
 ==============================================================================*/

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS
==============================================================================*/
extern void* uheap_malloc(size_t size);
extern void uheap_free(void *p);

/*============================================================================n
      Constants and Macros
==============================================================================*/

/*==============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
DALResult DALSYS_MemAllocQurtRegion( const DALSYSMemReq *pMemReq,
      DALSYSMemInfoEx *pMemInfo, qurt_mem_region_t *p_qmem_region )
{
   if ((DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemReq->memInfo.physicalAddr ) || 
       (DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemReq->memInfo.virtualAddr ))
   {
      return DAL_ERROR;
   }

   if (DALSYS_MEM_CACHE_NONE == pMemReq->memInfo.cacheType)
   {
      pMemInfo->virtualAddr = (DALSYSMemAddr)uheap_malloc(
            pMemReq->memInfo.size);
   }
   else
   {
      if (DAL_SUCCESS != DALSYS_Malloc(pMemReq->memInfo.size, 
               (void **)&pMemInfo->virtualAddr))
      {
         pMemInfo->virtualAddr = NULL;
      }
   }

   if (pMemInfo->virtualAddr)
   {
      pMemInfo->physicalAddr = qurt_lookup_physaddr(pMemInfo->virtualAddr);
      pMemInfo->size = pMemReq->memInfo.size;
      pMemInfo->cacheType = pMemReq->memInfo.cacheType;
      pMemInfo->permission = pMemReq->memInfo.permission;
      pMemInfo->shareType = pMemReq->memInfo.shareType;
      return DAL_SUCCESS;
   }
   else
   {
      pMemInfo->physicalAddr = NULL;
      return DAL_ERROR;
   }
}

void DALSYS_MemFreeQurtRegion( DALSYSMemInfoEx const *pMemInfo, 
      qurt_mem_region_t qmem_region )
{
   if (DALSYS_MEM_CACHE_NONE == pMemInfo->cacheType)
   {
      uheap_free((void *)pMemInfo->virtualAddr);
   }
   else
   {
      DALSYS_Free((void*)pMemInfo->virtualAddr);
   }
}
