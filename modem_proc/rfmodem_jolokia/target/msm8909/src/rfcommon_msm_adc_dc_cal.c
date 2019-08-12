/*! 
  @file
  rfcommon_msm_bbrx_adc_dc_cal_tbl.c
 
  @brief
  captures the BBRx ADC DC cal table 
 
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfmodem/jolokia/main/1.0/target/msm8909/src/rfcommon_msm_adc_dc_cal.c

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/22/15   lm      Removed config3 param from rfcommon_msm_adc_vcm_cal_tbl
12/16/14   vv      Initial version.
==============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rflm_adc_dc_cal.h"

const rfcommon_msm_adc_vcm_cal_tbl_type rfcommon_msm_adc_vcm_cal_tbl[MAX_ADC_DC_CAL_ENTRIES] =
{
  /*-10mv <= DC <= 10mv*/
  {
     -10,               /*start range*/
      10,               /*stop_range*/
     0x00000000,        /*config2 reg DC value from modem*/
  },

  /*10mv < DC <= 15mv*/
  {
     10,                /*start range*/
     15,                /*stop_range*/
     0x00002800,        /*config2 reg DC value from modem*/
  },

  /*15mv < DC <= 25mv*/
  {
     15,                /*start range*/
     25,                /*stop_range*/
     0x00003000,        /*config2 reg DC value from modem*/
  },

  /*25mv < DC <= 35mv*/
  {
     25,                /*start range*/
     35,                /*stop_range*/
     0x00003800,        /*config2 reg DC value from modem*/
  },

  /*35mv < DC <= 45mv*/
  {
     35,                /*start range*/
     45,                /*stop_range*/
     0x00008800,        /*config2 reg DC value from modem*/
  },

  /*45mv < DC <= 57mv*/
  {
     45,                /*start range*/
     57,                /*stop_range*/
     0x00009000,        /*config2 reg DC value from modem*/
  },

  /*57mv < DC <= 68mv*/
  {
     57,                /*start range*/
     68,                /*stop_range*/
     0x00009800,        /*config2 reg DC value from modem*/
  },

  /*68mv < DC <= 80mv*/
  {
     68,                /*start range*/
     80,                /*stop_range*/
     0x0000A000,        /*config2 reg DC value from modem*/
  },

  /*80mv < DC <= 90mv*/
  {
     80,                /*start range*/
     90,                /*stop_range*/
     0x00008000,        /*config2 reg DC value from modem*/
  },

  /*90mv < DC <= 100mv*/
  {
     90,                /*start range*/
     100,                /*stop_range*/
     0x0000A800,        /*config2 reg DC value from modem*/
  },

  /*100mv < DC <= 110mv*/
  {
     100,                /*start range*/
     110,                /*stop_range*/
     0x0000B000,        /*config2 reg DC value from modem*/
  },


  /*DC > 110mv*/
  {
     110,               /*start range*/
     0x7FFFFFFF,        /*stop_range*/
     0x0000B800,        /*config2 reg DC value from modem*/
  },

  /*-15mv =< DC < -10mv*/
  {
     -15,               /*start range*/
     -10,               /*stop_range*/
     0x00002000,        /*config2 reg DC value from modem*/
  },

  /*-25mv <= DC < -15mv*/
  {
     -25,               /*start range*/
     -15,               /*stop_range*/
     0x00001800,        /*config2 reg DC value from modem*/
  },

  /*-35mv <= DC < -25mv*/
  {
     -35,               /*start range*/
     -25,               /*stop_range*/
     0x00001000,        /*config2 reg DC value from modem*/
  },

  /*-45mv <= DC < -35mv*/
  {
     -45,               /*start range*/
     -35,               /*stop_range*/
     0x00000800,        /*config2 reg DC value from modem*/
  },

  /*-55mv <= DC < -45mv*/
  {
     -55,               /*start range*/
     -45,               /*stop_range*/
     0x0000E000,        /*config2 reg DC value from modem*/
  },

  /*-65mv <= DC < -55mv*/
  {
     -65,               /*start range*/
     -55,               /*stop_range*/
     0x0000D800,        /*config2 reg DC value from modem*/
  },

  /*-75mv <= DC < -65mv*/
  {
     -75,               /*start range*/
     -65,               /*stop_range*/
     0x0000D000,        /*config2 reg DC value from modem*/
  },

  /*-85mv <= DC < -75mv*/
  {
     -85,               /*start range*/
     -75,               /*stop_range*/
     0x0000C800,        /*config2 reg DC value from modem*/
  },

  /*-95mv <= DC < -85mv*/
  {
     -95,               /*start range*/
     -85,               /*stop_range*/
     0x00006000,        /*config2 reg DC value from modem*/
  },

  /*-105mv <= DC < -95mv*/
  {
     -105,               /*start range*/
     -95,               /*stop_range*/
     0x00005800,        /*config2 reg DC value from modem*/
  },

  /*-115mv <= DC < -105mv*/
  {
     -115,              /*start range*/
     -105,               /*stop_range*/
     0x00005000,        /*config2 reg DC value from modem*/
  },

  /*DC < -115mv*/
  {
     0x80000000,        /*start range*/
     -115,              /*stop_range*/
     0x00004800,        /*config2 reg DC value from modem*/
  },

};

boolean get_rfcommon_msm_adc_vcm_cal_tbl(const rfcommon_msm_adc_vcm_cal_tbl_type **rfcommon_msm_adc_vcm_cal_tbl_p)
{

   *rfcommon_msm_adc_vcm_cal_tbl_p = (const rfcommon_msm_adc_vcm_cal_tbl_type*)rfcommon_msm_adc_vcm_cal_tbl;

   return TRUE;

}
