
#ifndef RFLM_ADC_DC_CAL_H
#define RFLM_ADC_DC_CAL_H


#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/rfmodem/jolokia/main/1.0/target/msm8909/inc/rflm_adc_dc_cal.h
$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     -------------------------------------------------------------

01/13/15   vv     Initial version

==============================================================================*/
                              

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"
#include "rxlm_intf.h"

#define EFS_ADC_DC_CAL_PATH             "selfcal/modem/adc.dat"
#define EFS_ADC_DC_CAL_LOG_PATH         "selfcal/modem/adclog.dat"
#define ADC_DC_OFFSET_SPEC_THRESHOLD    100 /*100mv*/
#define MAX_ADC_DC_CAL_ENTRIES          24
#define ADC_DC_CAL_MAX_ITERATIONS       2
#define ADC_CONFIG2_REG_DEFAULT         0x00000000
#define ADC_CONFIG3_REG_DEFAULT         0xFFFFFFFF    /*config3 register is not used for ADC VCm calibration on  Bolt/Bolt+ modems*/
#define ADC_REG_INVALID                 0xFFFFFFFF
#define ADC_INDEX_INVALID               0xFFFFFFFF
#define ADC_DC_VAL_THRESHOLD            10     /*10 mv*/
#define MILLI_VOLT_SCALING_FACTOR       1290   /*note the value is modem specific. it is same for Bolt/Bolt+ and thor modems*/

typedef struct{
   int32  start_range;
   int32  stop_range;
   uint32 config2_register_value;
}rfcommon_msm_adc_vcm_cal_tbl_type; 

typedef struct
{
   uint32 adc;
   uint32 config2_register_value;
}efs_adc_vcm_cal_data_type;

typedef struct
{
   uint32 adc;
   uint32 run_index;
   int32  curr_i;
   int32  curr_q;
   int32  adc_value_mv;
}efs_adc_vcm_cal_log_data_type;

/*To detect ADC VCM Calibration status*/
boolean rfcommon_msm_adc_vcm_cal_done;

efs_adc_vcm_cal_data_type      efs_adc_vcm_cal_data[RXLM_CHAIN_MAX];     /*to be written to efs file to store the calibrated data*/
efs_adc_vcm_cal_log_data_type  efs_adc_vcm_cal_log_data[RXLM_CHAIN_MAX*ADC_DC_CAL_MAX_ITERATIONS]; /*to be written to efs file for debug*/

boolean get_rfcommon_msm_adc_vcm_cal_tbl(const rfcommon_msm_adc_vcm_cal_tbl_type **rfcommon_msm_adc_vcm_cal_tbl_p);

#ifdef __cplusplus
}
#endif



#endif


