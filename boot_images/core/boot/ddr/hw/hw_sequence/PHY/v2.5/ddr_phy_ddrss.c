//uncomment the below to enable ddr training logs
#if 0
 
#define ENABLE_PRINTF

#define ENABLE_PRINTF_RD_DBG   //after training DQ read taining report
#define ENABLE_PRINTF_WR_DBG   //after training DQ write report
#define ENABLE_PRINTF_CA_DBG   //after training CA taining report

#include <string.h> 
#include "ddr_log.h"
static char logbuf[DDR_LOG_BUF_LEN_MAX];

#endif

#include "ddrss_rcw.h"
#include "bimc_scale.h"
#include "bimc_struct.h"
//#include "ddr_params.h"
#include "ddr_phy_ddrss.h"




// ca_jcpll_dcc_training_g[chnl][pll_sel][site][wrlvl/wr90]
uint32 extern ca_jcpll_dcc_training_g[2][2][2][2];

// dq_jcpll_dcc_training_g[chnl][dq][pll_sel][byte][wrlvl/wr90]
uint32 extern dq_jcpll_dcc_training_g[2][4][2][2][2];

// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
// which_jcpll by 
//  if(HWIO_INF(CH0_JCPLL_STATUS,ACTIVE_JCPLL)==0) {...}
//
void DDRSS_dcc_cal( uint32 _inst_, uint32 chnl, uint32 ca_dcc_training[2][2][2][2], uint32 dq_dcc_training[2][4][2][2][2])
{
  int i,j,k,l,m;
  uint32 which_jcpll = 0;

  /// Detect which_jcpll
  if (chnl == 0) {
    which_jcpll = HWIO_INXF (_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DDR_CC_OFFSET, DDR_CC_JCPLL_STATUS, ACTIVE_JCPLL);
  }
#ifdef CHNL_CNT_2  
  if (chnl == 1) {
    which_jcpll = HWIO_INXF (_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DDR_CC_OFFSET, DDR_CC_JCPLL_STATUS, ACTIVE_JCPLL);
  }
#endif

  ///  Turn-on PHY clocks in debug mode
  BIMC_Enable_CLK_To_PHY (_inst_ + SEQ_DDRSS_BIMC_OFFSET, (SDRAM_INTERFACE) chnl );
  
  ///  Perform DCC calibration for first jcpll
  EBI1_PHY_CFG_dcc_init(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
	                      chnl,
	                      which_jcpll);
  ///  Turn-off PHY clocks in debug mode
  BIMC_Disable_CLK_To_PHY (_inst_ + SEQ_DDRSS_BIMC_OFFSET, (SDRAM_INTERFACE) chnl );
  
  //Store CA values
  for (i = 0; i< CHNL_CNT; i++){		// chnl
    for (j = 0; j< JCPLL_CNT;j++){		// pll_sel
	for (k = 0; k< SITE_PER_CA_CNT; k++){	// site
	  for (l = 0; l< DCC_CNT; l++){		// 0:wr90; 1:wrlvl
	    ca_dcc_training[i][j][k][l] = ca_jcpll_dcc_training_g[i][j][k][l];
	  }
	}
      }
    }

  //Store DQ values
  for (i = 0; i< CHNL_CNT; i++){		// chnl
    for (j = 0; j< DQ_CNT;j++){		        // DQ
      for (k = 0; k< JCPLL_CNT; k++){		// pll_sel
	for (l = 0; l< BYTE_PER_DQ_CNT; l++){	// byte
	  for (m = 0; m< DCC_CNT; m++){		// 0:wr90; 1:wrlvl
	    dq_dcc_training[i][j][k][l][m] = dq_jcpll_dcc_training_g[i][j][k][l][m];
	  }
	}
      }
    }
  }

}

// ***********************************************************************
/// Non-SCALe DCC restore
// ***********************************************************************
void DDRSS_dcc_cal_set_trained_results( uint32 _inst_, uint32 ca_dcc_training[2][2][2][2], uint32 dq_dcc_training[2][4][2][2][2])
{
  int i,j;
  
  // Restore CA values
  for (i = 0; i< CHNL_CNT; i++){// chnl
	    EBI1_PHY_CFG_set_ca_dcc_cal( _inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, i, ca_dcc_training[i]);
    }

  // Restore DQ values
  for (i = 0; i< CHNL_CNT; i++){// chnl
    for (j = 0; j< DQ_CNT;j++) {// DQ
	    EBI1_PHY_CFG_set_dq_dcc_cal( _inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, i, j, dq_dcc_training[i][j]);
    }
  }

}









































uint32 extern wrlvl_status_g[DQ_CNT];
uint32 extern wrlvl_delay_g[DQ_CNT];
uint32 byte0_one_count[DQ_CNT];
uint32 byte0_zero_count[DQ_CNT];
uint32 byte1_one_count[DQ_CNT];
uint32 byte1_zero_count[DQ_CNT];
uint32 mr1_without_wrlvl ;
uint32 mr1_with_wrlvl ;
uint32 auto_refresh_status;  
uint32 wrlvl_cfg_sel;
  
uint32 wrlvl_freq_plan_g[WRLVL_TOTAL_FREQ];
uint16 byte0_wrlvl_dly[DQ_CNT];
uint16 byte1_wrlvl_dly[DQ_CNT];

uint32 byte0_neg_wrlvl_flag[CHNL_CNT][DQ_CNT];
uint32 byte1_neg_wrlvl_flag[CHNL_CNT][DQ_CNT];
uint32 byte0_small_wrlvl_delay[CHNL_CNT][DQ_CNT];
uint32 byte1_small_wrlvl_delay[CHNL_CNT][DQ_CNT];

uint32 need_ck_half_cycle_delay[CHNL_CNT];

uint32  wrlvl_training_pass;
uint32  apply_ck_half_cycle;

// ***********************************************************************
/// -------------------------------------------------------------
///  populate_wrlvl_freq_plan:
///    writes to the wrlvl_cfg(i)
/// -------------------------------------------------------------
// ***********************************************************************
void DDRSS_populate_wrlvl_freq_plan( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 clk_plan[WRLVL_TOTAL_FREQ])
{
  int i;
  uint32 period_100ps;
  uint32 period_1ps;


  for (i = 0; i < WRLVL_TOTAL_FREQ;  i ++){
    //period_100ps = 10000000 / wrlvl_freq_plan[i];
    wrlvl_freq_plan_g[i]= clk_plan[i];
    if(clk_plan[i] == 0){
      period_100ps = 0;
    }else{
      // fix for JIRA: DDRSS-12  removed "<<1"
      period_1ps = 1000000000 / clk_plan[i];
      period_100ps = ((period_1ps << 5) + (period_1ps << 3) + (period_1ps)) >> 12;
    }

    EBI1_PHY_CFG_wrlvl_freq_cfg(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
		                            channel_sel,
		                            i,
		                            period_100ps);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------
///  wrlvl_training:
///    Calls all the sub wrlvl functions
/// -------------------------------------------------------------
// ***********************************************************************
uint32 DDRSS_wrlvl_training( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select, uint32 clk_freq_in_khz, uint32 max_loop_count, uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2])
{

  wrlvl_training_pass = 1;

  // for PCDDR3, 
  //* which_LUT = 0, return WL
  //* which_LUT = 1, return RL 
  //* which_LUT = 2, return MR0 
  //* which_LUT = 3, return MR1_without_wrlvl 
  //* which_LUT = 4, return MR1_with_wrlvl  

  //uint32 Get_BIMC_LUT (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 which_LUT)
  mr1_without_wrlvl = Get_BIMC_LUT (channel_sel, clk_freq_in_khz, 0x3);
  mr1_with_wrlvl    = Get_BIMC_LUT (channel_sel, clk_freq_in_khz, 0x4);

  DDRSS_wrlvl_training_pre_switch(_inst_,
                                        channel_sel,
	                                chip_select);
  
  DDRSS_wrlvl_training_post_switch(_inst_,
	                                 channel_sel,
	                                 chip_select,
	                                 clk_freq_in_khz,
	                                 max_loop_count);
  
  // Error detection
  if (wrlvl_training_pass) {
      DDRSS_wrlvl_retimer_cfg(_inst_, channel_sel, dq_wrlvl_retimer_training, dq_wrlvl_delay_training, clk_freq_in_khz);

      // Debug print statements
      print_stmt1(channel_sel, clk_freq_in_khz);
#ifdef WRLVL_PRINT
     ddr_printf(DDR_STATUS, "Done ddr_wrlvl_training @ %d MHz \n", clk_freq_in_khz/1000);
#endif
      return(wrlvl_training_pass);
  } else {
#ifdef WRLVL_PRINT
      ddr_printf(DDR_ERROR, "Error: ddr_wrlvl_training @ %d MHz \n", clk_freq_in_khz/1000);
#endif
      return(0);
  }
}


// ***********************************************************************
/// -------------------------------------------------------------
///  wrlvl_training_pre_switch:
///    Initial MR and config prior to frequency switch
/// -------------------------------------------------------------
// ***********************************************************************
void DDRSS_wrlvl_training_pre_switch( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select )
{
  ///  Stop all periodic events at the BIMC level
  //BIMC_Disable_All_Periodic(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select);
  BIMC_Enable_CLK_To_PHY(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel);
  /// Set DRAM in write-leveling mode.
  /// Enable WRLVL Training.
  /// BIMC::bimc_mode_register_write(channel_sel, chip_select, 0x02, 0x80);
  //BIMC_Write_MR(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select, 0x01, (0x6| 1<<7));
  BIMC_Write_MR(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select, 0x01, mr1_with_wrlvl);
  /// BIMC_MC_SHKE::ABHN_SHKE_Write_MR( uint32 _inst_, uint32 chip_select, uint32 mr_addr, uint32 mr_data )
 
  //Get Auto_Refresh Enable Status
  auto_refresh_status = BIMC_Get_Auto_Refresh_Status(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel);

  //Disable auto-refresh if its already set
  if (auto_refresh_status & chip_select) {
    BIMC_Set_Auto_Refresh_Status( _inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, auto_refresh_status & ~chip_select );
  }

}


// ***********************************************************************
/// -------------------------------------------------------------
///  wrlvl_training_post_switch:
///    Main write-leveling sequence starts after frequency switch
/// -------------------------------------------------------------
// ***********************************************************************
void DDRSS_wrlvl_training_post_switch( uint32 _inst_, SDRAM_INTERFACE channel_sel, uint32 chip_select, uint32 clk_freq_in_khz, uint32 max_loop_count )
{
  int tbyte;
  int ii;
  int kk;
  int loop_cnt;
  int total_pulses;
  uint32 byte0_done[DQ_CNT];
  uint32 byte1_done[DQ_CNT];
  uint32 byte0_hit_ck_low[DQ_CNT];
  uint32 byte1_hit_ck_low[DQ_CNT];
  total_pulses = (max_loop_count * 2) + 1;
  
  

  ///  initialize variables to zero
  for (tbyte = 0; tbyte < DQ_CNT;  tbyte ++){
    byte0_done[tbyte] = 0;
    byte1_done[tbyte] = 0;
    byte0_hit_ck_low[tbyte] = 0;
    byte1_hit_ck_low[tbyte] = 0;
    byte0_wrlvl_dly[tbyte] = 0;
    byte1_wrlvl_dly[tbyte] = 0;
  }
  ///  Enable write-leveling mode at each DQ
  EBI1_PHY_CFG_enable_wrlvl_mode(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
	                               channel_sel);

  //Exit training when FPM is not setup properly to match the current frequency
  wrlvl_cfg_sel = EBI1_PHY_CFG_get_wrlvl_cfg(_inst_  + SEQ_DDRSS_EBI1_PHY_OFFSET , channel_sel);
  if(wrlvl_cfg_sel >= 0x7){wrlvl_training_pass = 0;}
  else {
    //enable DRAM ODT to terminate DQS
    BIMC_Enable_ODT(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select);
    
    for (tbyte = 0; tbyte < DQ_CNT;  tbyte ++){
      EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
    	                                 channel_sel,
    	                                 0,
    	                                 tbyte,
    	                                 wrlvl_cfg_sel,
    	                                 0x0);
    
      EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
    	                                 channel_sel,
    	                                 1,
    	                                 tbyte,
    	                                 wrlvl_cfg_sel,
    	                                 0x0);
    }

    ///  Wait until all the bytes have been trained.
#ifdef NUM_BYTES_8
    while ( (!byte0_done[0]) | (!byte0_done[1]) | (!byte0_done[2]) | (!byte0_done[3]) | (!byte1_done[0]) | (!byte1_done[1]) | (!byte1_done[2]) | (!byte1_done[3]) )
#else
    while ( (!byte0_done[0]) | (!byte0_done[1]) | (!byte1_done[0]) | (!byte1_done[1]) )
#endif  
    {
      // force need_ck_half_cycle_delay=0
      if((need_ck_half_cycle_delay[channel_sel] == 1) & (apply_ck_half_cycle == 0)){need_ck_half_cycle_delay[channel_sel] = 0;}

    ///  Initial wrlvl_pulse
    EBI1_PHY_CFG_wrlvl_trigger_pulse(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
		                                 channel_sel);
    ///  Read Write-Leveling status 
    EBI1_PHY_CFG_read_wrlvl_status(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
		                               channel_sel);
  
    for (ii = 0; ii < DQ_CNT;  ii ++){
      if (wrlvl_status_g[ii] & 0xFF) {
        byte0_one_count[ii] = 1;
        byte0_zero_count[ii] = 0;
      }
      else
      {
        byte0_one_count[ii] = 0;
        byte0_zero_count[ii] = 1;
      }
  
      if ((wrlvl_status_g[ii] >> 8) & 0xFF) {
        byte1_one_count[ii] = 1;
        byte1_zero_count[ii] = 0;
      }
      else
      {
        byte1_one_count[ii] = 0;
        byte1_zero_count[ii] = 1;
      }
    }
  
    for (loop_cnt = 0; loop_cnt < max_loop_count;  loop_cnt ++){
      EBI1_PHY_CFG_wrlvl_trigger_zero(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
			                                channel_sel);
      /// Read Write-Leveling status 
      DDRSS_check_wrlvl_status(_inst_,
			                         channel_sel);
      EBI1_PHY_CFG_wrlvl_trigger_one(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
			                               channel_sel);
      /// Read Write-Leveling status 
      DDRSS_check_wrlvl_status(_inst_,
			                         channel_sel);
    }
    ///  Check if done: one_count is greater than zero_count, 
    ///  or reached the max wrlvl delay (0x7f).
    for (kk = 0; kk < DQ_CNT;  kk ++){
      /// ----- Check done status for byte0
      if (byte0_done[kk] == 0) {
        if ((byte0_zero_count[kk] != total_pulses)) { // Detect the falling edge
          if ((byte0_hit_ck_low[kk] == 1)&& (byte0_one_count[kk] == total_pulses)) {
            byte0_done[kk] = 1;
            if ((byte0_wrlvl_dly[kk] & 0x7) > 0x4) {
            byte0_wrlvl_dly[kk]&=0x7C;  //trim off last two bits.  
            }
          }
          else
          {
            if (byte0_wrlvl_dly[kk] < MAX_WRLVL_CDC_DLY) {
              byte0_wrlvl_dly[kk] = byte0_wrlvl_dly[kk] + 1;
              if (((byte0_wrlvl_dly[kk] & 0x7) < 0x5) || ((byte0_wrlvl_dly[kk] & 0x78) == 0x78)) {
                EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
								                                 channel_sel,
								                                 0,
								                                 kk,
								                                 wrlvl_cfg_sel,
								                                 byte0_wrlvl_dly[kk]);
              }
            }
            else
            {
              byte0_done[kk] = 1;
            }
          }
        }
        else
        {
          byte0_hit_ck_low[kk] = 1;
          if (byte0_wrlvl_dly[kk] < MAX_WRLVL_CDC_DLY) {
            byte0_wrlvl_dly[kk] = byte0_wrlvl_dly[kk] + 1;
            if (((byte0_wrlvl_dly[kk] & 0x7) < 0x5) || ((byte0_wrlvl_dly[kk] & 0x78) == 0x78)) {
              EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
							                                 channel_sel,
							                                 0,
							                                 kk,
							                                 wrlvl_cfg_sel,
							                                 byte0_wrlvl_dly[kk]);
            }
          }
          else
          {
            byte0_done[kk] = 1;
          }
        }
      }
      /// ----- Check done status for BYTE1
      if (byte1_done[kk] == 0) {
        if ((byte1_zero_count[kk] != total_pulses)) {
          if ((byte1_hit_ck_low[kk] == 1)&&(byte1_one_count[kk] == total_pulses)) { // Detect the falling edge
            byte1_done[kk] = 1;
            if ((byte1_wrlvl_dly[kk] & 0x7) > 0x4) {
              byte1_wrlvl_dly[kk]&=0x7C;  //trim off last two bits.  
            }
          }
          else
          {
            if (byte1_wrlvl_dly[kk] < MAX_WRLVL_CDC_DLY) {
              byte1_wrlvl_dly[kk] = byte1_wrlvl_dly[kk] + 1;
              if (((byte1_wrlvl_dly[kk] & 0x7) < 0x5) || ((byte1_wrlvl_dly[kk] & 0x78) == 0x78)) {
                EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
								                                 channel_sel,
								                                 1,
								                                 kk,
								                                 wrlvl_cfg_sel,
								                                 byte1_wrlvl_dly[kk]);
              }
            }
            else
            {
            byte1_done[kk] = 1;
            }
          }
        }
        else
        {
          byte1_hit_ck_low[kk] = 1;
          if (byte1_wrlvl_dly[kk] < MAX_WRLVL_CDC_DLY) {
            byte1_wrlvl_dly[kk] = byte1_wrlvl_dly[kk] + 1;
            if (((byte1_wrlvl_dly[kk] & 0x7) < 0x5) || ((byte1_wrlvl_dly[kk] & 0x78) == 0x78)) {
              EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
							                                 channel_sel,
							                                 1,
							                                 kk,
							                                 wrlvl_cfg_sel,
							                                 byte1_wrlvl_dly[kk]);
            }
          }
          else
          {
            byte1_done[kk] = 1;
          }
        }
      }
    }
  }

  }  // if-else no error end

  // Debug print statements  
  print_stmt2(channel_sel, byte0_done, byte1_done, byte0_hit_ck_low, byte1_hit_ck_low);

  //Disable DRAM ODT
  BIMC_Disable_ODT(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select);

  ///  Disable write-leveling mode at the PHY for normal operation
  EBI1_PHY_CFG_disable_wrlvl_mode(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
	                                channel_sel);

  /// Disable WRLVL Training for normal operation at DRAM.
  //BIMC_Write_MR(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select, 0x01, 0x6);
  BIMC_Write_MR(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select, 0x01, mr1_without_wrlvl);

  BIMC_Disable_CLK_To_PHY (_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel );
  
  /// enable Auto refresh
  //BIMC_Enable_Auto_Refresh(_inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, chip_select); 

  //Sets the original auto_refresh status
  if (auto_refresh_status & chip_select) {
      BIMC_Set_Auto_Refresh_Status( _inst_ + SEQ_DDRSS_BIMC_OFFSET, channel_sel, auto_refresh_status ); 
  }

}


// ***********************************************************************
/// -------------------------------------------------------------
///  check_wrlvl_status:
///    after a wrlvl toggle, check the status 
/// -------------------------------------------------------------
// ***********************************************************************
void DDRSS_check_wrlvl_status( uint32 _inst_, SDRAM_INTERFACE channel_sel )
{
  int jj;

  

  /// Read Write-Leveling status 
  EBI1_PHY_CFG_read_wrlvl_status(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
	                               channel_sel);
  for (jj = 0; jj < DQ_CNT;  jj ++){
    if (wrlvl_status_g[jj] & 0xFF) {
      byte0_one_count[jj] = byte0_one_count[jj] + 1;
    }
    else
    {
      byte0_zero_count[jj] = byte0_zero_count[jj] + 1;
    }
  
    if ((wrlvl_status_g[jj] >> 8) & 0xFF) {
      byte1_one_count[jj] = byte1_one_count[jj] + 1;
    }
    else
    {
      byte1_zero_count[jj] = byte1_zero_count[jj] + 1;
    }
  }
}

// ***********************************************************************
/// ---------------------------------------------------------
/// DDRPHY_DQ_WRLVL_DELAYn[BYTE0|1_RETIMER_DDR] Value               
/// 0 For SDR data (not used in DQ DDR PHY Retimer)             
/// 1 When WRLVL CDC delay <= T/4             
/// 2 When WRLVL CDC delay > T/4 and <= 3T/4              
/// 3 When WRLVL CDC delay > 3T/4             
/// ---------------------------------------------------------
// ***********************************************************************
uint32 DDRSS_retimer_sel_func( uint32 _inst_, uint32 clk_freq_in_khz, uint32 cdc_wrlvl_delay )
{
  uint32 period_div4_1ps;
  uint32 period_3div4_1ps;
  uint32 cdc_wrlvl_delay_1ps;
  uint32 period_1ps = 1000000000 / clk_freq_in_khz;
  uint32 retimer_sel_ddr = 0;


  ///  1/4*period and 3/4*period
  period_div4_1ps = period_1ps / 4;
  period_3div4_1ps = 3 * (period_div4_1ps);
  ///  Convert CSR results to 1ps
  ///  cdc_wrlvl_delay[6:3] coarse delay @ 79ps 
  ///  cdc_wrlvl_delay[2:0] coarse delay @ 15ps 
  cdc_wrlvl_delay_1ps = ((cdc_wrlvl_delay >> 3) * WRLVL_COARSE_DELAY) + ((cdc_wrlvl_delay & 0x7) * WRLVL_FINE_DELAY);
  
  if (cdc_wrlvl_delay_1ps <= period_div4_1ps) {
    retimer_sel_ddr = 1;
  }
  else
  {
    if ((cdc_wrlvl_delay_1ps > period_div4_1ps) && (cdc_wrlvl_delay_1ps <= period_3div4_1ps)) {
      retimer_sel_ddr = 2;
    }
    else
    {
      if (cdc_wrlvl_delay_1ps > period_3div4_1ps) {
        retimer_sel_ddr = 3;
      }
    }
  }
  return ((retimer_sel_ddr));
}


// ***********************************************************************
/// ---------------------------------------------------------
/// DDRPHY_DQ_WRLVL_DELAYn[BYTE0|1_RETIMER_IE_OE] Value               
/// 0 When WRLVL CDC delay <= T/2 - Tmin              
/// 1 When WRLVL CDC delay > T/2 - Tmin             
/// ---------------------------------------------------------
// ***********************************************************************
uint32 DDRSS_retimer_oe_ie_func( uint32 _inst_, uint32 clk_freq_in_khz, uint32 cdc_wrlvl_delay )
{
  uint32 period_div2_1ps;
  uint32 cdc_wrlvl_delay_1ps;
  uint32 period_1ps = 1000000000 / clk_freq_in_khz;
  uint32 retimer_sel_oe_ie = 0;


  ///  1/2*period 
  period_div2_1ps = period_1ps / 2;
  ///  Convert CSR results to 1ps
  ///  cdc_wrlvl_delay[6:3] coarse delay @ 79ps
  ///  cdc_wrlvl_delay[2:0] coarse delay @ 15ps
  cdc_wrlvl_delay_1ps = ((cdc_wrlvl_delay >> 3) * WRLVL_COARSE_DELAY) + ((cdc_wrlvl_delay & 0x7) * WRLVL_FINE_DELAY);
  
  if (cdc_wrlvl_delay_1ps <= (period_div2_1ps - TMIN)) {
    retimer_sel_oe_ie = 0;
  }
  else
  {
    retimer_sel_oe_ie = 1;
  }
  
  return ((retimer_sel_oe_ie));
}

// NEEDS SCALe CODE
// Restore
void DDRSS_wrlvl_set_trained_results( uint32 _inst_, uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2])
{
int chnl_index, dq_cnt_index, wrlvl_cfg_index,dq_byte_index; 

  for (chnl_index = 0; chnl_index < CHNL_CNT;  chnl_index ++){                              // 0-1
    for (dq_cnt_index = 0; dq_cnt_index < DQ_CNT;  dq_cnt_index ++){                        // 0-3
      for (wrlvl_cfg_index = 0; wrlvl_cfg_index < WRLVL_TOTAL_FREQ;  wrlvl_cfg_index ++){   // 0-6
        for (dq_byte_index = 0; dq_byte_index < BYTE_PER_DQ_CNT;  dq_byte_index ++){        // 0-1

          EBI1_PHY_CFG_set_cdc_wrlvl_training( _inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
              chnl_index,      // uint32 channel_sel,
              dq_byte_index,   // uint32 byte_index,
              dq_cnt_index,    // uint32 dq_index,
              wrlvl_cfg_index, // uint32 delay_index,
              dq_wrlvl_retimer_training[chnl_index][dq_cnt_index][wrlvl_cfg_index ][dq_byte_index], //uint32 retimer_value 
              dq_wrlvl_delay_training[chnl_index][dq_cnt_index][wrlvl_cfg_index ][dq_byte_index]  //uint32 delay_value 
              );
        }
      }
    }
  }
}

// ***********************************************************************
/// Non-SCALe code
/// -------------------------------------------------------------
///  Apply ck_half_cycle to Trahira CDP 
/// -------------------------------------------------------------
// ***********************************************************************
void Set_Apply_Half_Cycle( uint32 flag )
{
  apply_ck_half_cycle = flag;
}

// ***********************************************************************
/// Non-SCALe code
/// -------------------------------------------------------------
///  wrlvl_retimer_cfg2:
///    Determine the retimer_sel, retimer_sel_oe_ie, and cycle_delay
/// -------------------------------------------------------------
// ***********************************************************************
void DDRSS_wrlvl_retimer_cfg( uint32 _inst_, SDRAM_INTERFACE channel_sel, 
    uint16 dq_wrlvl_retimer_training[2][4][7][2], uint16 dq_wrlvl_delay_training[2][4][7][2], uint32 clk_freq_in_khz )
{
  int byte0_need_cycle_delay[DQ_CNT];
  int byte1_need_cycle_delay[DQ_CNT];
  uint32 byte0_retimer_sel_LUT_value[DQ_CNT];
  uint32 byte0_retimer_oe_ie_LUT_value[DQ_CNT];
  uint32 byte1_retimer_sel_LUT_value[DQ_CNT];
  uint32 byte1_retimer_oe_ie_LUT_value[DQ_CNT];
  uint32 byte0_retimer_cycle_delay_sel[DQ_CNT];
  uint32 byte1_retimer_cycle_delay_sel[DQ_CNT];
  uint32 cfg_index;
  uint32 freq_plan_period;
  int tbyte;
  int k;

  uint16 byte0_retimer = {0};
  uint16 byte1_retimer = {0};
 
  

  ///  initialize variables to zero
  for (tbyte = 0; tbyte < DQ_CNT;  tbyte ++){
    byte0_need_cycle_delay[tbyte] = 0;
    byte1_need_cycle_delay[tbyte] = 0;
    byte0_retimer_sel_LUT_value[tbyte] = 0;
    byte0_retimer_oe_ie_LUT_value[tbyte] = 0;
    byte1_retimer_sel_LUT_value[tbyte] = 0;
    byte1_retimer_oe_ie_LUT_value[tbyte] = 0;
    byte0_retimer_cycle_delay_sel[tbyte] = 0;
    byte1_retimer_cycle_delay_sel[tbyte] = 0;
  }

    cfg_index = wrlvl_cfg_sel; // use the same variable
    freq_plan_period = clk_freq_in_khz;

    /// ----- retimer values for BYTE0
    for (k = 0; k < DQ_CNT;  k ++){
     // Check for potential negative-write-level for Byte0
     if(freq_plan_period <= 667000){
        if (byte0_wrlvl_dly[k] >= 0x68) { //  For low frequencies....
          byte0_neg_wrlvl_flag[channel_sel][k] = 1;
          byte0_wrlvl_dly[k] = 0;
          EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
                                           channel_sel,
                                           0,
                                           k,
                                           cfg_index,
                                           byte0_wrlvl_dly[k]);
          } else {
              if (byte0_wrlvl_dly[k] <= 0x10) { //Small delay, record delay
              byte0_small_wrlvl_delay[channel_sel][k] = byte0_wrlvl_dly[k];
              byte0_neg_wrlvl_flag[channel_sel][k] = 1;
              }
          }
        need_ck_half_cycle_delay[channel_sel] |= (byte0_neg_wrlvl_flag[channel_sel][k]);
      } else { // For high frequencies ...

     if ((k == 0) &(byte0_wrlvl_dly[k] < 0x40) & (apply_ck_half_cycle == 1)){ // apply to longer trace. CDP is byte0 (k=0)
       byte0_need_cycle_delay[k] = 1; 
     } else {
       byte0_need_cycle_delay[k] = 0; 
     }
        if((byte0_neg_wrlvl_flag[channel_sel][k] == 1) & (need_ck_half_cycle_delay[channel_sel] == 0) & 
          ((byte0_wrlvl_dly[k] > 0x3F)| ((byte0_wrlvl_dly[k] - byte0_small_wrlvl_delay[channel_sel][k]) < 0x4C))){
          byte0_wrlvl_dly[k] = 0;
          EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
                                           channel_sel,
                                           0,
                                           k,
                                           cfg_index,
                                           byte0_wrlvl_dly[k]);
        }
      }

      byte0_retimer_cycle_delay_sel[k] = 0;
      byte0_retimer_sel_LUT_value[k] = DDRSS_retimer_sel_func(_inst_ , freq_plan_period, byte0_wrlvl_dly[k]);
      byte0_retimer_oe_ie_LUT_value[k] = DDRSS_retimer_oe_ie_func(_inst_ , freq_plan_period, byte0_wrlvl_dly[k]);
      if (byte0_need_cycle_delay[k] == 1) {
        if (byte0_wrlvl_dly[k] != MAX_WRLVL_DELAY) {
          if (byte0_wrlvl_dly[k] != MID_WRLVL_DELAY) {
            ///  Need write leveling FIFO delay
            byte0_retimer_cycle_delay_sel[k] = 1;
          }
        }
      }
    // Store trained values
    byte0_retimer = (byte0_retimer_cycle_delay_sel[k]<<3 |
                     byte0_retimer_oe_ie_LUT_value[k]<<2 |
                     byte0_retimer_sel_LUT_value[k]
                     );
    dq_wrlvl_retimer_training[channel_sel][k][cfg_index][0] = byte0_retimer;
    dq_wrlvl_delay_training[channel_sel][k][cfg_index][0] = byte0_wrlvl_dly[k];
    }
    /// ----- retimer values for BYTE1
    for (k = 0; k < DQ_CNT;  k ++){
     // Check for potential negative-write-level for Byte1
     if(freq_plan_period <= 667000){
        if (byte1_wrlvl_dly[k] >= 0x68) { //  For low frequencies....
          byte1_neg_wrlvl_flag[channel_sel][k] = 1;
          byte1_wrlvl_dly[k] = 0;
          EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
                                           channel_sel,
                                           1,
                                           k,
                                           cfg_index,
                                           byte1_wrlvl_dly[k]);
          } else {
              if (byte1_wrlvl_dly[k] <= 0x10) { //Small delay, record delay
              byte1_small_wrlvl_delay[channel_sel][k] = byte1_wrlvl_dly[k];
              byte1_neg_wrlvl_flag[channel_sel][k] = 1;
              }
          }
        need_ck_half_cycle_delay[channel_sel] |= (byte1_neg_wrlvl_flag[channel_sel][k]);
      } else { // For high frequencies ...

       if ((k == 0) &(byte1_wrlvl_dly[k] < 0x40)& (apply_ck_half_cycle == 1)){ // apply to longer trace. CDP is byte0 (k=0)
         byte1_need_cycle_delay[k] = 1; 
       } else {
         byte1_need_cycle_delay[k] = 0; 
       }

        if((byte1_neg_wrlvl_flag[channel_sel][k] == 1) & (need_ck_half_cycle_delay[channel_sel] == 0) & 
          ((byte1_wrlvl_dly[k] > 0x3F)| ((byte1_wrlvl_dly[k] - byte1_small_wrlvl_delay[channel_sel][k]) < 0x4C))){
          byte1_wrlvl_dly[k] = 0;
          EBI1_PHY_CFG_set_cdc_wrlvl_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
                                           channel_sel,
                                           1,
                                           k,
                                           cfg_index,
                                           byte1_wrlvl_dly[k]);
        }
      }

      byte1_retimer_cycle_delay_sel[k] = 0;
      byte1_retimer_sel_LUT_value[k] = DDRSS_retimer_sel_func(_inst_ , freq_plan_period, byte1_wrlvl_dly[k]);
      byte1_retimer_oe_ie_LUT_value[k] = DDRSS_retimer_oe_ie_func(_inst_ , freq_plan_period, byte1_wrlvl_dly[k]);
      if (byte1_need_cycle_delay[k] == 1) {
        if (byte1_wrlvl_dly[k] != MAX_WRLVL_DELAY) {
          if (byte1_wrlvl_dly[k] != MID_WRLVL_DELAY) {
            ///  Need write leveling FIFO delay
            byte1_retimer_cycle_delay_sel[k] = 1;
          }
        }
      }
    // Store trained values
    byte1_retimer = (byte1_retimer_cycle_delay_sel[k]<<3 |
                     byte1_retimer_oe_ie_LUT_value[k]<<2 |
                     byte1_retimer_sel_LUT_value[k]
                    );
    dq_wrlvl_retimer_training[channel_sel][k][cfg_index][1] = byte1_retimer;
    dq_wrlvl_delay_training[channel_sel][k][cfg_index][1] = byte1_wrlvl_dly[k];
    }
  
    EBI1_PHY_CFG_set_wrlvl_cdc_retimer(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,
		                                   channel_sel,
		                                   cfg_index,
		                                   byte0_retimer_sel_LUT_value,
		                                   byte0_retimer_oe_ie_LUT_value,
		                                   byte0_retimer_cycle_delay_sel,
		                                   byte1_retimer_sel_LUT_value,
		                                   byte1_retimer_oe_ie_LUT_value,
		                                   byte1_retimer_cycle_delay_sel);
        // Need half-cycle if neg-wrlvl is detect at 400MHz
        // Set half-cycle at CA and train
        if((need_ck_half_cycle_delay[channel_sel] == 1) & (apply_ck_half_cycle == 1)){
          EBI1_PHY_CFG_set_wrlvl_half_cycle_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET,channel_sel);
        }
}


void print_stmt1(SDRAM_INTERFACE channel_sel, uint32 clk_freq_in_khz)
{

   #ifdef WRLVL_PRINT
      if((wrlvl_cfg_sel)==0x0 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG0 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG0));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG0));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG0));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG0));
#endif
      }

      if((wrlvl_cfg_sel)==0x0 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG0 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG0));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG0));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG0));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG0));
#endif
      }
      if((wrlvl_cfg_sel)==0x1 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG1 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG1));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG1));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG1));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG1));
#endif
      }

      if((wrlvl_cfg_sel)==0x1 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG1 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG1));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG1));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG1));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG1));
#endif
      }
      if((wrlvl_cfg_sel)==0x2 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG2 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG2));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG2));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG2));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG2));
#endif
      }

      if((wrlvl_cfg_sel)==0x2 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG2 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG2));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG2));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG2));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG2));
#endif
      }
      if((wrlvl_cfg_sel)==0x3 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG3 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG3));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG3));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG3));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG3));
#endif
      }

      if((wrlvl_cfg_sel)==0x3 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG3 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG3));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG3));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG3));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG3));
#endif
      }
      if((wrlvl_cfg_sel)==0x4 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG4 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG4));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG4));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG4));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG4));
#endif
      }

      if((wrlvl_cfg_sel)==0x4 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG4 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG4));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG4));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG4));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG4));
#endif
      }
      if((wrlvl_cfg_sel)==0x5 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG5 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG5));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG5));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG5));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG5));
#endif
      }

      if((wrlvl_cfg_sel)==0x5 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG1 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG5));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG5));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG5));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG5));
      }
#endif
      if((wrlvl_cfg_sel)==0x6 && channel_sel==0)
      {
         ddr_printf(DDR_NORMAL, "CH0 CFG6 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH0_DQ0_DDRPHY_DQ_WRLVL_CFG6));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH0_DQ1_DDRPHY_DQ_WRLVL_CFG6));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH0_DQ2_DDRPHY_DQ_WRLVL_CFG6));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH0_DQ3_DDRPHY_DQ_WRLVL_CFG6));
#endif
      }

      if((wrlvl_cfg_sel)==0x6 && channel_sel==1)
      {
         ddr_printf(DDR_NORMAL, "CH1 CFG6 for %d MHz after setting Retimer\n", clk_freq_in_khz/1000);
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ0_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ1_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ2_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE0: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE0_RETIMER_CYCLE_DELAY_SEL));
         ddr_printf(DDR_NORMAL, "CH0_DQ3_BYTE1: RETIMER_DDR=%X RETIMER_IE_OE=%X RETIMER_CYCLE_DELAY=%X\n",HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_DDR),
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_SEL_OE_IE),      
                                                                                                          HWIO_INF(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6,BYTE1_RETIMER_CYCLE_DELAY_SEL)); 
#endif
         ddr_printf(DDR_NORMAL, "Write-leveling results:\n");
         ddr_printf(DDR_NORMAL, "CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH1_DQ0_DDRPHY_DQ_WRLVL_CFG6));
         ddr_printf(DDR_NORMAL, "CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH1_DQ1_DDRPHY_DQ_WRLVL_CFG6));
#ifdef NUM_BYTES_8
         ddr_printf(DDR_NORMAL, "CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH1_DQ2_DDRPHY_DQ_WRLVL_CFG6));
         ddr_printf(DDR_NORMAL, "CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6=0x%8X\n", HWIO_IN(CH1_DQ3_DDRPHY_DQ_WRLVL_CFG6));
#endif
      }
      ddr_printf(DDR_NORMAL, "\n");  
   #endif      
}

void print_stmt2(SDRAM_INTERFACE channel_sel, uint32 byte0_done[DQ_CNT],uint32 byte1_done[DQ_CNT],uint32 byte0_hit_ck_low[DQ_CNT], uint32 byte1_hit_ck_low[DQ_CNT])
{
   #ifdef WRLVL_PRINT
            
            if(byte0_done[0] && byte1_done[0] && byte0_done[1] && byte1_done[1] && byte0_done[2] && byte1_done[2] && byte0_done[3] && byte1_done[3]/* && kk==3*/ )
            {
               ddr_printf(DDR_NORMAL, "-- Channel %d wrlvl results:\n",channel_sel);
               ddr_printf(DDR_NORMAL, "BYTE0_DQ0=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt0_hit_ck_low=%d\n",byte0_wrlvl_dly[0], byte0_one_count[0],byte0_zero_count[0],byte0_done[0],byte0_hit_ck_low[0]);
               ddr_printf(DDR_NORMAL, "BYTE1_DQ0=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt1_hit_ck_low=%d\n",byte1_wrlvl_dly[0], byte1_one_count[0],byte1_zero_count[0],byte1_done[0],byte1_hit_ck_low[0]);
               ddr_printf(DDR_NORMAL, "BYTE0_DQ1=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt0_hit_ck_low=%d\n",byte0_wrlvl_dly[1], byte0_one_count[1],byte0_zero_count[1],byte0_done[1],byte0_hit_ck_low[1]);
               ddr_printf(DDR_NORMAL, "BYTE1_DQ1=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt1_hit_ck_low=%d\n",byte1_wrlvl_dly[1], byte1_one_count[1],byte1_zero_count[1],byte1_done[1],byte1_hit_ck_low[1]);
               ddr_printf(DDR_NORMAL, "BYTE0_DQ2=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt0_hit_ck_low=%d\n",byte0_wrlvl_dly[2], byte0_one_count[2],byte0_zero_count[2],byte0_done[2],byte0_hit_ck_low[2]);
               ddr_printf(DDR_NORMAL, "BYTE1_DQ2=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt1_hit_ck_low=%d\n",byte1_wrlvl_dly[2], byte1_one_count[2],byte1_zero_count[2],byte1_done[2],byte1_hit_ck_low[2]);
               ddr_printf(DDR_NORMAL, "BYTE0_DQ3=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt0_hit_ck_low=%d\n",byte0_wrlvl_dly[3], byte0_one_count[3],byte0_zero_count[3],byte0_done[3],byte0_hit_ck_low[3]);
               ddr_printf(DDR_NORMAL, "BYTE1_DQ3=%2X: 1'count:%5d > 0'count:%5d, T/F=%d\t byt1_hit_ck_low=%d\n",byte1_wrlvl_dly[3], byte1_one_count[3],byte1_zero_count[3],byte1_done[3],byte1_hit_ck_low[3]);
               ddr_printf(DDR_NORMAL, "\n");
            }
   #endif

}



















































#if ( BUS_SIZE == 32 )
 uint32 dq_training_pattern[] =
{
0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE, 0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D, // Tom's Pattern 
0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0x0f0f0f0f, 0xf0f0f0f0, 0x00000000, 0xffffffff, // Yong's Pattern 
0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, // Mike/Scott's Pattern 
0x21524110, 0xA5A5A5A5, 0x5A5A5A5A, 0x01120531, 0x35014541, 0x5A5A5A5A, 0xA5A5A5A5, 0xF4520FF2, // Tom's Pattern inverted 
0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0xf0f0f0f0, 0x0f0f0f0f, 0xffffffff, 0x00000000, // Yong's Pattern inverted 
0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, // Mike/Scott's Pattern inverted 
};
#endif


#if ( BUS_SIZE == 64 )
 uint32 dq_training_pattern[] =
{
0xDEADBEEF, 0xDEADBEEF,0x5A5A5A5A, 0x5A5A5A5A,0xA5A5A5A5, 0xA5A5A5A5,0xFEEDFACE, 0xFEEDFACE,0xCAFEBABE, 0xCAFEBABE,0xA5A5A5A5, 0xA5A5A5A5,0x5A5A5A5A, 0x5A5A5A5A,0x0BADF00D, 0x0BADF00D, // Tom's Pattern 
0xa5a5a5a5, 0xa5a5a5a5,0x5a5a5a5a, 0x5a5a5a5a,0xa5a5a5a5, 0xa5a5a5a5,0x5a5a5a5a, 0x5a5a5a5a,0x0f0f0f0f, 0x0f0f0f0f,0xf0f0f0f0, 0xf0f0f0f0,0x00000000, 0x00000000,0xffffffff, 0xffffffff, // Yong's Pattern 
0xFFFFFFFF, 0xFFFFFFFF,0x00000000, 0x00000000,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0x00000000, 0x00000000,0xFFFFFFFF, 0xFFFFFFFF,0x00000000, 0x00000000,0x00000000, 0x00000000, // Mike/Scott's Pattern 
0x21524110, 0x21524110,0xA5A5A5A5, 0xA5A5A5A5,0x5A5A5A5A, 0x5A5A5A5A,0x01120531, 0x01120531,0x35014541, 0x35014541,0x5A5A5A5A, 0x5A5A5A5A,0xA5A5A5A5, 0xA5A5A5A5,0xF4520FF2, 0xF4520FF2, // Tom's Pattern inverted 
0x5a5a5a5a, 0x5a5a5a5a,0xa5a5a5a5, 0xa5a5a5a5,0x5a5a5a5a, 0x5a5a5a5a,0xa5a5a5a5, 0xa5a5a5a5,0xf0f0f0f0, 0xf0f0f0f0,0x0f0f0f0f, 0x0f0f0f0f,0xffffffff, 0xffffffff,0x00000000, 0x00000000, // Yong's Pattern inverted 
0x00000000, 0x00000000,0xFFFFFFFF, 0xFFFFFFFF,0x00000000, 0x00000000,0x00000000, 0x00000000,0xFFFFFFFF, 0xFFFFFFFF,0x00000000, 0x00000000,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF  // Mike/Scott's Pattern inverted 
};
#endif

uint8 fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE]={0};

uint16 DDRSS_ddr_phy_dq_rd_training_ddrss( uint32 _inst_, SDRAM_INTERFACE interface, uint16 chip_select, uint16 fine_delay_training, uint16
per_bit_deskew_training, uint16 max_loop_count, uint16 coarse_init_val, uint16 fine_init_val, uint16 per_bit_deskew_init_val,
uint16 dq_read_training[2][4][2][4], uint32 dq_read_perbit_training[2][4][2][2],uint32 dq_read_perbit_dm_training[2][4],
  uint32 clk_freq_in_khz )

{
  uint8
    deskew_delay_value=0,
    bit=0,
    byte_in_bus=0,
    phy_in_bus=0,
    byte_in_phy=0,
    histogram_size=0,
    max_coarse=0,
    coarse_sweep=0,
    fine_sweep=0,
    loop_count=0,
    odd_even=0,
    delay_step=0,
    delay=0,
	pass_min_edge[BYTE_PER_BUS][BYTE_SIZE][2]={{{0}},{{0}}},
    pass_max_edge[BYTE_PER_BUS][BYTE_SIZE][2]={{{0}},{{0}}};
  uint16
    rank,first_rank,last_rank,
    training_period_1ps=0,
    perbit_mid[BYTE_PER_BUS][BYTE_SIZE][2]={{{0}},{{0}}},
    perbit_delay[BYTE_PER_BUS][BYTE_SIZE][2]={{{0}},{{0}}},
    perbit_cdc[BYTE_PER_BUS][BYTE_SIZE][2]={{{0}},{{0}}},
    max_pass_min_edge[BYTE_PER_BUS]={0},
    min_pass_max_edge[BYTE_PER_BUS]={0},
    min_mid[BYTE_PER_BUS]={0},
    max_mid[BYTE_PER_BUS]={0},
    min_eye_width[BYTE_PER_BUS]={0},
    min_eye_width_overbytes=0,
    programed_mid[BYTE_PER_BUS]={0},
    skew_range[BYTE_PER_BUS]={0},
    eye_width[BYTE_PER_BUS]={0};
  uint32
    allbit=0,
    read_data[DQ_TRAINING_PATTERN_SIZE]={0};
  
  delay_step = delay_step;
  training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse = (((training_period_1ps / 2) + fine_lut[DESKEW_CDC_INITIAL_VALUE] + 100) / COARSE_STEP) + 2;
  if(max_coarse >= MAX_COARSE_STEP) max_coarse = MAX_COARSE_STEP-1;
  histogram_size = max_coarse * MAX_FINE_STEP;
  if(max_loop_count > 255) max_loop_count = 255;
  
  if     (clk_freq_in_khz >= FPM_DELAY0_FMIN_IN_KHZ)  { delay = 0;}
  else if(clk_freq_in_khz >= FPM_DELAY1_FMIN_IN_KHZ)  { delay = 1;}
  else if(clk_freq_in_khz >= FPM_DELAY2_FMIN_IN_KHZ)  { delay = 2;}
  else                                                { delay = 3;}
   
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus  = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    dq_read_perbit_dm_training[interface][phy_in_bus] = DM_CSR_INITIAL;
    dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][0] = PER_BIT_CSR_INITIAL;
    dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][1] = PER_BIT_CSR_INITIAL;
    
    max_pass_min_edge[byte_in_bus] = 0;
    max_mid[byte_in_bus] = 0;
    min_pass_max_edge[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_mid[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_eye_width[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
  } // byte_in_bus
     min_eye_width_overbytes = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
 
  clear_histogram();
 
  if ( chip_select == SDRAM_RANK_BOTH ){//dual rank
     first_rank = SDRAM_RANK_CS0;
     last_rank  = SDRAM_RANK_CS1;
  }
  else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
  }
  for ( rank=first_rank ; rank <=last_rank ; rank++){ 
   
     ddr_mem_write_low_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
     DDRSS_dq_rd_set_dq_cdc_rd90_value(_inst_, interface, dq_read_training, dq_read_perbit_training, dq_read_perbit_dm_training);
     
     // left
     for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
     {
       DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, 2);
       DDRSS_RCW_training(interface, rank, 1, clk_freq_in_khz, 1);
       for(fine_sweep = fine_init_val; fine_sweep < MAX_FINE_STEP; fine_sweep++)
       {
         DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_rd_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
     
     // right
     for(coarse_sweep = max_coarse; coarse_sweep > 0;)
     {
       -- coarse_sweep;
       DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface,coarse_sweep, 2);
       DDRSS_RCW_training(interface, rank, 1, clk_freq_in_khz, 1);
       for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;){
         -- fine_sweep;
         DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_rd_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
   }//rank  

  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(odd_even=0;odd_even<2;odd_even++)
    {
      dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][odd_even] = 0;
      
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
        DDRSS_histogram_min_max_scan(_inst_,
          &pass_min_edge[byte_in_bus][bit][odd_even],
          &pass_max_edge[byte_in_bus][bit][odd_even],
          fail_count_histogram_perbit[byte_in_bus][bit][odd_even],
          histogram_size);
          
        perbit_mid[byte_in_bus][bit][odd_even] = ((cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]]
          + cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]]) / 2)
          - fine_lut[DESKEW_CDC_INITIAL_VALUE];
    
        if(max_pass_min_edge[byte_in_bus] < cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]]) max_pass_min_edge[byte_in_bus] = cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]];
        if(min_pass_max_edge[byte_in_bus] > cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]]) min_pass_max_edge[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]];
        if(max_mid[byte_in_bus] < perbit_mid[byte_in_bus][bit][odd_even]) max_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit][odd_even];
        if(min_mid[byte_in_bus] > perbit_mid[byte_in_bus][bit][odd_even]) min_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit][odd_even];
        if(min_eye_width[byte_in_bus] > (cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]] - cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]])) min_eye_width[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]] - cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]];
      }
    }
    
    skew_range[byte_in_bus] = max_mid[byte_in_bus] - min_mid[byte_in_bus];
    eye_width[byte_in_bus] = min_pass_max_edge[byte_in_bus] - max_pass_min_edge[byte_in_bus];
    skew_range[byte_in_bus] = skew_range[byte_in_bus]; 
    eye_width[byte_in_bus] = eye_width[byte_in_bus];
     if(min_eye_width_overbytes > eye_width[byte_in_bus]) {      min_eye_width_overbytes = eye_width[byte_in_bus];    }

    DDRSS_look_up_table_full_cdc(_inst_, max_mid[byte_in_bus], clk_freq_in_khz, dq_read_training[interface][phy_in_bus][byte_in_phy]);
    
    programed_mid[byte_in_bus] = cdc_lut[((dq_read_training[interface][phy_in_bus][byte_in_phy][delay] >> 3) * MAX_FINE_STEP)
      + (dq_read_training[interface][phy_in_bus][byte_in_phy][delay] & 0x07)];
    
    for(odd_even = 0; odd_even < 2; odd_even++)
    {
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
        perbit_delay[byte_in_bus][bit][odd_even] = programed_mid[byte_in_bus] - perbit_mid[byte_in_bus][bit][odd_even];   
        for(deskew_delay_value = 0; deskew_delay_value < 7; deskew_delay_value++)
        {
          if(fine_lut[deskew_delay_value + 1] > perbit_delay[byte_in_bus][bit][odd_even])
          {
            if((fine_lut[deskew_delay_value + 1] - perbit_delay[byte_in_bus][bit][odd_even]) < (perbit_delay[byte_in_bus][bit][odd_even] - fine_lut[deskew_delay_value]))
            {
              deskew_delay_value++;
            }
            break;
          }
        }
        perbit_cdc[byte_in_bus][bit][odd_even] = deskew_delay_value;
        perbit_cdc[byte_in_bus][bit][odd_even] = perbit_cdc[byte_in_bus][bit][odd_even];
        dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][odd_even] += (deskew_delay_value << (bit * 4));
      } // bit
    } // odd_even
  }	// byte_in_bus
  
    dq_read_perbit_dm_training[interface][0] = 0x0;
	dq_read_perbit_dm_training[interface][1] = 0x0;
#ifdef NUM_BYTES_8
	dq_read_perbit_dm_training[interface][2] = 0x0;
	dq_read_perbit_dm_training[interface][3] = 0x0;
#endif
  
  DDRSS_dq_rd_set_dq_cdc_rd90_value(_inst_, interface, dq_read_training, dq_read_perbit_training, dq_read_perbit_dm_training);

#ifdef ENABLE_PRINTF
  print_table_header();
  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(odd_even = 0; odd_even < 2; odd_even++)
    {
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
    
		 snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,	"             RD,%15d,%15d,%15d,%15d,%15d,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,%15d,%15d,          ",
          interface,
          phy_in_bus,
          byte_in_phy,
          bit,
          odd_even, 
          cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]],
          cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]],
          perbit_mid[byte_in_bus][bit][odd_even],
          perbit_delay[byte_in_bus][bit][odd_even],
          perbit_cdc[byte_in_bus][bit][odd_even]);
        
        for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
        {
     
		  snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[byte_in_bus][bit][odd_even][delay_step]);
        } // delay_step
        
		ddr_printf(DDR_DETAIL,"%s", logbuf);
      } // bit
    } // odd_even
    
    ddr_printf(DDR_NORMAL, "         RD_SUM,%15d,%15d,%15d,               ,               ,%15d,%15d,%15d,%15d,%15d,%15d\n",
      interface,
      phy_in_bus,
      byte_in_phy,
      min_mid[byte_in_bus],
      max_mid[byte_in_bus],
      programed_mid[byte_in_bus],
      min_eye_width[byte_in_bus],
      skew_range[byte_in_bus],
      eye_width[byte_in_bus]);
  }	// byte_in_bus
#endif

#ifdef ENABLE_PRINTF_RD_DBG
  //max_coarse = ((training_period_1ps / 2) + 100) / COARSE_STEP;
  histogram_size = max_coarse * MAX_FINE_STEP;
  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus  = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    //dq_read_perbit_dm_training[interface][phy_in_bus] = DM_CSR_INITIAL;
    //dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][0] = PER_BIT_CSR_INITIAL;
    //dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][1] = PER_BIT_CSR_INITIAL;
    
    max_pass_min_edge[byte_in_bus] = 0;
    max_mid[byte_in_bus] = 0;
    min_pass_max_edge[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_mid[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_eye_width[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
  }	// byte_in_bus
  
  clear_histogram();
  
  //ddr_mem_write_low_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
  //DDRSS_dq_rd_set_dq_cdc_rd90_value(_inst_, interface, dq_read_training, dq_read_perbit_training, dq_read_perbit_dm_training);
  
  if ( chip_select == SDRAM_RANK_BOTH ){//dual rank
     first_rank = SDRAM_RANK_CS0;
     last_rank  = SDRAM_RANK_CS1;
  }
  else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
  }
  for ( rank=first_rank ; rank <=last_rank ; rank++){ 
      //left
     for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
     {
       DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, 2);
       DDRSS_RCW_training(interface, rank, 1, clk_freq_in_khz, 1);
       for(fine_sweep = fine_init_val; fine_sweep < MAX_FINE_STEP; fine_sweep++)
       {
         DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_rd_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
     
     // right
     for(coarse_sweep = max_coarse; coarse_sweep > 0;)
     {
       -- coarse_sweep;
       DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface,coarse_sweep, 2);
       DDRSS_RCW_training(interface, rank, 1, clk_freq_in_khz, 1);
       for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;){
         -- fine_sweep;
         DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_rd_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
   }  // rank



  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(odd_even = 0; odd_even < 2; odd_even++)
    {
      //dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][odd_even] = 0;
      
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
        DDRSS_histogram_min_max_scan(_inst_,
          &pass_min_edge[byte_in_bus][bit][odd_even],
          &pass_max_edge[byte_in_bus][bit][odd_even],
          fail_count_histogram_perbit[byte_in_bus][bit][odd_even],
          histogram_size);
          
        perbit_mid[byte_in_bus][bit][odd_even] = ((cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]]
          + cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]]) / 2);
          //- fine_lut[DESKEW_CDC_INITIAL_VALUE];
    
        if(max_pass_min_edge[byte_in_bus] < cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]]) max_pass_min_edge[byte_in_bus] = cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]];
        if(min_pass_max_edge[byte_in_bus] > cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]]) min_pass_max_edge[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]];
        if(max_mid[byte_in_bus] < perbit_mid[byte_in_bus][bit][odd_even]) max_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit][odd_even];
        if(min_mid[byte_in_bus] > perbit_mid[byte_in_bus][bit][odd_even]) min_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit][odd_even];
        if(min_eye_width[byte_in_bus] > (cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]] - cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]])) min_eye_width[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]] - cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]];
      }
    }
    
    skew_range[byte_in_bus] = max_mid[byte_in_bus] - min_mid[byte_in_bus];
    eye_width[byte_in_bus] = min_pass_max_edge[byte_in_bus] - max_pass_min_edge[byte_in_bus];
    skew_range[byte_in_bus] = skew_range[byte_in_bus]; 
    eye_width[byte_in_bus] = eye_width[byte_in_bus];
    
    /*DDRSS_look_up_table_full_cdc(_inst_, (max_mid[byte_in_bus] + min_mid[byte_in_bus]) / 2, clk_freq_in_khz, dq_read_training[interface][phy_in_bus][byte_in_phy]);
		
    programed_mid[byte_in_bus] = cdc_lut[((dq_read_training[interface][phy_in_bus][byte_in_phy][delay] >> 3) * MAX_FINE_STEP)
      + (dq_read_training[interface][phy_in_bus][byte_in_phy][delay] & 0x07)];
    
    for(odd_even = 0; odd_even < 2; odd_even++)
    {
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
        perbit_delay[byte_in_bus][bit][odd_even] = programed_mid[byte_in_bus] - perbit_mid[byte_in_bus][bit][odd_even][0];   
        for(deskew_delay_value = 0; deskew_delay_value < 7; deskew_delay_value++)
        {
          //if(fine_lut[deskew_delay_value + 1] > perbit_delay[byte_in_bus][bit][odd_even]) break;
          if((fine_lut[deskew_delay_value + 1] - perbit_delay[byte_in_bus][bit][odd_even]) < (perbit_delay[byte_in_bus][bit][odd_even] - fine_lut[deskew_delay_value]))
          {
            deskew_delay_value++;
          }
          break;
        }
        perbit_cdc[byte_in_bus][bit][odd_even] = deskew_delay_value;
        perbit_cdc[byte_in_bus][bit][odd_even] = perbit_cdc[byte_in_bus][bit][odd_even];
        dq_read_perbit_training[interface][phy_in_bus][byte_in_phy][odd_even] += (deskew_delay_value << (bit * 4));
      } // bit
    } // odd_even*/
  }	// byte_in_bus
  
    dq_read_perbit_dm_training[interface][0] = 0x0;
	dq_read_perbit_dm_training[interface][1] = 0x0;
#ifdef NUM_BYTES_8
	dq_read_perbit_dm_training[interface][2] = 0x0;
	dq_read_perbit_dm_training[interface][3] = 0x0;
#endif
  
  DDRSS_dq_rd_set_dq_cdc_rd90_value(_inst_, interface, dq_read_training, dq_read_perbit_training, dq_read_perbit_dm_training);
  
  print_table_header();

  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(odd_even = 0; odd_even < 2; odd_even++)
    {
      for(bit = 0; bit < BYTE_SIZE; bit++)
      {
     
		snprintf(logbuf, DDR_LOG_BUF_LEN_MAX, "         RD_DBG,%15d,%15d,%15d,%15d,%15d,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,               ,               ,          ", 	
          interface,
          phy_in_bus,
          byte_in_phy,
          bit,
          odd_even,
          cdc_lut[pass_min_edge[byte_in_bus][bit][odd_even]],
          cdc_lut[pass_max_edge[byte_in_bus][bit][odd_even]],
          perbit_mid[byte_in_bus][bit][odd_even]);
          //perbit_delay[byte_in_bus][bit][odd_even],
         // perbit_cdc[byte_in_bus][bit][odd_even]);
        
        for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
        {
     
		   snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[byte_in_bus][bit][odd_even][delay_step]);
        } // delay_step
        
		 ddr_printf(DDR_DETAIL,"%s", logbuf);
      } // bit
    } // odd_even
    
    ddr_printf(DDR_NORMAL, "     RD_DBG_SUM,%15d,%15d,%15d,               ,               ,%15d,%15d,               ,%15d,%15d,%15d\n",
      interface,
      phy_in_bus,
      byte_in_phy,
      min_mid[byte_in_bus],
      max_mid[byte_in_bus],
      min_eye_width[byte_in_bus],
      skew_range[byte_in_bus],
      eye_width[byte_in_bus]);
  }	// byte_in_bus
#endif

return(min_eye_width_overbytes);

 } // DDRSS_ddr_phy_dq_rd_training_ddrss

void clear_histogram(void)
{
  uint8
    byte_in_bus,
    odd_even,
    bit,
    delay_step;
  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
      for(odd_even = 0; odd_even < 2; odd_even++)
      {
        for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
        {
          fail_count_histogram_perbit[byte_in_bus][bit][odd_even][delay_step] = 0;
        }
      }
    }
  }
}
 
void print_table_header(void)
{
  #ifdef ENABLE_PRINTF
  
  uint8 
    header_row,
    summary_column,
    delay_step;
    
  char summary_header[17][15] = {
    "phase",
    "channel",
    "phy_in_bus",
    "byte_in_phy",
    "bit",
    "odd_even",
    "min_mid",
    "max_mid",
    "programmed_mid",
    "min_eye_width",
    "skew_range",
    "eye_width",
    "pass_min_edge",
    "pass_max_edge",
    "perbit_mid",
    "perbit_delay",
    "perbit_cdc"};
    
	char table_header[4][10] = {
    "CDC Value",
    "Coarse",
    "Fine",
    "Delay"};
    
  static uint32 printed = 0;
  
  if(!printed)
  {
    for(header_row = 0; header_row < 4; header_row++)
    {
      for(summary_column = 0; summary_column < 17; summary_column++)
      {
        if(header_row == 3)
        {
			if(summary_column == 0 )
				snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,"%15s,", summary_header[summary_column]);
			else 
				snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%15s,", summary_header[summary_column]);
        }
        else
        {
          if(summary_column == 0 )
			 snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,"               ,");
	      else
			snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"               ,");
        }
      }
 
      snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%10s,", table_header[header_row]);
      for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
      {
        switch(header_row)
        {
          case 0:
          
			 snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%4x,", (delay_step / MAX_FINE_STEP << 3) + (delay_step % MAX_FINE_STEP));
            break;
          case 1:
          
			snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%4d,", delay_step / MAX_FINE_STEP);
            break;
          case 2:
           
			snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%4d,", delay_step % MAX_FINE_STEP);
            break;
          case 3:
           
			snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),"%4d,", cdc_lut[delay_step]);
            break;
        }
      }
  
	     ddr_printf(DDR_DETAIL,"%s", logbuf);
    }
  }
  
  printed = 1;
  
  #endif
  
}


//**********************************************************************************************************************************************************************************************************************

//**********************************************************************************************************************************************************************************************************************
void DDRSS_dq_set_all_dq_cdc_rd90_to_coarse_fine( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 coarse, uint16 fine )
{ 
 if(interface==0){
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, coarse , fine );
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, coarse , fine );
#endif
}

#ifdef CHNL_CNT_2
 if(interface==1){
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, coarse , fine );
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_rd90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, coarse , fine );
#endif
  }
#endif
  
}
//**********************************************************************************************************************************************************************************************************************
 void DDRSS_dq_rd_set_dq_cdc_rd90_value( uint32 _inst_,SDRAM_INTERFACE  interface,  uint16 dq_read_training[2][4][2][4], uint32 dq_read_perbit_training[2][4][2][2], uint32 dq_read_perbit_dm_training[2][4])
{ 
 if(interface==0){
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, dq_read_training[0][0]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, dq_read_training[0][1]  );
       DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][0], dq_read_perbit_dm_training[0][0] );
       DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][1], dq_read_perbit_dm_training[0][1] );

#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, dq_read_training[0][2]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, dq_read_training[0][3]  );
      DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][2], dq_read_perbit_dm_training[0][2] );
      DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][3], dq_read_perbit_dm_training[0][3] );
#endif
}

#ifdef CHNL_CNT_2

 if(interface==1){
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, dq_read_training[1][0]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, dq_read_training[1][1]  );
       DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][0], dq_read_perbit_dm_training[1][0] );
       DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][1], dq_read_perbit_dm_training[1][1] );
  #ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, dq_read_training[1][2]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, dq_read_training[1][3]  );
      DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][2], dq_read_perbit_dm_training[1][2] );
      DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][3], dq_read_perbit_dm_training[1][3] );
  #endif
  }
#endif
  
}

//**********************************************************************************************************************************************************************************************************************
void DDRSS_dq_rd_training_set_trained_results( uint32 _inst_,  uint16 dq_read_training[2][4][2][4], uint32 dq_read_perbit_training[2][4][2][2], uint32 dq_read_perbit_dm_training[2][4])
{
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET,dq_read_training[0][0]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET,dq_read_training[0][1]  );
    DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][0], dq_read_perbit_dm_training[0][0] );
    DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][1], dq_read_perbit_dm_training[0][1] );


#ifdef CHNL_CNT_2
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET,dq_read_training[1][0]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET,dq_read_training[1][1]  );
    DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][0], dq_read_perbit_dm_training[1][0] );
    DDRPHY_DQ_set_rd_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][1], dq_read_perbit_dm_training[1][1] );
#endif

#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET,  dq_read_training[0][2]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET,  dq_read_training[0][3]  );
    DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][2], dq_read_perbit_dm_training[0][2] );
    DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, dq_read_perbit_training[0][3], dq_read_perbit_dm_training[0][3] );

  #ifdef CHNL_CNT_2
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET,  dq_read_training[1][2]  );
    DDRPHY_DQ_set_dq_cdc_rd90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET,  dq_read_training[1][3]  );
    DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][2], dq_read_perbit_dm_training[1][2] );
    DDRPHY_DQ_set_rd_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, dq_read_perbit_training[1][3], dq_read_perbit_dm_training[1][3] );
  #endif
#endif
} /// function DDRSS_ddr_phy_dq_rd_training_restore

//common Sequences
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
uint32 DDRSS_dq_rd_training_histogram_update(SDRAM_INTERFACE  interface, uint8 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32 expected[DQ_TRAINING_PATTERN_SIZE], uint8 fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE])
{
  uint8
    odd_even,
    word_in_bus,
    word_address,
    byte_in_word,
    byte_in_bus,
    remapped_byte_in_bus,
    bit_in_byte,
    remapped_bit_in_byte;
  uint32
    fail_in_word,
    failed_always=ALL_DQ_FAIL_PAT,
    failed_once=ALL_DQ_PASS_PAT;
  
  for(odd_even = 0; odd_even < 2; odd_even++)
  {
    for(word_in_bus = 0; word_in_bus < WORD_PER_BUS; word_in_bus++)
    {
      fail_in_word = ALL_DQ_PASS_PAT;
      for(word_address = (WORD_PER_BUS * odd_even) + word_in_bus; word_address < DQ_TRAINING_PATTERN_SIZE; word_address += (WORD_PER_BUS * 2))
      {
        fail_in_word |= read[word_address] ^ expected[word_address];
        if(fail_in_word == ALL_DQ_FAIL_PAT) break;
      }
      for(byte_in_word = 0; byte_in_word < BYTE_PER_WORD; byte_in_word++)
      {
        byte_in_bus = (BYTE_PER_WORD * word_in_bus) + byte_in_word;
        remapped_byte_in_bus = dq_remap_lut[interface][byte_in_bus];
        for(bit_in_byte = 0; bit_in_byte < BYTE_SIZE; bit_in_byte++)
        {
          remapped_bit_in_byte = bit_remap_lut[remapped_byte_in_bus][bit_in_byte];
          if(((fail_in_word >> (byte_in_word * BYTE_SIZE)) >> bit_in_byte) & 0x1) {fail_count_histogram_perbit[remapped_byte_in_bus][remapped_bit_in_byte][odd_even][delay_step]++;}
        }
      }
      failed_once |= fail_in_word;
      failed_always &= fail_in_word;
    }
  }
  
  if(failed_once == ALL_DQ_PASS_PAT) return failed_once;
  else if(failed_always == ALL_DQ_FAIL_PAT) return failed_always;
  else return 0x55555555;
}

// ***********************************************************************
///  histogram post processing to find min (a) and max(b) cdc value to find middle of the eye
///  min, max are pointer to return calculated mi and max
// ***********************************************************************
void DDRSS_histogram_min_max_scan(uint32 _inst_, uint8 *min, uint8 *max, uint8 fail_count_histogram[FAIL_HISTOGRAM_SIZE], uint8 histogram_size)
{
	uint8 lenght_cur = 0;
	uint8 min_cur = 0;
	uint8 max_cur = 0;
	uint8 lenght_max = 0;
	uint8 sweep;

	*max = 0;
	*min = 0;
	
	for(sweep = 1; sweep < histogram_size - 1; sweep++)
  {
		if((fail_count_histogram[sweep - 1] != 0) && (fail_count_histogram[sweep] == 0))
    {
			min_cur = sweep;
			max_cur = sweep;
			lenght_cur = 0;
		}
		if((fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] != 0))
    {
			max_cur = sweep;
			lenght_cur = max_cur - min_cur + 1;
		}
		if((sweep == histogram_size - 2) && (fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] == 0))
    {
			max_cur = sweep + 1;
			lenght_cur = max_cur - min_cur + 1;
		}
		if(lenght_cur > lenght_max)
    {
			lenght_max = lenght_cur;
			*min = min_cur;
			*max = max_cur;
		}
	}
}

void DDRSS_look_up_table_full_cdc(uint32 _inst_, uint16 middle, uint32 clk_freq_in_khz, uint16 full_cdc_delay[4])
{
  uint8
    fine_delay,
    fine_cdc,
    coarse_cdc;
  uint16
    mid_point[4],
    i;
  
  if(clk_freq_in_khz >= FPM_DELAY0_FMIN_IN_KHZ)
  {
    mid_point[0] = middle;
    mid_point[1] = clk_freq_in_khz * middle / FPM_DELAY1_FMAX_IN_KHZ;
    mid_point[2] = clk_freq_in_khz * middle / FPM_DELAY2_FMAX_IN_KHZ;
    mid_point[3] = clk_freq_in_khz * middle / FPM_DELAY3_FMAX_IN_KHZ;
  }
  else if(clk_freq_in_khz >= FPM_DELAY1_FMIN_IN_KHZ)
  {
    mid_point[0] = clk_freq_in_khz * middle / FPM_DELAY0_FMAX_IN_KHZ;
    mid_point[1] = middle;
    mid_point[2] = clk_freq_in_khz * middle / FPM_DELAY2_FMAX_IN_KHZ;
    mid_point[3] = clk_freq_in_khz * middle / FPM_DELAY3_FMAX_IN_KHZ;
  }
  else if(clk_freq_in_khz >= FPM_DELAY2_FMIN_IN_KHZ)
  {
    mid_point[0] = clk_freq_in_khz * middle / FPM_DELAY0_FMAX_IN_KHZ;
    mid_point[1] = clk_freq_in_khz * middle / FPM_DELAY1_FMAX_IN_KHZ;
    mid_point[2] = middle;
    mid_point[3] = clk_freq_in_khz * middle / FPM_DELAY3_FMAX_IN_KHZ;
  }
  else
  {
    mid_point[0] = clk_freq_in_khz * middle / FPM_DELAY0_FMAX_IN_KHZ;
    mid_point[1] = clk_freq_in_khz * middle / FPM_DELAY1_FMAX_IN_KHZ;
    mid_point[2] = clk_freq_in_khz * middle / FPM_DELAY2_FMAX_IN_KHZ;
    mid_point[3] = middle;
  }
    
  for(i = 0; i < 4; i++)
  {
    coarse_cdc = mid_point[i] / COARSE_STEP;
    fine_delay = mid_point[i] % COARSE_STEP;

    for(fine_cdc = 0; fine_cdc < 6;  fine_cdc++)
    {
      if(fine_lut[fine_cdc] >= fine_delay)
      {
        break;
      }
    }
    
    full_cdc_delay[i] = (coarse_cdc << 3) | fine_cdc;
  }
}  

//common Sequences
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************































uint16 DDRSS_ddr_phy_dq_wr_training_ddrss( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 chip_select, uint16 fine_delay_training, uint16
per_bit_deskew_training, uint16 max_loop_count, uint16 coarse_init_val, uint16 fine_init_val, uint16 per_bit_deskew_init_val,
uint16 dq_write_training[2][4][2][4],  uint32 dq_write_perbit_training[2][4][2],  uint32 dq_write_perbit_dm_training[2][4],
uint32 clk_freq_in_khz )
{
  uint8
    deskew_delay_value=0,
    bit=0,
    byte_in_bus=0,
    phy_in_bus=0,
    byte_in_phy=0,
    histogram_size=0,
    max_coarse=0,
    coarse_sweep=0,
    fine_sweep=0,
    loop_count=0,
    delay_step=0,
    delay=0,
    pass_min_edge[BYTE_PER_BUS][BYTE_SIZE]={0},
    pass_max_edge[BYTE_PER_BUS][BYTE_SIZE]={0};
  uint16
    rank,first_rank,last_rank,
    training_period_1ps=0,
    perbit_mid[BYTE_PER_BUS][BYTE_SIZE]={0},
    perbit_delay[BYTE_PER_BUS][BYTE_SIZE]={0},
    perbit_cdc[BYTE_PER_BUS][BYTE_SIZE]={0},
    max_pass_min_edge[BYTE_PER_BUS]={0},
    min_pass_max_edge[BYTE_PER_BUS]={0},
    min_mid[BYTE_PER_BUS]={0},
    max_mid[BYTE_PER_BUS]={0},
    min_eye_width[BYTE_PER_BUS]={0},
    min_eye_width_overbytes=0,
    programed_mid[BYTE_PER_BUS]={0},
    skew_range[BYTE_PER_BUS]={0},
    eye_width[BYTE_PER_BUS]={0};
  uint32
    allbit=0,
    read_data[DQ_TRAINING_PATTERN_SIZE]={0};
  
  delay_step = delay_step;
  training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse = (((training_period_1ps / 2) + fine_lut[DESKEW_CDC_INITIAL_VALUE] + 100) / COARSE_STEP) + 2;
  if(max_coarse >= MAX_COARSE_STEP) max_coarse = MAX_COARSE_STEP-1;
  histogram_size = max_coarse * MAX_FINE_STEP;
  if(max_loop_count > 255) max_loop_count = 255;
  
  if     (clk_freq_in_khz >= FPM_DELAY0_FMIN_IN_KHZ)  { delay = 0;}
  else if(clk_freq_in_khz >= FPM_DELAY1_FMIN_IN_KHZ)  { delay = 1;}
  else if(clk_freq_in_khz >= FPM_DELAY2_FMIN_IN_KHZ)  { delay = 2;}
  else                                                { delay = 3;}
  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus  = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    dq_write_perbit_dm_training[interface][phy_in_bus] = DM_CSR_INITIAL;
    dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] = PER_BIT_CSR_INITIAL;
    
    max_pass_min_edge[byte_in_bus] = 0;
    max_mid[byte_in_bus] = 0;
    min_pass_max_edge[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_mid[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_eye_width[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
  } // byte_in_bus
    min_eye_width_overbytes = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];

  clear_histogram();
  
  DDRSS_dq_wr_set_dq_cdc_wr90_value(  _inst_, interface,   dq_write_training,  dq_write_perbit_training,  dq_write_perbit_dm_training);
 

  if ( chip_select == SDRAM_RANK_BOTH ){//dual  rank
     first_rank = SDRAM_RANK_CS0;
     last_rank  = SDRAM_RANK_CS1;
  }
  else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
  }
  for ( rank=first_rank ; rank <=last_rank ; rank++){ 
   
    // left
    for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
    {
      for(fine_sweep = fine_init_val; fine_sweep < MAX_FINE_STEP; fine_sweep++)
      {
        DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
        for(loop_count = 0; loop_count < max_loop_count; loop_count++)
        {
          ddr_mem_write_high_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
          ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
          allbit = DDRSS_dq_wr_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
          if(allbit == ALL_DQ_FAIL_PAT) break;
        } // loop_count
        if(allbit == ALL_DQ_PASS_PAT) break;
      } // fine_sweep
      if(allbit == ALL_DQ_PASS_PAT) break;
    } // coarse_sweep
    
    // right
    for(coarse_sweep = max_coarse; coarse_sweep > 0;)
    {
      -- coarse_sweep;
      for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;){
        -- fine_sweep;
        DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
        for(loop_count = 0; loop_count < max_loop_count; loop_count++)
        {
          ddr_mem_write_high_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
          ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
          allbit = DDRSS_dq_wr_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
          if(allbit == ALL_DQ_FAIL_PAT) break;
        } // loop_count
        if(allbit == ALL_DQ_PASS_PAT) break;
      } // fine_sweep
      if(allbit == ALL_DQ_PASS_PAT) break;
    } // coarse_sweep
  }//rank  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] = 0;
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
      DDRSS_histogram_min_max_scan(_inst_,
        &pass_min_edge[byte_in_bus][bit],
        &pass_max_edge[byte_in_bus][bit],
        fail_count_histogram_perbit[byte_in_bus][bit][0],
        histogram_size);
        
      perbit_mid[byte_in_bus][bit] = ((cdc_lut[pass_min_edge[byte_in_bus][bit]]
        + cdc_lut[pass_max_edge[byte_in_bus][bit]]) / 2)
        - fine_lut[DESKEW_CDC_INITIAL_VALUE];
  
      if(max_pass_min_edge[byte_in_bus] < cdc_lut[pass_min_edge[byte_in_bus][bit]]) {  max_pass_min_edge[byte_in_bus] = cdc_lut[pass_min_edge[byte_in_bus][bit]]  ;}
      if(min_pass_max_edge[byte_in_bus] > cdc_lut[pass_max_edge[byte_in_bus][bit]]) {  min_pass_max_edge[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit]]  ;}
      if(          max_mid[byte_in_bus] < perbit_mid[byte_in_bus][bit]            ) {  max_mid[byte_in_bus]           = perbit_mid[byte_in_bus][bit]  ;}
      if(          min_mid[byte_in_bus] > perbit_mid[byte_in_bus][bit]            ) {  min_mid[byte_in_bus]           = perbit_mid[byte_in_bus][bit]  ;}
      if(    min_eye_width[byte_in_bus] > (cdc_lut[pass_max_edge[byte_in_bus][bit]] - cdc_lut[pass_min_edge[byte_in_bus][bit]])){
             min_eye_width[byte_in_bus] =  cdc_lut[pass_max_edge[byte_in_bus][bit]] - cdc_lut[pass_min_edge[byte_in_bus][bit]];
      }
    }
    
    skew_range[byte_in_bus] = max_mid[byte_in_bus] - min_mid[byte_in_bus];
    eye_width[byte_in_bus] = min_pass_max_edge[byte_in_bus] - max_pass_min_edge[byte_in_bus];
    skew_range[byte_in_bus] = skew_range[byte_in_bus]; 
    eye_width[byte_in_bus] = eye_width[byte_in_bus];
    if(min_eye_width_overbytes > eye_width[byte_in_bus]) {      min_eye_width_overbytes = eye_width[byte_in_bus];    }
    
    DDRSS_look_up_table_full_cdc(_inst_, max_mid[byte_in_bus], clk_freq_in_khz, dq_write_training[interface][phy_in_bus][byte_in_phy]);
    
    programed_mid[byte_in_bus] = cdc_lut[((dq_write_training[interface][phy_in_bus][byte_in_phy][delay] >> 3) * MAX_FINE_STEP)
      + (dq_write_training[interface][phy_in_bus][byte_in_phy][delay] & 0x07)];
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
      perbit_delay[byte_in_bus][bit] = programed_mid[byte_in_bus] - perbit_mid[byte_in_bus][bit];   
      for(deskew_delay_value = 0; deskew_delay_value < 7; deskew_delay_value++)
      {
        if(fine_lut[deskew_delay_value + 1] > perbit_delay[byte_in_bus][bit])
        {
          if((fine_lut[deskew_delay_value + 1] - perbit_delay[byte_in_bus][bit]) < (perbit_delay[byte_in_bus][bit] - fine_lut[deskew_delay_value]))
          {
            deskew_delay_value++;
          }
          break;
        }
      }
      perbit_cdc[byte_in_bus][bit] = deskew_delay_value;
      perbit_cdc[byte_in_bus][bit] = perbit_cdc[byte_in_bus][bit];
      dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] += (deskew_delay_value << (bit * 4));
    } // bit
  }	// byte_in_bus
  
    dq_write_perbit_dm_training[interface][0] = 0x0;
	dq_write_perbit_dm_training[interface][1] = 0x0;
#ifdef NUM_BYTES_8
	dq_write_perbit_dm_training[interface][2] = 0x0;
	dq_write_perbit_dm_training[interface][3] = 0x0;
#endif

  DDRSS_dq_wr_set_dq_cdc_wr90_value(_inst_, interface, dq_write_training, dq_write_perbit_training, dq_write_perbit_dm_training);

#ifdef ENABLE_PRINTF
  print_table_header();
    
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
   
        snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,"             WR,%15d,%15d,%15d,%15d,               ,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,%15d,%15d,          ",
        interface,
        phy_in_bus,
        byte_in_phy,
        bit,
        cdc_lut[pass_min_edge[byte_in_bus][bit]],
        cdc_lut[pass_max_edge[byte_in_bus][bit]],
        perbit_mid[byte_in_bus][bit],
        perbit_delay[byte_in_bus][bit],
        perbit_cdc[byte_in_bus][bit]);
      
      for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
      {
   
		snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[byte_in_bus][bit][0][delay_step]);
      } // delay_step
     
	ddr_printf(DDR_DETAIL,"%s", logbuf);
    } // bit
    
    ddr_printf(DDR_NORMAL, "         WR_SUM,%15d,%15d,%15d,               ,               ,%15d,%15d,%15d,%15d,%15d,%15d\n",
      interface,
      phy_in_bus,
      byte_in_phy,
      min_mid[byte_in_bus],
      max_mid[byte_in_bus],
      programed_mid[byte_in_bus],
      min_eye_width[byte_in_bus],
      skew_range[byte_in_bus],
      eye_width[byte_in_bus]);
  }	// byte_in_bus
#endif

#ifdef ENABLE_PRINTF_WR_DBG
  //max_coarse = ((training_period_1ps / 2) + 100) / COARSE_STEP;
  histogram_size = max_coarse * MAX_FINE_STEP;
  
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus  = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    //dq_write_perbit_dm_training[interface][phy_in_bus] = DM_CSR_INITIAL;
    //dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] = PER_BIT_CSR_INITIAL;
    
    max_pass_min_edge[byte_in_bus] = 0;
    max_mid[byte_in_bus] = 0;
    min_pass_max_edge[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_mid[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
    min_eye_width[byte_in_bus] = cdc_lut[FAIL_HISTOGRAM_SIZE - 1];
  } // byte_in_bus
  
  clear_histogram();
  
  //DDRSS_dq_wr_set_dq_cdc_wr90_value(  _inst_, interface,   dq_write_training,  dq_write_perbit_training,  dq_write_perbit_dm_training);
  if ( chip_select == SDRAM_RANK_BOTH ){//dual  rank
     first_rank = SDRAM_RANK_CS0;
     last_rank  = SDRAM_RANK_CS1;
  }
  else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
  }
  for ( rank=first_rank ; rank <=last_rank ; rank++){ 
  
     // left
     for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
     {
       for(fine_sweep = fine_init_val; fine_sweep < MAX_FINE_STEP; fine_sweep++)
       {
         DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_write_high_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_wr_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
     
     // right
     for(coarse_sweep = max_coarse; coarse_sweep > 0;)
     {
       -- coarse_sweep;
       for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;){
         -- fine_sweep;
         DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine(_inst_, interface, coarse_sweep, fine_sweep);
         for(loop_count = 0; loop_count < max_loop_count; loop_count++)
         {
           ddr_mem_write_high_speed(interface, rank, dq_training_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
           ddr_mem_read(interface, rank, read_data, DQ_TRAINING_PATTERN_SIZE, 1);
           allbit = DDRSS_dq_wr_training_histogram_update(interface, (coarse_sweep * MAX_FINE_STEP) + fine_sweep, read_data, dq_training_pattern, fail_count_histogram_perbit);
           if(allbit == ALL_DQ_FAIL_PAT) break;
         } // loop_count
         if(allbit == ALL_DQ_PASS_PAT) break;
       } // fine_sweep
       if(allbit == ALL_DQ_PASS_PAT) break;
     } // coarse_sweep
   }//rank  


  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    //dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] = 0;
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
      DDRSS_histogram_min_max_scan(_inst_,
        &pass_min_edge[byte_in_bus][bit],
        &pass_max_edge[byte_in_bus][bit],
        fail_count_histogram_perbit[byte_in_bus][bit][0],
        histogram_size);
        
      perbit_mid[byte_in_bus][bit] = ((cdc_lut[pass_min_edge[byte_in_bus][bit]]
        + cdc_lut[pass_max_edge[byte_in_bus][bit]]) / 2);
        //- fine_lut[DESKEW_CDC_INITIAL_VALUE];
  
      if(max_pass_min_edge[byte_in_bus] < cdc_lut[pass_min_edge[byte_in_bus][bit]]) max_pass_min_edge[byte_in_bus] = cdc_lut[pass_min_edge[byte_in_bus][bit]];
      if(min_pass_max_edge[byte_in_bus] > cdc_lut[pass_max_edge[byte_in_bus][bit]]) min_pass_max_edge[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit]];
      if(max_mid[byte_in_bus] < perbit_mid[byte_in_bus][bit]) max_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit];
      if(min_mid[byte_in_bus] > perbit_mid[byte_in_bus][bit]) min_mid[byte_in_bus] = perbit_mid[byte_in_bus][bit];
      if(min_eye_width[byte_in_bus] > (cdc_lut[pass_max_edge[byte_in_bus][bit]] - cdc_lut[pass_min_edge[byte_in_bus][bit]])) min_eye_width[byte_in_bus] = cdc_lut[pass_max_edge[byte_in_bus][bit]] - cdc_lut[pass_min_edge[byte_in_bus][bit]];
    }
    
    skew_range[byte_in_bus] = max_mid[byte_in_bus] - min_mid[byte_in_bus];
    eye_width[byte_in_bus] = min_pass_max_edge[byte_in_bus] - max_pass_min_edge[byte_in_bus];
    skew_range[byte_in_bus] = skew_range[byte_in_bus]; 
    eye_width[byte_in_bus] = eye_width[byte_in_bus];
    
    /*DDRSS_look_up_table_full_cdc(_inst_, (max_mid[byte_in_bus] + min_mid[byte_in_bus]) / 2, clk_freq_in_khz, dq_write_training[interface][phy_in_bus][byte_in_phy]);
    
    programed_mid[byte_in_bus] = cdc_lut[((dq_write_training[interface][phy_in_bus][byte_in_phy][delay] >> 3) * MAX_FINE_STEP)
      + (dq_write_training[interface][phy_in_bus][byte_in_phy][delay] & 0x07)];
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
      perbit_delay[byte_in_bus][bit] = programed_mid[byte_in_bus] - perbit_mid[byte_in_bus][bit];   
      for(deskew_delay_value = 0; deskew_delay_value < 7; deskew_delay_value++)
      {
        if(fine_lut[deskew_delay_value + 1] > perbit_delay[byte_in_bus][bit]) break;
      }
      perbit_cdc[byte_in_bus][bit] = deskew_delay_value;
      perbit_cdc[byte_in_bus][bit] = perbit_cdc[byte_in_bus][bit];
      dq_write_perbit_training[interface][phy_in_bus][byte_in_phy] += (deskew_delay_value << (bit * 4));
    } // bit*/
  }	// byte_in_bus
  
    dq_write_perbit_dm_training[interface][0] = 0x0;
	dq_write_perbit_dm_training[interface][1] = 0x0;
#ifdef NUM_BYTES_8
	dq_write_perbit_dm_training[interface][2] = 0x0;
	dq_write_perbit_dm_training[interface][3] = 0x0;
#endif

  DDRSS_dq_wr_set_dq_cdc_wr90_value(_inst_, interface, dq_write_training, dq_write_perbit_training, dq_write_perbit_dm_training);

  print_table_header();
    
  for(byte_in_bus = 0; byte_in_bus < BYTE_PER_BUS; byte_in_bus++)
  {
    phy_in_bus = byte_in_bus / BYTE_PER_PHY;
    byte_in_phy = byte_in_bus % BYTE_PER_PHY;
    
    for(bit = 0; bit < BYTE_SIZE; bit++)
    {
     
       snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,"         WR_DBG,%15d,%15d,%15d,%15d,               ,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,               ,               ,          ",      
        interface,
        phy_in_bus,
        byte_in_phy,
        bit,
        cdc_lut[pass_min_edge[byte_in_bus][bit]],
        cdc_lut[pass_max_edge[byte_in_bus][bit]],
        perbit_mid[byte_in_bus][bit]);
        //perbit_delay[byte_in_bus][bit],
        //perbit_cdc[byte_in_bus][bit]);
      
      for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
      {
    
		 snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[byte_in_bus][bit][0][delay_step]);
      } // delay_step
      
    
	 ddr_printf(DDR_DETAIL,"%s", logbuf);
    } // bit
    ddr_printf(DDR_DETAIL,"     WR_DBG_SUM,%15d,%15d,%15d,               ,               ,%15d,%15d,               ,%15d,%15d,%15d\n",
      interface,
      phy_in_bus,
      byte_in_phy,
      min_mid[byte_in_bus],
      max_mid[byte_in_bus],
      min_eye_width[byte_in_bus],
      skew_range[byte_in_bus],
      eye_width[byte_in_bus]);
  }	// byte_in_bus
#endif
return(min_eye_width_overbytes);

} // DDRSS_ddr_phy_dq_wr_training_ddrss

//**********************************************************************************************************************************************************************************************************************
uint32 DDRSS_dq_wr_training_histogram_update( SDRAM_INTERFACE  interface,uint16 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32  expected[DQ_TRAINING_PATTERN_SIZE] , uint8  fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE])
{
 uint32 failed_always=ALL_DQ_FAIL_PAT,
        failed_once=ALL_DQ_PASS_PAT,
        fail_in_word,
        word_in_bus,
        word_address,
        byte_in_word,
        byte_in_bus,
        remapped_byte_in_bus,
        bit_in_byte,
        remapped_bit_in_byte;
        
  for(word_in_bus = 0; word_in_bus < WORD_PER_BUS; word_in_bus++)
  {
    fail_in_word = 0;
    for(word_address = word_in_bus; word_address < DQ_TRAINING_PATTERN_SIZE; word_address += WORD_PER_BUS)
    {
      fail_in_word |= read[word_address] ^ expected[word_address];
      if (fail_in_word == ALL_DQ_FAIL_PAT) break;
    }
    for(byte_in_word = 0; byte_in_word < BYTE_PER_WORD; byte_in_word++)
    {
      byte_in_bus = (BYTE_PER_WORD * word_in_bus) + byte_in_word;
      remapped_byte_in_bus = dq_remap_lut[interface][byte_in_bus];
      for(bit_in_byte = 0; bit_in_byte < BYTE_SIZE; bit_in_byte++)
      {
        remapped_bit_in_byte = bit_remap_lut[remapped_byte_in_bus][bit_in_byte];
        if (((fail_in_word >> (byte_in_word * BYTE_SIZE)) >> bit_in_byte) & 0x1) {fail_count_histogram_perbit[remapped_byte_in_bus][remapped_bit_in_byte][0][delay_step]++;}
      }
    }
    failed_once |= fail_in_word;
    failed_always &= fail_in_word;
  }
  
  if(failed_once == ALL_DQ_PASS_PAT) return failed_once;
  else if(failed_always == ALL_DQ_FAIL_PAT) return failed_always;
  else return 0x55555555;
}
//**********************************************************************************************************************************************************************************************************************
void DDRSS_dq_set_all_dq_cdc_wr90_to_coarse_fine( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 coarse, uint16 fine )
{ 
 if(interface==0){
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, coarse , fine );
#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, coarse , fine );
#endif
}

#ifdef CHNL_CNT_2

 if(interface==1){
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, coarse , fine );
  #ifdef NUM_BYTES_8
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, coarse , fine );
    DDRPHY_DQ_set_all_dq_cdc_wr90_to_coarse_fine( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, coarse , fine );
  #endif
  }
#endif

}
//**********************************************************************************************************************************************************************************************************************
 void DDRSS_dq_wr_set_dq_cdc_wr90_value( uint32 _inst_,SDRAM_INTERFACE  interface,  uint16 dq_write_training[2][4][2][4], uint32 dq_write_perbit_training[2][4][2], uint32 dq_write_perbit_dm_training[2][4])
{ 
 if(interface==0){
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET,dq_write_training[0][0]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET,dq_write_training[0][1]  );

    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][0], dq_write_perbit_dm_training[0][0] );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][1], dq_write_perbit_dm_training[0][1] );

#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET,  dq_write_training[0][2]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET,  dq_write_training[0][3]  );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][2], dq_write_perbit_dm_training[0][2] );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][3], dq_write_perbit_dm_training[0][3] );
#endif
}

#ifdef CHNL_CNT_2

 if(interface==1){
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET,dq_write_training[1][0]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET,dq_write_training[1][1]  );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][0], dq_write_perbit_dm_training[1][0] );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][1], dq_write_perbit_dm_training[1][1] );
  #ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET,  dq_write_training[1][2]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET,  dq_write_training[1][3]  );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][2], dq_write_perbit_dm_training[1][2] );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][3], dq_write_perbit_dm_training[1][3] );
  #endif
  }
#endif  
}
//**********************************************************************************************************************************************************************************************************************
void DDRSS_dq_wr_training_set_trained_results( uint32 _inst_,  uint16 dq_write_training[2][4][2][4], uint32 dq_write_perbit_training[2][4][2], uint32 dq_write_perbit_dm_training[2][4])
{
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET,dq_write_training[0][0]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET,dq_write_training[0][1]  );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ0_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][0], dq_write_perbit_dm_training[0][0] );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ1_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][1], dq_write_perbit_dm_training[0][1] );
	
#ifdef CHNL_CNT_2
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET,dq_write_training[1][0]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET,dq_write_training[1][1]  );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ0_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][0], dq_write_perbit_dm_training[1][0] );
    DDRPHY_DQ_set_wr_deskew_value(_inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ1_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][1], dq_write_perbit_dm_training[1][1] );
#endif

#ifdef NUM_BYTES_8
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET,  dq_write_training[0][2]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET,  dq_write_training[0][3]  );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ2_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][2], dq_write_perbit_dm_training[0][2] );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH0_DQ3_DDRPHY_DQ_OFFSET, dq_write_perbit_training[0][3], dq_write_perbit_dm_training[0][3] );

  #ifdef CHNL_CNT_2
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET,  dq_write_training[1][2]  );
    DDRPHY_DQ_set_dq_cdc_wr90_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET,  dq_write_training[1][3]  );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ2_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][2], dq_write_perbit_dm_training[1][2] );
    DDRPHY_DQ_set_wr_deskew_value( _inst_ + SEQ_DDRSS_EBI1_PHY_CH1_DQ3_DDRPHY_DQ_OFFSET, dq_write_perbit_training[1][3], dq_write_perbit_dm_training[1][3] );
  #endif
#endif
} /// function DDRSS_ddr_phy_dq_wr_training_restore
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************















































uint16 ca_training_pattern[CA_PATTERN_NUM][6] = {
  { 0x0000, 0x0000, 0x03FF, 0x0000, 0x03FF, 0x03FF }, 
  { 0x03FF, 0x03FF, 0x0000, 0x03FF, 0x0000, 0x0000 }, 
  { 0x0155, 0x0155, 0x02aa, 0x0155, 0x02aa, 0x02aa }, 
  { 0x02aa, 0x02aa, 0x0155, 0x02aa, 0x0155, 0x0155 }};

// ***********************************************************************
/// No description is provided for this Sequence 
// ***********************************************************************
uint16 DDRSS_get_ca_exp_pattern(uint32 _inst_, uint16 ca_pat_rise, uint16 ca_pat_fall, uint16 ca_train_mapping)
{
	uint16 i;
	uint16 index0;
	uint16 index1;
	uint16 exp_pattern_ph0;
	uint16 exp_pattern_ph1;

	index0 = 0;
	index1 = 0;
	exp_pattern_ph0 = 0;
	exp_pattern_ph1 = 0;
	
	for (i = 0; i <= 9;  i ++)
  {
		if (i != 4 && i != 9 && ca_train_mapping == 0)
    {
			exp_pattern_ph0 = exp_pattern_ph0 | (((ca_pat_rise >> i) & 0x1) << index0);
			exp_pattern_ph0 = exp_pattern_ph0 | (((ca_pat_fall >> i) & 0x1) << index0 + 1);
			index0 = index0 + 2;
		}
		else
		{
			if(ca_train_mapping == 1 && (i == 4 || i == 9))
      {
				exp_pattern_ph1 = exp_pattern_ph1 | (((ca_pat_rise >> i) & 0x1) << index1);
				exp_pattern_ph1 = exp_pattern_ph1 | (((ca_pat_fall >> i) & 0x1) << index1 + 1);
				index1 = index1 + 8;
			}
		}
	}
	
	if(ca_train_mapping == 0)
  {
		return exp_pattern_ph0;
	}
	else
	{
		return exp_pattern_ph1;
	}
}

uint16 DDRSS_ca_training_histogram_update(uint8 ca_mapping, uint8 site, uint8 delay_step, uint16 read, uint16 expected, uint8 fail_count_histogram_perbit[BYTE_PER_BUS][BYTE_SIZE][2][FAIL_HISTOGRAM_SIZE])
{
  uint16 fail = 0;
  
  if(ca_mapping == 0)
  {
    if((read & 0x0003) != (expected & 0x0003)) {fail_count_histogram_perbit[site*2][0][0][delay_step] += 1; fail |= 0x0001;}
    if((read & 0x000C) != (expected & 0x000C)) {fail_count_histogram_perbit[site*2][1][0][delay_step] += 1; fail |= 0x0002;}
    if((read & 0x0030) != (expected & 0x0030)) {fail_count_histogram_perbit[site*2][2][0][delay_step] += 1; fail |= 0x0004;}
    if((read & 0x00C0) != (expected & 0x00C0)) {fail_count_histogram_perbit[site*2][3][0][delay_step] += 1; fail |= 0x0008;}
    if((read & 0x0300) != (expected & 0x0300)) {fail_count_histogram_perbit[site*2][5][0][delay_step] += 1; fail |= 0x0020;}
    if((read & 0x0C00) != (expected & 0x0C00)) {fail_count_histogram_perbit[site*2][6][0][delay_step] += 1; fail |= 0x0040;}
    if((read & 0x3000) != (expected & 0x3000)) {fail_count_histogram_perbit[site*2][7][0][delay_step] += 1; fail |= 0x0080;}
    if((read & 0xC000) != (expected & 0xC000)) {fail_count_histogram_perbit[(site*2)+1][0][0][delay_step] += 1; fail |= 0x0100;}
  }
  
  if (ca_mapping != 0){
    if((read & 0x0003) != (expected & 0x0003)) {fail_count_histogram_perbit[site*2][4][0][delay_step] += 1; fail |= 0x0010;}
    if((read & 0x0300) != (expected & 0x0300)) {fail_count_histogram_perbit[(site*2)+1][1][0][delay_step] += 1; fail |= 0x0200;}
  }
  
  return fail;
}

uint16 DDRSS_ddr_phy_ca_training(
  uint32 _inst_,
  SDRAM_INTERFACE interface,
  uint16 chip_select,
  uint16 fine_delay_training,
  uint16 per_bit_deskew_training,
  uint16 ca_training_full_cdc[2][2][4],
  uint32 ca_training_perbit[2][2][2],
  uint16 max_loop_count,
  uint16 coarse_init_val,
  uint16 fine_init_val,
  uint16 per_bit_deskew_init_val,
  uint32 clk_freq_in_khz)
{
  uint8
    bit=0,
    site=0,
    pattern=0,
    ca_mapping=0,
    histogram_size=0,
    max_coarse=0,
    coarse_sweep=0,
    fine_sweep=0,
    loop_count=0,
    delay_step=0,
    delay=0,
    pass_min_edge[10]={0},
    pass_max_edge[10]={0};
  uint16
    rank,first_rank,last_rank,
    deskew_delay_value=DESKEW_CDC_INITIAL_VALUE,
    ca_training_pattern_result_read=0,
    ca_training_expected=0,
    cs_rise_pat=0,
    cs_fall_pat=0,
    training_period_1ps=0,
    perbit_mid[10]={0},
    perbit_delay=0,
#ifdef ENABLE_PRINTF
	perbit_cdc[10]={0},
    skew_range=0,
    eye_width=0,
#endif
    max_pass_min_edge=0,
    min_pass_max_edge=cdc_lut[FAIL_HISTOGRAM_SIZE-1],
    min_mid=cdc_lut[FAIL_HISTOGRAM_SIZE-1],
    max_mid=0,
    min_eye_width=cdc_lut[FAIL_HISTOGRAM_SIZE-1],
    programed_mid=0,
    safe_ca_cdc_wr90_delays[4] = {0x18, 0x20, 0x30, 0x40};
  uint32    failed_once=0;
  
  delay_step = delay_step;
  training_period_1ps = 1000000000/clk_freq_in_khz;
  max_coarse = (((training_period_1ps / 2) + fine_lut[DESKEW_CDC_INITIAL_VALUE] + 100) / COARSE_STEP)+2;
  histogram_size = max_coarse * MAX_FINE_STEP;
  
  if(max_loop_count >= (256/CA_PATTERN_NUM)) max_loop_count = (256/CA_PATTERN_NUM) - 1;
  
  if     (clk_freq_in_khz >= FPM_DELAY0_FMIN_IN_KHZ)  { delay = 0;}
  else if(clk_freq_in_khz >= FPM_DELAY1_FMIN_IN_KHZ)  { delay = 1;}
  else if(clk_freq_in_khz >= FPM_DELAY2_FMIN_IN_KHZ)  { delay = 2;}
  else                                                { delay = 3;}
  
	clear_histogram();
	
	//EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 1);//ngamini
	
	
//************************************************************************************************************************************
//   Histogram extraction
//************************************************************************************************************************************	
if ( chip_select == SDRAM_RANK_BOTH ){//dual rank
    first_rank = SDRAM_RANK_CS0;
    last_rank  = SDRAM_RANK_CS1;
}
else {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
}
for ( rank=first_rank ; rank <=last_rank ; rank++)
{ 

    EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 1);

   for(site = 0; site < SITE_PER_CA_CNT;  site ++) 
   {
   		EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_perbit_deskew_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, deskew_delay_value);
   }
     
    for(pattern = 0; pattern < CA_PATTERN_NUM;  pattern ++)
	{
      for(ca_mapping = 0; ca_mapping < 2; ca_mapping ++)
	    {                
      for(site = 0; site < SITE_PER_CA_CNT; site++)
      {            
        EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, safe_ca_cdc_wr90_delays);
      }
          BIMC_Set_CA_Training_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, 
                                        interface,
                                        rank, 
                                        ca_mapping,
                                        ca_training_pattern[pattern][PRECS_RISE], 
                                        ca_training_pattern[pattern][PRECS_FALL], 
                                        ca_training_pattern[pattern][CS_RISE], 
                                        ca_training_pattern[pattern][CS_FALL], 
                                        ca_training_pattern[pattern][POSTCS_RISE], 
                                        ca_training_pattern[pattern][POSTCS_FALL]);
         cs_rise_pat = ca_training_pattern[pattern][CS_RISE];
         cs_fall_pat = ca_training_pattern[pattern][CS_FALL];
         ca_training_expected = DDRSS_get_ca_exp_pattern(_inst_ , cs_rise_pat, cs_fall_pat, ca_mapping);
        //********************************************************************************
        // left
        //********************************************************************************
         for(coarse_sweep = 0x0; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++){
           for(fine_sweep = 0x0; fine_sweep < MAX_FINE_STEP; fine_sweep++){
              for(site = 0; site < SITE_PER_CA_CNT; site++){
                 EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, coarse_sweep, fine_sweep);
              }
              failed_once = ALL_CA_PASS_PAT;
              for(loop_count = 0; loop_count < max_loop_count; loop_count++)             {
                 BIMC_CA_Training_Toggle_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface);
                 for(site = 0; site < SITE_PER_CA_CNT;  site ++){
                    ca_training_pattern_result_read = EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site);
                    if(ca_mapping == 1) ca_training_pattern_result_read &= 0x0303;
                    failed_once |= (DDRSS_ca_training_histogram_update(ca_mapping, 
					                                                         site, 
					                                                         (coarse_sweep*MAX_FINE_STEP)+fine_sweep, 
					                                                         ca_training_pattern_result_read, ca_training_expected, 
					                                                         fail_count_histogram_perbit) << (site * 16));
               } // site
               if((failed_once == ALL_CA_FAIL_MAP0) || (failed_once == ALL_CA_FAIL_MAP1)) break;
             } // loop_count
             if(!failed_once) break;
           } // fine_sweep
         if(!failed_once) break;
         } // coarse_sweep
         //********************************************************************************
         //right
         //********************************************************************************
        for(coarse_sweep = max_coarse; coarse_sweep > 0;)
		 {
            -- coarse_sweep;
            for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;)
			{
               -- fine_sweep;
               for(site = 0; site < SITE_PER_CA_CNT; site++) 
			   {
                  EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, coarse_sweep, fine_sweep);
               }
               failed_once = ALL_CA_PASS_PAT;
               for(loop_count = 0; loop_count < max_loop_count; loop_count++)
			   {
                  BIMC_CA_Training_Toggle_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface);
                  for(site = 0; site < SITE_PER_CA_CNT;  site ++)
				  {
                     ca_training_pattern_result_read = EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site);
                     if(ca_mapping == 1) ca_training_pattern_result_read &= 0x0303;
                     failed_once |= (DDRSS_ca_training_histogram_update(ca_mapping, 
					                                                          site, 
																			  (coarse_sweep*MAX_FINE_STEP)+fine_sweep, 
																			  ca_training_pattern_result_read, 
																			  ca_training_expected, 
																			  fail_count_histogram_perbit) << (site * 16));
                  } // site
                  if((failed_once == ALL_CA_FAIL_MAP0) || (failed_once == ALL_CA_FAIL_MAP1)) break;
               } // loop_count
             if(!failed_once) break;
           } // fine_sweep
           if(!failed_once) break;
         } // coarse_sweep
        } // ca_mapping
    } // pattern
	
	for(site = 0; site < SITE_PER_CA_CNT; site++)
     {            
      EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, safe_ca_cdc_wr90_delays);
     } 

    BIMC_Exit_CA_Training(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface, rank);
	EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 0);

 }//rank  
//************************************************************************************************************************************
//   Histogram extraction
//************************************************************************************************************************************	
 for(site = 0; site < SITE_PER_CA_CNT; site++)  
 {
    for(bit = 0; bit < 10; bit++)
	{
        if(bit < 8){
          DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[bit], &pass_max_edge[bit], fail_count_histogram_perbit[(site*2)][bit][0], histogram_size);
        }
        else{
          DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[bit], &pass_max_edge[bit], fail_count_histogram_perbit[(site*2)+1][bit-8][0], histogram_size);
        }
        perbit_mid[bit] = ((cdc_lut[pass_min_edge[bit]] + cdc_lut[pass_max_edge[bit]]) / 2) - (fine_lut[DESKEW_CDC_INITIAL_VALUE]);
        if ( max_pass_min_edge <  cdc_lut[pass_min_edge[bit]]) max_pass_min_edge = cdc_lut[pass_min_edge[bit]];
        if ( min_pass_max_edge >  cdc_lut[pass_max_edge[bit]]) min_pass_max_edge = cdc_lut[pass_max_edge[bit]];
        if (           max_mid <             perbit_mid[bit] )           max_mid =            perbit_mid[bit];
        if (           min_mid >             perbit_mid[bit])            min_mid =            perbit_mid[bit];
        if (     min_eye_width > (cdc_lut[pass_max_edge[bit]] - cdc_lut[pass_min_edge[bit]])) min_eye_width = (cdc_lut[pass_max_edge[bit]] - cdc_lut[pass_min_edge[bit]]);
    }
    #ifdef ENABLE_PRINTF 
    skew_range =           max_mid - min_mid;
    eye_width  = min_pass_max_edge - max_pass_min_edge;
    #endif
    DDRSS_look_up_table_full_cdc(_inst_, max_mid, clk_freq_in_khz, ca_training_full_cdc[interface][site]);
    EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, ca_training_full_cdc[interface][site]);
    programed_mid = cdc_lut[ ((ca_training_full_cdc[interface][site][delay] >>3)*MAX_FINE_STEP)+ (ca_training_full_cdc[interface][site][delay] & 0x07)];
    EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_perbit_deskew_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, 0);
    for(bit = 0; bit < 10; bit++)
	{
        perbit_delay = programed_mid - perbit_mid[bit];   
        for(deskew_delay_value = 0; deskew_delay_value < 7;  deskew_delay_value++)
		{
            if(fine_lut[deskew_delay_value + 1] > perbit_delay)
			{
                if((fine_lut[deskew_delay_value + 1] - perbit_delay) < (perbit_delay - fine_lut[deskew_delay_value]))
				{
                 deskew_delay_value++;
                }
                break;
            }
        }//deskew_delay_value loop
        #ifdef ENABLE_PRINTF 
        perbit_cdc[bit] = deskew_delay_value;// used to print logs
        #endif
       //************************************************************************************************************************************
       //  set per bit CDC 
       //************************************************************************************************************************************
        EBI1_PHY_CFG_ebi_ca_training_seq_set_cdc_perbit_deskew_delay( _inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, ca_remap_lut[site][bit], deskew_delay_value);
    }
    //************************************************************************************************************************************
    //  read back per bit CDC as return value
    //************************************************************************************************************************************
    EBI1_PHY_CFG_ebi_ca_training_seq_read_cdc_perbit_deskew_delay( _inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, ca_training_perbit[interface][site] );
 // ***********************************************************************
 //  log print out  Start
 // ***********************************************************************
 #ifdef ENABLE_PRINTF
    print_table_header();
    for(bit = 0; bit < 10; bit++)
	{

		
		snprintf(logbuf, DDR_LOG_BUF_LEN_MAX, "           CA,%15d,               ,%15d,%15d,               ,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,%15d,%15d,          ",
        interface,
        site,
        bit,
        cdc_lut[pass_min_edge[bit]],
        cdc_lut[pass_max_edge[bit]],
        perbit_mid[bit],
        programed_mid - perbit_mid[bit],
        perbit_cdc[bit]);
      for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
      {
        if(bit < 8)
        {
       
		  snprintf(logbuf+strlen(logbuf),DDR_LOG_BUF_LEN_MAX-strlen(logbuf), ",%4d", fail_count_histogram_perbit[site*2][bit][0][delay_step]);
		  
        }
        else
        {
    
		  snprintf(logbuf+strlen(logbuf),DDR_LOG_BUF_LEN_MAX-strlen(logbuf), ",%4d", fail_count_histogram_perbit[(site*2)+1][bit-8][0][delay_step]);
        }
      } // delay_step
      
    
	  ddr_printf(DDR_DETAIL,"%s", logbuf);
    } // bit
	ddr_printf(DDR_NORMAL, "         CA_SUM,%15d,               ,%15d,               ,               ,%15d,%15d,%15d,%15d,%15d,%15d\n",
      interface,
      site,
      min_mid,
      max_mid,
      programed_mid,
      min_eye_width,
      skew_range,
      eye_width);
    #endif
  } // site
  
  
  #ifdef ENABLE_PRINTF_CA_DBG
  max_mid = 0;
  min_mid = cdc_lut[FAIL_HISTOGRAM_SIZE-1];
  min_eye_width = cdc_lut[FAIL_HISTOGRAM_SIZE-1];
  max_pass_min_edge = 0;
  min_pass_max_edge = cdc_lut[FAIL_HISTOGRAM_SIZE-1];
  if(max_coarse >= MAX_COARSE_STEP) max_coarse = MAX_COARSE_STEP-1;
  histogram_size = max_coarse * MAX_FINE_STEP;
	
	clear_histogram();
	
	EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 1);
  
  if ( chip_select == SDRAM_RANK_BOTH )
  {//dual rank
     first_rank = SDRAM_RANK_CS0;
     last_rank  = SDRAM_RANK_CS1;
  }
  else 
  {//single  rank ( either SDRAM_RANK_CS0 or SDRAM_RANK_CS1
     first_rank = chip_select;
     last_rank  = chip_select;
  }
  for ( rank=first_rank ; rank <=last_rank ; rank++){ 
       EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 1);

     for(pattern = 0; pattern < CA_PATTERN_NUM;  pattern ++)
     {
       for(ca_mapping = 0; ca_mapping < 2; ca_mapping ++)
       {                
      for(site = 0; site < SITE_PER_CA_CNT; site++)
      {
        EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, safe_ca_cdc_wr90_delays);
      }        
         BIMC_Set_CA_Training_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, 
           interface,
           rank , 
           ca_mapping,
           ca_training_pattern[pattern][PRECS_RISE], 
           ca_training_pattern[pattern][PRECS_FALL], 
           ca_training_pattern[pattern][CS_RISE], 
           ca_training_pattern[pattern][CS_FALL], 
           ca_training_pattern[pattern][POSTCS_RISE], 
           ca_training_pattern[pattern][POSTCS_FALL]);
           
         cs_rise_pat = ca_training_pattern[pattern][CS_RISE];
         cs_fall_pat = ca_training_pattern[pattern][CS_FALL];
         ca_training_expected = DDRSS_get_ca_exp_pattern(_inst_ , cs_rise_pat, cs_fall_pat, ca_mapping);
         
         //********************************************************************************
         // left
         //********************************************************************************
        for(coarse_sweep = 0x0; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
         {
           for(fine_sweep = 0x0; fine_sweep < MAX_FINE_STEP; fine_sweep++)
           {
             for(site = 0; site < SITE_PER_CA_CNT; site++)
             {
               EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, coarse_sweep, fine_sweep);
             }
             failed_once = ALL_CA_PASS_PAT;
             for(loop_count = 0; loop_count < max_loop_count; loop_count++)
             {
               BIMC_CA_Training_Toggle_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface);
               for(site = 0; site < SITE_PER_CA_CNT;  site ++)
               {
                 ca_training_pattern_result_read = EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site);
                 if(ca_mapping == 1) ca_training_pattern_result_read &= 0x0303;
                 failed_once |= (DDRSS_ca_training_histogram_update(ca_mapping, site, (coarse_sweep*MAX_FINE_STEP)+fine_sweep, ca_training_pattern_result_read, ca_training_expected, fail_count_histogram_perbit) << (site * 16));
               } // loop_count
               if((failed_once == ALL_CA_FAIL_MAP0) || (failed_once == ALL_CA_FAIL_MAP1)) break;
             } // site
             
             if(!failed_once) break;
           } // fine_sweep
         if(!failed_once) break;
         } // coarse_sweep
         //********************************************************************************
         //right
         //********************************************************************************
         for(coarse_sweep = max_coarse; coarse_sweep > 0;)
         {
           -- coarse_sweep;
           for(fine_sweep = MAX_FINE_STEP; fine_sweep > 0;)
           {
             -- fine_sweep;
             for(site = 0; site < SITE_PER_CA_CNT; site++)
             {
               EBI1_PHY_CFG_ebi_ca_training_seq_set_initial_cdc_wr90_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, coarse_sweep, fine_sweep);
             }
             failed_once = ALL_CA_PASS_PAT;
             for(loop_count = 0; loop_count < max_loop_count; loop_count++)
             {
               BIMC_CA_Training_Toggle_Pattern(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface);
               for(site = 0; site < SITE_PER_CA_CNT;  site ++)
               {
                 ca_training_pattern_result_read = EBI1_PHY_CFG_ebi_read_dq_training_status_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site);
                 if(ca_mapping == 1) ca_training_pattern_result_read &= 0x0303;
                 failed_once |= (DDRSS_ca_training_histogram_update(ca_mapping, site, (coarse_sweep*MAX_FINE_STEP)+fine_sweep, ca_training_pattern_result_read, ca_training_expected, fail_count_histogram_perbit) << (site * 16));
               } // loop_count
               if((failed_once == ALL_CA_FAIL_MAP0) || (failed_once == ALL_CA_FAIL_MAP1)) break;
             } // site
             if(!failed_once) break;
           } // fine_sweep
           if(!failed_once) break;
         } // coarse_sweep
       } // ca_mapping
     } // pattern

    for(site = 0; site < SITE_PER_CA_CNT; site++)
     {            
      EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, safe_ca_cdc_wr90_delays);
     } 

    BIMC_Exit_CA_Training(_inst_ + SEQ_DDRSS_BIMC_OFFSET, interface, rank);
	EBI1_PHY_CFG_ebi_set_exit_dq_for_ca_training(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 0);
   }//rank

  
  
  for(site = 0; site < SITE_PER_CA_CNT; site++)
  {
    for(bit = 0; bit < 10; bit++)
    {
      if(bit < 8)
      {
        DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[bit], &pass_max_edge[bit], fail_count_histogram_perbit[(site*2)][bit][0], histogram_size);
      }
      else
      {
        DDRSS_histogram_min_max_scan(_inst_, &pass_min_edge[bit], &pass_max_edge[bit], fail_count_histogram_perbit[(site*2)+1][bit-8][0], histogram_size);
      }
      
      perbit_mid[bit] = ((cdc_lut[pass_min_edge[bit]] + cdc_lut[pass_max_edge[bit]]) / 2);// - (fine_lut[DESKEW_CDC_INITIAL_VALUE]);
      
      if(max_pass_min_edge < cdc_lut[pass_min_edge[bit]]) max_pass_min_edge = cdc_lut[pass_min_edge[bit]];
      if(min_pass_max_edge > cdc_lut[pass_max_edge[bit]]) min_pass_max_edge = cdc_lut[pass_max_edge[bit]];
      if(max_mid < perbit_mid[bit]) max_mid = perbit_mid[bit];
      if(min_mid > perbit_mid[bit]) min_mid = perbit_mid[bit];
      if(min_eye_width > (cdc_lut[pass_max_edge[bit]] - cdc_lut[pass_min_edge[bit]])) min_eye_width = (cdc_lut[pass_max_edge[bit]] - cdc_lut[pass_min_edge[bit]]);
    }
    
    skew_range = max_mid - min_mid;
    eye_width = min_pass_max_edge - max_pass_min_edge;
    skew_range = skew_range;
    eye_width = eye_width;
   
    EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, interface, site, ca_training_full_cdc[interface][site]);
    
  		
    print_table_header();
    
    for(bit = 0; bit < 10; bit++)
    {

       snprintf(logbuf, DDR_LOG_BUF_LEN_MAX,"         CA_DBG,%15d,               ,%15d,%15d,               ,               ,               ,               ,               ,               ,               ,%15d,%15d,%15d,               ,               ,          ",
        interface,
        site,
        bit,
        cdc_lut[pass_min_edge[bit]],
        cdc_lut[pass_max_edge[bit]],
        perbit_mid[bit]);
        //programed_mid - perbit_mid[bit][0],
        //perbit_cdc[bit][0]);
      
      for(delay_step = 0; delay_step < FAIL_HISTOGRAM_SIZE; delay_step++)
      {
        if(bit < 8)
        {
         
		   snprintf(logbuf+strlen(logbuf),DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[site*2][bit][0][delay_step]);
		  
        }
        else
        {
       
		   snprintf(logbuf+strlen(logbuf), DDR_LOG_BUF_LEN_MAX-strlen(logbuf),",%4d", fail_count_histogram_perbit[(site*2)+1][bit-8][0][delay_step]);
        }
      } // delay_step
      
   
	   ddr_printf(DDR_DETAIL,"%s", logbuf);
    } // bit
    
    ddr_printf(DDR_NORMAL, "     CA_DBG_SUM,%15d,               ,%15d,               ,               ,%15d,%15d,               ,%15d,%15d,%15d\n",
      interface,
      site,
      min_mid,
      max_mid,
      min_eye_width,
      skew_range,
      eye_width);
  } // site
  
  
  #endif
 
 
// ***********************************************************************
//  log End
// ***********************************************************************

  
  
  return(min_eye_width);

} // DDRSS_ddr_ca_training_ddrss

// ***********************************************************************
// No description is provided for this Sequence 
// uint16 training_period_100ps,  replaces fmax
// ***********************************************************************
void DDRSS_ca_training_set_trained_results(uint32 _inst_, uint16 restore_full_cdc_value[2][2][4], uint32 deskew_delay_value[2][2][2])
{
  uint32 site, channel;
  for(channel = 0; channel < CHNL_CNT; channel++)
  {
    for(site = 0; site < SITE_PER_CA_CNT; site++)
    {
      EBI1_PHY_CFG_ebi_set_ca_cdc_wr90(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, channel, site, restore_full_cdc_value[channel][site]);
      EBI1_PHY_CFG_ebi_ca_training_seq_restore_cdc_perbit_deskew_delay(_inst_ + SEQ_DDRSS_EBI1_PHY_OFFSET, channel, site, deskew_delay_value[channel][site]);
    }
	}
}







































/* =============================================================================
**  Function : DDRSS_EBI1_PHY_Set_Training_Data
** =============================================================================
*/
/**
*   @brief
*
*   @param[in]
*   @param[in]
*
*   @retval
*   @retval
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean DDRSS_EBI1_PHY_Set_Training_Data(SDRAM_INTERFACE interface, union ddr_trained_params tr_data)
{


    if(BIMC_Get_Device_Type(interface) == SDRAM_LPDDR3) // use BIMC API and get the device type here .. already BIMC has an API .
    {
#if 1
          DDRSS_ca_training_set_trained_results(0,
                                      tr_data.lpddr3_training.phy_cdc_params.ca_training_full_cdc,
                                      tr_data.lpddr3_training.phy_cdc_params.ca_training_perbit);
#endif

          DDRSS_dq_wr_training_set_trained_results(0,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_write_training,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_write_perbit_training,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_write_perbit_dm_training);


          DDRSS_dq_rd_training_set_trained_results(0,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_read_training,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_read_perbit_training,
                                                 tr_data.lpddr3_training.phy_cdc_params.dq_read_perbit_dm_training);


          DDRSS_dcc_cal_set_trained_results(0,
                            tr_data.lpddr3_training.ca_dcc_training,
                            tr_data.lpddr3_training.dq_dcc_training);


    }
    else if(BIMC_Get_Device_Type(interface) == SDRAM_PCDDR3)
    {
#if !MSM8936
          DDRSS_wrlvl_set_trained_results(0,
                                   tr_data.pcddr3_training.dq_wrlvl_retimer_training,
                                   tr_data.pcddr3_training.dq_wrlvl_delay_training);


          DDRSS_REN_RCW_Set_Trained_Results(
                                            tr_data.pcddr3_training.dq_ren_training,
                                            tr_data.pcddr3_training.bimc_rcw_params. start_dly_byte0_3,
                                            tr_data.pcddr3_training.bimc_rcw_params. start_dly_byte4_7,
											tr_data.pcddr3_training.freq,
											tr_data.pcddr3_training.nlevels);

          DDRSS_dq_wr_training_set_trained_results(0,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_write_training,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_write_perbit_training,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_write_perbit_dm_training);


          DDRSS_dq_rd_training_set_trained_results(0,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_read_training,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_read_perbit_training,
                                                 tr_data.pcddr3_training.phy_cdc_params.dq_read_perbit_dm_training);


          DDRSS_dcc_cal_set_trained_results(0,
                            tr_data.pcddr3_training.ca_dcc_training,
                            tr_data.pcddr3_training.dq_dcc_training);
#endif
    }
    return TRUE;
} /* DDRSS_EBI1_PHY_Set_Training_Data */

















