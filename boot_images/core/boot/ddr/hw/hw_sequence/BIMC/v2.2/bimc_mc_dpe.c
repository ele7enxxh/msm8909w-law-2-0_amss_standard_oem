/**
 * @file bimc_mc_dpe.c
 * @brief BIMC DPE APIs
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
10/28/14   sk         Changes for 8952 Chipset
03/12/14   nevedeth   Modified API to program ODTOFF_MIN for LP only.
02/05/14   cchou      Change the pe/ne_status check criteria for PCDDR 3 rank detection
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
#include "bimc_mc_dpe.h"
#include "ddr_target.h"

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
extern uint32 ddr_divide_func(uint32 a, uint32 b);
/* =============================================================================
 *  Function : Post_Init_Setup
 ** =============================================================================
 */
/**
 *    @details
 *    DPE post initialization settings, including configurating the device, the number of rank per rank, enable power down etc.
 **/
void ABHN_DPE_Post_Init_Setup( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 interface_width_cs0, uint32 num_banks_cs0, uint32 interface_width_cs1, uint32 num_banks_cs1 )
{
  uint32 tmp;

  ///  device width for cs0
  switch(interface_width_cs0){
    case 64 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK0, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK0_X64_FVAL);
      break;
    case 32 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK0, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK0_X32_FVAL);
      break;
    case 16 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK0, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK0_X16_X16_FVAL);
      break;
    default :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK0, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK0_X8_X8_X8_X8_FVAL);
      break;
  }
  ///  device width for cs1
  switch(interface_width_cs1){
    case 64 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK1, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK1_X64_FVAL);
      break;
    case 32 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK1, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK1_X32_FVAL);
      break;
    case 16 :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK1, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK1_X16_X16_FVAL);
      break;
    default :
      HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK1, HWIO_ABHN_DPE_CONFIG_0_DEVICE_CFG_RANK1_X8_X8_X8_X8_FVAL);
      break;
  }
  ///  bank count for both ranks
  tmp = HWIO_INX (_inst_, ABHN_DPE_CONFIG_1 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_CONFIG_1_NUM_BANKS_RANK0_BMSK)) | ((num_banks_cs0) << HWIO_ABHN_DPE_CONFIG_1_NUM_BANKS_RANK0_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_CONFIG_1_NUM_BANKS_RANK1_BMSK)) | ((num_banks_cs1) << HWIO_ABHN_DPE_CONFIG_1_NUM_BANKS_RANK1_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_CONFIG_1, tmp );
  ///  apply changed settings
  ABHN_DPE_Load_All_Config(_inst_);

  ABHN_DPE_Enable_Power_Down(_inst_,
      device_type);
}



/* =============================================================================
 *  Function : ODT_Init
 ** =============================================================================
 */
/* 
 *    @details
 *    Enable on-die termination (ODT)   
 *    for Gandalf LPDDR3, not needed, for Gandalf PCDDR3, need to be enable for freq greater than 400MHz
 **/
void ABHN_DPE_ODT_Init( uint32 _inst_, uint32 chip_select, uint32 ODT_mode )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_0, CA_BUS_OE_CTRL, HWIO_ABHN_DPE_CONFIG_0_CA_BUS_OE_CTRL_SR_ONLY_FVAL);

  if (ODT_mode & SCALE_RANK1_ODT_ON_WR_RANK1) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_WR_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK1_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_WR_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK1_DISABLED_FVAL);
  }

  if (ODT_mode & SCALE_RANK1_ODT_ON_WR_RANK0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_WR_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK0_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_WR_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_WR_RANK0_DISABLED_FVAL);
  }

  if (ODT_mode & SCALE_RANK1_ODT_ON_RD_RANK0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_RD_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_RD_RANK0_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_ODT_ON_RD_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK1_ODT_ON_RD_RANK0_DISABLED_FVAL);
  }

  if (ODT_mode & SCALE_RANK0_ODT_ON_WR_RANK1) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_WR_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK1_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_WR_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK1_DISABLED_FVAL);
  }

  if (ODT_mode & SCALE_RANK0_ODT_ON_WR_RANK0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_WR_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK0_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_WR_RANK0, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_WR_RANK0_DISABLED_FVAL);
  }

  if (ODT_mode & SCALE_RANK0_ODT_ON_RD_RANK1) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_RD_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_RD_RANK1_ENABLED_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_ODT_ON_RD_RANK1, HWIO_ABHN_DPE_ODT_CONFIG_0_RANK0_ODT_ON_RD_RANK1_DISABLED_FVAL);
  }
}

/* =============================================================================
 *  Function : Update_Latency_LPDDR3
 ** =============================================================================
 */
/*
 *    @details
 *    subfunctions for clock switch, update the rd/wr latency in DPE and MR values for new frequency
 **/
void ABHN_DPE_Update_Latency_LPDDR3( uint32 _inst_, uint32 WL_value, uint32 RL_value, uint32 MR1_value, uint32 MR2_value )
{
 if (ENABLE_LPDDR3) {
  uint32 tmp;

   ///program DPE_MRW_AFTER_FREQ_SWITCH_0 and set DPE_MRW_FREQ_SWITCH[AFTER_0] programming MR2
  ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH(_inst_,
      0,
      1,
      0x2,
      MR2_value);
  ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH(_inst_,
      1,
      1,
      0x1,
      MR1_value);
  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_16 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_16_RD_LATENCY_BMSK)) | ((RL_value) << HWIO_ABHN_DPE_DRAM_TIMING_16_RD_LATENCY_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_16_WR_LATENCY_BMSK)) | ((WL_value) << HWIO_ABHN_DPE_DRAM_TIMING_16_WR_LATENCY_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_16, tmp );
  }
}


/* =============================================================================
 *  Function : Update_Latency_PCDDR3
 ** =============================================================================
 */
/*
 *    @details
 *    subfunctions for clock switch, update the rd/wr latency in DPE and MR values for new frequency
 **/
void ABHN_DPE_Update_Latency_PCDDR3( uint32 _inst_, uint32 WL_value, uint32 RL_value, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value )
{
 if (ENABLE_PCDDR3) {
  uint32 tmp;

  /// Write data to MRW_AFTER_FREQ_SWITCH to load latency after clock switch   
  /// program DPE_MRW_AFTER_FREQ_SWITCH_0/1/2 and set DPE_MRW_FREQ_SWITCH[AFTER_0/1/2]
  ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH(_inst_,
      0,
      1,
      0x2,
      MR2_value);
  ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH(_inst_,
      1,
      1,
      0x0,
      MR0_value);
  ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH(_inst_,
      2,
      1,
      0x1,
      MR1_value);
  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_16 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_16_RD_LATENCY_BMSK)) | ((RL_value) << HWIO_ABHN_DPE_DRAM_TIMING_16_RD_LATENCY_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_16_WR_LATENCY_BMSK)) | ((WL_value) << HWIO_ABHN_DPE_DRAM_TIMING_16_WR_LATENCY_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_16, tmp );
  }
}



/* =============================================================================
 *  Function : Status_Check_SR 
 ** =============================================================================
 */
/**
 *    @details
 *    make sure the specific rank is not in self refresh, used in Enter_DPD
 **/
void ABHN_DPE_Status_Check_SR( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    while (HWIO_INXF (_inst_, ABHN_DPE_DRAM_STATUS_0, RANK_0_SR) == HWIO_ABHN_DPE_DRAM_STATUS_0_RANK_0_SR_IN_SR_FVAL );
  }
  if (chip_select & SDRAM_RANK_CS1) {
    while (HWIO_INXF (_inst_, ABHN_DPE_DRAM_STATUS_0, RANK_1_SR) == HWIO_ABHN_DPE_DRAM_STATUS_0_RANK_1_SR_IN_SR_FVAL );
  }
}

/* =============================================================================
 *  Function : Status_Check_DPD
 ** =============================================================================
 */
 /**
 *    @details
 *    make sure the specific rank is in deep power down, used in Enter_DPD for LPDDR only
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_Status_Check_DPD( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    while (HWIO_INXF (_inst_, ABHN_DPE_DRAM_STATUS_0, RANK_0_DPD) == HWIO_ABHN_DPE_DRAM_STATUS_0_RANK_0_DPD_NOT_IN_DPD_FVAL );
  }
  if (chip_select & SDRAM_RANK_CS1) {
    while (HWIO_INXF (_inst_, ABHN_DPE_DRAM_STATUS_0, RANK_1_DPD) == HWIO_ABHN_DPE_DRAM_STATUS_0_RANK_1_DPD_NOT_IN_DPD_FVAL );
  }
}

/* =============================================================================
 *  Function : Status_Check_DEVICE_CFG_RANK
 ** =============================================================================
 */
/**
 *    @details
 *    make sure the specific rank is in deep power down, used in Enter_DPD for LPDDR only
 **/
uint32 ABHN_DPE_Status_Check_DEVICE_CFG_RANK( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    return ((HWIO_INXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK0)));
  }
  else
  {
    return ((HWIO_INXF (_inst_, ABHN_DPE_CONFIG_0, DEVICE_CFG_RANK1)));
  }
}

/* =============================================================================
 *  Function : CA_Training_Pattern_PRE
 ** =============================================================================
 */
/**
 *    @details
 *    two values, rising and falling patterns, are driven onto the CA bus before CS goes low (PRE CS)
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_CA_Training_Pattern_PRE( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern )
{

  if (rising) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_PRE_CS, CA_BUS_1, rising_pattern);
  }
  if (falling) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_PRE_CS, CA_BUS_2, falling_pattern);
  }
}

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
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_CA_Training_Pattern( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern )
{

  if (rising) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_CS, CA_BUS_3, rising_pattern);
  }
  if (falling) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_CS, CA_BUS_4, falling_pattern);
  }
}

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
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_CA_Training_Pattern_POST( uint32 _inst_, uint32 rising, uint32 falling, uint32 rising_pattern, uint32 falling_pattern )
{

  if (rising) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_POST_CS, CA_BUS_5, rising_pattern);
  }
  if (falling) {
    HWIO_OUTXF (_inst_, ABHN_DPE_CA_TRAIN_POST_CS, CA_BUS_6, falling_pattern);
  }
}


/* =============================================================================
 *  Function : RCW_DQcal_read_check
 ** =============================================================================
 */
/**
 *    @details
 *    this funcion is used for rank auto dection and RCW. After SW triggers a read dq calibration command to certain rank, for upon completion of the
 *    RD_DQCAL command, SW check the status whether we can reach certain number of edge count.
 **/
__attribute__((section("ddr_boot_funcs")))
uint32 ABHN_DPE_RCW_DQcal_read_check( uint32 _inst_, uint32 chip_select, uint32 which_byte, uint32 pattern_mode )
{
  uint32 dq_byte_status0 = 0;
  uint32 pe_status = 0;
  uint32 ne_status = 0;


  if (chip_select & SDRAM_RANK_CS0) {
    switch(which_byte){
      case 0 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE0);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_PE_BYTE0);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_NE_BYTE0);
        break;
      case 1 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE1);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_PE_BYTE1);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_NE_BYTE1);
        break;
      case 2 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE2);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_PE_BYTE2);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_NE_BYTE2);
        break;
      case 3 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE3);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_PE_BYTE3);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK0, DQS_NE_BYTE3);
        break;
      case 4 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE4);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_PE_BYTE4);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_NE_BYTE4);
        break;
      case 5 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE5);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_PE_BYTE5);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_NE_BYTE5);
        break;
      case 6 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE6);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_PE_BYTE6);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_NE_BYTE6);
        break;
      case 7 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK0, DQCAL_COMPARE_BYTE7);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_PE_BYTE7);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK0, DQS_NE_BYTE7);
        break;
    }
  }
  else
  {
    switch(which_byte){
      case 0 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE0);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_PE_BYTE0);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_NE_BYTE0);
        break;
      case 1 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE1);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_PE_BYTE1);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_NE_BYTE1);
        break;
      case 2 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE2);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_PE_BYTE2);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_NE_BYTE2);
        break;
      case 3 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE3);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_PE_BYTE3);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS2_RANK1, DQS_NE_BYTE3);
        break;
      case 4 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE4);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_PE_BYTE4);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_NE_BYTE4);
        break;
      case 5 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE5);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_PE_BYTE5);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_NE_BYTE5);
        break;
      case 6 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE6);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_PE_BYTE6);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_NE_BYTE6);
        break;
      case 7 :
        dq_byte_status0 = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS0_RANK1, DQCAL_COMPARE_BYTE7);
        pe_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_PE_BYTE7);
        ne_status = HWIO_INXF (_inst_, ABHN_DPE_DQCAL_STATUS1_RANK1, DQS_NE_BYTE7);
        break;
    }
  }

  if (pattern_mode == 2) {
    if ((dq_byte_status0 == 0x1) && (pe_status == 0x4) && (ne_status == 0x4)) {
      return ((1));
    }
    else
    {
      return ((0));
    }
  }
  else
  {
    if (pattern_mode == 1) {
      if ((pe_status == 0x4) && (ne_status == 0x4)) {
        return ((1));
      }
      else
      {
        return ((0));
      }
    }
    else
    {
      if ((pe_status >= 4) && (ne_status >= 4)) {
        return ((1));
      }
      else
      {
        return ((0));
      }
    }
  }
}



/* =============================================================================
 *  Function : RCW_Exp_Pattern
 ** =============================================================================
 */
/** 
 *    @details
 *    set the expected pattern for RCW and set the burst_len to 8
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_RCW_Exp_Pattern( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_RCW_CTRL, EXP_PATTERN, 0x55);
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_2, MRR_BURST_LEN, 0x8);
  ABHN_DPE_Load_All_Config(_inst_);
}


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
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_Rcw_Reset( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_RCW_CTRL );
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE0_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE0_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE0_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE1_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE1_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE1_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE2_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE2_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE2_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE3_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE3_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE3_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE4_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE4_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE4_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE5_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE5_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE5_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE6_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE6_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE6_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE7_BMSK)) | ((HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE7_RESET_ON_FVAL) << HWIO_ABHN_DPE_RCW_CTRL_RCW_ASYNC_RESET_BYTE7_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_RCW_CTRL, tmp );
}

/* =============================================================================
 *  Function : Recalc_AC_Parameters
 ** =============================================================================
 */
/**
 *    @details
 *    Trigger the hardware computation engine
 **/
void ABHN_DPE_Recalc_AC_Parameters( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, RECALC_PS_PARAMS, HWIO_ABHN_DPE_CONFIG_4_RECALC_PS_PARAMS_RECALC_FVAL);

  while (HWIO_INXF (_inst_, ABHN_DPE_MEMC_STATUS_1, CYC_CALC_VALID) == 1 );
}



/* =============================================================================
 *  Function : Load_All_Config
 ** =============================================================================
 */
/**
 *    @details
 *     load the timing registers to the DPE core clock domain
 **/
void ABHN_DPE_Load_All_Config( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
}


/* =============================================================================
 *  Function : Get_RL_From_REG
 ** =============================================================================
 */
/**
 *    @details
 *     function of getting the read latendy for Timing 16
 *    @param[out]   read latency  
 **/
uint32 ABHN_DPE_Get_RL_From_REG( uint32 _inst_ )
{

  return ((HWIO_INXF (_inst_, ABHN_DPE_DRAM_TIMING_16, RD_LATENCY)));
}



/* =============================================================================
 *  Function : Program_xxx (Program all timing parameters)
 ** =============================================================================
 */
/** 
 *    @details
 *     loading the timing parameter values (from CDT in SW) to DPE Timing registers 
 **/
void ABHN_DPE_Program_tRASmin( uint32 _inst_, uint32 tRASmin )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_0, TRASMIN, tRASmin);
}


void ABHN_DPE_Program_tWR( uint32 _inst_, uint32 tWR )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_1, TWR, tWR);
}


void ABHN_DPE_Program_tRCD( uint32 _inst_, uint32 tRCD )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_1, TRCD, tRCD);
}


void ABHN_DPE_Program_tWTR( uint32 _inst_, uint32 tWTR )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_2, TWTR, tWTR);
}


void ABHN_DPE_Program_tRRD( uint32 _inst_, uint32 tRRD )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_2, TRRD, tRRD);
}

/* for LP2/3 Timing_3 */
void ABHN_DPE_Program_tRFCAB( uint32 _inst_, uint32 tRFCab )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_3, TRFCAB, tRFCab);
}

void ABHN_DPE_Program_tRFCPB( uint32 _inst_, uint32 tRFCpb )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_3, TRFCPB, tRFCpb);
}

/* for PC3 Timing_3 */
void ABHN_DPE_Program_tRFC( uint32 _inst_, uint32 tRFC )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_3 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_3_TRFCAB_BMSK)) | ((tRFC) << HWIO_ABHN_DPE_DRAM_TIMING_3_TRFCAB_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_3_TRFCPB_BMSK)) | ((tRFC) << HWIO_ABHN_DPE_DRAM_TIMING_3_TRFCPB_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_3, tmp );
}

void ABHN_DPE_Program_tRTP( uint32 _inst_, uint32 tRTP )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_4, TRTP, tRTP);
}

/* for LP2/3 Timing_5 */
void ABHN_DPE_Program_tRP_AB( uint32 _inst_, uint32 tRPab )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_5, TRPAB, tRPab);
}

/* for LP2/3 Timing_5 */
void ABHN_DPE_Program_tRP_PB( uint32 _inst_, uint32 tRPpb )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_5, TRPPB, tRPpb);
}

/*  for PC3 Timing_5 */
void ABHN_DPE_Program_tRP( uint32 _inst_, uint32 tRP )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_5 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_5_TRPAB_BMSK)) | ((tRP) << HWIO_ABHN_DPE_DRAM_TIMING_5_TRPAB_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_5_TRPPB_BMSK)) | ((tRP) << HWIO_ABHN_DPE_DRAM_TIMING_5_TRPPB_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_5, tmp );
}

void ABHN_DPE_Program_tFAW( uint32 _inst_, uint32 tFAW )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_6, TFAW, tFAW);
}

void ABHN_DPE_Program_tCKE( uint32 _inst_, uint32 tCKE )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_6, TCKE, tCKE);
}

void ABHN_DPE_Program_tZQCS( uint32 _inst_, uint32 tZQCS )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_7, TZQCS, tZQCS);
}

void ABHN_DPE_Program_tMOD( uint32 _inst_, uint32 tMOD )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_8, TMOD, tMOD);
}

void ABHN_DPE_Program_tCKESR( uint32 _inst_, uint32 tCKESR )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_9, MIN_SR_DURATION, tCKESR);
}

/*  for LP2/3 Timing_10 */
void ABHN_DPE_Program_tXSR( uint32 _inst_, uint32 tXSR )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_10 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_10_TXSRD_BMSK)) | ((tXSR) << HWIO_ABHN_DPE_DRAM_TIMING_10_TXSRD_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_10_TXSNR_BMSK)) | ((tXSR) << HWIO_ABHN_DPE_DRAM_TIMING_10_TXSNR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_10, tmp );
}

/*  for PC3 Timing_10 */
void ABHN_DPE_Program_tXSDLL( uint32 _inst_, uint32 tXSDLL )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_10, TXSRD, tXSDLL);
}

/*  for PC3 Timing_10 */
void ABHN_DPE_Program_tXS( uint32 _inst_, uint32 tXS )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_10, TXSNR, tXS);
}

void ABHN_DPE_Program_tXP( uint32 _inst_, uint32 tXP )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_11 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_BMSK)) | ((tXP) << HWIO_ABHN_DPE_DRAM_TIMING_11_TXPNR_ACT_PWR_DN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_BMSK)) | ((tXP) << HWIO_ABHN_DPE_DRAM_TIMING_11_TXPR_ACT_PWR_DN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_11, tmp );
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_12, TXPNR_PCHG_PWR_DN, tXP);
}

/*  for LP2/3 Timing_12 */
void ABHN_DPE_Program_tXPR_PCHG( uint32 _inst_, uint32 tXP )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_12, TXPR_PCHG_PWR_DN, tXP);
}

/*  for PC3 Timing_12 */
void ABHN_DPE_Program_tXPDLL( uint32 _inst_, uint32 tXPDLL )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_12, TXPR_PCHG_PWR_DN, tXPDLL);
}

/*  for PC3 Timing_13 */
void ABHN_DPE_Program_tCKSRX( uint32 _inst_, uint32 tCKSRX )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_13, CLK_BEFORE_SR_EXIT, tCKSRX + 40);
}

void ABHN_DPE_Program_tCKSRE( uint32 _inst_, uint32 tCKSRE )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_13, CLK_AFTER_SR_ENTRY, tCKSRE);
}

/* for LP2/3 Timing_13
 *  TIMING_16 and Timing_23 for ODT_ON and ODT_OFF
 **/
void ABHN_DPE_Program_ODTonmax_min( uint32 _inst_, uint32 tDQSCK_min, uint32 tODTonmax, uint32 tODTonmin )
{
  if ((tODTonmax - tDQSCK_min) > 0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_23, ODTON_MAX, tODTonmax - tDQSCK_min);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_23, ODTON_MAX, 0);
  }
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_23, ODTON_MIN, tODTonmin);
}

/* PC3: timing_23 */
void ABHN_DPE_Program_tAONmax_min( uint32 _inst_, uint32 tAONmax, uint32 tAONmin )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_DRAM_TIMING_23 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_23_ODTON_MAX_BMSK)) | ((tAONmax) << HWIO_ABHN_DPE_DRAM_TIMING_23_ODTON_MAX_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_DRAM_TIMING_23_ODTON_MIN_BMSK)) | ((tAONmin) << HWIO_ABHN_DPE_DRAM_TIMING_23_ODTON_MIN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_23, tmp );
}

/* combining timing16 and timing23 for ODT_OFF for LP only*/
void ABHN_DPE_Program_ODT_OFF( uint32 _inst_, uint32 clk_freq_in_khz, uint32 tODToffmax, uint32 tODToffmin )
{
  //uint32 clk_period_in_ps = 0;
  uint32 ODToff_min;
  
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_23, ODTOFF_MAX, tODToffmax);
  ODToff_min = ddr_divide_func(((tODToffmin * 100) - ( clk_freq_in_khz/ 2)) , (clk_freq_in_khz));
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_23, ODTOFF_MIN, ODToff_min);
}

/*  timing_16: RL */
void ABHN_DPE_Program_RD_Latency( uint32 _inst_, uint32 RL_value )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_16, RD_LATENCY, RL_value);
}

/* timing_16: WL*/
void ABHN_DPE_Program_WR_Latency( uint32 _inst_, uint32 WL_value )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_16, WR_LATENCY, WL_value);
}

/* LP2/3 timing_17 */
void ABHN_DPE_Program_tZQCL( uint32 _inst_, uint32 tZQCL )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_17, TZQCL, tZQCL);
}

void ABHN_DPE_Program_tZQoper( uint32 _inst_, uint32 tZQoper )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_17, TZQCL, tZQoper);
}

__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_Program_RCW_START_DLY( uint32 _inst_, uint32 which_byte, uint32 start_delay )
{
  switch(which_byte){
    case 0 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_19, RCW_START_DLY_BYTE0, start_delay);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_19, RCW_START_DLY_BYTE1, start_delay);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_19, RCW_START_DLY_BYTE2, start_delay);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_19, RCW_START_DLY_BYTE3, start_delay);
      break;
    case 4 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_18, RCW_START_DLY_BYTE4, start_delay);
      break;
    case 5 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_18, RCW_START_DLY_BYTE5, start_delay);
      break;
    case 6 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_18, RCW_START_DLY_BYTE6, start_delay);
      break;
    case 7 :
      HWIO_OUTXF (_inst_, ABHN_DPE_DRAM_TIMING_18, RCW_START_DLY_BYTE7, start_delay);
      break;
  }
}

__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_Program_RCW_START_DLY_All( uint32 _inst_, uint32 timing_18, uint32 timing_19 )
{
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_18, timing_18);
  HWIO_OUTX (_inst_, ABHN_DPE_DRAM_TIMING_19, timing_19);
}

void ABHN_DPE_Program_tMRW_LP3( uint32 _inst_, uint32 tMRW )
{
  if (15 < tMRW) {
    HWIO_OUTXF (_inst_, ABHN_DPE_TIMER_2, TMRW, tMRW);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_DPE_TIMER_2, TMRW, 15);
  }
}

void ABHN_DPE_Program_tMRW_LP2( uint32 _inst_, uint32 tMRW )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_TIMER_2, TMRW, tMRW);
}

void ABHN_DPE_Program_tMRW_PC3( uint32 _inst_, uint32 tMRW )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_TIMER_2, TMRW, tMRW);
}

void ABHN_DPE_Program_tMRR( uint32 _inst_, uint32 tMRR )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_TIMER_2, TMRR, tMRR);
}

void ABHN_DPE_Program_TREFBW_XO_DIV_6_CYCLES( uint32 _inst_, uint32 tREFBW_XO_DIV_6_CYCLES )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_3, TREFBW_XO_DIV_6_CYCLES, tREFBW_XO_DIV_6_CYCLES);
}



/* =============================================================================
 *  Function : Program_MRW_BEFORE_FREQ_SWITCH
 ** =============================================================================
 */
/** 
 *    @details 
 *    The MRW will be scheduled to occur before frequency switch, if the operation is valid (indicated by 
 *    MRW_FREQ_SWITCH). 
 **/
void ABHN_DPE_Program_MRW_BEFORE_FREQ_SWITCH( uint32 _inst_, uint32 mrw_indx, uint32 w_addr, uint32 w_data )
{
  uint32 val = (w_addr << HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_0_W_ADDR_SHFT) | (w_data);

  if (mrw_indx == 0) {    
	HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_0_OUT(_inst_,val);
    HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, BEFORE_0, 0);
  }
  else
  {
    if (mrw_indx == 1) {
	  HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_1_OUT(_inst_,val);
      HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, BEFORE_1, 0);
    }
    else
    {
      if (mrw_indx == 2) {
	    HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_2_OUT(_inst_,val);
        HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, BEFORE_2, 0);
      }
      else
      {
		HWIO_ABHN_DPE_MRW_BEFORE_FREQ_SWITCH_3_OUT(_inst_,val);
        HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, BEFORE_3, 0);
      }
    }
  }
}



/* =============================================================================
 *  Function : Program_MRW_AFTER_FREQ_SWITCH
 ** =============================================================================
 */
/**
 *    @details 
 *    The MRW will be scheduled to occur after frequency switch, if the operation is valid (indicated by 
 *    MRW_FREQ_SWITCH). This is typically used for changing Read/Write latencies on frequency switch.
 **/
void ABHN_DPE_Program_MRW_AFTER_FREQ_SWITCH( uint32 _inst_, uint32 mrw_indx, uint32 mrw_en, uint32 w_addr, uint32 w_data )
{
  uint32 val = (w_addr << HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_0_W_ADDR_SHFT) | (w_data);

  if (mrw_indx == 0) {
	HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_0_OUT(_inst_,val);
    HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, AFTER_0, mrw_en);
  }
  else
  {
    if (mrw_indx == 1) {
      HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_1_OUT(_inst_,val);
      HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, AFTER_1, mrw_en);
    }
    else
    {
      if (mrw_indx == 2) {
		HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_2_OUT(_inst_,val);
        HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, AFTER_2, mrw_en);
      }
      else
      {
		HWIO_ABHN_DPE_MRW_AFTER_FREQ_SWITCH_3_OUT(_inst_,val);
        HWIO_OUTXF (_inst_, ABHN_DPE_MRW_FREQ_SWITCH, AFTER_3, mrw_en);
      }
    }
  }
}

/* =============================================================================
 *  Function : Disable_Power_Down 
 ** =============================================================================
 */
/** 
 *    @details 
 *    when disabling powerdown /clockstop-powerdown, the controller will NOT power down the memory device when the proper conditions are met
 **/
void ABHN_DPE_Disable_Power_Down( uint32 _inst_ )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_DPE_PWR_CTRL_0 );
  tmp = (tmp & ~(HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_BMSK)) | ((HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_DISABLED_FVAL) << HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_BMSK)) | ((HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_DISABLED_FVAL) << HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_DPE_PWR_CTRL_0, tmp );
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, HWIO_ABHN_DPE_CONFIG_4_LOAD_ALL_CONFIG_LOAD_FVAL);
}



/* =============================================================================
 *  Function : Enable_Power_Down 
 ** =============================================================================
 */
/** 
 *    @details 
 *    when enabling powerdown /clockstop-powerdown, the controller will power down the memory device when the proper conditions are met
 *    NOTE: need to follow the spreadsheet for different chipset
 * 
 **/
void ABHN_DPE_Enable_Power_Down( uint32 _inst_, uint32 device_type )
{

  if (device_type == SDRAM_LPDDR2) {
    HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_ENABLED_FVAL);
    HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, CLK_STOP_PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_ENABLED_FVAL);
  }
  else
  {
    if (device_type == SDRAM_LPDDR3) {
      HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_ENABLED_FVAL);
      HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, CLK_STOP_PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_ENABLED_FVAL);
    }
    else
    {
      HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_PWR_DN_EN_ENABLED_FVAL);
      HWIO_OUTXF (_inst_, ABHN_DPE_PWR_CTRL_0, CLK_STOP_PWR_DN_EN, HWIO_ABHN_DPE_PWR_CTRL_0_CLK_STOP_PWR_DN_EN_DISABLED_FVAL);
    }
  }
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, HWIO_ABHN_DPE_CONFIG_4_LOAD_ALL_CONFIG_LOAD_FVAL);
}



/* =============================================================================
 *  Function : Enable_Clk_To_PHY
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
void ABHN_DPE_Enable_Clk_To_PHY( uint32 _inst_ )
{
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_6, IOSTAGE_WR_DEBUG_MODE, 1);
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
}


/* =============================================================================
 *  Function : Enable_RCW
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_DPE_Enable_RCW( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_RCW_CTRL, RCW_ENABLE, 1);
  ABHN_DPE_Load_All_Config(_inst_);
}


/* =============================================================================
 *  Function : Disable_Clk_To_PHY
 ** =============================================================================
 */
/** 
 *    @details 
 *    This should only be required during legacy mode of operation 
 **/
void ABHN_DPE_Disable_Clk_To_PHY( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_6, IOSTAGE_WR_DEBUG_MODE, 0);
  HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
}

/* =============================================================================
 *  Function : Enable_ODT 
 ** =============================================================================
 */
/** 
 *    @details 
 *    Enable ODT, i.e. force ODT pin high
 **/
void ABHN_DPE_Enable_ODT( uint32 _inst_, uint32 chip_select )
{
  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_FORCE_ODT, 1);
    HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_FORCE_ODT, 1);
    HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
  }
}

/* =============================================================================
 *  Function : Disable_ODT  
 ** =============================================================================
 */
/** 
 *    @details 
 *    Disable ODT, i.e. force ODT pin low
 **/
void ABHN_DPE_Disable_ODT( uint32 _inst_, uint32 chip_select )
{
  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK0_FORCE_ODT, 0);
    HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_DPE_ODT_CONFIG_0, RANK1_FORCE_ODT, 0);
    HWIO_OUTXF (_inst_, ABHN_DPE_CONFIG_4, LOAD_ALL_CONFIG, 1);
  }
}

