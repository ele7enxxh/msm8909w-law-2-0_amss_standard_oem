#ifndef QMEMORY_H
#define QMEMORY_H
/*=============================================================================

                 qmemoryt.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qmemory API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qmemory.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <qurt_error.h>
#include <qpd.h>
#include <assert.h>
#include <qtypes.h>
#include <qurt_memory.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        CONSTANTS AND MACROS DECLARATIONS
=============================================================================*/

/* translation of qmem to qurt_mem */

#define QMEM_MAPPING_VIRTUAL            QURT_MEM_MAPPING_VIRTUAL
#define QMEM_MAPPING_PHYS_CONTIGUOUS    QURT_MEM_MAPPING_PHYS_CONTIGUOUS
#define QMEM_MAPPING_IDEMPOTENT         QURT_MEM_MAPPING_IDEMPOTENT
#define QMEM_MAPPING_VIRTUAL_FIXED      QURT_MEM_MAPPING_VIRTUAL_FIXED
#define QMEM_MAPPING_NONE               QURT_MEM_MAPPING_NONE
#define qmem_mapping_t                  qurt_mem_mapping_t

#define QMEM_CACHE_WRITEBACK            QURT_MEM_CACHE_WRITEBACK
#define QMEM_CACHE_NONE_SHARED          QURT_MEM_CACHE_NONE_SHARED
#define QMEM_CACHE_WRITETHROUGH         QURT_MEM_CACHE_WRITETHROUGH
#define QMEM_CACHE_WRITEBACK_NONL2CACHEABLE     QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE
#define QMEM_CACHE_WRITETHROUGH_NONL2CACHEABLE  QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE
#define QMEM_CACHE_WRITEBACK_L2CACHEABLE        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE
#define QMEM_CACHE_WRITETHROUGH_L2CACHEABLE     QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE
#define QMEM_CACHE_NONE                 QURT_MEM_CACHE_NONE
#define qmem_cache_mode_t               qurt_mem_cache_mode_t

#define QMEM_ICACHE                     QURT_MEM_ICACHE
#define QMEM_DCACHE                     QURT_MEM_DCACHE
#define qmem_cache_type_t               qurt_mem_cache_type_t

#define QMEM_CACHE_FLUSH                QURT_MEM_CACHE_FLUSH
#define QMEM_CACHE_INVALIDATE           QURT_MEM_CACHE_INVALIDATE
#define QMEM_CACHE_FLUSH_INVALIDATE     QURT_MEM_CACHE_FLUSH_INVALIDATE
#define qmem_cache_op_t                 qurt_mem_cache_op_t


#define QMEM_REGION_LOCAL               QURT_MEM_REGION_LOCAL
#define QMEM_REGION_SHARED              QURT_MEM_REGION_SHARED
#define qmem_region_type_t              qurt_mem_region_type_t

#define qmem_region_t                   qurt_mem_region_t
#define qmem_region_attr_t              qurt_mem_region_attr_t
#define qmem_region_attr_init		qurt_mem_region_attr_init

#define qmem_region_create              qurt_mem_region_create
#define qmem_region_delete              qurt_mem_region_delete
#define blast_mem_region_query          qurt_mem_region_query

#define qmem_region_get_attr		qurt_mem_region_attr_get
#define qmem_region_attr_getsize	qurt_mem_region_attr_get_size
#define qmem_region_attr_gettype 	qurt_mem_region_attr_get_type
#define qmem_region_attr_getphysaddr	qurt_mem_region_attr_get_physaddr
#define qmem_region_attr_getvirtaddr	qurt_mem_region_attr_get_virtaddr
#define qmem_region_attr_getmapping	qurt_mem_region_attr_get_mapping
#define qmem_region_attr_getcachemode	qurt_mem_region_attr_get_cache_mode
#define qmem_region_attr_settype	qurt_mem_region_attr_set_type
#define qmem_region_attr_setphysaddr	qurt_mem_region_attr_set_physaddr
#define qmem_region_attr_setvirtaddr	qurt_mem_region_attr_set_virtaddr
#define qmem_region_attr_setmapping	qurt_mem_region_attr_set_mapping
#define qmem_region_attr_setcachemode	qurt_mem_region_attr_set_cache_mode

#define qmem_pool_t                     qurt_mem_pool_t
#define qmem_default_pool               qurt_mem_default_pool
#define qmem_pool_attach(name,pool)     qurt_mem_pool_attach((char *)name, (qurt_mem_pool_t *)pool)
#define qmem_cache_flushall(a)          qurt_mem_cache_clean(0, 0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE)
#define qmem_cache_clean(addr,size,opcode,type)		qurt_mem_cache_clean((qurt_addr_t)addr,(qurt_size_t)size,(qurt_mem_cache_op_t)opcode,(qurt_mem_cache_type_t)type)
#define qmem_cache_clean_aligning(addr,size,opcode,type)	qurt_mem_cache_clean((qurt_addr_t)addr,(qurt_size_t)size,(qurt_mem_cache_op_t)opcode,(qurt_mem_cache_type_t)type)

#define qmem_perm_t                     qurt_perm_t
#define QMEM_PERM_READ                  QURT_PERM_READ
#define QMEM_PERM_WRITE                 QURT_PERM_WRITE
#define QMEM_PERM_EXECUTE               QURT_PERM_EXECUTE

/* end of qmem to qurt_mem translations */


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QMEMORY_H */
