/*===========================================================================

                   T I M E T E S T    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/timetest.c_v   1.2   15 Nov 2001 20:48:24   omichael  $
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/timetest.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/05   src     Reworked the previous modification to correct the order and
                   conditionality of certain hash-includes and to remove a few
                   unnecessary headers.
06/07/05   nl/src  Added hash-include for CUSTOMER header conditionally for
                   time-test builds.
07/25/01   om      Added support for nested interrupts.
07/16/01   om      Fixed includes.
03/08/01   om      Created module.

===========================================================================*/

/* Include TIMETEST header to set the requisite time-test symbols */
#include "timetest.h"

/*** This file is empty if timetest is disabled ***/
#ifdef TIMETEST

#include "memory.h"
#include "msg.h"

#ifdef TIMETEST_PROF


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

unsigned long timetest_parse_token = TT_PARSE_TOKEN;

unsigned char timetest_isr_markers[32];
unsigned char timetest_isr_level = 0;

#ifdef TIMETEST_FUNCS
/* Array to store markers during context switches */
unsigned long timetest_markers[TIMETEST_MAX_TASK_ID][TIMETEST_MAX_FUNC_LEVEL+1];
#endif


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

void timetest_prof_init( void )
{
#ifdef TIMETEST_FUNCS
  memset( timetest_markers, 
      0, 
      TIMETEST_MAX_TASK_ID * TIMETEST_MAX_FUNC_LEVEL+1 );
#endif

  MSG_ERROR( "TIMETEST initialized.", 0, 0, 0 );
}

#endif  /* TIMETEST_PROF */

#endif  /* TIMETEST */



















