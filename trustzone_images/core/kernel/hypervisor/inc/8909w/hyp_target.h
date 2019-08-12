/**
@file hyp_target.h
@brief Hypervisor Target specific definitions

This file contains the definition of hypervisor target specific constants

*/
/*===========================================================================
   Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/inc/8909w/hyp_target.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
=============================================================================*/
#ifndef HYP_TARGET_H
#define HYP_TARGET_H

#define NUM_CLUSTERS           1
#define NUM_CPUS_PER_CLUSTER   4
#define CLUSTER_RSHIFT         6 // equals to (8 - log(NUM_CPUS_PER_CLUSTER)
#define NUM_CPUS               (NUM_CLUSTERS * NUM_CPUS_PER_CLUSTER)

#define HYP_STACK_SIZE          0x1000

#define HYP_MEMORY_ADDR         0x86400000
#define HYP_MEMORY_SIZE         0x00100000
#define HYP_IMAGE_SIZE          0x00010400

#define HYP_COPY_ADDR           (HYP_MEMORY_ADDR + HYP_IMAGE_SIZE)
#define PAGETABLE_MEMORY_ADDR   (HYP_COPY_ADDR + HYP_IMAGE_SIZE)
#define PAGETABLE_MEMORY_SIZE   (HYP_MEMORY_SIZE - (HYP_IMAGE_SIZE*2))

// L1PT addresses must be aligned to 32B
#define VIRT_L1PT_MEMORY_ADDR       PAGETABLE_MEMORY_ADDR
#define VIRT_L1PT_MEMORY_SIZE       0x00000100
#define SMMU_L1PT_MEMORY_ADDR       (VIRT_L1PT_MEMORY_ADDR + VIRT_L1PT_MEMORY_SIZE)
#define SMMU_L1PT_MEMORY_SIZE       0x00000700

// pool memory must be 4KB-aligned
#define VIRT_PT_POOL_MEMORY_ADDR    (SMMU_L1PT_MEMORY_ADDR + SMMU_L1PT_MEMORY_SIZE)
#define VIRT_PT_POOL_MEMORY_SIZE    0x0007d000
#define SMMU_PT_POOL_MEMORY_ADDR    (VIRT_PT_POOL_MEMORY_ADDR + VIRT_PT_POOL_MEMORY_SIZE)
#define SMMU_PT_POOL_MEMORY_SIZE    (PAGETABLE_MEMORY_SIZE - VIRT_L1PT_MEMORY_SIZE - SMMU_L1PT_MEMORY_SIZE - VIRT_PT_POOL_MEMORY_SIZE) 

#define HYP_DEFAULT_LOG_SIZE     4096
#define HYP_DEFAULT_LOG_HDR_SIZE 512

#define IMEM_BASE          0x08600000
#define IMEM_HYP_OFFSET    0x00000B20
#define IMEM_BACKUP_ADDR   (IMEM_BASE + IMEM_HYP_OFFSET)
#define IMEM_HYP_LOG_ADDR  (IMEM_BASE + IMEM_HYP_OFFSET + 0x10)
#define IMEM_HYP_LOG_SIZE  (IMEM_BASE + IMEM_HYP_OFFSET + 0x14)
#define IMEM_BACKUP_MAGIC  0xC1F8DB42

#define HYP_BRING_RPM_OUT_OF_RESET  1
#ifdef TZBSP_WITH_TBASE
#define GCC_APSS_MISC_ADDR          (0x1860000 + 0x04800000)
#else
#define GCC_APSS_MISC_ADDR          0x1860000
#endif
#define RPM_RESET_REMOVAL_BMSK      0x6

#endif
