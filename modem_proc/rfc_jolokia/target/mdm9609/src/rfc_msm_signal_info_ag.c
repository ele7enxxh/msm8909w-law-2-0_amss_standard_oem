
/*
WARNING: This file is auto-generated.

Generated using: rfc_autogen.exe
Generated from:  v4.12.421 of BOLT_RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfc_jolokia/target/mdm9609/src/rfc_msm_signal_info_ag.c#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_msm_signal_info_ag.h" 
#include "rfc_msm_typedef.h" 
#include "rfcommon_msm.h" 



rfc_msm_signal_info_type rfc_mdm9609_signal_info[RFC_MSM_SIG_NUM] = 
{
  {  RFC_ANT_SEL                         ,  75              ,  1              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_00, MSM Pin Name: GPIO_75*/
  {  RFC_ANT_SEL                         ,  74              ,  0              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_01, MSM Pin Name: GPIO_74*/
  {  RFC_ANT_SEL                         ,  76              ,  2              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_02, MSM Pin Name: GPIO_76*/
  {  RFC_ANT_SEL                         ,  79              ,  5              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_03, MSM Pin Name: GPIO_79*/
  {  RFC_ANT_SEL                         ,  78              ,  4              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_04, MSM Pin Name: GPIO_78*/
  {  RFC_ANT_SEL                         ,  93              ,  19             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_05, MSM Pin Name: GPIO_93*/
  {  RFC_ANT_SEL                         ,  83              ,  9              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_06, MSM Pin Name: GPIO_83*/
  {  RFC_ANT_SEL                         ,  85              ,  11             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_07, MSM Pin Name: GPIO_85*/
  {  RFC_ANT_SEL                         ,  89              ,  15             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_08, MSM Pin Name: GPIO_89*/
  {  RFC_ANT_SEL                         ,  92              ,  18             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_09, MSM Pin Name: GPIO_92*/
  {  RFC_ANT_SEL                         ,  80              ,  6              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_10, MSM Pin Name: GPIO_80*/
  {  RFC_ANT_SEL                         ,  88              ,  14             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_11, MSM Pin Name: GPIO_88*/
  {  RFC_ANT_SEL                         ,  91              ,  17             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_12, MSM Pin Name: GPIO_91*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_13, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  84              ,  10             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_14, MSM Pin Name: GPIO_84*/
  {  RFC_ANT_SEL                         ,  90              ,  16             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_15, MSM Pin Name: GPIO_90*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_16, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_17, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_18, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  82              ,  8              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_20, MSM Pin Name: GPIO_82*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_21, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_22, MSM Pin Name: MAX*/
  {  RFC_ANT_SEL                         ,  87              ,  13             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_23, MSM Pin Name: GPIO_87*/
  {  RFC_ANT_SEL                         ,  95              ,  21             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_PATH_SEL_24, MSM Pin Name: GPIO_95*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_PATH_SEL_25, MSM Pin Name: MAX*/
  {  RFC_PA_RANGE0                       ,  94              ,  20             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA0_R0, MSM Pin Name: GPIO_94*/
  {  RFC_PA_RANGE1                       ,  87              ,  13             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA0_R1, MSM Pin Name: GPIO_87*/
  {  RFC_PA_RANGE0                       ,  92              ,  18             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA1_R0_DEFAULT, MSM Pin Name: GPIO_92*/
  {  RFC_PA_RANGE1                       ,  88              ,  14             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA1_R1_DEFAULT, MSM Pin Name: GPIO_88*/
  {  RFC_PA_RANGE0                       ,  79              ,  5              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA0_R0_DEFAULT, MSM Pin Name: GPIO_79*/
  {  RFC_PA_RANGE0                       ,  89              ,  15             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA1_R2, MSM Pin Name: GPIO_89*/
  {  RFC_PA_RANGE1                       ,  90              ,  16             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA0_R2, MSM Pin Name: GPIO_90*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA0_R3, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA1_R3, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  82              ,  8              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_00, MSM Pin Name: GPIO_82*/
  {  RFC_PA_ON                           ,  75              ,  1              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_01, MSM Pin Name: GPIO_75*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_02, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_03, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_04, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_05, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  78              ,  4              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_06, MSM Pin Name: GPIO_78*/
  {  RFC_PA_ON                           ,  79              ,  5              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_07, MSM Pin Name: GPIO_79*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_08, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_09, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  92              ,  18             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_10, MSM Pin Name: GPIO_92*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_11, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  89              ,  15             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_12, MSM Pin Name: GPIO_89*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_13, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_14, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_15, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_16, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_PA_ON_17, MSM Pin Name: MAX*/
  {  RFC_PA_ON                           ,  83              ,  9              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_18, MSM Pin Name: GPIO_83*/
  {  RFC_PA_ON                           ,  77              ,  3              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_ON_19, MSM Pin Name: GPIO_77*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RX_ON0, MSM Pin Name: MAX*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RX_ON1, MSM Pin Name: MAX*/
  {  RFC_RX_ON_AG                        ,  80              ,  6              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RX_ON0_alt, MSM Pin Name: GPIO_80*/
  {  RFC_TX_ON_AG                        ,  81              ,  7              ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RF_ON0, MSM Pin Name: GPIO_81*/
  {  RFC_SIG_TYPE_INVALID                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_PLACEHOLDER_DALGpioDirectionType },  /* Signal: RFC_MSM_RF_ON1, MSM Pin Name: MAX*/
  {  RFC_RFIC0_SSBI1                     ,  104             ,  RFC_GRFC_NUM_NA,  RFC_SSBI       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFIC0_SSBI1, MSM Pin Name: GPIO_104*/
  {  RFC_RFIC0_SSBI2                     ,  103             ,  RFC_GRFC_NUM_NA,  RFC_SSBI       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFIC0_SSBI2, MSM Pin Name: GPIO_103*/
  {  RFC_RFIC1_SSBI1                     ,  106             ,  RFC_GRFC_NUM_NA,  RFC_SSBI       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFIC1_SSBI1, MSM Pin Name: GPIO_106*/
  {  RFC_RFIC1_SSBI2                     ,  105             ,  RFC_GRFC_NUM_NA,  RFC_SSBI       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFIC1_SSBI2, MSM Pin Name: GPIO_105*/
  {  RFC_RFFE1_CLK                       ,  70              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  1,  DAL_GPIO_INPUT                       },  /* Signal: RFC_MSM_RFFE1_CLK, MSM Pin Name: GPIO_70*/
  {  RFC_RFFE1_DATA                      ,  71              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  1,  DAL_GPIO_INPUT                       },  /* Signal: RFC_MSM_RFFE1_DATA, MSM Pin Name: GPIO_71*/
  {  RFC_RFFE2_CLK                       ,  72              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE2_CLK, MSM Pin Name: GPIO_72*/
  {  RFC_RFFE2_DATA                      ,  73              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE2_DATA, MSM Pin Name: GPIO_73*/
  {  RFC_RFFE3_CLK                       ,  74              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  2,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE3_CLK, MSM Pin Name: GPIO_74*/
  {  RFC_RFFE3_DATA                      ,  76              ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  2,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE3_DATA, MSM Pin Name: GPIO_76*/
  {  RFC_RFFE5_CLK                       ,  103             ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  2,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE5_CLK, MSM Pin Name: GPIO_103*/
  {  RFC_RFFE5_DATA                      ,  104             ,  RFC_GRFC_NUM_NA,  RFC_RFFE       ,  2,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_RFFE5_DATA, MSM Pin Name: GPIO_104*/
  {  RFC_GPDATA_CHAN0                    ,  100             ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_GPDATA0_1, MSM Pin Name: GPIO_100*/
  {  RFC_GPDATA_CHAN0                    ,  99              ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_GPDATA0_0, MSM Pin Name: GPIO_99*/
  {  RFC_GPDATA_CHAN1                    ,  102             ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_GPDATA1_1, MSM Pin Name: GPIO_102*/
  {  RFC_GPDATA_CHAN1                    ,  101             ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_GPDATA1_0, MSM Pin Name: GPIO_101*/
  {  RFC_EXT_GPS_LNA_EN_AG               ,  96              ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_EXT_GPS_LNA_EN, MSM Pin Name: GPIO_96*/
  {  RFC_EXT_GPS_LNA_EN_AG               ,  96              ,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_EXT_GPS_LNA_EN_ALT1, MSM Pin Name: GPIO_96*/
  {  RFC_GNSS_BLANK                      ,  RFC_GPIO_INVALID,  31             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_INTERNAL_GNSS_BLANK, MSM Pin Name: MAX*/
  {  RFC_GNSS_BLANK                      ,  RFC_GPIO_INVALID,  26             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_INTERNAL_GNSS_BLANK_CONCURRENCY, MSM Pin Name: MAX*/
  {  RFC_GPS_TX_AGGRESSOR                ,  RFC_GPIO_INVALID,  17             ,  RFC_GRFC       ,  1,  DAL_GPIO_INPUT                       },  /* Signal: RFC_MSM_GPS_TX_AGGRESSOR, MSM Pin Name: GPIO_91*/
  {  RFC_PA_IND                          ,  86              ,  12             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_PA_IND, MSM Pin Name: GPIO_86*/
  {  RFC_TX_GTR_THRESH                   ,  87              ,  13             ,  RFC_GRFC       ,  1,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TX_GTR_TH, MSM Pin Name: GPIO_87*/
  {  RFC_LTE_ACTIVITY_FACTOR             ,  RFC_GPIO_INVALID,  30             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_LTE_ACTIVITY_FACTOR, MSM Pin Name: LTE_ACTIVITY_FACTOR*/
  {  RFC_LTE_FRAME_SYNC                  ,  RFC_GPIO_INVALID,  29             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_LTE_FRAME_SYNC, MSM Pin Name: LTE_FRAME_SYNC*/
  {  RFC_LTE_SUBFRAME_MARKER             ,  RFC_GPIO_INVALID,  25             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_LTE_SUBFRAME_MARKER, MSM Pin Name: LTE_SUBFRAME_MARKER*/
  {  RFC_COEX_TX_ACTIVITY_FACTOR         ,  RFC_GPIO_INVALID,  27             ,  RFC_GRFC       ,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_COEX_TX_ACTIVITY_FACTOR, MSM Pin Name: COEX_TX_ACTIVITY_FACTOR*/
  {  RFC_WCN_PRIORITY                    ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO       ,  1,  DAL_GPIO_INPUT                       },  /* Signal: RFC_MSM_WCN_PRIORITY, MSM Pin Name: GPIO_34*/
  {  RFC_TIMING_PA_CTL                   ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PA_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PA_RANGE                 ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PA_RANGE, MSM Pin Name: MAX*/
  {  RFC_TIMING_ASM_CTL                  ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_ASM_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_TUNER_CTL                ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_TUNER_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_CTL                 ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PAPM_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_TX_TX_RF_ON0             ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_TX_TX_RF_ON0, MSM Pin Name: MAX*/
  {  RFC_TIMING_TX_RX_RF_ON0             ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_TX_RX_RF_ON0, MSM Pin Name: MAX*/
  {  RFC_TIMING_ASM_TRIGGER              ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_ASM_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_TX_TX_TRIGGER       ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PAPM_TX_TX_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PA_TRIGGER               ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PA_TRIGGER, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_OFF_TX_RX_TX_CTL    ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_CTL, MSM Pin Name: MAX*/
  {  RFC_TIMING_PAPM_TX_TX_CTL           ,  RFC_GPIO_INVALID,  RFC_GRFC_NUM_NA,  RFC_GPIO_GRFC_INVALID,  0,  DAL_GPIO_OUTPUT                      },  /* Signal: RFC_MSM_TIMING_PAPM_TX_TX_CTL, MSM Pin Name: MAX*/
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
  *msm_info_table = &rfc_mdm9609_signal_info[0];
  return RFC_MSM_SIG_NUM;
}



rfc_timing_info_type rfc_msm_timing_info[RFC_TIMING_PARAMS_NUM] = 
{
  {  LTE_TX_ON_UL_SF_SMPS_PGM,  -700 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_UL_SF_PA_CTL_ON,  -654 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_QPOET_NonTr_To_QPOET_TR,  -614 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_PA_CTL_ON,  -979 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_SMPS_PGM,  -1026 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_UL_SF_PA_CTL_ON,  16 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_UL_SF_SMPS_PGM,  -30 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_POST_UE_SRS_PA_CTL_ON,  16 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_POST_UE_SRS_SMPS_PGM,  -30 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_PRE_UE_SRS_PA_CTL_ON,  16 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_CONT_PRE_UE_SRS_SMPS_PGM,  -30 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_OFF_NULL_SF_PA_CTL_OFF,  215 },  /* FDD, Unit: Ts */
  {  LTE_TX_OFF_CELL_SRS_PA_CTL_OFF,  215 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_SMPS_EN_LOW,  614 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_SMPS_EN_HIGH,  -1382 },  /* FDD, TDD, Unit: Ts */
  {  LTE_TX_ON_UL_SF_ASM_TX,  -169 },  /* TDD, Unit: Ts */
  {  LTE_TX_ON_PRACH_OR_SRS_ONLY_ASM_TX,  -730 },  /* TDD, Unit: Ts */
  {  LTE_TX_OFF_NULL_SF_ASM_RX,  430 },  /* TDD, Unit: Ts */
  {  LTE_TX_OFF_CELL_SRS_ASM_RX,  430 },  /* TDD, Unit: Ts */
};

