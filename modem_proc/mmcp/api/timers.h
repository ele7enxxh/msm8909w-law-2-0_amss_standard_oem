#ifndef TIMERS_H
#define TIMERS_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/timers.h_v   1.22   03 Jul 2002 12:50:56   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/timers.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/10   MNK     Add T3417_EXT timer
05/17/10   zren    Updated EMM to only process EMM timers
04/13/10   MNK     Add ISR deactivate timer T3423 for LTE
02/03/10   zren    Added ESM new timer machanism
11/30/09   zren    Modified access barring timeout handling
05/29/09   zren    Added support for TAU feature
05/08/09   MNK     Added new ESM timers T3480,T3481,T3482 and T3492
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
10/29/02   jar     Increased GSDI_READ_TIMER_VALUE from 10 seconds to 180 
                   seconds for GCF Mandatory 11.10 SIM Test Case 27.11.1.3c
04/04/01   jca     Fixed LINT errors.
05/03/01   jca     Added GMM timers.
06/11/01   TS      Added GSM build support.
08/14/01   AB      Renamed connection manager reference to CNM from CM.
08/15/01   AB      Renamed all MMI references to CM.
10/24/01   VT      DTMF: Added TIMER_T336 & TIMER_T337 and removed TIMER_DTMF_ON & TIMER_DTMF_OFF
02/05/02   rmc     Renamed RR cell reselection timers. Added new RR_RESELECTION_SI_TIMER.
02/12/02   jca     Removed timer TIMER_RR_ABORT.
03/05/02   JC      Eliminated legacy L1 timer declarations.
04/19/02   CD      Added CNM_MM_REL_TIMER
04/22/02   CD      Changed value of TR2M expiration from 1000 to 19000
04/30/02   ks      Added TIMER_DETACH_REL definition
05/30/02   JAC     added L2_SEND_DCCH timer
06/02/02   Rao     Getting rid of Pathloss timer and adding 5 Sec Timers for
                   Serving & Surr Cells.
06/16/02   ks      Added timers for GPRS
07/03/02   jca     Deleted timers TIMER_PLMN_SELECT and TIMER_PLMN_LIST.
08/14/02   rmc     Removed RR timer definitions from this file
12/09/03   ks      Setting MIN_MM_TIMER to TIMER_T3210 and added comments.
04/06/04   cd      LCS support. Added TIMER_T3241
08/12/04   ab      Link control support. Added TLC1M.
12/17/04   vdr     Added definitions for TIMER_T3218 & TIMER_T3316
08/08/06   NR      Chaning the SMS_TR1M_TIMEOUT_VALUE & SMS_TR2M_TIMEOUT_VALUE values to 
                   spec compliant values
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/




/*
 * Timer Ids.
 ************
 */

typedef byte                           timer_id_T;
/*
** GSDI TIMERS
*/

#define GS_TIMER_GSDI_TASK             0x01
#define GS_TIMER_GSDI_PING             0x02

#define GSDI_TASK_TIMER_VALUE          20000  /* 20 secs */
#define GSDI_READ_TIMER_VALUE          180000 /* 180 secs */
#define PING_SIM_PERIOD                25000  /* 25 secs, used in phone call */

/*******************************/
/* Timers for the layer 2 task */
/*******************************/

#define SAPI0_T200                      1
#define SAPI3_T200                      2
#define L2_SEND_DCCH                    3


#define TIMER_MODIFY_CANCEL            0
#define TIMER_MODIFY_PAUSE             1
#define TIMER_MODIFY_RESUME            2
#define TIMER_MODIFY_ENQUIRE           3




#define MAXNO_TIMERS 127


/*
 * Definitions for timer modification
 */

typedef byte                           timer_modify_action_T;


typedef struct{

        word data;
        word timer_id;

} timer_data_T;

#endif /* TIMERS_H */
