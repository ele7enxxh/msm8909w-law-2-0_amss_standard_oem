
/*!
  @file
  emm_timer_sigs.h

  @brief
  EMM internal Timer and REX Signal definitins.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_timer_sigs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/10   MNK     CSFB changes
10/06/09   vdr     Compiler warnings fixed
09/25/09   hnam    Added TLB support
05/29/09   zren    Added support for TAU feature
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Added support for service request and paging feature
===========================================================================*/
#ifndef _EMM_TIMER_SIGS_H_
#define _EMM_TIMER_SIGS_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "rex.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/
//#define MM_CMD_Q_SIG                  1      /* Command queue signal                          */
//#define MM_DOGRPT_TIMER_SIG           2      /* Watchdog report signal                        */
//#define MM_NVIF_SIG                   4      /* NV interface signal                           */

#define EMM_STOP_SIG                  8


#define EMM_T3411_TIMER_DURATION      10000  /* */



#define EMM_POWEROFF_DETACH_TIMER_DURATION     5000
#define MAX_POWEROFF_ATTEMPTS 4
#define MAX_POWEROFF_SLEEP_DURATION 1000


#define EMM_T3421_TIMER_DURATION    15000  /* */
#define UIM_EMM_CMD_Q_SIG             16384


#define EMM_DETACH_GUARD_TIMER_SIG    512  /* */

#define ESM_EMM_CMD_Q_SIG             1024
#define RRC_EMM_CMD_Q_SIG             2048
#define TLB_EMM_CMD_Q_SIG             262144

#define CSFB_EMM_CMD_Q_SIG            0x80000

#if defined (FEATURE_LTE_REL9)
#define GPS_EMM_CMD_Q_SIG            32768
#endif /* FEATURE_LTE_REL9*/ 

#define EMM_T3410_TIMER_SIG           4096   /* Attach Guard Timer */

#define EMM_NVIF_SIG                  8192

#define EMM_T3410_TIMER_DURATION      15000  /* In msec            */

#define EMM_T3417_TIMER_DURATION      5000  /* In msec */

#define EMM_T3440_TIMER_DURATION      10000  /* In msec */

#define EMM_T3430_TIMER_DURATION      15000 /* In msec */

#define EMM_T3417_EXT_TIMER_DURATION      10000  /* In msec */

#define EMM_FORBIDDEN_TAI_TIMER_DURATION  12*60*60*1000  /* In msec */

#define EMM_PS_DETACH_TIMER_DURATION  78000 /* In msec */

#endif /* _EMM_TIMER_SIGS_H_ */

#endif /*FEATURE_LTE*/

