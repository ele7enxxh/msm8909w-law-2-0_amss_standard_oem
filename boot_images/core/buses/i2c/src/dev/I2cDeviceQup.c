/*=============================================================================

FILE:      I2cDeviceQup.c

OVERVIEW:  This file contains the implementation of the API for the QUPI2CDeviceQup
           I2CDEV_Init must be called before any other api call.

           Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
           All Rights Reserved.
           Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE
  

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/i2c/src/dev/I2cDeviceQup.c#1 $
$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $ 
 
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/26/11 LK     Created


=============================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "I2cDevice.h"
#include "I2cPlatSvc.h"
#include "I2cPlatBam.h"
#include "I2cSys.h"
#include "QupeRegs.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

#define MAX_DATA_SEGMENT_LENGTH       255
#define I2CDEVQUP_DEFAULT_BUS_FREQ_KHZ               100
#define I2CDEVQUP_DEFAULT_BYTE_TRANSFER_TIMEOUT_US   200

// this can be a constant for now but it can be
// read from enumeration data.
#define MAX_DEVICES    12
// pause is the worst case
#define QUP_RUN_STATE_CHANGE_TIME_OUT_US         100
#define QUP_BUSY_WAIT_STEP_US                    5

#define IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTrh,pClntCfg,puNumCompleted)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTrh)?           I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE:     \
       (NULL == puNumCompleted)? I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER: \
         I2CDEVQUP_RES_SUCCESS                                                    \
     )

#define IQUPI2CDEV_VALIDATE_SYNC_SEQ(hDev,hSeq,pClntCfg,puNumCompleted) \
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == hSeq)?           I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE:     \
       (NULL == puNumCompleted)? I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER: \
       I2CDEVQUP_RES_SUCCESS                                                    \
     )

#define IQUPI2CDEV_VALIDATE_ASYNC_TRAN(hDev,pTrh,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTrh)?           I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE:     \
       (NULL == pIoRes)?       I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2CDEVQUP_RES_SUCCESS                                                    \
     )
#define IQUPI2CDEV_VALIDATE_ASYNC_SEQ(hDev,hSeq,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == hSeq)?           I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE:     \
       (NULL == pIoRes)?       I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2CDEVQUP_RES_SUCCESS                                                   \
     )

#define                            BAM_TMPBUFF_MAX             (768)
#define                            I2CDEVQUP_HW_EVT_QUE_MAX    (16)

/*-------------------------------------------------------------------------
* Type Declarations
* ----------------------------------------------------------------------*/

enum  I2CDEVQUP_Error
{
   I2CDEVQUP_RES_SUCCESS   =      I2C_RES_SUCCESS,
   I2CDEVQUP_ERR_BASE = I2C_RES_ERROR_CLS_QUP_DEV,
   // Under device class the first byte is reserved to qup errors
   I2CDEVQUP_ERR_INVALID_DEVICE_ID = 0x30000100,
   I2CDEVQUP_ERR_IN_SEQ_OUT_OF_SYNC            ,
   I2CDEVQUP_ERR_INVALID_RUNSTATE              ,
   I2CDEVQUP_ERR_RUNSTATE_CHANGE_TIMEOUT       ,
   I2CDEVQUP_ERR_NEW_RUNSTATE_INVALID          ,
   I2CDEVQUP_ERR_INVALID_POWER_STATE           ,
   I2CDEVQUP_ERR_BUS_BUSY                      ,
   I2CDEVQUP_ERR_BUS_NOT_RELEASED              ,
   I2CDEVQUP_ERR_INPUT_DONE_TRANSFERS_NOT      ,

   I2CDEVQUP_ERR_INVALID_DEV_HANDLE            ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE       ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_OBJPTR       ,
   I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE       ,
   I2CDEVQUP_ERR_INVALID_SEQUENCE_OBJPTR       ,
   I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER   ,
   I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER   ,
   I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER   ,
   I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER  ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION    ,
   I2CDEVQUP_ERR_INPUT_DESC_BUFFER_ALLOC_FAILED,
   I2CDEVQUP_ERR_API_NOT_SUPPORTED             ,
   I2CDEVQUP_ERR_INVALID_RUN_STATE             ,
   I2CDEVQUP_ERR_INVALID_OUTPUT_BYTE_CNT       ,
   I2CDEVQUP_ERR_INVALID_INPUT_BYTE_CNT        ,
   I2CDEVQUP_ERR_INVALID_I2C_CLOCK             ,
   I2CDEVQUP_ERR_INVALID_I2C_FS_DIVIDER        ,
   

   I2CDEVQUP_ERR_BAM_ERROR    ,

};

#define QUPE_DATA_SEGMENT_LEN             255
#define QUPE_PACK28_I2C_STATE(MAS_STATUS, LINE_STATE)\
                ( ((MAS_STATUS) & 0x0FFFFFFC) |\
                  ((LINE_STATE) &0x03)         \
                )

//TODO: Ask HW designer for the values of the tags
typedef enum QUPI2CPD_TagValueType {
   QUPE_TAG_START                        = 0x81,
   QUPE_TAG_START_10BIT                  = 0x89,
   QUPE_TAG_START_STOP                   = 0x8A,
   QUPE_TAG_DATA_WRITE                   = 0x82,
   QUPE_TAG_DATA_WRITE_STOP              = 0x83,
   QUPE_TAG_DATA_READ                    = 0x85,
   QUPE_TAG_DATA_READ_NACK               = 0x86,
   QUPE_TAG_DATA_READ_STOP               = 0x87,

   QUPE_TAG_NOP_WAIT                     = 0x80,
   QUPE_TAG_NOP_MARK                     = 0x90,
   QUPE_TAG_NOP_ID                       = 0x91,
   QUPE_TAG_TIME_STAMP                   = 0x92,
   QUPE_TAG_INPUT_EOT                    = 0x93,
   QUPE_TAG_INPUT_EOT_FLUSH              = 0x94,
   QUPE_TAG_NOP_LOCAL                    = 0x95,
   QUPE_TAG_FLUSH_STOP                   = 0x96,

   QUPE_TAG_EXTEND = 0x7FFFFFFF
} QUPE_I2cTagValueType;

typedef struct I2CDEVQUP_SequenceIOInfo I2CDEVQUP_SequenceIOInfo;

typedef enum I2CDEVQUP_InitState
{
   I2CDEVQUP_InitState_Uninitialized             = 0x0,
   I2CDEVQUP_InitState_TargetInit_Done           = 0x1,
   I2CDEVQUP_InitState_Events_Done               = 0x2,
   I2CDEVQUP_InitState_DeviceInit_Done           = 0x4,
   I2CDEVQUP_InitState_HardwareInit_Done         = 0x8,
   I2CDEVQUP_InitState_PhysMemAlloc_Done         = 0x10,
   I2CDEVQUP_InitState_CritSectionCreate_Done    = 0x20,
   I2CDEVQUP_InitState_ISR_IST_Registration_Done = 0x40,

} I2CDEVQUP_InitState;

typedef volatile struct I2CDEVQUP_QupIsrHwEvt
{
   uint32                              uIrqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   uint32                              uOutTranCnt;
   uint32                              uInTranCnt;
   uint32                              i2cStatus;

} I2CDEVQUP_QupIsrHwEvt;

typedef volatile struct I2CDEVQUP_QupBamHwEvt
{
   uint32                              uBamEvt;
} I2CDEVQUP_QupBamHwEvt;

typedef volatile struct I2CDEVQUP_QupHwEvtQueue
{
   I2CDEVQUP_QupIsrHwEvt      aHwEvt[I2CDEVQUP_HW_EVT_QUE_MAX];
   I2CDEVQUP_QupIsrHwEvt     *pHead;
   I2CDEVQUP_QupIsrHwEvt     *pTail;

} I2CDEVQUP_QupHwEvtQueue;

typedef volatile struct I2CDEVQUP_QupInfo
{
   /* Register access */
   QupRegsType                        *pQupRegs;
   I2cRegsType                        *pI2cRegs;

   uint32                              uIrqCnt;
   I2CDEVQUP_QupHwEvtQueue             hwEvtQueue;
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
   QUPE_IoModeType                     eOutFifoMode;
   QUPE_IoModeType                     eInFifoMode;
} I2CDEVQUP_QupInfo;

typedef enum I2CDEVQUP_TransferState
{
   I2CDEVQUP_TR_STATE_ID_TAG,
   I2CDEVQUP_TR_STATE_ID_VALUE,
   I2CDEVQUP_TR_STATE_TIME_STAMP_TAG,
   I2CDEVQUP_TR_STATE_TIME_STAMP_VALUE,
   I2CDEVQUP_TR_STATE_START_TAG,
   I2CDEVQUP_TR_STATE_SLAVE_ADDR,
   I2CDEVQUP_TR_STATE_RD_DATA_TAG,
   I2CDEVQUP_TR_STATE_WR_DATA_TAG,
   I2CDEVQUP_TR_STATE_DATA_CNT,
   I2CDEVQUP_TR_STATE_EOT_TAG,
   I2CDEVQUP_TR_STATE_DATA_VALUE,
   I2CDEVQUP_TR_STATE_FLUSH_STOP,
   I2CDEVQUP_TR_STATE_DATA_PAD,

   I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER,

   I2CDEVQUP_TR_STATE_EXTEND  = 0x7FFFFFFF
} I2CDEVQUP_TransferState;


typedef enum I2CDEVQUP_OutputState
{
   I2CDEVQUP_OUTPUT_IDDLE            ,
   I2CDEVQUP_OUTPUT_STALLED          ,
   I2CDEVQUP_OUTPUT_FILL_FIFO        ,
   I2CDEVQUP_OUTPUT_FILL_FIFO_DONE   ,
   I2CDEVQUP_OUTPUT_TRANSMIT_DONE    ,
} I2CDEVQUP_OutputState;

typedef struct I2CDEVQUP_OutSeqInfo
{
   I2CDEVQUP_OutputState               eOutputState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalOutBytes; //data + format
   uint32                              uCurOutBytes; //data + format
   uint32                              uNumOutDtBytes;//data cnt
   uint32                              uOutTrCnt;
   uint32                              uTrIndex;
   uint8                               *aOutBamBuff;
   uint32                              uBamBuffIndex;

   struct OutTransferInfo
   {
      I2CDEVQUP_TransferState          eOutTrState;
      uint32                           uDataIndex;
      uint32                           uNextDataSegIndex;
      I2cBuffDesc                     *pTrIOVec;
      uint32                           IoVecBuffIndex;
   } outTr;
   I2CDEVQUP_SequenceIOInfo           *pSeq;
} I2CDEVQUP_OutSeqInfo;

typedef enum I2CDEVQUP_InputState
{
   I2CDEVQUP_INPUT_IDDLE,
   I2CDEVQUP_INPUT_STALLED,
   I2CDEVQUP_INPUT_MEMORY_FLUSH,
   I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO,
   I2CDEVQUP_INPUT_PUSH_DESCRIPTORS,
   I2CDEVQUP_INPUT_WAIT_DESC_DATA_READY,
   I2CDEVQUP_INPUT_COPY_INPUT_BUFFERS,
   I2CDEVQUP_INPUT_RECEIVE_DONE
} I2CDEVQUP_InputState;

typedef struct  I2CDEVQUP_InSeqInfo
{
   I2CDEVQUP_InputState                eInSeqState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalInBytes;//data + format
   uint32                              uNumInDtBytes;//data cnt
   uint32                              uInPadCnt;
   uint32                              uInTrCnt;
   uint32                              uTrIndex;
   uint32                              uLastTrIndex;
   uint32                              uNumInBytesReceived;
   /* v1 single input descriptor physical memory */
   //I2CSYS_PhysMemDesc                  inPhysMem;
   //boolean                             bUseInPhysMem;
   /* uint8                              *aInBamBuff; */

   struct InTransferInfo
   {
      I2CDEVQUP_TransferState          eInTrState;
      uint32                           uDataIndex; //transfer data index
      uint32                           uNextDataSegIndex;
      //uint32                           uTimeStamp; // for now do nothing with this.
      I2cBuffDesc                     *pTrIOVec;
      uint8                           *pIoVecBuff;
      uint32                           IoVecBuffIndex;
   } inTr;
   I2CDEVQUP_SequenceIOInfo           *pSeq;
} I2CDEVQUP_InSeqInfo;

typedef enum I2CDEVQUP_SequenceState
{
   I2CDEVQUP_SEQSTATE_IDDLE,
   I2CDEVQUP_SEQSTATE_INPUT_OUTPUT,
   I2CDEVQUP_SEQSTATE_COMPLETING,   
   I2CDEVQUP_SEQSTATE_COMPLETE,
} I2CDEVQUP_SequenceState;

struct I2CDEVQUP_SequenceIOInfo
{
   I2cTransfer                     *pTransfer;
   uint32                           uNumTransfers;
   uint32                           uSeqTimeoutUs;
   I2CDEVQUP_OutSeqInfo             outSeqInfo;
   I2CDEVQUP_InSeqInfo              inSeqInfo;
   //I2CSYS_PhysMemDesc               physMem;
   boolean                          bBamTransfer;
   I2CDEVQUP_SequenceState          eSeqState;
   I2cIoResult                      ioRes;
};

typedef struct I2CDEVQUP_AsyncCallData
{
   DEV_ASYNC_CB                        pCallbackFn;
   void                               *pArg;
   I2cIoResult                        *pIoRes;
} I2CDEVQUP_AsyncCallData;

typedef struct I2CDEVQUP_Device I2CDEVQUP_Device;

typedef struct I2CDEVQUP_HwInfoType
{

   uint32                uNumInputSlots;
   uint32                uNumOutputSlots;
   uint32                uNumOutputBlockSlots;
   uint32                uNumOutputBlocks;
   uint32                uNumInputBlockSlots;
   uint32                uNumInputBlocks;
   uint32                uHWVersion;
} I2CDEVQUP_HwInfoType;

struct I2CDEVQUP_Device
{
   uint32                              uPlatDevId;
   I2CPLATDEV_PropertyType             devProps;
   I2CDEVQUP_HwInfoType                devHwInfo;
   char                               *pHalVer;
   uint32                              devInitState;
   I2CDEVQUP_SequenceIOInfo            seqInfo;
   I2CDEVQUP_QupInfo                   qupInfo;
   I2CSYS_EVENT_HANDLE                 hQupEvt;

   I2CPLATDEV_HANDLE                   hDevTgt;

   I2cClientConfig                     clntCfg;
   I2cClientConfig                    *pLastClntCfg;
   I2CDEV_PowerStates                  devPowerState;
   //I2CDEVQUP_AsyncCallData             asyncCall;
   I2CSYS_CRITSECTION_HANDLE           hOperationSync;

   //PFnPerformInput                     pFnPerformInput;
   //I2CLOG_DevType                     *pDevLog;

   I2CDEVQUP_Device                   *pNext; /* used to link the dev structs */
};


/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

I2CDEVQUP_Device *pDevListHead = NULL;

/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/
static void
QUP_ProcessNextInputSlot
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   uint32                          uSlot
);

static uint32 
QUP_GetNextOutSeqSlot
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo   
);


/** @brief Searches for a device in the list.
  
    @param[in] uPlatDevId    Platform id for the device.
    
    @return             Pointer to dev if found, otherwise NULL.
  */
static I2CDEVQUP_Device * 
I2CDEVQUP_SearchDevice
(
   uint32          uPlatDevId 
)
{
   I2CDEVQUP_Device * pDev = pDevListHead;
   while ( pDev ) {
      if ( pDev->uPlatDevId == uPlatDevId ) {
         return pDev;
      }
      pDev = pDev->pNext;
   }

   return NULL;
}

/** @brief Links a device in the list.

    @param[in] pDev     Pointer to device structure.
    
    @return             Nothing.
  */
static void 
I2CDEVQUP_LinkDevice
(
   I2CDEVQUP_Device * pDev 
)
{
   pDev->pNext = pDevListHead;
   pDevListHead = pDev;
}

/** @brief Unlinks a device in the list.

    @param[in] pDev     Pointer to device structure.
    
    @return             Nothing.
  */
static void 
I2CDEVQUP_UnlinkDevice
(
   I2CDEVQUP_Device * pDev 
)
{
   I2CDEVQUP_Device  *pCurDev;
   I2CDEVQUP_Device **ppPrev;

   ppPrev  = &pDevListHead;
   pCurDev = pDevListHead;

   while ( NULL != pCurDev ) {
      if ( pCurDev == pDev ) {
         *ppPrev = pCurDev->pNext;
         return;
      }
      ppPrev = &pCurDev->pNext;
      pCurDev = pCurDev->pNext;  
   }
}

/** @brief Updatest the transfer count stored in the device.

    This Function returns the input/output the transfer count in
    the locations pointed by pOutTrCnt/pInTrCnt pointers.

    @param[in] pDev         Pointer to device struct.
    @param[in] pOutTrCnt    Pointer to output transfer count.
    @param[in] pInTrCnt     Pointer to input transfer count.

    @return          Nothing.
  */
static void QUP_GetTransferCnt
(
   I2CDEVQUP_Device *pDev,
   volatile uint32           *pOutTrCnt,
   volatile uint32           *pInTrCnt
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   QupRegsType                    *pQupRegs;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   

   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   

   if (QUPE_IO_MODE_FIFO == pQupInfo->eOutFifoMode) { 
      *pOutTrCnt = pOutSeqInfo->uTotalOutBytes -
              (pQupRegs->MX_WRITE_CNT_CURRENT & QUP_MX_WRITE_COUNT_RMSK);
   }
   else if (QUPE_IO_MODE_BLOCK == pQupInfo->eOutFifoMode) {
      *pOutTrCnt = pOutSeqInfo->uTotalOutBytes -
              (pQupRegs->MX_OUTPUT_CNT_CURRENT & QUP_MX_OUTPUT_CNT_CURRENT_RMSK);
   }

   if (QUPE_IO_MODE_FIFO == pQupInfo->eInFifoMode) {
      *pInTrCnt = pInSeqInfo->uTotalInBytes - 
              (pQupRegs->MX_READ_CNT_CURRENT & QUP_MX_READ_CNT_CURRENT_RMSK); 
   }
   else if (QUPE_IO_MODE_BLOCK == pQupInfo->eInFifoMode) {
      *pInTrCnt = pInSeqInfo->uTotalInBytes - 
              (pQupRegs->MX_INPUT_CNT_CURRENT & QUP_MX_INPUT_CNT_CURRENT_RMSK);
   }
}

/** @brief Wait for state to go valid or timeout.
           
    @param[in, out] pDev      Pointer to qup device structure.
    @param[out]     pState    Pointer to state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2CDEVQUP_WaitForRunStateValid
(
   I2CDEVQUP_Device      *pDev,
   uint32                *pState
)
{
   QupRegsType                        *pQupRegs;
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   uint32         uState;

   pQupRegs = pDev->qupInfo.pQupRegs;

   uState = pQupRegs->STATE;
   while( (!(uState & QUP_STATE_STATE_VALID_BMSK)) &&
            (0 != uWaitTimeUs) ) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      uState = pQupRegs->STATE;
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEVQUP_ERR_INVALID_RUNSTATE;
   }

   *pState = uState;

   return I2C_RES_SUCCESS;
}

/** @brief Resets the qup core and waits until state is valid.
           
    @param[in, out] pDev      Pointer to qup device structure.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2CDEVQUP_InitQupCore
(
   I2CDEVQUP_Device      *pDev
)
{
   int32              res;
   uint32             uState;
   uint32             uRegVal;
   I2cRegsType       *pI2cRegs;
   QupRegsType       *pQupRegs;


   pI2cRegs = pDev->qupInfo.pI2cRegs;
   pQupRegs = pDev->qupInfo.pQupRegs;

   pQupRegs->SW_RESET = 1;
   res = I2CDEVQUP_WaitForRunStateValid(pDev, &uState);
   if ( I2C_RES_SUCCESS != res ) {
      return res;     
   }

   uRegVal = (QUPE_MINI_CORE_I2C_MASTER << QUP_CONFIG_MINI_CORE_SHFT) |
         //    (1 << QUP_CONFIG_EN_EXT_STATE_VALID_SHFT)                |
             QUPE_CONFIG_N_8BITS;
   pQupRegs->CONFIG = uRegVal;

   pI2cRegs->CONFIG = (QUPE_I2C_TAGS_V2 << I2C_CONFIG_EN_VERSION_TWO_TAG_SHFT);
   return I2C_RES_SUCCESS;
}

/** @brief Sets the runstate. It will wait untill the state is valid
           before trying to set the new state or timeout.
           
    @param[in, out] pDev      Pointer to qup device structure.
    @param[in] eNewRunState   The new run state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2CDEVQUP_SetRunState
(
   I2CDEVQUP_Device         *pDev,
   const QUPE_RunStateType   eNewRunState
)
{
   QupRegsType              *pQupRegs;     
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   QUPE_RunStateType     eRunState;
   uint32                uState;
   uint32                uNewState;
   int32                 res;
   

   pQupRegs = pDev->qupInfo.pQupRegs;
   res = I2CDEVQUP_WaitForRunStateValid(pDev, &uState);
   if ( I2C_RES_SUCCESS != res ) {
      return I2CDEVQUP_ERR_INVALID_RUNSTATE;
   }

   eRunState = (QUPE_RunStateType)(uState & QUP_STATE_STATE_BMSK);
   if ( eNewRunState == eRunState ) {
     return I2C_RES_SUCCESS;
   }
   uNewState = uState & (~QUP_STATE_STATE_BMSK);
   /* the doc says to write two times */
   switch(eNewRunState)
   {
      case QUPE_RUNSTATE_RESET :
         uNewState |= QUPE_RUNSTATE_CMD_RESET;
         if (QUPE_RUNSTATE_PAUSE == eRunState) {
            pQupRegs->STATE = uNewState;     
        	}
         pQupRegs->STATE = uNewState;
         break;

     case QUPE_RUNSTATE_RUN   :
         uNewState |= QUPE_RUNSTATE_CMD_RUN;
         pQupRegs->STATE = uNewState;
         break;

      case QUPE_RUNSTATE_PAUSE :
         uNewState |= QUPE_RUNSTATE_CMD_PAUSE;
         pQupRegs->STATE = uNewState;
         break;

      default:
         return (I2CDEVQUP_ERR_INVALID_RUN_STATE);
   }

   uState = pQupRegs->STATE;
   eRunState = (QUPE_RunStateType)(uState & QUP_STATE_STATE_BMSK);

   while ( ( 0 != uWaitTimeUs ) && ( eNewRunState != eRunState ) ) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      uState = pQupRegs->STATE;
      eRunState = (QUPE_RunStateType)(uState & QUP_STATE_STATE_BMSK);
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEVQUP_ERR_RUNSTATE_CHANGE_TIMEOUT;
   }

   return I2C_RES_SUCCESS;
}

/** @brief Initializes the event queue


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_Init
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   pHwEvtQ->pHead = pHwEvtQ->aHwEvt;
   pHwEvtQ->pTail = pHwEvtQ->aHwEvt;
}

/** @brief Checks if the event queue is full


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   TRUE if full, FALSE otherwise.
  */
static boolean
I2CDEVQUP_QupHwEvtQueue_IsFull
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   return (pNext == pHwEvtQ->pTail);
}

/** @brief Checks if the event queue is full


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   TRUE if full, FALSE otherwise.
  */
static boolean
I2CDEVQUP_QupHwEvtQueue_IsEmpty
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   return (pHwEvtQ->pHead == pHwEvtQ->pTail);
}

/** @brief Moves tail pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_TailNext
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pTail + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pTail = pNext;
}

/** @brief Moves head pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_HeadNext
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pHead = pNext;
}


/** @brief This function Performs fifo/block mode specific servicing.
   
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
I2CDEVQUP_QupHwEvtQueue_ProcessIsrEvt
(
   I2CDEVQUP_Device      *pDev,
   I2CDEVQUP_QupIsrHwEvt *pIsrEvt
)
{   
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   QupRegsType                    *pQupRegs; 

   pQupInfo    = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pQupRegs    = pQupInfo->pQupRegs;

   pQupInfo->uQupErrors       = pIsrEvt->uQupErrors;
   pQupInfo->uOperational     = pIsrEvt->uOperational;
   pQupInfo->i2cStatus        = pIsrEvt->i2cStatus;

   pQupInfo->uCurOutTransfers = pIsrEvt->uOutTranCnt;
   pQupInfo->uCurInTransfers  = pIsrEvt->uInTranCnt;

   /* If output service then fill the output queue with another block.       */
   if ( pIsrEvt->uOperational & QUPE_OPSTATE_OUTPUT_SERVICE ) {
      if ( (QUPE_IO_MODE_BLOCK == pQupInfo->eOutFifoMode) &&
       (pIsrEvt->uOperational & QUPE_OPSTATE_OUT_BLOCK_WRITE_REQ) ) {
         pQupInfo->uOutFreeSlotReqCnt += pDev->devHwInfo.uNumOutputBlockSlots;
      }
      /* In fifo mode the OUTPUT_SERVICE is set and the MAX_OUTPUT_DONE
       * is not set at the same time. */
      if ( QUPE_IO_MODE_FIFO == pQupInfo->eOutFifoMode ) {
         pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
      }
   }
   if ( pIsrEvt->uOperational & QUPE_OPSTATE_MAX_OUTPUT_DONE ) {
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
   }

   /* If input service then get the blocks from queue.                       */
   if ( pIsrEvt->uOperational & QUPE_OPSTATE_INPUT_SERVICE ) {
      if ( QUPE_IO_MODE_BLOCK == pQupInfo->eInFifoMode ) {
         /* last block gets this as well as normal block requests. */
         pQupInfo->uInFreeSlotReqCnt += pDev->devHwInfo.uNumInputBlockSlots;
      }
      else {
         pQupInfo->uInFreeSlotReqCnt += 
            (pQupRegs->INPUT_FIFO_WORD_CNT & QUP_INPUT_FIFO_WORD_CNT_RMSK);
      }
   }
   //if ( pIsrEvt->i2cStatus & I2C_OP_STATUS_ERROR_FIELD ) {
      /* Collect the input if there is an error. */
      
   //}

   return;
}

/** @brief Calculate the number of input and output bytes on a transfer.

    Calculate the number of input and output bytes for a transfer.

    @param[in]  pTransfer       Transfer pointer.
    @param[out] puNumInBytes    Pointer to num input bytes.
    @param[out] puNumOutBytes   Pointer to num output bytes.
          reads.

    @return                   Nothing.
  */
void
I2CDEVQUP_CalcTransferInOutSize
(
   I2cTransfer                     *pTransfer,
   uint32                 *puNumInBytes,
   uint32                 *puNumOutBytes
)
{
   uint32 uNumDataSegments;
   uint32 uTransferSize = pTransfer->uTrSize;
  
   uNumDataSegments = uTransferSize/MAX_DATA_SEGMENT_LENGTH;
   if ( uTransferSize % MAX_DATA_SEGMENT_LENGTH ) {
      uNumDataSegments++;
   }
   if ( I2cTranDirIn == pTransfer->eTranDirection ) {
      /* read:
         st_tag + sl_addr + {dt_tag + bt_cnt} * n */
      *puNumOutBytes = 2 +  uNumDataSegments*2;
      /* {dt_tag + bt_cnt} * n + data_len               */
      *puNumInBytes  =  uNumDataSegments*2 + uTransferSize;
   }
   else {
      /* write:
         st_tag + sl_addr + {dt_tag + bt_cnt} * n + data_len */
      *puNumOutBytes = 2 +  uNumDataSegments*2 + uTransferSize;
      *puNumInBytes  = 0;
   }
}



/** @brief Calculate the number of input and output data bytes transferred.

    Calculate the number of input and output bytes for a transfer.

    @param[in]  pDev            Pointer to device.
 
    @return                   Nothing.
  */
static void
I2CDEVQUP_CalcTransferredDataBytes
(
   I2CDEVQUP_Device                    *pDev
)
{
   uint32                              uNumDataSegments;
   I2CDEVQUP_SequenceIOInfo           *pSeqInfo;
   I2CDEVQUP_QupInfo                  *pQupInfo;
   uint32                              uOutByteCnt;
   uint32                              uInByteCnt;
   uint32                              uNumInTrBytes;
   uint32                              uNumOutTrBytes;
   I2CDEVQUP_OutSeqInfo               *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo                *pInSeqInfo;
   I2cTransfer                        *pTransfer;
   uint32                              uTrIndex;
   boolean                             bOutComplete;
   boolean                             bInComplete;

   pSeqInfo    = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pQupInfo    = &pDev->qupInfo;


   if (I2C_RES_SUCCESS == pSeqInfo->ioRes.nOperationResult) {
      pSeqInfo->ioRes.uInByteCnt  = pInSeqInfo->uNumInDtBytes;
      pSeqInfo->ioRes.uOutByteCnt = pOutSeqInfo->uNumOutDtBytes;
      return;
   }

   //uInByteCnt   = pSeqInfo->inSeqInfo.uTotalInBytes - pQupInfo->uCurInTransfers;
   uInByteCnt   = pQupInfo->uCurInTransfers;
   //uOutByteCnt  = pSeqInfo->outSeqInfo.uTotalOutBytes - pQupInfo->uCurOutTransfers;
   uOutByteCnt  = pQupInfo->uCurOutTransfers;
   pSeqInfo->ioRes.uInByteCnt  = 0;
   pSeqInfo->ioRes.uOutByteCnt = 0;
   bOutComplete = bInComplete = FALSE;

   if ( uOutByteCnt != 0 ) {
      uTrIndex = 0;
      do {
         pTransfer = &pSeqInfo->pTransfer[uTrIndex];
         I2CDEVQUP_CalcTransferInOutSize(pTransfer,&uNumInTrBytes, &uNumOutTrBytes);

         if ( uOutByteCnt >= uNumOutTrBytes ) {
            uOutByteCnt -= uNumOutTrBytes;
            if ( 0 == uOutByteCnt ) {
               bOutComplete = TRUE;/* full output last */
            }
         }
         else {
            break;/* partial output do not count this transfer */
         }

         if ( uInByteCnt >= uNumInTrBytes ) {
            uInByteCnt -= uNumInTrBytes;
            if ( 0 == uInByteCnt ) {
               bInComplete = TRUE;/* full input last */
            }
         }
         else {
            break;/* partial input do not count this */
         }

         if ( I2cTranDirIn == pTransfer->eTranDirection ) {
            pInSeqInfo->uNumInDtBytes += pTransfer->uTrSize;
         }
         else {
            pOutSeqInfo->uNumOutDtBytes += pTransfer->uTrSize;
         }

         ++uTrIndex;
      } while( (uTrIndex < pSeqInfo->uNumTransfers) &&
               (!bOutComplete || !bInComplete) );
   }

   if ( !bOutComplete || !bInComplete ) {
      if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
         /* read
             {bt_cnt} * n + data_len */
         uNumDataSegments = uInByteCnt/(MAX_DATA_SEGMENT_LENGTH + 1);
         if (uInByteCnt % (MAX_DATA_SEGMENT_LENGTH + 1)) {
            uNumDataSegments++;
         }
         /* count out the formating overhead, 1 byte per segment */
         pInSeqInfo->uNumInDtBytes += (uInByteCnt - uNumDataSegments);
      }
      else {
         /* write
             st_tag + sl_addr + {dt_tag + bt_cnt} * n + data_segment */
         if (uOutByteCnt > 4) {
            uOutByteCnt -= 2; /* drop st_tag + sl_addr */
            uNumDataSegments = uOutByteCnt/(MAX_DATA_SEGMENT_LENGTH + 2);
            if (uOutByteCnt % (MAX_DATA_SEGMENT_LENGTH + 2)) {
               uNumDataSegments++;
            }
            /* count out the formating overhead, 2 bytes per segment */
            pOutSeqInfo->uNumOutDtBytes += (uOutByteCnt - uNumDataSegments*2);
         }
      }
   }
}

/**
    This function waits for bus free.

    @param[in, out] pDev      Pointer to qup device structure.
    @param[in]      uTimeout  Timeout value.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_WaitForI2cBusFree
(
   I2CDEVQUP_Device               *pDev,
   uint32                          uTimeout
)
{
   const uint32      cuTimeStep = 5; /* 5 us */
   uint32            uI2cStatus;
   boolean           bBusFree;
   I2cRegsType      *pI2cRegs;

   pI2cRegs = pDev->qupInfo.pI2cRegs;

   uI2cStatus = pI2cRegs->STATUS;
   bBusFree = !(uI2cStatus & I2C_STATUS_BUS_ACTIVE_BMSK);
   
   if ( bBusFree ) {
      return TRUE;
   }
   
   do{
      I2CSYS_BusyWait(cuTimeStep);
      if ( uTimeout > cuTimeStep ) {
         uTimeout -= cuTimeStep;
      }
      else {
         uTimeout = 0;
      }
      uI2cStatus = pI2cRegs->STATUS;
      bBusFree = !(uI2cStatus & I2C_STATUS_BUS_ACTIVE_BMSK);
   } while ( uTimeout && (!bBusFree) );

   return bBusFree;
}


/**
    This function performs completion.


    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_Completion
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   int32  res;
   /* If this is an asynchronous transfer then set results and
      dispatch callback else just signal the waiting thread. */

   pQupInfo = &pDev->qupInfo;

   /* 
     QCTDD00352568.
     The driver needs to wait for bus idle before in sets the runstate 
     to RESET.
    */
   if ( !QUP_WaitForI2cBusFree(pDev, pDev->clntCfg.uByteTransferTimeoutUs ) ) {
      if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
         pDev->seqInfo.ioRes.nOperationResult = I2CDEVQUP_ERR_BUS_NOT_RELEASED;
      }     
   }

   /* Do not update the count if there is a core error.
      The core will have been reset and the count is 0. */ 
   if ( (!(pQupInfo->uQupErrors & I2CDEVQUP_ERR_BASE )) &&
        (!(pDev->seqInfo.ioRes.nOperationResult & I2C_RES_ERROR_CLS_I2C_CORE)) ) {
      QUP_GetTransferCnt(pDev, &pQupInfo->uCurOutTransfers, &pQupInfo->uCurInTransfers);
   }

   I2CDEVQUP_CalcTransferredDataBytes(pDev);

   res = I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      I2CDEVQUP_InitQupCore(pDev);
      if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
         pDev->seqInfo.ioRes.nOperationResult = res;
      }
   }

}

#if 0
/**
    This function checks if this call is synchronous with
    interrupts enabled.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if this call is synchronous and interrupts enabled,
                    FALSE otherwise.
  */
static boolean
QUP_IsSyncEvtCall
(
   I2CDEVQUP_Device               *pDev
)
{
   return ( pDev->devProps.bInterruptBased && 
        ( NULL == pDev->asyncCall.pCallbackFn ) );
}

/**
    This function checks if this call is asynchronous with
    interrupts enabled.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if this call is asynchronous and interrupts enabled,
                    FALSE otherwise.
  */
static boolean
QUP_IsAsyncCall
(
   I2CDEVQUP_Device               *pDev
)
{
   return ( pDev->devProps.bInterruptBased && 
        ( NULL != pDev->asyncCall.pCallbackFn ) );
}
#endif

/**
    This function performs calls callbacks or sets events.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
/*
static void 
QUP_AsyncCallbackAsyncCall
(
   I2CDEVQUP_Device               *pDev
)
{
   *pDev->asyncCall.pIoRes      = pDev->seqInfo.ioRes;
   pDev->asyncCall.pCallbackFn(pDev->asyncCall.pArg);   
}
*/

/**
    This function processes the qup hw events .

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
void
I2CDEVQUP_ProcesQupHwEvts
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo     *pQupInfo;
   I2CDEVQUP_QupIsrHwEvt *pIsrEvt;

   /* I2CPLATDEV_AcquireIntLock(pDev->hDevTgt); */
   pQupInfo = &pDev->qupInfo;
   
   while ( !I2CDEVQUP_QupHwEvtQueue_IsEmpty(&pQupInfo->hwEvtQueue) ) {
      /* Tail is modified the I2CDEVQUP_QupHwEvtQueue_TailNext. */     
      pIsrEvt  = pQupInfo->hwEvtQueue.pTail; 
      I2CDEVQUP_QupHwEvtQueue_ProcessIsrEvt(pDev, pIsrEvt);     
      I2CDEVQUP_QupHwEvtQueue_TailNext(&pQupInfo->hwEvtQueue);
   }
   /* I2CPLATDEV_ReleaseIntLock(pDev->hDevTgt); */
}

#if 0
/**
    This function processes the bam events .

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
void
I2CDEVQUP_ProcesBamEvts
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;

   pQupInfo = &pDev->qupInfo;
   pSeqInfo = &pDev->seqInfo;

   switch ( pQupInfo->eBamEvt ) {
      case I2CPLATBAM_EVT_BAM_ERROR:
           /* TODO: should not happen abort transfer. */
           pDev->seqInfo.ioRes.nOperationResult = I2CDEVQUP_ERR_BAM_ERROR;
           break;
      case I2CPLATBAM_EVT_TX_EOT:
           pSeqInfo->outSeqInfo.eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
           break;
      case I2CPLATBAM_EVT_TX_DESC_DONE: /* INTERRUPT fired. */
           pSeqInfo->outSeqInfo.eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO;
           break;
      case I2CPLATBAM_EVT_RX_EOT:
           pSeqInfo->inSeqInfo.eInSeqState = I2CDEVQUP_INPUT_COPY_INPUT_BUFFERS;
           break;
      case I2CPLATBAM_EVT_RX_DESC_DONE:/* INTERRUPT fired */
           if ( I2CDEVQUP_INPUT_STALLED == pSeqInfo->inSeqInfo.eInSeqState ) {
              pSeqInfo->inSeqInfo.eInSeqState = I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO;
           }
           else {
              /* I2CDEVQUP_INPUT_MEMORY_FLUSH == pSeqInfo->inSeqInfo.eInSeqState */
              pSeqInfo->inSeqInfo.eInSeqState = I2CDEVQUP_INPUT_COPY_INPUT_BUFFERS;
           }
           break;

      case I2CPLATBAM_EVT_NULL: /*  */
           break;
   }

   pQupInfo->eBamEvt = I2CPLATBAM_EVT_NULL;
}
#endif

/**
    This function performs output on the output fifo if there are free slots.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_PerformOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   uint32                          uSlot;
   QupRegsType                    *pQupRegs;

   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   
   
   /* If transfer to be queued Check if there is space in the output fifo
      If to be qued and space continue state machine. */

   /* If output is done update the state of the output sequence. */
   while ( (I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (pQupInfo->uOutFreeSlotReqCnt > pQupInfo->uOutFreeSlotServicedCnt) ) {
      uSlot = QUP_GetNextOutSeqSlot(pOutSeqInfo);
      /* push slot into output fifo. */
      pQupRegs->OUTPUT_FIFO0 = uSlot;
      pQupInfo->uOutFreeSlotServicedCnt++;

   }
}

/**
    This function empties the input queue in block/fifo mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_PerformInput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   QupRegsType                    *pQupRegs;
   uint32                          uSlot;

   pQupInfo    = &pDev->qupInfo;
   pQupRegs    = pDev->qupInfo.pQupRegs;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   while ((I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState) &&
          (pQupInfo->uInFreeSlotReqCnt > pQupInfo->uInFreeSlotServicedCnt) ) {
      uSlot = pQupRegs->INPUT_FIFO0;
      QUP_ProcessNextInputSlot(pInSeqInfo, uSlot);
      pQupInfo->uInFreeSlotServicedCnt++;
   }
}

/**
    This function performs sequence input/output.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_PerformInputOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   boolean                         bComplete;
   //I2CDEVQUP_AsyncCallData         asyncCall;
   //boolean                         bAsyncNotify;
   uint32                          uWhileOnce = 0;
   


   pQupInfo = &pDev->qupInfo;
   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   
   bComplete        = FALSE;
   //bAsyncNotify     = FALSE;


   I2CSYS_EnterCriticalSection(pDev->hOperationSync);
   do {
       
      I2CDEVQUP_ProcesQupHwEvts(pDev);
      //I2CDEVQUP_ProcesBamEvts(pDev);
      
      //asyncCall = pDev->asyncCall;

      if ( I2CDEVQUP_SEQSTATE_INPUT_OUTPUT != pSeqInfo->eSeqState ) {
         break;     
      }

      /* Check for qup hw errors. This should not happen.
         It indicates most likely a programming error.
       */
      if ( pQupInfo->uQupErrors & QUPE_ERRSTATE_FIELD ) {
         pSeqInfo->ioRes.nOperationResult = I2CDEVQUP_ERR_BASE |
                     (pQupInfo->uQupErrors & QUPE_ERRSTATE_FIELD);
         I2CDEVQUP_InitQupCore(pDev);
         bComplete = TRUE;
         break;
      }

      /* Input sequence handling.
         If there is an error collect as much input as possible.
         All the data in the input fifo.
       */
      if ( (I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState)
           //|| (I2CDEVQUP_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState)
           //|| (I2CDEVQUP_INPUT_COPY_INPUT_BUFFERS == pInSeqInfo->eInSeqState)
         ) {
         //pDev->pFnPerformInput(pDev);
         QUP_PerformInput(pDev);
         ///* In bam mode input can cause errors */
         //if ( I2C_RES_SUCCESS != pSeqInfo->ioRes.nOperationResult ) {
         //   bComplete = TRUE;
         //   break;     
         //}
      }

      if ( pQupInfo->i2cStatus & I2C_OP_STATUS_ERROR_FIELD ) {
         pSeqInfo->ioRes.nOperationResult = I2C_RES_ERROR_CLS_I2C_CORE;
         pSeqInfo->ioRes.nOperationResult |=
             QUPE_PACK28_I2C_STATE(pQupInfo->i2cStatus, 0);
         /* In bam mode the sequence has to be flushed out of the input output
            pipes the same way as a normal sequence. */
         //if ( QUPE_IO_MODE_BAM != pQupInfo->eOutFifoMode ) {
            bComplete = TRUE;
            break;
         //}
      }

      /* Check if there is a transfer to be placed on the out fifo. */
      if ( I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState ) {
         //pDev->pFnPerformOutput(pDev);
         QUP_PerformOutput(pDev);
      }
      /* If the transfer has already finished do completion. */
      if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) &&
           (0 == pInSeqInfo->uTotalInBytes) ) {
         bComplete = TRUE;
         break;
      }

      if ( (I2CDEVQUP_INPUT_RECEIVE_DONE == pInSeqInfo->eInSeqState) &&
           (I2CDEVQUP_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) ) {
         bComplete = TRUE;
         break;
      }
   } while ( uWhileOnce );

   if ( bComplete ) {
      pSeqInfo->eSeqState = I2CDEVQUP_SEQSTATE_COMPLETING;
      QUP_Completion(pDev);
      /*
          The synchronous event notification needs to be done within critical
          section otherwise there is a chance that the notification could go
          to the next transfer if the previous one terminated and the next one
          scheduled.
       */
      //if ( QUP_IsSyncEvtCall(pDev) ) {
         I2CSYS_SetEvent(pDev->hQupEvt);
      //}
      //else if ( QUP_IsAsyncCall(pDev) ) {
      //   /* ... store the callback arguments within crit section */
      //   *pDev->asyncCall.pIoRes      = pDev->seqInfo.ioRes;
      //   asyncCall = pDev->asyncCall;
      //   bAsyncNotify = TRUE;
      //}
   }
   
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

   //if ( bAsyncNotify ) {
   //   asyncCall.pCallbackFn(asyncCall.pArg); 
   //}
   
   
   return;
}

/** @brief This is a hook for the thread servicing the qup.

    This Function is called by the framework to handle the data
    transfer in response to qup irq events.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
static void 
I2CDEVQUP_IST_HOOK
(
   void *pdata
)
{
   if ( NULL == pdata) {
      return;
   }
   QUP_PerformInputOutput((I2CDEVQUP_Device*)pdata);
}



/** @brief This function initializes the output transfer info structure.

    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_OutSeqInitTransfer
(
   I2CDEVQUP_OutSeqInfo       *pOutSeqInfo
)
{   
    pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_START_TAG;
    pOutSeqInfo->outTr.uNextDataSegIndex = 0;
    pOutSeqInfo->outTr.uDataIndex = 0;

    if ( I2cTranDirOut == pOutSeqInfo->pTransfer->eTranDirection ) {
       pOutSeqInfo->outTr.pTrIOVec  = pOutSeqInfo->pTransfer->pI2cBuffDesc;
       pOutSeqInfo->outTr.IoVecBuffIndex = 0;
    }
}

/** @brief This function initializes the input transfer info structure.
      
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_InSeqInitTransfer
(
   I2CDEVQUP_InSeqInfo       *pInSeqInfo
)
{
    pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
    pInSeqInfo->inTr.uDataIndex = 0;
    pInSeqInfo->inTr.uNextDataSegIndex = 0;

    pInSeqInfo->inTr.pTrIOVec   = pInSeqInfo->pTransfer->pI2cBuffDesc;
    pInSeqInfo->inTr.IoVecBuffIndex = 0;
}

/******************************************************************************
 * The next group of functions handle output sequence/transfer state
 * transitions.
 *****************************************************************************/
/** @brief This function moves the output transfer pointer to next transfer.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_MoveNextTransfer
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo
)
{
   pOutSeqInfo->uTrIndex++;
   if ( pOutSeqInfo->uTrIndex == pOutSeqInfo->pSeq->uNumTransfers ) {
      /* all data has been pushed into the output fifo. */
      /*
       * QCTDD00309260 prevents the driver from adding a FLUSH_STOP tag to
       * terminate the transfer if fifo/block mode.
       * QCTDD00309260:
       * When using i2c version2 tags in non-BAM mode, MAX_OUTPUT_DONE_FLAG
       * is not asserted if the last tag is not DATA_WR_*, DATA_RD_* or
       * NOP_WAIT.
       */
      /* Do not add FLUSH_STOP to handle QCTDD00352578.
       *  See issue above  QCTDD00309260 
       */
      if ( pOutSeqInfo->pSeq->bBamTransfer ) { 
         pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_FLUSH_STOP;
      }
      else {
         pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO_DONE;
      }
      
   }
   else { /* initialize next transfer.*/
      pOutSeqInfo->pTransfer++; /* move to next transfer. */
      QUP_OutSeqInitTransfer(pOutSeqInfo);
   }
}
/** @brief This function handles transfer transition from StartTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_StartTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   if ( (I2cTranDirOut == pOutSeqInfo->pTransfer->eTranDirection ) &&
        (0 == pOutSeqInfo->pTransfer->uTrSize) ){
      *pData = QUPE_TAG_START_STOP;
   }
   else {
      *pData = QUPE_TAG_START;
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_SLAVE_ADDR;
   pOutSeqInfo->uCurOutBytes++;
}

/** @brief This function handles transfer transition from SlaveAddress.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_SlaveAddressNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   
   utmpVal =  (uint8) (pOutSeqInfo->pTransfer->tranCfg.uSlaveAddr<<1);
   if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
      utmpVal |=    1;
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
   else if ( 0 == pOutSeqInfo->pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}

/** @brief This function handles transfer transition from RdDataTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_RdDataTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   uint32 uBytesLeft;
   
   if ( (pOutSeqInfo->pTransfer->uTrSize -
         pOutSeqInfo->outTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH ) {
      pOutSeqInfo->outTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      utmpVal =    QUPE_TAG_DATA_READ;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pOutSeqInfo->pTransfer->uTrSize - pOutSeqInfo->outTr.uNextDataSegIndex -1;
      if ( 1 == uBytesLeft ) {
         pOutSeqInfo->outTr.uNextDataSegIndex--;
      }
   }
   else {
      pOutSeqInfo->outTr.uNextDataSegIndex =
              pOutSeqInfo->pTransfer->uTrSize;
      /* TODO: not supported currently, support will be added in v2
        if ((I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt) ||
            (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt)) {
           uByteVal = QUPE_TAG_DATA_READ_NACK;
        }
        else { */
         utmpVal = QUPE_TAG_DATA_READ_STOP;
      /*} */
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}

/** @brief This function handles transfer transition from WrDataTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_WrDataTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   if ( (pOutSeqInfo->pTransfer->uTrSize -
         pOutSeqInfo->outTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH) {// shift the next segment
      pOutSeqInfo->outTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      utmpVal =    QUPE_TAG_DATA_WRITE;
   }
   else {
      pOutSeqInfo->outTr.uNextDataSegIndex =
                        pOutSeqInfo->pTransfer->uTrSize;
      if ( (I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt)   ||
           (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt) ) {
         utmpVal =    QUPE_TAG_DATA_WRITE;
      }
      else {
         utmpVal = QUPE_TAG_DATA_WRITE_STOP;
      }
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}



/** @brief This function handles transfer transition from DataCnt.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_DataCntNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 uByteCnt;

   uByteCnt = (uint8) (pOutSeqInfo->outTr.uNextDataSegIndex -
                            pOutSeqInfo->outTr.uDataIndex);
   if ( 0 == uByteCnt ) { 
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
      pOutSeqInfo->outTr.uDataIndex += uByteCnt;
      if ( pOutSeqInfo->outTr.uDataIndex == pOutSeqInfo->pTransfer->uTrSize ) {
         //last read and bam transferthen next is EOT
         if ( pOutSeqInfo->pSeq->bBamTransfer &&
              (pOutSeqInfo->pSeq->inSeqInfo.uLastTrIndex == pOutSeqInfo->uTrIndex) ) {
            pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_EOT_TAG;
         }
         else {
            QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
         }
      }
      else {
         pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
      }
   }
   else {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_VALUE;
   }
   pOutSeqInfo->uCurOutBytes++;
   *pData = uByteCnt;
}

/** @brief This function handles transfer transition from DataValue.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_DataValueNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   
   if ( pOutSeqInfo->outTr.IoVecBuffIndex == pOutSeqInfo->outTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pOutSeqInfo->outTr.pTrIOVec++;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   /* gather index. */
   utmpVal = pOutSeqInfo->outTr.pTrIOVec->pBuff[pOutSeqInfo->outTr.IoVecBuffIndex++];
   pOutSeqInfo->outTr.uDataIndex++;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;

   if ( pOutSeqInfo->outTr.uDataIndex == pOutSeqInfo->pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   } 
   else if ( pOutSeqInfo->outTr.uDataIndex ==
             pOutSeqInfo->outTr.uNextDataSegIndex ) {
      /* next segment. */
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
}

#if 0
/** @brief This function handles transfer transition from DataValue.

    This function moves a buffer at a time.
    This is used in bam mode only all other functions move a byte at a time.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     ppData        Pointer to pointer to buffer where the data chunk is.
    @param[out]     pSize         Pointer to size of the data chunk.
    @return         Nothing.
  */
static void
QUP_OutFsm_DataValueNextBuff
(
   I2CDEVQUP_OutSeqInfo  *pOutSeqInfo,
   uint8                **ppData,
   uint32                *pSize
)
{
   uint8 *ptmpData;
   uint32 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;
   
   if ( pOutSeqInfo->outTr.IoVecBuffIndex == pOutSeqInfo->outTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pOutSeqInfo->outTr.pTrIOVec++;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   
   ptmpData = &pOutSeqInfo->outTr.pTrIOVec->pBuff[pOutSeqInfo->outTr.IoVecBuffIndex];
   uBuffSize = pOutSeqInfo->outTr.pTrIOVec->uBuffSize -
                pOutSeqInfo->outTr.IoVecBuffIndex;
   uNumTotalRemaining = pOutSeqInfo->pTransfer->uTrSize -
                    pOutSeqInfo->outTr.uDataIndex;
   uNumToEndDataSeg = pOutSeqInfo->outTr.uNextDataSegIndex - pOutSeqInfo->outTr.uDataIndex;

   /* lowest number. */
   if ( uBuffSize > uNumTotalRemaining ) {
      uBuffSize = uNumTotalRemaining;
   }
   if ( uBuffSize > uNumToEndDataSeg ) {
      uBuffSize = uNumToEndDataSeg;
   }

   /* advance data pointer. */
   pOutSeqInfo->outTr.uDataIndex     += uBuffSize;
   pOutSeqInfo->outTr.IoVecBuffIndex += uBuffSize;

   if ( uBuffSize == uNumTotalRemaining ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   } 
   else if ( uBuffSize == uNumToEndDataSeg ) {
      /* next segment. */
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
   pOutSeqInfo->uCurOutBytes += uBuffSize;
   *ppData = ptmpData;
   *pSize = uBuffSize;
}
#endif

#if 0
/** @brief This function handles transfer transition from EotTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_EotTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,
   uint8                *pData
)
{
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;

   pInSeqInfo = &pOutSeqInfo->pSeq->inSeqInfo;

   QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   pOutSeqInfo->uCurOutBytes++;
   pInSeqInfo->uInPadCnt++; /* eot will be copied to input. */
   *pData = QUPE_TAG_INPUT_EOT;
}
#endif

/** @brief This function handles transfer transition from FlushTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_FlushTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,
   uint8                *pData
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;

   pSeqInfo = pOutSeqInfo->pSeq;
   pOutSeqInfo->uCurOutBytes++;

   *pData = QUPE_TAG_FLUSH_STOP;
   
   /* Pad with extra FLUSH_STOP under this condition */
   if ( (!pSeqInfo->bBamTransfer) ||
        ( 3 != (pOutSeqInfo->uCurOutBytes & 0x3)) ) {
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO_DONE;
   }
   
}

/** @brief This function drives output seq state transitions.

    It advances the output sequence one slot at the time.
    This function is used to fill the output fifo in fifo/block mode.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static uint32 
QUP_GetNextOutSeqSlot
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo   
)
{
   uint32                          uSlot;
   uint32                          uSlotByteIndex;
   uint8                           uByteVal;

   uSlot = 0;
   uSlotByteIndex = 0;

   /* create slot. */
   while ( (uSlotByteIndex < 4) &&
	        (pOutSeqInfo->eOutputState != I2CDEVQUP_OUTPUT_FILL_FIFO_DONE) ) {

      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         case I2CDEVQUP_TR_STATE_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_StartTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_SLAVE_ADDR      :
               QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG     :
               QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_WR_DATA_TAG     :
               QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_OutFsm_DataCntNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_OutFsm_DataValueNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_FLUSH_STOP:
            QUP_OutFsm_FlushTagNext(pOutSeqInfo, &uByteVal);
            break;
         
         default:
              /*TODO: set error sate of invalid output seq state. */
              return 0;
      }
      uSlot |= (((uint32)uByteVal ) << (uSlotByteIndex * 8));
      uSlotByteIndex++;
   }
   return uSlot;   
}



#if 0
/**
    This function handles bam callbacks.

    @param[in]      bamPlatRes  Bam callback result structure.
    @return         Nothing.
  */
void 
QUP_BamCallback
(
   I2CPLATBAM_CallbackResultType bamPlatRes
)
{
   I2CDEVQUP_Device               *pDev;
   I2CDEVQUP_QupInfo              *pQupInfo;

   pDev = (I2CDEVQUP_Device *)bamPlatRes.pArg;

   pQupInfo = &pDev->qupInfo;
   
   I2CSYS_EnterCriticalSection(pDev->hOperationSync);
   pQupInfo->eBamEvt = bamPlatRes.bamEvt;
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);
   QUP_PerformInputOutput(pDev);
}

#endif

#if 0
/** @brief This function drives output seq state transitions in bam mode.

    It advances the output sequence one descriptor at the time.
    This function is used to fill the output fifo in bam mode.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pBamIoVec     Pointer to bam io vector.
    @return         Nothing.
  */
static void
QUP_GetNextOutSeqDescriptor
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   uint8                          *pDataAddr; /* used for unbuffered tr. */
   uint32                          uDataSize; /* used for unbuffered tr. */
   uint8                          *pBamBuffAddr;/* used for buffered tr. */
   uint16                          uFlags;
   boolean                         bDescReady;
   boolean                         bUsesBamBuffer;

   uFlags = 0;
   pDataAddr = NULL;
   uDataSize = 0;

   pBamBuffAddr = &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex];

   bDescReady = FALSE;
   bUsesBamBuffer = TRUE;
   /* The driver loops under two scenarios:
      1. no descriptor is ready to be pushed into the bam
         and the transfer is buffered (tags,addr ... etc)
      2. no descriptor is ready and the client data is being
         pushed into the bam so no need for buffering.  */
   /* create descriptor. */
   while( (!bDescReady) &&
          ((pOutSeqInfo->uBamBuffIndex < BAM_TMPBUFF_MAX) ||
          (I2CDEVQUP_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState)) &&
          (pOutSeqInfo->eOutputState != I2CDEVQUP_OUTPUT_FILL_FIFO_DONE) ) {
      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         case I2CDEVQUP_TR_STATE_START_TAG       : /* start of a transfer. */
            QUP_OutFsm_StartTagNext(pOutSeqInfo,
                                    &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_SLAVE_ADDR      :
            QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,
                                        &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
            QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_WR_DATA_TAG        :
            QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
            QUP_OutFsm_DataCntNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            /* if next state is data value then flush the buffer. */
            if ( I2CDEVQUP_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState ) {
               bDescReady = TRUE;
            }
            break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
            QUP_OutFsm_DataValueNextBuff(pOutSeqInfo,&pDataAddr,&uDataSize);
            bDescReady = TRUE;
            bUsesBamBuffer = FALSE;/* the data is the raw clnt data. */
            break;
         case I2CDEVQUP_TR_STATE_EOT_TAG:
            QUP_OutFsm_EotTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_FLUSH_STOP:
            QUP_OutFsm_FlushTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         default:
            // do nothing
            break;
      }
   }
   /* last descriptor for output transfer. */
   if ( pOutSeqInfo->eOutputState == I2CDEVQUP_OUTPUT_FILL_FIFO_DONE ) {
      uFlags |= ( PLAT_BAM_DESC_FLAG_EOT |
                  PLAT_BAM_DESC_FLAG_NWD |
                  PLAT_BAM_DESC_FLAG_INT );
   }
   else if ( (BAM_TMPBUFF_MAX == pOutSeqInfo->uBamBuffIndex) &&
             (I2CDEVQUP_TR_STATE_DATA_VALUE != pOutSeqInfo->outTr.eOutTrState) ) {
      /* Buffered and unbuffered pointing descriptors can be
         interleavead and when the buffer is full and more data
         are to be placed on the buffer then the buffer must be
         flushed first. */
      uFlags |= PLAT_BAM_DESC_FLAG_INT;
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_STALLED;
      pOutSeqInfo->uBamBuffIndex = 0;
   }
   
   /* pick the buffer and size. */
   if (!bUsesBamBuffer) {/* this descriptor points to clnt data. */
      pBamIoVec->pBuff = pDataAddr;
      pBamIoVec->buffSize = (uint16)uDataSize;
   }
   else {
      /* this descriptor points to the allocated buffer.*/
      pBamIoVec->pBuff = pBamBuffAddr;
      pBamIoVec->buffSize = (uint16) (&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex] - pBamBuffAddr);
      pBamBuffAddr += pBamIoVec->buffSize; 
   }
   /* push slot into output fifo. */
   pBamIoVec->eDir = I2CPLATBAM_DIR_OUTPUT;
   pBamIoVec->flags = uFlags;
}
#endif

#if 0
/**
    This function fills the output queue one descriptor at a time.
   
    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_BamPerformOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   uint32                          uNumFreeDesc;
   I2CPLATBAM_BamIoVecType         bamIoVec;
  
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   /* If transfer to be queued Check if there is space in the bam queue. */
   uNumFreeDesc = I2CPLATDEV_BamGetPipeFreeSpace(pDev->hDevTgt,
                                                 I2CPLATBAM_DIR_OUTPUT);

   /* If output is done update the state of the output sequence. */
   while( (I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (uNumFreeDesc > 0) ) {
      QUP_GetNextOutSeqDescriptor(pOutSeqInfo, &bamIoVec);
      /* last free bam descriptor, flush the pipe.*/
      if ( (1 == uNumFreeDesc) &&
           (0 == (bamIoVec.flags & (PLAT_BAM_DESC_FLAG_EOT | PLAT_BAM_DESC_FLAG_NWD)))
         ) { 
         bamIoVec.flags |= PLAT_BAM_DESC_FLAG_INT;
         pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_STALLED;
      }
      I2CPLATDEV_PlatBamTransfer(pDev->hDevTgt,
                                 &bamIoVec,
                                 &uNumFreeDesc,
                                 QUP_BamCallback,
                                 (void *)pDev);
   }
}
#endif

/******************************************************************************
 * The next group of functions handle input sequence/transfer state
 * transitions.
 *****************************************************************************/

/** @brief This function moves the input transfer pointer to next transfer.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_MoveNextTransfer
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uInTrCnt--;
   if ( 0 == pInSeqInfo->uInTrCnt ) {
      /* all data has been read from input fifo */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER;
      return;
   }
   
   do {
      pInSeqInfo->uTrIndex++;
      pInSeqInfo->pTransfer = &pInSeqInfo->pSeq->pTransfer[pInSeqInfo->uTrIndex];
      if ( I2cTranDirIn == pInSeqInfo->pTransfer->eTranDirection ) {
         break;
      }
   } while(pInSeqInfo->uTrIndex != pInSeqInfo->uLastTrIndex);

   QUP_InSeqInitTransfer(pInSeqInfo);
}

/** @brief This function handles transfer transition from RdDataTag.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataTagNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   uint32 uBytesLeft;

   if ( (pInSeqInfo->pTransfer->uTrSize -
         pInSeqInfo->inTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH ) {
      pInSeqInfo->inTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pInSeqInfo->pTransfer->uTrSize - pInSeqInfo->inTr.uNextDataSegIndex -1;
      if (1 == uBytesLeft) {
         pInSeqInfo->inTr.uNextDataSegIndex--;
      }
   }
   else {
      pInSeqInfo->inTr.uNextDataSegIndex =
              pInSeqInfo->pTransfer->uTrSize;
      /* TODO: not supported currently, support will be added in v2
      if ((I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt) ||
            (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt)) {
         uByteVal = QUPE_TAG_DATA_READ_NACK;
      }
      else {
      } */
   }
   pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
}

/** @brief This function handles transfer transition from RdDataCntTag.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataCntTagNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   uint8 utmpVal;

   utmpVal = (uint8)(pInSeqInfo->inTr.uNextDataSegIndex -
                            pInSeqInfo->inTr.uDataIndex);
   if ( 0 == utmpVal ) { 
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } else {
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_VALUE;
   }
}

/** @brief This function handles transfer transition from RdDataValue one byte at a time.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[out]     uByte         Data with this single byte read from this state-transition.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataValueSingleNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo,uint8 uByte
)
{
   uint8 *pVal;

   if ( pInSeqInfo->inTr.IoVecBuffIndex == pInSeqInfo->inTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pInSeqInfo->inTr.pTrIOVec++;
      pInSeqInfo->inTr.IoVecBuffIndex = 0;
   }
   /* gather index. */
   pVal = &pInSeqInfo->inTr.pTrIOVec->pBuff[pInSeqInfo->inTr.IoVecBuffIndex++];
   *pVal = uByte;
   pInSeqInfo->inTr.uDataIndex++;
   if ( pInSeqInfo->inTr.uDataIndex == pInSeqInfo->pTransfer->uTrSize ) {
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } 
   else if ( pInSeqInfo->inTr.uDataIndex ==
             pInSeqInfo->inTr.uNextDataSegIndex ) {
      /* next segment. */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
}

#if 0
/** @brief This function handles transfer transition from RdDataValue a buffer at a time.

    The data read will be place on the supplied client buffer of the specified size.
    Used in bam mode.

    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[out]     pClntDtBuff   Pointer to a client buffer pointer.
    @param[out]     pClntDtSize   Pointer to buffer size.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataValueMultipleNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo,
   uint8              **pClntDtBuff,
   uint16              *pClntDtSize
)
{
   uint8 *pTmpClntDtBuff;
   uint16 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;

   if ( pInSeqInfo->inTr.IoVecBuffIndex == pInSeqInfo->inTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pInSeqInfo->inTr.pTrIOVec++;
      pInSeqInfo->inTr.IoVecBuffIndex = 0;
   }

   /* gather index. */
   pTmpClntDtBuff = &pInSeqInfo->inTr.pTrIOVec->pBuff[pInSeqInfo->inTr.IoVecBuffIndex];
   uBuffSize = (uint16)(pInSeqInfo->inTr.pTrIOVec->uBuffSize - pInSeqInfo->inTr.IoVecBuffIndex);

   uNumTotalRemaining = pInSeqInfo->pTransfer->uTrSize - pInSeqInfo->inTr.uDataIndex;
   uNumToEndDataSeg = pInSeqInfo->inTr.uNextDataSegIndex - pInSeqInfo->inTr.uDataIndex;

   /* lowest number. */
   if ( uBuffSize > uNumTotalRemaining ) {
      uBuffSize = (uint16) uNumTotalRemaining;
   }
   if ( uBuffSize > uNumToEndDataSeg ) {
      uBuffSize = (uint16) uNumToEndDataSeg;
   }

   /* advance data pointer. */
   pInSeqInfo->inTr.uDataIndex     += uBuffSize;
   pInSeqInfo->inTr.IoVecBuffIndex += uBuffSize;

   if ( pInSeqInfo->inTr.uDataIndex == pInSeqInfo->pTransfer->uTrSize ) {
      /* if the last transfer then move to eot. */
      if ( 1 == pInSeqInfo->uInTrCnt ) {
         pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_PAD;
      }
      else {
         QUP_InFsm_MoveNextTransfer(pInSeqInfo);
      }
   } 
   else if ( pInSeqInfo->inTr.uDataIndex ==
             pInSeqInfo->inTr.uNextDataSegIndex ) {
      /* next segment. */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
   *pClntDtBuff  = pTmpClntDtBuff;
   *pClntDtSize = uBuffSize;
}
#endif

#if 0
/** @brief This function handles transfer transition from RdTrailer.

    The new state is a wait for the memory to be flushed.

    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdTrailerNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_DONE;
}
#endif

/** @brief This function advances input sequence one slot at a time.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[in]      uSlot         Data slot received.
    @return         Nothing.
  */
static void
QUP_ProcessNextInputSlot
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   uint32                          uSlot
)
{
   uint32                          uSlotByteIndex;
   uint32                          uByteVal;

   uSlotByteIndex = 0;
   
   /* unpack slot. */
   while ( (uSlotByteIndex < 4) && 
           (I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER != pInSeqInfo->inTr.eInTrState) ) {
      uByteVal = (uSlot >> (uSlotByteIndex * 8)) &0xFF;
      uSlotByteIndex++;
      switch (pInSeqInfo->inTr.eInTrState) {
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueSingleNext(pInSeqInfo, (uint8) uByteVal);
               break;
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   } /* end of slot processing. */
   
   if ((pInSeqInfo->uNumInBytesReceived == pInSeqInfo->uTotalInBytes) ||
       (0 == pInSeqInfo->uInTrCnt))    {
      pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_DONE;
   }
}



/**
    This function advances the input sequence one io vector at a time.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
#if 0
/* This implementation generates multiple input descriptors.
   The V1 chip dosn't support canceling/closing multiple
   input descriptors in case of NACK or error.
 */ 
static void
QUP_GetNextInSeqDescriptor
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   uint8                          *pClntDtBuff;
   uint16                          uClntDtSize;
   uint16                          uTagDataSize;
   boolean                         bUsesBamBuffer;
   boolean                         bDescReady;

  
   pClntDtBuff  = NULL;
   uClntDtSize  = 0;
   uTagDataSize = 0;

   bDescReady = FALSE;
   bUsesBamBuffer = TRUE;

   pBamIoVec->flags = 0;

   /* unpack slot. */
   while( !bDescReady ) {
      switch ( pInSeqInfo->inTr.eInTrState ) {
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               uTagDataSize   = 2;
               bDescReady     = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = TRUE;
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueMultipleNext(pInSeqInfo,&pClntDtBuff, &uClntDtSize);
               bDescReady = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = FALSE;
               break;
         case I2CDEVQUP_TR_STATE_DATA_PAD      :
               QUP_InFsm_RdTrailerNext(pInSeqInfo);
               uTagDataSize   = pInSeqInfo->uInPadCnt;
               bDescReady     = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = TRUE;
               pBamIoVec->flags |= PLAT_BAM_DESC_FLAG_INT;
               break;   
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   }

   /* pick the buffer and size. */
   if ( !bUsesBamBuffer ) {
      /* this descriptor points to clnt data. */
      pBamIoVec->pBuff    = pClntDtBuff;
      pBamIoVec->buffSize = (uint16) uClntDtSize;
   }
   else {
      /* this descriptor points to the allocated buffer. */
      pBamIoVec->pBuff    = pInSeqInfo->aInBamBuff;
      pBamIoVec->buffSize = uTagDataSize; /* throw away tags. */
   }
   /* push slot into input fifo. */
   pBamIoVec->eDir = I2CPLATBAM_DIR_INPUT;
}
#endif

#if 0
/* This implementation scatters the data read
 * from bam into the client buffers.
 */ 
static void
QUP_ScatterInputData
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo
)
{
   uint8                          *pClntDtBuff;
   uint16                          uClntDtSize;
   uint8                          *pDescBuff;
   uint32                          uClntDataInd;

  
   pClntDtBuff  = NULL;
   uClntDtSize  = 0;
  

   pDescBuff = pInSeqInfo->inPhysMem.pVirtAddress;

   /* unpack slot. */
   while( pDescBuff < 
          (pInSeqInfo->inPhysMem.pVirtAddress + pInSeqInfo->inPhysMem.uSize) 
        ) {
      switch ( pInSeqInfo->inTr.eInTrState ) {
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               pDescBuff++; /* Throw tag byte */
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               pDescBuff++; /* Throw byte cnt */
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueMultipleNext(pInSeqInfo,&pClntDtBuff, &uClntDtSize);
               for ( uClntDataInd = 0; uClntDataInd < uClntDtSize; uClntDataInd++ ) {
                  pClntDtBuff[uClntDataInd] = pDescBuff[uClntDataInd];     
               }
               pDescBuff += uClntDtSize;
               break;
         case I2CDEVQUP_TR_STATE_DATA_PAD      :
               QUP_InFsm_RdTrailerNext(pInSeqInfo);
               pDescBuff++; /* Throw EOT */
               break;   
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   }
}
#endif

#if 0
/**
    This function makes the next input descriptor.
    Used in bam mode.

    @param[in, out] pInSeqInfo  Pointer to input sequence structure.
    @param[out]     pBamIoVec   Pointer to bam vector.
    @return         Nothing.
  */
static int32
QUP_GetNextInSeqDescriptor
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   int32                           res;
   
   pInSeqInfo->inPhysMem.uSize = pInSeqInfo->uTotalInBytes;
   res = I2CSYS_PhysMemAlloc(&pInSeqInfo->inPhysMem);
   if (I2C_RES_SUCCESS != res) {
      return I2CDEVQUP_ERR_INPUT_DESC_BUFFER_ALLOC_FAILED;
   }
   pInSeqInfo->bUseInPhysMem  = TRUE;
   pBamIoVec->eDir            = I2CPLATBAM_DIR_INPUT;
   pBamIoVec->pBuff           = pInSeqInfo->inPhysMem.pVirtAddress;
   pBamIoVec->buffSize        = (uint16) pInSeqInfo->uTotalInBytes;
   pBamIoVec->flags           = 0;

   return I2C_RES_SUCCESS;
}
#endif

#if 0
/**
    This function Performs input in bam mode.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_BamPerformInput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   uint32                          uNumFreeDesc;
   I2CPLATBAM_BamIoVecType         bamIoVec;
   int32                           res;

   pSeqInfo    = &pDev->seqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   /* Check if there is a transfer to be placed on the out fifo
      If transfer to be queued Check if there is space in the output fifo
      If to be queued and space continue state machine
      As we loop through the transfers we update read tr cnt
      If output is done update the state of the output sequence. */
   uNumFreeDesc = I2CPLATDEV_BamGetPipeFreeSpace(pDev->hDevTgt, I2CPLATBAM_DIR_INPUT);
   while( (I2CDEVQUP_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState) &&
          (uNumFreeDesc > 0) ) {
      res = QUP_GetNextInSeqDescriptor(pInSeqInfo, &bamIoVec);
      if ( I2C_RES_SUCCESS != res ) {
         pSeqInfo->ioRes.nOperationResult = res;     
         break;     
      }
      pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_WAIT_DESC_DATA_READY;     
      /* last free bam descriptor, flush the pipe. */
      /* v1 hw there is only one input descriptor.
      if ( (1 == uNumFreeDesc) &&
           (I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState) ) { 
         bamIoVec.flags |= PLAT_BAM_DESC_FLAG_INT;
         pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_STALLED;
      }
      */
      I2CPLATDEV_PlatBamTransfer(pDev->hDevTgt,
                                 &bamIoVec,
                                 &uNumFreeDesc,
                                 QUP_BamCallback,
                                 (void *)pDev);
   }
   if ( I2CDEVQUP_INPUT_COPY_INPUT_BUFFERS == pInSeqInfo->eInSeqState ) {
      QUP_ScatterInputData(pInSeqInfo);
   }
}
#endif

/** @brief This function Performs qup servicing.
 
    This function is the main entry point for qup servicing.
    It is called when events happen.
      
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_ServiceQUP
(
   I2CDEVQUP_Device *pDev
)
{
   I2CDEVQUP_QupInfo     *pQupInfo;
   I2CDEVQUP_QupIsrHwEvt *pIsrEvt;
   QupRegsType           *pQupRegs;
   I2cRegsType           *pI2cRegs;
   uint32                 uQupErrors;
   uint32                 uOperational;
   uint32                 uOperationaSvcFlags;
   uint32                 i2cStatus;
   uint32                 uI2cErrors;
   
      
   
   pI2cRegs = pDev->qupInfo.pI2cRegs;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pQupInfo = &pDev->qupInfo;
   pIsrEvt  = pQupInfo->hwEvtQueue.pHead;


   /* Check operating and error status.                                      */
   /* According to the HW designer both the QUP error flags and the minicore
      status register need to be read and then the interrupts can be cleared.
      If status registers are not read then the interrupt will remain set.
      I have seen this happen for the interrupts asserted by the minicore.   */
   uQupErrors   = pQupRegs->ERROR_FLAGS & QUPE_ERRSTATE_FIELD;
   uOperational = pQupRegs->OPERATIONAL;
   i2cStatus    = pI2cRegs->STATUS;
   uI2cErrors   = i2cStatus & I2C_OP_STATUS_ERROR_FIELD;

   uOperationaSvcFlags =
       (uOperational & 
       (QUPE_OPSTATE_OUTPUT_SERVICE | QUPE_OPSTATE_INPUT_SERVICE));

   /* Check if service events are set return if not. */
   if ( (!uQupErrors) &&
        (!uI2cErrors) &&
        (!uOperationaSvcFlags)
      )
   {
      return;     
   }

   pQupInfo->uIrqCnt++; /* Count interrupts */
   pIsrEvt->uIrqCnt = pQupInfo->uIrqCnt;
   pIsrEvt->uQupErrors   = uQupErrors;
   pIsrEvt->uOperational = uOperational;
   pIsrEvt->i2cStatus    = i2cStatus;

   QUP_GetTransferCnt(pDev, &pIsrEvt->uOutTranCnt, &pIsrEvt->uInTranCnt);

   /* Check for errors.                                                      */
   if ( uQupErrors ){
      pQupRegs->ERROR_FLAGS = uQupErrors;
   }

   if( uI2cErrors ) {
      /* Should reset the core to protect against re-entering the ISR due to
         the same error condition being set.                              */
      /* TODO: _qupe_LogRegs(pDev->devProps.virtBlockAddr); */
      
      pI2cRegs->STATUS = uI2cErrors;
   }
   if ( uOperationaSvcFlags ) {
      pQupRegs->OPERATIONAL = uOperationaSvcFlags;
   }
   
#if 0
   if ( QUPE_IO_MODE_BAM == pQupInfo->eOutFifoMode ) {
      /* In bam mode we only get here if there are errors.
         The core is in pause state.
       */
      _qupe_I2cFlush(pDev->devProps.virtBlockAddr);
      I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RUN);
   }
#endif

   if ( !I2CDEVQUP_QupHwEvtQueue_IsFull(&pQupInfo->hwEvtQueue) ) {
      I2CDEVQUP_QupHwEvtQueue_HeadNext(&pQupInfo->hwEvtQueue);     
   }

}

/** @brief This is a hook for the ISR servicing the qup.

    This Function is called by the framework to handle the irq
    events triggered by the core. This function does not do
    data transfer. The IST thread handles data transfer.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
static void
I2CDEVQUP_ISR_HOOK
(
   void *pdata
)
{
   if ( NULL == pdata ) {
      return;
   }
   QUP_ServiceQUP((I2CDEVQUP_Device*)pdata);
}

/** @brief Configures the device.

    This function configures the device and registers the
    IST hook.

    @param[in] uPlatDevId    Platform Device ID.
    @param[in] phDev         Pointer to device handle.

    @return          int32 .
  */
static int32
I2CDEVQUP_SetupHw
(
   I2CDEVQUP_Device *pDev
)
{
   int32                 res;
   I2CDEVQUP_HwInfoType *pHwInfo;
   QupRegsType          *pQupRegs;
   uint32                uIoMode;
   uint32                uFifoSzFld;
   uint32                uBlkSzFld;

   pHwInfo  = &pDev->devHwInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;

   res = I2CDEVQUP_InitQupCore(pDev);
   if ( I2C_RES_SUCCESS != res ) {
      return res;     
   }

   uIoMode = pQupRegs->IO_MODES;

   uBlkSzFld = (uIoMode & QUP_IO_MODES_OUTPUT_BLOCK_SIZE_BMSK) >>
                   QUP_IO_MODES_OUTPUT_BLOCK_SIZE_SHFT;
   pHwInfo->uNumOutputBlockSlots = 1 << (uBlkSzFld + ((0xE >> uBlkSzFld) & 0x1));
   
   uFifoSzFld = (uIoMode & QUP_IO_MODES_OUTPUT_FIFO_SIZE_BMSK) >>
                   QUP_IO_MODES_OUTPUT_FIFO_SIZE_SHFT;
   pHwInfo->uNumOutputBlocks  = 1 << (uFifoSzFld + 1);

   pHwInfo->uNumOutputSlots = pHwInfo->uNumOutputBlocks * pHwInfo->uNumOutputBlockSlots;

   uBlkSzFld = (uIoMode & QUP_IO_MODES_INPUT_BLOCK_SIZE_BMSK) >>
                   QUP_IO_MODES_INPUT_BLOCK_SIZE_SHFT;
   pHwInfo->uNumInputBlockSlots  = 1 << (uBlkSzFld + ((0xE >> uBlkSzFld) & 0x1));

   uFifoSzFld = (uIoMode & QUP_IO_MODES_INPUT_FIFO_SIZE_BMSK) >>
                   QUP_IO_MODES_INPUT_FIFO_SIZE_SHFT;
   pHwInfo->uNumInputBlocks   = 1 << (uFifoSzFld + 1);
   
   pHwInfo->uNumInputSlots = pHwInfo->uNumInputBlocks * pHwInfo->uNumInputBlockSlots;
   
   pHwInfo->uHWVersion = pQupRegs->HW_VERSION;

   if ( pDev->devProps.bInterruptBased ) {
      res = I2CSYS_RegisterISTHooks(pDev->hDevTgt,
                                    I2CDEVQUP_ISR_HOOK,
                                    (void*) pDev,
                                    I2CDEVQUP_IST_HOOK,
                                    (void*) pDev);
      if ( I2C_RES_SUCCESS != res ) {
         return (int32)res;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_ISR_IST_Registration_Done;
   }
   pDev->devInitState |= I2CDEVQUP_InitState_HardwareInit_Done;

   return I2C_RES_SUCCESS;
}





/**
    This function free's the bus.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_FreeI2cBus
(
   I2CDEVQUP_Device               *pDev
)
{
   I2cRegsType      *pI2cRegs;
   uint32            uI2cStatus;
   boolean           bBusFree;


   pI2cRegs = pDev->qupInfo.pI2cRegs;

   uI2cStatus = pI2cRegs->STATUS;
   bBusFree = !(uI2cStatus & I2C_STATUS_BUS_ACTIVE_BMSK);
   if ( bBusFree ) {
      return TRUE;
   }

   /* clear bus resets the core in virtio. */
   pI2cRegs->BUS_CLEAR = 1;
   I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);
   //_qupe_Configure(pDev->devProps.virtBlockAddr);
   uI2cStatus = pI2cRegs->STATUS;
   bBusFree = !(uI2cStatus & I2C_STATUS_BUS_ACTIVE_BMSK);
   
   return bBusFree;
}



/*
static boolean QUP_PauseQup
(
   I2CDEVQUP_Device               *pDev
)
{
   if (_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr)){
      return TRUE;
   }

   _qupe_ClearI2CBus(pDev->devProps.virtBlockAddr);
   I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);

   return (!_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr));
}
*/

/**
    This Checks if the transfer has finished.
    It checks if the transfer has completed transmiton and/or
    reception or it is interrupted due to rerrors.

    @param[in, out] pDev     Pointer to Qup device structure.
    @param[in]      pSeqInfo Pointer to sequence info.
    @return         TRUE if the transfer completed or was interrupted,
                    FALSE otherwise.
  */
static boolean
QUP_IsTransferFinished
(
   I2CDEVQUP_Device               *pDev,
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo
)
{

   if ( I2C_RES_SUCCESS != pDev->seqInfo.ioRes.nOperationResult ) {
      return TRUE;
   }
   else if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE ==
              pSeqInfo->outSeqInfo.eOutputState) &&
             (0 == pSeqInfo->inSeqInfo.uTotalInBytes) ) {
      return TRUE;
   }
   else if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE ==
              pSeqInfo->outSeqInfo.eOutputState) &&
             (I2CDEVQUP_INPUT_RECEIVE_DONE ==
              pSeqInfo->inSeqInfo.eInSeqState) ) {
      return TRUE;
   }

   return FALSE;
}

/** @brief Initializes output sequence info structures.

    Initializes output sequence info structures.

    @param[out] pOutSeqInfo   Pointer to out sequence struct.

    @return                   Nothing.
  */
void
I2CDEVQUP_InitOutSeqInfo
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo
)
{
   pOutSeqInfo->pTransfer    = NULL;
   pOutSeqInfo->uOutTrCnt    = 0;
   pOutSeqInfo->uTrIndex     = 0;
   pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO;
   pOutSeqInfo->uBamBuffIndex = 0;
   pOutSeqInfo->uCurOutBytes  = 0;
   pOutSeqInfo->uNumOutDtBytes = 0;
   pOutSeqInfo->uTotalOutBytes = 0;
}

/** @brief Initializes input sequence info structures.

    Initializes input sequence info structures.

    @param[out] pInSeqInfo    Pointer to input info sequence.

    @return                   Nothing.
  */
void
I2CDEVQUP_InitInSeqInfo
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uNumInBytesReceived      = 0;
   pInSeqInfo->eInSeqState              = I2CDEVQUP_INPUT_IDDLE;
   pInSeqInfo->pTransfer                = NULL;
   pInSeqInfo->uInTrCnt                 = 0;
   pInSeqInfo->uTrIndex                 = 0;
   pInSeqInfo->uLastTrIndex             = 0;
   pInSeqInfo->uNumInDtBytes            = 0;
   pInSeqInfo->uTotalInBytes            = 0;
   pInSeqInfo->uInPadCnt                = 0;
   //pInSeqInfo->bUseInPhysMem            = FALSE;
}


void
I2CDEVQUP_ConfigureSequenceTransferIo
(
   I2CDEVQUP_Device                    *pDev,
   I2cSequence                         *pSeq
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   uint32 uTranIndex;
   I2cTransfer                    *pTransfer;
   uint32                          uNumInBytes, uNumOutBytes;
   

   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   pSeqInfo->pTransfer              = pSeq->pTransfer;
   pSeqInfo->uNumTransfers          = pSeq->uNumTransfers;
   pSeqInfo->ioRes.uInByteCnt       = pSeqInfo->ioRes.uOutByteCnt = 0;
   pSeqInfo->ioRes.nOperationResult = I2C_RES_SUCCESS;
   pSeqInfo->bBamTransfer           = FALSE;
   pSeqInfo->eSeqState              = I2CDEVQUP_SEQSTATE_INPUT_OUTPUT;

   I2CDEVQUP_InitOutSeqInfo(pOutSeqInfo);
   pOutSeqInfo->pTransfer       = pSeqInfo->pTransfer;
   pOutSeqInfo->uOutTrCnt       = pSeqInfo->uNumTransfers;
   QUP_OutSeqInitTransfer(pOutSeqInfo);

   I2CDEVQUP_InitInSeqInfo(pInSeqInfo);
  
   pTransfer = pOutSeqInfo->pTransfer; /* get first transfer. */
   
   for (uTranIndex = 0; uTranIndex < pSeqInfo->uNumTransfers; uTranIndex++) {
      pTransfer = &pSeq->pTransfer[uTranIndex]; /* get transfer */
      
      if ( I2cTranDirIn == pTransfer->eTranDirection ) {
         pInSeqInfo->uInTrCnt++;
         if ( 1 == pInSeqInfo->uInTrCnt ) { /* first read of the sequence. */
            pInSeqInfo->pTransfer   = pTransfer;  /* first read transfer. */
            pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO;
            pInSeqInfo->uTrIndex    = uTranIndex;
            QUP_InSeqInitTransfer(pInSeqInfo);
         }
         pInSeqInfo->uLastTrIndex = uTranIndex;
         pInSeqInfo->uNumInDtBytes += pTransfer->uTrSize;
      }
      else {
         pOutSeqInfo->uNumOutDtBytes += pTransfer->uTrSize;
      }
      I2CDEVQUP_CalcTransferInOutSize(pTransfer,&uNumInBytes, &uNumOutBytes);
      //if ( I2CPLATDEV_IsBamEnabled(pDev->hDevTgt) ) {
      //   if ( (uNumInBytes  >= pDev->devProps.uBamThreshold) ||
      //        (uNumOutBytes >= pDev->devProps.uBamThreshold) ) {
      //      pSeqInfo->bBamTransfer = TRUE;
      //   }
      //}
      pInSeqInfo->uTotalInBytes   += uNumInBytes;
      pOutSeqInfo->uTotalOutBytes += uNumOutBytes;
   }

#if 0
   /* adjust for EOT TAG and FLUSH_TAG to mark end of input. */
   if ( pSeqInfo->bBamTransfer ) {
      pOutSeqInfo->uTotalOutBytes++; /* Account for FLUSH_STOP */

      /* Padding to handle QCTDD00347454.
         If transfer_length %4 = 3 pad with additional
         FLUSH_STOP.
       */
      if ( 3 == (pOutSeqInfo->uTotalOutBytes & 0x3) ) {
         pOutSeqInfo->uTotalOutBytes++;      
      }

      if ( pInSeqInfo->uTotalInBytes ) {
         pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_PUSH_DESCRIPTORS;     
         pOutSeqInfo->uTotalOutBytes++; /* Account for EOT */
         pInSeqInfo->uTotalInBytes++;   /* Account for EOT */
      }
   }
#endif

   pSeqInfo->uSeqTimeoutUs =
      (pInSeqInfo->uTotalInBytes + pOutSeqInfo->uTotalOutBytes) *
      pDev->clntCfg.uByteTransferTimeoutUs;
}

/** @brief Packs the operational state for debug.
    @param[in]  pQupAddr index of the qup.
    @return     28 bit value indicating state.
  */
/*
static uint32 QUPE_Pack28_OperationalState
(
   I2CDEVQUP_Device                    *pDev
)
{
   QupRegsType                    *pQupRegs;
   I2cRegsType                    *pI2cRegs;   
   uint32 uI2cStatusVal;
   uint32 uI2cStatusPacked;
   uint32 uOperationalVal;
   uint32 uOpPacked;
   uint32 val;

   pQupRegs = pDev->qupInfo.pQupRegs;
   pI2cRegs = pDev->qupInfo.pI2cRegs;

   uI2cStatusVal = pI2cRegs->STATUS;
   uI2cStatusPacked = (uI2cStatusVal & 
            (I2C_STATUS_BUS_ACTIVE_BMSK       |
             I2C_STATUS_BUS_MASTER_BMSK       |
             I2C_STATUS_DATA_STATE_BMSK       |
             I2C_STATUS_CLK_STATE_BMSK        |
             I2C_STATUS_OUTPUT_FSM_STATE_BMSK |
             I2C_STATUS_INPUT_FSM_STATE_BMSK) 
         );
   uI2cStatusPacked |= ((uI2cStatusVal &
                         (I2C_STATUS_I2C_SCL_BMSK | I2C_STATUS_I2C_SDA_BMSK)
                        ) >>
                       3);
                    
   uI2cStatusPacked = uI2cStatusPacked >> 8; // I2C_STATUS fields packed in val[16:0]
   
   uOperationalVal = pQupRegs->OPERATIONAL;

   // Bits [5:4] 
   uOpPacked = (uOperationalVal &
                  (QUPE_OPSTATE_INPUT_FIFO_NOT_EMPTY |
                   QUPE_OPSTATE_OUTPUT_FIFO_NOT_EMPTY) );
   
   uOpPacked |= ((uOperationalVal &
                   (QUPE_OPSTATE_OUTPUT_SERVICE       |
                    QUPE_OPSTATE_INPUT_SERVICE        |
                    QUPE_OPSTATE_MAX_INPUT_DONE       |
                    QUPE_OPSTATE_MAX_OUTPUT_DONE      |
                    QUPE_OPSTATE_OUTPUT_SERVICE       |
                    QUPE_OPSTATE_INPUT_SERVICE        |
                    QUPE_OPSTATE_NWD                  |
                    QUPE_OPSTATE_DONE_TOGGLE          )
                   ) 
                >> (QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT - QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT - 1));

   uOpPacked = uOpPacked >> QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT; 

   // Bits [9:0]
   val = uI2cStatusPacked | (uOpPacked << 17);
  
   val |= (1 <<27);//1bit mark as an operational status within timeout class

   return val;
}
*/

/** @brief Waits for completion.
    @param[in] pDev         Pointer to qup device structure.
    @return                 Nothing.
  */
static void
QUP_WaitForCompletion
(
   I2CDEVQUP_Device                    *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_QupInfo              *pQupInfo;
   uint32                          uTmpTimeout;
   boolean                         bTimeout      = FALSE;
   int32                           osRes;


   pSeqInfo = &pDev->seqInfo;
   pQupInfo = &pDev->qupInfo;
   if (pDev->devProps.bInterruptBased) /* Interrupt based wait.          */
   {
      osRes = I2CSYS_Wait(pDev->hQupEvt, pSeqInfo->uSeqTimeoutUs/1000);
      if (I2CSYS_RESULT_ERROR_EVT_WAIT_TIMEOUT == osRes) {
         bTimeout = TRUE;
      }
      else if (I2C_RES_SUCCESS != (I2cResult)osRes) {
         pDev->seqInfo.ioRes.nOperationResult = osRes;
      }
   }
   else {
      /* Polled wait.                                                */
      uTmpTimeout = pSeqInfo->uSeqTimeoutUs;
      /* loop until either an event happens or timeout                 */
      while( (!QUP_IsTransferFinished(pDev, pSeqInfo)) &&
             (!bTimeout) ) {
         QUP_ServiceQUP(pDev);
         
         QUP_PerformInputOutput(pDev);
         if ( I2C_RES_SUCCESS != pDev->seqInfo.ioRes.nOperationResult ) {
            break;
         }

         I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
         if ( uTmpTimeout > QUP_BUSY_WAIT_STEP_US ) {
            uTmpTimeout -= QUP_BUSY_WAIT_STEP_US;
         }
         else {
            bTimeout    = TRUE;
         }
      }
   }

   if (bTimeout) { /* set error code. */
      I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_PAUSE);
      QUP_GetTransferCnt(pDev, &pQupInfo->uCurOutTransfers, &pQupInfo->uCurOutTransfers);
      I2CDEVQUP_CalcTransferredDataBytes(pDev);
      I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RESET);
      pDev->seqInfo.ioRes.nOperationResult = I2C_RES_ERROR_CLS_QUP_DEV_TIMEOUT; 
      // | QUPE_Pack28_OperationalState(pDev);
   }
}




/** @brief Configures qup hw base on the current sequence.

    Initializes transfer info structures.

    @param[in] pDev           Pointer to device struct.

    @return                   result of operation.
  */
int32
I2CDEVQUP_ConfigureQupIo
(
   I2CDEVQUP_Device                    *pDev
)
{
   int32                           res;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   I2CDEVQUP_QupInfo              *pQupInfo;
   QupRegsType                    *pQupRegs;
   I2cRegsType                    *pI2cRegs;
   uint32                          uIoMode;
   uint32                          uScaler;
   uint32                          uI2cClkCtl;
   //uint32                          uI2cInputFreq;
   //I2CDEVQUP_SequenceIOInfo       *pSeqInfo;


   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pI2cRegs = pDev->qupInfo.pI2cRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   //pSeqInfo = &pDev->seqInfo;

   /* Set Reset run state.                                                   */
   res = I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      return res;
   }
   res = I2CDEVQUP_InitQupCore(pDev);
   if( I2C_RES_SUCCESS != res ) {
      return res;
   }
   if ( !QUP_FreeI2cBus(pDev) ) {
      return I2CDEVQUP_ERR_BUS_BUSY;
   }
   
   I2CDEVQUP_QupHwEvtQueue_Init(&pQupInfo->hwEvtQueue);
   //pQupInfo->eBamEvt = I2CPLATBAM_EVT_NULL;
   pQupInfo->uIrqCnt                                       = 0;

   pQupInfo->uInFreeSlotReqCnt                 = 0;
   pQupInfo->uOutFreeSlotReqCnt                = 0;
   pQupInfo->uQupErrors                        = 0;
   pQupInfo->uOperational                      = 0;
   pQupInfo->i2cStatus                         = 0;

   pQupInfo->uInFreeSlotServicedCnt            = 0;
   pQupInfo->uOutFreeSlotServicedCnt           = 0;

   uIoMode = (1 << QUP_IO_MODES_PACK_EN_SHFT) |
               (1 << QUP_IO_MODES_UNPACK_EN_SHFT);
   if ( ((pOutSeqInfo->uTotalOutBytes+3)/4) >
          QUP_MX_OUTPUT_CNT_CURRENT_RMSK) {
        return I2CDEVQUP_ERR_INVALID_OUTPUT_BYTE_CNT;
   }
   else if ( ((pOutSeqInfo->uTotalOutBytes+3)/4) > /* block mode. */
          pDev->devHwInfo.uNumOutputSlots ) {
        pQupInfo->eOutFifoMode = QUPE_IO_MODE_BLOCK;
   }
   else { /* fifo mode. */
        pQupInfo->eOutFifoMode = QUPE_IO_MODE_FIFO;
   }
   uIoMode |= (pQupInfo->eOutFifoMode << QUP_IO_MODES_OUTPUT_MODE_SHFT);

   if ( ((pInSeqInfo->uTotalInBytes+3)/4) > QUP_MX_INPUT_CNT_CURRENT_RMSK ) {
        return I2CDEVQUP_ERR_INVALID_INPUT_BYTE_CNT;
   }
   if ( ((pInSeqInfo->uTotalInBytes+3)/4) > pDev->devHwInfo.uNumInputSlots ) {
        pQupInfo->eInFifoMode = QUPE_IO_MODE_BLOCK;
   }
   else {
        pQupInfo->eInFifoMode = QUPE_IO_MODE_FIFO;
   }
   uIoMode |= (pQupInfo->eInFifoMode << QUP_IO_MODES_INPUT_MODE_SHFT);
   pQupRegs->IO_MODES = uIoMode;

   if (QUPE_IO_MODE_FIFO == pQupInfo->eOutFifoMode) {
        pQupInfo->uOutFreeSlotReqCnt  = pDev->devHwInfo.uNumOutputSlots;
        pQupRegs->MX_WRITE_COUNT = pOutSeqInfo->uTotalOutBytes; // fifo     
   }
   else {
        pQupRegs->MX_OUTPUT_COUNT = pOutSeqInfo->uTotalOutBytes;     
   }
     
   if (QUPE_IO_MODE_FIFO == pQupInfo->eInFifoMode) {
        pQupRegs->MX_READ_COUNT = pInSeqInfo->uTotalInBytes; // fifo     
   }
   else {
        pQupRegs->MX_INPUT_COUNT = pInSeqInfo->uTotalInBytes;     
   }

     /* enable interrupts. */
     //_qupe_SetIrqMask(pDev->devProps.virtBlockAddr,0);
   //}
   //

   if ( 0 == pDev->clntCfg.uBusFreqKhz) {
      return I2CDEVQUP_ERR_INVALID_I2C_CLOCK;
   }
   uScaler = pDev->devProps.uI2cInputClkKhz/(2*pDev->clntCfg.uBusFreqKhz) - 3;
   /* The lowest scaler should be 3 */
   if ( uScaler < I2C_CLK_CTL_FS_MIN_DIV_VAL ) {
      return I2CDEVQUP_ERR_INVALID_I2C_FS_DIVIDER;     
   }
   uI2cClkCtl = (uScaler << I2C_CLK_CTL_FS_DIVIDER_VALUE_SHFT) |
                (1 << I2C_CLK_CTL_SDA_NOISE_REJECTION_SHFT)    |
                (1 << I2C_CLK_CTL_SCL_NOISE_REJECTION_SHFT);

   pI2cRegs->CLK_CTL = uI2cClkCtl;

   res = I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RUN);
   if( I2C_RES_SUCCESS != res )
   {
      I2CDEVQUP_InitQupCore(pDev);
      I2CDEVQUP_SetRunState(pDev, QUPE_RUNSTATE_RESET);
      return res;
   }

   return I2C_RES_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

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
   I2CDEV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,
   uint32                               *puNumCompleted
)
{
   int32                                  res;
   I2cSequence                            seq;
   I2cIoResult                            ioRes;


   res = (int32)IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
 
   if ( pTransfer->eTranDirection != I2cTranDirIn ) {
      return I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
   }
   //I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);
   seq.pTransfer = pTransfer;
   seq.uNumTransfers = 1;
   res = I2CDEV_BatchTransfer(hDev, &seq, pClntCfg, &ioRes);
   *puNumCompleted = ioRes.uInByteCnt;

   return res;
}


/** @brief Initializes the device.

    This Function Initializes the device and creates the
    necessary data structures to support other calls into the
    device.

    @param[in] uPlatDevId    Platform Device ID.
    @param[inout] phDev         Pointer to device handle.

    @return          int32 .
  */
int32
I2CDEV_Init
(
   uint32                               uPlatDevId,
   I2CDEV_HANDLE                       *phDev
)
{
   int32                               res = I2C_RES_SUCCESS;
   I2CDEVQUP_Device                    *pDev = NULL;
   uint32                               uWhileOnce = 0;


   pDev = I2CDEVQUP_SearchDevice(uPlatDevId);
   if ( NULL != pDev ) {
      *phDev = pDev;
      return I2C_RES_SUCCESS;
   }

   res = (int32)I2CSYS_Malloc((void **)&pDev, sizeof(I2CDEVQUP_Device));
   if ( I2C_RES_SUCCESS != res )  {
      return res;
   }
   res = I2CSYS_Memset(pDev, 0, sizeof(I2CDEVQUP_Device));
   if ( I2C_RES_SUCCESS != res )  {
      I2CSYS_Free(pDev);     
      return res;
   }

   pDev->uPlatDevId = uPlatDevId;
   pDev->devInitState = I2CDEVQUP_InitState_Uninitialized;

   do {
      res = (int32)I2CPLATDEV_InitTarget(uPlatDevId, &pDev->hDevTgt);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_TargetInit_Done;

      res = (int32)I2CPLATDEV_ReadProperties(pDev->hDevTgt, &pDev->devProps);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->seqInfo.outSeqInfo.pSeq      = &pDev->seqInfo;
      pDev->seqInfo.inSeqInfo.pSeq       = &pDev->seqInfo;
#if 0
      if ( pDev->devProps.bBamAvailable ) {
         /* allocate bam buffer. */
         pDev->seqInfo.physMem.uSize = 2*BAM_TMPBUFF_MAX;// input+output
         res = I2CSYS_PhysMemAlloc(&pDev->seqInfo.physMem);
         if (I2C_RES_SUCCESS != res) {
            break;
         }
         pDev->devInitState |= I2CDEVQUP_InitState_PhysMemAlloc_Done;
         /* use top half for output and bottom half for input. */
         pDev->seqInfo.outSeqInfo.aOutBamBuff =
                     pDev->seqInfo.physMem.pVirtAddress;
         //pDev->seqInfo.inSeqInfo.aInBamBuff = 
         //    &pDev->seqInfo.outSeqInfo.aOutBamBuff[BAM_TMPBUFF_MAX];
      }
#endif

      if ( pDev->devProps.bInterruptBased ) {
         res = (int32)I2CSYS_CreateEvent(&pDev->hQupEvt);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
         pDev->devInitState |= I2CDEVQUP_InitState_Events_Done;
      }

      res = I2CSYS_CreateCriticalSection(&pDev->hOperationSync);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_CritSectionCreate_Done;

      pDev->clntCfg.uBusFreqKhz = I2CDEVQUP_DEFAULT_BUS_FREQ_KHZ;
      pDev->clntCfg.uByteTransferTimeoutUs =
         I2CDEVQUP_DEFAULT_BYTE_TRANSFER_TIMEOUT_US;
      pDev->devInitState |= I2CDEVQUP_InitState_DeviceInit_Done;
      pDev->devPowerState = I2CDEV_POWER_STATE_0;
      /* Init device logging. */
      //I2cLog_Init(pDev->uPlatDevId, &pDev->pDevLog);
      // I2cLog_AssociateBlockAddress(pDev->pDevLog, (uint32)pDev->devProps.virtBlockAddr);
      pDev->qupInfo.pQupRegs = (QupRegsType *) (pDev->devProps.virtBlockAddr +
                                                QUPREGS_OFFSET_FROM_BLSP);
      pDev->qupInfo.pI2cRegs = (I2cRegsType *) (pDev->devProps.virtBlockAddr +
                                                I2CREGS_OFFSET_FROM_BLSP);
      I2CDEVQUP_LinkDevice(pDev);
      
      *phDev = pDev;
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) { /* in case of error undo initialization. */
      I2CDEV_DeInit(pDev);
   }

   return res;
}

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
)
{
   int32 res = I2C_RES_SUCCESS;
   int32 opRes;
   I2CDEVQUP_Device *pDev = (I2CDEVQUP_Device *)hDev;


   if ( NULL == pDev ) {
      return I2C_RES_SUCCESS;
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_DeviceInit_Done ) {
      I2CDEVQUP_UnlinkDevice(pDev);     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_CritSectionCreate_Done ) {
      opRes = I2CSYS_CloseHandle(pDev->hOperationSync);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_Events_Done ) {
      opRes = I2CSYS_CloseHandle(pDev->hQupEvt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }
#if 0   
   if ( pDev->devInitState & I2CDEVQUP_InitState_PhysMemAlloc_Done ) {
      opRes = I2CSYS_PhysMemFree(&pDev->seqInfo.physMem);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }
#endif
   if ( pDev->devInitState & I2CDEVQUP_InitState_ISR_IST_Registration_Done ) {
      opRes = I2CSYS_UnRegisterISTHooks(pDev->hDevTgt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_TargetInit_Done ) {
      opRes = I2CPLATDEV_DeInitTarget(pDev->hDevTgt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }
   
   //I2cLog_DeInit(pDev->pDevLog);
   opRes = (int32)I2CSYS_Free(pDev);
   if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
   }

   return res;
}

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
   I2CDEV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   uint32                               *puNumCompleted
)
{
   int32                                res;
   I2cSequence                          seq;
   I2cIoResult                          ioRes;


   res = IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted);
   if (I2C_RES_SUCCESS != (I2cResult)res) {
      return res;
   }
   if ( pTransfer->eTranDirection != I2cTranDirOut ) {
      return I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
   }
   //I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);
   seq.pTransfer = pTransfer;
   seq.uNumTransfers = 1;
   res = I2CDEV_BatchTransfer(hDev, &seq, pClntCfg, &ioRes);
   
   *puNumCompleted = ioRes.uOutByteCnt;

   return res;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
    @param[out] pIoRes        Pointer to result.
    
    @return          int32 .
  */
int32
I2CDEV_BatchTransfer
(
   I2CDEV_HANDLE                         hDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   I2cIoResult                          *pIoRes
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int                                  res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;

   res = IQUPI2CDEV_VALIDATE_SYNC_SEQ(hDev, pSequence, pClntCfg, pIoRes);
   if ( I2C_RES_SUCCESS != (I2cResult)res ) {
      return res;
   }
   do {
      if ( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
        res =  I2CDEVQUP_ERR_INVALID_POWER_STATE ;
        break;
      }
      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }

      if ( pDev->devProps.bInterruptBased ) {
         res  = I2CSYS_ClearEvent( pDev->hQupEvt);
         if (I2C_RES_SUCCESS != res) {
            break;
         }
      }

      //pDev->asyncCall.pCallbackFn = NULL;
      //pDev->asyncCall.pArg        = NULL;
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->pLastClntCfg = pClntCfg;
         pDev->clntCfg = *pClntCfg;
      }
      //I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, pSequence);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
      QUP_WaitForCompletion(pDev);
      *pIoRes = pDev->seqInfo.ioRes;
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) {
     pDev->seqInfo.ioRes.nOperationResult = res;
   }

   return pDev->seqInfo.ioRes.nOperationResult;
}


/** @brief Schedules a async buffer read from i2c device.

    Schedules an async buffer read from i2c device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32 .
  */
int32
I2CDEV_AsyncRead
(
   I2CDEV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg           ,
   I2cIoResult                          *pIoRes           ,
   DEV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg
)
{
   return I2CDEVQUP_ERR_API_NOT_SUPPORTED;
}


/** @brief Schedules an assynchronous buffer write to i2c device.

    Schedules an asynchronous buffer write to i2c device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
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
   I2CDEV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg           ,
   I2cIoResult                          *pIoRes           ,
   DEV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg
)
{
   return I2CDEVQUP_ERR_API_NOT_SUPPORTED;
}

/** @brief Asynchronously schedules a batch of transfers in a sequence.

    Asynchronously schedules a batch of transfers in a sequence and returns.
    The callback will be called to notify whether the transfer
    is done or has failed.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
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
)
{
   return I2CDEVQUP_ERR_API_NOT_SUPPORTED;
}

/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by reseting the core.

    @param[in] hDev           Device handle.

    @return                   int32.
  */
int32
I2CDEV_CancelTransfer
(
   I2CDEV_HANDLE                        hDev
)
{
   return I2CDEVQUP_ERR_API_NOT_SUPPORTED;
}



/** @brief Set the power state of the device.

    This Function sets the power state for the device.

    @param[in] hDev           Device handle.
    @param[in] ePowerState    the new power state

    @return          int32 .
  */
int32
I2CDEV_SetPowerState
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_PowerStates                   ePowerState
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res;

   if( !pDev ) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }

   res = I2CPLATDEV_SetPowerState(pDev->hDevTgt, ePowerState);
   if ( I2C_RES_SUCCESS == res ) {
      pDev->devPowerState = ePowerState;
   }
   
   return res;
}

/** @brief Gets the power state of the device.

    This Function gets the power state of the device.

    @param[in] hDev           Device handle.
    @param[in] pePowerState   Pointer to power state.

    @return          int32 .
  */
int32
I2CDEV_GetPowerState
(
   I2CDEV_HANDLE                         hDev      ,
   I2CDEV_PowerStates                   *pePowerState
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   
   if (NULL == hDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }

   *pePowerState = pDev->devPowerState;
   
   return res;
}

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
)
{
#if 0
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;

   (void) pCmdData;

   if (NULL == hDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }

   switch ( devCmd ) {
      case I2CDEV_COMMAND_RESET_DEVICE:
        res = I2CDEVQUP_InitQupCore(pDev);      
        break;
      case I2CDEV_COMMAND_NULL:
      default:
        break;
   }

   return res;
#endif

   return I2CDEVQUP_ERR_API_NOT_SUPPORTED;
}


