#ifndef __DDR_TARGET_H__
#define __DDR_TARGET_H__

/*=============================================================================

                                   DDR HAL
                            Header File
GENERAL DESCRIPTION
This is the target header file for DDR HAL.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8952/ddr_target.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/01/13   sr      Fix for number of shifts needed to get to CH1
09/10/13   sr      Updated the PHY CA , DQ Macros .
09/06/13   sr      Define feature flag for saving training data to EMMC .
06/21/13   tw      Initial Port for 8084
09/17/12   sl      Removed DDR_CLK_PERIOD_INIT_IN_100PS.
09/06/12   sl      Relocated macros.
08/24/12   sl      Changed DDR_XO_SPEED to XO_SPEED_IN_KHZ.
08/17/12   sl      Removed included LPDDR3_BIMC_v1.h.
05/06/12   tw      Added definition for XO speed
05/17/12   tw      Initial revision.
=============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "HALhwio.h"



typedef enum
{
  DDRCHIPINFO_ID_APQ8084    = 178,
  DDRCHIPINFO_ID_MPQ8092    = 146
} DdrChipInfoIdType;

/**
  Target type of the device on which the platform is running.
 */
typedef enum
{
  DDRPLATFORMINFO_TYPE_UNKNOWN      = 0x00,  /**< Unknown target device. */
  DDRPLATFORMINFO_TYPE_SURF         = 0x01,  /**< Target is a SURF device. */
  DDRPLATFORMINFO_TYPE_FFA          = 0x02,  /**< Target is an FFA device. */
  DDRPLATFORMINFO_TYPE_FLUID        = 0x03,  /**< Target is a FLUID device. */
  DDRPLATFORMINFO_TYPE_FUSION       = 0x04,  /**< Target is a FUSION device. */
  DDRPLATFORMINFO_TYPE_OEM          = 0x05,  /**< Target is an OEM device. */
  DDRPLATFORMINFO_TYPE_QT           = 0x06,  /**< Target is a QT device. */
  DDRPLATFORMINFO_TYPE_CDP          = DDRPLATFORMINFO_TYPE_SURF,
                                             /**< Target is a CDP (aka SURF) device. */
  DDRPLATFORMINFO_TYPE_MTP_MDM      = 0x07,  /**< Target is a MDM MTP device. */
  DDRPLATFORMINFO_TYPE_MTP_MSM      = 0x08,  /**< Target is a MSM MTP device. */
  DDRPLATFORMINFO_TYPE_MTP          = DDRPLATFORMINFO_TYPE_MTP_MSM,
                                             /**< Target is a MTP device. */
  DDRPLATFORMINFO_TYPE_LIQUID       = 0x09,  /**< Target is a LiQUID device. */
  DDRPLATFORMINFO_TYPE_DRAGONBOARD  = 0x0A,  /**< Target is a DragonBoard device. */
  DDRPLATFORMINFO_TYPE_QRD          = 0x0B,  /**< Target is a QRD device. */
  DDRPLATFORMINFO_TYPE_EVB          = 0x0C,  /**< Target is an EVB device. */
  DDRPLATFORMINFO_TYPE_HRD          = 0x0D,  /**< Target is a HRD device. */
  DDRPLATFORMINFO_TYPE_DTV          = 0x0E,  /**< Target is a DTV device. */
  DDRPLATFORMINFO_TYPE_RUMI         = 0x0F,  /**< Target is on RUMI. */
  DDRPLATFORMINFO_TYPE_VIRTIO       = 0x10,  /**< Target is on VIRTIO. */
  DDRPLATFORMINFO_TYPE_GOBI         = 0x11,  /**< Target is a GOBI device. */
  DDRPLATFORMINFO_TYPE_CBH          = 0x12,  /**< Target is a CBH device. */
  DDRPLATFORMINFO_TYPE_BTS          = 0x13,  /**< Target is a BTS device. */
  DDRPLATFORMINFO_TYPE_XPM          = 0x14,  /**< Target is a XPM device. */
  DDRPLATFORMINFO_TYPE_RCM          = 0x15,  /**< Target is a RCM device. */
  DDRPLATFORMINFO_TYPE_DMA          = 0x16,  /**< Target is a DMA device. */

  DDRPLATFORMINFO_NUM_TYPES         = 0x17,  /**< Number of known targets
                                               (including unknown). */
  /** @cond */
  DDRPLATFORMINFO_TYPE_32BITS       = 0x7FFFFFFF
  /** @endcond */
} DdrPlatformInfoPlatformType;


/**
* Chip version type with major number in the upper 16 bits and minor
* number in the lower 16 bits.  For example:
*   1.0 -> 0x00010000
*   2.3 -> 0x00020003
* The DALCHIP_VERSION macro should be used to generate the appropriate
* comparison value as the format is not guaranteed to remain unchanged.
*/
typedef uint32 DdrChipInfoVersionType;



typedef struct chip_platform_info 
{
     DdrChipInfoIdType            chip_id;      // 8092/8084
     DdrPlatformInfoPlatformType  platform_id ; // CDP/MTP/FLUID/LIQUID/RD etc
     DdrChipInfoVersionType       version;      // ver info i.e; v1.1 , v1.0 etc 
} chip_platform_info;

typedef enum {
	dynamic_legacy = 0,
	static_legacy = 1,
	srpll_mode = 2
	}phy_clk_type;

/*==============================================================================
                                  MACROS
==============================================================================*/
#define DDR_ADDR(ddr_reg) BIMC_S_DDR0_##ddr_reg
#define PHY_DQ_ADDR(dim_reg) CH0_DQ0_DDRPHY_DQ_##dim_reg
#define PHY_CA_ADDR(dim_reg) CH0_CA_DDRPHY_CA_##dim_reg
#define SITE_ADDR(dim_reg) CH0_SITE_CA_##dim_reg

/* DDR base address */
#define DDR_BASE 0x80000000

#define AHB2PHY_CH1_SHIFT_VALUE 5

/* Define the chip select information based on controller register definition */
#define CS0_ONLY 1ul << HWIO_SHFT(DDR_ADDR(DDR_MANUAL_CMD), RANK_SEL)
#define CS1_ONLY 2ul << HWIO_SHFT(DDR_ADDR(DDR_MANUAL_CMD), RANK_SEL)
#define CS_BOTH  3ul << HWIO_SHFT(DDR_ADDR(DDR_MANUAL_CMD), RANK_SEL)

// Define the ddr base
#define EBI1_BIMC_BASE 0x60400000
#define EBI1_PHY_CFG_BASE 0x60480000

/* Define target specific offsets on each interface */
#define SDRAM_0_OFFSET 0
#define SDRAM_1_OFFSET (BIMC_S_DDR1_REG_BASE - BIMC_S_DDR0_REG_BASE)

#define DIM_0_OFFSET 0
#define DIM_1_OFFSET (CH1_DQ0_DDRPHY_DQ_REG_BASE - CH0_DQ0_DDRPHY_DQ_REG_BASE)

#define SITE_0_OFFSET 0
#define SITE_1_OFFSET (CH1_SITE_CA_SITE_REG_BASE - CH0_SITE_CA_SITE_REG_BASE)

/* Define target specific ddr type on each interface */
#define SDRAM_0_TYPE DDR_TYPE_LPDDR3
#define SDRAM_1_TYPE DDR_TYPE_LPDDR3

/* Define DQ off set and IO CAL offsets */
#define SDRAM_DQ0_OFFSET 0x0
#define SDRAM_DQ1_OFFSET 0x800
#define SDRAM_DQ2_OFFSET 0x1000
#define SDRAM_DQ3_OFFSET 0x1800
#define SDRAM_CA_OFFSET  0x0

/* Broadcast offsets */
#define CA_BCAST_OFFSET (HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(0) \
        - CH0_CA_DDRPHY_CA_REG_BASE)
#define DQ_BCAST_OFFSET (HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(0) \
        - CH0_DQ0_DDRPHY_DQ_REG_BASE)
#define SITE_BCAST_OFFSET (HWIO_EBI1_AHB2PHY_BROADCAST_ADDRESS_SPACE_n_ADDR(0) \
        - CH0_SITE_CA_SITE_REG_BASE)

/* SITE broadcast offsets */
#define SITE_CA_OFFSET 0
#define SITE_DQ00_OFFSET (CH0_SITE_DQ0_SITE_REG_BASE - CH0_SITE_CA_SITE_REG_BASE)
#define SITE_DQ01_OFFSET (CH0_SITE_DQ1_SITE_REG_BASE - CH0_SITE_CA_SITE_REG_BASE)
#define SITE_DQ02_OFFSET (CH0_SITE_DQ2_SITE_REG_BASE - CH0_SITE_CA_SITE_REG_BASE)
#define SITE_DQ03_OFFSET (CH0_SITE_DQ3_SITE_REG_BASE - CH0_SITE_CA_SITE_REG_BASE)

/* Define DDR frequencies */
#define DDR_CLK_FREQ_LV1 166000
#define DDR_CLK_FREQ_LV2 200000
#define DDR_CLK_FREQ_LV3 400000
#define DDR_CLK_FREQ_MAX_IN_KHZ  800000

/* Define DIM register broadcast offset */
#define DDR_DIM_BROADCAST_OFFSET 0x00080000

/* Define the PHY CDC delay mode thresholds */
#define DDR_PHY_MAX_THRESHOLD_NUM 4

/* Define the XO speed used for ddr counters (auto refresh, zqcal, srr) */
#define XO_SPEED_IN_KHZ  19200

/* DQ bit mask */
#define DQ0_MASK  0x1
#define DQ1_MASK  0x2
#define DQ2_MASK  0x4
#define DQ3_MASK  0x8
#define DQ4_MASK  0x10
#define DQ5_MASK  0x20
#define DQ6_MASK  0x40
#define DQ7_MASK  0x80
#define DQ8_MASK  0x1
#define DQ9_MASK  0x2
#define DQ10_MASK 0x4
#define DQ11_MASK 0x8
#define DQ12_MASK 0x10
#define DQ13_MASK 0x20
#define DQ14_MASK 0x40
#define DQ15_MASK 0x80

#define DQ0_SHIFT 0x0
#define DQ1_SHIFT 0x1
#define DQ2_SHIFT 0x2
#define DQ3_SHIFT 0x3
#define DQ4_SHIFT 0x4
#define DQ5_SHIFT 0x5
#define DQ6_SHIFT 0x6
#define DQ7_SHIFT 0x7
#define DQ8_SHIFT 0x0
#define DQ9_SHIFT 0x1
#define DQ10_SHIFT 0x2
#define DQ11_SHIFT 0x3
#define DQ12_SHIFT 0x4
#define DQ13_SHIFT 0x5
#define DQ14_SHIFT 0x6
#define DQ15_SHIFT 0x7

/* Define training count value */
#define DDR_LOOP_COUNT_VALUE 128

/* Define ddr training address */
#define DDR_TRAINING_TEST_ADDRESS_CS0 0x10000000

/* Define the PHY CDC delay mode thresholds */
#define DDR_PHY_DELAY0_THRESHOLD 667000
#define DDR_PHY_DELAY1_THRESHOLD 533000
#define DDR_PHY_DELAY2_THRESHOLD 400000

/* TLMM MSM Revision id */
#define DDR_TLMM_HW_REV_ID_V10   0
#define DDR_TLMM_HW_REV_ID_V111  1
#define DDR_TLMM_HW_REV_ID_V112  2
#define DDR_TLMM_HW_REV_ID_V12   3
#define DDR_TLMM_HW_REV_ID_V20   4
#define DDR_TLMM_HW_REV_ID_V201  5

/* Define the current ddr training version number */
#define DDR_TRAINING_PARAM_VERSION 0x2

/* Define the feature flag for saving training data EMMC */
#define FEATURE_DDR_PARAM_SAVE_TO_EMMC 0

#define PC32    FALSE 
#define PC64    FALSE
#define PCLP64  TRUE
#define PCRCW   False
#define MPQ8092 FALSE
#define APQ8084 FALSE
#define FSM9900 FALSE
#define MSM8936 TRUE
#endif /* __DDR_TARGET_H__ */
