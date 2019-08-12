/**
  @file pcieosal.c
  @brief
  This file contains implementation of interfaces that provide a thin
  abstraction to various OS-specific APIs used by the pcie driver.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/pcie/pcieosal.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/21/14   Dilip        Removed DAL_DeviceAttach for PERST in SBL
05/28/14   Dilip        Added functions for Aligned Malloc and free
05/21/14   Dilip        Enabled BAMDMA
04/23/14   Dilip        Fix KW warnings
04/21/14   Dilip        Display the actual message during error fatal
04/04/14   Dilip        Added 32-bit memcpy function pcie_osal_memcpy_reg
03/17/14   Dilip        Introduced MSG_FATAL in pcie_osal_debug and replaced assert with pcie_osal_debug
02/21/14   Dilip        Added pcie_osal_memset function
08/05/13   Weijunz      Created

===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "com_dtypes.h"
#include "pcieosal.h"
#ifdef PCIE_OSAL_DEBUG
#include "stdio.h"
#endif
#ifndef  PCIE_BOOT
#include "ULogFront.h"
#include "err.h"
#include <kernel.h>
#endif


#define	PCIE_ULOG_BUFFER_MAX		4096
/*Interrupt and clock Handle*/
DalDeviceHandle* hpcieOsalDALInterrupt = NULL;
DalDeviceHandle* hpcieOsalDALClockPwr = NULL;

#ifdef PCIE_XML_PROPS 
DALSYS_PROPERTY_HANDLE_DECLARE(hProppcie);
#endif

uint32 pcie_osal_init_done = 0;
#ifndef PCIE_BOOT
/*  ULog global handle */
ULogHandle	pcieulog;
#endif
boolean	pcie_osal_ulog_enable  ;


pcie_gpio_handle_ctrl_type pcie_gpio_perst = 
{
   NULL,
   NULL,
   0,
};


#ifndef PCIE_BOOT
/****BAMDMA ******/
bamdma_chan_handle_t hBamDMA;
#endif
#ifdef PCIE_USE_BAMDMA_ASYNC_MODE  
bamdma_chan_handle_t hAsyncBamDMA;
bamdma_cb_t AsyncBamDMA_cb={0};
#endif

/* Generic callback for asynchronous read/write requests */
extern dma_req_cb async_rw_req_cb;
#ifndef PCIE_BOOT
static void pcie_osal_uloginit(void);
#endif

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_osal_ulogInit
** ============================================================================
*/
/**
  Ulog messsage logging interface initialization.

  Ulog message logging interface.Allocates buffer for logging.On successful intialization it will return the
  Ulog pcie handle.

  @return
  pcie status(Success or failure)
*/
void pcie_osal_uloginit(void)
{
   /* Ulog Handle and buffer intialization. */
    ULogFront_RealTimeInit(&pcieulog, "pcie", PCIE_ULOG_BUFFER_MAX, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
    pcie_osal_ulog_enable = TRUE ;
    

}

/* ============================================================================
**  Function : pcie_osal_ulogdeinit
** ============================================================================
*/
/**
  Ulog messsgae logging interface Deintialiization.

  @param   None.
  @return
  None.

*/
void pcie_osal_ulogdeinit(void)
{
    
   pcie_osal_ulog_enable = FALSE;
   return;
}

#endif

/* ============================================================================
**  Function : pcie_osal_init
** ============================================================================
*/
/**
  Initializes OSAL Interfaces.

  Initializes OSAL Interfaces. Performs any intiialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
extern int bamdma_init(void);
extern void bam_drv_init(void);

void pcie_osal_init(void)
{

   DALResult           dal_result; 

   #ifdef PCIE_XML_PROPS
    DALSYSPropertyVar pcieprop;
    DALResult status = DAL_SUCCESS;
   #endif
    if(pcie_osal_init_done)
    {
        return;
    }

    DALSYS_InitMod(NULL);
    DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                     &hpcieOsalDALInterrupt);
    DAL_DeviceAttach(DALDEVICEID_CLOCK, &hpcieOsalDALClockPwr);


   #ifdef PCIE_XML_PROPS
    status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/pcie", hProppcie);
    if(status != DAL_SUCCESS) 
        return;
    if((status = DALSYS_GetPropertyValue(hProppcie, "pcie_tgt_config", 0, &pcieprop)) != DAL_SUCCESS)
        return;

    #endif /* pcie_XML_PROPS */

#ifndef PCIE_BOOT
    pcie_osal_uloginit();
     
    dal_result = DAL_DeviceAttach(DALDEVICEID_TLMM,&pcie_gpio_perst.h_pcie_tlmm);

    if(dal_result != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_TLMM failed");
    }

    if(!pcie_gpio_perst.h_pcie_tlmm)
    {
        pcie_osal_debug(MSG_FATAL, "pcie_gpio_perst.h_pcie_tlmm is NULL");
        return;
    }


    dal_result = DAL_DeviceAttach(DALDEVICEID_GPIOINT,&pcie_gpio_perst.h_pcie_gpio_int) ;

    if(dal_result != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_GPIOINT failed");
    }

    if(!pcie_gpio_perst.h_pcie_gpio_int)
    {
        pcie_osal_debug(MSG_FATAL, "pcie_gpio_perst.h_pcie_gpio_int is NULL");
        return;
    }

    pcie_gpio_perst.pcie_gpio_pin = DAL_GPIO_CFG(
       PCIE_GPIO_PERST_PINOUT,
       0,
       DAL_GPIO_INPUT,
       DAL_GPIO_PULL_DOWN,
       DAL_GPIO_2MA); 

    dal_result = DalTlmm_ConfigGpio(
      pcie_gpio_perst.h_pcie_tlmm, 
      pcie_gpio_perst.pcie_gpio_pin,
      DAL_TLMM_GPIO_ENABLE);

    if(dal_result != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_ConfigGpio failed");
    }

    bam_drv_init();
    bamdma_init();
#endif

    (void)dal_result;
 	  	
    pcie_osal_init_done = 1;

}

/* ============================================================================
**  Function : pcie_osal_deinit
** ============================================================================
*/
/**
  DeInitializes OSAL Interfaces.

  Deinitializes OSAL Interfaces. Performs any deinitialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void pcie_osal_deinit(void)
{

  #ifndef PCIE_BOOT
    pcie_osal_ulogdeinit();
  #endif

    if (!pcie_osal_init_done)
    {
        return;
    }
    pcie_osal_init_done = 0;
    
    DAL_DeviceDetach(hpcieOsalDALInterrupt);
    hpcieOsalDALInterrupt=NULL;
    DAL_DeviceDetach(hpcieOsalDALClockPwr);
    hpcieOsalDALClockPwr = NULL;
    
    DAL_DeviceDetach(pcie_gpio_perst.h_pcie_tlmm);
    DAL_DeviceDetach(pcie_gpio_perst.h_pcie_gpio_int);
    pcie_gpio_perst.h_pcie_tlmm = NULL ;
    pcie_gpio_perst.h_pcie_gpio_int = NULL ;
    pcie_gpio_perst.pcie_gpio_pin = 0 ;
   
    DALSYS_DeInitMod();
}

/* ============================================================================
**  Function : pcie_osal_malloc
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
  pcie_osal_mallocmemregion, pcie_osal_free
*/
void * pcie_osal_malloc(uint32 size)
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
**  Function : pcie_osal_free
** ============================================================================
*/
/**
  Frees the memory allocated by pcie_osal_malloc.

  Frees the memory (virtual) using OS Free API.

  @param[in,out]  mem    Pointer to the memory to be freed

  @return
  None.

  @sa
  pcie_osal_malloc.
*/
void pcie_osal_free(void *pmem)
{

    DALSYS_Free(pmem);

}

/* ============================================================================
**  Function : pcie_osal_memset
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
void pcie_osal_memset(void *mem, uint8 val, uint32 size)
{
    DALSYS_memset(mem, (int)val, size);
}

/* ============================================================================
**  Function : pcie_osal_malloc_aligned
** ============================================================================
*/
/**
  Alloc an aligned chunk of memory 
    
  Alloc an aligned chunk of memory 
 
  @param[in]  size  - size to malloc in bytes
  @param[in]  align - amount to align by in bytes no bigger than 255. Must be power of 2

  @return
  None.
  
*/
void* pcie_osal_malloc_aligned(uint32 size, byte align)
{
   void *mem, *mem_aligned;
   byte offset;

   mem=pcie_osal_malloc(size+align);

   if(NULL == mem)
      return mem;

   mem_aligned=(void*)(((uint32)mem+align)&~(align-1));
   offset=(uint32)mem_aligned-(uint32)mem;

   /* Pad the unused region with alignment offset */
   pcie_osal_memset(mem, offset, offset);

   return mem_aligned; 
}

/* ============================================================================
**  Function : pcie_osal_free_aligned
** ============================================================================
*/
/**
  Free an aligned chunk of memory 
    
  Free an aligned chunk of memory
 
  @param[in]  void* - memory to free
 
 
  @return
  None.
  
*/
void pcie_osal_free_aligned(void* mem)
{
   uint8 pad, i; 

   if(NULL == mem)
      return; 

   pad=*(((uint8*)mem)-1);

   for(i=0; i < pad && *(((uint8*)mem)-i-1) == pad; i++);

   if(i!=pad)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_osal_free_aligned: pad corrupted, fatal error");
   }

   pcie_osal_free((uint8*)mem-i);

}
/* ============================================================================
**  Function : pcie_osal_memcpy_reg
** ============================================================================
*/
/**
  Copies uint32 data from source to destination.

  Copies uint32 data from source to destination.

  @param[out]  d      Pointer to the destination memory
  @param[in]   s      Pointer to the source memory
  @param[in]   size   number of bytes to be copied (usually a multiple of 4 since we have 32 bit registers)

  @return
  None.


*/
void pcie_osal_memcpy_reg(uint32 *d, uint32* s, uint32 size)
{
    uint32 *dst = d;
    const uint32 *src = s;
	uint32 m = (size%4)?((uint32)(size/4)+1):(size/4);
    while (m--) {
        *dst++ = *src++;
    }
}

#ifdef PCIE_USE_BAMDMA_ASYNC_MODE
/* ============================================================================
** Function: pcie_osal_async_memcpy
** ============================================================================
*/
/**
  Asynchronous memcpy
 
   
  @param[in]      dst       Pointer to desitnation buffer
  @param[in]      src       Pointer to source buffer
  @param[in]      size      Size of transfer
  @param[in]      user_data User data to callback with

  @return
  Error code.  
*/
uint32 pcie_osal_async_memcpy(void *dst, void* src, uint32 size, void* user_data)
{
   return bamdma_memcpy(hAsyncBamDMA,(uint8*)dst, 
                                     (uint8*)src, 
                                      size,BAMDMA_ASYNC,user_data,0);
}
  
/* ============================================================================
**  Function :pcie_async_memcpy_cb
** ============================================================================
*/
/**
  Callback supplied to async memcpy driver for completion async requests.
  
   
  @param[in]      result      Pointer to result of async memcpy

  @return
  None
  
  @dependencies
  None.
  
  @sa
  pcie_link_restart.
*/
void pcie_osal_async_memcpy_cb(bamdma_result_t *result)
{
  dma_read_write_req_type rw_req={0};

  if (NULL == async_rw_req_cb)
  {
    pcie_osal_debug(MSG_FATAL,"Generic async transfer callback not installed");
    return;
  }

  rw_req.user_data=result->user_data;

  if (BAMDMA_TRANSFER_SUCCESS != result->result)
     pcie_osal_debug(MSG_FATAL,"Async transaction failed with code %d",result->result);
  else
     rw_req.result=PCIE_STATUS_SUCCESS;
    
  /* Invoke the generic callback */
  async_rw_req_cb(rw_req);
}
#endif /* #ifdef PCIE_USE_BAMDMA_ASYNC_MODE */

/* ============================================================================
**  Function : pcie_osal_memcpy_enable
** ============================================================================
*/
/**
  Enables the memcpy engine.
    
  Enables the memcpy engine.
    
  @return
  None.
  
  @sa
  pcie_osal_memcpy_enable.
*/
void pcie_osal_memcpy_enable()
{
#ifndef  PCIE_BOOT
    bamdma_ret_t        dwReturn;

    dwReturn = bamdma_chan_open(6, &hBamDMA, NULL);
    if(BAMDMA_SUCCESS != dwReturn)
    {
        pcie_osal_debug(MSG_FATAL, "Unable to use bamdma :(");
    }
    dwReturn = bamdma_chan_set_mode(hBamDMA, BAMDMA_CHAN_MODE_POLLING);
    if(BAMDMA_SUCCESS != dwReturn)
    {
        pcie_osal_debug(MSG_FATAL, "Unable to set chmode :(");
    }

#ifdef PCIE_USE_BAMDMA_ASYNC_MODE
         
    AsyncBamDMA_cb.user_data=NULL;
    AsyncBamDMA_cb.cb=pcie_osal_async_memcpy_cb;  

    dwReturn = bamdma_chan_open(7, &hAsyncBamDMA, &AsyncBamDMA_cb);
    if(BAMDMA_SUCCESS != dwReturn)
    {
       pcie_osal_debug(MSG_FATAL, "Unable to use bamdma :(");
    }
    dwReturn = bamdma_chan_set_mode(hAsyncBamDMA, BAMDMA_CHAN_MODE_INTR);
    if(BAMDMA_SUCCESS != dwReturn)
    {
       pcie_osal_debug(MSG_FATAL, "Unable to set chmode :(");
    }
#endif /* #ifdef PCIE_USE_BAMDMA_ASYNC_MODE */     
#endif
}

/* ============================================================================
**  Function : pcie_osal_memcpy_disable
** ============================================================================
*/
/**
  Disables the memcpy engine.
    
  Disables the memcpy engine.
    
  @return
  None.
  
  @sa
  pcie_osal_memcpy_disable.
*/
void pcie_osal_memcpy_disable()
{
#ifndef  PCIE_BOOT
    if ( BAMDMA_SUCCESS != bamdma_chan_close(hBamDMA))
    {
        pcie_osal_debug(MSG_FATAL, "error closing BAMDMA channel");
    }

#ifdef PCIE_USE_BAMDMA_ASYNC_MODE    
    if ( BAMDMA_SUCCESS != bamdma_chan_close(hAsyncBamDMA))
    {
        pcie_osal_debug(MSG_FATAL, "error closing BAMDMA channel");
    }
#endif /* #ifdef PCIE_USE_BAMDMA_ASYNC_MODE */  
#endif

}


/* ============================================================================
**  Function : pcie_osal_memcpy
** ============================================================================
*/
/**
  Copies data in memory.

  Copies data in memory.

  @param[out]  dst    Pointer to the destination memory
  @param[in]   src    Pointer to the source memory
  @param[in]   size   size of data to be copied

  @return
  None.

  @sa
  pcie_osal_malloc.
*/
void pcie_osal_memcpy(void *dst, void* src, uint32 size)
{
#ifndef  PCIE_BOOT

   bamdma_ret_t      dwReturn;

   dwReturn = bamdma_memcpy(hBamDMA,(uint8*)dst, 
                                     (uint8*)src, 
                                     size,BAMDMA_SYNC,NULL,0);

   if(BAMDMA_TRANSFER_SUCCESS != dwReturn)
   {
      pcie_osal_debug(MSG_ERR, "pcie_osal_memcpy: BAM DMA transfer failed with code %d",dwReturn);
   }
#else
   DALSYS_memcpy(dst, src, size);
#endif
}

/* ============================================================================
**  Function : pcie_osal_mallocmemregion
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
void pcie_osal_mallocmemregion(pcie_osal_meminfo *meminfo, uint32 size)
{


    DALSYSMemInfo dalmeminfo;

    if (meminfo == NULL)
    {
        return;
    }

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
        /*check that requested region lies within the allocated space.*/
        if (meminfo->pa < dalmeminfo.PhysicalAddr ||
            meminfo->pa + size > dalmeminfo.PhysicalAddr + dalmeminfo.dwLen)
        {
            DALSYS_DestroyObject(meminfo->handle);
            meminfo->handle = NULL;
            return;
        }

        /*allocated region may not start right at the requested physical address
        so the virtual address has to be adjusted accordingly. */
        meminfo->va = dalmeminfo.VirtualAddr + meminfo->pa - dalmeminfo.PhysicalAddr;
    }
    else
    {
        meminfo->va = dalmeminfo.VirtualAddr;
        meminfo->pa = dalmeminfo.PhysicalAddr;
    }

    meminfo->size = dalmeminfo.dwLen;

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
void pcie_osal_freememregion(pcie_osal_meminfo *meminfo)
{

    if (meminfo != NULL && meminfo->handle != NULL)
    {
        DALSYS_DestroyObject(meminfo->handle);
    }

}

/* ============================================================================
**  Function : pcie_osal_cachecmd
** ============================================================================
*/
/**
  Performs a cache maintanence operation.

  Performs a cache maintanence operation.

  @param[in]  cache_op    Cache Operation to perform (clean/flush/invalidate)
  @param[in]  address     Memory for which cache maintanence to be performed.
  @param[in]  size        Size of the memory

  @return
  None.

*/
void pcie_osal_cachecmd(pcie_cache_op_type cache_op,uint32 address, uint32 size)
{

    uint32 cmd;

    switch (cache_op)
    {
    case PCIE_CACHE_INVALIDATE:
        cmd = DALSYS_CACHE_CMD_INVALIDATE;
        break;
    case PCIE_CACHE_FLUSH:
        cmd = DALSYS_CACHE_CMD_FLUSH;
        break;
    case PCIE_CACHE_CLEAN:
        cmd = DALSYS_CACHE_CMD_CLEAN;
        break;
    default:
        return;
    }

    DALSYS_CacheCommand(cmd, address, size);

}

/* ============================================================================
**  Function : pcie_osal_memorybarrier
** ============================================================================
*/
/**
  Performs a processor specific memory barrier operation.

  Performs a processor specific memory barrier operation.

  @param  None

  @return
  None.

*/
void pcie_osal_memorybarrier(void)
{

    DALFW_MemoryBarrier();

}


/* ============================================================================
**	Function : pcie_osal_atomic_inc
** ============================================================================
*/
/**
  Atomically increment .

  Atomically increment .

  @param[in]  *addr - Address of uint32 value to increment

  @return
  None.

*/
void pcie_osal_atomic_inc(uint32 *addr)
{
#ifndef PCIE_BOOT
   okl4_atomic_inc((okl4_atomic_word_t*)addr);
#endif
}



/* ============================================================================
**	Function : pcie_osal_atomic_dec
** ============================================================================
*/
/**
  Atomically decrement .

  Atomically decrement .

  @param[in]  *addr - Address of uint32 value to increment

  @return
  None.

*/
void pcie_osal_atomic_dec(uint32 *addr)
{
#ifndef PCIE_BOOT
   okl4_atomic_dec((okl4_atomic_word_t*)addr);
#endif
}

/* ============================================================================
**  Function : pcie_osal_syncinit
** ============================================================================
*/
/**
  Initializes a sync object.

  Initializes a sync object.

  @param[in,out] sync     Pointer to the object to be used for sync

  @return
  Success of the operation.

*/
pcie_osal_status_type pcie_osal_syncinit(pcie_osal_sync_type *sync)
{

  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                       (DALSYSSyncHandle*)sync,
                                       NULL) )
  {
      *(DALSYSSyncHandle*)sync = NULL;
      return PCIE_FAILED;
  }

  return PCIE_SUCCESS;
}
/* ============================================================================
**  Function : pcie_osal_syncdestroy
** ============================================================================
*/
/**
  Destroys a synchronization object.

  Destroys a synchronization object.

  @param[in,out]  sync    Sync object to be destroyed

  @return
  None.

*/
pcie_osal_status_type pcie_osal_syncdestroy(pcie_osal_sync_type *sync)
{

   DALSYS_DestroyObject((DALSYSObjHandle*)sync);
   *(DALSYSSyncHandle*)sync = NULL;

   return PCIE_SUCCESS;

}
/* ============================================================================
**  Function : pcie_osal_syncenter
** ============================================================================
*/
/**
  Enters into a synchronized context.

  Enters into a synchronized context.

  @param[in,out]  sync    Sync object to be used

  @return
  None.

*/
void pcie_osal_syncenter(pcie_osal_sync_type *sync)
{
    
    DALSYS_SyncEnter(*((DALSYSSyncHandle*)sync));
    
}

/* ============================================================================
**  Function : pcie_osal_syncleave
** ============================================================================
*/
/**
  Leaves a synchronized context.

  Leaves a synchronized context.

  @param[in,out]  sync    Sync object to be used

  @return
  None.

*/
void pcie_osal_syncleave(pcie_osal_sync_type *sync)
{
    
    DALSYS_SyncLeave(*((DALSYSSyncHandle*)sync));
    
}


/* ============================================================================
**  Function : pcie_osal_isrinstall
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
pcie_osal_status_type pcie_osal_isrinstall(pcie_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data,uint32 int_type)
{
    
    if (DAL_SUCCESS != DalInterruptController_RegisterISR(hpcieOsalDALInterrupt,
                                                          irq,
                                                          (DALISR)isr,
                                                          (DALISRCtx)data,
                                                          int_type))
    {
        return PCIE_FAILED;
    }

    irqhandle->dwIrqId = irq;
    
   return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_isruninstall
** ============================================================================
*/
/**
  Deregisters the ISR from the interrupt controller.

  Deregisters the ISR from the interrupt controller.

  @param[in,out]  irqhandle    Interrupt controller handle to be used

  @return
  None.

*/
void pcie_osal_isruninstall(pcie_osal_irq_type *irqhandle)
{
   
    DalInterruptController_Unregister(hpcieOsalDALInterrupt,
                                      irqhandle->dwIrqId);
    
}

/* ============================================================================
**  Function : pcie_osal_israck
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
void pcie_osal_israck(pcie_osal_irq_type *irqhandle)
{
    
    DalInterruptController_InterruptDone(hpcieOsalDALInterrupt,
                                         irqhandle->dwIrqId);
    
}
/* ============================================================================
**  Function : pcie_osal_isrinstall
** ============================================================================
*/
/**
  Registers an isr to an IRQ id.

  Registers with the interrupt controller, an ISR to service an IRQ.

  
  @param[in]      irq          IRQ ID to be registered for
  @param[in]  ctrl         Enable(True) or Diable(False) interrupt
  

  @return
  Success of the operation.

  @sa
  Other_Relevant_APIS_or_Delete.
*/
pcie_osal_status_type pcie_osal_ctrl_interrupt(uint32 irq,boolean ctrl)
{
   if (ctrl) 
   {
   
      if (DAL_SUCCESS != DalInterruptController_InterruptEnable(hpcieOsalDALInterrupt,irq) )
                                                          
      {
        return PCIE_FAILED;
      }

   }
   else
   {
      if (DAL_SUCCESS != DalInterruptController_InterruptDisable(hpcieOsalDALInterrupt,irq) )
                                                          
      {
        return PCIE_FAILED;
      }
   }
    
   return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_clock_ctl_init
** ============================================================================
*/
/**
  Initializes a clock control object.

  Initializes a clock control object.

  @param[in,out] clock_ctl     Pointer to the object to be used for clock control
  @param[in,out] clock_id      Pointer to the object to identify the clock

  @return
  Success of the operation.

*/
pcie_osal_status_type pcie_osal_clock_ctl_init(pcie_osal_clock_ctl_type *clock_ctl, char* clock_name)
{
   DALResult           dal_result;

  if (!clock_ctl)
  {
    return PCIE_FAILED;
  }
  
  dal_result = DalClock_GetClockId(hpcieOsalDALClockPwr, (char*)clock_name, &clock_ctl->clock_id) ;

  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_GetClockId failed");
  }
	


  return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_clock_ctl_deinit
** ============================================================================
*/
/**
  Deinitializes a clock control object.

  Deinitializes a clock control object.

  @param[in,out] clock_ctl     Pointer to the object to be used for clock control

  @return
  Success of the operation.

*/
pcie_osal_status_type pcie_osal_clock_ctl_deinit(pcie_osal_clock_ctl_type *clock_ctl)
{
  return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_clock_ctl_enable
** ============================================================================
*/
/**
  Turn on clock.

  Turn on clock.

  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.

*/
pcie_osal_status_type pcie_osal_clock_ctl_enable(pcie_osal_clock_ctl_type *clock_ctl)
{
  DALResult           dal_result;

  if (!clock_ctl)
  {
    return PCIE_FAILED;
  }

  dal_result = DalClock_EnableClock(hpcieOsalDALClockPwr, clock_ctl->clock_id) ;
  
  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_EnableClock failed");
  }

  return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_clock_ctl_disable
** ============================================================================
*/
/**
  Turn off clock.

  Turn off clock.

  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.

*/
pcie_osal_status_type pcie_osal_clock_ctl_disable(pcie_osal_clock_ctl_type *clock_ctl)
{
  DALResult           dal_result;

  if (!clock_ctl)
  {
    return PCIE_FAILED;
  }
  
  dal_result = DalClock_DisableClock(hpcieOsalDALClockPwr, clock_ctl->clock_id) ;

  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_DisableClock failed");
  }

  return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_osal_is_clock_ctl_on
** ============================================================================
*/
/**
  Checks if the clock is on.

  @param[in,out]  clock_ctl    clock control object to be used

  @return
  True if the clock is on else returns false.

*/
boolean pcie_osal_is_clock_ctl_on(pcie_osal_clock_ctl_type *clock_ctl)
{
  boolean           dal_result;

  dal_result = DalClock_IsClockOn(hpcieOsalDALClockPwr, clock_ctl->clock_id) ;

  return dal_result;
}

/* ============================================================================
**  Function : pcie_osal_set_clock_freq
** ============================================================================
*/
/**
  Set Clock Frequence.
    
  
  @param[in] clock_ctl     Pointer to the object to be used for clock control
  @param nFreq   [in] Clock frequency in Hz, kHz, or MHz, depending on the
                      eMatch parameter.
  @param eMatch  [in] Type of match request.
  
 

  @return
  Success of the operation.
 
*/
pcie_osal_status_type pcie_osal_set_clock_freq(pcie_osal_clock_ctl_type *clock_ctl,uint32  nFreq,  ClockFrequencyType  eMatch)
{
   uint32 result_freq ;
   DALResult           dal_result;

  if (!clock_ctl)
  {
    return PCIE_FAILED;
  }
  
  dal_result = DalClock_SetClockFrequency(hpcieOsalDALClockPwr, clock_ctl->clock_id,nFreq,eMatch,&result_freq) ;  
  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_SetClockFrequency failed");
  }
  return PCIE_SUCCESS;


}

/* ============================================================================
**  Function : pcie_osal_select_extern_source
** ============================================================================
*/
/**
  Selects an external source for a clock.
    
  
  @param[in] clock_ctl     Pointer to the object to be used for clock control
  @param nFreqHz   [in] Source frequency in Hz. The value '0' indicates that
                        voltage scaling is not expected for this source.
  @param nSource   [in] Source to use. This value should match the MUX input
                        on which the external source is connected.
                        @note1 If the specified clock does not support the 
                        requested external source, this request selects the first 
                        source (in most cases XO) available to the clock.
  @param nDiv2x    [in] Integer divider to use (2x the desired divider).
  @param nM        [in] The M value for any M/N counter (0 to bypass). 
                        Not all clock domains have M/N counters; these 
                        values will be ignored in such cases.
  @param nN        [in] The N value for any M/N counter.
  @param n2D       [in] Twice the D value for any M/N counter. The raw value 
                        is doubled to allow for half-integer duty cycles.

  
 

  @return
  Success of the operation.
 
*/
pcie_osal_status_type pcie_osal_select_extern_source(pcie_osal_clock_ctl_type *clock_ctl,uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D)
{
   
   DALResult           dal_result;

  if (!clock_ctl)
  {
    return PCIE_FAILED;
  }

  dal_result = DalClock_SelectExternalSource(hpcieOsalDALClockPwr, clock_ctl->clock_id,nFreqHz,nSource, nDiv2x, nM, nN, n2D);
  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_SelectExternalSource failed");
  }
  return PCIE_SUCCESS;

}



/* ============================================================================
**  Function : pcie_osal_pwr_domain_init
** ============================================================================
*/
/**
  Initializes a power domain control object.
    
  
  @param[in,out] pwr_ctl     Pointer to the object to be used for clock control
  @param[in,out] pwr_name      Pointer to the object to identify the clock

  @return
  Success of the operation.
 
*/
pcie_osal_status_type pcie_osal_pwr_domain_init(pcie_osal_pwr_ctl_type *pwr_ctl, char* pwr_name)
{
   DALResult           dal_result;

  if (!pwr_ctl)
  {
    return PCIE_FAILED;
  }
  
  dal_result = DalClock_GetPowerDomainId(hpcieOsalDALClockPwr, (char*)pwr_name, &pwr_ctl->pwr_id) ;
  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_GetPowerDomainId failed");
  }

  return PCIE_SUCCESS;

}

/* ============================================================================
**  Function : pcie_osal_pwr_domain_deinit
** ============================================================================
*/
/**
  Deinitializes a Power control object.
    
  
  @param[in,out] pwr_ctl     Pointer to the object to be used for clock control

  @return
  Success of the operation.
 
*/
pcie_osal_status_type pcie_osal_pwr_domain_deinit(pcie_osal_pwr_ctl_type *pwr_ctl)
{
   return PCIE_SUCCESS;

}

/* ============================================================================
**  Function : pcie_osal_pwr_domain_enable
** ============================================================================
*/
/**
   Enable the power domain.
    
  
  @param[in,out]  pwr_ctl    clock control object to be used

  @return
  Success of the operation.
  
*/
pcie_osal_status_type pcie_osal_pwr_domain_enable(pcie_osal_pwr_ctl_type *pwr_ctl)
{
   DALResult           dal_result;

  if (!pwr_ctl)
  {
    return PCIE_FAILED;
  }

  dal_result = DalClock_EnablePowerDomain(hpcieOsalDALClockPwr, pwr_ctl->pwr_id) ;
  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_EnablePowerDomain failed");
  }

  return PCIE_SUCCESS;

}

/* ============================================================================
**  Function : pcie_osal_pwr_domain_disable
** ============================================================================
*/
/**
  Disable power domain.
  
  @param[in,out]  pwr_ctl    clock control object to be used

  @return
  Success of the operation.
  
*/
pcie_osal_status_type pcie_osal_pwr_domain_disable(pcie_osal_pwr_ctl_type *pwr_ctl)
{
   DALResult           dal_result;

  if (!pwr_ctl)
  {
    return PCIE_FAILED;
  }
  
  dal_result = DalClock_DisablePowerDomain(hpcieOsalDALClockPwr, pwr_ctl->pwr_id) ;

  if(dal_result != DAL_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "DalClock_DisablePowerDomain failed");
  }
  return PCIE_SUCCESS;

}




/* ============================================================================
**  Function : pcie_osal_debug_i
** ============================================================================
*/
/**
  Debug message logging interface.

  Debug message logging interface. Depending upon the OSAL implementation, it
  could be printed to a term window or sent as a diag msg or saved to a file

  @param[in]  fmt    Format similar to printf
  @param[in]  ...    Variable list of arguments to be processed

  @return
  None.

*/

void pcie_osal_debug_i(const char *file,uint16 line, pcie_osal_msg_type msgLevel, uint32 nArgs, const char *fmt, ...)
{
#ifndef PCIE_BOOT
   va_list arglist;

   if(pcie_osal_ulog_enable != TRUE)
      return;

   if(PCIE_OSAL_MSG_LEVEL > msgLevel)
      return;

   va_start( arglist, fmt );

   /* Ulog messages print */
   ULogFront_RealTimeVprintf(pcieulog, nArgs, fmt, arglist );

   /* For fatal call err fatal */
   if (msgLevel == MSG_FATAL)
   {
      msg_const_type msg={{NULL, MSG_SSID_DFLT, MSG_LEGACY_FATAL }, NULL, NULL  };
      msg.desc.line=line;
      msg.fname=file; 
      msg.fmt=fmt;
      err_SaveFatal0(&msg); 
   }

   va_end( arglist );
#endif
}

