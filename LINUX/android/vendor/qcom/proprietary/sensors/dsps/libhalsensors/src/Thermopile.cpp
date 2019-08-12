/*============================================================================
  @file Thermopile.cpp

  @brief
  Thermopile class implementation.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "Thermopile.h"

/*============================================================================
  Thermopile Constructor
============================================================================*/
Thermopile::Thermopile(int handle)
    :SMGRSensor(handle)
{
    trigger_mode = SENSOR_MODE_EVENT;
    bWakeUp = false;
}

/*============================================================================
  Thermopile Destructor
============================================================================*/
Thermopile::~Thermopile()
{

}
/*===========================================================================
  FUNCTION:  setSensorInfo
    Fill the sensor information from the sns_smgr_sensor_datatype_info_s_v01 type
    Parameters
    @datatype : sensor information got from the sensor1 callback
    @info : sensor information to be reported to the framework
===========================================================================*/
void Thermopile::setSensorInfo(sns_smgr_sensor_datatype_info_s_v01* sensor_datatype)
{
    HAL_LOG_DEBUG("%s: Thermopile DTy: %d", __FUNCTION__, sensor_datatype->DataType);
    setType(SENSOR_TYPE_THERMOPILE);
    setFlags(SENSOR_FLAG_ON_CHANGE_MODE);
    setResolution((float)((float)sensor_datatype->Resolution *
            UNIT_CONVERT_Q16));
    setMaxRange((float)((float)sensor_datatype->MaxRange *
            UNIT_CONVERT_Q16));
    return;
}

/*===========================================================================
  FUNCTION:  processReportInd
    process the sensor data indicator from the sensor1 type to sensor event type
    Parameters
    @smgr_data : the sensor1 data message from the sensor1 callback
    @sensor_data : the sensor event message that will be send to framework
===========================================================================*/
void Thermopile::processReportInd(sns_smgr_periodic_report_ind_msg_v01* smgr_ind,
            sns_smgr_data_item_s_v01* smgr_data, sensors_event_t &sensor_data)
{
    UNREFERENCED_PARAMETER(smgr_ind);
    sensor_data.type = SENSOR_TYPE_THERMOPILE;
    sensor_data.sensor = HANDLE_THERMOPILE;
    HAL_LOG_VERBOSE("%s:sensor %s ",__FUNCTION__,
                    Utility::SensorTypeToSensorString(getType()));
    sensor_data.data[0] = (float)(smgr_data->ItemData[0]) * UNIT_CONVERT_Q16;
    sensor_data.data[1] = (float)(smgr_data->ItemData[1]) * UNIT_CONVERT_Q16;
    sensor_data.data[2] = (float)(smgr_data->ItemData[2]) * UNIT_CONVERT_Q16;
    HAL_LOG_VERBOSE("%s: Obj Temp: %f, Human Temp: %f, Ambient Temp: %f", __FUNCTION__,
                    sensor_data.data[0], sensor_data.data[1], sensor_data.data[2]);
}

/*===========================================================================
  FUNCTION:  prepareAddMsg
    SMGRSensor::SMGRPrepareAddMsg will call this function and this func will
    fill the item that needed for this type of sensor.
    Parameters
    @buff_req : the sensor1 message buffer
===========================================================================*/
void Thermopile::prepareAddMsg(sns_smgr_buffering_req_msg_v01 **buff_req)
{
    (*buff_req)->Item[0].SensorId = SNS_SMGR_ID_OBJECT_TEMP_V01;
    (*buff_req)->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
}
