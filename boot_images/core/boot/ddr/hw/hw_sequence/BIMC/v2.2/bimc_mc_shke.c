/**
 * @file bimc_mc_shke.c
 * @brief BIMC SHKE APIs
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
05/09/14   nevedeth   Included API to get/set HW_Self_Refresh register value
02/06/13   nevedeth   Added two new API for get/set auto refresh status
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/***************************************************************************************************
  Auto-generated sequence C codes from Autoseq 2.2 12/3/2013
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
#include "bimc_mc_shke.h"
#include "ddr_target.h"

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/   
/** =============================================================================
 *  Function : Program_tREFI
 ** =============================================================================
 */
/**
 *    @details
 *    Program two registers for tREFI 
 **/
void ABHN_SHKE_Program_tREFI( uint32 _inst_, uint32 tREFI )
{
  uint32 tREFI_in_XO = 0;


  ///  tREFI is expressed in 100ps resolution.
  ///  tREFI/1000 makes it 39, instead of 3.9us
  ///  final /10000 adjusts for the factor of 10
  tREFI_in_XO = (tREFI / 1000) * XO_SPEED_IN_KHZ / 10000 - 1;
  HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, TREFI, tREFI_in_XO);
  HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL_1, TREFI, tREFI_in_XO);
}



/** =============================================================================
 *  Function : Post_Init_Setup
 ** =============================================================================
 */
/**
 *    @details
 *    program number of rank and enable auto-refresh, ZQCAL, and SRR
 **/
void ABHN_SHKE_Post_Init_Setup( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 num_banks_cs0, uint32 concurrent_sr )
{

  if (num_banks_cs0 == 8) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, NUM_BANKS, HWIO_ABHN_SHKE_CONFIG_NUM_BANKS_ENUM_8_BANKS_FVAL);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, NUM_BANKS, HWIO_ABHN_SHKE_CONFIG_NUM_BANKS_ENUM_4_BANKS_FVAL);
  }

  ABHN_SHKE_Enable_All_Periodic(_inst_,
      chip_select,
      device_type,
      concurrent_sr);
}



/** =============================================================================
 *  Function : Device_Init
 ** =============================================================================
 */
/**
 *    @details
 *    device initilization for LP/PC
 **/
void ABHN_SHKE_Device_Init( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 clk_freq_in_khz, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value )
{

  ABHN_SHKE_Disable_All_Periodic(_inst_,
      chip_select);

  if (ENABLE_LPDDR2 &&(device_type == SDRAM_LPDDR2)) {
    ABHN_SHKE_Device_Init_LPDDR2(_inst_,
        chip_select,
        clk_freq_in_khz);
  }
  else
  {
    if (ENABLE_LPDDR3 &&(device_type == SDRAM_LPDDR3)) {
      ABHN_SHKE_Device_Init_LPDDR3(_inst_,
          chip_select,
          clk_freq_in_khz,
          MR1_value,
          MR2_value);
    }
    else
    {
      ABHN_SHKE_Device_Init_PCDDR3(_inst_,
          chip_select,
          clk_freq_in_khz,
          MR0_value,
          MR1_value,
          MR2_value);
    }
  }
}



/** =============================================================================
 *  Function : Device_Init_LPDDR2
 ** =============================================================================
 */
/**
 *    @details
 *    LPDDR2 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 **/
void ABHN_SHKE_Device_Init_LPDDR2( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz )
{
 #if ENABLE_LPDDR2
  uint32 wdata;

  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_SHKE_CONFIG );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_CONFIG, tmp );
  ///  ------------------
  ///  start memory clock
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for ck_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CK_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  drive CKE high and wait 500us
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x11) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for cke_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CKE_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  reset DRAM
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x2) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x3F,
      0xFC);
  ///  ------------------
  ///  starting zqcal if the rank is initialized
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x14) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  ///  ------------------
  ///  zqcal cs0
  ///  ------------------
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs0 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs1 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x1,
      0xC3);
  ///  ------------------
  ///  mode register 2
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  if (clk_freq_in_khz <= 400000) {
    wdata = 0x1;
  }
  else
  {
    if (clk_freq_in_khz <= 533000) {
      wdata = 0x2;
    }
    else
    {
      if (clk_freq_in_khz <= 667000) {
        wdata = 0x3;
      }
      else
      {
        if (clk_freq_in_khz <= 800000) {
          wdata = 0x4;
        }
        else
        {
          if (clk_freq_in_khz <= 933000) {
            wdata = 0x5;
          }
          else
          {
            wdata = 0x6;
          }
        }
      }
    }
  }

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x2,
      wdata);
  ///  ------------------
  ///  clean up
  ///  ------------------
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0);

  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
 #endif
}



/** =============================================================================
 *  Function : Device_Init_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    LPDDR3 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 **/
void ABHN_SHKE_Device_Init_LPDDR3( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz, uint32 MR1_value, uint32 MR2_value )
{
#if ENABLE_LPDDR3
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_SHKE_CONFIG );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_CONFIG, tmp );
  ///  ------------------
  ///  start memory clock
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for ck_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CK_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  drive CKE high and wait 500us
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x11) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for cke_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CKE_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  reset DRAM
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x2) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x3F,
      0xFC);
  ///  ------------------
  ///  zqcal cs0
  ///  ------------------
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_DOMAIN, HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0x14);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs0 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_DOMAIN, HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0x14);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs1 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x2,
      MR2_value);
  ///  ------------------
  ///  mode register 1 (moving after MR2 setting is done)
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x5) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x1,
      MR1_value);
  ///  ------------------
  ///  clean up
  ///  ------------------
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0);

  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
 #endif 
}



/** =============================================================================
 *  Function : Device_Init_PCDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    PCDDR3 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 **/
void ABHN_SHKE_Device_Init_PCDDR3( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value )
{
#if ENABLE_PCDDR3
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_SHKE_CONFIG );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_CONFIG, tmp );
  ///  ------------------
  ///  start memory clock
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x7) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for ck_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CK_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  assert reset and wait 200us
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x7) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_ON_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for resetn_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RESETN_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  deassert reset and wait 500us
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_TIMER_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x12) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_OFF_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_OFF_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_OFF_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for resetn_off done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RESETN_OFF) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_RESETN_OFF_EXECUTE_FVAL );
  ///  ------------------
  ///  drive CKE high and wait 360ns
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x7) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  ------------------
  ///  poll for cke_on done
  ///  ------------------
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CKE_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL );
  ///  ------------------
  ///  mode register 2
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x8) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x2,
      MR2_value);
  ///  ------------------
  ///  mode register 3
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x8) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x3,
      0x0);
  ///  ------------------
  ///  mode register 1 to enable DLL
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x8) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x1,
      MR1_value);
  ///  ------------------
  ///  mode register 0 to reset DLL
  ///  ------------------
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_1 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_BMSK)) | ((0x8) << HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_BEFORE_HW_CLEAR_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_1, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x0,
      MR0_value);
  ///  ------------------
  ///  zqcal cs0
  ///  ------------------
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_DOMAIN, HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0x28);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs0 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_DOMAIN, HWIO_ABHN_SHKE_DRAM_MANUAL_1_WAIT_TIMER_DOMAIN_XO_CLOCK_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0x28);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    ///  ------------------
    ///  poll for zqcal cs1 done
    ///  ------------------
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_1, WAIT_TIMER_BEFORE_HW_CLEAR, 0);

  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL);
  }
 #endif
}



/** =============================================================================
 *  Function : Read_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Mode register read; applies to LPDDR2/3 device type 
 *    note: chip_select needs to be rank0 or rank 1
 **/
uint64 ABHN_SHKE_Read_MR( uint32 _inst_, uint32 chip_select, uint64 mr_addr )
{
  uint64 rdata_upper;
  uint64 rdata_lower;
  uint64 rdata = 0;


  if (chip_select == SDRAM_RANK_BOTH) {
    chip_select = SDRAM_RANK_CS0;
  }
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, chip_select);
  HWIO_OUTXF (_inst_, ABHN_SHKE_MREG_ADDR_WDATA_CNTL, MREG_ADDR, mr_addr);
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, MODE_REGISTER_READ, HWIO_ABHN_SHKE_DRAM_MANUAL_0_MODE_REGISTER_READ_EXECUTE_FVAL);

  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, MODE_REGISTER_READ) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_MODE_REGISTER_READ_EXECUTE_FVAL );

  if (chip_select == SDRAM_RANK_CS0) {
    rdata_upper = HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_RANK0_U, MREG_RDATA);
    rdata_lower = HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_RANK0_L, MREG_RDATA);
    rdata = (rdata_upper << 32) | rdata_lower;
    return ((rdata));
  }
  else
  {
    rdata_upper = HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_RANK1_U, MREG_RDATA);
    rdata_lower = HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_RANK1_L, MREG_RDATA);
    rdata = (rdata_upper << 32) | rdata_lower;
    return ((rdata));
  }
}



/** =============================================================================
 *  Function : Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Mode register write; applies to LPDDR2/3 & PCDDR3 device types 
 *    including the extend mode register write
 **/
void ABHN_SHKE_Write_MR( uint32 _inst_, uint32 chip_select, uint32 mr_addr, uint32 mr_data )
{


  if ((mr_addr == 41) || (mr_addr == 42) || (mr_addr == 48)) {
    ABHN_SHKE_Extended_Write_MR(_inst_,
        chip_select,
        mr_addr,
        mr_data);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, chip_select);
    HWIO_OUTXF (_inst_, ABHN_SHKE_MREG_ADDR_WDATA_CNTL, MREG_ADDR, mr_addr);
    HWIO_OUTXF (_inst_, ABHN_SHKE_MREG_ADDR_WDATA_CNTL, MREG_WDATA, mr_data);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, MODE_REGISTER_WRITE, HWIO_ABHN_SHKE_DRAM_MANUAL_0_MODE_REGISTER_WRITE_EXECUTE_FVAL);

    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, MODE_REGISTER_WRITE) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_MODE_REGISTER_WRITE_EXECUTE_FVAL );
  }
}



/** =============================================================================
 *  Function : Extended_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Extended mode register write
 *  only applies to LPDDR3 device types which require CA calibration (calibration of the DDR command bus) 
 **/
void ABHN_SHKE_Extended_Write_MR( uint32 _inst_, uint32 chip_select, uint32 mr_addr, uint32 mr_data )
{
  uint32 tmp;

  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, chip_select);
  tmp = HWIO_INX (_inst_, ABHN_SHKE_MREG_ADDR_WDATA_CNTL );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_MREG_ADDR_WDATA_CNTL_MREG_ADDR_BMSK)) | ((mr_addr) << HWIO_ABHN_SHKE_MREG_ADDR_WDATA_CNTL_MREG_ADDR_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_MREG_ADDR_WDATA_CNTL_MREG_WDATA_BMSK)) | ((mr_data) << HWIO_ABHN_SHKE_MREG_ADDR_WDATA_CNTL_MREG_WDATA_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_MREG_ADDR_WDATA_CNTL, tmp );
  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXTND_MODE_REGISTER_WRITE, HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXTND_MODE_REGISTER_WRITE_EXECUTE_FVAL);

  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXTND_MODE_REGISTER_WRITE) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXTND_MODE_REGISTER_WRITE_EXECUTE_FVAL );
}



/** =============================================================================
 *  Function : Enter_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs into SW_based self refresh
 *    The memory controller provides SW the ability to enter and exit self refresh. 
 *    This is intended to be used for debug and for a few SW based system sequences.
 **/
void ABHN_SHKE_Enter_Self_Refresh( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  ///  disable hw self refresh
  tmp = HWIO_INX (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_BMSK)) | ((HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_DISABLE_FVAL) << HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_BMSK)) | ((HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_DISABLE_FVAL) << HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_BMSK)) | ((HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_DISABLE_FVAL) << HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, tmp );
  ///  disable PHY periodic IO calibration, since they won't get acks, have moved to ddrss level
  ///  start real sequence
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  ///  poll for self refresh entry done
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ENTER_SELF_REFRESH_IDLE) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_SELF_REFRESH_IDLE_EXECUTE_FVAL );
  ///  poll for status to indicate self refresh
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_STATUS, SW_SELF_RFSH) == HWIO_ABHN_SHKE_DRAM_STATUS_SW_SELF_RFSH_NOT_IN_SELF_REFRESH_FVAL );
}



/** =============================================================================
 *  Function : Exit_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs out of SW_based self refresh
 **/
void ABHN_SHKE_Exit_Self_Refresh( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );

  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXIT_SELF_REFRESH) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_SELF_REFRESH_EXECUTE_FVAL );

  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_STATUS, SW_SELF_RFSH) == HWIO_ABHN_SHKE_DRAM_STATUS_SW_SELF_RFSH_IN_SELF_REFRESH_FVAL );
}



/** =============================================================================
 *  Function : Enable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    This mode is entirely HW controlled. 
 *    The controller will enter self refresh if the controller is idle for a programmable number of cycles 
 **/
void ABHN_SHKE_Enable_HW_Self_Refresh( uint32 _inst_, uint32 chip_select, uint32 concurrent_sr )
{

  ///  Concurrent self-refresh is needed for dual rank where inactive rank
  ///  is providing ODT.
  ///  Recommended for Dual rank PCDDR3 & Dual rank external LPDDR3
  if (concurrent_sr) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_DISABLE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_DISABLE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, CONCURRENT_SELF_RFSH_EN, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_ENABLE_FVAL);
  }
  else
  {
    ///  enable hw self refresh
    if (chip_select & SDRAM_RANK_CS0) {
      HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_ENABLE_FVAL);
    }
    if (chip_select & SDRAM_RANK_CS1) {
      HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_ENABLE_FVAL);
    }
  }
}



/** =============================================================================
 *  Function : Disable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs out of HW_based self refresh
 **/
void ABHN_SHKE_Disable_HW_Self_Refresh( uint32 _inst_, uint32 chip_select )
{

  ///  disable concurrent self refresh
  HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, CONCURRENT_SELF_RFSH_EN, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_CONCURRENT_SELF_RFSH_EN_DISABLE_FVAL);
  ///  disable hw self refresh
  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK0_DISABLE_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, HW_SELF_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_SELF_REFRESH_CNTL_HW_SELF_RFSH_ENABLE_RANK1_DISABLE_FVAL);
  }
}



/** =============================================================================
 *  Function : Enable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Enable periodic auto refresh
 **/
void ABHN_SHKE_Enable_Auto_Refresh( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_ENABLE_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_ENABLE_FVAL);
  }
}



/** =============================================================================
 *  Function : Disable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Disable periodic auto refresh
 **/
void ABHN_SHKE_Disable_Auto_Refresh( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_DISABLE_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_DISABLE_FVAL);
  }
}



/** =============================================================================
 *  Function : Manual_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Enable ZQ Calibration
 *    can only be done when the device is in idle state with all banks are precharged
 **/
void ABHN_SHKE_Manual_ZQCAL( uint32 _inst_, uint32 short_seq )
{

  if (short_seq == 1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_SHORT, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_SHORT_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_SHORT) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_SHORT_EXECUTE_FVAL );
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ZQCAL_LONG) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ZQCAL_LONG_EXECUTE_FVAL );
  }
}



/** =============================================================================
 *  Function : Enable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Enable periodic ZQ Calibration
 *    note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the ranks present
 **/
void ABHN_SHKE_Enable_Periodic_ZQCAL( uint32 _inst_, uint32 chip_select )
{

  ///  Periodic ZQCAL interval is already programmed in static_settings()
  HWIO_OUTXF (_inst_, ABHN_SHKE_PERIODIC_ZQCAL, RANK_SEL, chip_select);
}



/** =============================================================================
 *  Function : Disable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Disable periodic ZQ Calibration
 *    note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the disable ranks 
 **/
void ABHN_SHKE_Disable_Periodic_ZQCAL( uint32 _inst_, uint32 chip_select )
{
  uint32 enabled;


  enabled = HWIO_INXF (_inst_, ABHN_SHKE_PERIODIC_ZQCAL, RANK_SEL);
  enabled &=  ~ chip_select;
  HWIO_OUTXF (_inst_, ABHN_SHKE_PERIODIC_ZQCAL, RANK_SEL, enabled);
}



/** =============================================================================
 *  Function : Enable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *    DRAM Temperature Status Read (SRR)
 *    Enable periodic SRR; The memory controller is able to change the refresh rate based on the
 *    temperature range where the memory device is operating.
 *    This is only applicable for LPDDR2/3 devices.
 *    note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the ranks present
 **/
void ABHN_SHKE_Enable_Periodic_SRR( uint32 _inst_, uint32 chip_select )
{

  ///  SRR register address & interval are already programmed in static_settings()
  HWIO_OUTXF (_inst_, ABHN_SHKE_PERIODIC_MRR, MRR_RANK_SEL, chip_select);
}



/** =============================================================================
 *  Function : Disable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *    Disable  periodic SRR 
 *    note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the disable ranks 
 **/
void ABHN_SHKE_Disable_Periodic_SRR( uint32 _inst_, uint32 chip_select )
{
  uint32 enabled;


  enabled = HWIO_INXF (_inst_, ABHN_SHKE_PERIODIC_MRR, MRR_RANK_SEL);
  enabled &=  ~ chip_select;
  HWIO_OUTXF (_inst_, ABHN_SHKE_PERIODIC_MRR, MRR_RANK_SEL, enabled);
}



/** =============================================================================
 *  Function : Manual_SRR
 ** =============================================================================
 */
/**
 *    @details
 *    SW can trigger a read of the temperature status register  
 **/
uint64 ABHN_SHKE_Manual_SRR( uint32 _inst_, uint32 chip_select )
{

  return ((ABHN_SHKE_Read_MR(_inst_ , chip_select, 0x4)));
}



/** =============================================================================
 *  Function : Enter_DPD
 ** =============================================================================
 */
/**
 *    @details
 *    Enter Deep Power Down (DPD), a low power state of the memory device wherein no data is retained
 *    A complete re_initialization sequence is needed for the memory back to normal mode
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_SHKE_Enter_DPD( uint32 _inst_, uint32 chip_select )
{

  ABHN_SHKE_Disable_Periodic_ZQCAL(_inst_,
      chip_select);
  ABHN_SHKE_Disable_HW_Self_Refresh(_inst_,
      chip_select);
  ///  SW issues the manual DPD, if two banks need to go DPD, needs to be done sequentially
  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ENTER_DEEP_PD, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ENTER_DEEP_PD) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_EXECUTE_FVAL );
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ENTER_DEEP_PD, HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, ENTER_DEEP_PD) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_ENTER_DEEP_PD_EXECUTE_FVAL );
  }
}



/** =============================================================================
 *  Function : Enable_CKE
 ** =============================================================================
 */
/**
 *    @details
 *    Turn on CKE
 **/
void ABHN_SHKE_Enable_CKE( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  ///  enable CKE
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CKE_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_ON_EXECUTE_FVAL );
}



/** =============================================================================
 *  Function : Disable_CKE
 ** =============================================================================
 */
/**
 *    @details
 *    Turn off CKE
 **/
void ABHN_SHKE_Disable_CKE( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  ///  disable CKE
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CKE_OFF) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_EXECUTE_FVAL );
}



/** =============================================================================
 *  Function : Enable_CK
 ** =============================================================================
 */
/**
 *    @details
 *    Turn on CK
 **/
void ABHN_SHKE_Enable_CK( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  ///  disable memory clock
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CK_ON) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_ON_EXECUTE_FVAL );
}



/** =============================================================================
 *  Function : Disable_CK
 ** =============================================================================
 */
/**
 *    @details
 *    Turn off CK
 **/
void ABHN_SHKE_Disable_CK( uint32 _inst_, uint32 chip_select )
{
  uint32 tmp;

  ///  disable memory clock
  tmp = HWIO_INX (_inst_, ABHN_SHKE_DRAM_MANUAL_0 );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_BMSK)) | ((chip_select) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_OFF_BMSK)) | ((HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_OFF_EXECUTE_FVAL) << HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_OFF_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_DRAM_MANUAL_0, tmp );
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CK_OFF) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CK_OFF_EXECUTE_FVAL );
}

/** =============================================================================
 *  Function : Enable_INITCOMPLETE
 ** =============================================================================
 */
/**
 *    @details
 *    Set INITCOMPLETE bit
 *    Once the rank enable and rank initcomplete bits are cleared, the status of the rank is uninitialized.
 **/
void ABHN_SHKE_Enable_INITCOMPLETE( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, 1);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, 1);
  }
}

/** =============================================================================
 *  Function : Disable_INITCOMPLETE
 ** =============================================================================
 */
/**
 *    @details
 *    clear INITCOMPLETE bit
 **/
void ABHN_SHKE_Disable_INITCOMPLETE( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, 0);
  }
  else
  {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, 0);
  }
}



/** =============================================================================
 *  Function : Exit_DPD
 ** =============================================================================
 */
/**
 *    @details
 *    Exit Deep Power Down (DPD) mode, bring the memory out of DPD mode
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_SHKE_Exit_DPD( uint32 _inst_, uint32 chip_select )
{

  ///  SW issues the manual DPD, if two banks need to go DPD, needs to be done sequentially
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS0_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXIT_DEEP_PD, HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXIT_DEEP_PD) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_EXECUTE_FVAL );
  }
  if (chip_select & HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RANK_SEL_CS1_ONLY_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXIT_DEEP_PD, HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, EXIT_DEEP_PD) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_EXIT_DEEP_PD_EXECUTE_FVAL );
  }
}



/** =============================================================================
 *  Function : Rank_Detection_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    The Rank Auto Detect sequence is used by SW to determine if a memory device is present on a specific rank.
 **/
uint32 ABHN_SHKE_Rank_Detection_LPDDR3( uint32 _inst_ )
{
  uint32 populated_ranks = SDRAM_RANK_NONE;
#if ENABLE_LPDDR3
  uint32 tmp;

  ///  this is optional, since these haven't been enabled yet
  ABHN_SHKE_Disable_Periodic_SRR(_inst_,
      SDRAM_RANK_BOTH);
  ABHN_SHKE_Disable_Periodic_ZQCAL(_inst_,
      SDRAM_RANK_BOTH);
  ABHN_SHKE_Disable_HW_Self_Refresh(_inst_,
      SDRAM_RANK_BOTH);
  ABHN_SHKE_Disable_Auto_Refresh(_inst_,
      SDRAM_RANK_BOTH);
  tmp = HWIO_INX (_inst_, ABHN_SHKE_CONFIG );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_CONFIG, tmp );

  ABHN_SHKE_Read_MR(_inst_,
      SDRAM_RANK_CS0,
      0x20);
  if (HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_STATUS, MREG_TIMEOUT) == HWIO_ABHN_SHKE_MREG_RDATA_STATUS_MREG_TIMEOUT_TIMEOUT_FVAL) {
    ///  rank is NOT populated        
    ABHN_SHKE_Disable_CKE(_inst_,
        SDRAM_RANK_CS0);
    ABHN_SHKE_Disable_CK(_inst_,
        SDRAM_RANK_CS0);
    ABHN_SHKE_Disable_INITCOMPLETE(_inst_,
        SDRAM_RANK_CS0);
    ABHN_SHKE_Disable_Rank(_inst_,
        SDRAM_RANK_CS0);
  }
  else
  {
    ///  rank is populated
    populated_ranks |= SDRAM_RANK_CS0;
  }

  ABHN_SHKE_Read_MR(_inst_,
      SDRAM_RANK_CS1,
      0x20);
  if (HWIO_INXF (_inst_, ABHN_SHKE_MREG_RDATA_STATUS, MREG_TIMEOUT) == HWIO_ABHN_SHKE_MREG_RDATA_STATUS_MREG_TIMEOUT_TIMEOUT_FVAL) {
    ///  rank is NOT populated        
    ABHN_SHKE_Disable_CKE(_inst_,
        SDRAM_RANK_CS1);
    ABHN_SHKE_Disable_CK(_inst_,
        SDRAM_RANK_CS1);
    ABHN_SHKE_Disable_INITCOMPLETE(_inst_,
        SDRAM_RANK_CS1);
    ABHN_SHKE_Disable_Rank(_inst_,
        SDRAM_RANK_CS1);
  }
  else
  {
    ///  rank is populated
    populated_ranks |= SDRAM_RANK_CS1;
  }
#endif
  return ((populated_ranks));
 
}



/** =============================================================================
 *  Function : Rank_Detection_PCDDR3_Pre/Post
 ** =============================================================================
 */
/**
 *    @details
 *    The Rank Auto Detect sequence is used by SW to determine if a memory device is present on a specific rank.
 **/
void ABHN_SHKE_Rank_Detection_PCDDR3_Pre( uint32 _inst_, uint32 chip_select )
{
#if ENABLE_PCDDR3
  uint32 tmp;

  ///  this is optional, since these haven't been enabled yet
  ABHN_SHKE_Disable_Periodic_SRR(_inst_,
      chip_select);
  ABHN_SHKE_Disable_Periodic_ZQCAL(_inst_,
      chip_select);
  ABHN_SHKE_Disable_HW_Self_Refresh(_inst_,
      chip_select);
  ABHN_SHKE_Disable_Auto_Refresh(_inst_,
      chip_select);
  tmp = HWIO_INX (_inst_, ABHN_SHKE_CONFIG );
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK0_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK0_EN_SHFT ) ;
  tmp = (tmp & ~(HWIO_ABHN_SHKE_CONFIG_RANK1_EN_BMSK)) | ((HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL) << HWIO_ABHN_SHKE_CONFIG_RANK1_EN_SHFT ) ;
  HWIO_OUTX (_inst_, ABHN_SHKE_CONFIG, tmp );

  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x3,
      0x4);
  /// enable MPR mode
  ABHN_SHKE_Read_DQCAL(_inst_,
      chip_select);
 #endif
}

void ABHN_SHKE_Rank_Detection_PCDDR3_Post( uint32 _inst_, uint32 chip_select )
{
#if ENABLE_PCDDR3
  ABHN_SHKE_Write_MR(_inst_,
      chip_select,
      0x3,
      0);
  /// disable MPR mode
 #endif
}



/** =============================================================================
 *  Function : Enable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *    Enable SHKE Rank
 **/
void ABHN_SHKE_Enable_Rank( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_EN, HWIO_ABHN_SHKE_CONFIG_RANK0_EN_ENABLE_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_EN, HWIO_ABHN_SHKE_CONFIG_RANK1_EN_ENABLE_FVAL);
  }
}



/** =============================================================================
 *  Function : Disable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *    Disable SHKE Rank
 **/
void ABHN_SHKE_Disable_Rank( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_INCOMPLETE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK0_EN, HWIO_ABHN_SHKE_CONFIG_RANK0_EN_DISABLE_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE, HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_INCOMPLETE_FVAL);
    HWIO_OUTXF (_inst_, ABHN_SHKE_CONFIG, RANK1_EN, HWIO_ABHN_SHKE_CONFIG_RANK1_EN_DISABLE_FVAL);
  }
}



/** =============================================================================
 *  Function : Enable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *    Enable all periodic events in SHKE, including SRR, ZQCAL, and auto refresh
 **/
void ABHN_SHKE_Enable_All_Periodic( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 concurrent_sr )
{

  ABHN_SHKE_Enable_Auto_Refresh(_inst_,
      chip_select);
  
  ABHN_SHKE_Enable_HW_Self_Refresh(_inst_, chip_select, concurrent_sr);
  
  ABHN_SHKE_Enable_Periodic_ZQCAL(_inst_,
      chip_select);

  if (device_type == SDRAM_LPDDR2 || device_type == SDRAM_LPDDR3) {
    ABHN_SHKE_Enable_Periodic_SRR(_inst_,
        chip_select);
  }
}



/** =============================================================================
 *  Function : Disable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *    Disable all periodic events in SHKE, including SRR, ZQCAL, and auto refresh
 **/
void ABHN_SHKE_Disable_All_Periodic( uint32 _inst_, uint32 chip_select )
{

  ABHN_SHKE_Disable_Auto_Refresh(_inst_,
      chip_select);
  ABHN_SHKE_Disable_HW_Self_Refresh(_inst_,
      chip_select);
  ABHN_SHKE_Disable_Periodic_ZQCAL(_inst_,
      chip_select);
  ABHN_SHKE_Disable_Periodic_SRR(_inst_,
      chip_select);
}



/** =============================================================================
 *  Function : Get_Init_Complete
 ** =============================================================================
 */
/**
 *    @details
 *    used for Ram Size Detection in LPDDR3, check the inicomplete status
 **/
uint32 ABHN_SHKE_Get_Init_Complete( uint32 _inst_, uint32 chip_select )
{
  uint32 rank_complete = 0;


  if ((chip_select & SDRAM_RANK_CS0) && (HWIO_INXF (_inst_, ABHN_SHKE_CONFIG, RANK0_INITCOMPLETE) == HWIO_ABHN_SHKE_CONFIG_RANK0_INITCOMPLETE_INIT_COMPLETE_FVAL)) {
    rank_complete = rank_complete | SDRAM_RANK_CS0;
  }
  if ((chip_select & SDRAM_RANK_CS1) && (HWIO_INXF (_inst_, ABHN_SHKE_CONFIG, RANK1_INITCOMPLETE) == HWIO_ABHN_SHKE_CONFIG_RANK1_INITCOMPLETE_INIT_COMPLETE_FVAL)) {
    rank_complete = rank_complete | SDRAM_RANK_CS1;
  }
  return ((rank_complete));
}



/** =============================================================================
 *  Function : Read_DQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    the DQ calibration read command. This relates to the MRR command for LPDDR3 and 
 *    the MPR command for PCDDR3. This command is used as part of the read enable (or read 
 *    capture window (rcw)) training for LPDDR3 and PCDDR3. This command can also be used 
 *    for PCDDR3 Rank Auto detect
 **/
void ABHN_SHKE_Read_DQCAL( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, SDRAM_RANK_CS0);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RD_DQCAL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RD_DQCAL_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RD_DQCAL) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_RD_DQCAL_EXECUTE_FVAL );
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, SDRAM_RANK_CS1);
    HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RD_DQCAL, HWIO_ABHN_SHKE_DRAM_MANUAL_0_RD_DQCAL_EXECUTE_FVAL);
    while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, RD_DQCAL) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_RD_DQCAL_EXECUTE_FVAL );
  }
}



/** =============================================================================
 *  Function : CA_Training_Toggle_Pattern 
 ** =============================================================================
 */
/**
 *    @details
 *    for CA training to toggle the pattern
 *    note: During CA calibration, the DRAM samples the CA bus and drives the sampled value back on 
 *    the DQ bus. SW can then perform the necessary checks on the sampled data pattern by reading the PHY CSRs.
 **/
__attribute__((section("ddr_boot_funcs")))
void ABHN_SHKE_CA_Training_Toggle_Pattern( uint32 _inst_ )
{

  HWIO_OUTXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CA_TRAIN_PATTERN_CMD, HWIO_ABHN_SHKE_DRAM_MANUAL_0_CA_TRAIN_PATTERN_CMD_EXECUTE_FVAL);
  while (HWIO_INXF (_inst_, ABHN_SHKE_DRAM_MANUAL_0, CA_TRAIN_PATTERN_CMD) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CA_TRAIN_PATTERN_CMD_EXECUTE_FVAL );
}



/** =============================================================================
 *  Function : Get_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    get the status of Auto refresh for the ranks
 **/
uint32 ABHN_SHKE_Get_Auto_Refresh_Status( uint32 _inst_ )
{
  uint32 auto_refresh_status = 0;

  auto_refresh_status = ((HWIO_INXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1) << 1) | HWIO_INXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0));	
  return ((auto_refresh_status));
}

/** =============================================================================
 *  Function : Set_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    Sets the status of Auto refresh for the ranks
 **/
void ABHN_SHKE_Set_Auto_Refresh_Status( uint32 _inst_, uint32 chip_select )
{
  switch(chip_select){
    case 0 :
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_DISABLE_FVAL);
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_DISABLE_FVAL);
      break;
    case 1 :
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_DISABLE_FVAL);
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_ENABLE_FVAL);
      break;
    case 2 :
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_ENABLE_FVAL);
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_DISABLE_FVAL);
      break;
    case 3 :
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK1, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK1_ENABLE_FVAL);
      HWIO_OUTXF (_inst_, ABHN_SHKE_AUTO_REFRESH_CNTL, AUTO_RFSH_ENABLE_RANK0, HWIO_ABHN_SHKE_AUTO_REFRESH_CNTL_AUTO_RFSH_ENABLE_RANK0_ENABLE_FVAL);
      break;
  }
}

/** =============================================================================
 *  Function : Get_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    gets the HW self refresh register value for the ranks
 **/
uint32 ABHN_SHKE_Get_HW_Self_Refresh_Reg( uint32 _inst_ )
{
  return (HWIO_INX (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL));
}

/** =============================================================================
 *  Function : Set_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    restores the HW self refresh register value for the ranks.
 **/
void ABHN_SHKE_Set_HW_Self_Refresh_Reg( uint32 _inst_, uint32 reg_value)
{
  HWIO_OUTX (_inst_, ABHN_SHKE_SELF_REFRESH_CNTL, reg_value );
}


















