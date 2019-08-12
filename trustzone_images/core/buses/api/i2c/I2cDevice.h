#ifndef _I2CDEVICE_H_
#define _I2CDEVICE_H_
/*
===========================================================================

FILE:   I2cDevice.h

DESCRIPTION:
    This file contains the API for the I2CDevice 
    Interface: II2cDevice 

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/api/i2c/I2cDevice.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/26/11 LK     Created

===========================================================================
        Copyright c 2011 - 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "I2cTransfer.h"
#include "I2cError.h"


typedef void (*DEV_ASYNC_CB)(void * pArg);

typedef void*     I2CDEV_HANDLE;



typedef enum I2CDEV_PowerStates
{
   I2CDEV_POWER_STATE_0,/**< Bus and clocks off. */
   I2CDEV_POWER_STATE_1,/**< Bus on clocks off. */
   I2CDEV_POWER_STATE_2,/**< Bus on clocks on. */
} I2CDEV_PowerStates;


typedef enum I2CDEV_Command
{
   I2CDEV_COMMAND_NULL        , /**< Do nothing. */
   I2CDEV_COMMAND_RESET_DEVICE, /**< Resets the device. */
   
} I2CDEV_Command;



/** @brief Initializes the device.

    This Function Initializes the device and creates the
    necessary data structures to support other calls into the
    device.

    @param[in] uPlatDevId    Platform Device ID.
    @param[in] phDev         Pointer to device handle.

    @return          int32 .
  */
int32
I2CDEV_Init
(
   uint32                                  uPlatDevId,
   I2CDEV_HANDLE                          *phDev 
);

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.

    @param[in] hDev      Device handle.

    @return          int32 .
  */
int32
I2CDEV_DeInit
(
   I2CDEV_HANDLE                        hDev 
);

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer where transfer data will
          be stored.
    @param[in] pClntCfg        Pointer to Client configuration.
                                If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          reads.

    @return          int32 .
  */
int32
I2CDEV_Read
(
   I2CDEV_HANDLE                        hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,
   uint32                               *puNumCompleted
); 

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] pClntCfg        Pointer to Client configuration.
                                If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          int32 .
  */
int32
I2CDEV_Write
(
   I2CDEV_HANDLE             hDev,
   I2cTransfer               *pTransfer,
   I2cClientConfig           *pClntCfg,
   uint32                    *puNumCompleted
);

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to a sequence of transfer
          data.
    @param[in] pClntCfg       Pointer to Client configuration.
                                If null keep previous.
    @param[in] pIoRes         Pointer to input output result.

    @return          int32 .
  */
int32
I2CDEV_BatchTransfer
(
   I2CDEV_HANDLE                        hDev     ,
   I2cSequence                         *pSequence,
   I2cClientConfig                     *pClntCfg ,// if null keep previous
   I2cIoResult                         *pIoRes
);



/** @brief Schedules a buffer read from i2c device.

    Schedules a buffer read from i2c device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                                If null keep previous.
    @param[in] pIoRes         Pointer to result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          int32 .
  */
int32
I2CDEV_AsyncRead
(
   I2CDEV_HANDLE                        hDev               ,
   I2cTransfer                         *pTransfer          ,
   I2cClientConfig                     *pClntCfg           ,
   I2cIoResult                         *pIoRes             ,
   DEV_ASYNC_CB                         pCallbackFn        ,
   void                                *pArg            
); 

/** @brief Schedules a buffer write to i2c device.

    Schedules a buffer write to i2c device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                                If null keep previous.
    @param[in] pIoRes         Pointer to result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32.
  */
int32
I2CDEV_AsyncWrite
(
   I2CDEV_HANDLE                        hDev               ,
   I2cTransfer                         *pTransfer          ,
   I2cClientConfig                     *pClntCfg           ,
   I2cIoResult                         *pIoRes             ,
   DEV_ASYNC_CB                         pCallbackFn        ,
   void                                *pArg            
);

/** @brief Schedules a batch of transfers in a sequence.

    Schedulest a batch of transfers in a sequence and returns.
    The callback will be called to notify transfer is done or
    has failed.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to the sequence of
          transfers.
    @param[in] pClntCfg       Pointer to Client configuration.
                                If null keep previous.
    @param[in] pIoRes         Pointer to sequence result.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32.
  */
int32
I2CDEV_AsyncBatchTransfer
(
   I2CDEV_HANDLE                        hDev               ,
   I2cSequence                         *pSequence          ,
   I2cClientConfig                     *pClntCfg           ,
   I2cIoResult                         *pIoRes             ,
   DEV_ASYNC_CB                         pCallbackFn        ,
   void                                *pArg            
);

/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by reseting the core.

    @param[in] hDev           Device handle.

    @return                   int32.
  */
int32
I2CDEV_CancelTransfer
(
   I2CDEV_HANDLE                        hDev
);

/** @brief Set the power state of the device.

    This Function sets the power state for the device.

    @param[in] hDev           Device handle.
    @param[in] ePowerState    New power state.

    @return          int32 .
  */
int32
I2CDEV_SetPowerState
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_PowerStates                   ePowerState
);

/** @brief Gets the power state of the device.

    This Function gets the power state of the device.

    @param[in] hDev            Device handle.
    @param[out] pePowerState   Pointer to returned power state.

    @return          int32 .
  */
int32
I2CDEV_GetPowerState
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_PowerStates                  *pePowerState
);

/** @brief Sends a cmd to the i2c dev.

    This Function  Sends a cmd to the i2c dev.

    @param[in]     hDev            Device handle.
    @param[in]     devCmd          Command to be sent to device.
    @param[inout]  pCmdData        Data related to the command.

    @return          int32 .
  */
int32
I2CDEV_SendCmd
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_Command                       devCmd    ,
   void                                *pCmdData
);


#endif // _I2CDEVICEQUP_H_
