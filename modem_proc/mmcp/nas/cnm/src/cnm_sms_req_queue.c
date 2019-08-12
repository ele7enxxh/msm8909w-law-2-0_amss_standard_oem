/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_req_queue.c_v   1.2   14 Aug 2001 15:17:42   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_req_queue.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

02/11/06   NR      Fixed lint errors
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

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "mnglobal.h"
#include <stringl/stringl.h>

/*
 * global data declaration
 */
#define  MAXNO_SMS_QUEUE_ENTRIES 8

typedef  struct sms_request_queue_entry_tag
{
   cp_data_msg_T           cp_data_req;
   connection_id_T         connection_id;
} sms_request_queue_entry_T;


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

#ifndef FEATURE_MODEM_HEAP
static sms_request_queue_entry_T sms_request_queue_sim[MAX_AS_IDS][MAXNO_SMS_QUEUE_ENTRIES];
#else
static sms_request_queue_entry_T *sms_request_queue_sim[MAX_AS_IDS][MAXNO_SMS_QUEUE_ENTRIES];
#endif
static byte    sms_queue_write_ptr_sim[MAX_AS_IDS];
static byte    sms_queue_read_ptr_sim[MAX_AS_IDS];

#define sms_request_queue (sms_request_queue_sim[mn_dsda_as_id])
#define sms_queue_write_ptr (sms_queue_write_ptr_sim[mn_dsda_as_id])
#define sms_queue_read_ptr (sms_queue_read_ptr_sim[mn_dsda_as_id])

#else

#ifndef FEATURE_MODEM_HEAP
static sms_request_queue_entry_T sms_request_queue[MAXNO_SMS_QUEUE_ENTRIES];
#else
static sms_request_queue_entry_T *sms_request_queue[MAXNO_SMS_QUEUE_ENTRIES];
#endif


static byte    sms_queue_write_ptr;
static byte    sms_queue_read_ptr;

#endif

/**************** Begin of   CNM_init_sms_request_queue ***************************
 *
 *  Function name: CNM_init_sms_request_queue ()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function initialises the sms request queue
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

VOID_FUNC CNM_init_sms_request_queue ( void )
{
   sms_queue_write_ptr = 0;
   sms_queue_read_ptr = 0;
}
/**************** End of   CNM_init_sms_request_queue ***************************/


/**************** Begin of   CNM_add_to_sms_request_queue *************************
 *
 *  Function name: CNM_add_to_sms_request_queue ()
 *  ---------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function adds a new entry to the sms request queue
 *
 *  Parameters:
 *  -----------
 *  cp_data_msg : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_add_to_sms_request_queue ( cp_data_msg_T *cp_data_msg,
                                        connection_id_T connection_id )
{
   byte  write_ptr;

   write_ptr = (byte) ((sms_queue_write_ptr + 1) % MAXNO_SMS_QUEUE_ENTRIES);

   /*
    * check that queue will not overflow
    */

   if ( write_ptr != sms_queue_read_ptr )
   {
#ifdef FEATURE_MODEM_HEAP
      sms_request_queue[sms_queue_write_ptr] = (sms_request_queue_entry_T*)modem_mem_calloc(1, sizeof(sms_request_queue_entry_T),MODEM_MEM_CLIENT_NAS);
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALLOC - sms_request_queue[%d]\n", sms_queue_write_ptr );
      if(sms_request_queue[sms_queue_write_ptr] != NULL)
      {
         (void)memscpy( (void *) &sms_request_queue[sms_queue_write_ptr]->cp_data_req,
                       sizeof(cp_data_msg_T),
                       (const void *) cp_data_msg,
                        sizeof(cp_data_msg_T) );

         sms_request_queue[sms_queue_write_ptr]->connection_id = connection_id;

         sms_queue_write_ptr = write_ptr;
      }
      else
      {
           MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Memory allocation failure for sms_request_queue[%d]\n", sms_queue_write_ptr, 0, 0 );
      }
#else
      (void)memscpy( (void *) &sms_request_queue[sms_queue_write_ptr].cp_data_req,
                      sizeof(cp_data_msg_T),
                      (const void *) cp_data_msg,
                       sizeof(cp_data_msg_T) );

      sms_request_queue[sms_queue_write_ptr].connection_id = connection_id;

      sms_queue_write_ptr = write_ptr;
#endif
   }
   else
   {
      ERR("SMS queue full - cp data msg discarded\n",
              0, 0, 0);
   }
}
/**************** End of   CNM_add_to_sms_request_queue ***************************/


/**************** Begin of   CNM_read_sms_request_queue ***************************
 *
 *  Function name: CNM_read_sms_request_queue ()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function reads and removes the top entry from the sms request queue
 *
 *  Parameters:
 *  -----------
 *  p_cp_data_msg : IN
 *  p_connection_id : IN
 *
 *  Returns:
 *  --------
 *  entry_present
 *
 ******************************************************************************/

boolean CNM_read_sms_request_queue ( cp_data_msg_T *p_cp_data_msg,
                                    connection_id_T *p_connection_id )
{

   boolean   entry_present;

   /*
    * check that queue is not empty
    */

   if ( sms_queue_write_ptr == sms_queue_read_ptr )
   {
      entry_present = FALSE;
   }
   else
   {
#ifdef FEATURE_MODEM_HEAP
      if(sms_request_queue[sms_queue_read_ptr] != NULL)
      {
        (void)memscpy((void *) p_cp_data_msg,
                   sizeof(cp_data_msg_T),
                   (const void *) &sms_request_queue[sms_queue_read_ptr]->cp_data_req,
                    sizeof(cp_data_msg_T) );

        *p_connection_id = sms_request_queue[sms_queue_read_ptr]->connection_id;
        modem_mem_free(sms_request_queue[sms_queue_read_ptr], MODEM_MEM_CLIENT_NAS);
        sms_request_queue[sms_queue_read_ptr] = NULL;
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - sms_request_queue[%d]\n", sms_queue_read_ptr);
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot read from sms queue as sms_request_queue[%d] is NULL", sms_queue_read_ptr );
      }

#else
      (void)memscpy((void *) p_cp_data_msg,
                   sizeof(cp_data_msg_T),
                   (const void *) &sms_request_queue[sms_queue_read_ptr].cp_data_req,
                    sizeof(cp_data_msg_T) );

      *p_connection_id = sms_request_queue[sms_queue_read_ptr].connection_id;
#endif
      
      sms_queue_read_ptr = (unsigned char) ((sms_queue_read_ptr + 1) % MAXNO_SMS_QUEUE_ENTRIES);
      entry_present = TRUE;
   }

   return entry_present;
}
/**************** End of   CNM_read_sms_request_queue ***************************/

