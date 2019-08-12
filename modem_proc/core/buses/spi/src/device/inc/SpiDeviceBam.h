#ifndef _SPIDEVICEBAM_H_
#define _SPIDEVICEBAM_H_

/*
===========================================================================

FILE:   SpiDeviceBam.h

DESCRIPTION:
    This file contains the definitions for the SPI BAM mode operation.

===========================================================================

        Edit History

$Header: 

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
03/28/12 ddk   Added de-init API.
02/26/12 ag    Created

===========================================================================
        Copyright c 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceTypes.h"
#include "SpiDeviceTransfer.h"
#include "HALqupe.h"

#define RESET_CMD_BUFFER_SIZE 16
#define SETTINGS_CMD_BUFFER_SIZE 512

typedef enum SpiDeviceBamTags SpiDeviceBamTags;
enum SpiDeviceBamTags {
	SPIDEVICE_BAM_READ_TAG,
	SPIDEVICE_BAM_WRITE_TAG,
	SPIDEVICE_BAM_CMD_TAG,
};

typedef void* SpiDeviceBamCmdParms;

typedef struct SpiDeviceBamResult SpiDeviceBamResult;
struct SpiDeviceBamResult{
	void   *pUser;
	uint32 result;
};
typedef void (*SPIDEVICE_BAM_TRANSFER_CB)(SpiDeviceBamResult *pResult);

typedef struct SpiDeviceBamType SpiDeviceBamType;

typedef struct SpiBamPlatCBArgs SpiBamPlatCBArgs;

struct SpiBamPlatCBArgs{
	uint32 bamTags;
	SpiDeviceBamType *spiBamObj;
};


typedef struct SpiDeviceCmdDescInfo{
   void    *physMemHandle;
   uint32  physAddr;
   uint32  virtualAddr;
   uint32  cmd_element_index;
   uint32  size_in_bytes;
}SpiDeviceCmdDescInfo;   

struct  SpiDeviceBamType{
   SpiDeviceTransferType *transfer;
   SPIDEVICE_BAM_TRANSFER_CB pCallbackFn;
   IBUSPLATCFG_HANDLE hPlat;
   SPIDEVICE_PLATBAM_HANDLE hPlatBam;
   HAL_QUPE_SPI_TransferParametersType mHalTransferParameters;
   HAL_QUPE_SPI_DeviceParametersType mHalDeviceParameters;
   SpiDeviceBamResult result;
   SpiDeviceBamCmdParms *cmdParms;
   SpiBamPlatCBArgs readArgs;
   SpiBamPlatCBArgs writeArgs;
   SpiDeviceCmdDescInfo resetCmdDesc;
   SpiDeviceCmdDescInfo settingsCmdDesc;
   boolean eot;
   int32 (*startBamTransfer)(SpiDeviceBamType *spiBam, SpiDeviceTransferType *transfer, void *pUser, boolean eot);
   void (*cancelBamTransfer) (SpiDeviceBamType *spiBam, SpiDeviceTransferType *transfer);
   void (*setBamTransferCallback) (SpiDeviceBamType *spiBam, SPIDEVICE_BAM_TRANSFER_CB pCallBackFn);
   void (*setBamTransferCmdParms) (SpiDeviceBamType *spiBam, SpiDeviceBamCmdParms *cmdParms);
   void (*setQupTransferParms) (SpiDeviceBamType *spiBam, HAL_QUPE_SPI_TransferParametersType *mHalTransferParameters);
   void (*setQupDeviceParms) (SpiDeviceBamType *spiBam, HAL_QUPE_SPI_DeviceParametersType *mHalDeviceParameters);

};

int32 SpiDeviceBamInit
(
   SpiDeviceBamType **spiBam, 
   IBUSPLATCFG_HANDLE	hPlat
);

int32 SpiDeviceBamDeInit
(
   SpiDeviceBamType *spiBam, 
   IBUSPLATCFG_HANDLE	hPlat
);

#endif
