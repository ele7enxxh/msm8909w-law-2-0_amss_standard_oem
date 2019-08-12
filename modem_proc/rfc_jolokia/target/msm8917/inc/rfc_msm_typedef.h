#ifndef RFC_MSM_TYPEDEF_H
#define RFC_MSM_TYPEDEF_H

/*!
  @file
  rfc_msm_typedef.h

  @brief
  This modules contains type definitions which will be required and used by RFC modules.
*/

/*==============================================================================
Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/msm8917/inc/rfc_msm_typedef.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/24/14   hoh     Add multislot APT control feature
04/23/14   sd      New revision. 

==============================================================================*/

#include "comdef.h"
#include "DDITlmm.h"
#include "rfcommon_core_sbi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFC_GPIO_INVALID 0xFFFFFFFF

typedef enum
{
  RFC_PA_ON,
  RFC_ANT_SEL,
  RFC_TX_ON_AG,
  RFC_RX_ON_AG,
  RFC_PA_RANGE0,  
  RFC_PA_RANGE1,  
  RFC_RFIC0_SSBI1,
  RFC_RFIC0_SSBI2,
  RFC_RFIC1_SSBI1,
  RFC_RFIC1_SSBI2,
  RFC_RFIC2_SSBI1,
  RFC_PDM,
  RFC_APT_SEL,
  RFC_APT_BYPASS,
  RFC_APT_MODE,
  RFC_INIT_LOW,
  RFC_INIT_HIGH,
  RFC_TX_GTR_THRESH, 
  RFC_GNSS_BLANK,
  RFC_GNSS_BLANK_TDS,
  RFC_DAC0_UPDATE,
  RFC_DAC1_UPDATE, 
  RFC_PA_IND,
  RFC_QPT,
  RFC_GPDATA_CHAN0,
  RFC_GPDATA_CHAN1,
  RFC_TIMING_PA_ON,
  RFC_TIMING_TX_ON,   
  RFC_TIMING_PA_RANGE,
  RFC_TIMING_TX_TX_RF_ON0,
  RFC_TIMING_TX_RX_RF_ON0,
  RFC_RFFE1_DATA,
  RFC_RFFE1_CLK,
  RFC_RFFE2_DATA,
  RFC_RFFE2_CLK,
  RFC_RFFE3_DATA,
  RFC_RFFE3_CLK,
  RFC_RFFE4_DATA,
  RFC_RFFE4_CLK,
  RFC_RFFE5_DATA,
  RFC_RFFE5_CLK,
  RFC_HSIC_STROBE,
  RFC_HSIC_DATA,
  RFC_TIMING_PA_CTL,
  RFC_TIMING_ASM_CTL,
  RFC_GPS_TX_AGGRESSOR,
  RFC_LTE_ACTIVE,
  RFC_LTE_SYNC,
  RFC_WCN_PRIORITY,
  RFC_EXT_GPS_LNA_EN_AG,
  RFC_SSBI_SPARE_0,
  RFC_TIMING_TUNER_CTL,
  RFC_TIMING_PAPM_CTL,
  RFC_LTE_ACTIVITY_FACTOR,
  RFC_LTE_FRAME_SYNC,
  RFC_LTE_SUBFRAME_MARKER,
  RFC_COEX_TX_ACTIVITY_FACTOR,
  RFC_TIMING_ASM_TRIGGER,
  RFC_TIMING_PAPM_TX_TX_TRIGGER,
  RFC_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER,
  RFC_TIMING_PA_TRIGGER,
  RFC_TIMING_PAPM_OFF_TX_RX_TX_CTL,
  RFC_TIMING_PAPM_TX_TX_CTL,
  RFC_TIMING_PAPM_MULTISLOT_CTL,
  RFC_SIG_TYPE_INVALID
} rfc_signal_type;


typedef enum
{
  RFC_GPIO,
  RFC_SSBI,
  RFC_RFFE,
  RFC_GRFC,
  RFC_GPIO_GRFC_INVALID
} rfc_gpio_grfc_type;


typedef struct
{
  rfc_signal_type signal_type;
  uint32 msm_gpio;
  uint8 grfc_num; 
  rfc_gpio_grfc_type output_type;
  uint8 function_select;
  DALGpioDirectionType direction;
  char* tlmm_gpio_name;
} rfc_msm_signal_info_type;

#define RFC_TARGET_SSBI_CHIP0_RX_PHYSICAL_BUS SSBI_BUS_2

#define RFC_TARGET_SSBI_CHIP0_TX_PHYSICAL_BUS SSBI_BUS_1

#define RFC_TARGET_SSBI_CHIP1_RX_PHYSICAL_BUS SSBI_BUS_4

#define RFC_TARGET_SSBI_CHIP1_TX_PHYSICAL_BUS SSBI_BUS_3

#ifdef __cplusplus
}
#endif



#endif


