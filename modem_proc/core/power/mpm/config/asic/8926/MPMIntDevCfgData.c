/*===========================================================================
   FILE:         MPMIntDevCfgData.c
 
   DESCRIPTION:  MPM hardware to local interrupt/gpio mapping info.

                 Copyright © 2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/8926/MPMIntDevCfgData.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "mpmint_target.h"
#include "mpminti.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/**
 * Table mapping the external enumeration's representation of interrupts to the
 * HAL's internal enumeration values. Any entries which are unused or reserved
 * in hal table, are skipped here.
 */
mpmint_irq_data_type devcfgMpmIntMappingTable[] =
{
  /* MPM_QTIMER_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_PEN_DOWN_ISR           */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_TSENS_UPPER_LOWER_ISR  */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_GPIO102_ISR            */  {3,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO1_ISR              */  {4,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO5_ISR              */  {5,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO9_ISR              */  {6,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO18_ISR             */  {7,                             MPMINT_NULL_IRQ}, 
  /* MPM_GPIO20_ISR             */  {8,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO24_ISR             */  {9,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO27_ISR             */  {10,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO28_ISR             */  {11,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO34_ISR             */  {12,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO35_ISR             */  {13,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO37_ISR             */  {14,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO42_ISR             */  {15,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO44_ISR             */  {16,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO46_ISR             */  {17,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO50_ISR             */  {18,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO54_ISR             */  {19,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO59_ISR             */  {20,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO61_ISR             */  {21,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO62_ISR             */  {22,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO64_ISR             */  {23,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO65_ISR             */  {24,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO66_ISR             */  {25,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO67_ISR             */  {26,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO68_ISR             */  {27,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO71_ISR             */  {28,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO72_ISR             */  {29,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO73_ISR             */  {30,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO74_ISR             */  {31,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO75_ISR             */  {32,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO77_ISR             */  {33,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO79_ISR             */  {34,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO80_ISR             */  {35,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO82_ISR             */  {36,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO86_ISR             */  {37,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO92_ISR             */  {38,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO93_ISR             */  {39,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO95_ISR             */  {40,                            MPMINT_NULL_IRQ},
  /* MPM_HSIC_STROBE_CORE_ISR   */  {41,                            MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA1_ISR         */  {42,                            MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA3_ISR         */  {43,                            MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA1_ISR         */  {44,                            MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA3_ISR         */  {45,                            MPMINT_NULL_IRQ},
  /* MPM_SRST_N_ISR             */  {46,                            MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_OTG_0_ISR     */  {47,                            MPMINT_NULL_IRQ},
  /* MPM_USB2_ID_HV_0_ISR       */  {48,                            MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DPSE_0_ISR    */  {49,                            MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_OTG_1_ISR     */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_ID_HV_1_ISR       */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DPSE_1_ISR    */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_HDMI_PHY_RCV_ISR       */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DMSE_0_ISR    */  {58,                            MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DMSE_1_ISR    */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_UIM_CTL_CARD_ISR       */  {60,                            MPMINT_NULL_IRQ},
  /* MPM_UIM_CTL_BATT_ISR       */  {61,                            MPMINT_NULL_IRQ},
  /* MPM_MPM_SPMI_WAKE_ISR      */  {62,                            MPMINT_NULL_IRQ},
  
  /* End of table Entry         */  {MPMINT_IMT_EOS_HAL_IRQ,        MPMINT_IMT_EOS_LOCAL_IRQ},
};
