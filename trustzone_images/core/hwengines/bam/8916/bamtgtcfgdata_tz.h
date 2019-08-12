#ifndef BAMTGTCFGDATA_TZ_H_
#define BAMTGTCFGDATA_TZ_H_

/**
  @file bamtgtcfgdata_tz.h
  @brief
  This file contains configuration data for the BAM driver for the 
  8916 TZ system.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/hwengines/bam/8916/bamtgtcfgdata_tz.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/11/13   SA      Branched from 8x26 and updated for 8916.
17/10/14   MS      Updated BLSP1 control resource group access config 
                   to give read access to all and write only to TZ. 
===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

/** Configs supported for
    CE1
    BLSP1
    USB2
	QDSS
    BAM_TGT_CFG_LAST
 */
#include "tzbsp_vmid_config.h" 

#define BAM_CNFG_BITS_VAL 0x7FFFF004

bam_sec_config_type bam_tgt_ce1_secconfig_8916 =
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

bam_sec_config_type bam_tgt_blsp1_secconfig_8916 =
{
    {
        {0x0000FFFF, TZBSP_VMID_AP, 0xFFFF, TZBSP_VMID_AP_BIT},
    },
    {   {0x0}  /* SG not supported*/
    },
    (((~(TZBSP_VMID_NOACCESS_BIT))&(~TZBSP_VMID_TZ_BIT)) & TZBSP_ALL_VMID_BMSK), /*1. TZ have implicit read and write access 2. Settling TZ VMID causing AP(EE0) to loose write access*/
    0x0 /* IRQ will be routed to EE0 by default */
};

bam_sec_config_type bam_tgt_qdss_secconfig_8916 =
{
    {
        {0x0000FFFF, TZBSP_VMID_AP, 0x0, TZBSP_VMID_AP_BIT},
    },
    {   {0x0}  /* SG not supported*/
    },
    TZBSP_VMID_MSS_BIT |TZBSP_VMID_AP_BIT,
    0x0 /* IRQ will be routed to EE0 by default */
};

bam_sec_config_type bam_tgt_usb2_secconfig_8916 =
{
    {
        {0x0000FFFF, TZBSP_VMID_AP, 0x0, TZBSP_VMID_AP_BIT},
    },
    {   {0x0}  /* SG not supported*/
    },
    TZBSP_VMID_MSS_BIT |TZBSP_VMID_AP_BIT,
    0x0 /* IRQ will be routed to EE0 by default */
};

const bam_target_config_type  bam_tgt_config[] = {
    { // APSS_CE
        0x00704000, // CRYPTO0_CRYPTO_BAM: 0x00700000 + 0x00004000 (0x00704000)
        BAM_TGT_CFG_SECURE, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        0,   // EE index
        &bam_tgt_ce1_secconfig_8916 // Security Config
    },
    { // QDSS
        0x00884000, // QDSS_NDPBAM_BAM: 0x00800000 + 0x00084000 (0x00884000) 
        BAM_TGT_CFG_SECURE, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        BAM_EE_TRUST,   // EE index
        &bam_tgt_qdss_secconfig_8916 // Security Config
    },
    { // BLSP1_BAM
        0x07884000, // BLSP1_BLSP_BAM: 0x07880000 + 0x00004000 (0x07884000) 
        (BAM_TGT_CFG_SECURE | BAM_TGT_CFG_SHARABLE), // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        BAM_EE_TRUST,   // EE index
        &bam_tgt_blsp1_secconfig_8916 // Security Config
    },
    { // USB2_HSIC
        0x078C4000, // USB2_HSIC_BAM: 0x078C0000 + 0x00004000 (0x078C4000) 
        BAM_TGT_CFG_SECURE, // BAM CFG OPTIONS
        BAM_CNFG_BITS_VAL,         // default chicken bits
        BAM_EE_TRUST,   // EE index
        &bam_tgt_usb2_secconfig_8916 // Security Config
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

