/*==================-===========================================================
  Copyright (c) 2009 - 2013
  Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/dal/src/DALGlbCtxt.c#1 $

when       who     description
--------   ---     -------------------------------------------------------------
06/19/12   aa      Cleanup Featurization and enabling SMEM
01/17/11   sc      Removed all FEATURE_SPINLOCK_NOT_USED flags
10/01/10   sc      Stub out spinlock for SPS/ARM7
09/07/10   sc      Fixed the compiler warning
03/12/09   VK      Changed DALLOCK prefix to DALGLBCTXT
03/07/08   VK      Initial revision

==============================================================================*/

#include "stringl/stringl.h"
#include "assert.h"
#include "smem.h"

#include "dalconfig.h"
#include "DALSysInt.h"
#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"

// pages size and lock area size in page units
#define DALGLBCTXT_PAGE_SIZE                    0x1000
#define DALGLBCTXT_SMEM_AREA_SIZE               0x1000
#define DALGLBCTXT_CACHE_LINE_SIZE_BYTES        16
#define DALGLBCTXT_SLEEP_MS                     1
#define DALGLBCTXT_MAX_SLEEP_MS                 500
#define DALGLBCTXT_strlen(name) ((strlen(name)<DALGLBCTXT_GLB_HDR_NAME_LEN)?strlen(name):(DALGLBCTXT_GLB_HDR_NAME_LEN-1))

static DALGLB_HEADER * g_shared_token_hdr = NULL;
static DALSYSEventHandle hTimeoutEvent    = NULL;
static DALSYSSyncHandle  hGlbCtxtSync     = NULL;
static int GlbCtxtIsInit   = 0;

static inline
uint32 _AllocGlbCtxtMem(void)
{
   // Add page size so as to always align it to page start
   return (uint32) smem_alloc(SMEM_DAL_AREA,(DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE));
}

static
void _LockYield(int times)
{
    DALSYSEventHandle hEvent[] = {0};
    uint32 dwIdx = 0;
   if(times)
   {
        hEvent[0] = hTimeoutEvent;
      DALSYS_EventMultipleWait(hEvent,1,times*1000,&dwIdx);
   }
}

static
void _PopulateLockHdr(DALGLB_HEADER * lock_hdr, uint32 size, const char *name )
{
   
   lock_hdr->size = size;
   DALSYS_memset(lock_hdr->name,0,DALGLBCTXT_GLB_HDR_NAME_LEN);
   strlcpy(lock_hdr->name,name,DALGLBCTXT_GLB_HDR_NAME_LEN);
   lock_hdr->lock.lock = 0;
   lock_hdr->lock.version = 0;
}

static
DALGLB_HEADER * _FindOrAllocHeader(const char *name, uint32 size_req)
{

   DALGLB_HEADER *glb_hdr = g_shared_token_hdr;
   uint32 size_avail = DALGLBCTXT_SMEM_AREA_SIZE;
   //Assume virt == physical
   if( NULL == glb_hdr)
      return NULL;

   //assume smem support
   while(glb_hdr->size && size_avail)
   {
       // Add page size so as to always align it to page start
     if(name && (0 == memcmp(glb_hdr->name,name,DALGLBCTXT_strlen(name))))
          return glb_hdr;

     size_avail -= glb_hdr->size;
     glb_hdr = (DALGLB_HEADER *)(((uint32) glb_hdr) + glb_hdr->size);
   }
   // return null if search for existing ctxt, or if nothing
   //   available for new ctxt
   return ((name || (size_avail < size_req))?NULL:glb_hdr);
   }

extern uint32 DALFW_LockedGetW(volatile uint32 *pTarget);
extern DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget, uint32 value);

static void InterProcSpinUnLock(void)
{
   // TODO: below is not a correct implementation of atomic unlock, 
   // but a temporary workaround for a HW issue
    g_shared_token_hdr->lock.lock = 0;
   }

static DALResult InterProcSpinLock(void)
{
   // TODO: below is not a correct implementation of atomic lock, 
   //For Bootloader env assume smem physical == virt and NOTE no API to mark attributes as strongly ordered
   int waitTime = 0;
   while (1)
   {
      if (g_shared_token_hdr->lock.lock == 0)
      {
         g_shared_token_hdr->lock.lock = DALGLBCTXT_SPINLOCK_PID;
         return DAL_SUCCESS;
      }
      
      if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
      {
         // check for time-out
         return DAL_ERROR;
      }

   //just return back virtual adddr
      waitTime += DALGLBCTXT_SLEEP_MS;
      _LockYield(DALGLBCTXT_SLEEP_MS);
  }
}


static DALResult GlbCtxtLock(void)
{
   DALResult ret;
    // initialize to the smem region start

   DALSYS_SyncEnter(hGlbCtxtSync);

    // Get to next free location
   ret = InterProcSpinLock();
   if (DAL_SUCCESS != ret)
    {
      DALSYS_SyncLeave(hGlbCtxtSync);
   }
   return ret;
}
        // are we searching for specific name
static void GlbCtxtUnLock(void)
{

   InterProcSpinUnLock();
    // return null if search for existing ctxt, or if nothing
    //   available for new ctxt
   DALSYS_SyncLeave(hGlbCtxtSync);
}

DALResult
DALGLBCTXT_Init()
{
    DALResult ret          = DAL_ERROR;
    uint32 DALSmemBaseAddr = 0;
    DALGLB_HEADER *glb_hdr = 0;

   
   DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,&hGlbCtxtSync,NULL);
        

   DALSYS_SyncEnter(hGlbCtxtSync);

   if (GlbCtxtIsInit)
    {
        //prevent other thread on the same core.
        ret = DAL_SUCCESS;
        goto exit_dalglbctxt_init;
    }

    DALSmemBaseAddr = _AllocGlbCtxtMem();
    glb_hdr = (DALGLB_HEADER *)DALSmemBaseAddr;

    // return DAL_ERROR if we cannot alloc
    if (0 == DALSmemBaseAddr)
        goto exit_dalglbctxt_init;

    //Create a timeout event to be used for wait
    if ( 0 == hTimeoutEvent)
        DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,&hTimeoutEvent,0);

    // Round the next avail page boundary, if not on page boundary
    //set the size of the un-aligned location to point to page boundary
    if (DALSmemBaseAddr&(DALGLBCTXT_PAGE_SIZE-1))
        glb_hdr->size = ((DALSmemBaseAddr+DALGLBCTXT_PAGE_SIZE) & (~(DALGLBCTXT_PAGE_SIZE-1))) - DALSmemBaseAddr;

    if(!g_shared_token_hdr)
    {
        g_shared_token_hdr = (DALGLB_HEADER *)((uint32)DALSmemBaseAddr+ glb_hdr->size);
    }

    if(DAL_SUCCESS == InterProcSpinLock())
    {
        if (0 != memcmp(g_shared_token_hdr->name,"dalspinlock",11))
        {
            // zero out memory
            DALSYS_memset((void *)g_shared_token_hdr,0,DALGLBCTXT_SMEM_AREA_SIZE);

            // populate and init the dal specific region
            _PopulateLockHdr(g_shared_token_hdr,sizeof(DALGLB_HEADER),"dalspinlock");

        }
        //release lock
        InterProcSpinUnLock();
        GlbCtxtIsInit = 1;
        ret = DAL_SUCCESS;
    }
    else
    {
        //error, so memory must be in non zero state....
        DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR, "Cannot lock in DALGLBCTXT_Init: lock:0x%x",
        g_shared_token_hdr->lock.lock);
    }

    exit_dalglbctxt_init:
    DALSYS_SyncLeave(hGlbCtxtSync);
    return ret;
}

DALResult
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 lock_type, void ** ctxt)
{
    // search for an existing header
    DALGLB_HEADER * _hdr = 0;
    DALResult ret        = DAL_ERROR;

   if (!GlbCtxtIsInit)
   {
      if (DAL_SUCCESS != DALGLBCTXT_Init())
      {
         // failed to initialize
         return DAL_ERROR;
      }
   }

    // round size_req to cache line size
    if (size_req&(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1))
    {
        size_req = (size_req+DALGLBCTXT_CACHE_LINE_SIZE_BYTES) & ~(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1);
    }

    //grab the global lock to prevent other cores from accessing.
   if(DAL_SUCCESS == GlbCtxtLock())
    {
        //check to see if this previously exists
        _hdr = _FindOrAllocHeader(name,0);

        //if none exists, then allocate mem area
        if (!_hdr)
        {
            _hdr = _FindOrAllocHeader(NULL,size_req);

            //cannot allocate return error
            if (NULL == _hdr)
                goto exit_dal_alloc_ctxt;

            // populate and init the dal specific region
         _PopulateLockHdr(_hdr,size_req,name);

        }

        if (ctxt)
            *ctxt = (void *)_hdr;

        ret = DAL_SUCCESS;

        exit_dal_alloc_ctxt:
        //release this lock for other cores
      GlbCtxtUnLock();
   }
   else
   {
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR, 
         "Cannot lock in DALGLBCTXT_AllocCtxt: lock:0x%x",
         g_shared_token_hdr->lock.lock);
    }

    return ret;
}

DALResult
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt)
{
    // search for an existing header
    DALGLB_HEADER * _hdr = 0;

   if (!GlbCtxtIsInit)
    {
      if (DAL_SUCCESS != DALGLBCTXT_Init())
{

        return DAL_ERROR;
    }
}


   if(DAL_SUCCESS == GlbCtxtLock())
{
      _hdr = _FindOrAllocHeader(name,0);

      if(ctxt)
         *ctxt = (void *)_hdr;


      GlbCtxtUnLock();
        }
        else
        {
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR, 
         "Cannot lock in DALGLBCTXT_FindCtxt: lock:0x%x",
         g_shared_token_hdr->lock.lock);
    }
   return (_hdr?DAL_SUCCESS:DAL_ERROR);
}
