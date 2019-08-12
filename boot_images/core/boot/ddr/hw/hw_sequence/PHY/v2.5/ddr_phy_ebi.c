
#include "ddr_phy_ebi.h"

uint32       dcc_wait_time_swc[4][2] = { {  0x2,  0x2 }, {  0x2,  0x2 }, {   0x2, 0x2 }, {  0x2,  0x2 }};
uint32         dcc_div_ref_swc[4][2] = { { 0x1A, 0x1A }, { 0x1A, 0x1A }, { 0x1A, 0x1A }, { 0x1A, 0x1A }};
uint32         dcc_man_adj_swc[4][2] = { { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }};
uint32      dcc_adj_sw_val_swc[4][2] = { { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }};
uint32    dcc_man_adj_swc_pll2[4][2] = { { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }};
uint32 dcc_adj_sw_val_swc_pll2[4][2] = { { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }, { 0x10, 0x10 }};

#pragma arm section rwdata = "BOOT_RPM_RAM_DATA_ZONE"
uint32 extern ca_pad_cfg0_lp_rec ;
uint32 extern ca_pad_cfg1_lp_rec ;
uint32 extern ca_pad_cfg2_lp_rec ;
uint32 extern ca_pad_cfg3_lp_rec ;
uint32 extern ca_pad_cfg4_lp_rec ;
uint32 extern ca_pad_cfg5_lp_rec ;

uint32 extern dq_pad_cfg0_lp_rec ;
uint32 extern dq_pad_cfg1_lp_rec ;
uint32 extern dq_pad_cfg2_lp_rec ;
uint32 extern dq_pad_cfg3_lp_rec ;

uint32 extern ca_pad_cfg0_pc_rec ;
uint32 extern ca_pad_cfg1_pc_rec ;
uint32 extern ca_pad_cfg2_pc_rec ;
uint32 extern ca_pad_cfg3_pc_rec ;
uint32 extern ca_pad_cfg4_pc_rec ;
uint32 extern ca_pad_cfg5_pc_rec ;

uint32 extern dq_pad_cfg0_pc_rec ;
uint32 extern dq_pad_cfg1_pc_rec ;
uint32 extern dq_pad_cfg2_pc_rec ;
uint32 extern dq_pad_cfg3_pc_rec ;

#if 1
static uint32 ddrphy_ca_mision_mode_cfg[][2] = 
{
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_ADDR(0)                  , DDRPHY_CA_IOC_CTLR_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_PNCNT_CFG_ADDR(0)            , DDRPHY_CA_IOC_CTLR_PNCNT_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_TIMER_CFG_ADDR(0)            , DDRPHY_CA_IOC_CTLR_TIMER_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CHAR_CFG_ADDR(0)             , DDRPHY_CA_IOC_CTLR_CHAR_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CA_IOC_SLV_CFG_ADDR(0)                , DDRPHY_CA_CA_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CK_IOC_SLV_CFG_ADDR(0)                , DDRPHY_CA_CK_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CK_ADJUST_DELAY_ADDR(0)               , DDRPHY_CA_CK_ADJUST_DELAY_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3_ADDR(0)               , DDRPHY_CA_CDC_WR90_DELAY3_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0_ADDR(0)              , DDRPHY_CA_SITE0_WR_DESKEW0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1_ADDR(0)              , DDRPHY_CA_SITE0_WR_DESKEW1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_LP_DELAY_ADDR(0)                  , DDRPHY_CA_CDC_LP_DELAY_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_DELAY_SEL_ADDR(0)                 , DDRPHY_CA_CDC_DELAY_SEL_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_TEST_CFG_ADDR(0)                  , DDRPHY_CA_CDC_TEST_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG2_ADDR(0)                      , DDRPHY_CA_FPM_CFG2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_ADDR(0)            , DDRPHY_CA_SITE0_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_CA_SITE0_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG_ADDR(0)          , DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_TOP_CFG_ADDR(0)                       , DDRPHY_CA_TOP_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_TOP_CFG1_ADDR(0)                      , DDRPHY_CA_TOP_CFG1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_HW_INFO_ADDR(0)                       , DDRPHY_CA_HW_INFO_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_HW_VERSION_ADDR(0)                    , DDRPHY_CA_HW_VERSION_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_ADDR(0)                      , DDRPHY_CA_PAD_CFG0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_ADDR(0)                      , DDRPHY_CA_PAD_CFG1_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG2_ADDR(0)                      , DDRPHY_CA_PAD_CFG2_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG3_ADDR(0)                      , DDRPHY_CA_PAD_CFG3_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG4_ADDR(0)                      , DDRPHY_CA_PAD_CFG4_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG5_ADDR(0)                      , DDRPHY_CA_PAD_CFG5_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_CDC_CFG_ADDR(0)                       , DDRPHY_CA_CDC_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_CSD_RX_CFG_ADDR(0)              , DDRPHY_CA_SITE0_CSD_RX_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_ADDR(0)                , DDRPHY_CA_SITE_CK_DP_CFG_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG0_ADDR(0)                      , DDRPHY_CA_FPM_CFG0_RECVAL ,
            HWIO_DDRPHY_CA_DDRPHY_CA_FPM_CFG1_ADDR(0)                      , DDRPHY_CA_FPM_CFG1_RECVAL ,
			// SHOWS END OF SETTING
            END_OF_SETTING                                                 , END_OF_SETTING  
};
static uint32 ddrphy_dq_mision_mode_cfg[][2] = 
{
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DQ_IOC_SLV_CFG_ADDR(0)                , DDRPHY_DQ_DQ_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DQS_IOC_SLV_CFG_ADDR(0)               , DDRPHY_DQ_DQS_IOC_SLV_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG4_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG5_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6_ADDR(0)                    , DDRPHY_DQ_WRLVL_CFG6_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3_ADDR(0)               , DDRPHY_DQ_CDC_WR90_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY0_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY1_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY2_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY3_ADDR(0)               , DDRPHY_DQ_CDC_RD90_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_WR_DESKEW_DQ_ADDR(0)            , DDRPHY_DQ_BYTE0_WR_DESKEW_DQ_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_WR_DESKEW_DQ_ADDR(0)            , DDRPHY_DQ_BYTE1_WR_DESKEW_DQ_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_WR_DESKEW_DM_ADDR(0)                  , DDRPHY_DQ_WR_DESKEW_DM_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN_ADDR(0)       , DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN_ADDR(0)       , DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD_ADDR(0)        , DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD_ADDR(0)        , DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_RD_DESKEW_DM_ADDR(0)                  , DDRPHY_DQ_RD_DESKEW_DM_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY0_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY1_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY2_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY3_ADDR(0)              , DDRPHY_DQ_CDC_RD180_DELAY3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_LP_DELAY_ADDR(0)                  , DDRPHY_DQ_CDC_LP_DELAY_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_DELAY_SEL_ADDR(0)                 , DDRPHY_DQ_CDC_DELAY_SEL_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_TEST_CFG_ADDR(0)                  , DDRPHY_DQ_CDC_TEST_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG0_ADDR(0)                      , DDRPHY_DQ_REN_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG1_ADDR(0)                      , DDRPHY_DQ_REN_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG2_ADDR(0)                      , DDRPHY_DQ_REN_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG3_ADDR(0)                      , DDRPHY_DQ_REN_CFG3_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG4_ADDR(0)                      , DDRPHY_DQ_REN_CFG4_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG5_ADDR(0)                      , DDRPHY_DQ_REN_CFG5_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_REN_CFG6_ADDR(0)                      , DDRPHY_DQ_REN_CFG6_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG2_ADDR(0)                      , DDRPHY_DQ_FPM_CFG2_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_ADDR(0)            , DDRPHY_DQ_BYTE0_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_ADDR(0)            , DDRPHY_DQ_BYTE1_DCC_WR90_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_ADDR(0)           , DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG_ADDR(0)          , DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG0_ADDR(0)                      , DDRPHY_DQ_FPM_CFG0_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG1_ADDR(0)                      , DDRPHY_DQ_FPM_CFG1_RECVAL ,
            HWIO_DDRPHY_DQ_DDRPHY_DQ_FPM_CFG3_ADDR(0)                      , DDRPHY_DQ_FPM_CFG3_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_ADDR(0)                       , DDRPHY_DQ_TOP_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_HW_INFO_ADDR(0)                       , DDRPHY_DQ_HW_INFO_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_HW_VERSION_ADDR(0)                    , DDRPHY_DQ_HW_VERSION_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_ADDR(0)                      , DDRPHY_DQ_PAD_CFG0_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_ADDR(0)                      , DDRPHY_DQ_PAD_CFG1_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG2_ADDR(0)                      , DDRPHY_DQ_PAD_CFG2_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG3_ADDR(0)                      , DDRPHY_DQ_PAD_CFG3_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_CDC_CFG_ADDR(0)                       , DDRPHY_DQ_CDC_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE0_CSD_RX_CFG_ADDR(0)              , DDRPHY_DQ_SITE0_CSD_RX_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE1_CSD_RX_CFG_ADDR(0)              , DDRPHY_DQ_SITE1_CSD_RX_CFG_RECVAL ,
			HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_ADDR(0)                , DDRPHY_DQ_SITE_CK_DP_CFG_RECVAL ,
			// SHOWS END OF SETTING
            END_OF_SETTING                                                 , END_OF_SETTING  
};
#endif

#pragma arm section rwdata 

uint32 ca_jcpll_dcc_training_g[2][2][2][2];
uint32 dq_jcpll_dcc_training_g[2][4][2][2][2];

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  PHY INIT provides the steps to initalize DDR PHY
///  it calls CA and DQ sequences according to the HPG
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_phy_init( uint32 _inst_, uint32 device_type, uint32 chnl, phy_clk_type clk_mode, uint32 odt_en )
{


  if (chnl == 0) {
       EBI1_PHY_Set_Config_ca( _inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
                            
       HWIO_OUTX(  _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_SWMAN_OFFSET, AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_CH0_DQs);  
       //EBI1_PHY_Set_Config( _inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
       EBI1_PHY_Set_Config_dq( _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_BROADCAST_SWMAN_OFFSET );
       HWIO_OUTX(  _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_SWMAN_OFFSET, AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, PHY_BC_DISABLE);
}

#ifdef CHNL_CNT_2
  if (chnl == 1) {
       EBI1_PHY_Set_Config_ca( _inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
       HWIO_OUTX(  _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_SWMAN_OFFSET, AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_CH1_DQs) ; 
       //EBI1_PHY_Set_Config( _inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
       EBI1_PHY_Set_Config_dq( _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_BROADCAST_SWMAN_OFFSET);
       HWIO_OUTX(  _inst_ + SEQ_EBI1_PHY_CFG_EBI1_AHB2PHY_SWMAN_OFFSET, AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, PHY_BC_DISABLE); 
}
#endif
   
  EBI1_PHY_CFG_ddr_phy_clk_mode(_inst_,  chnl, clk_mode);
  EBI1_PHY_CFG_pads_odt_enable(_inst_, chnl, odt_en);

}
#pragma arm section code

// ***********************************************************************
/// -------------------------------------------------------------
///  auto_io_cal function 
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_auto_io_cal( uint32 _inst_, uint32 chnl )
{

  ///  Channel 0 auto_io_cal
  if (chnl == 0) {
    DDRPHY_CA_auto_io_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_auto_io_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
  }
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_disable_auto_io_cal( uint32 _inst_, uint32 chnl )
{

  ///  Channel 0 auto_io_cal
  if (chnl == 0) {
    DDRPHY_CA_disable_auto_io_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_disable_auto_io_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
  }
#endif  
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC Apply calibrates both DCC WR90 and WRLVL within CA DDR PHY
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_dcc_cal_ca( uint32 _inst_, uint32 chnl, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 ch0_wr90_dcc_man_adj[2], uint32 ch0_wr90_dcc_adj_sw_ovrd[2], uint32 ch0_wr90_dcc_man_adj_pll2[2], uint32 ch0_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch0_wrlvl_dcc_man_adj[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch0_wrlvl_dcc_man_adj_pll2[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wr90_dcc_man_adj[2], uint32 ch1_wr90_dcc_adj_sw_ovrd[2], uint32 ch1_wr90_dcc_man_adj_pll2[2], uint32 ch1_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wrlvl_dcc_man_adj[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch1_wrlvl_dcc_man_adj_pll2[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd_pll2[2] )
{

  ///  Channel 0 DCC
  if (chnl == 0) {
    /// DCC Cal for CA
    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                       bypass,
		                       dcc_manual,
		                       dcc_sw_ovrd,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       ch0_wr90_dcc_man_adj,
		                       ch0_wr90_dcc_adj_sw_ovrd,
		                       ch0_wr90_dcc_man_adj_pll2,
		                       ch0_wr90_dcc_adj_sw_ovrd_pll2);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                        bypass,
		                        dcc_manual,
		                        dcc_sw_ovrd,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        ch0_wrlvl_dcc_man_adj,
		                        ch0_wrlvl_dcc_adj_sw_ovrd,
		                        ch0_wrlvl_dcc_man_adj_pll2,
		                        ch0_wrlvl_dcc_adj_sw_ovrd_pll2);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    /// DCC Cal for CA
    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                       bypass,
		                       dcc_manual,
		                       dcc_sw_ovrd,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       ch1_wr90_dcc_man_adj,
		                       ch1_wr90_dcc_adj_sw_ovrd,
		                       ch1_wr90_dcc_man_adj_pll2,
		                       ch1_wr90_dcc_adj_sw_ovrd_pll2);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                        bypass,
		                        dcc_manual,
		                        dcc_sw_ovrd,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        ch1_wrlvl_dcc_man_adj,
		                        ch1_wrlvl_dcc_adj_sw_ovrd,
		                        ch1_wrlvl_dcc_man_adj_pll2,
		                        ch1_wrlvl_dcc_adj_sw_ovrd_pll2);
  }
#endif
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC Apply calibrates both DCC WR90 and WRLVL within DQ DDR PHY
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_dcc_cal_dq( uint32 _inst_, uint32 chnl, uint32 dq_num, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 ch0_wr90_dcc_man_adj[2], uint32 ch0_wr90_dcc_adj_sw_ovrd[2], uint32 ch0_wr90_dcc_man_adj_pll2[2], uint32 ch0_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch0_wrlvl_dcc_man_adj[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch0_wrlvl_dcc_man_adj_pll2[2], uint32 ch0_wrlvl_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wr90_dcc_man_adj[2], uint32 ch1_wr90_dcc_adj_sw_ovrd[2], uint32 ch1_wr90_dcc_man_adj_pll2[2], uint32 ch1_wr90_dcc_adj_sw_ovrd_pll2[2], uint32 ch1_wrlvl_dcc_man_adj[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd[2], uint32 ch1_wrlvl_dcc_man_adj_pll2[2], uint32 ch1_wrlvl_dcc_adj_sw_ovrd_pll2[2] )
{

  ///  Channel 0 DCC Cal
  if (chnl == 0) {
    /// DCC Cal for DQ 
    switch(dq_num){
      case 0 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch0_wrlvl_dcc_man_adj,
				                        ch0_wrlvl_dcc_adj_sw_ovrd,
				                        ch0_wrlvl_dcc_man_adj_pll2,
				                        ch0_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch0_wr90_dcc_man_adj,
				                       ch0_wr90_dcc_adj_sw_ovrd,
				                       ch0_wr90_dcc_man_adj_pll2,
				                       ch0_wr90_dcc_adj_sw_ovrd_pll2);
        break;
      case 1 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch0_wrlvl_dcc_man_adj,
				                        ch0_wrlvl_dcc_adj_sw_ovrd,
				                        ch0_wrlvl_dcc_man_adj_pll2,
				                        ch0_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch0_wr90_dcc_man_adj,
				                       ch0_wr90_dcc_adj_sw_ovrd,
				                       ch0_wr90_dcc_man_adj_pll2,
				                       ch0_wr90_dcc_adj_sw_ovrd_pll2);
        break;

#ifdef NUM_BYTES_8
      case 2 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch0_wrlvl_dcc_man_adj,
				                        ch0_wrlvl_dcc_adj_sw_ovrd,
				                        ch0_wrlvl_dcc_man_adj_pll2,
				                        ch0_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch0_wr90_dcc_man_adj,
				                       ch0_wr90_dcc_adj_sw_ovrd,
				                       ch0_wr90_dcc_man_adj_pll2,
				                       ch0_wr90_dcc_adj_sw_ovrd_pll2);
        break;
      case 3 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch0_wrlvl_dcc_man_adj,
				                        ch0_wrlvl_dcc_adj_sw_ovrd,
				                        ch0_wrlvl_dcc_man_adj_pll2,
				                        ch0_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch0_wr90_dcc_man_adj,
				                       ch0_wr90_dcc_adj_sw_ovrd,
				                       ch0_wr90_dcc_man_adj_pll2,
				                       ch0_wr90_dcc_adj_sw_ovrd_pll2);
        break;
#endif
    }
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    /// DCC Cal for DQ 
    switch(dq_num){
      case 0 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch1_wrlvl_dcc_man_adj,
				                        ch1_wrlvl_dcc_adj_sw_ovrd,
				                        ch1_wrlvl_dcc_man_adj_pll2,
				                        ch1_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch1_wr90_dcc_man_adj,
				                       ch1_wr90_dcc_adj_sw_ovrd,
				                       ch1_wr90_dcc_man_adj_pll2,
				                       ch1_wr90_dcc_adj_sw_ovrd_pll2);
        break;
      case 1 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch1_wrlvl_dcc_man_adj,
				                        ch1_wrlvl_dcc_adj_sw_ovrd,
				                        ch1_wrlvl_dcc_man_adj_pll2,
				                        ch1_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch1_wr90_dcc_man_adj,
				                       ch1_wr90_dcc_adj_sw_ovrd,
				                       ch1_wr90_dcc_man_adj_pll2,
				                       ch1_wr90_dcc_adj_sw_ovrd_pll2);
        break;
#ifdef NUM_BYTES_8
      case 2 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch1_wrlvl_dcc_man_adj,
				                        ch1_wrlvl_dcc_adj_sw_ovrd,
				                        ch1_wrlvl_dcc_man_adj_pll2,
				                        ch1_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch1_wr90_dcc_man_adj,
				                       ch1_wr90_dcc_adj_sw_ovrd,
				                       ch1_wr90_dcc_man_adj_pll2,
				                       ch1_wr90_dcc_adj_sw_ovrd_pll2);
        break;
      case 3 :
        DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                        bypass,
				                        dcc_manual,
				                        dcc_sw_ovrd,
				                        pll_sel,
				                        dcc_wait_time,
				                        div_ref,
				                        ch1_wrlvl_dcc_man_adj,
				                        ch1_wrlvl_dcc_adj_sw_ovrd,
				                        ch1_wrlvl_dcc_man_adj_pll2,
				                        ch1_wrlvl_dcc_adj_sw_ovrd_pll2);
        DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                       bypass,
				                       dcc_manual,
				                       dcc_sw_ovrd,
				                       pll_sel,
				                       dcc_wait_time,
				                       div_ref,
				                       ch1_wr90_dcc_man_adj,
				                       ch1_wr90_dcc_adj_sw_ovrd,
				                       ch1_wr90_dcc_man_adj_pll2,
				                       ch1_wr90_dcc_adj_sw_ovrd_pll2);
        break;
#endif
    }
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_apply_dcc_cal_adj( uint32 _inst_, uint32 chnl, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2] )
{
  uint32 ca_wr90_dcc_man_adj[2];
  uint32 ca_wrlvl_dcc_man_adj[2];
  uint32 dq0_wr90_dcc_man_adj[2];
  uint32 dq0_wrlvl_dcc_man_adj[2];
  uint32 dq1_wr90_dcc_man_adj[2];
  uint32 dq1_wrlvl_dcc_man_adj[2];

#ifdef NUM_BYTES_8
  uint32 dq2_wr90_dcc_man_adj[2];
  uint32 dq2_wrlvl_dcc_man_adj[2];
  uint32 dq3_wr90_dcc_man_adj[2];
  uint32 dq3_wrlvl_dcc_man_adj[2];
#endif
  uint32 dummy_man_adj[2] = { 0x0, 0x0 };

  
  

  ///  Channel 0 DCC Cal
  if (chnl == 0) {
    ca_wr90_dcc_man_adj[0] = DDRPHY_CA_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 0);
    ca_wr90_dcc_man_adj[1] = DDRPHY_CA_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 1);
    ca_wrlvl_dcc_man_adj[0] = DDRPHY_CA_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 0);
    ca_wrlvl_dcc_man_adj[1] = DDRPHY_CA_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 1);
    dq0_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET , 0);
    dq0_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET , 1);
    dq0_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET , 0);
    dq0_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET , 1);
    dq1_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET , 0);
    dq1_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET , 1);
    dq1_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET , 0);
    dq1_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET , 1);
#ifdef NUM_BYTES_8
    dq2_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET , 0);
    dq2_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET , 1);
    dq2_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET , 0);
    dq2_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET , 1);
    dq3_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET , 0);
    dq3_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET , 1);
    dq3_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET , 0);
    dq3_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET , 1);
#endif

    //Store Values for Re-training
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][0][0]    = ca_wr90_dcc_man_adj[0]   ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][1][0]    = ca_wr90_dcc_man_adj[1]   ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][0][1]    = ca_wrlvl_dcc_man_adj[0]  ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][1][1]    = ca_wrlvl_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][0][0] = dq0_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][1][0] = dq0_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][0][1] = dq0_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][1][1] = dq0_wrlvl_dcc_man_adj[1] ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][0][0] = dq1_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][1][0] = dq1_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][0][1] = dq1_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][1][1] = dq1_wrlvl_dcc_man_adj[1] ; 

    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       ca_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       ca_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        ca_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        ca_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq0_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq0_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq0_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq0_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq1_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq1_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq1_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq1_wr90_dcc_man_adj,
		                       dummy_man_adj);

#ifdef NUM_BYTES_8

    //Store Values for Re-training
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][0][0] = dq2_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][1][0] = dq2_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][0][1] = dq2_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][1][1] = dq2_wrlvl_dcc_man_adj[1] ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][0][0] = dq3_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][1][0] = dq3_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][0][1] = dq3_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][1][1] = dq3_wrlvl_dcc_man_adj[1] ; 

    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq2_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq2_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq2_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq2_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq3_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq3_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq3_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq3_wr90_dcc_man_adj,
		                       dummy_man_adj);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    ca_wr90_dcc_man_adj[0] = DDRPHY_CA_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 0);
    ca_wr90_dcc_man_adj[1] = DDRPHY_CA_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 1);
    ca_wrlvl_dcc_man_adj[0] = DDRPHY_CA_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 0);
    ca_wrlvl_dcc_man_adj[1] = DDRPHY_CA_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 1);
    dq0_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET , 0);
    dq0_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET , 1);
    dq0_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET , 0);
    dq0_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET , 1);
    dq1_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET , 0);
    dq1_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET , 1);
    dq1_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET , 0);
    dq1_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET , 1);

#ifdef NUM_BYTES_8
    dq2_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET , 0);
    dq2_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET , 1);
    dq2_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET , 0);
    dq2_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET , 1);
    dq3_wr90_dcc_man_adj[0] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET , 0);
    dq3_wr90_dcc_man_adj[1] = DDRPHY_DQ_wr90_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET , 1);
    dq3_wrlvl_dcc_man_adj[0] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET , 0);
    dq3_wrlvl_dcc_man_adj[1] = DDRPHY_DQ_wrlvl_dcc_adj_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET , 1);
#endif
  
    //Store Values for Re-training
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][0][0]    = ca_wr90_dcc_man_adj[0]   ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][1][0]    = ca_wr90_dcc_man_adj[1]   ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][0][1]    = ca_wrlvl_dcc_man_adj[0]  ; 
    ca_jcpll_dcc_training_g[chnl][pll_sel-1][1][1]    = ca_wrlvl_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][0][0] = dq0_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][1][0] = dq0_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][0][1] = dq0_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][0][pll_sel-1][1][1] = dq0_wrlvl_dcc_man_adj[1] ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][0][0] = dq1_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][1][0] = dq1_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][0][1] = dq1_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][1][pll_sel-1][1][1] = dq1_wrlvl_dcc_man_adj[1] ; 

    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       ca_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       ca_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        ca_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        ca_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq0_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq0_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq0_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq0_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq1_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq1_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq1_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq1_wr90_dcc_man_adj,
		                       dummy_man_adj);

#ifdef NUM_BYTES_8

    //Store Values for Re-training
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][0][0] = dq2_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][1][0] = dq2_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][0][1] = dq2_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][2][pll_sel-1][1][1] = dq2_wrlvl_dcc_man_adj[1] ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][0][0] = dq3_wr90_dcc_man_adj[0]  ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][1][0] = dq3_wr90_dcc_man_adj[1]  ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][0][1] = dq3_wrlvl_dcc_man_adj[0] ; 
    dq_jcpll_dcc_training_g[chnl][3][pll_sel-1][1][1] = dq3_wrlvl_dcc_man_adj[1] ; 

    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq2_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq2_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq2_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq2_wr90_dcc_man_adj,
		                       dummy_man_adj);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                        0,
		                        1,
		                        0,
		                        pll_sel,
		                        dcc_wait_time,
		                        div_ref,
		                        dq3_wrlvl_dcc_man_adj,
		                        dummy_man_adj,
		                        dq3_wrlvl_dcc_man_adj,
		                        dummy_man_adj);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                       0,
		                       1,
		                       0,
		                       pll_sel,
		                       dcc_wait_time,
		                       div_ref,
		                       dq3_wr90_dcc_man_adj,
		                       dummy_man_adj,
		                       dq3_wr90_dcc_man_adj,
		                       dummy_man_adj);
#endif
  }
#endif
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC INIT calibrates both DCC WR90 and WRLVL within each DDR PHY
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_dcc_init( uint32 _inst_, uint32 chnl, uint32 jcpll_sel )
{
  uint32 gcc_pll;


  gcc_pll = jcpll_sel + 1;
  ///  Channel 0 DCC INIT
  if (chnl == 0) {
    /// DCC Cal for CA (bypass, dcc_wait_time, dcc_div, dcc_man_adj, dcc_sw_ovrd_val)
    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[1],
		                       dcc_div_ref_swc[1],
		                       dcc_man_adj_swc[1],
		                       dcc_adj_sw_val_swc[1],
		                       dcc_man_adj_swc_pll2[1],
		                       dcc_adj_sw_val_swc_pll2[1]);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[0],
		                        dcc_div_ref_swc[0],
		                        dcc_man_adj_swc[0],
		                        dcc_adj_sw_val_swc[0],
		                        dcc_man_adj_swc_pll2[0],
		                        dcc_adj_sw_val_swc_pll2[0]);
    /// DCC Cal for DQ 
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);

#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    /// DCC Cal for CA (bypass, dcc_wait_time, 0, dcc_div, dcc_man_adj, dcc_sw_ovrd_val)
    DDRPHY_CA_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[1],
		                       dcc_div_ref_swc[1],
		                       dcc_man_adj_swc[1],
		                       dcc_adj_sw_val_swc[1],
		                       dcc_man_adj_swc_pll2[1],
		                       dcc_adj_sw_val_swc_pll2[1]);
    DDRPHY_CA_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[0],
		                        dcc_div_ref_swc[0],
		                        dcc_man_adj_swc[0],
		                        dcc_adj_sw_val_swc[0],
		                        dcc_man_adj_swc_pll2[0],
		                        dcc_adj_sw_val_swc_pll2[0]);
    /// DCC Cal for DQ 
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
    DDRPHY_DQ_wrlvl_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                        0,
		                        0,
		                        0,
		                        gcc_pll,
		                        dcc_wait_time_swc[3],
		                        dcc_div_ref_swc[3],
		                        dcc_man_adj_swc[3],
		                        dcc_adj_sw_val_swc[3],
		                        dcc_man_adj_swc_pll2[3],
		                        dcc_adj_sw_val_swc_pll2[3]);
    DDRPHY_DQ_wr90_dcc_cal(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                       0,
		                       0,
		                       0,
		                       gcc_pll,
		                       dcc_wait_time_swc[2],
		                       dcc_div_ref_swc[2],
		                       dcc_man_adj_swc[2],
		                       dcc_adj_sw_val_swc[2],
		                       dcc_man_adj_swc_pll2[2],
		                       dcc_adj_sw_val_swc_pll2[2]);
#endif
  }
#endif
  EBI1_PHY_CFG_apply_dcc_cal_adj(_inst_,
	                               chnl,
	                               0,
	                               0,
	                               0,
	                               gcc_pll,
	                               dcc_wait_time_swc[0],
	                               dcc_div_ref_swc[0]);
  
  EBI1_PHY_CFG_print_log(_inst_);

  EBI1_PHY_CFG_dcc_cal_stop(_inst_,
	                          chnl);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_dcc_cal_stop( uint32 _inst_, uint32 chnl )
{

  ///  Channel 0 DCC Cal stop
  if (chnl == 0) {
    DDRPHY_CA_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
    DDRPHY_CA_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);

#ifdef NUM_BYTES_8
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    /// DCC Cal stop manual update
    DDRPHY_CA_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
    DDRPHY_CA_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wr90_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_dcc_stop(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD ODT enable control
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_pads_odt_enable( uint32 _inst_, uint32 chnl, uint32 odt_ena )
{

  ///  Channel 0 ODT Enable
  if (chnl == 0) {
    DDRPHY_CA_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                             odt_ena);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                             odt_ena);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                             odt_ena);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                             odt_ena);
    DDRPHY_DQ_io_pads_odt_enable(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                             odt_ena);
#endif
  }
#endif
}


// ***********************************************************************
/// -------------------------------------------------------------
///  CLK mode  function
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_clk_mode( uint32 _inst_, uint32 chnl, phy_clk_type clk_mode )
{

  ///  Channel 0 ODT Enable
  if (chnl == 0) {
    DDRPHY_CA_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_site_clk_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#endif
  }
#endif
}
#pragma arm section code

// ***********************************************************************
/// -------------------------------------------------------------
///  PNSLEW update
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_pnslew_update( uint32 _inst_, uint32 chnl )
{

  ///  Channel 0 ODT Enable
  if (chnl == 0) {
    if (DDRPHY_CA_pncnt_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 16) == 1) {
      DDRPHY_CA_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
			                           3);
#ifdef NUM_BYTES_8
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
			                           3);
#endif
    }
    else
    {
      if (DDRPHY_CA_pncnt_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET , 12) == 1) {
        DDRPHY_CA_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                           0);
#ifdef NUM_BYTES_8
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                           0);
#endif
      }
    }
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    if (DDRPHY_CA_pncnt_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 16) == 1) {
      DDRPHY_CA_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
			                           3);
#ifdef NUM_BYTES_8
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
			                           3);
      DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
			                           3);
#endif
    }
    else
    {
      if (DDRPHY_CA_pncnt_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET , 12) == 1) {
        DDRPHY_CA_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                           0);
#ifdef NUM_BYTES_8
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                           0);
        DDRPHY_DQ_new_pncnt_update(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                           0);
#endif
      }
    }
  }
#endif
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  CSD COntrol to enable/disable for freq_switch and power_down
/// -------------------------------------------------------------
// ***********************************************************************
void EBI1_PHY_CFG_enable_csd_rx( uint32 _inst_, uint32 chnl, phy_clk_type clk_mode )
{

  ///  Channel 0 CSR_RX enable
  if (chnl == 0) {
    DDRPHY_CA_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                        clk_mode);
    DDRPHY_DQ_enable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                        clk_mode);
#endif
  }
#endif
}


// ***********************************************************************
/// end of enable_csd_rx
// ***********************************************************************
void EBI1_PHY_CFG_disable_csd_rx( uint32 _inst_, uint32 chnl , int selfrefresh_mode)
{

  ///  Channel 0 CSR_RX enable
  if (chnl == 0) {
    DDRPHY_CA_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,selfrefresh_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,selfrefresh_mode);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,selfrefresh_mode);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,selfrefresh_mode);
    DDRPHY_DQ_disable_csd_rx(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,selfrefresh_mode);
#endif
  }
#endif
}
#pragma arm section code

// Non-SCALe Store calibratrion results
void EBI1_PHY_CFG_set_ca_dcc_cal( uint32 _inst_, uint32 chnl, uint32 dcc_man_adj[2][2][2] )
{

  ///  Channel 0 CSR_RX enable
  if (chnl == 0) {
    DDRPHY_CA_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET, dcc_man_adj);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET, dcc_man_adj);
  }
#endif  
}

// Non-SCALe Store calibratrion results
void EBI1_PHY_CFG_set_dq_dcc_cal( uint32 _inst_, uint32 chnl, uint32 dq_num, uint32 dcc_man_adj[2][2][2] )
{

  ///  Channel 0 CSR_RX enable
  if (chnl == 0) {
    if(dq_num == 0){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, dcc_man_adj);}
    if(dq_num == 1){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET, dcc_man_adj);}
#ifdef NUM_BYTES_8
    if(dq_num == 2){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET, dcc_man_adj);}
    if(dq_num == 3){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET, dcc_man_adj);}
#endif
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    if(dq_num == 0){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, dcc_man_adj);}
    if(dq_num == 1){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET, dcc_man_adj);}
#ifdef NUM_BYTES_8
    if(dq_num == 2){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET, dcc_man_adj);}
    if(dq_num == 3){DDRPHY_DQ_dcc_set(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET, dcc_man_adj);}
#endif
  }
#endif
}

// Non-SCALe Store calibratrion results
void EBI1_PHY_CFG_set_ca_neg_wrlvl( uint32 _inst_, uint32 chnl)
{
  if (chnl == 0) {
    DDRPHY_CA_neg_wrlvl(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    DDRPHY_CA_neg_wrlvl(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET);
  }
#endif  
}

// Non-SCALe set_dq_pad_pull
void EBI1_PHY_CFG_set_dq_pad_pull( uint32 _inst_, uint32 chnl, uint32 pull_value)
{
  if (chnl == 0) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
#ifdef NUM_BYTES_8
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
#endif    
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
#ifdef NUM_BYTES_8
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B, pull_value);
#endif
  }
#endif
}

// Non-SCALe dq_pad_pull_status
uint32 EBI1_PHY_CFG_dq_pad_pull_status( uint32 _inst_, uint32 chnl)
{
  uint32 dq_pad_pull=0;
  if (chnl == 0) {
    dq_pad_pull = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B);
    }
#ifdef CHNL_CNT_2
  if (chnl == 1) {
    dq_pad_pull = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_PULL_B);
    }
#endif    

  return ((dq_pad_pull));
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// Non-SCALe set_dqs_pad_pull
void EBI1_PHY_CFG_set_dqs_pad_pull( uint32 _inst_, uint32 chnl, uint32 pull_value)
{
  if (chnl == 0) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
#ifdef NUM_BYTES_8
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
#endif    
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
#ifdef NUM_BYTES_8
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B, pull_value);
#endif
  }
#endif 
}

// Non-SCALe dqs_pad_pull_status
uint32 EBI1_PHY_CFG_dqs_pad_pull_status( uint32 _inst_, uint32 chnl)
{
  uint32 dqs_pad_pull=0;
  if (chnl == 0) {
    dqs_pad_pull = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B);
    }
#ifdef CHNL_CNT_2
  if (chnl == 1) {
    dqs_pad_pull = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_PULL_B);
    }
#endif
  return ((dqs_pad_pull));
}

// Non-SCALe dqs_pad_pull_status
uint32 EBI1_PHY_CFG_hw_info(uint32 _inst_ )
{
  return (HWIO_INX (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_HW_INFO));
}

// Non-SCALe print_log
void EBI1_PHY_CFG_print_log(uint32 _inst_ )
{
#ifdef PHY_DEBUG_PRINT
#ifdef SITE_PER_CA_CNT_2  
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WR90_CFG=0x%X   CH0_CA_SITE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WR90_CFG=0x%X   CH1_CA_SITE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG));
#endif      
#endif      
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG));
#endif      
      ddr_printf(DDR_NORMAL,"CH0_DQ0_BYTE0_DCC_WR90_CFG=0x%X  CH0_DQ0_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
      ddr_printf(DDR_NORMAL,"CH0_DQ1_BYTE0_DCC_WR90_CFG=0x%X  CH0_DQ1_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH0_DQ2_BYTE0_DCC_WR90_CFG=0x%X  CH0_DQ2_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
      ddr_printf(DDR_NORMAL,"CH0_DQ3_BYTE0_DCC_WR90_CFG=0x%X  CH0_DQ3_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
#endif

#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_DQ0_BYTE0_DCC_WR90_CFG=0x%X  CH1_DQ0_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
      ddr_printf(DDR_NORMAL,"CH1_DQ1_BYTE0_DCC_WR90_CFG=0x%X  CH1_DQ1_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH1_DQ2_BYTE0_DCC_WR90_CFG=0x%X  CH1_DQ2_BYTE1_DCC_WR90_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
      ddr_printf(DDR_NORMAL,"CH1_DQ3_BYTE0_DCC_WR90_CFG=0x%X  CH1_DQ3_BYTE1_DCC_WR90_CFG=0x%X \n\n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG));
#endif
#endif

#ifdef SITE_PER_CA_CNT_2  
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WLV_CFG=0x%X   CH0_CA_SITE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WLV_CFG=0x%X   CH1_CA_SITE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG));
#endif
#endif
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG));
#endif
      ddr_printf(DDR_NORMAL,"CH0_DQ0_BYTE0_DCC_WLV_CFG=0x%X  CH0_DQ0_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
      ddr_printf(DDR_NORMAL,"CH0_DQ1_BYTE0_DCC_WLV_CFG=0x%X  CH0_DQ1_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH0_DQ2_BYTE0_DCC_WLV_CFG=0x%X  CH0_DQ2_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
      ddr_printf(DDR_NORMAL,"CH0_DQ3_BYTE0_DCC_WLV_CFG=0x%X  CH0_DQ3_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
#endif

#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_DQ0_BYTE0_DCC_WLV_CFG=0x%X  CH1_DQ0_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
      ddr_printf(DDR_NORMAL,"CH1_DQ1_BYTE0_DCC_WLV_CFG=0x%X  CH1_DQ1_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH1_DQ2_BYTE0_DCC_WLV_CFG=0x%X  CH1_DQ2_BYTE1_DCC_WLV_CFG=0x%X \n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
      ddr_printf(DDR_NORMAL,"CH1_DQ3_BYTE0_DCC_WLV_CFG=0x%X  CH1_DQ3_BYTE1_DCC_WLV_CFG=0x%X \n\n",HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG),HWIO_INX(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG));
#endif
#endif

#ifdef SITE_PER_CA_CNT_2  
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WR90=0x%X   CH0_CA_SITE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_STATUS,DCC_OFFSET));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WR90=0x%X   CH1_CA_SITE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_STATUS,DCC_OFFSET));
#endif
#endif
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS,DCC_OFFSET));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS,DCC_OFFSET));
#endif
      ddr_printf(DDR_NORMAL,"CH0_DQ0_BYTE0_DCC_WR90=0x%X  CH0_DQ0_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH0_DQ1_BYTE0_DCC_WR90=0x%X  CH0_DQ1_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH0_DQ2_BYTE0_DCC_WR90=0x%X  CH0_DQ2_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH0_DQ3_BYTE0_DCC_WR90=0x%X  CH0_DQ3_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
#endif

#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_DQ0_BYTE0_DCC_WR90=0x%X  CH1_DQ0_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH1_DQ1_BYTE0_DCC_WR90=0x%X  CH1_DQ1_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH1_DQ2_BYTE0_DCC_WR90=0x%X  CH1_DQ2_BYTE1_DCC_WR90=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH1_DQ3_BYTE0_DCC_WR90=0x%X  CH1_DQ3_BYTE1_DCC_WR90=0x%X \n\n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS,DCC_OFFSET));
#endif
#endif

#ifdef SITE_PER_CA_CNT_2  
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WLV=0x%X   CH0_CA_SITE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_STATUS,DCC_OFFSET));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WLV=0x%X   CH1_CA_SITE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_STATUS,DCC_OFFSET));
#endif
#endif
      ddr_printf(DDR_NORMAL,"CH0_CA_SITE0_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS,DCC_OFFSET));
#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_CA_SITE0_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS,DCC_OFFSET));
#endif
      ddr_printf(DDR_NORMAL,"CH0_DQ0_BYTE0_DCC_WLV=0x%X  CH0_DQ0_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH0_DQ1_BYTE0_DCC_WLV=0x%X  CH0_DQ1_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH0_DQ2_BYTE0_DCC_WLV=0x%X  CH0_DQ2_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH0_DQ3_BYTE0_DCC_WLV=0x%X  CH0_DQ3_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
#endif

#ifdef CHNL_CNT_2  
      ddr_printf(DDR_NORMAL,"CH1_DQ0_BYTE0_DCC_WLV=0x%X  CH1_DQ0_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH1_DQ1_BYTE0_DCC_WLV=0x%X  CH1_DQ1_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));

#ifdef NUM_BYTES_8
      ddr_printf(DDR_NORMAL,"CH1_DQ2_BYTE0_DCC_WLV=0x%X  CH1_DQ2_BYTE1_DCC_WLV=0x%X \n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
      ddr_printf(DDR_NORMAL,"CH1_DQ3_BYTE0_DCC_WLV=0x%X  CH1_DQ3_BYTE1_DCC_WLV=0x%X \n\n",HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS,DCC_OFFSET),HWIO_INXF(_inst_+SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS,DCC_OFFSET));
#endif
#endif
#endif
}
#pragma arm section code





























// ***********************************************************************
/// Global variables
///  BYTE REMAP function
///  Comply with byte remapping per project (see EBI HDD for mapping)
///  chip_id= 0: Trahira (one-to-one remapping), ie,DQ0 contains byte0 and byte1, etc..
///           1: Gandalf (remapping), ie,DQ0 contains byte0 and byte2,etc...
// ***********************************************************************
/*uint32 EBI1_PHY_CFG_remap_byte( uint32 _inst_, uint32 chip_id, uint32 which_byte )
{
  uint32 remapped = 0;


  if (chip_id == 1) {
    /// seq_msg(INFO, 0, "byte_remap:  using Gandalf byte mapping");
    switch(which_byte){
      case 0 :
        remapped = 0;
        break;
      case 1 :
        remapped = 3;
        break;
      case 2 :
        remapped = 1;
        break;
      case 3 :
        remapped = 2;
        break;
      case 4 :
        remapped = 4;
        break;
      case 5 :
        remapped = 7;
        break;
      case 6 :
        remapped = 5;
        break;
      case 7 :
        remapped = 6;
        break;
      default :
        remapped = 0;
        break;
    }
  }
  else
  {
    if (chip_id == 2) {
      /// seq_msg(INFO, 0, "byte_remap:  using Luxor byte mapping");
      switch(which_byte){
        case 0 :
          remapped = 0;
          break;
        case 1 :
          remapped = 1;
          break;
        case 2 :
          remapped = 2;
          break;
        case 3 :
          remapped = 3;
          break;
        case 4 :
          remapped = 5;
          break;
        case 5 :
          remapped = 4;
          break;
        case 6 :
          remapped = 7;
          break;
        case 7 :
          remapped = 6;
          break;
        default :
          remapped = 0;
          break;
      }
    }
    else
    {
      remapped = which_byte;
    }
  }
  
  return ((remapped));
}
*/

// ***********************************************************************
///  REN Functions
///  These REN functions support RCW trainning initiated by BIMC.
///  For each byte, the REN fields in  DRPHY_DQ_REN_CFG0 are configured as followed:
///        ren_cfg[9]= 0: 0 PHY delay; 1: T/2 PHY delay
///        ren_cfg[8]= 0: 0 delay; 1: ~320 ps delay (OR'ed with ren_cfg[5])
///        ren_cfg[6:3]= CDC Coarse delay when ren_cfg[8]=0
///        ren_cfg[2:0]= CDC Fine delay
/// 
///  The functions provided below support the following:
///    1. update half-cycle delay only.
///    2. update coarse delay only.
///    3. update fine delay only.
/// 
///  Function: provides T/2 CDC path delay per byte
///   byte_hcycle: 0 PHY delay; 1: T/2 PHY delay
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_ren_delay_update( uint32 _inst_, uint32 chip_id, uint32 channel_select, uint32 which_ren_cfg, uint32 which_byte, uint32 value )
{
  uint32 which_remapped_byte;



  if (channel_select == 0) {
  /// Here 
   which_remapped_byte = dq_remap_lut[0][which_byte];         
    /// seq_msg(INFO, 0, "Ch0 ddr_phy_ren_sw_half_cycle_update: updating REN half-cycle delay");
    switch(which_remapped_byte){
      case 0 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 1 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 2 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 3 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;

#ifdef NUM_BYTES_8
      case 4 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 5 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 6 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 7 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
#endif
      default :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
    }
  }
#ifdef CHNL_CNT_2  
  if (channel_select == 1 ) {
   which_remapped_byte = dq_remap_lut[1][which_byte];
   /// seq_msg(INFO, 0, "Ch1 ddr_phy_ren_sw_half_cycle_update: updating REN half-cycle delay");
    switch(which_remapped_byte){
      case 0 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 1 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 2 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 3 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
#ifdef NUM_BYTES_8
      case 4 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 5 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 6 :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
      case 7 :
        DDRPHY_DQ_ren_cfg_byte1_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
#endif
      default :
        DDRPHY_DQ_ren_cfg_byte0_sel(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                            which_ren_cfg,
				                            value);
        break;
    }
  }
#endif
}


// ***********************************************************************
///  Function: provides coarse CDC delay per byte
///   byte_coarse_delay[3:0]: <value> ~79 ps increments
///  Function: provides fine CDC delay per byte
///   byte_fine_delay[2:0]: <value> ~15 ps increments
/// / This function needs to be called by DDRSS to select t
/// / cfg index
///  REN Functions
///  These REN functions support RCW trainning initiated by BIMC.
///  For each byte, the REN fields in  DRPHY_DQ_REN_CFG0 are configured as followed:
///        ren_cfg[9]= 0: 0 PHY delay; 1: T/2 PHY delay
///        ren_cfg[8]= 0: 0 delay; 1: ~320 ps delay (OR'ed with ren_cfg[5])
///        ren_cfg[6:3]= CDC Coarse delay when ren_cfg[8]=0
///        ren_cfg[2:0]= CDC Fine delay
/// 
///  The functions provided below support the following:
///    1. SW override to control REN via CSR
///    2. Enable FPM control.
///    3. Force a reset to REN logic.
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_ren_sw_mode( uint32 _inst_, uint32 chip_select, uint32 ren_cfg_num )
{

  /// seq_msg(INFO, 0, "ddr_phy_ren_sw_mode: DDR PHY is in REN SW mode");
  ///  Channel 0 CDC delays
  if (chip_select == 0) {
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chip_select == 1) {
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
    DDRPHY_DQ_ren_sw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                      ren_cfg_num);
#endif
  }
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_ren_hw_mode( uint32 _inst_, uint32 chip_select )
{

  /// seq_msg(INFO, 0, "ddr_phy_ren_hw_mode: DDR PHY is in REN HW (FPM) mode");
  ///  Channel 0 CDC delays
  if (chip_select == 0) {
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chip_select == 1) {
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_ren_hw_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ddr_phy_ren_reset( uint32 _inst_, uint32 chip_select, uint32 byte0_reset, uint32 byte1_reset )
{
  
  
  
  
  
  

  ///  Channel 0 CDC delays
  if (chip_select == 0) {
    /// seq_msg(INFO, 0, "Ch0 ddr_phy_ren_reset:  REN Reset asserted");
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
#endif
  }
#ifdef CHNL_CNT_2  
  if (chip_select == 1) {
    /// seq_msg(INFO, 0, "Ch1 ddr_phy_ren_reset:  REN Reset asserted");
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
#ifdef NUM_BYTES_8
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
  
    DDRPHY_DQ_ren_reset(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                    byte0_reset,
		                    byte1_reset);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 EBI1_PHY_CFG_get_ren_cfg( uint32 _inst_, uint32 channel_sel )
{
  uint32 ren_cfg_num = 0x7;


  if (channel_sel == 0) {
    if (HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, FPM_CTRL_EN_REN)) {
      ren_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_FPM_STATUS, WLVL_HASH_SELECT);
    }
    else
    {
      ren_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD);
    }
  }
#ifdef CHNL_CNT_2  
  if (channel_sel == 1) {
    if (HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, FPM_CTRL_EN_REN)) {
      ren_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_FPM_STATUS, WLVL_HASH_SELECT);
    }
    else
    {
      ren_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD);
    }
  }
#endif  
  return ((ren_cfg_num));
}


/* =============================================================================
**  Non-SCALe Function : EBI1_PHY_REN_Get_Trained_Results
** =============================================================================
*/
/**
*    @details
*     save REN training results for PHY 
*
*   @param[in]  which_freq_plan   which frequency
*   @param[in]  channel_sel    channel_sel to set 
*   @param[in]  RCW_Hcycle_Delay_Result value of half-cycle delay
*   @param[in]  RCW_Coarse_Delay_Result value of coarse delay
*   @param[in]  RCW_Fine_Delay_Result   value of fine delay
**/
void EBI1_PHY_REN_Get_Trained_Results  (uint32 which_ren_cfg, SDRAM_INTERFACE channel_sel, uint32 RCW_Hcycle_Delay_Result[7][2][8], uint32 RCW_Coarse_Delay_Result[7][2][8], uint32 RCW_Fine_Delay_Result[7][2][8], uint16 dq_ren_training[2][7][8])
{
int which_byte =0;
  for (which_byte=0;which_byte<NUM_BYTES;which_byte++){
     dq_ren_training[channel_sel][which_ren_cfg][which_byte]= 
        ( RCW_Hcycle_Delay_Result[which_ren_cfg][channel_sel][which_byte] << 9 |
          RCW_Coarse_Delay_Result[which_ren_cfg][channel_sel][which_byte] << 3 |
          RCW_Fine_Delay_Result[which_ren_cfg][channel_sel][which_byte]         );
    }
}

/* =============================================================================
**  Non-SCALe Function : EBI1_PHY_REN_Set_Trained_results
** =============================================================================
*/
/**
*    @details
*     restore/set ren training results
*
*   @param[in]  dq_ren_training   Byte assignment
**/
void EBI1_PHY_REN_Set_Trained_results( uint32 _inst_, uint16 dq_ren_training[2][7][8])
{
int chnl_index, ren_cfg_index, dq_byte_index;

  for (chnl_index = 0; chnl_index < CHNL_CNT;  chnl_index ++){              // 0-1
    for (ren_cfg_index = 0; ren_cfg_index < REN_CFG_CNT; ren_cfg_index ++){ // 0-6
      for (dq_byte_index = 0; dq_byte_index < NUM_BYTES; dq_byte_index ++){ // 0-7
        EBI1_PHY_CFG_ddr_phy_ren_delay_update( _inst_ ,
                                               0,                // chip_id (don't care), 
                                               chnl_index,       // channel_select, 
                                               ren_cfg_index,    // which_ren_cfg, 
                                               dq_byte_index,    // which_byte, 
                                               dq_ren_training[chnl_index][ren_cfg_index][dq_byte_index] //  value 
                                              );
      }
    }
  }
}





























uint32 wrlvl_status_g[DQ_CNT];
uint32 wrlvl_delay_g[DQ_CNT];

// ***********************************************************************
///  sequences which write to wrlv_cfg0...6 fields.
// ***********************************************************************
void EBI1_PHY_CFG_enable_wrlvl_mode( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
 
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_enable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_disable_wrlvl_mode( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_disable_wrlvl_mode(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_set_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 delay_index, uint32 wrlvl_delay_val )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
    DDRPHY_DQ_set_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                          delay_index,
		                          wrlvl_delay_val);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_set_wrlvl_cdc_retimer( uint32 _inst_, uint32 channel_sel, uint32 cfg_index, uint32 byte0_retimer_sel_LUT_value[DQ_CNT], uint32 byte0_retimer_ie_oe_LUT_value[DQ_CNT], uint32 byte0_retimer_cycle_delay_sel[DQ_CNT], uint32 byte1_retimer_sel_LUT_value[DQ_CNT], uint32 byte1_retimer_ie_oe_LUT_value[DQ_CNT], uint32 byte1_retimer_cycle_delay_sel[DQ_CNT] )
{
  
  
  
  
  
  

  if (channel_sel == 0) {
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[0],
		                                byte0_retimer_ie_oe_LUT_value[0],
		                                byte0_retimer_cycle_delay_sel[0],
		                                byte1_retimer_sel_LUT_value[0],
		                                byte1_retimer_ie_oe_LUT_value[0],
		                                byte1_retimer_cycle_delay_sel[0]);
  
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[1],
		                                byte0_retimer_ie_oe_LUT_value[1],
		                                byte0_retimer_cycle_delay_sel[1],
		                                byte1_retimer_sel_LUT_value[1],
		                                byte1_retimer_ie_oe_LUT_value[1],
		                                byte1_retimer_cycle_delay_sel[1]);
  
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[2],
		                                byte0_retimer_ie_oe_LUT_value[2],
		                                byte0_retimer_cycle_delay_sel[2],
		                                byte1_retimer_sel_LUT_value[2],
		                                byte1_retimer_ie_oe_LUT_value[2],
		                                byte1_retimer_cycle_delay_sel[2]);
  
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[3],
		                                byte0_retimer_ie_oe_LUT_value[3],
		                                byte0_retimer_cycle_delay_sel[3],
		                                byte1_retimer_sel_LUT_value[3],
		                                byte1_retimer_ie_oe_LUT_value[3],
		                                byte1_retimer_cycle_delay_sel[3]);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[0],
		                                byte0_retimer_ie_oe_LUT_value[0],
		                                byte0_retimer_cycle_delay_sel[0],
		                                byte1_retimer_sel_LUT_value[0],
		                                byte1_retimer_ie_oe_LUT_value[0],
		                                byte1_retimer_cycle_delay_sel[0]);
  
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[1],
		                                byte0_retimer_ie_oe_LUT_value[1],
		                                byte0_retimer_cycle_delay_sel[1],
		                                byte1_retimer_sel_LUT_value[1],
		                                byte1_retimer_ie_oe_LUT_value[1],
		                                byte1_retimer_cycle_delay_sel[1]);
  
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[2],
		                                byte0_retimer_ie_oe_LUT_value[2],
		                                byte0_retimer_cycle_delay_sel[2],
		                                byte1_retimer_sel_LUT_value[2],
		                                byte1_retimer_ie_oe_LUT_value[2],
		                                byte1_retimer_cycle_delay_sel[2]);
  
    DDRPHY_DQ_set_wrlvl_cdc_retimer(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                                cfg_index,
		                                byte0_retimer_sel_LUT_value[3],
		                                byte0_retimer_ie_oe_LUT_value[3],
		                                byte0_retimer_cycle_delay_sel[3],
		                                byte1_retimer_sel_LUT_value[3],
		                                byte1_retimer_ie_oe_LUT_value[3],
		                                byte1_retimer_cycle_delay_sel[3]);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_cdc_wr90_delay( uint32 _inst_, uint32 channel_sel, uint32 delay_index, uint32 delay_value )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
    DDRPHY_DQ_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                         delay_index,
		                         delay_value);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_set_cdc_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 dq_index, uint32 delay_index, uint32 delay_value )
{

  if (channel_sel == 0) {
    switch(dq_index){
      case 0 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
      case 1 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
#ifdef NUM_BYTES_8
      case 2 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
      case 3 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
#endif
      default :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
    }
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    switch(dq_index){
      case 0 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
      case 1 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
#ifdef NUM_BYTES_8
      case 2 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
      case 3 :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
#endif
      default :
        DDRPHY_DQ_set_cdc_wrlvl_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
				                              byte_index,
				                              delay_index,
				                              delay_value);
        break;
    }
  }
#endif
}


// ***********************************************************************
///  WRLVL_TRIGGER FUCNTIONS
// ***********************************************************************
void EBI1_PHY_CFG_wrlvl_trigger_pulse( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_pulse(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_wrlvl_trigger_zero( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_zero(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_wrlvl_trigger_one( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET);
    DDRPHY_DQ_wrlvl_trigger_one(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_read_wrlvl_status( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    wrlvl_status_g[0] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET );
    wrlvl_status_g[1] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET );
#ifdef NUM_BYTES_8
    wrlvl_status_g[2] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET );
    wrlvl_status_g[3] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET );
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    wrlvl_status_g[0] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET );
    wrlvl_status_g[1] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET );
#ifdef NUM_BYTES_8
    wrlvl_status_g[2] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET );
    wrlvl_status_g[3] = DDRPHY_DQ_read_wrlvl_status(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET );
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_read_wrlvl_delay( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 cfg_index )
{

  if (channel_sel == 0) {
    wrlvl_delay_g[0] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
    wrlvl_delay_g[1] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
#ifdef NUM_BYTES_8
    wrlvl_delay_g[2] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
    wrlvl_delay_g[3] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
#endif
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    wrlvl_delay_g[0] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
    wrlvl_delay_g[1] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
#ifdef NUM_BYTES_8
    wrlvl_delay_g[2] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
    wrlvl_delay_g[3] = DDRPHY_DQ_read_wrlvl_delay(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET , byte_index, cfg_index);
#endif
  }
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 EBI1_PHY_CFG_get_wrlvl_cfg( uint32 _inst_, uint32 channel_sel )
{
  uint32 wrlvl_cfg_num = 0x7;


  if (channel_sel == 0) {
    if (HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_FROM_CSR_FVAL) {
      wrlvl_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD);
    }
    else
    {
      wrlvl_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_FPM_STATUS, WLVL_HASH_SELECT);
    }
  }
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    if (HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_FROM_CSR_FVAL) {
      wrlvl_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD);
    }
    else
    {
      wrlvl_cfg_num = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_FPM_STATUS, WLVL_HASH_SELECT);
    }
  }
#endif
  return ((wrlvl_cfg_num));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 EBI1_PHY_CFG_get_wrlvl_period( uint32 _inst_, uint32 channel_sel, uint32 cfg_index )
{
  uint32 wrlvl_period = 0;
  uint32 wrlvl_local_index = 0;

  
  

  if (channel_sel == 0) {
    if (HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_WRLVL_FROM_CSR_FVAL) {
      wrlvl_local_index = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_CDC_DELAY_SEL, FPM_WRLVL_SEL_OVRD);
  
      switch(wrlvl_local_index){
        case 0 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
        case 1 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, DDR_CLK_PERIOD);
          break;
        case 2 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, DDR_CLK_PERIOD);
          break;
        case 3 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, DDR_CLK_PERIOD);
          break;
        case 4 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, DDR_CLK_PERIOD);
          break;
        case 5 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, DDR_CLK_PERIOD);
          break;
        case 6 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, DDR_CLK_PERIOD);
          break;
        default :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
      }
    }
    else
    {
      switch(cfg_index){
        case 0 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
        case 1 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, DDR_CLK_PERIOD);
          break;
        case 2 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, DDR_CLK_PERIOD);
          break;
        case 3 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, DDR_CLK_PERIOD);
          break;
        case 4 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, DDR_CLK_PERIOD);
          break;
        case 5 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, DDR_CLK_PERIOD);
          break;
        case 6 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, DDR_CLK_PERIOD);
          break;
        default :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
      }
    }
  }
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    if (HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_WRLVL_FROM_CSR_FVAL) {
      wrlvl_local_index = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_CDC_DELAY_SEL, FPM_WRLVL_SEL_OVRD);
  
      switch(wrlvl_local_index){
        case 0 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
        case 1 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, DDR_CLK_PERIOD);
          break;
        case 2 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, DDR_CLK_PERIOD);
          break;
        case 3 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, DDR_CLK_PERIOD);
          break;
        case 4 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, DDR_CLK_PERIOD);
          break;
        case 5 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, DDR_CLK_PERIOD);
          break;
        case 6 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, DDR_CLK_PERIOD);
          break;
        default :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
      }
    }
    else
    {
      switch(cfg_index){
        case 0 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
        case 1 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, DDR_CLK_PERIOD);
          break;
        case 2 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, DDR_CLK_PERIOD);
          break;
        case 3 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, DDR_CLK_PERIOD);
          break;
        case 4 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, DDR_CLK_PERIOD);
          break;
        case 5 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, DDR_CLK_PERIOD);
          break;
        case 6 :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, DDR_CLK_PERIOD);
          break;
        default :
          wrlvl_period = HWIO_INXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD);
          break;
      }
    }
  }
#endif  
  return ((wrlvl_period));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_wrlvl_freq_cfg( uint32 _inst_, uint32 channel_sel, uint32 freq_index, uint32 freq_val )
{

  if (channel_sel == 0) {
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
#endif
  }
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
#ifdef NUM_BYTES_8
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
    DDRPHY_DQ_wrlvl_freq_cfg(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                         freq_index,
		                         freq_val);
#endif
  }
#endif
}

// NON_SCALE: Need to generate from scale!!!!
void EBI1_PHY_CFG_set_cdc_wrlvl_training( uint32 _inst_, uint32 channel_sel, uint32 byte_index, uint32 dq_index, uint32 delay_index, uint16 retimer_value, uint16 delay_value )
{

  if (channel_sel == 0) {
    switch(dq_index){
      case 0 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
      case 1 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
#ifdef NUM_BYTES_8
	  case 2 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
      case 3 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
#endif        
      default :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
    }
  }
  
#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    switch(dq_index){
      case 0 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
      case 1 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;

#ifdef NUM_BYTES_8
      case 2 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
      case 3 :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
#endif
        default :
        DDRPHY_DQ_set_cdc_wrlvl_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
                                      byte_index,
                                      delay_index,
                                      retimer_value,
                                      delay_value);
        break;
    }
  }
#endif
}

// ***********************************************************************
//   Set Half-cycle delay at CA for neg-wrlvl
// ***********************************************************************
void EBI1_PHY_CFG_set_wrlvl_half_cycle_delay( uint32 _inst_, uint32 channel_sel )
{

  if (channel_sel == 0) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET, DDRPHY_CA_DDRPHY_CA_TOP_CFG, FPM_CTRL_EN_NEG_WRLVL_HALF_T_DELAY, 1);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET, DDRPHY_CA_DDRPHY_CA_FPM_CFG2, NEG_WRLVL_HALF_T_DELAY_THRESHOLD, 0xE);
  }

#ifdef CHNL_CNT_2 
  if (channel_sel == 1) {
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET, DDRPHY_CA_DDRPHY_CA_TOP_CFG, FPM_CTRL_EN_NEG_WRLVL_HALF_T_DELAY, 1);
    HWIO_OUTXF (_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET, DDRPHY_CA_DDRPHY_CA_FPM_CFG2, NEG_WRLVL_HALF_T_DELAY_THRESHOLD, 0xE);
  }
#endif
}



























// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay( uint32 _inst_, uint16 chnl, uint16 site, uint16 coarse_delay_value, uint16 fine_delay_value )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                                                     site,
		                                                     coarse_delay_value,
		                                                     fine_delay_value);
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                                                     site,
		                                                     coarse_delay_value,
		                                                     fine_delay_value);
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint16 deskew_delay_value )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_seq_set_initial_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                                                              site,
		                                                              deskew_delay_value);
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_seq_set_initial_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                                                              site,
		                                                              deskew_delay_value);
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_seq_set_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site,uint16 bitn, uint16 deskew_delay_value )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_seq_set_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                                                              site,
									      bitn,
		                                                              deskew_delay_value);
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_seq_set_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                                                              site,
									      bitn,
		                                                              deskew_delay_value);
	}
#endif
}
// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_seq_restore_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint32 deskew_delay_value[2] )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_seq_restore_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                                 			   site,
		                                 			   deskew_delay_value);
	}
#ifdef CHNL_CNT_2	
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_seq_restore_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                                                              site,
		                                                              deskew_delay_value);
	}
#endif	
}
// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_seq_read_cdc_perbit_deskew_delay( uint32 _inst_, uint16 chnl, uint16 site, uint32 deskew_delay_value[2] )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_seq_read_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                                 			   site,
		                                 			   deskew_delay_value);
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_seq_read_cdc_perbit_deskew_delay(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                                                              site,
		                                                              deskew_delay_value);
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_ca_training_cdc_wr90_delay_select( uint32 _inst_, uint16 chnl,uint16 sel )
{

	if (chnl == 0x0) {
		DDRPHY_CA_ca_training_cdc_wr90_delay_select(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET, sel);
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_CA_ca_training_cdc_wr90_delay_select(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET, sel);
	}
#endif	
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_set_ca_cdc_wr90( uint32 _inst_, uint16 chnl, uint16 site, uint16 ca_cdc_wr90_delay[4] )
{

	if (chnl == 0x0) {
		DDRPHY_CA_set_ca_cdc_wr90(_inst_ + SEQ_EBI1_PHY_CFG_CH0_CA_DDRPHY_CA_OFFSET,
		                          site,
		                          ca_cdc_wr90_delay);
					  }
#ifdef CHNL_CNT_2	
	if (chnl == 0x1) {
		DDRPHY_CA_set_ca_cdc_wr90(_inst_ + SEQ_EBI1_PHY_CFG_CH1_CA_DDRPHY_CA_OFFSET,
		                          site,
		                          ca_cdc_wr90_delay);
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training( uint32 _inst_, uint16 chnl, uint16 mode )
{

	if (chnl == 0x0) {
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET,
		                                      mode);
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET,
		                                      mode);
#ifdef NUM_BYTES_8		                                      
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET,
		                                      mode);
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET,
		                                      mode);
#endif
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET,
		                                      mode);
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET,
		                                      mode);
#ifdef NUM_BYTES_8		                                      
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET,
		                                      mode);
		DDRPHY_DQ_set_exit_dq_for_ca_training(_inst_ + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET,
		                                      mode);
#endif
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint16 EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training( uint32 _inst_, uint16 chnl, uint16 site )
{
	uint16 remap_gandalf=0;
	//uint16 remap_shere[16]=0; //syntax is not compatible in gcc c_jramak
	uint16 remap_shere[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint16 dq_0;
	uint16 dq_1;
#if APQ8084 || FSM9900
	uint16 dq_2;
	uint16 dq_3;
#endif	
	
	uint16 final_remap =0;
	int i =0;


	final_remap = remap_gandalf;
	
	if (chnl == 0x0) {
		if (site == 0x0) {
			dq_0 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ0_DDRPHY_DQ_OFFSET );
			dq_1 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ1_DDRPHY_DQ_OFFSET );

			remap_gandalf = (dq_0 & 0xFF) | (dq_1 & 0xFF00);
			//printf("\nDQPHY 0 = 0x%x,DQPHY1 = 0x%x\n",dq_0,dq_1);	
			//remap_shere = (dq_0 & 0xFF) | (dq_1 & 0xFF00);
			//remap_shere   = (dq_0 >> 8  ) | (dq_1 <<    8 );
			dq_0 = dq_0 & 0xFF00;
			dq_1 = dq_1 & 0xFF00;

			//remap_shere   = remap_shere & 0x00FF;
			remap_shere[5]  = (dq_0 & 0x8000)?1:0;
			remap_shere[7]  = (dq_0 & 0x4000)?1:0;;
			remap_shere[6]  = (dq_0 & 0x2000)?1:0;
			remap_shere[1]  = (dq_0 & 0x1000)?1:0;
			remap_shere[3]  = (dq_0 & 0x0800)?1:0;
			remap_shere[4]  = (dq_0 & 0x0400)?1:0;
			remap_shere[0]  = (dq_0 & 0x0200)?1:0;
			remap_shere[2]  = (dq_0 & 0x0100)?1:0;
			remap_shere[10] = (dq_1 & 0x8000)?1:0;
			remap_shere[9]  = (dq_1 & 0x4000)?1:0;
			remap_shere[11] = (dq_1 & 0x2000)?1:0;
			remap_shere[13] = (dq_1 & 0x1000)?1:0;
			remap_shere[14] = (dq_1 & 0x0800)?1:0;
			remap_shere[8]  = (dq_1 & 0x0400)?1:0;
			remap_shere[12] = (dq_1 & 0x0200)?1:0;
			remap_shere[15] = (dq_1 & 0x0100)?1:0;

			for(i=0;i<16;i++){

				final_remap = final_remap | (remap_shere[i]<<i); 

			}
			

		}
		#ifdef SITE_PER_CA_CNT_2
		if (site == 0x1) {
			dq_2 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ2_DDRPHY_DQ_OFFSET );
			dq_3 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH0_DQ3_DDRPHY_DQ_OFFSET );
			remap_gandalf = (dq_2 & 0xFF) | (dq_3 & 0xFF00);
		}
		#endif
		
	}
#ifdef CHNL_CNT_2
	if (chnl == 0x1) {
		if (site == 0x0) {	
			dq_2 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ2_DDRPHY_DQ_OFFSET );
			dq_3 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ3_DDRPHY_DQ_OFFSET );
			remap_gandalf = (dq_2 & 0xFF) | (dq_3 & 0xFF00);
		}
#ifdef SITE_PER_CA_CNT_2
		if (site == 0x1) {	
			dq_0 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ0_DDRPHY_DQ_OFFSET );
			dq_1 = DDRPHY_DQ_read_dq_training_status_for_ca_training(_inst_  + SEQ_EBI1_PHY_CFG_CH1_DQ1_DDRPHY_DQ_OFFSET );
			remap_gandalf = (dq_0 & 0xFF) | (dq_1 & 0xFF00);
		}
	}
#endif
#endif


#if MSM8936
return (final_remap);
#endif


#if APQ8084 || FSM9900
 return (remap_gandalf);
#endif

}



#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"


void EBI1_PHY_Set_Config_dq (uint32 _inst_)
{
  uint32 reg;

      for (reg = 0; ddrphy_dq_mision_mode_cfg[reg][0] != END_OF_SETTING; reg++)
      {
       out_dword( _inst_ + ddrphy_dq_mision_mode_cfg[reg][0] , ddrphy_dq_mision_mode_cfg[reg][1]);
      }
}

void EBI1_PHY_Set_Config_ca (uint32 _inst_)
{
  uint32 reg;

      for (reg = 0; ddrphy_ca_mision_mode_cfg[reg][0] != END_OF_SETTING; reg++)
      {
       out_dword( _inst_ + ddrphy_ca_mision_mode_cfg[reg][0] ,ddrphy_ca_mision_mode_cfg[reg][1]);
      }
}

#pragma arm section code



