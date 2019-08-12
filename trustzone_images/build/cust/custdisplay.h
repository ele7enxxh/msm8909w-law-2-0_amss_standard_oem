#ifndef CUSTDISPLAY_H
#define CUSTDISPLAY_H
/*===========================================================================

                  D I S P L A Y   H E A D E R    F I L E

DESCRIPTION
  Configuration for SURF Target for display.
  This particular build supports display related features.

  Copyright (c) 2006, 2007   by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custdisplay.h#1 $DateTime:  $Author: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/08    dw      Include all display related feature names
12/15/06    dw      Initial version

===========================================================================*/

/* MDP SW support */
#define FEATURE_MDP

/* MDP 2.x SW support */
#define FEATURE_MDP2

/* This feature is used to reduce power consumption by dynamically 
** adjusting backlight levels during YCbCr video full screen updates */
#define FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT

/* Display profiling support */
#define FEATURE_DISPLAY_PROFILING

/* Enable display power profiling */
#define FEATURE_DISPLAY_POWER_DEBUG

/* Enable display disp2d interface */
#define FEATURE_DISPLAY_DISP2D_INTERFACE

/* Enable MDDI driver support */
#define FEATURE_MDDI

/* Enable TV Encoder support */
#define FEATURE_TVENC

/* Enable QMDP test application */
#define FEATURE_APP_QMDP

/* Enable display task */
#define FEATURE_DISP_TASK

#endif /* CUSTDISPLAY_H */

