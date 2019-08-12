/*
===========================================================================

FILE:   HALqupeSpi.c

DESCRIPTION:
    This file implements a qup hal specific to SPI related operations

===========================================================================

        Edit History

$Header: 

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/06/12 ddk    Created

===========================================================================
        Copyright c 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "HALqupe.h"
#include "HALqupeTarget.h"


#define HAL_QUPE_VERSION_STRING "HAL_QUPE_V_2.0"

#define HAL_QUPE_MAX(x,y) ((x>y)?x:y)



//#include "BusSpiPlatLog.h"

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


/**
 * This function configures the SPI device-specific parameters
 * before starting an SPI transfer.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param pmDeviceParameters - Pointer to the HAL_QUPE_SPI_DeviceConfiguration
 *                          struct which contains the device configuration
 *                          parameters.
 */
void HAL_qupe_Spi_SetDeviceParameters(uint8  *pQupAddr, 
  HAL_QUPE_SPI_DeviceParametersType *pmDeviceParameters)
{
   uint32 u32Temp;
   uint32 u32TempMask;

   /* Configure the clock polarity to use for this device */
   if(pmDeviceParameters->eClockPolarity == HAL_QUPE_SPI_CLK_IDLE_HIGH)
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CLK_IDLE_HIGH, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CLK_IDLE_HIGH, 0);
   }

   /* Configure the clock phase to use for this device */
   if(pmDeviceParameters->eTransferMode == HAL_QUPE_SPI_INPUT_FIRST_MODE)
   {
      QUPE_OUTF(pQupAddr, SPI_CONFIG, INPUT_FIRST, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, SPI_CONFIG, INPUT_FIRST, 0);
   }

   /* Configure whether the clock should always be on while talking
    * to this device */
   if(pmDeviceParameters->bClockAlwaysOn)
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CLK_ALWAYS_ON, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CLK_ALWAYS_ON, 0);
   }

   /* Configure whether the chip select should stay asserted during
    * the entire transfer to this device
    */
   if(pmDeviceParameters->bChipSelectAlwaysOn)
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, MX_CS_MODE, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, MX_CS_MODE, 0);
   }

   /*
    * Configure the chip select polarity for this device.
    *
    * Active high chip select is configured by setting bit
    * corresponding to the chip select number in the CS_N_POLARITY
    * field of SPI_IO_CONTROL.
    *
    * We read the field first and mask out the other bits in order
    * to preserve the configurations for the other chip selects.
    */
   u32Temp = QUPE_INF(pQupAddr, SPI_IO_CONTROL, CS_N_POLARITY);
   u32TempMask = 1 << pmDeviceParameters->nChipSelectId;
   if(pmDeviceParameters->eCSPolarity == HAL_QUPE_SPI_CS_ACTIVE_HIGH)
   {
      u32Temp |= u32TempMask;
   }
   else
   {
      u32Temp &= ~u32TempMask;
   }
   QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CS_N_POLARITY, u32Temp);

   /* Configure which chip select to use to talk to the device */
   QUPE_OUTF(pQupAddr, SPI_IO_CONTROL, CS_SELECT, 
                 pmDeviceParameters->nChipSelectId);

   /* Configure the deassertion time (inter-byte delay) between transfers
    * to this device */
   if(pmDeviceParameters->nDeassertionClockTicks > 0)
   {
      /* SPI_DEASSERT_WAIT waits the specified number of clock ticks plus one, 
       * so we need to subtract one from the desired number of clock ticks when 
       * writing to the register
       */
      QUPE_OUT(pQupAddr, SPI_DEASSERT_WAIT, 
                   pmDeviceParameters->nDeassertionClockTicks-1);
   }
   else
   {
      /* If value of zero was specified, then we write 0, which means the
       * deassertion time will be one clock cycle */
      QUPE_OUT(pQupAddr, SPI_DEASSERT_WAIT, 0);
   }

  QUPE_OUT(pQupAddr, QUP_OPERATIONAL_MASK, 0);
   /* the mini core status is reset if there was a HAL_QUPE_SPI_Reset, we have to 
    * reset it back to SPI mode.
    */
   //HALSPIi_MiniCore_Init(pQupAddr);
} /* END HAL_QUPE_SPI_SetDeviceParameters */

/**
 * This function configures the transfer-specific parameters
 * before starting an SPI transfer.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param pmTransferParameters - Pointer to the
 *                  HAL_QUPE_SPI_TransferParametersType struct which
 *                  contains the transfer parameters.
 */
void HAL_qupe_Spi_SetTransferParameters
(
   uint8  *pQupAddr, 
   HAL_QUPE_SPI_TransferParametersType *pmTransferParameters
)
{
   /* Configure the number of bits per transfer unit.
    *
    * The SPI_CONFIG register uses the register value plus one as
    * the number of bits, so we need to subtract one before passing to the
    * register.
    */
   QUPE_OUTF(pQupAddr, QUP_CONFIG, N, (pmTransferParameters->nNumBits - 1));

   /* Configure whether internal loopback mode is used for the transfer. */
   if(pmTransferParameters->eLoopbackMode == HAL_QUPE_SPI_LOOPBACK_ENABLED)
   {
      QUPE_OUTF(pQupAddr, SPI_CONFIG, LOOP_BACK, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, SPI_CONFIG, LOOP_BACK, 0);
   }

   /* Configure whether data received into the SPI input FIFO should be
    * packed into the FIFO word when it is shifted in from the SPI bus */
   if(pmTransferParameters->bReceivedDataPacked)
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, PACK_EN, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, PACK_EN, 0);
   }

   /* Configure whether output data should be unpacked from the SPI
    * output FIFO as it is sent out on the bus.
    */
   if(pmTransferParameters->bTransmitDataPacked)
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, UNPACK_EN, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, UNPACK_EN, 0);
   }

   /*
    * Configure whether the output bit shift function should
    * be enabled or disabled.
    */
   if(pmTransferParameters->bOutputBitShift)
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN, 1);
   }
   else
   {
      QUPE_OUTF(pQupAddr, QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN, 0);
   }

   QUPE_OUT(pQupAddr, QUP_MX_INPUT_COUNT, pmTransferParameters->nNumInputTransfers);
   QUPE_OUT(pQupAddr, QUP_MX_OUTPUT_COUNT, pmTransferParameters->nNumOutputTransfers);
   QUPE_OUTF(pQupAddr, QUP_IO_MODES, INPUT_MODE, pmTransferParameters->eIOMode);
   QUPE_OUTF(pQupAddr, QUP_IO_MODES, OUTPUT_MODE, pmTransferParameters->eIOMode);

   switch (pmTransferParameters->eTransferDirection)
   {
      case HAL_QUPE_SPI_INPUT_ONLY:
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_OUTPUT, 1);
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_INPUT, 0);
         break;

      case HAL_QUPE_SPI_OUTPUT_ONLY:
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_OUTPUT, 0);
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_INPUT, 1);
         break;

      case HAL_QUPE_SPI_BI_DIRECTIONAL:
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_OUTPUT, 0);
         QUPE_OUTF(pQupAddr, QUP_CONFIG, NO_INPUT, 0);
         break;

      default:
         break;
   }

}
/**
 * This function configures the specified SPI core to run in
 * master or slave mode.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eCoreMode - The core mode.
 */
void HAL_qupe_Spi_SetCoreMode
(
   uint8  *pQupAddr, 
   HAL_QUPE_SPI_CoreModeType eCoreMode
)
{
   switch (eCoreMode)
   {
      case HAL_QUPE_SPI_SLAVE:
         QUPE_OUTF(pQupAddr, SPI_CONFIG, SLAVE_OPERATION, 1);
         break;

      case HAL_QUPE_SPI_MASTER:
         QUPE_OUTF(pQupAddr, SPI_CONFIG, SLAVE_OPERATION, 0);
         break;

      default:
         break;
   }
} /* END HAL_spi_SetCoreMode */


