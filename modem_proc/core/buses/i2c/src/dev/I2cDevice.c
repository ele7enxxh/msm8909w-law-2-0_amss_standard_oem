/*=============================================================================

FILE:      I2cDevice.c

OVERVIEW:  This file contains the implementation of the API for the QUPI2CDeviceQup
           I2CDEV_Init must be called before any other api call.

           Copyright (c) 2015 Qualcomm Technologies Incorporated.
           All Rights Reserved.
           Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE
  

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/dev/I2cDevice.c#1 $
$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $ 
 
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Created


=============================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "I2cDevice.h"
#include "I2cDeviceTypes.h"

#include "I2cPlatSvc.h"
#include "QupRegs.h"
#include "QupRegFieldTypes.h"
#include "I2cError.h"
#include "I2cSys.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

#define MAX_DATA_SEGMENT_LENGTH                      255


// this can be a constant for now but it can be
// read from enumeration data.
// pause is the worst case
#define QUP_RUN_STATE_CHANGE_TIME_OUT_US             100
#define QUP_BUSY_WAIT_STEP_US                         5

#define I2C_HS_MASTER_ADDRESS                        (0x0F)
#define I2C_FAST_STANDARD_FREQ_THRESHOLD_KHZ         (1000)

#define                            BAM_TMPBUFF_MAX             (768)

#define I2CDEV_IS_FIRST_HS_TRANSFER(pSeqInfo, pTransfer) \
(                                                           \
   pSeqInfo->bHsTransfer &&                                 \
   ((I2cTrCtxSeqStart == pTransfer->eTranCtxt) ||           \
   (I2cTrCtxNotASequence == pTransfer->eTranCtxt)           \
   )                                                        \
)


#define QUP_DATA_SEGMENT_LEN             255
#define QUP_PACK28_I2C_STATE(MAS_STATUS, LINE_STATE)\
                ( ((MAS_STATUS) & 0x0FFFFFFC) |\
                  ((LINE_STATE) &0x03)         \
                )

//TODO: Ask HW designer for the values of the tags
typedef enum I2cDev_TagValueType {
   QUP_TAG_START                        = 0x81,
   QUP_TAG_START_10BIT                  = 0x89,
   QUP_TAG_START_STOP                   = 0x8A,
   QUP_TAG_DATA_WRITE                   = 0x82,
   QUP_TAG_DATA_WRITE_STOP              = 0x83,
   QUP_TAG_DATA_READ                    = 0x85,
   QUP_TAG_DATA_READ_NACK               = 0x86,
   QUP_TAG_DATA_READ_STOP               = 0x87,

   QUP_TAG_NOP_WAIT                     = 0x80,
   QUP_TAG_NOP_MARK                     = 0x90,
   QUP_TAG_NOP_ID                       = 0x91,
   QUP_TAG_TIME_STAMP                   = 0x92,
   QUP_TAG_INPUT_EOT                    = 0x93,
   QUP_TAG_INPUT_EOT_FLUSH              = 0x94,
   QUP_TAG_NOP_LOCAL                    = 0x95,
   QUP_TAG_FLUSH_STOP                   = 0x96,

   QUP_TAG_EXTEND = 0x7FFFFFFF
} I2cDev_TagValueType;




/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/
static void
QUP_ProcessNextInputSlot
(
   I2cDev_InSeqInfo               *pInSeqInfo,
   uint32                          uSlot
);

static uint32 
QUP_GetNextOutSeqSlot
(
   I2cDev_OutSeqInfo           *pOutSeqInfo   
);



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
   I2cDev_Device             *pDev,
   volatile uint32           *pOutTrCnt,
   volatile uint32           *pInTrCnt
)
{
   I2cDev_QupInfo              *pQupInfo;
   QupRegsType                    *pQupRegs;
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   I2cDev_InSeqInfo            *pInSeqInfo;
   

   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   

   if (QUP_IO_MODE_FIFO == pQupInfo->eOutFifoMode) { 
      *pOutTrCnt = pOutSeqInfo->uTotalOutBytes -
              (pQupRegs->QUP_MX_WRITE_CNT_CURRENT & QUP_MX_WRITE_COUNT_MX_WRITE_COUNT_BMSK);
   }
   else if (QUP_IO_MODE_BLOCK == pQupInfo->eOutFifoMode) {
      *pOutTrCnt = pOutSeqInfo->uTotalOutBytes -
              (pQupRegs->QUP_MX_OUTPUT_CNT_CURRENT & QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK);
   }

   if (QUP_IO_MODE_FIFO == pQupInfo->eInFifoMode) {
      *pInTrCnt = pInSeqInfo->uTotalInBytes - 
              (pQupRegs->QUP_MX_READ_CNT_CURRENT & 
               QUP_MX_READ_CNT_CURRENT_MX_READ_CNT_CURRENT_BMSK); 
   }
   else if (QUP_IO_MODE_BLOCK == pQupInfo->eInFifoMode) {
      *pInTrCnt = pInSeqInfo->uTotalInBytes - 
              (pQupRegs->QUP_MX_INPUT_CNT_CURRENT & 
               QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK);
   }
}

/** @brief Wait for state to go valid or timeout.
           
    @param[in, out] pDev      Pointer to qup device structure.
    @param[out]     pState    Pointer to state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2cDev_WaitForRunStateValid
(
   I2cDev_Device         *pDev,
   uint32                *pState
)
{
   QupRegsType                        *pQupRegs;
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   uint32         uState;

   pQupRegs = pDev->qupInfo.pQupRegs;

   uState = pQupRegs->QUP_STATE;
   while( (!(uState & QUP_STATE_STATE_VALID_BMSK)) &&
            (0 != uWaitTimeUs) ) {
      I2cSys_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      uState = pQupRegs->QUP_STATE;
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEV_ERR_INVALID_RUNSTATE;
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
I2cDev_InitQupCore
(
 I2cDev_Device      *pDev
 )
{
   int32              res;
   uint32             uState;
   uint32             uRegVal;
   QupRegsType       *pQupRegs;
   
   
   pQupRegs = pDev->qupInfo.pQupRegs;
   
   pQupRegs->QUP_SW_RESET = 1;
   res = I2cDev_WaitForRunStateValid(pDev, &uState);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   
   uRegVal = (QUP_MINI_CORE_I2C_MASTER << QUP_CONFIG_MINI_CORE_SHFT) |
           (1 << QUP_CONFIG_FIFO_CLK_GATE_EN_SHFT)                   |
           (1 << QUP_CONFIG_CORE_CLK_ON_EN_SHFT)                     |
           (1 << QUP_CONFIG_APP_CLK_ON_EN_SHFT)                      |
           QUP_CONFIG_N_8BITS;
   pQupRegs->QUP_CONFIG = uRegVal;
   uRegVal = (1 << QUP_IO_MODES_PACK_EN_SHFT) |
   (1 << QUP_IO_MODES_UNPACK_EN_SHFT);
   pQupRegs->QUP_IO_MODES = uRegVal;
   
   /* Disable input/output interrupts. */
   uRegVal = (1 << QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT) |
   (1 << QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT);
   pQupRegs->QUP_OPERATIONAL_MASK = uRegVal;
   /* Disable error flag interrupts. */
   pQupRegs->QUP_ERROR_FLAGS_EN = 0;
   pQupRegs->I2C_MASTER_CONFIG = I2C_MASTER_CONFIG_EN_VERSION_TWO_TAG_BMSK;
   return I2C_RES_SUCCESS;
}

/** @brief Resets the qup core and waits until state is valid.
 
    @param[in, out] pDev      Pointer to qup device structure.
    @return           Returns timeout error if the previous state is invalid,
 success otherwise.
 */

static int32
I2cDev_ResetQupCore
(
 I2cDev_Device      *pDev
 )
{
   int32               res; //, platRes;
   uint32              uState;
   QupRegsType        *pQupRegs;
   
   pQupRegs = pDev->qupInfo.pQupRegs;
   //I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
   //                     "DEV:0x%x: I2CDEVQUP_ResetQupCore: Reseting Platform pDev=0x%x",
   //                     pDev->uPlatDevId, (uint32) pDev);
   //platRes = I2CPLATDEV_ResetTarget(pDev->hDevTgt);
   
   //I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
   //                     "DEV:0x%x: I2CDEVQUP_ResetQupCore: Reseting QUP pDev=0x%x",
   //                     pDev->uPlatDevId, (uint32) pDev);
   pQupRegs->QUP_SW_RESET = 1;
   res = I2cDev_WaitForRunStateValid(pDev, &uState);
   if ( I2C_RES_SUCCESS != res ) {
     pDev->devInitState &= ~I2cDev_InitState_HardwareInit_Done;
     //diagRes = I2CPLATDEV_Diagnostic(pDev->hDevTgt, 0);
     //if ( I2C_RES_SUCCESS != diagRes) {
     //   I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2,
     //                    "DEV:0x%x: I2CDEVQUP_ResetQupCore(): I2CPLATDEV_Diagnostic()=0x%x"
     //                    ,pDev->uPlatDevId ,diagRes);     
     //}     
  }
  else {
     I2cDev_InitQupCore(pDev);
  }
   return res;
}




/** @brief Sets the runstate. It will wait untill the state is valid
           before trying to set the new state or timeout.
           
    @param[in, out] pDev      Pointer to qup device structure.
    @param[in] eNewRunState   The new run state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32 QUP_SetRunState
(
   I2cDev_Device            *pDev,
   const QUP_RunStateType   eNewRunState
)
{
   QupRegsType              *pQupRegs;     
   uint32                    uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   QUP_RunStateType         eRunState;
   uint32                    uState;
   uint32                    uNewState;
   

   pQupRegs = pDev->qupInfo.pQupRegs;
   
   uNewState = 0;
   /* the doc says to write two times */
   switch(eNewRunState)
   {
      case QUP_RUNSTATE_RESET :
         uNewState |= QUP_RUNSTATE_CMD_RESET;
         pQupRegs->QUP_STATE = uNewState;
         pQupRegs->QUP_STATE = uNewState;
         break;

     case QUP_RUNSTATE_RUN   :
         uNewState |= QUP_RUNSTATE_CMD_RUN;
         pQupRegs->QUP_STATE = uNewState;
         break;

      case QUP_RUNSTATE_PAUSE :
         uNewState |= QUP_RUNSTATE_CMD_PAUSE;
         pQupRegs->QUP_STATE = uNewState;
         break;

      default:
         return (I2CDEV_ERR_INVALID_RUN_STATE);
   }

   uState = pQupRegs->QUP_STATE;
   eRunState = (QUP_RunStateType)(uState & QUP_STATE_STATE_BMSK);

   while ( ( 0 != uWaitTimeUs ) && ( eNewRunState != eRunState ) ) {
      I2cSys_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      uState = pQupRegs->QUP_STATE;
      eRunState = (QUP_RunStateType)(uState & QUP_STATE_STATE_BMSK);
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEV_ERR_RUNSTATE_CHANGE_TIMEOUT;
   }

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
QUP_SetRunStateIfValid
(
 I2cDev_Device            *pDev,
 const QUP_RunStateType   eNewRunState
 )
{
   QUP_RunStateType         eRunState;
   uint32                    uState;
   int32                     res;
   
   res = I2cDev_WaitForRunStateValid(pDev, &uState);
   if ( I2C_RES_SUCCESS != res ) {
      return I2CDEV_ERR_INVALID_RUNSTATE;
   }
   
   eRunState = (QUP_RunStateType)(uState & QUP_STATE_STATE_BMSK);
   if ( eNewRunState == eRunState ) {
      return I2C_RES_SUCCESS;
   }
   return QUP_SetRunState(pDev, eNewRunState);
}



/** @brief Initializes the event queue


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2cDev_QupHwEvtQueue_Init
(
   I2cDev_QupHwEvtQueue *pHwEvtQ
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
I2cDev_QupHwEvtQueue_IsFull
(
   I2cDev_QupHwEvtQueue *pHwEvtQ
)
{
   I2cDev_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEV_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   return (pNext == pHwEvtQ->pTail);
}

/** @brief Checks if the event queue is full


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   TRUE if full, FALSE otherwise.
  */
static boolean
I2cDev_QupHwEvtQueue_IsEmpty
(
   I2cDev_QupHwEvtQueue *pHwEvtQ
)
{
   return (pHwEvtQ->pHead == pHwEvtQ->pTail);
}

/** @brief Moves tail pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2cDev_QupHwEvtQueue_TailNext
(
   I2cDev_QupHwEvtQueue *pHwEvtQ
)
{
   I2cDev_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pTail + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEV_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pTail = pNext;
}

/** @brief Moves head pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2cDev_QupHwEvtQueue_HeadNext
(
   I2cDev_QupHwEvtQueue *pHwEvtQ
)
{
   I2cDev_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEV_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pHead = pNext;
}


/** @brief This function Performs fifo/block mode specific servicing.
   
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
I2cDev_QupHwEvtQueue_ProcessIsrEvt
(
   I2cDev_Device      *pDev,
   I2cDev_QupIsrHwEvt *pIsrEvt
)
{   
   I2cDev_QupInfo              *pQupInfo;
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   QupRegsType                 *pQupRegs;

   pQupInfo    = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pQupRegs    = pQupInfo->pQupRegs;

   pQupInfo->uQupErrors       |= pIsrEvt->uQupErrors;
   pQupInfo->uOperational     |= pIsrEvt->uOperational;
   pQupInfo->i2cStatus        |= (pIsrEvt->i2cStatus & I2C_OP_STATUS_ERROR_FIELD);

   pQupInfo->uCurOutTransfers = pIsrEvt->uOutTranCnt;
   pQupInfo->uCurInTransfers  = pIsrEvt->uInTranCnt;

   /* If output service then fill the output queue with another block.       */
   if ( pIsrEvt->uOperational & QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK ) {
      if ( (QUP_IO_MODE_BLOCK == pQupInfo->eOutFifoMode) &&
       (pIsrEvt->uOperational & QUP_OPERATIONAL_OUT_BLOCK_WRITE_REQ_BMSK) ) {
         pQupInfo->uOutFreeSlotReqCnt += pDev->devHwInfo.uNumOutputBlockSlots;
      }
      /* In fifo mode the OUTPUT_SERVICE is set and the MAX_OUTPUT_DONE
       * is not set at the same time. */
      if ( QUP_IO_MODE_FIFO == pQupInfo->eOutFifoMode ) {
         pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_TRANSMIT_DONE;
      }
   }
   if ( pIsrEvt->uOperational & QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK ) {
      pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_TRANSMIT_DONE;
   }

   /* If input service then get the blocks from queue.                       */
   if ( pIsrEvt->uOperational & QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK ) {
      if ( QUP_IO_MODE_BLOCK == pQupInfo->eInFifoMode ) {
         /* last block gets this as well as normal block requests. */
         pQupInfo->uInFreeSlotReqCnt += pDev->devHwInfo.uNumInputBlockSlots;
      }
      else {
         pQupInfo->uInFreeSlotReqCnt += 
            (pQupRegs->QUP_INPUT_FIFO_WORD_CNT & QUP_INPUT_FIFO_WORD_CNT_INPUT_FIFO_WORD_CNT_BMSK);
      }
   }
   if ( pIsrEvt->i2cStatus & I2C_OP_STATUS_ERROR_FIELD ) {
      /* Don't continue filling the output fifo if there is an error. */
      pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_ERROR;
   }

   return;
}

/** @brief Calculate the number of input and output bytes on a transfer.
 
 Calculate the number of input and output bytes for a transfer.
 
 @param[in]  pSeqInfo        Sequence info pointer.
 @param[in]  pTransfer       Transfer pointer.
 @param[out] puNumInBytes    Pointer to num input bytes.
 @param[out] puNumOutBytes   Pointer to num output bytes.
 reads.
 
 @return                   Nothing.
 */
static void
I2cDev_CalcTransferInOutSize
(
 I2cDev_SequenceIoInfo           *pSeqInfo,
 I2cTransfer                     *pTransfer,
 uint32                          *puNumInBytes,
 uint32                          *puNumOutBytes
 )
{
   uint32                          uNumDataSegments;
   uint32                          uNumInBytes, uNumOutBytes;
   uint32                          uTotalOutBytesLookAhead;
   uint32                          uPadCnt;
   I2cDev_OutSeqInfo              *pOutSeqInfo;
   I2cTransferConfig              *pTrCfg;
   boolean                         bFirstHsTransfer;
   uint32                          uTransferSize = pTransfer->uTrSize;
   
   pOutSeqInfo       = &pSeqInfo->outSeqInfo;
   pTrCfg            = &pTransfer->tranCfg;
   uNumOutBytes      = 0;
   uNumInBytes       = 0;
   uPadCnt           = 0;
   
   uNumDataSegments = uTransferSize/MAX_DATA_SEGMENT_LENGTH;
   if ( uTransferSize % MAX_DATA_SEGMENT_LENGTH ) {
      uNumDataSegments++;
   }
   
   bFirstHsTransfer = I2CDEV_IS_FIRST_HS_TRANSFER(pSeqInfo, pTransfer);
   
   /* Timestamp the start of the transfer. */
   //if ( pTrCfg->cmd & I2cTrCfgCmd_TimeStampStart ) {
   //   uNumOutBytes += 1; /*TIME_STAMP tag set in the output fifo. */
   //   uNumInBytes += 4; /*TIME_STAMP + 3 bytes copied into input fifo. */
   //}
   
   if ( bFirstHsTransfer ) {
      uNumOutBytes += 4;/* st_tag + master_addr + st_tag + slave_addr */
   }
   else {
      uNumOutBytes += 2; /* st_tag + slave_addr */
   }
   if ( I2cTranDirIn == pTransfer->eTranDirection ) {
      /* If single byte input transfer then pad so that it starts at
       * slot boundary */
      if ( 1 == pTransfer->uTrSize ) {
         uTotalOutBytesLookAhead = pOutSeqInfo->uTotalOutBytes + uNumOutBytes;
         /* Pad to slot boundary. */
         uPadCnt = ((4 - (uTotalOutBytesLookAhead & 0x3)) & 0x3);
         uNumOutBytes += uPadCnt;
      }
      /* read:
       slave_setup + (dt_tag + bt_cnt)*n */
      uNumOutBytes += uNumDataSegments*2;
      /* (dt_tag + bt_cnt)*n + data_len               */
      uNumInBytes  +=  uNumDataSegments*2 + uTransferSize;
   }
   else {
      /* write:
       slave_setup + (dt_tag + bt_cnt)*n + data_len */
      uNumOutBytes += uNumDataSegments*2 + uTransferSize;
   }
   /* Timestamp the start of the transfer. */
   //if ( pTrCfg->cmd & I2cTrCfgCmd_TimeStampStop ) {
   //   uNumOutBytes += 1; /*TIME_STAMP tag set in the output fifo. */
   //   uNumInBytes += 4; /*TIME_STAMP + 3 bytes copied into input fifo. */
   //}
   /* store these calculations for latter use */
   pTrCfg->aWordVal[I2CDEVQUP_NumOutBytes_DtIdx] = uNumOutBytes;
   pTrCfg->aWordVal[I2CDEVQUP_NumInBytes_DtIdx]  = uNumInBytes;
   pTrCfg->aWordVal[I2CDEVQUP_NumPrepadBytes_DtIdx]= uPadCnt;
   *puNumOutBytes = uNumOutBytes;
   *puNumInBytes = uNumInBytes ;
}




/** @brief Calculate the number of input and output data bytes transferred.

    Calculate the number of input and output bytes for a transfer.

    @param[in]  pDev            Pointer to device.
 
    @return                   Nothing.
  */
static void
I2cDev_CalcTransferredDataBytes
(
   I2cDev_Device                    *pDev
)
{
   uint32                              uNumDataSegments;
   I2cDev_SequenceIoInfo              *pSeqInfo;
   I2cDev_QupInfo                     *pQupInfo;
   uint32                              uOutByteCnt;
   uint32                              uInByteCnt;
   uint32                              uNumInTrBytes;
   uint32                              uNumOutTrBytes;
   I2cDev_OutSeqInfo                  *pOutSeqInfo;
   I2cDev_InSeqInfo                   *pInSeqInfo;
   I2cTransfer                        *pTransfer;
   uint32                              uTrIndex;
   boolean                             bOutComplete;
   boolean                             bInComplete;
   I2cTransferConfig                  *pTrCfg;
   
   pSeqInfo    = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pQupInfo    = &pDev->qupInfo;
   
   if (I2C_RES_SUCCESS == pSeqInfo->ioRes.nOperationResult) {
      pSeqInfo->ioRes.uInByteCnt  = pInSeqInfo->uNumInDtBytes;
      pSeqInfo->ioRes.uOutByteCnt = pOutSeqInfo->uNumOutDtBytes;
      return;
   }
   
   uInByteCnt   = pQupInfo->uCurInTransfers;
   uOutByteCnt  = pQupInfo->uCurOutTransfers;
   pSeqInfo->ioRes.uInByteCnt  = 0;
   pSeqInfo->ioRes.uOutByteCnt = 0;
   bOutComplete = bInComplete = FALSE;
   
   if ( uOutByteCnt != 0 ) {
      uTrIndex = 0;
      do {
         pTransfer = &pSeqInfo->pTransfer[uTrIndex];
         pTrCfg    = &pTransfer->tranCfg;
         uNumOutTrBytes = pTrCfg->aWordVal[I2CDEVQUP_NumOutBytes_DtIdx];
         uNumInTrBytes =  pTrCfg->aWordVal[I2CDEVQUP_NumInBytes_DtIdx];
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
            pSeqInfo->ioRes.uInByteCnt += pTransfer->uTrSize;
         }
         else {
            pSeqInfo->ioRes.uOutByteCnt += pTransfer->uTrSize;
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
         pSeqInfo->ioRes.uInByteCnt += (uInByteCnt - uNumDataSegments);
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
            pSeqInfo->ioRes.uOutByteCnt += (uOutByteCnt - uNumDataSegments*2);
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
   I2cDev_Device               *pDev,
   uint32                       uTimeout
)
{
   const uint32      cuTimeStep = 5; /* 5 us */
   uint32            uI2cStatus;
   boolean           bBusFree;
   //I2cDev_QupInfo   *pQupInfo;
   QupRegsType      *pQupRegs;

   //pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;

#if 0
   if ( !(pQupInfo->i2cStatus & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK) ) {
      return TRUE;
   }
#endif   

   uI2cStatus = pQupRegs->I2C_MASTER_STATUS;
   bBusFree = !(uI2cStatus & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK);
   
   if ( bBusFree ) {
      return TRUE;
   }
   
   do{
      I2cSys_BusyWait(cuTimeStep);
      if ( uTimeout > cuTimeStep ) {
         uTimeout -= cuTimeStep;
      }
      else {
         uTimeout = 0;
      }
      uI2cStatus = pQupRegs->I2C_MASTER_STATUS;
      bBusFree = !(uI2cStatus & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK);
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
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo              *pQupInfo;
   QupRegsType                 *pQupRegs;
   int32                        res;
   uint32                       uRegVal;
   /* If this is an asynchronous transfer then set results and
      dispatch callback else just signal the waiting thread. */

   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   
   /* 
     QCTDD00352568.
     The driver needs to wait for bus idle before in sets the runstate 
     to RESET.
    */
   if ( !QUP_WaitForI2cBusFree(pDev, pDev->clntCfg.uByteTransferTimeoutUs ) ) {
      if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
         pDev->seqInfo.ioRes.nOperationResult = I2CDEV_ERR_BUS_NOT_RELEASED;
      }     
   }

   /* Do not update the count if there is a core error.
      The core will have been reset and the count is 0. */ 
   if ( (!(pQupInfo->uQupErrors & I2CDEV_ERR_BASE )) &&
        (!(pDev->seqInfo.ioRes.nOperationResult & I2C_RES_ERROR_CLS_I2C_CORE)) ) {
      QUP_GetTransferCnt(pDev, &pQupInfo->uCurOutTransfers, &pQupInfo->uCurInTransfers);
   }

   I2cDev_CalcTransferredDataBytes(pDev);


   /* Reset the state */
   res = QUP_SetRunStateIfValid(pDev, QUP_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      I2cDev_ResetQupCore(pDev);
      if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
         pDev->seqInfo.ioRes.nOperationResult = res;
      }
   }

  /*
    *  To Avoid race condition between writes posted, Which not yet gone through AHB bus 
    *  AND calling thread disabling clocks via Drv_Close, hence read back interrupt status here   
    *  to confirm write is done.
    */
   for(;;)
   {
	  uint32	uQupSvcFlags = 0;
	  uint32	uQupErrFlags = 0;	
	  
	  /* Disable input/output interrupts. */
	  uRegVal = (1 << QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT) |
				(1 << QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT);
	  pQupRegs->QUP_OPERATIONAL_MASK = uRegVal;
	  /* Disable error flag interrupts. */
	  pQupRegs->QUP_ERROR_FLAGS_EN = 0;


      /*Whether all three interrupts are disabled, read back and confirm*/	
      uQupErrFlags = pQupRegs->QUP_ERROR_FLAGS_EN ;
      uQupSvcFlags = pQupRegs->QUP_OPERATIONAL_MASK;
	  
      if( (uQupSvcFlags & (1 << QUP_OPERATIONAL_MASK_INPUT_SERVICE_MASK_SHFT) )&&
	  	  (uQupSvcFlags & (1 << QUP_OPERATIONAL_MASK_OUTPUT_SERVICE_MASK_SHFT) )&& 
          (!uQupErrFlags))
      {
        break;
      }   
   }
   
}


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
)
{
   I2cDev_SequenceIoInfo       *pSeqInfo = &pDev->seqInfo;
   I2cDev_OutSeqInfo           *pOutSeqInfo = &pSeqInfo->outSeqInfo;
   I2cDev_InSeqInfo            *pInSeqInfo  = &pSeqInfo->inSeqInfo;

   if ( I2C_RES_SUCCESS != pSeqInfo->ioRes.nOperationResult ) {
      return TRUE;
   }
   else if ( (I2CDEV_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) &&
             (0 == pInSeqInfo->uTotalInBytes) ) {
      return TRUE;
   }
   else if ( (I2CDEV_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) &&
             (I2CDEV_INPUT_RECEIVE_DONE == pInSeqInfo->eInSeqState) ) {
      return TRUE;
   }

   return FALSE;
}

/** @brief Initializes input/output sequence info structures.

    Initializes input/output sequence info structures.

    @param[out] pSeqInfo   Pointer to in/out sequence struct.
    @param[out] pSeq      Pointer to request sequence struct.
    @param[out] pClntCfg  Pointer to client config struct.

    @return                   Nothing.
  */
static void
I2cDev_InitSeqInfo
(
   I2cDev_SequenceIoInfo          *pSeqInfo,
   I2cSequence                    *pSeq,
   I2cClientConfig                *pClntCfg
)
{
   pSeqInfo->pTransfer              = pSeq->pTransfer;
   pSeqInfo->uNumTransfers          = pSeq->uNumTransfers;
   pSeqInfo->ioRes.uInByteCnt       = pSeqInfo->ioRes.uOutByteCnt = 0;
   pSeqInfo->ioRes.nOperationResult = I2C_RES_SUCCESS;
   if (pClntCfg->uBusFreqKhz > I2C_FAST_STANDARD_FREQ_THRESHOLD_KHZ) {
      pSeqInfo->bHsTransfer         = TRUE; /* High speed transfer */     
   }
   else {
      pSeqInfo->bHsTransfer         = FALSE; /* High speed transfer */     
   }
   pSeqInfo->bBamTransfer           = FALSE;
   pSeqInfo->eSeqState              = I2CDEV_SEQSTATE_INPUT_OUTPUT;
}

/** @brief Initializes output sequence info structures.

    Initializes output sequence info structures.

    @param[out] pOutSeqInfo   Pointer to out sequence struct.

    @return                   Nothing.
  */
static void
I2cDev_InitOutSeqInfo
(
   I2cDev_OutSeqInfo           *pOutSeqInfo
)
{
   I2cDev_SequenceIoInfo       *pSeqInfo;
   pSeqInfo = pOutSeqInfo->pSeq;

   pOutSeqInfo->pTransfer      = pSeqInfo->pTransfer;
   pOutSeqInfo->uOutTrCnt      = pSeqInfo->uNumTransfers;
   pOutSeqInfo->uTrIndex       = 0;
   pOutSeqInfo->eOutputState   = I2CDEV_OUTPUT_FILL_FIFO;
   //pOutSeqInfo->uBamBuffIndex  = 0;
   pOutSeqInfo->uCurOutBytes   = 0;
   pOutSeqInfo->uNumOutDtBytes = 0;
   pOutSeqInfo->uTotalOutBytes = 0;
}

/** @brief Initializes input sequence info structures.

    Initializes input sequence info structures.

    @param[out] pInSeqInfo    Pointer to input info sequence.

    @return                   Nothing.
  */
static void
I2cDev_InitInSeqInfo
(
   I2cDev_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uNumInBytesReceived      = 0;
   pInSeqInfo->eInSeqState              = I2CDEV_INPUT_IDDLE;
   pInSeqInfo->pTransfer                = NULL;
   pInSeqInfo->uInTrCnt                 = 0;
   pInSeqInfo->uTrIndex                 = 0;
   pInSeqInfo->uLastTrIndex             = 0;
   pInSeqInfo->uNumInDtBytes            = 0;
   pInSeqInfo->uTotalInBytes            = 0;
   pInSeqInfo->uInPadCnt                = 0;

}

/** @brief This function initializes the input transfer info structure.
 
 @param[in, out] pDev   Pointer to qup device structure.
 @return         Nothing.
 */
static void
QUP_InSeqInitTransfer
(
 I2cDev_InSeqInfo       *pInSeqInfo
 )
{
   I2cTransfer            *pTransfer;
   I2cDev_InTransferInfo  *pTrInfo;
   
   pTransfer                  = pInSeqInfo->pTransfer;
   pTrInfo                    = &pInSeqInfo->inTr;
   pTrInfo->uDataIndex        = 0;
   pTrInfo->uNextDataSegIndex = 0;
   
   pTrInfo->pIoVec       = pTransfer->pIoVec;
   pTrInfo->IoVecBuffIndex = 0;
   pTrInfo->uFieldByteCnt  = 0;
   
   //if ( pTrCfg->cmd & I2cTrCfgCmd_TimeStampStart ) {
   //   pTrInfo->eInTrState = I2CDEV_TR_STATE_TIME_STAMP_START;
   //}
   //else {
   pTrInfo->eInTrState = I2CDEV_TR_STATE_RD_DATA_TAG;
   //}
}

/** @brief This function initializes the output transfer info structure.

    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
I2cDev_OutSeqInitTransfer
(
   I2cDev_OutSeqInfo       *pOutSeqInfo
)
{
   I2cTransfer            *pTransfer;
   I2cDev_OutTransferInfo *pTrInfo;
   I2cTransferConfig      *pTrCfg;
 
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   pTrCfg    = &pTransfer->tranCfg;

   pTrInfo->uNextDataSegIndex = 0;
   pTrInfo->uDataIndex        = 0;
   pTrInfo->uFieldByteCnt     = 0;

   /* Find initial state. */
   if ( 0 != pTrCfg->aWordVal[I2CDEVQUP_NumPrepadBytes_DtIdx] ) {
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_PAD_OUTPUT;
   }
   //else if ( pTrCfg->cmd & I2cTrCfgCmd_TimeStampStart ) {
   //   pTrInfo->eOutTrState = I2CDEV_TR_STATE_TIME_STAMP_START;
   //}
   else if ( I2CDEV_IS_FIRST_HS_TRANSFER(pOutSeqInfo->pSeq, pTransfer) ) {
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_MASTER_START_TAG;
   }
   else {
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_START_TAG;
   }

   if ( I2cTranDirOut == pTransfer->eTranDirection ) {
      pTrInfo->pIoVec  = pTransfer->pIoVec;
      pTrInfo->IoVecBuffIndex = 0;
   }
}

static void
I2cDev_ConfigureSequenceTransferIo
(
   I2cDev_Device                       *pDev,
   I2cSequence                         *pSeq
)
{
   I2cDev_SequenceIoInfo       *pSeqInfo;
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   I2cDev_InSeqInfo            *pInSeqInfo;
   uint32 uTranIndex;
   I2cTransfer                    *pTransfer;
   uint32                          uNumInBytes, uNumOutBytes;
   
   ////I2C_LOG_TIMESTAMP("I2cDev_ConfigureSequenceTransferIo start");

   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   I2cDev_InitSeqInfo(pSeqInfo, pSeq, &pDev->clntCfg);
   I2cDev_InitOutSeqInfo(pOutSeqInfo);
   I2cDev_InitInSeqInfo(pInSeqInfo);
  
   for (uTranIndex = 0; uTranIndex < pSeqInfo->uNumTransfers; uTranIndex++) {
      pTransfer = &pSeq->pTransfer[uTranIndex]; /* get transfer */
      I2cDev_CalcTransferInOutSize(pSeqInfo, pTransfer,&uNumInBytes, &uNumOutBytes);
      if ( 0 != uNumInBytes ) {
         pInSeqInfo->uInTrCnt++;
         pInSeqInfo->uLastTrIndex = uTranIndex;
         if ( 1 == pInSeqInfo->uInTrCnt ) { /* first read of the sequence. */
            pInSeqInfo->pTransfer   = pTransfer;  /* first read transfer. */
            pInSeqInfo->eInSeqState = I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO;
            pInSeqInfo->uTrIndex    = uTranIndex;
            QUP_InSeqInitTransfer(pInSeqInfo);
         }
         
      }
      if ( I2cTranDirIn == pTransfer->eTranDirection ) {
         pInSeqInfo->uNumInDtBytes += pTransfer->uTrSize;
      }
      else {
         pOutSeqInfo->uNumOutDtBytes += pTransfer->uTrSize;
      }

      //if ( I2cPlat_IsBamEnabled(pDev->hDevTgt) ) {
      //   if ( (uNumInBytes  >= pDev->devProps.uBamThreshold) ||
      //        (uNumOutBytes >= pDev->devProps.uBamThreshold) ) {
      //      pSeqInfo->bBamTransfer = TRUE;
      //   }
      //}
      pInSeqInfo->uTotalInBytes   += uNumInBytes;
      pOutSeqInfo->uTotalOutBytes += uNumOutBytes;
   }
   I2cDev_OutSeqInitTransfer(pOutSeqInfo);
   /* Sequence formating below. */
   /* adjust for EOT TAG and FLUSH_TAG to mark end of input. */
   //if ( pSeqInfo->bBamTransfer ) {
   //   pOutSeqInfo->uTotalOutBytes++; /* Account for FLUSH_STOP */
   //   /* Padding to handle QCTDD00347454.
   //      If transfer_length %4 = 3 pad with additional
   //      FLUSH_STOP.
   //    */
   //   // FLUSH_STOP seems to cause problems so will disable for 9625
   //   //if ( 3 == (pOutSeqInfo->uTotalOutBytes & 0x3) ) {
   //   //   pOutSeqInfo->uTotalOutBytes++;
   //   //}
   //
   //   if ( pInSeqInfo->uTotalInBytes ) {
   //      pInSeqInfo->eInSeqState = I2CDEV_INPUT_PUSH_DESCRIPTORS;
   //      pOutSeqInfo->uTotalOutBytes++; /* Account for EOT */
   //      pInSeqInfo->uTotalInBytes++;   /* Account for EOT */
   //   }
   //}

   pSeqInfo->uSeqTimeoutUs =
      (pInSeqInfo->uTotalInBytes + pOutSeqInfo->uTotalOutBytes) *
      pDev->clntCfg.uByteTransferTimeoutUs;
   ////I2C_LOG_TIMESTAMP("I2cDev_ConfigureSequenceTransferIo finish");
}




/**
    This function processes the qup hw events .

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
void
I2cDev_ProcesQupHwEvts
(
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo     *pQupInfo;
   I2cDev_QupIsrHwEvt *pIsrEvt;

   /* I2CPLATDEV_AcquireIntLock(pDev->hDevTgt); */
   pQupInfo = &pDev->qupInfo;
   
   while ( !I2cDev_QupHwEvtQueue_IsEmpty(&pQupInfo->hwEvtQueue) ) {
      /* Tail is modified the I2cDev_QupHwEvtQueue_TailNext. */     
      pIsrEvt  = pQupInfo->hwEvtQueue.pTail; 
      I2cDev_QupHwEvtQueue_ProcessIsrEvt(pDev, pIsrEvt);     
      I2cDev_QupHwEvtQueue_TailNext(&pQupInfo->hwEvtQueue);
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
I2cDev_ProcesBamEvts
(
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo              *pQupInfo;
   I2cDev_SequenceIoInfo       *pSeqInfo;

   pQupInfo = &pDev->qupInfo;
   pSeqInfo = &pDev->seqInfo;

   switch ( pQupInfo->eBamEvt ) {
      case I2CPLATBAM_EVT_BAM_ERROR:
           /* TODO: should not happen abort transfer. */
           pDev->seqInfo.ioRes.nOperationResult = I2CDEV_ERR_BAM_ERROR;
           break;
      case I2CPLATBAM_EVT_TX_EOT:
           pSeqInfo->outSeqInfo.eOutputState = I2CDEV_OUTPUT_TRANSMIT_DONE;
           break;
      case I2CPLATBAM_EVT_TX_DESC_DONE: /* INTERRUPT fired. */
           pSeqInfo->outSeqInfo.eOutputState = I2CDEV_OUTPUT_FILL_FIFO;
           break;
      case I2CPLATBAM_EVT_RX_EOT:
           pSeqInfo->inSeqInfo.eInSeqState = I2CDEV_INPUT_COPY_INPUT_BUFFERS;
           break;
      case I2CPLATBAM_EVT_RX_DESC_DONE:/* INTERRUPT fired */
           if ( I2CDEV_INPUT_STALLED == pSeqInfo->inSeqInfo.eInSeqState ) {
              pSeqInfo->inSeqInfo.eInSeqState = I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO;
           }
           else {
              /* I2CDEV_INPUT_MEMORY_FLUSH == pSeqInfo->inSeqInfo.eInSeqState */
              pSeqInfo->inSeqInfo.eInSeqState = I2CDEV_INPUT_COPY_INPUT_BUFFERS;
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
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo              *pQupInfo;
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   uint32                          uSlot;
   QupRegsType                    *pQupRegs;

   pQupInfo = &pDev->qupInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   
   
   /* If transfer to be queued Check if there is space in the output fifo
      If to be qued and space continue state machine. */

   /* If output is done update the state of the output sequence. */
   while ( (I2CDEV_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (pQupInfo->uOutFreeSlotReqCnt > pQupInfo->uOutFreeSlotServicedCnt) ) {
      uSlot = QUP_GetNextOutSeqSlot(pOutSeqInfo);
      /* push slot into output fifo. */
      pQupRegs->QUP_OUTPUT_FIFO0 = uSlot;
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
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo              *pQupInfo;
   I2cDev_InSeqInfo            *pInSeqInfo;
   QupRegsType                    *pQupRegs;
   uint32                          uSlot;

   pQupInfo    = &pDev->qupInfo;
   pQupRegs    = pDev->qupInfo.pQupRegs;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   while ((I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState) &&
          (pQupInfo->uInFreeSlotReqCnt > pQupInfo->uInFreeSlotServicedCnt) ) {
      uSlot = pQupRegs->QUP_INPUT_FIFO0;
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
   I2cDev_Device               *pDev
)
{
   I2cDev_QupInfo              *pQupInfo;
   I2cDev_SequenceIoInfo       *pSeqInfo;
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   I2cDev_InSeqInfo            *pInSeqInfo;
   boolean                      bComplete;
   uint32                       uWhileOnce = 0;
   


   pQupInfo = &pDev->qupInfo;
   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   bComplete        = FALSE;

   do {
       
      I2cDev_ProcesQupHwEvts(pDev);
      //I2cDev_ProcesBamEvts(pDev);
      
      if ( I2CDEV_SEQSTATE_INPUT_OUTPUT != pSeqInfo->eSeqState ) {
         break;     
      }

      /* Check for qup hw errors. This should not happen.
         It indicates most likely a programming error.
       */
      if ( pQupInfo->uQupErrors & QUP_ERRSTATE_FIELD ) {
         pSeqInfo->ioRes.nOperationResult = I2CDEV_ERR_BASE |
                     (pQupInfo->uQupErrors & QUP_ERRSTATE_FIELD);
         I2cDev_ResetQupCore(pDev);
         bComplete = TRUE;
         break;
      }
      
      /* Check if there is a transfer to be placed on the out fifo. */
      if ( I2CDEV_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState ) {
         //pDev->pFnPerformOutput(pDev);
         QUP_PerformOutput(pDev);
      }

      /* Input sequence handling.
         If there is an error collect as much input as possible.
         All the data in the input fifo.
       */
      if ( I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState ) {
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
             QUP_PACK28_I2C_STATE(pQupInfo->i2cStatus, 0);
         /* In bam mode the sequence has to be flushed out of the input output
            pipes the same way as a normal sequence. */
         //if ( QUP_IO_MODE_BAM != pQupInfo->eOutFifoMode ) {
         /* Handle the case where the slave stretches the clock after NACK */
         if ( pQupInfo->i2cStatus & I2C_MASTER_STATUS_PACKET_NACKED_BMSK ) {
            QUP_WaitForI2cBusFree(pDev, pDev->clntCfg.uByteTransferTimeoutUs);
         }
         
         I2cDev_ResetQupCore(pDev);
         bComplete = TRUE;
         break;
         //}
      }
      
      /* When we get an I2C error interrupt in BAM mode, we should not push
       * descriptors */
      //if ( I2CDEVQUP_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState ) {
      //   pDev->pFnPerformInput(pDev);
      //}
      
      if ( I2C_RES_SUCCESS != pSeqInfo->ioRes.nOperationResult ) {
         bComplete = TRUE;
         break;
      }

      
      /* If the transfer has already finished do completion. */
      if ( (I2CDEV_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) &&
           (0 == pInSeqInfo->uTotalInBytes) ) {
         bComplete = TRUE;
         break;
      }

      if ( (I2CDEV_INPUT_RECEIVE_DONE == pInSeqInfo->eInSeqState) &&
           (I2CDEV_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) ) {
         bComplete = TRUE;
         break;
      }
   } while ( uWhileOnce );

   if ( bComplete ) {
      pSeqInfo->eSeqState = I2CDEV_SEQSTATE_COMPLETING;
      QUP_Completion(pDev);
     
   }
   
   return;
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
   I2cDev_OutSeqInfo *pOutSeqInfo
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
      pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_FILL_FIFO_DONE;
      
   }
   else { /* initialize next transfer.*/
      pOutSeqInfo->pTransfer++; /* move to next transfer. */
      I2cDev_OutSeqInitTransfer(pOutSeqInfo);
   }
}
/** @brief This function handles transfer transition from output pad Tag.
 
 @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
 @param[out]     pData         Data produced from this state-transition.
 @return         Nothing.
 */
static void
QUP_OutFsm_PadOutputTagNext
(
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8             *pData
 )
{
   uint32                           uFieldByteCnt;
   I2cTransfer                     *pTransfer;
   I2cTransferConfig               *pTrCfg;
   I2cDev_OutTransferInfo          *pTrInfo;
   uint32                           uPadFieldLength;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrCfg    = &pTransfer->tranCfg;
   pTrInfo   = &pOutSeqInfo->outTr;
   
   uFieldByteCnt   = ++pTrInfo->uFieldByteCnt;
   *pData          = QUP_TAG_FLUSH_STOP;
   pOutSeqInfo->uCurOutBytes++;
   
   uPadFieldLength = pTrCfg->aWordVal[I2CDEVQUP_NumPrepadBytes_DtIdx];
   /* If all padding has been applied then transition to next state. */
   if ( uFieldByteCnt == uPadFieldLength ){
      //if ( pTransfer->tranCfg.cmd & I2cTrCfgCmd_TimeStampStart ) {
      //   pTrInfo->eOutTrState = I2CDEVQUP_TR_STATE_TIME_STAMP_START;
      //}
      //else
      if ( I2CDEV_IS_FIRST_HS_TRANSFER(pOutSeqInfo->pSeq, pTransfer) ) {
         pTrInfo->eOutTrState = I2CDEV_TR_STATE_MASTER_START_TAG;
      }
      else {
         pTrInfo->eOutTrState = I2CDEV_TR_STATE_START_TAG;
      }
   }
}

/** @brief This function handles transfer transition from MasterStartTag.
 
 @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
 @param[out]     pData         Data produced from this state-transition.
 @return         Nothing.
 */
static void
QUP_OutFsm_MasterStartTagNext
(
 I2cDev_OutSeqInfo *pOutSeqInfo,uint8* pData
 )
{
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTrInfo   = &pOutSeqInfo->outTr;
   
   *pData = QUP_TAG_START;
   pTrInfo->eOutTrState = I2CDEV_TR_STATE_MASTER_ADDR;
   
   pOutSeqInfo->uCurOutBytes++;
}

/** @brief This function handles transfer transition from MasterAddress.
 
 @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
 @param[out]     pData         Data produced from this state-transition.
 @return         Nothing.
 */
static void
QUP_OutFsm_MasterAddressNext
(
    I2cDev_OutSeqInfo *pOutSeqInfo,
    uint8             *pData
 )
{
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTrInfo   = &pOutSeqInfo->outTr;
   
   *pData = I2C_HS_MASTER_ADDRESS;
   pTrInfo->eOutTrState = I2CDEV_TR_STATE_START_TAG;
   pOutSeqInfo->uCurOutBytes++;
}


/** @brief This function handles transfer transition from StartTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_StartTagNext
(
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8             *pData
)
{
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   
   if ( (I2cTranDirOut == pTransfer->eTranDirection ) &&
        (0 == pTransfer->uTrSize) ){
      *pData = QUP_TAG_START_STOP;
   }
   else {
      *pData = QUP_TAG_START;
   }
   pTrInfo->eOutTrState = I2CDEV_TR_STATE_SLAVE_ADDR;
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
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8             *pData
)
{
   uint8 utmpVal;
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   I2cClientConfig                 *pClntCfg;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   pClntCfg  = &pOutSeqInfo->pSeq->pDev->clntCfg;
   
   utmpVal =  (uint8) (pClntCfg->uSlaveAddr<<1);
   if ( I2cTranDirIn == pTransfer->eTranDirection ) {
      utmpVal |=    1;
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_RD_DATA_TAG;
   }
   else if ( 0 == pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else {
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_WR_DATA_TAG;
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
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8             *pData
)
{
   uint8                            uByteVal;
   uint32                           uBytesLeft;
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   
   
   if ( (pTransfer->uTrSize - pTrInfo->uNextDataSegIndex) > MAX_DATA_SEGMENT_LENGTH ) {
      pTrInfo->uNextDataSegIndex += MAX_DATA_SEGMENT_LENGTH;
      uByteVal = QUP_TAG_DATA_READ;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pTransfer->uTrSize - pTrInfo->uNextDataSegIndex -1;
      if ( 1 == uBytesLeft ) {
         pTrInfo->uNextDataSegIndex--;
      }
   }
   else {
      pTrInfo->uNextDataSegIndex = pTransfer->uTrSize;
      if ((I2cTrCtxSeqStart == pTransfer->eTranCtxt) ||
            (I2cTrCtxSeqContinue == pTransfer->eTranCtxt)) {
           uByteVal = QUP_TAG_DATA_READ_NACK;
      }
      else {
         uByteVal = QUP_TAG_DATA_READ_STOP;
      }
   }
   pTrInfo->eOutTrState = I2CDEV_TR_STATE_DATA_CNT;
   pOutSeqInfo->uCurOutBytes++;
   *pData = uByteVal;
}

/** @brief This function handles transfer transition from WrDataTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_WrDataTagNext
(
   I2cDev_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   
   if ( (pTransfer->uTrSize - pTrInfo->uNextDataSegIndex) > MAX_DATA_SEGMENT_LENGTH) {// shift the next segment
      pTrInfo->uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      utmpVal =    QUP_TAG_DATA_WRITE;
   }
   else {
      pTrInfo->uNextDataSegIndex = pTransfer->uTrSize;
      if ( (I2cTrCtxSeqStart == pTransfer->eTranCtxt)   ||
           (I2cTrCtxSeqContinue == pTransfer->eTranCtxt) ) {
         utmpVal =    QUP_TAG_DATA_WRITE;
      }
      else {
         utmpVal = QUP_TAG_DATA_WRITE_STOP;
      }
   }
   pTrInfo->eOutTrState = I2CDEV_TR_STATE_DATA_CNT;
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
   I2cDev_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 uByteCnt;
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   

   uByteCnt = (uint8) (pTrInfo->uNextDataSegIndex - pTrInfo->uDataIndex);
   if ( 0 == uByteCnt ) { 
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else if ( I2cTranDirIn == pTransfer->eTranDirection ) {
      pTrInfo->uDataIndex += uByteCnt;
      if ( pTrInfo->uDataIndex == pTransfer->uTrSize ) {
         //last read and bam transfer then next is EOT
         if ( pOutSeqInfo->pSeq->bBamTransfer &&
              (pOutSeqInfo->pSeq->inSeqInfo.uLastTrIndex == pOutSeqInfo->uTrIndex) ) {
            pOutSeqInfo->outTr.eOutTrState = I2CDEV_TR_STATE_EOT_TAG;
         }
         else {
            QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
         }
      }
      else {
         pTrInfo->eOutTrState = I2CDEV_TR_STATE_RD_DATA_TAG;
      }
   }
   else {
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_DATA_VALUE;
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
   I2cDev_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   I2cTransfer                     *pTransfer;
   I2cDev_OutTransferInfo          *pTrInfo;
   
   pTransfer = pOutSeqInfo->pTransfer;
   pTrInfo   = &pOutSeqInfo->outTr;
   
   
   if ( pTrInfo->IoVecBuffIndex == pTrInfo->pIoVec->size ) {
      /* get next buffer. */
      pTrInfo->pIoVec++;
      pTrInfo->IoVecBuffIndex = 0;
   }
   /* gather index. */
   utmpVal = pTrInfo->pIoVec->pBuff[pTrInfo->IoVecBuffIndex++];
   pTrInfo->uDataIndex++;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;

   if ( pTrInfo->uDataIndex == pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   } 
   else if ( pTrInfo->uDataIndex == pTrInfo->uNextDataSegIndex ) {
      /* next segment. */
      pTrInfo->eOutTrState = I2CDEV_TR_STATE_WR_DATA_TAG;
   }
}

#if 0
/** @brief This function handles transfer transition from DataValue.
 
 @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
 @param[out]     pData         Data produced from this state-transition.
 @return         Nothing.
 */
static void
QUP_OutFsm_TimeStampStartNext
(
 I2CDEVQUP_OutSeqInfo *pOutSeqInfo, uint8* pData
 )
{
   I2cDev_OutTransferInfo          *pTrInfo;
   I2cTransfer                     *pTransfer;
   
   pTrInfo   = &pOutSeqInfo->outTr;
   pTransfer = pOutSeqInfo->pTransfer;
   *pData = HAL_QUP_TAG_TIME_STAMP;
   pOutSeqInfo->uCurOutBytes++;
   if ( I2CDEVQUP_IS_FIRST_HS_TRANSFER(pOutSeqInfo->pSeq, pTransfer) ) {
      pTrInfo->eOutTrState = I2CDEVQUP_TR_STATE_MASTER_START_TAG;
   }
   else {
      pTrInfo->eOutTrState = I2CDEVQUP_TR_STATE_START_TAG;
   }
}


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
   I2cDev_OutSeqInfo  *pOutSeqInfo,
   uint8                **ppData,
   uint32                *pSize
)
{
   uint8 *ptmpData;
   uint32 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;
   
   if ( pOutSeqInfo->outTr.IoVecBuffIndex == pOutSeqInfo->outTr.pIoVec->size ) {
      /* get next buffer. */
      pOutSeqInfo->outTr.pIoVec++;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   
   ptmpData = &pOutSeqInfo->outTr.pIoVec->pBuff[pOutSeqInfo->outTr.IoVecBuffIndex];
   uBuffSize = pOutSeqInfo->outTr.pIoVec->size -
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
      pOutSeqInfo->outTr.eOutTrState = I2CDEV_TR_STATE_WR_DATA_TAG;
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
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8                *pData
)
{
   I2cDev_InSeqInfo            *pInSeqInfo;

   pInSeqInfo = &pOutSeqInfo->pSeq->inSeqInfo;

   QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   pOutSeqInfo->uCurOutBytes++;
   pInSeqInfo->uInPadCnt++; /* eot will be copied to input. */
   *pData = QUP_TAG_INPUT_EOT;
}
#endif

#if 0
/** @brief This function handles transfer transition from FlushTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_FlushTagNext
(
   I2cDev_OutSeqInfo *pOutSeqInfo,
   uint8                *pData
)
{
   I2cDev_SequenceIoInfo       *pSeqInfo;

   pSeqInfo = pOutSeqInfo->pSeq;
   pOutSeqInfo->uCurOutBytes++;

   *pData = QUP_TAG_FLUSH_STOP;
   
   /* Pad with extra FLUSH_STOP under this condition */
   if ( (!pSeqInfo->bBamTransfer) ||
        ( 3 != (pOutSeqInfo->uCurOutBytes & 0x3)) ) {
      pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_FILL_FIFO_DONE;
   }
   
}
#endif


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
   I2cDev_OutSeqInfo           *pOutSeqInfo   
)
{
   uint32                          uSlot;
   uint32                          uSlotByteIndex;
   uint8                           uByteVal;

   uSlot = 0;
   uSlotByteIndex = 0;

   /* create slot. */
   while ( (uSlotByteIndex < 4) &&
	        (pOutSeqInfo->eOutputState != I2CDEV_OUTPUT_FILL_FIFO_DONE) ) {

      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         //case I2CDEV_TR_STATE_TIME_STAMP_START:
         //   QUP_OutFsm_TimeStampStartNext(pOutSeqInfo,&uByteVal);
         //   break;
         case I2CDEV_TR_STATE_PAD_OUTPUT       : /* start of a transfer. */
               QUP_OutFsm_PadOutputTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_MASTER_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_MasterStartTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_MASTER_ADDR       : /* start of a transfer. */
               QUP_OutFsm_MasterAddressNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_StartTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_SLAVE_ADDR      :
               QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_RD_DATA_TAG     :
               QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_WR_DATA_TAG     :
               QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_DATA_CNT        :
               QUP_OutFsm_DataCntNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEV_TR_STATE_DATA_VALUE      :
               QUP_OutFsm_DataValueNext(pOutSeqInfo,&uByteVal);
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
   I2cDev_Device               *pDev;
   I2cDev_QupInfo              *pQupInfo;

   pDev = (I2cDev_Device *)bamPlatRes.pArg;

   pQupInfo = &pDev->qupInfo;
   
   pQupInfo->eBamEvt = bamPlatRes.bamEvt;

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
   I2cDev_OutSeqInfo           *pOutSeqInfo,
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
          (I2CDEV_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState)) &&
          (pOutSeqInfo->eOutputState != I2CDEV_OUTPUT_FILL_FIFO_DONE) ) {
      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         case I2CDEV_TR_STATE_START_TAG       : /* start of a transfer. */
            QUP_OutFsm_StartTagNext(pOutSeqInfo,
                                    &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEV_TR_STATE_SLAVE_ADDR      :
            QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,
                                        &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEV_TR_STATE_RD_DATA_TAG        :
            QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEV_TR_STATE_WR_DATA_TAG        :
            QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEV_TR_STATE_DATA_CNT        :
            QUP_OutFsm_DataCntNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            /* if next state is data value then flush the buffer. */
            if ( I2CDEV_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState ) {
               bDescReady = TRUE;
            }
            break;
         case I2CDEV_TR_STATE_DATA_VALUE      :
            QUP_OutFsm_DataValueNextBuff(pOutSeqInfo,&pDataAddr,&uDataSize);
            bDescReady = TRUE;
            bUsesBamBuffer = FALSE;/* the data is the raw clnt data. */
            break;
         case I2CDEV_TR_STATE_EOT_TAG:
            QUP_OutFsm_EotTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEV_TR_STATE_FLUSH_STOP:
            QUP_OutFsm_FlushTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         default:
            // do nothing
            break;
      }
   }
   /* last descriptor for output transfer. */
   if ( pOutSeqInfo->eOutputState == I2CDEV_OUTPUT_FILL_FIFO_DONE ) {
      uFlags |= ( PLAT_BAM_DESC_FLAG_EOT |
                  PLAT_BAM_DESC_FLAG_NWD |
                  PLAT_BAM_DESC_FLAG_INT );
   }
   else if ( (BAM_TMPBUFF_MAX == pOutSeqInfo->uBamBuffIndex) &&
             (I2CDEV_TR_STATE_DATA_VALUE != pOutSeqInfo->outTr.eOutTrState) ) {
      /* Buffered and unbuffered pointing descriptors can be
         interleavead and when the buffer is full and more data
         are to be placed on the buffer then the buffer must be
         flushed first. */
      uFlags |= PLAT_BAM_DESC_FLAG_INT;
      pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_STALLED;
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
   I2cDev_Device               *pDev
)
{
   I2cDev_OutSeqInfo           *pOutSeqInfo;
   uint32                          uNumFreeDesc;
   I2CPLATBAM_BamIoVecType         bamIoVec;
  
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   /* If transfer to be queued Check if there is space in the bam queue. */
   uNumFreeDesc = I2CPLATDEV_BamGetPipeFreeSpace(pDev->hDevTgt,
                                                 I2CPLATBAM_DIR_OUTPUT);

   /* If output is done update the state of the output sequence. */
   while( (I2CDEV_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (uNumFreeDesc > 0) ) {
      QUP_GetNextOutSeqDescriptor(pOutSeqInfo, &bamIoVec);
      /* last free bam descriptor, flush the pipe.*/
      if ( (1 == uNumFreeDesc) &&
           (0 == (bamIoVec.flags & (PLAT_BAM_DESC_FLAG_EOT | PLAT_BAM_DESC_FLAG_NWD)))
         ) { 
         bamIoVec.flags |= PLAT_BAM_DESC_FLAG_INT;
         pOutSeqInfo->eOutputState = I2CDEV_OUTPUT_STALLED;
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
   I2cDev_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uInTrCnt--;
   if ( 0 == pInSeqInfo->uInTrCnt ) {
      /* all data has been read from input fifo */
      pInSeqInfo->inTr.eInTrState = I2CDEV_TR_STATE_NOT_IN_TRANSFER;
      return;
   }
   
   do {
      pInSeqInfo->uTrIndex++;
      pInSeqInfo->pTransfer = &pInSeqInfo->pSeq->pTransfer[pInSeqInfo->uTrIndex];
      if ( (I2cTranDirIn == pInSeqInfo->pTransfer->eTranDirection)
          //|| (pTrCfg->cmd & (I2cTrCfgCmd_TimeStampStart |
          //                I2cTrCfgCmd_TimeStampStop ) )
          // /* only timestamping start supported for now. */
          ) {
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
   I2cDev_InSeqInfo *pInSeqInfo
)
{
   uint32 uBytesLeft;
   I2cTransfer                     *pTransfer;
   I2cDev_InTransferInfo           *pTrInfo;
   
   pTransfer = pInSeqInfo->pTransfer;
   pTrInfo   = &pInSeqInfo->inTr;

   if ( (pTransfer->uTrSize - pTrInfo->uNextDataSegIndex) > MAX_DATA_SEGMENT_LENGTH ) {
      pTrInfo->uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pTransfer->uTrSize - pTrInfo->uNextDataSegIndex -1;
      if (1 == uBytesLeft) {
         pTrInfo->uNextDataSegIndex--;
      }
   }
   else {
      pTrInfo->uNextDataSegIndex = pTransfer->uTrSize;
      /* TODO: not supported currently, support will be added in v2
      if ((I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt) ||
            (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt)) {
         uByteVal = QUP_TAG_DATA_READ_NACK;
      }
      else {
      } */
   }
   pTrInfo->eInTrState = I2CDEV_TR_STATE_DATA_CNT;
}

#if 0
/** @brief This function handles transfer transition from
 TimeStampTag.
 
 @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
 @param[in]      uByte         Data with this single byte
 @return         Nothing.
 */
static void
QUP_InFsm_TimeStampStartNext
(
 I2CDEVQUP_InSeqInfo *pInSeqInfo,
 uint8 uByte
 )
{
   I2cTransfer                     *pTransfer;
   I2cTransferConfig               *pTrCfg;
   uint32                           uFieldByteCnt;
   I2cDev_InTransferInfo           *pTrInfo;
   uint32                          uTimeValue;
   uint32                          uTmpVal;
   
   pTrInfo = &pInSeqInfo->inTr;
   pTransfer = pInSeqInfo->pTransfer;
   pTrCfg    = &pTransfer->tranCfg;
   uTmpVal   = uByte;
   
   uFieldByteCnt = pTrInfo->uFieldByteCnt;
   uTmpVal   = uByte;
   
   if ( 0 == uFieldByteCnt ) { /* the tag byte */
      uTimeValue = 0;   /* store a zero in this location. */
   } else {
      uTimeValue = pTrCfg->startTime;
      uTimeValue += (uTmpVal << (8* uFieldByteCnt ));
   }
   uFieldByteCnt++;
   pTrInfo->uFieldByteCnt = uFieldByteCnt;
   pTrCfg->startTime = uTimeValue;
   
   if ( 4 == uFieldByteCnt) {/* last byte of the time stamp. */
      if ( I2cTranDirOut == pTransfer->eTranDirection ) {
         /* This is the only input this transfer generates. */
         QUP_InFsm_MoveNextTransfer(pInSeqInfo);
      }
      else {
         pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
      }
      
   }
}

static void
QUP_InFsm_TimeStampStopNext
(
 I2CDEVQUP_InSeqInfo *pInSeqInfo,
 uint8 uByte
 )
{
   I2cTransfer                     *pTransfer;
   I2cTransferConfig               *pTrCfg;
   uint32                           uFieldByteCnt;
   I2cDev_InTransferInfo           *pTrInfo;
   uint32                          uTimeValue;
   
   pTrInfo = &pInSeqInfo->inTr;
   pTransfer = pInSeqInfo->pTransfer;
   pTrCfg    = &pTransfer->tranCfg;
   
   uFieldByteCnt = ++pTrInfo->uFieldByteCnt;
   uTimeValue = pTrCfg->startTime;
   
   if ( 1 == uFieldByteCnt ) { /* the tag byte */
      pTrCfg->startTime = 0;   /* store a zero in this location. */
   } else {
      uTimeValue = (uTimeValue << 8) + uByte;
   }
   if ( 4 == uFieldByteCnt) {/* last byte of the time stamp. */
      uTimeValue <<=8;
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   }
   pTrCfg->startTime = uTimeValue;
}



#endif

/** @brief This function handles transfer transition from RdDataCntTag.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataCntTagNext
(
   I2cDev_InSeqInfo *pInSeqInfo
)
{
   uint8                            utmpVal;
   I2cDev_InTransferInfo           *pTrInfo;
   
   pTrInfo   = &pInSeqInfo->inTr;

   utmpVal = (uint8)(pTrInfo->uNextDataSegIndex - pTrInfo->uDataIndex);
   if ( 0 == utmpVal ) { 
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } else {
      pTrInfo->eInTrState = I2CDEV_TR_STATE_DATA_VALUE;
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
   I2cDev_InSeqInfo *pInSeqInfo,uint8 uByte
)
{
   uint8                           *pVal;
   I2cTransfer                     *pTransfer;
   I2cDev_InTransferInfo           *pTrInfo;
   
   pTransfer = pInSeqInfo->pTransfer;
   pTrInfo   = &pInSeqInfo->inTr;

   if ( pTrInfo->IoVecBuffIndex == pTrInfo->pIoVec->size ) {
      /* get next buffer. */
      pTrInfo->pIoVec++;
      pTrInfo->IoVecBuffIndex = 0;
   }
   /* gather index. */
   pVal = &pTrInfo->pIoVec->pBuff[pTrInfo->IoVecBuffIndex++];
   *pVal = uByte;
   pTrInfo->uDataIndex++;
   if ( pTrInfo->uDataIndex == pTransfer->uTrSize ) {
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } 
   else if ( pTrInfo->uDataIndex == pTrInfo->uNextDataSegIndex ) {
      /* next segment. */
      pTrInfo->eInTrState = I2CDEV_TR_STATE_RD_DATA_TAG;
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
   I2cDev_InSeqInfo *pInSeqInfo,
   uint8              **pClntDtBuff,
   uint16              *pClntDtSize
)
{
   uint8 *pTmpClntDtBuff;
   uint16 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;

   if ( pInSeqInfo->inTr.IoVecBuffIndex == pInSeqInfo->inTr.pIoVec->size ) {
      /* get next buffer. */
      pInSeqInfo->inTr.pIoVec++;
      pInSeqInfo->inTr.IoVecBuffIndex = 0;
   }

   /* gather index. */
   pTmpClntDtBuff = &pInSeqInfo->inTr.pIoVec->pBuff[pInSeqInfo->inTr.IoVecBuffIndex];
   uBuffSize = (uint16)(pInSeqInfo->inTr.pIoVec->size - pInSeqInfo->inTr.IoVecBuffIndex);

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
         pInSeqInfo->inTr.eInTrState = I2cDev_TR_STATE_DATA_PAD;
      }
      else {
         QUP_InFsm_MoveNextTransfer(pInSeqInfo);
      }
   } 
   else if ( pInSeqInfo->inTr.uDataIndex ==
             pInSeqInfo->inTr.uNextDataSegIndex ) {
      /* next segment. */
      pInSeqInfo->inTr.eInTrState = I2CDEV_TR_STATE_RD_DATA_TAG;
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
   I2cDev_InSeqInfo *pInSeqInfo
)
{
   QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   pInSeqInfo->eInSeqState = I2CDEV_INPUT_RECEIVE_DONE;
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
   I2cDev_InSeqInfo            *pInSeqInfo,
   uint32                          uSlot
)
{
   uint32                          uSlotByteIndex;
   uint32                          uByteVal;
   I2cDev_InTransferInfo           *pTrInfo;
   

   pTrInfo   = &pInSeqInfo->inTr;
   uSlotByteIndex = 0;
   
   /* unpack slot. */
   while ( (uSlotByteIndex < 4) && 
           (I2CDEV_TR_STATE_NOT_IN_TRANSFER != pTrInfo->eInTrState) ) {
      uByteVal = (uSlot >> (uSlotByteIndex * 8)) &0xFF;
      uSlotByteIndex++;
      switch (pTrInfo->eInTrState) {
         //case I2CDEVQUP_TR_STATE_TIME_STAMP_START   :
         //      QUP_InFsm_TimeStampStartNext(pInSeqInfo, uByteVal);
         //      break;
         case I2CDEV_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEV_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               break;
         case I2CDEV_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueSingleNext(pInSeqInfo, (uint8) uByteVal);
               break;
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   } /* end of slot processing. */
   
   pInSeqInfo->uNumInBytesReceived += 4;
   
   if ((pInSeqInfo->uNumInBytesReceived >= pInSeqInfo->uTotalInBytes) ||
       (0 == pInSeqInfo->uInTrCnt))    {
      pInSeqInfo->eInSeqState = I2CDEV_INPUT_RECEIVE_DONE;
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
   I2cDev_InSeqInfo            *pInSeqInfo,
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
         case I2CDEV_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEV_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               uTagDataSize   = 2;
               bDescReady     = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = TRUE;
               break;
         case I2CDEV_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueMultipleNext(pInSeqInfo,&pClntDtBuff, &uClntDtSize);
               bDescReady = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = FALSE;
               break;
         case I2CDEV_TR_STATE_DATA_PAD      :
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
   I2CDEV_InSeqInfo            *pInSeqInfo
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
         case I2CDEV_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               pDescBuff++; /* Throw tag byte */
               break;
         case I2CDEV_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               pDescBuff++; /* Throw byte cnt */
               break;
         case I2CDEV_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueMultipleNext(pInSeqInfo,&pClntDtBuff, &uClntDtSize);
               for ( uClntDataInd = 0; uClntDataInd < uClntDtSize; uClntDataInd++ ) {
                  pClntDtBuff[uClntDataInd] = pDescBuff[uClntDataInd];     
               }
               pDescBuff += uClntDtSize;
               break;
         case I2CDEV_TR_STATE_DATA_PAD      :
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
   I2cDev_InSeqInfo            *pInSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   int32                           res;
   
   pInSeqInfo->inPhysMem.uSize = pInSeqInfo->uTotalInBytes;
   res = I2CSYS_PhysMemAlloc(&pInSeqInfo->inPhysMem);
   if (I2C_RES_SUCCESS != res) {
      return I2CDEV_ERR_INPUT_DESC_BUFFER_ALLOC_FAILED;
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
   I2cDev_Device               *pDev
)
{
   I2cDev_SequenceIoInfo       *pSeqInfo;
   I2cDev_InSeqInfo            *pInSeqInfo;
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
   while( (I2CDEV_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState) &&
          (uNumFreeDesc > 0) ) {
      res = QUP_GetNextInSeqDescriptor(pInSeqInfo, &bamIoVec);
      if ( I2C_RES_SUCCESS != res ) {
         pSeqInfo->ioRes.nOperationResult = res;     
         break;     
      }
      pInSeqInfo->eInSeqState = I2CDEV_INPUT_WAIT_DESC_DATA_READY;     
      /* last free bam descriptor, flush the pipe. */
      /* v1 hw there is only one input descriptor.
      if ( (1 == uNumFreeDesc) &&
           (I2CDEV_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState) ) { 
         bamIoVec.flags |= PLAT_BAM_DESC_FLAG_INT;
         pInSeqInfo->eInSeqState = I2CDEV_INPUT_STALLED;
      }
      */
      I2CPLATDEV_PlatBamTransfer(pDev->hDevTgt,
                                 &bamIoVec,
                                 &uNumFreeDesc,
                                 QUP_BamCallback,
                                 (void *)pDev);
   }
   if ( I2CDEV_INPUT_COPY_INPUT_BUFFERS == pInSeqInfo->eInSeqState ) {
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
   I2cDev_Device *pDev
)
{
   I2cDev_QupInfo        *pQupInfo;
   I2cDev_QupIsrHwEvt    *pIsrEvt;
   QupRegsType           *pQupRegs;
   uint32                 uQupErrors;
   uint32                 uOperational;
   uint32                 uOperationaSvcFlags;
   uint32                 i2cStatus;
   uint32                 uI2cErrors;
   
   pQupRegs = pDev->qupInfo.pQupRegs;
   pQupInfo = &pDev->qupInfo;
   pIsrEvt  = pQupInfo->hwEvtQueue.pHead;


   /* Check operating and error status.                                      */
   /* According to the HW designer both the QUP error flags and the minicore
      status register need to be read and then the interrupts can be cleared.
      If status registers are not read then the interrupt will remain set.
      I have seen this happen for the interrupts asserted by the minicore.   */
   uQupErrors   = pQupRegs->QUP_ERROR_FLAGS & QUP_ERRSTATE_FIELD;
   uOperational = pQupRegs->QUP_OPERATIONAL;
   i2cStatus    = pQupRegs->I2C_MASTER_STATUS;
   uI2cErrors   = i2cStatus & I2C_OP_STATUS_ERROR_FIELD;

   uOperationaSvcFlags =
       (uOperational & 
       (QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK | QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK));

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
   if ( !I2cDev_QupHwEvtQueue_IsFull(&pQupInfo->hwEvtQueue) ) {
      I2cDev_QupHwEvtQueue_HeadNext(&pQupInfo->hwEvtQueue);
   }
   
   /* Check for errors.                                                      */
   if ( uQupErrors ){
      pQupRegs->QUP_ERROR_FLAGS = uQupErrors;
   }
   if( uI2cErrors ) {
      /* Should reset the core to protect against re-entering the ISR due to
         the same error condition being set.                              */
      /* TODO: _qupe_LogRegs(pDev->devProps.virtBlockAddr); */
      
      pQupRegs->I2C_MASTER_STATUS = uI2cErrors;
   }
   if ( uOperationaSvcFlags ) {
      pQupRegs->QUP_OPERATIONAL = uOperationaSvcFlags;
   }
}


/** @brief Configures the device.

    This function configures the device and registers the
    IST hook.

    @param[in] uPlatDevId    Platform Device ID.
    @param[in] phDev         Pointer to device handle.

    @return          int32 .
  */
static int32
I2cDev_SetupHw
(
   I2cDev_Device *pDev
)
{
   int32                 res;
   I2cDev_HwInfoType *pHwInfo;
   QupRegsType          *pQupRegs;
   uint32                uIoMode;
   uint32                uFifoSzFld;
   uint32                uBlkSzFld;

   pHwInfo  = &pDev->devHwInfo;
   pQupRegs = pDev->qupInfo.pQupRegs;

   res = I2cDev_InitQupCore(pDev);
   if ( I2C_RES_SUCCESS != res ) {
      return res;     
   }

   uIoMode = pQupRegs->QUP_IO_MODES;

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
   
   pHwInfo->uHWVersion = pQupRegs->QUP_HW_VERSION;

   return I2C_RES_SUCCESS;
}




/**
    This function free's the bus.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_FreeI2cBus
(
   I2cDev_Device               *pDev
)
{
   uint32            uI2cStatus;
   QupRegsType       *pQupRegs;
   boolean           bBusFree;


   pQupRegs = pDev->qupInfo.pQupRegs;

   uI2cStatus = pQupRegs->I2C_MASTER_STATUS;
   bBusFree = !(uI2cStatus & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK);
   if ( bBusFree ) {
      return TRUE;
   }

   /* clear bus resets the core in virtio. */
   pQupRegs->I2C_MASTER_BUS_CLEAR = 1;
   I2cSys_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);

   uI2cStatus = pQupRegs->I2C_MASTER_STATUS;
   bBusFree = !(uI2cStatus & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK);
   
   return bBusFree;
}


/*
static boolean QUP_PauseQup
(
   I2cDev_Device               *pDev
)
{
   if (_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr)){
      return TRUE;
   }

   QUP_FreeI2cBus(pDev->devProps.virtBlockAddr);
   I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);

   return (!_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr));
}
*/

/** @brief Packs the operational state for debug.
    @param[in]  pQupAddr index of the qup.
    @return     28 bit value indicating state.
  */
static uint32 QUP_Pack28_OperationalState
(
   I2cDev_Device                    *pDev
)
{
   QupRegsType                    *pQupRegs;
   uint32 uI2cStatusVal;
   uint32 uI2cStatusPacked;
   uint32 uOperationalVal;
   uint32 uOpPacked;
   uint32 val;

   pQupRegs = pDev->qupInfo.pQupRegs;

   uI2cStatusVal = pQupRegs->I2C_MASTER_STATUS;
   uI2cStatusPacked = (uI2cStatusVal & 
            (I2C_MASTER_STATUS_BUS_ACTIVE_BMSK       |
             I2C_MASTER_STATUS_BUS_MASTER_BMSK       |
             I2C_MASTER_STATUS_DATA_STATE_BMSK       |
             I2C_MASTER_STATUS_CLK_STATE_BMSK        |
             I2C_MASTER_STATUS_OUTPUT_FSM_STATE_BMSK |
             I2C_MASTER_STATUS_INPUT_FSM_STATE_BMSK) 
         );
   uI2cStatusPacked |= ((uI2cStatusVal &
                         (I2C_MASTER_STATUS_I2C_SCL_BMSK | I2C_MASTER_STATUS_I2C_SDA_BMSK)
                        ) >>
                       3);
                    
   uI2cStatusPacked = uI2cStatusPacked >> 8; // I2C_STATUS fields packed in val[16:0]
   
   uOperationalVal = pQupRegs->QUP_OPERATIONAL;

   // Bits [5:4] 
   uOpPacked = (uOperationalVal &
                  (QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_BMSK |
                   QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_BMSK) );
   
   uOpPacked |= ((uOperationalVal &
                   (QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_BMSK       |
                    QUP_OPERATIONAL_INPUT_SERVICE_FLAG_BMSK        |
                    QUP_OPERATIONAL_MAX_INPUT_DONE_FLAG_BMSK       |
                    QUP_OPERATIONAL_MAX_OUTPUT_DONE_FLAG_BMSK      |
                    QUP_OPERATIONAL_NWD_BMSK                  |
                    QUP_OPERATIONAL_DONE_TOGGLE_BMSK          )
                   ) 
                >> (QUP_OPERATIONAL_OUTPUT_SERVICE_FLAG_SHFT - QUP_OPERATIONAL_INPUT_FIFO_NOT_EMPTY_SHFT - 1));

   uOpPacked = uOpPacked >> QUP_OPERATIONAL_OUTPUT_FIFO_NOT_EMPTY_SHFT; 

   // Bits [9:0]
   val = uI2cStatusPacked | (uOpPacked << 17);
  
   val |= (1 <<27);//1bit mark as an operational status within timeout class

   return val;
}

#if 0
static void
QUP_ConfigureI2cClock
(
   I2cDev_Device                    *pDev
)
{
   uint32 uScaler;
   uint32 uRegVal;
   uint32 uHighTimeDividerValue = 0;

   if ((0 == uFSI2cBusClockReqKhz) || (0 == uInputClockKhz) ||
       (eI2cTags > HAL_QUP_I2C_TAGS_FLD_MAX)) {
      return HAL_QUP_RESULT_INVALID_ARG;
   }
   uScaler = uInputClockKhz/(2*uFSI2cBusClockReqKhz) - 3;
   /* The lowest scaler should be 3 */
   if ( uScaler < HAL_QUP_MIN_FS_DIV_VAL ) {
      return HAL_QUP_RESULT_INVALID_FS_DIVIDER;     
   }

/*
 *uI2cMasterReg = QUP_IN(pQupAddr, I2C_MASTER_STATUS);
   uActiveField  = (uI2cMasterReg &
                   (QUP_FMSK(I2C_MASTER_STATUS, BUS_ACTIVE)) )>>
                   QUP_SHFT(I2C_MASTER_STATUS, BUS_ACTIVE);
 *
 * */

   uRegVal = QUP_IN(pQupAddr, I2C_MASTER_CLK_CTL);
   /* clear the previous value */
   uRegVal &= (~ (QUP_FMSK(I2C_MASTER_CLK_CTL, FS_DIVIDER_VALUE) |
                  QUP_FMSK(I2C_MASTER_CLK_CTL, SDA_NOISE_REJECTION) |
                  QUP_FMSK(I2C_MASTER_CLK_CTL, SCL_NOISE_REJECTION) )
              );
   
   uRegVal |= (uScaler << QUP_SHFT(I2C_MASTER_CLK_CTL, FS_DIVIDER_VALUE) &
                QUP_FMSK(I2C_MASTER_CLK_CTL, FS_DIVIDER_VALUE));

   uRegVal |= (1 << QUP_SHFT(I2C_MASTER_CLK_CTL, SDA_NOISE_REJECTION) &
                QUP_FMSK(I2C_MASTER_CLK_CTL, SDA_NOISE_REJECTION));

   uRegVal |= (1 << QUP_SHFT(I2C_MASTER_CLK_CTL, SCL_NOISE_REJECTION) &
                QUP_FMSK(I2C_MASTER_CLK_CTL, SCL_NOISE_REJECTION));

   if (HAL_QUP_STD_FREQ_KHZ_MAX >= uFSI2cBusClockReqKhz) {
      if ( uHighTimeDividerFactor >= HAL_QUP_STD_MODE_HIGH_TIME_DIVIDER_FACTOR ) {
         uHighTimeDividerValue = uHighTimeDividerFactor * uScaler /100;
      } else {
         uHighTimeDividerValue = HAL_QUP_STD_MODE_HIGH_TIME_DIVIDER_FACTOR * uScaler /100;
      }
   } else if (HAL_QUP_FAST_FREQ_KHZ_MAX >= uFSI2cBusClockReqKhz ) {
     if ( uHighTimeDividerFactor >= HAL_QUP_FAST_MODE_HIGH_TIME_DIVIDER_FACTOR ) {
         uHighTimeDividerValue = uHighTimeDividerFactor * uScaler /100;
      } else {
         uHighTimeDividerValue = HAL_QUP_FAST_MODE_HIGH_TIME_DIVIDER_FACTOR * uScaler /100;
      }
   } else if (HAL_QUP_FASTPLUS_FREQ_KHZ_MAX >= uFSI2cBusClockReqKhz ) {
      if ( uHighTimeDividerFactor >= HAL_QUP_FASTPLUS_MODE_HIGH_TIME_DIVIDER_FACTOR ) {
         uHighTimeDividerValue = uHighTimeDividerFactor * uScaler /100;
      } else {
         uHighTimeDividerValue = HAL_QUP_FASTPLUS_MODE_HIGH_TIME_DIVIDER_FACTOR * uScaler /100;
      }
   } 

   uRegVal |= (uHighTimeDividerValue << QUP_SHFT(I2C_MASTER_CLK_CTL, HIGH_TIME_DIVIDER_VALUE) &
               QUP_FMSK(I2C_MASTER_CLK_CTL, HIGH_TIME_DIVIDER_VALUE));


   QUP_OUT(pQupAddr, I2C_MASTER_CLK_CTL, uRegVal);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  I2C_MASTER_CLK_CTL = %x",
                    pQupAddr, uRegVal);
   
   return HAL_QUP_RESULT_SUCCESS;

}
#endif

/** @brief Configures qup hw base on the current sequence.

    Initializes transfer info structures.

    @param[in] pDev           Pointer to device struct.

    @return                   result of operation.
  */
static int32
I2cDev_ConfigureQupIo
(
   I2cDev_Device                    *pDev
)
{
   int32                           res;
   I2cDev_OutSeqInfo              *pOutSeqInfo;
   I2cDev_InSeqInfo               *pInSeqInfo;
   I2cDev_QupInfo                 *pQupInfo;
   QupRegsType                    *pQupRegs;
   uint32                          uIoMode;
   uint32                          uScaler;
   uint32                          uI2cClkCtl;
   QUP_IoModeType                 eOutFifoMode;
   QUP_IoModeType                 eInFifoMode;
   //uint32                          uI2cInputFreq;
   I2cDev_SequenceIoInfo          *pSeqInfo;


   pQupInfo    = &pDev->qupInfo;
   pQupRegs    = pDev->qupInfo.pQupRegs;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pSeqInfo    = &pDev->seqInfo;

   /* Set Reset run state.                                                   */
   res = QUP_SetRunStateIfValid(pDev, QUP_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      I2cDev_ResetQupCore(pDev);
      return res;
   }
   
   I2cDev_QupHwEvtQueue_Init(&pQupInfo->hwEvtQueue);
   //pQupInfo->eBamEvt = I2CPLATBAM_EVT_NULL;
   pQupInfo->uIrqCnt                           = 0;

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
              QUP_MX_OUTPUT_CNT_CURRENT_MX_OUTPUT_CNT_CURRENT_BMSK) {
      return I2CDEV_ERR_INVALID_OUTPUT_BYTE_CNT;
   }
   else if ( ((pOutSeqInfo->uTotalOutBytes+3)/4) > pDev->devHwInfo.uNumOutputSlots ) {
      eOutFifoMode = QUP_IO_MODE_BLOCK;
   }
   else { /* fifo mode. */
      eOutFifoMode = QUP_IO_MODE_FIFO;
   }
   pQupInfo->eOutFifoMode = eOutFifoMode;
   uIoMode |= (eOutFifoMode << QUP_IO_MODES_OUTPUT_MODE_SHFT);

   if ( ((pInSeqInfo->uTotalInBytes+3)/4) > QUP_MX_INPUT_CNT_CURRENT_MX_INPUT_CNT_CURRENT_BMSK ) {
      return I2CDEV_ERR_INVALID_INPUT_BYTE_CNT;
   }
   if ( ((pInSeqInfo->uTotalInBytes+3)/4) > pDev->devHwInfo.uNumInputSlots ) {
      eInFifoMode = QUP_IO_MODE_BLOCK;
   }
   else {
        eInFifoMode = QUP_IO_MODE_FIFO;
   }
   pQupInfo->eInFifoMode = eInFifoMode;
   uIoMode |= (eInFifoMode << QUP_IO_MODES_INPUT_MODE_SHFT);
   pQupRegs->QUP_IO_MODES = uIoMode;

   if (QUP_IO_MODE_FIFO == eOutFifoMode) {
        pQupInfo->uOutFreeSlotReqCnt  = pDev->devHwInfo.uNumOutputSlots;
        pQupRegs->QUP_MX_WRITE_COUNT = pOutSeqInfo->uTotalOutBytes; // fifo     
   }
   else {
        pQupRegs->QUP_MX_OUTPUT_COUNT = pOutSeqInfo->uTotalOutBytes;     
   }
     
   if (QUP_IO_MODE_FIFO == eInFifoMode) {
        pQupRegs->QUP_MX_READ_COUNT = pInSeqInfo->uTotalInBytes; // fifo     
   }
   else {
        pQupRegs->QUP_MX_INPUT_COUNT = pInSeqInfo->uTotalInBytes;     
   }
   /* Enable operational in/out interrupts. */
   pQupRegs->QUP_OPERATIONAL_MASK = 0;
   /* Enable error flag interrupts. */
   pQupRegs->QUP_ERROR_FLAGS_EN = (
							   QUP_ERROR_FLAGS_EN_OUTPUT_OVER_RUN_ERR_EN_BMSK |
							   QUP_ERROR_FLAGS_EN_INPUT_UNDER_RUN_ERR_EN_BMSK |
							   QUP_ERROR_FLAGS_EN_OUTPUT_UNDER_RUN_ERR_EN_BMSK|
							   QUP_ERROR_FLAGS_EN_INPUT_OVER_RUN_ERR_EN_BMSK );

   //TODO: Add calculation for HS clock

   if ( 0 == pDev->clntCfg.uBusFreqKhz) {
      return I2CDEV_ERR_INVALID_I2C_CLOCK;
   }
   uScaler = pDev->props.uI2cInputClkKhz/(2*pDev->clntCfg.uBusFreqKhz) - 3;
   /* The lowest scaler should be 3 */
   if ( uScaler < I2C_CLK_CTL_FS_DIVIDER_VALUE_MIN ) {
      return I2CDEV_ERR_INVALID_I2C_FS_DIVIDER;     
   }
   uI2cClkCtl = (uScaler << I2C_MASTER_CLK_CTL_FS_DIVIDER_VALUE_SHFT) |
                (1 << I2C_MASTER_CLK_CTL_SDA_NOISE_REJECTION_SHFT)    |
                (1 << I2C_MASTER_CLK_CTL_SCL_NOISE_REJECTION_SHFT);

   pQupRegs->I2C_MASTER_CLK_CTL = uI2cClkCtl;

   res = QUP_SetRunState(pDev, QUP_RUNSTATE_RUN);
   if( I2C_RES_SUCCESS != res )
   {
      pSeqInfo->eSeqState = I2CDEV_SEQSTATE_QUP_STATE_ERROR;
      I2cDev_ResetQupCore(pDev);
      return res;
   }

   return I2C_RES_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Calls into the handler for events related to the qup
    	   core.

    @param[in] pDev           Device structure pointer.
    @param[in] evt            Event to process.
    @param[in] pData          Pointer to data associated with
    						  the event.
    
    @return          Nothing .
  */
void
I2cDev_EventCall
(
   I2cDev_Device                        *pDev ,
   I2cDev_Event                          evt , 
   void                                 *pData
)
{

	switch (evt) {
	case I2CDEV_EVT_ISR  :
		QUP_ServiceQUP(pDev);
		break;
	case I2CDEV_EVT_IST  :
      QUP_PerformInputOutput(pDev);
		break;
	case I2CDEV_EVT_POLL :
		QUP_ServiceQUP(pDev);
      QUP_PerformInputOutput(pDev);
		break;
   default:
      break;
	}
}

/** @brief Submits a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] pDev           Device structure pointer.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
    @param[out] pIoRes        Pointer to result.
    
    @return          int32 .
  */
int32
I2cDev_SubmitSequence
(
   I2cDev_Device                        *pDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg 
)
{
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;

   do {
      
      if ( 0 == (pDev->devInitState & I2cDev_InitState_HardwareInit_Done) ) {
         res = I2cDev_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->pLastClntCfg = pClntCfg;
         pDev->clntCfg = *pClntCfg;
      }
      //I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      I2cDev_ConfigureSequenceTransferIo(pDev, pSequence);
      res = I2cDev_ConfigureQupIo(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
 
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) {
     pDev->seqInfo.ioRes.nOperationResult = res;
   }

   return pDev->seqInfo.ioRes.nOperationResult;
}

/** @brief Checks if bus is clear.
 @param[in]  pQupAddr index of the qup.
 @return Nothing.
 */
boolean
QUP_IsI2cBusClkForcedLow
(
   I2cDev_Device               *pDev
)
{
   uint32 uClkState, uBusActive, uOutFsmState, uI2cMasterReg;
   QupRegsType                 *pQupRegs;
   
   pQupRegs = pDev->qupInfo.pQupRegs;
   
   uI2cMasterReg = pQupRegs->I2C_MASTER_STATUS;
   uBusActive = (uI2cMasterReg & I2C_MASTER_STATUS_BUS_ACTIVE_BMSK);
   
   uOutFsmState = ((uI2cMasterReg &
                    I2C_MASTER_STATUS_OUTPUT_FSM_STATE_BMSK) >>
                    I2C_MASTER_STATUS_OUTPUT_FSM_STATE_SHFT);
   
   uClkState = (uI2cMasterReg &I2C_MASTER_STATUS_CLK_STATE_BMSK) >>
	            I2C_MASTER_STATUS_CLK_STATE_SHFT;
   
   
   return (
           (I2C_MASTER_STATUS_CLK_STATE_FORCED_LOW_STATE == uClkState) &&
           (
            (I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_IDLE_STATE  == uOutFsmState) ||
            (I2C_MASTER_STATUS_OUTPUT_FSM_STATE_V1_V2_MI_REC_STATE == uOutFsmState)
            )
           ) || (!uBusActive);
}

/**
 This function waits for bus clk to go to forced low state.
 
 @param[in, out] pDev  Pointer to qup device structure.
 @return         TRUE if the bus is free, FALSE otherwise.
 */
static boolean QUP_WaitForI2cBusClkForcedLow
(
   I2cDev_Device               *pDev
)
{
   uint32 uTimeout;
   const uint32 cuTimeStep = QUP_BUSY_WAIT_STEP_US; /* 5 us */
   
   if ( QUP_IsI2cBusClkForcedLow(pDev) ) {
      return TRUE;
   }
   
   uTimeout = pDev->clntCfg.uByteTransferTimeoutUs;
   do{
      I2cSys_BusyWait(cuTimeStep);
      if ( uTimeout > cuTimeStep ) {
         uTimeout -= cuTimeStep;
      }
      else {
         uTimeout = 0;
      }
   } while ( uTimeout && !QUP_IsI2cBusClkForcedLow(pDev) );
   
   return (QUP_IsI2cBusClkForcedLow(pDev));
}

/** @brief Cancels ongoing transfer.
 
 Cancels ongoing transfer by reseting the core.
 
 @param[in] pDev           Device structure.
 
 @return                   int32.
 */
int32
I2cDev_AbortSequence
(
   I2cDev_Device *pDev
)
{
   I2cDev_SequenceIoInfo          *pSeqInfo;
   I2cTransfer                    *pTransfer;
   //HAL_qupe_RunStateType           eRunState;
   boolean                         bSkipBusClear = FALSE;
   QupRegsType                    *pQupRegs;
   I2cDev_QupInfo                 *pQupInfo;
   uint32                         uState;
   uint32                         uI2cDataState;
   QUP_RunStateType               eRunState;
   
   pSeqInfo = &pDev->seqInfo;
   pQupInfo = &pDev->qupInfo;
   pTransfer = pSeqInfo->pTransfer;
   pQupRegs = pDev->qupInfo.pQupRegs;
   
   uState = pQupRegs->QUP_STATE;
   eRunState = (QUP_RunStateType)(uState & QUP_STATE_STATE_BMSK);
   
   //I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(pDev=0x%x)",
   //                     pDev->uPlatDevId, (uint32) pDev);
   
   //eRunState = HAL_qupe_GetRunState(pDev->devProps.virtBlockAddr);
   
   /* No point of all this if we are already in RESET state */
   if ((eRunState != QUP_RUNSTATE_RESET) &&
       (pSeqInfo->eSeqState != I2CDEV_SEQSTATE_COMPLETE))
   {
      /* Try to gracefully stop at data boundary. */
      QUP_SetRunState(pDev, QUP_RUNSTATE_PAUSE);
      
      pSeqInfo->eSeqState = I2CDEV_SEQSTATE_COMPLETE;
      
      /* CHECK: We still see a Bus Clear and STOP on the fly. If multiple
       * transfers are queued and we paused the core just around the end of one
       * of the transfers, then the bus may already be idle, but we do not know
       * what condition to query to determine that. If you see a stray bus clear
       * and STOP without any START bit, you need to add a logic here */
      
      /* A PAUSE will let core transfer a couple more bytes and CLK goes to
       * FORCED_LOW_STATE. Note that if we did not reach FORCED_LOW, it means
       * that we have already waited for the clock-stretch amount of time that
       * was configured */
      if (!QUP_WaitForI2cBusClkForcedLow(pDev)) {
         //I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), Wait for FORCED CLK LOW failed", pDev->uPlatDevId);
      }
      
      QUP_GetTransferCnt(pDev,
                         &pQupInfo->uCurOutTransfers,
                         &pQupInfo->uCurInTransfers);
      I2cDev_CalcTransferredDataBytes(pDev);
      pDev->seqInfo.ioRes.nOperationResult = I2C_RES_ERROR_CLS_QUP_DEV_TIMEOUT |
      QUP_Pack28_OperationalState(pDev);
      
      /* Reset core calls into reset pipes and that will cause a deadlock in the
       * bam driver if the callback tries to get the same locks. */
      
      // I2CDEVQUP_ResetQupCore(pDev);
      // HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
      
      /* I see that slave stretches the clock here right after the address is
       * sent out on the bus. In this case, we may prematurely put the core into
       * RUN state and that results in BUS ERROR. We must wait for the clock
       * stretch amount of time before doing anything. DATA_STATE is
       * TX_ADDR_STATE at this point. We will check for that.  Note that in this
       * case the core does send out 9 clocks irrespective or a read/write
       * transfer, so we will skip the bus clear.
       */
      uI2cDataState = (pQupRegs->I2C_MASTER_STATUS & I2C_MASTER_STATUS_DATA_STATE_BMSK) >>
                    I2C_MASTER_STATUS_DATA_STATE_SHFT ;
      if ( I2C_MASTER_STATUS_DATA_STATE_TX_ADDR_STATE == uI2cDataState )
      {
         I2cSys_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);
         bSkipBusClear = TRUE;
      }
      
      QUP_SetRunState(pDev, QUP_RUNSTATE_RESET);
      
      if (!QUP_WaitForI2cBusFree(pDev, pDev->clntCfg.uByteTransferTimeoutUs)) {
         //I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), After FORCED LOW Wait for IDLE failed", pDev->uPlatDevId);
      }
      
      if ((pTransfer->eTranDirection == I2cTranDirIn) &&
          (bSkipBusClear == FALSE))
      {
         /* Bus Clear will not work if core is not in RUN STATE */
         QUP_SetRunState(pDev, QUP_RUNSTATE_RUN);
         
         /* We need to do Bus Clear especially to handle Read termination */
         QUP_FreeI2cBus(pDev);
         
         if (!QUP_WaitForI2cBusFree(pDev, pDev->clntCfg.uByteTransferTimeoutUs)) {
            //I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), Wait for BUS FREE failed", pDev->uPlatDevId);
         }
      }
   }
   
   /* Reset BAM if we cancel an ongoing BAM transfer */
   //I2CPLATDEV_ResetTarget(pDev->hDevTgt);
   
   //I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer()=", pDev->uPlatDevId);
   return I2C_RES_SUCCESS;
}

/** @brief Gets the device operation status and sequence state.

    @param[in] pDev           Device structure pointer.
    @param[in] pDevState      Pointer to device state.
    
    @return          int32 .
  */
void I2cDev_GetState
(
   I2cDev_Device                        *pDev ,
   I2cDev_State                         *pDevState
)
{
  pDevState->uTimeoutUs =  pDev->seqInfo.uSeqTimeoutUs;
  pDevState->eIoState   =  pDev->seqInfo.eSeqState;
  pDevState->opResult   =  pDev->seqInfo.ioRes;
}


/** @brief Sends a cmd to the i2c dev.

    This Function  Sends a cmd to the i2c dev.

    @param[in]     hDev            Device handle.
    @param[in]     devCmd          Command to be sent to device.
    @param[inout]  pCmdData        Data related to the command.

    @return          int32 .
  */
int32
I2cDev_SendCmd
(
   I2cDev_Device                        *pDev     ,
   I2cDev_Command                       devCmd    ,
   void                                *pCmdData
)
{
#if 0
   I2cDev_Device                    *pDev = (I2cDev_Device *)hDev;
   int32                             res = I2C_RES_SUCCESS;

   (void) pCmdData;

   if (NULL == hDev) {
      return I2CDEV_ERR_INVALID_DEV_HANDLE;
   }

   switch ( devCmd ) {
      case I2CDEV_COMMAND_RESET_DEVICE:
        res = I2cDev_InitQupCore(pDev);      
        break;
      case I2CDEV_COMMAND_NULL:
      default:
        break;
   }

   return res;
#endif

   return I2CDEV_ERR_API_NOT_SUPPORTED;
}


