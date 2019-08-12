#ifndef __DDR_TARGET_H__
#define __DDR_TARGET_H__


/**
 * @file ddr_target.h
 * @brief
 * Header file for DDR target-specific info.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/mdm9x45/ddr_target.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/04/15   sc      Moved PHY_DQ_MAX_NUM from DDR_PHY.c to ddr_target.h
04/10/13   sl      Use voltage mode instead of voltage number.
04/03/13   sl      Added macros for vddmx voltage levels.
02/21/13   sl      Initial version.
================================================================================
             Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_hwio.h"


/*==============================================================================
                                  MACROS
==============================================================================*/
 
/* DQ PHY offsets */
#define PHY_DQ_MAX_NUM 4

/* DDR controller register names */
#define DDR_ADDR(ddr_reg)     BIMC_S_DDR0_##ddr_reg
#define PHY_CA_ADDR(ddr_reg)  DIM_C00_DIM_CA_##ddr_reg
#define PHY_DQ_ADDR(ddr_reg)  DIM_D00_DIM_DQ_##ddr_reg

/* DDR controller offsets */
#define SDRAM_0_OFFSET     0x0
#define SDRAM_1_OFFSET     0xFFFFFFFF
#define PHY_0_OFFSET       0x0
#define PHY_1_OFFSET       0xFFFFFFFF
#define PHY_CA_MST_OFFSET  0x0
#define PHY_DQ0_OFFSET     0x0
#define PHY_DQ1_OFFSET     (DIM_D01_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define PHY_DQ2_OFFSET     (DIM_D02_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define PHY_DQ3_OFFSET     (DIM_D03_DDRPHY_DQ_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)
#define DQ_BCAST_OFFSET    (EBI1_AHB2PHY_BROADCAST_SWMAN_REG_BASE - DIM_D00_DDRPHY_DQ_REG_BASE)

/* DDR base address */
#define DDR_BASE  0x80000000

/* Maximum DDR clock frequency in KHz */
#define DDR_CLK_FREQ_MAX_IN_KHZ  533000

/* XO clock frequency in KHz */
#define XO_SPEED_IN_KHZ  19200

/* Initial DDR PHY CDC delay */
extern uint32 DDR_PHY_INIT_CDC_DELAY;


#endif /* __DDR_TARGET_H__ */
