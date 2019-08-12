#include "ddr_phy_ca.h"

#pragma arm section rwdata = "BOOT_RPM_RAM_DATA_ZONE"
uint32 ca_pad_cfg0_lp_rec ;
uint32 ca_pad_cfg1_lp_rec ;
uint32 ca_pad_cfg2_lp_rec ;
uint32 ca_pad_cfg3_lp_rec ;
uint32 ca_pad_cfg4_lp_rec ;
uint32 ca_pad_cfg5_lp_rec ;

uint32 ca_pad_cfg0_pc_rec ;
uint32 ca_pad_cfg1_pc_rec ;
uint32 ca_pad_cfg2_pc_rec ;
uint32 ca_pad_cfg3_pc_rec ;
uint32 ca_pad_cfg4_pc_rec ;
uint32 ca_pad_cfg5_pc_rec ;
#pragma arm section rwdata

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_init_io_pads( uint32 _inst_, uint32 device_type )
{

  if (device_type) {
#if !MSM8936  
    /// CA PCDDR3
    /// seq_msg(INFO, 0, "Initializing CA PADS for PCDDR3");
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, 0x7C400001);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG1, 0xFFFFFFFF);
    /// /Pad Config for PCDDR3
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0, ca_pad_cfg0_pc_rec ); //0x2000F170);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1, ca_pad_cfg1_pc_rec ); //0x200000F0);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG2, ca_pad_cfg2_pc_rec ); //0x10000000);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG3, ca_pad_cfg3_pc_rec ); //0x1103C0);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG4, ca_pad_cfg4_pc_rec ); //0xFFFFFFFF);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG5, ca_pad_cfg5_pc_rec ); //0x0);
#endif
  }
  else
  {
    /// CA LPDDR3
    /// seq_msg(INFO, 0, "Initializing CA PADS for LPDDR3");
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, 0x7C400000);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG1, 0x000003E0);
    /// /Pad Config for LPDDR3
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0, ca_pad_cfg0_lp_rec ); // 0x2000F170);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1, ca_pad_cfg1_lp_rec ); // 0x200000F0);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG2, ca_pad_cfg2_lp_rec ); // 0x10000000);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG3, ca_pad_cfg3_lp_rec ); // 0x60000C0);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG4, ca_pad_cfg4_lp_rec ); // 0x60000C0);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG5, ca_pad_cfg5_lp_rec ); // 0x0);
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_set_dev_type( uint32 _inst_, uint32 device_type )
{

  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, MODE_SDR, device_type);
}


// ***********************************************************************
/// -------------------------------------------------------------
///  Set 1x vs 2x(testmode) clocking 
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_site_clk_mode( uint32 _inst_, int clk_mode )
{

  ///  Clock mode
  ///                                             |Reset | Static Legacy | Dynamic Legacy | SRPLL Mode   
  ///  DDRPHY clock                               | 1x   | 2x            | 1x             | 1x         
  ///  DDRPHY_[CA/DQ]_SITE_CK_DP[MODE_1X])        | b'1' | b'0'          | b'1'           | Don't Care 
  ///  DDRPHY_[CA/DQ]_SITE_CK_DP[CLK_MODE_STATIC] | b'0' | b'1'          | b'0'           | b'0'       
  if (clk_mode == 1) {
    ///  static_legacy
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, MODE_1X, 0);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, CLK_MODE_STATIC, 1);
  }
  else
  {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, MODE_1X, 1);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, CLK_MODE_STATIC, 0);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------     
///  CDC INIT sequence begin here                                     
/// -------------------------------------------------------------     
// ***********************************************************************
void DDRPHY_CA_init_cdc( uint32 _inst_ )
{

  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0, 0x0018);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1, 0x0020);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2, 0x0030);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3, 0x0040);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_LP_DELAY, 0x1);
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DQ CSR INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_init_ca_csr( uint32 _inst_ )
{

  //HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, 0x51010715); has auto_iocal enabled
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, 0x01010715); // Disabled auto_iocal
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_PNCNT_CFG, 0x1010);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_TIMER_CFG, 0xFA000020);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CA_IOC_SLV_CFG, 0x80000000);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CK_IOC_SLV_CFG, 0x80000000);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, 0x121A4210);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_CSD_RX_CFG, 0x80000100);
                     
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, 0x121A4210);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_CSD_RX_CFG, 0x80000100);

#endif  
  /// FPM CONFIG
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_FPM_CFG0, 0xC02C864);
  //HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_FPM_CFG1, 0x1800028);// SVS @ 250MHz
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_FPM_CFG1, 0x18000FF); // SVS @ <50MHz
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_FPM_CFG2, 0xC00032);
  //  WRLVL_DCC_MAN_ADJ_PLL2  set
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, 0x10);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, 0x10);
#endif  
  //  WR90_DCC_MAN_ADJ_PLL2  set
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, 0x10);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, 0x10);
#endif  
}


// ***********************************************************************
/// -------------------------------------------------------------
///  IO Calibration sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_manual_io_cal( uint32 _inst_ )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Manual IO CAL Trigger ....");
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_BMSK)) | ((HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_START_IO_CAL_IMMEDIATELY_FVAL) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, tmp );
  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_STATUS, IOCAL_DONE_D) == 0 );
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_auto_io_cal( uint32 _inst_ )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Auto IO CAL Enabled ....");
  ///  Wait for on-going IOCAL to complete
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_BANDGAP_ENA_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_BANDGAP_ENA_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_PN_SEL_CA_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_PN_SEL_CA_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_PN_SEL_DATA_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_PN_SEL_DATA_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_USE_LAST_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_USE_LAST_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, tmp );
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CA_IOC_SLV_CFG, 0x80000000);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_CK_IOC_SLV_CFG, 0x80000000);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_PNCNT_CFG, 0x1010);
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_TIMER_CFG, 0xFA000020);
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, CAL_NOW, HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_START_IO_CAL_IMMEDIATELY_FVAL);
  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_STATUS, IOCAL_DONE_D) == 0 );
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_BMSK)) | ((HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_NO_OP_FVAL) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_NOW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_USE_LAST_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_CAL_USE_LAST_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_AUTO_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG_IO_CAL_BANDGAP_DYN_CTRL_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_disable_auto_io_cal( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_CFG, IO_CAL_AUTO, 0);
}

#pragma arm section code


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC WRLVL sequences begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_wrlvl_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{
  uint32 dcc_auto;

  

  dcc_auto =  ! dcc_manual;
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, DCC_WRLVL_BYPASS, bypass);
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MODE, dcc_auto);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MODE, dcc_auto);
#endif  
  ///  Set override bit
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
#endif  

  if (bypass == 0) {
    if (dcc_auto == 1) {
      DDRPHY_CA_wrlvl_auto_dcc_calibration(_inst_,
			                                     dcc_wait_time,
			                                     div_ref);
    }
    else
    {
      /// seq_msg(INFO, 0, "Manual WRLVL DCC CAL Enabled ....");
      if (pll_sel & 0x1) {
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0]);
#ifdef SITE_PER_CA_CNT_2  
      	HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[1]);
#endif  
      }
  
      if (pll_sel & 0x2) {
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[0]);
#ifdef SITE_PER_CA_CNT_2  
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[1]);
#endif  
      }
    }
    if (dcc_sw_ovrd) {
      DDRPHY_CA_wrlvl_dcc_override(_inst_,
			                             pll_sel,
			                             dcc_adj_sw_ovrd,
			                             dcc_adj_sw_ovrd_pll2);
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wrlvl_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Auto WRLVL DCC CAL Enabled ....");
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[0]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[0]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, tmp );
#ifdef SITE_PER_CA_CNT_2  
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[1]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[1]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, tmp );
#endif  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_CONTROL_START, 1);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_CONTROL_START, 1);
#endif  
  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS, DCC_DONE) == 0 );
#ifdef SITE_PER_CA_CNT_2  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_STATUS, DCC_DONE) == 0 ); 	
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wrlvl_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{

  ///  Set override value for pll0
  if (pll_sel & 0x1) {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[1]);
#endif  
  }
  ///  Set override value for pll1
  if (pll_sel & 0x2) {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG, SITE0_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG, SITE1_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[1]);
#endif  
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wrlvl_dcc_stop( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_CONTROL_START, 0);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_CONTROL_START, 0);
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_CA_wrlvl_dcc_adj_status( uint32 _inst_, uint32 site_num )
{
  uint32 dcc_offset = 0x1F;


  switch(site_num){
    case 0 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_STATUS, DCC_OFFSET);
      break;
#ifdef SITE_PER_CA_CNT_2  
    case 1 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_STATUS, DCC_OFFSET);
      break;  
#endif  
  }
  
  return ((dcc_offset));
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC WR90 sequences begin here
/// -------------------------------------------------------------
///  Requirements:
///    1. The fixed frequency clock must be ON (19.2MHz or 27MHz).
///    2. The DDR clock must be at highest operation frequency.
///    3. The DCC_WR90s must be calibrated first, followed by
///       DCC_WRLVLs calibration if necessary. NOTE THIS IS OPPOSITE
/// 	 OF DQ.
// ***********************************************************************
void DDRPHY_CA_wr90_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{
  uint32 dcc_auto;

  
  dcc_auto =  ! dcc_manual;
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, DCC_WR90_BYPASS, bypass);
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MODE, dcc_auto);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MODE, dcc_auto);
#endif  
  /// Set override bit
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
#endif  
  
  if (bypass == 0) {
    if (dcc_auto == 1) {
      DDRPHY_CA_wr90_auto_dcc_calibration(_inst_,
			                                    dcc_wait_time,
			                                    div_ref);
    }
    else
    {
      /// seq_msg(INFO, 0, "Manual WRLVL DCC CAL Enabled ....");
      if (pll_sel & 0x1) {
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0]);
#ifdef SITE_PER_CA_CNT_2  
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[1]);
#endif  
      }
  
      if (pll_sel & 0x2) {
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[0]);
#ifdef SITE_PER_CA_CNT_2  
        HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[1]);
#endif  
      }
    }
    if (dcc_sw_ovrd) {
      DDRPHY_CA_wr90_dcc_override(_inst_,
			                            pll_sel,
			                            dcc_adj_sw_ovrd,
			                            dcc_adj_sw_ovrd_pll2);
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wr90_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Auto WRLVL DCC CAL Enabled ....");
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[0]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[0]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, tmp );
#ifdef SITE_PER_CA_CNT_2  
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[1]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[1]) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, tmp );
#endif  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_CONTROL_START, 1);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_CONTROL_START, 1);
#endif  
  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS, DCC_DONE) == 0 );
#ifdef SITE_PER_CA_CNT_2  
  while (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_STATUS, DCC_DONE) == 0 );
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wr90_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{

  /// Set override value for pll0
  if (pll_sel & 0x1) {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[1]);
#endif  
  }
  /// Set override value for pll1
  if (pll_sel & 0x2) {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG, SITE0_DCC_WR90_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG, SITE1_DCC_WR90_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[1]);
#endif  
    //HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_DCC_PLL2_SW_OVRD_CFG, SITE1_DCC_WR90_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[1]);
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_wr90_dcc_stop( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_CONTROL_START, 0);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_CONTROL_START, 0);
#endif  
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_CA_wr90_dcc_adj_status( uint32 _inst_, uint32 site_num )
{
  uint32 dcc_offset = 0x1F;


  switch(site_num){
    case 0 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_STATUS, DCC_OFFSET);
      break;
#ifdef SITE_PER_CA_CNT_2  
    case 1 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_STATUS, DCC_OFFSET);
      break;      
#endif  
  }
  
  return ((dcc_offset));
}


#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD ODT enable control
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_io_pads_odt_enable( uint32 _inst_, uint32 odt_ena )
{

  /// CA ODT enable bits by removing pull to no-pull
  ///     CA_ODT_ENA         28 
  ///     CA_ODT_MODE        26
  ///     CA_ODT           25:24
  ///     CA_PULL          23:22
  if (odt_ena) {
    /// seq_msg(INFO, 0, " No-Pull CA PADS when ODT enable");
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0, CA_PULL, HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_PULL_NO_PULL_FVAL);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1, CK_PULL_B, HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_PULL_B_NO_PULL_FVAL);
  }
  else
  {
    /// seq_msg(INFO, 0, " Pull-down CA PADS all the time");
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0, CA_PULL, HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_PULL_PULL_DOWN_FVAL);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1, CK_PULL_B, HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_PULL_B_NO_PULL_FVAL);
  }
}
#pragma arm section code

// ***********************************************************************
/// -------------------------------------------------------------
/// -------------------------------------------------------------
// ***********************************************************************
uint32 DDRPHY_CA_pncnt_status( uint32 _inst_, uint32 pncnt_val )
{
  uint32 pncnt_match = 0;


  if ((HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_STATUS, PCNT_HOLD) == pncnt_val) && (HWIO_INXF (_inst_, DDRPHY_CA_DDRPHY_CA_IOC_CTLR_STATUS, PCNT_HOLD) == pncnt_val)) {
    pncnt_match = 1;
  }
  
  return ((pncnt_match));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_new_pncnt_update( uint32 _inst_, uint32 pncnt_val )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0 );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_PSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_PSLEW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_NSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG0_CA_NSLEW_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG0, tmp );
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1 );
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_PSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_PSLEW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_NSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_CA_DDRPHY_CA_PAD_CFG1_CK_NSLEW_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG1, tmp );
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_enable_csd_rx( uint32 _inst_, int clk_mode )
{
  uint32 tmp;

  if (clk_mode == 2) {
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_CSD_RX_CFG, CSD_RX_ANA_PU, 1);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_CSD_RX_CFG, CSD_RX_ANA_PU, 1);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, CLK_CGC_SITE1_DP_DDR_EN, 1);
#endif    
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, CLK_CGC_SITE0_DP_DDR_EN, 1);
  }
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG );
#ifdef SITE_PER_CA_CNT_2  
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_SHFT ) ;
#endif    
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_SHFT ) ;
#ifdef SITE_PER_CA_CNT_2  
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_SHFT ) ;
#endif    
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_disable_csd_rx( uint32 _inst_ , int selfrefresh_mode)
{
  uint32 tmp;

  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_CSD_RX_CFG, CSD_RX_ANA_PU, 0);
#ifdef SITE_PER_CA_CNT_2  
  HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_CSD_RX_CFG, CSD_RX_ANA_PU, 0);
#endif    
  tmp = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG );

  if (selfrefresh_mode == 1) {
#ifdef SITE_PER_CA_CNT_2  
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_SHFT ) ;
#endif    
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_SHFT ) ;
  }
#ifdef SITE_PER_CA_CNT_2  
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_DDR_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_DDR_EN_SHFT ) ;
#endif    
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_DDR_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_DDR_EN_SHFT ) ;
#ifdef SITE_PER_CA_CNT_2  
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_SHFT ) ;
#endif    
  tmp = (tmp & ~(HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE_CK_DP_CFG, tmp );
}
#pragma arm section code

// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_ch0_misc_init_io_pads( uint32 _inst_, uint32 device_type )
{

  if (device_type){
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG3, 0x30000080);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG4, 0x30000080);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_CA_ch1_misc_init_io_pads( uint32 _inst_, uint32 device_type )
{

  if (device_type){
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG3, 0x40802000);
    HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_PAD_CFG4, 0x40802000);
  }
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
//Non-SCALe set
void DDRPHY_CA_dcc_set( uint32 _inst_, uint32 dcc_man_adj[2][2][2] )
{
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MODE, 0);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MODE, 0);
#endif  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MODE, 0);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MODE, 0);
#endif  
    // DCC WR90
//    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0][0][0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0][1][0]);
#endif  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][0][0]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][1][0]);
#endif  
    // DCC WRLVL
//    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0][0][1]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0][1][1]);
#endif  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][0][1]);
#ifdef SITE_PER_CA_CNT_2  
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][1][1]);
#endif  
}
#pragma arm section code

//Non-SCALe set half-cycle for neg wrlvl
void DDRPHY_CA_neg_wrlvl(uint32 _inst_)
{
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_TOP_CFG, FPM_CTRL_EN_NEG_WRLVL_HALF_T_DELAY, 1);
    HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_FPM_CFG2, NEG_WRLVL_HALF_T_DELAY_THRESHOLD,0xE);
}




// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_seq_set_initial_cdc_wr90_delay( uint32 _inst_, uint16 site, uint16 coarse_delay_value, uint16 fine_delay_value )
{
	uint16 coarse_fine_combined;

	coarse_fine_combined = coarse_delay_value << 3 | fine_delay_value;
	if (site == 0x0) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0, SITE0_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1, SITE0_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2, SITE0_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3, SITE0_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0, SITE1_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1, SITE1_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2, SITE1_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3, SITE1_CDC_WR90_DELAY_VALUE, coarse_fine_combined);
	}
#endif	
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_seq_set_initial_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site, uint16 deskew_delay_value )
{

	if (site == 0x0) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA7_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA6_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA5_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA4_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA3_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA2_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA1_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA0_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA14_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA13_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA12_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA11_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA10_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA9_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA8_DELAY_VALUE, deskew_delay_value);
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA7_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA6_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA5_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA4_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA3_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA2_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA1_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA0_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA14_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA13_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA12_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA11_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA10_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA9_DELAY_VALUE, deskew_delay_value);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA8_DELAY_VALUE, deskew_delay_value);
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_seq_set_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site, uint16 bitn, uint16 deskew_delay_value )
{

	if (site == 0x0) {
		if (bitn == 7) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA7_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 6) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA6_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 5) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA5_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 4) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA4_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 3) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA3_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 2) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA2_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 1) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA1_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 0) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0, CA0_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 14) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA14_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 13) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA13_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 12) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA12_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 11) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA11_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 10) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA10_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 9) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA9_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 8) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1, CA8_DELAY_VALUE, deskew_delay_value);
		}
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
		if (bitn == 7) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA7_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 6) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA6_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 5) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA5_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 4) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA4_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 3) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA3_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 2) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA2_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 1) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA1_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 0) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0, CA0_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 14) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA14_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 13) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA13_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 12) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA12_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 11) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA11_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 10) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA10_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 9) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA9_DELAY_VALUE, deskew_delay_value);
		}
		if (bitn == 8) {
			HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1, CA8_DELAY_VALUE, deskew_delay_value);
		}
	}
#endif
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_seq_restore_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site,  uint32 deskew_delay_value[2] )
{

	if (site == 0x0) {
			HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0,  deskew_delay_value[0]);
			HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1,  deskew_delay_value[1]);
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
			HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0,  deskew_delay_value[0]);
			HWIO_OUTX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1,  deskew_delay_value[1]);
	}
#endif
}
// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_seq_read_cdc_perbit_deskew_delay( uint32 _inst_, uint16 site,  uint32 deskew_delay_value[2] )
{

	if (site == 0x0) {
			deskew_delay_value[0] = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW0);
			deskew_delay_value[1] = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE0_WR_DESKEW1);
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
			deskew_delay_value[0] = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW0);
			deskew_delay_value[1] = HWIO_INX (_inst_, DDRPHY_CA_DDRPHY_CA_SITE1_WR_DESKEW1);
	}
#endif
}

// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_ca_training_cdc_wr90_delay_select( uint32 _inst_ ,uint16 sel)
{

	HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_DELAY_SEL, CDC_WR90_DELAY_SEL, sel);		// Select CDC_DELAY0
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_CA_set_ca_cdc_wr90( uint32 _inst_, uint16 site, uint16 ca_cdc_wr90_delay[4] )
{

	if (site == 0x0) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0, SITE0_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[0]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1, SITE0_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[1]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2, SITE0_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[2]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3, SITE0_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[3]);
	}
#ifdef SITE_PER_CA_CNT_2
	if (site == 0x1) {
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY0, SITE1_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[0]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY1, SITE1_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[1]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY2, SITE1_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[2]);
		HWIO_OUTXF (_inst_, DDRPHY_CA_DDRPHY_CA_CDC_WR90_DELAY3, SITE1_CDC_WR90_DELAY_VALUE, ca_cdc_wr90_delay[3]);
	}
#endif
}


