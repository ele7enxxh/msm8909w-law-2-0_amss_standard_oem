/**
 * @file  ddrcc_config.c
 * @brief  the ebi ddrcc static setting
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
10/28/14     sk           Initial version.
================================================================================
                   Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "ddrcc_config.h"
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"

/*==============================================================================
                                           VARIABLE DEFINITIONS                     
==============================================================================*/
uint32 ddrcc_config[][2] =
  {
    {HWIO_DDR_CC_DDR_CLK_CTRL_ADDR(0), 0x00008402},
    {HWIO_DDR_CC_DDR_CLK_SWITCH_CTRL_ADDR(0), 0x00000100},
    {HWIO_DDR_CC_JCPLL_CTRL_ADDR(0), 0x010301A6},
    {HWIO_DDR_CC_TPD_CTRL_ADDR(0), 0x0141049A},
    {HWIO_DDR_CC_DLL_CTRL_ADDR(0), 0xC8F13001},
    {HWIO_DDR_CC_TX_RX_CTRL_ADDR(0), 0x00000220},
    {HWIO_DDR_CC_DLL_AVERAGING_ADDR(0), 0x00000101},
    {HWIO_DDR_CC_HIST_CTRL1_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_HIST_CTRL2_ADDR(0), 0x0008000B},
    {HWIO_DDR_CC_DEBUG_SELECT_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_TEST_CLOCK_SELECT_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CGC_CTRL_ADDR(0), 0x00007004},
    {HWIO_DDR_CC_DLL0_MODE_ADDR(0), 0x00001000},
    {HWIO_DDR_CC_DLL1_MODE_ADDR(0), 0x00001000},
    {HWIO_DDR_CC_JCPLL0_MODE_ADDR(0), 0x00000005},
    {HWIO_DDR_CC_JCPLL0_L_VAL_ADDR(0), 0x00000004},
    {HWIO_DDR_CC_JCPLL0_M_VAL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_JCPLL0_N_VAL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_JCPLL0_USER_CTRL_ADDR(0), 0x00100060},
    {HWIO_DDR_CC_JCPLL0_CONFIG_CTRL_ADDR(0), 0x00141200},
    {HWIO_DDR_CC_JCPLL0_TEST_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_JCPLL1_MODE_ADDR(0), 0x00000005},
    {HWIO_DDR_CC_JCPLL1_L_VAL_ADDR(0), 0x00000004},
    {HWIO_DDR_CC_JCPLL1_M_VAL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_JCPLL1_N_VAL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_JCPLL1_USER_CTRL_ADDR(0), 0x00100060},
    {HWIO_DDR_CC_JCPLL1_CONFIG_CTRL_ADDR(0), 0x00141200},
    {HWIO_DDR_CC_JCPLL1_TEST_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_RX_CONFIG_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_RX_USER_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_RX_TEST_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_TX_CONFIG_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_TX_USER_CTRL_ADDR(0), 0x000000C0},
    {HWIO_DDR_CC_CSD_TX_TEST_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_DLL0_CONFIG_CTRL_ADDR(0), 0x00000200},
    {HWIO_DDR_CC_CSD_DLL0_USER_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_DLL0_TEST_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_DLL1_CONFIG_CTRL_ADDR(0), 0x00000200},
    {HWIO_DDR_CC_CSD_DLL1_USER_CTRL_ADDR(0), 0x00000000},
    {HWIO_DDR_CC_CSD_DLL1_TEST_CTRL_ADDR(0), 0x00000000},
    {0x0, 0x0}  };
  
/*==============================================================================
                                  FUNCTIONS
==============================================================================*/ 
/* =============================================================================
 **  Function : EBI_DDRCC_Set_Config
 ** =============================================================================
 */
/**
 *    @details
 *     program EBI_DDRCC static settings
 **/
void 
EBI_DDRCC_Set_Config(uint32 reg_base)
{
  uint32 reg;
  
  if ( ddrcc_config != NULL) {
    for (reg = 0; ddrcc_config[reg][0] != 0; reg++)
    {
      out_dword(ddrcc_config[reg][0] + reg_base, ddrcc_config[reg][1]);
    }
  }  
}
