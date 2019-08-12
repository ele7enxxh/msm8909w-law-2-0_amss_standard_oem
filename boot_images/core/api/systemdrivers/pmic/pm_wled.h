#ifndef __PM_WLED_H__
#define __PM_WLED_H__

/*! \file  pm_uefi_wled.h 
 *  \n
 *  \brief  PMIC-WLED MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC WLED module.
 *
 *  \n &copy; Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated. All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_wled.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/20/14   al      Architecture update
05/16/14   al      Add AMOLED, PFM and PSM
03/26/14   al      8994 WLED support
09/17/13   al      Adding WLED IRQ and updating get_status 
01/14/13   al      Replacing enums with type uint32 and removing 'dev_' from api name
12/21/12   al      Updating eumn for SWITCHING FREQUENCY  
10/29/12   al      removed register structs to make it generic and added wled get status api
08/15/12   al	   Changed according to naming conventions
7/27/12    al	   Renamed all Enums,added device index and resource index
03/05/12   sm      Changes parameters for pm_uefi_wled_en_sync
02/08/12   sm      New file
===========================================================================*/

/*===========================================================================
                INCLUDE FILES FOR MODULE
===========================================================================*/
//#include "pm_lib_err.h"
//#include "pm_irq.h"
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"



/*===========================================================================
                     GLOBAL TYPE DEFINITIONS
===========================================================================*/
typedef enum {
  PM_WLED_ILED_1,
  PM_WLED_ILED_2,
  PM_WLED_ILED_3,
  PM_WLED_ILED_4,
  PM_WLED_ILED_ALL,
  PM_WLED_ILED_INVALID
} pm_wled_iled_type;

typedef enum {
  PM_WLED_NONE,
  PM_WLED_LED1,
  PM_WLED_LED2,
  PM_WLED_LED3,
  PM_WLED_LED4,
  PM_WLED_ALL,
  PM_WLED_INVALID
} pm_wled_led_type;


typedef enum {
  PM_WLED_WARMUP_DELAY_0US,
  PM_WLED_WARMUP_DELAY_6P7US,
  PM_WLED_WARMUP_DELAY_13P3US,
  PM_WLED_WARMUP_DELAY_26P7US,
  PM_WLED_WARMUP_DELAY_INVALID
} pm_wled_warmup_dly_type;

typedef enum {
  PM_WLED_PWM_1,
  PM_WLED_PWM_2,
  PM_WLED_PWM_3,
  PM_WLED_PWM_INVALID
} pm_wled_pwm_type;

typedef enum {
  PM_WLED_MODE_12b_SDM = 0,
  PM_WLED_MODE_9b_PWM,
  PM_WLED_MODE_INVALID
} pm_wled_pwm_mode_type;


typedef enum {
  PM_WLED_BST_MAX_DUTY_SUB_26 = 0, //(1/Fsw - 26ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_52,     //(1/Fsw - 52ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_104,    //(1/Fsw - 104ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_156,    //(1/Fsw - 156ns)*Fsw
  PM_WLED_BST_MAX_DUTY_SUB_INVALID
} pm_wled_bst_max_duty_type;

typedef enum {
  PM_WLED_MODE_CLK_19P2MHz = 0, // 19.2MHz
  PM_WLED_MODE_CLK_4P8MHz,      //  4.8MHz
  PM_WLED_MODE_CLK_2P4MHz,      //  2.4MHz
  PM_WLED_MODE_CLK_1P2MHz,      //  1.2MHz
  PM_WLED_MODE_CLK_INVALID
} pm_wled_mode_clk_type;

typedef enum {
  PM_WLED_MODE_DIM_MODE_ANALOG = 0,
  PM_WLED_MODE_DIM_MODE_DIGITAL,
  PM_WLED_MODE_DIM_MODE_INVALID
} pm_wled_mode_dim_mode_type;

typedef enum {
  PM_WLED_OVP_THR_31V=0,         //31V
  PM_WLED_OVP_THR_29P5V,         //29.5V
  PM_WLED_OVP_THR_19P4V,         //19.4V
  PM_WLED_OVP_THR_17P8V,         //17.8V
  PM_WLED_OVP_THR_7P84V=0,       //7.84V
  PM_WLED_OVP_THR_7P65V,          //7.65V
  PM_WLED_OVP_THR_6P11V,         //6.11V
  PM_WLED_OVP_THR_5P80V,         //5.8V
  PM_WLED_OVP_THR_INVALID
} pm_wled_ovp_threshold_type;


typedef enum {
  PM_WLED_BST_ILIMIT_105mA,     //105mA
  PM_WLED_BST_ILIMIT_385mA,     //385mA
  PM_WLED_BST_ILIMIT_660mA,     //660mA
  PM_WLED_BST_ILIMIT_980mA,     //980mA
  PM_WLED_BST_ILIMIT_1150mA,    //1150mA
  PM_WLED_BST_ILIMIT_1420mA,    //1420mA
  PM_WLED_BST_ILIMIT_1700mA,    //1700mA
  PM_WLED_BST_ILIMIT_1980mA,    //1980mA
  PM_WLED_BST_ILIMIT_INVALID
} pm_wled_bst_ilimit_type;


typedef enum {
  PM_WLED_SLEW_RATE_20nS = 0,  //20 nS
  PM_WLED_SLEW_RATE_15nS,      //15 nS
  PM_WLED_SLEW_RATE_10nS,      //10 nS
  PM_WLED_SLEW_RATE_5nS,       // 5 nS
  PM_WLED_SLEW_RATE_1nS=0,     // 1.0 nS
  PM_WLED_SLEW_RATE_1p5nS,     // 1.5 nS
  PM_WLED_SLEW_RATE_2nS,       // 2.0 nS
  PM_WLED_SLEW_RATE_2p5nS,     // 2.5 nS
  PM_WLED_SLEW_RATE_INVALID
} pm_wled_slew_rate_type;

typedef enum {
    PM_WLED_IRQ_ILIM_FAULT ,
    PM_WLED_IRQ_OVP_FAULT,
    PM_WLED_IRQ_SC_FAULT,
    PM_WLED_IRQ_INVALID
} pm_wled_irq_type;


typedef struct
{
   boolean                     led1_modulator_enable;
   boolean                     led2_modulator_enable;
   boolean                     led3_modulator_enable;
   boolean                     led4_modulator_enable;

   boolean                     led1_current_sink_enable;
   boolean                     led2_current_sink_enable;
   boolean                     led3_current_sink_enable;
   boolean                     led4_current_sink_enable;

   uint32                      slope_trim;

   pm_wled_mode_clk_type      mode_clk; 
   boolean                    en_phase_stagger;
   pm_wled_mode_clk_type      accum_clk;
   boolean                    en_hybrid_dim;
   pm_wled_pwm_mode_type      pwm_mode;
   pm_wled_mode_dim_mode_type dim_mode;
   pm_wled_bst_max_duty_type   bst_max_duty;
   uint32                      set_fsw_ctrl;
   uint32                      ramp_time;
   pm_wled_bst_ilimit_type     bst_ilimit;
   pm_wled_slew_rate_type      slew_rate;
   uint32                      vref_ctrl;
   pm_wled_warmup_dly_type     warmup_dly;
   pm_wled_ovp_threshold_type  ovp_thr;
   uint32                      led1_full_scale_current;
   uint32                      led2_full_scale_current;
   uint32                      led3_full_scale_current;
   uint32                      led4_full_scale_current;
   uint32                      led1_brightness;
   uint32                      led2_brightness;
   uint32                      led3_brightness;
   uint32                      led4_brightness;
}pm_wled_status_type;
/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/**
 * @brief This function enables WLED module
 * 
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_module
(
  uint32 pmic_device_index, boolean enable 
);

/**
 * @brief This function enables digital modulator generator and modulator input
 *        for each current sink.
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_modulator
(
  uint32 pmic_device_index, pm_wled_led_type led_type
);

/**
 * @brief This function enables control for LED current sink outputs
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_enable_current_sink
(
  uint32 pmic_device_index, pm_wled_led_type led_type
);

/**
 * @brief This function  sets delay of activation of 
 *        WLED_FULL_SCALE_SETTING register to IDAC relative
 *        to write of WLED_SYNC bit. WLED_SYNC is used to
 *        synchronize IDAC and modulator updates
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  led_type:
 *				      refer enum pm_wled_led_type
 * @param[in]  delay_usec:
 *               led delay in micro seconds. Valid values are:
 *                0,200,400,600,800,1000,1200,1400
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_sync_delay
(
  uint32           pmic_device_index, 
  pm_wled_led_type led_type, 
  uint32           delay_usec
);

/**
 * @brief This function LED current sink number 
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  led_type:
 *				    refer enum pm_wled_led_type
 * @param[in]  current_milliamp:
 *                 current value in milliamp. Valid range is 0 to 25.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_current_sink
(
  uint32 pmic_device_index, 
  pm_wled_led_type led_type, 
  uint32 current_milliamp
);

/**
 * @brief This function enables use of pwm_in_ext as pwm dimming control
 * 
 * @param[in] enable
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_ext_pwm
(
 uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable
);

/**
 * @brief This function selects 3-bit control for the cap of the slope 
 *        compensation ramp
 *
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				    valid value 0 
 * @param[in] slope_trim:
 *                  slope trim value. Valid range is 3000 to 6500 nano farad
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_slope_trim
(
  uint32 pmic_device_index, uint32 resource_index, uint32 slope_trim
);

/**
 * @brief This function sets 12b duty cycle control for indicated LED digital 
 *        modulator control.
 * 
 * @param[in] led_type
 * @param[in] data
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_led_duty_cycle
(
  uint32 pmic_device_index, pm_wled_led_type led_type, uint16 data
);

/**
 * @brief This function sets slope compensation adjustment for inductor 
 *        component value.
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				    valid value 0 
 * @param[in] slope_comp
 *                  slope comp trim value in micro-amp. Valid
 *                  values are 3,4,5
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_slope_comp
(
  uint32 pmic_device_index, uint32 resource_index, uint32 slope_comp
);

/**
 * @brief This function enables content adaptive backlight control
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_cabc
(
  uint32 pmic_device_index, pm_wled_led_type led_type
);

/**
 * @brief This function enables LED sync
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_sync
(
 uint32 pmic_device_index,  
 pm_wled_led_type led_type,
  boolean          enable
);

/**
 * @brief This function selects internal digital modulator type
 * 
 * @param[in] pwm_mode_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_pwm_mode
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_pwm_mode_type pwm_mode_type
);


/**
 * @brief This function configures different sink modulation 
 *        properties
 * 
 * @param[in] pmic_device_index 
 * @param[in] mode_clk 
 * @param[in] en_phase_stagger 
 * @param[in] accum_clk 
 * @param[in] en_hybrid_dim 
 * @param[in] pwm_mode 
 * @param[in] dim_mode 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_config_sink_modulation
(
  uint32 pmic_device_index,  
  pm_wled_mode_clk_type mode_clk, 
  boolean en_phase_stagger, 
  pm_wled_mode_clk_type accum_clk, 
  boolean en_hybrid_dim,
  pm_wled_pwm_mode_type pwm_mode, 
  pm_wled_mode_dim_mode_type dim_mode
);


/**
 * @brief This function selects maximum duty cycle control for boost.
 * 
 * @param[in] bst_max_duty_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_bst_max_duty
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_bst_max_duty_type bst_max_duty_type
);

/**
 * @brief This function selects boost switching frequency control
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				    valid value 0 
 * @param[in] fsw_khz:
 *                  boost switching frequency in KHz.
 *                  Acceptable values are: 9600, 4800, 3200, 2400, 1920, 1600, 1370, 1200, 1060,  960,  872,  800, 738, 685, 640, 600
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_fsw_ctrl
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  uint32   fsw_khz
);

/**
 * @brief This function controls WLED bias generator warm-up time.
 * 
 * @param[in] warmup_dly_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_warmup_dly
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_warmup_dly_type warmup_dly_type
);

/**
 * @brief This function selects OVP threshold
 * 
 * @param[in] ovp_threshold_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_ovp_thr
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_ovp_threshold_type ovp_threshold_type
);

/**
 * @brief This function sets time to ramp Ilim from 0 to 100% of Ilim threshold
 *        during soft start
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				       valid value 0
 * @param[in] time_usec:
 *                  ramp time in micro second. Valid values are: 93 to 750 microsecods
 *                  
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_set_ramp_time
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  uint32 time_usec
);

/**
 * @brief This function enable/disable soft start Ilim ramp after WLED module 
 *        enable
 * 
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_soft_start
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  boolean enable
);

/**
 * @brief This function selects boost current limit
 * 
 * @param[in] bst_ilimit_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_bst_ilimit
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_bst_ilimit_type bst_ilimit_type
);

/**
 * @brief This function forces selection of a LED output as feedback node for 
 *        the boost
 * 
 * @param[in] led_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_force_vmin
(
  uint32 pmic_device_index,
  pm_wled_led_type led_type
);

/**
 * @brief This function selects slew rate
 * 
 * @param[in] slew_rate_type
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_slew_rate
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  pm_wled_slew_rate_type slew_rate_type
);

/**
 * @brief This function selects compensation resistor
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in] rz_kohm:
 *                 compensation resistor value in kilo-ohm. 
 *                 Recommended values are:  20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,320
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_rz
(
  uint32 pmic_device_index, 
  uint32 rz_kohm
);

/**
 * @brief This function selects gm for the gm stage
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in] gm_sel
 *                 conductance value. Valid values are within range 0 to 15
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_gm
(
  uint32 pmic_device_index,
  uint32   gm_sel
);

/**
 * @brief This function enables sleep
 * 
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_sleepb
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  boolean enable
);

/**
 * @brief This function selects reference voltage for boost feedback
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in]  resource_index:
 *				    valid value 0  
 * @param[in]  milli_volt
 *             reference voltage in milli volt. 
 *             Valid values are 300, 400, 500, 600, 700, 800, 900, 1000 mV
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_vref_ctrl
(
  uint32 pmic_device_index, 
  uint32 resource_index,
  uint32 milli_volt
);

/**
 * @brief This function sets compensator capacitance
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in] cz_picofarad
 *                  compensator capacitance in pico-farad. The
 *                  valid values are 50,60,70,80 pf.
 * @return  pm_err_flag_type PM_ERR_FLAG__FEATURE_NOT_SUPPORTED 
 *          = Feature not available on this version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_comp_cap
(
  uint32 pmic_device_index, 
  uint32 cz_picofarad
);


/**
 * @brief This function sets high pole capacitance
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic
 * @param[in] cp_picofarad
 *                  pole capacitance value in pico-farad. The acceptable values are 0,1,2,3 pf.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_sel_high_pole_cap
(
  uint32 pmic_device_index, 
  uint32 cp_picofarad
);


/**
 * @brief This function enables/disables amoled
 * 
 * @param[in]  pmic_device_index:
 *			        0 for primary pmic; 1 for secondary
 * @param[in] enable
 *                  TRUE to enable and FALSE to disable.
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_en_amoled
(
   uint32 pmic_device_index,
   boolean enable
);

/**
 * @brief This function gets the current status for wled:

 * 
 * @param[in] pmic_device_index : Pmic Chip device index.
 *                                For Primary :0
 *      
 *             
 * @param[out] wled_status       :refer pm_wled_status_type for more info  
 *                                pm_wled_led_type set for enabling modulator;
 *                                pm_wled_led_type set for enabling current sink;
 *                                value set for slope trim;
 *                                pm_wled_pwm_mode_type set for pwm mode;
 *                                pm_wled_mod_clk_type set for mod clk;
 *                                pm_wled_mod_dim_mode_type set for mod dim mode;
 *                                pm_wled_bst_max_duty_type set for bst max duty;
 *                                value set for fsw ctrl;
 *                                value set for ramp time;
 *                                pm_wled_bst_ilimit_type set for bst ilimit;
 *                                pm_wled_slew_rate_type set for slew rate;
 *                                pm_wled_vref_ctrl_type set for vref ctrl;
 *                                pm_wled_warmup_dly_type set for warmup dly;
 *                                pm_wled_ovp_threshold_type set for ovp thr; 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_get_status
(
  uint32 pmic_device_index, 
  pm_wled_status_type *wled_status
);

/**
 * @brief This function enables or disables WLED irq
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding WLED interrupt 
 *                FALSE:Disable corresponding WLED interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_enable(uint32 device_index, pm_wled_irq_type irq, boolean enable);


/**
 * @brief This function clears the WLED irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_clear(uint32  device_index, pm_wled_irq_type irq);


/**
 * @brief This function configures the WLED irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_uefi_irq.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_set_trigger(uint32 device_index, pm_wled_irq_type irq, pm_irq_trigger_type trigger);

/**
 * @brief This function reads the WLED irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                WLED irq type. Refer enum pm_wled_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_uefi_irq.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_wled_irq_status(uint32 device_index, pm_wled_irq_type irq, pm_irq_status_type type, boolean *status);



/**
 * @brief This function configures pulse skipping modes 
 * 
 * @param[in] device_index    : PMIC device index. 0 for primary and 1 for secondary
 * @param[in] hyst_lvl_high   : Hysterisis level. High is 50mv and low is 25mv
 * @param[in] ref_volt_mv     : PSM reference voltage in milli volt. Range is 400 to 750mv
 * @param[in] psm_en          : enable/disable pulse skipping mode 
 * 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 */
pm_err_flag_type pm_wled_config_psm(uint32 device_index, boolean hyst_lvl_high, uint32 ref_volt_mv, boolean psm_en);

 /**
 * @brief This function configures pulse frequency modes 
 * 
 * @param[in] device_index      : PMIC device index. 0 for primary and 1 for secondary
 * @param[in] hyst_lvl_high     : Hysterisis level. High is 50mv(WLED) and 12.5mV (AMOLED) and low is 50mv(WLED) and 6.25mv(AMOLED)
 * @param[in] nfet_off_time     : PFM NFET off time in nano seconds. Valid range is 110ns to 480ns. 
 * @param[in] current_limit_ma  : PFM current limit in milli amp. Valid range is 130 to 340 milli amp. 
 * @param[in] pfm_en            : enable/disable pulse frequency mode
 * 
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 */
pm_err_flag_type pm_wled_config_pfm(uint32 device_index, boolean hyst_lvl_high, uint32 nfet_off_time, uint32 current_limit_ma, boolean pfm_en);


#endif /* __PM_UEFI_WLED_H__ */

