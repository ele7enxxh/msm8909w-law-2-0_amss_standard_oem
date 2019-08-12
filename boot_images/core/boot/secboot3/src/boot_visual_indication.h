#ifndef BOOT_VISUAL_INDICATION_H
#define BOOT_VISUAL_INDICATION_H
/*=============================================================================

                        Boot Visual Indication

GENERAL DESCRIPTION
  This file defines SBL visual indication functions

Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_visual_indication.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/09/13   dh      Initial creation
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function turn on/off the blue led according to input
 * @return None.
 *
 */ 
boot_boolean boot_enable_led(boot_boolean led_on);


/*===========================================================================

**  Function :  boot_toggle_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to toggle LED
 * @return TRUE if operation is successful, FALSE if not.
 *
 */ 
boot_boolean boot_toggle_led(void);


/*===========================================================================

**  Function :  boot_toggle_led_init

** ==========================================================================
*/
/*! 
 * @brief This function initializes the timer count for led toggling
 * @return None.
 *
 */ 
void boot_toggle_led_init(void);


#endif  /* BOOT_VISUAL_INDICATION_H */

