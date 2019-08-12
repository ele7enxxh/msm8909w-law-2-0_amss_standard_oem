#ifndef CB_MEM_H
#define CB_MEM_H
/*===========================================================================

               CB TASK HEADER FILE (CB_MEM.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast memory management utilities

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_mem.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/06   kvk     Renamed cb_free and cb_alloc to cb_mem_free and cb_mem_alloc 
                   to make it compatible with L4 builds
05/16/05   kvk     New memory pool id defined for New Message bitmap memory pool.
07/14/04   vdr     Added support for Skip request memory pool
01/04/04   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include "comdef.h"

typedef enum
{
  CB_FIRST_TX_SCHEDULE_ALLOC_ID,
  CB_RETX_SCHEDULE_ALLOC_ID,
  CB_NEW_MSG_ALLOC_ID,
  CB_USER_PAGE_INFO_ALLOC_ID,
  CB_MESSAGE_LIST_ALLOC_ID,
  CB_MISSING_PAGE_ALLOC_ID,
  CB_SKIP_SEQUENCE_ALLOC_ID
} cb_mem_alloc_id_type ;

/*===========================================================================

FUNCTION    CB_MEM_INIT

DESCRIPTION
  This function initializes CB task's memory pools

DEPENDENCIES
  None

RETURN VALUE
  TRUE if memory pools have been allocated successfully and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_mem_init
(
  void
) ;

/*===========================================================================

FUNCTION    CB_MEM_ALLOC

DESCRIPTION
  This function allocate memory from the CB TASK's memory pools

DEPENDENCIES
  None

RETURN VALUE
  Allocated buffer pointer if there is requested memory buffer available or 
  NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
void *cb_mem_alloc
(
  cb_mem_alloc_id_type mem_alloc_id,
  dword required_length
) ;

/*===========================================================================

FUNCTION    CB_MEM_FREE 

DESCRIPTION
  This function deallocates allocated memory block and returns it into 
  corresponding memory pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_mem_free
(
  void *memory_block_ptr
) ;

#endif /* #ifdef FEATURE_GSM_CB */

#endif /* #ifndef CB_SCHEDULE_H */
