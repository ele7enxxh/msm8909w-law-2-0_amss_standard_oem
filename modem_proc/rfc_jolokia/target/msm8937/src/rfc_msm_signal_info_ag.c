
/*
WARNING: This file is auto-generated.

Generated using: xmlautogen.exe
Generated from: V5.9.321 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2016 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/msm8937/src/rfc_msm_signal_info_ag.c#1 $ 


=============================================================================*/


/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_msm_signal_info_ag.h"
#include "rfc_msm_typedef.h"
#include "rfcommon_msm.h"

rfc_msm_signal_info_type rfc_msm8937_signal_info[RFC_MSM_SIG_NUM] =
{
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_00, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  104             ,  0              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[0]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_01, MSM Pin Name: GPIO_104*/
  {  RFC_ANT_SEL                         ,  106             ,  2              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[2]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_02, MSM Pin Name: GPIO_106*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_03, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  108             ,  4              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[4]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_04, MSM Pin Name: GPIO_108*/
  {  RFC_ANT_SEL                         ,  111             ,  7              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[7]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_05, MSM Pin Name: GPIO_111*/
  {  RFC_ANT_SEL                         ,  113             ,  9              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[9]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_06, MSM Pin Name: GPIO_113*/
  {  RFC_ANT_SEL                         ,  115             ,  11             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[11]"            },  /* Signal: RFC_MSM_RF_PATH_SEL_07, MSM Pin Name: GPIO_115*/
  {  RFC_ANT_SEL                         ,  117             ,  13             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[13]"            },  /* Signal: RFC_MSM_RF_PATH_SEL_08, MSM Pin Name: GPIO_117*/
  {  RFC_ANT_SEL                         ,  105             ,  1              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[1]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_09, MSM Pin Name: GPIO_105*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_10, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  107             ,  3              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[3]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_11, MSM Pin Name: GPIO_107*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_12, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_13, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  114             ,  10             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[10]"            },  /* Signal: RFC_MSM_RF_PATH_SEL_14, MSM Pin Name: GPIO_114*/
  {  RFC_ANT_SEL                         ,  110             ,  6              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[6]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_15, MSM Pin Name: GPIO_110*/
  {  RFC_ANT_SEL                         ,  116             ,  12             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[12]"            },  /* Signal: RFC_MSM_RF_PATH_SEL_16, MSM Pin Name: GPIO_116*/
  {  RFC_ANT_SEL                         ,  109             ,  5              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[5]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_17, MSM Pin Name: GPIO_109*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_18, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  112             ,  8              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[8]"             },  /* Signal: RFC_MSM_RF_PATH_SEL_20, MSM Pin Name: GPIO_112*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_21, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_22, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_23, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_24, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_PATH_SEL_25, MSM Pin Name: MAX*/
  {  RFC_PA_RANGE0                       ,  109             ,  5              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[5]"             },  /* Signal: RFC_MSM_PA0_R0, MSM Pin Name: GPIO_109*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA0_R1, MSM Pin Name: MAX*/
  {  RFC_PA_RANGE0                       ,  112             ,  8              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[8]"             },  /* Signal: RFC_MSM_PA0_R0_ALT1, MSM Pin Name: GPIO_112*/
  {  RFC_PA_RANGE1                       ,  113             ,  9              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[9]"             },  /* Signal: RFC_MSM_PA0_R1_ALT1, MSM Pin Name: GPIO_113*/
  {  RFC_PA_RANGE0                       ,  105             ,  1              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[1]"             },  /* Signal: RFC_MSM_PA1_R0_DEFAULT, MSM Pin Name: GPIO_105*/
  {  RFC_PA_RANGE1                       ,  107             ,  3              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[3]"             },  /* Signal: RFC_MSM_PA1_R1_DEFAULT, MSM Pin Name: GPIO_107*/
  {  RFC_PA_RANGE0                       ,  117             ,  13             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[13]"            },  /* Signal: RFC_MSM_PA1_R2, MSM Pin Name: GPIO_117*/
  {  RFC_PA_RANGE1                       ,  110             ,  6              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[6]"             },  /* Signal: RFC_MSM_PA0_R2, MSM Pin Name: GPIO_110*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA0_R3, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA1_R3, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  112             ,  8              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[8]"             },  /* Signal: RFC_MSM_PA_ON_00, MSM Pin Name: GPIO_112*/
  {  RFC_PA_ON                           ,  107             ,  3              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[3]"             },  /* Signal: RFC_MSM_PA_ON_01, MSM Pin Name: GPIO_107*/
  {  RFC_PA_ON                           ,  111             ,  7              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[7]"             },  /* Signal: RFC_MSM_PA_ON_02, MSM Pin Name: GPIO_111*/
  {  RFC_PA_ON                           ,  116             ,  12             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[12]"            },  /* Signal: RFC_MSM_PA_ON_03, MSM Pin Name: GPIO_116*/
  {  RFC_PA_ON                           ,  104             ,  0              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[0]"             },  /* Signal: RFC_MSM_PA_ON_04, MSM Pin Name: GPIO_104*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_05, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  108             ,  4              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[4]"             },  /* Signal: RFC_MSM_PA_ON_06, MSM Pin Name: GPIO_108*/
  {  RFC_PA_ON                           ,  106             ,  2              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[2]"             },  /* Signal: RFC_MSM_PA_ON_07, MSM Pin Name: GPIO_106*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_08, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_09, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  105             ,  1              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[1]"             },  /* Signal: RFC_MSM_PA_ON_10, MSM Pin Name: GPIO_105*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_11, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  117             ,  13             ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[13]"            },  /* Signal: RFC_MSM_PA_ON_12, MSM Pin Name: GPIO_117*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_13, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_14, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_15, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_16, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_17, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  113             ,  9              ,  RFC_GRFC             ,  1,  DAL_GPIO_OUTPUT                     , "grfc[9]"             },  /* Signal: RFC_MSM_PA_ON_18, MSM Pin Name: GPIO_113*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_PA_ON_19, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RX_ON0, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RX_ON1, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RX_ON0_alt, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_ON0, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RF_ON1, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RFIC0_SSBI1, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RFIC0_SSBI2, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RFIC1_SSBI1, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_RFIC1_SSBI2, MSM Pin Name: MAX*/
  {  RFC_RFFE1_CLK                       ,  100             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe1_clk"           },  /* Signal: RFC_MSM_RFFE1_CLK, MSM Pin Name: GPIO_100*/
  {  RFC_RFFE1_DATA                      ,  101             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe1_data"          },  /* Signal: RFC_MSM_RFFE1_DATA, MSM Pin Name: GPIO_101*/
  {  RFC_RFFE2_CLK                       ,  102             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe2_clk"           },  /* Signal: RFC_MSM_RFFE2_CLK, MSM Pin Name: GPIO_102*/
  {  RFC_RFFE2_DATA                      ,  103             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe2_data"          },  /* Signal: RFC_MSM_RFFE2_DATA, MSM Pin Name: GPIO_103*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rffe3_clk"           },  /* Signal: RFC_MSM_RFFE3_CLK, MSM Pin Name: GPIO_0*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rffe3_data"          },  /* Signal: RFC_MSM_RFFE3_DATA, MSM Pin Name: GPIO_0*/
  {  RFC_RFFE4_CLK                       ,  122             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe4_clk"           },  /* Signal: RFC_MSM_RFFE4_CLK, MSM Pin Name: GPIO_122*/
  {  RFC_RFFE4_DATA                      ,  123             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe4_data"          },  /* Signal: RFC_MSM_RFFE4_DATA, MSM Pin Name: GPIO_123*/
  {  RFC_RFFE5_CLK                       ,  120             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe5_clk"           },  /* Signal: RFC_MSM_RFFE5_CLK, MSM Pin Name: GPIO_120*/
  {  RFC_RFFE5_DATA                      ,  121             ,  RFC_GRFC_NUM_NA,  RFC_RFFE             ,  1,  DAL_GPIO_OUTPUT                     , "rffe5_data"          },  /* Signal: RFC_MSM_RFFE5_DATA, MSM Pin Name: GPIO_121*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_GPDATA0_1, MSM Pin Name: MAX*/
  {  RFC_GPDATA_CHAN0                    ,  119             ,  RFC_GRFC_NUM_NA,  RFC_GPIO             ,  1,  DAL_GPIO_OUTPUT                     , "gsm0_tx_phase_d"     },  /* Signal: RFC_MSM_GPDATA0_0, MSM Pin Name: GPIO_119*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_GPDATA1_1, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_GPDATA1_0, MSM Pin Name: MAX*/
  {  RFC_GNSS_BLANK                      ,  RFC_GPIO_INVALID,  31             ,  RFC_GRFC             ,  0,  DAL_GPIO_OUTPUT                     , "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_INTERNAL_GNSS_BLANK, MSM Pin Name: MAX*/
  {  RFC_GNSS_BLANK                      ,  RFC_GPIO_INVALID,  31             ,  RFC_GRFC             ,  0,  DAL_GPIO_OUTPUT                     , "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_INTERNAL_GNSS_BLANK_CONCURRENCY, MSM Pin Name: MAX*/
  {  RFC_GPS_TX_AGGRESSOR                ,  115             ,  11             ,  RFC_GRFC             ,  1,  DAL_GPIO_INPUT                      , "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_GPS_TX_AGGRESSOR, MSM Pin Name: MAX*/
  {  RFC_PA_IND                          ,  116             ,  RFC_GRFC_NUM_NA,  RFC_GPIO             ,  2,  DAL_GPIO_OUTPUT                     , "pa_indicator_1_or_2" },  /* Signal: RFC_MSM_PA_IND, MSM Pin Name: GPIO_116*/
  {  RFC_TX_GTR_THRESH                   ,  RFC_GPIO_INVALID,  29             ,  RFC_GRFC             ,  0,  DAL_GPIO_OUTPUT                     , "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TX_GTR_TH, MSM Pin Name: MAX*/
  {  RFC_TX_GTR_THRESH                   ,  RFC_GPIO_INVALID,  30             ,  RFC_GRFC             ,  0,  DAL_GPIO_OUTPUT                     , "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TX_GTR_TH_CONCURRENCY, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_LTE_ACTIVITY_FACTOR, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_LTE_FRAME_SYNC, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_LTE_SUBFRAME_MARKER, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_COEX_TX_ACTIVITY_FACTOR, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_WCN_PRIORITY, MSM Pin Name: MAX*/
  {  RFC_TIMING_PA_CTL                   ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PA_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PA_RANGE                 ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PA_RANGE, MSM Pin Name: MAX*/
  {  RFC_TIMING_ASM_CTL                  ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_ASM_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_TUNER_CTL                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_TUNER_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_CTL                 ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_TX_TX_RF_ON0             ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_TX_TX_RF_ON0, MSM Pin Name: MAX*/
  {  RFC_TIMING_TX_RX_RF_ON0             ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_TX_RX_RF_ON0, MSM Pin Name: MAX*/
  {  RFC_TIMING_ASM_TRIGGER              ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_ASM_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_TX_TX_TRIGGER       ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_TX_TX_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PA_TRIGGER               ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PA_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_OFF_TX_RX_TX_CTL    ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_TX_TX_CTL           ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_TX_TX_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_MULTISLOT_CTL       ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType, "rf_gpio_invalid_name"},  /* Signal: RFC_MSM_TIMING_PAPM_MULTISLOT_CTL, MSM Pin Name: MAX*/
};


void rfc_msm_update_settings_modem_v1(void)
{

}


uint32 rfc_msm_sig_info_table_get(rfc_msm_signal_info_type **msm_info_table)
{
  if (NULL == msm_info_table)
  {
    return 0;
  }

  if (0xB0000 == rfcommon_msm_read_modem_id())
  {
    rfc_msm_update_settings_modem_v1();
  }
  *msm_info_table = &rfc_msm8937_signal_info[0];
  return RFC_MSM_SIG_NUM;
}



rfc_timing_info_type rfc_msm_timing_info[RFC_TIMING_PARAMS_NUM] = 
{
  {  LTE_TX_ON_UL_SF_SMPS_PGM,  -700 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_UL_SF_PA_CTL_ON,  -654 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_QPOET_NonTr_To_QPOET_TR,  -614 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_PA_CTL_ON,  -979 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_SMPS_PGM,  -1026 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_UL_SF_PA_CTL_ON,  -46 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_UL_SF_SMPS_PGM,  -92 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_POST_UE_SRS_PA_CTL_ON,  -46 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_POST_UE_SRS_SMPS_PGM,  -92 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_PRE_UE_SRS_PA_CTL_ON,  -46 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_PRE_UE_SRS_SMPS_PGM,  -92 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_OFF_NULL_SF_PA_CTL_OFF,  -62 },  /* FDD, Unit: Ts */
  {  LTE_TX_OFF_CELL_SRS_PA_CTL_OFF,  -62 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_SMPS_EN_LOW,  614 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_SMPS_EN_HIGH,  -1382 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_UL_SF_ASM_TX,  -461 },  /* TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_ASM_TX,  -730 },  /* TDD, Unit: Ts */
  {  LTE_TX_OFF_NULL_SF_ASM_RX,  154 },  /* TDD, Unit: Ts */
  {  LTE_TX_OFF_CELL_SRS_ASM_RX,  154 },  /* TDD, Unit: Ts */
};   





