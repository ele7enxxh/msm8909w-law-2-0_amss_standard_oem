#ifndef TIME_H
#define TIME_H
/*===========================================================================
  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/qsee/qsee_time.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/14   rk      Included new header file time_msg.h
02/04/14   rk      Moved out of tzdrm
02/18/11   CM      Initial Version. 
03/04/11   jct     Defined command structures for Time service functions 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup playready
  @} */

//#include "comdef.h"
#include "time_msg.h"

#define TZ_MAX_TIME_NAME_LEN    256
#define TZ_MAX_TIME_DATA_LEN    18000

/* The norm_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a non-leap year. 
 */
static const uint16 norm_month_tab[] = {
        0,                                    /* --- */
	31,                                   /* Jan */
	31+28,                                /* Feb */
	31+28+31,                             /* Mar */
	31+28+31+30,                          /* Apr */
	31+28+31+30+31,                       /* May */
	31+28+31+30+31+30,                    /* Jun */
	31+28+31+30+31+30+31,                 /* Jul */
	31+28+31+30+31+30+31+31,              /* Aug */
	31+28+31+30+31+30+31+31+30,           /* Sep */
	31+28+31+30+31+30+31+31+30+31,        /* Oct */
	31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
	31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

int time_is_leap_year(unsigned int year);
int time_getutcsec(struct tztimespec *tzTimeSpec);
int time_getsystime(struct tztm *tzTime);
int time_getmsec(void);

/*
Custom Function
*/
int time_end(void);

#endif //TIME_H
