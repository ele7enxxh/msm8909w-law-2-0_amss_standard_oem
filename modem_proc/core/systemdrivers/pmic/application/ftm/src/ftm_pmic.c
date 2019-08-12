/*! \file
 *  \n
 *  \brief  ftm_pmic.c ---- PMIC-FTM
 *  \details  This file contains function implementations to support
 *   the PMIC FTM interface.
 *
 *
 *  \n &copy; Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/ftm/src/ftm_pmic.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/13   rk      removing MPP InOut functionality as it is no longer supported in 8916.
03/12/13   vk      Fixed KW error
02/14/12   hs      Added data in the response package.
01/04/12   hs      New file
===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "ftm_pmic.h"
#include "ftm_pmic_diag_dispatch.h"
#include "pm.h"

PACKED void *  ftm_pmic_process_rtc_commands ( PACKED void * request , uint16 length )
{
    pm_rtc_julian_type                          time;
    pm_err_flag_type                            err_flag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_alarm_type                           alarm;
    uint8                                       alarm_status;
    uint8                                       time_adjust;
    ftm_pmic_rtc_set_get_time_cmd_type          *rtc_set_get_time_cmd;
    ftm_pmic_rtc_set_get_alarm_time_cmd_type    *rtc_set_get_alarm_time_cmd;

    ftm_pmic_rtc_disable_alarm_cmd_type         *rtc_disable_alarm_cmd;
    ftm_pmic_rtc_set_get_time_adjust_cmd_type   *rtc_set_get_time_adj_cmd;
    ftm_pmic_rtc_get_alarm_status_cmd_type      *rtc_get_alarm_status_cmd;
    pm_rtc_display_type                         display_mode = PM_RTC_12HR_MODE;

    ftm_pmic_rtc_header_type* rtc_header = (ftm_pmic_rtc_header_type*)request;

    ftm_pmic_rtc_rsp_pkt_type* response = (ftm_pmic_rtc_rsp_pkt_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_PMIC_C, sizeof (ftm_pmic_rtc_rsp_pkt_type));

    if(rtc_header != NULL && response != NULL)
    {
    switch( rtc_header->rtc_cmd_id )
    {
        case FTM_PMIC_RTC_SET_GET_TIME:
            rtc_set_get_time_cmd = (ftm_pmic_rtc_set_get_time_cmd_type *) request;

            if( rtc_set_get_time_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {
                if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_AM ||
                    rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    display_mode = PM_RTC_12HR_MODE;
                }
                else if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_24_HOUR)
                {
                    display_mode = PM_RTC_24HR_MODE;
                }
                else
                {
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                }

                err_flag = pm_set_rtc_display_mode( display_mode);

                time.day_of_week = 5; /* TGIF!...day of week not really necessary */
                time.month       = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_month;
                time.day         = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_day;
                time.year        = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_year;
                time.hour        = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_hour;
                time.minute      = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_min;
                time.second      = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_sec;

                /* Convert 12 PM mode times into internal format used by API */

                if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    time.hour += 20;
                }

                // update the response package data
                response->data.ftm_rtc_mode        = (byte) display_mode;
                response->data.ftm_rtc_month       = (byte) rtc_set_get_time_cmd->rtc_time.ftm_rtc_month;
                response->data.ftm_rtc_day         = (byte) rtc_set_get_time_cmd->rtc_time.ftm_rtc_day;
                response->data.ftm_rtc_year        = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_year;
                response->data.ftm_rtc_hour        = (byte) rtc_set_get_time_cmd->rtc_time.ftm_rtc_hour;
                response->data.ftm_rtc_min         = (byte) rtc_set_get_time_cmd->rtc_time.ftm_rtc_min;
                response->data.ftm_rtc_sec         = (byte) rtc_set_get_time_cmd->rtc_time.ftm_rtc_sec;

                err_flag = pm_rtc_rw_cmd( PM_RTC_SET_CMD, &time);
            }
            else if( rtc_set_get_time_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {
                err_flag = pm_set_rtc_display_mode( PM_RTC_24HR_MODE);
                err_flag = pm_rtc_rw_cmd( PM_RTC_GET_CMD, &time);

                rtc_set_get_time_cmd->time_mode = FTM_PMIC_RTC_MODE_24_HOUR;

                rtc_set_get_time_cmd->rtc_time.ftm_rtc_month = (byte) time.month;
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_day   = (byte) time.day;
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_year  = (word) time.year;
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_hour  = (byte) time.hour;
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_min   = (byte) time.minute;
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_sec   = (byte) time.second;

                // update the response package data
                response->data.ftm_rtc_mode        = (byte) FTM_PMIC_RTC_MODE_24_HOUR;
                response->data.ftm_rtc_month       = (byte) time.month;
                response->data.ftm_rtc_day         = (byte) time.day;
                response->data.ftm_rtc_year        = (word) time.year;
                response->data.ftm_rtc_hour        = (word) time.hour;
                response->data.ftm_rtc_min         = (byte) time.minute;
                response->data.ftm_rtc_sec         = (byte) time.second;
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
            break;

        case FTM_PMIC_RTC_STOP:
            err_flag = pm_hal_rtc_stop();
            break;

        case FTM_PMIC_RTC_SET_GET_ALARM_TIME:
            rtc_set_get_alarm_time_cmd = (ftm_pmic_rtc_set_get_alarm_time_cmd_type *) request;

            alarm = rtc_set_get_alarm_time_cmd->alarm_id;

            if( rtc_set_get_alarm_time_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {
                if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_AM ||
                    rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    display_mode = PM_RTC_12HR_MODE;
                }
                else if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_24_HOUR)
                {
                    display_mode = PM_RTC_24HR_MODE;
                }
                else
                {
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                }

                err_flag = pm_set_rtc_display_mode( display_mode);

                time.day_of_week = 5; /* TGIF!...day of week not really necessary */
                time.month       = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_month;
                time.day         = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_day;
                time.year        = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_year;
                time.hour        = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_hour;
                time.minute      = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_min;
                time.second      = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_sec;

                if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    time.hour += 20;
                }

                err_flag = pm_rtc_alarm_rw_cmd( PM_RTC_SET_CMD, alarm, &time);

                // update the response package data
                response->data.ftm_rtc_alarm_id    = (byte) alarm;
                response->data.ftm_rtc_mode        = (byte) display_mode;
                response->data.ftm_rtc_month       = (byte) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_month;
                response->data.ftm_rtc_day         = (byte) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_day;
                response->data.ftm_rtc_year        = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_year;
                response->data.ftm_rtc_hour        = (byte) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_hour;
                response->data.ftm_rtc_min         = (byte) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_min;
                response->data.ftm_rtc_sec         = (byte) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_sec;
            }
            else if( rtc_set_get_alarm_time_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {
                err_flag = pm_set_rtc_display_mode( PM_RTC_24HR_MODE);
                err_flag = pm_rtc_alarm_rw_cmd( PM_RTC_GET_CMD, alarm, &time);

                rtc_set_get_alarm_time_cmd->time_mode = FTM_PMIC_RTC_MODE_24_HOUR;

                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_month = (byte) time.month;
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_day   = (byte) time.day;
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_year  = (word) time.year;
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_hour  = (byte) time.hour;
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_min   = (byte) time.minute;
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_sec   = (byte) time.second;

                // update the response package data
                response->data.ftm_rtc_alarm_id    = (byte) alarm;
                response->data.ftm_rtc_mode        = (byte) FTM_PMIC_RTC_MODE_24_HOUR;
                response->data.ftm_rtc_month       = (byte) time.month;
                response->data.ftm_rtc_day         = (byte) time.day;
                response->data.ftm_rtc_year        = (word) time.year;
                response->data.ftm_rtc_hour        = (word) time.hour;
                response->data.ftm_rtc_min         = (byte) time.minute;
                response->data.ftm_rtc_sec         = (byte) time.second;
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
            break;

        case FTM_PMIC_RTC_GET_ALARM_STATUS:
            rtc_get_alarm_status_cmd = (ftm_pmic_rtc_get_alarm_status_cmd_type *) request;

            err_flag =  pm_get_rtc_alarm_status( &alarm_status);
            rtc_get_alarm_status_cmd->alarm_status = alarm_status;

            // update the response package data
            response->data.ftm_rtc_alarm_status = (byte) alarm_status;

            break;

        case FTM_PMIC_RTC_DISABLE_ALARM:
            rtc_disable_alarm_cmd = ( ftm_pmic_rtc_disable_alarm_cmd_type *) request;

            alarm = rtc_disable_alarm_cmd->alarm_id;

            err_flag  = pm_reset_rtc_alarm( alarm);

            break;

        case FTM_PMIC_RTC_SET_GET_TIME_ADJUST:
            rtc_set_get_time_adj_cmd = ( ftm_pmic_rtc_set_get_time_adjust_cmd_type *) request;

            if( rtc_set_get_time_adj_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {
                time_adjust = (uint8) rtc_set_get_time_adj_cmd->time_adjust;
                err_flag    = pm_hal_rtc_set_time_adjust( time_adjust);

                // update the response package data
                response->data.ftm_rtc_time_adjust = (byte) time_adjust;
            }
            else if( rtc_set_get_time_adj_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {
                err_flag = pm_hal_rtc_get_time_adjust( &time_adjust);
                rtc_set_get_time_adj_cmd->time_adjust = time_adjust;

                // update the response package data
                response->data.ftm_rtc_time_adjust = (byte) time_adjust;
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }

            break;
        default:
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            break;

    }

    response->header                     = rtc_header->header;
    response->ftm_rsp_header.rsp_id      = (uint16) FTM_PMIC_GEN_RESPONSE;
    response->ftm_rsp_header.rsp_cid     = FTM_PMIC_TEST_RTC;
    response->ftm_rsp_header.rsp_scid    = rtc_header->rtc_cmd_id;
    response->status                     = err_flag;
    }

    return response;
}

PACKED void *  ftm_pmic_process_mpp_commands ( PACKED void * request , uint16 length )
{
    ftm_pmic_mpp_digital_in_cmd_type        *mpp_digital_in_cmd;
    ftm_pmic_mpp_digital_out_cmd_type       *mpp_digital_out_cmd;
    ftm_pmic_mpp_digital_inout_cmd_type     *mpp_digital_inout_cmd;
    ftm_pmic_mpp_analog_in_cmd_type         *mpp_analog_in_cmd;
    ftm_pmic_mpp_analog_out_cmd_type        *mpp_analog_out_cmd;
    ftm_pmic_mpp_i_sink_cmd_type            *mpp_i_sink_cmd;

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    ftm_pmic_mpp_header_type* mpp_header = (ftm_pmic_mpp_header_type*)request;

    ftm_pmic_rsp_pkt_type* response = (ftm_pmic_rsp_pkt_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_PMIC_C, sizeof (ftm_pmic_rsp_pkt_type));

    if(mpp_header != NULL && response != NULL)
    {
     switch(mpp_header->mpp_cmd_id)
     {
        case FTM_PMIC_MPP_CONFIG_DIGITAL_IN:
            mpp_digital_in_cmd = (ftm_pmic_mpp_digital_in_cmd_type *)request;
            err_flag = pm_mpp_config_digital_input(mpp_digital_in_cmd->mpp,
                                                 mpp_digital_in_cmd->level,
                                                  mpp_digital_in_cmd->dbus);
            break;

        case FTM_PMIC_MPP_CONFIG_DIGITAL_OUT:
            mpp_digital_out_cmd = (ftm_pmic_mpp_digital_out_cmd_type *)request;
            err_flag = pm_mpp_config_digital_output(mpp_digital_out_cmd->mpp,
                                             mpp_digital_out_cmd->level,
                                             mpp_digital_out_cmd->ctrl);
            break;

            case FTM_PMIC_MPP_CONFIG_DIGITAL_INOUT:
                mpp_digital_inout_cmd = (ftm_pmic_mpp_digital_inout_cmd_type *)request;
                err_flag = pm_mpp_config_digital_inout(mpp_digital_inout_cmd->mpp,
                                                    mpp_digital_inout_cmd->level,
                                                    mpp_digital_inout_cmd->pup);
                 break;

        case FTM_PMIC_MPP_CONFIG_ANALOG_IN:
            mpp_analog_in_cmd = (ftm_pmic_mpp_analog_in_cmd_type *)request;
            err_flag = pm_mpp_config_analog_input(mpp_analog_in_cmd->mpp,
                                                mpp_analog_in_cmd->channel);
            break;

        case FTM_PMIC_MPP_CONFIG_ANALOG_OUT:
            mpp_analog_out_cmd = (ftm_pmic_mpp_analog_out_cmd_type *)request;
            err_flag = pm_mpp_config_analog_output(mpp_analog_out_cmd->mpp,
                                            mpp_analog_out_cmd->level,
                                            mpp_analog_out_cmd->_switch_);
            break;

        case FTM_PMIC_MPP_CONFIG_I_SINK:
            mpp_i_sink_cmd = (ftm_pmic_mpp_i_sink_cmd_type *)request;
            err_flag = pm_mpp_config_i_sink(mpp_i_sink_cmd->mpp,
                                         mpp_i_sink_cmd->level,
                                         mpp_i_sink_cmd->_switch_);
            break;

        default:
            break;
     }

     response->header = ((ftm_pmic_mpp_header_type *)request)->header;
     response->ftm_rsp_header.rsp_id = (uint16) FTM_PMIC_GEN_RESPONSE;
     response->ftm_rsp_header.rsp_cid = FTM_PMIC_TEST_MPP;
     response->ftm_rsp_header.rsp_scid = mpp_header->mpp_cmd_id;
     response->status = err_flag;
     }

     return response;
}

PACKED void *  ftm_pmic_process_gpio_commands ( PACKED void * request , uint16 length )
{
    ftm_pmic_gpio_digital_in_cmd_type               *gpio_digital_in_cmd;
    ftm_pmic_gpio_digital_out_cmd_type              *gpio_digital_out_cmd;
    ftm_pmic_gpio_voltage_source_cmd_type           *gpio_voltage_source_cmd;
    ftm_pmic_gpio_buffer_config_cmd_type            *gpio_buffer_config_cmd;
    ftm_pmic_gpio_inversion_cmd_type                *gpio_inversion_cmd;
    ftm_pmic_gpio_current_source_pulls_cmd_type     *gpio_current_source_pulls_cmd;
    ftm_pmic_gpio_ext_pin_config_cmd_type           *gpio_ext_pin_config_cmd;
    ftm_pmic_gpio_buffer_driver_strength_cmd_type   *gpio_buffer_driver_strength_cmd;
    ftm_pmic_gpio_source_cmd_type                   *gpio_source_cmd;
    ftm_pmic_gpio_mux_ctrl_cmd_type                 *gpio_mux_ctrl_cmd;
    ftm_pmic_gpio_bias_voltage_cmd_type             *gpio_bias_voltage_cmd;
    ftm_pmic_gpio_status_cmd_type                   *gpio_status_cmd;

    pm_gpio_status_type                             gpio_status;

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    ftm_pmic_gpio_header_type* gpio_header = (ftm_pmic_gpio_header_type*)request;

    ftm_pmic_rsp_pkt_type* response = (ftm_pmic_rsp_pkt_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_PMIC_C, sizeof (ftm_pmic_rsp_pkt_type));

    if(gpio_header != NULL && response != NULL)
    {
     switch(gpio_header->gpio_cmd_id)
     {
        case FTM_PMIC_GPIO_CONFIG_DIGITAL_IN:
            gpio_digital_in_cmd = (ftm_pmic_gpio_digital_in_cmd_type *)request;
            err_flag = pm_gpio_config_digital_input(gpio_digital_in_cmd->gpio,
                                                 gpio_digital_in_cmd->i_source_pulls,
                                                 gpio_digital_in_cmd->voltage_source,
                                                 gpio_digital_in_cmd->buffer_strength,
                                                 gpio_digital_in_cmd->source);
             break;

        case FTM_PMIC_GPIO_CONFIG_DIGITAL_OUT:
            gpio_digital_out_cmd = (ftm_pmic_gpio_digital_out_cmd_type *)request;
            err_flag = pm_gpio_config_digital_output(gpio_digital_out_cmd->gpio,
                                                  gpio_digital_out_cmd->buffer_config,
                                                  gpio_digital_out_cmd->voltage_source,
                                                  gpio_digital_out_cmd->source,
                                                  gpio_digital_out_cmd->buffer_strength,
                                                  gpio_digital_out_cmd->inversion);
             break;

        case FTM_PMIC_GPIO_CONFIG_VOLTAGE_SOURCE:
            gpio_voltage_source_cmd = (ftm_pmic_gpio_voltage_source_cmd_type *)request;
            err_flag = pm_gpio_set_voltage_source(gpio_voltage_source_cmd->gpio,
                                               gpio_voltage_source_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_OUTPUT_BUFFER:
            gpio_buffer_config_cmd = (ftm_pmic_gpio_buffer_config_cmd_type *)request;
            err_flag = pm_gpio_set_output_buffer_configuration(gpio_buffer_config_cmd->gpio,
                                                            gpio_buffer_config_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_INVERSION:
            gpio_inversion_cmd = (ftm_pmic_gpio_inversion_cmd_type *)request;
            err_flag = pm_gpio_set_inversion_configuration(gpio_inversion_cmd->gpio,
                                                        gpio_inversion_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_CURRENT_SOURCE_PULLS:
            gpio_current_source_pulls_cmd = (ftm_pmic_gpio_current_source_pulls_cmd_type *)request;
            err_flag = pm_gpio_set_current_source_pulls(gpio_current_source_pulls_cmd->gpio,
                                                     gpio_current_source_pulls_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_EXT_PIN:
            gpio_ext_pin_config_cmd = (ftm_pmic_gpio_ext_pin_config_cmd_type *)request;
            err_flag = pm_gpio_set_ext_pin_config(gpio_ext_pin_config_cmd->gpio,
                                               gpio_ext_pin_config_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_BUFFER_DRIVER_STRENGTH:
            gpio_buffer_driver_strength_cmd = (ftm_pmic_gpio_buffer_driver_strength_cmd_type *)request;
            err_flag = pm_gpio_set_output_buffer_drive_strength(gpio_buffer_driver_strength_cmd->gpio,
                                                             gpio_buffer_driver_strength_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_SOURCE:
            gpio_source_cmd = (ftm_pmic_gpio_source_cmd_type *)request;
            err_flag = pm_gpio_set_source_configuration(gpio_source_cmd->gpio,
                                                     gpio_source_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_MUX_CONTROL:
            gpio_mux_ctrl_cmd = (ftm_pmic_gpio_mux_ctrl_cmd_type *)request;
            err_flag = pm_gpio_set_mux_ctrl(gpio_mux_ctrl_cmd->config);
             break;

        case FTM_PMIC_GPIO_CONFIG_BIAS_VOLTAGE:
            gpio_bias_voltage_cmd = (ftm_pmic_gpio_bias_voltage_cmd_type *)request;
            err_flag = pm_gpio_config_bias_voltage(gpio_bias_voltage_cmd->gpio,
                                                gpio_bias_voltage_cmd->config);
             break;

        case FTM_PMIC_GPIO_GET_STATUS:
            gpio_status_cmd = (ftm_pmic_gpio_status_cmd_type *)request;

            err_flag = pm_gpio_status_get( gpio_status_cmd->gpio,
                                       &gpio_status );

                gpio_status_cmd->config.gpio_volt_src =  gpio_status.gpio_volt_src;
                gpio_status_cmd->config.gpio_out_buffer_config =  gpio_status.gpio_out_buffer_config;
                gpio_status_cmd->config.gpio_out_buffer_drv_strength =  gpio_status.gpio_out_buffer_drv_strength;
                gpio_status_cmd->config.gpio_current_src_pulls =  gpio_status.gpio_current_src_pulls;
                gpio_status_cmd->config.gpio_src_config =  gpio_status.gpio_src_config;
                gpio_status_cmd->config.gpio_dtest_buffer_on_off =  gpio_status.gpio_dtest_buffer_on_off;
                gpio_status_cmd->config.gpio_ext_pin_config =  gpio_status.gpio_ext_pin_config;
             break;
        default:
             break;
     }

     response->header = ((ftm_pmic_gpio_header_type *)request)->header;
     response->ftm_rsp_header.rsp_id = (uint16) FTM_PMIC_GEN_RESPONSE;
     response->ftm_rsp_header.rsp_cid = FTM_PMIC_TEST_GPIO;
     response->ftm_rsp_header.rsp_scid = gpio_header->gpio_cmd_id;
     response->status = err_flag;
     }

     return response;
}