/**
  @file bamtgtcfgdata_mpss.h
  @brief
  This file contains configuration data for the BAM driver for the
  8916 mpss system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/bam/8916/bamtgtcfgdata_mpss.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/13   SA      Created.

===============================================================================
                   Copyright (c) 2013 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
  - APSS_CE
  - A2
  - MSS_CE
  - QDSS
  - BLSP
  - USB2_HSIC

 */

#define BAM_CNFG_BITS_VAL 0x7FFFF004

const bam_target_config_type  bam_tgt_config[] = {
    { // APSS_CE
        0x00704000, // CRYPTO0_CRYPTO_BAM: 0x00700000 + 0x00004000 (0x00704000)
        0x0, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        NULL // Security Config
    },
    { // MSS_CE
        0x041C4000, // MSS_CRYPTO_BAM: 0x04000000 + 0x001C4000 (0x041C4000) 
        0x0, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        NULL // Security Config
    },
    { // A2_BAM
        0x04044000, // MSS_BAM_NDP_AUTO_SCALE_V2_0: 0x04000000 + 0x00044000 (0x04044000)
        BAM_TGT_CFG_SW_DEBUG_INIT, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        NULL // Security Config
    },
    { // QDSS
        0x00884000, // QDSS_NDPBAM_BAM: 0x00800000 + 0x00084000 (0x00884000) 
        0x0, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        NULL // Security Config
    },
    { // BLSP1_BAM
        0x07884000, // BLSP1_BLSP_BAM: 0x07880000 + 0x00004000 (0x07884000) 
        BAM_TGT_CFG_SHARABLE, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        2,   // EE index
        NULL // Security Config
    },
    { // USB2_HSIC
        0x078C4000, // USB2_HSIC_BAM: 0x078C0000 + 0x00004000 (0x078C4000) 
        0x0, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        2,   // EE index
        NULL // Security Config
    },
    { //dummy config
        BAM_TGT_CFG_LAST,
        0,
        0,
        2,
        NULL
    },
};



