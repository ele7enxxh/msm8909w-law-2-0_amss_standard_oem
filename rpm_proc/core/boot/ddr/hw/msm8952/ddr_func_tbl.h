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

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8952/ddr_func_tbl.h#1 $
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
#include "ddr_target.h"

/*==============================================================================
                                  STRUCTURE
==============================================================================*/
/* function table for sbl and RPM to share */
typedef struct 
{
  /* self refresh api */
  void (*enter_self_refresh) (SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);
  void (*exit_self_refresh) (SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);
  
  /* clock switch api */
  uint32 (*get_read_latency) (SDRAM_INTERFACE interface, uint32 clk_speed);
  uint32 (*get_write_latency) (SDRAM_INTERFACE interface, uint32 clk_speed); 
  void (*pre_clock_switch) (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);
  void (*post_clock_switch) (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

  /* voltage switch api */
  
  /* low power api */
  void (*ebi_phy_cfg_disable_csd_rx) (uint32 base, uint32 chnl, int selfrefresh_mode);
  void (*ebi_phy_cfg_enable_csd_rx) (uint32 base, uint32 chnl, phy_clk_type clk_mode );

  /* xo shutdown api */
   
  /* ddr phy api */

  /* DMM api */

  /* currently not supported */

}ddr_func;

#endif
