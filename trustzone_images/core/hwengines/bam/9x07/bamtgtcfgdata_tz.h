#ifndef BAMTGTCFGDATA_TZ_H_
#define BAMTGTCFGDATA_TZ_H_

/**
  @file bamtgtcfgdata_tz.h
  @brief
  This file contains configuration data for the BAM driver for the 
  8909 TZ system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/hwengines/bam/9x07/bamtgtcfgdata_tz.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/04/15   PR      updated for 9x07.
06/23/14   PR      Branched from 8916 and updated for 8909.

===============================================================================
                   Copyright (c) 2015 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    CE1
    BLSP1
    USB2
    QDSS
    QPIC
    BAM_TGT_CFG_LAST
 */
#include "tzbsp_vmid_config.h" 

#define BAM_CNFG_BITS_VAL 0x7FFFF004

bam_sec_config_type bam_tgt_ce1_secconfig_9x07 =
{
    {
        { 0x800000C3 , TZBSP_VMID_TZ, 0x0, TZBSP_VMID_TZ_BIT},  /* krait tz */
        { 0x0000003C , TZBSP_VMID_AP, 0xFF, TZBSP_VMID_AP_BIT}  /* krait apps */
    },
    {   {0x0}  /* SG not supported*/
    },  
    TZBSP_VMID_TZ_BIT | TZBSP_VMID_AP_BIT,
    0x0 /* IRQ will be routed to EE0 */
};

bam_sec_config_type bam_tgt_blsp1_secconfig_9x07 =
{
    {
        {0x00FFFFFF, TZBSP_VMID_AP, 0xFFFFFF, TZBSP_VMID_AP_BIT},
    },
    {   {0x0}  /* SG not supported*/
    },
    TZBSP_VMID_AP_BIT,
    0x0 /* IRQ will be routed to EE0 by default */
};

bam_sec_config_type bam_tgt_qpic_secconfig_9x07 = 
{
    {
        {0x00000047, TZBSP_VMID_AP,  0xFF, TZBSP_VMID_AP_BIT},   // APPS
        {0x00000038, TZBSP_VMID_MSS, 0x0, TZBSP_VMID_MSS_BIT}, // MPSS
    },
    {
        {0x0000005B},                                          // SG APPS 
        {0x00000024},                                          // SG MPSS
    },
    TZBSP_VMID_AP_BIT | TZBSP_VMID_MSS_BIT,                    // CTRL VMID
    0x0                                                        // BAM EE
};

const bam_target_config_type  bam_tgt_config[] = {
    { // APSS_CE
        0x00704000, // CRYPTO0_CRYPTO_BAM: 0x00700000 + 0x00004000 (0x00704000)
        BAM_TGT_CFG_SECURE, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        &bam_tgt_ce1_secconfig_9x07 // Security Config
    },
    { // BLSP1_BAM
        0x07884000, // BLSP1_BLSP_BAM: 0x07880000 + 0x00004000 (0x07884000) 
        (BAM_TGT_CFG_SECURE | BAM_TGT_CFG_SHARABLE), // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        BAM_EE_TRUST,   // EE index
        &bam_tgt_blsp1_secconfig_9x07 // Security Config
    },
    {   //QPIC_BAM_LITE
        0x7984000,                // QPIC_BAM
		
        (BAM_TGT_CFG_SECURE), // BAM CFG OPTIONS 
        BAM_CNFG_BITS_VAL,         // default chicken bits
        BAM_EE_TRUST,              // EE index
        &bam_tgt_qpic_secconfig_9x07    // Security Config
    },
    { //dummy config
        BAM_TGT_CFG_LAST,
        0,
        0,
        2,
        NULL
    },
};

#endif

