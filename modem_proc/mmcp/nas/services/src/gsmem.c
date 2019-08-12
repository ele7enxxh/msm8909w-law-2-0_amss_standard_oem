/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited..

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gsmem.c_v   1.1   19 Apr 2001 13:40:36   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsmem.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/01   TS      Removed gs_attached_shared_memory() and 
                   gs_free_attached_shared_memory()

07/25/03   AB      Initialized/Clean the new heap buffer in gs_alloc().
04/21/04   vdr     Now allocating small (smaller than MEM_HEAP_THRESHOLD) 
                   memory buffers from tmc_heap_small heap
===========================================================================*/

/* Include files */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "memheap.h"
#include "environ.h"
#include "sys_cnst_v.h"
#include "sys_type.h"
#include "ms.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "msg.h"
#include "err.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#ifndef FEATURE_MODEM_HEAP
/*******************************************************************************
 *
 *  Function name: gs_alloc
 *  -----------------------
 *  Description:
 *  ------------
 *  Gets N bytes of dynamic memory.
 *
 *  Parameters:
 *  -----------
 *  IN          byte_size
 *
 *  Returns:
 *  --------
 *  A pointer to dynamically allocated memory.
 *
 ******************************************************************************/
#ifdef FEATURE_HEAP_SELECTION
#define GS_HEAP_ALLOC(size) ((size<=MEM_HEAP_THRESHOLD)?(&tmc_heap_small):(&tmc_heap))
#endif

VOID_FUNC *gs_alloc(unsigned int   byte_size)
{
        void  *allocation = NULL;

        /* Call function to allocate memory */
#ifdef FEATURE_HEAP_SELECTION
        allocation = mem_malloc(GS_HEAP_ALLOC(byte_size), byte_size);
#else
        allocation = mem_malloc(&tmc_heap, byte_size);
#endif

        /* If unable to allocate memory */
        if(allocation == NULL)
        {
           /* Raise Error Condition */
           ERR_FATAL("ERROR: Unable to allocate memory",0,0,0);
        }
        else
        {
           /* Clean the buffer */
           memset(allocation, NULL, byte_size);
        }

        return allocation;
}

/*******************************************************************************
 *
 *  Function name: gs_free
 *  ----------------------
 *  Description:
 *  ------------
 *  Frees memory which was got from gs_alloc.
 *
 *  Parameters:
 *  -----------
 *  IN         ptr
 *
 *  Returns:
 *  --------
 *  none.
 *
 ******************************************************************************/

VOID_FUNC gs_free(VOID_DATA   *ptr)
{

#ifdef FEATURE_HEAP_SELECTION
        /* Left tmc_heap's boundary */
        VOID_DATA *lb_ptr = (VOID_DATA *)tmc_heap.first_block;
        /* Right tmc_heap's boundary */
        VOID_DATA *rb_ptr = (VOID_DATA *)((unsigned long)lb_ptr + tmc_heap.total_bytes) ;
        /* Left tmc_heap_small's boundary */
        VOID_DATA *lb_small_ptr = (VOID_DATA *)tmc_heap_small.first_block;
        /* Right tmc_heap_small's boundary */
        VOID_DATA *rb_small_ptr = (VOID_DATA *)((unsigned long)lb_small_ptr + tmc_heap_small.total_bytes) ;
#endif

        /* If attempting to free null pointer */
        if ( ptr == 0)
        {
           ERR_FATAL("ERROR: Attempting to free null pointer",0,0,0);
        }
        else
        {
        
#ifdef FEATURE_HEAP_SELECTION
           if ((ptr >= lb_ptr) && (ptr < rb_ptr))
           {
              /* 
              ** If the ptr is laid between the left and right tmc_heap's boundaries,
              ** hence it has been allocated from tmc_heap 
              **/
              mem_free(&tmc_heap, ptr);
           }
           else if ((ptr >= lb_small_ptr) && (ptr < rb_small_ptr))
           {
              /* 
              ** If the ptr is laid between the left and right tmc_heap_small's 
              ** boundaries, hence it has been allocated from tmc_heap_small 
              **/
              mem_free(&tmc_heap_small, ptr);
           }
           else
           {
              ERR_FATAL("ERROR: Pointer is out of heap boundaries %X",(uint32)ptr,0,0);
           }
#else
           mem_free(&tmc_heap, ptr);
#endif
        }

}

#endif

