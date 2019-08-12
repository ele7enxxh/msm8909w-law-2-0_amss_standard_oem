
#ifndef _EMM_MEM_H_
#define _EMM_MEM_H_
/*===========================================================================

               EMM TASK HEADER FILE (EMM_MEM.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  EMM memory management utilities

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


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_mem.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
#include "comdef.h"
#include "mm_v.h"


#define emm_modem_mem_alloc(x, y)   modem_mem_calloc(1,x, MODEM_MEM_CLIENT_NAS)

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
extern void *emm_modem_mem_alloc_debug
(
  size_t               size,  /* size of memory to be allocated */
  char                *objname, /* Used to see the name of the object being freed if "sizeof" operator is used. */
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

#define emm_modem_mem_free(x, y) emm_modem_mem_free_debug(x, #x, y)

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
extern void emm_modem_mem_free_debug
(
  void                *ptr,
  char                *objname,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);


#endif /* #ifndef _EMM_MEM_H_ */

#endif /*FEATURE_LTE*/

