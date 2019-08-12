/*============================================================================
  @file Utility.h

  @brief
  Utility class definition.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef ANDROID_UTILITY_H
#define ANDROID_UTILITY_H

#include "Sensor.h"

namespace Utility {

extern hal_sensor_dataq_t* q_head_ptr;
extern hal_sensor_dataq_t* q_tail_ptr;
extern hal_data_cb_t data_cb;

/*===========================================================================
      FUNCTION:  Utility::SensorTypeToSensorString
        Get the sensor type as enum and return sensor type string.
        Return value
            sensor type in string.
===========================================================================*/
    const char * SensorTypeToSensorString(int SensorType);

/*===========================================================================
  FUNCTION:  getDataCb
    Get the data mutex and cond structure.
    Return value
        @hal_data_cb_t* : The sensor data callback pointer.
===========================================================================*/
     hal_data_cb_t* getDataCb();
/*============================================================================
  FUNCTION isMagAvailable
    Check the magnetic sensor is available or not.
    Return value
        true : The sensor is available.
        false : The sensor is not available.
============================================================================*/
     bool isMagAvailable();
/*============================================================================
  FUNCTION isGyroAvailable
    Check the Gyroscope sensor is available or not.
    Return value
        true : The sensor is available.
        false : The sensor is not available.
============================================================================*/
     bool isGyroAvailable();
/*============================================================================
  FUNCTION waitForResponse
    With the response for one sensor1 communication.
    Parameters:
        @timeout : the pthread wait time out time, 0 means wait forever
        @cb_mutex_ptr : the callback metux to protect the cond
        @cond_ptr : the callback cond
        @cond_var : the variable used to indicated the response arrived
    Return value
        true : The sensor1 responses successfully.
        false : The sensor1 dose not response successfully.
============================================================================*/
     bool waitForResponse(int timeout,
                        pthread_mutex_t* cb_mutex_ptr,
                        pthread_cond_t* cond_ptr,
                        bool *cond_var);
/*===========================================================================
  FUNCTION:  QuatToRotMat
    Calculate the RotMat data by the Quat data.
    Parameters
        @rot_mat : The rot_mat data
        @quat : The quat data
===========================================================================*/
     void QuatToRotMat(float rot_mat[9], float quat[4]);
/*===========================================================================
  FUNCTION:  RotMatToOrient
    Calculate the orientation data by the rot_mat data.
    Parameters
        @values : The orientation data
        @rot_mat: The rot_mat data
===========================================================================*/
     void RotMatToOrient(float values[3], float rot_mat[9]);
/*===========================================================================
  FUNCTION:  insertQueue
    Insert the sensor event data to the data queue.
    Parameters
        @data_ptr : the sensor event data
        @wakeup_smple: is it wake up sample or normal sample
        @data_cb : data cb structure to track wake up events
    Return value
        true : Insert the event data successfully.
        false : Insert the event data not successfully.
===========================================================================*/
     bool insertQueue(sensors_event_t const *data_ptr, bool wakeup_smple, hal_data_cb_t* data_cb);
/*===========================================================================
  FUNCTION:  removeFromQueue
    Remove the sensor event data from the data queue.
    Parameters
       @data_ptr   : the sensor event data
       @data_cb    : data cb structure to track wake up events
       @mSensors : Sensors structure storing sensor handles
     Return value
        true : Remove the event data successfully.
        false : Remove the event data not successfully.
===========================================================================*/
     bool removeFromQueue(sensors_event_t* data_ptr, hal_data_cb_t* data_cb, Sensor **mSensors);
/*===========================================================================
  FUNCTION:  signalResponse
    Send the signal for the sensor1 response
    Parameters:
        @error : indicate the error is happened
        @cond_ptr : the pointer to the cond
===========================================================================*/
     void signalResponse(bool error, hal_sensor1_cb_t* sensor1_cb);
/*===========================================================================
  FUNCTION:  signalInd
    Send the siginal for the sensor1 data indicator
    Parameters:
        @is_ind_arrived : the reference to the is_ind_arrived
        @cond_ptr : the reference to the cond
===========================================================================*/
     void signalInd(hal_data_cb_t* data_cb);
/*===========================================================================
  FUNCTION: getSunriseSunsetTimes
    Provides sunrise and sunset times given the latitude/longitude in degrees
    Parameters:
        latitude_in_degrees  - GPS latitude in degrees (North: +ve, South: -ve)
        longitude_in_degrees - GPS longitude in degrees (East: +ve, West: -ve)

    Return value:
        sunrise_ts - seconds in epoch time
        sunset_ts  - seconds in epoch time
===========================================================================*/
     void getSunriseSunsetTimes(const double latitude_in_degrees,
                                const double longitude_in_degrees,
                                uint64_t& sunrise_ts,
                                uint64_t& sunset_ts);

     uint32_t increment_wake_events_in_queue_count(hal_data_cb_t *data_cb);
     uint32_t increment_wake_events_in_queue_count_locked(hal_data_cb_t *data_cb);
     uint32_t decrement_wake_events_in_queue_count(hal_data_cb_t *data_cb);
     uint32_t decrement_wake_events_in_queue_count_locked(hal_data_cb_t *data_cb);
     void acquire_sensor_wake_lock(hal_data_cb_t *data_cb, const char *name);
     void release_sensor_wake_lock(hal_data_cb_t *data_cb, const char *name);
};

#endif
