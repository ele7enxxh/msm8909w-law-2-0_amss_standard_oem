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

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8936/ddr_func_tbl.h#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/21/14   aus     Initial version.
================================================================================
                      Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"

/*==============================================================================
                                  STRUCTURE
==============================================================================*/
/* function table for sbl and RPM to share */
typedef struct 
{
  /* self refresh api */
  void (*enter_self_refresh) (uint32 base_addr, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);
  void (*exit_self_refresh) (uint32 base_addr, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);
  
  /* clock switch api */
  uint32 (*get_read_latency) (DDR_TYPE ddr_type, uint32 clk_speed, SDRAM_INTERFACE interface);
  uint32 (*get_write_latency) (DDR_TYPE ddr_type, uint32 clk_speed, SDRAM_INTERFACE interface); 
  void (*pre_clock_switch) (uint32 base_addr, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);
  void (*post_clock_switch) (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

  /* voltage switch api */
  
  /* power collapse api */
  void (*enter_deep_power_down) (SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);
  void (*exit_deep_power_down) (SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_freq_in_khz);

  /* xo shutdown api */
   
  /* ddr phy api */

  /* DMM api */

}ddr_func;

#endif
