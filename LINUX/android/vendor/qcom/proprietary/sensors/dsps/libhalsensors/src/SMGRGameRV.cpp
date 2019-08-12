/*============================================================================
  @file SMGRGameRV.cpp

  @brief
  SMGRGameRV class implementation.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include "SMGRGameRV.h"

/*============================================================================
  SMGRGameRV Constructor
============================================================================*/
SMGRGameRV::SMGRGameRV(int handle)
    :SMGRSensor(handle)
{
    trigger_mode = SENSOR_MODE_CONT;
    (handle == HANDLE_SMGR_GAME_RV_WAKE_UP)?(bWakeUp = true):(bWakeUp = false);
}

/*============================================================================
  SMGRGameRV Destructor
============================================================================*/
SMGRGameRV::~SMGRGameRV()
{

}
/*===========================================================================
  FUNCTION:  setSensorInfo
    Fill the sensor information from the sns_smgr_sensor_datatype_info_s_v01 type
    Parameters
    @datatype : sensor information got from the sensor1 callback
    @info : sensor information to be reported to the framework
===========================================================================*/
void SMGRGameRV::setSensorInfo(sns_smgr_sensor_datatype_info_s_v01* sensor_datatype)
{
    HAL_LOG_DEBUG("%s: Game RV DTy: %d", __FUNCTION__, sensor_datatype->DataType);
    setType(SENSOR_TYPE_GAME_ROTATION_VECTOR);

    if(bWakeUp == false) {
        setFlags(SENSOR_FLAG_CONTINUOUS_MODE);
    } else {
        setFlags(SENSOR_FLAG_CONTINUOUS_MODE|SENSOR_FLAG_WAKE_UP);
        strlcat(name, " -Wakeup", sizeof(name));
    }

    setResolution((float)((float)sensor_datatype->Resolution * UNIT_CONVERT_Q16));
    setMaxRange((float)((float)sensor_datatype->MaxRange * UNIT_CONVERT_Q16));
    setMinFreq(SNS_SMGR_REPORT_RATE_MIN_HZ_V01);
    return;
}

/*===========================================================================
  FUNCTION:  processReportInd
    process the sensor data indicator from the sensor1 type to sensor event type
    Parameters
    @smgr_data : the sensor1 data message from the sensor1 callback
    @sensor_data : the sensor event message that will be send to framework
===========================================================================*/
void SMGRGameRV::processReportInd(sns_smgr_periodic_report_ind_msg_v01* smgr_ind,
            sns_smgr_data_item_s_v01* smgr_data, sensors_event_t &sensor_data)
{
    UNREFERENCED_PARAMETER(smgr_ind);
    UNREFERENCED_PARAMETER(smgr_data);
    sensor_data.type = SENSOR_TYPE_GAME_ROTATION_VECTOR;

    if(bWakeUp == false) {
        HAL_LOG_VERBOSE("%s:(GAME RV non wake_up)",__FUNCTION__);
        sensor_data.sensor = HANDLE_SMGR_GAME_RV;
    } else {
        HAL_LOG_VERBOSE("%s:(GAME RV wake_up)",__FUNCTION__);
        sensor_data.sensor = HANDLE_SMGR_GAME_RV_WAKE_UP;
    }
    /*
    * 1. Converts from SAE to Android coordinates.
    *       x' = y;
    *       y' = x;
    *       z' = -z;
    *       w' = sqrt(1 - x'*x' - y'*y' - z'*z')
    * 2. Assumes unit quaternion.
    */
    if ( smgr_ind->Item_len == 2 ) {
        sns_smgr_data_item_s_v01* data_lsb = &smgr_ind->Item[0];
        sns_smgr_data_item_s_v01* data_msb = &smgr_ind->Item[1];

        sensor_data.data[0] = (float)(data_lsb->ItemData[1]) * UNIT_CONVERT_Q16;
        sensor_data.data[1] = (float)(data_lsb->ItemData[0]) * UNIT_CONVERT_Q16;
        sensor_data.data[2] = -(float)(data_lsb->ItemData[2]) * UNIT_CONVERT_Q16;
        sensor_data.data[3] = (float)(data_msb->ItemData[0]) * UNIT_CONVERT_Q16;

        HAL_LOG_VERBOSE("%s: Game RV x:%f y:%f z:%f w:%f", __FUNCTION__,
            sensor_data.data[0], sensor_data.data[1],
            sensor_data.data[2], sensor_data.data[3] );
    }
    else {
        HAL_LOG_ERROR( "%s: Incorrect item len %d", __FUNCTION__,
                       smgr_ind->Item_len );
    }
}

/*===========================================================================
  FUNCTION:  prepareAddMsg
    SMGRSensor::SMGRPrepareAddMsg will call this function and this func will
    fill the item that needed for this type of sensor.
    Parameters
    @buff_req : the sensor1 message buffer
===========================================================================*/
void SMGRGameRV::prepareAddMsg(sns_smgr_buffering_req_msg_v01 **buff_req)
{
    (*buff_req)->Item_len = 2;
    (*buff_req)->Item[0].SensorId = SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01;
    (*buff_req)->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    (*buff_req)->Item[0].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;
    (*buff_req)->Item[1].SensorId = SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01;
    (*buff_req)->Item[1].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    (*buff_req)->Item[1].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;
}
