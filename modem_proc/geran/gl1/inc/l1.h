#ifndef L1_H
#define L1_H
/*===========================================================================

          G S M   L 1   S E R V I C E S  H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains declarations associated with the GSM L1 services module.


Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09-02-19   tjw     Automatically split from original header file and then tidied.

===========================================================================*/

/*===========================================================================

FUNCTION GL1_ENTER_LOW_POWER_MODE

DESCRIPTION
  Force L1 hardware into low power mode.
  This function will not have any affect unless L1 is inactive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gl1_enter_low_power_mode( void );

#endif /* L1_H */
