/*==============================================================================
  Copyright (c) 2008 Qualcomm Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/dal/framework/src/DALMemDesc.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
12/11/08   ML      Error checking and config switching between membuffers
                   If new size is larger than free and alloc new block
                   More optimal Cache management.
12/14/08   ML      Added more configurability under L4
12/22/08   ML      Added support for unused buffers within buffer desclists
                   Added support for cache maintanence when physical contiguous
                   memory has been used but memory is cached.
==============================================================================*/
#include "DALSys.h"
#include "DALSysCmn.h"
#include "DALFramework.h"
#include "dalconfig.h"

// assumptions for cache line size is always 32
#define CACHE_ALIGN_MASK 0x0000001F
//#define CACHE_ALIGN_MASK 0x00000003    // unittest mask for PC

// default DMA align mask, this needs be communicated in bufdesc in the
// future if necessary
#define DMA_ALIGN_MASK    0x00000000
// use the following for buffer alignment this is for DMA HW alignment and not cache
// currently there is no alignment requirements so disable...
//#define ALIGN_BUF_SIZE( s, mask ) ( ( (uint32)(s) + mask) & ~mask)
#define ALIGN_BUF_SIZE( s, mask ) ( s )

// internal flag in the case the entire list is unused
#define EMPTY_DESC_LIST 0xFFFFFFFE


/*
   Op is either 0 or 1 i.e. ALLOC_PHYS_MEMDESC_OP_WRITE/READ
   Disable can be used to overwrite default cache ops
*/
#define SET_BUF_CACHE_OP_DISABLE( pDescList ) (((pDescList)->hOwnerProc | 2 ))
#define GET_BUF_CACHE_OP_DISABLE( pDescList ) (((pDescList)->hOwnerProc & 2 ))

#define SET_BUF_OPERATION( pDescList, op ) ((pDescList)->hOwnerProc = ((pDescList)->hOwnerProc & ~1) | (op))
#define GET_BUF_OPERATION( pDescList ) ((pDescList)->hOwnerProc & 1)
#define CLEAR_BUF_OP_FLAGS( pDescList ) ((pDescList)->hOwnerProc &= ~3)

typedef struct DAL_MMU_RegionConfig DAL_MMU_RegionConfig;
struct DAL_MMU_RegionConfig
{
   DALSYSMemAddr VirtualAddr;
   DALSYSMemAddr PhysicalAddr;
   uint32        Len;
   uint32        MemProps;
};


// vars inited from properties
static uint32 g_arch_id=0;
static uint32 g_dal_env=0;

#define _DALGETPHYSICALADDR(pMMUCfg,_VirtualAddr) (pMMUCfg->PhysicalAddr+((_VirtualAddr)-pMMUCfg->VirtualAddr))


/*
   Based on Environment create mapping
      NONE
      QCOM-BOOT
      L4
      HLOS

   NOTE: DAL_CONFIG_MMU_SCHEME is an interim solution to accomplishing the
     functionality within this file.
     the options are:
         DAL_CONFIG_MMU_SCHEME_NONE - This indicate that MMU is not enabled
          or that all memory is VA:PA 1:1 and uncached.

         DAL_CONFIG_MMU_SCHEME_QCOM_BOOT - This indicates the MMU is setup
          as WT cached and VA:PA 1:1

         DAL_CONFIG_MMU_SCHEME_L4_PMEM - This indicates the MMU is setup in
          L4/AMSS/APPS style where some memory is VA:PA 1:1 and the rest is
          unknown and DMA must happen to intermediate uncached buffers and
          memcpy-ed to/from caller's buffer

         ALL OTHERS or not defined - Indicate that the SW does not know how
          to analyze caller buffers so it always uses intermediate uncached
          buffers and then memcpy-ies to/from caller's buffers.
          This is what is use by HLOS and in L4 when a buffer address properties
          are not known.
*/


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#if defined(DAL_CONFIG_MMU_SCHEME) && (DAL_CONFIG_MMU_SCHEME == DAL_CONFIG_MMU_SCHEME_L4_PMEM)
//---------------------------------------------------------------------------------
// L4 based settings
//---------------------------------------------------------------------------------
// The following only does something useful under L4 images
// Always define 3 regions and in this order:
//    1) MMHEAP1 if does not exist placeholder
//    2) MMHEAP2 if does not exist placeholder
//    3) Fixed=Phys Contiguous (1:1)

#define DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_BASE_VA       0x0
#define DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_BASE_PA       0x0
#define DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_SIZE          0x20000000

// Make place holders of the following if not defined
// address 0 should not be used within any buffer descriptors
// under L4

#ifndef SCL_MM_HEAP1_BASE
#define SCL_MM_HEAP1_BASE  0x0
#define SCL_MM_HEAP1_SIZE  0x0
#endif
#ifndef SCL_MM_HEAP2_BASE
#define SCL_MM_HEAP2_BASE  0x0
#define SCL_MM_HEAP2_SIZE  0x0
#endif

static DAL_MMU_RegionConfig
 DAL_MMUConfig[3] = {
                      {SCL_MM_HEAP1_BASE ,SCL_MM_HEAP1_BASE, SCL_MM_HEAP1_SIZE ,DALSYS_MEM_PROPS_UNCACHED},
                      {SCL_MM_HEAP2_BASE ,SCL_MM_HEAP2_BASE, SCL_MM_HEAP2_SIZE ,DALSYS_MEM_PROPS_UNCACHED},
                      { DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_BASE_VA,
                        DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_BASE_PA,
                        DAL_CONFIG_MMU_SCHEME_L4_PMEM_FIXED_SIZE, 0 // zezo for mem-props means not defined
                      }
                   };


static void _initPlatformCfg()
{
   int i;
   uint32 cache_policy;

   DALSYS_PROPERTY_HANDLE_DECLARE(hSystemProps);
   DALSYSPropertyVar SystemPropVar;

   DALSYS_GetDALPropertyHandle(0, hSystemProps);

   // get default cache policy for fixed 1:1 mem
   SystemPropVar.Val.dwVal = 0;
   DALSYS_GetPropertyValue(hSystemProps,"FixedMemCachePolicy",0,&SystemPropVar);
   cache_policy = SystemPropVar.Val.dwVal;

   // get default max VA to PA 1:1, VA address
   SystemPropVar.Val.dwVal = 0;
   DALSYS_GetPropertyValue(hSystemProps,"FixedMemLength",0,&SystemPropVar);

   for(i = 0; i < sizeof(DAL_MMUConfig)/sizeof(DAL_MMU_RegionConfig); i++)
   {
      // we always looks for the uninitialized memprops
      // this we consider the entry which is tested last
      // this is 1:1 and cached somehow.
      if(DAL_MMUConfig[i].MemProps == 0)
      {
         if(cache_policy == 0xFFFFFFFF)   //no fixed mem?
         {
            DAL_MMUConfig[i].Len = 0;
            DAL_MMUConfig[i].VirtualAddr = 0;
            DAL_MMUConfig[i].PhysicalAddr = 0;
         }
         else
         {
            // setup memprops default
            if(SystemPropVar.Val.dwVal)
            {
               DAL_MMUConfig[i].Len = SystemPropVar.Val.dwVal;
               DAL_MMUConfig[i].VirtualAddr = 0;
               DAL_MMUConfig[i].PhysicalAddr = 0;
            }
            if(cache_policy)
               DAL_MMUConfig[i].MemProps = cache_policy;
            else
            {
               if(g_arch_id == DAL_CONFIG_ARCH_ARM_V5)
                  DAL_MMUConfig[i].MemProps = DALSYS_MEM_PROPS_CACHED_WT;
               else // else ARM11 and QDSP
                  DAL_MMUConfig[i].MemProps = DALSYS_MEM_PROPS_CACHED_WB;
            }
         }
      }
   }
}

static DAL_MMU_RegionConfig *
_DALFindMMUConfig(DALSYSMemAddr VirtualAddr)
{
   uint32 idx = 0;
   for(idx=0; idx < sizeof(DAL_MMUConfig)/sizeof(DAL_MMU_RegionConfig); idx++)
   {
      DAL_MMU_RegionConfig * pDAL_MMUConfig = &(DAL_MMUConfig[idx]);
      if(VirtualAddr >= pDAL_MMUConfig->VirtualAddr &&
         VirtualAddr < (pDAL_MMUConfig->VirtualAddr + pDAL_MMUConfig->Len))
      {
         return pDAL_MMUConfig;
      }
   }
   //not mapped region in config.
   return NULL;
}


#elif defined(DAL_CONFIG_MMU_SCHEME) && (DAL_CONFIG_MMU_SCHEME == DAL_CONFIG_MMU_SCHEME_QCOM_BOOT)
//---------------------------------------------------------------------------------
// All memory used for DMA Write-Through
//---------------------------------------------------------------------------------
static DAL_MMU_RegionConfig
DAL_MMUConfig[1] = {
                     {0x00000000,0x00000000,0xFFFFFFFF,DALSYS_MEM_PROPS_CACHED_WT},
                   };

static DAL_MMU_RegionConfig *
_DALFindMMUConfig(DALSYSMemAddr VirtualAddr)
{
   return &DAL_MMUConfig[0];
}


static void _initPlatformCfg()
{
   return;
}



#elif defined(DAL_CONFIG_MMU_SCHEME) && (DAL_CONFIG_MMU_SCHEME == DAL_CONFIG_MMU_SCHEME_NONE)
//---------------------------------------------------------------------------------
// Assumes MMU is not enabled so all memory used for DMA is Non-Cached
//---------------------------------------------------------------------------------
static DAL_MMU_RegionConfig
DAL_MMUConfig[1] = {
                     {0x00000000,0x00000000,0xFFFFFFFF,DALSYS_MEM_PROPS_UNCACHED},
                   };

static DAL_MMU_RegionConfig *
_DALFindMMUConfig(DALSYSMemAddr VirtualAddr)
{
   return &DAL_MMUConfig[0];
}


static void _initPlatformCfg()
{
   return;
}

#else
//---------------------------------------------------------------------------------
// Assuming HLOS where MMU is dynamic
//---------------------------------------------------------------------------------
static DAL_MMU_RegionConfig *
_DALFindMMUConfig(DALSYSMemAddr VirtualAddr)
{
   return NULL;
}

static void _initPlatformCfg()
{
   return;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------





static void _Init(void)
{
   // Only static in this file which gets shared by multiple threads
   // if colision its okay if two threads update, it will produce the same
   // result
   volatile static uint32 inited=0;
   DALSYS_PROPERTY_HANDLE_DECLARE(hSystemProps);
   DALSYSPropertyVar SystemPropVar;

   if(inited)  //initialized?
      return;

   // Get arch_id first
   hSystemProps[0] = 0;
   SystemPropVar.Val.pdwVal = 0;
   DALSYS_GetDALPropertyHandle(0, hSystemProps);
   DALSYS_GetPropertyValue(hSystemProps,DALPROP_TARGET_PROP_NAME,0,&SystemPropVar);
   if(SystemPropVar.Val.pdwVal)
   {
      g_arch_id = SystemPropVar.Val.pdwVal[DALPROP_TARGET_ARCH_IDX];
   }

   // Get SW Environment and initialize lookup table
   SystemPropVar.Val.dwVal = 0;
   DALSYS_GetPropertyValue(hSystemProps,"DalEnv",0,&SystemPropVar);
   g_dal_env = SystemPropVar.Val.dwVal;

   // init platform specific settings
   _initPlatformCfg();

   inited = 1;    // okay now initialized
}



DALSysMemDescBuf *
DALFW_MemDescBufPtr(DALSysMemDescList * pMemDescList, uint32 idx)
{
   if(pMemDescList && (idx < pMemDescList->dwNumDescBufs))
      return &(pMemDescList->BufInfo[idx]);

   return NULL;
}

DALResult
DALFW_MemDescInit(DALSYSMemHandle hMem, DALSysMemDescList * pMemDescList,
                   uint32 dwNumBufs)
{
   uint32 dwAttrib=0;
   DALSYSMemObjBase *pMemObj = (DALSYSMemObjBase *) DALSYSCMN_HANDLE_TO_OBJECT(hMem);

   if(!pMemDescList || !dwNumBufs)
       return DAL_ERROR;

   DALSYSCMN_RESET_ALL_BITS(pMemDescList->dwObjInfo);
   DALSYSCMN_SET_OBJECT_ID(pMemDescList->dwObjInfo,DALSYSCMN_MEM_DESC_OBJECT_ID);

   if(pMemObj)
   {
      DALSYSCMN_GET_ATTRIB(pMemObj->dwObjInfo,dwAttrib);
      pMemDescList->hOwnerProc = pMemObj->hOwnerProc;
      pMemDescList->PhysicalAddr = pMemObj->PhysicalAddr;
      pMemDescList->VirtualAddr = pMemObj->VirtualAddr;
   }
   else
   {
      pMemDescList->hOwnerProc = 0xFFFFFFFF;
      pMemDescList->PhysicalAddr = (DALSYSMemAddr)0xFFFFFFFF;
      pMemDescList->VirtualAddr = (DALSYSMemAddr)0xFFFFFFFF;
   }
   DALSYSCMN_SET_ATTRIB(pMemDescList->dwObjInfo,dwAttrib);
   CLEAR_BUF_OP_FLAGS(pMemDescList);
   pMemDescList->thisVirtualAddr = (DALSYSMemAddr)pMemDescList;
   pMemDescList->dwNumDescBufs = dwNumBufs;
   pMemDescList->dwCurBufIdx = 0;
   memset(pMemDescList->BufInfo, -1, sizeof(DALSysMemDescBuf)*dwNumBufs);
   return DAL_SUCCESS;
}

DALResult
DALFW_MemDescAddBuf(DALSysMemDescList * pMemDescList, uint32 bufIdx,
                     uint32 offset, uint32 size, uint32 userInfo)
{
   if(pMemDescList && bufIdx<pMemDescList->dwNumDescBufs
      && pMemDescList->PhysicalAddr != (DALSYSMemAddr)0xFFFFFFFF)
   {
      DALSysMemDescBuf *pMemDescBuf = &pMemDescList->BufInfo[bufIdx];
      pMemDescBuf->VirtualAddr = pMemDescList->VirtualAddr+offset;
      pMemDescBuf->PhysicalAddr = pMemDescList->PhysicalAddr+offset;
      pMemDescBuf->size = size;
      pMemDescBuf->user = userInfo;
      return DAL_SUCCESS;
   }
   return DAL_ERROR;
}


static void _MemDescCache( uint32 cachePolicy, uint32 operation, DALSYSMemAddr virtAddr, uint32 size )
{
   // quick test for uncached, unknown or unused buffer
   if((cachePolicy == DALSYS_MEM_PROPS_UNCACHED) || (cachePolicy == 0) || (virtAddr == DALSYS_MEM_ADDR_NOT_SPECIFIED))
      return;

   if(ALLOC_PHYS_MEMDESC_OP_READ == operation)   //read?
   {
      if((virtAddr & CACHE_ALIGN_MASK) || ((virtAddr + size) & CACHE_ALIGN_MASK))
         DALSYS_CacheCommand(DALSYS_CACHE_CMD_FLUSH, virtAddr, size); // not aligned assume single threading
      else
         DALSYS_CacheCommand(DALSYS_CACHE_CMD_INVALIDATE, virtAddr, size); //aligned
   }
   else if(cachePolicy == DALSYS_MEM_PROPS_CACHED_WB) // operation=write and cache=WB
   {
      DALSYS_CacheCommand(DALSYS_CACHE_CMD_CLEAN, virtAddr, size);
   }
}


static DALSYSMemHandle
_PopulateOutDescList(DALSYSMemHandle hMem,
                          DALSysMemDescList *pInDescList,
                          DALSysMemDescList *pOutDescList,
                          uint32 operation)
{
   uint32 dwLen;
   uint32 offset;
   uint32 idx;
   DAL_MMU_RegionConfig * pMMU_Config = NULL;
   DAL_MMU_RegionConfig * pMMU_OrigConfig = NULL;
   DALSysMemDescBuf * pInDescBuf = NULL;
   DALSysMemDescBuf * pOutDescBuf = NULL;
   uint32 firstValidBufferIdx=DALSYS_MEM_ADDR_NOT_SPECIFIED;
   uint32 dma_align_mask = DMA_ALIGN_MASK;

   dwLen = 0;
   pInDescBuf = &pInDescList->BufInfo[0];

   for(idx=0;idx<pInDescList->dwNumDescBufs;idx++,pInDescBuf++)
   {
      if(pInDescBuf->VirtualAddr == DALSYS_MEM_ADDR_NOT_SPECIFIED)
         continue;

      // we have a buffer

      // update total size
      dwLen += ALIGN_BUF_SIZE(pInDescBuf->size, dma_align_mask);

      //check for overflow
      if(dwLen < pInDescBuf->size)
      {
         dwLen = 0;
         break;
      }
    
      // check if first valid buffer and init
      if(firstValidBufferIdx == DALSYS_MEM_ADDR_NOT_SPECIFIED)
      {
         firstValidBufferIdx = idx;
         // Initialize this if user forgot to do so...
         if(pInDescList->VirtualAddr == DALSYS_MEM_ADDR_NOT_SPECIFIED)
            pInDescList->VirtualAddr = pInDescBuf->VirtualAddr;
         pMMU_OrigConfig = _DALFindMMUConfig(pInDescList->VirtualAddr);
      }

      // keep making sure that all buffers are compatible
      if(pMMU_OrigConfig)
      {
         pMMU_Config = _DALFindMMUConfig(pInDescBuf->VirtualAddr);
         if(pMMU_Config) // found buffer address in mapping
         {
            if(g_dal_env >= DALPROP_MOD_ENV_QCOM)
            {
               // check if buffer is not cache aligned and cache is on for this block
               // TODO: perhaps this could be further optimized based on operation and WT
               if(pMMU_Config->MemProps != DALSYS_MEM_PROPS_UNCACHED)
               {
                  if(pInDescBuf->VirtualAddr & CACHE_ALIGN_MASK)
                     pMMU_OrigConfig = NULL;
                  if((pInDescBuf->VirtualAddr + pInDescBuf->size) & CACHE_ALIGN_MASK)
                     pMMU_OrigConfig = NULL;
               }
            }
            if(pInDescBuf->VirtualAddr & dma_align_mask)
               pMMU_OrigConfig = NULL;
         }
         else
         {
            pMMU_OrigConfig = NULL; // dont allow some in fixed regions some not
         }
      }
   }

   // No data buffers in input
   if(dwLen == 0)
      return (void *)EMPTY_DESC_LIST;

   if(NULL == pMMU_OrigConfig)
   {
      //no mapped region configured. need to allocate memory if not passed in?
      DALSYSMemObjBase *pMemObj = (DALSYSMemObjBase *) DALSYSCMN_HANDLE_TO_OBJECT(hMem);
      if(pMemObj)
      {
         // has size exceeded previous allocation
         if(dwLen > pMemObj->dwLen)
         {
            // free it and allocate a bigger size
            DALSYS_DestroyObject(hMem);
            hMem = NULL;
            pMemObj = NULL;
         }
      }

      if(NULL == pMemObj)
      {
         //create a new mem object after determining len of this region
         if(DAL_SUCCESS !=  DALSYS_MemRegionAlloc((DALSYS_MEM_PROPS_NORMAL|
                                                   DALSYS_MEM_PROPS_PHYS_CONT|
                                                   DALSYS_MEM_PROPS_UNCACHED),
                                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                                   DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                                   dwLen,&hMem, NULL))
         {
            return (DALSYSMemHandle)DALSYS_MEM_INVALID_ADDR;
         }
      }
      DALFW_MemDescInit(hMem, pOutDescList, pInDescList->dwNumDescBufs);
   }
   else
   {
      DALFW_MemDescInit(NULL, pOutDescList, pInDescList->dwNumDescBufs);
      pOutDescList->hOwnerProc   = pInDescList->hOwnerProc;
      pOutDescList->PhysicalAddr = _DALGETPHYSICALADDR(pMMU_OrigConfig, pInDescList->VirtualAddr);
      pOutDescList->VirtualAddr  = pInDescList->VirtualAddr;
      pOutDescList->dwObjInfo    = pMMU_OrigConfig->MemProps;  //store props here
   }

   // set operation on outdesc list
   SET_BUF_OPERATION(pOutDescList, operation);

   offset = 0;
   pInDescBuf = &pInDescList->BufInfo[0];
   pOutDescBuf = &pOutDescList->BufInfo[0];
   for(idx=0;idx<pInDescList->dwNumDescBufs; idx++ , pInDescBuf++, pOutDescBuf++ )
   {
      if(pInDescBuf->VirtualAddr == DALSYS_MEM_ADDR_NOT_SPECIFIED)
      {
         pOutDescBuf->size             = pInDescBuf->size;
         pOutDescBuf->user             = pInDescBuf->user;
         continue;
      }

      if(pMMU_OrigConfig)
      {
         // update outdesc to match input along with physical
         pMMU_Config = _DALFindMMUConfig(pInDescBuf->VirtualAddr);
         pOutDescBuf->VirtualAddr      = pInDescBuf->VirtualAddr;
         pOutDescBuf->PhysicalAddr     = _DALGETPHYSICALADDR(pMMU_Config, pInDescBuf->VirtualAddr);
      }
      else
      {
         // create buffers out of allocated memory
         pOutDescBuf->VirtualAddr      = pOutDescList->VirtualAddr+offset;
         pOutDescBuf->PhysicalAddr     = pOutDescList->PhysicalAddr+offset;
         // increment offset
         offset += ALIGN_BUF_SIZE(pInDescBuf->size, dma_align_mask);
      }
      pOutDescBuf->size             = pInDescBuf->size;
      pOutDescBuf->user             = pInDescBuf->user;
      // now do cache coherency stuff, but only for read and if not overriden
      // write must be done at copybufs
      if((operation == ALLOC_PHYS_MEMDESC_OP_READ) && (GET_BUF_CACHE_OP_DISABLE(pOutDescList) == 0))
      {
         _MemDescCache( pOutDescList->dwObjInfo & DALSYS_MEM_PROPS_CACHED_MASK, operation,
              pOutDescBuf->VirtualAddr, pOutDescBuf->size );
      }

   }
   return hMem;
}

DALSYSMemHandle
DALFW_AllocPhysForMemDescBufs( DALSYSMemHandle hMem,
                                uint32 operation,
                                DALSysMemDescList *pInDescList,
                                DALSysMemDescList ** pOutDescList )
{
   DALSysMemDescBuf * pInDescBuf;
   DALSYSMemHandle htempMem;
   uint32 idx;

   _Init();

   // check for uninitialized hMem, it must be pointer or NULL
   if(hMem == (DALSYSMemHandle)DALSYS_MEM_ADDR_NOT_SPECIFIED)
      return hMem;
   SET_BUF_OPERATION(pInDescList, operation);

   if(pInDescList->PhysicalAddr != (DALSYSMemAddr)DALSYS_MEM_ADDR_NOT_SPECIFIED)
   {
      //we have physical address, just asign the out as in
      *pOutDescList = pInDescList;
      //check if memory is cached
      if((pInDescList->dwObjInfo & DALSYS_MEM_PROPS_CACHED_MASK) != DALSYS_MEM_PROPS_UNCACHED)
      {
         if( ! GET_BUF_CACHE_OP_DISABLE(pInDescList) )
         {
            //perform cache coherency stuff directly on input buffers
            //memory must already cache friendly
            pInDescBuf = &pInDescList->BufInfo[0];
            for(idx=0;idx<pInDescList->dwNumDescBufs;idx++,pInDescBuf++)
            {
               // check each buffer if used
               if(pInDescBuf->VirtualAddr != (DALSYSMemAddr)DALSYS_MEM_ADDR_NOT_SPECIFIED)
               {
                  _MemDescCache( pInDescList->dwObjInfo & DALSYS_MEM_PROPS_CACHED_MASK, operation,
                        pInDescBuf->VirtualAddr, pInDescBuf->size );
               }
            }
         }
      }
      return hMem;
   }
   else
   {
      //populate the output list
      htempMem = _PopulateOutDescList(hMem,pInDescList,*pOutDescList,operation);
      if(htempMem == (void *)EMPTY_DESC_LIST) // flag for no buffers in indesclist
      {
         *pOutDescList = pInDescList;
         return hMem;
      }
      return htempMem;  //return actual handle
   }
}

DALResult
DALFW_MemDescListCopyBufs( DALSysMemDescList * pDestDescList,
                           DALSysMemDescList * pSrcDescList )
{
   uint32 idx = 0;
   DALSysMemDescBuf *pSrcDescBuf;
   DALSysMemDescBuf *pDestDescBuf;
   uint32 operation;
   uint32 notCacheCmd; // indicate user does not want cache ops (override)

   _Init();

   if((pDestDescList == NULL) || (pSrcDescList == NULL))
      return DAL_ERROR;

   operation = GET_BUF_OPERATION( pSrcDescList ); // operation is saved in both in and out

   // destination must be at least as big as source
   if(pDestDescList->dwNumDescBufs < pSrcDescList->dwNumDescBufs)
      return DAL_ERROR;

   // Uninitialized lists?
   if(pSrcDescList->VirtualAddr == (DALSYSMemAddr)DALSYS_MEM_ADDR_NOT_SPECIFIED)
      return DAL_ERROR;
   if(pDestDescList->VirtualAddr == (DALSYSMemAddr)DALSYS_MEM_ADDR_NOT_SPECIFIED)
      return DAL_ERROR;

   // setup for overrides
   notCacheCmd = 0; // assume do cache commands based on bufs
   if(operation == ALLOC_PHYS_MEMDESC_OP_WRITE)
      notCacheCmd = GET_BUF_CACHE_OP_DISABLE(pDestDescList);
   else
      notCacheCmd = GET_BUF_CACHE_OP_DISABLE(pSrcDescList);

   pSrcDescBuf  = &pSrcDescList->BufInfo[0];
   pDestDescBuf = &pDestDescList->BufInfo[0];

   for(idx=0;idx<pSrcDescList->dwNumDescBufs;idx++)
   {
      // do cache commands as needed...
      if(!notCacheCmd) // check for override first...
      {
         if(operation == ALLOC_PHYS_MEMDESC_OP_WRITE)
         {
            _MemDescCache( pDestDescList->dwObjInfo & DALSYS_MEM_PROPS_CACHED_MASK, operation,
                  pDestDescBuf->VirtualAddr, pDestDescBuf->size );
         }
         else
         {
            // READ use source but only do this for ARMV7 which has speculative
            // loads and requires 128 byte alignment
            // we could test and not do this but this is already complex enough...
            if (DAL_CONFIG_ARCH_ARM_V7 == g_arch_id)
            {
               _MemDescCache( pSrcDescList->dwObjInfo & DALSYS_MEM_PROPS_CACHED_MASK, operation,
                     pSrcDescBuf->VirtualAddr, pSrcDescBuf->size );
            }
         }
      }

      // Note this test also works for unused buffers
      if(pSrcDescBuf->VirtualAddr != pDestDescBuf->VirtualAddr)
         DALSYS_memscpy((void *)pDestDescBuf->VirtualAddr, 
               pDestDescBuf->size,
               (void *)pSrcDescBuf->VirtualAddr,pSrcDescBuf->size);

      pSrcDescBuf++;
      pDestDescBuf++;
   }
   return DAL_SUCCESS;
}

