/**
  @file bamtgtcfgdata_boot.h
  @brief
  This file contains configuration data for the BAM driver for the 
  9x45 boot and tools.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/bam/9x45/bamtgtcfgdata_boot.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/18/14   ss      Added IPA BAM to boot
03/24/14   rl      branched from 9x35

===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    QPIC
    BAM_TGT_CFG_LAST
 */

#define BAM_CNFG_BITS_VAL 0xFFFFF004
 
const bam_target_config_type  bam_tgt_config[] = {
        {                     // QPIC_BAM _LITE
         /* .bam_pa     */    0x07984000,
         /* .options    */    BAM_TGT_CFG_FORCEINIT,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    0,
         /* .sec_config */    NULL,
         /* .size       */    BAM_MAX_MMAP
        },
        {                     // CE_S_BAM
         /* .bam_pa     */    0x00704000,
         /* .options    */    BAM_TGT_CFG_FORCEINIT,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    1,
         /* .sec_config */    NULL,
         /* .size       */    BAM_MAX_MMAP
        },
		{                     // IPA_BAM
         /* .bam_pa     */    0x7904000,
         /* .options    */    BAM_TGT_CFG_FORCEINIT,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    2,
         /* .sec_config */    NULL,
         /* .size       */    BAM_MAX_MMAP
        },
        {                     // LAST
         /* .bam_pa     */    BAM_TGT_CFG_LAST,
         /* .options    */    0,
         /* .cfg_bits   */    0,
         /* .ee         */    0,
         /* .sec_config */    NULL,
         /* .size       */    0
        },
};


