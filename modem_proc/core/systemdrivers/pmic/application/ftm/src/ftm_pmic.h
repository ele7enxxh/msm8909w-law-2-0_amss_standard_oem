/*! \file 
 *  \n
 *  \brief  ftm_pmic.h ---- PMIC-FTM
 *  \details  This file contains variable and function declarations to support 
 *   the PMIC FTM interface.
 *
 *
 *  \n &copy; Copyright (c) 2012  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/ftm/src/ftm_pmic.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/14/12   hs      Added RTC data in the response package. 
01/04/12   hs      New file
===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"
#include "msg.h"
#include "pm.h"

#ifndef FTM_PMIC_C
#define FTM_PMIC_C 11
#endif

typedef enum
{
    FTM_PMIC_TEST_RTC       = 0,
    FTM_PMIC_TEST_MPP       = 12,
    FTM_PMIC_TEST_GPIO      = 13,
} ftm_pmic_cmd_id_type;

typedef enum
{
    FTM_PMIC_GEN_RESPONSE      = 0x1000
} ftm_pmic_rsp_id_type;

typedef PACK( struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;
} ftm_pmic_rsp_header_type;

/* DIAG Header request type */
typedef PACKED struct
{
  diagpkt_cmd_code_type                  cmd_code;
  diagpkt_subsys_id_type                 subsys_id;
  diagpkt_subsys_cmd_code_type           subsys_cmd_code;
} ftm_pmic_diagpkt_subsys_header_type;

/* FTM PKT Header */
typedef PACKED struct
{
  word cmd_id;            /* command id (required) */
  word cmd_data_len;      /* request pkt data length, excluding the diag and ftm headers 
                             (optional, set to 0 if not used) 
                          */
  word cmd_rsp_pkt_size;  /* rsp pkt size, size of response pkt if different then req pkt 
                             (optional, set to 0 if not used) 
                          */
} ftm_pmic_cmd_header_type;

/* FTM PMIC request type */
typedef PACK(struct)
{
  ftm_pmic_diagpkt_subsys_header_type   header;
  ftm_pmic_cmd_header_type          	ftm_cmd_header;
  void                         			*pmic_params;
} ftm_pmic_pkt_type;

/*===========================================================================
                     RTC DEFINITIONS
===========================================================================*/

/* This type specifies the RTC commands for the PMIC */
typedef enum
{
    FTM_PMIC_RTC_SET_GET_TIME           = 0,
    FTM_PMIC_RTC_SET_GET_ALARM_TIME     = 1,
    FTM_PMIC_RTC_STOP                   = 2,
    FTM_PMIC_RTC_GET_ALARM_STATUS       = 3,
    FTM_PMIC_RTC_DISABLE_ALARM          = 4,
    FTM_PMIC_RTC_SET_GET_TIME_ADJUST    = 5,
} ftm_pmic_rtc_cmd_id_type;

/* This type spcifies the time format for the RTC commands */
typedef enum
{
    FTM_PMIC_RTC_MODE_12_HOUR_AM  = 0,
    FTM_PMIC_RTC_MODE_12_HOUR_PM  = 1,
    FTM_PMIC_RTC_MODE_24_HOUR     = 2
} ftm_pmic_rtc_mode_type;

typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
} ftm_pmic_rtc_header_type;

/* Used when response to FTM PMIC RTC data */
typedef PACK( struct)
{
    byte                         ftm_rtc_alarm_id;
    byte                         ftm_rtc_mode;
    byte                         ftm_rtc_month;
    byte                         ftm_rtc_day;
    word                         ftm_rtc_year;
    byte                         ftm_rtc_hour;
    byte                         ftm_rtc_min;
    byte                         ftm_rtc_sec;
    byte                         ftm_rtc_alarm_status;
    byte                         ftm_rtc_time_adjust;
} ftm_pmic_rtc_rsp_pkt_data_type;

/* Used when response to FTM PMIC RTC command requires sending back */
/* command status info */
typedef PACK( struct)
{
	ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_rsp_header_type        	   ftm_rsp_header;
    uint16                          	   status;
    ftm_pmic_rtc_rsp_pkt_data_type         data;
} ftm_pmic_rtc_rsp_pkt_type;

/* Used when response to FTM PMIC RTC command requires sending back */
/* command status info */
typedef PACK( struct)
{
	ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_rsp_header_type        	   ftm_rsp_header;
    uint16                          	   status;
} ftm_pmic_rsp_pkt_type;

/* This type specifies the interface for the set time rtc cmd */
typedef PACK( struct)
{
    byte      ftm_rtc_month;
    byte      ftm_rtc_day;
    word      ftm_rtc_year;
    byte      ftm_rtc_hour;
    byte      ftm_rtc_min;
    byte      ftm_rtc_sec;
} ftm_pmic_rtc_set_time_cmd_type;

/* This type specifies the interface for the set time rtc cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
    pm_rtc_cmd_type                        ftm_rtc_rw;
    ftm_pmic_rtc_mode_type                 time_mode;
    ftm_pmic_rtc_set_time_cmd_type         rtc_time;
} ftm_pmic_rtc_set_get_time_cmd_type;

/* This type specifies the interface for the set alarm time rtc cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
    pm_rtc_cmd_type                        ftm_rtc_rw;
    pm_rtc_alarm_type                      alarm_id;
    ftm_pmic_rtc_mode_type                 time_mode;
    ftm_pmic_rtc_set_time_cmd_type         alarm_time;
} ftm_pmic_rtc_set_get_alarm_time_cmd_type;


/* This type specifies the interface for the stop rtc cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
} ftm_pmic_rtc_stop_cmd_type;


/* This type specifies the interface for the get alarm status cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
    uint8                                  alarm_status;
} ftm_pmic_rtc_get_alarm_status_cmd_type;

/* This type specifies the interface for the disable alarm cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type    header;
    ftm_pmic_cmd_header_type               ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
    pm_rtc_alarm_type                      alarm_id;
} ftm_pmic_rtc_disable_alarm_cmd_type;


/* this type specifies the interface for the rtc set/get time adjust cmd */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type             header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;
    pm_rtc_cmd_type                        ftm_rtc_rw;
    uint8                                  time_adjust;
} ftm_pmic_rtc_set_get_time_adjust_cmd_type;

/*===========================================================================
                     MPP DEFINITIONS
===========================================================================*/
/* This type specifies the MPP commands for the PMIC */
typedef enum
{
    FTM_PMIC_MPP_CONFIG_DIGITAL_IN      = 0, /*configure selected mpp be digital input*/
    FTM_PMIC_MPP_CONFIG_DIGITAL_OUT     = 1, /*configure selected mpp be digital output*/
    FTM_PMIC_MPP_CONFIG_DIGITAL_INOUT   = 2, /*configure selected mpp be digital in and output*/
    FTM_PMIC_MPP_CONFIG_ANALOG_IN       = 3, /*configure selected mpp be analog input*/
    FTM_PMIC_MPP_CONFIG_ANALOG_OUT      = 4, /*configure selected mpp be analog output*/
    FTM_PMIC_MPP_CONFIG_I_SINK          = 5  /*configure selected mpp be to a current sink*/
} ftm_pmic_mpp_cmd_id_type;

typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type			header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
} ftm_pmic_mpp_header_type;

/* This type specifies the MPP digital in config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_dlogic_lvl_type                      level;
    pm_mpp_dlogic_in_dbus_type                  dbus;
} ftm_pmic_mpp_digital_in_cmd_type;

/* This type specifies the MPP digital out config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_dlogic_lvl_type                      level;
    pm_mpp_dlogic_out_ctrl_type                 ctrl;
} ftm_pmic_mpp_digital_out_cmd_type;

/* This type specifies the MPP digital in_out config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_dlogic_lvl_type                      level;
    pm_mpp_dlogic_inout_pup_type                pup;
} ftm_pmic_mpp_digital_inout_cmd_type;

/* This type specifies the MPP analog in config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_ain_ch_type                          channel;
} ftm_pmic_mpp_analog_in_cmd_type;

/* This type specifies the MPP analog out config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_aout_level_type                      level;
    pm_mpp_aout_switch_type                     _switch_;
} ftm_pmic_mpp_analog_out_cmd_type;

/* This type specifies the MPP analog out config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;
    pm_mpp_which_type                           mpp;
    pm_mpp_i_sink_level_type                    level;
    pm_mpp_i_sink_switch_type                   _switch_;
} ftm_pmic_mpp_i_sink_cmd_type;


/*===========================================================================
                     GPIO DEFINITIONS
===========================================================================*/
/* This type specifies the GPIO commands for the PMIC */
typedef enum
{
    FTM_PMIC_GPIO_CONFIG_DIGITAL_IN               = 0,  /*configure selected gpio be digital input*/
    FTM_PMIC_GPIO_CONFIG_DIGITAL_OUT              = 1,  /*configure selected gpio be digital output*/
    FTM_PMIC_GPIO_CONFIG_VOLTAGE_SOURCE           = 2,  /*configure selected gpio's voltage source*/
    FTM_PMIC_GPIO_CONFIG_MODE_SELECTION           = 3,  /*configure selected gpio's mode selection*/
    FTM_PMIC_GPIO_CONFIG_OUTPUT_BUFFER            = 4,  /*configure selected gpio's output buffer*/
    FTM_PMIC_GPIO_CONFIG_INVERSION                = 5,  /*configure selected gpio's inversion*/
    FTM_PMIC_GPIO_CONFIG_CURRENT_SOURCE_PULLS     = 6,  /* configure selected gpio's current source pulls*/
    FTM_PMIC_GPIO_CONFIG_EXT_PIN                  = 7,  /* configure selected gpio's external pin config*/
    FTM_PMIC_GPIO_CONFIG_BUFFER_DRIVER_STRENGTH   = 8,  /* configure selected gpio's output buffer driver strength*/
    FTM_PMIC_GPIO_CONFIG_SOURCE                   = 9,  /*configure selected gpio's source*/
    FTM_PMIC_GPIO_CONFIG_MUX_CONTROL              = 10, /*configure gpio mux control*/
    FTM_PMIC_GPIO_CONFIG_BIAS_VOLTAGE             = 11, /*configure selected gpio's bias voltage source*/
    FTM_PMIC_GPIO_GET_STATUS                      = 12  /*configure selected gpio's bias voltage source*/
} ftm_pmic_gpio_cmd_id_type;

typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
} ftm_pmic_gpio_header_type;

/* This type specifies the GPIO digital in config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_current_src_pulls_type           i_source_pulls;
    pm_gpio_volt_src_type                 voltage_source;
    pm_gpio_out_buffer_drv_strength_type      buffer_strength;
    pm_gpio_src_config_type                  source;
} ftm_pmic_gpio_digital_in_cmd_type;

/* This type specifies the GPIO digital out config command packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_out_buffer_config_type              buffer_config;
    pm_gpio_volt_src_type                 voltage_source;
    pm_gpio_src_config_type                  source;
    pm_gpio_out_buffer_drv_strength_type      buffer_strength;
    boolean                                     inversion;
} ftm_pmic_gpio_digital_out_cmd_type;

/* This type specifies the GPIO set voltage source packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_volt_src_type                 config;
} ftm_pmic_gpio_voltage_source_cmd_type;

/* This type specifies the GPIO config mode selection packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    boolean                                     config;
} ftm_pmic_gpio_mode_selection_cmd_type;

/* This type specifies the GPIO config mode selection packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_out_buffer_config_type              config;
} ftm_pmic_gpio_buffer_config_cmd_type;

/* This type specifies the GPIO set inversion packet */

typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    boolean                                     config;
} ftm_pmic_gpio_inversion_cmd_type;

/* This type specifies the GPIO set current source pulls packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_current_src_pulls_type           config;
} ftm_pmic_gpio_current_source_pulls_cmd_type;

/* This type specifies the GPIO set ext pin config packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_ext_pin_config_type                 config;
} ftm_pmic_gpio_ext_pin_config_cmd_type;

/* This type specifies the GPIO set output buffer driver strength packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_out_buffer_drv_strength_type      config;
} ftm_pmic_gpio_buffer_driver_strength_cmd_type;


/* This type specifies the GPIO set source packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_src_config_type                  config;
} ftm_pmic_gpio_source_cmd_type;

/* This type specifies the GPIO set mux ctrl packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_uart_path_type                      config;
} ftm_pmic_gpio_mux_ctrl_cmd_type;

/* This type specifies the GPIO config bias voltage packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_volt_src_type                 config;
} ftm_pmic_gpio_bias_voltage_cmd_type;

/* This type specifies the GPIO config bias voltage packet */
typedef PACK( struct)
{
    ftm_pmic_diagpkt_subsys_header_type         header;
    ftm_pmic_cmd_header_type                    ftm_cmd_header;
    ftm_pmic_gpio_cmd_id_type                   gpio_cmd_id;
    pm_gpio_perph_index                          gpio;
    pm_gpio_status_type                         config;
} ftm_pmic_gpio_status_cmd_type;

PACKED void *  ftm_pmic_process_rtc_commands ( PACKED void * request , uint16 length );
PACKED void *  ftm_pmic_process_mpp_commands ( PACKED void * request , uint16 length );
PACKED void *  ftm_pmic_process_gpio_commands ( PACKED void * request , uint16 length );					