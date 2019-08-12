/**
  @file mhi_osal.c
  @brief
  This file contains the implementation of the MHI OS abstraction layer  
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include <stdlib.h>
#include <string.h> 
#include <stdarg.h>
#include <stdio.h>
#include "mhi_i.h"
#include "busywait.h"
#include "mhi_osal.h"

#ifndef MHI_BOOT
#include "err.h"
#include <kernel.h>
#endif 

/* Interrupt controller */
#include "DDIInterruptController.h"

#ifndef MHI_BOOT
/* Ulog */ 
#include "ULogFront.h"

/*  ULog global handle */
#define	MHI_ULOG_BUFFER_MAX		4096*10
ULogHandle	mhi_ulog;
static void mhi_osal_uloginit(void);
#endif 

/* Ulog enabled flag */
uint32	mhi_osal_ulog_enable = 0;

/* OSAL Initialized flag */ 
static uint32 mhi_osal_init_done=0;

#ifndef NOINTERRUPT
/* Dal Device ID*/
DalDeviceHandle* hmhiOsalDALInterrupt = NULL;
#endif

#ifdef MHI_BOOT
typedef struct
{
   uint32 tail;
   uint32 head;
   byte raw_log[2000];
}mhi_log_t;

mhi_log_t* mhi_log;
#endif

/* ============================================================================
**  Function : mhi_osal_ulogInit
** ============================================================================
*/
/**
  Ulog message logging interface initialization.

  Ulog message logging interface.Allocates buffer for logging.On successful initialization it will return the
  Ulog handle.

  @return
*/

#ifndef MHI_BOOT
void mhi_osal_uloginit(void)
{
	ULogResult status = ULOG_ERR_INITINCOMPLETE;;

    /* Ulog Handle and buffer initialization. */
    if(!mhi_osal_ulog_enable)
    {
      status = ULogFront_RealTimeInit(&mhi_ulog, "mhi", MHI_ULOG_BUFFER_MAX, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
    }
	if(!status)
	{
	   mhi_osal_ulog_enable = 1;
	}
	return;
}
#endif
/* ============================================================================
**  Function : mhi_osal_ulogdeinit
** ============================================================================
*/
/**
  Ulog message logging interface De-initialization.

  @param   None.
  @return
  None.

*/
void mhi_osal_ulogdeinit(void)
{
    if(mhi_osal_ulog_enable)
	 {
		 mhi_osal_ulog_enable = 0;
	 }
	return;
}

/* ============================================================================
**  Function : mhi_osal_init
** ============================================================================
*/
/**
  Initializes OSAL Interfaces.

  Initializes OSAL Interfaces. Performs any initialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void mhi_osal_init(void)
{
    if(mhi_osal_init_done)
    {
        return;
    }
    DALSYS_InitMod(NULL);
#ifndef MHI_BOOT
    DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                     &hmhiOsalDALInterrupt);


	 mhi_osal_uloginit();
#else
    /* Initialize mhi boot logger */
    mhi_log=mhi_osal_malloc(sizeof(mhi_log_t));
#endif
    mhi_osal_init_done = 1;
}

/* ============================================================================
**  Function : mhi_osal_deinit
** ============================================================================
*/
/**


  De-initializes OSAL Interfaces. Performs any de-initialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void mhi_osal_deinit(void)
{

#ifndef MHI_BOOT
    mhi_osal_ulogdeinit();

    if (!mhi_osal_init_done)
    {
        return;
    }
    mhi_osal_init_done = 0;

    DAL_DeviceDetach(hmhiOsalDALInterrupt);
    hmhiOsalDALInterrupt=NULL;
#endif
    DALSYS_DeInitMod();
}

/* ============================================================================
**  Function : mhi_osal_syncinit
** ============================================================================
*/
/**
  Initializes a sync object.
    
  Initializes a sync object.
    
  @param[in,out] sync     Pointer to the object to be used for sync

  @return
  Success of the operation.
 
*/
mhi_status_t mhi_osal_syncinit(mhi_osal_sync_type *sync)
{
   if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
            (DALSYSSyncHandle*)sync,
            NULL) )
   {
      *(DALSYSSyncHandle*)sync = NULL;
      return MHI_ERROR;
   }
  
   return MHI_SUCCESS;
}
/* ============================================================================
**  Function : mhi_osal_syncdestroy
** ============================================================================
*/
/**
  Destroys a synchronization object.
    
  Destroys a synchronization object.
    
  @param[in,out]  sync    Sync object to be destroyed

  @return
  None.
  
*/
mhi_status_t mhi_osal_syncdestroy(mhi_osal_sync_type *sync)
{
   DALSYS_DestroyObject(*((DALSYSObjHandle*)sync));
   *(DALSYSSyncHandle*)sync = NULL;

   return MHI_SUCCESS; 
}

/* ============================================================================
**  Function : mhi_osal_syncenter
** ============================================================================
*/
/**
  Enters into a synchronized context.
    
  Enters into a synchronized context.
    
  @param[in,out]  sync    Sync object to be used

  @return
  None.
  
*/
void mhi_osal_syncenter(mhi_osal_sync_type *sync)
{
    DALSYS_SyncEnter(*((DALSYSSyncHandle*)sync));
}

/* ============================================================================
**  Function : mhi_osal_syncleave
** ============================================================================
*/
/**
  Leaves a synchronized context.
    
  Leaves a synchronized context.
    
  @param[in,out]  sync    Sync object to be used

  @return
  None.
  
*/
void mhi_osal_syncleave(mhi_osal_sync_type *sync)
{
   DALSYS_SyncLeave(*((DALSYSSyncHandle*)sync));
}


/* ============================================================================
**  Function : mhi_osal_malloc
** ============================================================================
*/
/**
  Mallocs a memory (virtual) using OS Malloc API.
    
  Mallocs a memory (virtual) using OS Malloc API. The memory allocated
  is for SW usage only.
    
  @param[in]  size    Size of the memory to be allocated

  @return
  Pointer to the memory allocated. NULL if failed.
  
  @sa
  mhi_osal_mallocmemregion, mhi_osal_free
*/
void* mhi_osal_malloc(uint32 size)
{
   void *ptr;
   DALResult res = DALSYS_Malloc(size, &ptr);
   
   if (res != DAL_SUCCESS)
   {
      return NULL;
   }
   return ptr;
}

/* ============================================================================
**  Function : mhi_osal_free
** ============================================================================
*/
/**
  Frees the memory allocated by mhi_osal_malloc.
    
  Frees the memory (virtual) using OS Free API.   
    
  @param[in,out]  mem    Pointer to the memory to be freed

  @return
  None.
  
  @sa
  mhi_osal_malloc.
*/
void mhi_osal_free(void *mem)
{
   DALSYS_Free(mem);
}

/* ============================================================================
**  Function : mhi_osal_memset
** ============================================================================
*/
/**
  Sets memory of specified size to specified value 
    
  Fills memory mem ,of size size, to value val. 
    
  @param[in] mem Pointer to the block of memory to fill
  @param[in] val Value of byte to fill the memory with  
  @param[in] size Number of bytes to fill  


  @return
  None.
  
*/
void mhi_osal_memset(void *mem, uint8 val, uint32 size)
{
   DALSYS_memset(mem, (int)val, size);
}

/* ============================================================================
**  Function : mhi_osal_memcpy
** ============================================================================
*/
/**
  Copies memory of a specofied size to the destination. 
    
  @param[in] dest Pointer to the destination memory address
  @param[in] source Pointer to the source memory address  
  @param[in] size Number of bytes to copy  


  @return
  None.
  
*/
void mhi_osal_memcpy(void *dest, void *source, uint32 size)
{
 DALSYS_memcpy(dest, source, size);
}

/** 
 * @brief      mhi_osal_eventcreate
 *
 *             Create an event 
 *              
 * 
 * @param[out]  *handle - Handle to event
 * @param[in]   *obj    - Event object
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if successful otherwise MHI_ERROR
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_osal_eventcreate(mhi_osal_event_handle *handle, mhi_osal_event *obj )
{
#ifndef MHI_BOOT
   DALSYSEventHandle hevent; 
   if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL,&hevent, NULL))
   {
      *handle=NULL; 
      return MHI_ERROR; 
   }

   *handle=hevent; 
#else
   /* Set dummy handle */
   *handle=(mhi_osal_event_handle*)0xDEADBEAD; 
#endif
   
   return MHI_SUCCESS; 
}

/** 
 * @brief      mhi_osal_eventdestroy
 *
 *             Destroy the event
 *              
 * 
 * @param[in]  *handle - handle to event
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS
 *
 * @sideeffect None 
 */
mhi_status_t mhi_osal_eventdestroy(mhi_osal_event_handle *handle )
{
#ifndef MHI_BOOT
   DALSYS_DestroyObject(*((DALSYSObjHandle*)handle));
#endif

   *handle=NULL;
   return MHI_SUCCESS; 
}

/** 
 * @brief      mhi_osal_eventwait
 *
 *             Wait for an event to be signalled
 *              
 * 
 * @param[in]  handle - handle to event
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_eventwait(mhi_osal_event_handle handle)
{
#ifndef MHI_BOOT
  DALSYS_EventWait(handle); 
#endif
}

/** 
 * @brief      mhi_osal_eventsignal
 *
 *             Signal event
 *              
 * 
 * @param[in]  handle
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_eventsignal(mhi_osal_event_handle handle)
{
#ifndef MHI_BOOT
   DALSYS_EventCtrl(handle, DALSYS_EVENT_CTRL_TRIGGER);
#endif
}

/** 
 * @brief      mhi_osal_thread_entry
 *
 *             Thread entry routine for worker threads
 *              
 * 
 * @param[in]  h  - Worker therad event handle
 * @param[in]  *arg - User arguments
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_thread_entry(DALSYSEventHandle h, void *arg)
{
  mhi_osal_thread_t *thread=(mhi_osal_thread_t*)arg; 

  /* Call the entry routine */
  thread->entry(thread->arg);
}

/** 
 * @brief      mhi_osal_spawn_thread
 *
 *             Spawn a worker thread 
 *              
 * 
 * @param[in]  *obj   - Thread object
 * @param[in]  *name  - Name of thread
 * @param[in]  *entry - Entry point function 
 * @param[in]  *ctx   - Context to pass to entry
 * @param[in]  priority - Priority of thread
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_osal_spawn_thread(mhi_osal_thread_t *obj, char *name, void *entry, void *ctx, uint32 priority)
{
#ifndef MHI_BOOT
   if(DAL_SUCCESS != DALSYS_RegisterWorkLoopEx(name, 0, priority, 10, &obj->handle, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_osal_spawn_thread: Failed to register workloop ");  
      return MHI_ERROR;
   }
 
   if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &obj->startEvent, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_osal_spawn_thread: Failed to create workloop event ");  
      return MHI_ERROR;
   }

   /* Set up thread object with entry and context */
   obj->arg=ctx; 
   obj->entry=(void(*)(void*))entry; 

   if(DAL_SUCCESS != DALSYS_AddEventToWorkLoop(obj->handle, (DALSYSWorkLoopExecute)mhi_osal_thread_entry, obj, obj->startEvent, NULL) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_osal_spawn_thread: Failed to associate event with workloop ");  
      return MHI_ERROR;
   }
 
   /* Kick off the workloop */
   if(DAL_SUCCESS != DALSYS_EventCtrl(obj->startEvent, DALSYS_EVENT_CTRL_TRIGGER) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_osal_spawn_thread: Failed to kick off workloop ");  
      return MHI_ERROR;
   }
#endif 
   return MHI_SUCCESS; 
}


mhi_osal_destroy_thread(mhi_osal_thread_t *obj)
{
   /* Not required */
}

/* ============================================================================
**  Function : mhi_osal_mallocmemregion
** ============================================================================
*/
/**
  Allocates a physically contiguous memory location using OS specific API.

  Allocates a physically contiguous memory location using OS specific API. The
  memory allocated is expected to be share-able with the hw.

  @param[out]  meminfo    Info structure for the memory allocated.
  @param[in]      size    Size of the memory to be allocated.

  @return
  None.

  @sa
  pcie_osal_freememregion.
*/
void mhi_osal_mallocmemregion(mhi_osal_meminfo *meminfo, uint32 size)
{
    #ifndef MHI_BOOT
    DALSYSMemInfo dalmeminfo;
    #endif

    if (meminfo == NULL)
    {
        return;
    }
    #ifndef MHI_BOOT
    if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
                                              DALSYS_MEM_PROPS_PHYS_CONT,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              meminfo->pa ? meminfo->pa : DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              size,
                                              &meminfo->handle,
                                              NULL) )
    {
        meminfo->handle = NULL;
        return;
    }

    DALSYS_MemInfo(meminfo->handle, &dalmeminfo);

    if (meminfo->pa != 0)
    {
      /* check that requested region lies within the allocated space */ 
        if (meminfo->pa < dalmeminfo.PhysicalAddr ||
            meminfo->pa + size > dalmeminfo.PhysicalAddr + dalmeminfo.dwLen)
        {
            DALSYS_DestroyObject(meminfo->handle);
            meminfo->handle = NULL;
            return;
        }

      /* allocated region may not start right at the requested physical address
       * so the virtual address has to be adjusted accordingly */
        meminfo->va = dalmeminfo.VirtualAddr + meminfo->pa - dalmeminfo.PhysicalAddr;
    }
    else
    {
        meminfo->va = dalmeminfo.VirtualAddr;
        meminfo->pa = dalmeminfo.PhysicalAddr;
    }

    meminfo->size = dalmeminfo.dwLen;
    
    #else

    meminfo->handle=(void*)0xFFFFFFFF;
    meminfo->va=meminfo->pa; 

    #endif

    return;
}

/* ============================================================================
**  Function : pcie_osal_freememregion
** ============================================================================
*/
/**
  Frees the memory allocated using pcie_osal_mallocmemregion.

  Frees the memory allocated using pcie_osal_mallocmemregion.

  @param[in]  meminfo    Info structure for the memory region to be freed.

  @return
  None.

  @sa
  pcie_osal_mallocmemregion.
*/
void mhi_osal_freememregion(mhi_osal_meminfo *meminfo)
{
    if (meminfo != NULL && meminfo->handle != NULL)
    {
    #ifndef MHI_BOOT
        DALSYS_DestroyObject(meminfo->handle);
    #else
        meminfo->handle=NULL; 
    #endif
    }

}

/* ============================================================================
**  Function : mhi_osal_isrinstall
** ============================================================================
*/
/**
  Registers an isr to an IRQ id.

  Registers with the interrupt controller, an ISR to service an IRQ.

  @param[in,out]  irqhandle    Handle to be used with the interrupt controller
  @param[in]      irq          IRQ ID to be registered for
  @param[in,out]  isr          ISR to be invoked to the service the irq
  @param[in,out]  data         data to be supplied to the isr upon invocation

  @return
  Success of the operation.

  @sa
  Other_Relevant_APIS_or_Delete.
*/
mhi_status_t mhi_osal_isrinstall(mhi_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data)
{
#ifndef MHI_BOOT
   /* Set this first as interrupt could already fire. Alternatively mask off all MHI interrupts first, register the interrupt, and then 
    * enable interrupts */
   irqhandle->dwIrqId = irq;
   if (DAL_SUCCESS != DalInterruptController_RegisterISR(hmhiOsalDALInterrupt,
            irq,
            (DALISR)isr,
            (DALISRCtx)data,
            DALINTRCTRL_ENABLE_DEFAULT_SETTINGS))
   {
      return MHI_ERROR;
   }
#endif
   return MHI_SUCCESS;
}

/** 
 * @brief      mhi_osal_isrmask
 *
 *             Mask Interrupt
 *              
 * 
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_isrmask(mhi_osal_irq_type *irqhandle)
{
   DalInterruptController_InterruptDisable(hmhiOsalDALInterrupt, irqhandle->dwIrqId);
}

/** 
 * @brief      mhi_osal_isrunmask
 *
 *             Unmask interrupt
 *              
 * 
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_isrunmask(mhi_osal_irq_type *irqhandle)
{
   DalInterruptController_InterruptEnable(hmhiOsalDALInterrupt, irqhandle->dwIrqId);
}

/* ============================================================================
**  Function : mhi_osal_isruninstall
** ============================================================================
*/
/**
  Deregisters the ISR from the interrupt controller.

  Deregisters the ISR from the interrupt controller.

  @param[in,out]  irqhandle    Interrupt controller handle to be used

  @return
  None.

*/
void mhi_osal_isruninstall(mhi_osal_irq_type *irqhandle)
{
#ifndef MHI_BOOT
   DalInterruptController_Unregister(hmhiOsalDALInterrupt,
                                      irqhandle->dwIrqId);
#endif
}

/* ============================================================================
**  Function : mhi_osal_israck
** ============================================================================
*/
/**
  Acknowledges the IRQ.

  Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
  to re-activate the IRQ.

  @param[in,out]  irqhandle    Interrupt handle

  @return
  None.

*/
void mhi_osal_israck(mhi_osal_irq_type *irqhandle)
{
#ifndef MHI_BOOT
   DalInterruptController_InterruptDone(hmhiOsalDALInterrupt,
                                         irqhandle->dwIrqId);
#endif
}

/** 
 * @brief      mhi_osal_wait
 *
 *             Waits for specified amount of milliseconds
 *              
 * 
 * @param[in]  time_in_milli - time in milliseconds to wait 
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_wait(uint32 time_in_milli)
{
   /* check for overflow */
   if(time_in_milli>(0XFFFFFFFF/1000))
   {
      return; 
   }

   busywait(time_in_milli*1000);
}

 /** 
 * @brief      mhi_osal_atomic_set_bit
 *
 *             Atomically sets a bit of a uint32 dword 
 *              
 * 
 * @param[in]  *addr  
 * @param[in]  bit
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_atomic_set_bit(uint32 *addr, uint32 bit )
{
#ifndef MHI_BOOT
   okl4_atomic_set_bit((okl4_atomic_word_t*)addr, bit);
#else
   *addr|=(1<<bit); 
#endif

}

/** 
 * @brief      mhi_osal_atomic_clear_bit
 *
 *             Atomically clears a bit of a uint32 dword 
 *              
 * 
 * @param[in]  *addr  
 * @param[in]  bit
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_atomic_clear_bit(uint32 *addr, uint32 bit)
{
#ifndef MHI_BOOT
   okl4_atomic_clear_bit((okl4_atomic_word_t*)addr, bit);
#else
   *addr&=~(1<<bit); 
#endif
}

/** 
 * @brief      mhi_osal_atomic_dec
 *
 *             Atomically decrement 
 *              
 * 
 * @param[in]  *addr - Address of uint32 value to decrement
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_atomic_dec(uint32 *addr)
{
#ifndef MHI_BOOT
   okl4_atomic_dec((okl4_atomic_word_t*)addr);
#else
   (*addr)--;
#endif
}

/** 
 * @brief      mhi_osal_atomic_inc
 *
 *             Atomically increment 
 *              
 * 
 * @param[in]  *addr - Address of uint32 value to increment
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_atomic_inc(uint32 *addr)
{
#ifndef MHI_BOOT
   okl4_atomic_inc((okl4_atomic_word_t*)addr);
#else
   (*addr)++;
#endif

}

/** 
 * @brief      mhi_osal_timer_init
 *
 *             Defines a simple clock callback style timer. More complexity can
 *             be added to the mhi timer apis in future, if required. 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define 
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_init(mhi_timer_t *timer)
{
#ifndef MHI_BOOT
  timer_def2(timer, NULL);
#endif
}

/** 
 * @brief      mhi_osal_timer_start
 *
 *             Starts a timer with the supplied parameters 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define 
 * @param[in]  mhi_timer_cb - callback at timer expiry
 * @param[in]  mhi_timer_user_data - User supplied data to be supplied to callback
 * @param[in]  mhi_timer_time_ms - Time (in ms) until first timer expiry
 * @param[in]  mhi_timer_reload_ms - periodicity of timer in ms
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_start(mhi_timer_t *timer, 
                          mhi_timer_cb cb, 
                          mhi_timer_user_data user_data,
                          mhi_timer_time_ms time_ms,
                          mhi_timer_reload_ms reload_ms)
{
#ifndef MHI_BOOT
  timer_reg(timer, cb, user_data, time_ms, reload_ms);
#endif
}

/** 
 * @brief      mhi_osal_timer_stop
 *
 *             Stops the timer associated with the supplied timer handle 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define  
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_stop(mhi_timer_t *timer)
{
#ifndef MHI_BOOT
  timer_clr(timer, T_NONE);
#endif
}

#ifdef MHI_DEBUG
#ifndef MHI_BOOT
void mhi_osal_debug(mhi_osal_msg_type msgLevel, const char *fmt, ...)
{
   va_list arglist;
   const char *p = fmt;
   uint32 datacount = 0;

   if(MHI_OSAL_MSG_LEVEL > msgLevel)
      return;

   va_start( arglist, fmt );


    /* ulog message data count */
	while(*p != '\0')
	{
		if((*p == '%') && (*(p +1) == '%'))
		{
			p += 2;
		}
		else
		{
			if(*p == '%')
			{
				datacount++;
			}
			p++;
		}
	}

  if(!mhi_osal_ulog_enable)
  {
   (void) vprintf( fmt, arglist);
  }
#ifndef MHI_BOOT
  else
  {
	  /* Ulog messages print */
     ULogFront_RealTimeVprintf(mhi_ulog, datacount, fmt, arglist );
  }
#endif

   /* For fetal call err fatal */
   if (msgLevel == MSG_FATAL)
   {
#ifndef MHI_BOOT
      ERR_FATAL("mhi fatal error, check mhi ulog",0,0,0 ); 
#endif
   }

   va_end( arglist );

}
#else

typedef __packed struct
{
   uint16   id; 
   byte   args; 
}mhi_log_element_header; 

void log_put(byte* addr, byte* data, uint32 size, byte* start, byte* limit)
{
   uint32 bytes=0; 

   bytes=(addr+size>limit?(uint32)(limit-addr):size);
   mhi_osal_memcpy((void*)addr, (void*)data, bytes); 
 
   /* Put remainder */
   if(bytes < size)
      mhi_osal_memcpy((void*)start, (void*)(data+bytes), size-bytes); 

}
#define ELEMENT_SIZE(n) (sizeof(uint16)+sizeof(byte)+(n)*sizeof(uint64))

#include "boot_error_if.h"
void mhi_osal_debug_c(mhi_osal_msg_type msgLevel, uint32 id, char* args, ...)
{
   va_list arglist;
   uint32 head, tail, i, size; 
   uint64 val; 
   mhi_log_element_header h;
   uint32 numArgs=strlen(args);
   
   va_start( arglist, args );

   if(MHI_OSAL_MSG_LEVEL > msgLevel)
      return;

   head=mhi_log->head;
   tail=mhi_log->tail;
   size=sizeof(mhi_log->raw_log);
   
   /* Advance head to next element if we are going to pass it. Assume one element 
    * will never be greater than size of the entire queue */  
   while(  ((tail + ELEMENT_SIZE(numArgs))%size) - head < tail - head )
   {
      head=(head+ELEMENT_SIZE(((mhi_log_element_header*)(mhi_log->raw_log+head))->args))%size;
   }

   /* Fill the header */
   h.id=id; 
   h.args=numArgs; 

   log_put(mhi_log->raw_log+tail, (byte*)&h, sizeof(mhi_log_element_header), mhi_log->raw_log, mhi_log->raw_log+size);
;

   /* Fill in the args */
   for(i = 0; i < numArgs; i++)
   {
      /* In order not to parse the actual format string, we are treating everything as 64. This is technically getting more than we need and overflowing
       * in to other data types, but that should be ok, when we process it later we will only look at the relevant size */
      if(args[i] == '4')
         val=va_arg(arglist, uint32);
      else if(args[i] == '8')
         val=va_arg(arglist, uint64);

      log_put(mhi_log->raw_log+(tail+sizeof(mhi_log_element_header)+i*sizeof(uint64))%size,(byte*)&val, sizeof(uint64), mhi_log->raw_log, mhi_log->raw_log+size);
   }

   mhi_log->tail=((tail + ELEMENT_SIZE(numArgs))%size);
   mhi_log->head=head;

   va_end( arglist );

   /* If error is fatal call error fatal handler */
   if (msgLevel == MSG_FATAL)
   {
      BL_ERR_FATAL(0);
   }
}


#endif /* MHI_DEBUG */



#endif 
