/*!
  @file
  modem_mem.h

  @brief
  Contains the modem heap manager public interface.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/modem_mem.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/14   rks    Added new api for modem clients for setting caller functions in heap
01/17/12   dc      Added new client IDs for QMI
06/10/11   rs      Added four new client IDs for TDSCDMA 
04/11/11   ars     Added new client MODEM_MEM_CLIENT_UTILS
06/04/09   ic      #include <stddef.h> explicitly for size_t
05/31/09   ic      Two new client ID added; __FILE__ fix
05/05/09   ic      Six new client ID added
04/01/09   ic      Rearrangement of client ID enum
03/16/09   ic      Initial Version
===========================================================================*/



#ifndef MODEM_MEM_H
#define MODEM_MEM_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stddef.h>            /* standard definition file (for size_t) */


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/* --------------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

/*! @brief 
** The MSB in the client ID is used to indicate if the memory allocation
** request is a critical one. Note that the size of client ID is one byte.
*/
#define MODEM_MEM_CRIT_FLAG          (1<<7)


/* --------------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* Enumeration of Modem Heap clients - passed as a parameter at the time of 
** memory (re-)allocation and free. This is used for memory usage tracking
** and memory budgeting purposes as well as debugging. Note that the size of
** client ID is one byte.
**
** By default, each client is given two client ID's:
** (1) use the client ID with "_CRIT" postfix, if the memory request is a 
**     critical one, i.e., the memory is absolutely required for its normal
**     operation, and the memory allocation failure leads to an err fatal, 
** (2) use the client ID w/o "_CRIT" postfix, if the memory request is not a
**     critical one and the memory allocation failure leads to a soft error.
*/
typedef enum _modem_mem_client_e
{
  MODEM_MEM_CLIENT_1X              = 1,
  MODEM_MEM_CLIENT_1X_CRIT         = (MODEM_MEM_CLIENT_1X | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_CNE             = 2,
  MODEM_MEM_CLIENT_CNE_CRIT        = (MODEM_MEM_CLIENT_CNE | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_DATA            = 3,
  MODEM_MEM_CLIENT_DATA_CRIT       = (MODEM_MEM_CLIENT_DATA | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_DATACOMMON      = 4,
  MODEM_MEM_CLIENT_DATACOMMON_CRIT = (MODEM_MEM_CLIENT_DATACOMMON | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_DSM             = 5,
  MODEM_MEM_CLIENT_DSM_CRIT        = (MODEM_MEM_CLIENT_DSM | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_ECALL           = 6,
  MODEM_MEM_CLIENT_ECALL_CRIT      = (MODEM_MEM_CLIENT_ECALL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_GERAN           = 7,
  MODEM_MEM_CLIENT_GERAN_CRIT      = (MODEM_MEM_CLIENT_GERAN | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_GPS             = 8,
  MODEM_MEM_CLIENT_GPS_CRIT        = (MODEM_MEM_CLIENT_GPS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_GSLIB           = 9,
  MODEM_MEM_CLIENT_GSLIB_CRIT      = (MODEM_MEM_CLIENT_GSLIB | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_HDR             = 10,
  MODEM_MEM_CLIENT_HDR_CRIT        = (MODEM_MEM_CLIENT_HDR | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_IMS             = 11,
  MODEM_MEM_CLIENT_IMS_CRIT        = (MODEM_MEM_CLIENT_IMS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_MAC         = 12,
  MODEM_MEM_CLIENT_LTE_MAC_CRIT    = (MODEM_MEM_CLIENT_LTE_MAC | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_PDCPDL      = 13,
  MODEM_MEM_CLIENT_LTE_PDCPDL_CRIT = (MODEM_MEM_CLIENT_LTE_PDCPDL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_PDCPUL      = 14,
  MODEM_MEM_CLIENT_LTE_PDCPUL_CRIT = (MODEM_MEM_CLIENT_LTE_PDCPUL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_RLCDL       = 15,
  MODEM_MEM_CLIENT_LTE_RLCDL_CRIT  = (MODEM_MEM_CLIENT_LTE_RLCDL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_RLCUL       = 16,
  MODEM_MEM_CLIENT_LTE_RLCUL_CRIT  = (MODEM_MEM_CLIENT_LTE_RLCUL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_LTE_RRC         = 17,
  MODEM_MEM_CLIENT_LTE_RRC_CRIT    = (MODEM_MEM_CLIENT_LTE_RRC | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_MCS             = 18,
  MODEM_MEM_CLIENT_MCS_CRIT        = (MODEM_MEM_CLIENT_MCS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_MFLO            = 19,
  MODEM_MEM_CLIENT_MFLO_CRIT       = (MODEM_MEM_CLIENT_MFLO | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_MMODE           = 20,
  MODEM_MEM_CLIENT_MMODE_CRIT      = (MODEM_MEM_CLIENT_MMODE | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_NAS             = 21,
  MODEM_MEM_CLIENT_NAS_CRIT        = (MODEM_MEM_CLIENT_NAS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_QMI             = 22,
  MODEM_MEM_CLIENT_QMI_CRIT        = (MODEM_MEM_CLIENT_QMI | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_RFA             = 23,
  MODEM_MEM_CLIENT_RFA_CRIT        = (MODEM_MEM_CLIENT_RFA | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_UIM             = 24,
  MODEM_MEM_CLIENT_UIM_CRIT        = (MODEM_MEM_CLIENT_UIM | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WCDMA           = 25,
  MODEM_MEM_CLIENT_WCDMA_CRIT      = (MODEM_MEM_CLIENT_WCDMA | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WCDMA_EXT_CMD   = 26,
  MODEM_MEM_CLIENT_WCDMA_EXT_CMD_CRIT  
                                   = (MODEM_MEM_CLIENT_WCDMA_EXT_CMD | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WCDMA_RLC       = 27,
  MODEM_MEM_CLIENT_WCDMA_RLC_CRIT  = (MODEM_MEM_CLIENT_WCDMA_RLC | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WCDMA_RRC_CMD   = 28,
  MODEM_MEM_CLIENT_WCDMA_RRC_CMD_CRIT
                                   = (MODEM_MEM_CLIENT_WCDMA_RRC_CMD | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL= 29,
  MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT
                                   = (MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WLAN            = 30,
  MODEM_MEM_CLIENT_WLAN_CRIT       = (MODEM_MEM_CLIENT_WLAN | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_WMS             = 31,
  MODEM_MEM_CLIENT_WMS_CRIT        = (MODEM_MEM_CLIENT_WMS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_UTILS           = 32,
  MODEM_MEM_CLIENT_UTILS_CRIT      = (MODEM_MEM_CLIENT_UTILS | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_TDSCDMA_L1      = 33,
  MODEM_MEM_CLIENT_TDSCDMA_L1_CRIT = (MODEM_MEM_CLIENT_TDSCDMA_L1 | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_TDSCDMA_L2      = 34,
  MODEM_MEM_CLIENT_TDSCDMA_L2_CRIT = (MODEM_MEM_CLIENT_TDSCDMA_L2 | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD = 35,
  MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD_CRIT
                                   = (MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_TDSCDMA_RRC_GLOBAL= 36,
  MODEM_MEM_CLIENT_TDSCDMA_RRC_GLOBAL_CRIT
                                   = (MODEM_MEM_CLIENT_TDSCDMA_RRC_GLOBAL | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_QCHAT           = 37,
  MODEM_MEM_CLIENT_QCHAT_CRIT      = (MODEM_MEM_CLIENT_QCHAT | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_QMI_MMODE       = 38,
  MODEM_MEM_CLIENT_QMI_MMODE_CRIT  = (MODEM_MEM_CLIENT_QMI_MMODE | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_QMI_PBM         = 39,
  MODEM_MEM_CLIENT_QMI_PBM_CRIT    = (MODEM_MEM_CLIENT_QMI_PBM | 
                                      MODEM_MEM_CRIT_FLAG),

  MODEM_MEM_CLIENT_QMI_WMS         = 40,
  MODEM_MEM_CLIENT_QMI_WMS_CRIT    = (MODEM_MEM_CLIENT_QMI_WMS | 
                                     MODEM_MEM_CRIT_FLAG),

  /* Number of clients - same as the last entry becase the entry is starting
  ** with 1.
  */
  MODEM_MEM_NUM_CLIENTS            = MODEM_MEM_CLIENT_QMI_WMS,

  /* Do NOT change the MAX value below, and do NOT add any new clients
  ** beyond the MAX value 
  */
  MODEM_MEM_CLIENT_MAX             = 256
}  modem_mem_client_e;

/* --------------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================

  FUNCTION:  modem_mem_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated.

    @note
    The modem heap clients should use modem_mem_alloc(), and should not 
    call modem_mem_alloc_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_alloc_ext() to modem_mem_alloc() 
*/
#define  modem_mem_alloc_ext(size, client, file, line)                     \
         modem_mem_alloc(size, client)

void* modem_mem_alloc(
  size_t               size,       /*!< Number of bytes to allocate */
  modem_mem_client_e   client      /*!< Client ID for this mem block */
);

void* modem_mem_alloc_setcaller(
  size_t               size,       /*!< Number of bytes to allocate */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
);


/*===========================================================================

  FUNCTION:  modem_mem_calloc

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
    The modem heap clients should use modem_mem_calloc(), and should not
    call modem_mem_call_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_calloc_ext() to modem_mem_calloc() 
*/
#define  modem_mem_calloc_ext(elt_count, elt_size, client, file, line)     \
         modem_mem_calloc(elt_count, elt_size, client)

void* modem_mem_calloc(
  size_t              elt_count,  /*!< Number of elements to allocate */
  size_t              elt_size,   /*!< Size of each element*/
  modem_mem_client_e  client      /*!< Client ID for this mem block */
);

void* modem_mem_calloc_setcaller(
  size_t              elt_count,  /*!< Number of elements to allocate */
  size_t              elt_size,   /*!< Size of each element*/
  modem_mem_client_e  client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
);


/*===========================================================================

  FUNCTION:  modem_mem_realloc

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
    If ptr is NULL, the function behaves exactly like modem_mem_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_mem_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_mem_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.

    @note
    The modem heap clients should use modem_mem_realloc(), and should not
    call modem_mem_realloc_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_realloc_ext() to modem_mem_realloc() 
*/
#define  modem_mem_realloc_ext(ptr, size, client, file, line)              \
         modem_mem_realloc(ptr, size, client)
         
void *modem_mem_realloc(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client      /*!< Client ID for this mem block */
);

void *modem_mem_realloc_setcaller(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */

  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
);



/*===========================================================================

  FUNCTION:  modem_mem_free

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
    The modem heap clients should use modem_mem_free(), and should not call
    modem_mem_free_ext() directly.
*/
/*=========================================================================*/

/*! @brief convert modem_mem_free_ext() to modem_mem_free() 
*/
#define  modem_mem_free_ext(ptr, client, file, line)                       \
         modem_mem_free(ptr, client)

void modem_mem_free(
  void                *ptr,        /*!< Memory to free */
  modem_mem_client_e   client      /*!< User ID for this mem block */
);

void modem_mem_get_section_info(
	void **section_info
);

#ifdef __cplusplus
}
#endif

#endif /* MODEM_MEM_H */
