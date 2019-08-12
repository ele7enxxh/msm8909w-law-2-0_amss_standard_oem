#ifndef TZBSP_PERIPHERAL_INFO
#define TZBSP_PERIPHERAL_INFO

/*===========================================================================
 *    Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 ===========================================================================*/

/*===========================================================================
 *
 *                          EDIT HISTORY FOR FILE
 *
 *  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909w/src/tzbsp_peripheral_info.h#1 $
 *  $DateTime: 2016/06/17 14:31:11 $
 *  $Author: pwbldsvc $
 *
 *  when       who      what, where, why
 *  --------   ---      ------------------------------------
 *  05/24/12   pre      Initial version
 *
 *===========================================================================*/

/*============================================================================
 *
 *           INCLUDE FILES
 *
 *============================================================================*/
#include "tzbsp_mmu_cache.h"

/**
   Used as indices into tzbsp_mmp_map for looking up peripheral memory
   addresses.  MMP = memory mapped peripheral
 */
typedef enum
{
  TZBSP_MMP_SEC_CTRL_QFPROM_RAW       = 0,
  TZBSP_MMP_SEC_CTRL_SEC_JTAG         = 1,
  TZBSP_MMP_SEC_CTRL_QFPROM_CORR      = 2,
  TZBSP_MMP_SEC_CTRL_SECURITY_CONTROL = 3,
  TZBSP_MMP_PRNG                      = 4,
  TZBSP_NUM_MMP,
} tzbsp_mmp_type;


typedef enum tzbsp_boot_type
{
 BOOT_DEFAULT_OPTION = 0, /* EMMC->HSUSB2.0 */
 BOOT_SDC_PORT2_THEN_SDC_PORT1_OPTION,
 BOOT_SDC_PORT1_OPTION, /* 2, eMMC@SDC1 */
 BOOT_USB_OPTION,
 BOOT_NAND_OPTION, /* 4, NAND BOOT */
}tzbsp_boot_type_enum_type;


/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

int tzbsp_get_peripheral_mem_block(tzbsp_mmp_type  mmp,
                                   mem_block_type* mmp_block);

#endif /* TZBSP_PERIPHERAL_INFO */
