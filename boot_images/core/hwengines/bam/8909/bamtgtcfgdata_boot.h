/**
  @file bamtgtcfgdata_boot.h
  @brief
  This file contains configuration data for the BAM driver for the 
  8909 boot and tools.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/bam/8909/bamtgtcfgdata_boot.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/03/14   PR      branched from 9x45

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
        {                     // LAST
         /* .bam_pa     */    BAM_TGT_CFG_LAST,
         /* .options    */    0,
         /* .cfg_bits   */    0,
         /* .ee         */    0,
         /* .sec_config */    NULL,
         /* .size       */    0
        },
};


