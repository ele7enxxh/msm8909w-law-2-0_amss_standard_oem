
#ifndef _DDR_PHY_CA_INIT_CA_SEQ_HEADER_
#define _DDR_PHY_CA_INIT_CA_SEQ_HEADER_
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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/hw_sequence/PHY/v2.5/ddr_phy_ca.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who           what, where, why
--------   --------      -------------------------------------------------------
10/28/14   sk            Changes for 8952 Chipset
07/07/14   amoussav      Initial version.
==============================================================================*/

/*==============================================================================
                               INCLUDE FILES
==============================================================================*/
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "ddr_phy_config.h"
/*==============================================================================
                                  TYPES
==============================================================================*/
/*NA*/



/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* =============================================================================
**  Function Name: DDRPHY_CA_init_io_pads
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
void DDRPHY_CA_init_io_pads( uint32 _inst_, uint32 device_type );
/* =============================================================================
**  Function Name: DDRPHY_CA_set_dev_type
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
void DDRPHY_CA_set_dev_type( uint32 _inst_, uint32 device_type );
/* =============================================================================
**  Function Name: DDRPHY_CA_site_clk_mode
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
void DDRPHY_CA_site_clk_mode( uint32 _inst_, int clk_mode );
/* =============================================================================
**  Function Name: DDRPHY_CA_init_cdc
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
void DDRPHY_CA_init_cdc( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_init_ca_csr
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
void DDRPHY_CA_init_ca_csr( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_manual_io_cal
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
void DDRPHY_CA_manual_io_cal( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_auto_io_cal
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
void DDRPHY_CA_auto_io_cal( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_disable_auto_io_cal
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
void DDRPHY_CA_disable_auto_io_cal( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_wrlvl_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] bypass                TBD
*   @param[in] dcc_manual            TBD
*   @param[in] dcc_sw_ovrd           TBD
*   @param[in] pll_sel               TBD
*   @param[in] dcc_wait_time         TBD
*   @param[in] div_ref               TBD
*   @param[in] dcc_man_adj           TBD
*   @param[in] dcc_adj_sw_ovrd       TBD
*   @param[in] dcc_man_adj_pll2      TBD
*   @param[in] dcc_adj_sw_ovrd_pll2  TBD
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
void DDRPHY_CA_wrlvl_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wrlvl_auto_dcc_calibration
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
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
void DDRPHY_CA_wrlvl_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wrlvl_dcc_override
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] pll_sel         TBD
*   @param[in] dcc_adj_sw_ovrd               TBD
*   @param[in] dcc_adj_sw_ovrd_pll2               TBD
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
void DDRPHY_CA_wrlvl_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wrlvl_dcc_stop
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
void DDRPHY_CA_wrlvl_dcc_stop( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_wrlvl_dcc_adj_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] site_num              TBD
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
uint32 DDRPHY_CA_wrlvl_dcc_adj_status( uint32 _inst_, uint32 site_num );
/* =============================================================================
**  Function Name: DDRPHY_CA_wr90_dcc_cal
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] bypass              TBD
*   @param[in] dcc_manual              TBD
*   @param[in] dcc_sw_ovrd              TBD
*   @param[in] pll_sel              TBD
*   @param[in] dcc_wait_time              TBD
*   @param[in] div_ref              TBD
*   @param[in] dcc_man_adj              TBD
*   @param[in] dcc_adj_sw_ovrd              TBD
*   @param[in] dcc_man_adj_pll2              TBD
*   @param[in] dcc_adj_sw_ovrd_pll2              TBD
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
void DDRPHY_CA_wr90_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wr90_auto_dcc_calibration
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                Base address
*   @param[in] dcc_wait_time         TBD
*   @param[in] div_ref               TBD
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
void DDRPHY_CA_wr90_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wr90_dcc_override
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
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
void DDRPHY_CA_wr90_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_wr90_dcc_stop
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
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
void DDRPHY_CA_wr90_dcc_stop( uint32 _inst_ );
/* =============================================================================
**  Function Name: DDRPHY_CA_wr90_dcc_adj_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] site_num                TBD
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
uint32 DDRPHY_CA_wr90_dcc_adj_status( uint32 _inst_, uint32 site_num );
/* =============================================================================
**  Function Name: DDRPHY_CA_io_pads_odt_enable
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] odt_ena                TBD
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
void DDRPHY_CA_io_pads_odt_enable( uint32 _inst_, uint32 odt_ena );
/* =============================================================================
**  Function Name: DDRPHY_CA_pncnt_status
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] pncnt_val                TBD
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
uint32 DDRPHY_CA_pncnt_status( uint32 _inst_, uint32 pncnt_val );
/* =============================================================================
**  Function Name: DDRPHY_CA_new_pncnt_update
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] pncnt_val                TBD
*
*   @return
*   TBD
*
*   @dependencies
*   void
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void DDRPHY_CA_new_pncnt_update( uint32 _inst_, uint32 pncnt_val );
/* =============================================================================
**  Function Name: DDRPHY_CA_enable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] clk_mode                TBD
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
void DDRPHY_CA_enable_csd_rx( uint32 _inst_, int clk_mode );
/* =============================================================================
**  Function Name: DDRPHY_CA_disable_csd_rx
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] selfrefresh_mode                TBD
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
void DDRPHY_CA_disable_csd_rx( uint32 _inst_, int selfrefresh_mode);
/* =============================================================================
**  Function Name: DDRPHY_CA_ch0_misc_init_io_pads
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] device_type                TBD
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
void DDRPHY_CA_ch0_misc_init_io_pads( uint32 _inst_, uint32 device_type );
/* =============================================================================
**  Function Name: DDRPHY_CA_ch1_misc_init_io_pads
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] device_type                TBD
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
void DDRPHY_CA_ch1_misc_init_io_pads( uint32 _inst_, uint32 device_type );
/* =============================================================================
**  Function Name: DDRPHY_CA_dcc_set
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] dcc_man_adj                TBD
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
void DDRPHY_CA_dcc_set( uint32 _inst_, uint32 dcc_man_adj[2][2][2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_neg_wrlvl
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*
*   @return
*   TBD
*
*   @dependencies
*   void
*
*   @sideeffects
*   TBD
*
*   @sa
*   TBD
*/
void DDRPHY_CA_neg_wrlvl(uint32 _inst_);
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_seq_set_initial_cdc_wr90_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] site                   TBD
*   @param[in] coarse_delay_value     TBD
*   @param[in] fine_delay_value       TBD
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
void DDRPHY_CA_ca_training_seq_set_initial_cdc_wr90_delay( uint32 _inst_, uint16 site, uint16 coarse_delay_value, uint16 fine_delay_value );
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_seq_set_initial_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] site                   TBD
*   @param[in] deskew_delay_value     TBD
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
void DDRPHY_CA_ca_training_seq_set_initial_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site, uint16 deskew_delay_value );
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_seq_set_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] bit                    TBD
*   @param[in] deskew_delay_value     TBD
*   @param[in] site                   TBD
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
void DDRPHY_CA_ca_training_seq_set_cdc_perbit_deskew_delay( uint32 _inst_, uint16 bit, uint16 deskew_delay_value, uint16 site );
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_cdc_wr90_delay_select
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] sel                   TBD
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
void DDRPHY_CA_ca_training_cdc_wr90_delay_select( uint32 _inst_ , uint16 sel);
/* =============================================================================
**  Function Name: DDRPHY_CA_set_ca_cdc_wr90
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] ca_cdc_wr90_delay                    TBD
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
void DDRPHY_CA_set_ca_cdc_wr90( uint32 _inst_, uint16 site, uint16 ca_cdc_wr90_delay[4] );
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_seq_restore_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] site                    TBD
*   @param[in] deskew_delay_value     TBD
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
void DDRPHY_CA_ca_training_seq_restore_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site,  uint32 deskew_delay_value[2] );
/* =============================================================================
**  Function Name: DDRPHY_CA_ca_training_seq_read_cdc_perbit_deskew_delay
** =============================================================================
*/
/**
*   @brief
*   TBD
*
*   @param[in] _inst_                 Base address
*   @param[in] site                    TBD
*   @param[in] deskew_delay_value     TBD
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
void DDRPHY_CA_ca_training_seq_read_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site,  uint32 deskew_delay_value[2] );
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************
//***************************************************************************************************************************************************************

#endif
