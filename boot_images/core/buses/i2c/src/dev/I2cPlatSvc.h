#ifndef I2CPLATSVC_H
#define I2CPLATSVC_H
/*=============================================================================

  @file   I2cPlatSvc.h

   This file contains the API for the platform services.
 
   Copyright (c) 2011 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/i2c/src/dev/I2cPlatSvc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cTypes.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef struct I2CPLATDEV_PropertyType
{
   //char    *pDevName;     
   uint8   *virtBlockAddr;
   uint32   uI2cInputClkKhz;
   boolean  bInterruptBased;
   //boolean  bDisablePm;
   //boolean  bBamAvailable;
   //uint32   uBamThreshold; /* size in bytes when to decide to use bam. */
} I2CPLATDEV_PropertyType;


//typedef void (*BAM_CB)(void * pArg);

//typedef void (*DEV_ASYNC_CB_DISP)(void * pArg);
//typedef void* QUPRES_HANDLE;
typedef void* I2CPLATDEV_HANDLE;

typedef void (*IST_HOOK)(void *pdata);
typedef void (*ISR_HOOK)(void *pdata);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/** @brief Releases data structures necessary to support the target calls.

    @param[in] hPlatDev platform device handle.

    @return          int32 .
  */
int32
I2CPLATDEV_DeInitTarget
(
   I2CPLATDEV_HANDLE hPlatDev
);


/** @brief Initializes data structures necessary to support the target calls.

    @param[in] uPlatDevId platform target id to platform device handle.
    @param[in] phPlatDev pointer to platform device handle.

    @return          int32 .
  */
int32
I2CPLATDEV_InitTarget
(
   uint32             uPlatDevId,
   I2CPLATDEV_HANDLE *phPlatDev
);

/** @brief Resets resources like bam pipes following a qup core reset.

    @param[in] hPlatDev platform device handle.

    @return          int32 .
  */
int32
I2CPLATDEV_ResetTarget
(
   I2CPLATDEV_HANDLE hPlatDev
);



/** @brief Sets the power state of the device.

    @param[in] hPlatDev platform device handle.
    @param[in] state    new power state.

    @return          int32 .
  */
  
int32
I2CPLATDEV_SetPowerState
(
   I2CPLATDEV_HANDLE hPlatDev,
   uint32            state
);

/** @brief Gets the power state of the device.

    @param[in]  hPlatDev platform device handle.
    @param[out] pState   pointer to state.

    @return          int32 .
  */
int32
I2CPLATDEV_GetPowerState
(
   I2CPLATDEV_HANDLE  hPlatDev,
   uint32            *pState
);

/** @brief Reads the properties for this device.

    @param[in] hPlatDev platform device handle.
    @param[in] pDevProp       Pointer to properties structure.

    @return          int32 .
  */
int32 I2CPLATDEV_ReadProperties
(
   I2CPLATDEV_HANDLE        hPlatDev,
   I2CPLATDEV_PropertyType *pDevProp
);


/** @brief Acquires an interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          Nothing .
  */
void
I2CPLATDEV_AcquireIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
);

/** @brief Releases the interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          Nothing .
  */
void
I2CPLATDEV_ReleaseIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
);

/** @brief Checks if BAM is enabled.

    @param[in] hPlatDev platform device handle.
    @return    TRUE if BAM is enabled FALSE otherwise .
  */
boolean
I2CPLATDEV_IsBamEnabled
(
   I2CPLATDEV_HANDLE hPlatDev
);

/** @brief Registers IST/ISR hooks for the indicated device.

    @param[in] hPlatDev platform device handle.
    @param[in] IsrHook  ISR hook.
    @param[in] pIsrData Pointer to data handed back when ISR is called.
    @param[in] IstHook  IST hook, thread handling the request.
    @param[in] pIstData Pointer to data handed back when IST is called.
    @return          int32.
  */

int32
I2CSYS_RegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev, 
   ISR_HOOK          IsrHook,
   void*             pIsrData,
   IST_HOOK          IstHook,
   void*             pIstData
);

/** @brief Unregisters IST/ISR hooks for the indicated device.

    @param[in] hPlatDev platform device handle.
  @return          int32.
  */
int32
I2CSYS_UnRegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev
);





#endif /* #ifndef I2CPLATSVC_H */

