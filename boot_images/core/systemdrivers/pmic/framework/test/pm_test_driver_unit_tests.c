/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             PM DRIVER FRAMEWORK TEST

GENERAL DESCRIPTION
  This file contains Driver Framework test codes.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2014-15        by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:
$DateTime:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/15   mr      Created.
===========================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_clk.h"
#include "pm_coincell.h"
#include "pm_gpio.h"
#include "pm_mpp.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_lpg.h"
#include "pm_pon.h"
#include "pm_rgb.h"
#include "pm_rtc.h"

#include "pm_test_framework.h"


/*===========================================================================

                LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                GLOBAL AND EXTERN VARIABLE DEFINITIONS

===========================================================================*/
int g_status = 0;
int g_status_all_gpio_api = 0;
int g_status_all_mpp_api = 0;
int g_status_all_pon_api = 0;
int g_status_all_clk_api = 0;
int g_status_all_ldo_api = 0;
int g_status_all_smps_api = 0;
int g_status_all_lpg_api = 0;
int g_status_all_rbg_api = 0;
int g_status_all_rtc_api = 0;
int g_status_all_coincell_api = 0;


/*===========================================================================

                TEST FUNCTION DEFINITIONS

===========================================================================*/
void pmic_unit_testcases1 (void)
{
    pm_err_flag_type err1 = PM_ERR_FLAG__INVALID, err2 = PM_ERR_FLAG__INVALID, err3=PM_ERR_FLAG__INVALID, err4 =PM_ERR_FLAG__INVALID;
    pm_gpio_status_type status1, status2, status3, status4;

    err1 = pm_gpio_status_get (0, PM_GPIO_1, &status1);
    err2 = pm_gpio_status_get (0, PM_GPIO_2, &status2);
    err3 = pm_gpio_status_get (0, PM_GPIO_3, &status3);
    err4 = pm_gpio_status_get (0, PM_GPIO_4, &status4);

    g_status = err1 | err2 | err3 | err4;
}

void pmic_unit_testcases2 (void)
{
    pm_err_flag_type err1 = PM_ERR_FLAG__INVALID, err2 = PM_ERR_FLAG__INVALID, err3=PM_ERR_FLAG__INVALID, err4 =PM_ERR_FLAG__INVALID;

    err1 = pm_gpio_set_output_buffer_drive_strength (0, PM_GPIO_1, PM_GPIO_OUT_BUFFER_MEDIUM);
    err2 = pm_gpio_set_output_buffer_drive_strength (0, PM_GPIO_2, PM_GPIO_OUT_BUFFER_MEDIUM);
    err3 = pm_gpio_set_output_buffer_drive_strength (0, PM_GPIO_3, PM_GPIO_OUT_BUFFER_MEDIUM);
    err4 = pm_gpio_set_output_buffer_drive_strength (0, PM_GPIO_4, PM_GPIO_OUT_BUFFER_MEDIUM);

    g_status = err1 | err2 | err3 | err4;
}


void pm_testcase_gpio_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    // boolean b_status;
    pm_gpio_status_type gpio_status;

    for (i=0; i < g_pm_test_gpio_num; i++)
    {
        err = pm_gpio_status_get (0, (pm_gpio_perph_index)i, &gpio_status);
        err += pm_gpio_set_output_buffer_drive_strength (0, (pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_MEDIUM);
        err +=  pm_gpio_config_bias_voltage(0, (pm_gpio_perph_index)i, PM_GPIO_VIN4);
        err += pm_gpio_config_digital_input (0, (pm_gpio_perph_index)i, PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
                    PM_GPIO_VIN4, PM_GPIO_OUT_BUFFER_MEDIUM, PM_GPIO_SOURCE_SPECIAL_FUNCTION2);
        err += pm_gpio_config_digital_output (0, (pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN,
                    PM_GPIO_VIN4, PM_GPIO_SOURCE_SPECIAL_FUNCTION2, PM_GPIO_OUT_BUFFER_MEDIUM, 1);
        err += pm_gpio_config_digital_input_output (0, (pm_gpio_perph_index)i, PM_GPIO_SOURCE_SPECIAL_FUNCTION2,
                    PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST, PM_GPIO_VIN4, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN, PM_GPIO_OUT_BUFFER_MEDIUM);
        err += pm_gpio_set_volt_source (0, (pm_gpio_perph_index)i, PM_GPIO_VIN4);
        err += pm_gpio_set_output_buffer_configuration (0, (pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN);
        err += pm_gpio_set_inversion_configuration (0, (pm_gpio_perph_index)i, 1);
        err += pm_gpio_set_current_source_pulls (0, (pm_gpio_perph_index)i, PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST);
        err += pm_gpio_set_ext_pin_config (0, (pm_gpio_perph_index)i, PM_GPIO_EXT_PIN_DISABLE);
        err += pm_gpio_set_source_configuration (0, (pm_gpio_perph_index)i, PM_GPIO_SOURCE_SPECIAL_FUNCTION2);

        //These following APIs are not implemented in boot driver in bear family

        //err += pm_gpio_configure_type (0, (pm_gpio_perph_index)i, PM_GPIO_DIG_IN_DIG_OUT);
        //err += pm_gpio_enable (0, i, 1); //TODO - it can be combined with get
        //err += pm_gpio_irq_set_trigger (0, (pm_gpio_perph_index)i,PM_IRQ_TRIGGER_FALLING_EDGE);
        //err += pm_gpio_irq_clear (0, (pm_gpio_perph_index)i);
        //err = pm_gpio_irq_status (0, (pm_gpio_perph_index)i, PM_IRQ_STATUS_RT, &b_status);
    }

    g_status_all_gpio_api = err;
}

pm_err_flag_type pm_mpp_set_list_mpp_with_shunt_cap(unsigned pmic_chip, pm_mpp_perph_index mpp);
pm_err_flag_type pm_mpp_get_mpp_with_shunt_cap_list_status_for_device(unsigned pmic_chip, pm_mpp_perph_index mpp, uint32 *shuntList);

void pm_testcase_mpp_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    pm_mpp_status_type mpp_status;
    uint32 shuntlist_out;

    for (i=0; i < g_pm_test_mpp_num; i++)
    {
        err = pm_mpp_config_digital_input (0, (pm_mpp_perph_index)i, PM_MPP__DLOGIC__LVL_VIO_4, (pm_mpp_dlogic_in_dbus_type)i); //PM_MPP__DLOGIC_IN__DBUS3);
        err += pm_mpp_config_digital_output (0, (pm_mpp_perph_index)i, PM_MPP__DLOGIC__LVL_VIO_4, PM_MPP__DLOGIC_OUT__CTRL_MPP);
        err += pm_mpp_config_digital_inout (0, (pm_mpp_perph_index)i, PM_MPP__DLOGIC__LVL_VIO_4, PM_MPP__DLOGIC_INOUT__PUP_30K);
        err += pm_mpp_config_analog_input (0, (pm_mpp_perph_index)i, PM_MPP__AIN__CH_ABUS2);
        err += pm_mpp_config_analog_output (0, (pm_mpp_perph_index)i, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH);
        err += pm_mpp_config_i_sink (0, (pm_mpp_perph_index)i, PM_MPP__I_SINK__LEVEL_25mA, PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH);
        err += pm_mpp_config_atest (0, (pm_mpp_perph_index)i, PM_MPP__CH_ATEST4);
        err += pm_mpp_set_list_mpp_with_shunt_cap (0, (pm_mpp_perph_index)i);
        err += pm_mpp_get_mpp_with_shunt_cap_list_status_for_device (0, (pm_mpp_perph_index)i, &shuntlist_out);
        err += pm_mpp_status_get (0, (pm_mpp_perph_index)i, &mpp_status);
        err += pm_mpp_enable (0, (pm_mpp_perph_index)i, 1); //it can be made automatic based on get
    }

    g_status_all_mpp_api = err;
}

void pm_testcase_pon_all_api(void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    pm_pon_poff_reason_type poff_reason;
    pm_pon_warm_reset_reason_type warmreset_reason;
    pm_pon_soft_reset_reason_type softreset_reason;
    uint64 all_reasons;
    boolean b_status;

    err = pm_pon_overtemp_reset_cfg (0, PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET); //it can be typecasted and run in a for loop (pm_pon_reset_cfg_type)i
    err += pm_pon_ps_hold_cfg (0, PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET);
    err += pm_pon_reset_source_cfg (0, PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, 3072, 1000, PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET);
    err += pm_pon_reset_source_ctl (0, PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, PM_ON); //(pm_on_off_type)1 would also do
    err += pm_pon_stage3_reset_source_cfg (0, PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, 64);

    err += pm_pon_get_poff_reason (0, &poff_reason);
    err += pm_pon_get_warm_reset_reason (0, &warmreset_reason);
    err += pm_pon_get_soft_reset_reason (0, &softreset_reason);
    err += pm_pon_get_all_pon_reasons (0, &all_reasons);

    err += pm_pon_irq_enable (0, PM_PON_IRQ_KPDPWR_BARK, 1);
    err += pm_pon_irq_clear (0, PM_PON_IRQ_KPDPWR_BARK);
    err += pm_pon_irq_set_trigger (0, PM_PON_IRQ_KPDPWR_BARK, PM_IRQ_TRIGGER_RISING_EDGE);
    err += pm_pon_irq_status (0, PM_PON_IRQ_KPDPWR_BARK, PM_IRQ_STATUS_PENDING, &b_status);

    err += pm_pon_config_uvlo (0, 3000, 300); // range1: 2875 - 3225; range2: 175 - 425

    err += pm_pon_warm_reset_status (0, &b_status); // TODO - need to declare it in header
    err += pm_pon_warm_reset_status_clear (0); //TODO - need to declare it in header

    g_status_all_pon_api = err;
}

void pm_testcase_clk_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;

    for (i = 0; i < PM_ALL_CLKS; i++)
    {
        err = pm_clk_drv_strength (0, (pm_clk_type)i, PM_CLK_DRV_STRENGTH_3X); //(pm_clk_drv_strength_type)i);
        err += pm_clk_sw_enable (0, (pm_clk_type)i, (pm_on_off_type)1);
        err += pm_clk_pin_ctrled (0, (pm_clk_type)i, (pm_on_off_type)1);
        err += pm_clk_pull_down (0, (pm_clk_type)i,  (pm_on_off_type)1);
    }

    g_status_all_clk_api = err;
}

void pm_testcase_ldo_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    uint32 volt_level1;
    uint32 volt_level2;
    pm_sw_mode_type mode_status;

    for (i=0; i < g_pm_test_ldo_num; i++)
    {
        err = pm_ldo_sw_mode (0, i, PM_SW_MODE_NPM); //(pm_sw_mode_type)i);
        err += pm_ldo_sw_enable (0, i, (pm_on_off_type)1);
        err += pm_ldo_pull_down (0, i, (pm_on_off_type)1);
        err += pm_ldo_sw_mode_status (0, i,  &mode_status);
        err += pm_ldo_pin_ctrled (0, i,  1); //TODO what should be the third parameter
        //err += pm_ldo_volt_level (0, i,  1500000);
        err += pm_ldo_volt_level_status (0, i,  &volt_level1);
        err += pm_ldo_volt_level (0, i,  volt_level1+25000);
        err += pm_ldo_volt_level_status (0, i,  &volt_level2);
    }

    g_status_all_ldo_api = err;
}

void pm_testcase_smps_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    uint32 volt_level1, volt_level2;
    uint32 ilim_level1, ilim_level2;
    pm_sw_mode_type mode_status;
    pm_smps_switching_freq_type freq;

    for (i=0; i < g_pm_test_smps_num; i++)
    {
        err = pm_smps_sw_mode (0, i, PM_SW_MODE_NPM); //(pm_sw_mode_type)i);
        err += pm_smps_sw_enable (0, i, (pm_on_off_type)1);
        err += pm_smps_pull_down (0, i, (pm_on_off_type)1);
        err += pm_smps_sw_mode_status (0, i,  &mode_status);
        err += pm_smps_pin_ctrled (0, i,  1); //TODO what should be the third parameter

        //err += pm_ldo_volt_level (0, i,  1500000);
        err += pm_smps_volt_level_status (0, i,  &volt_level1);
        err += pm_smps_volt_level (0, i,  volt_level1+25000);
        err += pm_smps_volt_level_status (0, i,  &volt_level2);

        //ilim APIs need LUT be defined - in 8909 there is no LUT defined, same in 8916
        err += pm_smps_inductor_ilim_status (0, i, &ilim_level1 ,PM_ILIM_SMPS_PWM_MODE);
        err += pm_smps_inductor_ilim (0, i, ilim_level1+100 ,PM_ILIM_SMPS_PWM_MODE);
        err += pm_smps_inductor_ilim_status (0, i, &ilim_level2 ,PM_ILIM_SMPS_PWM_MODE);

        err += pm_smps_inductor_ilim (0, i, 900 ,PM_ILIM_SMPS_PWM_MODE);//3rd param range  3500 to 600
        err += pm_smps_inductor_ilim_status (0, i, &ilim_level1 ,PM_ILIM_SMPS_PWM_MODE);

        err += pm_smps_quiet_mode (0, i, PM_QUIET_MODE__SUPER_QUIET, 1000); //TODO what should be the last param in volt in mv
        err += pm_smps_switching_freq (0, i, PM_CLK_2p4_MHz);
        err += pm_smps_switching_freq_status (0, i, &freq);
    }

    g_status_all_smps_api = err;
}

void pm_testcase_lpg_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    pm_lpg_status_type status;
    int pwm_value;

    for (i=0; i < g_pm_test_lpg_num; i++)
    {
        err = pm_lpg_pwm_enable (0, (pm_lpg_chan_type)i, 1); //(pm_sw_mode_type)i);
        err += pm_lpg_pwm_output_enable (0, (pm_lpg_chan_type)i, 1);

        err += pm_lpg_pwm_set_pwm_value (0, (pm_lpg_chan_type)i, 50);

        err += pm_lpg_pwm_set_pre_divide (0, (pm_lpg_chan_type)i, PM_LPG_PWM_PRE_DIV_5, PM_LPG_PWM_FREQ_EXPO_4);
        err += pm_lpg_pwm_clock_sel (0, (pm_lpg_chan_type)i, PM_LPG_PWM_19_2_MHZ);
        err += pm_lpg_set_pwm_bit_size (0, (pm_lpg_chan_type)i, PM_LPG_PWM_8BIT);

        err += pm_lpg_pwm_src_select (0, (pm_lpg_chan_type)i, PM_LPG_PWM_SRC_PWM_REGISTER);
        err += pm_lpg_config_pwm_type (0, (pm_lpg_chan_type)i, 1, 1, 1, PM_LPG_PHASE_STAG_SHIFT_180_DEG);
        err += pm_lpg_pattern_config (0, (pm_lpg_chan_type)i, 1, 1, 1, 1, 1);
        err += pm_lpg_lut_config_set (0,  50, 100); //2nd param (0-64), 3rd PWM value

        err += pm_lpg_lut_config_get (0,  50, &pwm_value);
        err += pm_lpg_lut_config_set_array (0,  50, &pwm_value, 4);
        err += pm_lpg_lut_config_get_array (0,  50, &pwm_value, 4);

        err += pm_lpg_pwm_ramp_generator_start (0,  (pm_lpg_chan_type)i);
        err += pm_lpg_pwm_lut_index_set (0,  (pm_lpg_chan_type)i, 20, 60);
        err += pm_lpg_config_pause_time (0,  (pm_lpg_chan_type)i, 25, 50, 40);

        err += pm_lpg_get_status (0,  (pm_lpg_chan_type)i, &status);
    }

    g_status_all_lpg_api = err;
}

void pm_testcase_rgb_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    pm_rgb_status_type status;

    for (i=0; i < g_pm_test_rgb_num; i++)
    {
        err = pm_rgb_set_voltage_source (0, (pm_rgb_which_type)i, PM_RGB_VOLTAGE_SOURCE_5V); //(pm_rgb_voltage_source_type)i);
        err += pm_rgb_enable (0, (pm_rgb_which_type)i, 0xFF, 1, 1);//third param is mask
        err += pm_rgb_get_status (0, (pm_rgb_which_type)i, &status);
    }

    g_status_all_rbg_api = err;
}

void pm_testcase_rtc_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_rtc = 1;    /* Target Config/DAL Prop for RTC is not defined */

    uint32 timer_value;

    for (i=0; i < max_rtc; i++)
    {
        err = pm_rtc_get_time (0, &timer_value);
    }

    g_status_all_rtc_api = err;
}

void pm_testcase_coincell_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    boolean coincell_stat = 0;

    err += pm_coincell_get_status (0,  &coincell_stat);
    err += pm_coincell_enable (0,  PM_ON);
    err += pm_coincell_set_cc_current_limit_resistor (0,  PM_COINCELL_RSET_2K1);
    err += pm_coincell_set_cc_charging_voltage (0,  PM_COINCELL_VSET_2V5);

    g_status_all_coincell_api = err;
}


void pm_test_unit_testcases (void)
{
    pm_testcase_gpio_all_api ();
    pm_testcase_mpp_all_api ();
    pm_testcase_pon_all_api ();
    pm_testcase_clk_all_api ();
    pm_testcase_ldo_all_api ();
    pm_testcase_smps_all_api();
    pm_testcase_lpg_all_api ();
    pm_testcase_rgb_all_api ();
    pm_testcase_rtc_all_api ();
    pm_testcase_coincell_all_api ();

    pmic_unit_testcases1 ();
    pmic_unit_testcases2 ();
}
