
#ifndef _DDR_PHY_DQ_INIT_DQ_SEQ_HEADER_
#define _DDR_PHY_DQ_INIT_DQ_SEQ_HEADER_
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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/hw_sequence/PHY/v2.5/ddr_phy_dq.h#1 $
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
/*==============================================================================
                                  TYPES
==============================================================================*/
/*NA*/



/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* =============================================================================
**  Function Name: DDRPHY_DQ_init_io_pads
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] device_type           TBD
*
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

void DDRPHY_DQ_init_io_pads( uint32 _inst_, uint32 device_type );
/* =============================================================================
**  Function Name: DDRPHY_DQ_site_clk_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] clk_mode           TBD
*
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
void DDRPHY_DQ_site_clk_mode( uint32 _inst_, int clk_mode );
/* =============================================================================
**  Function Name: DDRPHY_DQ_init_cdc
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_init_cdc( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_init_dq_csr
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_init_dq_csr( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] bypass           TBD
*   @param[in] dcc_manual           TBD
*   @param[in] dcc_sw_ovrd           TBD
*   @param[in] pll_sel           TBD
*   @param[in] dcc_wait_time           TBD
*   @param[in] dcc_wait_time           TBD
*   @param[in] div_ref           TBD
*   @param[in] dcc_man_adj           TBD
*   @param[in] dcc_adj_sw_ovrd           TBD
*   @param[in] dcc_man_adj_pll2           TBD
*   @param[in] dcc_adj_sw_ovrd_pll2           TBD
*
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
void DDRPHY_DQ_wrlvl_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_auto_dcc_calibration
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_            Base address
*   @param[in] dcc_wait_time     TBD
*   @param[in] div_ref           TBD
*
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
void DDRPHY_DQ_wrlvl_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_dcc_override
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] pll_sel           TBD
*   @param[in] dcc_adj_sw_ovrd           TBD
*   @param[in] dcc_adj_sw_ovrd_pll2           TBD
*
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
void DDRPHY_DQ_wrlvl_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_dcc_stop
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_wrlvl_dcc_stop( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_dcc_adj_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte_num           TBD
*
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
uint32 DDRPHY_DQ_wrlvl_dcc_adj_status( uint32 _inst_, uint32 byte_num );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wr90_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] bypass           TBD
*   @param[in] dcc_manual           TBD
*   @param[in] dcc_sw_ovrd           TBD
*   @param[in] pll_sel           TBD
*   @param[in] dcc_wait_time           TBD
*   @param[in] div_ref           TBD
*   @param[in] dcc_man_adj           TBD
*   @param[in] dcc_adj_sw_ovrd       TBD
*   @param[in] dcc_adj_sw_ovrd_pll2           TBD
*   @param[in] dcc_adj_sw_ovrd_pll2           TBD
*
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
void DDRPHY_DQ_wr90_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wr90_auto_dcc_calibration
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dcc_wait_time           TBD
*   @param[in] div_ref           TBD
*
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
void DDRPHY_DQ_wr90_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wr90_dcc_override
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] pll_sel                TBD
*   @param[in] dcc_adj_sw_ovrd        TBD
*   @param[in] dcc_adj_sw_ovrd_pll2   TBD
*
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
void DDRPHY_DQ_wr90_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wr90_dcc_stop
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_wr90_dcc_stop( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wr90_dcc_adj_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte_num           TBD
*
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
uint32 DDRPHY_DQ_wr90_dcc_adj_status( uint32 _inst_, uint32 byte_num );
/* =============================================================================
**  Function Name: DDRPHY_DQ_io_pads_odt_enable
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dynamic_odt_ena           TBD
*
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
void DDRPHY_DQ_io_pads_odt_enable( uint32 _inst_, uint32 dynamic_odt_ena );
/* =============================================================================
**  Function Name: DDRPHY_DQ_new_pncnt_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] pncnt_val           TBD
*
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
void DDRPHY_DQ_new_pncnt_update( uint32 _inst_, uint32 pncnt_val );
/* =============================================================================
**  Function Name: DDRPHY_DQ_enable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] clk_mode           TBD
*
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
void DDRPHY_DQ_enable_csd_rx( uint32 _inst_, int clk_mode );
/* =============================================================================
**  Function Name: DDRPHY_DQ_disable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] selfrefresh_mode           TBD
*
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
void DDRPHY_DQ_disable_csd_rx( uint32 _inst_, int selfrefresh_mode);
/* =============================================================================
**  Function Name: DDRPHY_DQ_dcc_set
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dcc_man_adj           TBD
*
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
void DDRPHY_DQ_dcc_set( uint32 _inst_, uint32 dcc_man_adj[2][2][2] );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************



#define HALF_CYCLE_MASK                                              0x0FF
#define COARSE_MASK                                                  0x307
#define FINE_MASK                                                    0x3F8


/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_sw_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*
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
void DDRPHY_DQ_ren_sw_mode( uint32 _inst_, uint32 ren_cfg_num );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_hw_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_ren_hw_mode( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_done
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_ren_done( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_sw_fine_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*   @param[in] byte0_fine_delay           TBD
*   @param[in] byte1_fine_delay           TBD
*
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
void DDRPHY_DQ_ren_sw_fine_update( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_fine_delay, uint32 byte1_fine_delay );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_sw_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*   @param[in] byte0_delay           TBD
*   @param[in] byte1_delay           TBD
*
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
void DDRPHY_DQ_ren_sw_update( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_delay, uint32 byte1_delay );
/* =============================================================================
**  Function Name: DDRPHY_DQ_write_ren_bytes_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*   @param[in] byte0_delay           TBD
*   @param[in] byte1_delay           TBD
*
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
void DDRPHY_DQ_write_ren_bytes_cfg( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_delay, uint32 byte1_delay );
/* =============================================================================
**  Function Name: DDRPHY_DQ_read_ren_byte0_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*
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
uint32 DDRPHY_DQ_read_ren_byte0_cfg( uint32 _inst_, uint32 ren_cfg_num );
/* =============================================================================
**  Function Name: DDRPHY_DQ_read_ren_byte1_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] ren_cfg_num           TBD
*
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
uint32 DDRPHY_DQ_read_ren_byte1_cfg( uint32 _inst_, uint32 ren_cfg_num );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_reset
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte0_reset           TBD
*   @param[in] byte1_reset           TBD
*
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
void DDRPHY_DQ_ren_reset( uint32 _inst_, uint32 byte0_reset, uint32 byte1_reset );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_cfg_byte0_sel
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] cfg_sel           TBD
*   @param[in] value           TBD
*
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
void DDRPHY_DQ_ren_cfg_byte0_sel( uint32 _inst_, uint32 cfg_sel, uint32 value );
/* =============================================================================
**  Function Name: DDRPHY_DQ_ren_cfg_byte1_sel
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] cfg_sel                TBD
*   @param[in] value                  TBD
*
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
void DDRPHY_DQ_ren_cfg_byte1_sel( uint32 _inst_, uint32 cfg_sel, uint32 value );

//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

/* =============================================================================
**  Function Name: DDRPHY_DQ_enable_wrlvl_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_enable_wrlvl_mode( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_disable_wrlvl_mode
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_disable_wrlvl_mode( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_wrlvl_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] delay_index           TBD
*   @param[in] wrlvl_delay_val       TBD
*
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
void DDRPHY_DQ_set_wrlvl_delay( uint32 _inst_, uint32 delay_index, uint32 wrlvl_delay_val );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_wrlvl_cdc_retimer
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] cfg_index                               TBD
*   @param[in] byte0_retimer_sel_LUT_value             TBD
*   @param[in] byte0_retimer_ie_oe_LUT_value           TBD
*   @param[in] byte0_retimer_cycle_delay_sel           TBD
*   @param[in] byte1_retimer_sel_LUT_value             TBD
*   @param[in] byte1_retimer_ie_oe_LUT_value           TBD
*   @param[in] byte1_retimer_cycle_delay_sel           TBD

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
void DDRPHY_DQ_set_wrlvl_cdc_retimer( uint32 _inst_, uint32 cfg_index, uint32 byte0_retimer_sel_LUT_value, uint32 byte0_retimer_ie_oe_LUT_value, uint32 byte0_retimer_cycle_delay_sel, uint32 byte1_retimer_sel_LUT_value, uint32 byte1_retimer_ie_oe_LUT_value, uint32 byte1_retimer_cycle_delay_sel );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] cfg_index           TBD
*   @param[in] byte_index           TBD
*   @param[in] retimer_sel_LUT_value           TBD
*   @param[in] retimer_ie_oe_LUT_value           TBD
*   @param[in] retimer_ie_oe_LUT_value           TBD
*
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
void DDRPHY_DQ_set_wrlvl_cdc_retimer_value( uint32 _inst_, uint32 cfg_index, uint32 byte_index, uint32 retimer_sel_LUT_value, uint32 retimer_ie_oe_LUT_value, uint32 retimer_cycle_delay_sel );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_cdc_wrlvl_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte_index            TBD
*   @param[in] delay_index           TBD
*   @param[in] delay_value           TBD
*
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
void DDRPHY_DQ_set_cdc_wrlvl_delay( uint32 _inst_, uint32 byte_index, uint32 delay_index, uint32 delay_value );
/* =============================================================================
**  Function Name: DDRPHY_DQ_cdc_wr90_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_cdc_wr90_delay( uint32 _inst_, uint32 delay_index, uint32 delay_val );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_trigger_pulse
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_wrlvl_trigger_pulse( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_trigger_zero
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_wrlvl_trigger_zero( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_trigger_one
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*
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
void DDRPHY_DQ_wrlvl_trigger_one( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_read_wrlvl_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
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
uint32 DDRPHY_DQ_read_wrlvl_status( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_DQ_read_wrlvl_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte_index            TBD
*   @param[in] cfg_index             TBD
*
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
uint32 DDRPHY_DQ_read_wrlvl_delay( uint32 _inst_, uint32 byte_index, uint32 cfg_index );
/* =============================================================================
**  Function Name: DDRPHY_DQ_wrlvl_freq_cfg
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] freq_index           TBD
*   @param[in] freq_val           TBD
*
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
void DDRPHY_DQ_wrlvl_freq_cfg( uint32 _inst_, uint32 freq_index, uint32 freq_val );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_cdc_wrlvl_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] byte_index           TBD
*   @param[in] delay_index           TBD
*   @param[in] retimer_value           TBD
*   @param[in] delay_value           TBD
*
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
void DDRPHY_DQ_set_cdc_wrlvl_training( uint32 _inst_, uint32 byte_index, uint32 delay_index, uint16 retimer_value, uint16 delay_value );

//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

/* =============================================================================
**  Function Name: DDRPHY_DQ_set_dq_wr_cdc_deskew
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_set_dq_wr_cdc_deskew( uint32 _inst_, int wr90_delay, int rd180_delay, uint16 deskew_dq, uint16 deskew_dm );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_set_dq_rd_cdc_t4( uint32 _inst_, uint16 byte0_cdc_delay, uint16 byte1_cdc_delay );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_update_dqs_delay( uint32 _inst_, int fine_training, int byte_delay );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_update_dqs_delay_fine_sweep( uint32 _inst_, uint16 byte0_delay, uint16 byte1_delay );
/* =============================================================================
**  Function Name: 
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_update_dqs_rd90_coarse_delays( uint32 _inst_, uint16 min_a0, uint16 max_b0, uint16 min_a1, uint16 max_b1, uint16 training_period_100ps );
/* =============================================================================
**  Function Name: DDRPHY_DQ_update_dqs_rd90_full_cdc
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*   @param[in] param           TBD
*
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
void DDRPHY_DQ_update_dqs_rd90_full_cdc( uint32 _inst_, uint16 full_cdc_delay0_byte0, uint16 full_cdc_delay1_byte0, uint16 full_cdc_delay2_byte0, uint16 full_cdc_delay3_byte0, uint16 full_cdc_delay0_byte1, uint16 full_cdc_delay1_byte1, uint16 full_cdc_delay2_byte1, uint16 full_cdc_delay3_byte1 );

/* =============================================================================
**  Function Name: DDRPHY_DQ_set_dqs_rd90_full_cdc_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_read           TBD
*
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
void DDRPHY_DQ_set_dqs_rd90_full_cdc_value( uint32 _inst_, uint16 dq_read[2][4]  );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_rd_deskew_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_read_perbit           TBD
*   @param[in] dq_read_perbit_dm           TBD
*
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
void DDRPHY_DQ_set_rd_deskew_value( uint32 _inst_, uint32 dq_read_perbit[2][2], uint32 dq_read_perbit_dm );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] coarse           TBD
*   @param[in] fine           TBD
*
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
void DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine(uint32 _inst_, uint16 coarse, uint16 fine );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_dq_cdc_rd90_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_write           TBD
*
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
void DDRPHY_DQ_set_dq_cdc_rd90_value( uint32 _inst_, uint16 dq_write[2][4]  );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

/* =============================================================================
**  Function Name: DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] coarse           TBD
*   @param[in] fine            TBD
*
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
void DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine(uint32 _inst_, uint16 coarse, uint16 fine );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_dq_cdc_wr90_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_write           TBD
*
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
void DDRPHY_DQ_set_dq_cdc_wr90_value( uint32 _inst_, uint16 dq_write[2][4]  );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_wr_deskew_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_write_perbit           TBD
*   @param[in] dq_write_perbit_dm           TBD
*
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
void DDRPHY_DQ_set_wr_deskew_value( uint32 _inst_, uint32 dq_write_perbit[2], uint32 dq_write_perbit_dm );
/* =============================================================================
**  Function Name: DDRPHY_DQ_set_dqs_wr90_full_cdc_value
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dq_write           TBD
*
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
void DDRPHY_DQ_set_dqs_wr90_full_cdc_value( uint32 _inst_, uint16 dq_write[2][4]  );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

/* =============================================================================
**  Function Name: DDRPHY_DQ_set_exit_dq_for_ca_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] mode           TBD
*
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
void DDRPHY_DQ_set_exit_dq_for_ca_training( uint32 _inst_, uint16 mode );
/* =============================================================================
**  Function Name: DDRPHY_DQ_read_dq_training_status_for_ca_training
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
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
uint16 DDRPHY_DQ_read_dq_training_status_for_ca_training( uint32 _inst_ );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************


#endif
