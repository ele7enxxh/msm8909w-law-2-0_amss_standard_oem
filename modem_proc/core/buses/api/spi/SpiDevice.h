#ifndef _SPIDEVICE_H_
#define _SPIDEVICE_H_
/*
===========================================================================

FILE:   SpiDevice.h

DESCRIPTION:
    This file contains the API for the BusSPIQupDevice 
    Interface: ISpiDeviceDevice 

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/api/spi/SpiDevice.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/11/12 dk     Added review comments.
03/27/12 ag     Added batch transfer updates.
09/26/11 ag     Created

===========================================================================
        Copyright c 2011-2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"
#include "spi_defs.h"

#define SPIDEVICE_DEFAULT_BUS_FREQ_KHZ               10000
#define SPIDEVICE_DEFAULT_BYTE_TRANSFER_TIMEOUT_US   200

typedef struct SpiBufferType SpiBufferType;
struct SpiBufferType{
    SpiDataAddrType *pData;
   uint32 uLen;
};
/*SpiLinkedTransferType is only used for Batch transfer*/
typedef struct SpiLinkedTransferType SpiLinkedTransferType;
struct SpiLinkedTransferType {
   SpiBufferType *pReadBuffVec;
   uint32 uNumReadVecs;
   SpiBufferType *pWriteBuffVec;
   uint32 uNumWriteVecs;
   uint32 uTotalReadLen;
   uint32 uTotalWriteLen;
    SpiDeviceInfoType *pQupConfig; /*If this is null the one provided in batch transfer API
                            will be used*/
};
typedef struct SpiDevice_AsyncResult SpiDevice_AsyncResult;
struct SpiDevice_AsyncResult{
   int32 nTransferStatus;
   void  *pArg;
};

typedef void (*SPI_DEV_ASYNC_CB)(SpiDevice_AsyncResult *pResult);

typedef void*     SPIDEVICE_HANDLE;
typedef void*     SpiPlatBuffer;


typedef enum SpiDevice_PowerStates
{
   SPIDEVICE_POWER_STATE_0,/**< Bus and clocks off. */
   SPIDEVICE_POWER_STATE_1,/**< Bus on clocks off. */
   SPIDEVICE_POWER_STATE_2,/**< Bus on clocks on. */
} SpiDevice_PowerStates;


/** @brief Initializes the device.

    This Function Initializes the device and creates the
    necessary data structures to support other calls into the
    device.

    @param[in] uDevId    Device index.
    @param[in] phDev     Pointer to device handle.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_Init
(
   uint32                               uDevId,
   SPIDEVICE_HANDLE                     *phDev 
);

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.

    @param[in] hDev      Device handle.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_DeInit
(
   SPIDEVICE_HANDLE                        hDev 
);

/** @brief Set client Configuration for the subsequent 
           transfers.

    @param[in] hDev      Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_Configure
(
   SPIDEVICE_HANDLE                        hDev,
   SpiDeviceInfoType                       *pQupConfig
);

/** @brief Peform a bi-direction transfer

    Read a buffer from SPI device and/or Write a buffer of 
   data to a SPI device

    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
                                           Client function can set it to NULL 
                                           if it need to use the settings used 
                                           by previous call with the same 
                                           device context.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_WriteRead
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen
); 

/** @brief Read a buffer from spi device.

    Read a buffer from spi device.
    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_Read
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen
); 

/** @brief Write a buffer to spi device.

    Write a buffer to spi device.
    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */

int32
SpiDevice_Write
(
   SPIDEVICE_HANDLE                          hDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen
);

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev              Device handle.
    @param[in] *pQupConfig    Pointer to the QUP config for data transfer.
    @param[in] hTransfer      Transfer handle.
    @param[out] puNumCompleted  Pointer to return number of completed transfers.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
*/
int32
SpiDevice_BatchTransfer
(
   SPIDEVICE_HANDLE                          hDev,
   SpiLinkedTransferType                     *pTransfer,
   uint32                                    uNumTransferVecs,
   SpiDeviceInfoType                         *pQupConfig,
   uint32                                    *puNumCompleted
);


/** @brief Peform a bi-direction transfer

    Schedules a read/write/read-write(bi-directional) transfer.
    Once the transfer is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.
    @return          int32 .
  */
void
SpiDevice_AsyncWriteRead
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
); 
/** @brief Schedules a buffer read from spi device.

    Schedules a buffer read from spi device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   Nothing.
  */
void
SpiDevice_AsyncRead
(
   SPIDEVICE_HANDLE                          hDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
); 

/** @brief Schedules a buffer write to spi device.

    Schedules a buffer write to spi device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   Nothing.
  */
void
SpiDevice_AsyncWrite
(
   SPIDEVICE_HANDLE                          hDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
);



/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by doing necessary operations related to 
    hw core.

    @param[in] hDev           Device handle.

    @return                   Nothing.
  */
void
SpiDevice_CancelTransfer
(
   SPIDEVICE_HANDLE                        hDev
);

/** @brief Set the power state of the device.

    This Function sets the power state for the device.

    @param[in] hDev           Device handle.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_SetPowerState
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDevice_PowerStates                   ePowerState
);

/** @brief Gets the power state of the device.

    This Function gets the power state of the device.

    @param[in] hDev           Device handle.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_GetPowerState
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDevice_PowerStates                   *pePowerState
);
/** @brief Gets the HW device information.


    @param[in] hDev           Device handle.
    @param[out] hDev           spiInfo.

    @return int32 - SPI_DEVICE_SUCCESS on success, error code on error.
  */
int32
SpiDevice_GetHwInfo
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiInfoType                             *spiInfo
);
#endif /* _SPIDEVICE_H_ */
