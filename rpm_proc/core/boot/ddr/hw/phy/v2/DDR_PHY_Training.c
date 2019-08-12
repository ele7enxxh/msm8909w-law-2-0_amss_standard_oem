/*
===========================================================================

FILE:         DDR_PHY_v2.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  PHY Gen 2 controller.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/phy/v2/DDR_PHY_Training.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/02/13   tw      Added bitfield mask for CA training patterns
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

#define DQ_TRAINING_PATTERN_1_SIZE 24
uint32 dq_training_pattern_1[] =
{
/* Tom's Pattern */
0xDEADBEEF,
0x5A5A5A5A,
0xA5A5A5A5,
0xFEEDFACE,
0xCAFEBABE,
0xA5A5A5A5,
0x5A5A5A5A,
0x0BADF00D,

/* Yong's Pattern */
0xa5a5a5a5,
0x5a5a5a5a,
0xa5a5a5a5,
0x5a5a5a5a,
0x0f0f0f0f,
0xf0f0f0f0,
0x00000000,
0xffffffff,

/* Mike/Scott's Pattern */
0xFFFFFFFF,
0x00000000,
0xFFFFFFFF,
0xFFFFFFFF,
0x00000000,
0xFFFFFFFF,
0x00000000,
0x00000000

};


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

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
**  Function : HAL_SDRAM_Apply_Phy_Training_Pattern
** ============================================================================
*/
/*!
*   @brief
*   This function sets the training pattern for the given interface and chip select
*
*   @details
*   This function sets the training pattern for the given interface and chip select
*
*   @param interface_name             -  [IN] interface to set pattern for
*   @param chip_select                -  [IN] chip select to set pattern for
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
void HAL_SDRAM_Apply_Phy_Training_Pattern(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, boolean check_result)
{
  volatile uint32 * test_address;
  uint32 pattern_offset;
  
  if(chip_select & SDRAM_CS0)
  {
    test_address = (uint32 *) HAL_SDRAM_Get_Training_Address(interface_name, SDRAM_CS0);
    
    /* Memory controller writes data pattern into DRAM */
    for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
    {
      test_address[pattern_offset] = dq_training_pattern_1[pattern_offset];
    }
	
	if(check_result)
	{
		/* Sanity check to make sure pattern was written as we expected */
		for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
		{
		  if(test_address[pattern_offset] != dq_training_pattern_1[pattern_offset])
		  {
			while(1){}
		  }
		}	
	}
  }
  if(chip_select & SDRAM_CS1)
  {
    test_address = (uint32 *) HAL_SDRAM_Get_Training_Address(interface_name, SDRAM_CS1);
    
    /* Memory controller writes data pattern into DRAM */
    for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
    {
      test_address[pattern_offset] = dq_training_pattern_1[pattern_offset];
    }

	if(check_result)
	{
		/* Sanity check to make sure pattern was written as we expected */
		for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
		{
		  if(test_address[pattern_offset] != dq_training_pattern_1[pattern_offset])
		  {
			while(1){}
		  }
		}	
	}
  } 
}

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
     
   }
   else
   {
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
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[16],
    boolean fine_training, uint8 min, uint8 max)
{

  uint32 offset;
  ddr_mask ddr_broadcast_mask = { 0 };
  uint32 loop_count;
  uint32 failures;
  volatile uint32 data_dq0 = 0;
  volatile uint32 data_dq1 = 0;
  uint32 mapping = 0;
  int32  delay;

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
  ddr_broadcast_mask.DQ02_PHY = TRUE;
  ddr_broadcast_mask.DQ03_PHY = TRUE;

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
     * [6:3] coarse delay
     * [2:0] fine delay
     */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(DDRPHY_CA_DELAY_SEL), offset,
        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DELAY_SEL), CDC_DELAY_SEL), 0x0);

    /* Set per bit skew to middle point */
    BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_WR_DESKEW0), offset, 0x44444);
    BOOT_HWIO_OUT(PHY_CA_ADDR(DDRPHY_CA_WR_DESKEW1), offset, 0x44444);

    /* Initialize coarse/fine delay to 0xF or 0x7, max delay */
    for(delay = fine_training ? 0x7 : 0xF; delay >= 0; delay--)
    {
      /* coarse delay is bit 6:3, fine delay is set to middle point*/
      if(fine_training)
      {
        /* if fine delay is 0x5 or above, use max trained coarse delay,
         * otherwise use min trained coarse delay
         */
        BOOT_HWIO_OUTM(
            PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
            offset,
            HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
            (delay | (((delay >= 0x5) ? max : min) << 0x3)) << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));
      }
      else
      {
      BOOT_HWIO_OUTM(
          PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
          offset,
          HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
          (0x4 | (delay << 0x3)) << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));
      }
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
          if( ((data_dq0 & 0xC0) != 0) ||
              ((data_dq1 & 0x3) != 0x2) )
          {
            failures++;
          }
        }

      }

      /* record the number of failures in histogram */
      fail_count_histogram[delay] += failures;
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

boolean HAL_SDRAM_CA_Training(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 * ca_trained_delay_coarse,
    uint32 * ca_trained_delay_fine)
{
  uint32 fail_count_histogram_coarse [16] ={0};
  uint32 fail_count_histogram_fine [16] ={0};
  uint8  min_coarse = 0xF;
  uint8  max_coarse = 0;
  uint8  min_fine = 0x7;
  uint8  max_fine = 0;
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
    HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS0, fail_count_histogram_coarse, FALSE, 0, 0);
  }
  if(chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS1, fail_count_histogram_coarse, FALSE, 0, 0);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  if(HAL_SDRAM_Min_Max_Scan(&min_coarse, &max_coarse, fail_count_histogram_coarse, 0xF) != TRUE)
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
    *ca_trained_delay_coarse = ((max_coarse + min_coarse)/2);

    /* Apply Fine training */
    if(chip_select & SDRAM_CS0)
    {
      HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS0, fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
    }
    if(chip_select & SDRAM_CS1)
    {
      HAL_SDRAM_CA_Training_Helper(interface_name, SDRAM_CS1, fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
    }

    /* SW process histogram for fine training */
    if(HAL_SDRAM_Min_Max_Scan(&min_fine, &max_fine, fail_count_histogram_fine, 0x7) != TRUE)
    {
      /* Unable to find MIN/MAX from histogram,
       * Training has failed
       */
      return FALSE;
    }

    /* Update the parameters with interpolated ca training coarse delay */
    /* DDRPHY_CA_CDC_DELAY0[2:0] = int((c+d)/2) if (a+b) is even
     * DDRPHY_CA_CDC_DELAY0[2:0] = int((c+d)/2) + 2 if (a+b) is odd
     */
    *ca_trained_delay_fine = ((max_fine + min_fine) / 2);
    *ca_trained_delay_fine += ((min_coarse + max_coarse) % 2 == 0) ? 0 : 2;

    /* Apply the trained value */
    BOOT_HWIO_OUTM(
        PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0),
        offset,
        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE),
        (*ca_trained_delay_fine | (*ca_trained_delay_coarse) << 0x3)
        << HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_CDC_DELAY0), DELAY_VALUE));

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

boolean HAL_SDRAM_Write_Leveling_Helper(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 *result)
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

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);
  
  /* Exit DQ DDR PHY write leveling mode */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), DQ_BCAST_OFFSET,
           HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE),
           0x0 << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_TOP_CFG), WR_LEVELING_MODE));

  /* restore MR2 value */
  HAL_SDRAM_Write_MR(interface_name, chip_select, 0x2, 0xA);
  
  /* store the trained value in shared memory */
  result[0] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ0_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  result[1] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ1_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  result[2] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ2_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  result[3] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ3_OFFSET,
                                                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  //training_params->wrlvl_delay[0] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ0_OFFSET,
  //                                                HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  //training_params->wrlvl_delay[1] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ1_OFFSET,
  //                                                HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  //training_params->wrlvl_delay[2] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ2_OFFSET,
  //                                                HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  //training_params->wrlvl_delay[3] = BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), offset + SDRAM_DQ3_OFFSET,
  //                                                HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WRLVL_DELAY), WRLVL_DELAY));
  return training_result;
}

boolean HAL_SDRAM_Write_Leveling(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  uint32 result_cs0[4];
  uint32 result_cs1[4];
  
  if(chip_select & SDRAM_CS0)
  {
    if(!HAL_SDRAM_Write_Leveling_Helper(interface_name, SDRAM_CS0, &result_cs0[0]))
    {
      return FALSE;
    }
  }
  if(chip_select & SDRAM_CS1)
  {
    if(!HAL_SDRAM_Write_Leveling_Helper(interface_name, SDRAM_CS0, &result_cs1[0]))
    {
      return FALSE;
    }
  }
  
  return TRUE;
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
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[4][16],
    boolean fine_training, uint8 min[4], uint8 max[4])
{
    ddr_mask ddr_broadcast_mask = {0};
    uint32 loop_count;
    uint32 byte_counter;
    int32  delay;
    uint32 pattern_offset;
    uint32 read_mask;
    uint32 offset;
    volatile uint32 * test_address = (uint32 *) HAL_SDRAM_Get_Training_Address(
      interface_name, chip_select);

    /* Configure for correct offset */
    if(interface_name == SDRAM_INTERFACE_0)
    {
      offset = DIM_0_OFFSET;
    }
    else
    {
      offset = DIM_1_OFFSET;
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
    
    /* Initialize coarse/fine delay to 0xF/0x7 max delay */
    for(delay = fine_training ? 0x7 : 0xF; delay >= 0; delay--)
    {
      if(fine_training)
      {
        for(byte_counter =0; byte_counter < 4; byte_counter++)
        {
          BOOT_HWIO_OUTM(
              PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
              (offset + SDRAM_DQ1_OFFSET * byte_counter),
              HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
              (delay | (((delay >= 0x5) ? max[byte_counter] : min[byte_counter]) << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));
        }
      }
      else
      {
        BOOT_HWIO_OUTM(
                  PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
                  DQ_BCAST_OFFSET,
                  HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
                  (0x4 | (delay << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));
      }


      /* Set loop count and fail flag */
      for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
      {
        for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
          {
          /* for each byte, data == pattern? */
          /* error_count[byte] = error_count[byte]+1 */
          for(byte_counter = 0, read_mask = 0xFF; byte_counter < 4; byte_counter++, read_mask<<=8)
          {
            if(((test_address[pattern_offset])&read_mask) != (dq_training_pattern_1[pattern_offset]&read_mask))
            {
              /* failure detected for byte, store in histogram */
              fail_count_histogram[byte_counter][delay]++;
            }
          }
        }
        dog_kick();
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

boolean HAL_SDRAM_Read_Training(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 * read_training_coarse,
    uint32 * read_training_fine)
{
  uint32 byte_counter;
  uint32 fail_count_histogram_coarse [4][16] = {0}; // [byte][coarse_delay]
  uint32 fail_count_histogram_fine [4][16] = {0}; // [byte][fine_delay]
  uint8 min_coarse[4] = {0};
  uint8 max_coarse[4] = {0};
  uint8 min_fine[4] = {0};
  uint8 max_fine[4] = {0};
  uint32 offset;

  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    min_coarse[byte_counter] = 0xF;
    min_fine[byte_counter] = 0x7;
  }

#if (defined(FEATURE_PRESILICON_DEV))
  /* DDR PHY only available on Silicon */
  return;
#endif

  /* Configure for correct offset */
  if (interface_name == SDRAM_INTERFACE_0)
  {
    offset = DIM_0_OFFSET;
  }
  else
  {
    offset = DIM_1_OFFSET;
  }

  if (chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS0,
        fail_count_histogram_coarse, FALSE, min_coarse, max_coarse);
  }
  if (chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS1,
        fail_count_histogram_coarse, FALSE, min_coarse, max_coarse);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  /* scan for min(a) */
  for (byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if (HAL_SDRAM_Min_Max_Scan(&min_coarse[byte_counter],
        &max_coarse[byte_counter], fail_count_histogram_coarse[byte_counter],
        0xF))
    {
      read_training_coarse[byte_counter] = (max_coarse[byte_counter]
          + min_coarse[byte_counter]) / 2;
    }
    else
    {
      return FALSE;
      /* unable to find min and max, training has failed */
    }
  }

  /* Perform Fine Delay Training */
  if (chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS0,
        fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
  }
  if (chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Read_Training_Helper(interface_name, SDRAM_CS1,
        fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
  }

  /* Scan for fine delay */
  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if (HAL_SDRAM_Min_Max_Scan(&min_fine[byte_counter], &max_fine[byte_counter],
        fail_count_histogram_fine[byte_counter], 0x7))
    {
      /* Update the parameters with interpolated ca training coarse delay */
      /* DDRPHY_DQ_RD_CDC_DELAY0[2:0] = int ( (c+d)/2 ) if (a+b) is even
       * DDRPHY_DQ_RD_CDC_DELAY0[2:0] = int ( (c+d)/2 ) + 2 if (a+b) is odd
       */
      read_training_fine[byte_counter] = ((max_fine[byte_counter] + min_fine[byte_counter]) / 2);
      read_training_fine[byte_counter] += ((min_coarse[byte_counter] + max_coarse[byte_counter]) % 2 == 0) ? 0 : 2;
    }
    else
    {
      return FALSE;
      /* unable to find min and max, training has failed */
    }
  }

  /* apply the trained values */

  for (byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0),
        (offset + SDRAM_DQ1_OFFSET * byte_counter),
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE),
        (read_training_fine[byte_counter] | ( read_training_coarse[byte_counter] << 3))
        << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_RD_CDC_DELAY0), DELAY_VALUE));
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
    SDRAM_CHIPSELECT chip_select, uint32 fail_count_histogram[4][16],
    boolean fine_training, uint8 min[4], uint8 max[4])
{
  ddr_mask ddr_broadcast_mask = {0};
  uint32 loop_count;
  uint32 byte_counter;
  int32  delay;
  uint32 read_mask;
  uint32 pattern_offset;
  volatile uint32 * test_address = (uint32 *)HAL_SDRAM_Get_Training_Address(interface_name, chip_select);
  uint32 offset;

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

  /* Initialize coarse/fine delay to 0xF/0x7 max delay */
  for(delay = fine_training ? 0x7 : 0xF; delay >= 0; delay--)
  {
    if(fine_training)
    {
      for(byte_counter =0; byte_counter < 4; byte_counter++)
      {
        BOOT_HWIO_OUTM(
            PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
            (offset + SDRAM_DQ1_OFFSET * byte_counter),
            HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
            (delay | (((delay >= 0x5) ? max[byte_counter] : min[byte_counter]) << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
      }
    }
    else
    {
      /* coarse delay is bit 6:3, fine delay is set to middle point*/
      BOOT_HWIO_OUTM(
          PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
          DQ_BCAST_OFFSET,
          HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
          (0x4 | (delay << 0x3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
    }
    /* Set loop count and fail flag */
    for (loop_count = DDR_LOOP_COUNT_VALUE; loop_count > 0; loop_count--)
    {
      /* Memory controller writes data pattern into DRAM */
      HAL_SDRAM_Apply_Phy_Training_Pattern(interface_name, chip_select, FALSE);
      
      for(pattern_offset = 0; pattern_offset < DQ_TRAINING_PATTERN_1_SIZE; pattern_offset++)
      {
        /* for each byte, data == pattern? */
        /* error_count[byte] = error_count[byte]+1 */
        for(byte_counter = 0, read_mask = 0xFF; byte_counter < 4; byte_counter++, read_mask<<=8)
        {
          if(((test_address[pattern_offset])&read_mask) != (dq_training_pattern_1[pattern_offset]&read_mask))
          {
            /* failure detected for byte, store in histogram */
            fail_count_histogram[byte_counter][delay]++;
          }
        }
        
        /* invert the pattern for the next round of training */
        dq_training_pattern_1[pattern_offset] = ~dq_training_pattern_1[pattern_offset];
      }    
    dog_kick();
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

boolean HAL_SDRAM_Write_Training(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select, uint32 *write_training_coarse,
    uint32 *write_training_fine)
{
  uint32 byte_counter;
  uint32 fail_count_histogram_coarse [4][16] = {0}; // [byte][coarse_delay]
  uint32 fail_count_histogram_fine [4][16] = {0}; // [byte][fine_delay]
  uint8 min_coarse[4] = {0};
  uint8 max_coarse[4] = {0};
  uint8 min_fine[4] = {0};
  uint8 max_fine[4] = {0};
  uint32 offset;

  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    min_coarse[byte_counter] = 0xF;
    min_fine[byte_counter] = 0x7;
  }

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

  if (chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS0,
        fail_count_histogram_coarse, FALSE, min_coarse, max_coarse);
  }
  if (chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS1,
        fail_count_histogram_coarse, FALSE, min_coarse, max_coarse);
  }

  /* SW process histogram, find out the overlapping region
   * find the overlapping results from the two mappings
   * and interpolate delay for other speeds.
   * */

  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if (HAL_SDRAM_Min_Max_Scan(&min_coarse[byte_counter],
        &max_coarse[byte_counter], fail_count_histogram_coarse[byte_counter],
        0xF))
    {
      /* Interpolate and update training parameters */
      write_training_coarse[byte_counter] = (max_coarse[byte_counter] + min_coarse[byte_counter]) / 2;

    }
    else
    {
      /* unable to find min and max from histogram training has failed */
      return FALSE;
    }
  }

  /* Perform Fine Delay Training */
  if (chip_select & SDRAM_CS0)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS0,
        fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
  }
  if (chip_select & SDRAM_CS1)
  {
    HAL_SDRAM_Write_Training_Helper(interface_name, SDRAM_CS1,
        fail_count_histogram_fine, TRUE, min_coarse, max_coarse);
  }

  /* Scan for fine delay */
  for(byte_counter = 0; byte_counter < 4; byte_counter++)
  {
    if (HAL_SDRAM_Min_Max_Scan(&min_fine[byte_counter], &max_fine[byte_counter],
        fail_count_histogram_fine[byte_counter], 0x7))
    {
      /* Update the parameters with interpolated ca training coarse delay */
      /* DDRPHY_DQ_RD_CDC_DELAY0[2:0] = int ( (c+d)/2 ) if (a+b) is even
       * DDRPHY_DQ_RD_CDC_DELAY0[2:0] = int ( (c+d)/2 ) + 2 if (a+b) is odd
       */
      write_training_fine[byte_counter] = ((max_fine[byte_counter] + min_fine[byte_counter]) / 2);
      write_training_fine[byte_counter] += ((min_coarse[byte_counter] + max_coarse[byte_counter]) % 2 == 0) ? 0 : 2;
    }
    else
    {
      return FALSE;
      /* unable to find min and max, training has failed */
    }
  }

  /* Apply the trained value */

  for(byte_counter =0; byte_counter < 4; byte_counter++)
  {
    BOOT_HWIO_OUTM(
        PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0),
        (offset + SDRAM_DQ1_OFFSET * byte_counter),
        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE),
        (write_training_fine[byte_counter] | ( write_training_coarse[byte_counter] << 3)) << HWIO_SHFT(PHY_DQ_ADDR(DDRPHY_DQ_WR_CDC_DELAY0), DELAY_VALUE));
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
boolean HAL_SDRAM_PHY_DCC_Calibration(SDRAM_INTERFACE interface_name, uint32 dcc_offset, uint8 * dcc_result)
{
  uint32 dim_offset;
  ddr_mask ddr_broadcast_mask = {0};
  uint32 offset;
  
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

  /* Check for DCC calibration status */
  
  if(dcc_offset == 0)
  {
    while(BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                        dim_offset,
                        HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                            DCC0_DCC_IN_PROGRESS)));

    while(BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                        DQ_BCAST_OFFSET,
                        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                            DCC0_DCC_IN_PROGRESS)));
  }
  else
  {
    while(BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                      dim_offset,
                      HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_RB_STATUS),
                      DCC1_DCC_IN_PROGRESS)));

    while(BOOT_HWIO_INM(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                        DQ_BCAST_OFFSET,
                        HWIO_FMSK(PHY_DQ_ADDR(DDRPHY_DQ_RB_STATUS),
                        DCC1_DCC_IN_PROGRESS)));
  }
  
  for(offset = 0; offset < 5; offset++)
  {
    dcc_result[offset] = BOOT_HWIO_INM(PHY_CA_ADDR(DDRPHY_CA_DCC0_STATUS), (dim_offset + dcc_offset + 
    offset*DDR_DCC_CA_DQ_OFFSET),
    HWIO_FMSK(PHY_CA_ADDR(DDRPHY_CA_DCC0_STATUS), DCC_OFFSET))
    >> HWIO_SHFT(PHY_CA_ADDR(DDRPHY_CA_DCC0_STATUS), DCC_OFFSET);
  }
  
  return TRUE;
}
