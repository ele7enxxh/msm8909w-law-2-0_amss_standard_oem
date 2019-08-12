/*
===========================================================================

FILE:         DDR_PHY_v2.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  PHY Gen 2 controller.

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/phy/v2.5/DDR_PHY.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/21/13   tw      Initial Port for 8084
05/13/13   sl      Updated delta table calls.
03/01/13   tw      Added calling delta table
11/15/12   tw      klockwork fix
11/02/12   sy      Backout 800Mhz
10/31/12   sl      Changed ddr_static_config_init() to ddr_set_config().
10/09/12   tw      error checking for klocwork
09/19/12   sl      Updated IOCAL functions.
08/17/12   sl      Renamed IOCAL functions.
08/10/12   sl      Added HAL_SDRAM_PHY_Init(); code cleanup.
06/06/12   tw      Initial version.
===========================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_drivers.h"
#include "ddr_target.h"
#include "ddr_internal.h"
#include "ddr_params.h"
#include "ddr_config.h"
#include "AHB2PHY.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  Function : HAL_SDRAM_PHY_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR PHY.
*
*   @param[in]  interface  Interface to initialize for
*   @param[in]  clk_speed  Clock speed (in KHz) to initialize for
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
void HAL_SDRAM_PHY_Init(SDRAM_INTERFACE interface, uint32 clk_speed)
{

} /* HAL_SDRAM_PHY_Init */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable auto IOCAL.
*
*   @param[in]  interface  Interface to enable auto IOCAL for
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
void HAL_SDRAM_PHY_Enable_IO_Cal(SDRAM_INTERFACE interface)
{

} /* HAL_SDRAM_PHY_Enable_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable auto IOCAL.
*
*   @param[in]  interface  Interface to disable auto IOCAL for
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
void HAL_SDRAM_PHY_Disable_IO_Cal(SDRAM_INTERFACE interface)
{

} /* HAL_SDRAM_PHY_Disable_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Manual_IO_Cal
** =============================================================================
*/
/**
*   @brief
*   Issue manual IOCAL.
*
*   @param[in]  interface  Interface to issue manual IOCAL for
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
void HAL_SDRAM_PHY_Manual_IO_Cal(SDRAM_INTERFACE interface)
{
} /* HAL_SDRAM_PHY_Manual_IO_Cal */

/* ============================================================================
**  Function : HAL_SDRAM_CA_Training
** ============================================================================
*/
/*!
*   @brief
*   This function trains the CA address bus to the CK
*
*   @details
*   CA training aligns the CA address bus to the CK. This training is performed once after DRAM
*   initialization. The training is performed in a way that DRAM returns latched CA addresses to the
*   DQ DDR PHY.
*   Note: HPG refers to mapping 1 and 2, to make code more readable we use terminology of mapping
*   0 and 1 to be consistent with array index.
*
*   @param interface_name   -  [IN] The interface to training ca training for
*   @param chip_select      -  [IN] The chip select to train ca training for
*
*   @par Dependencies
*   This calibration should be run at the max speed the interface supports
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

boolean HAL_SDRAM_CA_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 * ca_trained_delay)
{
  return FALSE;
}

/* ============================================================================
**  Function : HAL_SDRAM_Write_Leveling
** ============================================================================
*/
/*!
*   @brief
*   This function trains the DQ DDR Phy write level delay
*
*   @details
*   The DQ DDR PHY write level delay is set by programming SW registers. This
*   setting controls the delay of DQS (and DQ) from the 1X ddr clock (i_ddr_clk).
*   LPDDR3 DRAMs have a special mode which returns a zero or one deepening on
*   whether DQS arrives before or after the CK signal (Refer to JEDEC LPDDR3 spec).
*   By iteratively changing the delay setting, we can determine the correct DQS
*   delay setting to match CK delay.
*
*   The SW algorithm implemented first starts off by enabling all the DQ phy for
*   broadcast. It then triggers for a series of write leveling pulses. With each
*   pulse the software will trigger for a helper function to read and update the
*   one_count and zero_count value. Training is finished for that particular DQ
*   once the one_count value exceeds the zero_count values. Once training is
*   finished for that particular DQ, we disable broadcasting for that DQ. The
*   loop for calibration might still run for more iterations until all the DQs
*   have completed their training, or until we run out of delay settings to try,
*   in which we then will return FALSE to indicate training failure. If all DQs
*   are trained successfully, we will return TRUE.
*
*   @param interface_name   -  [IN] The interface to training write leveling for
*   @param chip_select      -  [IN] The chip select to train write leveling for
*   @param first_pass       -  [IN] First pass training uses default coarse delay
*                                   second pass training does not need to reset
*                                   the coarse delay.
*
*   @par Dependencies
*   This calibration should be run at the max speed the interface supports
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

boolean HAL_SDRAM_Write_Leveling(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, boolean first_pass)
{
  return FALSE;
}

/* ============================================================================
**  Function : HAL_SDRAM_Read_Training
** ============================================================================
*/
/*!
*   @brief
*   This function trains the read on DQ-DQS
*
*   @details
*   The goal of read training is to maximize the eye opening on each DQ by removing the relative
*   static skew between read DQS and each DQ. This is achieved by adjusting programmable delay on
*   read DQS (T/4) and each individual DQ bit (per-bit deskew).
*
*   @param interface_name   -  [IN] The interface to training ca training for
*   @param interface_name   -  [IN] The chip select ti train ca training for
*
*   @par Dependencies
*   This calibration should be run at the max speed the interface supports
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

boolean HAL_SDRAM_Read_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 * read_training)
{
  return FALSE;
}

/* ============================================================================
**  Function : HAL_SDRAM_Write_Training
** ============================================================================
*/
/*!
*   @brief
*   This function trains the write on the DQ-DQS
*
*   @details
*   The goal of read training is to maximize the eye opening on each DQ by removing the relative
*   static skew between read DQS and each DQ. This is achieved by adjusting programmable delay on
*   read DQS (T/4) and each individual DQ bit (per-bit deskew).
*
*   @param interface_name   -  [IN] The interface to training write training for
*   @param interface_name   -  [IN] The chip select to train write training for
*
*   @par Dependencies
*   This calibration should be run at the max speed the interface supports
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

boolean HAL_SDRAM_Write_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 *write_training)
{
  return FALSE;
}

/* ============================================================================
**  Function : HAL_SDRAM_PHY_DCC_Calibration
** ============================================================================
*/
/*!
*   @brief
*   DCC usage is only recommended when the highest DDR operation frequency is greater than
*   [TBD] MHz. It is recommended to bypass DCC (thus, bypassing the DCC calibration sequence)
*   when the highest DDR opera to in frequency is lower than [TBD] MHz.
*   Calibration of DCC is required during power-on-reset before BIMC/DRAM initialization, as part
*   of DDR initialization.
*
*   @details
*   DCC usage is only recommended when the highest DDR operation frequency is greater than
*   [TBD] MHz. It is recommended to bypass DCC (thus, bypassing the DCC calibration sequence)
*   when the highest DDR opera to in frequency is lower than [TBD] MHz.
*   Calibration of DCC is required during power-on-reset before BIMC/DRAM initialization, as part
*   of DDR initialization.
*
*   @param
*   None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  boolean - dcc calibration success or not.
*
*   @sa None
*/
boolean HAL_SDRAM_PHY_DCC_Calibration(SDRAM_INTERFACE interface_name, uint32 dcc_offset)
{
  return FALSE;
}

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Get_HW_Major_Revision
** ============================================================================
*/
/*!
*   @brief
*   This function returns the phy hardware major revision id
*
*   @details
*   This function returns the phy hardware major revision id
*
*   @param
*   None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  uin8 - major revision id
*
*   @sa None
*/
uint8 HAL_SDRAM_PHY_Get_HW_Major_Revision(void)
{
  return 0;
}

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Apply_Training_Parameters
** ============================================================================
*/
/*!
*   @brief
*   This function updates the phy delays to trained values
*
*   @details
*   This function updates the phy delays to trained values
*
*   @param
*   None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval  boolean - operation successful/failure
*
*   @sa None
*/
boolean HAL_SDRAM_PHY_Apply_Training_Parameters(SDRAM_INTERFACE interface_name)
{
  return FALSE;
}
