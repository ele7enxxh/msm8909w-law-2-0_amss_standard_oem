
#ifndef _DDR_PHY_INIT_EBI_SEQ_HEADER_
#define _DDR_PHY_INIT_EBI_SEQ_HEADER_
/*=============================================================================

                                DDR PARAMETERS
                                Header File
GENERAL DESCRIPTION
TBD

               Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
					 
					 
 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.

 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/hw_sequence/PHY/v2.5/ddr_phy_ebi.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who           what, where, why
--------   --------      -------------------------------------------------------
07/07/14   amoussav      Initial version. 
==============================================================================*/

/*==============================================================================
                               INCLUDE FILES
==============================================================================*/
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "ddr_phy_ca.h"
#include "ddr_phy_dq.h"
#include "ddr_phy_config.h"
/*==============================================================================
                                  TYPES
==============================================================================*/
typedef enum {
	dynamic_legacy = 0,
	static_legacy = 1,
	srpll_mode = 2
	}phy_clk_type;



/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_phy_init
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] device_type           TBD
*   @param[in] chnl           TBD
*   @param[in] clk_mode           TBD
*   @param[in] odt_en           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/


void EBI1_PHY_CFG_phy_init( uint32 _inst_, uint32 device_type, uint32 chnl, phy_clk_type clk_mode, uint32 odt_en );
void EBI1_PHY_CFG_phy_init_1( uint32 _inst_, uint32 device_type, uint32 chnl, phy_clk_type clk_mode, uint32 odt_en );//Ali temp
void EBI1_PHY_CFG_phy_init_2( uint32 _inst_, uint32 device_type, uint32 chnl, phy_clk_type clk_mode, uint32 odt_en );//Ali temp


/* =============================================================================
**  Function Name: EBI1_PHY_CFG_auto_io_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_auto_io_cal( uint32 _inst_, uint32 chnl );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_disable_auto_io_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_disable_auto_io_cal( uint32 _inst_, uint32 chnl );

extern void ddr_mem_write_low_speed(uint16 channel_num, uint16 rank, uint32 *pattern, uint16 size, uint16 burst);
extern chip_platform_info Ddr_get_chip_platform_info(void );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dcc_cal_ca
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl                             TBD
*   @param[in] bypass                           TBD
*   @param[in] dcc_manual                       TBD
*   @param[in] dcc_sw_ovrd                      TBD
*   @param[in] pll_sel                          TBD
*   @param[in] dcc_wait_time                    TBD
*   @param[in] div_ref                          TBD
*   @param[in] ch0_wr90_dcc_man_adj             TBD
*   @param[in] ch0_wr90_dcc_adj_sw_ovrd         TBD
*   @param[in] ch0_wr90_dcc_man_adj_pll2        TBD
*   @param[in] ch0_wr90_dcc_adj_sw_ovrd_pll2    TBD
*   @param[in] ch0_wrlvl_dcc_man_adj            TBD
*   @param[in] ch0_wrlvl_dcc_adj_sw_ovrd        TBD
*   @param[in] ch0_wrlvl_dcc_man_adj_pll2       TBD
*   @param[in] ch0_wrlvl_dcc_adj_sw_ovrd_pll2   TBD
*   @param[in] ch1_wr90_dcc_man_adj             TBD
*   @param[in] ch1_wr90_dcc_adj_sw_ovrd         TBD
*   @param[in] ch1_wr90_dcc_man_adj_pll2        TBD
*   @param[in] ch1_wr90_dcc_adj_sw_ovrd_pll2    TBD
*   @param[in] ch1_wrlvl_dcc_man_adj            TBD
*   @param[in] ch1_wrlvl_dcc_adj_sw_ovrd        TBD
*   @param[in] ch1_wrlvl_dcc_man_adj_pll2       TBD
*   @param[in] ch1_wrlvl_dcc_adj_sw_ovrd_pll2   TBD


*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_dcc_cal_ca( uint32 _inst_, uint32 chnl, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 ch0_wr90_dcc_man_adj[2], uint32 ch0_wr90_dcc_adj_sw_ovrd[2], uint32 ch0_wr90_dcc_man_adj_pll2[2], uint32 ch0_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch0_wrlvl_dcc_man_adj[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch0_wrlvl_dcc_man_adj_pll2[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wr90_dcc_man_adj[2], uint32 ch1_wr90_dcc_adj_sw_ovrd[2], uint32 ch1_wr90_dcc_man_adj_pll2[2], uint32 ch1_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wrlvl_dcc_man_adj[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch1_wrlvl_dcc_man_adj_pll2[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dcc_cal_dq
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl                            TBD
*   @param[in] dq_num                          TBD
*   @param[in] bypass                          TBD
*   @param[in] dcc_manual                      TBD
*   @param[in] dcc_sw_ovrd                     TBD
*   @param[in] pll_sel                         TBD
*   @param[in] dcc_wait_time                   TBD
*   @param[in] div_ref[2]                      TBD
*   @param[in] ch0_wr90_dcc_man_adj            TBD
*   @param[in] ch0_wr90_dcc_adj_sw_ovrd        TBD
*   @param[in] ch0_wr90_dcc_man_adj_pll2       TBD
*   @param[in] ch0_wr90_dcc_adj_sw_ovrd_pll2   TBD
*   @param[in] ch0_wrlvl_dcc_man_adj           TBD
*   @param[in] ch0_wrlvl_dcc_adj_sw_ovrd       TBD
*   @param[in] ch0_wrlvl_dcc_man_adj_pll2      TBD
*   @param[in] ch0_wrlvl_dcc_adj_sw_ovrd_pll2  TBD
*   @param[in] ch1_wr90_dcc_man_adj            TBD
*   @param[in] ch1_wr90_dcc_adj_sw_ovrd\       TBD
*   @param[in] ch1_wr90_dcc_man_adj_pll2       TBD
*   @param[in] ch1_wr90_dcc_adj_sw_ovrd_pll2   TBD
*   @param[in] ch1_wrlvl_dcc_man_adj           TBD
*   @param[in] ch1_wrlvl_dcc_adj_sw_ovrd       TBD
*   @param[in] ch1_wrlvl_dcc_man_adj_pll2      TBD
*   @param[in] ch1_wrlvl_dcc_adj_sw_ovrd_pll2  TBD


*   @return
*   void
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_dcc_cal_dq( uint32 _inst_, uint32 chnl, uint32 dq_num, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 ch0_wr90_dcc_man_adj[2], uint32 ch0_wr90_dcc_adj_sw_ovrd[2], uint32 ch0_wr90_dcc_man_adj_pll2[2], uint32 ch0_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch0_wrlvl_dcc_man_adj[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch0_wrlvl_dcc_man_adj_pll2[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wr90_dcc_man_adj[2], uint32 ch1_wr90_dcc_adj_sw_ovrd[2], uint32 ch1_wr90_dcc_man_adj_pll2[2], uint32 ch1_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wrlvl_dcc_man_adj[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch1_wrlvl_dcc_man_adj_pll2[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_apply_dcc_cal_adj
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl                  TBD
*   @param[in] bypass                TBD
*   @param[in] dcc_manual            TBD
*   @param[in] dcc_sw_ovrd           TBD
*   @param[in] pll_sel               TBD
*   @param[in] dcc_wait_time         TBD
*   @param[in] div_ref               TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_apply_dcc_cal_adj( uint32 _inst_, uint32 chnl, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dcc_init
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_              Base address
*   @param[in] chnl                TBD
*   @param[in] jcpll_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_dcc_init( uint32 _inst_, uint32 chnl, uint32 jcpll_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dcc_cal_stop
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_         Base address
*   @param[in] chnl           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_dcc_cal_stop( uint32 _inst_, uint32 chnl );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_pads_odt_enable
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_            Base address
*   @param[in] chnl              TBD
*   @param[in] odt_ena           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_pads_odt_enable( uint32 _inst_, uint32 chnl, uint32 odt_ena );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_clk_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl                TBD
*   @param[in] clk_mode            TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_clk_mode( uint32 _inst_, uint32 chnl, phy_clk_type clk_mode );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_pnslew_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_pnslew_update( uint32 _inst_, uint32 chnl );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_enable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] clk_mode           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_enable_csd_rx( uint32 _inst_, uint32 chnl, phy_clk_type clk_mode );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_disable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] selfrefresh_mode           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_disable_csd_rx( uint32 _inst_, uint32 chnl, int selfrefresh_mode);
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_ca_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] dcc_man_adj           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_ca_dcc_cal( uint32 _inst_, uint32 chnl, uint32 dcc_man_adj[2][2][2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_dq_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] dq_num           TBD
*   @param[in] dcc_man_adj           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_dq_dcc_cal( uint32 _inst_, uint32 chnl, uint32 dq_num, uint32 dcc_man_adj[2][2][2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_ca_neg_wrlvl
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_ca_neg_wrlvl( uint32 _inst_, uint32 chnl);
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_print_log
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_print_log( uint32 _inst_ );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_dq_pad_pull
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] pull_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_dq_pad_pull( uint32 _inst_, uint32 chnl, uint32 pull_value);

/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dq_pad_pull_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   uint32
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_dq_pad_pull_status( uint32 _inst_, uint32 chnl);
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_dqs_pad_pull
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] pull_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_dqs_pad_pull( uint32 _inst_, uint32 chnl, uint32 pull_value);
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_dqs_pad_pull_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*
*   @return
*   uint32
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_dqs_pad_pull_status( uint32 _inst_, uint32 chnl);
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_hw_info
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_hw_info( uint32 _inst_);
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_remap_byte
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chip_id           TBD
*   @param[in] which_byte           TBD
*
*   @return
*   uint32
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_remap_byte( uint32 _inst_, uint32 chip_id, uint32 which_byte );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_ren_delay_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chip_id           TBD
*   @param[in] channel_select           TBD
*   @param[in] which_ren_cfg           TBD
*   @param[in] which_byte           TBD
*   @param[in] value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_ren_delay_update( uint32 _inst_, uint32 chip_id, uint32 channel_select, uint32 which_ren_cfg, uint32 which_byte, uint32 value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_ren_sw_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chip_select           TBD
*   @param[in] ren_cfg_num           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_ren_sw_mode( uint32 _inst_, uint32 chip_select, uint32 ren_cfg_num );

/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_ren_hw_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chip_select           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_ren_hw_mode( uint32 _inst_, uint32 chip_select );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ddr_phy_ren_reset
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chip_select           TBD
*   @param[in] byte0_reset           TBD
*   @param[in] byte1_reset           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ddr_phy_ren_reset( uint32 _inst_, uint32 chip_select, uint32 byte0_reset, uint32 byte1_reset );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_get_ren_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   uint32
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_get_ren_cfg( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_REN_Get_Trained_Results
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] RCW_Hcycle_Delay_Result           TBD
*   @param[in] RCW_Coarse_Delay_Result           TBD
*   @param[in] RCW_Fine_Delay_Result           TBD
*   @param[in] dq_ren_training           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_REN_Get_Trained_Results(uint32 which_ren_cfg, SDRAM_INTERFACE channel_sel, uint32 RCW_Hcycle_Delay_Result[7][2][8], uint32 RCW_Coarse_Delay_Result[7][2][8], uint32 RCW_Fine_Delay_Result[7][2][8], uint16 dq_ren_training[2][7][8]);
/* =============================================================================
**  Function Name: EBI1_PHY_REN_Set_Trained_results
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_ren_training           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_REN_Set_Trained_results( uint32 _inst_, uint16 dq_ren_training[2][7][8]);
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************


/* =============================================================================
**  Function Name: EBI1_PHY_CFG_enable_wrlvl_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_enable_wrlvl_mode( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_disable_wrlvl_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_disable_wrlvl_mode( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] delay_index           TBD
*   @param[in] wrlvl_delay_val           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 delay_index, uint32 wrlvl_delay_val );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_wrlvl_cdc_retimer
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] cfg_index           TBD
*   @param[in] byte0_retimer_sel_LUT_value           TBD
*   @param[in] byte0_retimer_ie_oe_LUT_value           TBD
*   @param[in] byte0_retimer_cycle_delay_sel           TBD
*   @param[in] byte1_retimer_sel_LUT_value           TBD
*   @param[in] byte1_retimer_ie_oe_LUT_value           TBD
*   @param[in] byte1_retimer_cycle_delay_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_wrlvl_cdc_retimer( uint32 _inst_, uint32 channel_sel, uint32 cfg_index, uint32 byte0_retimer_sel_LUT_value[4], uint32 byte0_retimer_ie_oe_LUT_value[4], uint32 byte0_retimer_cycle_delay_sel[4], uint32 byte1_retimer_sel_LUT_value[4], uint32 byte1_retimer_ie_oe_LUT_value[4], uint32 byte1_retimer_cycle_delay_sel[4] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_cdc_wr90_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] delay_index           TBD
*   @param[in] delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_cdc_wr90_delay( uint32 _inst_, uint32 channel_sel, uint32 delay_index, uint32 delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_cdc_wrlvl_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] byte_index           TBD
*   @param[in] dq_index           TBD
*   @param[in] delay_index           TBD
*   @param[in] delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_cdc_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 dq_index, uint32 delay_index, uint32 delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_wrlvl_trigger_pulse
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_wrlvl_trigger_pulse( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_wrlvl_trigger_zero
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_wrlvl_trigger_zero( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_wrlvl_trigger_one
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_wrlvl_trigger_one( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_read_wrlvl_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_read_wrlvl_status( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_read_wrlvl_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] byte_index           TBD
*   @param[in] cfg_index           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_read_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 cfg_index );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_get_wrlvl_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   uint32
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_get_wrlvl_cfg( uint32 _inst_, uint32 channel_sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_get_wrlvl_period
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] cfg_index           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint32 EBI1_PHY_CFG_get_wrlvl_period( uint32 _inst_, uint32 channel_sel, uint32 cfg_index );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_wrlvl_freq_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] freq_index           TBD
*   @param[in] freq_val           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_wrlvl_freq_cfg( uint32 _inst_, uint32 channel_sel, uint32 freq_index, uint32 freq_val );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_cdc_wrlvl_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*   @param[in] byte_index            TBD
*   @param[in] dq_index              TBD
*   @param[in] delay_index           TBD
*   @param[in] retimer_value         TBD
*   @param[in] delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_cdc_wrlvl_training( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 dq_index, uint32 delay_index, uint16 retimer_value, uint16 delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_set_wrlvl_half_cycle_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] channel_sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_set_wrlvl_half_cycle_delay( uint32 _inst_, uint32 channel_sel );

//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*   @param[in] coarse_delay_value           TBD
*   @param[in] fine_delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay( uint32 _inst_, uint16 chnl, uint16 site, uint16 coarse_delay_value, uint16 fine_delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*   @param[in] deskew_delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint16 deskew_delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_seq_set_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*   @param[in] bitn           TBD
*   @param[in] deskew_delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_seq_set_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site,uint16 bitn, uint16 deskew_delay_value );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_cdc_wr90_delay_select
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] sel           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_cdc_wr90_delay_select( uint32 _inst_, uint16 chnl, uint16 sel );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_set_ca_cdc_wr90
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] ca_cdc_wr90_delay           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_set_ca_cdc_wr90( uint32 _inst_, uint16 chnl, uint16 site, uint16 ca_cdc_wr90_delay[4] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] mode           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training( uint32 _inst_, uint16 chnl, uint16 mode );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_seq_restore_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*   @param[in] deskew_delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_seq_restore_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint32 deskew_delay_value[2] );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_ca_training_seq_read_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*   @param[in] deskew_delay_value           TBD
*
*   @return
*   TBD
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void EBI1_PHY_CFG_ebi_ca_training_seq_read_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint32 deskew_delay_value[2] );

/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*
*   @return
*   uint16
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint16 EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training( uint32 _inst_, uint16 chnl, uint16 site );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_trahira_reverse_byte
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] original           TBD
*
*   @return
*   uint16
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint16 EBI1_PHY_CFG_trahira_reverse_byte( uint32 _inst_, uint16 original );
/* =============================================================================
**  Function Name: EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training_trahira
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] chnl           TBD
*   @param[in] site           TBD
*
*   @return
*   uint16
*
*   @dependencies
*   TBD
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
uint16 EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training_trahira( uint32 _inst_, uint16 chnl, uint16 site );

//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

void EBI1_PHY_Set_Config_dq (uint32 _inst_);
void EBI1_PHY_Set_Config_ca (uint32 _inst_);

#endif
