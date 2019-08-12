#ifndef BOOT_ROLLBACK_QFPROM
#define BOOT_ROLLBACK_QFPROM

/*===========================================================================

                  Boot Module Version Rollback Qfprom Header File

GENERAL DESCRIPTION
  This header file contains the definition of the Qfprom fuses used for
  Version rollback prevention.

Copyright 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/13   jz      Updated for 9x45
06/19/13   kedara  Initial creation

============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
/*
  Note:
  The register names for Qfprom fuses used for Version rollback prevention
  are not yet allocated in SWI. This header file serves as a wrapper to map
  the rollback prevention fuse registers to current Qpfrom spare regsiter.
  For Future targets having dedicated rollback prevention registers, the
  header file can be dropped.
*/



#include "msmhwioreg.h"

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR                        HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_BMSK  HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_SHFT  HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_MSA_ANTI_ROLLBACK_EN_BMSK    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_BMSK
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_MSA_ANTI_ROLLBACK_EN_SHFT    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_SHFT

#endif /* BOOT_ROLLBACK_QFPROM */
