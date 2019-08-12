#ifndef DISTREG_H
#define DISTREG_H
/*===========================================================================

           D I S T A N C E - B A S E D    R E G I S T R A T I O N

DESCRIPTION
  This header file contains general definitions for the distance-based
  registration module.

  Since the registration parameters are 22 bit fields of 0.25 second
  per lsb and the distance measurements must be within 5% of the correct
  value across a wide range of values, even restricted lookup tables
  would be too large. Thus algorithmic approaches are taken to determine
  the square root and cosine involved in the distance calculation.

  Care should be taken that the routines should not be called too often
  in real-time as they are time consuming. It is assumed that distance varies
  more on the order of minutes than fractions of a second and the change
  in distance should be evaluated accordingly.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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

  $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/distreg.h_v   1.0.2.0   30 Nov 2001 16:55:18   fchan  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/distreg.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
05/21/04   fc      Lint clean up.
03/22/93   gb      Created File.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                        STANDARD CONSTANTS
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         TYPES
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          MACROS
---------------------------------------------------------------------------*/

#define BIT22   0x200000
#define XTEND22 0xFFE00000

  /* sign extend 22 bit signed integer to signed long */
#define EXTEND22(dword22) \
  ( long ) (( dword22 & BIT22 ) ? ( dword22 | XTEND22 ) : dword22 )

#define BIT23   0x400000
#define XTEND23 0xFFC00000

  /* sign extend 23 bit signed integer to signed long */
#define EXTEND23(dword23) \
  ( long ) (( dword23 & BIT23 ) ? ( dword23 | XTEND23 ) : dword23 )

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION REG_DIST

DESCRIPTION
  This function calculates the distance the mobile has moved since it last
  registered and checks whether distance-based registration is required.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if distance based registration is required,  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean reg_dist
(
  long reg_lat1,               /* previous base station lattitude */
  long reg_lat2,               /* current base station lattitude */
  long reg_long1,              /* previous base station longitude */
  long reg_long2,              /* current base station longitude */
  word dist_reg                /* distance to trigger registration */
);

#endif  /* DISTREG_H */

