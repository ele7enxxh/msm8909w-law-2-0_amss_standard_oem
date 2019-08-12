/*
===========================================================================

FILE:         DDR_PHY_v2.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  PHY Gen 2 controller.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/phy/v2/DDR_PHY.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/20/13   tw      added api to switch between lp cdc and hpcdc by changing the
                   fpm threshold
03/27/13   tw      Reduced phy training parameter size
10/09/12   tw      Fixes for PHY training
10/09/12   tw      error checking for klocwork
09/19/12   sl      Updated IOCAL functions.
08/17/12   sl      Renamed IOCAL functions.
08/10/12   sl      Added HAL_SDRAM_PHY_Init(); code cleanup.
06/06/12   tw      Initial version.
===========================================================================
             Copyright 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_drivers.h"
#include "ddr_target.h"
#include "ddr_params.h"
#include "ddr_config.h"
#include "AHB2PHY.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "dog.h"


/*==============================================================================
                                  DATA
==============================================================================*/
extern uint32 phy_threshold[DDR_PHY_MAX_THRESHOLD_NUM];

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
{} /* HAL_SDRAM_PHY_Init */

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
  uint32 dim_offset;

  /* DIM offset */
  dim_offset = (interface == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  /* Enable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_USE_LAST),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_USE_LAST));

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
  uint32 dim_offset;

  /* DIM offset */
  dim_offset = (interface == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  /* Disable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO));

  /* Check for any on-going IOCAL operations to be completed */
  while ( BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS), dim_offset,
                        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS), IOCAL_IN_PROGRESS)) )
  {}

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
  uint32 dim_offset;
  uint32 io_cal_auto;

  /* DIM offset */
  dim_offset = (interface == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  io_cal_auto = BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                              HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO));
  if (io_cal_auto)
  {
    /* Disable auto IOCAL */
    HAL_SDRAM_PHY_Disable_IO_Cal(interface);
  }

  /* Disable dynamic bandgap control to force bandgap on */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL));

  /* Wait for 10us */
  HAL_SDRAM_DDR_Wait(10, 0);

  /* Configure IOCAL controller for manual IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW));

  /* Check for IOCAL to be completed */
  while ( BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS), dim_offset,
                        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS), IOCAL_IN_PROGRESS)) )
  {}

  /* De-assert manual IOCAL trigger */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW));

  /* Enable dynamic bandgap control */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL));

  if (io_cal_auto)
  {
    /* Enable auto IOCAL */
    HAL_SDRAM_PHY_Enable_IO_Cal(interface);
  }

} /* HAL_SDRAM_PHY_Manual_IO_Cal */

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
  uint32 major_rev = 0;

  major_rev = BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_HW_INFO),
                            0x0,
                            HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_HW_INFO), MAJOR_REV));

  major_rev >>= HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_HW_INFO), MAJOR_REV);

  return major_rev;
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
  uint32 offset;
  uint32 i, byte_counter;
  uint32 frequency;
  uint32 delay_offset = 0;
  uint8 delay_setting = 0;
  /* Structure to save training data */
  struct ddr_trained_params_lpddr3 *training_params;

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  /* Get DDR Training parameters */
  training_params = &(ddr_get_trained_params(interface_name)->lpddr3_training);

  if(training_params == NULL)
  {
    /* training parameter not found */
    return FALSE;
  }

  /* Update max frequency in phy threshold */
  phy_threshold[DDR_PHY_MAX_THRESHOLD_NUM - 1] = training_params->freq[training_params->nlevels - 1];

  /* get number of frequencies trained for */

  for(i = 0; i < DDR_PHY_MAX_THRESHOLD_NUM; i++)
  {
    frequency = phy_threshold[i];

    if(frequency > DDR_PHY_DELAY0_THRESHOLD)
    {
      delay_offset = 0;
    }
    else if(frequency > DDR_PHY_DELAY1_THRESHOLD)
    {
      delay_offset = sizeof(uint32) * 1;
    }
    else if(frequency > DDR_PHY_DELAY2_THRESHOLD)
    {
      delay_offset = sizeof(uint32) * 2;
    }
    else
    {
      delay_offset = sizeof(uint32) * 3;
    }

    /* Ca training parameters */
        
    delay_setting = training_params->phy_params.ca_fine_dly;
    delay_setting |= (training_params->phy_params.ca_coarse_dly * phy_threshold[DDR_PHY_MAX_THRESHOLD_NUM - 1] / phy_threshold[i]) << 3;
    
    BOOT_HWIO_OUTM(
        PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
        (offset + delay_offset),
        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
        delay_setting
        << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));

    for (byte_counter = 0; byte_counter < 4; byte_counter++)
    {    
      /* Read training parameters */
      
      delay_setting = training_params->phy_params.read_fine_delay[byte_counter];
      delay_setting |= (training_params->phy_params.read_coarse_delay[byte_counter] * phy_threshold[DDR_PHY_MAX_THRESHOLD_NUM - 1] / phy_threshold[i]) << 3;
          
      BOOT_HWIO_OUTM(
          PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
          (offset + delay_offset + SDRAM_DQ1_OFFSET * byte_counter),
          HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
          delay_setting
          << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));

      /* Write training parameters */

      delay_setting = training_params->phy_params.write_fine_delay[byte_counter];
      delay_setting |= (training_params->phy_params.write_coarse_delay[byte_counter] * phy_threshold[DDR_PHY_MAX_THRESHOLD_NUM - 1] / phy_threshold[i]) << 3;
      
      BOOT_HWIO_OUTM(
          PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
          (offset + delay_offset + SDRAM_DQ1_OFFSET * byte_counter),
          HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
          delay_setting
          << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
    }
  }
  
  /* Apply DCC training and set to manual adjustment mode*/
  for(i = 0; i < 5; i++)
  {
    BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), (offset + i*DDR_DCC_CA_DQ_OFFSET), 
      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), DCC_MAN_ADJ) | 
      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), DCC_MODE),
      training_params->phy_dcc[i].dcc0 | 
      0x0 << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), DCC_MODE));
    BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_DCC1_CFG), (offset + i*DDR_DCC_CA_DQ_OFFSET), 
      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC1_CFG), DCC_MAN_ADJ) | 
      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC1_CFG), DCC_MODE),
      training_params->phy_dcc[i].dcc1 | 
      0x0 << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_DCC1_CFG), DCC_MODE));    
  }

  return TRUE;
}

/* =============================================================================
**  Function : HAL_SDRAM_PHY_HP_CDC_Workaround
** =============================================================================
*/
/**
*   @brief
*   Apply HP CDC workaround for one time configuration during boot.
*
*   @param[in]  interface  Interface to apply workaround on.
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
void HAL_SDRAM_PHY_HP_CDC_Workaround(SDRAM_INTERFACE interface_name)
{
  uint32 dim_offset;
  ddr_mask ddr_broadcast_mask = {0};

  /* Enable broadcast mode on DQ0 and 1 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;
  
  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* DIM offset */
  dim_offset = (interface_name == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_CDC_CFG), dim_offset, 0x00091901);
  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_CDC_CFG), DQ_BCAST_OFFSET, 0x00091901);
  
  /* wait for 50us */
  HAL_SDRAM_DDR_Wait(50, 0);
  
  BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_CDC_CFG), dim_offset, 0x00091913);
  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_CDC_CFG), DQ_BCAST_OFFSET, 0x00091913);
  
   /* wait for 50us */
  HAL_SDRAM_DDR_Wait(50, 0);

} /* HAL_SDRAM_PHY_HP_CDC_Workaround */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_LP_HP_CDC_Threshold
** =============================================================================
*/
/**
*   @brief This api takes in the new threshold passed in as paramter and applies it
*          as the HP and LP cdc threshold
*   
*
*   @param[in]  interface  Interface to apply workaround on.
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
void HAL_SDRAM_PHY_LP_HP_CDC_Threshold(SDRAM_INTERFACE interface_name, uint32 threshold)
{
  uint32 dim_offset;
  ddr_mask ddr_broadcast_mask = {0};

  /* Enable broadcast mode on DQ0 and 1 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;
  
  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* DIM offset */
  dim_offset = (interface_name == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_FPM_CFG1), 
                  dim_offset, 
                  HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_FPM_CFG1), CDC_LP_HP_THRESHOLD_PERIOD),
                  threshold);
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_FPM_CFG1), 
                  DQ_BCAST_OFFSET, 
                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_FPM_CFG1), CDC_LP_HP_THRESHOLD_PERIOD),
                  threshold);
} /* HAL_SDRAM_PHY_LP_HP_CDC_SWITCH */
