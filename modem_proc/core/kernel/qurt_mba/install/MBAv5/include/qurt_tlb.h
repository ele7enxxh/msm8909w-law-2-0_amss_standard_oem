#ifndef QURT_TLB_H
#define QURT_TLB_H

/*=============================================================================

                 qurt_tlb.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
    Prototypes of TLB API  
		The following APIs allow explicit control of the portion of TLB that between TLB_firs_replaceble and TLB_LAST_REPLACEABLE. 
		Both are non-configurable for the time being. This portion of TLB is permanently assigned/locked unless manually removed 
		by qurt_tlb_remove. Implementation does not change depending on the configuration, i.e. whether CONFIG_STATIC is set or not. 
		In CONFIG_STATIC=y, TLB_LAST_REPLACEABLE is set to the last tlb index, which indicates that the entire TLB is permanently 
		assigned and is not backed up by pagetable (pagetable does not exist). TLB indecies are maintained through a 64-bit bitmask. 
		A new entry is placed in the first available free slot. 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_tlb.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/
#include <qurt_types.h>

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/** create TLB mapping, assuming this entry is not backed up by pagetable.
 *   @ param   entry_id:         index of successfully created entry, -1 otherwise
 *   @ param   vaddr:			virtual address to map
 *   @ param   paddr: 			physical address to map
 *   @ param   size: 			size of region to map in bytes
 *   @ param   cache_attribs: 	cacheability
 *   @ param   perms: 			permissions  
 *   @ param   asid: 			asid
 *
 *   @ return	 error if mapping unsuccessful
 *   @ return	 QURT_EFATAL - TLB full
 *   @ return	 QURT_ETLBCREATESIZE - TLB create error: incorrect size in input param
 *   @ return	 QURT_ETLBCREATEUNALIGNED - TLB create error: unaligned address in input param
 *   Note: it is user's responsibility to provide correct input parameters, the API will return error code 
 *         if address is not aligned to the size. 
 *
 *   Note: To set G bit in TLB entry, uses asid=-1.
 */
int  qurt_tlb_entry_create (unsigned int *entry_id, qurt_addr_t vaddr, qurt_paddr_t paddr, qurt_size_t size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perms, int asid);


/** remove TLB mapping by address, assuming this entry is not backed up by pagetable.
 *  @ param   entry_id: 			tlb_index (0..63) of entry to unmap
 *  @ return	int   status
 *                  EOK : unmapping successful
 *                  E_FATAL : entry does not exist
 **/
int qurt_tlb_entry_delete (unsigned int entry_id);

/** query TLB mapping.
 *  @ param   entry_id:         tlb index if entry is found   
 *  @ param   vaddr : 			virtual address 
 *  @ param    asid : 			asid 
 *  @ return  int   error_code
 *                  E_FATAL : entry does not exist
 **/
int qurt_tlb_entry_query (unsigned int *entry_id, qurt_addr_t vaddr, int asid);

/**write to TLB.
 *  @ param   entry_id : 			tlb index of entry to write 
 *  @ param   entry : 			entry to write
 *  @ retunr    int    error code
 *                     EOK: successful write to tlb
 *                     E_FATAL: index out of bounds
 **/
int qurt_tlb_entry_set (unsigned int entry_id, unsigned long long int entry);

/** read TLB entry.
 *  @ param    entry_id : 			index of entry to read
 *  @ return   int   error_code/entry
 *                   64-bit entry : entry exists
 *                   E_FATAL: entry does not exist
 **/
int qurt_tlb_entry_get (unsigned int entry_id, unsigned long long int *entry);

/** get number of available entries in tlb
 * @ param      void
 * @ return     int     number of un-occupied entries in tlb
 *  
 **/
unsigned short qurt_tlb_entry_get_available(void);


#endif /* QURT_TLB_H */
