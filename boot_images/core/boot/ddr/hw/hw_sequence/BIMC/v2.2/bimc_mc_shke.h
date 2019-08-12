#ifndef _BIMC_MC_SHKE_SEQ_HEADER_
#define _BIMC_MC_SHKE_SEQ_HEADER_
/**
 * @file bimc_mc_shke.h
 * @brief  This is header file for bimc_mc_shke.c implementation.
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
05/09/14   nevedeth   added API to get/set HW_Self_Refresh_Enable register value
02/06/14   nevedeth   added two API for Get/Set Auto refresh status
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/***************************************************************************************************
  Auto-generated sequence C codes from Autoseq 2.2 12/6/2013
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
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#include "bimc_struct.h"

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
 * 
 *    @param[in] tREFI   timing value: tREFI
 **/
void ABHN_SHKE_Program_tREFI( uint32 _inst_, uint32 tREFI );

/** =============================================================================
 *  Function : Post_Init_Setup
 ** =============================================================================
 */
/**
 *    @details
 *    program number of rank and enable auto-refresh, ZQCAL, and SRR
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] device_type  device type to initialize for
 *    @param[in] num_banks_cs0 number of banks for rank 0
 *    @param[in] concurrent_sr set to 1 if in self refresh concueent mode
 **/
void ABHN_SHKE_Post_Init_Setup( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 num_banks_cs0, uint32 concurrent_sr );

/** =============================================================================
 *  Function : Device_Init
 ** =============================================================================
 */
/**
 *    @details
 *    device initilization for LP/PC
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] device_type  device type to initialize for
 *    @param[in] clk_freq_in_khz clock frequency (in khz) to update
 *    @param[in] MR0_value  Mode register value MR0
 *    @param[in] MR1_value  Mode register value MR1
 *    @param[in] MR2_value  Mode register value MR2
 **/
void ABHN_SHKE_Device_Init( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 clk_freq_in_khz, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value );

/** =============================================================================
 *  Function : Device_Init_LPDDR2
 ** =============================================================================
 */
/**
 *    @details
 *    LPDDR2 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] clk_freq_in_khz clock frequency (in khz) to update
 **/
void ABHN_SHKE_Device_Init_LPDDR2( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz );

/** =============================================================================
 *  Function : Device_Init_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    LPDDR3 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] clk_freq_in_khz clock frequency (in khz) to update
 *    @param[in] MR1_value  Mode register value MR1
 *    @param[in] MR2_value  Mode register value MR2
 **/
void ABHN_SHKE_Device_Init_LPDDR3( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz, uint32 MR1_value, uint32 MR2_value );

/** =============================================================================
 *  Function : Device_Init_PCDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    PCDDR3 initilization process
 *    note: one-time setting with DRAM timing needs to be done before this
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] clk_freq_in_khz clock frequency (in khz) to update
 *    @param[in] MR0_value  Mode register value MR0
 *    @param[in] MR1_value  Mode register value MR1
 *    @param[in] MR2_value  Mode register value MR2
 **/
void ABHN_SHKE_Device_Init_PCDDR3( uint32 _inst_, uint32 chip_select, uint32 clk_freq_in_khz, uint32 MR0_value, uint32 MR1_value, uint32 MR2_value );

/** =============================================================================
 *  Function : Read_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Mode register read; applies to LPDDR2/3 device type 
 *    note: chip_select needs to be rank0 or rank 1
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] mr_addr      mode register address to be read from the SDRAM device
 *    @param[out] The read data for the MRR (64 bit for Gandalf)
 **/
uint64 ABHN_SHKE_Read_MR( uint32 _inst_, uint32 chip_select, uint64 mr_addr );

/** =============================================================================
 *  Function : Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Mode register write; applies to LPDDR2/3 & PCDDR3 device types 
 *    including the extend mode register write
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] mr_addr      mode register address to be written to the SDRAM device
 *    @param[in] mr_data      write data for the SDRAM device
 **/
void ABHN_SHKE_Write_MR( uint32 _inst_, uint32 chip_select, uint32 mr_addr, uint32 mr_data );

/** =============================================================================
 *  Function : Extended_Write_MR
 ** =============================================================================
 */
/**
 *    @details
 *    Extended mode register write
 *  only applies to LPDDR3 device types which require CA calibration (calibration of the DDR command bus) 
 * 
 *    @param[in] chip_select  chip select to initialize for
 *    @param[in] mr_addr      mode register address to be written to the SDRAM device
 *    @param[in] mr_data      write data for the SDRAM device
 **/
void ABHN_SHKE_Extended_Write_MR( uint32 _inst_, uint32 chip_select, uint32 mr_addr, uint32 mr_data );

/** =============================================================================
 *  Function : Enter_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs into SW_based self refresh
 *    The memory controller provides SW the ability to enter and exit self refresh. 
 *    This is intended to be used for debug and for a few SW based system sequences.
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enter_Self_Refresh( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Exit_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs out of SW_based self refresh
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Exit_Self_Refresh( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    This mode is entirely HW controlled. 
 *    The controller will enter self refresh if the controller is idle for a programmable number of cycles 
 * 
 *    @param[in] chip_select  chip select to set
 *    @param[in] concurrent_sr set to 1 if in self refresh concueent mode
 **/
void ABHN_SHKE_Enable_HW_Self_Refresh( uint32 _inst_, uint32 chip_select, uint32 concurrent_sr );

/** =============================================================================
 *  Function : Disable_HW_Self_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Put DRAMs out of HW_based self refresh
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Disable_HW_Self_Refresh( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Enable periodic auto refresh
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_Auto_Refresh( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_Auto_Refresh
 ** =============================================================================
 */
/**
 *    @details
 *    Disable periodic auto refresh
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Disable_Auto_Refresh( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Manual_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Enable ZQ Calibration
 *    can only be done when the device is in idle state with all banks are precharged
 *    @param[in] short_seq short/long sequence
 **/
void ABHN_SHKE_Manual_ZQCAL( uint32 _inst_, uint32 short_seq );

/** =============================================================================
 *  Function : Enable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Enable periodic ZQ Calibration
 *    note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the ranks present
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_Periodic_ZQCAL( uint32 _inst_, uint32 chip_select );
/** =============================================================================
 *  Function : Disable_Periodic_ZQCAL
 ** =============================================================================
 */
/**
 *    @details
 *    Disable periodic ZQ Calibration
 *    note: ZQCAL_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the disable ranks 
 *    
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_Periodic_ZQCAL( uint32 _inst_, uint32 chip_select );

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
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_Periodic_SRR( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_Periodic_SRR
 ** =============================================================================
 */
/**
 *    @details
 *    Disable  periodic SRR 
 *    note: MRR_INTERVAL has been set in pre_init, here only set to the correct value 
 *          based on the disable ranks 
 *    
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_Periodic_SRR( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Manual_SRR
 ** =============================================================================
 */
/**
 *    @details
 *    SW can trigger a read of the temperature status register  
 *    
 *    @param[in] chip_select  chip select to set
 *    @param[out] the data for the SRR 
 **/
uint64 ABHN_SHKE_Manual_SRR( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enter_DPD
 ** =============================================================================
 */
/**
 *    @details
 *    Enter Deep Power Down (DPD), a low power state of the memory device wherein no data is retained
 *    A complete re_initialization sequence is needed for the memory back to normal mode
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enter_DPD( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_CKE
 ** =============================================================================
 */
/**
 *    @details
 *    Turn on CKE
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_CKE( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_CKE
 ** =============================================================================
 */
/**
 *    @details
 *    Turn off CKE
 *    
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_CKE( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_CK
 ** =============================================================================
 */
/**
 *    @details
 *    Turn on CK
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_CK( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_CK
 ** =============================================================================
 */
/**
 *    @details
 *    Turn off CK
 *    
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_CK( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_INITCOMPLETE
 ** =============================================================================
 */
/**
 *    @details
 *    Set INITCOMPLETE bit
 *    Once the rank enable and rank initcomplete bits are cleared, the status of the rank is uninitialized.
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_INITCOMPLETE( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_INITCOMPLETE
 ** =============================================================================
 */
/**
 *    @details
 *    clear INITCOMPLETE bit
 *    O
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_INITCOMPLETE( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Exit_DPD
 ** =============================================================================
 */
/**
 *    @details
 *    Exit Deep Power Down (DPD) mode, bring the memory out of DPD mode
 *    
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Exit_DPD( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Rank_Detection_LPDDR3
 ** =============================================================================
 */
/**
 *    @details
 *    The Rank Auto Detect sequence is used by SW to determine if a memory device is present on a specific rank.
 * 
 *    @param[out] populated rank(s)
 **/
uint32 ABHN_SHKE_Rank_Detection_LPDDR3( uint32 _inst_ );


/** =============================================================================
 *  Function : Rank_Detection_PCDDR3_Pre/Post
 ** =============================================================================
 */
/**
 *    @details
 *    The Rank Auto Detect sequence is used by SW to determine if a memory device is present on a specific rank.
 **/
void ABHN_SHKE_Rank_Detection_PCDDR3_Pre( uint32 _inst_, uint32 chip_select );
void ABHN_SHKE_Rank_Detection_PCDDR3_Post( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *    Enable SHKE Rank
 *    @param[in] chip_select  chip select to set
 **/
void ABHN_SHKE_Enable_Rank( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Disable_Rank
 ** =============================================================================
 */
/**
 *    @details
 *    Disable SHKE Rank
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_Rank( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Enable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *    Enable all periodic events in SHKE, including SRR, ZQCAL, and auto refresh
 * 
 *    @param[in] chip_select  chip select to set
 *    @param[in] device_type  device type to set
 *    @param[in] concurrent_sr set to 1 if in self refresh concueent mode
 **/
void ABHN_SHKE_Enable_All_Periodic( uint32 _inst_, uint32 chip_select, uint32 device_type, uint32 concurrent_sr );

/** =============================================================================
 *  Function : Disable_All_Periodic
 ** =============================================================================
 */
/**
 *    @details
 *    Disable all periodic events in SHKE, including SRR, ZQCAL, and auto refresh
 * 
 *    @param[in] chip_select  chip select to disable
 **/
void ABHN_SHKE_Disable_All_Periodic( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Get_Init_Complete
 ** =============================================================================
 */
/**
 *    @details
 *    used for Ram Size Detection in LPDDR3, check the inicomplete status
 * 
 *    @param[in] chip_select  chip select to check
 *    @param[out] the inicomplete status for selected rank
 **/
uint32 ABHN_SHKE_Get_Init_Complete( uint32 _inst_, uint32 chip_select );

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
 *    @param[in] chip_select  chip select to check
 **/
void ABHN_SHKE_Read_DQCAL( uint32 _inst_, uint32 chip_select );

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
void ABHN_SHKE_CA_Training_Toggle_Pattern( uint32 _inst_ );

/** =============================================================================
 *  Function : Get_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    get the status of Auto refresh for the ranks
 *    @param[out] Auto-refresh enable status for the ranks
 **/
uint32 ABHN_SHKE_Get_Auto_Refresh_Status( uint32 _inst_ );

/** =============================================================================
 *  Function : Set_Auto_Refresh_Status
 ** =============================================================================
 */
/**
 *    @details
 *    Sets the status of Auto refresh for the ranks
 *    @param[in] chip_select chip_select to initialize for, must be 0-3
 **/
void ABHN_SHKE_Set_Auto_Refresh_Status( uint32 _inst_, uint32 chip_select );

/** =============================================================================
 *  Function : Get_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    gets the HW self refresh register value for the ranks 
 *    @param[out] returns SELF_REFRESH_CNTL register value for the ranks
 **/
uint32 ABHN_SHKE_Get_HW_Self_Refresh_Reg( uint32 _inst_ );

/** =============================================================================
 *  Function : Set_HW_Self_Refresh_Reg
 ** =============================================================================
 */
/**
 *    @details
 *    restores the HW self refresh register value for the ranks.
 *    @param[in] reg_value has the register value to be restored
 **/
void ABHN_SHKE_Set_HW_Self_Refresh_Reg( uint32 _inst_, uint32 reg_value);

#endif







