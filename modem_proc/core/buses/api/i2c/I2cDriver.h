#ifndef _I2CDRIVER_H_
#define _I2CDRIVER_H_
/*=============================================================================

FILE:   I2cDriver.h

        This file contains the API for the I2cDriver
 
        Copyright (c) 2015 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/buses/api/i2c/I2cDriver.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "I2cRegisterTransfer.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

//typedef void (*DRV_ASYNC_CB)(void * pArg);

typedef void*     I2CDRV_HANDLE;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

typedef enum I2cDrv_I2cBusId
{
   I2CDRV_I2C_MIN  = 0,
   I2CDRV_I2C_1    = 0,
   I2CDRV_I2C_2    = 1,
   I2CDRV_I2C_3    = 2,
   I2CDRV_I2C_4    = 3,
   I2CDRV_I2C_5    = 4,
   I2CDRV_I2C_6    = 5,
   I2CDRV_I2C_7    = 6,
   I2CDRV_I2C_8    = 7,
   I2CDRV_I2C_9    = 8,
   I2CDRV_I2C_10   = 9,
   I2CDRV_I2C_11   = 10,
   I2CDRV_I2C_12   = 11,
   I2CDRV_I2C_MAX  = I2CDRV_I2C_12,
   I2CDRV_I2C_NUM  = 12,

   I2CDRV_I2C_DEV_INVALID = -1,
} I2cDrv_I2cBusId;

typedef struct I2cDrv_I2cBus
{
   I2cDrv_I2cBusId      i2cId;
   I2cClientConfig      clntCfg;
   uint32               cookie;
} I2cDrv_I2cBus;

typedef enum I2cDrvCmdType
{
   I2cDrvCmd_Vote_PnocOn,
   I2cDrvCmd_Vote_PnocOff,
   I2cDrvCmd_Vote_I2cClocsOn,
   I2cDrvCmd_Vote_I2cClocsOff,
      
} I2cDrvCmdType;

/** Data to be sent with I2cDrvCmd_Vote_PnocOn .
    The units use for IB/AB are bytes/sec.
    In general terms, IB value is the peak bandwidth requirement,
    while AB value is your average bandwidth requirement over whatever your active time period happens to be.
  */
typedef struct I2cDrvPnocVoteData
{
   uint64    uPnocIb;
   uint64    uPnocAb;
      
} I2cDrvPnocVoteData;



/** @brief Initializes the device.

    This Function opens the i2c bus.

    @param[in] eI2cBusId     I2C Bus ID.
    @param[in] pI2cBus       Pointer to I2C bus struct.
    @param[in]  dwaccessMode Access mode.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Open
(
   I2cDrv_I2cBusId                          eI2cBusId   ,
   I2cDrv_I2cBus                           *pI2cBus     ,
   uint32                                   dwaccessMode
);

/**
    This function is the main device close  function.

    @param[in] pI2cBus       Pointer to I2C bus struct.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Close
(
   I2cDrv_I2cBus                           *pI2cBus
);

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] pTransfer       Pointer where transfer data will
          be stored.
    @param[in] pClntCfg        Pointer to Client configuration.
                                If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          reads.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Read
(
   I2cDrv_I2cBus                        *pI2cBus,
   I2cTransfer                          *pTransfer,
   uint32                               *puNumCompleted
); 

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_Write
(
   I2cDrv_I2cBus             *pI2cBus,
   I2cTransfer               *pTransfer,
   uint32                    *puNumCompleted
);

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] pI2cBus        Pointer to I2C bus struct.
    @param[in] pSequence      Pointer to a sequence of transfer
          data.
    @param[in] pIoRes         Pointer to io result.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_BatchTransfer
(
   I2cDrv_I2cBus                       *pI2cBus,
   I2cSequence                         *pSequence,
   I2cIoResult                         *pIoRes
);

/** @brief Send a command to the i2c driver.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] cmd             command to send to the driver.
    @param[in] pData           data related to the command.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_Cmd
(
   I2cDrv_I2cBus             *pI2cBus,
   I2cDrvCmdType             cmd,
   void                      *pData
);




#endif /* _I2CDRIVER_H_ */
