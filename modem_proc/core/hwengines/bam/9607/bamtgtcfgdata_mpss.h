/**
  @file bamtgtcfgdata_mpss.h
  @brief
  This file contains configuration data for the BAM driver for the
  9x07 mpss system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/bam/9607/bamtgtcfgdata_mpss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/18/15   SA      Created (from 8909).

-----------------
Previous history
-----------------
07/10/14   PR      Branched from 8916 and updated for 8909.

===============================================================================
                   Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    CE_M
    A2
    BLSP1
    QPIC
    BAM_TGT_CFG_LAST
 */

#define BAM_CNFG_BITS_VAL 0xFFFFF004

const bam_target_config_type  bam_tgt_config[] = {
        {                     // CE_M_BAM
         /* .bam_pa     */    0x041C4000,
         /* .options    */    0x0,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    0,
         /* .sec_config */    NULL
    },
    { // A2_BAM
         /* .bam_pa     */    0x04044000,
         /* .options    */    BAM_TGT_CFG_SW_DEBUG_INIT,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    0,
         /* .sec_config */    NULL
    },
    { // BLSP1_BAM
         /* .bam_pa     */    0x07884000,
         /* .options    */    BAM_TGT_CFG_SHARABLE,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    2,
         /* .sec_config */    NULL
    },
        {                     // QPIC_BAM_LITE
         /* .bam_pa     */    0x07984000,
         /* .options    */    0x0,
         /* .cfg_bits   */    BAM_CNFG_BITS_VAL,
         /* .ee         */    1,
         /* .sec_config */    NULL
    },
        {                     //LAST
         /* .bam_pa     */    BAM_TGT_CFG_LAST,
         /* .options    */    0,
         /* .cfg_bits   */    0,
         /* .ee         */    0,
         /* .sec_config */    NULL
    },
};



