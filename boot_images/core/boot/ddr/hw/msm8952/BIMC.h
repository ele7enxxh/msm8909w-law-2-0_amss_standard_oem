#ifndef __BIMC_H__
#define __BIMC_H__

/*
===========================================================================

FILE:         BIMC.h

DESCRIPTION:
  This is header file for BIMC v1 implementation. This file will contain BIMC
  specific HAL APIs.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8952/BIMC.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/24/14   tw      changed update clock period api to take in divider function
                   pointer for shared library support
02/09/14   tw      added support for common ddr library
09/17/12   sl      Updated HAL_SDRAM_RL/WL() etc..
09/06/12   sl      Added training pattern macros.
08/24/12   sl      Updated HAL_SDRAM_SHKE_Init() etc..
08/17/12   sl      Removed duplicate APIs with HAL_DDR.h.
08/06/12   tw      Added api for initialization done query
03/05/12   tw      Initial version
===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"


/*==============================================================================
                                  MACROS
==============================================================================*/
/* Training patterns */
#define DDR_CA_TRAIN_PRE_CS_PATTERN_MAP_0   0x02BB02AA
#define DDR_CA_TRAIN_CS_PATTERN_MAP_0       0x034E0062
#define DDR_CA_TRAIN_POST_CS_PATTERN_MAP_0  0x02FF02EE

#define DDR_CA_TRAIN_PRE_CS_PATTERN_MAP_1   0x02BB02AA
#define DDR_CA_TRAIN_CS_PATTERN_MAP_1       0x034E0062
#define DDR_CA_TRAIN_POST_CS_PATTERN_MAP_1  0x02FF02EE

#define DDR_CA_TRAINING_PATTERN_MAPPING_0_RAISING 0x0062
#define DDR_CA_TRAINING_PATTERN_MAPPING_1_RAISING 0x0062

#define DDR_CA_TRAINING_PATTERN_MAPPING_0_FALLING 0x034E
#define DDR_CA_TRAINING_PATTERN_MAPPING_1_FALLING 0x034E

#define DDR_DQ_READ_TRAINING_PATTERN 0xDEADBEEF

typedef uint32 (*ddr_divide)(uint32 a, uint32 b);

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ============================================================================
**  Function : HAL_SDRAM_BIMC_Set_CA_Training_Pattern
** ============================================================================
*/
/*!
*   @brief
*   This function will update the ac parameter
*
*   @details
*   This function will update the ac parameter
*
*   @param interface   -  [IN] interface to set training value for
*   @param chip_select      -  [IN] chip select to set training value for
*   @param mapping          -  [IN] set for either mapping 0 or 1
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_BIMC_Set_CA_Training_Pattern(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint8 mapping);

/* ============================================================================
**  Function : HAL_SDRAM_BIMC_CA_Training_Toggle_Pattern
** ============================================================================
*/
/*!
*   @brief
*   This function toggle the shke command to send one pulse of ca
*   training pattern
*
*   @details
*   This function toggle the shke command to send one pulse of ca
*   training pattern.
*
*   @param interface   -  [IN] interface to toggle training value for
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_BIMC_CA_Training_Toggle_Pattern(SDRAM_INTERFACE interface);

/* ============================================================================
**  Function : HAL_SDRAM_BIMC_Exit_CA_Training
** ============================================================================
*/
/*!
*   @brief
*   This function restores normal operation to bimc
*
*   @details
*   This function exits CA training and restores normal operations
*
*   @param interface   -  [IN] interface to set training value for
*   @param chip_select      -  [IN] chip select to set training value for
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_BIMC_Exit_CA_Training(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);

/* =============================================================================
**  Function : HAL_SDRAM_BIMC_Update_Clock_Period
** =============================================================================
*/
/**
*   @brief
*   Updates the bimc clock period for clock switching
*
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @return
*   None
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
void HAL_SDRAM_BIMC_Update_Clock_Period(uint32 base, uint32 clk_speed, ddr_divide divide_func);

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Pre_Clock_Switch
** ============================================================================
*/
/*!
 *   @brief
 *   This function update DPE configuration for preparing to clock switch
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed and update DPE configuration for clock switch
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param old_clk    -  [IN] Old clock speed (in KHz)
 *   @param new_clk    -  [IN] New clock speed (in KHz)
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Pre_Clock_Switch(uint32 base, SDRAM_INTERFACE interface,  uint32 old_clk, uint32 new_clk);

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Post_Clock_Switch
** ============================================================================
*/
/*!
 *   @brief
 *   This function update DPE configuration cleaning up after clock switch
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed, and also prepare the DPE for clock switch operation
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param old_clk    -  [IN] Old clock speed (in KHz)
 *   @param new_clk    -  [IN] New clock speed (in KHz)
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Post_Clock_Switch(uint32 base, SDRAM_INTERFACE interface,  uint32 old_clk, uint32 new_clk);

/* =============================================================================
**  Function : HAL_SDRAM_BIMC_Rank_Detection
** =============================================================================
*/
/**
*   @brief
*   Detect present ranks by reading a mode register on that rank. A rank is
*   present if and only if the rank is initialized and the mode register read
*   does not time out.
*
*   @param[in]  interface  Interface to detect for
*
*   @return
*   Present ranks.
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
SDRAM_CHIPSELECT HAL_SDRAM_BIMC_Rank_Detection(SDRAM_INTERFACE interface);

/* =============================================================================
**  Function : HAL_SDRAM_SHKE_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize BIMC SHKE.
*
*   @param[in]  interface  Interface to initialize for
*
*   @retval  None
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
void HAL_SDRAM_SHKE_Init(uint32 base, SDRAM_INTERFACE interface);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Device_Init
** ============================================================================
*/
/**
*   @brief
*   Initialize DDR device.
*
*   @param[in]  interface    Interface to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_speed    Clock speed (in KHz)
*
*   @retval  None
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
void HAL_SDRAM_SHKE_Device_Init(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select, uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the requested ddr into self refresh
*
*   @details
*   This function will put the requested ddr into self refresh.
*   Note in dual rank configuration both rank must go into self refresh
*   at the same time.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Enter_Self_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take the requested ddr out of self refresh
*
*   @details
*   This function will take the requested ddr out of self refresh.
*   Note in dual rank configuration both rank must go and out of self refresh
*   at the same time.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Exit_Self_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_Auto_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will turn on periodic auto refresh
*
*   @details
*   This function will turn on periodic auto refresh
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   Periodic timer (TREFI) must be programmed prior to calling this api
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Enable_Auto_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_Auto_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will turn off periodic auto refresh
*
*   @details
*   This function will turn off periodic auto refresh
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Disable_Auto_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_HW_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take turn on the hw based self refresh
*
*   @details
*   This function will take turn on the hardware based self refresh.
*   This will put the ddr into self refresh after a predetermined idle duration.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_HW_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will take turn off the hw based self refresh
*
*   @details
*   This function will take turn off the hardware based self refresh.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_Periodic_SRR
** ============================================================================
*/
/*!
*   @brief
*   This function will turn on period SRR
*
*   @details
*   This function will turn on period SRR to enable temperature compensated
*   auto refresh.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Enable_Periodic_SRR(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_Periodic_SRR
** ============================================================================
*/
/*!
*   @brief
*   This function will turn off period SRR
*
*   @details
*   This function will turn off period SRR to enable temperature compensated
*   auto refresh.
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Disable_Periodic_SRR(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Enable_ZQCAL
** ============================================================================
*/
/*!
*   @brief
*   This function will turn on period ZQCAL
*
*   @details
*   This function will turn on period ZQCAL
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Enable_ZQCAL(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_Disable_ZQCAL
** ============================================================================
*/
/*!
*   @brief
*   This function will turn off period ZQCAL
*
*   @details
*   This function will turn off period ZQCAL
*
*   @param interface    -  [IN] ddr interface select
*   @param chip_select  -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_Disable_ZQCAL(uint32 base, SDRAM_INTERFACE interface,  SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_DPD_Enter
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr device into deep power down mode
*
*   @details
*   This function will put the ddr device into deep power down mode
*
*   @param interface   -  [IN] ddr interface select
*   @param chip_select      -  [IN] ddr chip select
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   All content on the ddr being put into deep power down will be lost.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_DPD_Enter(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SHKE_DPD_Exit
** ============================================================================
*/
/*!
*   @brief
*   This function will take the ddr device out of deep power down mode
*
*   @details
*   This function will take the ddr device out of deep power down mode
*
*   @param interface   -  [IN] ddr interface select
*   @param chip_select      -  [IN] ddr chip select
*   @param clk_speed        -  [IN] ddr clock speed (in KHz)
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   All content on the ddr that was on ddr is gone, reset value is not specified.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SHKE_DPD_Exit(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed);

/* =============================================================================
**  Function : HAL_SDRAM_DPE_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize BIMC DPE.
*
*   @param[in]  interface  Interface to initialize for
*
*   @retval  None
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
void HAL_SDRAM_DPE_Init(uint32 base, SDRAM_INTERFACE interface);

/* =============================================================================
**  Function : HAL_SDRAM_DPE_Update_AC_Parameters
** =============================================================================
*/
/**
*   @brief
*   Configure DPE timing.
*
*   @param[in]  interface  Interface to configure for
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @retval  None
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
void HAL_SDRAM_DPE_Update_AC_Parameters(uint32 base, SDRAM_INTERFACE interface,  uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Disable_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will Disable Power down and clock stop power down
*
*   @details
*   This function will Disable Power down and clock stop power down
*
*   @param interface   -  [IN] Param_description_goes_here
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_DPE_Disable_Power_Down(uint32 base, SDRAM_INTERFACE interface);

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Update_Latency
** ============================================================================
*/
/*!
 *   @brief
 *   This function will update the read write latency based on the given
 *   clock speed
 *
 *   @details
 *   This function will update the read write latency based on the given
 *   clock speed
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param clk_speed  -  [IN] clock speed in khz
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Update_Latency(uint32 base, SDRAM_INTERFACE interface,  uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Force_Update_Latency
** ============================================================================
*/
/*!
 *   @brief
 *   This function will force update the read write latency based on the given
 *   clock speed
 *
 *   @details
 *   This function will force update the read write latency based on the given
 *   clock speed, unlike HAL_SDRAM_DPE_Update_Latency, this function will apply
 *   the new latency immediately
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param clk_speed  -  [IN] clock speed in khz
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Force_Update_Latency(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed);

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Init
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the DPE block.
*
*   @details
*   Detailed_Description_Goes_here
*
*   @param interface   -  [IN] Param_description_goes_here
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Init(uint32 base, SDRAM_INTERFACE interface);

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Disable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the disable the rank given in the parameter
*
*   @details
*   This function will initialize the disable the rank given in the parameter
*
*   @param interface   -  [IN] interface to disable the rank for
*   @param chip_select      -  [IN] chipselect for rank disable
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   Any new traffic going to the rank disabled will be blocked and generate an error
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Rank_Disable(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Enable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the enable the rank given in the parameter
*
*   @details
*   This function will initialize the enable the rank given in the parameter
*
*   @param interface   -  [IN] interface to enable the rank for
*   @param chip_select      -  [IN] chipselect for rank enable
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Rank_Enable(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select);


#endif /* __BIMC_H__ */
