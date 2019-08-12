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
04/25/16 dpk    Added BAM Pipe deinit.
05/05/13 ag    Created

===========================================================================
        Copyright c 2013-16 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceTypes.h"
#include "SpiDeviceTransfer.h"
#include "HALqupe.h"
#include "bam.h"

#define RESET_CMD_BUFFER_SIZE 16
#define SETTINGS_CMD_BUFFER_SIZE 512

typedef struct SpiDeviceBamCfg_s
{
   uint32      uBamBlspId;
   uint32      uBamBaseAddr;
   uint32      uBamInterruptId;
   uint32      uBamTxPipeNum;
   uint32      uBamRxPipeNum;
} SpiDeviceBamCfg_t;

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


typedef struct SpiDeviceBamPipeMemInfo{
   void    *physMemHandle;
   uint32  physAddr;
   uint32  virtualAddr;
}SpiDeviceBamPipeMemInfo;   

typedef SpiDeviceBamResult *(*SPIDEVICE_BAM_TRANSFER_POLL)(SpiDeviceBamType *spiBam);

struct  SpiDeviceBamType{
   SpiDeviceTransferType *transfer;
   bam_handle  bamHandle;
   bam_handle  rxPipeHandle;
   bam_handle  txPipeHandle;
   bam_result_type     pipeResult;
   SpiDeviceBamResult result;
   SpiDeviceBamPipeMemInfo rxPipeMem;
   SpiDeviceBamPipeMemInfo txPipeMem;

   boolean eot;
   int32 (*startBamTransfer)(SpiDeviceBamType *spiBam, SpiDeviceTransferType *transfer, void *pUser, boolean eot);
   void (*cancelBamTransfer) (SpiDeviceBamType *spiBam, SpiDeviceTransferType *transfer);
   SpiDeviceBamResult *(*pollBamTransfer)(SpiDeviceBamType *spiBam);
};

int32 SpiDeviceBamInit
(
   SpiDeviceBamType **spiBam, 
   SPIDEVICE_PLATCFG_HANDLE	hPlat
);

int32 SpiDeviceBamDeInit
(
   SpiDeviceBamType **spiBam
);

#endif
