#ifndef __DDITSENS_H__
#define __DDITSENS_H__

/*============================================================================
  @file DDITsens.h
 
  TSENS DAL device driver interface template.
  
               Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/DDITsens.h#1 $ */

#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "TsensCommonData.h"
#include "DDIInterruptController.h"

#define DALTSENS_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef struct
{
   int32 LowerLimit;  /* in degree Celsius */
   int32 UpperLimit;  /* in degree Celsius */
   DALSYSEventHandle hThresholdTriggeredEvent;
} TsensTempTriggerType;

typedef struct DalTsens DalTsens;
struct DalTsens
{
   DalDevice DalDevice;
   DALResult (*EnableSensors)( DalDeviceHandle *);
   DALResult (*DisableSensors)( DalDeviceHandle *);
   DALResult (*SetTempTrigger)( DalDeviceHandle *, TsensTempTriggerType *);
   DALResult (*SetPeriodicAutoSensing)( DalDeviceHandle *, uint32 PeriodInMicroSec);
   DALResult (*GetTempInDegC)( DalDeviceHandle *, TsensSensorType, int32 * pDegreeCelsius);
   DALResult (*GetTempRangeInDegC)( DalDeviceHandle *, TsensTempRangeType * );
};

typedef struct DalTsensHandle DalTsensHandle; 
struct DalTsensHandle 
{
   uint32 dwDalHandleId;
   const DalTsens * pVtbl;
   void * pClientCtxt;
};

/**
   @brief Attaches to the TSENS driver

   This function attaches the client to the TSENS driver. This function is used
   to obtain the device handle which is required to use the TSENS driver APIs.
   
   @param  DevId: [in] The device ID to attach to (e.g., DALDEVICEID_TSENS)
   @param  hDalDevice: [out] Pointer to DAL device handle pointer which will receive
                             a pointer to the TSENS device handle

   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/
#define DAL_TsensDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTSENS_INTERFACE_VERSION,hDalDevice)

/**
   @brief Enable the sensors for a given client

   This function enables the sensors.  The device handle must already been created
   
   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_EnableSensors(DalDeviceHandle * _h)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, EnableSensors ), _h, 0);
   }
   return ((DalTsensHandle *)_h)->pVtbl->EnableSensors( _h);
}

/**
   @brief Disable the sensors for a given client

   This function disables the sensors.  The device handle must already been created
   
   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_DisableSensors(DalDeviceHandle * _h)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, DisableSensors ), _h, 0);
   }
   return ((DalTsensHandle *)_h)->pVtbl->DisableSensors( _h);
}

/**
   @brief Sets Temperature triggers along w/ the associated event used for triggering.

   This function sets the trigger thresholds in deg. C.  It also provide an event used
   to trigger when thresholds are met.
   
   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @param  pTsensTempTrigger: [in] pointer to TsensTempTrigger data structure
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_SetTempTrigger(DalDeviceHandle * _h,  TsensTempTriggerType * pTsensTempTrigger)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_5(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, SetTempTrigger ), _h, pTsensTempTrigger, sizeof( TsensTempTriggerType ));
   }
   return ((DalTsensHandle *)_h)->pVtbl->SetTempTrigger( _h, pTsensTempTrigger);
}

/**
   @brief Sets the period in which the temperature sensor restart measurement.

   This function sets the period in microsecond.  The interval is used to restart
   measurement.  A unit of 0 microsec is used to "measure-once"
   
   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @param  nPeriodInMicroSec: [in] interval in microsec
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_SetPeriodicAutoSensing(DalDeviceHandle * _h,  uint32 nPeriodInMicroSec)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, SetPeriodicAutoSensing ), _h, nPeriodInMicroSec);
   }
   return ((DalTsensHandle *)_h)->pVtbl->SetPeriodicAutoSensing( _h, nPeriodInMicroSec);
}

/**
   @brief Gets the last temperature measurement.

   This function gets the last temperature reading that occured as long as a period ago 
   and as short as a moment ago.

   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @param  Sensor: [in] which of the available sensors
   @param  pDegreeCelsius: [out] pointer to the temperature returned in deg. C
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_GetTempInDegC(DalDeviceHandle * _h, TsensSensorType Sensor, int32 * pDegreeCelsius)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_11(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, GetTempInDegC ), _h, Sensor, pDegreeCelsius, sizeof( pDegreeCelsius ));
   }
   return ((DalTsensHandle *)_h)->pVtbl->GetTempInDegC( _h, Sensor, pDegreeCelsius);
}

/**
   @brief Gets the Temperature Range supported for a given device.

   This routine is used in either a 1-point or a 2-point calibration method since the
   temperature range could be different for each phone.  In a 0-point calibration method,
   the temperature range will return the same for all phone.  At this time of writing,
   the calibration method is still up in the air.  However, this API will support any
   of the 3 methods properly.

   @param  h: [in] Handle to the TSENS driver instance instantiated by DAL_TsensDeviceAttach
   @param  pTsensTempRange: [out] pionter to the temperature range in deg. C
   @return DAL_SUCCESS if the attach was successful. Other return values
           indicate that an error occurred.
           
*/

static __inline DALResult
DalTsens_GetTempRangeInDegC(DalDeviceHandle * _h, TsensTempRangeType * pTsensTempRange)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_9(DALVTBLIDX(((DalTsensHandle *)_h)->pVtbl, GetTempRangeInDegC ), _h, pTsensTempRange, sizeof( TsensTempRangeType ));
   }
   return ((DalTsensHandle *)_h)->pVtbl->GetTempRangeInDegC( _h, pTsensTempRange);
}
#endif
