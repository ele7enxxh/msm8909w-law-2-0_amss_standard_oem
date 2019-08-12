
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     EMM MEMORY CONTROL MODULE (EMM_MEM.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_mem.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09   vdr     Fixed Lint errors & warnings
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "customer.h"
#include <string.h>
#include "comdef.h"
#include "emm_mem.h"
#include "emm_database.h"
#include "emm_connection_handler.h"
#include "err.h"
#include "cm_msgr_msg.h"
#include "naslog_v.h"
#include "msgr_nas.h"
#include "stringl.h"
#include "lte_nas_log.h"
/*#if defined(TEST_FRAMEWORK) && defined(FEATURE_EMM_UNIT_TEST)
#include <stdio.h>
#include <stdlib.h>

#undef MSG_LOW
#undef MSG_HIGH
#undef ERR
#undef ERR_FATAL

#define MSG_LOW_DS(MM_SUB,a,b,c,d) {printf(a,b,c,d); printf("\n");}
#define MSG_HIGH_DS(MM_SUB,a,b,c,d) {printf(a,b,c,d); printf("\n");}
#define MSG_ERROR_DS(MM_SUB,a,b,c,d) {printf(a,b,c,d); printf("\n");}
#define MSG_FATAL_DS(MM_SUB,a,b,c,d) {printf(a,b,c,d); printf("\n"); exit(1);}

#endif // TEST_FRAMEWORK
*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
              
/*===========================================================================

FUNCTION   EMM_MODEM_MEM_ALLOC_DEBUG

DESCRIPTION
  This function allocates dynamic memory using modem heap. This function simply
  calls modem_mem_alloc and returns pointer to the allocated memory.  
  The memory allocated is initialized with zeros.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void *emm_modem_mem_alloc_debug
(
  size_t               size,
  char                *objname,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  void *mem_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != NULL);

  mem_ptr = modem_mem_alloc(size, MODEM_MEM_CLIENT_NAS);
  mm_check_for_null_ptr((void*)mem_ptr);

  /* Init the allocated memory */
  memset(mem_ptr, 0x0, size);

  emm_ctrl_data_ptr->emm_heap_mem_block_num++;

  return mem_ptr;

} /* emm_modem_mem_alloc() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION   EMM_MODEM_MEM_FREE_DEBUG

DESCRIPTION
   This function returuns the specified memory back to the Heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_modem_mem_free_debug
(
  void                *ptr,
  char                *objname,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(ptr != NULL)
  {
    /* Deallocate the memory to heap */
    modem_mem_free(ptr, MODEM_MEM_CLIENT_NAS);

    emm_ctrl_data_ptr->emm_heap_mem_block_num--;
  }

} /* emm_modem_mem_free_debug */


#endif /*FEATURE_LTE*/

