#ifndef HALVMIDMTTARGET_H
#define HALVMIDMTTARGET_H
/**

@file   HALvmidmtTarget.h

@brief  This module provides the target specific VMIDMT settings 
        to the VMIDMT HAL layer.
*/
/*
===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/vmidmt/hal/inc/8909w/HALvmidmtTarget.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
05/28/2014   sc      Created

===========================================================================
Copyright (c) 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================
*/
/** Maximum allowed VMID value */
#define HAL_VMIDMT_MAX_VMID 31

/** symbolic definitions for each VMID index */
/** use in HAL_vmidmt_ConfigVmidContext() */
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_0 0
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_1 1
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_2 2
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_3 3
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_4 4
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_5 5
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_6 6
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_7 7
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_8 8
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_9 9
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_10 10
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_11 11
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_12 12
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_13 13
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_14 14
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_15 15
#define HAL_VMIDMT_BAM_BLSP1_DMA_PIPE_16 16

#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_0 0
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_1 1
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_2 2
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_3 3
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_4 4
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_5 5
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_6 6
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_7 7
#define HAL_VMIDMT_CRYPTO0_BAM_PIPE_8 8

#define HAL_VMIDMT_DEHR_PIPE_0 0

#define HAL_VMIDMT_QPIC_PIPE_0 0

#define HAL_VMIDMT_RPM_MSGRAM_PIPE_0 0

#endif // HALVMIDMTTARGET_H
