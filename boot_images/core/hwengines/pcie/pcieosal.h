#ifndef _PCIE_OSAL_H_
#define _PCIE_OSAL_H_

/**
  @file pcieosal.h
  @brief
  This file contains implementation of interfaces that provide a thin 
  abstraction to various OS-specific APIs used by the pcie driver.
  
*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/pcie/pcieosal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/28/14   Dilip       Added functions for Aligned Malloc and free
05/21/14   Dilip       Enabled BAMDMA
04/21/14   Dilip       Display the actual message during error fatal
04/04/14   Dilip       Added 32-bit memcpy function pcie_osal_memcpy_reg
03/17/14   Dilip       Introduced MSG_FATAL in pcie_osal_debug
02/21/14   Dilip       Added pcie_osal_memset function
08/05/13   Weijunz     Created

===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/



#include "com_dtypes.h"
#include "pcie.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIClock.h"
#include "DDITlmm.h"
#ifndef  PCIE_BOOT
#include "DDIGPIOInt.h"
#include "bamdma.h"
#endif
#include "DalDevice.h"


#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6, _7, _8, _9, _10, N,...) N

#define pcie_osal_debug(LEVEL, FMT, ...) pcie_osal_debug_i(__FILE__, __LINE__, LEVEL, (VA_NUM_ARGS(__VA_ARGS__)+1), "(%s): " FMT,__func__,##__VA_ARGS__)

/** @brief
    Information on a piece of memory allocated
  */

typedef struct _pcie_meminfo{
    uint32 pa;     /**< -- Physical address of the memory */
    uint32 va;     /**< -- Virtual address of the memory */
    uint32 size;   /**< -- Size of the memory */
    void *handle;  /**< -- Target specific handle to the memory (reserved) */
}pcie_osal_meminfo;

/** Various status codes returned by PCIe APIs */
typedef enum
{
    PCIE_SUCCESS          = 0x0, /**< Operation was a success */
    PCIE_FAILED           = 0x1, /**< Operation failed        */
    PCIE_INVALID_CONFIG   = 0x2, /**< Invalid Config provided */
    PCIE_INVALID_BUFSIZE  = 0X3  /**< Invalid buffer size for dump */
}pcie_osal_status_type;


/** @name Cache flags
 *  @brief Flags used for various cache operations
 */

typedef enum
{
    PCIE_CACHE_INVALIDATE=0x0, /**< -- Invalidates a cache */
    PCIE_CACHE_FLUSH=0x1,      /**< -- Flush a cache */
    PCIE_CACHE_CLEAN=0x2       /**< -- Clean a cache */  
}pcie_cache_op_type;

/* @name Sync object
  */
typedef struct {
   uint32 none;
}pcie_osal_sync_type;

/** @name Clock control object
  */
typedef struct {
   uint32 clock_id;
}pcie_osal_clock_ctl_type;

/** @name power control object
  */
typedef struct{
   uint32 pwr_id ;
} pcie_osal_pwr_ctl_type ;

/** @name IrqHandle Object
  */
typedef struct { 
   /**< -- IRQ number to be registered to */  
   uint32  dwIrqId;        
}pcie_osal_irq_type;

/** @name Message Type */ 
typedef enum {
   MSG_LOW=1,
   MSG_MEDIUM,
   MSG_HIGH, 
   MSG_ERR,
   MSG_FATAL
}pcie_osal_msg_type; 

/** @name GPIO control Handle Object
  */
typedef struct
{
  DalDeviceHandle    *h_pcie_tlmm;
  DalDeviceHandle    *h_pcie_gpio_int;
  DALGpioSignalType  pcie_gpio_pin ;
} pcie_gpio_handle_ctrl_type;

extern pcie_gpio_handle_ctrl_type pcie_gpio_perst ;
#define PCIE_GPIO_PERST_PINOUT  65
#define PCIE_GPIO_WAKE_PINOUT  61
#define PCIE_GPIO_CLKREQ_PINOUT 64

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
void pcie_osal_init(void);

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
void pcie_osal_deinit(void);


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
void * pcie_osal_malloc(uint32 size);

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
void pcie_osal_free(void *mem);

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
void pcie_osal_memset(void *mem, uint8 val, uint32 size);

/* ============================================================================
**  Function : pcie_osal_malloc_aligned
** ============================================================================
*/
/**
  Alloc an aligned chunk of memory 
    
  Alloc an aligned chunk of memory 
 
  @param[in]  size - size to malloc in bytes
  @param[in]  align - amount to align by in bytes no bigger than 255. Must be a power of 2

  @return
  None.
  
*/

void* pcie_osal_malloc_aligned(uint32 size, byte align);

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

void pcie_osal_free_aligned(void* mem);

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
void pcie_osal_memcpy_reg(uint32 *d, uint32* s, uint32 size);

/* ============================================================================
**  Function : pcie_osal_enable
** ============================================================================
*/
/**
  Enables the memcpy engine.
    
  Enables the memcpy engine.
    
  @return
  None.
  
  @sa
  pcie_osal_memcpy.
*/
void pcie_osal_memcpy_enable(void);

/* ============================================================================
**  Function : pcie_osal_disable
** ============================================================================
*/
/**
  Disables the memcpy engine.
    
  Disables the memcpy engine.
    
  @return
  None.
  
  @sa
  pcie_osal_memcpy.
*/
void pcie_osal_memcpy_disable(void);


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
void pcie_osal_memcpy(void *dst, void* src, uint32 size);

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
uint32 pcie_osal_async_memcpy(void *dst, void* src, uint32 size, void* user_data);
#endif /* #ifdef PCIE_USE_BAMDMA_ASYNC_MODE */

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
void pcie_osal_mallocmemregion(pcie_osal_meminfo *meminfo, uint32 size);

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
void pcie_osal_freememregion(pcie_osal_meminfo *meminfo);

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
void pcie_osal_cachecmd(pcie_cache_op_type cache_op,uint32 address, uint32 size);


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
void pcie_osal_memorybarrier(void);


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
void pcie_osal_atomic_inc(uint32 *addr);


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
void pcie_osal_atomic_dec(uint32 *addr);

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
pcie_osal_status_type pcie_osal_syncinit(pcie_osal_sync_type *sync);


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
pcie_osal_status_type pcie_osal_syncdestroy(pcie_osal_sync_type *sync);

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
void pcie_osal_syncenter(pcie_osal_sync_type *sync);

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
void pcie_osal_syncleave(pcie_osal_sync_type *sync);

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
pcie_osal_status_type pcie_osal_isrinstall(pcie_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data,uint32 int_type);

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
void pcie_osal_isruninstall(pcie_osal_irq_type *irqhandle);

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
void pcie_osal_israck(pcie_osal_irq_type *irqhandle);

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
pcie_osal_status_type pcie_osal_ctrl_interrupt(uint32 irq,boolean ctrl)  ;
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
pcie_osal_status_type pcie_osal_clock_ctl_init(pcie_osal_clock_ctl_type *clock_ctl, char* clock_name);

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
pcie_osal_status_type pcie_osal_clock_ctl_deinit(pcie_osal_clock_ctl_type *clock_ctl);

/* ============================================================================
**  Function : pcie_osal_clock_ctl_enable
** ============================================================================
*/
/**
  Turn on clock.
    
  
  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.
  
*/
pcie_osal_status_type pcie_osal_clock_ctl_enable(pcie_osal_clock_ctl_type *clock_ctl);

/* ============================================================================
**  Function : pcie_osal_clock_ctl_disable
** ============================================================================
*/
/**
  Turn off clock.
    
  
  @param[in,out]  clock_ctl    clock control object to be used

  @return
  Success of the operation.
  
*/
pcie_osal_status_type pcie_osal_clock_ctl_disable(pcie_osal_clock_ctl_type *clock_ctl);

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
boolean pcie_osal_is_clock_ctl_on(pcie_osal_clock_ctl_type *clock_ctl);

/* ============================================================================
**  Function : pcie_osal_set_clock_freq
** ============================================================================
*/
/**
  Sets the frequency of a clock.
    
 
  @param[in] clock_ctl     Pointer to the object to be used for clock control
  @param nFreq   [in] Clock frequency in Hz, kHz, or MHz, depending on the
                      eMatch parameter.
  @param eMatch  [in] Type of match request.
  
 

  @return
  Success of the operation.
 
*/
pcie_osal_status_type pcie_osal_set_clock_freq(pcie_osal_clock_ctl_type *clock_ctl,uint32  nFreq,  ClockFrequencyType  eMatch);

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
pcie_osal_status_type pcie_osal_select_extern_source(pcie_osal_clock_ctl_type *clock_ctl,uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D);


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

void pcie_osal_debug_i(const char* file, uint16 line, pcie_osal_msg_type msgLevel, uint32 nArgs, const char *fmt, ...);

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
pcie_osal_status_type pcie_osal_pwr_domain_init(pcie_osal_pwr_ctl_type *pwr_ctl, char* pwr_name);

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
pcie_osal_status_type pcie_osal_pwr_domain_deinit(pcie_osal_pwr_ctl_type *pwr_ctl);

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
pcie_osal_status_type pcie_osal_pwr_domain_enable(pcie_osal_pwr_ctl_type *pwr_ctl);

/* ============================================================================
**  Function : pcie_osal_pwr_domain_disable
** ============================================================================
*/
/**
  Diable power domain.
  
  @param[in,out]  pwr_ctl    clock control object to be used

  @return
  Success of the operation.
  
*/
pcie_osal_status_type pcie_osal_pwr_domain_disable(pcie_osal_pwr_ctl_type *pwr_ctl);




#endif // #define _pcie_OSAL_H_
