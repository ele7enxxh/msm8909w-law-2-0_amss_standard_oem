/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief Vehicle Network provider interface file.

This file declares VNW provider object.

@ingroup slim_VNWProvider
*/
#ifndef __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__
#define __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__

#include <MultiplexingProvider.h>

namespace slim
{

/*!
 * @brief Vehicle Network Provider
 *
 * This provider enables access to vehicle services.
 *
 * @ingroup slim_VNWProvider
 */
class VehicleNetworkProvider :
  public MultiplexingProvider
{
  enum
  {
    eTIMER_DEFAULT,
  };

  static const uint32_t s_qThreadId;
  static Mutex s_zInstanceMutex;
  static VehicleNetworkProvider *s_pzInstance;
  static const slimAvailableServiceMaskT s_qSupportedServices;

  slim_ThreadT m_zThread;
  Mutex m_zMutex;
  slim_TimerT *m_pzTimer;
  mutable slimAvailableServiceMaskT m_qEnabledServices;


  static void threadMain
  (
    void *pData
  );

  VehicleNetworkProvider();
  ~VehicleNetworkProvider();
  bool initialize();
  void runEventLoop();

  /**
  @brief Method for enabling or disabling vehicle services.

  @param[in] uEnable Flag that indicates if the service shall be enabled or
                     disabled.
  @param[in] eService      Service to control.

  @return eSLIM_SUCCESS is operation succeeded.
  */
  virtual slimErrorEnumT doUpdateVehicleDataStatus
  (
    bool uEnable,
    slimServiceEnumT eService
  );

  /**
  @brief Initiates time offset request.

  Function for making the time request. Successful response enable SLIM to
  calculate the offset between modem time and sensor time.

  @param[in] lTxnId Service transaction id.
  @return eSLIM_SUCCESS if time request is made successfully.
  */
  virtual slimErrorEnumT getTimeUpdate
  (
    int32_t lTxnId
  );
public:
  /*!
   * @brief Returns provider instance.
   *
   * Method provides access to provider instance. If necessary, the instance
   * is created and initialized.
   *
   * @return Provider instance.
   * @retval 0 On error.
   */
  static slim_ServiceProviderInterfaceType *getProvider();
};

}

#endif /* __VEHICLE_NETWORK_PROVIDER_H_INCLUDED__ */
