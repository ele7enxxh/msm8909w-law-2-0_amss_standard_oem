/**
 * @file bimc_mc_scmo.c
 * @brief  BIMC SCMO APIs
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
05/30/14   rj         Updated size for 32 bit device.
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
#include "bimc_mc_scmo.h"
#include "bimc_config.h"

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
/** =============================================================================
 **  Function : Post_Init_Setup 
 ** =============================================================================
 */
/** 
 *  @details
 *    post initilization for SCMO; updating number of banks, rows, columns, address base etc.
 **/
void SCMO_CFG_Post_Init_Setup( uint32 _inst_, uint32 index, uint32 num_banks, uint32 num_rows, uint32 num_cols, uint32 interleave_en, uint32 addr_base )
{
  uint32 size_in_mb = 0;

  uint32 tmp;

  ///   in mbytes (rows, cols are address bits, banks is actual count)
  size_in_mb = ((1 << (num_rows + num_cols - 20)) * num_banks * BIMC_NUM_BYTES);

  if (interleave_en & SDRAM_RANK_CS0) {
    HWIO_OUTXF (_inst_, SCMO_CFG_SLV_INTERLEAVE_CFG, INTERLEAVE_CS0, 1);
  }
  if (interleave_en & SDRAM_RANK_CS1) {
    HWIO_OUTXF (_inst_, SCMO_CFG_SLV_INTERLEAVE_CFG, INTERLEAVE_CS1, 1);
  }
  HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, ADDR_MODE, HWIO_SCMO_CFG_ADDR_MAP_CSN_ADDR_MODE_RKRBC_FVAL);

  if (num_banks == 4) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, BANK_SIZE, HWIO_SCMO_CFG_ADDR_MAP_CSN_BANK_SIZE_BANKS_4_FVAL);
  }
  else
  {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, BANK_SIZE, HWIO_SCMO_CFG_ADDR_MAP_CSN_BANK_SIZE_BANKS_8_FVAL);
  }
  HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, ROW_SIZE, num_rows - 13);
  HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, COL_SIZE, num_cols - 8);
  tmp = (addr_base >> 26);
  HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_BASE_CSN, index, ADDR_BASE, tmp);

  switch(size_in_mb){
    case 8192 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_8GB_FVAL);
      break;
    case 4096 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_4GB_FVAL);
      break;
    case 2048 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_2GB_FVAL);
      break;
    case 1024 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_1GB_FVAL);
      break;
    case 512 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_512MB_FVAL);
      break;
    case 256 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_256MB_FVAL);
      break;
    case 128 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_128MB_FVAL);
      break;
    case 64 :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_DENSITY_64MB_FVAL);
      break;
    default :
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MASK_CSN, index, ADDR_MASK, HWIO_SCMO_CFG_ADDR_MASK_CSN_ADDR_MASK_RESERVED_FVAL);
      break;
  }
  if (num_banks != 0) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, index, RANK_EN, 1);
  }
}

/** =============================================================================
 **  Function : Enable_Rank
 ** =============================================================================
 */
/** 
 *    @details
 *    Enable rank(s) in SCMO registers
 * 
 *    @param[in] chip_select  chip select to set
 **/
void SCMO_CFG_Enable_Rank( uint32 _inst_, uint32 chip_select )
{

  /// Set SCMO_ADDR_MAP_CSn[RANK_EN] = 0 for the ranks which needs to be disabled
  if (chip_select & SDRAM_RANK_CS0) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 0, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_IS_PRESENT_FVAL);
  }
  if (chip_select & SDRAM_RANK_CS1) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 1, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_IS_PRESENT_FVAL);
  }
}

/** =============================================================================
 **  Function : Disable_Rank
 ** =============================================================================
 */
/** 
 *    @details
 *    Disable rank(s) in SCMO registers
 * 
 *    @param[in] chip_select  chip select to disable
 **/
void SCMO_CFG_Disable_Rank( uint32 _inst_, uint32 chip_select )
{

  if (chip_select & SDRAM_RANK_BOTH) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 0, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 1, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
  }
  else
  {
    if (chip_select & SDRAM_RANK_CS0) {
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 0, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
    }
    else
    {
      if (chip_select & SDRAM_RANK_CS1) {
        HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 1, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
      }
    }
  }
}

/** =============================================================================
 **  Function : Disable_Rank_With_Flush
 ** =============================================================================
 */
/**
 *    @details
 *    used for enter DPD sequence
 * 
 *    @param[in] chip_select  chip select to set
 **/
void SCMO_CFG_Disable_Rank_With_Flush( uint32 _inst_, uint32 chip_select )
{

  ///  if both ranks need to be disabled, do both in a single step
  if (chip_select & SDRAM_RANK_BOTH) {
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 0, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
    HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 1, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
    HWIO_OUTXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF, 0x3);

    while (HWIO_INXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF) != 0 );
    while (HWIO_INXF (_inst_, SCMO_CFG_SLV_STATUS, RANK_IDLE) == 0 );
  }
  else
  {
    if (chip_select & SDRAM_RANK_CS0) {
      HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 0, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
      HWIO_OUTXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF, 0x1);

      while ((HWIO_INXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF) & 0x1) != 0 );
      while ((HWIO_INXF (_inst_, SCMO_CFG_SLV_STATUS, RANK_IDLE) & 0x1) == 0 );
    }
    else
    {
      if (chip_select & SDRAM_RANK_CS1) {
        HWIO_OUTXFI(_inst_, SCMO_CFG_ADDR_MAP_CSN, 1, RANK_EN, HWIO_SCMO_CFG_ADDR_MAP_CSN_RANK_EN_RANK_NOT_PRESENT_FVAL);
        HWIO_OUTXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF, 0x2);

        while ((HWIO_INXF (_inst_, SCMO_CFG_FLUSH_CMD, FLUSH_ALL_BUF) & 0x2) != 0 );
        while ((HWIO_INXF (_inst_, SCMO_CFG_SLV_STATUS, RANK_IDLE) & 0x2) == 0 );
      }
    }
  }
}


