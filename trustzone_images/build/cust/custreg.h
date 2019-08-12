#ifndef CUSTREG_H
#define CUSTREG_H
/*===========================================================================

            " C u s t - R E G "   H E A D E R   F I L E

DESCRIPTION
  Configuration for REG Features.

  Copyright (c) 2000 - 2002 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custreg.h#1 $  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/06   hn      Removed C++ style comments for L4 kernel build.
10/25/02   sb      Initial revision.

===========================================================================*/

/*---------------------------------------------------------------------------
                            REG  Services
---------------------------------------------------------------------------*/

/* #define FEATURE_REG_DEEP_SLEEP */

#ifdef FEATURE_REG_DEEP_SLEEP

/*
** These constants are used to control the behavior of the deep sleep
** algorithm implemented in REG.  The deep sleep algorithm uses a timer
** to periodically wake up and search for service whenever the MS
** is unable to find service.  The timer is initially set to the
** default value specified by REG_TIMERS_DEFAULT_PLMN_SEARCH_PERIOD.
** The timer is set to this value REG_TIMERS_DEFAULT_PLMN_SEARCH_PERIOD_COUNT
** number of times.  Once the timer has been set more
** than REG_TIMERS_DEFAULT_PLMN_SEARCH_PERIOD_COUNT times,
** it is incremented by REG_TIMERS_PLMN_SEARCH_PERIOD_INCREMENT until
** it reaches the upper bound specified by REG_TIMERS_PLMN_SEARCH_PERIOD_MAX.
*/
#define REG_TIMERS_DEFAULT_PLMN_SEARCH_PERIOD         15       /* seconds */
#define REG_TIMERS_DEFAULT_PLMN_SEARCH_PERIOD_COUNT   12
#define REG_TIMERS_PLMN_SEARCH_PERIOD_INCREMENT       15       /* seconds */
#define REG_TIMERS_PLMN_SEARCH_PERIOD_MAX             180      /* seconds */

#endif

#endif /* CUSTREG_H */
