#ifndef ULPN_H
#define ULPN_H
/*===========================================================================

                   U L P N    H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used by the User Long PN services.

   It is used only in the portable subscriber station.

  Copyright (c) 1993 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/ulpn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ab      Fix compilation errors
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
11/12/09   bb      Added support for "LTE to 1X IRAT Core" feature
                   implementation
09/14/09   bb      Code changes for slew state optimization feature
04/01/09   adw     Categorized included header files and standardized format.
03/26/09   adw     Removed modem_1x_defs.h include.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
10/18/06   bn      Added new function prototype ulpn_advance_chips
12/03/02   bt      cleanup of the interface. All advance/retard func. were
                   moved into ulpn_adjust.
07/30/02   gs      Initial merge into perforce for cougar.
03/12/01   dgy     Added FEATURE_IS2000_QPCH_CCI.
11/18/99   ajn     Enhanced Standby III ... 320ms based ULPN calculations
08/06/99   ejv     ajn: Externed function ulpn_quarter_advance.
01/13/99   ejv     Removed _cdecl - not necessary for ARM.
09/01/98   thh     Added keyword _cdecl to the declarations of those functions
                   which are called by assembly codes using C function calling
                   convention.
                   Updated copyright date.
03/02/93   ptw     Created.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define ROLLS_MAX_ADJUST       24575
  /* Max Number of rolls that the ULPN can be adjusted by*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      Declaration of Data Type for User Long PN Code
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
  {
  dword  low;
  word   high;
  } ulpn_type;
   /* This structure of 48 bits is used to handle the 42-bit user long
      code PN states.  Most arithmetic on these numbers is done 48 bits
      wide for simplicity. */


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION ulpn_adjust

DESCRIPTION
  This function computes the user long PN code state at some time
  in the future/past, given a current user long PN code state and the
  number of intervening 26.666... ms periods.

DEPENDENCIES
  The function depends on the magic properties of the pn_advance/retard
  array. (See comment at pn_advance/retard array declarations.)

RETURN VALUE
  No formal return value, but the ULPN state at *pn_state_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ulpn_adjust
(
  ulpn_type *pn_state_ptr,
    /* Pointer to the value of the user long PN;
       At entry: the old value
       At exit: the new value */

  int16 move
    /* Number of rolls to move the long code by */
);

/*===========================================================================

FUNCTION ULPN_QUARTER_ADVANCE

DESCRIPTION
  Advances the PN value by 1/4 PN cirle.

DEPENDENCIES
  The function depends on the magic properties of the pn_advance array.
  (See comment at pn_advance array declaration.)

RETURN VALUE
  No formal return value, but the ULPN state at *new_pn_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None

===========================================================================*/
extern void ulpn_quarter_advance
(
  ulpn_type *pn_state_ptr
    /* Pointer to the ULPN to be changed */
);

/*===========================================================================

FUNCTION ULPN_3_QUARTER_RETARD

DESCRIPTION
  Retards the PN value by 3/4 PN ROLL in time.

DEPENDENCIES

RETURN VALUE
  No formal return value, but the ULPN state at *new_pn_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None

===========================================================================*/
extern void ulpn_3_quarter_retard
(
  ulpn_type *pn_state_ptr
    /* Pointer to the ULPN to be changed */
);

/*===========================================================================

FUNCTION       ULPN_ADVANCE_CHIPS

DESCRIPTION

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void ulpn_advance_chips
(
  ulpn_type *pn_state_ptr,
    /* Pointer to the value of the user long PN;
       At entry: the old value
       At exit: the new value */

  uint64 advance_in_chips
    /* Number of chips to adjust the long code by */
);

#endif /* ULPN_H */
