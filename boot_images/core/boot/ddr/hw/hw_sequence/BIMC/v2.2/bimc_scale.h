#ifndef _BIMC_SEQ_HEADER_
#define _BIMC_SEQ_HEADER_
/**
 * @file bimc_scale.h
 * @brief This is header file for bimc.c implementation.
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------
05/09/14   nevedeth   Included API to get/set HW_Self_Refresh enable register value.
03/16/14   nevedeth   Included API for LP3 ODTOFF_MIN during pre-clock switch
02/06/14   nevedeth   Update APIs to get/set Auto-Refresh status
02/03/14   cchou      Update APIs to Enable/Disable ODT pin
01/21/14   cchou      Update API for restore/store RCW results from/to SW
01/08/14   cchou      Code optimization
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
#include "bimc_struct.h"
#include "bimc_global.h"
#include "bimc_mc_scmo.h"
#include "bimc_mc_dpe.h"
#include "bimc_mc_shke.h"
#include "bimc_config.h"

/*==============================================================================
                                           VARIABLE DEFINITIONS                     
==============================================================================*/
typedef struct    
{
  uint32 byte0_3;
  uint32 byte4_7;        
} rcw_training;

extern uint32 training_clk_freq_in_khz[DDR_FREQ_MAX_NUM_LEVELS];   

extern rcw_training start_dly_result[7][2];  

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
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *   @param[in] clk_period_in_ps clock period(in ps) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Pre_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz, uint32 clk_period_in_ps);
    
/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Update_AC_Parameters(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz);
    
/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_LPDDR2
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for LPDDR2
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Update_AC_Parameters_LPDDR2(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz);
    
/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_LPDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for LPDDR3
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Update_AC_Parameters_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz);
    
/* =============================================================================
 **  Function : BIMC_Update_AC_Parameters_PCDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      AC Timing Parameters is update by SW CDT for PCDDR3
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Update_AC_Parameters_PCDDR3(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 clk_freq_in_khz);
    
/* =============================================================================
 **  Function : BIMC_Device_Init
 ** =============================================================================
 */
/**
 *   @brief
 *      execute full device initialization sequence
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select chip select to initilize
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
 void
BIMC_Device_Init(uint32 reg_base, SDRAM_INTERFACE interface,
    uint32 chip_select, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Rank_Detection
 ** =============================================================================
 */
/**
 *   @brief
 *      used by SW to determine if a memory device is present on a specific rank
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[out] whethere the rank is populated
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */

uint32
BIMC_Rank_Detection(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Rank_Detection_PCDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      used by SW to determine if a memory device is present on a specific rank for PCDDR3
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for (should be 01 or 10)
 *   @param[in]   pattern_mode       2: check both 4 PEs/NEs and DQCAL_COMPARE_BYTEx 
 *                                                1: check 4 PEs/NEs only 
 *                                                0: check non-zero PEs/NEs (for rank auto detection)
 *   @param[out] whethere the rank is populated
 *
 *   @dependencies None
 *
 *   @sideeffects  this API is not working due to no edge coming back from DQ.
 *      hard-code result returned from ABHN_DPE_RCW_DQcal_read_check to 1/0 for populated/non-populated rank, respectively.
 */
uint32
BIMC_Rank_Detection_PCDDR3 (uint32 reg_base, SDRAM_INTERFACE interface, uint32 pattern_mode);

/* =============================================================================
 **  Function : BIMC_Ram_Size_Detection
 ** =============================================================================
 */
/**
 *   @brief
 *      probe various address of ddr to find out the exact configuration of row and column size
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[out] return false if cannot find device dentisy within 1Gb-24Gb
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
boolean
BIMC_Ram_Size_Detection(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Ram_Size_Detection_LPDDR3
 ** =============================================================================
 */
/**
 *   @brief
 *      probe various address of ddr to find out the exact configuration of row and column size for LPDDR3
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[out] return false if cannot find device dentisy within 1Gb-24Gb
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
boolean
BIMC_Ram_Size_Detection_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Post_Init_Setup
 ** =============================================================================
 */
/**
 *   @brief
 *      post sequences after the device is initialized; 
 *      updating SCMO/DPE/SHKE base address, column, row, bank, interface_width info
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in]  concurrent_sr set if self_refresh in concurrent mode 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_Post_Init_Setup (uint32 reg_base, SDRAM_INTERFACE interface, 
    uint32 chip_select, uint32 concurrent_sr);

/* =============================================================================
 **  Function : BIMC_ODT_Init
 ** =============================================================================
 */
/**
 *   @brief
 *      enable ODT in BIMC
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in]  chip_select chip select to initilize
 *   @param[in]  ODT_mode specify read/write ODT between rank
 *                     ODT_mode [5:0]=  {SCALE_RANK1_ODT_ON_WR_RANK1,
 *                                                SCALE_RANK1_ODT_ON_WR_RANK0,
 *                                                SCALE_RANK1_ODT_ON_RD_RANK0,
 *                                                SCALE_RANK0_ODT_ON_WR_RANK1,
 *                                                SCALE_RANK0_ODT_ON_WR_RANK0,
 *                                                SCALE_RANK0_ODT_ON_RD_RANK1 }
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 */
void
BIMC_ODT_Init (uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 ODT_mode);

/* =============================================================================
 **  Function : BIMC_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Mode register write; applies to LPDDR2/3 & PCDDR3 device types 
 *     including the extend mode register write
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to initialize for
 *   @param[in] mr_addr      mode register address to be written to the SDRAM device
 *   @param[in] mr_data      write data for the SDRAM device
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Write_MR(uint32 reg_base, SDRAM_INTERFACE interface, 
    uint32 chip_select, uint32 mr_addr, uint32 mr_data);
    
/* =============================================================================
 **  Function : BIMC_Extended_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Extended mode register write
 *       only applies to LPDDR3 device types which require CA calibration (calibration of the DDR command bus) 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to initialize for
 *   @param[in] mr_addr      mode register address to be written to the SDRAM device
 *   @param[in] mr_data      write data for the SDRAM device
  *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Extended_Write_MR(uint32 reg_base, SDRAM_INTERFACE interface, 
    uint32 chip_select, uint32 mr_addr, uint32 mr_data);       
           
/* =============================================================================
 **  Function : BIMC_Read_MR
 ** =============================================================================
 */
/**
 *    @details
 *       Mode register read; applies to LPDDR2/3 device type 
 *       note: chip_select needs to be rank0 or rank 1
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to initialize for
 *   @param[in] mr_addr      mode register address to be written to the SDRAM device
 *   @param[out] The read data for the MRR (64 bit for Gandalf)
 **/
uint64
BIMC_Read_MR(uint32 reg_base, SDRAM_INTERFACE interface, 
    uint32 chip_select, uint32 mr_addr);    
               
/* =============================================================================
 **  Function : BIMC_Enter_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs into SW_based self refresh
 *       The memory controller provides SW the ability to enter and exit self refresh. 
 *       This is intended to be used for debug and for a few SW based system sequences
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Enter_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Exit_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs out of SW_based self refresh
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Exit_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Enable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       This mode is entirely HW controlled. 
 *       The controller will enter self refresh if the controller is idle for a programmable number of cycles 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *   @param[in] concurrent_sr set to 1 if in self refresh concueent mode
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enable_HW_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 concurrent_sr);

/* =============================================================================
 **  Function : BIMC_Disable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Put DRAMs out of HW_based self refresh 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Disable_HW_Self_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Enable_Auto_Refresh 
 ** =============================================================================
 */
/**
 *    @details
 *       Enable periodic auto refresh 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
  *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enable_Auto_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Disable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *       Disable periodic auto refresh 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Disable_Auto_Refresh(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

 /* =============================================================================
 **  Function : BIMC_Manual_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *       Enable ZQ Calibration
 *       can only be done when the device is in idle state with all banks are precharged
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] short_seq short/long sequence
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Manual_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 short_seq);

/* =============================================================================
 **  Function : BIMC_Enable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *     Enable periodic ZQ Calibration
 *     note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *             based on the ranks present 
 * 
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Enable_Periodic_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Disable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *     Disable periodic ZQ Calibration
 *     note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *            based on the disable ranks  
 * 
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to disable 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Disable_Periodic_ZQCAL(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

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
 * 
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enable_Periodic_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Disable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *      Disable  periodic SRR 
 *      note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *              based on the disable ranks 
 * 
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to disable  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Disable_Periodic_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Manual_SRR
 ** =============================================================================
 */
/**
 *    @details
 *      SW can trigger a read of the temperature status register
 * 
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *   @param[out] the data for the SRR  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint64
BIMC_Manual_SRR(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Enter_DPD
 ** =============================================================================
 */
/**
 *    @details
 *      Deep Power Down (DPD) is a low power state of the SDRAM memory device wherein no data is retained
 *     The only way to get the memory back into normal mode once DPD is entered is via a complete re-initialization sequence
 *     This is only applicable for LPDDR2/3 devices.
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enter_DPD(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

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
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Exit_DPD(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Set_CA_Training_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      Update the bimc CA training pattern
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to initialize for
 *   @param[in] chip_select  chip select to set
 *   @param[in  mapping     mapping pattern
 *   @param[in] pre_rising_pattern    set the pre rising pattern
 *   @param[in] pre_falling_pattern   set the pre falling pattern
 *   @param[in] rising_pattern    set the rising pattern
 *   @param[in] falling_pattern   set the falling pattern
 *   @param[in] prost_rising_pattern    set the post rising pattern
 *   @param[in] post_falling_pattern    set the post falling pattern
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Set_CA_Training_Pattern(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint8 mapping, uint32
    pre_rising_pattern, uint32 pre_falling_pattern, uint32 rising_pattern, uint32 falling_pattern, uint32 post_rising_pattern,
    uint32 post_falling_pattern);
    
/* =============================================================================
 **  Function : BIMC_CA_Training_Toggle_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      toggle the shke command to send one pulse of ca training pattern 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to toggle training value for
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_CA_Training_Toggle_Pattern(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_CA_Training_Toggle_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      restores normal operation to bimc 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set training value for 
 *   @param[in]  chip_select  chip select to set training value for
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Exit_CA_Training(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Enable_RCW
 ** =============================================================================
 */
/**
 *    @details
 *      enable RCW
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enable_RCW(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_RCW_Exp_Pattern
 ** =============================================================================
 */
/**
 *    @details
 *      set the expected pattern for RCW and set the burst_len
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_RCW_Exp_Pattern(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_RCW_Training_Read_Check
 ** =============================================================================
 */
/**
 *    @details
 *       this funcion is used for for rank aouto dection and RCW. After SW triggers a read dq calibration command to certain rank, for upon completion of the
 *       RD_DQCAL command, SW check the status whether we can reach certain number of edge count. 
 *       Also, the loop_cnt controls the passing/faling criteria
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select chip select to initialize for 
 *   @param[in]   which_byte        byte i for check; i = 0~7
 *   @param[in]   pattern_mode       2: check both 4 PEs/NEs and DQCAL_COMPARE_BYTEx 
 *                                                1: check 4 PEs/NEs only 
 *                                                0: check non-zero PEs/NEs (for rank auto detection)
 *   @param[in]   max_loop_count  loop count for controlling the passing/faling criteria 
 *   @param[out]   true if meeting the edge count with certain confidence
  *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
BIMC_RCW_Training_Read_Check( uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 which_byte, uint32 pattern_mode, uint32 max_loop_count);

/* =============================================================================
 **  Function : BIMC_Program_RCW_START_DLY
 ** =============================================================================
 */
/**
 *    @details
 *       program the start delay for RCW per bye 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in]   which_byte      byte i for check; i = 0~7
 *   @param[in]   start_delay      start delay value
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Program_RCW_START_DLY(uint32 reg_base, SDRAM_INTERFACE interface, uint32 which_byte, uint32 start_delay);

/* =============================================================================
 **  Function : BIMC_Enable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *       Enable rank(s) in SCMO registers 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select  chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None 
 **/
void
BIMC_Enable_Rank(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Disable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *       Disable rank(s) in SCMO registers
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select  chip select to disable 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Disable_Rank(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Get_RL_From_REG
 ** =============================================================================
 */
/**
 *    @details
 *      unction of getting the read latendy for Timing 16
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[out]   read latency  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
BIMC_Get_RL_From_REG(uint32 reg_base, uint32 interface);

/* =============================================================================
 **  Function : BIMC_Get_RL
 ** =============================================================================
 */
/**
 *    @details
 *      get read latency  by referencing BIMC LUT in bimc_config.c 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *   @param[out]   read latency  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
BIMC_Get_RL(uint32 reg_base,  SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Get_WL
 ** =============================================================================
 */
/**
 *    @details
 *      get write latency  by referencing BIMC LUT in bimc_config.c 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *   @param[out]   write latency  
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
BIMC_Get_WL(uint32 reg_base,  SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_BIMC_MISC_GLOBAL_CSR_Update_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *      update DDR clock period and corresponding resolution; clock period is used for converting timing settings from ns to cycles 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] resolution   clock resolution; 1 for 1ps resolution where 0 for 100ps resolution 
 *   @param[in] clk_period   clock period based the corresponding resolution
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_BIMC_MISC_GLOBAL_CSR_Update_Clock_Period(uint32 reg_base, uint32 interface, uint32 resolution, uint32 clk_period );

/* =============================================================================
 **  Function : BIMC_BIMC_MISC_GLOBAL_CSR_Get_Clock_Period
 ** =============================================================================
 */
/**
 *    @details
 *      get the current clock priod 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 
BIMC_BIMC_MISC_GLOBAL_CSR_Get_Clock_Period(uint32 reg_base, uint32 interface);

/* =============================================================================
 **  Function : BIMC_Update_Latency
 ** =============================================================================
 */
/**
 *    @details
 *     update the WL RL MR setting for pre_clock_switch
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Update_Latency(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Update_ODTOFF_MIN
 ** =============================================================================
 */
/**
 *    @details
 *     update the ODTOFF_MIN setting in TIMING_23 for LPDDR3  during clock_switch
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Update_ODTOFF_MIN(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Clock_Switch_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *     update resolution/period, clk_mode etc in global0 registers and WL RL MR setting in dpe registers
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update
 *   @param[in] clk_period   clock period based the corresponding resolution
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Clock_Switch_LPDDR3(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 clk_period_in_ps);

/* =============================================================================
 **  Function : BIMC_Clock_Switch_PCDDR3
 ** =============================================================================
 */
/**
 *    @details
 *     update resolution/period, clk_mode etc in global0 registers and WL RL MR setting in dpe registers
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update
 *   @param[in] clk_period   clock period based the corresponding resolution
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Clock_Switch_PCDDR3(uint32 reg_base, SDRAM_INTERFACE interface, uint32 clk_freq_in_khz, uint32 clk_period_in_ps);

/* =============================================================================
 **  Function : BIMC_Disable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *     disable all periodic events . (1) MRR (2) ZQCAL (3) auto refresh (4) self-refresh. need to be done before PHY training; 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select chip select to disable
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/

void
BIMC_Disable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Enable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *     enable all periodic events . (1) MRR (2) ZQCAL (3) auto refresh.  need to be done after PHY training; 
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select chip select to enable
 *   @param[in] concurrent_sr set to 1 if in concurrent self-refresh
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_Enable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint32 concurrent_sr);

/* =============================================================================
 **  Function : BIMC_Enable_CLK_To_PHY 
 ** =============================================================================
 */
/**
 *    @details
 *     enable clk to PHY
 *     should only be required during legacy mode of operation for rd/wr trining, CA training, and write leveling training
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Enable_CLK_To_PHY(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Disable_CLK_To_PHY
 ** =============================================================================
 */
/**
 *    @details
 *     disable clk to PHY
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Disable_CLK_To_PHY(uint32 reg_base, SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Save_RCW
 ** =============================================================================
 */
/**
 *    @details
 *     save rcw training results locally 
 *
 *   @param[in]  which_freq_plan   which frequency
 *   @param[in]  interface    Interface to set 
 *   @param[in]  timing_18_value  value of dpe timing_18
 *   @param[in]  timing_19_value  value of dpe timing_19
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_Save_RCW (uint32 which_freq_plan, SDRAM_INTERFACE interface, uint32 timing_18_value, uint32 timing_19_value, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_Restore_RCW
 ** =============================================================================
 */
/**
 *    @details
 *    restore rcw training results locally
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] clk_freq_in_khz  clock frequency (in khz) to update
 **/
void 
BIMC_Restore_RCW( uint32 reg_base,  SDRAM_INTERFACE interface, uint32 clk_freq_in_khz);

/* =============================================================================
 **  Function : BIMC_RCW_Get_Trained_Results
 ** =============================================================================
 */
/**
 *    @details
 *     for SW to save rcw training results
 *
 *   @param[in]  interface    Interface to set 
 *   @param[in]  *rcw_results rcw training data structure
 *   @param[in]  *freq          match the freq plan
 *   @param[in]   size          size of the frequency plan
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_RCW_Get_Trained_Results(uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32
start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size);
/* =============================================================================
 **  Function : BIMC_RCW_Set_Trained_Results
 ** =============================================================================
 */
/**
 *    @details
 *     for SW to restore rcw training results
 *
 *   @param[in]  interface    Interface to set 
 *   @param[in]  *rcw_results rcw training data structure
 *   @param[in]  *freq          match the freq plan
 *   @param[in]   size          size of the frequency plan
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void
BIMC_RCW_Set_Trained_Results(uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS], uint32
start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS], uint32 *freq, uint32 size );
/* =============================================================================
 **  Function : BIMC_DPE_Load_All_Config 
 ** =============================================================================
 */
/**
 *    @details
 *     load all config registers
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void 
BIMC_DPE_Load_All_Config ( uint32 reg_base,  SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Get_Device_Type
 ** =============================================================================
 */
/**
 *    @details
 *     obtain ddr device_type
 *
 *   @param[in]    interface    Interface to set 
 *   @param[out]  device_type
 *
 *   @dependencies None
 *
 *   @sideeffects  None 
 **/
uint32 
BIMC_Get_Device_Type(SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Enable_ODT
 ** =============================================================================
 */
/**
 *    @details
 *    enable ODT, i.e. force ODT pin high  
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_Enable_ODT(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Disable_ODT
 ** =============================================================================
 */
/**
 *    @details
 *    disable ODT, i.e. force ODT pin low
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set 
 *   @param[in] chip_select chip select to set
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_Disable_ODT(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Get_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *   @details
 *   gets the status of auto refresh enable for the ranks
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set   
 *   @param[out] auto_refresh_status 	bit 0 and 1 for Rank0/1 status respectively    
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 BIMC_Get_Auto_Refresh_Status ( uint32 reg_base,  SDRAM_INTERFACE interface);

/* =============================================================================
 **  Function : BIMC_Set_Auto_Refresh_status
 ** =============================================================================
 */
/**
 *   @details
 *   Sets the original status of auto refresh for the ranks
 *
 *   @param[in]  reg_base   BIMC reg_base
 *   @param[in]  interface    Interface to set
 *   @param[in] chip_select  represents which ranks had auto refresh enabled previously 	
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_Set_Auto_Refresh_Status ( uint32 reg_base,  SDRAM_INTERFACE interface, uint32 chip_select);

/* =============================================================================
 **  Function : BIMC_Get_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *   @details
 *   gets the HW self refresh register value for the ranks
 *
 *   @param[out] returns the SELF_REFRESH_CNTL register value from SHKE.
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
uint32 BIMC_Get_HW_Self_Refresh_Reg( uint32 reg_base,  SDRAM_INTERFACE interface); 

/* =============================================================================
 **  Function : BIMC_Set_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *   @details
 *   restores the HW self refresh register value for the ranks.
 *
 *   @param[in] reg_value has the register value to be restored
 *
 *   @dependencies None
 *
 *   @sideeffects  None
 **/
void BIMC_Set_HW_Self_Refresh_Reg( uint32 reg_base, SDRAM_INTERFACE interface, uint32 reg_value);

#endif










