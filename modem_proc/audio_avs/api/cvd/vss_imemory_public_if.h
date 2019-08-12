#ifndef __VSS_IMEMORY_PUBLIC_IF_H__
#define __VSS_IMEMORY_PUBLIC_IF_H__

/**
  @file vss_imemory_public_if.h
  @brief This file contains the APR API definitions for CVD shared memory
  management.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_imemory_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_imemory_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
04/12/12     HL    Initial version.

===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_imemory_public_if
@{ */

/****************************************************************************
 * MEMORY MAP/UNMAP APIS                                                    *
 ****************************************************************************/

/** @name Shared Memory Usage Guideline
  @latexonly \label{shmem_guideline} @endlatexonly
  @{ */

/**
  This section describes the guidelines to use shared memory for sharing data 
  between the client and CVD. 

  Before shared memory can be used, it must first be mapped with MVM. The 
  mapped memory will be accessible by all the sessions of all the CVD modules.
  The client must not map the same piece of memory twice with MVM.
  
  The client specifies the memory attributes in the memory mapping commands. 
  The attributes are summarized below:
  1. Cached or uncached – Indicates whether the memory is cached or uncached. 
  2. Cache line size if cached – Indicates the minimum amount of data in bytes 
     that can be affected by a single cache maintenance operation. 
  3. Access permission – Indicates the permission to access the memory after it
     is mapped, i.e. readable, writable. 
  4. Page alignment – Indicates the page frame alignment.
  5. Minimum native data type access width – Indicates the minimum native data
     type width in bits that can be accessed. 
  6. Maximum native data type access width – Indicates the maximum native data 
     type width in bits that can be accessed.

  Three types of shared memory are supported by CVD:
  1. Virtual memory. 
     Useful when the client and CVD both see a unified virtual address space,
     e.g. both the client and CVD are on the same processor. See 
     #VSS_IMEMORY_CMD_MAP_VIRTUAL on how to map virtual memory with CVD.
  2. Contiguous physical memory. 
     Useful when enough contiguous physical memory is available. See 
     #VSS_IMEMORY_CMD_MAP_PHYSICAL on how to map contiguous physical memory
     with CVD.
  3. Non-contiguous physical memory.
     Useful when enough contiguous memory is not available or not efficient 
     (in terms of memory utilization) to allocate, and multiple memory blocks
     are needed. See 
     #VSS_IMEMORY_CMD_MAP_PHYSICAL on how to map non-contiguous physical
     memory with CVD.

  CVD provides a handle to the client in response to the memory mapping 
  commands. The handle encapsulates the shared memory attributes. 

  After the memory is mapped, the client can use it for sharing data with CVD
  by providing the location and size of the data in shared memory as well as
  the handle returned by the memory mapping command. The location of the data
  is specified as a memory address or an address offset depending on the type
  of the shared memory used:
  1. Virtual memory. 
     The client specifies the virtual address to indicate the location of the 
     data. CVD will directly use the virtual address to access the memory.
  2. Contiguous physical memory. 
     The client specifies the physical address to indicate the location of the
     data. CVD will do physical to virtual address translation before accessing
     the memory.
  3. Non-contiguous physical memory.
     The client specifies an address offset to indicate the location of the 
     data. The offset is from the client’s starting virtual address of the 
     memory. It is possible to use an offset due to the fact that both the 
     client and CVD map non-contiguous physical memory to contiguous virtual
     memory. When CVD first maps the memory, it will obtain a virtual address,
     cvd_start_virt_addr. When an address offset is provided to indicate the 
     location of the data, CVD will use the cvd_start_virt_address + offset to
     access the shared memory.

  When specifying the location of the data, any memory address or address 
  offset provided to CVD must be aligned to the lcm_align address alignment and
  adhere to the memory attributes required for the region in which the memory 
  address or offset was mapped, where lcm_align = LCM( cache line size, maximum
  native data type access width ) and LCM = Least Common Multiple. Also, the 
  memory provided must be large enough to hold the data and must be a multiple
  of cache line size. For the memory provided, the client must not use the 
  subsequent padding bytes (if there are any) after the data for other purposes 
  as they can be affected by cache maintenance operations such as flushing.
*/

/** @} */ /* end_name_group Shared Memory Usage Guideline */


/** @brief Type definition for #vss_imemory_block_t.
*/
typedef struct vss_imemory_block_t vss_imemory_block_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying a single block of contiguous memory.
*/
struct vss_imemory_block_t
{
  uint64_t mem_address;
    /**< Base address of the memory block. The address is virtual for virtual
         memory and physical for physical memory. The address must be aligned
         to LCM( cache_line_size, page_align, max_data_width ), where the
         attributes are specified in #VSS_IMEMORY_CMD_MAP_VIRTUAL or
         #VSS_IMEMORY_CMD_MAP_PHYSICAL, and LCM = Least Common Multiple. */
  uint32_t mem_size;
    /**< Size in bytes of the memory block. The size must be multiple of 
         page_align, where page_align is specified in 
         #VSS_IMEMORY_CMD_MAP_VIRTUAL or #VSS_IMEMORY_CMD_MAP_PHYSICAL. */
}
#include "apr_pack_end.h"
;


/**
  Maps contiguous virtual memory with CVD. The mapped memory is accessible by
  all the sessions of all the CVD modules.

  @par Payload
  #vss_imemory_cmd_map_virtual_t

  @return
  #VSS_IMEMORY_RSP_MAP -- Success.\n
  #APRV2_IBASIC_RSP_RESULT -- Failure.

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to MVM address and the dst_port
  must be set to APR_NULL_V.
*/
#define VSS_IMEMORY_CMD_MAP_VIRTUAL ( 0x00011333 )

/** @brief Type definition for #vss_imemory_cmd_map_virtual_t.
*/
typedef struct vss_imemory_cmd_map_virtual_t vss_imemory_cmd_map_virtual_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for #VSS_IMEMORY_CMD_MAP_VIRTUAL.
*/
struct vss_imemory_cmd_map_virtual_t
{
  vss_imemory_block_t block;
    /**< Specifies a single block of contiguous virtual memory. */
  bool_t is_cached;
    /**< Indicates cached or uncached memory. Supported values:\n
         TRUE - Cached. */
  uint16_t cache_line_size;
    /**< Cache line size in bytes. Supported values:\n
         128. */
  uint32_t access_mask;
    /**< CVD's access permission to the memory while it is mapped. Supported
         values:\n
         bit 0 - If set, the memory is readable.\n
         bit 1 - If set, the memory is writable. */
  uint32_t page_align;
    /**< Page frame alignment in bytes. Supported values:\n
         4096. */
  uint8_t min_data_width;
    /**< Minimum native data type width in bits that can be accessed.
         Supported values:\n
         8. */
  uint8_t max_data_width;
    /**< Maximum native data type width in bits that can be accessed.
         Supported values:\n
         64. */
}
#include "apr_pack_end.h"
;


/**
  Maps physical memory with CVD. The mapped memory is accessible by all the
  sessions of all the CVD modules.

  @par Payload
  #vss_imemory_cmd_map_physical_t

  @return
  #VSS_IMEMORY_RSP_MAP -- Success.\n
  #APRV2_IBASIC_RSP_RESULT -- Failure.

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to MVM address and the dst_port
  must be set to APR_NULL_V.
  This command can be used for mapping either contiguous or non-contiguous 
  physical memory.

  Use Case 1: Maps contiguous physical memory. 
  The table_descriptor field in the command payload specifies the location and
  size of a table that contains information on a single memory block.

  Use Case 2: Maps non-contiguous physical memory.
  The table_descriptor field in the command payload specifies the location and
  size of a table that contains information on multiple memory blocks. The 
  order of the memory blocks provided to CVD must be the same as when the 
  client maps the blocks on its local processor.
*/
#define VSS_IMEMORY_CMD_MAP_PHYSICAL ( 0x00011334 )

/** @brief Type definition for #vss_imemory_table_descriptor_t.
*/
typedef struct vss_imemory_table_descriptor_t vss_imemory_table_descriptor_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying the location and size of a table that 
    contains information on memory blocks.
*/
struct vss_imemory_table_descriptor_t
{
  uint64_t mem_address;
    /**< Base physical address of the table. The address must be aligned
         to LCM( cache_line_size, page_align, max_data_width ), where the
         attributes are specified in #VSS_IMEMORY_CMD_MAP_PHYSICAL, and 
         LCM = Least Common Multiple. The table at the address must have
         the format specified by #vss_imemory_table_t. */
  uint32_t mem_size;
    /**< Size in bytes of the table. */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_imemory_table_t.
*/
typedef struct vss_imemory_table_t vss_imemory_table_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying the format of a table that contains
    information on memory blocks.
*/
struct vss_imemory_table_t
{
  vss_imemory_table_descriptor_t next_table_descriptor;
    /**< Specifies the next table. If there is no next table, set the size of
         the table to 0 and the table address is ignored. */
#if __STDC_VERSION__ >= 199901L
  vss_imemory_block_t blocks[];
    /**< Specifies one ore more memory blocks. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_imemory_cmd_map_physical_t.
*/
typedef struct vss_imemory_cmd_map_physical_t vss_imemory_cmd_map_physical_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for #VSS_IMEMORY_CMD_MAP_PHYSICAL.
*/
struct vss_imemory_cmd_map_physical_t
{
  vss_imemory_table_descriptor_t table_descriptor;
    /**< Specifies the location and size of a table that contains information
         on one or more memory blocks. */
  bool_t is_cached;
    /**< Indicates cached or uncached memory. Supported values:\n
         TRUE - Cached. */
  uint16_t cache_line_size;
    /**< Cache line size in bytes. Supported values:\n
         128. */
  uint32_t access_mask;
    /**< CVD's access permission to the memory while it is mapped. Supported
         values:\n
         bit 0 - If set, the memory is readable.\n
         bit 1 - If set, the memory is writable.\n */
  uint32_t page_align;
    /**< Page frame alignment in bytes. Supported values:\n
         4096. */
  uint8_t min_data_width;
    /**< Minimum native data type width in bits that can be accessed.
         Supported values:\n
         8. */
  uint8_t max_data_width;
    /**< Maximum native data type width in bits that can be accessed.
         Supported values:\n
         64. */
}
#include "apr_pack_end.h"
;


/**
  Response to the following memory mapping commands:\n
  #VSS_IMEMORY_CMD_MAP_VIRTUAL.\n
  #VSS_IMEMORY_CMD_MAP_PHYSICAL.

  @par Payload
  #vss_imemory_rsp_map_t

  @return
  None.

  @dependencies
  None.
 */
#define VSS_IMEMORY_RSP_MAP ( 0x00011336 )

/** @brief Type definition for #vss_imemory_rsp_map_t.
*/
typedef struct vss_imemory_rsp_map_t vss_imemory_rsp_map_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for #VSS_IMEMORY_RSP_MAP.
*/
struct vss_imemory_rsp_map_t
{
  uint32_t mem_handle;
    /**< Memory handle encapsulating shared memory attributes. The client
         provides this handle to CVD whenever a memory address or offset is 
         passed to CVD for sharing data. */
}
#include "apr_pack_end.h"
;


/**
  Unmaps memory with CVD.

  @par Payload
  #vss_imemory_cmd_unmap_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The dst_addr in the APR command must be set to MVM address and the dst_port
  must be set to APR_NULL_V.
*/
#define VSS_IMEMORY_CMD_UNMAP ( 0x00011337 )

/** @brief Type definition for #vss_imemory_cmd_unmap_t.
*/
typedef struct vss_imemory_cmd_unmap_t vss_imemory_cmd_unmap_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for #VSS_IMEMORY_CMD_UNMAP.
*/
struct vss_imemory_cmd_unmap_t
{
  uint32_t mem_handle;
    /**< Memory handle returned by #VSS_IMEMORY_RSP_MAP. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_imemory_public_if */

#endif /* __VSS_IMEMORY_PUBLIC_IF_H__ */

