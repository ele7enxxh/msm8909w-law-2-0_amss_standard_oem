#ifndef TZBSP_VMID_CONFIG_H
#define TZBSP_VMID_CONFIG_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_vmid_config.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/12   tk      First version.
============================================================================*/

#define TZBSP_VMID_NOACCESS     0
#define TZBSP_VMID_RPM          1
#define TZBSP_VMID_TZ           2
#define TZBSP_VMID_AP           3
#define TZBSP_VMID_MSS          4
#define TZBSP_VMID_LPASS        5
#define TZBSP_VMID_WLAN         6
#define TZBSP_VMID_CP           7
#define TZBSP_VMID_VIDEO        8
#define TZBSP_VMID_DEHR         9
#define TZBSP_VMID_OCMEM_DM     10
#define TZBSP_VMID_APCPU        11

#define TZBSP_VMID_NOACCESS_BIT (1<<TZBSP_VMID_NOACCESS)
#define TZBSP_VMID_TZ_BIT       (1<<TZBSP_VMID_TZ)
#define TZBSP_VMID_RPM_BIT      (1<<TZBSP_VMID_RPM)
#define TZBSP_VMID_MSS_BIT      (1<<TZBSP_VMID_MSS)
#define TZBSP_VMID_LPASS_BIT    (1<<TZBSP_VMID_LPASS)
#define TZBSP_VMID_APS_BIT      (1<<TZBSP_VMID_AP)
#define TZBSP_VMID_WLAN_BIT     (1<<TZBSP_VMID_WLAN)
#define TZBSP_VMID_CP_BIT       (1<<TZBSP_VMID_CP)
#define TZBSP_VMID_VIDEO_BIT    (1<<TZBSP_VMID_VIDEO)
#define TZBSP_VMID_DEHR_BIT     (1<<TZBSP_VMID_DEHR)   
#define TZBSP_VMID_OCMEM_DM_BIT (1<<TZBSP_VMID_OCMEM_DM)
#define TZBSP_VMID_APCPU_BIT    (1<<TZBSP_VMID_APCPU)
                    
#define TZBSP_VMID_AP_BIT    (TZBSP_VMID_APS_BIT | TZBSP_VMID_APCPU_BIT)

#define TZBSP_TZ_MSS_VMID_BMSK    (TZBSP_VMID_TZ_BIT        | \
                                   TZBSP_VMID_MSS_BIT       | \
                                   TZBSP_VMID_MSS_ADM_BIT)
#define TZBSP_TZ_MSS_AP_VMID_BMSK (TZBSP_TZ_MSS_VMID_BMSK   | \
                                   TZBSP_VMID_AP_BIT        | \
                                   TZBSP_VMID_AP_ADM_BIT)

#define VMID_BAM_BLSP1_DMA0     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA1     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA2     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA3     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA4     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA5     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA6     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA7     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA8     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA9     TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA10    TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA11    TZBSP_VMID_AP
#define VMID_BAM_BLSP1_DMA12    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA13    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA14    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA15    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA16    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA17    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA18    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA19    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA20    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA21    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA22    TZBSP_VMID_LPASS
#define VMID_BAM_BLSP1_DMA23    TZBSP_VMID_LPASS

#define VMID_DEHR               TZBSP_VMID_DEHR

#define VMID_RPM                TZBSP_VMID_RPM

#define TZBSP_ALL_VMID_BMSK     (TZBSP_VMID_NOACCESS_BIT  | \
                                 TZBSP_VMID_TZ_BIT        | \
                                 TZBSP_VMID_RPM_BIT       | \
                                 TZBSP_VMID_MSS_BIT       | \
                                 TZBSP_VMID_LPASS_BIT     | \
                                 TZBSP_VMID_APS_BIT       | \
                                 TZBSP_VMID_APCPU_BIT     | \
                                 TZBSP_VMID_WLAN_BIT      | \
                                 TZBSP_VMID_CP_BIT        | \
                                 TZBSP_VMID_VIDEO_BIT     | \
                                 TZBSP_VMID_DEHR_BIT      | \
                                 TZBSP_VMID_OCMEM_DM_BIT)

#endif /* TZBSP_VMID_CONFIG_H */
