/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief Vehicle Network provider implementation file.

This file defines VNW provider object implementation.

@ingroup slim_VNWProvider
*/
#include <inttypes.h>
#include "slim_os_log_api.h"
#include "VehicleNetworkProvider.h"

//! @addtogroup slim_VNWProvider
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* This unit logs as Sensor1 component. */
#undef SLIM_LOG_LOCAL_MODULE
//! @brief Logging module for VNW provider.
#define SLIM_LOG_LOCAL_MODULE SLIM_LOG_MOD_VNW

using namespace slim;

const uint32_t VehicleNetworkProvider::s_qThreadId = THREAD_ID_VNW;
VehicleNetworkProvider *VehicleNetworkProvider::s_pzInstance;
Mutex VehicleNetworkProvider::s_zInstanceMutex;
const slimAvailableServiceMaskT VehicleNetworkProvider::s_qSupportedServices =
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ACCEL) |
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_GYRO) |
    (uint32_t(1) << eSLIM_SERVICE_VEHICLE_ODOMETRY);

void VehicleNetworkProvider::threadMain
(
  void *pData
)
{
  VehicleNetworkProvider *pzVNWProvider = reinterpret_cast<VehicleNetworkProvider*>(pData);
  slim_IpcStart(s_qThreadId);
  pzVNWProvider->m_pzTimer = slim_TimerCreate(eTIMER_DEFAULT, s_qThreadId);
  /* Service support is enabled */
  pzVNWProvider->m_qServiceMask = s_qSupportedServices;
  pzVNWProvider->routeConfigurationChange(s_qSupportedServices);
  pzVNWProvider->runEventLoop();
}

/*!
 * @brief Returns provider instance.
 *
 * Method provides access to provider instance. If necessary, the instance
 * is created and initialized.
 *
 * @return Provider instance.
 * @retval 0 On error.
 */
slim_ServiceProviderInterfaceType *VehicleNetworkProvider::getProvider()
{

  MutexLock _l(s_zInstanceMutex);
  if (0 == s_pzInstance)
  {
    s_pzInstance = new VehicleNetworkProvider;
    if (!s_pzInstance->initialize())
    {
      SLIM_LOGE("VNW provider initialization failed;");
      delete s_pzInstance;
      s_pzInstance = 0;
      return 0;
    }
  }
  return s_pzInstance->getSlimInterface();
}

VehicleNetworkProvider::VehicleNetworkProvider() :
  MultiplexingProvider(SLIM_PROVIDER_VNW, false, 0),
  m_zThread(),
  m_zMutex(),
  m_pzTimer(0),
  m_qEnabledServices(0)
{
  memset(&m_zThread, 0, sizeof(m_zThread));
}
VehicleNetworkProvider::~VehicleNetworkProvider()
{
}
bool VehicleNetworkProvider::initialize()
{
  if (!slim_ThreadCreate(&m_zThread, threadMain, this, "VNWPoll"))
  {
    return false;
  }
  slim_TaskReadyWait(THREAD_ID_VNW);

  return true;
}
void VehicleNetworkProvider::runEventLoop()
{
  SLIM_LOGD("Starting event loop");
  // slim_TimerStart(m_pzTimer, 500, 0);
  // Notify controller that the task is ready to run.
  slim_TaskReadyAck();
  while (1)
  {
    slim_IpcMsgT* pz_Msg = NULL;
    while(NULL == (pz_Msg = slim_IpcReceive()));

    SLIM_LOGD("IPC message received. q_MsgId:%" PRIu32 ", q_SrcThreadId:%" PRIu32,
        (uint32_t)pz_Msg->q_MsgId,
        (uint32_t)pz_Msg->q_SrcThreadId);

    switch(pz_Msg->q_MsgId)
    {

    case M_IPC_MSG_TIMER:
        {
          os_TimerExpiryType z_TimerMsg;
          slim_Memscpy(&z_TimerMsg, sizeof(z_TimerMsg), pz_Msg->p_Data, pz_Msg->q_Size);
          SLIM_LOGD("Timer signal received");
          MutexLock _l(m_zMutex);
          if (0 != m_qEnabledServices)
          {
            if (0 != SLIM_MASK_IS_SET(m_qEnabledServices, eSLIM_SERVICE_VEHICLE_ACCEL))
            {
              slimVehicleSensorDataStructT zData;
              memset(&zData, 0, sizeof(zData));
              zData.timeBase = slim_TimeTickGetMilliseconds();
              zData.timeSource = eSLIM_TIME_SOURCE_COMMON;
              zData.flags = 0;
              zData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
              zData.sensorType = eSLIM_VEHICLE_SENSOR_TYPE_ACCEL;
              zData.axesValidity = 0;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_X_AXIS;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_Y_AXIS;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_Z_AXIS;
              zData.samples_len = 1;
              zData.samples[0].sampleTimeOffset = 10;
              zData.samples[0].sample[0] = .5f;
              zData.samples[0].sample[1] = .3f;
              zData.samples[0].sample[2] = -.2f;
              routeIndication(eSLIM_SERVICE_VEHICLE_ACCEL, zData);
            }
            if (0 != SLIM_MASK_IS_SET(m_qEnabledServices, eSLIM_SERVICE_VEHICLE_GYRO))
            {
              slimVehicleSensorDataStructT zData;
              memset(&zData, 0, sizeof(zData));
              zData.timeBase = slim_TimeTickGetMilliseconds();
              zData.timeSource = eSLIM_TIME_SOURCE_COMMON;
              zData.flags = 0;
              zData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
              zData.sensorType = eSLIM_VEHICLE_SENSOR_TYPE_GYRO;
              zData.axesValidity = 0;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_X_AXIS;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_Y_AXIS;
              zData.axesValidity |= SLIM_MASK_VEHICLE_SENSOR_Z_AXIS;
              zData.samples_len = 1;
              zData.samples[0].sampleTimeOffset = 10;
              zData.samples[0].sample[0] = .3f;
              zData.samples[0].sample[1] = .5f;
              zData.samples[0].sample[2] = -.1f;
              routeIndication(eSLIM_SERVICE_VEHICLE_GYRO, zData);
            }
            if (0 != SLIM_MASK_IS_SET(m_qEnabledServices, eSLIM_SERVICE_VEHICLE_ODOMETRY))
            {
              slimVehicleOdometryDataStructT zData;
              memset(&zData, 0, sizeof(zData));
              zData.timeBase = slim_TimeTickGetMilliseconds();
              zData.timeSource = eSLIM_TIME_SOURCE_COMMON;
              zData.provider = eSLIM_SERVICE_PROVIDER_NATIVE;
              zData.wheelFlags = 0;
              zData.wheelFlags |= SLIM_MASK_VEHICLE_ODOMETRY_LEFT_AND_RIGHT_AVERAGE;
              zData.wheelFlags |= SLIM_MASK_VEHICLE_ODOMETRY_LEFT;
              zData.wheelFlags |= SLIM_MASK_VEHICLE_ODOMETRY_RIGHT;
              zData.samples_len = 1;
              zData.samples[0].sampleTimeOffset = 10;
              zData.samples[0].distanceTravelled[0] = 1;
              zData.samples[0].distanceTravelled[1] = 2;
              zData.samples[0].distanceTravelled[2] = 3;
              routeIndication(zData);
            }

            if(!slim_TimerStart(m_pzTimer, 500, 0))
            {
                SLIM_LOGE("Error starting timer");
            }
          }
        }
        break;
    }

    slim_IpcDelete(pz_Msg);
  }
}

/**
@brief Method for enabling or disabling vehicle services.

@param[in] uEnable Flag that indicates if the service shall be enabled or
                   disabled.
@param[in] eService      Service to control.

@return eSLIM_SUCCESS is operation succeeded.
*/
slimErrorEnumT VehicleNetworkProvider::doUpdateVehicleDataStatus
(
  bool uEnable,
  slimServiceEnumT eService
)
{
  MutexLock _l(m_zMutex);
  if (uEnable)
  {
    SLIM_MASK_SET(m_qEnabledServices, eService);
  }
  else
  {
    SLIM_MASK_CLEAR(m_qEnabledServices, eService);
  }
  if (0 == m_qEnabledServices)
  {
    slim_TimerStop(m_pzTimer);
  }
  else
  {
    slim_TimerStart(m_pzTimer, 500, eTIMER_DEFAULT);
  }
  return eSLIM_SUCCESS;
}

/**
@brief Initiates time offset request.

Function for making the time request. Successful response enable SLIM to
calculate the offset between modem time and sensor time.

@param[in] lTxnId Service transaction id.
@return eSLIM_SUCCESS if time request is made successfully.
*/
slimErrorEnumT VehicleNetworkProvider::getTimeUpdate
(
  int32_t lTxnId
)
{
  return MultiplexingProvider::getTimeUpdate(lTxnId);
}

//! @}
