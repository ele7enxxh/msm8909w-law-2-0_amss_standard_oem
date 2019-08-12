#ifndef VM_MM_H
#define VM_MM_H

/*=============================================================================



                  MMU Manager for Virtual Machines


FILE:
  VM_MM


GENERAL DESCRIPTION

  This file contains data structures and exported API for MMU manager for virtual 
  machines to manage PL0/1 stage 2 translation tables.


EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

 

===============================================================================

Copyright (c) 2013

Qualcomm Technologies Incorporated.

All Rights Reserved.

Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================



                        EDIT HISTORY FOR MODULE



  This section contains comments describing changes made to the module.

  Notice that changes are listed in reverse chronological order.



$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/mmu/inc/vm_mm.h#1 $

$DateTime: 2016/06/17 14:31:11 $

$Author: pwbldsvc $

$Change: 10727476 $



when       who     what, where, why

--------   ---     ------------------------------------------------------------

08/14/13   sho     Major code reorg and add dynamic APIs
05/01/13   sasbe   Initial version

=============================================================================*/

/* ----------------------------------------------------------------------------

** Includes

** ------------------------------------------------------------------------- */

#include <stdint.h>

/* ----------------------------------------------------------------------------

** Definitions

** ------------------------------------------------------------------------- */

#define VMMM_MAX_CONTEXT_NUM  1

// permission bit mask

#define VMMM_PERM_X     0x1
#define VMMM_PERM_W     0x2
#define VMMM_PERM_R     0x4
#define VMMM_PERM_RWX   (VMMM_PERM_R | VMMM_PERM_W | VMMM_PERM_X)
#define VMMM_PERM_RW    (VMMM_PERM_R | VMMM_PERM_W )
#define VMMM_PERM_RX    (VMMM_PERM_R | VMMM_PERM_X)
#define VMMM_PERM_WX    (VMMM_PERM_W | VMMM_PERM_X)

// error codes
#define VMMM_OK                  0
#define VMMM_ERR_GENERIC         -1
#define VMMM_ERR_INVALID_HANDLE  -2
// error codes less than below originate from LPAE manager
#define VMMM_ERR_LPAE_OFFSET     -16   

/* ----------------------------------------------------------------------------

** Types

** ------------------------------------------------------------------------- */


// cache property
typedef enum

{

  // device non-gathering, non-reordering, no early write acknowledgement
  VMMM_CACHE_DEVICE_nGnRnE       = 0,   // b0000
  VMMM_CACHE_DEVICE_nGnRE        = 1,   // b0001
  VMMM_CACHE_DEVICE_nGRE         = 2,   // b0010
  VMMM_CACHE_DEVICE_GRE          = 3,   // b0011
  // outer non-cacheable, inner non-cacheable
  VMMM_CACHE_OUTER_NC_INNER_NC   = 5,   // b0101
  // outer non-cacheable, inner write-through
  VMMM_CACHE_OUTER_NC_INNER_WT   = 6,   // b0110
  // outer non-cacheable, inner writeback
  VMMM_CACHE_OUTER_NC_INNER_WB   = 7,   // b0111
  VMMM_CACHE_OUTER_WT_INNER_NC   = 9,   // b1001
  VMMM_CACHE_OUTER_WT_INNER_WT   = 10,  // b1010
  VMMM_CACHE_OUTER_WT_INNER_WB   = 11,  // b1011
  VMMM_CACHE_OUTER_WB_INNER_NC   = 13,  // b1101
  VMMM_CACHE_OUTER_WB_INNER_WT   = 14,  // b1110
  VMMM_CACHE_OUTER_WB_INNER_WB   = 15,  // b1111
  // same as defined in stage 1 
  VMMM_CACHE_USE_STAGE1          = 15,  
} 

VMMM_cache_t;

// sharability
typedef enum

{
   VMMM_SHARE_NONE         = 0,
   VMMM_SHARE_OUTER        = 2,
   VMMM_SHARE_INNER        = 3,
   // same as defined in stage 1 
   VMMM_SHARE_USE_STAGE1   = 0,
} 
VMMM_share_t;

// VM memory management context
typedef struct
{
   uint64_t vttbr;
   uint32_t vtcr;
   void     *l1pt;   // L1 page tabe base
}
VMMM_context_t;

// attributes for creating a memory region
typedef struct 
{
   uint64_t       ipa;
   uint64_t       pa;
   uint64_t       size;
   uint32_t       perm;       // VMMM_PERM_???
   VMMM_cache_t   cache_attr;
   VMMM_share_t   share_attr;
}
VMMM_attr_t;

/**
@brief Initialize VM context 

@param[in] vmid      - VM Id
@param[in] l1pt      - sufficient zero-initialized memory for L1 page table
@param[in] p_context - ptr to VMMM context 

@return VMMM_OK if successful; error codes otherwise
*/
int VMMM_Init(uint32_t vmid, void * l1pt, VMMM_context_t * p_context);

/**
@brief Enable MMU for VM context 

@param[in] p_context - ptr to VMMM context 
*/
void VMMM_Enable(const VMMM_context_t * p_context);

/**
@brief Map a new memory region

@param[in] p_context - ptr to VMMM context 
@param[in] p_map_attr- ptr to mapping attributes

@return VMMM_OK if successful; error codes otherwise
*/
int VMMM_Map(const VMMM_context_t * p_context, 
      const VMMM_attr_t * p_map_attr);

/**
@brief Map a new memory region

@param[in] p_context - ptr to VMMM context 
@param[in] vmid - virtual machine ID
@param[in] ipa - start IPA to be mapped
@param[in] size - size of memory to be mapped

@return VMMM_OK if successful; error codes otherwise
*/
int VMMM_MapFromList(const VMMM_context_t * p_context, unsigned int vmid, 
      uint64_t ipa, uint64_t size);

/**
@brief Unmap a new memory region

@param[in] p_context - ptr to VMMM context 
@param[in] ipa       - intermediate physical address
@param[in] size      - memory region size

@return VMMM_OK if successful; error codes otherwise
*/
int VMMM_Unmap(const VMMM_context_t * p_context, uint64_t ipa, 
      uint64_t size );

#endif // VM_MM_H
