#ifndef __DDR_TARGET_H__
#define __DDR_TARGET_H__


/**
 * @file ddr_target.h
 * @brief
 * Header file for DDR target-specific info.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8909w/ddr_target.h#1 $
$DateTime: 2015/09/16 01:53:21 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/11/15   aus     Initial version.
================================================================================
             Copyright 2015 Qualcomm Technologies Incorporated
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

/* XO clock frequency in KHz */
#define XO_SPEED_IN_KHZ  19200

/* Initial DDR PHY CDC delay */
extern uint32 DDR_PHY_INIT_CDC_DELAY;


/*struct for CDB2 pairs,enhance set*/
struct ddr_boot_setting_params
{
     uint32 address;
     uint32 mask;
     uint32 value;
};

/*struct for CDB3,enhance set*/
struct ddr_post_boot_setting_params
{
	uint32 dq_read_cdc_delay;
	uint32 dq_write_cdc_delay;
};


/*Head for CDB2 in CDT,enhance set*/
struct ddr_boot_set_header_v1
{
  /*Version number of the CDT DDR parameters, always the first four byte of this header*/
  uint32 version_number;

  /*Magic number of the DDR block of CDT, must have the value of DDR_PARAMS_MAGIC_NUM*/
  uint32 magic_number;
  
  /*Total size of the ddr parameters for a single interface*/
  uint32 num_of_groups;

  /**More fields can be added here, each field must be multiple of 4 bytes**/
  
};


/*Head for CDB3 in CDT,enhance set*/
struct ddr_post_boot_set_header_v1
{
  /*Version number of the CDT DDR parameters, always the first four byte of this header*/
  uint32 version_number;

  /*Magic number of the DDR block of CDT, must have the value of DDR_PARAMS_MAGIC_NUM*/
  uint32 magic_number;
  
  /*Total size of the ddr parameters for a single interface*/
  uint32 size_of_param;
};



/*Magic number for CDB2 and CDB3 in CDT,enhance set*/
#define DDR_BOOT_SET_MAGIC_NUM 0x06078899
#define DDR_POST_BOOT_SET_MAGIC_NUM 0x04030201


#endif /* __DDR_TARGET_H__ */
