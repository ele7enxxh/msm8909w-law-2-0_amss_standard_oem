/*
===========================================================================

FILE:         DDR_PHY_v2.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  PHY Gen 2 controller.

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/phy/v2/DDR_PHY.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/26/13   sr      Removed the unreferenced code HAL_SDRAM_PHY_Apply_Training_Parameters.
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
  uint32 dim_offset;
  ddr_mask ddr_broadcast_mask = {0};

  /* DIM offset */
  dim_offset = (interface == SDRAM_INTERFACE_0) ? DIM_0_OFFSET : DIM_1_OFFSET;

  /* Enable broadcast mode for DQ PHYs */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;
  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface, ddr_broadcast_mask);

  /* Initialize static settings of CA and DQ PHYs */
  ddr_set_config(dim_offset, ddr_caphy_config_base, ddr_caphy_config_delta);
  ddr_set_config(DQ_BCAST_OFFSET, ddr_dqphy_config_base, ddr_dqphy_config_delta);

  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), dim_offset,

                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), PN_SEL_CA) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), PN_SEL_DATA) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_USE_LAST) |
                 HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), BANDGAP_ENA),

                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_NOW) |
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_AUTO) |
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), IO_CAL_BANDGAP_DYN_CTRL) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), PN_SEL_CA) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), PN_SEL_DATA) |
                 0x0U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), CAL_USE_LAST) |
                 0x1U << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_IOC_CTLR_CFG), BANDGAP_ENA));

  /* Wait for 10us */
  HAL_SDRAM_DDR_Wait(10, 0);

  /* Issue manual IO Cal */
  HAL_SDRAM_PHY_Manual_IO_Cal(interface);

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
**  Function : HAL_SDRAM_Get_Training_Address
** ============================================================================
*/
/*!
*   @brief
*   This helper function finds the appropriate address for ddr to do training on.
*
*   @details
*   This helper function finds the appropriate address for ddr to do training on.
*   Taking into account for interleave status, interface, and chip select
*
*   @param interface_name              -  [IN] Interface to train on
*   @param chip_select                 -  [IN] chip select to train on

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

static uint32 HAL_SDRAM_Get_Training_Address(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select)
{
  uint32 test_address = DDR_TRAINING_TEST_ADDRESS_CS0;
  ddr_info ddr_configuration = ddr_get_info();
  if (chip_select == SDRAM_CS1)
  {
    test_address = ddr_configuration.ddr_size.sdram0_cs1_addr;
  }

  if (interface_name == SDRAM_INTERFACE_1)
  {
    if (chip_select == SDRAM_CS0)
    {
      if (ddr_configuration.interleaved_memory & SDRAM_CS0_INTERLEAVE)
      {
        test_address += 0x400;
      }
      else
      {
        test_address = ddr_configuration.ddr_size.sdram1_cs0_addr;
      }
    }
    else
    {
      if (ddr_configuration.interleaved_memory & SDRAM_CS1_INTERLEAVE)
      {
        test_address += 0x400;
      }
      else
      {
        test_address = ddr_configuration.ddr_size.sdram1_cs0_addr;
      }
    }
  }

  return test_address;
};

/* ============================================================================
**  Function : HAL_SDRAM_Scan_min_max
** ============================================================================
*/
/*!
*   @brief
*   This helper function finds
*
*   @details
*   The goal of read training is to maximize the eye opening on each DQ by removing the relative
*   static skew between read DQS and each DQ. This is achieved by adjusting programmable delay on
*   read DQS (T/4) and each individual DQ bit (per-bit deskew).
*
*   @param min_coarse             -  [IN] result minimum from histogram
*   @param max_coarse             -  [IN] result maximum from histogram
*   @param fail_count_histogram   -  [IN] input histogram to scan
*   @param histogram_size         -  [IN] input histogram size
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
static boolean HAL_SDRAM_Min_Max_Scan
(
  uint8 *min_coarse,
  uint8 *max_coarse,
  uint32 *fail_count_histogram,
  uint32 histogram_size
)
{
  int coarse_delay;

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  /* scan for min(a) 
   * Starting from coarse delay of 0 up until first passing value is found
   */
  for(coarse_delay = 0, *min_coarse = 0xF; coarse_delay <= histogram_size; coarse_delay++)
  {
   if(fail_count_histogram[coarse_delay] == 0)
   {
     *min_coarse = coarse_delay;
     break;
   }
  }

  /* scan for max(b) 
   * Starting from previously found minimum coarse delay up. This is to prevent
   * any holes in the histogram. E.g. (o is pass, x is fail)
   * xxoooooxoxxx 
   * in a histogram pattern like this we want to capture the 5 continuous passing
   * segment, scanning from the max downwards until first passing would result in 
   * incorrect max value.
   */
  for(coarse_delay = *min_coarse, *max_coarse = 0x0; coarse_delay <= histogram_size; coarse_delay++)
  {
   if(fail_count_histogram[coarse_delay] == 0)
   {
     *max_coarse = coarse_delay;
     break;
   }
  }

  if((*max_coarse) < (*min_coarse))
  {
    /* training has failed */
    return FALSE;
  }

  return TRUE;
}

/* ============================================================================
**  Function : HAL_SDRAM_CA_Training_Helper
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

static void HAL_SDRAM_CA_Training_Helper(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[16])
{

  uint32 offset;
  ddr_mask ddr_broadcast_mask = {0};
  uint32 loop_count;
  uint32 failures;
  uint32 data_dq0 = 0;
  uint32 data_dq1 = 0;
  uint32 mapping = 0;
  int32  coarse_delay;

  volatile uint8 pattern_raising;
  volatile uint8 pattern_falling;


#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }
  /* Enable broadcast mode on DQ0 and 1 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = FALSE;
  ddr_broadcast_mask.DQ03_PHY = FALSE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  do
  {
    /* Set CA training background pattern */
    /* Call BIMC function to set CA training pattern */
    HAL_SDRAM_BIMC_Set_CA_Training_Pattern(interface_name, chip_select, mapping);

    /*
     * Put DQ PHY in CA Training Mode
     * DDRPHY_DQ_TOP_CFG[CA_TRAINING_MODE] = 1
     */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), CA_TRAINING_MODE),
        1u << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), CA_TRAINING_MODE));

    /* Set initial CA CDC T/4 and per-bit deskew value
     * coarse delay = max
     * fine delay and per-bit deskew at middle point
     */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_DELAY_SEL), offset,
        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DELAY_SEL), CDC_DELAY_SEL), 0x0);

    /* Set per bit skew to middle point */
    BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_WR_DESKEW0), offset, 0x44444);
    BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_WR_DESKEW1), offset, 0x44444);

    /* Initialize coarse delay to 0xF, max delay */
    for(coarse_delay = 0xF; coarse_delay >= 0; coarse_delay--)
    {
      /* coarse delay is bit 6:3, fine delay is set to middle point*/
      BOOT_HWIO_OUTM(
          PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
          offset,
          HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
          (0x4 | (coarse_delay << 0x3)) << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));

      /* Set loop count and fail flag */
      failures = 0;
      for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
      {

        /* Trigger BIMC for ca pattern */
        HAL_SDRAM_BIMC_CA_Training_Toggle_Pattern(interface_name);

        /* Read Ca training status in DQ[0, 1]
         * DDRPHY_DQ_WRLVL_STATUS
         */

        data_dq0 = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS),
            offset + SDRAM_DQ0_OFFSET,
            HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS), WRLVL_STATUS));

        data_dq1 = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS),
            offset + SDRAM_DQ1_OFFSET,
            HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS), WRLVL_STATUS));

        /* reconstruct the pattern from the dq bits
         * CA to DQ mapping (CA training mode enabled by MR41)
         * CA0 CA1 CA2 CA3 CA5 CA6  CA7  CA8  CK edge
         * DQ7 DQ5 DQ3 DQ1 DQ8 DQ10 DQ12 DQ14 Rising
         * DQ6 DQ4 DQ2 DQ0 DQ9 DQ11 DQ13 DQ15 Falling
         *
         * Mapping 2, enabled by MR48
         * CA4 CA9       CK edge
         * DQ7 DQ8       Rising
         * DQ6 DQ9       Falling
         */
        if(mapping == 0)
        {
          if(data_dq0 != 0x35 ||
              data_dq1 !=0x8d)
          {
            failures++;
          }
        }
        else
        {
          if(data_dq0 != 0 ||
              data_dq1 !=0x2)
          {
            failures++;
          }
        }

      }

      /* record the number of failures in histogram */
      fail_count_histogram[coarse_delay] += failures;
    }

    mapping++;
  } while (mapping < 2);

  /* Toggle BIMC exit CA training */
  HAL_SDRAM_BIMC_Exit_CA_Training(interface_name, chip_select);


  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), DQ_BCAST_OFFSET,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), CA_TRAINING_MODE),
      0 << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), CA_TRAINING_MODE));
}
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
  uint32 fail_count_histogram [16] ={0};
  uint8  min_coarse = 7;
  uint8  max_coarse = 0;
  uint32 offset;

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  if(chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS0, fail_count_histogram);
  }
  if(chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS1, fail_count_histogram);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  if(HAL_SDRAM_Min_Max_Scan(&min_coarse, &max_coarse, fail_count_histogram, 0xF) != TRUE)
  {
    /* Unable to find MIN/MAX from histogram,
     * Training has failed
     */
    return FALSE;
  }
  else
  {
    /* Interpolate and update training parameters */

    /* Update the parameters with interpolated ca training coarse delay */
    *ca_trained_delay = ((max_coarse + min_coarse)/2);

    /* Apply the trained value */
    BOOT_HWIO_OUTM(
        PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
        offset,
        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
        (0x4 | (*ca_trained_delay) << 0x3) << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));

    return TRUE;
  }
}

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Read_Write_Leveling_Status
** ============================================================================
*/
/*!
*   @brief
*   This helper function reads the write leveling status and returns the results.
*
*   @details
*   This helper function reads the write leveling status and returns the results.
*
*   @param interface_offset   -  [IN] The offset for interface compared to CH0_DQ0
*   @param dq_offset          -  [IN] dq offset compared to DQ0
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
static void HAL_SDRAM_PHY_Read_Write_Leveling_Status
(
  uint32 interface_offset,
  uint32 dq_offset,
  uint32 *one_count,
  uint32 *zero_count
)
{
  /* Read Write Leveling Status */
  if (BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS),
      interface_offset + dq_offset,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_STATUS), WRLVL_STATUS))&0x1)
  {
    (*one_count)++;
  }
  else
  {
    (*zero_count)++;
  }
}

/* ============================================================================
**  Function : HAL_SDRAM_PHY_Write_Leveling_Pulse_And_Read
** ============================================================================
*/
/*!
*   @brief
*   This helper function triggers for a pulse and reads back the status
*
*   @details
*   This helper function triggers for a pulse and reads back the status
*
*   @param offset              -  [IN] The offset for interface compared
*   @param send_pulse          -  [IN] 0 or 1 to send the pulse
*   @param zero_count          -  [IN] count the number of zeros
*   @param ono_count           -  [IN] count the number of ones
*
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
static void HAL_SDRAM_PHY_Write_Leveling_Pulse_And_Read
(
  uint32 offset,
  uint32 send_pulse,
  uint32 *zero_count,
  uint32 *one_count
)
{
  /* Send write leveling pulse */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_TRIGGER), DQ_BCAST_OFFSET,
           HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_TRIGGER), SEND_PULSE),
           send_pulse << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_TRIGGER), SEND_PULSE));

  /* Read Write Leveling Status */
  HAL_SDRAM_PHY_Read_Write_Leveling_Status(offset, SDRAM_DQ0_OFFSET,
      &one_count[0], &zero_count[0]);
  HAL_SDRAM_PHY_Read_Write_Leveling_Status(offset, SDRAM_DQ1_OFFSET,
      &one_count[1], &zero_count[1]);
  HAL_SDRAM_PHY_Read_Write_Leveling_Status(offset, SDRAM_DQ2_OFFSET,
      &one_count[2], &zero_count[2]);
  HAL_SDRAM_PHY_Read_Write_Leveling_Status(offset, SDRAM_DQ3_OFFSET,
      &one_count[3], &zero_count[3]);
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
  uint32 offset;
  uint32 loop_count;
  uint32 one_count[4] = {0};
  uint32 zero_count[4] = {0};
  uint32 write_level_delay = 0;
  ddr_mask ddr_broadcast_mask = {0};
  boolean training_result = FALSE;
  struct ddr_trained_params_lpddr3 *training_params;

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Get DDR Training parameters */
  training_params = &(ddr_get_trained_params(interface_name)->lpddr3_training);

  /* training parameter not found */
  if(training_params == NULL)
  {
    return FALSE;
  }
  
  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  /* Enable broadcast mode on DQ0 and 1 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* Set DRAM to write leveling mode
   * We assume RL/WL of 12/9 because we have to do training at 800mhz, and that
   * is the JEDEC spec for latency at that speed.*/

  HAL_SDRAM_Write_MR(interface_name, chip_select, 0x2, 0x8A);

  /* Set DQ write CDC delay to T/4
   * DDRPHY_DQ_DELAY_SEL[WR_CDC_DELAY_SEL] = 0
   * DDRPHY_DQ_WR_CDC_DELAY0 <= <value>
   */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), DQ_BCAST_OFFSET,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), WR_CDC_DELAY_SEL), 0x0);

  /* if first pass training, use mid point on DELAY0,
   * else do not touch DELAY0 as it has been trained already
   */
  if(first_pass)
  {
    BOOT_HWIO_OUTM(
            PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
            DQ_BCAST_OFFSET,
            HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
            0x24 << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
  }

  /* Put DQ PHY in write leveling mode
   * DDRPHY_DQ_TOP_CFG[WR_LEVELING_MODE] = 1
   */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), DQ_BCAST_OFFSET,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE),
      0x1 << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE));

  do
  {
    /* Initialize one_count and zero_count to 0 */
    one_count[0] = one_count[1] =  one_count[2] = one_count[3] = 0;
    zero_count[0] = zero_count[1] =  zero_count[2] = zero_count[3] = 0;


    /* Set initial write leveling delay value
     * DDRPHY_DQ_WRLVL_DELAY[WRLVL_DELAY] = 0
     */
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY),
        DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY),
        write_level_delay << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));

    /*Send first write leveling pulse */
    BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_TRIGGER), DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_TRIGGER), SEND_PULSE), 0x0);

    HAL_SDRAM_PHY_Write_Leveling_Pulse_And_Read(offset, 0x1, zero_count, one_count);

    for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
    {
      /* Send write leveling pulse */
      HAL_SDRAM_PHY_Write_Leveling_Pulse_And_Read(offset, 0x0, zero_count, one_count);

      /* Send write leveling pulse */
      HAL_SDRAM_PHY_Write_Leveling_Pulse_And_Read(offset, 0x1, zero_count, one_count);
    }

    /* update write leveling delay value */
    write_level_delay++;

    /* Check to see if any of the phy is already finished calibration,
     * if one count is greater than zero count, take them off the calibration
     * broadcast.
     */
    ddr_broadcast_mask.DQ00_PHY =
        ddr_broadcast_mask.DQ00_PHY ?
            (one_count[0] <= zero_count[0]) : ddr_broadcast_mask.DQ00_PHY;
    ddr_broadcast_mask.DQ01_PHY =
            ddr_broadcast_mask.DQ01_PHY ?
                (one_count[1] <= zero_count[1]) : ddr_broadcast_mask.DQ01_PHY;
    ddr_broadcast_mask.DQ02_PHY =
            ddr_broadcast_mask.DQ02_PHY ?
                (one_count[2] <= zero_count[2]) : ddr_broadcast_mask.DQ02_PHY;
    ddr_broadcast_mask.DQ03_PHY =
            ddr_broadcast_mask.DQ03_PHY ?
                (one_count[3] <= zero_count[3]) : ddr_broadcast_mask.DQ03_PHY;

    HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  }while( (ddr_broadcast_mask.DQ00_PHY ||
           ddr_broadcast_mask.DQ01_PHY ||
           ddr_broadcast_mask.DQ02_PHY ||
           ddr_broadcast_mask.DQ03_PHY)
           && write_level_delay <= 127);

  /* if all broadcast mode were turned off that means all DQ is trained
   * otherwise we have completed all 127 iterations without finding a good */
  training_result = (ddr_broadcast_mask.DQ00_PHY == FALSE
                  && ddr_broadcast_mask.DQ01_PHY == FALSE
                  && ddr_broadcast_mask.DQ02_PHY == FALSE
                  && ddr_broadcast_mask.DQ03_PHY == FALSE);


  /* Re-enable broadcast for all DQs */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;

  /* Exit DQ DDR PHY write leveling mode */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), DQ_BCAST_OFFSET,
           HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE),
           0x0 << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE));

  /* store the trained value in shared memory */
  training_params->wrlvl_delay[0] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ0_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  training_params->wrlvl_delay[1] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ1_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  training_params->wrlvl_delay[2] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ2_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  training_params->wrlvl_delay[3] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ3_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  return training_result;
}

/* ============================================================================
**  Function : HAL_SDRAM_Read_Training_Helper
** ============================================================================
*/
/*!
*   @brief
*   This helper function trains the read on DQ-DQS
*
*   @details
*   The goal of read training is to maximize the eye opening on each DQ by removing the relative
*   static skew between read DQS and each DQ. This is achieved by adjusting programmable delay on
*   read DQS (T/4) and each individual DQ bit (per-bit deskew).
*
*   @param interface_name         -  [IN] The interface to training ca training for
*   @param interface_name         -  [IN] The chip select ti train ca training for
*   @param fail_count_histogram   -  [IN] The histogram to train to
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

static void HAL_SDRAM_Read_Training_Helper(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[4][16])
{
    ddr_mask ddr_broadcast_mask = {0};
    uint32 loop_count;
    uint32 byte_counter;
    int32  coarse_delay;
    uint32 read_mask;
    volatile uint32 * test_address = (uint32 *) HAL_SDRAM_Get_Training_Address(
      interface_name, chip_select);

    if(interface_name == SDRAM_INTERFACE_1)
    {
      test_address += 0x400;
    }

  #if (defined(FEATURE_PRESILICON_DEV))
    /* DDR PHY only available on Silicon */
    return;
  #endif

    /* Enable broadcast mode on DQ0 and 1 */
    ddr_broadcast_mask.DQ00_PHY = TRUE;
    ddr_broadcast_mask.DQ01_PHY = TRUE;
    ddr_broadcast_mask.DQ02_PHY = TRUE;
    ddr_broadcast_mask.DQ03_PHY = TRUE;

    HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

    /* Set DQ WR CDC and Read fifo clock delay value
     * DDRPHY_DQ_DELAY_SEL[WR_CDC_DELAY_SEL] = 0
     * DDRPHY_DQ_DELAY_SEL[RD_CLK_DELAY_SEL] = 0
     * DDRPHY_DQ_WR_CDC_DELAY0 = <value>
     * DDRPHY_DQ_RD_CLK_DELAY0 = <value>
     * DDRPHY_DQ_WR_DESKEW_DQ = 0x0
     * DDRPHY_DQ_WR_DESKEW_DM = 0x0 */

    BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), WR_CDC_DELAY_SEL) |
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), RD_CLK_DELAY_SEL) ,
        0);

    BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_WR_DESKEW_DQ), DQ_BCAST_OFFSET, 0);
    BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_WR_DESKEW_DM), DQ_BCAST_OFFSET, 0);


    /* Set initial DQ CDC t/5 and per-bit deskew value
     * coarse delay = 0
     * fine delay and per-bit deskew at middle point
     * DDRPHY_DQ_DELAY_SEL[RD_CDC_DELAY_SEL] = 0
     * DDRPHY_DQ_RD_CDC_DELAY0[6:3] = 0x0
     * DDRPHY_DQ_RD_CDC_DELAY0[3:0] = 0x4
     * DDRPHY_DQ_RD_DESKEW_DQ_EVEN = 0x44444444
     * DDRPHY_DQ_RD_DESKEW_DQ_ODD = 0x444444444
     */

    BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), RD_CDC_DELAY_SEL), 0);

    BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_RD_DESKEW_DQ_EVEN), DQ_BCAST_OFFSET,
        0x44444444);
    BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_RD_DESKEW_DQ_ODD), DQ_BCAST_OFFSET,
        0x44444444);

    /* Memory controller writes data pattern into DRAM */
    *test_address = DDR_DQ_READ_TRAINING_PATTERN;

    /* Initialize coarse delay to 0x7, max delay */
    for(coarse_delay = 0xF; coarse_delay >= 0; coarse_delay--)
    {
      /* coarse delay is bit 6:3, fine delay is set to middle point*/
      BOOT_HWIO_OUTM(
          PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
          DQ_BCAST_OFFSET,
          HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
          (0x4 | (coarse_delay << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));

      /* Set loop count and fail flag */
      for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
      {
        /* for each byte, data == pattern? */
        /* error_count[byte] = error_count[byte]+1 */
        for(byte_counter = 0, read_mask = 0xFF; byte_counter < 4; byte_counter++, read_mask<<=8)
        {
          if(((*test_address)&read_mask) != (DDR_DQ_READ_TRAINING_PATTERN&read_mask))
          {
            /* failure detected for byte, store in histogram */
            fail_count_histogram[byte_counter][coarse_delay]++;
          }
        }
      }
    }
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
  uint32 byte_counter;
  uint32 fail_count_histogram [4][16] = {0}; // [byte][coarse_delay]
  uint8 min_coarse = 0xF;
  uint8 max_coarse = 0x0;
  uint32 offset;

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  if(chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS0, fail_count_histogram);
  }
  if(chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS1, fail_count_histogram);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  /* scan for min(a) */
  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if(HAL_SDRAM_Min_Max_Scan(&min_coarse, &max_coarse, fail_count_histogram[byte_counter], 0xF))
    {
      read_training[byte_counter] = (max_coarse + min_coarse) / 2;

    }
    else
    {
      return FALSE;
      /* unable to find min and max, training has failed */
    }
  }

  /* apply the trained values */

  for(byte_counter =0; byte_counter < 4; byte_counter++)
  {
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
        (offset + sizeof(uint32) * byte_counter),
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
        (0x4 | ( read_training[byte_counter] << 3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));
  }


  return TRUE;
}

/* ============================================================================
**  Function : HAL_SDRAM_Write_Training_Helper
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
*   @param interface_name       -  [IN] The interface to training write training for
*   @param chip_select          -  [IN] The chip select to train write training for
*   @param fail_count_histogram -  [IN] The chip select to train write training for
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

static void HAL_SDRAM_Write_Training_Helper(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[4][16])
{
  ddr_mask ddr_broadcast_mask = {0};
  uint32 loop_count;
  uint32 byte_counter;
  int32  coarse_delay;
  uint32 read_mask;
  volatile uint32 * test_address = (uint32 *)HAL_SDRAM_Get_Training_Address(interface_name, chip_select);
  uint32 test_pattern = DDR_DQ_READ_TRAINING_PATTERN;

  /* Enable broadcast mode on DQ0 and 1 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* Set DQ WR CDC and Read fifo clock delay value
   * DDRPHY_DQ_DELAY_SEL[WR_CDC_DELAY_SEL] = 0
   * DDRPHY_DQ_DELAY_SEL[RD_CLK_DELAY_SEL] = 0
   * DDRPHY_DQ_WR_CDC_DELAY0 = <value>
   * DDRPHY_DQ_RD_CLK_DELAY0 = <value>
   * DDRPHY_DQ_WR_DESKEW_DQ = 0x0
   * DDRPHY_DQ_WR_DESKEW_DM = 0x0 */

  BOOT_HWIO_OUTM(
      PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL),
      DQ_BCAST_OFFSET,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), WR_CDC_DELAY_SEL) |
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), RD_CLK_DELAY_SEL),
      0);

  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_WR_DESKEW_DQ), DQ_BCAST_OFFSET, 0);
  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_WR_DESKEW_DM), DQ_BCAST_OFFSET, 0);


  /* Set initial DQ CDC t/5 and per-bit deskew value
   * coarse delay = 0
   * fine delay and per-bit deskew at middle point
   * DDRPHY_DQ_DELAY_SEL[RD_CDC_DELAY_SEL] = 0
   * DDRPHY_DQ_RD_CDC_DELAY0[6:3] = 0x0
   * DDRPHY_DQ_RD_CDC_DELAY0[3:0] = 0x4
   * DDRPHY_DQ_RD_DESKEW_DQ_EVEN = 0x44444444
   * DDRPHY_DQ_RD_DESKEW_DQ_ODD = 0x444444444
   */

  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), DQ_BCAST_OFFSET,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DELAY_SEL), RD_CDC_DELAY_SEL) ,
      0);

  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_RD_DESKEW_DQ_EVEN), DQ_BCAST_OFFSET, 0x44444444);
  BOOT_HWIO_OUT(PHY_DQ_ADDR(DDRPHY_DQ_RD_DESKEW_DQ_ODD), DQ_BCAST_OFFSET, 0x44444444);

  /* Initialize coarse delay to 0xF, max delay */
  for(coarse_delay = 0xF; coarse_delay >= 0; coarse_delay--)
  {
    /* coarse delay is bit 6:3, fine delay is set to middle point*/
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
        DQ_BCAST_OFFSET,
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
        (0x4 | (coarse_delay << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));

    /* Set loop count and fail flag */
    for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
    {
      /* Memory controller writes data pattern into DRAM */
      *test_address = test_pattern;

      /* for each byte, data == pattern? */
      /* error_count[byte] = error_count[byte]+1 */
      for(byte_counter = 0, read_mask = 0xFF; byte_counter < 4; byte_counter++, read_mask<<=8)
      {
        if(((*test_address)&read_mask) != (test_pattern&read_mask))
        {
          /* failure detected for byte, store in histogram */
          fail_count_histogram[byte_counter][coarse_delay]++;
        }
      }

      /* invert the test pattern for the next cycle */
      test_pattern = ~test_pattern;
    }


  }
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
  uint32 byte_counter;
  uint32 fail_count_histogram [4][16] = {0}; // [byte][coarse_delay]
  uint8 min_coarse = 0x7;
  uint8 max_coarse = 0x0;
  uint32 offset;

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  if(chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS0, fail_count_histogram);
  }
  if(chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS1, fail_count_histogram);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if(HAL_SDRAM_Min_Max_Scan(&min_coarse, &max_coarse, fail_count_histogram[byte_counter], 0xF))
    {
      /* Interpolate and update training parameters */
      write_training[byte_counter] = (max_coarse + min_coarse) / 2;

    }
    else
    {
      /* unable to find min and max from histogram training has failed */
      return FALSE;
    }
  }

  /* Apply the trained value */

  for(byte_counter =0; byte_counter < 4; byte_counter++)
  {
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
        (offset + sizeof(uint32) * byte_counter),
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
        (0x4 | ( write_training[byte_counter] << 3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
  }

  return TRUE;
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
  uint32 dim_offset;
  ddr_mask ddr_broadcast_mask = {0};

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    dim_offset = DIM_0_OFFSET;
  }
  else
  {
    dim_offset = DIM_1_OFFSET;
  }

  /* Enable broadcast mode on DQ0,1,2,3 */
  ddr_broadcast_mask.DQ00_PHY = TRUE;
  ddr_broadcast_mask.DQ01_PHY = TRUE;
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* Bypass DCC0 : No */
  /* Manual Adjust DCC0 : No */

  /* Write Recommended value to register
   * DCC0_CFG[DCC_WAIT_TIME]
   * DCC0_CFG[DCC_DIV_REF]
   * Note: this is filled by one time settings
   */

  /* Trigger DCC Calibration
   * DCC0_CFG[DCC_CONTROL_START]
   */
  BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG),
      dim_offset + dcc_offset,
      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), DCC_CONTROL_START),
      1u << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_DCC0_CFG), DCC_CONTROL_START));

  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_DCC0_CFG),
      DQ_BCAST_OFFSET + dcc_offset,
      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_DCC0_CFG), DCC_CONTROL_START),
      1u << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_DCC0_CFG), DCC_CONTROL_START));

  /* Check for DCC0 calibration status */

  while(BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                      dim_offset,
                      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                          DCC0_DCC_IN_PROGRESS)));

  while(BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                      DQ_BCAST_OFFSET,
                      HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                          DCC0_DCC_IN_PROGRESS)));

  /* SW override slave dcc0 adjustment : No */

  return TRUE;
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
  uint32 major_rev = 0;

  major_rev = BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_HW_INFO),
                            0x0,
                            HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_HW_INFO), MAJOR_REV));

  major_rev >>= HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_HW_INFO), MAJOR_REV);

  return major_rev;
}
