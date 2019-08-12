/**
 * @file boot_mbr_partition_id.c
 * @brief
 * Source file contains the MBR partition id for sbl2,sbl3,tz,rpm and appsbl
 *
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_mbr_partition_id.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
10/17/13   ck      Split TZ into QSEE and QHEE partition ids.
03/24/11   dh      Initial creation 

============================================================================
                     Copyright 2011 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                             INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_dev_if.h"

/*define MBR partition id for SBL2, SBL3, TZ, RPM, APPSBL, */
uint8 sbl2_partition_id[PARTITION_ID_MAX_SIZE]    = {0x51};
uint8 sbl3_partition_id[PARTITION_ID_MAX_SIZE]    = {0x45};
uint8 rpm_partition_id[PARTITION_ID_MAX_SIZE]     = {0x47};
uint8 appsbl_partition_id[PARTITION_ID_MAX_SIZE]  = {0x4C};
uint8 tz_qsee_partition_id[PARTITION_ID_MAX_SIZE] = {0x52};
uint8 tz_qhee_partition_id[PARTITION_ID_MAX_SIZE] = {0x53};
