/**
 * @file bimc.c
 * @brief  Bus Integrated Memory Controller (BIMC)  
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
05/09/14   nevedeth   Included API to get/set HW_Self_Refresh_Enable register value for the ranks
04/08/14   nevedeth   Updated the code to support the right device density value. 
03/16/14   nevedeth   Included API for LP3 ODTOFF_MIN during pre-clock switch.
02/06/14   nevedeth   Update APIs to get/set Auto-Refresh status
02/03/14   cchou      Update APIs to Enable/Disable ODT pin
01/21/14   cchou      Update API for restore/store RCW results from/to SW
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
#include "ddr_params.h"
#include "bimc_scale.h"
#include "ddr_target.h"

#ifndef SEQ_BIMC_BIMC_S_DDR1_SCMO_OFFSET
#define SEQ_BIMC_BIMC_S_DDR1_SCMO_OFFSET SEQ_BIMC_BIMC_S_DDR0_SCMO_OFFSET
#endif

#ifndef SEQ_BIMC_BIMC_S_DDR1_DPE_OFFSET
#define SEQ_BIMC_BIMC_S_DDR1_DPE_OFFSET SEQ_BIMC_BIMC_S_DDR0_DPE_OFFSET
#endif

#ifndef SEQ_BIMC_BIMC_S_DDR1_SHKE_OFFSET
#define SEQ_BIMC_BIMC_S_DDR1_SHKE_OFFSET SEQ_BIMC_BIMC_S_DDR0_SHKE_OFFSET
#endif

#ifndef SEQ_BIMC_GLOBAL0_OFFSET
#define SEQ_BIMC_GLOBAL0_OFFSET SEQ_BIMC_BIMC_MISC_OFFSET
#endif

#define REG_OFFSET_SCMO(INTERFACE) ((INTERFACE == SDRAM_INTERFACE_0) ? \
SEQ_BIMC_BIMC_S_DDR0_SCMO_OFFSET : \
    SEQ_BIMC_BIMC_S_DDR1_SCMO_OFFSET)

#define REG_OFFSET_DPE(INTERFACE)  ((INTERFACE == SDRAM_INTERFACE_0) ? \
    SEQ_BIMC_BIMC_S_DDR0_DPE_OFFSET  : \
    SEQ_BIMC_BIMC_S_DDR1_DPE_OFFSET)

#define REG_OFFSET_SHKE(INTERFACE) ((INTERFACE == SDRAM_INTERFACE_0) ? \
    SEQ_BIMC_BIMC_S_DDR0_SHKE_OFFSET : \
    SEQ_BIMC_BIMC_S_DDR1_SHKE_OFFSET)
    
/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
/* =============================================================================
 **  Function : BIMC_Pre_Init_Setup
 ** =============================================================================
 */
/**
 *   @brief
 *      one-time settings for SCMO/DPE/SHKE/BIMC_MISC; values are provided in bimc_config.c parsing from SWI
 *      AC Timing Parameters is update by SW CDT at the end
 *      ODT is on (now hard-coded for initialzing ODT for rank 0)
 **/
void
BIMC_Pre_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz, uint32 clk_period_in_ps)
{
  uint32 clk_mode = 0;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if(interface == SDRAM_INTERFACE_0) {
    BIMC_GLOBAL0_Set_Config( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface) ;       
  }

  if (ddr_params->common.device_type == SDRAM_LPDDR3)
    clk_mode = Get_BIMC_LUT (interface, clk_freq_in_khz, 5);
  else if (ddr_params->common.device_type == SDRAM_PCDDR3)
    clk_mode = Get_BIMC_LUT (interface, clk_freq_in_khz, 6);

  BIMC_MISC_GLOBAL_CSR_Mode_Switch(reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface, clk_mode);    
  BIMC_MISC_GLOBAL_CSR_Update_Clock_Period (reg_base + SEQ_BIMC_GLOBAL0_OFFSET,
      interface, CLK_PERIOD_RESOLUTION_1PS, clk_period_in_ps);

  BIMC_SCMO_Set_Config(reg_base + REG_OFFSET_SCMO(interface), interface);

  BIMC_DPE_Set_Config(reg_base+ REG_OFFSET_DPE(interface), interface);

  BIMC_SHKE_Set_Config(reg_base+ REG_OFFSET_SHKE(interface), interface);

  BIMC_Update_AC_Parameters(reg_base, interface, clk_freq_in_khz);
  
  if (ddr_params->common.device_type == SDRAM_PCDDR3)
    BIMC_ODT_Init(reg_base, interface, 1/*chip_select*/, 2/* ODT_mode*/);
}

/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT 
 **/
void
BIMC_Update_AC_Parameters(uint32 reg_base, SDRAM_INTERFACE interface,  uint32 clk_freq_in_khz)
{
  uint32 tREFI_value = 0;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if ( ENABLE_LPDDR2 && (ddr_params->common.device_type == SDRAM_LPDDR2))
  {
    BIMC_Update_AC_Parameters_LPDDR2(reg_base + REG_OFFSET_DPE(interface),
        interface, clk_freq_in_khz); 
    tREFI_value = ddr_params->lpddr.tREFI;
  }
  else if ( ENABLE_LPDDR3 && (ddr_params->common.device_type == SDRAM_LPDDR3))
  {
    BIMC_Update_AC_Parameters_LPDDR3(reg_base + REG_OFFSET_DPE(interface),
        interface, clk_freq_in_khz);
    tREFI_value = ddr_params->lpddr.tREFI;
  }
  else
  {
    BIMC_Update_AC_Parameters_PCDDR3(reg_base + REG_OFFSET_DPE(interface),
        interface, clk_freq_in_khz);
    tREFI_value = ddr_params->pcddr3.tREFI;
  }

  ABHN_DPE_Recalc_AC_Parameters(reg_base + REG_OFFSET_DPE(interface));

  ABHN_DPE_Load_All_Config(reg_base + REG_OFFSET_DPE(interface));

  ABHN_SHKE_Program_tREFI(reg_base + REG_OFFSET_SHKE(interface), tREFI_value);
}

/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_LPDDR2
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for LPDDR2
 **/
void
BIMC_Update_AC_Parameters_LPDDR2(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz)
{
  #if ENABLE_LPDDR2
	union ddr_device_params *ddr_params;
    ddr_params = ddr_get_params(interface);

	ABHN_DPE_Program_tRASmin (reg_base, ddr_params->lpddr.tRAS_Min);
	ABHN_DPE_Program_tWR (reg_base, ddr_params->lpddr.tWR);
	ABHN_DPE_Program_tRCD (reg_base, ddr_params->lpddr.tRCD);  
	ABHN_DPE_Program_tWTR (reg_base, ddr_params->lpddr.tWTR);
	ABHN_DPE_Program_tRRD (reg_base, ddr_params->lpddr.tRRD);
	ABHN_DPE_Program_tRFCAB (reg_base, ddr_params->lpddr.tRFC);
	ABHN_DPE_Program_tRFCPB (reg_base, ddr_params->lpddr.tRFC);
	ABHN_DPE_Program_tRTP (reg_base, ddr_params->lpddr.tRTP);
	ABHN_DPE_Program_tRP_AB (reg_base, ddr_params->lpddr.tRP_AB);
	ABHN_DPE_Program_tRP_PB (reg_base, ddr_params->lpddr.tRP_PB);
	ABHN_DPE_Program_tFAW (reg_base, ddr_params->lpddr.tFAW);
	ABHN_DPE_Program_tCKE (reg_base, ddr_params->lpddr.tCKE);
	ABHN_DPE_Program_tZQCS (reg_base, ddr_params->lpddr.tZQCS);
	ABHN_DPE_Program_tCKESR (reg_base, ddr_params->lpddr.tCKESR);
	ABHN_DPE_Program_tXSR (reg_base, ddr_params->lpddr.tXSR);
	ABHN_DPE_Program_tXP (reg_base, ddr_params->lpddr.tXP);
	ABHN_DPE_Program_tXPR_PCHG (reg_base, ddr_params->lpddr.tXP);
	ABHN_DPE_Program_ODTonmax_min (reg_base,
		ddr_params->lpddr.tDQSCK_min, ddr_params->lpddr.tODTonmax, ddr_params->lpddr.tODTonmin);
	ABHN_DPE_Program_ODT_OFF (reg_base, clk_freq_in_khz, 0, 0);
	ABHN_DPE_Program_RD_Latency(reg_base, 0);
	ABHN_DPE_Program_WR_Latency(reg_base, 0);
	ABHN_DPE_Program_tZQCL (reg_base, ddr_params->lpddr.tZQCL);
	ABHN_DPE_Program_tMRW_LP2 (reg_base, ddr_params->lpddr.tMRW);
	ABHN_DPE_Program_tMRR (reg_base, ddr_params->lpddr.tMRR);
  #endif
} 

/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_LPDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for LPDDR3
 **/
void
BIMC_Update_AC_Parameters_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz)
{
  #if ENABLE_LPDDR3
    union ddr_device_params *ddr_params;
    ddr_params = (ddr_get_params(interface));

    ABHN_DPE_Program_tRASmin (reg_base,
        ddr_params->lpddr.tRAS_Min);
    ABHN_DPE_Program_tWR (reg_base, ddr_params->lpddr.tWR);
	ABHN_DPE_Program_tRCD (reg_base, ddr_params->lpddr.tRCD);
	ABHN_DPE_Program_tWTR (reg_base, ddr_params->lpddr.tWTR);
	ABHN_DPE_Program_tRRD (reg_base, ddr_params->lpddr.tRRD);
	ABHN_DPE_Program_tRFCAB (reg_base, ddr_params->lpddr.tRFC);
	ABHN_DPE_Program_tRFCPB (reg_base, ddr_params->lpddr.tRFC);
	ABHN_DPE_Program_tRTP (reg_base, ddr_params->lpddr.tRTP);
	ABHN_DPE_Program_tRP_AB (reg_base, ddr_params->lpddr.tRP_AB);
	ABHN_DPE_Program_tRP_PB (reg_base, ddr_params->lpddr.tRP_PB);
	ABHN_DPE_Program_tFAW (reg_base, ddr_params->lpddr.tFAW);
	ABHN_DPE_Program_tCKE (reg_base, ddr_params->lpddr.tCKE);
	ABHN_DPE_Program_tZQCS (reg_base, ddr_params->lpddr.tZQCS);
	ABHN_DPE_Program_tCKESR (reg_base, ddr_params->lpddr.tCKESR);
	ABHN_DPE_Program_tXSR (reg_base, ddr_params->lpddr.tXSR);
	ABHN_DPE_Program_tXP (reg_base, ddr_params->lpddr.tXP);
	ABHN_DPE_Program_tXPR_PCHG (reg_base, ddr_params->lpddr.tXP);
	ABHN_DPE_Program_ODTonmax_min (reg_base,
        ddr_params->lpddr.tDQSCK_min, ddr_params->lpddr.tODTonmax, ddr_params->lpddr.tODTonmin);
	ABHN_DPE_Program_ODT_OFF (reg_base,clk_freq_in_khz,
		ddr_params->lpddr.tODToffmax, ddr_params->lpddr.tODToffmin);
	ABHN_DPE_Program_RD_Latency(reg_base, Get_BIMC_LUT (interface, clk_freq_in_khz, 1) );
	ABHN_DPE_Program_WR_Latency(reg_base, Get_BIMC_LUT (interface, clk_freq_in_khz, 0) );
	ABHN_DPE_Program_tZQCL (reg_base, ddr_params->lpddr.tZQCL);
	ABHN_DPE_Program_tMRW_LP3 (reg_base, ddr_params->lpddr.tMRW);
	ABHN_DPE_Program_tMRR (reg_base, ddr_params->lpddr.tMRR);
  #endif
}

/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_PCDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for PCDDR3
 **/
void
BIMC_Update_AC_Parameters_PCDDR3(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz)
{
  #if ENABLE_PCDDR3
	union ddr_device_params *ddr_params;
	ddr_params = ddr_get_params(interface);

	ABHN_DPE_Program_tRASmin (reg_base,
		ddr_params->pcddr3.tRAS_Min);
	ABHN_DPE_Program_tWR (reg_base, ddr_params->pcddr3.tWR);
	ABHN_DPE_Program_tRCD (reg_base, ddr_params->pcddr3.tRCD);
	ABHN_DPE_Program_tWTR (reg_base, ddr_params->pcddr3.tWTR);
	ABHN_DPE_Program_tRRD (reg_base, ddr_params->pcddr3.tRRD);
	ABHN_DPE_Program_tRFCAB (reg_base, ddr_params->pcddr3.tRFC);
	ABHN_DPE_Program_tRFCPB (reg_base, ddr_params->pcddr3.tRFC);
	ABHN_DPE_Program_tRTP (reg_base, ddr_params->pcddr3.tRTP);
	ABHN_DPE_Program_tRP (reg_base, ddr_params->pcddr3.tRP_AB);
	ABHN_DPE_Program_tFAW (reg_base, ddr_params->pcddr3.tFAW);
	ABHN_DPE_Program_tCKE (reg_base, ddr_params->pcddr3.tCKE);
	ABHN_DPE_Program_tZQCS (reg_base, ddr_params->pcddr3.tZQCS);
	ABHN_DPE_Program_tMOD (reg_base, ddr_params->pcddr3.tMOD);
	ABHN_DPE_Program_tCKESR (reg_base, ddr_params->pcddr3.tCKE);
	ABHN_DPE_Program_tXSDLL (reg_base, ddr_params->pcddr3.tXSDLL);
	ABHN_DPE_Program_tXS (reg_base, ddr_params->pcddr3.tXS);
	ABHN_DPE_Program_tXP (reg_base, ddr_params->pcddr3.tXP);
	ABHN_DPE_Program_tXPR_PCHG (reg_base, ddr_params->pcddr3.tXPDLL);
	ABHN_DPE_Program_tCKSRE (reg_base, ddr_params->pcddr3.tCKSRE);
	ABHN_DPE_Program_tCKSRX (reg_base, ddr_params->pcddr3.tCKSRX); 
	ABHN_DPE_Program_tAONmax_min (reg_base, ddr_params->pcddr3.tAONmax, ddr_params->pcddr3.tAONmin);
	ABHN_DPE_Program_RD_Latency(reg_base, Get_BIMC_LUT (interface, clk_freq_in_khz, 1) );
	ABHN_DPE_Program_WR_Latency(reg_base, Get_BIMC_LUT (interface, clk_freq_in_khz, 0) );
	ABHN_DPE_Program_tZQoper (reg_base, ddr_params->pcddr3.tZQoper);
	ABHN_DPE_Program_tMRW_PC3 (reg_base, ddr_params->pcddr3.tMRD);
  #endif
}

/* =============================================================================
 **  Function : BIMC_Device_Init
 ** =============================================================================
 */
/**
 *   @brief
 *      execute full device initialization sequence
 **/
 void
BIMC_Device_Init(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 chip_select, uint32 clk_freq_in_khz)
{
  union ddr_device_params *ddr_params;

  uint32 MR0_value, MR1_value, MR2_value;
  ddr_params = ddr_get_params(interface);

  if ( ENABLE_LPDDR3 && (ddr_params->common.device_type == SDRAM_LPDDR3)){
    MR0_value = 0;
    MR1_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 2); 
    MR2_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 3); 
  }
  else if (ENABLE_PCDDR3 && (ddr_params->common.device_type == SDRAM_PCDDR3)){
    MR0_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 2); 
    MR1_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 3); 
    MR2_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 5);  
  }
  else {  //LPDDR2
    MR0_value = 0;
    MR1_value = 0;
    MR2_value = 0;
  }

  ABHN_SHKE_Device_Init(reg_base + REG_OFFSET_SHKE(interface),
      chip_select, ddr_params->common.device_type,
      clk_freq_in_khz, MR0_value, MR1_value, MR2_value);
}

/* =============================================================================
 **  Function : BIMC_Rank_Detection
 ** =============================================================================
 */
/**
 *   @brief
 *      used by SW to determine if a memory device is present on a specific rank
 **/
uint32
BIMC_Rank_Detection(uint32 reg_base, SDRAM_INTERFACE interface)
{
  uint32 chip_select = SDRAM_CS_NONE;
  union ddr_device_params *ddr_params;

  ddr_params = ddr_get_params(interface);

  if (ENABLE_PCDDR3 && (ddr_params->common.device_type == SDRAM_PCDDR3)) {
    chip_select = BIMC_Rank_Detection_PCDDR3(reg_base, interface, 0 /*pattern_mode*/); // pattern_mode 0: check non-zero PEs/NEs
  } else {
    /* LPDDR2 or LPDDR3 */
    chip_select = ABHN_SHKE_Rank_Detection_LPDDR3(reg_base + REG_OFFSET_SHKE(interface));
  }

  /* zero out rows, cols, banks for non-populated ranks */
  if ((chip_select & SDRAM_CS0) == 0)
  {
    ddr_params->common.num_rows_cs0 = 0;
    ddr_params->common.num_cols_cs0 = 0;
    ddr_params->common.num_banks_cs0 = 0;
  }

  if ((chip_select & SDRAM_CS1) == 0)
  {
    ddr_params->common.num_rows_cs1 = 0;
    ddr_params->common.num_cols_cs1 = 0;
    ddr_params->common.num_banks_cs1 = 0;
  }

  return(chip_select);
}

/* =============================================================================
 **  Function : BIMC_Rank_Detection_PCDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      used by SW to determine if a memory device is present on a specific rank for PCDDR3
 **/
uint32
BIMC_Rank_Detection_PCDDR3 (uint32 reg_base, SDRAM_INTERFACE interface, uint32 pattern_mode)
{
  uint32 populated_ranks = SDRAM_CS_NONE;
  if (ENABLE_PCDDR3) {
    uint32 which_byte = 0;
	uint32 result = 0;

	///---------------------------
	///--------- for rank0 -------
	///---------------------------
	//set expected pattern - have been done in on-time setting
	//BIMC_RCW_Exp_Pattern(reg_base, interface);

	ABHN_SHKE_Rank_Detection_PCDDR3_Pre(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS0);

	//check RD_DQCAL results in DPE
	for(which_byte = 0; which_byte < BIMC_NUM_BYTES; which_byte++){
	  result = ABHN_DPE_RCW_DQcal_read_check(reg_base +  REG_OFFSET_DPE(interface), SDRAM_CS0, which_byte, pattern_mode);
	  if (result == 0) break; //rank is not populated
	}

	// currently, rank_auto_detection not working due to no edge coming back from DQ,
	// force rank0 to be populated. need to syn with PHY setting
	result = 1; 

	//check whether the rank is populated
	if(result == 1)
	  populated_ranks |= SDRAM_CS0;
	else { //disable CKE for non-populated rank
	  ABHN_SHKE_Disable_CKE(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS0);
	  ABHN_SHKE_Disable_CK(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS0);
	  ABHN_SHKE_Disable_INITCOMPLETE(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS0);
	  ABHN_SHKE_Disable_Rank(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS0);
    }
	///---------------------------
	///--------- for rank1 -------
	///--------------------------- 
	//set expected pattern - have been done in on-time setting
	//BIMC_RCW_Exp_Pattern(reg_base, interface);

	ABHN_SHKE_Rank_Detection_PCDDR3_Pre(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS1);

	//check RD_DQCAL results in DPE
	for(which_byte = 0; which_byte < BIMC_NUM_BYTES; which_byte++){
      result = ABHN_DPE_RCW_DQcal_read_check(reg_base + REG_OFFSET_DPE(interface), SDRAM_CS1, which_byte, pattern_mode);
	  if (result == 0) break; //rank is not populated
    }

	// currently, rank_auto_detection not working due to no edge coming back from DQ,
	// force rank0 to be populated. need to syn with PHY setting
	result = 0; 

	//check whether the rank is populated
	if(result == 1)
      populated_ranks |= SDRAM_CS1;
	else { //disable CKE for non-populated rank
      ABHN_SHKE_Disable_CKE(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS1);
      ABHN_SHKE_Disable_CK(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS1);
      ABHN_SHKE_Disable_INITCOMPLETE(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS1);
      ABHN_SHKE_Disable_Rank(reg_base + REG_OFFSET_SHKE(interface), SDRAM_CS1);
    }
	ABHN_SHKE_Rank_Detection_PCDDR3_Post(reg_base + REG_OFFSET_SHKE(interface), SDRAM_BOTH); //disable MPR mode
  }
  return(populated_ranks);
}

/* =============================================================================
 **  Function : BIMC_Ram_Size_Detection
 ** =============================================================================
 */
/**
 *   @brief
 *      probe various address of ddr to find out the exact configuration of row and column size
 **/
boolean
BIMC_Ram_Size_Detection(uint32 reg_base, SDRAM_INTERFACE interface)
{
  union ddr_device_params *ddr_params;

  ddr_params = ddr_get_params(interface);
  
  if (ddr_params->common.device_type == SDRAM_LPDDR3)
  {
    return BIMC_Ram_Size_Detection_LPDDR3(reg_base, interface);
  }
  else
  {
    return FALSE;
  }
}

/* =============================================================================
 **  Function : BIMC_Ram_Size_Detection_LPDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      probe various address of ddr to find out the exact configuration of row and column size for LPDDR3
 **/
boolean
BIMC_Ram_Size_Detection_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface)
{
  union ddr_device_params *ddr_params;
  uint32 mr8;
  uint32 device_type, device_density, device_io_width;
  uint32 nrows, ncols, nbanks, width;
  uint32 chip_select;

  ddr_params = ddr_get_params(interface);

  for (chip_select = SDRAM_CS0; chip_select <= SDRAM_CS1; chip_select++)
  {
    if (ABHN_SHKE_Get_Init_Complete(reg_base + REG_OFFSET_SHKE(interface), chip_select) == chip_select)
    {
      /*  Read manufacture ID from MR5 */
      ddr_params->common.manufacture_name = ABHN_SHKE_Read_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 0x5);

      /*
         Mode Register 8
         BIT[1:0] - Type
         'b00                'b11
         S4 SDRAM (LPDDR2)   S8 SDRAM (LPDDR3)
         All others: Do not detect

         BIT[5:2] - Density
         'b0100  'b0101  'b0110  'b0111  'b1000
         1Gb     2Gb     4Gb     8Gb     16Gb
         All others: Do not detect

         BIT[7:6] - I/O Width
         'b00  'b01
         x32   x16
         All others: Do not detect
         */
      mr8 = ABHN_SHKE_Read_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 0x8);
      device_type = (mr8 & 0x3);
      device_density = (mr8 & 0x3C) >> 2;
      device_io_width = (mr8 & 0xC0) >> 6;

      if (device_type != 0 && device_type != 3)
      {
        /* Detect S4 LPDDR2 and S8 LPDDR3 only */
        return FALSE;
      }
      switch (device_density)
      {
        case 4:
          /* 1Gb */
          nrows = 13; ncols = 9; nbanks = 8;
          break;
        case 5:
          /* 2Gb */
          nrows = 14; ncols = 9; nbanks = 8;
          break;
        case 6:
          /* 4Gb */
          nrows = 14; ncols = 10; nbanks = 8;
          break;
        case 7:
          /* 8Gb */
          nrows = 15; ncols = 10; nbanks = 8;
          break;
        case 8:
          /* 16Gb */
          nrows = 15; ncols = 11; nbanks = 8;
          break;
        case 14:
          /* 3GB support */
          nrows = 15; ncols = 10; nbanks = 8;
          break;    
        default:
          /* only 1Gb - 24Gb are valid densities */
          return FALSE;
      }

      switch (device_io_width) {
        case 0:
          width = 32;
          break;
        case 1:
          width = 16;
          ncols++;
          break;
        default:
          /* only x16 and x32 are valid io widths */
          return FALSE;
      }

      if (chip_select == SDRAM_CS0)
      {
        ddr_params->common.num_rows_cs0 = nrows;
        ddr_params->common.num_cols_cs0 = ncols;
        ddr_params->common.num_banks_cs0 = nbanks;
        ddr_params->common.interface_width_cs0 = width;
      }
      else if (chip_select == SDRAM_CS1)
      {
        ddr_params->common.num_rows_cs1 = nrows;
        ddr_params->common.num_cols_cs1 = ncols;
        ddr_params->common.num_banks_cs1 = nbanks;
        ddr_params->common.interface_width_cs1 = width;
      }
    }
  }

  return TRUE;
}

/* =============================================================================
 **  Function : BIMC_Post_Init_Setup
 ** =============================================================================
 */
/**
 *   @brief
 *      post sequences after the device is initialized; 
 *      updating SCMO/DPE/SHKE base address, column, row, bank, interface_width info
 **/
void
BIMC_Post_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 concurrent_sr)
{
  uint32 mr8, device_density;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  if (chip_select & SDRAM_CS0){
    SCMO_CFG_Post_Init_Setup(reg_base + REG_OFFSET_SCMO(interface), 0, ddr_params->common.num_banks_cs0,ddr_params->common.num_rows_cs0,
        ddr_params->common.num_cols_cs0, ddr_param_interleaved(), HAL_SDRAM_Get_Base_Addr(interface, SDRAM_CS0));
  }
  if(chip_select & SDRAM_CS1){
    SCMO_CFG_Post_Init_Setup(reg_base + REG_OFFSET_SCMO(interface), 1, ddr_params->common.num_banks_cs1,ddr_params->common.num_rows_cs1,
        ddr_params->common.num_cols_cs1, ddr_param_interleaved(), HAL_SDRAM_Get_Base_Addr(interface, SDRAM_CS1));
  }

  ABHN_DPE_Post_Init_Setup(reg_base + REG_OFFSET_DPE(interface), chip_select, ddr_params->common.device_type,
      ddr_params->common.interface_width_cs0, ddr_params->common.num_banks_cs0,
      ddr_params->common.interface_width_cs1, ddr_params->common.num_banks_cs1);

  ABHN_SHKE_Post_Init_Setup(reg_base + REG_OFFSET_SHKE(interface), chip_select, ddr_params->common.device_type,
      ddr_params->common.num_banks_cs0, concurrent_sr);

  BIMC_MISC_GLOBAL_CSR_Freq_Switch( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface);

  //update tRFC, tXSR suporting 2GB/3GB info from MR8    
  if (ddr_params->common.device_type == SDRAM_LPDDR3) {
    mr8 = ABHN_SHKE_Read_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 0x8);
    device_density = (mr8 & 0x3C) >> 2;

    if(device_density == 6) { //supports 4Gb device density
      ABHN_DPE_Program_TREFBW_XO_DIV_6_CYCLES(reg_base + REG_OFFSET_DPE(interface), 15);
      ABHN_DPE_Program_tRFCAB (reg_base + REG_OFFSET_DPE(interface), 1300);
      ABHN_DPE_Program_tRFCPB (reg_base + REG_OFFSET_DPE(interface), 600);
      ABHN_DPE_Program_tXSR (reg_base + REG_OFFSET_DPE(interface), 1400);
    }      
    else {  //supports all other device densities. Device makers might use any of the 4- bit reserved values for other densities. 
      ABHN_DPE_Program_TREFBW_XO_DIV_6_CYCLES(reg_base + REG_OFFSET_DPE(interface), 22);
      ABHN_DPE_Program_tRFCAB (reg_base + REG_OFFSET_DPE(interface), 2100);
      ABHN_DPE_Program_tRFCPB (reg_base + REG_OFFSET_DPE(interface), 900);
      ABHN_DPE_Program_tXSR (reg_base + REG_OFFSET_DPE(interface), 2200);
    }      
  }  
}

/* =============================================================================
 **  Function : BIMC_ODT_Init
 ** =============================================================================
 */
/**
 *   @brief
 *      enable ODT in BIMC
 **/
void
BIMC_ODT_Init(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 ODT_mode) {
  ABHN_DPE_ODT_Init (reg_base + REG_OFFSET_DPE(interface), chip_select, ODT_mode);
}

/* =============================================================================
 **  Function : BIMC_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Mode register write; applies to LPDDR2/3 & PCDDR3 device types 
 *     including the extend mode register write
 **/
void
BIMC_Write_MR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 mr_addr, uint32 mr_data) {
  ABHN_SHKE_Write_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, mr_addr, mr_data);
}

/* =============================================================================
 **  Function : BIMC_Extended_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Extended mode register write
 *       only applies to LPDDR3 device types which require CA calibration (calibration of the DDR command bus) 
 **/
void
BIMC_Extended_Write_MR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 mr_addr, uint32 mr_data) {
  ABHN_SHKE_Extended_Write_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, mr_addr, mr_data);
}

/* =============================================================================
 **  Function : BIMC_Read_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Mode register read; applies to LPDDR2/3 device type 
 *       note: chip_select needs to be rank0 or rank 1
 **/
uint64
BIMC_Read_MR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 mr_addr) {
  return(ABHN_SHKE_Read_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, mr_addr));
}

/* =============================================================================
 **  Function : BIMC_Enter_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs into SW_based self refresh
 *       The memory controller provides SW the ability to enter and exit self refresh. 
 *       This is intended to be used for debug and for a few SW based system sequences
 **/
void
BIMC_Enter_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select) {
  ABHN_SHKE_Enter_Self_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Exit_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs out of SW_based self refresh
 **/
void
BIMC_Exit_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select) {
  ABHN_SHKE_Exit_Self_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Enable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       This mode is entirely HW controlled. 
 *       The controller will enter self refresh if the controller is idle for a programmable number of cycles 
 **/
void
BIMC_Enable_HW_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 concurrent_sr){
  ABHN_SHKE_Enable_HW_Self_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select, concurrent_sr);
}

/* =============================================================================
 **  Function : BIMC_Disable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs out of HW_based self refresh 
 **/
void
BIMC_Disable_HW_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Disable_HW_Self_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Enable_Auto_Refresh 
 ** =============================================================================
 */
/**
 *    @details
 *       Enable periodic auto refresh 
 **/
void
BIMC_Enable_Auto_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Enable_Auto_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Disable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Disable periodic auto refresh 
 **/
void
BIMC_Disable_Auto_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Disable_Auto_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Manual_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *       Enable ZQ Calibration
 *       can only be done when the device is in idle state with all banks are precharged
 **/
void
BIMC_Manual_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 short_seq){
  ABHN_SHKE_Manual_ZQCAL(reg_base + REG_OFFSET_SHKE(interface), short_seq);
}

/* =============================================================================
 **  Function : BIMC_Enable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *     Enable periodic ZQ Calibration
 *     note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *             based on the ranks present 
 **/ 
void
BIMC_Enable_Periodic_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Enable_Periodic_ZQCAL(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Disable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *     Disable periodic ZQ Calibration
 *     note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *            based on the disable ranks  
 **/ 
void
BIMC_Disable_Periodic_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Disable_Periodic_ZQCAL(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Enable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *      DRAM Temperature Status Read (SRR)
 *     Enable periodic SRR; The memory controller is able to change the refresh rate based on the
 *     temperature range where the memory device is operating.
 *     This is only applicable for LPDDR2/3 devices.
 *     note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *        based on the ranks present 
 **/ 
void
BIMC_Enable_Periodic_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Enable_Periodic_SRR(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Disable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *      Disable  periodic SRR 
 *      note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *              based on the disable ranks 
 **/
void
BIMC_Disable_Periodic_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_SHKE_Disable_Periodic_SRR(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Manual_SRR
 ** =============================================================================
 */
/**
 *    @details
 *      SW can trigger a read of the temperature status register
 **/ 
uint64
BIMC_Manual_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  return(ABHN_SHKE_Manual_SRR(reg_base + REG_OFFSET_SHKE(interface), chip_select));
}

/* =============================================================================
 **  Function : BIMC_Enter_DPD
 ** =============================================================================
 */
/**
 *    @details
 *      Deep Power Down (DPD) is a low power state of the SDRAM memory device wherein no data is retained
 *     The only way to get the memory back into normal mode once DPD is entered is via a complete re-initialization sequence
 *     This is only applicable for LPDDR2/3 devices.
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Enter_DPD(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  //SCMO_CFG_Disable_Rank(reg_base + REG_OFFSET_SCMO(interface),chip_select ); //revisit
  //SCMO_CFG_Disable_Rank_With_Flush(reg_base + REG_OFFSET_SCMO(interface),chip_select ); //revisit ;  not in HPG ; not needed
  ABHN_SHKE_Disable_HW_Self_Refresh(reg_base + REG_OFFSET_SHKE(interface), chip_select);
  ABHN_DPE_Status_Check_SR(reg_base + REG_OFFSET_DPE(interface), chip_select );
  ABHN_SHKE_Enter_DPD(reg_base + REG_OFFSET_SHKE(interface), chip_select);
  ABHN_DPE_Status_Check_DPD(reg_base + REG_OFFSET_DPE(interface), chip_select );
  ABHN_SHKE_Disable_CKE(reg_base + REG_OFFSET_SHKE(interface), chip_select );
  ABHN_SHKE_Disable_CK( reg_base + REG_OFFSET_SHKE(interface),  chip_select );
  ABHN_SHKE_Disable_INITCOMPLETE( reg_base + REG_OFFSET_SHKE(interface),  chip_select );
  ABHN_SHKE_Disable_Rank(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Exit_DPD
 ** =============================================================================
 */
/**
 *    @details
 *      Bring the memory out of DPD mode 
 *      Once the DRAM is in Deep Power Down (DPD) mode, all it’s contents will be lost. Upon exiting 
 *       the DPD mode, the DRAM has to go through the entire DRAM initialization sequence to make the 
 *       memory active again.
 *      This is only applicable for LPDDR2/3 devices.
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Exit_DPD(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 clk_freq_in_khz){
  ABHN_DPE_Disable_Power_Down(reg_base + REG_OFFSET_DPE(interface)); //revisit
  SCMO_CFG_Enable_Rank(reg_base + REG_OFFSET_SCMO(interface),chip_select ); //revisit
  ABHN_SHKE_Exit_DPD(reg_base + REG_OFFSET_SHKE(interface), chip_select);
  //BIMC_Device_Init(reg_base, interface, chip_select, clk_freq_in_khz);
}


/* =============================================================================
 **  Function : BIMC_Set_CA_Training_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      Update the bimc CA training pattern
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Set_CA_Training_Pattern(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint8 mapping, uint32 pre_rising_pattern, uint32 pre_falling_pattern, uint32 rising_pattern, uint32 falling_pattern, uint32 post_rising_pattern, uint32 post_falling_pattern)
{

  ABHN_SHKE_Enable_CKE(reg_base + REG_OFFSET_SHKE(interface), chip_select);
  if (mapping == 0){
    ABHN_SHKE_Write_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 41, 0xA4);
  }
  else
    ABHN_SHKE_Write_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 48, 0xC0);

  ABHN_SHKE_Disable_INITCOMPLETE(reg_base + REG_OFFSET_SHKE(interface), chip_select);

  /// Drive CKE low for the rank being calibrated
  HWIO_OUTXF (reg_base + REG_OFFSET_SHKE(interface), ABHN_SHKE_DRAM_MANUAL_0, RANK_SEL, chip_select);
  HWIO_OUTXF (reg_base + REG_OFFSET_SHKE(interface), ABHN_SHKE_DRAM_MANUAL_0, CKE_OFF, HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_EXECUTE_FVAL);

  while (HWIO_INXF (reg_base + REG_OFFSET_SHKE(interface), ABHN_SHKE_DRAM_MANUAL_0, CKE_OFF) == HWIO_ABHN_SHKE_DRAM_MANUAL_0_CKE_OFF_EXECUTE_FVAL );

  /// load the address pattern to be driven onto the CA bus for calibration
  ABHN_DPE_CA_Training_Pattern_PRE(reg_base + REG_OFFSET_DPE(interface),
      1,
      1,
      pre_rising_pattern,
      pre_falling_pattern);
  ABHN_DPE_CA_Training_Pattern(reg_base + REG_OFFSET_DPE(interface),
      1,
      1,
      rising_pattern,
      falling_pattern);
  ABHN_DPE_CA_Training_Pattern_POST(reg_base + REG_OFFSET_DPE(interface),
      1,
      1,
      post_rising_pattern,
      post_falling_pattern);
}
/* =============================================================================
 **  Function : BIMC_CA_Training_Toggle_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      toggle the shke command to send one pulse of ca training pattern 
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_CA_Training_Toggle_Pattern(uint32 reg_base, SDRAM_INTERFACE interface)
{
  ABHN_SHKE_CA_Training_Toggle_Pattern(reg_base + REG_OFFSET_SHKE(interface));
}

/* =============================================================================
 **  Function : BIMC_CA_Training_Toggle_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      restores normal operation to bimc 
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Exit_CA_Training(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){

  //enable CKE
  ABHN_SHKE_Enable_CKE(reg_base + REG_OFFSET_SHKE(interface), chip_select);

  //Exit CA training Mode
  ABHN_SHKE_Write_MR(reg_base + REG_OFFSET_SHKE(interface), chip_select, 42, 0xA8);

  //Re-enable rank init
  ABHN_SHKE_Enable_INITCOMPLETE(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Enable_RCW
 ** =============================================================================
 */
/**
 *    @details
 *      enable RCW
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Enable_RCW(uint32 reg_base, SDRAM_INTERFACE interface){

  ABHN_DPE_Enable_RCW (reg_base + REG_OFFSET_DPE(interface));
}

/* =============================================================================
 **  Function : BIMC_RCW_Exp_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      set the expected pattern for RCW and set the burst_len
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_RCW_Exp_Pattern(uint32 reg_base, SDRAM_INTERFACE interface){

  ABHN_DPE_RCW_Exp_Pattern (reg_base + REG_OFFSET_DPE(interface));
}

/* =============================================================================
 **  Function : BIMC_RCW_Training_Read_Check
 ** =============================================================================
 */
/**
 *    @details
 *       this funcion is used for for rank aouto dection and RCW. After SW triggers a read dq calibration command to certain rank, for upon completion of the
 *       RD_DQCAL command, SW check the status whether we can reach certain number of edge count. 
 *       Also, the loop_cnt controls the passing/faling criteria
 **/
__attribute__((section("ddr_boot_funcs")))
uint32
BIMC_RCW_Training_Read_Check( uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 which_byte, uint32 pattern_mode, uint32 max_loop_count)
{

  uint32 result;
  uint32 loop_cnt;
  uint32 pass_count =0 ;
  uint32 fail_count =0 ;

  for (loop_cnt = 0; loop_cnt < max_loop_count;  loop_cnt ++){ 

    ABHN_SHKE_Read_DQCAL(reg_base + REG_OFFSET_SHKE(interface),chip_select); 

    /// pattern_mode 2: check both 4 PEs/NEs and DQCAL_COMPARE_BYTEx 
    ///                     1: check 4 PEs/NEs only 
    result = ABHN_DPE_RCW_DQcal_read_check(reg_base + REG_OFFSET_DPE(interface), chip_select, which_byte, pattern_mode);  

    if (result==1) {
      pass_count+=1;
    }
    else {
      fail_count+=1;
    }
  } // end of for loop_cnt

  if (pass_count> fail_count) {
    return ((1));
  }
  else {
    return ((0));
  }

}

/* =============================================================================
 **  Function : BIMC_Program_RCW_START_DLY
 ** =============================================================================
 */
/**
 *    @details
 *       program the start delay for RCW per bye 
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_Program_RCW_START_DLY(uint32 reg_base, SDRAM_INTERFACE interface, uint32 which_byte, uint32 start_delay){

  ABHN_DPE_Program_RCW_START_DLY(reg_base + REG_OFFSET_DPE(interface), which_byte, start_delay);
}

/* =============================================================================
 **  Function : BIMC_Enable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *       Enable rank(s) in SCMO registers 
 **/
void
BIMC_Enable_Rank(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  SCMO_CFG_Enable_Rank(reg_base + REG_OFFSET_SCMO(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Disable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *       Disable rank(s) in SCMO registers
 **/
void
BIMC_Disable_Rank(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  SCMO_CFG_Disable_Rank(reg_base + REG_OFFSET_SCMO(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Get_RL_From_REG
 ** =============================================================================
 */
/**
 *    @details
 *      unction of getting the read latendy for Timing 16
 **/
uint32
BIMC_Get_RL_From_REG(uint32 reg_base, uint32 interface){
  return (ABHN_DPE_Get_RL_From_REG(reg_base + REG_OFFSET_DPE(interface)));
}

/* =============================================================================
 **  Function : BIMC_Get_RL
 ** =============================================================================
 */
/**
 *    @details
 *      get read latency  by referencing BIMC LUT in bimc_config.c 
 **/
uint32
BIMC_Get_RL(uint32 reg_base,SDRAM_INTERFACE interface, uint32 clk_freq_in_khz){
  return( Get_BIMC_LUT (interface, clk_freq_in_khz, 1) ); 
}

/* =============================================================================
 **  Function : BIMC_Get_WL
 ** =============================================================================
 */
/**
 *    @details
 *      get write latency  by referencing BIMC LUT in bimc_config.c 
 **/

uint32
BIMC_Get_WL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz){
  return( Get_BIMC_LUT (interface, clk_freq_in_khz, 0) ); 
}

/* =============================================================================
 **  Function : BIMC_BIMC_MISC_GLOBAL_CSR_Update_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *      update DDR clock period and corresponding resolution; clock period is used for converting timing settings from ns to cycles 
 **/
void
BIMC_BIMC_MISC_GLOBAL_CSR_Update_Clock_Period(uint32 reg_base, uint32 interface, uint32 resolution, uint32 clk_period ){
  BIMC_MISC_GLOBAL_CSR_Update_Clock_Period( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface, resolution, clk_period );
}


/* =============================================================================
 **  Function : BIMC_BIMC_MISC_GLOBAL_CSR_Get_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *      get the current clock priod 
 **/
uint32
BIMC_BIMC_MISC_GLOBAL_CSR_Get_Clock_Period(uint32 reg_base, uint32 interface ){
  return(BIMC_MISC_GLOBAL_CSR_Get_Clock_Period( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface));
}

/* =============================================================================
 **  Function : BIMC_Update_Latency
 ** =============================================================================
 */
/**
 *    @details
 *     update the WL RL MR setting for pre_clock_switch
 **/
void
BIMC_Update_Latency(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz){
  uint32 MR0_value, MR1_value, MR2_value;
  uint32 RL_value, WL_value;

  union ddr_device_params *ddr_params;
  ddr_params = (ddr_get_params(interface));

  if( ddr_params->common.device_type ==  SDRAM_LPDDR3) {
    WL_value   =  Get_BIMC_LUT (interface, clk_freq_in_khz, 0); 
    RL_value   =   Get_BIMC_LUT (interface, clk_freq_in_khz, 1); 
    MR1_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 2); 
    MR2_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 3); 
    ABHN_DPE_Update_Latency_LPDDR3(reg_base + REG_OFFSET_DPE(interface), WL_value, RL_value, MR1_value, MR2_value);
  }
  else if   ( ddr_params->common.device_type ==  SDRAM_PCDDR3) {
    WL_value   =  Get_BIMC_LUT (interface, clk_freq_in_khz, 0); 
    RL_value   =   Get_BIMC_LUT (interface, clk_freq_in_khz, 1); 
    MR0_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 2);
    MR1_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 3); 
    MR2_value =  Get_BIMC_LUT (interface, clk_freq_in_khz, 5); 
    ABHN_DPE_Update_Latency_PCDDR3( reg_base + REG_OFFSET_DPE(interface), WL_value, RL_value, MR0_value, MR1_value, MR2_value);
  }
}

/* =============================================================================
 **  Function : BIMC_Update_ODTOFF_MIN
 ** =============================================================================
 */
/**
 *    @details
 *     update the ODTOFF_MIN setting in TIMING_23 for LPDDR3  during clock_switch
 **/
void
BIMC_Update_ODTOFF_MIN(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz){
  union ddr_device_params *ddr_params;
  ddr_params = (ddr_get_params(interface));

  if( ddr_params->common.device_type ==  SDRAM_LPDDR3) {
    ABHN_DPE_Program_ODT_OFF(reg_base + REG_OFFSET_DPE(interface), clk_freq_in_khz, ddr_params->lpddr.tODToffmax, ddr_params->lpddr.tODToffmin);
  }
}

/* =============================================================================
 **  Function : BIMC_Clock_Switch_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *     update resolution/period, clk_mode etc in global0 registers and WL RL MR setting in dpe registers
 **/
void
BIMC_Clock_Switch_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 clk_period_in_ps){
  if (ENABLE_LPDDR3) {
    BIMC_MISC_GLOBAL_CSR_Update_Clock_Period (reg_base + SEQ_BIMC_GLOBAL0_OFFSET,
      interface, CLK_PERIOD_RESOLUTION_1PS, clk_period_in_ps);

    BIMC_MISC_GLOBAL_CSR_Freq_Switch( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface);
    BIMC_MISC_GLOBAL_CSR_Mode_Switch(reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface, Get_BIMC_LUT (interface, clk_freq_in_khz, 5));
    BIMC_Update_Latency(reg_base, interface, clk_freq_in_khz);
    BIMC_Update_ODTOFF_MIN(reg_base, interface, clk_period_in_ps);
  }
}

/* =============================================================================
 **  Function : BIMC_Clock_Switch_PCDDR3
 ** =============================================================================
 */
/**
 *    @details
 *     update resolution/period, clk_mode etc in global0 registers and WL RL MR setting in dpe registers
 **/
void
BIMC_Clock_Switch_PCDDR3(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 clk_period_in_ps){
  if (ENABLE_PCDDR3){
    BIMC_MISC_GLOBAL_CSR_Update_Clock_Period (reg_base + SEQ_BIMC_GLOBAL0_OFFSET,
      interface, CLK_PERIOD_RESOLUTION_1PS, clk_period_in_ps);

	BIMC_MISC_GLOBAL_CSR_Freq_Switch( reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface);
	BIMC_MISC_GLOBAL_CSR_Mode_Switch(reg_base + SEQ_BIMC_GLOBAL0_OFFSET, interface, Get_BIMC_LUT (interface, clk_freq_in_khz, 6));
	BIMC_Update_Latency(reg_base, interface, clk_freq_in_khz);
	// store
	BIMC_Restore_RCW( reg_base,  interface, clk_freq_in_khz);
  }
}

/* =============================================================================
 **  Function : BIMC_Disable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *     disable all periodic events . (1) MRR (2) ZQCAL (3) auto refresh (4) self-refresh. need to be done before PHY training; 
 **/
void
BIMC_Disable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){

  //for ca training, for other training it does not matter
  HWIO_OUTXF (reg_base + REG_OFFSET_DPE(interface), ABHN_DPE_CONFIG_0, CA_BUS_OE_CTRL, 0);

  ABHN_DPE_Disable_Power_Down(reg_base + REG_OFFSET_DPE(interface));
  ABHN_SHKE_Disable_All_Periodic(reg_base + REG_OFFSET_SHKE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Enable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *     enable all periodic events . (1) MRR (2) ZQCAL (3) auto refresh.  need to be done after PHY training; 
 **/
void
BIMC_Enable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 concurrent_sr){
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  HWIO_OUTXF (reg_base + REG_OFFSET_DPE(interface), ABHN_DPE_CONFIG_0, CA_BUS_OE_CTRL, 1);
  ABHN_DPE_Enable_Power_Down(reg_base + REG_OFFSET_DPE(interface), ddr_params->common.device_type);
  ABHN_SHKE_Enable_All_Periodic(reg_base + REG_OFFSET_SHKE(interface), chip_select,  ddr_params->common.device_type, concurrent_sr);

}

/* =============================================================================
 **  Function : BIMC_Enable_CLK_To_PHY 
 ** =============================================================================
 */
/**
 *    @details
 *     enable clk to PHY
 *     should only be required during legacy mode of operation for rd/wr trining, CA training, and write leveling training
 **/
void BIMC_Enable_CLK_To_PHY(uint32 reg_base, SDRAM_INTERFACE interface){
  ABHN_DPE_Enable_Clk_To_PHY(reg_base + REG_OFFSET_DPE(interface));
}

/* =============================================================================
 **  Function : BIMC_Disable_CLK_To_PHY
 ** =============================================================================
 */
/**
 *    @details
 *     disable clk to PHY
 **/
void BIMC_Disable_CLK_To_PHY(uint32 reg_base, SDRAM_INTERFACE interface){
  ABHN_DPE_Disable_Clk_To_PHY(reg_base + REG_OFFSET_DPE(interface));
}

/* =============================================================================
 **  Function : BIMC_Save_RCW
 ** =============================================================================
 */
/**
 *    @details
 *     save rcw training results locally 
 **/
__attribute__((section("ddr_boot_funcs"))) 
void BIMC_Save_RCW (uint32 which_freq_plan, SDRAM_INTERFACE interface, uint32 timing_18_value, uint32 timing_19_value, uint32 clk_freq_in_khz)
{
  start_dly_result[which_freq_plan][interface].byte4_7 = timing_18_value;
  start_dly_result[which_freq_plan][interface].byte0_3 = timing_19_value;
  training_clk_freq_in_khz[which_freq_plan] = clk_freq_in_khz;
}


/* =============================================================================
 **  Function : BIMC_Restore_RCW
 ** =============================================================================
 */
/**
 *    @details
 *    restore rcw training results locally
 **/
__attribute__((section("ddr_boot_funcs")))
 void BIMC_Restore_RCW( uint32 reg_base,  SDRAM_INTERFACE interface, uint32 clk_freq_in_khz) {

  //figure out the which_ren_cfg first
  uint32 which_ren_cfg = 0;
  uint32 i;
  for (i = 0; i < 7;  i ++){
    if (training_clk_freq_in_khz[i]  == clk_freq_in_khz)
      which_ren_cfg = i;
  }

  // populate the start delay register based on the freq plan
  ABHN_DPE_Program_RCW_START_DLY_All(reg_base + REG_OFFSET_DPE(interface), start_dly_result[which_ren_cfg][interface].byte4_7, start_dly_result[which_ren_cfg][interface].byte0_3);
}

/* =============================================================================
 **  Function : BIMC_RCW_Get_Trained_Results
 ** =============================================================================
 */
/**
 *    @details
 *     for SW to save rcw training results
 **/
__attribute__((section("ddr_boot_funcs")))
void
BIMC_RCW_Get_Trained_Results(uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size)
{
  uint32 freq_idx,channel_sel;

  //loop here and store results  here
  for (freq_idx = 0; freq_idx < size;  freq_idx ++){
    for (channel_sel = 0; channel_sel < 2; channel_sel++) {
      start_dly_byte0_3[channel_sel][freq_idx] = start_dly_result[freq_idx][channel_sel].byte0_3;
      start_dly_byte4_7[channel_sel][freq_idx] = start_dly_result[freq_idx][channel_sel].byte4_7;
      freq[freq_idx] = training_clk_freq_in_khz[freq_idx];
    }  
  }
} 
/* =============================================================================
 **  Function : BIMC_RCW_Set_Trained_Results
 ** =============================================================================
 */
/**
 *    @details
 *     for SW to restore rcw training results
 **/
__attribute__((section("ddr_boot_funcs"))) 
void
BIMC_RCW_Set_Trained_Results(uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size )
{
  uint32 freq_idx,channel_sel;
  for (freq_idx = 0; freq_idx < size;  freq_idx ++){
    for (channel_sel = 0; channel_sel < 2; channel_sel++) {
      start_dly_result[freq_idx][channel_sel].byte0_3 = start_dly_byte0_3[channel_sel][freq_idx];
      start_dly_result[freq_idx][channel_sel].byte4_7 = start_dly_byte4_7[channel_sel][freq_idx];
      training_clk_freq_in_khz[freq_idx] = freq[freq_idx];
    }
  }
}

/* =============================================================================
 **  Function : BIMC_DPE_Load_All_Config 
 ** =============================================================================
 */
/**
 *    @details
 *     load all config registers
 **/

void BIMC_DPE_Load_All_Config ( uint32 reg_base,  SDRAM_INTERFACE interface) {
  ABHN_DPE_Load_All_Config(reg_base + REG_OFFSET_DPE(interface));
}

/* =============================================================================
 **  Function : BIMC_Get_Device_Type
 ** =============================================================================
 */
/**
 *    @details
 *     obtain ddr device_type
 **/

uint32 BIMC_Get_Device_Type(SDRAM_INTERFACE interface)
{
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  return (ddr_params->common.device_type);
}

/* =============================================================================
 **  Function : BIMC_Enable_ODT
 ** =============================================================================
 */
/**
 *    @details
 *    enable ODT, i.e. force ODT pin high 
 **/
void BIMC_Enable_ODT(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_DPE_Enable_ODT(reg_base + REG_OFFSET_DPE(interface), chip_select);
}

/* =============================================================================
 **  Function : BIMC_Disable_ODT
 ** =============================================================================
 */
/**
 *    @details
 *    disable ODT, i.e. force ODT pin low
 **/
void BIMC_Disable_ODT(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select){
  ABHN_DPE_Disable_ODT(reg_base + REG_OFFSET_DPE(interface),chip_select );
}

/* =============================================================================
 **  Function : BIMC_Get_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    gets the status of auto refresh for the ranks
 **/
uint32 BIMC_Get_Auto_Refresh_Status( uint32 reg_base,  SDRAM_INTERFACE interface) {
  return ( ABHN_SHKE_Get_Auto_Refresh_Status(reg_base + REG_OFFSET_SHKE(interface)));
}

/* =============================================================================
 **  Function : BIMC_Set_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    Sets the original status of auto refresh for the ranks
 **/
void BIMC_Set_Auto_Refresh_Status ( uint32 reg_base,  SDRAM_INTERFACE interface, uint32 chip_select) {
  ABHN_SHKE_Set_Auto_Refresh_Status (reg_base + REG_OFFSET_SHKE(interface),chip_select);
} 

/* =============================================================================
 **  Function : BIMC_Get_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    gets the HW self refresh register value for the ranks
 **/
uint32 BIMC_Get_HW_Self_Refresh_Reg( uint32 reg_base,  SDRAM_INTERFACE interface) {
  return ( ABHN_SHKE_Get_HW_Self_Refresh_Reg(reg_base + REG_OFFSET_SHKE(interface)));
}

/* =============================================================================
 **  Function : BIMC_Set_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    restores the HW self refresh register value for the ranks.
 **/
void BIMC_Set_HW_Self_Refresh_Reg( uint32 reg_base, SDRAM_INTERFACE interface, uint32 reg_value) {
  ABHN_SHKE_Set_HW_Self_Refresh_Reg(reg_base + REG_OFFSET_SHKE(interface), reg_value);
}







