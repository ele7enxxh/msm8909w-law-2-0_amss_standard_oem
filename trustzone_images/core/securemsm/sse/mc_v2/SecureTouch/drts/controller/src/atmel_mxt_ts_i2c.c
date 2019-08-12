/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "drStd.h"
#include "MtkApiTzbspApiWrapper.h"
#include "i2c_stub.h"
#include "atmel_mxt_ts_i2c.h"


static tzbspI2cBusConfig_t g_busConfig =
{
  100,                              // uFsBusFreqInKhz;
  0,                                // uHsBusFreqInKhz; not used
  TZBSP_I2C_STANDARD_INTERFACE,     // eInterface;  not used
  2500                              // nMaxBusAcquireWaitTimeMsec;
};

static tzbspI2cSlaveDeviceConfig_t g_deviceConfig = {
  .uSlaveAddress = I2C_ADDRESS,
  .eSlaveAddressOption = TZBSP_I2C_7_BIT_SLAVE_ADDRESS,
  .eSlaveDeviceType = TZBSP_I2C_MEMORY_ADDRESS_DEVICE,
  .eReadOption = TZBSP_I2C_GEN_STOP_START_BEFORE_READ,
  .nByteTransferWaitTimeUSec = 2500
};

static tzbspI2cConfig_t g_i2c_config = {
  .pBusConfig = &g_busConfig,
  .pSlaveConfig = &g_deviceConfig
};

int32_t i2cReadReg(const uint16_t address, void *buf, const uint32_t bufLen)
{
  tzbspI2cTransactionInfo_t  readInfo = {
    .uStartAddr = ((address >> 8) & 0x00FF) | ((address & 0x00FF) << 8),
    .pBuf = (unsigned char*)buf,
    .uBufLen = bufLen,
    .uTotalBytes = 0
  };
  return tzbspApi_I2CRead(
    I2C_DEVICE,
    &g_i2c_config,
    &readInfo
    );
}

int32_t i2cOpen(void)
{
  return tzbspApi_I2COpen(I2C_DEVICE);
}

int32_t i2cClose(void)
{
  return tzbspApi_I2CClose(I2C_DEVICE);
}
