#ifndef RFC_BSP_H
#define RFC_BSP_H

/*!
  @file
  rfc_bsp.h

  @brief
  This modules contains definitions to support the GPIO configurations 
  for the RFA module in 9x00 target. The definitions in this module are
  for use with TLMM HAL and corresponding BSP module.
*/

/*==============================================================================
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/msm8917/inc/rfc_bsp.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/05/14   aak     MPP for DAC Vref is MPP3 for MSM8916
04/23/14   sd      Initial revision for MSM8916

==============================================================================*/
#include "DDITlmm.h"
#include "rfd_grfc.h"

/*! \{ */
/*============================================================================*/
/*!
  @name Definitions of GPIO enumerations for RFA Driver code
  
  @brief
  The following statements define TLMM GPIO configurations available to RFA
  modules. These configurations are target specific. The location of this file 
  is significant since that determines the supported target.
*/
/*! \{ */

#define SSBI1_RTR0      (DALGpioSignalType)DAL_GPIO_CFG(143, 1, DAL_GPIO_OUTPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)
#define SSBI2_RTR0      (DALGpioSignalType)DAL_GPIO_CFG(142, 1, DAL_GPIO_OUTPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)
#define SSBI1_RTR1      (DALGpioSignalType)DAL_GPIO_CFG(141, 1, DAL_GPIO_OUTPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)
#define SSBI2_RTR1      (DALGpioSignalType)DAL_GPIO_CFG(140, 1, DAL_GPIO_OUTPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)
#define SSBI_QPA0        (DALGpioSignalType)DAL_GPIO_CFG(129, 2, DAL_GPIO_OUTPUT, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA)

/*----------------------------------------------------------------------------*/
#define RFC_GPIO_137 (DALGpioSignalType)DAL_GPIO_CFG(137, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA)

/*----------------------------------------------------------------------------*/
#define RFC_EXT_GPS_LNA_EN RFC_GPIO_137

#define COMMON_RF_ENGINE_MAX    RESERVED_ENGINE_MAX      
#define COMMON_RF_CTL_SIG_MAX   RESERVED_RF_CTL_SIG_MAX
#define COMMON_PA_RANGE_0       PA_RANGE_0    
#define COMMON_PA_RANGE_1       PA_RANGE_1 
#define COMMON_PA_RANGE2_0      PA_RANGE2_0    
#define COMMON_PA_RANGE2_1      PA_RANGE2_1     
#define COMMON_PA_ON_SRC        PA_ON_SRC 
#define COMMON_PA_ON2_SRC       PA_ON2_SRC 
#define COMMON_TX_ON_SRC        TX_ON_SRC 
#define COMMON_TX_ON2_SRC       TX_ON2_SRC 

#define RF_GRFCS_GFW_ADD_BASE GFW_GRFC_ENGINE_BASE
#define RF_RFCTL_GFW_ADD_BASE GFW_GRFC_ENGINE_RF_BASE

/* The MPP used for DAC Vref depends on the target */
#define PM_MPP_DAC_VREF  PM_MPP_3 /* FOR MSM8916 */

typedef rfd_rt_input_grfc_engine_type       rf_grfc_engine_type;
typedef rfd_internal_grfc_engine_rf_enum_type rf_internal_grfc_engine_type;
typedef rfd_grfc_engine_polarity_enum_type  rf_grfc_polarity_type;          
typedef rfd_rt_grfc_out_type                rf_grfc_out_type;
typedef uint16 rf_control_signal_type;

/*! \} */

#endif /* RFC_BSP_H */                           
                                                          
