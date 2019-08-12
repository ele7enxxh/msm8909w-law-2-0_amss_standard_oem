#ifndef _SNS_PLAYBACK_H
#define _SNS_PLAYBACK_H

/*============================================================================
  @file sns_playback.h

  The sensors playback header.

  This header file contains the public interface for the sensors playback.

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/playback/inc/sns_playback.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-05-29  ps   Add support virtio test cases
  2012-11-02  vh   Changed stress test agrumends
  2012-10-25  vh   Added macros & structures
  2012-03-20  sc   Added function to run playback standalone test
  2011-07-01  sc   Added function to enable/disable logging service
  2011-05-19  sc   Added declaration for global sns_reg_filename[]
  2011-01-17  sc   Initial version

============================================================================*/


/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "sns_common_v01.h"
#include "sns_debug_interface_v01.h"
#include "sns_ddf_common.h"


/*---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
/*
  * Use it only when when one combination of sensors is run at a given time.
  * It doesnt work when multiple combinations of sensors streamed at a time.
  */
//#define SNS_PLBK_SAMPLES_PER_SEC

//#define SNS_PLBK_TIME_CONDITION
#define PROX_LIGHT_LOWER_RATE
//#define SNS_BOTH_VIRTIO_AND_HARDWARE

#if defined(SNS_VIRTIO_ONLY_1)
#define SNS_PLBK_MSG(msg)        printf(msg)
#define SNS_PLBK_MSG_1(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_2(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_3(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_4(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_5(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_6(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_7(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_8(msg, ...) printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_9(msg, ...) printf(msg, ##__VA_ARGS__)
#elif defined(SNS_BOTH_VIRTIO_AND_HARDWARE)
#define SNS_PLBK_MSG(msg)        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg); printf(msg)
#define SNS_PLBK_MSG_1(msg, ...) MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_2(msg, ...) MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_3(msg, ...) MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_4(msg, ...) MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_5(msg, ...) MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_6(msg, ...) MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_7(msg, ...) MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_8(msg, ...) MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_9(msg, ...) MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__); printf(msg, ##__VA_ARGS__)
#else
#define SNS_PLBK_MSG(msg)        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg)
#define SNS_PLBK_MSG_1(msg, ...) MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_2(msg, ...) MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_3(msg, ...) MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_4(msg, ...) MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_5(msg, ...) MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_6(msg, ...) MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_7(msg, ...) MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_8(msg, ...) MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#define SNS_PLBK_MSG_9(msg, ...) MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, msg, ##__VA_ARGS__)
#endif

/*---------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/
typedef enum
{
  SNS_PLBK_ACCEL = 0,
  SNS_PLBK_GYRO,
  SNS_PLBK_MAG,
  SNS_PLBK_PRESS,
  SNS_PLBK_PROX,
  SNS_PLBK_LIG,
  SNS_PLBK_PROX_LIG,
  SNS_PLBK_ACCEL_GYRO,
  SNS_PLBK_ACCEL_GYRO_MAG,
  SNS_PLBK_ACCEL_GYRO_MAG_PRESS,
  SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX,
  SNS_PLBK_ACCEL_GYRO_MAG_PRESS_PROX_LIG,
  SNS_PLBK_ACCEL_GYRO_MAG_PROX,
  SNS_PLBK_ACCEL_GYRO_MAG_PROX_LIG,
  SNS_PLBK_HUMID,
  SNS_PLBK_RGB,
  SNS_PLBK_SAR,
  SNS_PLBK_HALL_EFFECT,
  SNS_PLBK_COMBINATIONS
} sns_plbk_sensor_comb_e;

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

typedef struct plbk_q_item_s
{
  sns_q_link_s       q_link;
  uint32             msg_id;
  uint32             user_data;
  uint16             plbk_msg_len;
  uint16             plbk_decode_len;
  uint8              msg_type;
  void               *body_ptr;
} plbk_q_item_s;

typedef struct plbk_stream_cnt_s
{
  uint32_t one_sns[SNS_PLBK_COMBINATIONS];
  uint32_t all_sns[SNS_PLBK_COMBINATIONS];
} plbk_stream_cnt_s;

typedef struct plbk_strm_res_s
{
  uint16 one_at_a_time;
  uint16 all_together;
} plbk_strm_res_s;

typedef struct plbk_msg_req_s
{
  sns_q_link_s q_link;
  uint8        plbk_cient;
  uint8        report_req;
  void         *rep_req_data;
  unsigned int rep_req_size;
  void         *rep_resp_data;
  unsigned int rep_resp_size;
  void         *resp_clbk_data;
} plbk_msg_req_s;

typedef struct plbk_test_fmt_s
{
   sns_plbk_sensor_comb_e sns_pattern;
   unsigned int samplingfreq;  // in Hz
   unsigned long duration;     // in sec
} plbk_test_fmt_s;

/*---------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/*=====================================================================================
  FUNCTION:  sns_playback_parse_config
=====================================================================================*/
/**
Parse config file for playback.
The config file contains information such as input data file and output log file names.

@retval none
*/
void sns_playback_parse_config(char *fname);

/*=====================================================================================
  FUNCTION:  sns_playback_accel_data
=====================================================================================*/
/**
Obtain accel data from playback file.

@retval sns_ddf_status_e
*/
sns_ddf_status_e sns_playback_accel_data(sns_ddf_sensor_data_s *data_ptr);

/*=====================================================================================
  FUNCTION:  sns_playback_gyro_data
=====================================================================================*/
/**
Obtain gyro data from playback file.

@retval sns_ddf_status_e
*/
sns_ddf_status_e sns_playback_gyro_data(sns_ddf_sensor_data_s *data_ptr);

/*=====================================================================================
  FUNCTION:  sns_playback_mag_data
=====================================================================================*/
/**
Obtain mag data from playback file.

@retval sns_ddf_status_e
*/
sns_ddf_status_e sns_playback_mag_data(sns_ddf_sensor_data_s *data_ptr);

/*=====================================================================================
  FUNCTION:  sns_playback_log_pkt
=====================================================================================*/
/**
Log packets on the playback environment.
The input is a pointer to the message containing packet content.

@retval none
*/
void sns_playback_log_pkt(sns_debug_log_ind_msg_v01 *msg_ptr);

/*=====================================================================================
  FUNCTION:  sns_playback_enable_algo
=====================================================================================*/
/**
Enable algorithms intended for playback (listed in the config file).

@retval none
*/
void sns_playback_enable_algo(void);

/*=====================================================================================
  FUNCTION:  sns_playback_disable_algo
=====================================================================================*/
/**
Disable algorithms that were enabled in the beginning of playback.

@retval none
*/
void sns_playback_disable_algo(void);

/*=========================================================================
  FUNCTION:  sns_playback_enable_log
=========================================================================*/
/*!
    @brief
    Enable output logging

    @return
    None
*/
/*=======================================================================*/
void sns_playback_enable_log(void);

/*=========================================================================
  FUNCTION:  sns_playback_disable_log
=========================================================================*/
/*!
    @brief
    Disable output logging

    @return
    None
*/
/*=======================================================================*/
void sns_playback_disable_log(void);

/*=========================================================================
  FUNCTION:  sns_sensors_playback_test
=========================================================================*/
/*!
    @brief
    Playback test for sensors

    @return
    None
*/
/*=======================================================================*/
int sns_sensors_playback_test(void);

#ifdef SNS_QMI_ENABLE
/*=========================================================================
  FUNCTION:  sns_sensors_stress_test
=========================================================================*/
/*!
    @brief
    Sensors stress test for external clients.

    @param[i]   samples    Number of samples to be streamed for each sensor combination
    @param[i]   frequency Frequency at which the sensors are to be streamed
    @return[i]   output      Stream test results bit values. Each bit represents the combination of sensors in
                                     sns_plbk_sensor_comb_e. If bit is set then status is true else  false.
    TRUE/FALSE
*/
/*=======================================================================*/
BOOLEAN sns_sensors_stress_test(uint32 samples, uint8 frequency, void *output);
#endif

#endif
