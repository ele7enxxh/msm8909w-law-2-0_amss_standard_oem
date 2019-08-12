#ifndef __DDII2C_H__
#define __DDII2C_H__

/**
  @file ddii2c.h
  @brief This file contains the interface declaration for I2C DAL device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The I2C_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the I2C_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "ddii2c" group 
      description in the I2C_API_.dox file. 
===========================================================================*/
/*
Copyright (c) 2008-2011 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
*/
/*===========================================================================
        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/i2c/ddii2c.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
05/10/11 llg    (Tech Pubs) Edited/added Doxygen comments and markup.
10/21/10 AL     Added Doxygen markup
02/02/10 UR     Changed Bus Acquire Wait Time resolution to milliseconds
                and set the default to 1 second
01/30/09 PS     Manually edited the file to remove Dummy parameters from 
                exported APIs 
12/08/08 PS     Added comments to API
06/29/08 PS     Created

===========================================================================*/

#include "DalDevice.h"
#include "I2cTransfer.h"
#include "I2cError.h"

/** @addtogroup ddii2c
@{ */

/** Defines the interface version; format is (major,minor). */
#define DALI2C_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/** Default byte transfer timeout time. A value of -1 is interpreted as 
  a standard default time predetermined by the driver. This timeout determines 
  how long to wait for a transaction to complete before timing out.
*/
#define I2C_DEFAULT_WAIT_TIME -1
/** Default bus acquisition wait time. If there is bus contention, this
  wait time determines how long to wait to acquire the bus before timing 
  out with I2C_ERROR_BUS_ACQUISITION_TIMEOUT.
*/
#define I2C_DEFAULT_BUS_ACQUIRE_WAIT_TIME 1000

typedef void (*DDIDRV_ASYNC_CB)(void * pArg);

/** Contains the function pointer table. */
typedef struct DalI2C DalI2C;
/** @brief Structure that contains the function pointer table. */
struct DalI2C
{
   DalDevice DalDevice;
   /**< Used as a function pointer table to common DAL functions such as 
        Attach, Open, Close, etc. */
   DALResult (*Read)(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, uint32 *puNumCompleted);
   /**< Reads the required number of bytes from an I2C slave device. */
   DALResult (*Write)(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, uint32 *puNumCompleted);
   /**< Writes the required number of bytes to an I2C slave device. */
   DALResult (*BatchTransfer)(DalDeviceHandle * _h, I2cSequence *pSequence, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes);
   /**< performs a batch of read/writes number of bytes, to/from an I2C slave device. */
   DALResult (*AsyncRead)(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg);
   /**< Reads the required number of bytes from an I2C slave device. */
   DALResult (*AsyncWrite)(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg);
   /**< Writes the required number of bytes to an I2C slave device. */
   DALResult (*AsyncBatchTransfer)(DalDeviceHandle * _h, I2cSequence *pSequence, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg);
   /**< performs a batch of read/writes number of bytes, to/from an I2C slave device. */
};

/** Used as the DAL device handle. */
typedef struct DalI2CHandle DalI2CHandle; 
/** @brief Structure that is used as the DAL device handle. */
struct DalI2CHandle 
{
   uint32 dwDalHandleId;   /**< Handle ID. */
   const DalI2C * pVtbl;   /**< Pointer to the function pointer table. */
   void *pClientCtxt;     /**< Pointer to the client context handle. */
};

/** @} */ /* end_addtogroup ddii2c */
/** @addtogroup ddii2c_configuration
@{ */

/** DAL_I2CDeviceAttach is a wrapper function around DAL_DeviceAttachEx.

  @param[in] DevId       DalDevice ID of the I2C device to which the client
                         intends to attach.
  @param[in] hDalDevice  Pointer location to be filled by the driver with a
                         handle to DalDevice.

  @return
  I2cOperationStatusType; for details, 
  see Section @latexonly\ref{opstatus}@endlatexonly.

  @dependencies
  None.
*/
#define DAL_I2CDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALI2C_INTERFACE_VERSION,hDalDevice)

/** @} */ /* end_addtogroup ddii2c_configuration */
/** @addtogroup ddii2c_data_transfer 
@{ */

/*===========================================================================
FUNCTION I2C_Read
===========================================================================*/
/** Reads the required number of bytes from an I2C slave device.
  To set the slave device address from which to read, DalI2C_Configure() 
  must be called once before calling DalI2C_Read(). 

  The read is aborted if the slave device does not acknowledge control/address 
  bytes written to it (before the data read starts). A read operation is 
  always terminated with a STOP condition.

  @param[in]  _h             Pointer to the client context handle.
  @param[in]  uReadStartAddr Memory/register address to be written to the 
                             slave device before it starts transmitting the 
                             bytes to be read by the I2C controller. If the 
                             slave device is an I2C_DEFAULT_ADDRESS_DEVICE 
                             type, this address is ignored and the I2C 
                             controller directly starts reading from the slave
                             device without any address byte writes.
  @param[out] pReadBuffer    Pointer to the read buffer that holds the data 
                             read from the slave device.
  @param[in]  uReadBufferLen Number of bytes to be read from the slave device.
  @param[out] puTotalBytesRead Pointer to the number of bytes actually read 
                               from the slave device.

  @return
  I2cOperationStatusType; for details, 
  see Section @latexonly\ref{opstatus}@endlatexonly.

  @dependencies 
  DalI2C_Configure() must have been called at least once by this client.

  @sa
  DalI2C_Write(), DalI2C_Configure()
*/
static __inline DALResult
DalI2C_Read(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, uint32 *puNumCompleted)
{
   return ((DalI2CHandle *)_h)->pVtbl->Read( _h, pTransfer, pClntCfg, puNumCompleted);
}

/*===========================================================================
FUNCTION I2C_Write
===========================================================================*/
/** Writes the required number of bytes to an I2C slave device.
  To set the slave device address to which to write, DalI2C_Configure() 
  must be called once before calling DalI2C_Write().

  DalI2C_Write() supports a zero-byte write. This is used to probe the presence
  of the device.

  The write is aborted if the slave device does not acknowledge control/address
  bytes written to it (before the data write starts). A write operation is 
  always terminated with a STOP condition.
  
  @param[in]  _h              Pointer to the client context handle.
  @param[in]  uWriteStartAddr Memory/register address to be written to the 
                              slave device before writing the actual write 
                              bytes. If the slave device is an 
                              I2C_DEFAULT_ADDRESS_DEVICE type, this address
                              is ignored and the I2C controller directly 
                              starts writing the write bytes to the slave 
                              device without any address byte writes.
  @param[in]  pWriteBuffer    Pointer to the write buffer that contains the 
                              data bytes to be written to the slave device.
  @param[in]  uWriteBufferLen Number of bytes to write to the slave device.
  @param[out] puTotalBytesWritten Pointer to the number of bytes actually 
                                  written to the slave device.

  @return
  I2cOperationStatusType; for details, 
  see Section @latexonly\ref{opstatus}@endlatexonly.
  
  @dependencies 
  DalI2C_Configure() must have been called at least once by this client.

  @sa
  DalI2C_Read(), DalI2C_Configure()
*/
static __inline DALResult
DalI2C_Write(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, uint32 *puNumCompleted)
{
   return ((DalI2CHandle *)_h)->pVtbl->Write( _h, pTransfer, pClntCfg, puNumCompleted);
}

/*===========================================================================
FUNCTION I2C_CombinedWriteRead
===========================================================================*/
/** Writes the required number of bytes, followed by a read of the required
  number of bytes, to/from an I2C slave device. No STOP condition is generated 
  between write and read. To set the slave device address to/from which to 
  write/read, DalI2C_Configure() must be called once before calling 
  DalI2C_CombinedWriteRead(). 

  @param[in]  _h                  Pointer to the client context handle.
  @param[in]  pWriteReadStartAddr Pointer to the I2cWriteReadStartAddr 
                        structure type: 
                        - uWriteStartAddr -- Memory/register address to be 
                          written to the slave device before writing the 
                          actual write bytes. If the slave device is an 
                          I2C_DEFAULT_ADDRESS_DEVICE type, this address is 
                          ignored and the I2C controller directly starts 
                          writing the write bytes to the slave device without
                          any address byte writes. \n
                        - uReadStartAddr -- Memory/register address to be 
                          written to the slave device before the slave device
                          starts transmitting the bytes to be read by the I2C
                          controller. If the slave device is an 
                          I2C_DEFAULT_ADDRESS_DEVICE type, this address is 
                          ignored and the I2C controller directly starts 
                          reading from the slave device without any address 
                          byte writes.
  @param[in]  pWriteBuffer     Pointer to the write buffer that contains the 
                               data bytes to be written to the slave device.
  @param[in]  uWriteBufferLen  Number of bytes to write to the slave device.
  @param[out] pReadBuffer      Pointer to the read buffer that holds the data
                               read from the slave device.
  @param[in]  uReadBufferLen   Number of bytes to be read from the slave 
                               device.
  @param[out] puTotalBytesRead Pointer to the number of bytes actually read 
                               from the slave device.
  @param[out] puTotalBytesWritten Pointer to the number of bytes actually 
                                  written to the slave device.

  @return
  I2cOperationStatusType; for details, 
  see Section @latexonly\ref{opstatus}@endlatexonly.

  @dependencies
  DalI2C_Configure() must have been called at least once by this client.

  @sa
  DalI2C_Configure()
*/
static __inline DALResult
DalI2C_BatchTransfer(DalDeviceHandle * _h, I2cSequence *pSequence, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes)
{
   return ((DalI2CHandle *)_h)->pVtbl->BatchTransfer( _h, pSequence, pClntCfg, pIoRes);
}

static __inline DALResult
DalI2C_AsyncRead(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg)
{
   return ((DalI2CHandle *)_h)->pVtbl->AsyncRead( _h, pTransfer, pClntCfg, pIoRes,pCallbackFn,pArg);
}

static __inline DALResult
DalI2C_AsyncWrite(DalDeviceHandle * _h, I2cTransfer *pTransfer, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg)
{
   return ((DalI2CHandle *)_h)->pVtbl->AsyncWrite( _h, pTransfer, pClntCfg, pIoRes,pCallbackFn,pArg);
}

static __inline DALResult
DalI2C_AsyncBatchTransfer(DalDeviceHandle * _h, I2cSequence *pSequence, I2cClientConfig *pClntCfg, I2cIoResult *pIoRes,
         DDIDRV_ASYNC_CB pCallbackFn, void *pArg)
{
   return ((DalI2CHandle *)_h)->pVtbl->AsyncBatchTransfer( _h, pSequence, pClntCfg, pIoRes, pCallbackFn, pArg);
}

/** @} */ /* end_addtogroup ddii2c_data_transfer */
/** @addtogroup ddii2c_configuration
@{ */


/** @} */ /* end_addtogroup ddii2c_configuration */

#endif /* __DDII2C_H__ */
