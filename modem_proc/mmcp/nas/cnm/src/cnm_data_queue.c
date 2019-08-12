/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_data_queue.c_v   1.3   12 Mar 2002 13:12:54   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_data_queue.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

12-Mar-2002  VT    Removed Lint errors.

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
#include "mnglobal.h"
#include <stringl/stringl.h>

/*
 * global data declaration
 */

#define MAXNO_CNM_DATA_QUEUE_ENTRIES     32


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

static byte cnm_data_queue_read_ptr_sim[MAX_AS_IDS];
static byte cnm_data_queue_write_ptr_sim[MAX_AS_IDS];
static VOID_DATA *      cnm_data_queue_sim[MAX_AS_IDS][MAXNO_CNM_DATA_QUEUE_ENTRIES];

#define cnm_data_queue_read_ptr (cnm_data_queue_read_ptr_sim[mn_dsda_as_id])
#define cnm_data_queue_write_ptr (cnm_data_queue_write_ptr_sim[mn_dsda_as_id])
#define cnm_data_queue (cnm_data_queue_sim[mn_dsda_as_id])

#else

static byte cnm_data_queue_read_ptr;
static byte cnm_data_queue_write_ptr;

/*
 * The queue contains mmcnm_data_req entries
 */

static VOID_DATA *      cnm_data_queue[MAXNO_CNM_DATA_QUEUE_ENTRIES];

#endif



/**************** Begin of   CNM_init_cnm_data_queue *******************************
 *
 *  Function name: CNM_init_cnm_data_queue()
 *  --------------------------------------
 *  Description:
 *  ------------
 *  This function initialises the cnm_data queue by setting the read and write
 *  pointers to zero.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_init_cnm_data_queue(void)
{

   cnm_data_queue_read_ptr = 0;
   cnm_data_queue_write_ptr = 0;

}

/**************** End of   CNM_init_cnm_data_queue ********************************/


/**************** Begin of   CNM_add_to_cnm_data_queue *****************************
 *
 *  Function name: CNM_add_to_cnm_data_queue()
 *  ----------------------------------------
 *  Description:
 *  ------------
 *  This function adds an entry pointed to by the input parameter to the
 *  cnm_data queue.
 *
 *  Parameters:
 *  -----------
 *  pointer to entry which is to be placed on the top of the queue
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_add_to_cnm_data_queue(mmcnm_data_req_T *cnm_data_queue_entry)
{
   byte        WritePtr;
   int         length;
   VOID_DATA   *ptr;

/*
 * check if queue is full first - if write pointer would become equal
 * to the read pointer after the write, then the queue is already as
 * full as it can be. First calculate what the write pointer would contain
 * after the update
 */


   WritePtr = (unsigned char)((cnm_data_queue_write_ptr+1) % MAXNO_CNM_DATA_QUEUE_ENTRIES);

   if (WritePtr != cnm_data_queue_read_ptr)
   {
      GET_IMH_LEN(length, &cnm_data_queue_entry->header);

      length += (int) sizeof(IMH_T);         /* calculate full message size */
#ifndef FEATURE_MODEM_HEAP
      ptr = gs_alloc( (unsigned int) length );
#else
      ptr = modem_mem_calloc(1,(unsigned int) length , MODEM_MEM_CLIENT_NAS);
#endif
      

      if (ptr != NULL)
      {
         (void) memscpy( ptr,
                        (unsigned int)length,
                        cnm_data_queue_entry,
                        (unsigned int)length );

         /*
          * store pointer to alocated memory where message is stored
          */

         cnm_data_queue[cnm_data_queue_write_ptr] = ptr;

         /*
          * update stored write pointer to point at next location
          */

         cnm_data_queue_write_ptr = WritePtr;

      }
      else
      {
         ERR("gs_alloc failed\n", 0, 0, 0);
      }
   }
   else
   {

      /*
       * queue is full
       */

      ERR("In CNM, cnm_data_queue full \n", 0, 0, 0);
   }
}
/**************** End of   CNM_add_to_cnm_data_queue ********************************/


/**************** Begin of   CNM_pop_cnm_data_queue ********************************
 *
 *  Function name: CNM_pop_cnm_data_queue_entry()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function removes the top entry from the cnm_data queue and returns it
 *  to the caller
 *
 *  Parameters:
 *  -----------
 *  cnm_data_queue_entry : INOUT - pointer to area where top entry of queue
 *                        is to be copied
 *
 *  Returns:
 *  --------
 *  boolean TRUE if queue not empty
 *
 ******************************************************************************/

boolean CNM_pop_cnm_data_queue(mmcnm_data_req_T *cnm_data_queue_entry)
{
   boolean     return_value;
   VOID_DATA   *ptr;
   int         length;

   /*
    * check if queue is not empty first - if write pointer is equal
    * to the read pointer, then the queue is empty
    */


   if (cnm_data_queue_read_ptr == cnm_data_queue_write_ptr)
   {

      /*
       * queue is empty
       */

      return_value = FALSE;
   }
   else
   {

      /*
       * get pointer to alloc'd memory where next message is stored
       */

      ptr = cnm_data_queue[cnm_data_queue_read_ptr];

      /*
       * get length of data
       */

      GET_IMH_LEN( length, (IMH_T *) ptr );

      length += (int) sizeof( IMH_T );

      /*
       * copy it to the output buffer
       */

      (void) memscpy( cnm_data_queue_entry,
                     (unsigned int) length,
                     ptr,
                     (unsigned int) length );


#ifndef FEATURE_MODEM_HEAP
      gs_free( ptr );
#else
      modem_mem_free(ptr, MODEM_MEM_CLIENT_NAS);
#endif


      /*
       * update stored read pointer to point at next location
       */

      cnm_data_queue_read_ptr = (unsigned char)((cnm_data_queue_read_ptr + 1) %
                                              MAXNO_CNM_DATA_QUEUE_ENTRIES);

      return_value = TRUE;
   }
   return(return_value);

}
/**************** End of   CNM_pop_cnm_data_queue ********************************/


/**************** Begin of   CNM_manage_cnm_data_queue *****************************
 *
 *  Function name: CNM_manage_cnm_data_queue()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function deals with requests to send messages over the air
 *  via MM. If the mm link is in the process of reestablishing, then
 *  the message is queued, otherwise it is sent straight away.
 *
 *  Parameters:
 *  -----------
 *  Message to send : IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_manage_cnm_data_queue( mmcnm_data_req_T * message )
{
   /* 
    * check link reestablishing flag
    */

   if ( mm_link_reestablishing != FALSE )
   {
      CNM_add_to_cnm_data_queue( (mmcnm_data_req_T *)message );
   }
   else
   {
      CNM_send_message( ( IMH_T *)message, GS_QUEUE_MM );
   }
}
/**************** End of   CNM_manage_cnm_data_queue ********************************/
