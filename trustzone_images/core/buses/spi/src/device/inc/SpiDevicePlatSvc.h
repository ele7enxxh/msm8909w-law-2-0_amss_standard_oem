#ifndef _SPIDEVICEPLATSVC_H_
#define _SPIDEVICEPLATSVC_H_
/*
===========================================================================

FILE:   SpiDevicePlatSvc.h

DESCRIPTION:
    This file contains the API for the platform services.

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/inc/SpiDevicePlatSvc.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
05/15/13 ag     Created
===========================================================================
        Copyright c 2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"
#include "SpiDeviceBam.h"


typedef struct _SpiDevicePlatInfoType
{
   uint32   		qupVirtBlockAddr;
   uint32   		qupPhysBlockAddr;
   uint32           tcsrVirtBlockAddr;
   boolean  		bInterruptBased;
   uint32 			uQupCoreNum;
   boolean  		bBamSupported;
   SpiDeviceBamType *spiBam;
   boolean              bTcsrInterruptRequired;
   uint32				uTcsrInterruptBitMask;
   uint32				uTcsrInterruptBitShift;
}SpiDevicePlatInfoType;


//Interface: IBusPlatCfg
/** @brief Releases data structures necessary to support the target calls.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_DeInitTarget(SPIDEVICE_PLATCFG_HANDLE hPlat);

//Interface: IBusPlatCfg
/** @brief Initializes data structures necessary to support the target calls.

    @param[in] phPlat pointer to platform device handle.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_InitTarget(uint32 uDevId, SPIDEVICE_PLATCFG_HANDLE *phPlat);


/** @brief Gets the app clock frequency in Hz.

    @param[in] hPlat platform device handle.
    @param[out] pFrequencyHz Frequency of the core.

    @return          int32 .
  */
uint32 SpiDevicePlatCfg_GetAppClkHz
(
   SPIDEVICE_PLATCFG_HANDLE hPlat,
   uint32 * pFrequencyHz
);

/** @brief Sets the app clock frequency in Hz.

    @param[in] hPlat platform device handle.
    @param[in] reqFrequencyHz Frequency requested
    @param[out] pFinalFrequencyHz Frequency after the request.

    @return          int32 .
  */
uint32 SpiDevicePlatCfg_SetAppClkHz
(
   SPIDEVICE_PLATCFG_HANDLE hPlat,
   uint32    reqFrequencyHz,
   uint32 * pFinalFrequencyHz
);

//Interface: IBusPower
/** @brief Sets the power state of the device.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_SetPowerState(SPIDEVICE_PLATCFG_HANDLE hPlat, uint32 state);

/** @brief Gets the power state of the device.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_GetPowerState(SPIDEVICE_PLATCFG_HANDLE hPlat, uint32 state);

//Interface: IBusStore
/** @brief Reads the properties for this device.

    @param[in] hPlat platform device handle.
    @param[in] pDevProp       Pointer to properties structure.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_GetPlatformInfo
(
   SPIDEVICE_PLATCFG_HANDLE hPlat, SpiDevicePlatInfoType *pDevProp 
);

/** @brief Returns the core number for this device.

    @param[in] hPlat platform device handle.

    @return               int32.
  */
int32 SpiDevicePlatCfg_PlatInternal_GetQupCoreNum
(
   SPIDEVICE_PLATCFG_HANDLE hPlat
);

/** @brief Returns the id for this device.

    @param[in] hPlat platform device handle.

    @return               int32.
  */
int32 SpiDevicePlatCfg_PlatInternal_GetDalId
(
   SPIDEVICE_PLATCFG_HANDLE hPlat
);

/** @brief Returns the physical address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 SpiDevicePlatCfg_PlatInternal_GetQupCorePhysicalAddr
(
    SPIDEVICE_PLATCFG_HANDLE hPlat
);

/** @brief Returns the virtual address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 SpiDevicePlatCfg_PlatInternal_GetQupCoreVirtualAddr
(
   SPIDEVICE_PLATCFG_HANDLE hPlat
);

/** @brief Returns the BAM configuration data of QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
SpiDeviceBamCfg_t* SpiDevicePlatCfg_PlatInternal_GetBamCfg
(
   SPIDEVICE_PLATCFG_HANDLE hPlat
);
#endif // _SPIDEVICEPLATSVC_H_

