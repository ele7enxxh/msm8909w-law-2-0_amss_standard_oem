/**
 * @file  bimc_config_8092.c
 * @brief  the bimc configurations for mpq8092, including the static setting,
 * read latency, write latency, MR values based on different frequency plans
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                                             INCLUDES
==============================================================================*/

#include "bimc_config.h"
#include "ddr_params.h"
#include "HALhwio.h"
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "bimc_struct.h"

/*==============================================================================
                                            VARIABLE DEFINITIONS
==============================================================================*/
/*note: MPQ8092 is now using JEDEC loose value for RL WL*/
/* all MR setting needs revisited */
uint32 BIMC_NUM_BYTES = 4;
uint32 BIMC_chip_id = 2;

#define PCDDR3_NUM_FREQ_TH   10   
#define PCDDR3_NUM_PARAMS     8
//[][0]-[][7]:   freq_th -  WL -  RL - MR0  - MR1_without_wrlvl - MR1_with_wrlvl - MR2 -  clock_mode 
//clk_mode (0: legacy mode; 1: pll mode) 
uint32 PCDDR3_Freq_Param[PCDDR3_NUM_FREQ_TH][PCDDR3_NUM_PARAMS] = { {333000,   5,    6,     0x220,  0x40,   0xc0,      0,  0}, \
  {400000,   5,    6,     0x420,  0x40,   0xc0,      0,  0}, \
  {466000,   6,    8,     0x640,  0x40,   0xc0,    0x8,  0}, \
  {533000,   6,    8,     0x840,  0x40,   0xc0,    0x8,  0}, \
  {600000,   7,    8,     0xA40,  0x40,  0xc0,    0x10,  1}, \
  {667000,   7,   10,     0xA60,  0x40,  0xc0,    0x10, 1}, \
  {800000,   8,   11,    0xC70,  0x40,  0xc0,    0x18,  1}, \
  {900000,   9,   14,    0xE24,  0x40,  0xc0,   0x20,   1}, \
  {933000,   9,   14,    0xE24,  0x04,  0x84,   0x20,   1}, \
  {2000000, 10,  14,    0x024,  0x04,  0x84,   0x28,   1}};

uint32 PCDDR3_bimc_global0_config[][2] =
{
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_2X_MODE_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_2X_MODE_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_PERIOD_ADDR(0,0), 0x100003AB},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CLK_PERIOD_ADDR(0,1), 0x100003AB},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_PHY_CLK_SEL_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_PHY_CLK_SEL_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_SCMON_LOCAL_CGC_THRESHOLD_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_SCMON_LOCAL_CGC_THRESHOLD_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DPEN_LOCAL_CGC_THRESHOLD_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DPEN_LOCAL_CGC_THRESHOLD_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_MPORTN_CLK_LOCAL_CGC_THRESHOLD_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_MPORTN_CLK_LOCAL_CGC_THRESHOLD_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_MPORTN_CLK_LOCAL_CGC_THRESHOLD_ADDR(0,2), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_MPORTN_CLK_LOCAL_CGC_THRESHOLD_ADDR(0,3), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_SWAY_LOCAL_CGC_THRESHOLD_ADDR(0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_SCMON_CGC_CFG_ADDR(0,0), 0x0100000B},
  {HWIO_BIMC_MISC_GLOBAL_CSR_SCMON_CGC_CFG_ADDR(0,1), 0x0100000B},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DPEN_CGC_CFG_ADDR(0,0), 0x0100000B},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DPEN_CGC_CFG_ADDR(0,1), 0x0100000B},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_0_ADDR(0,0), 0x10000014},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_0_ADDR(0,1), 0x10000014},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_1_ADDR(0,0), 0x00000100},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_1_ADDR(0,1), 0x00000100},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_2_ADDR(0,0), 0x10000100},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_2_ADDR(0,1), 0x10000100},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_3_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAL_CFG_3_ADDR(0,1), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CAL_ENA_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CAL_ENA_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_DDRCC_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_DDRCC_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_CAPHY_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_CAPHY_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_DQPHY_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLKEN_DQPHY_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLK_READY_OVERRIDE_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_SW_CLK_READY_OVERRIDE_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_DDRCC_CLK_EXTEND_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_DDRCC_CLK_EXTEND_ADDR(0,1), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAPHY_CLK_EXTEND_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_CAPHY_CLK_EXTEND_ADDR(0,1), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_DQPHY_CLK_EXTEND_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_DQPHY_CLK_EXTEND_ADDR(0,1), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DFIN_LP_CFG_ADDR(0,0), 0x0000000C},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DFIN_LP_CFG_ADDR(0,1), 0x0000000C},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_CFG_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_CFG_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE1_CTRL_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE1_CTRL_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE2_CTRL_ADDR(0,0), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE2_CTRL_ADDR(0,1), 0x00000000},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE1_HANDSHAKE_CTRL_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE1_HANDSHAKE_CTRL_ADDR(0,1), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE2_HANDSHAKE_CTRL_ADDR(0,0), 0x00000002},
  {HWIO_BIMC_MISC_GLOBAL_CSR_DDR_CHN_HFSC_STAGE2_HANDSHAKE_CTRL_ADDR(0,1), 0x00000002},
  {0x0, 0x0}
};

uint32 PCDDR3_bimc_scmo_config[][2] =
{
  {HWIO_SCMO_CFG_INTERRUPT_CLEAR_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_INTERRUPT_ENABLE_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_CLOCK_CTRL_ADDR(0), 0x00111111},
  {HWIO_SCMO_CFG_SLV_INTERLEAVE_CFG_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_BASE_CSN_ADDR(0,0), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_BASE_CSN_ADDR(0,1), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_MAP_CSN_ADDR(0,0), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_MAP_CSN_ADDR(0,1), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR(0,0), 0x00000000},
  {HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR(0,1), 0x00000000},
  {HWIO_SCMO_CFG_GLOBAL_MON_CFG_ADDR(0), 0x00000001},
  {HWIO_SCMO_CFG_CMD_BUF_CFG_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_RCH_SELECT_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_RCH_BKPR_CFG_ADDR(0), 0x30307070},
  {HWIO_SCMO_CFG_WCH_BUF_CFG_ADDR(0), 0x00000001},
  {HWIO_SCMO_CFG_FLUSH_CFG_ADDR(0), 0x00320F08},
  {HWIO_SCMO_CFG_FLUSH_CMD_ADDR(0), 0x00000000},
  {HWIO_SCMO_CFG_CMD_OPT_CFG0_ADDR(0), 0x00021110},
  {HWIO_SCMO_CFG_CMD_OPT_CFG1_ADDR(0), 0x010A0B1F},
  {HWIO_SCMO_CFG_CMD_OPT_CFG2_ADDR(0), 0x00000804},
  {HWIO_SCMO_CFG_CMD_OPT_CFG3_ADDR(0), 0x0004001F},
  {HWIO_SCMO_CFG_CMD_OPT_CFG4_ADDR(0), 0x00000011},
  {0x0, 0x0}
};

uint32 PCDDR3_bimc_dpe_config[][2] =
{
  {HWIO_ABHN_DPE_CONFIG_0_ADDR(0), 0x11010010},
  {HWIO_ABHN_DPE_CONFIG_1_ADDR(0), 0x32000000},
  {HWIO_ABHN_DPE_CONFIG_2_ADDR(0), 0x10810811},
  {HWIO_ABHN_DPE_CONFIG_3_ADDR(0), 0x17000012},
  {HWIO_ABHN_DPE_CONFIG_4_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_CONFIG_5_ADDR(0), 0x3FFFF7F7},
  {HWIO_ABHN_DPE_CONFIG_6_ADDR(0), 0x00001000},
  {HWIO_ABHN_DPE_CONFIG_7_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_CONFIG_DQ_MAP_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_ODT_CONFIG_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_ODT_CONFIG_1_ADDR(0), 0x61220210},
  {HWIO_ABHN_DPE_CA_TRAIN_PRE_CS_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_CA_TRAIN_CS_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_CA_TRAIN_POST_CS_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_RCW_CTRL_ADDR(0), 0x00000055},
  {HWIO_ABHN_DPE_PWR_CTRL_0_ADDR(0), 0x01010060},
  {HWIO_ABHN_DPE_OPT_CTRL_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_OPT_CTRL_1_ADDR(0), 0x00001080},
  {HWIO_ABHN_DPE_INTERRUPT_ENABLE_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_INTERRUPT_CLR_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_INTERRUPT_SET_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_ELEVATE_PRI_RD_ADDR(0), 0x40404000},
  {HWIO_ABHN_DPE_ELEVATE_PRI_WR_ADDR(0), 0x80808000},
  {HWIO_ABHN_DPE_TIMER_0_ADDR(0), 0x02102000},
  {HWIO_ABHN_DPE_TIMER_1_ADDR(0), 0x00000040},
  {HWIO_ABHN_DPE_TIMER_2_ADDR(0), 0x00000040},
  {HWIO_ABHN_DPE_TIMER_3_ADDR(0), 0x00000400},
  {HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_1_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_2_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_3_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_1_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_2_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_3_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_MRW_FREQ_SWITCH_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_0_ADDR(0), 0x0000014A},
  {HWIO_ABHN_DPE_DRAM_TIMING_1_ADDR(0), 0x00960082},
  {HWIO_ABHN_DPE_DRAM_TIMING_2_ADDR(0), 0x404B403C},
  {HWIO_ABHN_DPE_DRAM_TIMING_3_ADDR(0), 0x0A280A28},
  {HWIO_ABHN_DPE_DRAM_TIMING_4_ADDR(0), 0x0000404B},
  {HWIO_ABHN_DPE_DRAM_TIMING_5_ADDR(0), 0x00820082},
  {HWIO_ABHN_DPE_DRAM_TIMING_6_ADDR(0), 0x015E3032},
  {HWIO_ABHN_DPE_DRAM_TIMING_7_ADDR(0), 0x00400320},
  {HWIO_ABHN_DPE_DRAM_TIMING_8_ADDR(0), 0x0000C096},
  {HWIO_ABHN_DPE_DRAM_TIMING_9_ADDR(0), 0x00004032},
  {HWIO_ABHN_DPE_DRAM_TIMING_10_ADDR(0), 0x02005A8C},
  {HWIO_ABHN_DPE_DRAM_TIMING_11_ADDR(0), 0x303C303C},
  {HWIO_ABHN_DPE_DRAM_TIMING_12_ADDR(0), 0x303CA0F0},
  {HWIO_ABHN_DPE_DRAM_TIMING_13_ADDR(0), 0x5064708C},
  {HWIO_ABHN_DPE_DRAM_TIMING_14_ADDR(0), 0x01003011},
  {HWIO_ABHN_DPE_DRAM_TIMING_15_ADDR(0), 0x0000A0B1},
  {HWIO_ABHN_DPE_DRAM_TIMING_16_ADDR(0), 0x00040E0A},
  {HWIO_ABHN_DPE_DRAM_TIMING_17_ADDR(0), 0x01000C80},
  {HWIO_ABHN_DPE_DRAM_TIMING_18_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_19_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_20_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_21_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_22_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_DRAM_TIMING_23_ADDR(0), 0x02020100},
  {HWIO_ABHN_DPE_TESTBUS_CTRL_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_SPARE_REG_ADDR(0), 0x00000000},
  {HWIO_ABHN_DPE_CGC_CTRL_ADDR(0), 0x0001FFFF},
  {0x0, 0x0}
};

uint32 PCDDR3_bimc_shke_config[][2] =
{
  {HWIO_ABHN_SHKE_CONFIG_ADDR(0), 0x001F8001},
  {HWIO_ABHN_SHKE_CGC_CNTL_ADDR(0), 0x00000001},
  {HWIO_ABHN_SHKE_DRAM_MANUAL_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_DRAM_MANUAL_1_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_MREG_ADDR_WDATA_CNTL_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_DRAM_DEBUG_CMD_0_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_DRAM_DEBUG_CMD_1_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_DRAM_DEBUG_CMD_2_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_PERIODIC_MRR_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_PERIODIC_ZQCAL_ADDR(0), 0x00000800},
  {HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_ADDR(0), 0x00000094},
  {HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_ADDR(0), 0x00032000},
  {HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_1_ADDR(0), 0x00000094},
  {HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_2_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_TESTBUS_CNTL_ADDR(0), 0x00000000},
  {HWIO_ABHN_SHKE_SPARE_REG_ADDR(0), 0x00000000},
  {0x0, 0x0}
};

#define LPDDR3_NUM_FREQ_TH   1
#define LPDDR3_NUM_PARAMS    8
uint32 LPDDR3_Freq_Param[LPDDR3_NUM_FREQ_TH][LPDDR3_NUM_PARAMS] ={0,0,0,0,0,0,0,0};
uint32 LPDDR3_bimc_global0_config[][2] = { {0x0, 0x0} };
uint32 LPDDR3_bimc_scmo_config[][2] = { {0x0, 0x0} };
uint32 LPDDR3_bimc_dpe_config[][2] = { {0x0, 0x0} };
uint32 LPDDR3_bimc_shke_config[][2] = { {0x0, 0x0} };

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/ 
/* =============================================================================
 **  Function : BIMC_GLOBAL0_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_GLOBAL0 static settings
 **/
void 
BIMC_GLOBAL0_Set_Config( uint32 reg_base, SDRAM_INTERFACE interface)
{
  uint32 reg;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (ddr_params->common.device_type == SDRAM_LPDDR3){   
    if (LPDDR3_bimc_global0_config != NULL)
    {
      for (reg = 0; LPDDR3_bimc_global0_config[reg][0] != 0; reg++)
      {
        out_dword(LPDDR3_bimc_global0_config[reg][0] + reg_base, LPDDR3_bimc_global0_config[reg][1]);
      }
    }
  } 
  else if (ddr_params->common.device_type == SDRAM_PCDDR3){       
    if (PCDDR3_bimc_global0_config != NULL)
    {
      for (reg = 0; PCDDR3_bimc_global0_config[reg][0] != 0; reg++)
      {
        out_dword(PCDDR3_bimc_global0_config[reg][0] + reg_base, PCDDR3_bimc_global0_config[reg][1]);
      }
    }
  }
}

/* =============================================================================
 **  Function : BIMC_SCMO_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_SCMO static settings
 **/
void 
BIMC_SCMO_Set_Config( uint32 reg_base, SDRAM_INTERFACE interface)
{
  uint32 reg; 
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (ddr_params->common.device_type == SDRAM_LPDDR3){   
    if (LPDDR3_bimc_scmo_config != NULL)
    {
      for (reg = 0; LPDDR3_bimc_scmo_config[reg][0] != 0; reg++)
      {
        out_dword(LPDDR3_bimc_scmo_config[reg][0] + reg_base, LPDDR3_bimc_scmo_config[reg][1]);
      }
    }
  } 
  else if (ddr_params->common.device_type == SDRAM_PCDDR3){       
    if (PCDDR3_bimc_scmo_config != NULL)
    {
      for (reg = 0; PCDDR3_bimc_scmo_config[reg][0] != 0; reg++)
      {
        out_dword(PCDDR3_bimc_scmo_config[reg][0] + reg_base, PCDDR3_bimc_scmo_config[reg][1]);
      }
    }
  }

}

/* =============================================================================
 **  Function : BIMC_DPE_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_DPE static settings
 **/
void 
BIMC_DPE_Set_Config ( uint32 reg_base, SDRAM_INTERFACE interface)
{
  uint32 reg;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (ddr_params->common.device_type == SDRAM_LPDDR3){   
    if (LPDDR3_bimc_dpe_config != NULL)
    {
      for (reg = 0; LPDDR3_bimc_dpe_config[reg][0] != 0; reg++)
      {
        out_dword(LPDDR3_bimc_dpe_config[reg][0] + reg_base, LPDDR3_bimc_dpe_config[reg][1]);
      }
    }
  } 
  else if (ddr_params->common.device_type == SDRAM_PCDDR3){       
    if (PCDDR3_bimc_dpe_config != NULL)
    {
      for (reg = 0; PCDDR3_bimc_dpe_config[reg][0] != 0; reg++)
      {
        out_dword(PCDDR3_bimc_dpe_config[reg][0] + reg_base, PCDDR3_bimc_dpe_config[reg][1]);
      }
    }
  }   

}

/* =============================================================================
 **  Function : BIMC_SHKE_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program BIMC_SHKE static settings
 **/
void 
BIMC_SHKE_Set_Config ( uint32 reg_base, SDRAM_INTERFACE interface)
{
  uint32 reg;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (ddr_params->common.device_type == SDRAM_LPDDR3){   
    if (LPDDR3_bimc_shke_config != NULL)
    {
      for (reg = 0; LPDDR3_bimc_shke_config[reg][0] != 0; reg++)
      {
        out_dword(LPDDR3_bimc_shke_config[reg][0] + reg_base, LPDDR3_bimc_shke_config[reg][1]);
      }
    }
  } 
  else if (ddr_params->common.device_type == SDRAM_PCDDR3){       
    if (PCDDR3_bimc_shke_config != NULL)
    {
      for (reg = 0; PCDDR3_bimc_shke_config[reg][0] != 0; reg++)
      {
        out_dword(PCDDR3_bimc_shke_config[reg][0] + reg_base, PCDDR3_bimc_shke_config[reg][1]);
      }
    }
  }   


}

/* =============================================================================
 **  Function : Get_BIMC_LUT
 ** =============================================================================
 */
/**
 *    @details
 *     obtain WL/ RL/ MR setting from BIMC LUT
 **/

uint32 
Get_BIMC_LUT (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 which_LUT) {
  uint32 i, index = 0; 
  uint32 LUT_value = 0;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (ddr_params->common.device_type == SDRAM_LPDDR3){     
    for (i = 0; i < LPDDR3_NUM_FREQ_TH;  i ++) {
      if (clk_freq_in_khz <= LPDDR3_Freq_Param[i][0]) {
        index = i;
        break;
      }
    }
    if(which_LUT < (LPDDR3_NUM_PARAMS -1))
      LUT_value = LPDDR3_Freq_Param[index][which_LUT+1];
    
    return(LUT_value);
  }
  else if    (ddr_params->common.device_type == SDRAM_PCDDR3){       
    for (i = 0; i < PCDDR3_NUM_FREQ_TH;  i ++) {
      if (clk_freq_in_khz <= PCDDR3_Freq_Param[i][0]) {
        index = i;
        break;
      }
    }
    if(which_LUT < (PCDDR3_NUM_PARAMS -1))
      LUT_value = PCDDR3_Freq_Param[index][which_LUT+1];
 
    return(LUT_value);
  }
  return(0);
}
