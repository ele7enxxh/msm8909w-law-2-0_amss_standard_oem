#ifndef SIMLOCK_FUSE_H
#define SIMLOCK_FUSE_H
/*===========================================================================


            S I M L O C K   F U S E   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_fuse.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/14/16   bcho    Initial revision
===========================================================================*/

#ifdef FEATURE_SIMLOCK

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_FUSE_IS_FUSE_BLOWN

DESCRIPTION
  Function called to find out if the SIMLOCK fuse is blown or not.

DEPENDENCIES
  This routine should be called at boot up only. The underlying register
  contains fuses currently blown in actual QFPROM, at power up. If after
  a device is powered up, and a fuse bit is blown in QFPROM, this underlying
  register would be unaware of that until next boot up.

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean simlock_fuse_is_fuse_blown
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_FUSE_BLOW_FUSE

DESCRIPTION
  Function called to blow the SIMLOCK fuse that is in the MSA accessible
  AntiRollback region.

DEPENDENCIES
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_fuse_blow_fuse
(
  void
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_FUSE_H */
