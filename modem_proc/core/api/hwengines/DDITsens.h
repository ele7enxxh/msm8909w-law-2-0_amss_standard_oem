#ifndef __DDITSENS_H__
#define __DDITSENS_H__

/**
  @file DDITsens.h

  Temperature Sensor (TSens) Device Access Library (DAL) device driver interface.
  
  This file contains data structures and functions used to configure, control, 
  and query temperature sensors.
*/ 
 
/*============================================================================
               Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/api/hwengines/DDITsens.h#1 $ */

#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"

#define DALTSENS_INTERFACE_VERSION DALINTERFACE_VERSION(2,1)

enum
{
   TSENS_ERROR_INVALID_CALIBRATION = 1,
   TSENS_ERROR_TIMEOUT,
   TSENS_ERROR_INVALID_PARAMETER,
   TSENS_ERROR_UNSUPPORTED,
};

/*!
 * Threshold type - used when setting thresholds
 */
typedef enum
{
   TSENS_THRESHOLD_LOWER = 0,
   TSENS_THRESHOLD_UPPER,
   _TSENS_NUM_THRESHOLDS
} TsensThresholdType;

/*!
 * Critical threshold type - used when setting critical thresholds
 */
typedef enum
{
   TSENS_CRITICAL_THRESHOLD_MIN = 0,
   TSENS_CRITICAL_THRESHOLD_MAX,
   _TSENS_NUM_CRITICAL_THRESHOLDS
} TsensCriticalThresholdType;

#define TSENS_INVALID_THRESHOLD (0x7fffffff)

/** @addtogroup c_structs
@{ */

typedef struct
{
   int32 nMinDegC;
   int32 nMaxDegC;
} TsensTempRangeType;

typedef struct
{
   int32 nDegC;                    /**< Temperature in degrees Celsius. */
} TsensTempType;

typedef struct
{
   TsensThresholdType eThreshold;   /* Type of threshold that triggered */
   uint32 nSensor;                  /* Sensor that triggered */
   int32 nTriggeredDegC;            /* Temperature value that triggered */
} TsensCallbackPayloadType;
/** @} */ /* end_addtogroup c_structs */

/**@cond
*/
typedef struct DalTsens DalTsens;
struct DalTsens
{
   DalDevice DalDevice;
   DALResult (*GetTemp)(DalDeviceHandle *, uint32 nSensor, TsensTempType *);
   DALResult (*GetTempRange)(DalDeviceHandle *, uint32 nSensor, TsensTempRangeType *);
   DALResult (*SetThreshold)(DalDeviceHandle *, uint32 nSensor, TsensThresholdType, int32, DALSYSEventHandle);
   DALResult (*SetEnableThresholds)(DalDeviceHandle *, DALBOOL);
   DALResult (*SetCriticalThreshold)(DalDeviceHandle *, uint32, TsensCriticalThresholdType, int32);
   DALResult (*GetNumSensors)(DalDeviceHandle *, uint32 *);
   DALResult (*CheckCalibration)(DalDeviceHandle *, uint32 nSensor);
};

typedef struct DalTsensHandle DalTsensHandle; 
struct DalTsensHandle 
{
   uint32 dwDalHandleId;
   const DalTsens * pVtbl;
   void * pClientCtxt;
};
/** @endcond
*/

/** @addtogroup api_funcs
@{ */

/**
  Attaches the client to the TSens driver.
   
  This define is used to obtain the device handle that is required to use the TSens 
  driver APIs.

  @param[in]  DevId        Device ID (e.g., DALDEVICEID_TSENS).
  @param[out] hDalDevice   A double pointer to the DAL device handle.

  @return 
  DAL_SUCCESS -- Attach was successful.\n 
  DAL_ERROR -- An error occurred when attaching to the device.
*/
#define DAL_TsensDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTSENS_INTERFACE_VERSION,hDalDevice)

/**
  Gets the last temperature measurement that occurred within a measurement 
  period.
  
  This function waits until a measurement is complete. This means the calling 
  thread can be blocked by up to several hundreds of microseconds. The exact 
  delay depends on the number of sensors present in the hardware and the hardware 
  conversion time per sensor. There is a fixed timeout value built into this function. 
  If the measurement does not complete before the timeout, this function 
  will return TSENS_ERROR_TIMEOUT. 
        
  @param[in] _h               Handle to the TSens driver instance instantiated 
                              by DAL_TsensDeviceAttach.
  @param[in] nSensor          Sensor to read from.
  @param[out] pTemp           Pointer to the temperature

  @return 
  DAL_SUCCESS -- Successfully retrieved the temperature measurement.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n 
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n 
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.\n 
  TSENS_ERROR_TIMEOUT -- The TSENS reading timed out. 
*/
static __inline DALResult
DalTsens_GetTemp(DalDeviceHandle * _h, uint32 nSensor, TsensTempType *pTemp)
{
   if(pTemp == NULL)
   {
      return DAL_ERROR;
   }

   return ((DalTsensHandle *)_h)->pVtbl->GetTemp( _h, nSensor, pTemp);
}

/**
  Gets the temperature range supported for a given sensor.

  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[in] nSensor            Sensor to get range from.
  @param[out] pTsensTempRange   Pointer to the sensor range.

  @return 
  DAL_SUCCESS -- Successfully retrieved the supported temperature range.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n 
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n 
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.
*/
static __inline DALResult
DalTsens_GetTempRange(DalDeviceHandle * _h, uint32 nSensor, TsensTempRangeType * pTsensTempRange)
{
   if(pTsensTempRange == NULL)
   {
      return DAL_ERROR;
   }

   return ((DalTsensHandle *)_h)->pVtbl->GetTempRange( _h, nSensor, pTsensTempRange);
}

/**
  Configures a temperature threshold for the specified sensor
   
  The threshold event will be triggered once when the threshold is crossed. 
  After the event is triggered, the threshold will not trigger the event again 
  and will be in a triggered state until the client calls DalTsens_SetThreshold 
  to set a new threshold.
   
  Note that thresholds can be disabled/re-enabled on a per client basis by calling 
  DalTsens_SetEnableThresholds. Thresholds are enabled by default, but calling 
  DalTsens_SetThreshold does not automatically re-enable them if they were previously 
  disabled by a call to DalTsens_SetEnableThresholds. 
   
  The event type can be: 
    - DALSYS_EVENT_ATTR_NORMAL: the event will be triggered.
    - DALSYS_EVENT_ATTR_CALLBACK_EVENT: the callback will have a payload pointing
      to a TsensCallbackPayloadType struct. 
   
  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[in] nSensor            Sensor to use for the threshold. 
  @param[in] eThreshold         Type of threshold to set.
  @param[in] nDegC              Threshold temperature in degrees C. 
  @param[in] hEvent             Event which is notified when the threshold is reached.

  @return 
  DAL_SUCCESS -- Successfully set the threshold.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n 
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.
*/
static __inline DALResult
DalTsens_SetThreshold(DalDeviceHandle * _h, uint32 nSensor, TsensThresholdType eThreshold, int32 nDegC, DALSYSEventHandle hEvent)
{
   return ((DalTsensHandle *)_h)->pVtbl->SetThreshold( _h, nSensor, eThreshold, nDegC, hEvent);
}

/**
  Enables or disables the upper and lower thresholds that were registered by this 
  client by calls to DalTsens_SetThreshold.
   
  By default, thresholds are enabled. 
   
  Thresholds are not monitored while the thresholds are disabled, 
  and any threshold crossings which occurred while the thresholds were disabled 
  are ignored. 
   
  Threshold values and event handles set by DalTsens_SetThreshold 
  are still retained while thresholds are disabled. 
  
  This does not affect the critical thresholds. Critical thresholds are always 
  enabled. 

  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[in] bEnable            TRUE to enable thresholds. FALSE to disable thresholds.

  @return 
  DAL_SUCCESS -- Operation occurred successfully.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.
*/
static __inline DALResult
DalTsens_SetEnableThresholds(DalDeviceHandle * _h, DALBOOL bEnableThresholds)
{
   return ((DalTsensHandle *)_h)->pVtbl->SetEnableThresholds(_h, bEnableThresholds);
}

/**
  Configures a critical temperature threshold for the specified sensor. A critical 
  temperature threshold performs an automatic hardware shutdown.
   
  Critical thresholds are enabled as soon as they are set and are not affected 
  by calls to DalTsens_SetEnableThresholds.
    
  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[in] nSensor            Sensor to use for the threshold. 
  @param[in] eThreshold         Type of threshold to set.
  @param[in] nDegC              Threshold temperature in degrees C.

  @return 
  DAL_SUCCESS -- Successfully set the critical threshold.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n 
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.
*/
static __inline DALResult
DalTsens_SetCriticalThreshold(DalDeviceHandle * _h, uint32 nSensor, TsensCriticalThresholdType eThreshold, int32 nDegC)
{
   return ((DalTsensHandle *)_h)->pVtbl->SetCriticalThreshold( _h, nSensor, eThreshold, nDegC);
}

/**
  Returns the number of on-die temperature (TSENS) sensors present in the SoC.
   
  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[out] pnNumSensors      Number of sensors

  @return 
  DAL_SUCCESS -- Successfully retrieved the number of sensors.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.
*/
static __inline DALResult
DalTsens_GetNumSensors(DalDeviceHandle * _h, uint32 *pnNumSensors)
{
   return ((DalTsensHandle *)_h)->pVtbl->GetNumSensors( _h, pnNumSensors);
}

/**
  Checks the status of the Tsens calibration.
   
  @param[in]  _h                Handle to the TSens driver instance instantiated 
                                by DAL_TsensDeviceAttach.
  @param[in] nSensor            Sensor to check the calibration. 

  @return 
  DAL_SUCCESS -- The TSENS calibration data is present in the hardware.\n 
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n 
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n 
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.
*/
static __inline DALResult
DalTsens_CheckCalibration(DalDeviceHandle * _h, uint32 nSensor)
{
   return ((DalTsensHandle *)_h)->pVtbl->CheckCalibration( _h, nSensor);
}

/** @} */ /* addtogroup api_funcs */


#endif
