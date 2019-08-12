/*
===========================================================================

FILE:   HALqupe.c

DESCRIPTION:
    This file implements a qup hal.

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/hal/HALqupe.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
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
        Copyright c 2008, 2009, 2010-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "HALqupe.h"
#include "HALqupeTarget.h"

// Define and include the header file that adds the relevant log headers
#define HAL_QUPE_LOG
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

   //uTmpField = QUPE_INF(pQupAddr, QUP_STATE, STATE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 4,
                   "QUP[0x%x]: Read: QUP_STATE=0x%x, QUP_STATE[STATE]=0x%x, "
                   "QUP_STATE[STATE_VALID]=0x%x",
                   (uint32)pQupAddr, uRegVal,uStateField, uValidField);
   return (HAL_qupe_RunStateType)uStateField;
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
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2, "QUP[0x%x]: Write: QUP_CONFIG[MINI_CORE] = %x", (uint32) pQupAddr, eMiniCoreType);
} /* END HAL_qupe_SetSelectedMinicore */
