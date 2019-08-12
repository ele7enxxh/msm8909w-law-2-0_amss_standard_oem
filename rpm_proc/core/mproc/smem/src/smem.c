/*===========================================================================

                         S H A R E D   M E M O R Y

DESCRIPTION
    Defines the public api that is used to allocate memory in physical
    SMEM.  This API deal strictly with void * to reduce dependencies.

Copyright (c) 2004 - 2013 by Qualcomm Technologies, Incorporated.  All 
Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/13   bt      Refactor code to map base of SMEM.
11/05/13   bt      Support 64-bit architectures.
07/26/13   pa      Modify heap_info in smem_alloc_add_table_entry for the sake
                   of LA backwards compatibility.
06/25/13   pa      Added smem_get_base_addr, smem_get_size, and
                   smem_alloc_add_table_entry.
                   Also, update smem_set_base_addr.
06/10/13   bt      Featurize out call to smem_partition_init on SBL, TZ, RPM.
04/24/13   pa      Add call to smem_partition_init in smem_init.
02/26/13   pa      Protect against multiple calls to smem_init.
01/08/13   pa      Set spinlock API function pointers in smem_init
10/31/13   rv      Configuring CSR register when DDR access is needed.
09/14/12   bt      Use changed version mask name SMEM_MAJOR_VERSION_MASK.
05/11/12   bm      Added smem_alloc_rpm() API to allocate from MSG RAM.
03/22/12   bm      Use SMEM target specific macros for RPM msg RAM info.
01/17/12   bm      Adding support for allocating memory out of SMEM region.
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: initialize 
                   spinlocks region base address pointer
08/03/11   bt      Remove/reserve all unused smem_mem_types.
07/20/11   bt      Add ERR_FATAL message if unable to allocate version array.
05/26/11   bm      Moved BOOT related API into smem_boot.c
06/06/11   rs      + Use function smem_map_base_va for all OSs
                   + Corrected comments about smem_set_base_addr
04/26/11   bm      Conditional exclusion of SMSM code not supported by Q6FW.
04/22/11   bt      Use new SMEM PROC defines, remove usage of IMAGE_QDSP6_PROC.
04/19/11   tl      Remove assert.h in favor of platform-specific macros
03/24/11   bt      - Allocate/initialize smsm_size_info in smem_boot_init.
                   - Allow smem_version_set to use masks again.
03/10/11   bm      Removed depricated functions smem_alloc_anon(), 
                   smem_offset() and smem_addr() not needed for SMDLite v2
03/10/11   bt      Compiler warnings resolved for smem_base_addr.
01/31/11   bm      - Moved OS (rex and qube) specific functionality to 
                     smem_(rex/qube).c under os/rex AND os/qube folders.
                   - Cleaned up WINCE code.
01/31/11   rs      Added initialization of SMEM base address for QNX
01/25/11   rs      Renamed spinlock header file
01/06/11   bt      Replaced Proc Comm size definitions with generic definition.
04/30/10   rs      + Added generic memory barrier
                   + Integrated footprint optimizations for DSPS
09/30/10   esh     Removed virtual memory mapping for ADSP interrupt registers
09/09/10   bt      Added support for shared smem on Q6 v3+, removed v1 support.
08/05/10   bt      Consolidated version set for all internal + external calls.
07/27/10   bt      Merged function headers into Doxygen format.
07/22/10   hwu     Added SMEM initialization state. 
06/21/10   hwu     Change to use macro to map the VA/PA address. This adds 
                   flexibility when map memory.
04/19/10   esh     Moved memory definitions for Q6 to q6_memory_defns.h
03/02/10   tl      Added debug-smem-at-boot support
02/03/10   esh     Compiler warning cleanup for Q6
01/27/10   tl      Cleanup for 9600
12/04/09   ih      Fixed base address mapping for WM DLL
11/19/09   sa      warning cleanup, sync-up with symbian
11/09/09   rr      Add support for smem_ext,merge from 04.01
06/10/09   bfc     CMI updates.
06/08/09   hwu     Remove the static mapping for BLAST. 
05/01/09   tl      Added smem_alloc_anon()
03/29/09   hwu     Merged in SMD BLAST support.
10/29/08   jlk     Changed version check to only write smem version.
09/22/08   hwu     Clean up the compilation warnings.
06/18/08   ih      Fixed SMEM cache attributes
06/04/08   ih      Changed MmMapIoSpace to VirtualAlloc/Copy
04/29/08   ih      Added WM7 support
04/23/08   bfc     Added more allocation slots and some error checking to
                   see if the allocation table is large enough.
04/17/08   hwu     Added qdsp6_v2 support.
04/01/08   jlk     fixed version check bug in boot init
03/10/08   hwu     changed to use the new Qube API.
01/20/08   jlk     added smem_version_set
12/07/07   jlk     Changed rev number for additional smem item and proc comm
                   changes
11/19/07   taw     Updated SMEM_VERSION_ID because of adding SMEM_BATT_INFO.
11/12/07   hwu     Added function to get the shared memory base address 
                   for Q6. 
11/02/07   jlk     replaced smem_proc_comm.h with smem_pc_internal.h
10/30/07   hwu     Bumped up the version number due to the addition of
                   smem_channel_alloc_tbl.
09/26/07   jhs     Added SMEM_HS_SUSPEND_FILTER_INFO for HS to support RPC
                   filtering.
09/17/07   ih      Get SMEM address using OALPAtoUA instead of hardcoding it
09/10/07   hn      Update version number because of adding new fields.
08/15/07   bfc     Moved the heap info and allocations table up in SMEM.  
                   Fixed some compilation warnings.
                   Removed uneeded header.
08/10/07   ih      Fixed compiler warnings in WinMob
07/18/07   hwu     Moved err.h to smd_target.h.
06/11/07   ptm     Added smem get addr and smem free APIs and fixed HW RESET
                   addr calc.
05/02/07   hwu     Change to leave the CriticalSection instead of delete it.
04/27/07   ptm     Update version number because allocating 64 data structures
                   for SMD channels is incompatible with previous version.
04/08/07   ptm     Change smem_alloc to include smem_get_static_addr, fix
                   compiler warnings and add smem_ to spin lock symbols.
04/05/07   hwu     Moved SMEM_AARM_WARM_BOOT to static region. 
                   Removed the global access in smem_get_static_addr.
03/20/07   taw     Include smem_target.h instead of customer.h.  Also, moved
                   the definition of memory_barrier to smem_target.h.
03/14/07   bfc     Rearchitectured how SMEM is managed and accessed 
03/09/07   ptm     Fix size of HW Reset Detect field.
03/08/07   hwu     Featurize "cache_mmu_v.h"inclusion.
02/07/07   ptm     Add HW Reset Detect field.
02/06/07   ptm     Added smem_get_static_addr API.
01/23/07   mmg     Added the TSSC shared memory.
01/30/07   bfc     Stubbed out smem_init and moved the setting
                   of the smem base addr to smem_alloc
01/15/07    th     Return the statically mapped uncached virtual
                   address if FEATURE_WINCE_OAL is defined
01/14/07   ddh     Added support for limited sleep
01/04/07   bfc     Added a new api for boot
11/29/06   taw     Include customer.h and msg.h.
11/27/06   ptm     Move smem_get/set_diag_message to smem_diag.c.
11/14/06   taw     Return the physical address if FEATURE_WINCE_BOOTLOADER is
                   defined.
08/27/06   ptm     Add smem_get/set_diag_message APIs.
08/22/06   ptm     Merge WinMobile changes to main line.
08/15/06   taw     Initial version.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_os.h"

#include "smem_target.h"
#include "spinlock.h"

#include "smem.h"
#include "smem_v.h"
#include "smem_internal.h"
#include "smem_ext.h"
#include "smem_version.h"
#include "smem_os_common.h"
#include "smem_partition.h"

#ifdef SMEM_RPM_PROC
#include "rpm_messageram.h"
#endif
/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

#define SMEM_ALLOCATION_TABLE_SIZE    512

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/

const smem_buf_data_type smem_buf_data_tbl[] =
{
  { SMEM_FIRST_FIXED_BUFFER,
    SMEM_MEM_FIRST_SIZE_BYTES },

  { SMEM_VERSION_INFO,
    SMEM_VERSION_INFO_SIZE * sizeof(uint32) },

  { SMEM_HEAP_INFO,
    sizeof(smem_heap_info_type) },

  { SMEM_ALLOCATION_TABLE,
    SMEM_ALLOCATION_TABLE_SIZE * sizeof(smem_allocation_table_type) },

  { SMEM_HW_RESET_DETECT,
    sizeof(smem_hw_reset_id_type) },

  { SMEM_RESERVED_AARM_WARM_BOOT,
    sizeof(uint32) },

  { SMEM_DIAG_ERR_MESSAGE,
    SMEM_DIAG_BUFFER_LEN },

  { SMEM_SPINLOCK_ARRAY,
    SPINLOCK_NUM_LOCKS * sizeof(int) },

  { SMEM_MEMORY_BARRIER_LOCATION,
    sizeof(uint32) },

  { SMEM_INVALID,
    0 }
};

/* Initialize the global SMEM Init info stuct 
 */
smem_init_info_type smem_init_info =
                           {
                             SMEM_INVALID_ADDR,   /* *smem_heap_info */
                             SMEM_INVALID_ADDR,   /* *smem_allocation_table */
                             smem_buf_data_tbl,   /* *smem_buf_data_tbl */
                             SMEM_INVALID_ADDR,   /* *smem_data_base_addr */
                             SMEM_INVALID_ADDR,   /* *smem_phys_base_addr */
                             0,                   /* smem_data_size */
                             SMEM_INVALID_ADDR,   /* *smem_memory_barrier */
                             SMEM_UNINITIALIZED,  /* smem_state */
                           };
/*----------------------------------------------------------------------
 * External Region's Heap Allocation - tracks heap allocation from external
 * memory region, specified by API smem_set_ext_mem_region
 *----------------------------------------------------------------------*/
smem_ext_heap_info_type smem_ext_heap_info;

/** Defines the HWIO base address use by spinlocks */
uint8 *smem_hw_mutex_base_ptr = NULL;

/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_alloc_static
                                                                             
DESCRIPTION   Request a pointer to a static buffer in shared memory.  This
              function is designed to be callable at all times, and should
              never ERR_FATAL.

ARGUMENTS     smem_type     Type of memory to get a pointer for.  This 
                            function will only service static SMEM buffers.
              buf_size      Size of the buffer requested.  If the incorrect
                            size is provided, this request will fail.
                  
DEPENDENCIES  SMEM_HW_RESET_DETECT must be the second allocation in SMEM

RETURN VALUE  NULL if an error occurred.  Otherwise, pointer to the 
              requested buffer.
  
SIDE EFFECTS  If not already set, smem_data_base_addr will be initialized.
===========================================================================*/
static void *smem_alloc_static
(
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  uint32 tbl_idx;

  smem_set_base_addr();
  smem_set_spinlock_base_addr();
  
  /*------------------------------------------------------------
   * On the MODEM Processor, SMEM_HW_RESET_DETECT may be queried 
   * at times when memory is corrupted.  
   * Since we cannot gaurentee that smem_buf_tbl is not also 
   * corrupted, we must calculate the address using preprocessor 
   * resources.
   * Note:  This will not work if the MODEM processor needs to
   *        use virtual addressing for SMEM
   *------------------------------------------------------------*/
  if( smem_type == SMEM_HW_RESET_DETECT )
  {
    if( buf_size != sizeof(smem_hw_reset_id_type) )
    {
      return NULL;
    }
    
    return (void *)
      ( smem_init_info.smem_data_base_addr +
        SMEM_MEM_FIRST_SIZE_BYTES +
        SMEM_VERSION_INFO_SIZE * sizeof(uint32) +
        sizeof(smem_heap_info_type)  +
        SMEM_ALLOCATION_TABLE_SIZE * sizeof(smem_allocation_table_type));
  }
  
  for( tbl_idx = 0; 
       (smem_init_info.smem_buf_data_tbl[tbl_idx].tag != SMEM_INVALID &&
        smem_init_info.smem_buf_data_tbl[tbl_idx].tag != smem_type );
       tbl_idx++ )
  {
    /* Loop until we find the structure with the appropriate tag. */
  }

  if( smem_init_info.smem_buf_data_tbl[tbl_idx].tag != smem_type ||
      smem_init_info.smem_buf_data_tbl[tbl_idx].size != buf_size )
  {
    return NULL;
  }

  return (void *) ( smem_init_info.smem_data_base_addr +
                    smem_get_offset( smem_type ) );
}/* smem_alloc_static */

/*===========================================================================
FUNCTION      smem_alloc_dynamic
                                                                             
DESCRIPTION   Request a pointer to a dynamic item buffer in shared memory or 
              pre-initialized external memory. This function returns the 
              virtual address of the allocated memory item. If the memory has
              been already allocated, this function ignores the mem_bank
              parameter and returns address of allocated item, otherwise it
              allocates item from the mem_bank specified by that argument.
              This function does not handle statically allocated item types.

ARGUMENTS     smem_type     Type of memory to get a pointer for.  This 
                            function will not service static item types.
              buf_size      Size of the buffer requested.  If the incorrect
                            size is provided, this request will fail.
              mem_bank      Memory bank from which new memory is to be 
                            allocated.
                            0 - SMEM
                            1 - external memory (used for RPM)

                  
DEPENDENCIES  smem_set_ext_mem_region() must have been caled before trying to
              allocate items from external memory.

RETURN VALUE  NULL if an error occurred.  Otherwise, pointer to the 
              requested buffer.
  
SIDE EFFECTS  If not already set, smem_data_base_addr will be initialized.
===========================================================================*/
static void *smem_alloc_dynamic
(
  smem_mem_type         smem_type,
  uint32                buf_size,
  uint32                mem_bank
)
{
  uint32  remaining;
  uint32  offset;
  void   *result;

  /* We do not support allocation of static smem types, these item types
   * should have been dealt with by the caller of this function */
  ASSERT( (uint32) smem_type > (uint32) SMEM_LAST_FIXED_BUFFER); 

  /* call smem_init if not initialized yet */
  if( smem_init_info.smem_state == SMEM_UNINITIALIZED )   
  {
    smem_init();
  }

  /* Do not service invalid types */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if ( (uint32) smem_type > (uint32) SMEM_MEM_LAST )
  {
    result = NULL;
    ERR_FATAL( "Invalid SMEM memory type, %d, requested.", smem_type, 0, 0 );
  }

  /* buffer size topped off to a 64 bit alignment */
  buf_size = SMEM_ALIGN_SZ(buf_size);

  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );
  
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  if( smem_init_info.smem_allocation_table[smem_type].allocated == 
      SMEM_ALLOCATION_ENTRY_ALLOCATED )
  {
    /** If memory has already been allocated, we ignore the mem_bank param
     * and calculate the allocated item's address based on what is in the
     * smem_allocation_tbl */

    /* Check for valid buf_size */
    if( buf_size == smem_init_info.smem_allocation_table[smem_type].size )
    {
      /* If Memory allocated from external memory region and not SMEM */
      if( smem_init_info.smem_allocation_table[smem_type].base_ext & 
           (~SMEM_ALLOC_TBL_RESERVED_MASK) ) 
      {
        result = (void*) ( (uint32)smem_ext_heap_info.base_virt + 
              smem_init_info.smem_allocation_table[smem_type].offset );
      }
      else
      {
        result = smem_init_info.smem_data_base_addr + 
                 smem_init_info.smem_allocation_table[smem_type].offset;
      }
    }
    else
    {
      result = NULL;
      SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
      smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

      ERR_FATAL( "Invalid SMEM buffer size requested for memory %d.",
                 smem_type, 0, 0 );
    }
  }
  else
  {
    /* Item not allocated, do it here */
    if(mem_bank == 0)
    {
      remaining = smem_init_info.smem_heap_info->heap_remaining;
    }
    else
    {
      remaining = smem_ext_heap_info.heap_remaining;
    }

    if( buf_size <= remaining )
    {
      if(mem_bank == 0)
      {
        offset = smem_init_info.smem_heap_info->free_offset;

        /* Update heap info */
        smem_init_info.smem_heap_info->free_offset    = offset + buf_size;
        smem_init_info.smem_heap_info->heap_remaining = remaining - buf_size;

      }
      else
      {
        offset = smem_ext_heap_info.free_offset;

        /* Update heap info */
        smem_ext_heap_info.free_offset    = offset + buf_size;
        smem_ext_heap_info.heap_remaining = remaining - buf_size;

        /* Set the base_addr field of the smem_allocation_tbl entry */
        smem_init_info.smem_allocation_table[smem_type].base_ext  |= 
                                       (uint32)smem_ext_heap_info.base;      
      }

      /* Update allocation table entry */
      smem_init_info.smem_allocation_table[smem_type].offset  = (uint32) offset;
      smem_init_info.smem_allocation_table[smem_type].size      = buf_size;
      smem_init_info.smem_allocation_table[smem_type].allocated = 
                                              SMEM_ALLOCATION_ENTRY_ALLOCATED;

      if(mem_bank == 0)
      {
        result = smem_init_info.smem_data_base_addr + offset;
      }
      else
      {
        result = (void*) ( (uint32)smem_ext_heap_info.base_virt +
                       smem_init_info.smem_allocation_table[smem_type].offset );
      }
    }      
    else
    {
      result = NULL;
      SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
      smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

      ERR_FATAL( "Out of shared memory have %d requested %d for %d",
                 remaining, buf_size, smem_type );
    }

    SMEM_MEMORY_BARRIER();
  }

  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

  return result;
} /* smem_alloc_dynamic */

/*===========================================================================
  FUNCTION  smem_set_heap_info
===========================================================================*/
/**
  Sets the address for the heap info structure in SMEM.

  @return
  none
  
  @dependencies
  SMEM base virtual address should be mapped before calling.
*/
/*=========================================================================*/
static void smem_set_heap_info( void )
{
  if (smem_init_info.smem_heap_info == SMEM_INVALID_ADDR)
  {
    if (smem_init_info.smem_data_base_addr != SMEM_INVALID_ADDR)
    {
      smem_init_info.smem_heap_info = (smem_heap_info_type *)
        (smem_init_info.smem_data_base_addr + smem_get_offset(SMEM_HEAP_INFO));
    }
  }
}

/*===========================================================================

                      INTERNAL UTIL FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_get_offset
                                                                             
DESCRIPTION   Returns the offset to one of the "fixed" memory buffers.  This
              function uses the local smem_buf_data_tbl, and does not touch
              the actual allocation table in SMEM.

ARGUMENTS     tag     Type of memory to get a pointer for.
                  
DEPENDENCIES  None
      
RETURN VALUE  Return offset to the requested buffer, otherwise fatal error.
  
SIDE EFFECTS  None
===========================================================================*/
uint32 smem_get_offset( smem_mem_type tag )
{
  uint32 idx;
  uint32 offset;

  /* Search table for tag */
  offset = 0;

  for( idx = 0; (smem_init_info.smem_buf_data_tbl[idx].tag != SMEM_INVALID &&
                 smem_init_info.smem_buf_data_tbl[idx].tag != tag ); idx++ )
  {
    offset += smem_init_info.smem_buf_data_tbl[idx].size;
  }

  if( smem_buf_data_tbl[idx].tag != tag )
  {
    ERR_FATAL( "Invalid memory type %d", (uint32)tag, 0, 0 );
  }

  return offset;
} /* smem_get_offset */

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
uint32 smem_get_static_size( smem_mem_type tag )
{
  uint32 buf_tbl_idx;

  /* Must find the index within smem_buf_data_tbl for the correct info. */
  for( buf_tbl_idx = 0; buf_tbl_idx <= SMEM_LAST_FIXED_BUFFER && 
       smem_init_info.smem_buf_data_tbl[buf_tbl_idx].tag != tag;
       buf_tbl_idx++ )
  {
    /* Loop until we find the structure with the appropriate tag. */
  }
  ASSERT(smem_init_info.smem_buf_data_tbl[buf_tbl_idx].tag != SMEM_INVALID);

  return smem_init_info.smem_buf_data_tbl[buf_tbl_idx].size;
}

/*===========================================================================
FUNCTION      smem_set_base_addr
                                                                             
DESCRIPTION   Initializes the smem_data_base_addr pointer and smem_data_size.  
              This function must be called before any smem operations are 
              performed.

ARGUMENTS     None
                  
DEPENDENCIES  Even though calls to this function are gated by the value
              of smem_data_base_addr, this function must still be 
              archtectured to handle  simultaneous calls from multiple
              reads.
 
              This function may ERROR FATAL when the OS specific function
              cannot create the virtual mapping to the physical address,
              since all SMEM pointers depend on properly setting the
              base address.
      
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
void smem_set_base_addr( void )
{
  if (smem_init_info.smem_data_base_addr == SMEM_INVALID_ADDR) 
  {
    smem_init_info.smem_data_base_addr = smem_map_memory_va(
                                  smem_get_phys_base_addr(),
                                  smem_get_size());
  }
}/* smem_set_base_addr */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_init
===========================================================================*/
/**
@brief        Initialize the smem memory allocation structures.

@dependencies Shared memory must have been cleared and
              initialized by the first system bootloader, usually running
              on the modem processor.  Even though calls to this function
              are gated by the smem_init_finished flag, this function
              must still be architectured to handle  simultaneous calls
              from multiple threads.

@return       None

@sideeffect   None
*/
/*=========================================================================*/
void smem_init( void )
{
  /* Prevent this function from doing anything if it has already been called.
   * Caller must call each initialization functions serially from a single 
   * initialization thread, so there is no need to be thread-safe.  
   * But we need to protect against the critical sections being re-init'ed. */
  if( smem_init_info.smem_state == SMEM_INITIALIZED )
  {
    return;
  }

  smem_set_base_addr();

  smem_set_spinlock_base_addr();

  smem_init_info.smem_allocation_table = (smem_allocation_table_type *)
    ( smem_init_info.smem_data_base_addr + 
      smem_get_offset( SMEM_ALLOCATION_TABLE ) );


  smem_set_heap_info();


  smem_init_info.smem_memory_barrier = (uint32 *)
    ( smem_init_info.smem_data_base_addr + 
      smem_get_offset( SMEM_MEMORY_BARRIER_LOCATION ) );

  /*------------------------------------------------------------
   * The heap info should have already been intialized
   *------------------------------------------------------------*/
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  if( smem_init_info.smem_heap_info->initialized != SMEM_HEAP_INFO_INIT )
  {
    ERR_FATAL( "Can't initialize heap info", 0, 0, 0 );
  }
  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();

  /*------------------------------------------------------------
   * Validate that the allocation table is large enough.
   *------------------------------------------------------------*/
  if( SMEM_ALLOCATION_TABLE_SIZE < SMEM_MEM_LAST )
  {
    ERR_FATAL("The smem allocation table is too small",0,0,0);
  }

  /*------------------------------------------------------------
   Initialize spin locks
  *------------------------------------------------------------*/

#ifndef SMEM_RPM_PROC
  /* Set spinlock function pointer based on chip info */
  smem_set_spin_lock_api();
#endif

  smem_spin_lock_init((volatile uint32 *)( smem_init_info.smem_data_base_addr +
                                        smem_get_offset(SMEM_SPINLOCK_ARRAY) ),
                       NULL,
                       SPINLOCK_NUM_LOCKS );

#if !defined(SMEM_RPM_PROC) && !defined(SMEM_BOOT_IMAGE) && !defined(SMEM_TZ_IMAGE)
  /* Set up partition structures.  Unused and unnecessary on SBL, TZ, RPM. */
  smem_partition_init();
#endif

  /*------------------------------------------------------------
   * Set SMEM version info and verify that everyone registered so
   * far is compatible.
   * Each call to smem_init verifies that all previous calls are
   * compatible. So, the last call to smem_init will be certain to
   * detect if anyone is incompatible.
   *------------------------------------------------------------*/

  SMEM_MEMORY_BARRIER();

  /* Moved before the version check/set in order to fulfill that function's
   * dependency that smem has been initialized (and indicate so). */
  smem_init_info.smem_state = SMEM_INITIALIZED;

  if (smem_version_set(SMEM_VERSION_INFO, SMEM_VERSION_ID, 
                       SMEM_MAJOR_VERSION_MASK) == FALSE)
  {
    ERR_FATAL("smem_init: major version (%d) does not match all procs!",
        SMEM_VERSION_ID, 0, 0);
  }

  /*-------------------------------------------------------------------
   * For RPM clients, it is required that stream channel and FIFO
   * be allocated from dedicated piece of RPM memory. Initialize that
   * memory region here 
   * -----------------------------------------------------------------*/
  smem_set_ext_mem_region((void*) SMEM_RPM_MSG_RAM_BASE_PHYS, 
                        SMEM_RPM_MSG_RAM_SIZE);
}/* smem_init */

/*===========================================================================
FUNCTION      smem_alloc
===========================================================================*/
/**
@brief        Request a pointer to an item in shared/external memory. If the
              space for the item has been allocated in either shared/external 
              memory, return the pointer to that location, otherwise allocate
              buffer for the specified item from the shared memory.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[in]    buf_size      Size of the buffer requested.  For pre-allocated
                            buffers, an ERR_FATAL will occur if the buffer
                            size requested does not match the size of the
                            existing buffer.  If the buf_size
                            is not 64-bit aligned, this function will top
                            the size off to an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet. Otherwise, either
             return a valid SMEM address to the requested buffer or error fatal

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_alloc
(
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  /* return NULL if the shared memory is not zero initialized */
  if( smem_init_info.smem_state == SMEM_PRE_BOOT_INIT )
  {
    return NULL;
  }
  
  /* Hand static types to smem_alloc_static */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if( (uint32) smem_type <= (uint32) SMEM_LAST_FIXED_BUFFER )
  {
    return smem_alloc_static( smem_type, buf_size );
  }
  /* Call the internal alloc API for dynamic smem items */
  return smem_alloc_dynamic(smem_type, buf_size, 0);
} /* smem_alloc */

/*===========================================================================
FUNCTION      smem_alloc_ext_mem
===========================================================================*/
/**
@brief        Request a pointer to an item in shared/external memory. If the
              space for the item has been allocated in either shared/external 
              memory, return the pointer to that location, otherwise allocate
              buffer for the specified item from the external memory.
              If the requested smem_type is one of the statically allocated 
              items, return NULL.

@param[in]    smem_type     Type of memory to get a pointer for. Returns NULL
                            if a static SMEM item is requested.
@param[in]    buf_size      Size of the buffer requested.  For pre-allocated
                            buffers, an ERR_FATAL will occur if the buffer
                            size requested does not match the size of the
                            existing buffer. If the buf_size is not 64-bit 
                            aligned, this function will top the size off to
                            an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet or the requested
             item is a statically allocated one. Otherwise, either return a 
             valid SMEM address to the requested buffer or error fatal

@sideeffect  Uses spinlocks for exclusive access to the shared memory heap.
             This function should only be called from a single processor (RPM)
*/
/*=========================================================================*/
void* smem_alloc_ext_mem
(
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  /* Return NULL if a statically allocated item is requested */
  if( (uint32) smem_type <= (uint32) SMEM_LAST_FIXED_BUFFER )
  {
    return NULL;
  }
  /* Allocate the dynamic item */
  return smem_alloc_dynamic(smem_type, buf_size, 1);
}

/* The function smem_alloc_rpm is supposed to be used only by the RPM
 * processor to set SMEM drivers to use RPM ram, and to allocate port
 * from that memory region with appropriate XPU settings.
 */
#ifdef SMEM_RPM_PROC
/*===========================================================================
FUNCTION      smem_alloc_rpm
===========================================================================*/
/**
@brief        Request a pointer to an item in RPM MSG RAM

@param[in]    smem_host_type The other host for the SMEM region
@param[in]    smem_type      Type of memory to get a pointer for. Returns NULL
                             if a static SMEM item is requested.
@param[in]    buf_size       Size of the buffer requested.  For pre-allocated
                             buffers, an ERR_FATAL will occur if the buffer
                             size requested does not match the size of the
                             existing buffer. If the buf_size is not 64-bit 
                             aligned, this function will top the size off to
                             an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet or the requested
             item is a statically allocated one. Otherwise, either return a 
             valid SMEM address to the requested buffer or error fatal

@sideeffect  None
*/
/*=========================================================================*/
void* smem_alloc_rpm
(
  smem_host_type        other_host,
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  void* result;
  /* Return NULL if a statically allocated item is requested */
  if( (uint32) smem_type <= (uint32) SMEM_LAST_FIXED_BUFFER )
  {
    return NULL;
  }

  /* Call RPM API to allocate memory and set proper XPU access */
  result = message_ram_malloc(other_host, buf_size );
  
  /* Validate the returned pointer */
  if( ((uint32)result < (uint32)smem_ext_heap_info.base_virt) ||
      ((uint32)result + buf_size > 
       (uint32)smem_ext_heap_info.base_virt + 
       smem_ext_heap_info.heap_size) )
  {
    ERR_FATAL("smem_alloc_rpm: Invalid pointer returned by messge_ram_malloc!",
               0, 0, 0 );
  }

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  /* Check to make sure item is not being allocated twice */
  ASSERT(smem_init_info.smem_allocation_table[smem_type].allocated !=
      SMEM_ALLOCATION_ENTRY_ALLOCATED);
  
  /* Update the allocation table */
  /* Set the base_addr field of the smem_allocation_tbl entry */
  smem_init_info.smem_allocation_table[smem_type].base_ext  |= 
                                  (uint32)smem_ext_heap_info.base;      
  /* Update allocation table entry */
  smem_init_info.smem_allocation_table[smem_type].offset  = 
                         (uint32)result - (uint32)smem_ext_heap_info.base_virt;
  smem_init_info.smem_allocation_table[smem_type].size      = buf_size;
  smem_init_info.smem_allocation_table[smem_type].allocated = 
                                              SMEM_ALLOCATION_ENTRY_ALLOCATED;

  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
  return result;
}

#endif //SMEM_RPM_PROC

/*===========================================================================
FUNCTION      smem_set_ext_mem_region
===========================================================================*/
/**
@brief        Sets the External Memory region from which smem_alloc_ext_mem
              allocates the items.

              For Internal use only (Introduced for RPM clients).

@param[in]    ext_mem_region External memory region base address.
@param[in]    size           Size of the external memory region.

@dependencies Any allocation from the external memory region should happen
              after this function has been called.

@return       0  Success
             -1  Failure

@sideeffect   This function is intended for internal use only.
*/
/*=========================================================================*/
int32 smem_set_ext_mem_region(void* ext_mem_region, uint32 size)
{
  if(ext_mem_region == NULL || size == 0 || 
      ((uint32)ext_mem_region & SMEM_WORD_ALIGN_MASK))
  {
    return -1;
  }
  smem_ext_heap_info.base = ext_mem_region;
  smem_ext_heap_info.base_virt = smem_map_memory_va(ext_mem_region, size);
  smem_ext_heap_info.heap_remaining = size;
  smem_ext_heap_info.heap_size = size;
  smem_ext_heap_info.free_offset = 0;
  return 0;
}

/*===========================================================================
FUNCTION      smem_get_addr
===========================================================================*/
/**
@brief        Request a pointer to an already allocated buffer in shared
              memory. Returns the address and size of the allocated buffer.

              Newly-allocated shared memory buffers, which have never been
              allocated on any processor, are guaranteed to be zeroed.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[out]   buf_size      (Output) Size of buffer allocated in shared
                            memory.

@dependencies The buffer must already have been allocated.

@return       Pointer to the requested buffer, or NULL if the buffer has
              not been allocated.

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void *smem_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
)
{
  void  *result;
  
  /* Hand static (fixed) smem types to smem_alloc_static. */
  if( (uint32) smem_type <= (uint32) SMEM_LAST_FIXED_BUFFER )
  {
    *buf_size = smem_get_static_size(smem_type);
    return smem_alloc_static( 
                          smem_type,
                          *buf_size );
  }

  if( smem_init_info.smem_state == SMEM_UNINITIALIZED ) 
  {
    smem_init();
  }

  /* Do not service invalid types */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if ( (uint32) smem_type > (uint32) SMEM_MEM_LAST )
  {
    ERR_FATAL( "Invalid SMEM memory type, %d, requested.", smem_type, 0, 0 );
  }

  /* Even though this is read only operation, use spinlock to ensure that
     the state of the table is consistent */
  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  if( smem_init_info.smem_allocation_table[smem_type].allocated ==
      SMEM_ALLOCATION_ENTRY_ALLOCATED )
  {
    *buf_size = smem_init_info.smem_allocation_table[smem_type].size;
    /* If Memory allocated from external memory region and not SMEM */
    if( smem_init_info.smem_allocation_table[smem_type].base_ext & 
         (~SMEM_ALLOC_TBL_RESERVED_MASK) ) 
    {
      result = (void*) ( (uint32)smem_ext_heap_info.base_virt + 
            smem_init_info.smem_allocation_table[smem_type].offset );
    }
    else
    {
      result = smem_init_info.smem_data_base_addr + 
               smem_init_info.smem_allocation_table[smem_type].offset;
    }
  }
  else
  {
    /* object not allocated yet */
    *buf_size = 0;
    result = NULL;
  }


  /* Make sure the shared memory item fits in shared memory */
  if( result != NULL && 
      (smem_init_info.smem_allocation_table[smem_type].base_ext == 0))
  {
    char *smem_data_end_addr = smem_init_info.smem_data_base_addr +
                               smem_get_size();
    if( ((char *)result < smem_init_info.smem_data_base_addr) ||
        ((char *)result >= smem_data_end_addr) ||
        (((char *)result + *buf_size) >= smem_data_end_addr) )
    {
      /* The shared memory heap is corrupt, or this shared memory item was
       * allocated by a processor with a different shared memory size. */
      *buf_size = 0;
      result = NULL;
    }
  }

  SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );
  return result;
} /* smem_get_addr */

/*===========================================================================
FUNCTION      smem_free
===========================================================================*/
/**
@brief        Free a pointer in shared memory.

              This function is not actually implemented, but if it were, it
              would allow other functions to reuse the memory previously
              allocated in shared memory.

@param[in]    addr          Pointer in shared memory to free.

@dependencies
              - smem_init() must have been called on this processor.
              - The pointer must have been allocated using smem_alloc() or
                smem_alloc_anon() or obtained using smem_get_addr.

@return       None

@sideeffect   None
*/
/*=========================================================================*/
void smem_free( void *addr )
{
  /* nothing to do - full heap not implemented at this time */
}

/*===========================================================================
FUNCTION      smem_version_set
===========================================================================*/
/**
@brief        Sets the version number for this processor and a given object.

              The version number is compared against all previously set
              version numbers for this object.  So the last processor to
              register will check against all other processors.

@param[in]    type    the type of object being version checked.
                  must be between the #SMEM_VERSION_FIRST and
                  #SMEM_VERSION_LAST in the #smem_mem_type enum in smem.h,
                  unless it is of type SMEM_VERSION_INFO (for internal use).
 
@param[in]    version   the local version number for this memory
                 object

@param[in]    mask    The client may specify a mask in case the 32-bit #version
                  argument contains multiple protocols' versions, and not all
                  of them need to be checked.
                  
@dependencies smem_init() must have been called on this processor.
      
@return       boolean - true if the version number of the local processor
                 matches all previously registered versions for this object
                 type, false if there is a mismatch.

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
boolean smem_version_set
(
 smem_mem_type type,
 uint32 version,
 uint32 mask
)
{
  uint32 idx;
  uint32 my_version;
  uint32 other_version;
  boolean match = TRUE;
  uint32 *version_array;

  if((type != SMEM_VERSION_INFO) && ((type < SMEM_VERSION_FIRST) ||
                                      (type > SMEM_VERSION_LAST)))
  {
    match = FALSE;
  }
  else
  {
    /* Should work since all version type arrays should be same size. */
    version_array = (uint32 *)smem_alloc(type, 
              SMEM_VERSION_INFO_SIZE * sizeof(uint32));
    if (version_array == NULL)
    {
      ERR_FATAL("smem_version_set: Unable to allocate version array: type %d", 
                type, 0, 0);
    }
    my_version = (version & mask);
    SMEM_ACCESS_CONFIGURE_SHARED_RAM();
    version_array[SMEM_VERSION_INFO_OFFSET] |= my_version;
    
    SMEM_MEMORY_BARRIER();
    
    for(idx = 0; idx < SMEM_VERSION_INFO_SIZE; idx++)
    {
      other_version = version_array[idx] & mask;

      /* If a version exists then compare the major numbers, determined by the
       * value of mask. */
      if((other_version != 0) && (other_version != my_version) )
      {
        match = FALSE;
        idx = SMEM_VERSION_INFO_SIZE;  /* jump out of the loop */
      }
    }
    SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();
  }

  return match;
}/* smem_version_set */

/*===========================================================================
  FUNCTION  smem_generic_memory_barrier
===========================================================================*/
/**
  This function implements the generic memory barrier by performing a write
  and read to uncached/unbuffered location in SMEM.
 
  This function can be used when memory barrier is not implemented as a system
  function for a given processor, but it is not guaranteed for any specific
  architecture.

  @return
  None.

  @dependencies
  Variable smem_memory_barrier should be configured as uncached/unbuffered.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_generic_memory_barrier( void )
{
  static uint32 *memory_barrier_ptr = NULL;
  
  if(!memory_barrier_ptr)
  {
    memory_barrier_ptr = smem_init_info.smem_memory_barrier;
  }

  *memory_barrier_ptr = 0;

  while(0 != *memory_barrier_ptr);
}

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
void smem_set_spinlock_base_addr( void )
{
  /* Initialize the spinlocks region base address */
  if( smem_hw_mutex_base_ptr == NULL )
  {
    smem_hw_mutex_base_ptr = ( uint8 * )smem_map_spinlock_region_va();
  }
}

/*===========================================================================
  FUNCTION  smem_get_base_addr
===========================================================================*/
/**
  Returns the virtual base address of SMEM.

  @return
  A void pointer to the base of SMEM.

  @sideeffects
  Sets the virtual base address of SMEM if not previously set.
*/
/*=========================================================================*/
char *smem_get_base_addr( void )
{
  smem_set_base_addr();
  return smem_init_info.smem_data_base_addr;
}

/*===========================================================================
  FUNCTION  smem_get_phys_base_addr
===========================================================================*/
/**
  Returns the physical base address of SMEM.

  @return
  The physical address of the base of the SMEM region.

  @sideeffects
  Saves the physical base address of SMEM if not previously set.
*/
/*=========================================================================*/
char *smem_get_phys_base_addr( void )
{
  /* Initialize the physical base address of shared memory */
  if (smem_init_info.smem_phys_base_addr == SMEM_INVALID_ADDR) 
  {
    smem_init_info.smem_phys_base_addr = smem_ext_get_smem_base();
  }
  return smem_init_info.smem_phys_base_addr;
}

/*===========================================================================
  FUNCTION  smem_get_size
===========================================================================*/
/**
  Returns the size of SMEM.

  @return
  The size of SMEM.

  @sideeffects
  Saves the size of SMEM if not previously set.
*/
/*=========================================================================*/
uint32 smem_get_size( void )
{
  /* Initialize the size of shared memory */
  if (smem_init_info.smem_data_size == 0) 
  {
    smem_init_info.smem_data_size = smem_ext_get_smem_size();
  }
  return smem_init_info.smem_data_size;
}

/*===========================================================================
  FUNCTION  smem_get_heap_info
===========================================================================*/
/**
  Returns a direct pointer to the heap info structure in SMEM.

  @return
  A pointer to the heap info struct.

  @sideeffects
  Sets the global heap info structure pointer if not previously set.
  
  @dependencies
  SMEM base virtual address should be mapped before calling.
*/
/*=========================================================================*/
volatile smem_heap_info_type *smem_get_heap_info( void )
{
  smem_set_heap_info();
  return smem_init_info.smem_heap_info;
}

/*===========================================================================
  FUNCTION  smem_alloc_add_table_entry
===========================================================================*/
/**
  Add an entry to the SMEM allocation table without allocating any memory and 
  without updating the heap info structure.

  Caller must ensure that there is not currently an allocation at this location.

  This function is designed for use with the backwards compatibility mode 
  used during the roll out of SMEM partitioning.

  @param[in]    smem_type   SMEM ID to use for the allocation
  @param[in]    buffer      pointer to the existing allocation
  @param[in]    size        size of the existing allocation

  @return
  None.

  @dependencies
  SMEM must be initialized before calling.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_alloc_add_table_entry
( 
  smem_mem_type smem_type, 
  void * buffer, 
  uint32 size 
)
{
  uint32 offset = (uint32)buffer - (uint32)smem_init_info.smem_data_base_addr;

  if (smem_type <= SMEM_MEM_LAST)
  {
    /* Enter critical section */
    smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

    SMEM_ACCESS_CONFIGURE_SHARED_RAM();

    /* Update the allocation table */
    smem_init_info.smem_allocation_table[smem_type].offset    = offset;
    smem_init_info.smem_allocation_table[smem_type].size      = size;
    smem_init_info.smem_allocation_table[smem_type].allocated = 
                                                SMEM_ALLOCATION_ENTRY_ALLOCATED;

    /* Modify the heap info.  LA uses this field to detect any SMEM changes
     * and if unmodified, LA will not scan for new SMD ports. */
    smem_init_info.smem_heap_info->free_offset    += 8;
    smem_init_info.smem_heap_info->heap_remaining -= 8;

    SMEM_MEMORY_BARRIER();

    SMEM_ACCESS_CONFIGURE_EXT_SHARED_RAM();

    /* End critical section */
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
  }
}

