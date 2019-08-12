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

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/dev/I2cDevice.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Created

===========================================================================
        Copyright c 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "I2cTypes.h"
#include "I2cDeviceTypes.h"

/** @brief Initializes the device.

    This Function Initializes the device and creates the
    necessary data structures to support other calls into the
    device. This needs to be done outside uImage.

    @param[in] pDev    Pointer to device struct.
    @param[in] pProp   Pointer to properties struct.

    @return          int32 .
  */
int32 I2cDev_Init
(
   I2cDev_Device          *pDev,
   I2cDev_PropertyType    *pProp
);

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.

    @param[in] pDev    Pointer to device struct.

    @return          int32 .
  */
int32 I2cDev_DeInit
(
   I2cDev_Device          *pDev
);


/** @brief Submits a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] pDev           Device structure pointer.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
    @param[out] pIoRes        Pointer to result.
    
    @return          int32 .
  */
int32 I2cDev_SubmitSequence
(
   I2cDev_Device                        *pDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg 
);

/** @brief Calls into the handler for events related to the qup
    	   core.

    @param[in] pDev           Device structure pointer.
    @param[in] evt            Event to process.
    @param[in] pData          Pointer to data associated with
    						  the event.
    
    @return          int32 .
  */
void I2cDev_EventCall
(
   I2cDev_Device                        *pDev ,
   I2cDev_Event                          evt , 
   void                                 *pData
);


/** @brief Gets the device operation status and sequence state.

    @param[in] pDev           Device structure pointer.
    @param[in] pDevState      Pointer to device state.
    
    @return          int32 .
  */
void I2cDev_GetState
(
   I2cDev_Device                        *pDev ,
   I2cDev_State                         *pDevState
);

/**
 This Checks if the transfer has finished.
 It checks if the transfer has completed transmiton and/or
 reception or it is interrupted due to rerrors.
 
 @param[in, out] pDev     Pointer to Qup device structure.
 @param[in]      pSeqInfo Pointer to sequence info.
 @return         TRUE if the transfer completed or was interrupted,
 FALSE otherwise.
 */
boolean
I2cDev_IsTransferComplete
(
   I2cDev_Device               *pDev
);

/** @brief Cancels ongoing transfer.
 
 Cancels ongoing transfer by reseting the core.
 
 @param[in] pDev           Pointer to device structure.
 
 @return                   int32.
 */
int32 I2cDev_AbortSequence
(
   I2cDev_Device                        *pDev
);

/** @brief Sends a cmd to the i2c dev.

    This Function  Sends a cmd to the i2c dev.

    @param[in]     hDev            Device handle.
    @param[in]     devCmd          Command to be sent to device.
    @param[inout]  pCmdData        Data related to the command.

    @return          int32 .
  */
int32 I2cDev_SendCmd
(
   I2cDev_Device                       *pDev      ,
   I2cDev_Command                       devCmd    ,
   void                                *pCmdData
);


#endif // _I2CDEVICE_H_
