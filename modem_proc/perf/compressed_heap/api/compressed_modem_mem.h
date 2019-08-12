/*!
  @file
  compressed_modem_mem.h

  @brief
  Contains the modem heap manager public interface.

*/

/*===========================================================================

  Copyright (c) 2009-2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/perf/compressed_heap/api/compressed_modem_mem.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/30/15   rm       Fix for CR 955211 - KW P1 fixes and code cleanup for mem sections.
03/18/15   skp      Initial Version
===========================================================================*/



#ifndef COMPRESSED_MODEM_MEM_H
#define COMPRESSED_MODEM_MEM_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stddef.h>            /* standard definition file (for size_t) */
#include "modem_mem.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


typedef modem_mem_client_e compressed_modem_mem_client_e;
/* --------------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================

  FUNCTION:  modem_mem_alloc_ch

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated.

    @note
    The modem heap clients should use modem_mem_alloc_ch(), and should not 
    call modem_mem_alloc_ext_ch() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_alloc_ext_ch() to modem_mem_alloc_ch() 
*/
#define  modem_mem_alloc_ch_ext(size, client, file, line)                     \
         modem_mem_alloc_ch(size, client)

void* modem_mem_alloc_ch(
  size_t                                                    size,       /*!< Number of bytes to allocate */
  compressed_modem_mem_client_e   client      /*!< Client ID for this mem block */
);

void* modem_mem_alloc_ch_setcaller(
  size_t                                                    size,            /*!< Number of bytes to allocate */
  compressed_modem_mem_client_e   client,         /*!< Client ID for this mem block */
  void*                                                     caller_ptr   /*!< Caller Ptr to be set in debug information */
);


/*===========================================================================

  FUNCTION:  modem_mem_calloc_ch

===========================================================================*/
/*!
    @brief
    Allocates enough space for elt_count elements each of elt_size bytes
    from the heap and initializes the space to nul bytes.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated. If elt_count or elt_size is 0, the NULL pointer 
    will be silently returned.

    @note
    The modem heap clients should use modem_mem_calloc_ch(), and should not
    call modem_mem_call_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_calloc_ch_ext() to modem_mem_calloc_ch() 
*/
#define  modem_mem_calloc_ch_ext(elt_count, elt_size, client, file, line)     \
         modem_mem_calloc_ch(elt_count, elt_size, client)

void* modem_mem_calloc_ch(
  size_t                                                   elt_count,  /*!< Number of elements to allocate */
  size_t                                                   elt_size,     /*!< Size of each element                  */
  compressed_modem_mem_client_e  client         /*!< Client ID for this mem block       */
);

void* modem_mem_calloc_ch_setcaller(
  size_t                                                   elt_count,  /*!< Number of elements to allocate               */
  size_t                                                   elt_size,     /*!< Size of each element                                 */
  compressed_modem_mem_client_e  client,         /*!< Client ID for this mem block                     */
  void*                                                    caller_ptr   /*!< Caller Ptr to be set in debug information */
);


/*===========================================================================

  FUNCTION:  modem_mem_realloc_ch

===========================================================================*/
/*!
    @brief
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.

    @detail
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.  If the block is shortened, bytes are discarded off the
    end.  If the block is lengthened, the new bytes added are not initialized
    and will have garbage values.  
    If ptr is NULL, the function behaves exactly like modem_mem_alloc_ch 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_mem_free_ch (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_mem_alloc_ch if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.

    @note
    The modem heap clients should use modem_mem_realloc_ch(), and should not
    call modem_mem_realloc_ch_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_realloc_ch_ext() to modem_mem_realloc_ch() 
*/
#define  modem_mem_realloc_ch_ext(ptr, size, client, file, line)              \
         modem_mem_realloc_ch(ptr, size, client)
         
void *modem_mem_realloc_ch(
  void*                                                    ptr,          /*!< A block previously allocated from heap             */
  size_t                                                    size,        /*!< New size (in bytes) of the ptr block of memory */
  compressed_modem_mem_client_e   client      /*!< Client ID for this mem block                               */
);

void *modem_mem_realloc_ch_setcaller(
  void*                                                     ptr,             /*!< A block previously allocated from heap             */
  size_t                                                    size,           /*!< New size (in bytes) of the ptr block of memory */
  compressed_modem_mem_client_e   client,        /*!< Client ID for this mem block                               */
  void*                                                    caller_ptr   /*!< Caller Ptr to be set in debug information           */
);



/*===========================================================================

  FUNCTION:  modem_mem_free_ch

===========================================================================*/
/*!
    @brief
    Deallocates the ptr block of memory. 

    @detail
    Deallocates the ptr block of memory.  If ptr is NULL, or ptr is outside 
    the range of memory managed by heap, then this function call does nothing
    (and is guaranteed to be harmless).

    @return
    None.

    @note
    The modem heap clients should use modem_mem_free_ch(), and should not call
    modem_mem_free_ch_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_free_ch_ext() to modem_mem_free_ch() 
*/
#define  modem_mem_free_ch_ext(ptr, client, file, line)                       \
         modem_mem_free_ch(ptr, client)

void modem_mem_free_ch(
  void*                                                     ptr,         /*!< Memory to free                   */
  compressed_modem_mem_client_e   client      /*!< User ID for this mem block */
);


#ifdef __cplusplus
}
#endif

#endif /* COMPRESSED_MODEM_MEM_H */
