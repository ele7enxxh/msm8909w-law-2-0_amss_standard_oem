/**
 * @file ddrss_rcw.c
 * @brief rcw algorithm APIs
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     ------------------------------------------------------
10/28/14   sk         Changes for 8952 Chipset
02/06/14   nevedeth   Adding APIs to get/set Auto-Refresh status
01/21/14   cchou      Adding API for restore/store RCW results from/to SW
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

#include "ddrss_rcw.h"
//#include "HAL_DDR.h"
#include "ddr_params.h"
#include "bimc_config.h"

#define bimc_base SEQ_DDRSS_BIMC_OFFSET  
#define phy_base  SEQ_DDRSS_EBI1_PHY_OFFSET

uint32 RCW_Start_Delay_Result[8]; //[byte]
uint32 RCW_Hcycle_Delay_Result[7][2][8]; //[which_ren_cfg][channel_select][byte]
uint32 RCW_Coarse_Delay_Result[7][2][8]; //[which_ren_cfg][channel_select][byte]
uint32 RCW_Fine_Delay_Result[7][2][8]; //[which_ren_cfg][channel_select][byte]
rcw_training start_dly_result[7][2];
uint32 training_clk_freq_in_khz[DDR_FREQ_MAX_NUM_LEVELS];   
uint16 local_dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8];
/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
/* =============================================================================
 **  Function : DDRSS_RCW_training_Per_Byte
 ** =============================================================================
 */
/**
 *   @brief
 *      training rcw result for each byte
 **/
uint32 DDRSS_RCW_training_Per_Byte( SDRAM_INTERFACE channel_select, uint32 chip_select, uint32 which_ren_cfg, uint32 which_byte, uint32 pattern_mode, uint32 max_loop_cnt)
{

  ///  ----------------------------------------------------------------------------------   
  ///  ------- phase 0: reset start_delay, half cycle delay, coarse and fine delay  -----
  ///  ----------------------------------------------------------------------------------        
  boolean result_pass = 0;
  uint32 start_delay, hcycle_delay=0, coarse_delay=0, fine_delay=0;
  uint32 start_delay_ini = 0;
  uint32 start_delay_max = 31; // 5 bits for start delay
  uint32 hcycle_delay_max = 1;
  uint32 coarse_delay_max = 15;
  uint32 fine_delay_max = 7;
  uint32 RL_value = 0;
  uint32 do_fine_delay = 0;

  //Set RCW start delay, hcycle, coarse grain/fine delay to zero
  BIMC_Program_RCW_START_DLY (bimc_base, channel_select, which_byte, 0);
  BIMC_DPE_Load_All_Config(bimc_base, channel_select);
  EBI1_PHY_CFG_ddr_phy_ren_delay_update(phy_base, BIMC_chip_id, channel_select, which_ren_cfg, which_byte, 0);

  ///  ------------------------------------------------------------------   
  ///  ------- phase 1: train the start_delay and half cycle delay ------
  ///  ------------------------------------------------------------------

  RL_value  = BIMC_Get_RL_From_REG(bimc_base, channel_select);
  start_delay_ini = RL_value - 1;

  for (start_delay = start_delay_ini; start_delay <= start_delay_max;  start_delay ++){
    BIMC_Program_RCW_START_DLY(bimc_base, channel_select, which_byte,start_delay);
    BIMC_DPE_Load_All_Config(bimc_base, channel_select);

    for (hcycle_delay = 0; hcycle_delay <= hcycle_delay_max;  hcycle_delay ++){

      EBI1_PHY_CFG_ddr_phy_ren_delay_update(phy_base, BIMC_chip_id, channel_select, which_ren_cfg, which_byte, (hcycle_delay<<9));
      EBI1_PHY_CFG_ddr_phy_ren_reset(phy_base, channel_select, 1, 1);

      result_pass = BIMC_RCW_Training_Read_Check(bimc_base , channel_select, chip_select, which_byte, pattern_mode, max_loop_cnt);

      if(result_pass) break;

    }  // end of hcycle_delay loop  

    if(result_pass) break;
  } // end of start_delay loop


  ///  ----------------------------------------------------------------------------------------------
  ///  ------- phase 2: train the coarse_delay based on trained_start_delay and trained_hcycle ------
  ///  ----------------------------------------------------------------------------------------------
  if (!result_pass) {
    return(result_pass);           
    //break;
  }
  else { //keep training the coarse_delay
    result_pass = 0; 

    //subtract half cycle
    if(hcycle_delay == 1) hcycle_delay = 0;
    else {
      hcycle_delay = 1;
      start_delay --;
    }
    BIMC_Program_RCW_START_DLY(bimc_base, channel_select, which_byte,start_delay);
    BIMC_DPE_Load_All_Config(bimc_base, channel_select);
  }  

  for (coarse_delay = 0; coarse_delay <= coarse_delay_max;  coarse_delay ++){
    EBI1_PHY_CFG_ddr_phy_ren_delay_update(phy_base, BIMC_chip_id, channel_select, 
        which_ren_cfg, which_byte, (hcycle_delay<<9)|(coarse_delay<<3));

    EBI1_PHY_CFG_ddr_phy_ren_reset(phy_base, channel_select, 1, 1);

    result_pass = BIMC_RCW_Training_Read_Check(bimc_base , channel_select, chip_select, which_byte, pattern_mode, max_loop_cnt );
    if(result_pass) break;
  }		                                         

  ///  ---------------------------------------------------------------------------------------------------------------------
  ///  ------- phase 3: train the fine_delay based on trained_start_delay and trained_hcycle and trained_coarse_delay ------
  ///  ---------------------------------------------------------------------------------------------------------------------
  // take care of boundary case
  // (1) case 1:   if the coarse_delay ==0 , go ahead to phase 4
  // (2) case 2:   if the coarse_delay >0, subtract 1 and run fine delay loop
  // (3) case 3:   if cannot find any pass pattern in coarse_delay, run fine delay loop    
  if (result_pass){
    if(coarse_delay == 0) {
      do_fine_delay = 0;
      fine_delay = 0;
    }
    else {
      do_fine_delay = 1;
    }
  }
  else{
    do_fine_delay = 1;
  }  

  if(do_fine_delay){   // if the coarse_delay ==0 , go ahead to phase 4
    result_pass = 0;

    coarse_delay --;
    for (fine_delay = 0; fine_delay <= fine_delay_max;  fine_delay ++){
      EBI1_PHY_CFG_ddr_phy_ren_delay_update(phy_base, BIMC_chip_id, channel_select, which_ren_cfg, which_byte, (hcycle_delay<<9)|(coarse_delay<<3)|fine_delay);
      EBI1_PHY_CFG_ddr_phy_ren_reset(phy_base, channel_select, 1, 1);
      //BIMC_rcw_reset(bimc_base, channel_select);
      result_pass = BIMC_RCW_Training_Read_Check(bimc_base , channel_select, chip_select, which_byte, pattern_mode, max_loop_cnt);
      if(result_pass) break;
    }//end for fine delay	          
  } // end of if

  ///  --------------------------------------
  ///  ------- phase 4: add 1/2 cycle  ------
  ///  --------------------------------------
  if (!result_pass) {
    if(coarse_delay == coarse_delay_max)
    {
      /// reverse to phase 1 pass
      if(hcycle_delay == 0) {
        hcycle_delay = 1;
      }
      else{
        hcycle_delay = 0;
        start_delay ++;
      }   
      coarse_delay = 0;
      fine_delay = 0;  
    }
  }
  if(fine_delay > 0) {
    fine_delay --;      
  }   
  result_pass = 0;
  /// add 1/2 cycle for final check
  if(hcycle_delay == 0)
    hcycle_delay = 1;
  else{
    hcycle_delay = 0;
    start_delay ++;
  }   
  BIMC_Program_RCW_START_DLY(bimc_base, channel_select, which_byte,start_delay);        
  BIMC_DPE_Load_All_Config(bimc_base, channel_select);

  EBI1_PHY_CFG_ddr_phy_ren_delay_update(phy_base, BIMC_chip_id, channel_select, 
      which_ren_cfg, which_byte,
      ((hcycle_delay<<9)|(coarse_delay<<3)|fine_delay));

  EBI1_PHY_CFG_ddr_phy_ren_reset(phy_base, channel_select, 1, 1);

  result_pass = BIMC_RCW_Training_Read_Check(bimc_base , channel_select, chip_select, which_byte, pattern_mode, max_loop_cnt);

  if (!result_pass) {
    return(result_pass);  
  }
  else{
    //store the result back based on different ren_cfg_num

    RCW_Start_Delay_Result[which_byte] = start_delay; //[which_ren_cfg][channel_select][byte]
    RCW_Hcycle_Delay_Result[which_ren_cfg][channel_select][which_byte] = hcycle_delay; //[which_ren_cfg][channel_select][byte]
    RCW_Coarse_Delay_Result[which_ren_cfg][channel_select][which_byte] = coarse_delay; //[which_ren_cfg][channel_select][byte]
    RCW_Fine_Delay_Result[which_ren_cfg][channel_select][which_byte] = fine_delay; //[which_ren_cfg][channel_select][byte]	
  }   

  return(result_pass);
}

/* =============================================================================
 **  Function : DDRSS_RCW_training
 ** =============================================================================
 */
/**
 *   @brief
 *      the top level function for rcw training
 **/
uint32 DDRSS_RCW_training( SDRAM_INTERFACE channel_select, uint32 chip_select, uint32 pattern_mode, uint32 clk_freq_in_khz, uint32 max_loop_cnt)
{

  uint32 auto_refresh_status = 0;
  uint32 which_ren_cfg;
  uint32 dq;
  uint32 pass_per_byte = 1;
  uint32 do_phy_rcw_training;
  uint32 timing_18_value, timing_19_value;

  if(BIMC_Get_Device_Type(channel_select) ==  SDRAM_PCDDR3) 
    do_phy_rcw_training = 1;
  else do_phy_rcw_training = 0;  

  if(do_phy_rcw_training) {
  
    //Get Auto_Refresh Enable Status
    auto_refresh_status = BIMC_Get_Auto_Refresh_Status(bimc_base, channel_select);

    //Disable auto-refresh if its already set
    if (auto_refresh_status & chip_select) {
      BIMC_Set_Auto_Refresh_Status( bimc_base, channel_select, auto_refresh_status & ~chip_select);
    }

    which_ren_cfg = EBI1_PHY_CFG_get_ren_cfg(phy_base, channel_select);

    if(which_ren_cfg >= DDR_FREQ_MAX_NUM_LEVELS) {
      pass_per_byte = 0;      //return false if we cannot get right ren_cfg_num corresponding to the freq plan
    } 

    if(pass_per_byte !=0) {
      //Enable RCW in BIMC
      BIMC_Enable_RCW(bimc_base, channel_select);

      /// Enable MPR mode                                      
      BIMC_Write_MR(bimc_base, channel_select, chip_select, 0x3, 0x4);     

      // Set expected value
      BIMC_RCW_Exp_Pattern(bimc_base, channel_select);

      //main algorithm        
      for(dq=0;dq< BIMC_NUM_BYTES;dq++)
      {
        pass_per_byte = DDRSS_RCW_training_Per_Byte(channel_select, chip_select, which_ren_cfg, dq, pattern_mode, max_loop_cnt);
        if(pass_per_byte == 0){               
          break;
        }            

      }        
      //end of main algorithm

      // store the RCW start_delay and period info into the rcw structure
      timing_18_value = (RCW_Start_Delay_Result[7] << 24) | (RCW_Start_Delay_Result[6] << 16) | (RCW_Start_Delay_Result[5] << 8) | (RCW_Start_Delay_Result[4]);
      timing_19_value = (RCW_Start_Delay_Result[3] << 24) | (RCW_Start_Delay_Result[2] << 16) | (RCW_Start_Delay_Result[1] << 8) | (RCW_Start_Delay_Result[0]);
      BIMC_Save_RCW (which_ren_cfg, channel_select, timing_18_value, timing_19_value, clk_freq_in_khz);
      
      // store the REN  RCW_Hcycle_Delay_Result RCW_Coarse_Delay_Result RCW_Fine_Delay_Result into local_dqlocal_dq_ren_training _ren_training strcture
      EBI1_PHY_REN_Get_Trained_Results  (which_ren_cfg, channel_select, RCW_Hcycle_Delay_Result, RCW_Coarse_Delay_Result, RCW_Fine_Delay_Result, local_dq_ren_training);
  
      EBI1_PHY_CFG_ddr_phy_ren_hw_mode(phy_base, channel_select); 

      /// Disable MPR mode
      BIMC_Write_MR(bimc_base, channel_select, chip_select, 0x3, 0x0);
    } //end of (which_ren_cfg >= DDR_FREQ_MAX_NUM_LEVELS) check
     
    //Sets the original auto_refresh status
    if (auto_refresh_status & chip_select) {
      BIMC_Set_Auto_Refresh_Status( bimc_base, channel_select, auto_refresh_status ); 
    }
    	
    return(pass_per_byte);
  }
  else
    return (0);

}

/* =============================================================================
**  Function : DDRSS_REN_RCW_Set_Trained_Results 
** =============================================================================
*/
/**
*    @details
*     restore REN training results from SW to BIMC/PHY 
**/
void DDRSS_REN_RCW_Set_Trained_Results (uint16
dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8], uint32
start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size)
{
  uint32 freq_idx, freq_max_in_khz;
  BIMC_Enable_RCW(bimc_base, (SDRAM_INTERFACE)0);  
  BIMC_Enable_RCW(bimc_base, (SDRAM_INTERFACE)1); 
  BIMC_RCW_Set_Trained_Results( start_dly_byte0_3, start_dly_byte4_7, freq, size); 
  
  // populate the start_delay result of the top freq to DPE registers
  freq_max_in_khz = freq[0] ;
  for (freq_idx = 1; freq_idx < size; freq_idx++) {
    if (freq[freq_idx] > freq_max_in_khz){
      freq_max_in_khz = freq[freq_idx];
    } 
  }
  BIMC_Restore_RCW( bimc_base ,  (SDRAM_INTERFACE)0, freq_max_in_khz);
  BIMC_Restore_RCW( bimc_base ,  (SDRAM_INTERFACE)1, freq_max_in_khz);
    
  EBI1_PHY_REN_Set_Trained_results( phy_base, dq_ren_training);
}

/* =============================================================================
**  Function : DDRSS_REN_RCW_Get_Trained_Results
** =============================================================================
*/
/**
*    @details
*     SW gets RCW/REN training results from BIMC/PHY 
**/
void DDRSS_REN_RCW_Get_Trained_Results (uint16 dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8], uint32
start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size)
{
  int which_byte =0;
  int which_ren_cfg = 0; 
  int which_channel = 0; 
  
  /* Protect from too large “size” */
  size = (size <= DDR_FREQ_MAX_NUM_LEVELS) ? size : DDR_FREQ_MAX_NUM_LEVELS;
  
  BIMC_RCW_Get_Trained_Results( start_dly_byte0_3 ,start_dly_byte4_7, freq, size ); 
  
  for (which_channel = 0;which_channel < 2; which_channel ++) {
    for (which_ren_cfg =0;which_ren_cfg <size ; which_ren_cfg ++) {
      for (which_byte =0;which_byte < BIMC_NUM_BYTES ;which_byte++) {
        dq_ren_training[which_channel][which_ren_cfg][which_byte] = local_dq_ren_training[which_channel][which_ren_cfg][which_byte];
      }
    }  
  }     
}



