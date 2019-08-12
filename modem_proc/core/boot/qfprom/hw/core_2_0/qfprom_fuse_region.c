/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains target specific defintions and APIs to be used to read and write
 qfprom values.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_fuse_region.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/14   ck      Initial revision for Bear Qfprom driver in modem

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include QFPROM_HWIO_REG_INCLUDE_H
#include QFPROM_TARGET_INCLUDE_H

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/*
**  Array containing QFPROM data items that can be read and associated
**  registers, mask and shift values for the same.
**
**  Note: The table is image specific based on requirements to support only
**  regions which are required by that image.
**
*/
const QFPROM_REGION_INFO qfprom_region_data[] =
{
    {
        QFPROM_ANTI_ROLLBACK_3_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,
        HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR ,
        HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK,
        HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        10
    },

    {
        QFPROM_SPARE_REG19_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,
        HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK,
        HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        24
    },
    
    /* Add above this entry */
    {
        QFPROM_LAST_REGION_DUMMY,
        0,
        QFPROM_FEC_NONE,
        0,
        0,
        0,
        0,
        QFPROM_ROW_LSB
    }
};
