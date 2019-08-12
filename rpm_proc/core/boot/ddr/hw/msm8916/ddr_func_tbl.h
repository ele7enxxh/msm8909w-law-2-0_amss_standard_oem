#ifndef __DDR_FUNC_TBL_H__
#define __DDR_FUNC_TBL_H__


/**
 * @file ddr_target.h
 * @brief
 * Header file that contains shared function table structure for SBL and RPM to
 * use common ddr library
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8916/ddr_func_tbl.h#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/24/14   tw      updated clock period function to take in divider 
                   function pointer
01/24/14   tw      Initial version.
================================================================================
                      Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "BIMC.h"
#include "DDR_PHY.h"


typedef void (*ddr_wait)(uint32 usec, uint32 clk_speed);

/*==============================================================================
                                  STRUCTURE
==============================================================================*/
/* function table for sbl and RPM to share */
typedef struct 
{
  /* self refresh api */
  void (*enter_self_refresh) (uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait);
  void (*exit_self_refresh) (uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait);
  
  /* clock switch api */
  uint32 (*get_read_latency) (DDR_TYPE ddr_type, uint32 clk_speed);
  uint32 (*get_write_latency) (DDR_TYPE ddr_type, uint32 clk_speed);
  void (*bimc_update_clock_period) (uint32 base, uint32 clk_speed, ddr_divide divide_func);
  void (*bimc_dpe_pre_clock_switch) (uint32 base, SDRAM_INTERFACE interface, uint32 old_clk, uint32 new_clk);
  void (*bimc_dpe_post_clock_switch) (uint32 base, SDRAM_INTERFACE interface, uint32 old_clk, uint32 new_clk);  
  
  /* voltage switch api */
  
  /* power collapse api */
  
  /* xo shutdown api */
   
  /* ddr phy api */
  void (*phy_manual_io_cal) (uint32 base, SDRAM_INTERFACE interface, ddr_wait wait);
  void (*phy_update_cdc_config) (uint32 base, SDRAM_INTERFACE interface, uint32 config);
  void (*phy_manual_cdc_cal) (uint32 base, SDRAM_INTERFACE interface);
  void (*phy_enable_rapid_cdc_cal) (uint32 base, SDRAM_INTERFACE interface);
  void (*phy_disable_rapid_cdc_cal) (uint32 base, SDRAM_INTERFACE interface);
  void (*phy_update_drive_strength) (uint32 base, SDRAM_INTERFACE interface, uint32 rout, uint32 pslew, uint32 nslew);
  void (*phy_update_cdc_delay) (uint32 base, SDRAM_INTERFACE interface, boolean read, uint32 cdc_delay);
  /* DMM api */
  /* currently not supported */

}ddr_func;

#endif
