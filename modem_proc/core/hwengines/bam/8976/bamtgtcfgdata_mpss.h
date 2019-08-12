/**
  @file bamtgtcfgdata_mpss.h
  @brief
  This file contains configuration data for the BAM driver for the
  8952 mpss system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/bam/8976/bamtgtcfgdata_mpss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/20/14   SA      Created.

===============================================================================
                   Copyright (c) 2014 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
  - MSS_CE
  - IPA
  - BLSP1
  - BLSP2
 */

#define BAM_CNFG_BITS_VAL 0x7FFFF004

const bam_target_config_type  bam_tgt_config[] = {
    {                         // CE_M_BAM
         /* .bam_pa     */    0x041C4000,
         /* .options    */    0x0,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    0,
         /* .sec_config */    NULL
    },
    {                         // IPA_BAM
         /* .bam_pa     */    0x07904000,
         /* .options    */    0x0,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    1,
         /* .sec_config */    NULL
    },
    {                         // BLSP1_BAM
         /* .bam_pa     */    0x07884000,
         /* .options    */    BAM_TGT_CFG_SHARABLE,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    2,
         /* .sec_config */    NULL
    },
    {                         // BLSP2_BAM
         /* .bam_pa     */    0x07AC4000,
         /* .options    */    BAM_TGT_CFG_SHARABLE,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    2,
         /* .sec_config */    NULL
    },
    {                         // LAST
         /* .bam_pa     */    BAM_TGT_CFG_LAST,
         /* .options    */    0,
         /* .cfg_bits   */    0,
         /* .ee         */    0,
         /* .sec_config */    NULL
    },
};



