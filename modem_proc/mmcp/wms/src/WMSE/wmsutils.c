/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsutils.c --

  Implementations of common internal WMS functions.


  -------------

    Copyright (c) 2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsutils.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "string.h"
#include "wmsutils.h"
#include "err.h"
#include "msg.h"
#include "modem_mem.h"

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/



/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_mem_malloc

DESCRIPTION
  Function allocates memory from heap and memory would be initialized to zero

DEPENDENCIES
  None

RETURN VALUE
  void pointer

SIDE EFFECTS
  None
===========================================================================*/
void * wms_mem_malloc(
  size_t                             size
)
{
  void *mem_ptr = NULL;

  if (size > 0)
  {
    mem_ptr = modem_mem_alloc( size, MODEM_MEM_CLIENT_WMS );
  }

  if (NULL != mem_ptr)
  {
    memset(mem_ptr, 0, size);
  }
  //MSG_HIGH_2( "malloc @0x%x, size %d", mem_ptr, size );

  return mem_ptr;
} /* wms_mem_malloc() */


/*===========================================================================
FUNCTION wms_mem_free

DESCRIPTION
  Function free memory back to heap.

DEPENDENCIES
  Null pointer checking on mem_ptr must be done before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wms_mem_free(
  void                              *mem_ptr
)
{
  //MSG_HIGH_1( "free @0x%x", mem_ptr );
  modem_mem_free( mem_ptr, MODEM_MEM_CLIENT_WMS );
} /* wms_mem_free() */


void wms_check_for_null_ptr ()
{
   MSG_ERROR_0("Null Pointer");
}



