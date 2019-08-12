/*============================================================================
  @file SensorContext.cpp

  @brief
  SensorContext class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#include <stdlib.h>
#include <cutils/properties.h>
extern "C" {
#include "sensor_reg.h"
}
#include "sns_reg_api_v02.h"
#include "common_log.h"

#include "Sensor.h"
#include "Latency.h"
#include "Utility.h"
#include "Recovery.h"
#include "TimeSyncService.h"
#include "SensorsContext.h"
/* SMGR sensors */
#include "Accelerometer.h"
#include "Gyroscope.h"
#include "GyroscopeUncalibrated.h"
#include "Magnetic.h"
#include "MagneticUncalibrated.h"
#include "MagneticUncalibratedSAM.h"
#include "Light.h"
#include "Proximity.h"
#include "Pressure.h"
#include "Humidity.h"
#include "Temperature.h"
#include "SMGRStepCounter.h"
#include "SMGRStepDetector.h"
#include "SMGRSMD.h"
#include "SMGRGameRV.h"
#include "RGB.h"
#include "IRGesture.h"
#include "SAR.h"
#include "HallEffect.h"
#include "UV.h"
#include "Thermopile.h"
#include "CCT.h"
/* SAM sensors */
#include "AbsoluteMotionDetector.h"
#include "BringToEar.h"
#include "CoarseMotionClassifier.h"
#include "DevicePositionClassifier.h"
#include "Facing.h"
#include "GameRotationVector.h"
#include "GeoMagneticRotationVector.h"
#include "Gestures.h"
#include "Gravity.h"
#include "LinearAcceleration.h"
#include "MagneticCalibration.h"
#include "Orientation.h"
#include "Pedometer.h"
#include "RelativeMotionDetector.h"
#include "RotationVector.h"
#include "SignificantMotionDetector.h"
#include "StepCounter.h"
#include "StepDetector.h"
#include "Tap.h"
#include "TiltDetector.h"
#include "Thresh.h"
#include "VehicleMotionDetector.h"
#include "LowPowerLandscapePortrait.h"
#include "QHeart.h"
#include "FastAbsoluteMotionDetector.h"
#include "WristTilt.h"
#include "IOD.h"
#include "PickUpGesture.h"
#include "MultiShake.h"
#include "DeviceOrientation.h"
#include "PersistentMotionDetector.h"

//init sensor hal debug level
uint32_t sns_hal_debug_level;

SensorsContext SensorsContext::self;

/*============================================================================
  SensorsContext Constructor
============================================================================*/
SensorsContext::SensorsContext()
    : active_sensors(0),
      is_accel_available(false),
      is_gyro_available(false),
      is_mag_available(false),
      is_prox_available(false),
      smgr_version(0)
{
    int i;
    int err;

    /* Enable logging */
    enableLogging();
    HAL_LOG_INFO("%s", __FUNCTION__);

    char prop_val[PROPERTY_VALUE_MAX] = {0};
    property_get("debug.sns.hal.ftrace", prop_val, "0");
    sns_hal_debug_level = atoi(prop_val);
    prop_val[0] = '\0';

    /* Release wakelock if it is NOT due to any reason */
    release_wake_lock( SENSORS_WAKE_LOCK );

    /* Init sensor1 before every sensor1 connection */
    sensor1_init();

    /* get the data_cb form the Utility class*/
    data_cb = Utility::getDataCb();
    /* smgr_sensor1_cb is used by the SMGR sensor1 connection */
    smgr_sensor1_cb = SMGRSensor::getSMGRSensor1Cb();
    /* sensor_info_sensor1_cb is used by the getting the SMGR sensor list */
    sensor_info_sensor1_cb = new hal_sensor1_cb_t;
    memset(sensor_info_sensor1_cb, 0, sizeof(*sensor_info_sensor1_cb));
    sensor_info_sensor1_cb->is_resp_arrived = false;
    sensor_info_sensor1_cb->error = false;

    /* Init the mSensors */
    for( i = 0; i < MAX_NUM_SENSORS; i++ ) {
        mSensors[i] = NULL;
    }

    /* open the general SMGR sensors sensor1 connection */
    err = sensor1_open(&smgr_sensor1_cb->sensor1_handle, &SMGRSensor_sensor1_cb, (intptr_t)this);
    if(SENSOR1_SUCCESS != err) {
        HAL_LOG_ERROR("sensor1 open failed!");
        return;
    }
    /* open the sensor_info SMGR sensors sensor1 connection */
    err = sensor1_open(&sensor_info_sensor1_cb->sensor1_handle, &context_sensor1_cb, (intptr_t)this);
    if(SENSOR1_SUCCESS != err) {
        HAL_LOG_ERROR("sensor1 open failed!");
        return;
    }

    /* Check the NV3801 for Magnetic sensor source */
    checkMagCalSource();

    /* Init the mutex and cond */
    pthread_mutex_init(&active_mutex, NULL);
    pthread_mutex_init(&(sensor_info_sensor1_cb->cb_mutex), NULL);
    pthread_cond_init(&(sensor_info_sensor1_cb->cb_cond), NULL);

    /* Time Service */
    time_service = TimeSyncService::getTimeSyncService();

    char wu_sensor[PROPERTY_VALUE_MAX] = "false";
    property_get( HAL_PROP_WU_SENSORS, wu_sensor, "true" );
    if (!strncmp("true", wu_sensor, 4)) {
        HAL_LOG_DEBUG("%s: Wake Up Sensors are enabled!", __FUNCTION__);
    } else {
        HAL_LOG_DEBUG("%s: Wake Up sensors disabled!", __FUNCTION__);
    }

    err = sendSMGRVersionReq();
    if (err) {
        HAL_LOG_ERROR("%s: SMGR version request failed!", __FUNCTION__);
    }

    err = getSensorList();
    if (err) {
        HAL_LOG_ERROR("%s: get sensor list failed!", __FUNCTION__);
    }

    for (i=0; i < ARRAY_SIZE(g_sensor_list_order); i++) {
        int handle = g_sensor_list_order[i];
        /* Skip SMGR sensors as they are added above - in getSensorList */
        if (handle < SAM_HANDLE_BASE)
            continue;
        /* Skip wake up sensors if they aren't enabled */
        if (!strncmp("false", wu_sensor, 5) &&
            ((handle < SAM_HANDLE_BASE && handle >= WU_HANDLE_BASE) ||
             (handle < TXN_ID_BASE && handle >= WU_SAM_HANDLE_BASE)))
            continue;

        if (is_accel_available  ||
            handle == HANDLE_PROXIMITY ||
            handle == HANDLE_PROXIMITY_NON_WAKE_UP) {
            addSensor(handle);
        }
    }

    err = updateSensorList();
    if (err) {
        HAL_LOG_ERROR("%s: update sensor list failed!", __FUNCTION__);
    }

    /* Close the sensor_info_sensor1_cb sensor1 connection and clean the data */
    sensor1_close(sensor_info_sensor1_cb->sensor1_handle);
    pthread_mutex_destroy(&sensor_info_sensor1_cb->cb_mutex);
    pthread_cond_destroy(&sensor_info_sensor1_cb->cb_cond);
    delete sensor_info_sensor1_cb;

    /* Enable latency check */
    Latency::latencyCheckMeasure();
}

/*============================================================================
  SensorsContext Destructor
============================================================================*/
SensorsContext::~SensorsContext()
{
    HAL_LOG_DEBUG("%s", __FUNCTION__ );

    /* free up resources */
    if(smgr_sensor1_cb != NULL) {
        HAL_LOG_WARN("%s: closing smgr sensor1 connection!", __FUNCTION__);
        /* close sensor1 */
        sensor1_close(smgr_sensor1_cb->sensor1_handle);

        /* clean up mutex and cond var  */
        pthread_mutex_destroy(&smgr_sensor1_cb->cb_mutex );
        pthread_cond_destroy(&smgr_sensor1_cb->cb_cond );

        /* free up memory */
        delete smgr_sensor1_cb;
    }
    /* Close Time Service */
    delete time_service;
}

/*===========================================================================
  FUNCTION:  getInstance
    Get the singleton SensorContext object.
===========================================================================*/
SensorsContext* SensorsContext::getInstance()
{
    return &self;
}

/*===========================================================================
  FUNCTION:  getSensor1Cb
    Return the sensor_info_sensor1_cb;
===========================================================================*/
hal_sensor1_cb_t* SensorsContext::getSensor1Cb()
{
    return sensor_info_sensor1_cb;
}

/*===========================================================================
  FUNCTION:  getSensors
    Get all sensors objects.
===========================================================================*/
Sensor** SensorsContext::getSensors()
{
    return mSensors;
}

/*===========================================================================
  FUNCTION:  getSensor
    Get the sensor handle object.
===========================================================================*/
Sensor* SensorsContext::getSensor(int handle)
{
    if (mSensors[handle] != NULL) {
        return mSensors[handle];
    } else {
        HAL_LOG_DEBUG("SensorsContext::getSensor handle %d is NULL!", handle);
        return NULL;
    }
}

int SensorsContext::activate(int handle, int en)
{
    int err;
    HAL_LOG_VERBOSE("%s: handle is %d, en is %d", __FUNCTION__, handle, en);
    /* check all sensors and the time_service status */
    pthread_mutex_lock(&active_mutex);
    if (en) {
        time_service->timeServiceStart();
        active_sensors |= (1ULL << handle);
    }
    else {
        active_sensors &= ~(1ULL << handle);
        if(0 == active_sensors && 1 == time_service->getTimeSyncServiceStatus()) {
            HAL_LOG_VERBOSE("All sensors stop, stop the time_service.");
            time_service->timeServiceStop();
        }
    }
    pthread_mutex_unlock(&active_mutex);
    HAL_LOG_VERBOSE("activate sensors is %" PRIx64"", active_sensors);

    if (true == mSensors[handle]->getAttribOK()) {
        err = mSensors[handle]->enable(en);
        if (err) {
            HAL_LOG_ERROR("Activate the handle %d is not successful", handle);
            active_sensors &= ~(1ULL << handle);
        }
    }
    else {
        HAL_LOG_ERROR("The handle %d is not available!", handle);
        err = -EINVAL;
    }
    return err;
}

int SensorsContext::poll(sensors_event_t* data, int count)
{
    int i = 0;
    int err;
    bool wakeup_event_being_processed = false;
    int sensorFlag = -1;
    HAL_LOG_DEBUG("%s: count: %d", __FUNCTION__, count );
    ATRACE_BEGIN("SSCHAL:SensorsContext::poll");

    pthread_mutex_lock(&data_cb->data_mutex);
    while(i < count) {
        /* check if any responses have been buffered */
        if(!Utility::removeFromQueue(&data[i], data_cb, mSensors)) {
            break;
        }
        i++;
    }

    /* Release wakelock if held */
    if(data_cb->sensors_wakelock_held == true)
        Utility::release_sensor_wake_lock(data_cb, __FUNCTION__);

    ATRACE_BEGIN("SensorsContext::poll->waitForResponse");
    while(i == 0) {
        data_cb->is_ind_arrived = false;
        /* wait for notify cb - wait indefinitely */
        err = Utility::waitForResponse(0, &data_cb->data_mutex,
                        &data_cb->data_cond,
                        &data_cb->is_ind_arrived);
        if(err == false) {
            pthread_mutex_unlock(&data_cb->data_mutex);
            ATRACE_END();
            return -ETIMEDOUT;
        }
        /* Data received */
        while(i < count && Utility::removeFromQueue(&data[i], data_cb, mSensors)) {
            i++;
        }
    }
    ATRACE_END();

    /* latency mesaure */
    if (Latency::isLatencyMeasureEnabled()) {
        struct timespec current_time;
        uint64_t curr_timestamp = android::elapsedRealtimeNano();
        Latency::latencyMeasure(curr_timestamp, data, i);
    }

    pthread_mutex_unlock(&data_cb->data_mutex);
    HAL_LOG_DEBUG("%s:polldata:%d, sensor:%d, type:%d, x:%f y:%f z:%f",
                    __FUNCTION__, i,
                    data[0].sensor, data[0].type,
                    data[0].acceleration.x,
                    data[0].acceleration.y,
                    data[0].acceleration.z);
    ATRACE_END();
    return i;
}

int SensorsContext::batch(int handle, int flags,
                             int64_t period_ns, int64_t timeout)
{
    int err;
    if (true == mSensors[handle]->getAttribOK()) {
        err = mSensors[handle]->batch(flags, period_ns, timeout);
        if (err) {
            HAL_LOG_ERROR("%s: handle %d batch is not successful", __FUNCTION__, handle);
        }
    }
    else {
        HAL_LOG_ERROR("%s: The handle %d is not available!", __FUNCTION__,  handle);
        err = -EINVAL;
    }
    return err;
}

int SensorsContext::flush(int handle)
{
    int err;

    if (true == mSensors[handle]->getAttribOK()) {
        sensor_trigger_mode sensor_type;
        sensor_type = mSensors[handle]->getTriggerMode();

        if (sensor_type == SENSOR_MODE_TRIG) {
            HAL_LOG_ERROR("%s: The handle %d is one-shot sensor!", __FUNCTION__, handle);
            err = -EINVAL;
        }

        err = mSensors[handle]->flush();
        if (err) {
            HAL_LOG_ERROR("%s: handle %d flush is not successful", __FUNCTION__, handle);
        }
    }
    else {
        HAL_LOG_ERROR("%s: The handle %d is not available!", __FUNCTION__, handle);
        err = -EINVAL;
    }
    return err;
}

/*===========================================================================
  FUNCTION:  enableLogging
    Enable the log level by getting the property.
===========================================================================*/
void SensorsContext::enableLogging()
{
    int debug_prop_len;
    char debug_prop[PROPERTY_VALUE_MAX];

    /* Get the debug level from the property */
    debug_prop_len = property_get(HAL_PROP_DEBUG, debug_prop, "");
    if (debug_prop_len == 1) {
        switch (debug_prop[0]) {
            case '0':
                g_hal_log_level = HAL_LOG_LEVEL_DISABLED;
                break;
            case '1':
                g_hal_log_level = HAL_LOG_LEVEL_ALL;
                break;
            case 'v':
            case 'V':
                g_hal_log_level = HAL_LOG_LEVEL_VERBOSE;
                break;
            case 'd':
            case 'D':
                g_hal_log_level = HAL_LOG_LEVEL_DEBUG;
                break;
            case 'i':
            case 'I':
                g_hal_log_level = HAL_LOG_LEVEL_INFO;
                break;
            case 'w':
            case 'W':
                g_hal_log_level = HAL_LOG_LEVEL_WARN;
                break;
            case 'e':
            case 'E':
                g_hal_log_level = HAL_LOG_LEVEL_ERROR;
                break;
            default:
                break;
        }
        LOGI("%s: Setting log level to %d", __FUNCTION__, g_hal_log_level);
    }
    else if (debug_prop_len > 1) {
        LOGE("%s: invalid value for %s: %s. Enabling all logs", __FUNCTION__,
            HAL_PROP_DEBUG, debug_prop);
        g_hal_log_level = HAL_LOG_LEVEL_ALL;
    }
}

/*===========================================================================
  FUNCTION:  addSensor
    Init the sensor handle class.
    Parameters:
    @handle: the sensor handle
===========================================================================*/
void SensorsContext::addSensor(int handle)
{
    char add_sensor[PROPERTY_VALUE_MAX] = "false";

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    switch(handle){
        /* SMGR sensors */
        case HANDLE_ACCELERATION:
        case HANDLE_ACCELERATION_WAKE_UP:
            mSensors[handle] = new Accelerometer(handle);
            break;
        case HANDLE_GYRO:
        case HANDLE_GYRO_WAKE_UP:
            if (is_gyro_available) {
                HAL_LOG_DEBUG("%s: GYRO enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Gyroscope(handle);
            } else {
                HAL_LOG_DEBUG("%s: GYRO disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GYRO_UNCALIBRATED:
        case HANDLE_GYRO_UNCALIBRATED_WAKE_UP:
            if (is_gyro_available) {
                HAL_LOG_DEBUG("%s: UNCAL GYRO will be added later with handle:%d",
                               __FUNCTION__, handle);
            } else {
                HAL_LOG_DEBUG("%s: UNCAL GYRO disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MAGNETIC_FIELD:
        case HANDLE_MAGNETIC_FIELD_WAKE_UP:
            if (is_mag_available) {
                HAL_LOG_DEBUG("%s: SMGR MAG enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Magnetic(handle);
            } else {
                HAL_LOG_DEBUG("%s: SMGR MAG disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MAGNETIC_FIELD_UNCALIBRATED:
        case HANDLE_MAGNETIC_FIELD_UNCALIBRATED_WAKE_UP:
            if (mag_cal_src == HAL_MAG_CAL_SRC_SMGR) {
                HAL_LOG_DEBUG("%s: SMGR UNCAL MAG will be added later with handle:%d",
                               __FUNCTION__, handle);
            } else {
                HAL_LOG_DEBUG("%s: SMGR UNCAL MAG disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_LIGHT:
        case HANDLE_LIGHT_WAKE_UP:
            mSensors[handle] = new Light(handle);
            break;
        case HANDLE_PRESSURE:
        case HANDLE_PRESSURE_WAKE_UP:
            mSensors[handle] = new Pressure(handle);
            break;
        case HANDLE_RELATIVE_HUMIDITY:
        case HANDLE_RELATIVE_HUMIDITY_WAKE_UP:
            mSensors[handle] = new Humidity(handle);
            break;
        case HANDLE_AMBIENT_TEMPERATURE:
        case HANDLE_AMBIENT_TEMPERATURE_WAKE_UP:
            mSensors[handle] = new Temperature(handle);
            break;
        case HANDLE_SMGR_STEP_COUNT:
        case HANDLE_SMGR_STEP_COUNT_WAKE_UP:
            mSensors[handle] = new SMGRStepCounter(handle);
            break;
        case HANDLE_SMGR_STEP_DETECTOR:
        case HANDLE_SMGR_STEP_DETECTOR_WAKE_UP:
            mSensors[handle] = new SMGRStepDetector(handle);
            break;
        case HANDLE_SMGR_SMD:
            mSensors[handle] = new SMGRSMD(handle);
            break;
        case HANDLE_SMGR_GAME_RV:
        case HANDLE_SMGR_GAME_RV_WAKE_UP:
            mSensors[handle] = new SMGRGameRV(handle);
            break;
        case HANDLE_RGB:
            mSensors[handle] = new RGB(handle);
            break;
        case HANDLE_IR_GESTURE:
            mSensors[handle] = new IRGesture(handle);
            break;
        case HANDLE_SAR:
            mSensors[handle] = new SAR(handle);
            break;
        case HANDLE_HALL_EFFECT:
            mSensors[handle] = new HallEffect(handle);
            break;
        case HANDLE_UV:
            mSensors[handle] = new UV(handle);
            break;
        case HANDLE_THERMOPILE:
            mSensors[handle] = new Thermopile(handle);
            break;
        case HANDLE_CCT:
            mSensors[handle] = new CCT(handle);
            break;

        /* SAM sensors*/
        case HANDLE_PROXIMITY:
        case HANDLE_PROXIMITY_NON_WAKE_UP:
            property_get( HAL_PROP_PROXIMITY, add_sensor, "true" );
            if (is_prox_available && !strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: Proximity enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Proximity(handle);
            } else {
                HAL_LOG_DEBUG("%s: Proximity disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_THRESH:
        case HANDLE_THRESH_NON_WAKE_UP:
            property_get( HAL_PROP_PROXIMITY, add_sensor, "true" );
            if (is_prox_available && !strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: Proximity through Thresh algo will be enabled \
                               with handle :%d", __FUNCTION__, handle);
            } else {
                HAL_LOG_DEBUG("%s: Proximity through Thresh algo disabled!",
                               __FUNCTION__);
            }
            break;
        case HANDLE_CMC:
            property_get( HAL_PROP_CMC, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: CMC enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new CoarseMotionClassifier(handle);
            } else {
                HAL_LOG_DEBUG("%s: CMC disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GAME_ROTATION_VECTOR:
        case HANDLE_GAME_ROTATION_VECTOR_WAKE_UP:
            property_get( HAL_PROP_GRV, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4) && Utility::isGyroAvailable()) {
                HAL_LOG_DEBUG("%s: GAME RV enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new GameRotationVector(handle);
            } else {
                HAL_LOG_DEBUG("%s: GAME RV disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GEOMAGNETIC_ROTATION_VECTOR:
        case HANDLE_GEOMAGNETIC_ROTATION_VECTOR_WAKE_UP:
            property_get( HAL_PROP_GEOMAGNETIC_RV, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4) && is_mag_available) {
                HAL_LOG_DEBUG("%s: GEOMAGNETIC_RV enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new GeoMagneticRotationVector(handle);
            } else {
                HAL_LOG_DEBUG("%s: GEOMAGNETIC_RV disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GESTURE_BASIC_GESTURES:
            property_get( HAL_PROP_GESTURES, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: GESTURES enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Gestures(handle);
            } else {
                HAL_LOG_DEBUG("%s: GESTURES disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GESTURE_BRING_TO_EAR:
            property_get( HAL_PROP_BTE, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: BTE enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new BringToEar(handle);
            } else {
                HAL_LOG_DEBUG("%s: BTE disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GESTURE_FACING:
            property_get( HAL_PROP_FACING, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: FACING enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Facing(handle);
            } else {
                HAL_LOG_DEBUG("%s: FACING disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GESTURE_TAP:
            property_get( HAL_PROP_TAP, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: TAP enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Tap(handle);
            } else {
                HAL_LOG_DEBUG("%s: TAP disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_GRAVITY:
        case HANDLE_GRAVITY_WAKE_UP:
            property_get( HAL_PROP_GRAVITY, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: Gravity enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Gravity(handle);
            } else {
                HAL_LOG_DEBUG("%s: Gravity disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_LINEAR_ACCEL:
        case HANDLE_LINEAR_ACCEL_WAKE_UP:
            property_get( HAL_PROP_LACCEL, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: Linear Accel enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new LinearAcceleration(handle);
            } else {
                HAL_LOG_DEBUG("%s: Linear Accel disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MOTION_ABSOLUTE:
            property_get( HAL_PROP_AMD, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: AMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new AbsoluteMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: AMD disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MOTION_RELATIVE:
            property_get( HAL_PROP_RMD, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: RMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new RelativeMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: RMD disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MOTION_VEHICLE:
            property_get( HAL_PROP_VMD, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: VMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new VehicleMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: VMD disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_ORIENTATION:
        case HANDLE_ORIENTATION_WAKE_UP:
            property_get( HAL_PROP_ORIENTATION, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4) && is_mag_available) {
                HAL_LOG_DEBUG("%s: Orientation enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Orientation(handle);
            } else {
                HAL_LOG_DEBUG("%s: Orientation disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_PEDOMETER:
            property_get( HAL_PROP_PEDOMETER, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: PEDOMETER enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new Pedometer(handle);
            } else {
                HAL_LOG_DEBUG("%s: PEDOMETER disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_ROTATION_VECTOR:
        case HANDLE_ROTATION_VECTOR_WAKE_UP:
            property_get( HAL_PROP_ROTVEC, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4) && is_mag_available) {
                HAL_LOG_DEBUG("%s: Rotation Vector enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new RotationVector(handle);
            } else {
                HAL_LOG_DEBUG("%s: Rotation Vector disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_SIGNIFICANT_MOTION:
            property_get( HAL_PROP_SMD, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: SMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new SignificantMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: SMD disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_SAM_STEP_COUNTER:
        case HANDLE_SAM_STEP_COUNTER_WAKE_UP:
            property_get( HAL_PROP_STEP_COUNTER, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: STEP COUNTER enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new StepCounter(handle);
            } else {
                HAL_LOG_DEBUG("%s: STEP COUNTER disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_SAM_STEP_DETECTOR:
        case HANDLE_SAM_STEP_DETECTOR_WAKE_UP:
            property_get( HAL_PROP_STEP_DETECTOR, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: STEP DETECTOR enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new StepDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: STEP DETECTOR disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_TILT_DETECTOR:
            property_get( HAL_PROP_TILT_DETECTOR, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: TILT DETECTOR enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new TiltDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: TILT DETECTOR disabled!", __FUNCTION__);
            }
            break;
#ifdef WEAR_SUPPORT
        case HANDLE_WRIST_TILT:
            property_get( HAL_PROP_WRIST_TILT, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: WRIST TILT enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new WristTilt(handle);
            } else {
                HAL_LOG_DEBUG("%s: WRIST TILT disabled!", __FUNCTION__);
            }
            break;
#endif /* WEAR_SUPPORT */
        case HANDLE_PICK_UP_GESTURE:
            property_get( HAL_PROP_PUG, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: Pick-Up-Gesture enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new PickUpGesture(handle);
            } else {
                HAL_LOG_DEBUG("%s: Pick-Up-Gesture disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MAGNETIC_FIELD_SAM:
        case HANDLE_MAGNETIC_FIELD_SAM_WAKE_UP:
            if (Utility::isMagAvailable() && (mag_cal_src == HAL_MAG_CAL_SRC_SAM)) {
                HAL_LOG_DEBUG("%s: SAM MAG will be enabled handle:%d",
                               __FUNCTION__, handle);
                /* The sensor info of the Magnetic is not ready here!
                   Do not add new SAM sensor here. */
            } else {
                HAL_LOG_DEBUG("%s: SAM MAG disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM:
        case HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM_WAKE_UP:
            if (Utility::isMagAvailable() && (mag_cal_src == HAL_MAG_CAL_SRC_SAM)) {
                HAL_LOG_DEBUG("%s: SAM UNCAL MAG enabled handle:%d",
                               __FUNCTION__, handle);
                /* The sensor info of the Magnetic is not ready here!
                   Do not add new the SAM sensor here. */
            } else {
                HAL_LOG_DEBUG("%s: SAM UNCAL MAG disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MOTION_ACCEL:
            property_get( HAL_PROP_SCRN_ORTN, add_sensor, "true" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: LP2/MA enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new LowPowerLandscapePortrait(handle);
            } else {
                HAL_LOG_DEBUG("%s: LP2/MA disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_QHEART:
        case HANDLE_QHEART_WAKE_UP:
            property_get( HAL_PROP_QHEART, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: QHEART enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new QHeart(handle);
            } else {
                HAL_LOG_DEBUG("%s: QHEART disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_FAST_AMD:
            property_get( HAL_PROP_FAST_AMD, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: FAST AMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new FastAbsoluteMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: FAST AMD disabled!", __FUNCTION__);
            }
            break;
#ifndef WEAR_SUPPORT
#ifdef FEATURE_SNS_IOD
        case HANDLE_IOD:
            property_get( HAL_PROP_IOD, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: IOD enabled handle:%d", __FUNCTION__, handle);
                mSensors[handle] = new IOD(handle);
            } else {
                HAL_LOG_DEBUG("%s: IOD disabled!", __FUNCTION__);
            }
            break;
#endif /* FEATURE_SNS_IOD */
#endif /* WEAR_SUPPORT */
        case HANDLE_DPC:
            property_get( HAL_PROP_DPC, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: DPC enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new DevicePositionClassifier(handle);
            } else {
                HAL_LOG_DEBUG("%s: DPC disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MULTISHAKE:
            property_get( HAL_PROP_MULTISHAKE, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: MULTISHAKE enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new MultiShake(handle);
            } else {
                HAL_LOG_DEBUG("%s: MULTISHAKE disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_DEVICE_ORIENTATION:
        case HANDLE_DEVICE_ORIENTATION_WAKE_UP:
            property_get( HAL_PROP_DEV_ORI, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: DEV_ORI enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new DeviceOrientation(handle);
            } else {
                HAL_LOG_DEBUG("%s: DEV_ORI disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_PMD:
            property_get( HAL_PROP_PMD, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: PMD enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new PersistentMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: PMD disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_STATIONARY_DETECT:
            property_get( HAL_PROP_STA_DETECT, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: STATIONARY_DETECT enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new AndroidStationaryDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: STATIONARY_DETECT disabled!", __FUNCTION__);
            }
            break;
        case HANDLE_MOTION_DETECT:
            property_get( HAL_PROP_MOT_DETECT, add_sensor, "false" );
            if (!strncmp("true", add_sensor, 4)) {
                HAL_LOG_DEBUG("%s: MOTION_DETECT enabled handle:%d",
                               __FUNCTION__, handle);
                mSensors[handle] = new AndroidMotionDetector(handle);
            } else {
                HAL_LOG_DEBUG("%s: MOTION_DETECT disabled!", __FUNCTION__);
            }
            break;
        default:
            HAL_LOG_WARN("%s : Not supported sensor with handle %d!", __FUNCTION__, handle);
            break;
    }
}

/*===========================================================================
  FUNCTION:  sendSMGRVersionReq
    Get the SMGR version information by sending the sensor1 message.
===========================================================================*/
int SensorsContext::sendSMGRVersionReq()
{
    sensor1_error_e       error;
    sensor1_msg_header_s  msg_hdr;
    sns_common_version_req_msg_v01 *smgr_version_req;

    HAL_LOG_INFO("%s", __FUNCTION__);
    pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
    /* Send SMGR version req */
    msg_hdr.service_number = SNS_SMGR_SVC_ID_V01;
    msg_hdr.msg_id = SNS_SMGR_VERSION_REQ_V01;
    msg_hdr.msg_size = sizeof(sns_common_version_req_msg_v01);
    msg_hdr.txn_id = 0;
    error = sensor1_alloc_msg_buf(sensor_info_sensor1_cb->sensor1_handle,
                    sizeof(sns_common_version_req_msg_v01),
                    (void**)&smgr_version_req);
    if (SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR("%s: msg alloc failed: %d", __FUNCTION__, error );
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    }

    sensor_info_sensor1_cb->error = false;
    if ((error = sensor1_write(sensor_info_sensor1_cb->sensor1_handle,
                &msg_hdr, smgr_version_req)) != SENSOR1_SUCCESS) {
        /* free the message buffer */
        sensor1_free_msg_buf(sensor_info_sensor1_cb->sensor1_handle, smgr_version_req );
        HAL_LOG_ERROR("%s: Error in sensor1_write() %s", __FUNCTION__, strerror(errno));
        sensor_info_sensor1_cb->error = true;
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    }

    /* waiting for response */
    if (Utility::waitForResponse(TIME_OUT_MS,
                &sensor_info_sensor1_cb->cb_mutex,
                &sensor_info_sensor1_cb->cb_cond,
                &sensor_info_sensor1_cb->is_resp_arrived) == false) {
        HAL_LOG_ERROR("%s: SMGR version request timed out", __FUNCTION__);
        sensor_info_sensor1_cb->error = true;
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    } else {
        HAL_LOG_VERBOSE("%s: Received SMGR version response", __FUNCTION__);
    }
    pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
    return 0;
}

/*===========================================================================
  FUNCTION:  getSensorList
    Get the SMGR sensor information by sending the sensor1 message.
===========================================================================*/
int SensorsContext::getSensorList()
{
    sensor1_error_e       error;
    sensor1_msg_header_s  msg_hdr;
    sns_smgr_all_sensor_info_req_msg_v01 *smgr_req;
    sns_smgr_sensor_datatype_info_s_v01* sensor_datatype = NULL;

    HAL_LOG_INFO("%s", __FUNCTION__);
    pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
    msg_hdr.service_number = SNS_SMGR_SVC_ID_V01;
    msg_hdr.msg_id = SNS_SMGR_ALL_SENSOR_INFO_REQ_V01;
    msg_hdr.msg_size = sizeof( sns_smgr_all_sensor_info_req_msg_v01 );
    msg_hdr.txn_id = 0;
    error = sensor1_alloc_msg_buf(sensor_info_sensor1_cb->sensor1_handle,
                    sizeof(sns_smgr_all_sensor_info_req_msg_v01),
                    (void**)&smgr_req);
    if (SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR("%s: msg alloc failed: %d", __FUNCTION__, error );
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    }

    sensor_info_sensor1_cb->error = false;
    if ((error = sensor1_write(sensor_info_sensor1_cb->sensor1_handle,
                &msg_hdr, smgr_req)) != SENSOR1_SUCCESS) {
        sensor1_free_msg_buf(sensor_info_sensor1_cb->sensor1_handle, smgr_req );
        HAL_LOG_ERROR("%s: Error in sensor1_write() %s", __FUNCTION__, strerror(errno));
        sensor_info_sensor1_cb->error = true;
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    }

    /* waiting for response */
    if (Utility::waitForResponse(TIME_OUT_MS,
                &sensor_info_sensor1_cb->cb_mutex,
                &sensor_info_sensor1_cb->cb_cond,
                &sensor_info_sensor1_cb->is_resp_arrived) == false) {
        HAL_LOG_ERROR("%s: Request timed out", __FUNCTION__);
        sensor_info_sensor1_cb->error = true;
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        return -1;
    } else {
        HAL_LOG_VERBOSE("%s: Received Response", __FUNCTION__);
    }
    pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
    return 0;
}

/*===========================================================================
  FUNCTION:  updateSensorList
    Update sensor list with Uncal sensors and update sensor info
===========================================================================*/
int SensorsContext::updateSensorList()
{
    sns_smgr_sensor_datatype_info_s_v01* sensor_datatype = NULL;
    char wu_sensor[PROPERTY_VALUE_MAX] = "false";
    property_get( HAL_PROP_WU_SENSORS, wu_sensor, "true" );

    HAL_LOG_INFO("%s", __FUNCTION__);
    pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
    if (Utility::isMagAvailable()) {
        if (mag_cal_src == HAL_MAG_CAL_SRC_SMGR) {
            /* Add & update SMGR uncal mag and it's wake-up version */
            mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED] = new MagneticUncalibrated(HANDLE_MAGNETIC_FIELD_UNCALIBRATED);
            if(mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED] != NULL) {
                (static_cast<SMGRSensor*>(mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED]))->setSensorInfo(sensor_datatype);
            }
            if (!strncmp("true", wu_sensor, 4)) {
                mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED_WAKE_UP] = new MagneticUncalibrated(HANDLE_MAGNETIC_FIELD_UNCALIBRATED_WAKE_UP);
                if(mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED_WAKE_UP] != NULL) {
                    (static_cast<SMGRSensor*>(mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED_WAKE_UP]))->setSensorInfo(sensor_datatype);
                }
            }
        }
        else if (mag_cal_src == HAL_MAG_CAL_SRC_SAM) {
            /* Add & update mag cal (SAM) and it's wake-up version */
            mSensors[HANDLE_MAGNETIC_FIELD_SAM] = new MagneticCalibration(HANDLE_MAGNETIC_FIELD_SAM);
            mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM] = new MagneticUncalibratedSAM(HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM);
            /* Disable the HANDLE_MAGNETIC_FIELD after getting the sensor info */
            mSensors[HANDLE_MAGNETIC_FIELD]->setAttribOK(false);
            if (!strncmp("true", wu_sensor, 4)) {
                mSensors[HANDLE_MAGNETIC_FIELD_SAM_WAKE_UP] = new MagneticCalibration(HANDLE_MAGNETIC_FIELD_SAM_WAKE_UP);
                mSensors[HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM_WAKE_UP] = new MagneticUncalibratedSAM(HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM_WAKE_UP);
                mSensors[HANDLE_MAGNETIC_FIELD_WAKE_UP]->setAttribOK(false);
            }
        }
    }

    if (Utility::isGyroAvailable()) {
        /* Add & update uncal gyro and it's wake-up version */
        mSensors[HANDLE_GYRO_UNCALIBRATED] = new GyroscopeUncalibrated(HANDLE_GYRO_UNCALIBRATED);
        if(mSensors[HANDLE_GYRO_UNCALIBRATED] != NULL) {
            (static_cast<SMGRSensor*>(mSensors[HANDLE_GYRO_UNCALIBRATED]))->setSensorInfo(sensor_datatype);
        }
        if (!strncmp("true", wu_sensor, 4)) {
            mSensors[HANDLE_GYRO_UNCALIBRATED_WAKE_UP] = new GyroscopeUncalibrated(HANDLE_GYRO_UNCALIBRATED_WAKE_UP);
            if(mSensors[HANDLE_GYRO_UNCALIBRATED_WAKE_UP] != NULL) {
                (static_cast<SMGRSensor*>(mSensors[HANDLE_GYRO_UNCALIBRATED_WAKE_UP]))->setSensorInfo(sensor_datatype);
            }
        }
    }

    if (mSensors[HANDLE_PROXIMITY]) {
        mSensors[HANDLE_THRESH] = new Thresh(HANDLE_THRESH);
        /* disable proximity sensor*/
        if (mSensors[HANDLE_PROXIMITY]) {
            mSensors[HANDLE_PROXIMITY]->setAttribOK(false);
        }

        HAL_LOG_DEBUG("%s: Proximity through Thresh Algo handle:%d",
                       __FUNCTION__, HANDLE_THRESH);

        if (!strncmp("true", wu_sensor, 4)) {
            mSensors[HANDLE_THRESH_NON_WAKE_UP] = new Thresh(HANDLE_THRESH_NON_WAKE_UP);
            /* disable proximity sensor*/
            if (mSensors[HANDLE_PROXIMITY_NON_WAKE_UP])
            mSensors[HANDLE_PROXIMITY_NON_WAKE_UP]->setAttribOK(false);

            HAL_LOG_DEBUG("%s: Proximity through Thresh Algo handle:%d",
                           __FUNCTION__, HANDLE_THRESH_NON_WAKE_UP);
        }
    }
    else {
        HAL_LOG_DEBUG("%s: Proximity disabled!", __FUNCTION__);
    }

    /* Few attributes of Linear Aceel & Gravity are derived from Accel.
     * This will make sure attributes are set properly for Linear Accel & Gravity */
    if(mSensors[HANDLE_GRAVITY] != NULL) {
        (static_cast<SAMSensor*>(mSensors[HANDLE_GRAVITY]))->setSensorInfo();
    }
    if(mSensors[HANDLE_LINEAR_ACCEL] != NULL) {
        (static_cast<SAMSensor*>(mSensors[HANDLE_LINEAR_ACCEL]))->setSensorInfo();
    }
    if(mSensors[HANDLE_SAM_STEP_COUNTER] != NULL) {
        (static_cast<SAMSensor*>(mSensors[HANDLE_SAM_STEP_COUNTER]))->setSensorInfo();
    }
    if (!strncmp("true", wu_sensor, 4)) {
        if(mSensors[HANDLE_GRAVITY_WAKE_UP] != NULL) {
            (static_cast<SAMSensor*>(mSensors[HANDLE_GRAVITY_WAKE_UP]))->setSensorInfo();
        }
        if(mSensors[HANDLE_LINEAR_ACCEL_WAKE_UP] != NULL) {
            (static_cast<SAMSensor*>(mSensors[HANDLE_LINEAR_ACCEL_WAKE_UP]))->setSensorInfo();
        }
        if(mSensors[HANDLE_SAM_STEP_COUNTER_WAKE_UP] != NULL) {
            (static_cast<SAMSensor*>(mSensors[HANDLE_SAM_STEP_COUNTER_WAKE_UP]))->setSensorInfo();
        }
    }
    pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
    return 0;
}

/*===========================================================================
  FUNCTION:  checkMagCalSource
    Check the NV3801 for SAM or SMGR Magnetic calibration.
===========================================================================*/
void SensorsContext::checkMagCalSource()
{
    int err;
    err = sensor_reg_open();
    if (err == SENSOR_REG_SUCCESS) {
        uint8 reg_len = 0;
        uint8 *preg_val = NULL;
        int reg_error;
        reg_error = sensor_reg_read(SNS_REG_ITEM_QMAG_CAL_ENABLE_ALGO_V02, 1, &preg_val, &reg_len);
        if (reg_error == SENSOR_REG_SUCCESS && preg_val != NULL) {
            if(*preg_val) {
                /* Qmag_cal is enabled in registry MV3801 == 1 */
                mag_cal_src = HAL_MAG_CAL_SRC_SMGR;
            }
            else {
                /* Qmag_cal is not enabled in registry NV3801 == 0*/
                mag_cal_src = HAL_MAG_CAL_SRC_SAM;
            }
        }
        else {
            HAL_LOG_ERROR("%s: Error in sensor_reg_read(): %i",
                __FUNCTION__, reg_error );
        }
        free(preg_val);
        sensor_reg_close();
    }
    else {
        HAL_LOG_ERROR("%s: Error in sensor_reg_open(): %i",
            __FUNCTION__, err);
    }
    HAL_LOG_INFO("%s: SSI support for mag cal %d", __FUNCTION__, mag_cal_src);
}

/*===========================================================================
  FUNCTION:  processResp
    Process the response to the sensor1 SENSOR1_MSG_TYPE_RESP
    Parameters
    @msg_hdr : sensor1 message header
    @msg_ptr : sensor1 message data
===========================================================================*/
void SensorsContext::processResp(Sensor** mSensors, sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    HAL_LOG_DEBUG("%s: %d", __FUNCTION__,  msg_hdr->msg_id);
    switch(msg_hdr->msg_id) {
    case SNS_SMGR_VERSION_RESP_V01: {
        pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
        sns_common_version_resp_msg_v01* respMsgPtr = (sns_common_version_resp_msg_v01 *)msg_ptr;
        if (respMsgPtr->resp.sns_result_t == 0) {
            smgr_version = respMsgPtr->interface_version_number;
            HAL_LOG_DEBUG("%s: SMGR version=%d", __FUNCTION__, smgr_version);
            Utility::signalResponse(false, sensor_info_sensor1_cb);
        }
        else {
            HAL_LOG_ERROR("%s: Error in getting SMGR version!", __FUNCTION__);
            Utility::signalResponse(true, sensor_info_sensor1_cb);
        }
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        } break;
    case SNS_SMGR_ALL_SENSOR_INFO_RESP_V01: {
        pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
        processAllSensorInfoResp(
                    (sns_smgr_all_sensor_info_resp_msg_v01*)msg_ptr);
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        } break;
    case SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01: {
        pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
        processSingleSensorInfoResp(mSensors, msg_hdr->txn_id,
                    (sns_smgr_single_sensor_info_resp_msg_v01*)msg_ptr);
        pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        } break;
    }
    return;
}

/*===========================================================================
  FUNCTION:  processAllSensorInfoResp
    Process the response to the sensor1 SNS_SMGR_ALL_SENSOR_INFO_RESP_V01
    Parameters
    @smgr_info_resp : sensor1 sensor info response message
===========================================================================*/
void SensorsContext::processAllSensorInfoResp(sns_smgr_all_sensor_info_resp_msg_v01
                                                                *smgr_info_resp)
{
    uint32_t i;
    HAL_LOG_DEBUG("%s: SensorInfo_len: %d", __FUNCTION__,
                                            smgr_info_resp->SensorInfo_len);

    if (smgr_info_resp->Resp.sns_result_t != 0) {
        HAL_LOG_ERROR("%s: R: %u, E: %u", __FUNCTION__,
                            smgr_info_resp->Resp.sns_result_t,
                            smgr_info_resp->Resp.sns_err_t );
        Utility::signalResponse(true, sensor_info_sensor1_cb);
        return;
    }

    if (smgr_info_resp->SensorInfo_len > 0) {
        num_smgr_sensors = smgr_info_resp->SensorInfo_len;
        for (i = 0; i < num_smgr_sensors; i++) {
            singleSensorInfoRequest(i, smgr_info_resp->SensorInfo[i].SensorID);
        }
    } else {
        Utility::signalResponse(false, sensor_info_sensor1_cb);
    }
    return;
}

/*===========================================================================
  FUNCTION:  singleSensorInfoRequest
    Request to get the single sensor info
    Parameters
    @req_id : sensor1 transaction ID
    @sensor_id : the sensor ID
===========================================================================*/
void SensorsContext::singleSensorInfoRequest(int req_id, uint8_t sensor_id)
{
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_smgr_single_sensor_info_req_msg_v01* smgr_req;

    HAL_LOG_DEBUG("%s: %d", __FUNCTION__, sensor_id);

    req_hdr.service_number = SNS_SMGR_SVC_ID_V01;
    req_hdr.msg_id = SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01;
    req_hdr.msg_size = sizeof(sns_smgr_single_sensor_info_req_msg_v01);
    req_hdr.txn_id = req_id;

    error = sensor1_alloc_msg_buf(sensor_info_sensor1_cb->sensor1_handle,
                sizeof(sns_smgr_single_sensor_info_req_msg_v01),
                (void**)&smgr_req);
    if(SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR("%s: msg alloc failed: %d", __FUNCTION__, error );
        return;
    }

    smgr_req->SensorID = sensor_id;
    HAL_LOG_DEBUG("%s: txn_id: %d sensor_id: %u", __FUNCTION__,
            req_id, smgr_req->SensorID );

    if((error = sensor1_write(sensor_info_sensor1_cb->sensor1_handle, &req_hdr,
                    smgr_req)) != SENSOR1_SUCCESS) {
        /* free the message buffer */
        sensor1_free_msg_buf(sensor_info_sensor1_cb->sensor1_handle, smgr_req );
        HAL_LOG_ERROR("%s: Error in sensor1_write() %s", __FUNCTION__, strerror(errno));
        sensor_info_sensor1_cb->error = true;
    }
    return;
}

/*===========================================================================
  FUNCTION:  processSingleSensorInfoResp
    Process the response to the sensor1 SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01
    Parameters
    @msensors : the pointer to the sensor class
    @txn_id : sensor1 transaction ID
    @smgr_resp : the sensor1 response sensor info message
===========================================================================*/
void SensorsContext::processSingleSensorInfoResp(Sensor** mSensors, uint8_t txn_id,
            sns_smgr_single_sensor_info_resp_msg_v01* smgr_resp)
{
    int i;
    int handle = -1, handle_wakeup = -1;
    bool error = false;
    sns_smgr_sensor_datatype_info_s_v01* sensor_datatype;
    char max_rate_prop_value[PROPERTY_VALUE_MAX];
    char *strtol_endptr;
    int tmp_max_rate;
    char wu_sensor[PROPERTY_VALUE_MAX] = "false";
    property_get( HAL_PROP_WU_SENSORS, wu_sensor, "true" );
    /* keep tracking of sensor responses */
    sensor_responses++;
    HAL_LOG_DEBUG("%s: data_type_info_len: %d", __FUNCTION__,
            smgr_resp->SensorInfo.data_type_info_len);
    if (smgr_resp->Resp.sns_result_t != 0) {
        HAL_LOG_ERROR("%s: Error: %u ", __FUNCTION__,
            smgr_resp->Resp.sns_result_t);
        Utility::signalResponse(true, sensor_info_sensor1_cb);
        return;
    }
    for (i = 0; i < (int)smgr_resp->SensorInfo.data_type_info_len; i++) {
        handle = -1, handle_wakeup = -1, error = false;
        sensor_datatype = &smgr_resp->SensorInfo.data_type_info[i];

        HAL_LOG_DEBUG("%s: txn: %u, ns: %u", __FUNCTION__, txn_id,
            (unsigned int)num_smgr_sensors);

        switch (sensor_datatype->SensorID) {
        case SNS_SMGR_ID_ACCEL_V01:
            if(SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_ACCELERATION;
                handle_wakeup = HANDLE_ACCELERATION_WAKE_UP;
                is_accel_available = true;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_MAG_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_MAGNETIC_FIELD;
                handle_wakeup = HANDLE_MAGNETIC_FIELD_WAKE_UP;
                    is_mag_available = true;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_PROX_LIGHT_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_PROXIMITY;
                handle_wakeup = HANDLE_PROXIMITY_NON_WAKE_UP;
                is_prox_available = true;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_LIGHT;
                handle_wakeup = HANDLE_LIGHT_WAKE_UP;
            }
            break;
        case SNS_SMGR_ID_GYRO_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_GYRO;
                handle_wakeup = HANDLE_GYRO_WAKE_UP;
                    is_gyro_available = true;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_PRESSURE_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_PRESSURE;
                handle_wakeup = HANDLE_PRESSURE_WAKE_UP;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_HUMIDITY_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_RELATIVE_HUMIDITY;
                handle_wakeup = HANDLE_RELATIVE_HUMIDITY_WAKE_UP;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_AMBIENT_TEMPERATURE;
                handle_wakeup = HANDLE_AMBIENT_TEMPERATURE_WAKE_UP;
            }
            break;
        case SNS_SMGR_ID_STEP_EVENT_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_SMGR_STEP_DETECTOR;
                handle_wakeup = HANDLE_SMGR_STEP_DETECTOR_WAKE_UP;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_SMD_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_SMGR_SMD;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_STEP_COUNT_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_SMGR_STEP_COUNT;
                handle_wakeup = HANDLE_SMGR_STEP_COUNT_WAKE_UP;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_SMGR_GAME_RV;
                handle_wakeup = HANDLE_SMGR_GAME_RV_WAKE_UP;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_RGB_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_RGB;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_CCT;
            }
            break;
        case SNS_SMGR_ID_IR_GESTURE_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_IR_GESTURE;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_SAR_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_SAR;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_HALL_EFFECT_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_HALL_EFFECT;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_ULTRA_VIOLET_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_UV;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        case SNS_SMGR_ID_OBJECT_TEMP_V01:
            if (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == sensor_datatype->DataType) {
                handle = HANDLE_THERMOPILE;
            }
            else if (SNS_SMGR_DATA_TYPE_SECONDARY_V01 == sensor_datatype->DataType) {
                /* Skip secondary data and replace with proper handle if needed */
                error = true;
            }
            break;
        default:
            HAL_LOG_ERROR(" %s Unknown sensor type: %d", __FUNCTION__,
                                                    sensor_datatype->DataType);
            error = true;
            break;
        }

        if (handle != -1 && error == false) {
            addSensor(handle);

            if (mSensors[handle] != NULL) {
                /* Fill each sensor info in sub sensor classes */
                strlcpy(mSensors[handle]->getName(), sensor_datatype->SensorName,
                    SNS_MAX_SENSOR_NAME_SIZE);
                strlcpy(mSensors[handle]->getVendor(), sensor_datatype->VendorName,
                    SNS_MAX_VENDOR_NAME_SIZE);
                mSensors[handle]->setVersion(sensor_datatype->Version);
                /*<= 1000 is in Hz, and > 1000 is in ms*/
                if (sensor_datatype->MaxSampleRate <= 1000 ) {
                    mSensors[handle]->setMaxFreq(sensor_datatype->MaxSampleRate);
                } else {
                    mSensors[handle]->setMaxFreq(1000.0 / sensor_datatype->MaxSampleRate);
                }

                /* Lower max rate if property overrides value returned from sensor */
                switch (mSensors[handle]->getType()) {
                    case SENSOR_TYPE_ACCELEROMETER:
                        property_get(HAL_PROP_MAX_ACCEL, max_rate_prop_value,
                            FREQ_ACCEL_DEF_HZ_STR);
                        break;
                    case SENSOR_TYPE_MAGNETIC_FIELD:
                    case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                        property_get(HAL_PROP_MAX_MAG, max_rate_prop_value,
                            FREQ_MAG_DEF_HZ_STR);
                        break;
                    case SENSOR_TYPE_GYROSCOPE:
                        property_get(HAL_PROP_MAX_GYRO, max_rate_prop_value,
                            FREQ_GYRO_DEF_HZ_STR);
                        break;
                    default:
                        max_rate_prop_value[0] = 0;
                        break;
                }
                errno = 0;
                tmp_max_rate = strtol(max_rate_prop_value, &strtol_endptr, 0);
                if (0 == errno && strtol_endptr != max_rate_prop_value) {
                    mSensors[handle]->setMaxFreq(MIN(mSensors[handle]->getMaxFreq(), tmp_max_rate));
                }
                if (mSensors[handle]->getMaxFreq() >= FREQ_FASTEST_HZ) {
                    mSensors[handle]->setMaxFreq(FREQ_FASTEST_HZ);
                }
                /* check SMGR version and set min freq accordingly to support subHz */
                if (smgr_version >= SMGR_SUB_HZ_VERSION) {
                    mSensors[handle]->setMinFreq(SMGR_SUB_HZ_REPORT_RATE_MIN_HZ);
                }
                else {
                    mSensors[handle]->setMinFreq(SNS_SMGR_REPORT_RATE_MIN_HZ_V01);
                }
                mSensors[handle]->setPower((float)((float)sensor_datatype->MaxPower * UNIT_CONVERT_POWER));
                mSensors[handle]->setAttribOK(true);

                if (!smgr_resp->num_buffered_reports_valid ||
                    HANDLE_PROXIMITY == mSensors[handle]->getHandle()) {
                    mSensors[handle]->setMaxBufferedSamples(0);
                }
                else {
                    mSensors[handle]->setMaxBufferedSamples(smgr_resp->num_buffered_reports[i]);
                }

                /* check SMGR version and set Reserved FIFO accordingly */
                if (smgr_version >= SMGR_RES_FIFO_VERSION &&
                    smgr_resp->num_reserved_buffered_reports_valid) {
                    mSensors[handle]->setResBufferedSamples(smgr_resp->num_reserved_buffered_reports[i]);
                }
                else {
                    mSensors[handle]->setResBufferedSamples(0);
                }

                (static_cast<SMGRSensor*>(mSensors[handle]))->setSensorInfo(sensor_datatype);

                HAL_LOG_INFO("%s: sensor1: name: %s, vendor: %s, maxRange: %u, res: %u, \
                    power: %u, max_freq: %u max_buffered_samples: %u", __FUNCTION__,
                    sensor_datatype->SensorName, sensor_datatype->VendorName,
                    sensor_datatype->MaxRange, sensor_datatype->Resolution,
                    sensor_datatype->MaxPower, sensor_datatype->MaxSampleRate,
                    mSensors[handle]->getMaxBufferedSamples());
                HAL_LOG_DEBUG("%s: HAL: name: %s, vendor: %s, maxRange: %f, res: %f, \
                    power: %f, max_freq: %f min_freq: %f", __FUNCTION__,
                    mSensors[handle]->getName(), mSensors[handle]->getVendor(), mSensors[handle]->getMaxRange(),
                    mSensors[handle]->getResolution(), mSensors[handle]->getPower(), mSensors[handle]->getMaxFreq(),
                    mSensors[handle]->getMinFreq());
            }
        }
        else {
            HAL_LOG_ERROR("%s: either handle is -1 or error is true or mSensors[handle] is NULL!", __FUNCTION__);
        }

        if (handle_wakeup != -1 && error == false && !strncmp("true", wu_sensor, 4))
        {
            addSensor(handle_wakeup);

            if (mSensors[handle_wakeup] != NULL) {
                /* Fill each sensor info in sub sensor classes */
                strlcpy(mSensors[handle_wakeup]->getName(), sensor_datatype->SensorName,
                    SNS_MAX_SENSOR_NAME_SIZE);
                strlcpy(mSensors[handle_wakeup]->getVendor(), sensor_datatype->VendorName,
                    SNS_MAX_VENDOR_NAME_SIZE);
                mSensors[handle_wakeup]->setVersion(sensor_datatype->Version);
                /*<= 1000 is in Hz, and > 1000 is in ms*/
                if (sensor_datatype->MaxSampleRate <= 1000 ) {
                    mSensors[handle_wakeup]->setMaxFreq(sensor_datatype->MaxSampleRate);
                } else {
                    mSensors[handle_wakeup]->setMaxFreq(1000.0 / sensor_datatype->MaxSampleRate);
                }

                /* Lower max rate if property overrides value returned from sensor */
                switch (mSensors[handle_wakeup]->getType()) {
                    case SENSOR_TYPE_ACCELEROMETER:
                        property_get(HAL_PROP_MAX_ACCEL, max_rate_prop_value,
                            FREQ_ACCEL_DEF_HZ_STR);
                        break;
                    case SENSOR_TYPE_MAGNETIC_FIELD:
                    case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                        property_get(HAL_PROP_MAX_MAG, max_rate_prop_value,
                            FREQ_MAG_DEF_HZ_STR);
                        break;
                    case SENSOR_TYPE_GYROSCOPE:
                        property_get(HAL_PROP_MAX_GYRO, max_rate_prop_value,
                            FREQ_GYRO_DEF_HZ_STR);
                        break;
                    default:
                        max_rate_prop_value[0] = 0;
                        break;
                }
                errno = 0;
                tmp_max_rate = strtol(max_rate_prop_value, &strtol_endptr, 0);
                if (0 == errno && strtol_endptr != max_rate_prop_value) {
                    mSensors[handle_wakeup]->setMaxFreq(MIN(mSensors[handle_wakeup]->getMaxFreq(), tmp_max_rate));
                }
                if (mSensors[handle_wakeup]->getMaxFreq() >= FREQ_FASTEST_HZ) {
                    mSensors[handle_wakeup]->setMaxFreq(FREQ_FASTEST_HZ);
                }
                /* check SMGR version and set min freq accordingly to support subHz */
                if (smgr_version >= SMGR_SUB_HZ_VERSION) {
                    mSensors[handle_wakeup]->setMinFreq(SMGR_SUB_HZ_REPORT_RATE_MIN_HZ);
                }
                else {
                    mSensors[handle_wakeup]->setMinFreq(SNS_SMGR_REPORT_RATE_MIN_HZ_V01);
                }
                mSensors[handle_wakeup]->setPower((float)((float)sensor_datatype->MaxPower * UNIT_CONVERT_POWER));
                mSensors[handle_wakeup]->setAttribOK(true);

                if (!smgr_resp->num_buffered_reports_valid ||
                    HANDLE_PROXIMITY_NON_WAKE_UP == mSensors[handle_wakeup]->getHandle()) {
                    mSensors[handle_wakeup]->setMaxBufferedSamples(0);
                }
                else {
                    mSensors[handle_wakeup]->setMaxBufferedSamples(smgr_resp->num_buffered_reports[i]);
                }

                (static_cast<SMGRSensor*>(mSensors[handle_wakeup]))->setSensorInfo(sensor_datatype);

                HAL_LOG_INFO("%s: sensor1: name: %s, vendor: %s, maxRange: %u, res: %u, \
                    power: %u, max_freq: %u max_buffered_samples: %u", __FUNCTION__,
                    sensor_datatype->SensorName, sensor_datatype->VendorName,
                    sensor_datatype->MaxRange, sensor_datatype->Resolution,
                    sensor_datatype->MaxPower, sensor_datatype->MaxSampleRate,
                    mSensors[handle_wakeup]->getMaxBufferedSamples());
                HAL_LOG_DEBUG("%s: HAL: name: %s, vendor: %s, maxRange: %f, res: %f, \
                    power: %f, max_freq: %f", __FUNCTION__,
                    mSensors[handle_wakeup]->getName(), mSensors[handle_wakeup]->getVendor(), mSensors[handle_wakeup]->getMaxRange(),
                    mSensors[handle_wakeup]->getResolution(), mSensors[handle_wakeup]->getPower(), mSensors[handle_wakeup]->getMaxFreq());
            }
        }
        else {
            HAL_LOG_ERROR("%s: either handle_wakeup is -1 or error is true or mSensors[handle_wakeup] is NULL!", __FUNCTION__);
        }
    }

    HAL_LOG_DEBUG("%s: txn: %u, ns: %u, ss_resp: %u", __FUNCTION__, txn_id,
                            (unsigned int)num_smgr_sensors, sensor_responses);

    if (sensor_responses == num_smgr_sensors) {
        /* All responses received */
        Utility::signalResponse(false, sensor_info_sensor1_cb);
    }
    return;
}

/*===========================================================================

  FUNCTION:  context_sensor1_cb
    Handle the sensor1 callback for get SMGR sensors list.
    Parameters
    @cb_data : pointer of the callback data, SensorsContext is passed in
            this function
    @msg_hdr : sensor1 message header
    @msg_type : sensor1 message type, two major types are listed in the below:
            SENSOR1_MSG_TYPE_RESP
            SENSOR1_MSG_TYPE_IND
    @msg_ptr : sensor1 message pointer, do free this memory before return

===========================================================================*/
void context_sensor1_cb (intptr_t cb_data,
                     sensor1_msg_header_s *msg_hdr,
                     sensor1_msg_type_e msg_type,
                     void *msg_ptr)
{
    SensorsContext *context = (SensorsContext *)cb_data;
    Sensor **mSensors = context->getSensors();
    hal_sensor1_cb_t *sensor_info_sensor1_cb = context->getSensor1Cb();

    if (msg_hdr != NULL) {
        HAL_LOG_VERBOSE("%s: msg_type %d, Sn %d, msg Id %d, txn Id %d", __FUNCTION__,
            msg_type, msg_hdr->service_number, msg_hdr->msg_id, msg_hdr->txn_id );
    }
    else {
        if (msg_type != SENSOR1_MSG_TYPE_BROKEN_PIPE &&
            msg_type != SENSOR1_MSG_TYPE_REQ &&
            msg_type != SENSOR1_MSG_TYPE_RETRY_OPEN ) {
            HAL_LOG_ERROR("%s: Error - invalid msg type with NULL msg_hdr: %u",
                __FUNCTION__, msg_type);
            return;
        }
        else {
            HAL_LOG_VERBOSE("%s: msg_type %d", __FUNCTION__, msg_type);
        }
    }

    switch(msg_type) {
    case SENSOR1_MSG_TYPE_RESP_INT_ERR:
        if (msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
            pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
            Utility::signalResponse(true, sensor_info_sensor1_cb);
            pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
        }
        break;
    case SENSOR1_MSG_TYPE_RESP:
        if (msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
            context->processResp(mSensors, msg_hdr, msg_ptr);
        }
        break;
    case SENSOR1_MSG_TYPE_REQ:
    default:
        HAL_LOG_ERROR("%s: Error - invalid msg type in cb: %u", __FUNCTION__, msg_type);
        break;
    }

    pthread_mutex_lock(&sensor_info_sensor1_cb->cb_mutex);
    if (NULL != msg_ptr && sensor_info_sensor1_cb->sensor1_handle) {
        sensor1_free_msg_buf(sensor_info_sensor1_cb->sensor1_handle, msg_ptr);
    }
    pthread_mutex_unlock(&sensor_info_sensor1_cb->cb_mutex);
    return;
}
