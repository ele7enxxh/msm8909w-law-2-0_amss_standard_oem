#ifndef _SPIDEVICEPLATSVC_H_
#define _SPIDEVICEPLATSVC_H_
/*
===========================================================================

FILE:   SpiDevicePlatSvc.h

DESCRIPTION:
    This file contains the API for the platform services.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/inc/SpiDevicePlatSvc.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/14/12 ag      Fixed EA specific warnings.
05/30/12 ddk     Added APIs to set/get core clock.
04/19/12 ddk     Added APIs to get core physical and virtual address.
03/28/12 ddk     Added QUP physical address info for BAM cmd desc creation.
03/27/12 ag		 Added SPI BAM mode updates.
09/26/11 ag		 Created

===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"
#include "SpiDeviceBam.h"


typedef enum IBUSDMA_Result
{
   IBUSDMA_RESULT_OK = 0
} IBUSDMA_Result;

typedef struct IBusDMADescriptor
{
   uint32 bufSize;
   uint32 BuffVirtAddr;
   uint32 uFlags;
   //TBD
} IBusDMADescriptor;

typedef struct _IBusPlatInfoType
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
//TODO: Add properties as development continues
}IBusPlatInfoType;

typedef void (*BAM_CB)(void * pArg);

typedef void (*DEV_ASYNC_CB_DISP)(void * pArg);
typedef void* QUPRES_HANDLE;

//void IBUSLOG_PRINTFN(uint32 uDevId, uint32 uLogLvl, uint32 uDataCnt, args...);
// debug only
// #include <stdio.h>

extern uint32 IBUSLOGSPI_DynLogLevel;

//#define PRINTN( uDataCnt,...) printf(__VA_ARGS__);
#define PRINTN( uDataCnt,...)
// ULOG used but no build to compile it yet
//#define PRINTN( uDataCnt, args...) ULogFront_RealTimePrintf(ARR_LOG_HNDL[DEV_ID], dataCnt, args)

#define IBUSLOG_PRINTFN(uLogLvl,uDataCnt,...)\
 { if(IBUSLOGSPI_DynLogLevel >= (uLogLvl)) PRINTN(uDataCnt, __VA_ARGS__);}

//ULogFront_RealTimePrintf(ARR_LOG_HNDL[DEV_ID], dataCnt, fmt , args)

//Interface: IBusPlatCfg
/** @brief Releases data structures necessary to support the target calls.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_DeInitTarget(IBUSPLATCFG_HANDLE hPlat);

//Interface: IBusPlatCfg
/** @brief Initializes data structures necessary to support the target calls.

    @param[in] phPlat pointer to platform device handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_InitTarget(uint32 uDevId, IBUSPLATCFG_HANDLE *phPlat);


/** @brief Gets the app clock frequency in Hz.

    @param[in] hPlat platform device handle.
    @param[out] pFrequencyHz Frequency of the core.

    @return          int32 .
  */
uint32 IBUSPLATCFG_GetAppClkHz
(
   IBUSPLATCFG_HANDLE hPlat,
   uint32 * pFrequencyHz
);

/** @brief Sets the app clock frequency in Hz.

    @param[in] hPlat platform device handle.
    @param[in] reqFrequencyHz Frequency requested
    @param[out] pFinalFrequencyHz Frequency after the request.

    @return          int32 .
  */
uint32 IBUSPLATCFG_SetAppClkHz
(
   IBUSPLATCFG_HANDLE hPlat,
   uint32    reqFrequencyHz,
   uint32 * pFinalFrequencyHz
);

//Interface: IBusPower
/** @brief Sets the power state of the device.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_SetPowerState(IBUSPLATCFG_HANDLE hPlat, uint32 state);

/** @brief Gets the power state of the device.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_GetPowerState(IBUSPLATCFG_HANDLE hPlat, uint32 state);

/** @brief Sets the resource exclusive.

    @param[in] hPlat platform device handle.
    @param[in] hRes           Resource handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_SetResourceExclusive
(
   IBUSPLATCFG_HANDLE hPlat,
   QUPRES_HANDLE hRes
);

/** @brief Releases the resource exclusive.

    @param[in] hPlat platform device handle.
    @param[in] hRes           Resource handle.

    @return          int32 .
  */
int32 IBUSPLATCFG_ReleaseResourceExclusive
(
   IBUSPLATCFG_HANDLE hPlat,
   QUPRES_HANDLE      hRes
);

//Interface: IBusStore
/** @brief Reads the properties for this device.

    @param[in] hPlat platform device handle.
    @param[in] pDevProp       Pointer to properties structure.

    @return          int32 .
  */
int32 IBUSPLATCFG_GetPlatformInfo
(
   IBUSPLATCFG_HANDLE hPlat, IBusPlatInfoType *pDevProp 
);

/** @brief Reads the properties for this device.

    @param[in] pFn         Pointer to fn to dispatch.
    @param[in] pArg        Pointer to args to pass to dispach fn.

    @return                Nothing.
  */
void IBUSPLATCFG_DispatchCall
(
   DEV_ASYNC_CB_DISP pFn, void *pArg
);


/** @brief Returns the core number for this device.

    @param[in] hPlat platform device handle.

    @return               int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetQupCoreNum
(
   IBUSPLATCFG_HANDLE hPlat
);

/** @brief Returns the id for this device.

    @param[in] hPlat platform device handle.

    @return               int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetDalId
(
   IBUSPLATCFG_HANDLE hPlat
);

/** @brief Returns the physical address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetQupCorePhysicalAddr
(
    IBUSPLATCFG_HANDLE hPlat
);

/** @brief Returns the virtual address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetQupCoreVirtualAddr
(
   IBUSPLATCFG_HANDLE hPlat
);

#endif // _SPIDEVICEPLATSVC_H_

