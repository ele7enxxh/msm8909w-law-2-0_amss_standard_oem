/**
 * @file bimc_mc_dpe.h
 * @brief  This is header file for bimc_mc_dpe.c implementation.
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
02/03/14   cchou      Update APIs to Enable/Disable ODT pin
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/***************************************************************************************************
  Auto-generated sequence C codes from Autoseq 2.2 12/18/2013
  User Name:cchou
  Input File:ddrss.blk
 ***************************************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2011, Qualcomm Technologies Incorporated. All rights reserved.

 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.

 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
 ***************************************************************************************************/
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#ifndef _BIMC_MC_DPE_SEQ_HEADER_
#define _BIMC_MC_DPE_SEQ_HEADER_
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "bimc_struct.h"

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/
/* =============================================================================
 *  Function : Post_Init_Setup
 ** =============================================================================
 */
/**
 *    @details
 *    DPE post initialization settings, including configurating the device, the number of rank per rank, enable power down etc.
 * 
 *    @param[in] chip_select              chip select to initialize for
 *    @param[in] device_type             device type to initialize for, can be LPDDR2, LPDDR3, PCDDR3, etc
 *    @param[in] interface_width_cs0   device configuration for rank 0 
 *    @param[in] num_banks_cs0        number of banks for rank 0
 *    @param[in] interface_width_cs1   device configuration for rank 1
 *    @param[in] num_banks_cs1        number of banks for rank 1
 **/
void ABHN_DPE_Post_Init_Setup( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 interface_width_cs0, uint32 num_banks_cs0, uint32 interface_width_cs1, uint32 num_banks_cs1 );

/* =============================================================================
 *  Function : ODT_Init
 ** =============================================================================
 */
/* 
 *    @details
 *    Enable on-die termination (ODT)   
 *    for Gandalf LPDDR3, not needed, for Gandalf PCDDR3, need to be enable for freq greater than 400MHz
 * 
 *    @param[in] chip_select              chip select to initialize for
 *    @param[in] ODT_mode     ODT_mode [5:0]=  {RANK1_ODT_ON_WR_RANK1,
 *                                                                     RANK1_ODT_ON_WR_RANK0,
 *                                                                     RANK1_ODT_ON_RD_RANK0,  
 *                                                                     RANK0_ODT_ON_WR_RANK1, 
 *                                                                     RANK0_ODT_ON_WR_RANK0, 
 *                                                                     RANK0_ODT_ON_RD_RANK1}
 **/
void ABHN_DPE_ODT_Init( uint32 _inst_, uint32 chip_select, uint32 ODT_mode );

/* =============================================================================
 *  Function : Update_Latency_LPDDR3
 ** =============================================================================
 */
/*
 *    @details
 *    subfunctions for clock switch, update the rd/wr latency in DPE and MR values for new frequency
 * 
 *    @param[in] device_type device type to set 
 *    @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 **/
void ABHN_DPE_Update_Latency_LPDDR3( uint32 _inst_, uint32 WL_value, uint32 RL_value, uint32 MR1_value, uint32 MR2_value );

/* =============================================================================
 *  Function : Update_Latency_PCDDR3
 ** =============================================================================
 */
/*
 *    @details
 *    subfunctions for clock switch, update the rd/wr latency in DPE and MR values for new frequency
 * 
 *    @param[in] device_type device type to set 
 *    @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 **/
void ABHN_DPE_Update_Latency_PCDDR3( uint32 _inst_, uint32 WL_value, uint32 RL_value, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value );

/* =============================================================================
 *  Function : Status_Check_SR 
 ** =============================================================================
 */
/**
 *    @details
 *    make sure the specific rank is not in self refresh, used in Enter_DPD
 * 
 *    @param[in] chip_select chip select to detect
 **/
void ABHN_DPE_Status_Check_SR( uint32 _inst_, uint32 chip_select );

/* =============================================================================
 *  Function : Status_Check_DPD
 ** =============================================================================
 */
 /**
 *    @details
 *    make sure the specific rank is in deep power down, used in Enter_DPD for LPDDR only
 * 
 *    @param[in] chip_select chip select to detect
 **/
void ABHN_DPE_Status_Check_DPD( uint32 _inst_, uint32 chip_select );

/* =============================================================================
 *  Function : Status_Check_DEVICE_CFG_RANK
 ** =============================================================================
 */
/**
 *    @details
 *    make sure the specific rank is in deep power down, used in Enter_DPD for LPDDR only
 * 
 *    @param[in] chip_select chip select to detect
 *    @param[out] device configuration
 **/
uint32 ABHN_DPE_Status_Check_DEVICE_CFG_RANK( uint32 _inst_, uint32 chip_select );

/* =============================================================================
 *  Function : CA_Training_Pattern_PRE
 ** =============================================================================
 */
/**
 *    @details
 *    two values, rising and falling patterns, are driven onto the CA bus before CS goes low (PRE CS)
 * 
 *    @param[in] rising pattern     enable set the rising pattern
 *    @param[in] falling pattern    enable set the falling pattern
 *    @param[in] rising_pattern    set the rising pattern
 *    @param[in] falling_pattern   set the falling pattern
 **/
void ABHN_DPE_CA_Training_Pattern_PRE( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern );

/* =============================================================================
 *  Function : CA_Training_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *    two values, rising and falling patterns, are driven onto the CA bus while CS is low (During CS)
 * 
 *    @param[in] rising pattern     enable set the rising pattern
 *    @param[in] falling pattern    enable set the falling pattern
 *    @param[in] rising_pattern    set the rising pattern
 *    @param[in] falling_pattern   set the falling pattern
 **/
void ABHN_DPE_CA_Training_Pattern( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern );

/* =============================================================================
 *  Function : CA_Training_Pattern_POST
 ** =============================================================================
 */
/**
 *    @details
 *    two values, rising and falling patterns, are driven onto the CA bus after CS went low (POST CS)
 * 
 *    @param[in] rising pattern     enable set the rising pattern
 *    @param[in] falling pattern    enable set the falling pattern
 *    @param[in] rising_pattern    set the rising pattern
 *    @param[in] falling_pattern   set the falling pattern
 **/
void ABHN_DPE_CA_Training_Pattern_POST( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern );

/* =============================================================================
 *  Function : RCW_DQcal_read_check
 ** =============================================================================
 */
/**
 *    @details
 *    this funcion is used for rank aouto dection and RCW. After SW triggers a read dq calibration command to certain rank, for upon completion of the
 *    RD_DQCAL command, SW check the status whether we can reach certain number of edge count.
 * 
 *    @param[in]   chip_select        chip select to initialize for
 *    @param[in]   which_byte        byte i for check; i = 0~7
 *    @param[in]   pattern_mode     2: check both 4 PEs/NEs and DQCAL_COMPARE_BYTEx 
 *                                              1: check 4 PEs/NEs only 
 *                                              0: check non-zero PEs/NEs (for rank auto detection)
 *    @param[out]   meet the edge count
 **/
uint32 ABHN_DPE_RCW_DQcal_read_check( uint32 _inst_, uint32 chip_select, uint32 which_byte, uint32 pattern_mode );

/* =============================================================================
 *  Function : RCW_Exp_Pattern
 ** =============================================================================
 */
/** 
 *    @details
 *    set the expected pattern for RCW and set the burst_len to 8
 **/
void ABHN_DPE_RCW_Exp_Pattern( uint32 _inst_ );

/* =============================================================================
 *  Function : Rcw_Reset
 ** =============================================================================
 */
/** 
 *    @details
 *    Reset the RCW circuit inside the PHY. 
 *    Note: The functionality of these registers have been moved to 
 *    the PHY. These DPE CSR fields are no longer valid 
 **/
void ABHN_DPE_Rcw_Reset( uint32 _inst_ );

/* =============================================================================
 *  Function : Recalc_AC_Parameters
 ** =============================================================================
 */
/**
 *    @details
 *    Trigger the hardware computation engine
 **/
void ABHN_DPE_Recalc_AC_Parameters( uint32 _inst_ );

/* =============================================================================
 *  Function : Load_All_Config
 ** =============================================================================
 */
/**
 *    @details
 *     load the timing registers to the DPE core clock domain
 **/
void ABHN_DPE_Load_All_Config( uint32 _inst_ );

/* =============================================================================
 *  Function : Get_RL_From_REG
 ** =============================================================================
 */
/**
 *    @details
 *     function of getting the read latendy for Timing 16
 *    @param[out]   read latency  
 **/
uint32 ABHN_DPE_Get_RL_From_REG( uint32 _inst_ );

/* =============================================================================
 *  Function : Program_xxx (Program all timing parameters)
 ** =============================================================================
 */
/** 
 *    @details
 *     loading the timing parameter values (from CDT in SW) to DPE Timing registers 
 **/
void ABHN_DPE_Program_tRASmin( uint32 _inst_, uint32 tRASmin );
void ABHN_DPE_Program_tWR( uint32 _inst_, uint32 tWR );
void ABHN_DPE_Program_tRCD( uint32 _inst_, uint32 tRCD );
void ABHN_DPE_Program_tWTR( uint32 _inst_, uint32 tWTR );
void ABHN_DPE_Program_tRRD( uint32 _inst_, uint32 tRRD );
void ABHN_DPE_Program_tRFCAB( uint32 _inst_, uint32 tRFCab );
void ABHN_DPE_Program_tRFCPB( uint32 _inst_, uint32 tRFCpb );
void ABHN_DPE_Program_tRFC( uint32 _inst_, uint32 tRFC );
void ABHN_DPE_Program_tRTP( uint32 _inst_, uint32 tRTP );
void ABHN_DPE_Program_tRP_AB( uint32 _inst_, uint32 tRPab );
void ABHN_DPE_Program_tRP_PB( uint32 _inst_, uint32 tRPpb );
void ABHN_DPE_Program_tRP( uint32 _inst_, uint32 tRP );
void ABHN_DPE_Program_tFAW( uint32 _inst_, uint32 tFAW );
void ABHN_DPE_Program_tCKE( uint32 _inst_, uint32 tCKE );
void ABHN_DPE_Program_tZQCS( uint32 _inst_, uint32 tZQCS );
void ABHN_DPE_Program_tMOD( uint32 _inst_, uint32 tMOD );
void ABHN_DPE_Program_tCKESR( uint32 _inst_, uint32 tCKESR );
void ABHN_DPE_Program_tXSR( uint32 _inst_, uint32 tXSR );
void ABHN_DPE_Program_tXSDLL( uint32 _inst_, uint32 tXSDLL );
void ABHN_DPE_Program_tXS( uint32 _inst_, uint32 tXS );
void ABHN_DPE_Program_tXP( uint32 _inst_, uint32 tXP );
void ABHN_DPE_Program_tXPR_PCHG( uint32 _inst_, uint32 tXP );
void ABHN_DPE_Program_tXPDLL( uint32 _inst_, uint32 tXPDLL );
void ABHN_DPE_Program_tCKSRX( uint32 _inst_, uint32 tCKSRX );
void ABHN_DPE_Program_tCKSRE( uint32 _inst_, uint32 tCKSRE );
void ABHN_DPE_Program_ODTonmax_min( uint32 _inst_, uint32 tDQSCK_min, uint32 tODTonmax, uint32 tODTonmin );
void ABHN_DPE_Program_tAONmax_min( uint32 _inst_, uint32 tAONmax, uint32 tAONmin );
void ABHN_DPE_Program_ODT_OFF( uint32 _inst_, uint32 clk_freq_in_khz, uint32 tODToffmax, uint32 tODToffmin );
void ABHN_DPE_Program_RD_Latency( uint32 _inst_, uint32 RL_value );
void ABHN_DPE_Program_WR_Latency( uint32 _inst_, uint32 WL_value );
void ABHN_DPE_Program_tZQCL( uint32 _inst_, uint32 tZQCL );
void ABHN_DPE_Program_tZQoper( uint32 _inst_, uint32 tZQoper );
void ABHN_DPE_Program_RCW_START_DLY( uint32 _inst_, uint32 which_byte, uint32 start_delay );
void ABHN_DPE_Program_RCW_START_DLY_All( uint32 _inst_, uint32 timing_18, uint32 timing_19 );
void ABHN_DPE_Program_tMRW_LP3( uint32 _inst_, uint32 tMRW );
void ABHN_DPE_Program_tMRW_LP2( uint32 _inst_, uint32 tMRW );
void ABHN_DPE_Program_tMRW_PC3( uint32 _inst_, uint32 tMRW );
void ABHN_DPE_Program_tMRR( uint32 _inst_, uint32 tMRR );
void ABHN_DPE_Program_TREFBW_XO_DIV_6_CYCLES( uint32 _inst_, uint32 tREFBW_XO_DIV_6_CYCLES );

/* =============================================================================
 *  Function : Program_MRW_BEFORE_FREQ_SWITCH
 ** =============================================================================
 */
/** 
 *    @details 
 *    The MRW will be scheduled to occur before frequency switch, if the operation is valid (indicated by 
 *    MRW_FREQ_SWITCH). 
 **/
void ABHN_DPE_Program_MRW_BEFORE_FREQ_SWITCH( uint32 _inst_, uint32 mrw_indx, uint32 w_addr, uint32 w_data );

/* =============================================================================
 *  Function : Program_MRW_AFTER_FREQ_SWITCH
 ** =============================================================================
 */
/**
 *    @details 
 *    The MRW will be scheduled to occur after frequency switch, if the operation is valid (indicated by 
 *    MRW_FREQ_SWITCH). This is typically used for changing Read/Write latencies on frequency switch.
 **/
void ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH( uint32 _inst_, uint32 mrw_indx, uint32 mrw_en, uint32 w_addr, uint32 w_data );

/* =============================================================================
 *  Function : Disable_Power_Down 
 ** =============================================================================
 */
/** 
 *    @details 
 *    when disabling powerdown /clockstop-powerdown, the controller will NOT power down the memory device when the proper conditions are met
 **/
void ABHN_DPE_Disable_Power_Down( uint32 _inst_ );

/* =============================================================================
 *  Function : Enable_Power_Down 
 ** =============================================================================
 */
/** 
 *    @details 
 *    when enabling powerdown /clockstop-powerdown, the controller will power down the memory device when the proper conditions are met
 *    NOTE: need to follow the spreadsheet for different chipset
 * 
 *    @param[in] device_type      device type to set   
 **/
void ABHN_DPE_Enable_Power_Down( uint32 _inst_, uint32 device_type );

/* =============================================================================
 *  Function : Enable_Clk_To_PHY
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
void ABHN_DPE_Enable_Clk_To_PHY( uint32 _inst_ );

/* =============================================================================
 *  Function : Enable_RCW
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
void ABHN_DPE_Enable_RCW( uint32 _inst_ );

/* =============================================================================
 *  Function : Disable_Clk_To_PHY
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
void ABHN_DPE_Disable_Clk_To_PHY( uint32 _inst_ );

/* =============================================================================
 *  Function : Enable_ODT 
 ** =============================================================================
 */
/** 
 *    @details 
 *    Enable ODT, i.e. force ODT pin high
 *
 *    @param[in] chip_select              chip select to initialize for
 **/
void ABHN_DPE_Enable_ODT( uint32 _inst_, uint32 chip_select );

/* =============================================================================
 *  Function : Disable_ODT  
 ** =============================================================================
 */
/** 
 *    @details 
 *    Disable ODT, i.e. force ODT pin low
 *
 *    @param[in] chip_select              chip select to initialize for
 **/
void ABHN_DPE_Disable_ODT( uint32 _inst_, uint32 chip_select );


#endif
