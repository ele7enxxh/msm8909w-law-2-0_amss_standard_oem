#ifndef MS_TIMER_V_H
#define MS_TIMER_V_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ms_timer.h_v   1.3   31 Oct 2001 13:02:30   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/ms_timer_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/01   jca     Fixed Lint errors.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "ms_timer.h"
#include "timers_v.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/*******************************************************************************
 *
 * Message Structures
 *
 ******************************************************************************/

/*
 * MID_START_TIMER_REQUEST
 */

#ifdef HOST

typedef struct start_timer_request_S
{
   IMH_T                               IMH;
   gs_queue_id_T                       queue_id;
   timer_id_T                          timer_id;
   milli_secs_T                        milli_seconds;
   byte                                data;
} start_timer_request_T;

#endif

#ifdef RTXC

typedef struct start_timer_request_S
{
   IMH_T                               IMH;
   dword                               ticks;
   byte                                entry;
} start_timer_request_T;

#endif

/*
 * MID_MODIFY_TIMER_REQUEST
 */

#ifdef HOST

typedef struct modify_timer_request_S
{
   IMH_T                               IMH;
   gs_queue_id_T                       queue_id;
   timer_id_T                          timer_id;
   timer_modify_action_T               action;
} modify_timer_request_T;

#endif

#ifdef RTXC

typedef struct modify_timer_request_S
{
   IMH_T                               IMH;
   byte                                entry;
   timer_modify_action_T               action;
} modify_timer_request_T;

#endif

/*
 * MID_TIMER_EXPIRY
 */


typedef struct timer_enquiry_S
{
   IMH_T                               IMH;
   timer_id_T                          timer_id;
   dword                               num_milli_secs;
} timer_enquiry_T;

#ifdef RTXC

typedef struct timeout_block_tag
{
   struct timeout_block_tag   *flink;
   struct timeout_block_tag   *blink;
   dword                      timeout;
   byte                       timer_entry_id;
   byte                       status;
} timeout_block_T;

#endif

#endif /* MS_TIMER_H */

