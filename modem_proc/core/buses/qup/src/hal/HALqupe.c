/*
===========================================================================

FILE:   HALqupe.c

DESCRIPTION:
    This file implements a qup hal.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/src/hal/HALqupe.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/21/13 LK     Removed test gards on put/get word from/to fifo to speed up
                 the code. The tests are done somewhere else.
06/13/12 ddk    Removed unused include.
06/06/12 ddk    Removed non-common functions related to I2c and SPI.
06/05/12 ddk    Removed unwanted writes in bam mode.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
                Added updated HAL implementation used for I2C.
02/22/12 ddk    Added support for SPI BAM interface requirements.
02/14/11 LK     Changed QUP address block initialization from static to
                dynamic. Virtual addresses are intialized at runtime.
10/15/10 LK     Created

===========================================================================
        Copyright c 2008-2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "HALqupe.h"
#include "HALqupeTarget.h"

// Define and include the header file that adds the relevant log headers
#include "QupLog.h"

#define HAL_QUPE_VERSION_STRING "HAL_QUPE_V_2.0"




/*
#ifdef QUP_HAL_DBG
  #define WHERE_STR "FILE: %s LINE: %d"

  #define HAL_QUPE_ASSERT(COND)\
   {\
    if (COND) {\
      IBUSLOG(QUP_HAL_DBG, LOG_LEVEL_5,pQupAddr,2, WHERE_STR, __FILE__, __LINE__);\
      while(1);\
    }\
   }\

#else
*/
#define HAL_QUPE_ASSERT(...)

/*
#endif
*/


#define HAL_QUPE_TRANSFER_SIZE                          8

/** @enum HAL_qupe_RunStateType QUPe run state.                       */
typedef enum HAL_qupe_RunStateCmdType
{
   HAL_QUPE_RUNSTATE_CMD_RESET   = 0x2  , /**<Go to Reset State.          */
   HAL_QUPE_RUNSTATE_CMD_RUN     = 0x1  , /**<Go to RUN state.            */
   HAL_QUPE_RUNSTATE_CMD_PAUSE   = 0x3  , /**<Go to pause state.          */

   HAL_QUPE_RUNSTATE_CMD_EXTEND  = 0x7FFFFFFF
} HAL_qupe_RunStateCmdType;

/** @brief Resets QUPe.
    @param[in]  pQupAddr Qup Block address.
    @return Nothing.
  */
void
HAL_qupe_Reset
(
   uint8  *pQupAddr
)
{
   QUPE_OUT(pQupAddr, QUP_SW_RESET, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 1,"QUP[0x%x]: Write:  QUP_SW_RESET , VAL: 1", (uint32)pQupAddr);
}

/** @brief This function initializes the qupe HAL.
    @param[in]  pQupAddr        Qup Block address.
    @param[in]  ppszVersion   Version pointer.
    @return Nothing.
  */
void HAL_qupe_Init
(
  uint8 *pQupAddr,
  char **ppszVersion
)
{
   if (NULL != ppszVersion)
   {
      *ppszVersion = HAL_QUPE_VERSION_STRING;
   }
   HAL_qupe_Reset(pQupAddr);
}


/** @brief Returns the number of output FIFO slots(4 byte entries).
    @param[in]  pQupAddr Qup Block address.
    @return Returns the number of output FIFO slots(4 byte entries).
  */
uint32
HAL_qupe_GetNumOutputFifoBlockSlots
(
   uint8  *pQupAddr
)
{
   uint32 uOutBlockSizeFld;

   uOutBlockSizeFld   = QUPE_INF(pQupAddr, QUP_IO_MODES, OUTPUT_BLOCK_SIZE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_IO_MODES[OUTPUT_BLOCK_SIZE] , VAL:0x%x",
                   (uint32)pQupAddr, uOutBlockSizeFld);
   switch(uOutBlockSizeFld)
   {
      case 0: return 1;
      case 1: return 4;
      case 2: return 8;
      default: return 0;
   }
}

/** @brief Returns the number of input FIFO slots per block.
    @param[in]  pQupAddr Qup Block address.
    @return Returns the number of input FIFO slots per block.
  */
uint32
HAL_qupe_GetNumInputFifoBlockSlots
(
   uint8  *pQupAddr
)
{
   uint32 uInBlockSizeFld   = QUPE_INF(pQupAddr, QUP_IO_MODES, INPUT_BLOCK_SIZE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_IO_MODES[INPUT_BLOCK_SIZE] , VAL:0x%x",
                   (uint32)pQupAddr, (int32)uInBlockSizeFld);
   switch(uInBlockSizeFld)
   {
      case 0: return 1;
      case 1: return 4;
      case 2: return 8;
      default: return 0;
   }
}



/** @brief Reads the RTL info and returns it.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  pHwInfo Pointer to struct used to store hw info.
    @return Nothing.
  */
void HAL_qupe_GetHwInfo
(
   uint8               *pQupAddr          ,
   HAL_qupe_HwInfoType *pHwInfo
)
{
   uint32 uTmpField;
   HAL_QUPE_ASSERT(NULL == pHwInfo);

   pHwInfo->uNumOutputBlockSlots = HAL_qupe_GetNumOutputFifoBlockSlots(pQupAddr);
   pHwInfo->uNumInputBlockSlots  = HAL_qupe_GetNumInputFifoBlockSlots(pQupAddr);

   uTmpField = QUPE_INF(pQupAddr, QUP_IO_MODES, INPUT_FIFO_SIZE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_IO_MODES[INPUT_FIFO_SIZE] , VAL:0x%x",
                   (uint32)pQupAddr, (int32)uTmpField);
   pHwInfo->uNumInputBlocks   = 1 << (uTmpField + 1);

   uTmpField = QUPE_INF(pQupAddr, QUP_IO_MODES, OUTPUT_FIFO_SIZE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_IO_MODES[OUTPUT_FIFO_SIZE] , VAL:0x%x",
                   (uint32)pQupAddr, uTmpField);
   pHwInfo->uNumOutputBlocks  = 1 << (uTmpField + 1);

   pHwInfo->uNumInputSlots = pHwInfo->uNumInputBlocks * pHwInfo->uNumInputBlockSlots;
   pHwInfo->uNumOutputSlots = pHwInfo->uNumOutputBlocks * pHwInfo->uNumOutputBlockSlots;

   pHwInfo->uHWVersion = QUPE_IN(pQupAddr,QUP_HW_VERSION);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_HW_VERSION , VAL:0x%x",
                   (uint32)pQupAddr, (int32)pHwInfo->uHWVersion);
}



/** @brief Enables/disables the input fifo.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  bInFifoEnabled boolean indicating whether to enable the
                input fifo.
    @return Nothing.
  */
void
HAL_qupe_SetInputFifoEnabled
(
   uint8 *pQupAddr,
   const boolean bInFifoEnabled
)
{
   uint32 uVal;     
   if(FALSE == bInFifoEnabled){
      uVal = 1;
   }
   else{
      uVal = 0;
   }
   QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_INPUT, uVal);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[NO_INPUT] = %x", (uint32)pQupAddr, uVal);
}


/** @brief Gets the state of the input fifo.
    @param[in]  pQupAddr Qup Block address.
    @return TRUE if the input fifo is enabled, FALSE otherwise.
  */
boolean
HAL_qupe_GetInputFifoEnabled
(
   uint8 *pQupAddr
)
{
   uint32 uTmpField;
   uTmpField = QUPE_INF(pQupAddr, QUP_CONFIG, NO_INPUT);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_CONFIG[NO_INPUT] , VAL:0x%x",
                   (uint32)pQupAddr, (int32)uTmpField);
   return (!uTmpField);
}


/** @brief Enables/disables the output fifo.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  bOutFifoEnabled Boolean indicating whether to enable the
                output fifo.
    @return Nothing.
  */
void
HAL_qupe_SetOutputFifoEnabled
(
   uint8 *pQupAddr,
   const boolean bOutFifoEnabled
)
{
   uint32 uVal;
   if(FALSE == bOutFifoEnabled){
      uVal = 1;
   }
   else{
      uVal = 0;
   }
   QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_OUTPUT, uVal);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[NO_OUTPUT] = %x", 
                     (uint32)pQupAddr, uVal);
}


/** @fn boolean HAL_qupe_GetOutputFifoEnabled(void)
    @brief Gets the state of the output fifo enabled.
    @param Nothing.
    @return TRUE if the output fifo is enabled, FALSE otherwise.
  */
boolean
HAL_qupe_GetOutputFifoEnabled
(
   uint8 *pQupAddr
)
{
   uint32 uTmpField;
   uTmpField = QUPE_INF(pQupAddr, QUP_CONFIG, NO_OUTPUT);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_CONFIG[NO_OUTPUT] , VAL:0x%x",
                   (uint32)pQupAddr, uTmpField);
   return (!uTmpField);
}


/** @brief Gets the state of the run state field.
    @param[in]  pQupAddr Qup Block address.
    @return TRUE if the run state is valid, FALSE otherwise.
  */
boolean
HAL_qupe_RunStateIsValid
(
   uint8 *pQupAddr
)
{
   uint32 uValidField;
   uint32 uRegVal;

   uRegVal = QUPE_IN(pQupAddr, QUP_STATE);
   uValidField = (uRegVal & (QUPE_FMSK(QUP_STATE, STATE_VALID))) >>
         QUPE_SHFT(QUP_STATE, STATE_VALID);
   
   QUPLOG_LOG_READ((uint32)pQupAddr, 3,
                   "QUP[0x%x]: Read: QUP_STATE=0x%x, QUP_STATE[STATE_VALID]=0x%x",
                   (uint32)pQupAddr, uRegVal, uValidField);
   return (boolean)uValidField;
}

/** @brief Sets the state of the QUPe.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceQupeRunState Specifies the new state.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetRunState
(
   uint8 *pQupAddr,
   const HAL_qupe_RunStateType ceQupeRunState
)
{
   HAL_qupe_RunStateType curRunState;

   curRunState = HAL_qupe_GetRunState(pQupAddr);


   /* the doc says to write two times */
   switch(ceQupeRunState)
   {
      case HAL_QUPE_RUNSTATE_RESET :
         if (HAL_QUPE_RUNSTATE_PAUSE == curRunState) {
        	 QUPE_OUTF(pQupAddr, QUP_STATE, STATE, HAL_QUPE_RUNSTATE_CMD_RESET);
          QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE[STATE] = %x", (uint32)pQupAddr, HAL_QUPE_RUNSTATE_CMD_RESET);
         }
         QUPE_OUTF(pQupAddr, QUP_STATE, STATE, HAL_QUPE_RUNSTATE_CMD_RESET);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE[STATE] = %x", (uint32)pQupAddr, HAL_QUPE_RUNSTATE_CMD_RESET);
         break;

     case HAL_QUPE_RUNSTATE_RUN   :
         QUPE_OUTF(pQupAddr, QUP_STATE, STATE, HAL_QUPE_RUNSTATE_CMD_RUN);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE[STATE] = %x", (uint32)pQupAddr, HAL_QUPE_RUNSTATE_CMD_RUN);
         break;

      case HAL_QUPE_RUNSTATE_PAUSE :
         QUPE_OUTF(pQupAddr, QUP_STATE, STATE, HAL_QUPE_RUNSTATE_CMD_PAUSE);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE[STATE] = %x", (uint32)pQupAddr, HAL_QUPE_RUNSTATE_CMD_PAUSE);
         break;

      default:
         return (HAL_QUPE_RESULT_INVALID_ARG);
   }

   return HAL_QUPE_RESULT_SUCCESS;
}


/** @brief Sets the state of the QUPe. This is a more efficient implementation.
           It has no reads.

    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceNextRunState Specifies the new state.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetRunStateExt
(
   uint8 *pQupAddr,
   const HAL_qupe_RunStateType ceNextRunState
)
{

   switch(ceNextRunState)
   {
      case HAL_QUPE_RUNSTATE_RESET :
         /* the docummentation says to write two times */
         QUPE_OUT(pQupAddr, QUP_STATE, HAL_QUPE_RUNSTATE_CMD_RESET);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE = %x", pQupAddr, HAL_QUPE_RUNSTATE_CMD_RESET);
         QUPE_OUT(pQupAddr, QUP_STATE, HAL_QUPE_RUNSTATE_CMD_RESET);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE = %x", (uint32) pQupAddr, HAL_QUPE_RUNSTATE_CMD_RESET);
         break;

     case HAL_QUPE_RUNSTATE_RUN   :
         QUPE_OUT(pQupAddr, QUP_STATE, HAL_QUPE_RUNSTATE_CMD_RUN);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE = %x", (uint32) pQupAddr, HAL_QUPE_RUNSTATE_CMD_RUN);
         break;

      case HAL_QUPE_RUNSTATE_PAUSE :
         QUPE_OUT(pQupAddr, QUP_STATE, HAL_QUPE_RUNSTATE_CMD_PAUSE);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_STATE = %x", (uint32) pQupAddr, HAL_QUPE_RUNSTATE_CMD_PAUSE);
         break;

      default:
         return (HAL_QUPE_RESULT_INVALID_ARG);
   }

   return HAL_QUPE_RESULT_SUCCESS;
}


/** @brief Gets the run state of the QUPe.
    @param[in]   pQupAddr Qup Block address.
    @return QUPe run state.
  */
HAL_qupe_RunStateType
HAL_qupe_GetRunState
(
   uint8                 *pQupAddr
)
{
   uint32 uStateField;
   uint32 uValidField;
   uint32 uRegVal;

   uRegVal = QUPE_IN(pQupAddr, QUP_STATE);
   uValidField = (uRegVal & (QUPE_FMSK(QUP_STATE, STATE_VALID))) >>
         QUPE_SHFT(QUP_STATE, STATE_VALID);
   if ( 1 == uValidField ) {
      uStateField = (uRegVal & (QUPE_FMSK(QUP_STATE, STATE))) >>
         QUPE_SHFT(QUP_STATE, STATE);
   }
   else {
      uStateField = HAL_QUPE_RUNSTATE_INVALID;
   }

   QUPLOG_LOG_READ((uint32)pQupAddr, 4,
                   "QUP[0x%x]: Read: QUP_STATE=0x%x, QUP_STATE[STATE]=0x%x, "
                   "QUP_STATE[STATE_VALID]=0x%x",
                   (uint32)pQupAddr, uRegVal,uStateField, uValidField);
   return (HAL_qupe_RunStateType)uStateField;
}


/** @brief Get's the output and input mode.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  pOutMode pointer to store output mode.
    @param[in]  pInpMode pointer to store input mode.
    @return     Nothing.
    @sa HAL_qupe_SetOutInMode
  */
void
HAL_qupe_GetOutInMode
(
   uint8               *pQupAddr,
   HAL_qupe_IOModeType *pOutMode,
   HAL_qupe_IOModeType *pInpMode
)
{
   uint32 uRegVal;
   uRegVal = QUPE_IN(pQupAddr, QUP_IO_MODES);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_IO_MODES , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);
   *pOutMode = (HAL_qupe_IOModeType) 
       ( (uRegVal &(QUPE_FMSK(QUP_IO_MODES, OUTPUT_MODE))) >>
         QUPE_SHFT(QUP_IO_MODES, OUTPUT_MODE));
   *pInpMode = (HAL_qupe_IOModeType) 
       ( (uRegVal &(QUPE_FMSK(QUP_IO_MODES, INPUT_MODE))) >>
         QUPE_SHFT(QUP_IO_MODES, INPUT_MODE));
}


/** @brief Sets the output and input mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceOutMode  output mode FIFO, BLOCK or BAM mode.
    @param[in]  ceInpMode  input mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
/*
void
HAL_qupe_SetOutInMode
(
   uint8 *pQupAddr,
   const HAL_qupe_IOModeType ceOutMode,
   const HAL_qupe_IOModeType ceInpMode
)
{
   uint32 uRegVal;

   HAL_QUPE_ASSERT((ceOutMode < HAL_QUPE_IO_MODE_FLD_MIN) ||
                   (ceOutMode > HAL_QUPE_IO_MODE_FLD_MAX));
   HAL_QUPE_ASSERT((ceInpMode < HAL_QUPE_IO_MODE_FLD_MIN) ||
                   (ceInpMode > HAL_QUPE_IO_MODE_FLD_MAX));

   HAL_QUPE_ASSERT((ceOutMode != HAL_QUPE_IO_MODE_FIFO) &&
                   (0 == QUPE_INF(pQupAddr, QUP_IO_MODES, OUTPUT_BLOCK_SIZE)));
   HAL_QUPE_ASSERT((ceInMode != HAL_QUPE_IO_MODE_FIFO) &&
                   (0 == QUPE_INF(pQupAddr, QUP_IO_MODES, INPUT_BLOCK_SIZE)));
   
   uRegVal = QUPE_IN(pQupAddr, QUP_IO_MODES);
   // clear the field
   uRegVal &= (~(QUPE_FMSK(QUP_IO_MODES, OUTPUT_MODE) | QUPE_FMSK(QUP_IO_MODES, INPUT_MODE)));
   uRegVal |=((QUPE_FMSK(QUP_IO_MODES, OUTPUT_MODE) &
              (ceOutMode <<QUPE_SHFT(QUP_IO_MODES, OUTPUT_MODE)) ) |
             (QUPE_FMSK(QUP_IO_MODES, INPUT_MODE) &
              (ceInpMode <<QUPE_SHFT(QUP_IO_MODES, INPUT_MODE)) ));

   QUPE_OUT(pQupAddr, QUP_IO_MODES, uRegVal);
}
*/

/** @brief Sets the output mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceOutMode  output mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
void
HAL_qupe_SetOutIoMode
(
   uint8 *pQupAddr                ,
   const HAL_qupe_IOModeType ceOutMode
)
{
   HAL_QUPE_ASSERT((ceOutMode < HAL_QUPE_IO_MODE_FLD_MIN) ||
                   (ceOutMode > HAL_QUPE_IO_MODE_FLD_MAX));

   HAL_QUPE_ASSERT((ceOutMode != HAL_QUPE_IO_MODE_FIFO) &&
                   (0 == QUPE_INF(pQupAddr, QUP_IO_MODES, OUTPUT_BLOCK_SIZE)));

   /* Potentially switching modes so reset counters.
    * This is a HW bug. If counters are not cleared they may affect bam mode. */
   
   QUPE_OUTF(pQupAddr, QUP_IO_MODES, OUTPUT_MODE, ceOutMode);
   QUPE_OUT(pQupAddr, QUP_MX_WRITE_COUNT,  0);
   QUPE_OUT(pQupAddr, QUP_MX_OUTPUT_COUNT, 0);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES[OUTPUT_MODE] = %x", (uint32)pQupAddr, ceOutMode);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_MX_WRITE_COUNT  = %x", (uint32)pQupAddr, 0);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_MX_OUTPUT_COUNT = %x", (uint32)pQupAddr, 0);
}


/** @brief Sets the input mode to FIFO, Block or BAM Mode.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceInpMode  input mode FIFO, BLOCK or BAM mode.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
void
HAL_qupe_SetInIoMode
(
   uint8 *pQupAddr                ,
   const HAL_qupe_IOModeType ceInpMode
)
{
   HAL_QUPE_ASSERT((ceInpMode < HAL_QUPE_IO_MODE_FLD_MIN) ||
                   (ceInpMode > HAL_QUPE_IO_MODE_FLD_MAX));

   HAL_QUPE_ASSERT((ceInpMode != HAL_QUPE_IO_MODE_FIFO) &&
                   (0 == QUPE_INF(pQupAddr, QUP_IO_MODES, INPUT_BLOCK_SIZE)));
   
   QUPE_OUTF(pQupAddr, QUP_IO_MODES, INPUT_MODE, ceInpMode);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES[INPUT_MODE] = %x", (uint32)pQupAddr, ceInpMode);

   QUPE_OUT(pQupAddr, QUP_MX_READ_COUNT, 0);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_READ_COUNT = %x", (uint32)pQupAddr, 0);

   QUPE_OUT(pQupAddr, QUP_MX_INPUT_COUNT, 0);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_INPUT_COUNT = %x", (uint32)pQupAddr, 0);
}

/** @brief Sets the input/output mode to FIFO, Block or BAM Mode.
    Also configures packing/unpacking and output bit shifting.
 
    @param[in]  pQupAddr Qup Block address.
    @param[in]  ceOutMode  output mode FIFO, BLOCK or BAM mode.
    @param[in]  ceInpMode  input mode FIFO, BLOCK or BAM mode.
    @param[in]  bOutBitShiftEn Enable Output bit shifting .
    @param[in]  bPackEn        Enable packing.
    @param[in]  bUnpackEn      Enable unpacking.
    @return     None.
    @note based on RTL setting only FIFO Mode may be available.
    @sa HAL_qupe_GetOutInMode
  */
void
HAL_qupe_SetIoModeExt
(
   uint8 *pQupAddr                    ,
   HAL_qupe_IOModeType ceOutMode,
   HAL_qupe_IOModeType ceInpMode,
   boolean             bOutBitShiftEn,
   boolean             bPackEn,
   boolean             bUnpackEn
)
{
   uint32 uIoMode;
   uint32 uBitShftEn, uPackEn, uUnpackEn;


   uBitShftEn = bOutBitShiftEn ? 1: 0;
   uPackEn    = bPackEn ? 1: 0;
   uUnpackEn  = bUnpackEn ? 1: 0;

   uIoMode = ( QUPE_FMSK(QUP_IO_MODES, OUTPUT_MODE) &
                 (ceOutMode << QUPE_SHFT(QUP_IO_MODES, OUTPUT_MODE)) ) |
             ( QUPE_FMSK(QUP_IO_MODES, INPUT_MODE) &
                 (ceInpMode << QUPE_SHFT(QUP_IO_MODES, INPUT_MODE)) )  |
             ( QUPE_FMSK(QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN) &
                 (uBitShftEn << QUPE_SHFT(QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN)) ) |
             ( QUPE_FMSK(QUP_IO_MODES, PACK_EN) &
                 (uPackEn << QUPE_SHFT(QUP_IO_MODES, PACK_EN)) ) |
             ( QUPE_FMSK(QUP_IO_MODES, UNPACK_EN) &
                 (uUnpackEn << QUPE_SHFT(QUP_IO_MODES, UNPACK_EN)) );
   QUPE_OUT(pQupAddr, QUP_IO_MODES, uIoMode);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES = %x", (uint32) pQupAddr, uIoMode);
   /* Potentially switching modes so reset counters.
    * This is a HW bug. If counters are not cleared they may affect bam mode. */
   if ( HAL_QUPE_IO_MODE_BAM == ceOutMode ) {
      QUPE_OUT(pQupAddr, QUP_MX_WRITE_COUNT,  0);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write: QUP_MX_WRITE_COUNT  = %x", (uint32) pQupAddr, 0);
      QUPE_OUT(pQupAddr, QUP_MX_OUTPUT_COUNT, 0);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write: QUP_MX_OUTPUT_COUNT = %x", (uint32) pQupAddr, 0);
   }
   if ( HAL_QUPE_IO_MODE_BAM == ceInpMode ) {
      QUPE_OUT(pQupAddr, QUP_MX_READ_COUNT, 0);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_READ_COUNT = %x", (uint32) pQupAddr, 0);

      QUPE_OUT(pQupAddr, QUP_MX_INPUT_COUNT, 0);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_INPUT_COUNT = %x", (uint32) pQupAddr, 0);        
   }
}


/** @brief Clears the specified operational flags.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uOpFlagsBitmap bitmap of HAL_qupe_OpStateType
                flags to be cleared.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_ClearOpStatusFlags
(
   uint8         *pQupAddr,
   const uint32  uOpFlagsBitmap
)
{
   if(uOpFlagsBitmap & (~HAL_QUPE_OPSTATE_FIELD)){
      return (HAL_QUPE_RESULT_INVALID_ARG);
   }
   QUPE_OUT(pQupAddr, QUP_OPERATIONAL, uOpFlagsBitmap);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_OPERATIONAL = 0x%x", (uint32)pQupAddr, uOpFlagsBitmap);
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Gets the operational flags.
    @param[in]  pQupAddr Qup Block address.
    @return bitmap with operational flags from
            HAL_qupe_OpStateType set.
  */
uint32
HAL_qupe_GetOpStatusFlags
(
   uint8  *pQupAddr
)
{
   uint32 uRegVal;

   uRegVal = QUPE_IN(pQupAddr, QUP_OPERATIONAL);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_OPERATIONAL , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);     
   return uRegVal;
}


/** @brief Clears the specified operational flags.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uErrFlagsBitmap bitmap of error flags from
                HAL_qupe_ErrStateType enum to be cleared.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_ClearErrStatusFlags
(
   uint8  *pQupAddr,
   const uint32  uErrFlagsBitmap
)
{
   if(uErrFlagsBitmap & (~HAL_QUPE_ERRSTATE_FIELD)){
      return (HAL_QUPE_RESULT_INVALID_ARG);
   }
   QUPE_OUT(pQupAddr, QUP_ERROR_FLAGS, uErrFlagsBitmap);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_ERROR_FLAGS = %x", (uint32)pQupAddr, uErrFlagsBitmap);
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Gets the error flags.
    @param[in]  pQupAddr Qup Block address.
    @return bitmap with error flags from HAL_qupe_ErrStateType enum set.
  */
uint32
HAL_qupe_GetErrStatusFlags
(
   uint8  *pQupAddr
)
{
   uint32 uRegVal;
   
   uRegVal = QUPE_IN(pQupAddr, QUP_ERROR_FLAGS);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_ERROR_FLAGS , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);   
   return uRegVal;
}


/** @brief Sets the enabled error flags list.
          If set, each flag enables generation of qup_error_irq and the
          setting of the corresponding error flag in the QUP_ERROR_FLAGS
          register for the specified error case.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uEnabledErrFlagsBm bitmap of error flags from
                HAL_qupe_ErrStateType enum to enable.
    @return  HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
    @sa HAL_qupe_GetEnabledErrStatusFlags
  */
HAL_qupe_ResultType
HAL_qupe_SetErrStatusIrqEnabled
(
   uint8         *pQupAddr,
   const uint32  uEnabledErrFlagsBm
)
{
   if(uEnabledErrFlagsBm & (~HAL_QUPE_ERRSTATE_FIELD)){
      return (HAL_QUPE_RESULT_INVALID_ARG);
   }
   QUPE_OUT(pQupAddr, QUP_ERROR_FLAGS_EN, uEnabledErrFlagsBm);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_ERROR_FLAGS_EN = %x", (uint32)pQupAddr, uEnabledErrFlagsBm);
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Get the list of enabled error flags.
          If set, each flag enables generation of qup_error_irq and the
          setting of the corresponding error flag in the QUP_ERROR_FLAGS
          register for the specified error case.
    @param[in]  pQupAddr Qup Block address.
    @return bitmap with error flags from HAL_qupe_ErrStateType enum set.
    @sa HAL_qupe_SetEnabledErrStatusFlags
  */
uint32
HAL_qupe_GetErrStatusIrqEnabled
(
   uint8  *pQupAddr
)
{
   uint32 uRegVal;

   uRegVal = QUPE_IN(pQupAddr, QUP_ERROR_FLAGS_EN);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_ERROR_FLAGS_EN , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);
   return uRegVal;
}

/** @brief Sets the interrupt mask.
          If set, each flag masks generation of interrupt..
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uIrqBm bitmap from HAL_qupe_IrqMask of interrupts
                to be masked.
    @return  Nothing.
    @sa HAL_qupe_GetEnabledErrStatusFlags
  */
void HAL_qupe_SetIrqMask
(
   uint8         *pQupAddr,
   const uint32  uIrqBm
)
{
   HAL_QUPE_ASSERT(uIrqBm & (~HAL_QUPE_IRQMASK_FIELD));

   QUPE_OUT(pQupAddr, QUP_OPERATIONAL_MASK, uIrqBm);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_OPERATIONAL_MASK = %x",
                    (uint32)pQupAddr, uIrqBm);
}

/** @brief Get the list of masked interrupts.
          If set, each flag masks irq generation.
    @param[in]  pQupAddr Qup Block address.
    @return bitmap with interrupt source flags from HAL_qupe_IrqMask enum set.
    @sa HAL_qupe_SetEnabledErrStatusFlags
  */
uint32
HAL_qupe_GetIrqMask
(
   uint8  *pQupAddr
)
{
   uint32 uRegVal;

   uRegVal = QUPE_IN(pQupAddr, QUP_OPERATIONAL_MASK);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_OPERATIONAL_MASK , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);
   return uRegVal;
}

/** @brief Sets the number of writes of size N to the Mini Core per
           RUN_STATE. A value of zero indicates the output count
           function is not enabled for use.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uCount the number of writes.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetOutTransferCount
(
   uint8         *pQupAddr,
   const uint32  uCount
)
{
   HAL_qupe_IOModeType eOutMode,eInMode;

   HAL_qupe_GetOutInMode(pQupAddr, &eOutMode, &eInMode);
   if (HAL_QUPE_IO_MODE_FIFO == eOutMode) {
      if (uCount > HAL_QUPE_FIFO_MODE_OUT_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      else {
         QUPE_OUT(pQupAddr, QUP_MX_WRITE_COUNT, uCount);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_MX_WRITE_COUNT = %x", (uint32)pQupAddr, uCount);
      }
   } 
   else { // set count for block/bam mode
      if(uCount > HAL_QUPE_BLOCK_MODE_OUT_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      else {
         QUPE_OUT(pQupAddr, QUP_MX_OUTPUT_COUNT, uCount);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_MX_OUTPUT_COUNT = %x", (uint32)pQupAddr, uCount);
      }
   }
   
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Sets the number of writes of size N to the Mini Core per
           RUN_STATE. This is a more efficient implementation of the function.

    @param[in]  pQupAddr Qup Block address.
    @param[in]  eOutMode Output mode.
    @param[in]  uCount the number of writes.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetOutTransferCountExt
(
   uint8               *pQupAddr,
   HAL_qupe_IOModeType  eOutMode,
   uint32               uCount
)
{

   if (HAL_QUPE_IO_MODE_FIFO == eOutMode) {
      if (uCount > HAL_QUPE_FIFO_MODE_OUT_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      else {
         QUPE_OUT(pQupAddr, QUP_MX_WRITE_COUNT, uCount);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_WRITE_COUNT = %x", (uint32) pQupAddr, uCount);
      }
   } 
   else { // set count for block/bam mode
      if(uCount > HAL_QUPE_BLOCK_MODE_OUT_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      else {
         QUPE_OUT(pQupAddr, QUP_MX_OUTPUT_COUNT, uCount);
         QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_OUTPUT_COUNT = %x", (uint32) pQupAddr, uCount);
      }
   }
   
   return HAL_QUPE_RESULT_SUCCESS;
}


/** @brief Gets the Number of slots filled in the output FIFO.
    @param[in]  pQupAddr Qup Block address.
    @return Number of slots filled in the output FIFO.
  */
uint32
HAL_qupe_GetOutFifoOccupiedSlotsCount
(
   uint8  *pQupAddr
)
{
   uint32 uFieldVal;

   uFieldVal = QUPE_INF(pQupAddr, QUP_OUTPUT_FIFO_WORD_CNT, OUTPUT_FIFO_WORD_CNT);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_OUTPUT_FIFO_WORD_CNT[OUTPUT_FIFO_WORD_CNT] , VAL:0x%x",
                   (uint32)pQupAddr, uFieldVal);   
   return uFieldVal;
}


/** @brief Puts a word into output fifo.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uOutWord the word to be put in output fifo.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_PutWordIntoOutFifo
(
   uint8        *pQupAddr,
   const uint32 uOutWord
)
{
   QUPE_OUTI(pQupAddr, QUP_OUTPUT_FIFOc, 0, uOutWord);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write:  QUP_OUTPUT_FIFOc[] = %x", 
                    (uint32)pQupAddr, uOutWord);
   return HAL_QUPE_RESULT_SUCCESS;
}


/** @brief Sets the number of reads of size N from the Mini Core per
           RUN_STATE. A value of zero indicates the input count
           function is not enabled for use.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  uTransferCount the max number of reads.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetInTransferCount
(
   uint8        *pQupAddr,
   const uint32  uTransferCount
)
{
   HAL_qupe_IOModeType eOutMode,eInMode;

   HAL_qupe_GetOutInMode(pQupAddr, &eOutMode, &eInMode);
   if (HAL_QUPE_IO_MODE_FIFO == eInMode) {
      if(uTransferCount > HAL_QUPE_FIFO_MODE_IN_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      QUPE_OUT(pQupAddr, QUP_MX_READ_COUNT, uTransferCount);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_READ_COUNT = %x", (uint32)pQupAddr, uTransferCount);
   }
   else {
      if(uTransferCount > HAL_QUPE_BLOCK_MODE_IN_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
   
      QUPE_OUT(pQupAddr, QUP_MX_INPUT_COUNT, uTransferCount);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_INPUT_COUNT = %x", (uint32)pQupAddr, uTransferCount);
   }
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Sets the number of reads of size N from the Mini Core per
           RUN_STATE. A value of zero indicates the input count
           function is not enabled for use. This functin does no reads.

    @param[in]  pQupAddr Qup Block address.
    @param[in]  eInMode Input mode.
    @param[in]  uTransferCount the max number of reads.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetInTransferCountExt
(
   uint8                     *pQupAddr,
   const HAL_qupe_IOModeType  eInMode,
   const uint32               uTransferCount
)
{

   if (HAL_QUPE_IO_MODE_FIFO == eInMode) {
      if(uTransferCount > HAL_QUPE_FIFO_MODE_IN_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
      QUPE_OUT(pQupAddr, QUP_MX_READ_COUNT, uTransferCount);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_READ_COUNT = %x", (uint32) pQupAddr, uTransferCount);
   }
   else {
      if(uTransferCount > HAL_QUPE_BLOCK_MODE_IN_TRANSFER_CNT_MAX){
         return (HAL_QUPE_RESULT_INVALID_ARG);
      }
   
      QUPE_OUT(pQupAddr, QUP_MX_INPUT_COUNT, uTransferCount);
      QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_MX_INPUT_COUNT = %x", (uint32) pQupAddr, uTransferCount);
   }
   return HAL_QUPE_RESULT_SUCCESS;
}


/** @brief Gets the number of reads of size N from the Mini Core per
           RUN_STATE. A value of zero indicates the input count
           function is not enabled for use.
    @param[in]  pQupAddr Qup Block address.
    @return the max number of reads.
  */
uint32
HAL_qupe_GetInTransferCountCurrent
(
   uint8  *pQupAddr
)
{
   HAL_qupe_IOModeType eOutMode,eInMode;

   HAL_qupe_GetOutInMode(pQupAddr, &eOutMode, &eInMode);
   if (HAL_QUPE_IO_MODE_FIFO == eInMode) {
      return QUPE_INF(pQupAddr, QUP_MX_READ_COUNT, MX_READ_COUNT) - QUPE_INF(pQupAddr, QUP_MX_READ_CNT_CURRENT, MX_READ_CNT_CURRENT);
   }
   else if (HAL_QUPE_IO_MODE_BLOCK == eOutMode) {
      return QUPE_INF(pQupAddr, QUP_MX_INPUT_COUNT, MX_INPUT_COUNT) - QUPE_INF(pQupAddr, QUP_MX_INPUT_CNT_CURRENT, MX_INPUT_CNT_CURRENT);
   }
   else {   
      return QUPE_INF(pQupAddr, QUP_MX_INPUT_CNT_CURRENT, MX_INPUT_CNT_CURRENT);
   }
}

/** @brief Gets the number of reads of size N from the Mini Core per
           RUN_STATE. A value of zero indicates the input count
           function is not enabled for use.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  eInMode  Input mode.
    @param[in]  uRdCnt   Programmed read count.
    @return the max number of reads.
  */
uint32
HAL_qupe_GetInTransferCountCurrentExt
(
   uint8               *pQupAddr,
   HAL_qupe_IOModeType  eInMode,
   uint32               uRdCnt
)
{
   uint32 uRdCntCur;

   if (HAL_QUPE_IO_MODE_FIFO == eInMode) {
      uRdCntCur = QUPE_INF(pQupAddr, QUP_MX_READ_CNT_CURRENT, MX_READ_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_READ_CNT_CURRENT[MX_READ_CNT_CURRENT] , VAL:0x%x",
                      (int32)pQupAddr, uRdCntCur);
      return uRdCnt - uRdCntCur;
      
   }
   else if (HAL_QUPE_IO_MODE_BLOCK == eInMode) {
      uRdCntCur = QUPE_INF(pQupAddr, QUP_MX_INPUT_CNT_CURRENT, MX_INPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_INPUT_CNT_CURRENT[MX_INPUT_CNT_CURRENT] , VAL:0x%x",
                      (int32)pQupAddr, uRdCntCur);
      return uRdCnt - uRdCntCur;
   }
   else {
      uRdCntCur = QUPE_INF(pQupAddr, QUP_MX_INPUT_CNT_CURRENT, MX_INPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_INPUT_CNT_CURRENT[MX_INPUT_CNT_CURRENT] , VAL:0x%x",
                      (int32)pQupAddr, uRdCntCur);
      return uRdCntCur;
   }
}

/** @brief Gets the current number of writes of size N to the
          Mini Core. A value of zero indicates the output
          count function is not enabled for use.
    @param[in]  pQupAddr Qup Block address.
    @return the number of writes.
  */
uint32
HAL_qupe_GetOutTransferCountCurrent
(
   uint8  *pQupAddr
)
{
   HAL_qupe_IOModeType eOutMode,eInMode;
   uint32 uWrCntVal, uWrCntCurrVal;

   HAL_qupe_GetOutInMode(pQupAddr, &eOutMode, &eInMode);

   if (HAL_QUPE_IO_MODE_FIFO == eOutMode) {
       uWrCntVal = QUPE_INF(pQupAddr, QUP_MX_WRITE_COUNT, MX_WRITE_COUNT);
       QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                       "QUP[0x%x]: Read: QUP_MX_WRITE_COUNT[MX_WRITE_COUNT] , VAL:0x%x",
                       (uint32)pQupAddr, uWrCntVal);

       uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_WRITE_CNT_CURRENT, MX_WRITE_CNT_CURRENT);
       QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                       "QUP[0x%x]: Read: QUP_MX_WRITE_CNT_CURRENT[MX_WRITE_CNT_CURRENT] , VAL:0x%x",
                       (uint32)pQupAddr, uWrCntCurrVal);

       return  (uWrCntVal - uWrCntCurrVal);
   }
   else if (HAL_QUPE_IO_MODE_BLOCK == eOutMode) {
      uWrCntVal = QUPE_INF(pQupAddr, QUP_MX_OUTPUT_COUNT, MX_OUTPUT_COUNT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_OUTPUT_COUNT[MX_OUTPUT_COUNT] , VAL:0x%x",
                      (uint32)pQupAddr, uWrCntVal);

      uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_OUTPUT_CNT_CURRENT, MX_OUTPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_OUTPUT_CNT_CURRENT[MX_OUTPUT_CNT_CURRENT] , VAL:0x%x",
                      (uint32)pQupAddr, uWrCntCurrVal);

      return (uWrCntVal - uWrCntCurrVal);
   }
   else { /* bam mode */
      uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_OUTPUT_CNT_CURRENT, MX_OUTPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_OUTPUT_CNT_CURRENT[MX_OUTPUT_CNT_CURRENT] , VAL:0x%x",
                      (uint32)pQupAddr, uWrCntCurrVal);
      return  uWrCntCurrVal;
   }
}

/** @brief Gets the current number of writes of size N to the
          Mini Core. A value of zero indicates the output
          count function is not enabled for use.
          This is a more efficient implementation. It does no reads.

    @param[in]  pQupAddr   Qup Block address.
    @param[in]  eOutMode   Output mode.
    @param[in]  uWrCntVal  Programmed Output count.
    @return the number of writes.
  */
uint32
HAL_qupe_GetOutTransferCountCurrentExt
(
   uint8               *pQupAddr,
   HAL_qupe_IOModeType  eOutMode,
   uint32               uWrCntVal
)
{
   uint32 uWrCntCurrVal;

   if (HAL_QUPE_IO_MODE_FIFO == eOutMode) {
       uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_WRITE_CNT_CURRENT, MX_WRITE_CNT_CURRENT);
       QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                       "QUP[0x%x]: Read: QUP_MX_WRITE_CNT_CURRENT[MX_WRITE_CNT_CURRENT] , VAL:0x%x",
                       (int32)pQupAddr, uWrCntCurrVal);

       return  (uWrCntVal - uWrCntCurrVal);
   }
   else if (HAL_QUPE_IO_MODE_BLOCK == eOutMode) {

      uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_OUTPUT_CNT_CURRENT, MX_OUTPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_OUTPUT_CNT_CURRENT[MX_OUTPUT_CNT_CURRENT] , VAL:0x%x",
                      (int32)pQupAddr, uWrCntCurrVal);

      return (uWrCntVal - uWrCntCurrVal);
   }
   else { /* bam mode */
      uWrCntCurrVal = QUPE_INF(pQupAddr, QUP_MX_OUTPUT_CNT_CURRENT, MX_OUTPUT_CNT_CURRENT);
      QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                      "QUP[0x%x]: Read: QUP_MX_OUTPUT_CNT_CURRENT[MX_OUTPUT_CNT_CURRENT] , VAL:0x%x",
                      (int32)pQupAddr, uWrCntCurrVal);
      return  uWrCntCurrVal;
   }
}


/** @brief Gets the Number of filled slots  in the input FIFO.
    @param[in]  pQupAddr Qup Block address.
    @return Number of filled slots in the input FIFO.
  */
uint32
HAL_qupe_GetInFifoOccupiedSlotsCount
(
   uint8  *pQupAddr
)
{
   uint32 uRegVal;

   uRegVal = QUPE_INF(pQupAddr, QUP_INPUT_FIFO_WORD_CNT, INPUT_FIFO_WORD_CNT);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_INPUT_FIFO_WORD_CNT[INPUT_FIFO_WORD_CNT] , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);
   return uRegVal;
}


/** @brief Gets a word from input fifo.
    @param[in]  pQupAddr Qup Block address.
    @param[out] pInWord pointer to location to put word from input fifo.
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_GetWordFromInFifo
(
   uint8  *pQupAddr,
   uint32 *pInWord
)
{
   uint32 uRegVal;

   if(NULL == pInWord){
      return (HAL_QUPE_RESULT_INVALID_ARG);
   }

   /* Reading from address 0 emties the input fifo. */
   uRegVal = QUPE_INI(pQupAddr, QUP_INPUT_FIFOc, 0);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: QUP_INPUT_FIFO0 , VAL:0x%x",
                   (uint32)pQupAddr, uRegVal);
   *pInWord = uRegVal;
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Packs the operational state for debug.
    @param[in]  pQupAddr index of the qup.
    @return     28 bit value indicating state.
  */
uint32 HAL_qupe_Pack28_OperationalState
(
   uint8        *pQupAddr
)
{
   uint32 uI2cStatusVal;
   uint32 uOperationalVal;
   uint32 uQupStateVal;
   uint32 val;

   uI2cStatusVal = QUPE_IN(pQupAddr, I2C_MASTER_STATUS);

   val = (uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, I2C_SCL))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, I2C_SCL);//1bit

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, I2C_SDA))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, I2C_SDA)) << 1;//1bit

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, BUS_ACTIVE))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, BUS_ACTIVE)) << 2;//1bit

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, BUS_MASTER))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, BUS_MASTER)) << 3;//1bit

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, DATA_STATE))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, DATA_STATE)) << 4;

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, CLK_STATE))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, CLK_STATE)) << 7;

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, OUTPUT_FSM_STATE))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, OUTPUT_FSM_STATE)) << 10;

   val |= ((uI2cStatusVal &(QUPE_FMSK(I2C_MASTER_STATUS, INPUT_FSM_STATE))) >>
         QUPE_SHFT(I2C_MASTER_STATUS, INPUT_FSM_STATE)) << 13;

   uOperationalVal = QUPE_IN(pQupAddr, QUP_OPERATIONAL);

   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, OUTPUT_FIFO_NOT_EMPTY))) >>
         QUPE_SHFT(QUP_OPERATIONAL, OUTPUT_FIFO_NOT_EMPTY)) << 16;

   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, INPUT_FIFO_NOT_EMPTY))) >>
         QUPE_SHFT(QUP_OPERATIONAL, INPUT_FIFO_NOT_EMPTY)) << 17;
   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, OUTPUT_SERVICE_FLAG))) >>
         QUPE_SHFT(QUP_OPERATIONAL, OUTPUT_SERVICE_FLAG)) << 18;
   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, INPUT_SERVICE_FLAG))) >>
         QUPE_SHFT(QUP_OPERATIONAL, INPUT_SERVICE_FLAG)) << 19;
   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, MAX_OUTPUT_DONE_FLAG))) >>
         QUPE_SHFT(QUP_OPERATIONAL, MAX_OUTPUT_DONE_FLAG)) << 20;
   val |= ((uOperationalVal &(QUPE_FMSK(QUP_OPERATIONAL, MAX_INPUT_DONE_FLAG))) >>
         QUPE_SHFT(QUP_OPERATIONAL, MAX_INPUT_DONE_FLAG)) << 21;

   uQupStateVal = QUPE_IN(pQupAddr, QUP_STATE);

   val |= ((uQupStateVal &(QUPE_FMSK(QUP_STATE, STATE))) >>
         QUPE_SHFT(QUP_STATE, STATE)) << 22;
   val |= ((uQupStateVal &(QUPE_FMSK(QUP_STATE, STATE_VALID))) >>
         QUPE_SHFT(QUP_STATE, STATE_VALID)) << 24;

   val |= (1 <<27);//1bit mark as an operational status within timeout class

   return val;
}

/**
 * This function configures the specified QUP Mini core selection.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eMiniCoreType - The mini core type.
 */
void HAL_qupe_SetSelectedMinicore
(
   uint8  *pQupAddr, 
   HAL_qupe_MiniCoreType eMiniCoreType
)
{
   QUPE_OUTF(pQupAddr, QUP_CONFIG, MINI_CORE, eMiniCoreType);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_CONFIG[MINI_CORE] = %x", (uint32)pQupAddr, eMiniCoreType);
} /* END HAL_qupe_SetSelectedMinicore */
