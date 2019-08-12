#ifndef PS_MEM_EXT_H
#define PS_MEM_EXT_H
/*===========================================================================

   P R O T O T O C O L   S E R V I C E S   D Y N A M I C   M E M O R Y

                     E X  T E R N A L   H E A D E R   F I L E

===========================================================================*/

/**
   @file ps_mem_ext.h
   @brief This module implements a simple dynamic memory management mechanism for
  the protocol services.
 */

/*
EXTERNALIZED FUNCTIONS
  ps_ext_mem_get_buf()   - Allocates a memory buffer for the specified poolid
  ps_mem_free()          - Free the allocated memory, like free.
  ps_mem_dup()           - Dup the allocated memory. 
*/

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the utils group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_mem_ext.h#1 $
  $DateTime: 2007/05/18 18:56:57

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
05/07/11    SCD    (Tech Pubs) Continued edit.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
08/26/10    pp     Added PS_EXT_MEM_TX_META_INFO_TYPE pool for external 
                   clients.
06/09/10    pp     Added two EXT pools, cleaned up comments in this header.
08/31/09   mga     Merged from eHRPD branch
07/23/09    mt     New pool for DNS session-specific server buffers. 
02/19/09    am     Added new pool ID for DCC task cmd buffers.
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
3/17/09     en     API changes for better BCL memory management.
===========================================================================*/
/* group: utils */

#include "comdef.h"

/*===========================================================================

                         PUBLIC DEFINITIONS AND DATA

===========================================================================*/

/** @addtogroup constants 
  @{
*/

/** External memory pools. */
#define PS_EXT_MEM_POOLS
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  Identifies the external pool ID associated with the various external memory 
  buffers.

  This enumerator is primarily required by modules outside of common data, as 
  well as some internal modules. This enumerator is also used by data modules 
  running on various HLOSs.

  Every external memory pool has an internal mapping.

  Currently supported external memory pools include:\n
  @code
      External enumerator        Mapping   Internal enumerator value
  PS_EXT_MEM_META_INFO_TYPE_EX     =>      PS_MEM_META_INFO_TYPE_EX
  PS_EXT_MEM_RT_META_INFO_TYPE     =>      PS_MEM_RT_META_INFO_TYPE
  PS_EXT_MEM_PKT_META_INFO_TYPE    =>      PS_MEM_PKT_META_INFO_TYPE
  PS_EXT_MEM_TX_META_INFO_TYPE:    =>      PS_MEM_TX_META_INFO_TYPE
  @endcode

  See #ps_mem_pool_enum_type (defined in ps_mem.h) for all internal pools.
*/
typedef enum
{
  PS_EXT_MEM_DUMMY_POOL_TYPE              = 0, 
    /**< Dummy pool. */
  PS_EXT_MEM_META_INFO_TYPE_EX            = 1, 
    /**< Mapped to PS_MEM_META_INFO_TYPE_EX. */
  PS_EXT_MEM_RT_META_INFO_TYPE            = 2, 
    /**< Mapped to PS_MEM_RT_META_INFO_TYPE. */
  PS_EXT_MEM_PKT_META_INFO_TYPE           = 3, 
    /**< Mapped to PS_MEM_PKT_META_INFO_TYPE. */
  PS_EXT_MEM_TX_META_INFO_TYPE            = 4, 
    /**< Mapped to PS_MEM_TX_META_INFO_TYPE. */
  PS_EXT_MEM_MAX_POOLS                         
    /**< Maximum number of pools. */

} ps_ext_mem_pool_enum_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_utils
  @{
*/
/*===========================================================================
MACRO PS_MEM_FREE (buf)
===========================================================================*/
/**
  Deallocates memory.

  @param[in,out] buf  Memory to be deallocated.

  @return
  None.

  @dependencies
  None.
*/
#define PS_MEM_FREE(buf)                                                    \
{                                                                           \
  ps_mem_free((void *) buf);                                            \
  buf = NULL;                                                               \
}                              

#ifdef __cplusplus
extern "C" {
#endif

/** @} */ /* end_addtogroup macros_utils */
/*===========================================================================

                           PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION     PS_EXT_MEM_GET_BUF()
===========================================================================*/
/** @ingroup ps_ext_mem_get_buf
  External function to get the buffer for external memory pools. Current 
  external memory pools include:\n
             - PS_EXT_MEM_META_INFO_TYPE_EX
             - PS_EXT_MEM_RT_META_INFO_TYPE
             - PS_EXT_MEM_PKT_META_INFO_TYPE  
             - PS_EXT_MEM_TX_META_INFO_TYPE  

  @note1hang\n
  - Regular functions are to be used to reclaim/duplicate the 
  buffers.
  - No special functions exist for free/duplicate calls. Use ps_mem_free() or
  ps_mem_dup().

  @datatypes
  #ps_ext_mem_pool_enum_type

  @vertspace
  @param[in] poolid  Buffer allocated from the passed pool ID.

  @return
  If successful, a pointer to the allocated memory block is returned. 
  Otherwise, a NULL pointer is returned.

  @dependencies
  The passed pool ID must be valid.
  @newpage
*/
void *ps_ext_mem_get_buf
(
  ps_ext_mem_pool_enum_type poolid
);

/*===========================================================================
FUNCTION     PS_MEM_FREE()
===========================================================================*/
/** @ingroup ps_mem_free
  Deallocates the memory. This function also checks for possible memory 
  corruptions.

  @param[in] buf  Memory to be deallocated.

  @return
  Void.

  @dependencies
  None.
  @newpage
*/
void ps_mem_free
(
  void *buf
);

/*===========================================================================
FUNCTION     PS_MEM_DUP()
===========================================================================*/
/** @ingroup ps_mem_dup
  Duplicates a PS memory item.

  @param[in] buf  Memory to be duplicated.

  @return
  The pointer to buf after duplicating the memory item.

  @dependencies
  None.
  @newpage
*/
void* ps_mem_dup
(
  void *buf
);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* PS_MEM_H */
