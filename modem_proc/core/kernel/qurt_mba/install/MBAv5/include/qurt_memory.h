#ifndef QURT_MEMORY_H
#define QURT_MEMORY_H
/*======================================================================
                        qurt_memory.h

GENERAL DESCRIPTION
  Prototypes of Kernel memory API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================

											 EDIT HISTORY FOR FILE

	 This section contains comments describing changes made to the
	 module. Notice that changes are listed in reverse chronological
	 order.

	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_memory.h#1 $ 
	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $


when 				who 		what, where, why
---------- 	--- 		------------------------------------------------
2011-02-25 	op			Add Header file
======================================================================*/
#include <qurt_error.h>
#include <qurt_types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern qurt_mem_pool_t qurt_mem_default_pool;

/*=====================================================================
 Functions
======================================================================*/

/**
 * Perform cache clean in user mode based on option (flush, invalidate, or both) and type (icache, dcache).
 * Cache line is 32 bytes, therefore address and size are forced to align.
 *
 * @param data      Pointer to data or instruction memory that needs to be cleaned
 * @param size      Size of the data
 * @param opcode    Type of cache clean operation (QURT_MEM_CACHE_FLUSH, QURT_MEM_CACHE_INVALIDATE, 
                    QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_CACHE_FLUSH_ALL)
 * @param type      Type of cache to clean (QURT_MEM_ICACHE, QURT_MEM_DCACHE)
 *
 * @param return    QURT_EOK if operation successful
 *                  QURT_EVAL if type is not valid
 *                  QURT_EALIGN if aligning data or address failed
 *
*/
int qurt_mem_cache_clean(qurt_addr_t addr, qurt_size_t size, qurt_mem_cache_op_t opcode, qurt_mem_cache_type_t type);

/**
 * Initialize attribute object with default values. The default value for
 * physical address and virtual address is INVALID_ADDR (-1),
 * QMEM_MEMORY_DEFAULT for memory type, QMEM_MAPPING_VIRTUAL for mapping type,
 * and QMEM_CACHE_WRITEBACK_SHARED for cacheing property
 *
 * @param attr  Attributes object
 * @return      void
 */
static inline void qurt_mem_region_attr_init(qurt_mem_region_attr_t *attr){
    attr->mapping_type = QURT_MEM_MAPPING_VIRTUAL;
    attr->cache_mode = QURT_MEM_CACHE_WRITEBACK;
    attr->physaddr =(qurt_addr_t) -1; 
    attr->virtaddr =(qurt_addr_t) -1;
    attr->type = QURT_MEM_REGION_LOCAL;
    attr->size = (qurt_size_t)-1;
}

/**
 * Attach to a memory pool
 *
 * The qmem_pool_attach is used to attach to a memory pool.
 *
 * Qube memory model works with the concept of memory pool and memory region.
 * User can specify different types of PHYSICAL memory as different memory
 * pools.  For example EBI/SMI/TCM memory can be specified as EBI pool, SMI
 * pool and TCM pool respectively.
 *
 * Memory pools are static system information that can only be specified in xml
 * configuration file.  All pools are by default accessible by all PDs in the
 * system, a PD can get the handle of a pool with qmem_pool_attach function
 * with a name.
 *
 * The default physical pool specified in elfweaver configuration is mapped to
 * qmem_default_pool global handle.  Users only need to call qmem_pool_attach
 * for non-default physical memory pool
 *
 * @param name [IN]   Name of the memory pool. This shall be a name specified
 *                    in xml configuration file for a physical pool
 * @param pool [OUT]  memory pool object
 * @return            QURT_EOK:       Successful attach
*/
int qurt_mem_pool_attach(char *name, qurt_mem_pool_t *pool);


int qurt_mem_region_create(qurt_mem_region_t *region, qurt_size_t size, qurt_mem_pool_t pool, qurt_mem_region_attr_t *attr);


int qurt_mem_region_delete(qurt_mem_region_t region);


/**
* Get attributes of a memory region
*
* @param region        Memory region object
* @param attr   [OUT]  Memory region attributes
* @return              QURT_EOK if successful, error code otherwise
*/
int qurt_mem_region_attr_get(qurt_mem_region_t region, qurt_mem_region_attr_t *attr);


/**
 * Set memory type. The possible values are QURT_MEM_MEMORY_DEFAULT and
 * QURT_MEM_MEMORY_TCM
 *
 * @param attr  Attributes object
 * @param type  Memory type
 * @return      void
 */
static inline void qurt_mem_region_attr_set_type(qurt_mem_region_attr_t *attr, qurt_mem_region_type_t type){
    attr->type = type;
}

/**
 * Get memory size
 *
 * @param attr  Attributes object
 * @param size  Memory size
 * @return      void
 */
static inline void qurt_mem_region_attr_get_size(qurt_mem_region_attr_t *attr, qurt_size_t *size){
    (*size) = attr->size;
}

/**
 * Get memory type
 *
 * @param attr  Attributes object
 * @param type  Memory type
 * @return      void
 */
static inline void qurt_mem_region_attr_get_type(qurt_mem_region_attr_t *attr, qurt_mem_region_type_t *type){
    (*type) = attr->type;
}

/**
 * Set physical address attribute
 *
 * @param attr  Attributes object
 * @param addr  Physical address where memory region should be created
 * @return      void
 */
static inline void qurt_mem_region_attr_set_physaddr(qurt_mem_region_attr_t *attr, qurt_addr_t addr){
    attr->physaddr = addr;
}

/**
 * Get physical address attribute
 *
 * @param attr  Attributes object
 * @param addr  Physical address where memory region should be created
 * @return      void
 */
static inline void qurt_mem_region_attr_get_physaddr(qurt_mem_region_attr_t *attr, unsigned int *addr){
    (*addr) = (unsigned int) (attr->physaddr);
}

/**
 * Set virtual address attribute
 *
 * @param attr  Attributes object
 * @param addr  Virtual address where memory region should be created
 * @return      void
 */
static inline void qurt_mem_region_attr_set_virtaddr(qurt_mem_region_attr_t *attr, qurt_addr_t addr){
    attr->virtaddr = addr;
}

/**
 * Get virtual address attribute
 *
 * We don't have a setvirtaddr helper api because users can not specify virtual
 * address for a memory region
 *
 * @param attr  Attributes object
 * @param addr  Virtual address where memory region should be created
 * @return      void
 */
static inline void qurt_mem_region_attr_get_virtaddr(qurt_mem_region_attr_t *attr, unsigned int *addr){
    (*addr) = (unsigned int)(attr->virtaddr);
}

/**
 * Set memory mapping type. The possible values are QURT_MEM_MAPPING_VIRTUAL,
 * QURT_MEM_MAPPING_IDEMPOTENT and QURT_MEM_MAPPING_PHYS_CONTIGUOUS
 *
 * @param attr     Attributes object
 * @param mapping  Mapping property
 * @return         void
 */
static inline void qurt_mem_region_attr_set_mapping(qurt_mem_region_attr_t *attr, qurt_mem_mapping_t mapping){
    attr->mapping_type = mapping;
}

/**
 * Get memory mapping type
 *
 * @param attr     Attributes object
 * @param mapping  Mapping property
 * @return         void
 */
static inline void qurt_mem_region_attr_get_mapping(qurt_mem_region_attr_t *attr, qurt_mem_mapping_t *mapping){
    (*mapping) = attr->mapping_type;
}

/**
 * Set cache property. The possible values are QURT_MEM_CACHE_WRITEBACK,
 * QURT_MEM_CACHE_WRITETHROUGH and QURT_MEM_CACHE_NONE
 *
 * @param attr  Attributes object
 * @param mode  Cacheability property
 * @return      void
 */
static inline void qurt_mem_region_attr_set_cache_mode(qurt_mem_region_attr_t *attr, qurt_mem_cache_mode_t mode){
    attr->cache_mode = mode;
}

/**
 * Get cache property
 *
 * @param attr  Attributes object
 * @param mode  Cacheability property
 * @return      void
 */
static inline void qurt_mem_region_attr_get_cache_mode(qurt_mem_region_attr_t *attr, qurt_mem_cache_mode_t *mode){
    (*mode) = attr->cache_mode;
}

/**
 * Query physical address in STATICALLY created mappings: if given physical address is found in
 * existing static mappings, and memory attributes match, virtual address of
 * this mapping is returned back to the user, -1 otherwise
 *
 * @param paddr             Physical address
 * @param vaddr       [OUT] Virutal address corresponding to paddr
 * @param page_size         Size of memory to map
 * @param cache_atrribs     Cacheability
 * @param perm              Page permissions
 * @return                  int error code
 * @return                  QURT_EOK  : found corresponding mapping, return virtual address (0)
 * @return                  QURT_EMEM : mapping does not exist in static table of mappings, return -1 as virutual address (1)
 * @return                  QURT_EVAL : page attribute(s) do not match, return -1 as virtual address (2)
 */
int qurt_mem_map_static_query(qurt_addr_t *vaddr, qurt_addr_t paddr, unsigned int page_size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm);


/**
 * Query list of all existing DYNAMICALLY created regions (including image) based on virt or phys address;
 * to query for vaddr based on paddr, set vaddr = INVALID_ADDR, paddr = address;
 * to query for paddr based on vaddr, set vaddr = address, paddr = INVALID_ADDR;
 * setting both vaddr = INVALID_ADDR, paddr = INVALID_ADDR or vaddr = address, paddr = address are cases of invalid input;
 * valid region_handle can be further used as input to mem_region_get_attr() to retrieve desired attributes;
 *
 * @param region_handle   [OUT] handle to a region containing specified address
 * @param vaddr                 virtual address
 * @param paddr                 physical address
 * @return                      int error code
 * @return                      QURT_EOK  : found corresponding region, region_handle contains valid handle
 * @return                      QURT_EMEM : region with specified address could not be found, region_handle = INVALID_ADDR
 * @return                      E_FATAL : invalid input parameters, region_handle = INVALID_ADDR
 */
int qurt_mem_region_query(qurt_mem_region_t *region_handle, qurt_addr_t vaddr, qurt_addr_t paddr);


/**
* Create a mapping in pagetable using input parameters;
*
* @param vaddr			virtual addr
* @param paddr			phys addr
* @param size			size (4K-aligned)
* @param cache_attribs		cache attributes
* @param perm			permissions
* @return			QURT_EOK : created mapping
* @return 			QURT_EMEM : failed to create mapping 			
*/
int qurt_mapping_create(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size,
                         qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm);

/** 
 * Remove mapping from pagetable based on input paramenters
 *
 * @param vaddr			virt addr
 * @param paddr			phys addr
 * @param size			size (4K-aligned)
 * @return 			QURT_EOK : removed mapping
 * 		
 */ 		
int qurt_mapping_remove(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size);

/**
* Given virtual address, retrieve corresponding mapping entry from the pagetable, adjust offset based on the page size
* and return full physical address back to the user.
*
* @param vaddr                  virtual address in questions
* @return                       physical address if address is mapped,
* @return                       0 otherwise
*/
qurt_addr_t qurt_lookup_physaddr (qurt_addr_t vaddr);

    
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_MEMORY_H */
