/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Dynamic Buffer Module

GENERAL DESCRIPTION
  This module manages the Dynamic buffer pools of PDSM.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/pdapibuf.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/04/13   ah      Support dynamic memory alloc using modem heap vs PDSM heap
07/16/13   ah      Reverted back to original PDSM heap implementation
07/11/13   ah      Removed PDSM heap, instead use OS API for dynamic memory allocation
06/01/11   rc      Moving gnss_pdapi_buff_pool_mutex out of the FEATURE_INTLOCK_UNSUPPORTED 
10/17/03   AK      Added include for rex.h
06/26/02   SK      Added tasklock()/taskfree() for protection and few message's
04/11/02   cah     Fixed Header for file revision.
03/15/00   SK      Initial release.

===========================================================================*/

/*=========================================================================

             Include Files for Module 

============================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"

#include "aries_os_api.h"
#include "pdapibuf.h"
#include "pdapidbg.h"

#ifndef FEATURE_GNSS_IPC_USE_HEAP

/* ------------------------------------------------------------------------ */
/*                       Local Data                                         */
/* ------------------------------------------------------------------------ */

/* Memory pools storage allocation */

#ifndef FEATURE_QDSP6
static char  pdsm_heap[PDSM_DYN_MEMORY];
#else
/* Q6 requires 8 byte alignment for double word accesses */
static char  pdsm_heap[PDSM_DYN_MEMORY] __attribute__ ((aligned (8))); 
#endif

/* Allocation index for heap */
static int   alloc_idx = 0;

/* Buffer pool table */
static pdsm_bpool_type bptab[ PDSM_MAX_NBPOOLS ];

/* Pool information table. This table is used at init time
** to create buffer pools. When more pools are required, this
** table should be edited. Buffer pools should be in the oreder of 
** increasing sizes.
*/
static pdsm_pool_info_s_type pdsm_pool_info[] = 
                { {BPOOL_SIZE_00,BPOOL_NUM_00},
                  {BPOOL_SIZE_01,BPOOL_NUM_01},
                  {BPOOL_SIZE_02,BPOOL_NUM_02},
                  {BPOOL_SIZE_03,BPOOL_NUM_03},
                  {BPOOL_SIZE_04,BPOOL_NUM_04},
                  {BPOOL_SIZE_05,BPOOL_NUM_05},
                  {BPOOL_SIZE_06,BPOOL_NUM_06},
                  {BPOOL_SIZE_MAX,BPOOL_NUM_MAX}  /* End marker, used by for loop
                                                  ** to see the end of table 
                                                  */
                };

/* Current number of pools 
*/
static int    nbpools=0;                    
/*
    mutex that will be used to access nbpools, bptab.
*/ 
static os_MutexBlockType gnss_pdapi_buff_pool_mutex;

/*
 ******************************************************************************
 * Function gnss_pdapi_buff_pool_mutex_init
 *
 * Description:
 *
 *  Mutex initialization to safeguard nbpools, bptab. 
 *  This mutex is created as type MUTEX_DATA_ONLY_CONTEXT.
 *  (Other mutex type - ISR_CONTEXT and TASK_ONLY_CONTEXT are not supported)
 *
 * Parameters:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/

static boolean gnss_pdapi_buff_pool_mutex_init (void)
{
  return (os_MutexInit(&gnss_pdapi_buff_pool_mutex, MUTEX_DATA_ONLY_CONTEXT)); 
}

/* APIs that will lock/unlock gnss_pdapi_buff_pool_mutex */
void gnss_pdapi_buff_pool_mutex_take_api (void)
{
  os_MutexLock(&gnss_pdapi_buff_pool_mutex);
}

void gnss_pdapi_buff_pool_mutex_give_api (void)
{
  os_MutexUnlock(&gnss_pdapi_buff_pool_mutex);
}

/*===========================================================================

FUNCTION PDSM_GETMEM

DESCRIPTION
    This function is used to get RAM space. This interface is done so that if REX
    supports getmem() in future the interfaces of PDSM remain same. Currently it just
    returns the pointer to statically allocated buffer space.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

static char *pdsm_getmem( int size )
{
  char *buffer = NULL;

  if ( (alloc_idx + size) < PDSM_DYN_MEMORY )
  {
    buffer = &pdsm_heap[alloc_idx];  /* return pointer to statically allocated
                                         ** buffer space */
    alloc_idx += size;
  }
  return buffer;

} /* End pdsm_getbuf( ) */

/*===========================================================================

FUNCTION PDSM_MAKE_POOL

DESCRIPTION
    This function is used to make Dynamic buffer pools. 

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK

===========================================================================*/

static boolean pdsm_make_pool(int size, int num)
{
  int    poolid;
  char   *where;
  int paddingsize = 0;

  #ifndef FEATURE_QDSP6
  size = (size + 3) & ~3;  /* Round up to 4 byte boundary */
  #else
  size = (size + 7) & ~7;  /* Round up to 8 byte boundary for Q6 targets */
  #endif


  if ( size < PDSM_BPMINB || size > PDSM_BPMAXB || num > PDSM_BPMAXN )
  {
    /* This error should never happen */
    return FALSE;
  }

/*
  The format of a buffer is as follows for legacy targets:
  4 byte boundary           4n+4 boundary
        |<- poolid (4 bytes) ->|<-  data .......->|

  The format is as follows for QDSP6 targets:
  8 byte boundary        8n+4 boundary              8n+8 boundary
        |<- poolid (4 bytes) ->|<- padding (4 bytes) ->|<- data .... ->|
  pdsm_getmem
*/
  #ifdef FEATURE_QDSP6
  paddingsize = 8 - sizeof(int);
  #endif
  /* Now get memory for this pool. sizeof(int) is added so that enough space
  ** is allowed for storing pool id.
  */
  if (  (where = pdsm_getmem( (size + sizeof(int) + paddingsize)*num) ) == NULL )
  {
    /* Ran out of memory, return error */
    return FALSE;
  }
    
  /* memory allocation was a Success, go ahead and create a pool
  ** of buffers.
  */

  /* To maintain the functionality of previous implementation, we need
     to have a #ifdef for the INTLOCKS. The MUTEX macro maps to TASKLOCK
     and TASKFREE, if INTLOCK is supported. */
  GNSS_PDAPI_BUFF_POOL_MUTEX_TAKE;
    
  /* Increment the num of pools being currently used.
  */
  poolid = nbpools++;             
    
  /* Point to the first available buffer in this pool.
  */
  bptab[poolid].bpnext = (int *)where;
  
  /* Indicate the size of buffer allocated.
  */
  bptab[poolid].bpsize = size;
   
  /* Add size so that pool id can be stored along with the buffer.
  */
  size += sizeof(int) + paddingsize;
    
  /* Now link all the buffers together, last buffer will be linked
  ** at the end of this loop.
  */
  for (num--; num > 0; num--, where += size )
  {
    *( (int *) where) = size +(int)where;
  }
  *( (int *) where) = (int)NULL;   /* Last buffer in the pool */
    
  GNSS_PDAPI_BUFF_POOL_MUTEX_GIVE;
  return TRUE;

} /* End pdsm_make_pool( ) */

#endif /* FEATURE_GNSS_IPC_USE_HEAP */

/*===========================================================================

FUNCTION PDSM_POOL_INIT

DESCRIPTION
    This function is used to initialize the Dynamic buffer pools. 

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK

===========================================================================*/

void pdsm_pool_init ( void )
{
#ifndef FEATURE_GNSS_IPC_USE_HEAP
  int i;
  /* Initializing the pdapi buffer pool mutex */
  if (gnss_pdapi_buff_pool_mutex_init() == FALSE)
  {
    ERR_FATAL("Unable to initialize gnss_pdapi_buff_pool_mutex",0,0,0);
  }

  /* Memory pools storage allocation */
  for ( i=0; i < PDSM_MAX_NBPOOLS; i++ )
  {
    /* check if the size or num of buffer is > 0 */
    if ( (pdsm_pool_info[i].num != 0) && (pdsm_pool_info[i].size != 0) )
    {
      /* Now make pools of size and num specified in pool information data structure.
      */
      if(pdsm_make_pool( (int)pdsm_pool_info[i].size, (int)pdsm_pool_info[i].num) == FALSE)
      {
        /* Indicate fatal error */
        break;  /* Break out, can't make buffer pools */
      } /* If make_pool() failed to create pool */
    } /* End if number and size of pool to create != 0 */
  } /* End for all the buffer pools to be created */
#endif /* FEATURE_GNSS_IPC_USE_HEAP */

} /* End pdsm_pool_init() */

#ifndef FEATURE_GNSS_IPC_USE_HEAP

/*===========================================================================

FUNCTION PDSM_ALLOC

DESCRIPTION
    This function is used for dynamic buffer allocation. It searches
    in the buffer pool corresponding to the size requested. At the most 
    two pools are searched for a free buffer to avoid memory waistage.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static char  *pdsm_alloc( word size )
{
  int i;
  int paddingsize = 0;

  pdsm_bpool_type   *pool_ptr;  /* pointer to pool table */
  char entry        = ' ';     /* Indicate failure to get free buffer */
  int *buffer       = NULL;     /* Buffer pointer to return to user */

  #ifdef FEATURE_QDSP6
  paddingsize = 8 - sizeof(int); /* refer to comments in pdsm_make_pool() */
  #endif

  for (i=0,pool_ptr = &bptab[0]; i < nbpools; pool_ptr++,i++)
  {
    /* Check if pool buffer size fits the requested buffer size 
    */
    if (pool_ptr->bpsize >= size)
    {
      /* Find a free buffer */
      if (pool_ptr->bpnext != NULL)
      { 
        buffer = pool_ptr->bpnext;
        pool_ptr->bpnext = (int *)*buffer; /* Remove this buffer and link the 
                                           ** next free buffer in Q.
                                           */

        *buffer++ = i;  /* Store the poolid in allocated buffer */
        PDSM_MSG_LOW("Allocated buffer size=%d",size,0,0);
        return ((char*)buffer + paddingsize);
      }
      else /* No free buffers in this pool */
      {
         /* If it fails to get buffer in next higher size
         ** pool, then we simply return with NULL pointer.
         */
         if (entry == size )
         { /* Failure in next higher pool */
           PDSM_MSG_ERROR("No free buffer,size=%d,pool=%d",size,pool_ptr->bpsize,0);
           return ((char *)NULL);
         }
         entry = size;    /* Indicate that pool has been found
                          ** and there are no free buffers in pool.
                          */
                
      } /* End else  if(bpnext!=NULL) */
    }/* End if pool buffer size > size requested */
  }/* End For all the buffer pools */

  return ((char *)buffer);

} /* End pdsm_alloc() */

#endif /* FEATURE_GNSS_IPC_USE_HEAP */

/*===========================================================================

FUNCTION PDSM_GETBUF

DESCRIPTION
    This function is used to dynamically allocate a free buffer. It searches
    in the buffer pool corresponding to the size requested. At the most 
    two pools are searched for a free buffer to avoid memory waistage.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
char* pdsm_getbuf (word size)
{
#ifndef FEATURE_GNSS_IPC_USE_HEAP
   char   *buffer;

   GNSS_PDAPI_BUFF_POOL_MUTEX_TAKE;

   buffer = pdsm_alloc(size);

   GNSS_PDAPI_BUFF_POOL_MUTEX_GIVE;
   return buffer;
#else
  return (char*)os_MemAlloc(size, OS_MEM_SCOPE_TASK);
#endif /* FEATURE_GNSS_IPC_USE_HEAP */
} /* pdsm_getbuf() */

/*===========================================================================

FUNCTION PDSM_FREEBUF

DESCRIPTION
    This function is used to dynamically deallocate a used buffer.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean pdsm_freebuf (char* buf)
{
#ifndef FEATURE_GNSS_IPC_USE_HEAP
  int poolid;  
  int *buffer;

  /* Check if poolid is proper in user given buffer pointer.
  */
  if ( buf == NULL)
  {
    return FALSE;
  }

  #ifdef FEATURE_QDSP6
  /* need to account for the padding - refer to pdsm_make_pool */
  buf -= (8 - sizeof(int));
  #endif
  buffer = (int *)buf;
  
  /* extract the poolid from buffer pointer.
  */
  poolid = *(--buffer);

  /* If there is an error in pool id return error.
  */
  if( poolid < 0 || poolid >= nbpools )
  {
    /* Indicate an error here, this can happen only when user is 
    ** trying to deallocate a wrong buffer or the pool id in buffer
    ** got corrupted.
    */
    PDSM_MSG_ERROR("Wrong buffer dealloc,id=%d,ptr=%d",poolid,buf,0);
    return FALSE;
  }
  else
  {
    GNSS_PDAPI_BUFF_POOL_MUTEX_TAKE;
    /* Now link the buffer back to the free pool.
    */
    *buffer = (int)bptab[poolid].bpnext;
    bptab[poolid].bpnext = buffer;
    GNSS_PDAPI_BUFF_POOL_MUTEX_GIVE;
    return TRUE;
  } /* End Else when the pool id is proper. */   
#else
  os_MemFree((void**)&buf);
  return TRUE;
#endif /* FEATURE_GNSS_IPC_USE_HEAP */
} /* End pdsm_freebuf( ) */
