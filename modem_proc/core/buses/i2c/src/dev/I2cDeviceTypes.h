#ifndef _I2CDEVICETYPES_H_
#define _I2CDEVICETYPES_H_
/*
===========================================================================

FILE:   I2cDeviceTypes.h

DESCRIPTION:
    This file contains the types for the I2CDevice 
    Interface: II2cDevice 

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/dev/I2cDeviceTypes.h#1 $

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

#include "I2cTransfer.h"
#include "I2cError.h"
#include "QupRegs.h"
#include "QupRegFieldTypes.h"
#include "I2cPlatSvc.h"

#define I2CDEV_HW_EVT_QUE_MAX       (4)
#define I2CDEV_DEFAULT_BUS_FREQ_KHZ (400)

/*-------------------------------------------------------------------------
* Type Declarations
* ----------------------------------------------------------------------*/

enum  I2cDev_Error
{
   I2CDEV_RES_SUCCESS   =      I2C_RES_SUCCESS,
   I2CDEV_ERR_BASE = I2C_RES_ERROR_CLS_QUP_DEV,
   // Under device class the first byte is reserved to qup errors
   I2CDEV_ERR_INVALID_DEVICE_ID = 0x30000100,
   I2CDEV_ERR_IN_SEQ_OUT_OF_SYNC            ,
   I2CDEV_ERR_INVALID_RUNSTATE              ,
   I2CDEV_ERR_RUNSTATE_CHANGE_TIMEOUT       ,
   I2CDEV_ERR_NEW_RUNSTATE_INVALID          ,
   I2CDEV_ERR_INVALID_POWER_STATE           ,
   I2CDEV_ERR_BUS_BUSY                      ,
   I2CDEV_ERR_BUS_NOT_RELEASED              ,
   I2CDEV_ERR_INPUT_DONE_TRANSFERS_NOT      ,

   I2CDEV_ERR_INVALID_DEV_HANDLE            ,
   I2CDEV_ERR_INVALID_TRANSFER_HANDLE       ,
   I2CDEV_ERR_INVALID_TRANSFER_OBJPTR       ,
   I2CDEV_ERR_INVALID_SEQUENCE_HANDLE       ,
   I2CDEV_ERR_INVALID_SEQUENCE_OBJPTR       ,
   I2CDEV_ERR_INVALID_NUMCOMPLETE_POINTER   ,
   I2CDEV_ERR_INVALID_TRAN_RESULT_POINTER   ,
   I2CDEV_ERR_INVALID_CALLBACK_FN_POINTER   ,
   I2CDEV_ERR_INVALID_CALLBACK_ARG_POINTER  ,
   I2CDEV_ERR_INVALID_TRANSFER_DIRECTION    ,
   I2CDEV_ERR_INPUT_DESC_BUFFER_ALLOC_FAILED,
   I2CDEV_ERR_API_NOT_SUPPORTED             ,
   I2CDEV_ERR_INVALID_RUN_STATE             ,
   I2CDEV_ERR_INVALID_OUTPUT_BYTE_CNT       ,
   I2CDEV_ERR_INVALID_INPUT_BYTE_CNT        ,
   I2CDEV_ERR_INVALID_I2C_CLOCK             ,
   I2CDEV_ERR_INVALID_I2C_FS_DIVIDER        ,
   

   I2CDEV_ERR_BAM_ERROR    ,

};

typedef enum I2cDev_InitState
{
   I2cDev_InitState_Uninitialized             = 0x0,
   I2cDev_InitState_DeviceInit_Done           = 0x1,
   I2cDev_InitState_HardwareInit_Done         = 0x2,
   
} I2cDev_InitState;

typedef enum I2CDEVQUP_TrCfgValInd
{
   I2CDEVQUP_NumOutBytes_DtIdx             = 0x0,
   I2CDEVQUP_NumInBytes_DtIdx              = 0x1,
   I2CDEVQUP_NumPrepadBytes_DtIdx          = 0x2,
   I2CDEVQUP_Unused_DtIdx                  = 0x3,
   
} I2CDEVQUP_TrCfgValInd;


typedef volatile struct I2cDev_QupIsrHwEvt
{
   uint32                              uIrqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   uint32                              uOutTranCnt;
   uint32                              uInTranCnt;
   uint32                              i2cStatus;

} I2cDev_QupIsrHwEvt;

#define I2CDEV_QUPISRHWEVT_INIT {0,0,0,0,0,0}

typedef volatile struct I2cDev_QupBamHwEvt
{
   uint32                              uBamEvt;
} I2cDev_QupBamHwEvt;

#define I2CDEV_QUPBAMHWEVT_INIT   {0}


typedef volatile struct I2cDev_QupHwEvtQueue
{
   I2cDev_QupIsrHwEvt      aHwEvt[I2CDEV_HW_EVT_QUE_MAX];
   I2cDev_QupIsrHwEvt     *pHead;
   I2cDev_QupIsrHwEvt     *pTail;

} I2cDev_QupHwEvtQueue;

#define I2CDEV_QUPHWEVTQUEUE_INIT           \
{                                           \
  {I2CDEV_QUPISRHWEVT_INIT,                 \
  I2CDEV_QUPISRHWEVT_INIT,  				     \
  I2CDEV_QUPISRHWEVT_INIT,  				     \
  I2CDEV_QUPISRHWEVT_INIT},                 \
  NULL, NULL,                               \
}                                           \

typedef volatile struct I2cDev_QupInfo
{
   /* Register access */
   QupRegsType                        *pQupRegs;

   uint32                              uIrqCnt;
   I2cDev_QupHwEvtQueue                hwEvtQueue;
   //I2CPLATBAM_EventType                eBamEvt;
   uint32                              uInFreeSlotReqCnt;
   uint32                              uOutFreeSlotReqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   uint32                              uInFreeSlotServicedCnt;
   uint32                              uOutFreeSlotServicedCnt;
   uint32                              uCurOutTransfers;
   uint32                              uCurInTransfers;
   uint32                              i2cStatus;
   QUP_IoModeType                     eOutFifoMode;
   QUP_IoModeType                     eInFifoMode;
} I2cDev_QupInfo;

#define I2CDEV_QUPINFO_INIT {NULL,NULL,0,I2CDEV_QUPHWEVTQUEUE_INIT,0,0,0,0,0,0,0,0,0,0,0}


typedef enum I2cDev_TransferState
{
   I2CDEV_TR_STATE_PAD_OUTPUT,
   I2CDEV_TR_STATE_TIME_STAMP_START,
   I2CDEV_TR_STATE_TIME_STAMP_VALUE,
   
   I2CDEV_TR_STATE_MASTER_START_TAG,
   I2CDEV_TR_STATE_MASTER_ADDR,
   
   I2CDEV_TR_STATE_START_TAG,
   I2CDEV_TR_STATE_SLAVE_ADDR,
   I2CDEV_TR_STATE_RD_DATA_TAG,
   I2CDEV_TR_STATE_WR_DATA_TAG,
   I2CDEV_TR_STATE_DATA_CNT,
   I2CDEV_TR_STATE_EOT_TAG,
   I2CDEV_TR_STATE_DATA_VALUE,
   I2CDEV_TR_STATE_DATA_PAD,
   
   I2CDEV_TR_STATE_NOT_IN_TRANSFER,

   I2CDEV_TR_STATE_EXTEND  = 0x7FFFFFFF
} I2cDev_TransferState;

typedef enum I2cDev_OutputState
{
   I2CDEV_OUTPUT_IDDLE            ,
   I2CDEV_OUTPUT_STALLED          ,
   I2CDEV_OUTPUT_FILL_FIFO        ,
   I2CDEV_OUTPUT_FILL_FIFO_DONE   ,
   I2CDEV_OUTPUT_TRANSMIT_DONE    ,
   I2CDEV_OUTPUT_ERROR            ,
} I2cDev_OutputState;

typedef struct I2cDev_OutTransferInfo
{
      I2cDev_TransferState             eOutTrState;
      uint32                           uDataIndex;
      uint32                           uNextDataSegIndex;
      I2cIoVec                        *pIoVec;
      uint32                           IoVecBuffIndex;
      uint32                           uFieldByteCnt;
} I2cDev_OutTransferInfo;

#define I2CDEV_OUTTRANSFERINFO_INIT {I2CDEV_TR_STATE_NOT_IN_TRANSFER,0,0,NULL,0,0}


typedef struct I2cDev_SequenceIoInfo I2cDev_SequenceIoInfo;

typedef struct I2cDev_OutSeqInfo
{
   I2cDev_OutputState                  eOutputState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalOutBytes; //data + format
   uint32                              uCurOutBytes; //data + format
   uint32                              uNumOutDtBytes;//data cnt
   uint32                              uOutTrCnt;
   uint32                              uTrIndex;
 
   I2cDev_OutTransferInfo              outTr;
   I2cDev_SequenceIoInfo              *pSeq;
} I2cDev_OutSeqInfo;

#define I2CDEV_QUTSEQINFO_INIT                  \
{                                               \
   I2CDEV_OUTPUT_IDDLE,                         \
   NULL,                                        \
   0,0,0,0,0,                                   \
   I2CDEV_OUTTRANSFERINFO_INIT,                 \
   NULL                                         \
}                                               \


typedef enum I2cDev_InputState
{
   I2CDEV_INPUT_IDDLE,
   I2CDEV_INPUT_STALLED,
   I2CDEV_INPUT_MEMORY_FLUSH,
   I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO,
   I2CDEV_INPUT_PUSH_DESCRIPTORS,
   I2CDEV_INPUT_WAIT_READ_DATA_READY,
   I2CDEV_INPUT_RECEIVE_DONE,
   I2CDEV_INPUT_ERROR
} I2cDev_InputState;


typedef struct I2cDev_InTransferInfo
{
   I2cDev_TransferState             eInTrState;
   uint32                           uDataIndex; //transfer data index
   uint32                           uNextDataSegIndex;
      //uint32                           uTimeStamp; // for now do nothing with this.
   I2cIoVec                        *pIoVec;
   uint8                           *pIoVecBuff;
   uint32                           IoVecBuffIndex;
   uint32                           uFieldByteCnt;
} I2cDev_InTransferInfo;

#define I2CDEV_INTRANSFERINFO_INIT {I2CDEV_TR_STATE_NOT_IN_TRANSFER, 0,0,NULL,NULL,0,0}


typedef struct  I2cDev_InSeqInfo
{
   I2cDev_InputState                   eInSeqState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalInBytes;//data + format
   uint32                              uNumInDtBytes;//data cnt
   uint32                              uInPadCnt;
   uint32                              uInTrCnt;
   uint32                              uTrIndex;
   uint32                              uLastTrIndex;
   uint32                              uNumInBytesReceived;
   I2cDev_InTransferInfo               inTr;
   I2cDev_SequenceIoInfo              *pSeq;
} I2cDev_InSeqInfo;

#define I2CDEV_INSEQINFO_INIT               \
{                                           \
   I2CDEV_INPUT_IDDLE,                      \
   NULL,                                    \
   0,0,0,0,0,0,0,                           \
   I2CDEV_INTRANSFERINFO_INIT,              \
   NULL                                     \
}


typedef enum I2cDev_SequenceIoState
{
   I2CDEV_SEQSTATE_IDDLE,
   I2CDEV_SEQSTATE_INPUT_OUTPUT,
   I2CDEV_SEQSTATE_COMPLETING,   
   I2CDEV_SEQSTATE_COMPLETE,
   I2CDEV_SEQSTATE_QUP_STATE_ERROR,
} I2cDev_SequenceIoState;

typedef struct I2cDev_Device I2cDev_Device;

struct I2cDev_SequenceIoInfo
{
   I2cTransfer                     *pTransfer;
   I2cDev_Device                   *pDev;
   uint32                           uNumTransfers;
   uint32                           uSeqTimeoutUs;
   I2cDev_OutSeqInfo                outSeqInfo;
   I2cDev_InSeqInfo                 inSeqInfo;
   boolean                          bBamTransfer;
   boolean                          bHsTransfer;
   I2cDev_SequenceIoState           eSeqState;
   I2cIoResult                      ioRes;
};

#define I2CDEV_SEQUENCEIOINFO_INIT \
{                                  \
   (I2cTransfer *)NULL,                           \
   (I2cDev_Device *)NULL,                           \
   0,0,                            \
   I2CDEV_QUTSEQINFO_INIT,         \
   I2CDEV_INSEQINFO_INIT,          \
   FALSE,                          \
   FALSE,                          \
   I2CDEV_SEQSTATE_IDDLE,          \
   {0,0,0}                         \
}                                  \


typedef struct I2cDev_HwInfoType
{

   uint32                uNumInputSlots;
   uint32                uNumOutputSlots;
   uint32                uNumOutputBlockSlots;
   uint32                uNumOutputBlocks;
   uint32                uNumInputBlockSlots;
   uint32                uNumInputBlocks;
   uint32                uHWVersion;
} I2cDev_HwInfoType;

#define I2CDEV_HWINFOTYPE_INIT {0,0,0,0,0,0,0}


//TODO: rename this to I2cDeviceOperationState
typedef struct I2cDev_State
{
   uint32                           uTimeoutUs;
   I2cDev_SequenceIoState           eIoState;
   I2cIoResult                      opResult;
} I2cDev_State;

#define I2CDEV_STATE_INIT { 0, I2CDEV_SEQSTATE_IDDLE, {0,0,0}}



#define I2CDEV_PROPVALUE_INPUT_CLK_19200_KHZ    (19200)


/* Handle platform functions */
//typedef int32 (*PlatFnPtr)(void * pPlat, int32 cmd, void*pArg);
typedef struct I2cDev_PropertyType
{
   //char    *pDevName;
   uint8     *virtBlockAddr;
   uint32     uI2cInputClkKhz;
  //boolean  bBamAvailable;
   //uint32   uBamThreshold; /* size in bytes when to decide to use bam. */
   //PlatFnPtr pPlatFn;
   //void      *pArg;
} I2cDev_PropertyType;

#define I2CDEV_PROPERTY_INIT(CLK_KHZ)   \
{                                       \
    NULL,                               \
    CLK_KHZ                             \
}                                       \


struct I2cDev_Device
{
   //char                               *pHalVer;
   uint32                              devInitState;
   
   I2cDev_HwInfoType                   devHwInfo;
   I2cDev_SequenceIoInfo               seqInfo;
   I2cDev_QupInfo                      qupInfo;
   I2cDev_PropertyType                 props;
   I2cClientConfig                     clntCfg;
   I2cClientConfig                    *pLastClntCfg;
   
};

#define I2CDEV_DESC_INIT      \
{                             \
   0,                         \
   I2CDEV_HWINFOTYPE_INIT,    \
   I2CDEV_SEQUENCEIOINFO_INIT,\
   I2CDEV_QUPINFO_INIT,       \
   I2CPLAT_PROPERTY_INIT(0,0,0,0,0),     \
   I2CCLIENTCONFING_INIT,     \
   NULL                       \
}


typedef enum I2cDev_Command
{
   I2CDEV_COMMAND_NULL        , /**< Do nothing. */
   I2CDEV_COMMAND_RESET_DEVICE, /**< Resets the device. */
   
} I2cDev_Command;

typedef enum I2cDev_Event
{
   I2CDEV_EVT_ISR,/**< IRQ IST. */
   I2CDEV_EVT_IST, /**< IST Processing thread. */
   I2CDEV_EVT_BAM, /**< . */
   I2CDEV_EVT_POLL, /**< Polled call . */
   
} I2cDev_Event;

#define I2CDEV_DEFAULT_BYTE_TRANSFER_TIMEOUT_US      200

#endif // _I2CDEVICETYPES_H_
