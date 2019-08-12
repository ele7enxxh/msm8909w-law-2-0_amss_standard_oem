/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cmd_buffer_utils.c_v   1.2   14 Aug 2001 15:15:36   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cmd_buffer_utils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

12/17/03   AB      Added safety/NULL pointer check in CNM_free().
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "mn.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"


#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

/**************** Begin of   CNM_alloc ********************************************
 *
 *  Function name: CNM_alloc()
 *  -------------------------
 *
 *  Description:
 *  ------------
 *  This function allocates the required number of bytes using gs_alloc. If
 *  the allocation fails, a warning is generated and a pointer to a default
 *  data area is returned to avoid segmentation violations
 *
 *  Parameters:
 *  -----------
 *  size - number of bytes to allocate
 *
 *  Returns:
 *  --------
 *  Pointer to allocated memory 
 *
 ******************************************************************************/

VOID_FUNC * CNM_alloc( unsigned int size ) 
{
  VOID_DATA * p_mem;

#ifndef FEATURE_MODEM_HEAP
  p_mem = gs_alloc( size );
#else
  p_mem = modem_mem_calloc(1,size, MODEM_MEM_CLIENT_NAS);
#endif

  if ( p_mem == NULL )
  {
    ERR("gs_alloc failure for CNM task\n", 0, 0, 0);
  }

  return p_mem;
}
/**************** End of   CNM_alloc *********************************************/


/****************Begin of   CNM_free **********************************************
 *
 *  Function name: CNM_free()
 *  ------------------------
 *
 *  Description:
 *  ------------
 *  This function frees an area of memory allocated by CNM_alloc
 *
 *  Parameters:
 *  -----------
 *  p_mem - pointer to memeory buffer to free
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_free( VOID_DATA * p_mem ) 
{
  if(p_mem != NULL)   /* application can corrupt/set the p_mem to NULL */
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free( p_mem );
#else
    modem_mem_free(p_mem, MODEM_MEM_CLIENT_NAS);
#endif
  }
  else
  {
    ERR("CNM message buffer = %d corrupted", p_mem, 0, 0);
  }
}

/**************** End of   CNM_free **********************************************/
