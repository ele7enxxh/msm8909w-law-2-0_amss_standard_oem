/*==============================================================================
@file  DALGlbCtxt.c

DALSYS Global Context implementation

        Copyright © 2015 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

        
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/30/15   sb      Initial version.
==============================================================================*/

#include "stringl/stringl.h"

#include "dalconfig.h"
#include "DALSysInt.h"
#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include "DALSysCmn.h"

#include "msmhwiobase.h"
#include "HALhwio.h"
#include "DALHwio.h"
#include "smem.h"

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
static uint32 HWMutexId    = 0;
static int GlbCtxtIsInit   = 0;
static DALResult (*InterProcSpinLock)(void);
static void (*InterProcSpinUnLock)(void);

static
uint32 _AllocGlbCtxtMem(void)
{
   // Add page size so as to always align it to page start
   return (uint32) smem_alloc(SMEM_DAL_AREA,(DALGLBCTXT_SMEM_AREA_SIZE+DALGLBCTXT_PAGE_SIZE));
}

static
void _LockYield(int times)
{
   DALSYSEventHandle hEvent[1];
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
   //populate the global header
   lock_hdr->size = size;
   memset(lock_hdr->name,0,DALGLBCTXT_GLB_HDR_NAME_LEN);
   strlcpy(lock_hdr->name,name,DALGLBCTXT_GLB_HDR_NAME_LEN);
   lock_hdr->lock.lock = 0;
   lock_hdr->lock.version = 0;
}

static
DALGLB_HEADER * _FindOrAllocHeader(const char *name, uint32 size_req)
{
   // initialize to the smem region start
   DALGLB_HEADER *glb_hdr = g_shared_token_hdr;
   uint32 size_avail = DALGLBCTXT_SMEM_AREA_SIZE;

   if( NULL == glb_hdr)
      return NULL;

   // Get to next free location
   while(glb_hdr->size && size_avail)
   {
     // are we searching for specific name
     if(name && (0 == memcmp(glb_hdr->name,name,DALGLBCTXT_strlen(name))))
          return glb_hdr;

     size_avail -= glb_hdr->size;
     glb_hdr = (DALGLB_HEADER *)(((uint32) glb_hdr) + glb_hdr->size);
   }
   // return null if search for existing ctxt, or if nothing
   //   available for new ctxt
   return ((name || (size_avail < size_req))?NULL:glb_hdr);
}

static void InterProcHWSpinUnLock(void)
{
   /* Write '0' to the hardware mutex to unlock */
   HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, 0 );
}

static DALResult InterProcHWSpinLock(void)
{
   int waitTime = 0;

   /* Attempt to write PID */
   HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );

   while (HWIO_INI( TCSR_MUTEX_REG_n, HWMutexId) != 
          DALGLBCTXT_SPINLOCK_PID )
   {
      // unable to acquire spinlock
      if (waitTime > DALGLBCTXT_MAX_SLEEP_MS)
      {
         // check for time-out
         return DAL_ERROR;
      }
      waitTime += DALGLBCTXT_SLEEP_MS;
      _LockYield(DALGLBCTXT_SLEEP_MS);

      /* Attempt to write PID again */
      HWIO_OUTI( TCSR_MUTEX_REG_n, HWMutexId, DALGLBCTXT_SPINLOCK_PID );
   }
   return DAL_SUCCESS;
}

static void InterProcSpinInit(void)
{
   DALResult ret;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar glbCtxtProp;

   // get the HW mutex index
   ret = DALSYS_GetDALPropertyHandle(0,hProp);
   if (DAL_SUCCESS != ret)
   {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, "Failed to get prop handle");
      return;
   }

   if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp,
            "GlbCtxtHWMutexNumber",0,&glbCtxtProp))
   {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR, "Failed to get GlbCtxtHWMutexNumber");
      return;
   }

   HWMutexId = glbCtxtProp.Val.dwVal;
   InterProcSpinLock = InterProcHWSpinLock;
   InterProcSpinUnLock = InterProcHWSpinUnLock;

}

static DALResult GlbCtxtLock(void)
{
   DALResult ret;

   // lock SW mutex
   DALSYS_SyncEnter(hGlbCtxtSync);

   // lock inter-processor spinlock
   ret = InterProcSpinLock();
   if (DAL_SUCCESS != ret)
   {
      DALSYS_SyncLeave(hGlbCtxtSync);
   }
   return ret;
}

static void GlbCtxtUnLock(void)
{
   // unlock inter-processor spinlock
   InterProcSpinUnLock();
  
   // unlock SW mutex
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
      // no re-initialization if last run was successful
      ret = DAL_SUCCESS;
      goto exit_dalglbctxt_init;
   }

   DALSmemBaseAddr = _AllocGlbCtxtMem();
   glb_hdr = (DALGLB_HEADER *)DALSmemBaseAddr;
   
   // return DAL_ERROR if we cannot alloc
   if(0 == DALSmemBaseAddr)
      goto exit_dalglbctxt_init;

   //Create a timeout event to be used for wait
   if( 0 == hTimeoutEvent)
      DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,&hTimeoutEvent,0);

   // Round the next avail page boundary, if not on page boundary
   //set the size of the un-aligned location to point to page boundary
   if(DALSmemBaseAddr&(DALGLBCTXT_PAGE_SIZE-1))
      glb_hdr->size = ((DALSmemBaseAddr+DALGLBCTXT_PAGE_SIZE) & 
            (~(DALGLBCTXT_PAGE_SIZE-1))) - DALSmemBaseAddr;

   if(!g_shared_token_hdr)
   {
      g_shared_token_hdr = (DALGLB_HEADER *)((uint32)DALSmemBaseAddr+ glb_hdr->size);
   }

   // initialize interprocessor spin-lock
   InterProcSpinInit();

   //check to see if the dal global spin lock should be created or not
   if(DAL_SUCCESS == InterProcSpinLock())
   {
      if(0 != memcmp(g_shared_token_hdr->name,"dalspinlock",11))
      {
         // zero out memory
         memset((void *)g_shared_token_hdr,0,DALGLBCTXT_SMEM_AREA_SIZE);

         // populate and init the dal specific region
         _PopulateLockHdr(g_shared_token_hdr,sizeof(DALGLB_HEADER),"dalspinlock");

      }
      //release spin lock
      InterProcSpinUnLock();
      GlbCtxtIsInit = 1;
      ret = DAL_SUCCESS;
   }
   else
   {
      //error, so memory must be in non zero state....
       DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR, "failed- lock:0x%x", 
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
   if(size_req&(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1))
   {
      size_req = (size_req+DALGLBCTXT_CACHE_LINE_SIZE_BYTES) & ~(DALGLBCTXT_CACHE_LINE_SIZE_BYTES-1);
   }

   //grab the global lock to prevent other cores from accessing.
   if(DAL_SUCCESS == GlbCtxtLock())
   {
      //check to see if this previously exists
      _hdr = _FindOrAllocHeader(name,0);

      //if none exists, then allocate mem area
      if(!_hdr)
      {
         _hdr = _FindOrAllocHeader(NULL,size_req);

         //cannot allocate return error
         if(NULL == _hdr)
            goto exit_dal_alloc_ctxt;

         // populate and init the dal specific region
         _PopulateLockHdr(_hdr,size_req,name);

      }

      if(ctxt)
         *ctxt = (void *)_hdr;

      ret = DAL_SUCCESS;

exit_dal_alloc_ctxt:
      //release this lock for other cores
      GlbCtxtUnLock();
   }
   else
   {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR, "failed- lock:0x%x", 
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
         // failed to initialize
         return DAL_ERROR;
      }
   }

   //grab the global lock to prevent other cores from accessing.
   if(DAL_SUCCESS == GlbCtxtLock())
   {
      _hdr = _FindOrAllocHeader(name,0);

      if(ctxt)
         *ctxt = (void *)_hdr;

      //release this lock for other cores
      GlbCtxtUnLock();
   }
   else
   {
      DALSYS_LogEvent(0, DALSYS_LOGEVENT_ERROR, "failed- lock:0x%x", 
                     g_shared_token_hdr->lock.lock);
   }
   return (_hdr?DAL_SUCCESS:DAL_ERROR);
}
