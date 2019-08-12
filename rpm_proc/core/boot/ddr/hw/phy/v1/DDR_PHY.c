/**
 * @file DDR_PHY.c
 * @brief
 * Hardware abstraction layer of DDR PHY drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/phy/v1/DDR_PHY.c#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/26/12   sl      Initial version.
================================================================================
                      Copyright 2012 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_hwio.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "HAL_DDR.h"
#include "DDR_PHY.h"
#include "AHB2PHY.h"


/*==============================================================================
                                  DATA
==============================================================================*/
/* DQ PHY offsets */
static uint32 dq_offsets[] = { PHY_DQ0_OFFSET, PHY_DQ1_OFFSET,
                               PHY_DQ2_OFFSET, PHY_DQ3_OFFSET };


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
{}

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
  uint32 ca_master_offset;

  /* Offset for CA Master PHY */
  ca_master_offset = ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  /* Enable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO) |
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_USE_LAST),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_USE_LAST));

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
  uint32 ca_master_offset;

  /* Offset for CA Master PHY */
  ca_master_offset = ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  /* Disable auto IOCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                 HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO));

  /* Wait for 65us (at least 2 sleep clock cycles + 3 XO clock cycles) */
  HAL_SDRAM_DDR_Wait(65, 0);

  /* Check for any on-going IOCAL operations to be completed */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_STATUS), ca_master_offset,
                         HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_STATUS), IOCAL_DONE_D)) )
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
  uint32 ca_master_offset;
  uint32 bandgap_ena, io_cal_auto;

  /* Offset for CA Master PHY */
  ca_master_offset = ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET
                                                       : PHY_1_OFFSET) + PHY_CA_MST_OFFSET;

  bandgap_ena = BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                              HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA1) |
                              HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA0));

  /* Do manual IOCAL only when bandgap is enabled */
  if ( bandgap_ena == (0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA1) |
                       0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), BANDGAP_ENA0)) )
  {
    io_cal_auto = BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                                HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), IO_CAL_AUTO));

    if (io_cal_auto)
    {
      /* Disable auto IOCAL */
      HAL_SDRAM_PHY_Disable_IO_Cal(interface);
    }

    /* Configure IOCAL controller for manual IOCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                   HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW));

    /* Check for IOCAL to be completed */
    while ( !BOOT_HWIO_INM(PHY_CA_ADDR(IOC_CTLR_STATUS), ca_master_offset,
                           HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_STATUS), IOCAL_DONE_D)) )
    {}

    /* De-assert manual IOCAL trigger */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(IOC_CTLR_CFG), ca_master_offset,
                   HWIO_FMSK(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(IOC_CTLR_CFG), CAL_NOW));

    if (io_cal_auto)
    {
      /* Enable auto IOCAL */
      HAL_SDRAM_PHY_Enable_IO_Cal(interface);
    }
  }

} /* HAL_SDRAM_PHY_Manual_IO_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable auto CDCCAL.
*
*   @param[in]  interface  Interface to enable auto CDCCAL for
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
void HAL_SDRAM_PHY_Enable_CDC_Cal(SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

  for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + dq_offsets[dq];

    /* Enable DQ auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Enable DQ auto-CDCCAL timer */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Enable CA auto CDCCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

  /* Enable CA auto-CDCCAL timer */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

} /* HAL_SDRAM_PHY_Enable_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable auto CDCCAL.
*
*   @param[in]  interface  Interface to disable auto CDCCAL for
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
void HAL_SDRAM_PHY_Disable_CDC_Cal(SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

  for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + dq_offsets[dq];

    /* Disable DQ auto-CDCCAL timer */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

    /* Disable DQ auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  }

  for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + dq_offsets[dq];

    /* Check state machine idle status */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS0), CTLR_SM_IDLE)) )
    {}
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS0), CTLR_SM_IDLE)) )
    {}
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Disable CA auto-CDCCAL timer */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  /* Disable CA auto CDCCAL */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

  /* Check state machine idle status */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(CDC_STATUS0), offset,
                         HWIO_FMSK(PHY_CA_ADDR(CDC_STATUS0), CTLR_SM_IDLE)) )
  {}

} /* HAL_SDRAM_PHY_Disable_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Manual_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Issue manual CDCCAL.
*
*   @param[in]  interface  Interface to issue manual CDCCAL for
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
void HAL_SDRAM_PHY_Manual_CDC_Cal(SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 hw_autocal_ena, timer_ena;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

  hw_autocal_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CTLR_CFG0), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  timer_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), phy_offset + PHY_DQ0_OFFSET,
                            HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  if (hw_autocal_ena && timer_ena)
  {
    /* Disable auto CDCCAL */
    HAL_SDRAM_PHY_Disable_CDC_Cal(interface);
  }

  for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + dq_offsets[dq];

    /* Assert DQ SW CDCCAL trigger */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                   0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

    /* De-assert DQ SW CDCCAL trigger */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                   0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));
  }

  for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
  {
    /* Offset for each DQ PHY */
    offset = phy_offset + dq_offsets[dq];

    /* Wait for DQ write CDCCAL done */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS0), CALIBRATION_DONE)) )
    {}
    /* Wait for DQ read CDCCAL done */
    while ( !BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS0), offset,
                           HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS0), CALIBRATION_DONE)) )
    {}
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;

  /* Assert CA SW CDCCAL trigger */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

  /* De-assert CA SW CDCCAL trigger */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                 HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB),
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), SW_TRIG_FULL_CALIB));

  /* Wait for CA CDCCAL done */
  while ( !BOOT_HWIO_INM(PHY_CA_ADDR(CDC_STATUS0), offset,
                         HWIO_FMSK(PHY_CA_ADDR(CDC_STATUS0), CALIBRATION_DONE)) )
  {}

  if (hw_autocal_ena && timer_ena)
  {
    /* Enable auto CDCCAL */
    HAL_SDRAM_PHY_Enable_CDC_Cal(interface);
  }

} /* HAL_SDRAM_PHY_Manual_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Enable rapid CDCCAL.
*
*   @param[in]  interface  Interface to enable rapid CDCCAL for
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
void HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal(SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 hw_autocal_ena, timer_ena;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

  hw_autocal_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CTLR_CFG0), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));
  timer_ena = BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), phy_offset + PHY_DQ0_OFFSET,
                            HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG0), TIMER_ENA));

  /* Enable rapid auto CDCCAL only when normal auto CDCCAL is enabled */
  if (hw_autocal_ena && timer_ena)
  {
    /* Disable normal auto CDCCAL */
    HAL_SDRAM_PHY_Disable_CDC_Cal(interface);

    for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + dq_offsets[dq];

      /* Configure DQ CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                     0x10U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     0x8U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

      /* Enable DQ auto CDCCAL */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                     0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

      /* Enable DQ auto-CDCCAL timer based on ff_clk */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                     0x1U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));
    }

    /* Offset for CA Master PHY */
    offset = phy_offset + PHY_CA_MST_OFFSET;

    /* Configure CA CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_REFCOUNT_CFG), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                   0x10U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   0x8U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

    /* Enable CA auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Enable CA auto-CDCCAL timer based on ff_clk */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                   0x1U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));
  }

} /* HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal
** =============================================================================
*/
/**
*   @brief
*   Disable rapid CDCCAL.
*
*   @param[in]  interface  Interface to disable rapid CDCCAL for
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
void HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal(SDRAM_INTERFACE interface)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

  /* Disable rapid auto CDCCAL only when rapid auto CDCCAL is enabled */
  if ( BOOT_HWIO_INM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), phy_offset + PHY_DQ0_OFFSET,
                                 HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA)) )
  {
    for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + dq_offsets[dq];

      /* Disable DQ auto-CDCCAL timer based on ff_clk */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                     0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));

      /* Disable DQ auto CDCCAL */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_CTLR_CFG0), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                     0x0U << HWIO_SHFT(PHY_DQ_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

      /* Configure DQ CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     HWIO_FMSK(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                     0x20U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                     0x20U << HWIO_SHFT(PHY_DQ_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));
    }

    /* Offset for CA Master PHY */
    offset = phy_offset + PHY_CA_MST_OFFSET;

    /* Disable CA auto-CDCCAL timer based on ff_clk */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CAL_TIMER_CFG1), FF_TIMER_ENA));

    /* Disable CA auto CDCCAL */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_CTLR_CFG0), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA),
                   0x0U << HWIO_SHFT(PHY_CA_ADDR(CDC_CTLR_CFG0), HW_AUTOCAL_ENA));

    /* Configure CA CDC_REFCOUNT_CFG[CCAL/FCAL_REF_COUNT] */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_REFCOUNT_CFG), offset,
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   HWIO_FMSK(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT),
                   0x20U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), CCAL_REF_COUNT) |
                   0x20U << HWIO_SHFT(PHY_CA_ADDR(CDC_REFCOUNT_CFG), FCAL_REF_COUNT));

    /* Enable normal auto CDCCAL */
    HAL_SDRAM_PHY_Enable_CDC_Cal(interface);
  }

} /* HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal */

/* =============================================================================
**  Function : HAL_SDRAM_PHY_Update_CDC_Config
** =============================================================================
*/
/**
*   @brief
*   Update CDC configuration according to voltage.
*
*   @param[in]  interface  Interface to update CDC configuration for
*   @param[in]  voltage    Voltage (in uV)
*
*   @retval  TRUE   CDC configuration updated successfully
*   @retval  FALSE  CDC configuration updated unsuccessfully
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
boolean HAL_SDRAM_PHY_Update_CDC_Config(SDRAM_INTERFACE interface, uint32 voltage)
{
  uint32 config;
  uint32 delay;
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  for (config = 0; ddr_phy_config_vddmx[config].vltg_in_uv != 0; config++)
  {
    if (ddr_phy_config_vddmx[config].vltg_in_uv == voltage)
    {
      delay = ddr_phy_config_vddmx[config].delay;

      /* PHY offset */
      phy_offset = (interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET;

      for (dq = 0; dq < sizeof(dq_offsets)/sizeof(dq_offsets[0]); dq++)
      {
        /* Offset for each DQ PHY */
        offset = phy_offset + dq_offsets[dq];

        /* Update DQ read CDC delay */
        BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                       HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                       delay << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));

        /* Update DQ write CDC delay */
        BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                       HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                       delay << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
      }

      /* Offset for CA Master PHY */
      offset = phy_offset + PHY_CA_MST_OFFSET;

      /* Update CA CDC delay */
      BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                     HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                     delay << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));

      break;
    }
  }

  return (ddr_phy_config_vddmx[config].vltg_in_uv != 0);

} /* HAL_SDRAM_PHY_Update_CDC_Config */
