#ifndef SPIDEVICEPLATBAM_H
#define SPIDEVICEPLATBAM_H
/*
===========================================================================

FILE:         SpiDevicePlatBam.h

DESCRIPTION:  This file contains the function prototypes for SpiDevice
              platform driver BAM functionality.

===========================================================================

                             Edit History

$Header: 

When           Who    What, where, why
--------      ---    -----------------------------------------------------------
03/16/12      ddk    Added de-init API .
02/09/12      ag     Initial creation.

===========================================================================
             Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SpiDeviceTypes.h"

typedef struct SpiDevicePlatBamResult SpiDevicePlatBamResult;
struct SpiDevicePlatBamResult{
	void   *pUser;
	uint32 result;
};

typedef void (*SPIDEVICE_PLATBAM_CB)(SpiDevicePlatBamResult *pRes);

/*BIT mask Status portion of the descriptors*/
#define PLAT_BAM_DESC_FLAG_INT  0x8000  /* Generate interrupt */
#define PLAT_BAM_DESC_FLAG_EOT  0x4000  /* Generate end-of-transfer indication */
#define PLAT_BAM_DESC_FLAG_EOB  0x2000  /* Generate end-of-block indication */
#define PLAT_BAM_DESC_FLAG_NWD  0x1000  /* Generate Notify when done indication */
#define PLAT_BAM_DESC_FLAG_CMD  0x0800  /* Mark as command descriptors*/
#define PLAT_BAM_DESC_FLAG_NO_SUBMIT  0x0080

typedef enum SpiDevBamDir SpiDevBamDir;
enum SpiDevBamDir {
	SPIDEVICE_BAM_OUT,
	SPIDEVICE_BAM_IN,
};

typedef enum IBUSBAM_Result
{
   IBUSBAM_RESULT_OK = 0,
   IBUSBAM_INIT_ERROR = 1,
   IBUSBAM_MEM_ALLOCATE_ERROR,
   IBUSBAM_DEINIT_ERROR,
   IBUSBAM_NULLPTR_ERROR,
   IBUSBAM_RESET_ERROR,
   IBUSBAM_TRANSFER_REQUEST_ERROR,
   IBUSBAM_BAM_TRANSACTION_ERROR,
   IBUSBAM_CMD_ELEMENT_CREATION_FAILED,
} IBUSBAM_Result;

/**
 * @brief Init platform BAM driver
 *
 * This function attaches to the BAM driver
 * 
 * @param[in] hPlat   Handle to target Platform.
 * @param[out] hPlatBam Pointer to the location to store the Plat
 * 			   BAM handle
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error
 */
int32 SpiDevice_InitPlatBam(IBUSPLATCFG_HANDLE hPlat, 
      SPIDEVICE_PLATBAM_HANDLE *hPlatBam,
      SPIDEVICE_PLATBAM_CB pCallBackFn,
      void *pArg);

/**
 * @brief Detach from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in] hPlat   Handle to target Platform.
 * @param[in] hBAM  BAM driver handle
 * 
 * @return SPI_DEVICE_SUCCESS on success, error code on error
 */
int32 SpiDevice_DeInitPlatBam
(
   IBUSPLATCFG_HANDLE hPlat, 
   SPIDEVICE_PLATBAM_HANDLE hBAM
);

/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 * @param[in] hBAM  BAM device handle
 * 
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error 
 */
int32 SpiDevice_ResetPlatBam(SPIDEVICE_PLATBAM_HANDLE hBAM);

/**
 * @brief Schedule a single BAM transfer 
 *
 * This function schedules a single BAM transfer.
 * 
 * @param hPlatBam Platform BAM handle
 * @param uDir  Producer or Consumer transfer
 * @param uAddr  Physical start address for the transfer
 * @param uSize  Size of the transfer in bytes
 * @param pUser  User-defined pointer associated with the 
 *               transfer
 * @param uFlags  Flags for the transfer
 * 
 * @return SPI_DEVICE_SUCCESS  on sucess, error code on error
 */
uint32 SpiDevice_PlatBamTransfer(SPIDEVICE_PLATBAM_HANDLE hPlatBam,  
			uint32 uDir, uint32 uAddr, uint32 uSize, void *pUser, uint32 uFlags);

/**
 * @brief Find Available free space in a BAM pipe
 *
 * This function schedules a single BAM transfer.
 * 
 * @param hPlatBam Platform BAM handle
 * @param uDir  Producer or Consumer transfer
 * @param count  Available free space for descriptors on the 
 *                       platform pipe.
 * 
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error 
 */
uint32 SpiDevice_PlatBamGetPipeFreeSpace
(
   SPIDEVICE_PLATBAM_HANDLE hPlatBam,
   uint32 uDir,
   uint32 *count
);

#endif /* SPIDEVICEBAM_H */

