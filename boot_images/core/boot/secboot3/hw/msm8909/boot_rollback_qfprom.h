#ifndef BOOT_ROLLBACK_QFPROM
#define BOOT_ROLLBACK_QFPROM

/*===========================================================================

                  Boot Module Version Rollback Qfprom Header File

GENERAL DESCRIPTION
  This header file contains the definition of the Qfprom fuses used for
  Version rollback prevention.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/14   ck      Updated BOOT_ANTI_ROLLBACK_EN_BMSK and SHFT to 8916 values
03/13/14   ck      Removed HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR
03/13/14   ck      Updated to use proper CORR registers
01/15/13   kedara  Update bitmask and shift for BOOT_ANTI_ROLLBACK_EN
10/18/12   dh      Initial creation

============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
/*
  Note:
  ANTI_ROLLBACK_FEATURE_EN contains more than one bit.  Each bit represents
  enable of anti rollback for a particular subsystem.  I.E. Boot subsystem.
  The mask is not defined in HWIO so it is defined here.
*/

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_BOOT_ANTI_ROLLBACK_EN_BMSK                           0x00080000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_BOOT_ANTI_ROLLBACK_EN_SHFT                                 0x13


#endif /* BOOT_ROLLBACK_QFPROM */
