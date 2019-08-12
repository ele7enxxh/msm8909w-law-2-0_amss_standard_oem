#ifndef __DDR_PHY_H__
#define __DDR_PHY_H__

/*
===========================================================================

FILE:         DDR_PHY.h

DESCRIPTION:  
  This is the ddr rpm message driver that will handle the rpm message 
  requests. There is an init function to register with the rpmfw with the
  callback function, and the callback function that will handle the message
  requests.

Copyright 2012 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/phy/v2/DDR_PHY.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/26/13   sr      Removed the unreferenced declaration HAL_SDRAM_PHY_Apply_Training_Parameters.
08/17/12   sl      Added IOCAL PHY functions.
08/10/12   sl      Added HAL_SDRAM_PHY_Init(); code cleanup.
06/06/12   tw      Initial version.
===========================================================================
*/


/*==============================================================================
                                  TYPES
==============================================================================*/
typedef enum
{
  DDR_PHY_GEN_1 = 0,
  DDR_PHY_GEN_2,
  DDR_PHY_GEN_2_5,
  DDR_PHY_GEN_3,
} DDR_PHY_HW_INFO_MAJOR;


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
void HAL_SDRAM_PHY_Init(SDRAM_INTERFACE interface, uint32 clk_speed);

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
void HAL_SDRAM_PHY_Enable_IO_Cal(SDRAM_INTERFACE interface);

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
void HAL_SDRAM_PHY_Disable_IO_Cal(SDRAM_INTERFACE interface);

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
void HAL_SDRAM_PHY_Manual_IO_Cal(SDRAM_INTERFACE interface);

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
*   @param interface_name   -  [IN] The chip select ti train ca training for
*
*   @par Dependencies
*   This calibration should be run at the max speed the interface supports
*
*   @par Side Effects
*   None
*
*   @retval  boolean - TRUE/FALSE for training success.
*
*   @sa None
*/
boolean HAL_SDRAM_CA_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 *ca_trained_delay);

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

boolean HAL_SDRAM_Write_Leveling(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, boolean first_pass);

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

boolean HAL_SDRAM_Read_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 *read_training);

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

boolean HAL_SDRAM_Write_Training(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 *write_training);

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
boolean HAL_SDRAM_PHY_DCC_Calibration(SDRAM_INTERFACE interface_name, uint32 dcc_offset);

#endif /* __DDR_PHY_H__ */
