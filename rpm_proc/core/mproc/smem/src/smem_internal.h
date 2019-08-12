#ifndef _SMEM_INTERNAL_H_
#define _SMEM_INTERNAL_H_

/*===========================================================================

                    S H A R E D   M E M O R Y

DESCRIPTION
    Defines the private api that is used between SMEM module
    SMEM.  This API deal strictly with void * to reduce dependencies.

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_internal.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/30/12   bm      Add heap_size field in smem_ext_heap_info_type
01/19/12   bm      Add fatures to support SMEM allocation from ouside of
                   SMEM. Feature added for RPM clients.
01/05/12   hwu     Moved the host id defines to smem_type.h.
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: adding Spinlocks
                   Region base address initialization
07/25/11   rs      Added type to define SMEM hosts
05/31/11   bm      Initial Version - type/function declarations to be used
                   between smem.c and smem_boot.c
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_os.h"
#include "smem.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/*----------------------------------------------------------------------
 * Heap info structure - keeps track of which part of heap has been
 * allocated. The heap management scheme is trivial - we just allocate
 * from the end of the heap, we never free anything.
 *----------------------------------------------------------------------*/
#define SMEM_HEAP_INFO_INIT                    1
#define SMEM_ALLOCATION_ENTRY_ALLOCATED        1

/* Flags for smem_allocation_table_type.flags */
/* The last two bits of base_addr field (previously reserved) are
 * still kept reserved for future use */
#define SMEM_WORD_ALIGN_MASK            0x00000003
#define SMEM_ALLOC_TBL_RESERVED_MASK    SMEM_WORD_ALIGN_MASK

typedef struct
{
  uint32 initialized;
  uint32 free_offset;
  uint32 heap_remaining;
  uint32 reserved;
} smem_heap_info_type;

typedef struct
{
  char*  base;
  char*  base_virt;
  uint32 free_offset;
  uint32 heap_remaining;
  uint32 heap_size;
  uint32 reserved;
} smem_ext_heap_info_type;

/*----------------------------------------------------------------------
 * Allocation table - keeps track of which buffers have been allocated
 * and their sizes. Buffers are only allocated once, subsequent calls
 * to smem_alloc for that buffer return the same address regardless of
 * where smem_alloc is called.
 *----------------------------------------------------------------------*/
typedef struct
{
  uint32           allocated;
  uint32           offset;
  uint32           size;
  uint32           base_ext;
} smem_allocation_table_type;

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/
typedef struct
{
  smem_mem_type tag;            /* Tag associated with buffer */
  uint32        size;           /* Size of the buffer */
} smem_buf_data_type;

/*----------------------------------------------------------------------
 * Shared memory initialization state - tracks what initialization
 * has been done. 
 *----------------------------------------------------------------------*/
typedef enum
{
  SMEM_UNINITIALIZED,    /**< shared memory original state */ 
  SMEM_PRE_BOOT_INIT,    /**< shared memory is not zero-initialized yet */ 
  SMEM_BOOT_INITIALIZED, /**< shared memory is zero-ed out and initialized */ 
  SMEM_INITIALIZED       /**< shared memory is initialized */
}smem_state_type;

typedef struct {
  volatile smem_heap_info_type *smem_heap_info;

/*----------------------------------------------------------------------
 * Allocation table - keeps track of which buffers have been allocated
 * and their sizes. Buffers are only allocated once, subsequent calls
 * to smem_alloc for that buffer return the same address regardless of
 * where smem_alloc is called.
 *----------------------------------------------------------------------*/
  volatile smem_allocation_table_type *smem_allocation_table;

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/

  const smem_buf_data_type* smem_buf_data_tbl;

/*----------------------------------------------------------------------
 * Base virtual address of shared memory - it may be equivalent to the 
 * physical address depending on the environment.
 *----------------------------------------------------------------------*/
  char *smem_data_base_addr;

/*----------------------------------------------------------------------
 * Base physical address of shared memory - usually read from IMEM.
 * Should not be dereferenced, so it is an address and not a pointer.
 *----------------------------------------------------------------------*/
  char *smem_phys_base_addr;

/*----------------------------------------------------------------------
 * Size of shared memory
 *----------------------------------------------------------------------*/
  uint32 smem_data_size;

/*----------------------------------------------------------------------
 * memory barrier location - global pointer for memory barrier location
 *----------------------------------------------------------------------*/
  uint32 *smem_memory_barrier;

/*----------------------------------------------------------------------
 * Shared memory initialization state - tracks what initialization
 * has been done. 
 *----------------------------------------------------------------------*/
  smem_state_type smem_state;
}smem_init_info_type;

/*===========================================================================

                      INTERNAL UTIL FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_get_offset
                                                                             
DESCRIPTION   Returns the offset to one of the "fixed" memory buffers

ARGUMENTS     tag     Type of memory to get a pointer for.
                  
DEPENDENCIES  None
      
RETURN VALUE  Return offset to the requested buffer, otherwise fatal error.
  
SIDE EFFECTS  None
===========================================================================*/
uint32 smem_get_offset( smem_mem_type tag );

/*===========================================================================
FUNCTION      smem_get_static_size

DESCRIPTION   Returns the size of one of the "fixed" memory buffers.  This
              function uses the local smem_buf_data_tbl, and does not touch
              the actual allocation table in SMEM.

ARGUMENTS     tag     The SMEM item whose size to query.

DEPENDENCIES  None

RETURN VALUE  Return offset to the requested buffer, otherwise fatal error.

SIDE EFFECTS  None
===========================================================================*/
uint32 smem_get_static_size( smem_mem_type tag );

/*===========================================================================
FUNCTION      smem_set_base_addr
                                                                             
DESCRIPTION   Initializes the smem_data_base_addr pointer.  This function
              must be called before any smem operations are performed.

ARGUMENTS     None
                  
DEPENDENCIES  Even though calls to this function are gated by the value
              of smem_data_base_addr, this function must still be 
              archtectured to handle  simultaneous calls from multiple
              reads.

              This function should never call ERR_FATAL in an AMSS 
              environment.
      
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
void smem_set_base_addr( void );

/*===========================================================================
  FUNCTION  smem_set_spinlock_base_addr
===========================================================================*/
/**
  Initializes the spinlocks region base address.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_set_spinlock_base_addr( void );

/*===========================================================================
  FUNCTION  smem_get_base_addr
===========================================================================*/
/**
  Returns the virtual base address of SMEM.

  @return
  A void pointer to the base of SMEM.
*/
/*=========================================================================*/
char *smem_get_base_addr( void );

/*===========================================================================
  FUNCTION  smem_get_phys_base_addr
===========================================================================*/
/**
  Returns the physical base address of SMEM.

  @return
  The physical address of the base of the SMEM region.
*/
/*=========================================================================*/
char *smem_get_phys_base_addr( void );

/*===========================================================================
  FUNCTION  smem_get_size
===========================================================================*/
/**
  Returns the size of SMEM.

  @return
  The size of SMEM.
*/
/*=========================================================================*/
uint32 smem_get_size( void );

/*===========================================================================
  FUNCTION  smem_get_heap_info
===========================================================================*/
/**
  Returns a direct pointer to the heap info structure in SMEM.

  @return
  A pointer to the heap info struct.
  
  @dependencies
  SMEM must be initialized before calling.
*/
/*=========================================================================*/
volatile smem_heap_info_type *smem_get_heap_info( void );

#endif /* _SMEM_INTERNAL_H_ */
