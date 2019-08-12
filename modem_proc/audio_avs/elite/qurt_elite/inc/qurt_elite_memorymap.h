/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */
/**
@file qurt_elite_memorymap.h

@brief This file contains utilities for memory mapping and unmapping of shared
memory, LPM, hardware registers, MIDI etc.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_memorymap.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
02/07/12   kk      Badger API changes 
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/28/11   AZ      Support virtual contiguous memory.
06/26/10   kk      Created file.
========================================================================== */

#ifndef QURT_ELITE_MEMORYMAP_H
#define QURT_ELITE_MEMORYMAP_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_mutex.h"
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"

#if defined(__qdsp6__)
#include "qurt.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_memorymap
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
//***************************************************************************
// QURT_ELITE_MEMORYPOOLTYPE
//***************************************************************************

/** Defines memory map pool IDs. In the QuRT OS, memory map pool IDs are
    directly referred to by pool name.
 */
typedef enum
{
    QURT_ELITE_MEMORYMAP_DEFAULTPHY_POOL=0, /**< Default physical pool. */
    QURT_ELITE_MEMORYMAP_EBI1_POOL,         /**< EBI1 pool. */
    QURT_ELITE_MEMORYMAP_SMI_POOL,          /**< Stacked memory interface pool. */
    QURT_ELITE_MEMORYMAP_SHMEM8_4K_POOL,    /**< Shared memory 8-byte addressable,
                                                 4 KB aligned memory pool. */
    QURT_ELITE_MEMORYMAP_POOL_OUT_OF_RANGE  /**< Out of range. */    //keep at end
}  QURT_ELITE_MEMORYPOOLTYPE;


/** Enumeration of Memory Mapping modes.
*/
typedef enum {
    QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING = 0,
    /**< Physical memory is mapped. */

    QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING,
    /**< Virtual memory is mapped. */

    QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING_ACCESS_OFFSET,
    /**< Physical memory is mapped with an offset. */

    QURT_ELITE_MEMORYMAP_HEAP_ADDR_MAPPING
    /**< Heam memory is mapped. @newpage */

} qurt_elite_memorymap_mapping_mode_t;


/** @name Cache Properties
@{ */

/** L1 is write-back, and L2 is cached. */
#define HMEM_CACHE_WRITEBACK QURT_MEM_CACHE_WRITEBACK

/** L1 is write-through, and L2 is cached. */
#define HMEM_CACHE_WRITETHROUGH QURT_MEM_CACHE_WRITETHROUGH

/** L1 is write-back, and L2 is uncached. */
#define HMEM_CACHE_WRITEBACK_NONL2CACHEABLE QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE

/** L1 is write-through, and L2 is uncached. */
#define HMEM_CACHE_WRITETHROUGH_NONL2CACHEABLE QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE

/** L1 is write-back, and L2 is cached. */
#define HMEM_CACHE_WRITEBACK_L2CACHEABLE QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE

/** L1 is write-through, and L2 is cached. */
#define HMEM_CACHE_WRITETHROUGH_L2CACHEABLE QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE

/** L1 is uncached, and L2 is uncached device type. */
#define HMEM_CACHE_NONE QURT_MEM_CACHE_NONE

/** L1 is uncached, and L2 is uncached shared type. @newpage */
#define HMEM_CACHE_NONE_SHARED QURT_MEM_CACHE_NONE_SHARED

/** @} */ /* end_namegroup Cache Properties */

/** Defines qurt_elite_memorymap_cache_mode_t, which is a wrapper for
  qurt_mem_cache_mode_t.

  The following options are supported:
  - #HMEM_CACHE_WRITEBACK=7
  - #HMEM_CACHE_WRITETHROUGH=5
  - #HMEM_CACHE_WRITEBACK_NONL2CACHEABLE=0
  - #HMEM_CACHE_WRITETHROUGH_NONL2CACHEABLE=1
  - #HMEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK
  - #HMEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH
  - #HMEM_CACHE_NONE=4
  - #HMEM_CACHE_NONE_SHARED=6
 */
typedef qurt_mem_cache_mode_t qurt_elite_memorymap_cache_mode_t;

/** Defines qurt_elite_memorymap_region_t, which is a wrapper for
    qurt_mem_region_t.
 */
typedef qurt_mem_region_t qurt_elite_memorymap_region_t;

/** Contiguous shared memory region, with the start address and size.  
 */
typedef struct {
    uint32_t shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. */

    uint32_t shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw word must be
         contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32 bit value. @tablebulletend */

    uint32_t mem_size;
    /**< Size of the shared memory region. 
    
         Number of bytes in the shared memory region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. @newpagetable */

} qurt_elite_memorymap_shm_region_t
;


/** Memory mapped region attributes.
 */
typedef struct {
    uint32_t mem_reg_base_phy_addr_lsw;
   /**< Lower 32 bits of the 64-bit memory region start (base) physical
        address. */

    uint32_t mem_reg_base_phy_addr_msw;
   /**< Upper 32 bits of the 64-bitmemory region start (base) physical
        address.

         The 64-bit number formed by mem_reg_base_phy_addr_lsw and
         mem_reg_base_phy_addr_msw word must be contiguous memory, and it must
         be 4 KB aligned.

         @values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32-bit value. @tablebulletend */

    uint32_t mem_reg_size;
    /**< Size of the shared memory region. 

         Number of bytes in the shared memory region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */


    uint32_t mem_reg_base_virt_addr;
    /**< Memory region start (base) virtual address. */

    uint32_t req_virt_adrr;
    /**< Virtual address that corresponds to the requested physical address. */

    uint32_t rem_reg_size;
    /**< Remaining memory region size from the requested physical address,
         including the requested physical address:

         ([mem_reg_base_phy_addr_msw,mem_reg_base_phy_addr_lsw] +
         mem_reg_size - [requested phy addr] ) @newpagetable */

} qurt_elite_memorymap_mem_region_attrib_t
;

/**
  Registers a client with qurt_elite_memorymap.

  @param[out] client_token_ptr  Pointer to an instance of
                         qurt_elite_memorymap_client_t that is
                         created and returned as a handle/token to the client.
                         \n
                         This handle uniquely identifies the client, and the
                         client must use this handle for future communication
                         with qurt_elite_memorymap.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  None.
*/
ADSPResult qurt_elite_memorymap_register(uint32_t *client_token_ptr);


/**
  Deletes all regions tagged to this client and unregisters this client from
  qurt_elite_memorymap.

  @param[in] client_token  Client token.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_unregister(uint32_t client_token);


/**
  Maps the shared memory and adds the memory region to the client linked list.
  This function enables physical address mapping only.

  @datatypes
  qurt_elite_memorymap_shm_region_t \n
  #qurt_elite_memorymap_cache_mode_t \n
  #QURT_ELITE_MEMORYPOOLTYPE

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to an array of shared memory
                                     regions to map.
  @param[in]  num_shm_reg            Number of shared memory regions in the
                                     array.
  @param[in]  cache_attr             Any HMEM_CACHE attribute.
  @param[in]  pool_id                Specifies the memory pool ID to which this
                                     region is to be mapped.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     shared memory region created. This handle
                                     can be used later to unmap the shared
                                     memory.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_shm_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Maps the shared memory and adds the memory region to the client linked list.
  This function enables physical address mapping using Offset mode. 

  @datatypes
  qurt_elite_memorymap_shm_region_t \n
  #qurt_elite_memorymap_cache_mode_t \n
  #QURT_ELITE_MEMORYPOOLTYPE

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to an array of shared memory
                                     regions to map.
  @param[in]  num_shm_reg            Number of shared memory regions in the
                                     array.
  @param[in]  cache_attr             Any HMEM_CACHE attribute.
  @param[in]  pool_id                Memory pool ID to which this region is
                                     to be mapped.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     shared memory region created. This handle
                                     can be used later to unmap the shared
                                     memory.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_shm_mem_map_offset(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Maps the shared virtual address and adds the memory region to the client 
  linked list. This function must be used to map a virtual address.

  @datatypes
  qurt_elite_memorymap_shm_region_t \n
  #qurt_elite_memorymap_cache_mode_t \n
  #QURT_ELITE_MEMORYPOOLTYPE

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to an array of shared memory
                                     regions to map.
  @param[in]  num_shm_reg            Number of shared memory regions in the
                                     array.
  @param[in]  cache_attr             Any HMEM_CACHE attribute.
  @param[in]  pool_id                Memory pool ID to which this region is
                                     to be mapped.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     shared memory region created. This handle
                                     can be used later to unmap the shared
                                     memory.
       
  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_virtaddr_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint16_t num_shm_reg,
                    qurt_elite_memorymap_cache_mode_t cache_attr,
                    QURT_ELITE_MEMORYPOOLTYPE pool_id,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Maps the heap address and adds the memory region to the client linked list.
  This function must be used to map a heap address.

  @datatypes
  qurt_elite_memorymap_shm_region_t

  @param[in]  client_token           Client token.
  @param[in]  shm_mem_reg_ptr        Pointer to a single heap memory region to map.
  @param[out] shm_mem_map_handle_ptr Pointer to the memory map handle of the
                                     heap memory region. This handle can be
                                     used later to unmap the heap memory.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_heapaddr_mem_map(    uint32_t client_token,
                    qurt_elite_memorymap_shm_region_t* shm_mem_reg_ptr,
                    uint32_t* shm_mem_map_handle_ptr);

/**
  Gets the memory mapping mode for a specified memory map handle.

  @datatypes
  #qurt_elite_memorymap_mapping_mode_t

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling one of the following:
                                  - qurt_elite_memorymap_shm_mem_map()
                                  - qurt_elite_memorymap_shm_mem_map_offset()
                                  - qurt_elite_memorymap_virtaddr_mem_map()
                                  - qurt_elite_memorymap_heapaddr_mem_map()
                                  @tablebulletend
  @param[out] mapping_mode_ptr    Pointer to the Memory Mapping mode.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_get_mapping_mode(    uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    qurt_elite_memorymap_mapping_mode_t* mapping_mode_ptr);

/**
  Unmaps the memory region and deletes the entry from the client-given memory
  map handle.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
ADSPResult qurt_elite_memorymap_shm_mem_unmap(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );
                                  
/**
  Gets the corresponding virtual address for a specified shared memory address
  and memory map handle.

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling
                                  qurt_elite_memorymap_shm_mem_map().
  @param[in]  shm_addr_lsw        Any shared memory address least significant
                                  word of the mapped region.
  @param[in]  shm_addr_msw        Any shared memory address most significant
                                  word of the mapped region.
  @param[out] virt_addr_ptr       Pointer to the equivalent virtual address
                                  returned.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
ADSPResult qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    uint32_t *virt_addr_ptr);                                 

/**
  Gets the region attributes for a specified shared memory address and memory
  map handle.

  @datatypes
  qurt_elite_memorymap_mem_region_attrib_t

  @param[in]  client_token        Client token.
  @param[in]  shm_mem_map_handle  Memory map handle of the shared memory region
                                  created when calling
                                  qurt_elite_memorymap_shm_mem_map().
  @param[in]  shm_addr_lsw        Any shared memory address least significant
                                  word of the mapped region.
  @param[in]  shm_addr_msw        Any shared memory address most significant
                                  word of the mapped region.
  @param[out] mem_reg_attrib_ptr  Pointer to the memory region attribute whose
                                  fields are filled by this function.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
ADSPResult qurt_elite_memorymap_get_mem_region_attrib_from_shmm_handle(uint32_t client_token,
                                    uint32_t shm_mem_map_handle,
                                    uint32_t shm_addr_lsw,
                                    uint32_t shm_addr_msw,
                                    qurt_elite_memorymap_mem_region_attrib_t *mem_reg_attrib_ptr);

/**
  Unmaps all memory regions and deletes all nodes of the requested client.

  @param[in] client_token  Client token.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered before calling this function.
  @newpage
*/
ADSPResult qurt_elite_memorymap_unmap_all(uint32_t client_token);

/**
  Flushes the cache (data) of the specified memory region.

  @param[in] virt_addr  Starting virtual address.
  @param[in] mem_size   Size of the region to be flushed.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
static inline ADSPResult qurt_elite_memorymap_cache_flush(uint32_t virt_addr,
                      uint32_t mem_size)
{

#if (!defined(SIM))||(__QDSP6_ARCH__ >= 3)

    int qurt_mem_result;

    if( (EOK != (qurt_mem_result = qurt_mem_cache_clean( (qurt_addr_t)(virt_addr),
                                mem_size,
                                QURT_MEM_CACHE_FLUSH,
                                QURT_MEM_DCACHE ))))
    {
        return ADSP_EFAILED;
    }


#endif //__QDSP6_ARCH__

    return ADSP_EOK;


}

/**
Invalidates the cache (data) of the specified memory region.

  @param[in]  virt_addr  Starting virtual address.
  @param[in]  mem_size   Size of the region to be invalidated.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
static inline ADSPResult qurt_elite_memorymap_cache_invalidate(uint32_t virt_addr,
                       uint32_t mem_size)
{

#if (!defined(SIM))||(__QDSP6_ARCH__ >= 3)

    int qurt_mem_result;

    if( (EOK != (qurt_mem_result = qurt_mem_cache_clean( (qurt_addr_t)(virt_addr),
                                mem_size,
                                QURT_MEM_CACHE_INVALIDATE,
                                QURT_MEM_DCACHE ))))

    {
        return ADSP_EFAILED;
    }


#endif //__QDSP6_ARCH__

    return ADSP_EOK;


}


/**
  Flushes the cache (data) of the specified memory region if the Memory Mapping
  mode is:
  - #QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING
  - #QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING
  - #QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING_ACCESS_OFFSET

  No operation if the memory mapping mode is
  QURT_ELITE_MEMORYMAP_HEAP_ADDR_MAPPING.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling one of the following:
                                 - qurt_elite_memorymap_shm_mem_map()
                                 - qurt_elite_memorymap_shm_mem_map_offset()
                                 - qurt_elite_memorymap_virtaddr_mem_map()
                                 - qurt_elite_memorymap_heapaddr_mem_map() @tablebulletend
  @param[in] virt_addr           Starting virtual address.
  @param[in] mem_size            Size of the region to be flushed.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
ADSPResult qurt_elite_memorymap_cache_flush_v2(uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    uint32_t virt_addr,
                    uint32_t mem_size);

/**
  Invalidates the cache (data) of the specified memory region if the Memory
  Mapping mode is:
  - #QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING
  - #QURT_ELITE_MEMORYMAP_VIRTUAL_ADDR_MAPPING
  - #QURT_ELITE_MEMORYMAP_PHYSICAL_ADDR_MAPPING_ACCESS_OFFSET

  No operation if the memory mapping mode is
  QURT_ELITE_MEMORYMAP_HEAP_ADDR_MAPPING.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling one of the following:
                                 - qurt_elite_memorymap_shm_mem_map()
                                 - qurt_elite_memorymap_shm_mem_map_offset()
                                 - qurt_elite_memorymap_virtaddr_mem_map()
                                 - qurt_elite_memorymap_heapaddr_mem_map() @tablebulletend
  @param[in] virt_addr           Starting virtual address.
  @param[in] mem_size            Size of the region to be invalidated.

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
ADSPResult qurt_elite_memorymap_cache_invalidate_v2(uint32_t client_token,
                    uint32_t shm_mem_map_handle,
                    uint32_t virt_addr,
                    uint32_t mem_size);

/**
  Given a virtual address, retrieves the corresponding mapping entry from 
  the page table, adjusts the offset based on the page size, and returns the
  full 64-bit physical address back to the user.
 
  @param[in] virt_addr    Starting virtual address.
 
  @return 
  Physical address if the address is mapped; otherwise, 0. 

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function.
*/
static inline uint64_t qurt_elite_memorymap_get_physical_addr(uint32_t virt_addr){

    uint64_t phy_addr = (uint64_t)(qurt_lookup_physaddr (virt_addr));
    return phy_addr;

}

/**
  Increments the reference count of given memory map handle of a client.

  Incrementing this reference count suggests that the memory region abstracted
  by this memory map handle is in use. A nonzero reference count prevents the
  specific memory map regions to be unmapped by the aDSP client.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
ADSPResult qurt_elite_memorymap_shm_incr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );

/**
  Decrements the reference count of given memory map handle of a client.

  Decrementing this reference count suggests that this client is relinquishing
  the memory region abstracted by this memory map handle; the region is no
  longer required by this client. The reference count must reach zero for the
  memory region to be unmapped.

  @param[in] client_token        Client token.
  @param[in] shm_mem_map_handle  Memory map handle of the shared memory region
                                 created when calling
                                 qurt_elite_memorymap_shm_mem_map().

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  The client object must have been registered and the corresponding memory
  mapped before calling this function. @newpage
*/
ADSPResult qurt_elite_memorymap_shm_decr_refcount(uint32_t client_token,
                    uint32_t shm_mem_map_handle
                    );
/**
  Fetches the memory pool information needed for memory map

  This function uses a combination of 2 QURT APIs to fetch the details of the 
  memory pools to be used in the process of memory mapping

  @param[in] 
  None

  @return
  ADSPResult error code (refer to @xhyperref{Q4,[Q4]}).

  @dependencies
  None @newpage
*/
ADSPResult qurt_elite_memory_pool_info_fetch(void);

/** @} */ /* end_addtogroup qurt_elite_memorymap */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MEMORYMAP_H

