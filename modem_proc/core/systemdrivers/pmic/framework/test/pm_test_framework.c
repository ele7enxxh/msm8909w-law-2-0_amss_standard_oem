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
01/30/15   mr      Removed PMIC KW P1 errors (CR-789040)
08/06/10   akt     Created. (CR-713705)
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_npa.h"
#include "pmapp_npa.h"

#include "pm_target_information.h"
#include "pm_dal_prop_ids.h"

#include "pm_clk.h"
#include "pm_gpio.h"
#include "pm_mpps.h"
#include "pm_rtc.h"
#include "pm_xo_core.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
typedef enum
{
    PM_DRIVER_TEST_L1,
    PM_DRIVER_TEST_L2,
    PM_DRIVER_TEST_L3,
} pm_test_level;

typedef enum
{
    PM_NO_TEST,
    /* LDO */
    PM_LDO_SW_ENABLE_STATUS,
    PM_LDO_SW_MODE_STATUS,
    PM_LDO_VOLT_LEVEL_STATUS,
    PM_LDO_VREG_OK_STATUS,
    /* SMPS */
    PM_SMPS_SW_ENABLE_STATUS,
    PM_SMPS_SW_MODE_STATUS,
    PM_SMPS_VOLT_LEVEL_STATUS,
    PM_SMPS_VREG_OK_STATUS,
    /* CLK */
    PM_CLK_DRV_STRENGTH,
    PM_CLK_SW_ENABLE,
} pm_test_apis;


int g_status = 0;
int g_test_count = 0;
int g_status_all_gpio_api = -1;
int g_status_all_mpp_api = -1;
int g_status_all_clk_api = -1;
int g_status_all_ldo_api = -1;
int g_status_all_smps_api = -1;
int g_status_all_rtc_api = -1;
int g_status_all_bua_api = -1;

extern pm_err_flag_type pmapp_pwr_set_vdd_mss(uint32 voltage);

void pm_testcase_gpio_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_gpio = pm_target_information_get_periph_count_info(PM_PROP_GPIO_NUM, 0);
    boolean b_status = FALSE;
    pm_gpio_status_type gpio_status;

    for (i=0; i < max_gpio; i++)
    {
        err = pm_gpio_status_get ((pm_gpio_perph_index)i, &gpio_status);

        err +=  pm_gpio_config_bias_voltage((pm_gpio_perph_index)i, PM_GPIO_VIN4);

        err += pm_gpio_config_digital_input ((pm_gpio_perph_index)i, PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
                    PM_GPIO_VIN4, PM_GPIO_OUT_BUFFER_MEDIUM, PM_GPIO_SOURCE_SPECIAL_FUNCTION2);

        err += pm_gpio_set_output_buffer_drive_strength ((pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_MEDIUM);

        err +=  pm_gpio_config_bias_voltage((pm_gpio_perph_index)i, PM_GPIO_VIN4);

        err += pm_gpio_config_digital_output ((pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS,
                    PM_GPIO_VIN4, PM_GPIO_SOURCE_SPECIAL_FUNCTION2, PM_GPIO_OUT_BUFFER_MEDIUM, 1);

        err += pm_gpio_config_digital_input_output ((pm_gpio_perph_index)i, PM_GPIO_SOURCE_SPECIAL_FUNCTION2,
                    PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST, PM_GPIO_VIN4, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS, PM_GPIO_OUT_BUFFER_MEDIUM);

        err += pm_gpio_set_voltage_source ((pm_gpio_perph_index)i, PM_GPIO_VIN4);

        err += pm_gpio_set_output_buffer_configuration ((pm_gpio_perph_index)i, PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN_PMOS);

        err += pm_gpio_set_inversion_configuration ((pm_gpio_perph_index)i, 1);

        err += pm_gpio_set_current_source_pulls ((pm_gpio_perph_index)i, PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST);

        err += pm_gpio_set_ext_pin_config ((pm_gpio_perph_index)i, PM_GPIO_EXT_PIN_DISABLE);

        err += pm_gpio_set_source_configuration ((pm_gpio_perph_index)i, PM_GPIO_SOURCE_SPECIAL_FUNCTION2);

        //*
        //These following APIs are not implemented in boot driver in bear family
        err += pm_gpio_irq_set_trigger (0, (pm_gpio_perph_index)i,PM_IRQ_TRIGGER_FALLING_EDGE);
        err += pm_gpio_irq_clear (0, (pm_gpio_perph_index)i);
        err = pm_gpio_irq_status (0, (pm_gpio_perph_index)i, PM_IRQ_STATUS_RT, &b_status);
        //*/
    }

    g_status_all_gpio_api = err;
}

void pm_testcase_mpp_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_mpp = pm_target_information_get_periph_count_info(PM_PROP_MPP_NUM, 0);
    pm_mpp_status_type mpp_status;
    uint32 shuntlist_out;

    for (i=0; i < max_mpp; i++)
    {
        err = pm_mpp_config_digital_input ((pm_mpp_which_type)i, PM_MPP__DLOGIC__LVL_VIO_4, (pm_mpp_dlogic_in_dbus_type)i); //PM_MPP__DLOGIC_IN__DBUS3);
        err += pm_mpp_config_digital_output ((pm_mpp_which_type)i, PM_MPP__DLOGIC__LVL_VIO_4, PM_MPP__DLOGIC_OUT__CTRL_MPP);
        err += pm_mpp_config_digital_inout ((pm_mpp_which_type)i, PM_MPP__DLOGIC__LVL_VIO_4, PM_MPP__DLOGIC_INOUT__PUP_30K);
        err += pm_mpp_config_analog_input ((pm_mpp_which_type)i, PM_MPP__AIN__CH_ABUS2);
        err += pm_mpp_config_analog_output ((pm_mpp_which_type)i, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH);
        err += pm_mpp_config_i_sink ((pm_mpp_which_type)i, PM_MPP__I_SINK__LEVEL_25mA, PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH);
        err += pm_mpp_config_atest ((pm_mpp_which_type)i, PM_MPP__CH_ATEST4);
        err += pm_mpp_set_list_mpp_with_shunt_cap ((pm_mpp_which_type)i);
        err += pm_mpp_get_mpp_with_shunt_cap_list_status_for_device ((pm_mpp_which_type)i, &shuntlist_out);

        err += pm_mpp_status_get ((pm_mpp_which_type)i, &mpp_status);
    }

    g_status_all_mpp_api = err;
}

void pm_testcase_clk_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_clk = PM_ALL_CLKS;

    for (i=0; i < max_clk; i++)
    {
        err = pm_clk_drv_strength (0, i, PM_CLK_BUFF_OUT_DRV__4X);
        /*
        err = pm_clk_drv_strength (0, (pm_clk_type)i, PM_CLK_DRV_STRENGTH_3X); //(pm_clk_drv_strength_type)i);
        err += pm_clk_sw_enable (0, (pm_clk_type)i, (pm_on_off_type)1);
        err += pm_clk_pin_ctrled (0, (pm_clk_type)i, (pm_on_off_type)1);
        err += pm_clk_pull_down (0, (pm_clk_type)i,  (pm_on_off_type)1);
        */
    }

    g_status_all_clk_api = err;
}

int g_max_ldo = 0;
int g_ldo_i = 0;

void pm_testcase_ldo_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_ldo = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, 0);
    uint32 volt_level1=0;
    uint32 volt_level2=0;
    pm_sw_mode_type mode_status;
    pm_on_off_type on_off;
    boolean b_status = FALSE;
    uint32 vset = 0;

    g_max_ldo = max_ldo;

    for (g_ldo_i=0; g_ldo_i < g_max_ldo; g_ldo_i++)
    {
        i = g_ldo_i;

        err = pm_ldo_volt_level_status (0, i,  &volt_level1);
        err += pm_ldo_volt_level (0, i,  volt_level1+25000);
        err += pm_ldo_volt_level_status (0, i,  &volt_level2);
        if (volt_level2 - volt_level1 != 25000)
        {
            //TODO - how to handle error situations like this
            err = -1; //Define new testframework specific error codes
        }

        err += pm_ldo_sw_mode_status (0, i,  &mode_status);
        err += pm_ldo_pin_ctrled_status (0, i,  &on_off);
        err += pm_ldo_sw_enable_status (0, i,  &on_off);
        err += pm_ldo_soft_reset_status (0, i,  &on_off);

        err += pm_ldo_soft_reset_exit (0, i);

        err += pm_ldo_volt_level_stepper_done_status (0, i,  &b_status);
        err += pm_ldo_volt_calculate_vset (0, i,  volt_level2, &vset);
    }

    g_status_all_ldo_api = err;
}

//TODO - this function should be declared
extern pm_err_flag_type pm_smps_switching_freq(uint8 pmic_chip,
                                                uint8 peripheral_index,
                                                pm_smps_switching_freq_type freq);
extern pm_err_flag_type pm_smps_switching_freq_status(uint8 pmic_chip,
                                                     uint8 peripheral_index,
                                                     pm_smps_switching_freq_type *freq);


void pm_testcase_smps_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_smps = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, 0);
    uint32 volt_level1,volt_level2 ;
    uint16 ilim_level1, ilim_level2;
    pm_sw_mode_type mode_status;
    pm_on_off_type on_off;
    pm_smps_switching_freq_type freq;
    boolean b_status = FALSE;

    for (i=0; i < max_smps; i++)
    {
        err = pm_smps_sw_mode_status (0, i,  &mode_status);
        err += pm_smps_sw_enable_status (0, i,  &on_off);

        err += pm_smps_clk_source (0, PM_CLK_RC);

        err += pm_smps_volt_level_stepper_done_status (0, i,  &b_status);

        err += pm_smps_volt_level_status (0, i,  &volt_level1);
        err += pm_smps_volt_level (0, i,  volt_level1+25000);
        err += pm_smps_volt_level_status (0, i,  &volt_level2);
        if (volt_level2 - volt_level1 != 25000)
        {
            //TODO - how to handle error situations like this
            err = -1; //Define new testframework specific error codes
        }

        //ilim APIs need LUT be defined - in 8909 there is no LUT defined, same in 8916
        err += pm_smps_inductor_ilim_status (0, i, &ilim_level1, PM_ILIM_SMPS_PWM_MODE);
        err += pm_smps_inductor_ilim (0, i, ilim_level1, PM_ILIM_SMPS_PWM_MODE);
        err += pm_smps_inductor_ilim_status (0, i, &ilim_level2, PM_ILIM_SMPS_PWM_MODE);

        err += pm_smps_switching_freq (0, i, PM_CLK_2p4_MHz);
        err += pm_smps_switching_freq_status (0, i, &freq);

        //TODO what should be the last param in volt in mv
        err += pm_smps_quiet_mode (0, i, PM_QUIET_MODE__SUPER_QUIET, 1000);
    }

    g_status_all_smps_api = err;
}


//TODO - add this API into header if not present
extern pm_err_flag_type pm_rtc_get_time(uint8 pmic_index, uint32 *time_ptr);

void pm_testcase_rtc_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;
    int i = 0;
    int max_rtc = pm_target_information_get_periph_count_info(PM_PROP_RTC_NUM, 0);

    pm_hal_rtc_time_type handle;
    pm_hal_rtc_time_type rtc_alarm;
    pm_hal_rtc_time_type time;
    uint8 alarm_status = 0;

    for (i=0; i < max_rtc; i++)
    {
        //err = pm_rtc_get_time (0, &timer_value);
        err = pm_hal_rtc_start (&handle);

        err += pm_hal_rtc_get_time (&time);

        err += pm_hal_rtc_enable_alarm (PM_HAL_RTC_ALARM_1, &time);

        err += pm_hal_rtc_get_alarm_time (PM_HAL_RTC_ALARM_1, &rtc_alarm);

        err += pm_hal_rtc_disable_alarm (PM_HAL_RTC_ALARM_1);

        err += pm_hal_rtc_get_alarm_status (&alarm_status);

        err += pm_hal_rtc_get_time_adjust (&alarm_status);
        err += pm_hal_rtc_set_time_adjust (alarm_status + 10);
        err += pm_hal_rtc_get_time_adjust (&alarm_status);
        err += pm_hal_rtc_set_time_adjust (alarm_status - 10);

        err += pm_hal_rtc_stop();
    }

    g_status_all_rtc_api = err;
}

void pm_testcase_bua_all_api (void)
{
    pm_err_flag_type err = PM_ERR_FLAG__INVALID;

    boolean b_status1 = 0;
    boolean b_status2 = 0;
    pm_irq_trigger_type irq_triger = (pm_irq_trigger_type)0;

    //err = pm_rtc_get_time (0, &timer_value);
    err = pm_bua_enable_status (0, &b_status1);
    err += pm_bua_enable (0, ~b_status1);
    err = pm_bua_enable_status (0, &b_status2);
    err += pm_bua_enable (0, b_status1); //Revert to original state
    if (b_status1 == b_status2)
    {
        err += -1;
    }

    for (int i=0; i < (int)PM_BUA_INVALID_ALARM; i++)
    {
        err += pm_bua_alarm_detect_status (0, (pm_bua_alarm_type)i, &b_status2);
        err += pm_bua_irq_enable (0, (pm_bua_alarm_type)i, 1);
        err += pm_bua_irq_status (0, (pm_bua_alarm_type)i, PM_IRQ_STATUS_ENABLE, &b_status1);
        if (1 != b_status1)
        {
            err += -1;
        }
        err += pm_bua_irq_clear (0, (pm_bua_alarm_type)i);
        err += pm_bua_irq_set_trigger (0, (pm_bua_alarm_type)i, PM_IRQ_TRIGGER_FALLING_EDGE);
        err += pm_bua_irq_get_trigger (0, (pm_bua_alarm_type)i, &irq_triger);
        if (PM_IRQ_TRIGGER_FALLING_EDGE != irq_triger)
        {
            err += -1;
        }
    }

    g_status_all_bua_api = err;
}


pm_err_flag_type pm_clk_ret = PM_ERR_FLAG__INVALID;
pm_err_flag_type pm_gpio_ret = PM_ERR_FLAG__INVALID;
pm_err_flag_type pm_mpp_ret = PM_ERR_FLAG__INVALID;
pm_err_flag_type pm_rtc_ret = PM_ERR_FLAG__INVALID;
pm_err_flag_type pm_xo_ret = PM_ERR_FLAG__INVALID;

pm_gpio_status_type gpio_periph_info = {0};
pm_mpp_status_type mpp_periph_info = {0};
pm_hal_rtc_time_type alarm_time_info = {0};
uint8 xo_trim_info = {0};

npa_client_handle hNPA_test_RF1;

void pm_mpss_api_test ( void )
{
    /* CLK api */
    pm_clk_ret = pm_dev_clk_buff_set_output_drive_strength(0, PM_RF_CLK1, PM_CLK_BUFF_OUT_DRV__1X);

    /* GPIO api */
    pm_gpio_ret = pm_dev_gpio_status_get(0, PM_GPIO_2, &gpio_periph_info);

    /* MPP api */
    pm_mpp_ret = pm_dev_mpp_status_get(0, PM_MPP_2, &mpp_periph_info);

    /* RTC api */
    pm_rtc_ret = pm_dev_hal_rtc_get_alarm_time(0, PM_HAL_RTC_ALARM_1, &alarm_time_info);

    /* XO api */
    pm_xo_ret = pm_dev_mega_xo_get_xo_trim(0, 0, &xo_trim_info);

    /* To avoid optimization of this function */
    pm_gpio_ret = pm_dev_gpio_set_output_buffer_drive_strength(0, PM_GPIO_2, PM_GPIO_OUT_BUFFER_MEDIUM);

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_GPS, "rf1_tech_gps", NPA_CLIENT_REQUIRED);
    if (hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    pmapp_pwr_set_vdd_mss(1153000);
}


void pm_testcase_npa_all_nodes ( void )
{
    // npa_client_handle hNPA_test_RF1 = NULL;
    // npa_client_handle hNPA_test_ANT1 = NULL;
    // npa_client_handle hNPA_test_PLL_SR2 = NULL;

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_ANT1, "PM_TEST_ANT1", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_GPS, "PM_TEST_GPS", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_PLL_SR2, "PM_TEST_PLL_SR2", NPA_CLIENT_REQUIRED);
    if (NULL == hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_QFPROM, "PM_TEST_QFROM", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RAIL_CX, "PM_TEST_CX", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RAIL_MX, "PM_TEST_MX", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_1X, "PM_TEST_TECH_1X", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_DO, "PM_TEST_TECH_DO", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_GSM, "PM_TEST_TECH_GSM", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_GSM2, "PM_TEST_TECH_GSM2", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_GSM3, "PM_TEST_TECH_GSM3", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_LTE, "PM_TEST_TECH_LTE", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_LTE2, "PM_TEST_TECH_LTE2", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA, "PM_TEST_TECH_TDSCDMA", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_1X, "PM_TEST_RF2_TECH_1X", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_DO, "PM_TEST_RF2_TECH_DO", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_GPS, "PM_TEST_RF2_TECH_GPS", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_GSM, "PM_TEST_RF2_TECH_GSM", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_GSM2, "PM_TEST_RF2_TECH_GSM2", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_GSM3, "PM_TEST_RF2_TECH_GSM3", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_LTE, "PM_TEST_RF2_TECH_LTE", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_LTE2, "PM_TEST_RF2_TECH_LTE2", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_TDSCDMA, "PM_TEST_RF2_TECH_TDSCDMA", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_WCDMA, "PM_TEST_RF2_TECH_WCDMA", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_RF2_TECH_LTE2, "PM_TEST_RF2_TECH_LTE2", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_UIM1_ACTIVITY, "PM_TEST_UIM1_ACTIVITY", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_UIM1_ENABLE, "PM_TEST_UIM1_ENABLE", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_UIM2_ACTIVITY, "PM_TEST_UIM2_ACTIVITY", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    hNPA_test_RF1 = npa_create_sync_client (PMIC_NPA_GROUP_ID_UIM2_ENABLE, "PM_TEST_UIM2_ENABLE", NPA_CLIENT_REQUIRED);
    if (NULL != hNPA_test_RF1)
    {
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_ACTIVE);
        npa_issue_required_request (hNPA_test_RF1, PMIC_NPA_MODE_ID_GENERIC_STANDBY);
        npa_destroy_client (hNPA_test_RF1);
    }

    // pmapp_pwr_set_vdd_mss(1153000);
}


void pmic_unit_testcases (void)
{
    pm_mpss_api_test ();

    pm_testcase_gpio_all_api ();
    g_test_count++; //1
    pm_testcase_mpp_all_api ();
    g_test_count++; //2
    pm_testcase_clk_all_api ();
    g_test_count++; //3
    pm_testcase_ldo_all_api ();
    g_test_count++; //4
    pm_testcase_smps_all_api();
    g_test_count++; //5
    pm_testcase_rtc_all_api ();
    g_test_count++; //6
    pm_testcase_bua_all_api ();
    g_test_count++; //7
    pm_testcase_npa_all_nodes();
    g_test_count++; //8
}

