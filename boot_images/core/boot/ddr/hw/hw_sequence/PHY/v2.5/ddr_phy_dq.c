
#include "ddr_phy_dq.h"

#ifndef __SIMXL_SC_TB__
#define vv_msg_FILE_NUMBER 0
#endif

#pragma arm section rwdata = "BOOT_RPM_RAM_DATA_ZONE"
uint32 dq_pad_cfg0_lp_rec ;
uint32 dq_pad_cfg1_lp_rec ;
uint32 dq_pad_cfg2_lp_rec ;
uint32 dq_pad_cfg3_lp_rec ;

uint32 dq_pad_cfg0_pc_rec;
uint32 dq_pad_cfg1_pc_rec;
uint32 dq_pad_cfg2_pc_rec;
uint32 dq_pad_cfg3_pc_rec;

#pragma arm section rwdata

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
///  IO PAD INIT sequence begin here
// ***********************************************************************
void DDRPHY_DQ_init_io_pads( uint32 _inst_, uint32 device_type )
{

  if (device_type) {
#if !MSM8936  
    ///  DQ PCDDR3
    /// seq_msg(INFO, 0, "Initializing DQ PADS for PCDDR3");
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, 0x7E200001);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, dq_pad_cfg0_pc_rec ); //0x350FF170);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, dq_pad_cfg1_pc_rec ); //0x1D0F00F0);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG2, dq_pad_cfg2_pc_rec ); //0x10010000);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG3, dq_pad_cfg3_pc_rec ); //0x0);
#endif
  }
  else
  {
    ///  DQ LPDDR3
    /// seq_msg(INFO, 0, "Initializing DQ PADS for LPDDR3");
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, 0x7A200000);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, dq_pad_cfg0_lp_rec ); //0x270FF150);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, dq_pad_cfg1_lp_rec ); //0x170F00D0);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG2, dq_pad_cfg2_lp_rec ); //0x10010000);
    HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG3, dq_pad_cfg3_lp_rec ); //0x0);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------
///  Set 1x vs 2x(testmode) clocking 
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_site_clk_mode( uint32 _inst_, int clk_mode )
{

  ///  Clock mode
  ///                                             |Reset | Static Legacy | Dynamic Legacy | SRPLL Mode   
  ///  DDRPHY clock                               | 1x   | 2x            | 1x             | 1x         
  ///  DDRPHY_[CA/DQ]_SITE_CK_DP[MODE_1X])        | b'1' | b'0'          | b'1'           | Don't Care 
  ///  DDRPHY_[CA/DQ]_SITE_CK_DP[CLK_MODE_STATIC] | b'0' | b'1'          | b'0'           | b'0'       
  if (clk_mode == 1) {
    ///  static_legacy
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, MODE_1X, 0);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, CLK_MODE_STATIC, 1);
  }
  else
  {
    ///  dynamic_legacy and srpll_mode
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, MODE_1X, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, CLK_MODE_STATIC, 0);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------
///  CDC INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_init_cdc( uint32 _inst_ )
{

  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, 0x1818);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1, 0x2020);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2, 0x3030);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3, 0x4040);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY0, 0x1818);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY1, 0x2020);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY2, 0x3030);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY3, 0x4040);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY0, 0x30);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY1, 0x40);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY2, 0x60);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD180_DELAY3, 0x78);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_LP_DELAY, 0x0311);
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DQ CSR INIT sequence begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_init_dq_csr( uint32 _inst_ )
{

  /// Write-leveling
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, 0x09100100);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, 0x0A100100);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, 0x0C100100);
  /// MIsc 
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_RD_DESKEW_DM, 0x0);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_DQ_IOC_SLV_CFG, 0x80000000);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_DQS_IOC_SLV_CFG, 0x80000000);
  /// FPM CONFIG                                                         
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_FPM_CFG0, 0xC02C864);
  //HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_FPM_CFG1, 0x1800028); // SVS @ 250MHz
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_FPM_CFG1, 0x18000FF); // SVS @ <50MHz
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_FPM_CFG2, 0xC01900);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_FPM_CFG3, 0x13041232);
  //  WRLVL_DCC_MAN_ADJ_PLL2  set
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, 0x10);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, 0x10);
  //  WR90_DCC_MAN_ADJ_PLL2  set
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, 0x10);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, 0x10);

  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE0_CSD_RX_CFG, 0x80000100);
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE1_CSD_RX_CFG, 0x80000100);  
}
#pragma arm section code


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC WRLVL sequences begin here
/// -------------------------------------------------------------
///  Requirements:
///    1. The fixed frequency clock must be ON (19.2MHz or 27MHz).
///    2. The DDR clock must be at highest operation frequency.
///    3. The DCC_WRLVLs must be calibrated first, followed by
///       DCC_WR90s calibration if necessary.
// ***********************************************************************
void DDRPHY_DQ_wrlvl_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{
  uint32 dcc_auto;

  

  dcc_auto =  ! dcc_manual;
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, DCC_WRLVL_BYPASS, bypass);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MODE, dcc_auto);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MODE, dcc_auto);
  ///  Set override bit
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
  
  if (bypass == 0) {
    if (dcc_auto == 1) {
      DDRPHY_DQ_wrlvl_auto_dcc_calibration(_inst_,
			                                     dcc_wait_time,
			                                     div_ref);
    }
    else
    {
      /// seq_msg(INFO, 0, "Manual WRLVL DCC CAL Enabled ....");
      if (pll_sel & 0x1) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0]);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[1]);
      }
  
      if (pll_sel & 0x2) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[0]);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[1]);
      }
    }
    if (dcc_sw_ovrd) {
      DDRPHY_DQ_wrlvl_dcc_override(_inst_,
			                             pll_sel,
			                             dcc_adj_sw_ovrd,
			                             dcc_adj_sw_ovrd_pll2);
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Auto WRLVL DCC CAL Enabled ....");
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[0]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[0]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, tmp );
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[1]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[1]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, tmp );
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_CONTROL_START, 1);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_CONTROL_START, 1);
  
  while (HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS, DCC_DONE) == 0 );
  while (HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS, DCC_DONE) == 0 );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{

  if (pll_sel & 0x1) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[1]);
  }
  ///  Set override value for pll1
  if (pll_sel & 0x2) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG, BYTE0_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG, BYTE1_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[1]);
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_dcc_stop( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_CONTROL_START, 0);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_CONTROL_START, 0);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_wrlvl_dcc_adj_status( uint32 _inst_, uint32 byte_num )
{
  uint32 dcc_offset = 0x1F;


  switch(byte_num){
    case 0 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_STATUS, DCC_OFFSET);
      break;
    case 1 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_STATUS, DCC_OFFSET);
      break;
  }
  
  return ((dcc_offset));
}


// ***********************************************************************
/// -------------------------------------------------------------
///  DCC WR90 sequences begin here
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_wr90_dcc_cal( uint32 _inst_, uint32 bypass, uint32 dcc_manual, uint32 dcc_sw_ovrd, uint32 pll_sel, uint32 dcc_wait_time[2], uint32 div_ref[2], uint32 dcc_man_adj[2], uint32 dcc_adj_sw_ovrd[2], uint32 dcc_man_adj_pll2[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{
  uint32 dcc_auto;

  

  dcc_auto =  ! dcc_manual;
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, DCC_WR90_BYPASS, bypass);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MODE, dcc_auto);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MODE, dcc_auto);
  ///  Set override bit
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_ADJ_SW_OVRD, dcc_sw_ovrd);
  
  if (bypass == 0) {
    if (dcc_auto == 1) {
      DDRPHY_DQ_wr90_auto_dcc_calibration(_inst_,
			                                    dcc_wait_time,
			                                    div_ref);
    }
    else
    {
      /// seq_msg(INFO, 0, "Manual WRLVL DCC CAL Enabled ....");
      if (pll_sel & 0x1) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0]);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[1]);
      }
  
      if (pll_sel & 0x2) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[0]);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj_pll2[1]);
      }
    }
    if (dcc_sw_ovrd) {
      DDRPHY_DQ_wr90_dcc_override(_inst_,
			                            pll_sel,
			                            dcc_adj_sw_ovrd,
			                            dcc_adj_sw_ovrd_pll2);
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wr90_auto_dcc_calibration( uint32 _inst_, uint32 dcc_wait_time[2], uint32 div_ref[2] )
{
  uint32 tmp;

  /// seq_msg(INFO, 0, "Auto WRLVL DCC CAL Enabled ....");
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[0]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[0]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, tmp );
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_DCC_WAIT_TIME_BMSK)) | ((dcc_wait_time[1]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_DCC_WAIT_TIME_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_DCC_DIV_REF_BMSK)) | ((div_ref[1]) << HWIO_DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG_DCC_DIV_REF_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, tmp );
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_CONTROL_START, 1);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_CONTROL_START, 1);
  
  while (HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS, DCC_DONE) == 0 );
  while (HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS, DCC_DONE) == 0 );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wr90_dcc_override( uint32 _inst_, uint32 pll_sel, uint32 dcc_adj_sw_ovrd[2], uint32 dcc_adj_sw_ovrd_pll2[2] )
{

  if (pll_sel & 0x1) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_ADJ_SW_VAL, dcc_adj_sw_ovrd[1]);
  }
  ///  Set override value for pll1
  if (pll_sel & 0x2) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG, BYTE0_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_DCC_PLL2_SW_OVRD_CFG, BYTE1_DCC_WRLVL_ADJ_PLL2_SW_VAL, dcc_adj_sw_ovrd_pll2[1]);
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wr90_dcc_stop( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_CONTROL_START, 0);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_CONTROL_START, 0);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_wr90_dcc_adj_status( uint32 _inst_, uint32 byte_num )
{
  uint32 dcc_offset = 0x1F;


  switch(byte_num){
    case 0 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_STATUS, DCC_OFFSET);
      break;
    case 1 :
      dcc_offset = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_STATUS, DCC_OFFSET);
      break;
  }
  
  return ((dcc_offset));
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// -------------------------------------------------------------
///  IO PAD ODT enable control
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_io_pads_odt_enable( uint32 _inst_, uint32 dynamic_odt_ena )
{

  /// DQ ODT enable bits by allowing FPM to control odt dynamically
  ///     FPM_CTRL_EN_ODT  31
  if (dynamic_odt_ena) {
    /// seq_msg(INFO, 0, "FPM  ODT enable");
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, FPM_CTRL_EN_ODT, HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_FROM_FREQUENCY_POWER_MANAGEMENT_FVAL);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_ODT_ENA,  1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_ODT_ENA, 1);
  }
  else
  {
    /// seq_msg(INFO, 0, " SW ODT enable");
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, FPM_CTRL_EN_ODT, HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_FROM_CSR_FVAL);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, DQ_ODT_ENA,  0);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, DQS_ODT_ENA, 0);
  }
}
#pragma arm section code

// ***********************************************************************
/// -------------------------------------------------------------
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_new_pncnt_update( uint32 _inst_, uint32 pncnt_val )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0 );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_DQ_PSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_DQ_PSLEW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_DQ_NSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0_DQ_NSLEW_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG0, tmp );
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1 );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_DQS_PSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_DQS_PSLEW_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_DQS_NSLEW_BMSK)) | ((pncnt_val) << HWIO_DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1_DQS_NSLEW_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_PAD_CFG1, tmp );
}

#pragma arm section code = "BOOT_RPM_RAM_CODE_ZONE"
// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_enable_csd_rx( uint32 _inst_, int clk_mode )
{
  uint32 tmp;

  if (clk_mode == 2) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE0_CSD_RX_CFG, CSD_RX_ANA_PU, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE1_CSD_RX_CFG, CSD_RX_ANA_PU, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, CLK_CGC_SITE1_DP_DDR_EN, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, CLK_CGC_SITE0_DP_DDR_EN, 1);
  }
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_BMSK)) | ((1) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_disable_csd_rx( uint32 _inst_, int selfrefresh_mode )
{
  uint32 tmp;

  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE0_CSD_RX_CFG, CSD_RX_ANA_PU, 0);
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE1_CSD_RX_CFG, CSD_RX_ANA_PU, 0);
  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG );
 
  if (selfrefresh_mode == 1) {
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE1_CK_MC_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CDIV_SITE0_CK_MC_EN_SHFT ) ;
  }
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_DDR_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_DDR_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_DDR_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_DDR_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE1_DP_MC_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG_CLK_CGC_SITE0_DP_MC_EN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_SITE_CK_DP_CFG, tmp );
}


//Non-SCALe set
void DDRPHY_DQ_dcc_set( uint32 _inst_, uint32 dcc_man_adj[2][2][2] )
{
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MODE, 0);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MODE, 0);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MODE, 0);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MODE, 0);
    // DCC WR90
//    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0][0][0]);
//    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MAN_ADJ, dcc_man_adj[0][1][0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][0][0]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WR90_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][1][0]);
    // DCC WRLVL
//    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0][0][1]);
//    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MAN_ADJ, dcc_man_adj[0][1][1]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][0][1]);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_DCC_WRLVL_CFG, DCC_MAN_ADJ_PLL2, dcc_man_adj[1][1][1]);
}
#pragma arm section code 































// ***********************************************************************
///  REN Functions
///  These REN functions support RCW trainning initiated by BIMC.
///  For each byte, the REN fields in  DRPHY_DQ_REN_CFG6 are configured as followed:
///        ren_cfg[9]= 0: 0 PHY delay; 1: T/2 PHY delay
///        ren_cfg[8]= 0: 0 delay; 1: ~320 ps delay (OR'ed with ren_cfg[5])
///        ren_cfg[6:3]= CDC Coarse delay when ren_cfg[8]=0
///        ren_cfg[2:0]= CDC Fine delay
/// 
///  The functions provided below support the following:
///    1. SW override to control REN via CSR
///    2. Enable FPM control.
///    3. Update coarse_delay field in DRPHY_DQ_REN_CFG6 for either byte
///    4. Update fine_delay field in DRPHY_DQ_REN_CFG6 for either byte
///    5. Update half cycle field in DRPHY_DQ_REN_CFG6 for either byte
///    6. Force a reset to REN logic.
// ***********************************************************************
void DDRPHY_DQ_ren_sw_mode( uint32 _inst_, uint32 ren_cfg_num )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_LEGACY_MODE_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_REN_GATING_ENABLED_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_FROM_CSR_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, tmp );
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD, ren_cfg_num);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_ren_hw_mode( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_LEGACY_MODE_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_REN_GATING_ENABLED_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_FROM_FPM_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, tmp );
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD, 0);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_ren_done( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_LEGACY_MODE_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_REN_IO_MODE_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_REN_GATING_ENABLED_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_RCW_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_FROM_FPM_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_REN_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, tmp );
  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, FPM_REN_SEL_OVRD, 0);
}


// ***********************************************************************
///  Function: provides coarse CDC delay per byte
///   byte_coarse_delay[3:0]: <value> ~79 ps increments
///  Function: provides fine CDC delay per byte
///   byte_fine_delay[2:0]: <value> ~15 ps increments
// ***********************************************************************
void DDRPHY_DQ_ren_sw_fine_update( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_fine_delay, uint32 byte1_fine_delay )
{
  uint32 tmp_byte0;
  uint32 tmp_byte1;
  uint32 tmp_byte0_fine;
  uint32 tmp_byte1_fine;
  uint32 cfg_byte0;
  uint32 cfg_byte1;


  tmp_byte0_fine = byte0_fine_delay & ( ~ FINE_MASK);
  tmp_byte1_fine = byte1_fine_delay & ( ~ FINE_MASK);
  cfg_byte0 = DDRPHY_DQ_read_ren_byte0_cfg(_inst_ , ren_cfg_num);
  cfg_byte1 = DDRPHY_DQ_read_ren_byte1_cfg(_inst_ , ren_cfg_num);
  tmp_byte0 = (cfg_byte0) & FINE_MASK;
  tmp_byte1 = (cfg_byte1) & FINE_MASK;
  tmp_byte0 |= (tmp_byte0_fine);
  tmp_byte1 |= (tmp_byte1_fine);
  
  DDRPHY_DQ_write_ren_bytes_cfg(_inst_,
	                              ren_cfg_num,
	                              tmp_byte0,
	                              tmp_byte1);
}


// ***********************************************************************
///  Function: provides T/2 CDC path delay per byte
///   byte_hcycle: 0 PHY delay; 1: T/2 PHY delay
///  Function: provides T/2, coarse, or fine CDC delay per byte
///   byte_hcycle: 0 PHY delay; 1: T/2 PHY delay
///   byte_coarse_delay[3:0]: <value> ~79 ps increments
///   byte_fine_delay[2:0]: <value> ~15 ps increments
// ***********************************************************************
void DDRPHY_DQ_ren_sw_update( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_delay, uint32 byte1_delay )
{

  DDRPHY_DQ_write_ren_bytes_cfg(_inst_,
	                              ren_cfg_num,
	                              byte0_delay,
	                              byte1_delay);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_write_ren_bytes_cfg( uint32 _inst_, uint32 ren_cfg_num, uint32 byte0_delay, uint32 byte1_delay )
{

  switch(ren_cfg_num){
    case 0 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_CFG, byte1_delay);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE1_REN_CFG, byte1_delay);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE1_REN_CFG, byte1_delay);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE1_REN_CFG, byte1_delay);
      break;
    case 4 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE1_REN_CFG, byte1_delay);
      break;
    case 5 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE1_REN_CFG, byte1_delay);
      break;
    case 6 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE1_REN_CFG, byte1_delay);
      break;
    default :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE0_REN_CFG, byte0_delay);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE1_REN_CFG, byte1_delay);
      break;
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_read_ren_byte0_cfg( uint32 _inst_, uint32 ren_cfg_num )
{
  uint32 byte0_cfg;


  switch(ren_cfg_num){
    case 0 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_CFG);
      break;
    case 1 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE0_REN_CFG);
      break;
    case 2 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE0_REN_CFG);
      break;
    case 3 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE0_REN_CFG);
      break;
    case 4 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE0_REN_CFG);
      break;
    case 5 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE0_REN_CFG);
      break;
    case 6 :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE0_REN_CFG);
      break;
    default :
      byte0_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE0_REN_CFG);
      break;
  }
  return ((byte0_cfg));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_read_ren_byte1_cfg( uint32 _inst_, uint32 ren_cfg_num )
{
  uint32 byte1_cfg;


  switch(ren_cfg_num){
    case 0 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_CFG);
      break;
    case 1 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE1_REN_CFG);
      break;
    case 2 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE1_REN_CFG);
      break;
    case 3 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE1_REN_CFG);
      break;
    case 4 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE1_REN_CFG);
      break;
    case 5 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE1_REN_CFG);
      break;
    case 6 :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE1_REN_CFG);
      break;
    default :
      byte1_cfg = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE1_REN_CFG);
      break;
  }
  return ((byte1_cfg));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_ren_reset( uint32 _inst_, uint32 byte0_reset, uint32 byte1_reset )
{

  ///  Reset Byte0
  if (byte0_reset) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_RESET, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_RESET, 0);
  }
  ///  Reset Byte1
  if (byte1_reset) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_RESET, 1);
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_RESET, 0);
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_ren_cfg_byte0_sel( uint32 _inst_, uint32 cfg_sel, uint32 value )
{

  if (cfg_sel == 0) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_CFG, value);
  }
  else
  {
    if (cfg_sel == 1) {
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE0_REN_CFG, value);
    }
    else
    {
      if (cfg_sel == 2) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE0_REN_CFG, value);
      }
      else
      {
        if (cfg_sel == 3) {
          HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE0_REN_CFG, value);
        }
        else
        {
          if (cfg_sel == 4) {
            HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE0_REN_CFG, value);
          }
          else
          {
            if (cfg_sel == 5) {
              HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE0_REN_CFG, value);
            }
            else
            {
              if (cfg_sel == 6) {
                HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE0_REN_CFG, value);
              }
              else
              {
                HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE0_REN_CFG, value);
              }
            }
          }
        }
      }
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_ren_cfg_byte1_sel( uint32 _inst_, uint32 cfg_sel, uint32 value )
{

  if (cfg_sel == 0) {
    HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_CFG, value);
  }
  else
  {
    if (cfg_sel == 1) {
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG1, BYTE1_REN_CFG, value);
    }
    else
    {
      if (cfg_sel == 2) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG2, BYTE1_REN_CFG, value);
      }
      else
      {
        if (cfg_sel == 3) {
          HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG3, BYTE1_REN_CFG, value);
        }
        else
        {
          if (cfg_sel == 4) {
            HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG4, BYTE1_REN_CFG, value);
          }
          else
          {
            if (cfg_sel == 5) {
              HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG5, BYTE1_REN_CFG, value);
            }
            else
            {
              if (cfg_sel == 6) {
                HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG6, BYTE1_REN_CFG, value);
              }
              else
              {
                HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_REN_CFG0, BYTE1_REN_CFG, value);
              }
            }
          }
        }
      }
    }
  }
}
















































// ***********************************************************************
///  sequences which write to wrlv_cfg0...6 fields.
// ***********************************************************************
void DDRPHY_DQ_enable_wrlvl_mode( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_WR_LEVELING_MODE_BMSK)) | ((1) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_WR_LEVELING_MODE_SHFT ) ;
//  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_FROM_FREQUENCY_POWER_MANAGEMENT_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_disable_wrlvl_mode( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG );
  tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_WR_LEVELING_MODE_BMSK)) | ((0) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_WR_LEVELING_MODE_SHFT ) ;
  //tmp = (tmp & ~(HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_BMSK)) | ((HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_FROM_FREQUENCY_POWER_MANAGEMENT_FVAL) << HWIO_DDRPHY_DQ_DDRPHY_DQ_TOP_CFG_FPM_CTRL_EN_ODT_SHFT ) ;
  HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, tmp );
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_set_wrlvl_delay( uint32 _inst_, uint32 delay_index, uint32 wrlvl_delay_val )
{

  switch(delay_index){
    case 0 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 4 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 5 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    case 6 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
    default :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, wrlvl_delay_val);
      break;
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_set_wrlvl_cdc_retimer( uint32 _inst_, uint32 cfg_index, uint32 byte0_retimer_sel_LUT_value, uint32 byte0_retimer_ie_oe_LUT_value, uint32 byte0_retimer_cycle_delay_sel, uint32 byte1_retimer_sel_LUT_value, uint32 byte1_retimer_ie_oe_LUT_value, uint32 byte1_retimer_cycle_delay_sel )
{

  switch(cfg_index){
    case 0 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 4 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 5 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    case 6 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
    default :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, byte0_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, byte0_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, byte0_retimer_cycle_delay_sel);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, byte1_retimer_sel_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, byte1_retimer_ie_oe_LUT_value);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, byte1_retimer_cycle_delay_sel);
      break;
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_set_wrlvl_cdc_retimer_value( uint32 _inst_, uint32 cfg_index, uint32 byte_index, uint32 retimer_sel_LUT_value, uint32 retimer_ie_oe_LUT_value, uint32 retimer_cycle_delay_sel )
{

  switch(cfg_index){
    case 0 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 1 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 2 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 3 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 4 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 5 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    case 6 :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
    default :
      if (byte_index == 0) {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      else
      {
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
      }
      break;
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_set_cdc_wrlvl_delay( uint32 _inst_, uint32 byte_index, uint32 delay_index, uint32 delay_value )
{

  if (byte_index == 0) {
    switch(delay_index){
      case 0 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 1 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 2 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 3 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 4 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 5 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 6 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      default :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
    }
  }
  else
  {
    switch(delay_index){
      case 0 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 1 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 2 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 3 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 4 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 5 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 6 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      default :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
    }
  }
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_cdc_wr90_delay( uint32 _inst_, uint32 delay_index, uint32 delay_val )
{

  switch(delay_index){
    case 0 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, BYTE0_CDC_WR90_DELAY_VALUE, delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, BYTE1_CDC_WR90_DELAY_VALUE, delay_val);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1, BYTE0_CDC_WR90_DELAY_VALUE, delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1, BYTE1_CDC_WR90_DELAY_VALUE, delay_val);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2, BYTE0_CDC_WR90_DELAY_VALUE, delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2, BYTE1_CDC_WR90_DELAY_VALUE, delay_val);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3, BYTE0_CDC_WR90_DELAY_VALUE, delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3, BYTE1_CDC_WR90_DELAY_VALUE, delay_val);
      break;
    default :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, BYTE0_CDC_WR90_DELAY_VALUE, delay_val);
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, BYTE1_CDC_WR90_DELAY_VALUE, delay_val);
      break;
  }
}


// ***********************************************************************
///  WRLVL_TRIGGER FUCNTIONS
// ***********************************************************************
void DDRPHY_DQ_wrlvl_trigger_pulse( uint32 _inst_ )
{

  DDRPHY_DQ_wrlvl_trigger_zero(_inst_);
  DDRPHY_DQ_wrlvl_trigger_one(_inst_);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_trigger_zero( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_TRIGGER, SEND_PULSE, 0);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_trigger_one( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_TRIGGER, SEND_PULSE, 1);
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_read_wrlvl_status( uint32 _inst_ )
{
  uint32 wrlv_status_bit;


  wrlv_status_bit = HWIO_INX (_inst_, DDRPHY_DQ_DDRPHY_DQ_TRAINING_STATUS );
  return ((wrlv_status_bit));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint32 DDRPHY_DQ_read_wrlvl_delay( uint32 _inst_, uint32 byte_index, uint32 cfg_index )
{
  uint32 wrlvl_delay;


  if (byte_index == 0) {
    switch(cfg_index){
      case 0 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 1 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 2 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 3 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 4 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 5 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      case 6 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
      default :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE);
        break;
    }
  }
  else
  {
    switch(cfg_index){
      case 0 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 1 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 2 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 3 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 4 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 5 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      case 6 :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
      default :
        wrlvl_delay = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE);
        break;
    }
  }
  return ((wrlvl_delay));
}


// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
void DDRPHY_DQ_wrlvl_freq_cfg( uint32 _inst_, uint32 freq_index, uint32 freq_val )
{

  switch(freq_index){
    case 0 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD, freq_val);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, DDR_CLK_PERIOD, freq_val);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, DDR_CLK_PERIOD, freq_val);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, DDR_CLK_PERIOD, freq_val);
      break;
    case 4 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, DDR_CLK_PERIOD, freq_val);
      break;
    case 5 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, DDR_CLK_PERIOD, freq_val);
      break;
    case 6 :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, DDR_CLK_PERIOD, freq_val);
      break;
    default :
      HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, DDR_CLK_PERIOD, freq_val);
      break;
  }
}


// NON_SCALE: Need to generate from scale!!!!
void DDRPHY_DQ_set_cdc_wrlvl_training( uint32 _inst_, uint32 byte_index, uint32 delay_index, uint16 retimer_value, uint16 delay_value )
{
uint32 retimer_sel_LUT_value = (retimer_value) & 0x3;
uint32 retimer_ie_oe_LUT_value = (retimer_value>>2)& 0x1;
uint32 retimer_cycle_delay_sel = (retimer_value>>3)& 0x1;

  if (byte_index == 0) {
    switch(delay_index){
      case 0 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 1 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 2 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 3 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 4 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 5 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 6 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      default :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE0_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
    }
  }
  else
  {
    switch(delay_index){
      case 0 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 1 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG1, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 2 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG2, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 3 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG3, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 4 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG4, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 5 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG5, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      case 6 :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG6, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
      default :
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_DDR, retimer_sel_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_SEL_OE_IE, retimer_ie_oe_LUT_value);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_RETIMER_CYCLE_DELAY_SEL, retimer_cycle_delay_sel);
        HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_WRLVL_CFG0, BYTE1_CDC_WRLVL_DELAY_VALUE, delay_value);
        break;
    }
  }
}









































void DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine(uint32 _inst_, uint16 coarse, uint16 fine )
{
	uint32 tmp1,tmp2;
    tmp1= (coarse<<3)+fine;
	tmp2=(tmp1<<8)+tmp1; 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY0,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY1,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY2,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY3,tmp2 );
}

void DDRPHY_DQ_set_dq_cdc_rd90_value( uint32 _inst_, uint16 dq_write[2][4]  )
{
uint32 tmp;
    tmp = (dq_write[1][0]<< 8) | (dq_write[0][0]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY0, tmp );
    tmp = (dq_write[1][1]<< 8) | (dq_write[0][1]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY1, tmp );
    tmp = (dq_write[1][2]<< 8) | (dq_write[0][2]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY2, tmp );
    tmp = (dq_write[1][3]<< 8) | (dq_write[0][3]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_RD90_DELAY3, tmp );
}

void DDRPHY_DQ_set_rd_deskew_value( uint32 _inst_, uint32 dq_read_perbit[2][2], uint32 dq_read_perbit_dm )
{
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_EVEN, dq_read_perbit[0][0]);
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_RD_DESKEW_DQ_ODD , dq_read_perbit[0][1]);
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_EVEN, dq_read_perbit[1][0]);
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_RD_DESKEW_DQ_ODD , dq_read_perbit[1][1]);
	
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_RD_DESKEW_DM , dq_read_perbit_dm);

}






























void DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine(uint32 _inst_, uint16 coarse, uint16 fine )
{
	uint32 tmp1,tmp2;
    tmp1= (coarse<<3)+fine;
	tmp2=(tmp1<<8)+tmp1; 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2,tmp2 );
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3,tmp2 );
}

void DDRPHY_DQ_set_dq_cdc_wr90_value( uint32 _inst_, uint16 dq_write[2][4]  )
{
uint32 tmp;
    tmp = (dq_write[1][0]<< 8) | (dq_write[0][0]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY0, tmp );
    tmp = (dq_write[1][1]<< 8) | (dq_write[0][1]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY1, tmp );
    tmp = (dq_write[1][2]<< 8) | (dq_write[0][2]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY2, tmp );
    tmp = (dq_write[1][3]<< 8) | (dq_write[0][3]); 
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_CDC_WR90_DELAY3, tmp );
}

void DDRPHY_DQ_set_wr_deskew_value( uint32 _inst_, uint32 dq_write_perbit[2], uint32 dq_write_perbit_dm )
{
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE0_WR_DESKEW_DQ, dq_write_perbit[0]);
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_BYTE1_WR_DESKEW_DQ, dq_write_perbit[1]);
	HWIO_OUTX (_inst_, DDRPHY_DQ_DDRPHY_DQ_WR_DESKEW_DM , dq_write_perbit_dm);
}






























// ***********************************************************************
/// -------------------------------------------------------------
///  function:  set_dq_for_ca_training
/// -------------------------------------------------------------
// ***********************************************************************
void DDRPHY_DQ_set_exit_dq_for_ca_training( uint32 _inst_, uint16 mode )
{

	HWIO_OUTXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TOP_CFG, CA_TRAINING_MODE, mode);
}


// ***********************************************************************
/// -------------------------------------------------------------
///  function read_dq_training_status_for_ca_training
/// -------------------------------------------------------------
// ***********************************************************************
uint16 DDRPHY_DQ_read_dq_training_status_for_ca_training( uint32 _inst_ )
{
	uint16 read_data;


	read_data = HWIO_INXF (_inst_, DDRPHY_DQ_DDRPHY_DQ_TRAINING_STATUS, TRAINING_STATUS);
	return (read_data);
}





