/*============================================================================
  @file PersistentMotionDetector.h

  @brief
  PersistentMotionDetector class definition.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#ifndef ANDROID_PMD_SENSOR_H
#define ANDROID_PMD_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_pmd_v01.h"

/**
 * @brief class for Persistent Motion Detector sensor. this
 *        interfaces with PMD algorithm and provides support for
 *        persistent rest and motion detection
 *
 */
class PersistentMotionDetector : public SAMSensor
{
  int32_t sensorHandle;
  static const uint32_t PMD_DEFAULT_MONITOR_PERIOD = 5000;

  protected:
  bool oneShotMode;
  sns_sam_pmd_mode_e_v01 monitorMode;
  uint32_t monitorPeriod;
  int32_t sensorType;

  /**
   * @brief Deactivate PMD after indication is received, to ensure
   *        one-shot-mode
   */
  void deactivate();

  public:
  PersistentMotionDetector(int handle);

  /**
   * @brief enable/disable PMD sensor
   *
   * @param [in] en 1: enable, 0 :disable
   *
   * @return int
   */
  int enable(int en);

  /**
   * @brief Process the response to the sensor1
   *        SENSOR1_MSG_TYPE_RESP
   *
   *
   * @param [in] msg_hdr sensor1 message header
   * @param [in] msg_ptr sensor1 message data
   */
  void processResp(sensor1_msg_header_s* msg_hdr, void* msg_ptr);

  /**
   * @brief Process the response to the sensor1
   *        SENSOR1_MSG_TYPE_IND
   *
   * @param [in] msg_hdr sensor1 message header
   * @param [in] msg_ptr sensor1 message data
   */
  void processInd(sensor1_msg_header_s* msg_hdr, void* msg_ptr);
};


/**
 * @brief class to support SENSOR_TYPE_STATIONARY_DETECT, which
 *        is a special case of persistent motion detector
 */
class AndroidStationaryDetector : public PersistentMotionDetector
{
  public:
  AndroidStationaryDetector(int handle);
};

/**
 * @brief class to support SENSOR_TYPE_MOTION_DETECT, which is a
 *        special case of persistent motion detector
 */
class AndroidMotionDetector : public PersistentMotionDetector
{
  public:
  AndroidMotionDetector(int handle);
};

#endif
