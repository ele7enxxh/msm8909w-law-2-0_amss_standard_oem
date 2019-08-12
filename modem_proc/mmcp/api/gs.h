#ifndef LIB_GS_H
#define LIB_GS_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gs.h_v   1.15   01 Jul 2002 08:52:58   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gs.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "timers.h"
#include "sys_type.h"
#include "environ.h"
#include "queue.h"         // for q_type


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Defines and constants */

typedef dword                          gs_status_T;
#define GS_SUCCESS                     0
#define GS_WOULD_BLOCK                 1
#define GS_MESSAGE_TOO_BIG             2
#define GS_INVALID_ID                  3
#define GS_NOT_INITIALISED             4
#define GS_FAILURE                     5


typedef byte                           gs_queue_id_T;
#define GS_MIN_QUEUE                   1
#define GS_QUEUE_LAYER_1               1
#define GS_QUEUE_LAYER_2               2
#define GS_QUEUE_RR                    3
#define GS_QUEUE_MM                    4
#define GS_QUEUE_CM                    5 /* call manager */
#define GS_QUEUE_UASMS                 6 
#define GS_QUEUE_MN_CM                 7
#define GS_QUEUE_REG                   8
#define GS_QUEUE_LAYER_2_2             9  /* used to be GS_QUEUE_MMI */
#define GS_QUEUE_TIMERS                10
#define GS_QUEUE_CHECK_LOWER_RESPONSES 11 /* Test harness. */
#define GS_QUEUE_CHECK_UPPER_RESPONSES 12 /* Test harness. */
#define GS_QUEUE_LOG_RESULTS           13 /* Test harness. */
#define GS_QUEUE_SEND_TEST_MESSAGES    14 /* Test harness. */
#define GS_QUEUE_GHDI_TASK             15
#define GS_QUEUE_GHDI_KEYPRESS         16 /* Used only by GHDI, on Unix. */
#define GS_QUEUE_TEST_HARN_FRONT_END   17 /* Test harness. */
#define GS_QUEUE_DS                    18
#define GS_QUEUE_NOT_USED_1            19
#define GS_QUEUE_NOT_USED_2            20
#define GS_QUEUE_GSDI_TASK             21
#define GS_QUEUE_GSDI_LIB              22
#define GS_QUEUE_CB                    23
#define GS_QUEUE_SERIAL_OUT            24
#define GS_QUEUE_SERIAL_IN             25
#define GS_QUEUE_DIAG                  26
#define GS_QUEUE_GHDI_SIM              27
#define GS_QUEUE_SIM_POLL              28
#define GS_QUEUE_SM                    29
#define GS_QUEUE_RRC                   30
#define GS_QUEUE_MAC                   31
#define GS_QUEUE_RLC_UL                32
#define GS_QUEUE_RLC_DL                33
#define GS_QUEUE_LLC                   34
#define GS_QUEUE_SNDCP                 35
#define GS_QUEUE_LSM                   36
#define GS_QUEUE_FTM_GSM               37

#define GS_QUEUE_RLC_CB                38
#define GS_QUEUE_ESM                   39
#define GS_QUEUE_TDSRRC                40


#define GS_QUEUE_TDSRLCUL              41
#define GS_QUEUE_TDSRLCDL              42
#define GS_QUEUE_TDSMACHS              43
#define GS_QUEUE_TDSL1                 44
#define GS_QUEUE_TDSMACUL              45
#define GS_QUEUE_TDSMACDL              46
#define GS_QUEUE_TDSOTHER              47

#define GS_QUEUE_TC                    48
#define GS_QUEUE_MN_CM_1               49

#define GS_QUEUE_MM_SUB1               50
#define GS_QUEUE_MM_SUB2               51
#define GS_QUEUE_LAYER_2_3             52

#define GS_MAX_TIMER_CLIENT            53 /* this bounds the timer_info array */

#define GS_QUEUE_LAYER_1_2             53
#define GS_QUEUE_RR_2                  54
#define GS_QUEUE_MAC_2                 55
#define GS_QUEUE_RLC_UL_2              56
#define GS_QUEUE_RLC_DL_2              57
#define GS_QUEUE_LLC_2                 58
#define GS_QUEUE_SNDCP_2               59
#define GS_QUEUE_GRM                   60

/* Defining these Queue IDs for TSTS */
#define GS_QUEUE_LAYER_1_3             61
#define GS_QUEUE_RR_3                  62
#define GS_QUEUE_MAC_3                 63
#define GS_QUEUE_RLC_UL_3              64
#define GS_QUEUE_RLC_DL_3              65
#define GS_QUEUE_LLC_3                 66
#define GS_QUEUE_SNDCP_3               67

#define GS_MAX_QUEUE                   68


/* This value was emperically derived   */
/* I set up a call and kept counters on */
/* maximum queue size after each send   */
/* values did not exceed 3              */
/* -Tyler 2/2002                        */
#define GS_DEFAULT_QUEUE_SIZE           5



typedef byte                           gs_log_type_T;
#define GS_LOG_TYPE_NOTHING            0 /* Initialise only. */
#define GS_LOG_TYPE_ALL                1 /* Configuration only. */
#define GS_LOG_TYPE_WARNING            2
#define GS_LOG_TYPE_TASK_PROGRESS      3
#define GS_LOG_TYPE_TEST_RESULT        4
#define GS_LOG_TYPE_HIGH_LEVEL_TONE    5
#define GS_LOG_TYPE_LOW_LEVEL_TONE     6
#define GS_LOG_TYPE_TRANS_CHANNEL_OP   7





/* Function Prototypes */

gs_status_T gs_send_message
(
   gs_queue_id_T                       queue_id,
   void                               *msg_p,
   boolean                             wait
);

void gs_read_messages
(
   gs_queue_id_T                      queue_id
);


/* Define a PUT Handler Function Pointer Type */
typedef gs_status_T (*gs_put_handler_ptr)( void *msg );

/* Define a GET Handler Function Pointer Type */
typedef void (*gs_get_handler_ptr)( void *msg );

/* Registration Functions */
gs_status_T gs_init
(
   gs_queue_id_T      gs_queue_id,
   rex_tcb_type      *tcb_ptr,
   rex_sigs_type      cmd_q_sig,
   q_type            *rcv_q_ptr,
   q_type            *rcv_free_q_ptr,
   unsigned int       buffer_size,
   byte               high_watermark,
   gs_get_handler_ptr process_routine
);

// Simpler registration function
// for layers that are only providing
// gs_send_message() for other layers
gs_status_T gs_init_for_send_only
(
   gs_queue_id_T      gs_queue_id,
   gs_put_handler_ptr enqueue_routine
);


gs_status_T   gs_start_timer
(
   gs_queue_id_T       queue_id,
   timer_id_T          timer_id,
   rex_timer_cb_type   func_ptr,        
   milli_secs_T        milli_secs,
   byte                data
);

typedef void (*gs_non_def_timer_cb_ptr)( unsigned long param );

/* Generic timer start function. Takes extra parameter to specify if 
   the timer is non-deferrable */
gs_status_T   gs_start_timer_ext
(
   gs_queue_id_T             queue_id,
   timer_id_T                timer_id,
   gs_non_def_timer_cb_ptr   func_ptr,        
   milli_secs_T              milli_secs,
   byte                      data,
   boolean                   non_deferrable
);

void gs_clear_timer(gs_queue_id_T     queue_id,
                                    timer_id_T        timer_id);

/* Generic timer clear function. Takes extra parameter to specify if 
   the timer is non-deferrable */
void gs_clear_timer_ext
(
   gs_queue_id_T     queue_id,
   timer_id_T        timer_id,
   boolean           non_deferrable
);

gs_status_T gs_cancel_timer
(
   gs_queue_id_T                       queue_id,
   timer_id_T                          timer_id
);

/* Generic timer cancel function. Takes extra parameter to specify if 
   the timer is non-deferrable */
gs_status_T gs_cancel_timer_ext
(
   gs_queue_id_T     queue_id,
   timer_id_T        timer_id,
   boolean           non_deferrable
);

gs_status_T gs_modify_timer
(
   gs_queue_id_T                       queue_id,
   timer_id_T                          timer_id,
   timer_modify_action_T               action

);

/* Generic timer modify function. Takes extra parameter to specify if 
   the timer is non-deferrable */
gs_status_T gs_modify_timer_ext
(
   gs_queue_id_T            queue_id,
   timer_id_T               timer_id,
   timer_modify_action_T    action,
   boolean                  non_deferrable
);

milli_secs_T gs_enquire_timer
(
   gs_queue_id_T                       queue_id,
   timer_id_T                          timer_id
);

/* Generic timer enquire function. Takes extra parameter to specify if 
   the timer is non-deferrable */
milli_secs_T gs_enquire_timer_ext
(
   gs_queue_id_T     queue_id,
   timer_id_T        timer_id,
   boolean           non_deferrable
);

void *gs_alloc
(
   unsigned int                        byte_size
);

void gs_free
(
   void                          *ptr
);


/* Macro definitions. */
#define GS_IS_UNDEFINED(variable) \
(variable==(((((unsigned long int)(1)<<((sizeof(variable)*8)-1))-1)*2)|1))

#define GS_SET_UNDEFINED(variable)  \
(variable=(((((unsigned long int)(1)<<((sizeof(variable)*8)-1))-1)*2)|1))

#define GS_IS_NOT_UNDEFINED(variable) ( ! GS_IS_UNDEFINED(variable))



#endif /* LIB_GS_H */
