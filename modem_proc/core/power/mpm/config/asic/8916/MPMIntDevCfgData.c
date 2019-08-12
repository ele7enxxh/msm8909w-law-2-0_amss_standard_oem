/*===========================================================================
   FILE:         mpmint_map.c
 
   DESCRIPTION:  MPM HAL <--> Tramp IRQ mapping info.

                 Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/8916/MPMIntDevCfgData.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "mpmint_target.h"
#include "mpminti.h"


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


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
  /* MPM_GPIO1_ISR              */  {3,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO4_ISR              */  {4,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO5_ISR              */  {5,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO9_ISR              */  {6,                             MPMINT_NULL_IRQ}, 
  /* MPM_GPIO13_ISR             */  {7,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO17_ISR             */  {8,                             MPMINT_NULL_IRQ},
  /* MPM_GPIO21_ISR             */  {9,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO27_ISR             */  {10,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO28_ISR             */  {11,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO31_ISR             */  {12,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO33_ISR             */  {13,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO35_ISR             */  {14,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO37_ISR             */  {15,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO38_ISR             */  {16,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO49_ISR             */  {17,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO50_ISR             */  {18,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO51_ISR             */  {19,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO52_ISR             */  {20,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO54_ISR             */  {21,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO62_ISR             */  {22,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO63_ISR             */  {23,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO64_ISR             */  {24,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO65_ISR             */  {25,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO66_ISR             */  {26,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO67_ISR             */  {27,                            MPMINT_NULL_IRQ},
  /* MPM_GPIO68_ISR             */  {28,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO69_ISR             */  {29,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO107_ISR            */  {30,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO108_ISR            */  {31,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO119_ISR            */  {32,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO110_ISR            */  {33,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO111_ISR            */  {34,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO112_ISR            */  {35,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO113_ISR            */  {36,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO114_ISR            */  {37,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO115_ISR            */  {38,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO116_ISR            */  {39,   			    MPMINT_NULL_IRQ},
  /* MPM_GPIO117_ISR            */  {40,   			    MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA1_ISR         */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA3_ISR         */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA1_ISR         */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA3_ISR         */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SRST_N_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_OTG_0_ISR     */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_ID_HV_0_ISR       */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DPSE_0_ISR    */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DMSE_0_ISR    */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_UIM_CARD_ISR           */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_UIM_BATT_ISR           */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* MPM_SPMI_ISR               */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},
  /* RESERVED/UNUSED            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ,   MPMINT_NULL_IRQ},

  /* End of table Entry         */  {MPMINT_IMT_EOS_HAL_IRQ, MPMINT_IMT_EOS_LOCAL_IRQ},
};
