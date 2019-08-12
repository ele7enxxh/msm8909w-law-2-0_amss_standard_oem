#ifndef RFLM_DTR_TX_DAC_CTL_H
#define RFLM_DTR_TX_DAC_CTL_H

/*!
   @file
   txlm_hal.h

   @brief
   This file implements the TXLM MDM9K HAL functionality. These are platform
   dependent.

*/

/*===========================================================================

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/lm/inc/rflm_dtr_tx_dac_ctl.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
01/25/16  jhap    Updated Dac Iref LUT for supporting WTR2965 and WTR4905
02/05/15   dr      Adding MSB Cal Reprogramming.
01/08/14   kai     Update ET DAC config settings
12/20/13   cri     Changes for LTE digital modem settings
12/09/13   st      DAC Mission Mode: Restore TXDAC Mode for Companion
11/06/13   vrb     Fixes for compiler warnings
10/29/13   st      DAC Sleep Wakeup
10/29/13   st      Add IREF Update Latch for Bootup
10/29/13   st      Remove JTAG DBG DAC Disable
10/29/13   st      DAC Bootup Sequence
10/25/13   st      Fix Configuration codes
10/25/13   cvd     DAC IREF caliberation support 
10/16/13   st      Initial version.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#ifdef _cplusplus
extern "C" {
#endif

#include "txlm_intf.h"

#define DAC_PWR_ON_CLK_DIV_PGM		2
#define DAC_PWR_ON_CLK_PLL_SEL		0
#define DAC_PWR_ON_CLK_DAC1_CLK_SRC	0

#define DAC_PWRUP_TXDAC_CFG0	 0x0AA2001B
#define DAC_PWRUP_TXDAC_CFG1     0x8000D14A
#define DAC_PWRUP_TXDAC_CFG2     0xF1E004AC
#define DAC_PWRUP_TXDAC_CFG3	 0x1C
#define DAC_PWRUP_ETDAC_CFG0	0x0222021B
#define DAC_PWRUP_ETDAC_CFG1    0x8000D148
#define DAC_PWRUP_ETDAC_CFG2    0xF00404AC
#define DAC_PWRUP_ETDAC_CFG3	0x0

#define DAC_MSBCAL_TXDAC_CFG0	 	0x02A2001B
#define DAC_MSBCAL_TXDAC_CFG1  		0x8000D14C
#define DAC_MSBCAL_TXDAC_CFG2     	0xF16004AC
#define DAC_MSBCAL_TXDAC_CFG3	 	0x1C

#define DAC_DCCAL_TXDAC_CFG0	 	0x02A2011B
#define DAC_DCCAL_TXDAC_CFG1     	0x8004D044
#define DAC_DCCAL_TXDAC_CFG2     	0xF16C04AC
#define DAC_DCCAL_TXDAC_CFG3	 	0x0C

#define DAC_MSBCAL_ETDAC_CFG0	 	0x0222021B
#define DAC_MSBCAL_ETDAC_CFG1     	0x80005148
#define DAC_MSBCAL_ETDAC_CFG2     	0xF00004AC
#define DAC_MSBCAL_ETDAC_CFG3	 	0x0


#define DAC_MSBCAL_REGARRAY_MASK 0x3F
#define DAC_MSBCAL_REGARRAY_MASK_0 	0x3F
#define DAC_MSBCAL_REGARRAY_MASK_1 	0xFC0
#define DAC_MSBCAL_REGARRAY_MASK_2 	0x3F00
#define DAC_MSBCAL_REGARRAY_MASK_3 	0xFC000
#define DAC_MSBCAL_REGARRAY_SHFT_0 0
#define DAC_MSBCAL_REGARRAY_SHFT_1 6
#define DAC_MSBCAL_REGARRAY_SHFT_2 12
#define DAC_MSBCAL_REGARRAY_SHFT_3 18

#define DAC_DCCAL_REGARRAY_MASK 0x7F
#define DAC_DCCAL_REGARRAY_MASK_0 	0x7F
#define DAC_DCCAL_REGARRAY_MASK_1 	0x3F80
#define DAC_DCCAL_REGARRAY_SHFT_0 0
#define DAC_DCCAL_REGARRAY_SHFT_1 7

#define TXDAC_DC_CAL_SIZE 2
#define ETDAC_MSBCAL_SIZE 71
#define ETDAC_CAL_DATA_SIZE (ETDAC_MSBCAL_SIZE)

#define DAC_FCAL_CLK_10_CYCLES							1
#define DAC_PWR_ON_TX_ALL_DONE_WAIT_US					50  // Temporary for Torino Bringup (actual 10us)
#define TXDAC_PWR_ON_MSBCAL_MEM_WR_DONE_WAIT_US			50	// Temporary for Torino Bringup (actual 568 XO Cycles)
#define ETDAC_PWR_ON_MSBCAL_MEM_WR_DONE_WAIT_US			50  // Temporary for Torino Bringup (Actual 284 XO Cycles)
#define TXDAC_PWR_ON_DCCAL_MEM_WR_DONE_WAIT_US			50	// Temporary for Torino Bringup (Actual 8 XO Cycles)

#define DAC_IREF_LUT_LENGTH 22

typedef struct {
  uint16 iref_val_uA10;
  uint32 tx_dacn_iref_gain;
}iref_lut_t;

void rflm_dtr_remove_hwdbg_flags(void);
void rflm_dtr_tx_enable_xogate(void);
void rflm_dtr_tx_disable_xogate(void);
void rflm_dtr_tx_enable_fcal_clk(void);
void rflm_dtr_tx_disable_fcal_clk(void);
void rflm_dtr_tx_set_dac_clk_src_xo(void);
void rflm_dtr_tx_set_dac_clk_src_dtr(void);
void rflm_dtr_tx_txdac_enable_rt_cal_clk(txlm_chain_type chain);
void rflm_dtr_tx_txdac_disable_rt_cal_clk(txlm_chain_type chain);
void rflm_dtr_tx_etdac_enable_rt_cal_clk(txlm_chain_type chain);
void rflm_dtr_tx_etdac_disable_rt_cal_clk(txlm_chain_type chain);
void rflm_dtr_tx_enable_dac_clk_cgc(void);
void rflm_dtr_tx_txdac_pwr_on_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_etdac_pwr_on_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_txdac_msbcal_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_txdac_dccal_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_etdac_msbcal_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_txdac_pwr_on_restore_fusecode(txlm_chain_type chain, uint32 data);
void rflm_dtr_tx_etdac_pwr_on_restore_fusecode(txlm_chain_type chain, uint32 data);
void rflm_dtr_tx_set_rtune_range_bit(txlm_chain_type chain, uint32 rtune_range_bit);
void rflm_dtr_tx_set_avg_err_bit(txlm_chain_type chain, uint32 avg_err_bit);
void rflm_dtr_tx_txdac_pwr_on_set_mode(txlm_chain_type chain);
void rflm_dtr_tx_etdac_pwr_on_set_mode(txlm_chain_type chain);
void rflm_dtr_tx_txdac_factory_cal_set_mode(txlm_chain_type chain);
void rflm_dtr_tx_etdac_factory_cal_set_mode(txlm_chain_type chain);
void rflm_dtr_tx_dac_factory_cal_clear_mode(txlm_chain_type chain);
void rflm_dtr_tx_txdac_pwr_on_pwr_on_reset(txlm_chain_type chain);
void rflm_dtr_tx_etdac_pwr_on_pwr_on_reset(txlm_chain_type chain);
void rflm_dtr_tx_dac_set_dccal_start(txlm_chain_type chain);
void rflm_dtr_tx_dac_clear_dccal_start(txlm_chain_type chain);
void rflm_dtr_tx_txdac_load_regarray(txlm_chain_type chain, uint8* data);
void rflm_dtr_tx_etdac_load_regarray(txlm_chain_type chain, uint8* data);
void rflm_dtr_tx_txdac_read_msbcal_from_regarray(txlm_chain_type chain,uint32 mask,uint8 size,uint32 * data);
void rflm_dtr_tx_txdac_read_dccal_from_regarray(txlm_chain_type chain,uint32 mask,uint32 * data);
void rflm_dtr_tx_etdac_read_msbcal_from_regarray(txlm_chain_type chain,uint32 mask,uint8 size,uint32 * data);
void rflm_dtr_tx_txdac_pwr_on_read_all_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_etdac_pwr_on_read_all_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_dac_factory_cal_msbcal_mem_rd(txlm_chain_type chain);
void rflm_dtr_tx_dac_factory_cal_msbcal_mem_rd_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_dac_factory_cal_dccal_mem_rd(txlm_chain_type chain);
void rflm_dtr_tx_dac_factory_cal_dccal_mem_rd_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_txdac_factory_cal_read_rpoly(txlm_chain_type chain,uint32 * data);
void rflm_dtr_tx_etdac_factory_cal_read_rpoly(txlm_chain_type chain,uint32 * data);
void rflm_dtr_tx_dac_pwr_on_msbcal_mem_wr(txlm_chain_type chain);
void rflm_dtr_tx_dac_pwr_on_msbcal_mem_wr_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_dac_pwr_on_dccal_mem_wr(txlm_chain_type chain);
void rflm_dtr_tx_dac_pwr_on_dccal_mem_wr_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_txdac_pwrdn(txlm_chain_type chain);
void rflm_dtr_tx_etdac_pwrdn(txlm_chain_type chain);
void rflm_dtr_tx_txdac_en(txlm_chain_type chain);
void rflm_dtr_tx_etdac_en(txlm_chain_type chain);
void rflm_dtr_tx_start_dac_clk(uint8 dac_div_pgm, uint8 dac_pll_sel, uint8 dac1_pll_src);
void rflm_dtr_tx_read_qfuse(txlm_chain_type chain, uint32* qfuse_rd);
void rflm_dtr_tx_read_overflow_bit(txlm_chain_type chain, uint32* overflow_bit);
void rflm_dtr_tx_read_0_1_fuseflag_bit(uint32* fuseflag_bit);
void rflm_dtr_tx_read_avg_error(txlm_chain_type chain, uint32* avg_error);
void rflm_dtr_tx_read_dac_status_tx_all_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_et_all_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_msbcal_wr_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_dccal_wr_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_i_q_en(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_et_en(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_dccal_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_set_manual_iref_update(txlm_chain_type chain);
void rflm_dtr_tx_clear_manual_iref_update(txlm_chain_type chain);
void rflm_dtr_tx_dac_power_up_clk_enable(void);
void rflm_dtr_tx_txdac_power_up(txlm_chain_type chain);
void rflm_dtr_tx_txdac_power_down(txlm_chain_type chain);
void rflm_dtr_tx_txdac_reactivate(txlm_chain_type chain, boolean blocking);
void rflm_dtr_tx_etdac_power_up(txlm_chain_type chain);
void rflm_dtr_tx_etdac_power_down(txlm_chain_type chain);
void rflm_dtr_tx_etdac_reactivate(txlm_chain_type chain, boolean blocking);
void rflm_dtr_tx_dac_iref_cfg(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_cfg(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_txdac_reprogram_msbcal_code(txlm_chain_type chain);


#ifdef _cplusplus
}
#endif

#endif /* RFLM_DTR_TX_DAC_CTL_H */

