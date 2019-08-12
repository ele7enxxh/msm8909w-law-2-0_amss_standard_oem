#ifndef SMEM_PARTITION_INTERNAL_H
#define SMEM_PARTITION_INTERNAL_H

/*===========================================================================

                Shared Memory Partition Internal Header File


 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_partition.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/13   pa      Added canary and padding size to allocation header.
                   Changed documentation and naming from 'upwards' to 
                   'uncached' and 'downwards' to 'cached' to avoid confusion.
                   Removed list references and smem_partition_item_info_type.
04/24/13   pa      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_type.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/** Partition Identifier: "$PRT" */
#define SMEM_PARTITION_HEADER_ID 0x54525024

#define SMEM_READ_SMEM_4(addr)       (*(uint32*)(addr))
#define SMEM_READ_SMEM_2(addr)       (*(uint16*)(addr))
#define SMEM_WRITE_SMEM_4(addr, val) ((*(uint32*)(addr)) = (uint32)(val))
#define SMEM_WRITE_SMEM_2(addr, val) ((*(uint16*)(addr)) = (uint16)(val))

/** Return the nearest multiple of y at or above x */
#define ROUND_UP(x,y)  ((((x) + (y) - 1) / (y)) * (y))

/** Number of bytes of padding based on cacheline sizes */
#define SMEM_PARTITION_ITEM_PADDING(size_cl) \
      (ROUND_UP(sizeof(smem_partition_allocation_header_type), (size_cl)) - \
                sizeof(smem_partition_allocation_header_type))

/** Maximum allowed number of partitions */
#define SMEM_MAX_PARTITIONS 7

/** Canary placed in the allocation header for each item */
#define SMEM_ALLOC_HDR_CANARY 0xa5a5

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/

/** This structure is the header for the SMEM partition, and sits at the base
 ** of every partition in SMEM. */
typedef struct 
{
  /** Identifier magic number */
  uint32 identifier;

  /** IDs for the 2 subsystems which have access to this partition.
   ** Order does not matter.
   ** Use uint16, rather than enum type, to ensure size. */
  uint16 host0;
  uint16 host1;

  /** Size of the partition, in bytes, not including the exclusion ranges */
  uint32 size;

  /** Offset of the byte following the last uncached allocation */
  uint32 offset_free_uncached;

  /** Offset of the byte following the last cached allocation */
  uint32 offset_free_cached;

  /** Reserved fields */
  uint32 reserved[3];

} smem_partition_header_type;

/** This structure is the header for each SMEM allocation in the 
 ** protected partition. */
typedef struct 
{
  /** Canary value.  Can be used to detect corruption.
   ** Set to 0xa5a5 */
  uint16 canary;

  /** SMEM item ID.
   ** Use uint16 here, rather than enum, to ensure size. */
  uint16 smem_type;

  /** Size of the allocated item in SMEM, in bytes. 
   ** Includes padding_data bytes. */
  uint32 size;

  /** Size of data padding, in bytes.  
   ** If the requested size was padded (rounded up), this is the
   ** amount it was rounded. */
  uint16 padding_data;

  /** Size of header padding, in bytes. */
  uint16 padding_header;

  /** Reserved fields */
  uint32 reserved[1];

} smem_partition_allocation_header_type;

/** This structure holds debug info for the SMEM partition code */
typedef struct
{
  /** Number of allocations from the uncached side that are known to this proc.
   ** This may be stale if we have not done a scan of SMEM since the remote
   ** endpoint last allocated an item. */
  uint32 num_alloc_uncached;

  /** Number of allocations from the cached side that are known to this proc.
   ** This may be stale if we have not done a scan of SMEM since the remote
   ** endpoint last allocated an item. */
  uint32 num_alloc_cached;

  /** Number of times this endpoint has detected one or more new SMEM items
   ** allocated by the remote endpoint and performed a scan */
  uint32 num_scan_uncached;

  /** Number of times this endpoint has detected one or more new SMEM items
   ** allocated by the remote endpoint and performed a scan */
  uint32 num_scan_cached;

} smem_partition_debug_info_type;

/** This structure describes the state of the SMEM partition.  
 ** This resides in local memory. */
typedef struct 
{
  /** Pointer to the header for the partition which sits at the base */
  smem_partition_header_type * header;

  /** Size of the partition, in bytes, not including the exclusion ranges */
  uint32 size;

  /** Lowest common multiple of the cacheline sizes for this edge */
  uint32 size_cacheline;

  /** Offset of the byte following the last allocation from the uncached side.
   ** Compared against the value in SMEM to determine if the remote endpoint
   ** has allocated from the heap. */
  uint32 offset_free_uncached;

  /** Offset of the byte following the last allocation from the cached side.
   ** Compared against the value in SMEM to determine if the remote endpoint
   ** has allocated from the heap. */
  uint32 offset_free_cached;

  /** Debug info */
  smem_partition_debug_info_type debug_info;

} smem_partition_info_type;

/*===========================================================================
                            FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smem_partition_init
===========================================================================*/
/**
  Initializes the SMEM partitioning structures.

  This function checks for the existence of the SMEM table of contents and then
  reads it to determine partition locations and size.  Data structures are
  initialized to enable SMEM allocation from protected partitions.

  @return
  SMEM_STATUS_SUCCESS          - Initialization was successful
  SMEM_STATUS_FAILURE          - Initialization failed
*/
/*=========================================================================*/
int32 smem_partition_init(void);

#endif /* SMEM_PARTITION_H */
