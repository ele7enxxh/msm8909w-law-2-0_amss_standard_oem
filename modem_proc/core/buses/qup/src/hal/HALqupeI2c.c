/*
===========================================================================

FILE:   HALqupeI2c.c

DESCRIPTION:
    This file implements qup hal functions specific to I2c operations.

===========================================================================

        Edit History

$Header: 

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/12/12 LK     Driver sequence validation. Removing nonexisting reg writes.
06/12/12 LK     Added flush and error state clearing.
06/06/12 ddk    Created

===========================================================================
        Copyright c 2012-2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "HALqupe.h"
#include "HALqupeTarget.h"
#include "QupLog.h"



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
#define HAL_QUPE_MIN_FS_DIV_VAL                         3
#define HAL_QUPE_MIN_HS_DIV_VAL                         3

/** @brief Configures the mini core for i2c use.
    @param[in]  pQupAddr Qup Block address.
    @return Nothing.
  */
void
HAL_qupe_Configure
(
   uint8 *pQupAddr
)
{
   QUPE_OUTF(pQupAddr, QUP_CONFIG, MINI_CORE, HAL_QUPE_MINI_CORE_I2C_MASTER);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[MINI_CORE] = %x",
                    (uint32) pQupAddr, HAL_QUPE_MINI_CORE_I2C_MASTER);
   
   QUPE_OUTF(pQupAddr, QUP_CONFIG, N, (HAL_QUPE_TRANSFER_SIZE - 1));
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[N] = %x",
                    (uint32) pQupAddr, (HAL_QUPE_TRANSFER_SIZE - 1));

   QUPE_OUTF(pQupAddr, QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN, 0);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES[OUTPUT_BIT_SHIFT_EN] = %x",
                    (uint32) pQupAddr, 0);

   QUPE_OUTF(pQupAddr, QUP_IO_MODES, PACK_EN, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES[PACK_EN] = %x",
                    (uint32) pQupAddr, 1);

   QUPE_OUTF(pQupAddr, QUP_IO_MODES, UNPACK_EN, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_IO_MODES[UNPACK_EN] = %x",
                    (uint32) pQupAddr, 1);
   
   QUPE_OUTF(pQupAddr, QUP_CONFIG, EN_EXT_OUT_FLAG, 0);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[EN_EXT_OUT_FLAG] = %x",
                    (uint32) pQupAddr, 0);
   QUPE_OUTF(pQupAddr, QUP_CONFIG, FIFO_CLK_GATE_EN, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[FIFO_CLK_GATE_EN] = %x",
                    (uint32)pQupAddr, 1);
   QUPE_OUTF(pQupAddr, QUP_CONFIG, CORE_CLK_ON_EN, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[CORE_CLK_ON_EN] = %x",
                    (uint32)pQupAddr, 1);
   QUPE_OUTF(pQupAddr, QUP_CONFIG, APP_CLK_ON_EN, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,"QUP[0x%x]: Write:  QUP_CONFIG[APP_CLK_ON_EN] = %x",
                    (uint32)pQupAddr, 1);

   QUPE_OUTF(pQupAddr, QUP_OPERATIONAL_MASK, INPUT_SERVICE_MASK, 0); //en QUP irqs
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  QUP_OPERATIONAL_MASK[INPUT_SERVICE_MASK] = %x",
                    (uint32) pQupAddr, 0);

   QUPE_OUTF(pQupAddr, QUP_OPERATIONAL_MASK, OUTPUT_SERVICE_MASK, 0); //en QUP irqs
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  QUP_OPERATIONAL_MASK[OUTPUT_SERVICE_MASK] = %x",
                    (uint32) pQupAddr, 0);

   QUPE_OUTF(pQupAddr, I2C_MASTER_CONFIG, EN_VERSION_TWO_TAG, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  I2C_MASTER_CONFIG[EN_VERSION_TWO_TAG] = %x",
                    (uint32)pQupAddr, 1);
   
   HAL_qupe_SetOutputFifoEnabled(pQupAddr,1);
   HAL_qupe_SetInputFifoEnabled(pQupAddr,1);
}

/** @brief Performs flush for i2c use.
    @param[in]  pQupAddr Qup Block address.
    @return success.
  */
HAL_qupe_ResultType
HAL_qupe_I2cFlush
(
   uint8 *pQupAddr
)
{
   QUPE_OUTF(pQupAddr, QUP_STATE, I2C_FLUSH, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  QUP_STATE[I2C_FLUSH] = %x",
                    (uint32) pQupAddr, 1);
    return HAL_QUPE_RESULT_SUCCESS;
}
/** @brief Sets the fast standard clock given the input clock freq in KHz.
    @param[in]  pQupAddr Qup Block address.
    @param[in]  eI2cTags       Tag version.
    @param[in]  uInputClockKhz Input clock to the i2c core in KHz.
    @param[in]  uI2cBusClockReqKhz Requested clock in KHz.
                I2C freq in KHz.	
   
    @return HAL_QUPE_RESULT_SUCCESS if successful, error code otherwise.
  */
HAL_qupe_ResultType
HAL_qupe_SetI2cConfig
(
   uint8                      *pQupAddr,
   const HAL_qupe_I2cTagType  eI2cTags,
   const uint32               uInputClockKhz,
   const uint32               uI2cBusClockReqKhz
)
{
   uint32 uRegVal;
   uint32 uFsDivider, uHsDivider;
   uint32 uNoiseRejection;
   uint32 uFSBusFreqKHz;
   uint32 uRem;
   uint32 uDenominator;


   uRegVal = 0;

   if ( uI2cBusClockReqKhz > HAL_QUPE_HS_FREQ_KHZ_MAX ) {
       return HAL_QUPE_RESULT_INVALID_I2C_FREQ;     
   }

   uNoiseRejection = (1 << QUPE_SHFT(I2C_MASTER_CLK_CTL, SDA_NOISE_REJECTION)) |
                     (1 << QUPE_SHFT(I2C_MASTER_CLK_CTL, SCL_NOISE_REJECTION)); 

   if ( uI2cBusClockReqKhz <= HAL_QUPE_FS_FREQ_KHZ_MAX ) {
      uFSBusFreqKHz = uI2cBusClockReqKhz;     
   }
   else {
      uFSBusFreqKHz = 400; // default master code freq
      uDenominator = 3*uI2cBusClockReqKhz;
      uHsDivider = uInputClockKhz/uDenominator - 1;
      uRem       = uInputClockKhz % uDenominator;
      if (2*uRem > uDenominator) { // bigger than 1/2 then adjust
         uHsDivider++;     
      }
      
      if ( uHsDivider < HAL_QUPE_MIN_HS_DIV_VAL ) {
         return HAL_QUPE_RESULT_INVALID_HS_DIVIDER;     
      }
      uRegVal |= (uHsDivider << QUPE_SHFT(I2C_MASTER_CLK_CTL, HS_DIVIDER_VALUE) &
                QUPE_FMSK(I2C_MASTER_CLK_CTL, HS_DIVIDER_VALUE));
   }

   uFsDivider = uInputClockKhz/(2*uFSBusFreqKHz) - 3;

   /* The lowest scaler should be 3 */
   if ( uFsDivider < HAL_QUPE_MIN_FS_DIV_VAL ) {
      return HAL_QUPE_RESULT_INVALID_FS_DIVIDER;     
   }
   
   uRegVal |= (uFsDivider << QUPE_SHFT(I2C_MASTER_CLK_CTL, FS_DIVIDER_VALUE) &
                QUPE_FMSK(I2C_MASTER_CLK_CTL, FS_DIVIDER_VALUE));

   uRegVal |= uNoiseRejection;

   QUPE_OUT(pQupAddr, I2C_MASTER_CLK_CTL, uRegVal);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  I2C_MASTER_CLK_CTL = %x",
                    (uint32) pQupAddr, uRegVal);
   
   return HAL_QUPE_RESULT_SUCCESS;
}

/** @brief Issues bus clear cmd that is 9 clock cycles.
    @param[in]  pQupAddr index of the qup.
    @return Nothing.
  */
void
HAL_qupe_ClearI2CBus
(
   uint8  *pQupAddr
)
{
   QUPE_OUT(pQupAddr, I2C_MASTER_BUS_CLEAR, 1);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  I2C_MASTER_BUS_CLEAR = %x",
                    (uint32) pQupAddr, 1);
}

/** @brief Checks if bus is clear.
    @param[in]  pQupAddr index of the qup.
    @return Nothing.
  */
boolean
HAL_qupe_IsI2CBusClkForcedLow
(
   uint8  *pQupAddr
)
{
   uint32 uClkState, uBusActive, uOutFsmState, uI2cMasterReg;

   uI2cMasterReg = QUPE_IN(pQupAddr, I2C_MASTER_STATUS);

   uBusActive = (uI2cMasterReg &
         (QUPE_FMSK(I2C_MASTER_STATUS, BUS_ACTIVE))) >>
          QUPE_SHFT(I2C_MASTER_STATUS, BUS_ACTIVE);

   uOutFsmState = (uI2cMasterReg &
         (QUPE_FMSK(I2C_MASTER_STATUS, OUTPUT_FSM_STATE))) >>
          QUPE_SHFT(I2C_MASTER_STATUS, OUTPUT_FSM_STATE);

   uClkState = (uI2cMasterReg &
         (QUPE_FMSK(I2C_MASTER_STATUS, CLK_STATE))) >>
          QUPE_SHFT(I2C_MASTER_STATUS, CLK_STATE);

   QUPLOG_LOG_READ((uint32)pQupAddr, 3,
                   "QUP[0x%x]: Read: I2C_MASTER_STATUS = 0x%x,"
                   "I2C_MASTER_STATUS[BUS_ACTIVE] = 0x%x,"
                   "I2C_MASTER_STATUS[OFSM_STATE] = 0x%x,"
                   "I2C_MASTER_STATUS[CLK_STATE] = 0x%x",
                   (int32) pQupAddr, uI2cMasterReg, uBusActive, uOutFsmState, uClkState);

   return (
             (HAL_QUPE_I2C_CLOCK_FORCED_LOW_STATE == uClkState) &&
             (
                (HAL_QUPE_I2C_OUT_FSM_IDLE_STATE  == uOutFsmState) ||
                (HAL_QUPE_I2C_OUT_FSM_MIREC_STATE == uOutFsmState)
             )
          ) || (!uBusActive);
}

/** @brief Checks if bus is clear.
    @param[in]  pQupAddr index of the qup.
    @return Nothing.
  */
boolean
HAL_qupe_IsI2CBusBusy
(
   uint8  *pQupAddr
)
{
   uint32 uActiveField, uI2cMasterReg;

   uI2cMasterReg = QUPE_IN(pQupAddr, I2C_MASTER_STATUS);
   uActiveField  = (uI2cMasterReg &
                   (QUPE_FMSK(I2C_MASTER_STATUS, BUS_ACTIVE)) )>>
                   QUPE_SHFT(I2C_MASTER_STATUS, BUS_ACTIVE);
   QUPLOG_LOG_READ((uint32)pQupAddr, 3,
                   "QUP[0x%x]: Read: I2C_MASTER_STATUS = 0x%x, I2C_MASTER_STATUS[BUS_ACTIVE] = 0x%x",
                   (int32)pQupAddr, uI2cMasterReg, uActiveField);

   return (boolean) uActiveField;
}

/** @brief Checks if bus state given by the pointer is clear.
    @param[in]  pI2cStatus  Pointer to i2c status stored with HAL_qupe_GetI2cState.
    @return TRUE if bus busy FALSE otherwise.
  */
boolean
HAL_qupe_IsI2CStateBusy
(
   HAL_qupe_I2cStatus  *pI2cStatus
)
{
   uint32 uActiveField, uI2cMasterReg;

   uI2cMasterReg = pI2cStatus->uRawStatus;
   uActiveField  = (uI2cMasterReg &
                   (QUPE_FMSK(I2C_MASTER_STATUS, BUS_ACTIVE)) )>>
                   QUPE_SHFT(I2C_MASTER_STATUS, BUS_ACTIVE);

   // since this is a stored state do extra checks

   return (boolean) (uActiveField ||
                     (!(uI2cMasterReg & QUPE_FMSK(I2C_MASTER_STATUS, I2C_SCL))) ||
                     (!(uI2cMasterReg & QUPE_FMSK(I2C_MASTER_STATUS, I2C_SDA)))
                    );
}

/** @brief Gets the state of the I2C.
    @param[in]  pQupAddr index of the qup.
    @param[out] pI2cStatus pointer to a location to store status struct.
    @return     Nothing.
  */
void
HAL_qupe_GetI2cState
(
   uint8               *pQupAddr,
   HAL_qupe_I2cStatus  *pI2cStatus
)
{
   HAL_QUPE_ASSERT(NULL == pI2cStatus);

   pI2cStatus->uRawStatus    = QUPE_IN(pQupAddr, I2C_MASTER_STATUS);
   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: I2C_MASTER_STATUS = 0x%x",
                   (int32)pQupAddr, pI2cStatus->uRawStatus);
}

/** @brief Gets the data state of the I2C.
    @param[in]  pQupAddr index of the qup.
    @return     Data State
  */
uint32
HAL_qupe_GetI2cDataState
(
   uint8               *pQupAddr
)
{
   uint32 I2cDataState = 0;

   I2cDataState = (QUPE_IN(pQupAddr, I2C_MASTER_STATUS) &
                   QUPE_FMSK(I2C_MASTER_STATUS, DATA_STATE)) >>
                   QUPE_SHFT(I2C_MASTER_STATUS, DATA_STATE);

   QUPLOG_LOG_READ((uint32)pQupAddr, 2,
                   "QUP[0x%x]: Read: I2C_MASTER_STATUS[DATA_STATE] = 0x%x",
                   (int32)pQupAddr, I2cDataState);

   return I2cDataState;
}

/** @brief Clears I2c errors.
    @param[in]  pQupAddr index of the qup.
    @param[in]  uErrMask error mask to clear.
    @return     Nothing.
  */
void
HAL_qupe_ClearI2cErrors
(
   uint8               *pQupAddr,
   uint32              uErrMask
)
{
   QUPE_OUT(pQupAddr,I2C_MASTER_STATUS,uErrMask);
   QUPLOG_LOG_WRITE((uint32)pQupAddr, 2,
                    "QUP[0x%x]: Write:  I2C_MASTER_STATUS = %x",
                    (uint32) pQupAddr, uErrMask);
}
