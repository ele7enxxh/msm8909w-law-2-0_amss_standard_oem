
#ifndef LIB_GS_V_H
#define LIB_GS_V_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gs.h_v   1.15   01 Jul 2002 08:52:58   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/gs_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09   MNK     Added ESM Q
06/14/05   kvk     Added prototype for gs_clear_messages()
04/06/05   kvk     Added new Queue for RLC->BMC interfaces
04/04/01   jca     Corrected LINT errors.
04/19/01   TS      Removed references to gs_attached_shared_memory() and
                   gs_free_shared_memory() functions.

06/07/01   CD      Replaced GS_QUEUE_MN (obsolete) by GS_QUEUE_UASMS

07/13/01   CD      Replaced GS_QUEUE_MMI (obsolete) by GS_QUEUE_NOT_USED_3
08/15/01   AB      Renamed all MMI references to CM.
11/21/01   TS      Removed gs_read_message prototype.  Obsolete function.
11/29/01   ATM     Modifications for CFA
02/20/02   ATM     Merge GS_GSM and GS functions together
05/06/02   ATM     Added new QUEUEs for GPRS
07/01/02   jca     Modified function prototype for gs_enquire_timer.
09/18/02   AB      Added gs_count_message() function prototype.
06/08/04   cd      Added queue for LSM
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/24/04   cd      Added queue for FTM_GSM featurized under FEATURE_FTM_GSM_BER
10/10/11   HC      Created local definition of STD_OFFSETOF as aeestd library 
                   is being obsoleted 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "timers_v.h"

#include "gs.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Defines and constants */


typedef boolean                        gs_brief_id_T;
#define GS_BRIEF                       TRUE
#define GS_VERBOSE                     FALSE

typedef byte                           gs_dir_id_T;
#define GS_MIN_DIR                     0
#define GS_DIR_TX                      0
#define GS_DIR_RX                      1
#define GS_MAX_DIR                     1

typedef byte                           gs_task_id_T;
#define GS_MIN_TASK                    1
#define GS_TASK_L1                     1
#define GS_TASK_L2                     2
#define GS_TASK_RR                     3
#define GS_TASK_MM                     4
#define GS_TASK_CM                     5
#define GS_TASK_REG                    6
#define GS_TASK_MN                     7
#define GS_TASK_GxDI                   8
#define GS_TASK_MMI                    9
#define GS_MAX_TASK                    9



typedef byte                           gs_lock_id_T;
#define GS_MIN_LOCK                    1
#define GS_LOCK_CC_TRANSACTION_INFO    1
#define GS_LOCK_SS_TRANSACTION_INFO    2
#define GS_LOCK_SMS_TRANSACTION_INFO   3
#define GS_LOCK_SERVING_CELL_CM_INFO  4
#define GS_LOCK_ERROR_LOG              5
#define GS_LOCK_NVMEM_ACCESS           6
#define GS_LOCK_NVMEM_INITIALISE       7
#define GS_LOCK_OUTPUT_ACCESS          8
#define GS_LOCK_OUTPUT_INITIALISE      9
#define GS_LOCK_NOT_USED               10
#define GS_LOCK_GSDI_LIB               11
#define GS_LOCK_RESOUCE_ALLOCATION     12
#define GS_LOCK_GHDI_SIM               13
#define GS_LOCK_L1_TH                  14
#define GS_MAX_LOCK                    14

typedef byte                           gs_shmem_id_T;
#define GS_MIN_SHMEM                   1
#define GS_SHMEM_SERIAL_OP_BUFFER_1    1
#define GS_SHMEM_SERIAL_OP_BUFFER_2    2
#define GS_SHMEM_SERIAL_IP_BUFFER_1    3
#define GS_SHMEM_SERIAL_IP_BUFFER_2    4
#define GS_SHMEM_CODER_TEST_BUFFER     5
#define GS_SHMEM_TEST_MESSAGE_POOL     6
#define GS_SHMEM_NVMEM                 7
#define GS_SHMEM_OUTPUT                8
#define GS_SHMEM_SIM_BUF               9
#define GS_SHMEM_DS_ISR_CONFIG_DATA    10
#define GS_SHMEM_L1_TSK_ISR_BUFFER     11
#define GS_SHMEM_L1_TSK_ISR_MEAS       12
#define GS_MAX_SHMEM                   12



typedef byte                           gs_int_T;
#define GS_INTERRUPT_ENABLE            0
#define GS_INTERRUPT_DISABLE           1


/* Library private defines. */

#define GS_IS_VALID_QUEUE_ID(queue_id) \
           ((queue_id >= GS_MIN_QUEUE) \
         && (queue_id <= GS_MAX_QUEUE))

#define GS_IS_VALID_LOCK_ID(lock_id) \
           ((lock_id >= GS_MIN_LOCK) \
         && (lock_id <= GS_MAX_LOCK))

#define GS_IS_VALID_SHMEM_ID(sh_mem_id) \
           ((shmem_id >= GS_MIN_SHMEM) \
         && (shmem_id <= GS_MAX_SHMEM))

/*
 * SunOs message queues require that the first dword of the message is not 0.
 * To ensure that this is the case, set a (otherwise unused) bit of MSB to 1
 * for the send, then set it back again after the read.
 */

typedef enum {GS_NO_LOCK = 0, GS_WRITE_LOCK} gs_lock_type_T;

#define GS_DERIVE_KEY(id) \
           ((ftok (getenv ("HOME"), 0) * 0x100) + (id))
#define GS_STATUS_ERRNO_OFFSET         1000

// Temp MACRO define to flush out L1/L2 calls to this function
// DEVELOPERS should use MSG_ERROR if this is really an error
#define gs_fast_log_progress(exception_number,log_type,string) \
           MSG_ERROR("I'm obsolete! %s",string,0,0);
#define gs_raise_warning(exception,code,string) \
           MSG_ERROR("I'm obsolete! %s",string,0,0);

/********************************************************************************

 Created local definition of STD_OFFSETOF as aeestd library is being obsoleted

*********************************************************************************/

#define NAS_STD_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))




word gs_count_message 
(
    gs_queue_id_T gs_queue_id
);

void gs_clear_messages 
(
    gs_queue_id_T gs_queue_id
);



gs_status_T gs_attach_lock
(
   rex_crit_sect_type  *crit_sect
);

gs_status_T gs_lock
(
   rex_crit_sect_type  *crit_sect
);

gs_status_T gs_unlock
(
   rex_crit_sect_type  *crit_sect
);


milli_secs_T gs_get_time (void);

void gs_log_progress
(
   char                               *format,
   ...
);

/* GENERIC SERVICE -- Log message */
/* SIMPLER FORMAT --  SRC & DSTcan be derived by: IMH_T->MessageSet */
/*                    DIRECTION can be derived by QueueId           */
/* CALL IS NOW MADE BY GS_SEND_MESSAGE itself -- no need to call it */
/* in particular tasks -- filtering can be done my message set      */
/* Message length simply saves time since it is calculated already  */
/* by gs_send_message                                               */
void gs_log_message( IMH_T *          msg_ptr,
                     gs_queue_id_T    queue_id,
                     word             message_length );

/* Macro definitions. */

/*
 * A note about time wrapping (0xffffffff to 0).  Even though time
 * (milli_secs_T) wraps, the difference between two times always gives the
 * correct number of milliseconds, regardless of whether either, or both of
 * them have wrapped.
 */

#define GS_DIFF_TIME(Time2,Time1)      ((Time2) - (Time1))

/* External variables */

extern char * queue_name_table[];

#endif /* LIB_GS_H */
