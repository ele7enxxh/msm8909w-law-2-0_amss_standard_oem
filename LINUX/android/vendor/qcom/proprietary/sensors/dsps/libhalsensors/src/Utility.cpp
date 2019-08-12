/*============================================================================
  @file Utility.cpp

  @brief
  Utility class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Resereted.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#include <errno.h>
#include <time.h>
#include <stdlib.h>
extern "C" {
#include "sensor_reg.h"
}
#include "sns_reg_api_v02.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "Utility.h"
#include "Sensor.h"
#include "SensorsContext.h"

const double C_PI_BY_180 = M_PI / 180.0;
const double C_180_BY_PI = 180.0 / M_PI;
const double NUM_DAYS_IN_YEAR = 365.266667;

hal_sensor_dataq_t* Utility::q_head_ptr = NULL;
hal_sensor_dataq_t* Utility::q_tail_ptr = NULL;
hal_data_cb_t Utility::data_cb = {
                         PTHREAD_MUTEX_INITIALIZER,
                         PTHREAD_COND_INITIALIZER,
                         false, false,
                         PTHREAD_MUTEX_INITIALIZER,
                         0 };

hal_data_cb_t* Utility::getDataCb()
{
    return &data_cb;
}

/*============================================================================
  FUNCTION Utility::SensorTypeToSensorString
    Return the SensorType in String
    Parameters:
    @SensorType :SensorType in Int
============================================================================*/
const char * Utility::SensorTypeToSensorString(int SensorType)
{
    switch(SensorType){
        case SENSOR_TYPE_ACCELEROMETER:
            return SENSOR_STRING_TYPE_ACCELEROMETER;
        case SENSOR_TYPE_GEOMAGNETIC_FIELD:
            return SENSOR_STRING_TYPE_MAGNETIC_FIELD;
        case SENSOR_TYPE_ORIENTATION:
            return SENSOR_STRING_TYPE_ORIENTATION;
        case SENSOR_TYPE_GYROSCOPE:
            return SENSOR_STRING_TYPE_GYROSCOPE;
        case SENSOR_TYPE_LIGHT:
            return SENSOR_STRING_TYPE_LIGHT;
        case SENSOR_TYPE_PRESSURE:
            return SENSOR_STRING_TYPE_PRESSURE;
        case SENSOR_TYPE_TEMPERATURE:
            return SENSOR_STRING_TYPE_TEMPERATURE;
        case SENSOR_TYPE_PROXIMITY:
            return SENSOR_STRING_TYPE_PROXIMITY;
        case SENSOR_TYPE_GRAVITY:
            return SENSOR_STRING_TYPE_GRAVITY;
        case SENSOR_TYPE_LINEAR_ACCELERATION:
            return SENSOR_STRING_TYPE_LINEAR_ACCELERATION;
        case SENSOR_TYPE_ROTATION_VECTOR:
            return SENSOR_STRING_TYPE_ROTATION_VECTOR;
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
            return SENSOR_STRING_TYPE_RELATIVE_HUMIDITY;
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
            return SENSOR_STRING_TYPE_AMBIENT_TEMPERATURE;
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
            return SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
            return SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR;
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
            return SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED;
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
            return SENSOR_STRING_TYPE_SIGNIFICANT_MOTION;
        case SENSOR_TYPE_STEP_DETECTOR:
            return SENSOR_STRING_TYPE_STEP_DETECTOR;
        case SENSOR_TYPE_STEP_COUNTER:
            return SENSOR_STRING_TYPE_STEP_COUNTER;
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
            return SENSOR_STRING_TYPE_GEOMAGNETIC_ROTATION_VECTOR;
        case SENSOR_TYPE_HEART_RATE:
            return SENSOR_STRING_TYPE_HEART_RATE;
        case SENSOR_TYPE_DEVICE_ORIENTATION:
            return SENSOR_STRING_TYPE_DEVICE_ORIENTATION;
        case SENSOR_TYPE_MOTION_ABSOLUTE:
            return SENSOR_STRING_TYPE_MOTION_ABSOLUTE;
        case SENSOR_TYPE_GESTURE_BRING_TO_EAR:
            return SENSOR_STRING_TYPE_GESTURE_BRING_TO_EAR;
#ifdef FEATURE_SNS_HAL_SAM_INT
        case SENSOR_TYPE_GESTURE_FACE_N_SHAKE:
            return SENSOR_STRING_TYPE_GESTURE_FACE_N_SHAKE;
#endif /* FEATURE_SNS_HAL_SAM_INT */
        case SENSOR_TYPE_CMC:
            return SENSOR_STRING_TYPE_CMC;
        case SENSOR_TYPE_FACING:
            return SENSOR_STRING_TYPE_FACING;
        case SENSOR_TYPE_BASIC_GESTURES:
            return SENSOR_STRING_TYPE_BASIC_GESTURES;
        case SENSOR_TYPE_TAP:
            return SENSOR_STRING_TYPE_TAP;
        case SENSOR_TYPE_HALL_EFFECT:
            return SENSOR_STRING_TYPE_HALL_EFFECT;
        case SENSOR_TYPE_IR_GESTURE:
            return SENSOR_STRING_TYPE_IR_GESTURE;
        case SENSOR_TYPE_SCREEN_ORIENTATION:
            return SENSOR_STRING_TYPE_SCREEN_ORIENTATION;
        case SENSOR_TYPE_PAM:
            return SENSOR_STRING_TYPE_PAM;
        case SENSOR_TYPE_PEDOMETER:
            return SENSOR_STRING_TYPE_PEDOMETER;
        case SENSOR_TYPE_RGB:
            return SENSOR_STRING_TYPE_RGB;
        case SENSOR_TYPE_MOTION_RELATIVE:
            return SENSOR_STRING_TYPE_MOTION_RELATIVE;
        case SENSOR_TYPE_SAR:
            return SENSOR_STRING_TYPE_SAR;
        case SENSOR_TYPE_TILT:
            return SENSOR_STRING_TYPE_TILT;
        case SENSOR_TYPE_MOTION_VEHICLE:
            return SENSOR_STRING_TYPE_MOTION_VEHICLE;
        case SENSOR_TYPE_FAST_AMD:
            return SENSOR_STRING_TYPE_FAST_AMD;
        case SENSOR_TYPE_UV:
            return SENSOR_STRING_TYPE_UV;
        case SENSOR_TYPE_THERMOPILE:
            return SENSOR_STRING_TYPE_THERMOPILE;
        case SENSOR_TYPE_CCT:
            return SENSOR_STRING_TYPE_CCT;
        case SENSOR_TYPE_IOD:
            return SENSOR_STRING_TYPE_IOD;
        case SENSOR_TYPE_DPC:
            return SENSOR_STRING_TYPE_DPC;
        case SENSOR_TYPE_MULTISHAKE:
            return SENSOR_STRING_TYPE_MULTISHAKE;
        case SENSOR_TYPE_PMD:
            return SENSOR_STRING_TYPE_PMD;
        case SENSOR_TYPE_STATIONARY_DETECT:
            return SENSOR_STRING_TYPE_STATIONARY_DETECT;
        case SENSOR_TYPE_MOTION_DETECT:
            return SENSOR_STRING_TYPE_MOTION_DETECT;
        default:
            return "";
    }
}

/*============================================================================
  FUNCTION waitForResponse
    Calculate the sensor sample rate
    Parameters:
    @timeout : the pthread wait time out time, 0 means wait forever
    @cb_mutex_ptr : the callback metux to protect the cond
    @cond_ptr : the callback cond
    @cond_var : the variable used to indicated the response arrived
============================================================================*/
bool Utility::waitForResponse(int timeout,
                        pthread_mutex_t *cb_mutex_ptr,
                        pthread_cond_t *cond_ptr,
                        bool *cond_var)
{
    bool ret = false;            /* the return value of this function */
    int rc = 0;                  /* return code from pthread calls */
    struct timeval present_time;
    struct timespec expire_time;

    HAL_LOG_DEBUG("%s: timeout=%d", __FUNCTION__, timeout);

    /* special case where callback is issued before the main function
       can wait on cond */
    if (*cond_var == true) {
        HAL_LOG_DEBUG("%s: cb has arrived without waiting", __FUNCTION__);
        ret = true;
    }
    else {
        /* Calculate absolute expire time */
        gettimeofday(&present_time, NULL);
        /* Convert from timeval to timespec */
        expire_time.tv_sec  = present_time.tv_sec;
        expire_time.tv_nsec = present_time.tv_usec * 1000;
        expire_time.tv_sec += timeout / 1000;
        /* calculate carry over */
        if ((present_time.tv_usec + (timeout % 1000) * 1000) >= 1000000) {
            expire_time.tv_sec += 1;
        }
        expire_time.tv_nsec =
                (expire_time.tv_nsec + (timeout % 1000) * 1000000) % 1000000000;

        while (*cond_var != true && rc != ETIMEDOUT) {
            if(0 == timeout) {
                rc = pthread_cond_wait(cond_ptr, cb_mutex_ptr);
            }
            else {
                /* Wait for the callback until timeout expires */
                rc = pthread_cond_timedwait(cond_ptr, cb_mutex_ptr,
                                            &expire_time);
            }
            if(0 != rc) {
                /*
                 * Skip error message for SMD & DPC as disable request gets sent
                 * from those sensors are expected to fail as request gets sent
                 * as part of indication call back and response for the request
                 * will be blocked.
                 */
                if(timeout != TIME_OUT_MS_DPC_IND &&
                   timeout != TIME_OUT_MS_SMD_IND) {
                    HAL_LOG_ERROR("%s: pthread_cond_timedwait() rc=%d (cond: %i)",
                                    __FUNCTION__, rc, *cond_var);
                }
            }
        }
        ret = ( rc == 0 || *cond_var ) ? true : false;
    }

    *cond_var = false;
    return ret;
}

/*============================================================================
  FUNCTION isMagAvailable
    Check the magnetic sensor is available or not
============================================================================*/
bool Utility::isMagAvailable()
{
    SensorsContext *context = SensorsContext::getInstance();
    return((context->getSensor(HANDLE_MAGNETIC_FIELD) != NULL) ? true : false);
}

/*============================================================================
  FUNCTION isGyroAvailable
    Check the Gyroscope sensor is available or not
============================================================================*/
bool Utility::isGyroAvailable()
{
    SensorsContext *context = SensorsContext::getInstance();
    return((context->getSensor(HANDLE_GYRO) != NULL) ? true : false);
}

/*===========================================================================
  FUNCTION:  QuatToRotMat
  Convert quaternion to rotation matrix

    Quaternion
                Q = |X Y Z W|

    Rotation Matrix
                /  R[ 0]   R[ 1]   R[ 2]   0  \
                |  R[ 4]   R[ 5]   R[ 6]   0  |
                |  R[ 8]   R[ 9]   R[10]   0  |
                \  0       0       0       1  /

   M = 1- 2(Y*Y + Z*Z)  2XY - 2ZW       2XZ + 2YW       0
       2XY + 2ZW        1 - 2(XX + ZZ)  2YZ - 2XW       0
       2XZ - 2YW        2YZ + 2XW       1 - 2(XX + ZZ)  0
       0                0               0               1
===========================================================================*/
void Utility::QuatToRotMat(float rot_mat[9], float quat[4])
{
    float X = quat[0];
    float Y = quat[1];
    float Z = quat[2];
    float W = quat[3];

    float xx = X * X;
    float xy = X * Y;
    float xz = X * Z;
    float xw = X * W;
    float yy = Y * Y;
    float yz = Y * Z;
    float yw = Y * W;
    float zz = Z * Z;
    float zw = Z * W;

    HAL_LOG_DEBUG("%s: X=%f, Y=%f, Z=%f, W=%f", __FUNCTION__, X, Y, Z, W);

    rot_mat[0]  = 1 - 2 * ( yy + zz );
    rot_mat[1]  =     2 * ( xy - zw );
    rot_mat[2]  =     2 * ( xz + yw );
    rot_mat[3]  =     2 * ( xy + zw );
    rot_mat[4]  = 1 - 2 * ( xx + zz );
    rot_mat[5]  =     2 * ( yz - xw );
    rot_mat[6]  =     2 * ( xz - yw );
    rot_mat[7]  =     2 * ( yz + xw );
    rot_mat[8]  = 1 - 2 * ( xx + yy );
}

/*===========================================================================

  FUNCTION:  RotMatToOrient
  Convert rotation matrix to Orientation Sensor as defined in Sensor.TYPE_ORIENTATION:

    values[0]: Azimuth, angle between the magnetic north direction and the y-axis,
    around the z-axis (0 to 359). 0=North, 90=East, 180=South, 270=West

    values[1]: Pitch, rotation around x-axis (-180 to 180),
    with positive values when the z-axis moves toward the y-axis.

    values[2]: Roll, rotation around y-axis (-90 to 90),
    with positive values when the x-axis moves toward the z-axis.

===========================================================================*/
void Utility::RotMatToOrient(float values[3], float rot_mat[9])
{
    float xunit[3] = {rot_mat[0], rot_mat[3], rot_mat[6]};
    float yunit[3] = {rot_mat[1], rot_mat[4], rot_mat[7]};
    float zunit[3] = {rot_mat[2], rot_mat[5], rot_mat[8]};
    float xnorm = sqrt(xunit[0]*xunit[0] + xunit[1]*xunit[1]);

    if (fabs(zunit[2]) < MIN_FLT_TO_AVOID_SINGULARITY) {
        zunit[2] = MIN_FLT_TO_AVOID_SINGULARITY * (zunit[2] < 0 ? -1 : 1);
    }

    if (fabs(xunit[0]) < MIN_FLT_TO_AVOID_SINGULARITY) {
        xunit[0] = MIN_FLT_TO_AVOID_SINGULARITY * (xunit[0] < 0 ? -1 : 1);
    }

    if (fabs(xnorm) < MIN_FLT_TO_AVOID_SINGULARITY) {
        xnorm = MIN_FLT_TO_AVOID_SINGULARITY * (xnorm < 0 ? -1 : 1);
    }

    values[0] = RAD2DEG * atan2(xunit[1], xunit[0]);
    values[0] = fmodf(360.0f - values[0], 360.0f);
    values[1] = -RAD2DEG * atan2(yunit[2], zunit[2]);
    values[2] = RAD2DEG * atan2(xunit[2], xnorm);
}

/*===========================================================================

  FUNCTION:  insertQueue

===========================================================================*/
bool Utility::insertQueue(sensors_event_t const *data_ptr,
                bool wakeup_sample, hal_data_cb_t* data_cb)
{
    hal_sensor_dataq_t* q_ptr;
    bool rv = true;

    q_ptr = (hal_sensor_dataq_t*)malloc(sizeof(hal_sensor_dataq_t));
    if (NULL != q_ptr) {
        q_ptr->next = NULL;
        q_ptr->data = *data_ptr;

        if (q_head_ptr == NULL) {
        /* queue is empty */
            q_tail_ptr = q_ptr;
            q_head_ptr = q_ptr;
        } else {
        /* append to tail and update tail ptr */
            q_tail_ptr->next = q_ptr;
            q_tail_ptr = q_ptr;
        }
    } else {
        HAL_LOG_ERROR("%s: malloc() error", __FUNCTION__);
        rv = false;
    }
    /*increment the wakeup count*/
    if ((rv == true) && (wakeup_sample == true)) {
        increment_wake_events_in_queue_count(data_cb);
    }
    return rv;
}

/*===========================================================================

  FUNCTION:  removeFromQueue
  @brief
  Removes sensor data from head of the queue
  Helper function

  @param data_ptr: pointer to data
  @param data_cb    : data cb structure to track wake up events
  @param mSensors : Sensors structure storing sensor handles

  @return true if data exists in the queue

  @dependencies Caller needs to lock the g_sensor_control->data_mutex before
                calling this function
===========================================================================*/
bool Utility::removeFromQueue(sensors_event_t* data_ptr,
                     hal_data_cb_t* data_cb, Sensor **mSensors)
{
    hal_sensor_dataq_t* q_ptr;
    bool rv = false;
    int sensorFlag = -1;

    if (NULL != q_head_ptr) {
        /* copy the data from head */
        q_ptr = q_head_ptr;
        *data_ptr = q_head_ptr->data;

        /* update the pointers */
        if (q_head_ptr == q_tail_ptr) {
            /* queue has only one element */
            q_tail_ptr = NULL;
        }
        q_head_ptr = q_head_ptr->next;

        free(q_ptr);
        rv = true;
    }

    if ((rv == true) && (SENSOR_TYPE_META_DATA == data_ptr->type)) {
        if (mSensors[data_ptr->meta_data.sensor]->getFlags()&SENSOR_FLAG_WAKE_UP) {
           decrement_wake_events_in_queue_count(data_cb);
        }
        return rv;
    }

    /*decrement the wake up count in the queue*/
    if ((rv == true) && (mSensors[data_ptr->sensor]->getFlags()&SENSOR_FLAG_WAKE_UP)) {
       decrement_wake_events_in_queue_count(data_cb);
    }
    return rv;
}

/*===========================================================================
  FUNCTION:  signalResponse()
    Send the signal for the sensor1 response
    Parameters:
    @error : indicate the error is happened
    @cond_ptr : the pointer to the cond
===========================================================================*/
void Utility::signalResponse(bool error, hal_sensor1_cb_t* sensor1_cb)
{
    sensor1_cb->error = error;
    sensor1_cb->is_resp_arrived = true;
    pthread_cond_signal(&sensor1_cb->cb_cond);
}

/*===========================================================================
  FUNCTION:  signalInd()
    Send the siginal for the sensor1 data indicator
    Parameters:
    @is_ind_arrived : the reference to the is_ind_arrived
    @cond_ptr : the reference to the cond
===========================================================================*/
void Utility::signalInd(hal_data_cb_t* data_cb)
{
    data_cb->is_ind_arrived = true;
    pthread_cond_signal(&data_cb->data_cond);
}

/*===========================================================================
  FUNCTION:  decrement_wake_events_in_queue_count_locked()
    Decrement our counter tracking the number of wake events currently in the
    queue.  This serves as our wakelock reference counter.
    NOTE: Caller of this routine must hold data_cb->wakelock_mutex.
    Parameters:
    @data_cb : data callback
===========================================================================*/
uint32_t Utility::decrement_wake_events_in_queue_count_locked(hal_data_cb_t *data_cb)
{
    if (data_cb->wake_events_in_queue > 0) {
        data_cb->wake_events_in_queue--;
    } else {
        HAL_LOG_DEBUG("%s: tried decrementing counter past zero!", __FUNCTION__);
    }
    return data_cb->wake_events_in_queue;
}

/*===========================================================================
  FUNCTION:  decrement_wake_events_in_queue_count()
    Decrement our counter tracking the number of wake events currently in the
    queue.  This serves as our wakelock reference counter.
    Parameters:
    @data_cb : data callback
===========================================================================*/
uint32_t Utility::decrement_wake_events_in_queue_count(hal_data_cb_t *data_cb)
{
    uint32_t wake_events_in_queue;
    pthread_mutex_lock(&data_cb->wakelock_mutex);
    wake_events_in_queue = decrement_wake_events_in_queue_count_locked(data_cb);
    HAL_LOG_DEBUG("%s: Decremented wake events in queue to %d", __FUNCTION__,
                        wake_events_in_queue);
    pthread_mutex_unlock(&data_cb->wakelock_mutex);
    return wake_events_in_queue;
}

/*===========================================================================
  FUNCTION:  increment_wake_events_in_queue_count_locked()
    Increment our counter tracking the number of wake events currently in the
    queue.  This serves as our wakelock reference counter.
    NOTE: Caller of this routine must hold data_cb->wakelock_mutex.
    Parameters:
    @data_cb : data callback
===========================================================================*/
uint32_t Utility::increment_wake_events_in_queue_count_locked(hal_data_cb_t *data_cb)
{
    data_cb->wake_events_in_queue++;
    return data_cb->wake_events_in_queue;
}

/*===========================================================================
  FUNCTION:  increment_wake_events_in_queue_count()
    Increment our counter tracking the number of wake events currently in the
    queue.  This serves as our wakelock reference counter.
    Parameters:
    @data_cb : data callback
===========================================================================*/
uint32_t Utility::increment_wake_events_in_queue_count(hal_data_cb_t *data_cb)
{
    uint32_t wake_events_in_queue;
    pthread_mutex_lock(&data_cb->wakelock_mutex);
    wake_events_in_queue = increment_wake_events_in_queue_count_locked(data_cb);
    HAL_LOG_DEBUG("%s: incremented wake_events_in_queue to %d", __FUNCTION__, wake_events_in_queue);

    pthread_mutex_unlock(&data_cb->wakelock_mutex);
    return wake_events_in_queue;
}

/*===========================================================================
  FUNCTION:  acquire_sensor_wake_lock()
    Acquire a referenced wakelock.  If it's already held, just increment
    reference.  References tracked vi wake_events_in_queue.
    Parameters:
    @data_cb : data callback
    @name: name of calling function (for logging purposes)
===========================================================================*/
void Utility::acquire_sensor_wake_lock(hal_data_cb_t *data_cb, const char *name)
{
    pthread_mutex_lock(&data_cb->wakelock_mutex);
    if (data_cb->sensors_wakelock_held == false) {
        acquire_wake_lock( PARTIAL_WAKE_LOCK, SENSORS_WAKE_LOCK );
        data_cb->sensors_wakelock_held = true;
        HAL_LOG_DEBUG("%s: acquired wakelock %s", name, SENSORS_WAKE_LOCK);
    } else {
        HAL_LOG_DEBUG("%s: already has wakelock %d",
                name, data_cb->wake_events_in_queue);
    }
    pthread_mutex_unlock(&data_cb->wakelock_mutex);
}

/*===========================================================================
  FUNCTION:  release_sensor_wake_lock()
    Release a reference on a wakelock.  If it's the last reference, release
    the wakelock.  The references are tracked via data_cb->wake_events_in_queue.
    Parameters:
    @data_cb : data callback
    @name: name of calling function (for logging purposes)
===========================================================================*/
void Utility::release_sensor_wake_lock(hal_data_cb_t *data_cb, const char *name)
{
    pthread_mutex_lock(&data_cb->wakelock_mutex);
    if (data_cb->wake_events_in_queue == 0 && data_cb->sensors_wakelock_held) {
        data_cb->sensors_wakelock_held = false;
        release_wake_lock( SENSORS_WAKE_LOCK );
        HAL_LOG_DEBUG("%s: released wakelock %s", name, SENSORS_WAKE_LOCK);
    } else {
        HAL_LOG_DEBUG("%s: release_sensor_wake_lock called but still events in queue %d",
                name, data_cb->wake_events_in_queue);
    }
    pthread_mutex_unlock(&data_cb->wakelock_mutex);
}

/*===========================================================================
  sine function that takes input in degrees
===========================================================================*/
double sind(const double x)
{
    return sin(x * C_PI_BY_180);
}

/*===========================================================================
  cosine function that takes input in degrees
===========================================================================*/
double cosd(const double x)
{
    return cos(x * C_PI_BY_180);
}

/*===========================================================================
  inverse sine function that takes input in degrees
===========================================================================*/
double asind(const double x)
{
    return (asin(x) * C_180_BY_PI);
}

/*===========================================================================
  inverse cosine function that takes input in degrees
===========================================================================*/
double acosd(const double x)
{
    return (acos(x) * C_180_BY_PI);
}

/*===========================================================================
  FUNCTION: getDayInYear
    Converts month/day to day_in_year for use in getSunriseSunsetTimes
    day_in_year - The day number in the year. For Feb 1, day_in_year = 32
                  Must be between 1 and 365
                  For Feb 29 (during leap years), outputs 60
===========================================================================*/
int getDayInYear(const int month,
                 const int day)
{
    int day_in_year = 0;

    if (month == 1)
        day_in_year = day;
    else if (month == 2)
        day_in_year = 31 + day;
    else if (month == 3)
        day_in_year = 59 + day;
    else if (month == 4)
        day_in_year = 90 + day;
    else if (month == 5)
        day_in_year = 120 + day;
    else if (month == 6)
        day_in_year = 151 + day;
    else if (month == 7)
        day_in_year = 181 + day;
    else if (month == 8)
        day_in_year = 212 + day;
    else if (month == 9)
        day_in_year = 243 + day;
    else if (month == 10)
        day_in_year = 273 + day;
    else if (month == 11)
        day_in_year = 304 + day;
    else if (month == 12)
        day_in_year = 334 + day;

    return (day_in_year);
}

/*===========================================================================
  FUNCTION: UTCtoEpoch
    Converts UTC time to Epoch time
===========================================================================*/
uint64_t UTCtoEpoch(const int year,
                    const int month,
                    const int day,
                    const double seconds_from_midnight)
{
    const double epoch_time_jan_1_2000 = 946684800.0; /* January 1, 2000: 0h 0m 0s */

    double epoch_time_s = epoch_time_jan_1_2000 +
                          ((year-2000)*NUM_DAYS_IN_YEAR +
                          (getDayInYear(month, day)-1))*86400.0 + seconds_from_midnight;

    return ((uint64_t) epoch_time_s);

}

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
void Utility::getSunriseSunsetTimes(const double latitude_in_degrees,
                           const double longitude_in_degrees,
                           uint64_t& sunrise_ts,
                           uint64_t& sunset_ts)
{
    struct timeval tv;
    struct tm ts;
    struct tm *ts_ptr;
    int year, month, day;
    /* Elevation above sea level */
    const double elevation_above_sealevel = 0.0;

    HAL_LOG_DEBUG("%s: latitude_in_degrees: %f longitude_in_degrees: %f",
                  __FUNCTION__, latitude_in_degrees, longitude_in_degrees);
    gettimeofday(&tv, NULL);
    ts_ptr = localtime(&tv.tv_sec);
    if (ts_ptr == NULL) {
        HAL_LOG_DEBUG("%s: Error getting localtime", __FUNCTION__);
        return;
    }
    ts = *ts_ptr;
    year = ts.tm_year + 1900;
    month = ts.tm_mon + 1;
    day = ts.tm_mday;

    int day_in_year = getDayInYear(month, day);

    /* Julian date for Jan 1, 2000 */
    const int julian_date_jan_1_2000 = 2451545.0; /* January 1, 2000: 12h 0m 0s */

    // Calculate current Julian cycle
    double julian_date = julian_date_jan_1_2000 + (year-2000.0)*NUM_DAYS_IN_YEAR + day_in_year;
    double julian_cycle = floor(julian_date - julian_date_jan_1_2000 + longitude_in_degrees / 360.0 + 0.5);

    /* Approximate solar noon */
    double julian_date_solarnoon = julian_date_jan_1_2000 - longitude_in_degrees / 360.0 + julian_cycle;

    /* Solar mean anomaly */
    double mean_anomaly =  0.98560028 * julian_date_solarnoon - 2415885.90933; /* modulo 360 ignored */
    double sind_mean_anomaly = sind(mean_anomaly);

    /* Equation of center */
    double equation_of_center = 1.9148 * sind_mean_anomaly + 0.02 * sind(2*mean_anomaly) + 0.003 * sind(3*mean_anomaly);

    /* Ecliptic longitude */
    double ecliptic_longitude = mean_anomaly + equation_of_center + 282.9372; /* modulo 360 ignored */

    /* Solar transit */
    double julian_date_solartransit = julian_date_solarnoon + 0.0053 * sind_mean_anomaly - 0.0069 * sind (2*ecliptic_longitude);

    /* Declination of the Sun */
    double sine_declination = sind (ecliptic_longitude) * 0.3979; /* sind (23.45) = 0.3979 */

    /* Hour angle */
    double hour_angle_by_360 = 0.0;
    if (elevation_above_sealevel!=0.0)
    {
        double correction_term = -0.0346 * sqrt(elevation_above_sealevel);
        double hour_angle = acosd((sind(-0.83 + correction_term) - sind(latitude_in_degrees) * sine_declination) /
                            (cosd(latitude_in_degrees) * cosd(asind(sine_declination))));
        hour_angle_by_360 = hour_angle / 360.0;
    }
    else
    {
        double hour_angle = acosd((-0.0145 - sind(latitude_in_degrees) * sine_declination) /
                            (cosd(latitude_in_degrees) * cosd(asind(sine_declination))));
        hour_angle_by_360 = hour_angle / 360.0;
    }

    /* Calculate sunrise and sunset */
    double julian_date_sunset  = julian_date_solartransit + hour_angle_by_360;
    double julian_date_sunrise = julian_date_solartransit - hour_angle_by_360;

    double sunrise_seconds_from_midnight_utc = ((julian_date_sunrise - julian_date) * 24.0 + 12.0)*3600.0;
    double sunset_seconds_from_midnight_utc  = ((julian_date_sunset  - julian_date) * 24.0 + 12.0)*3600.0;

    /* output in number of seconds in UTC epoch */
    sunrise_ts = UTCtoEpoch(year, month, day, sunrise_seconds_from_midnight_utc);
    sunset_ts  = UTCtoEpoch(year, month, day, sunset_seconds_from_midnight_utc );
    HAL_LOG_DEBUG("%s: sunrise_ts: %" PRId64 " sunset_ts: %" PRId64 "", __FUNCTION__, sunrise_ts, sunset_ts);
}
