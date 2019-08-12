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
#include "bam.h"
#define HAL_QUPE_MAX(x,y) ((x>y)?x:y)
#define HAL_QUPE_SPI_BAM_CHECK_SET_DATA(a,b) ((a==b)?1:0)

/*
 * MAX Deassert wait time 
 */
#define HAL_QUPE_SPI_DEVICE_PLATBAM_MAX_DEASSERT_CLK_TICKS 0x3F


#define HAL_QUPE_BAM_FILL_CE_BY_FIELD(QUP_ADDR,CE_BASE, INDEX,CMD,REG_NAME,FIELD, DATA)   \
        BAM_FILL_CE(CE_BASE, INDEX, \
          (uint32)QUPE_PHYS_ADDR(QUP_ADDR,REG_NAME), \
          CMD , \
          (DATA <<QUPE_SHFT(REG_NAME, FIELD)), \
          QUPE_FMSK(REG_NAME, FIELD))


          
#define HAL_QUPE_BAM_FILL_CE_BY_REG(QUP_ADDR,CE_BASE, INDEX,CMD,REG_NAME,DATA,MASK)   \
        BAM_FILL_CE(CE_BASE, INDEX, \
          (uint32)QUPE_PHYS_ADDR(QUP_ADDR,REG_NAME), \
          CMD , \
          DATA, \
          MASK)

#define HAL_QUPE_BAM_ADD_REG_INFO(REG_NAME,FIELD,data,reg_data,mask) { \
       reg_data |= ((data) << QUPE_SHFT(REG_NAME, FIELD)); \
       mask |= QUPE_FMSK(REG_NAME, FIELD); }

typedef bam_ce_type HAL_qupe_BamCeType;

/**
 * This function shall perform command descriptor creation for 
 * given set of transfer parameters.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param mHalTransferParameters - The pointer to the transfer parameters.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_Spi_SetTransferParameters_CommandElement
(
   uint8         *pQupAddr,
   HAL_QUPE_SPI_TransferParametersType *mHalTransferParameters,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
)
{
   int32 result = HAL_QUPE_RESULT_SUCCESS;
   uint32 data = 0;
   uint32 reg_data = 0;
   uint32 mask = 0;
   uint32 index = 0;
 
   do
   {
      if ((NULL == mHalTransferParameters) || (NULL == halBamCmdDescInfo))
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }

      if (NULL == halBamCmdDescInfo->cmd_element_base)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }
      
      index = halBamCmdDescInfo->cmd_element_index;
      
      /* Setting the transfer mode */
      HAL_QUPE_BAM_FILL_CE_BY_FIELD(pQupAddr, 
         halBamCmdDescInfo->cmd_element_base,
         index,         
         0,
         SPI_CONFIG,
         LOOP_BACK, 
         mHalTransferParameters->eLoopbackMode);

      index++;      
      reg_data = 0;
      mask = 0;   
      
  
      /* Configure the number of bits per transfer unit.
       *
       * The QUP_CONFIG register uses the register value plus one as
       * the number of bits, so we need to subtract one before passing to the
       * register.
       */

      HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, N,
         (mHalTransferParameters->nNumBits - 1),reg_data,mask);

      switch (mHalTransferParameters->eTransferDirection)
      {
         case HAL_QUPE_SPI_INPUT_ONLY:
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_OUTPUT,1,reg_data,mask);
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_INPUT,0,reg_data,mask);         
            break;
   
         case HAL_QUPE_SPI_OUTPUT_ONLY:
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_OUTPUT,0,reg_data,mask);
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_INPUT,1,reg_data,mask);               
            break;
   
         case HAL_QUPE_SPI_BI_DIRECTIONAL:
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_OUTPUT,0,reg_data,mask);
            HAL_QUPE_BAM_ADD_REG_INFO(QUP_CONFIG, NO_INPUT,0,reg_data,mask);           
            break;
   
         default:
            break;
      }

      HAL_QUPE_BAM_FILL_CE_BY_REG(pQupAddr,
         halBamCmdDescInfo->cmd_element_base,
         index,                  
         0,
         QUP_CONFIG,
         reg_data,
         mask);     

      index++;
      reg_data = 0;
      mask = 0;   

      /* Configure whether data received into the SPI input FIFO should be
       * packed into the FIFO word when it is shifted in from the SPI bus 
       */  
      data = 
      HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalTransferParameters->bReceivedDataPacked,1);

      HAL_QUPE_BAM_ADD_REG_INFO(QUP_IO_MODES, PACK_EN,data,reg_data,mask);

   
      /* Configure whether output data should be unpacked from the SPI
       * output FIFO as it is sent out on the bus.
       */
      data = 
      HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalTransferParameters->bTransmitDataPacked,1);

      HAL_QUPE_BAM_ADD_REG_INFO(QUP_IO_MODES, UNPACK_EN,data,reg_data,mask);

  
     /*
      * Configure whether the output bit shift function should
      * be enabled or disabled.
      */
     data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalTransferParameters->bOutputBitShift,1);
  
     HAL_QUPE_BAM_ADD_REG_INFO(QUP_IO_MODES, OUTPUT_BIT_SHIFT_EN,data,reg_data,mask);

     HAL_QUPE_BAM_ADD_REG_INFO(QUP_IO_MODES, INPUT_MODE,
      mHalTransferParameters->eIOMode,reg_data,mask);
     HAL_QUPE_BAM_ADD_REG_INFO(QUP_IO_MODES, OUTPUT_MODE,
      mHalTransferParameters->eIOMode,reg_data,mask);


     HAL_QUPE_BAM_FILL_CE_BY_REG(pQupAddr,
        halBamCmdDescInfo->cmd_element_base,
        index,                  
        0,
        QUP_IO_MODES,
        reg_data,
        mask);     

      index++;
      reg_data = 0;
      mask = 0;   

      /*Set Input and output counts*/
      {
         uint32 tmp_input_cnt = 0;
         if (mHalTransferParameters->nNumInputTransfers !=
             mHalTransferParameters->nNumOutputTransfers)
         {
            tmp_input_cnt = mHalTransferParameters->nNumInputTransfers;
         }

         HAL_QUPE_BAM_FILL_CE_BY_FIELD(pQupAddr, 
            halBamCmdDescInfo->cmd_element_base,
            index,         
            0,
            QUP_MX_OUTPUT_COUNT,
            MX_OUTPUT_COUNT, 
            0);

         index++;

         HAL_QUPE_BAM_FILL_CE_BY_FIELD(pQupAddr, 
            halBamCmdDescInfo->cmd_element_base,
            index,         
            0,
            QUP_MX_INPUT_COUNT,
            MX_INPUT_COUNT,
            tmp_input_cnt);

         index++;
      }
           
      /* Save the index */
      halBamCmdDescInfo->cmd_element_index = index;
      halBamCmdDescInfo->cmd_size_in_bytes = 
      (halBamCmdDescInfo->cmd_element_index)*sizeof(HAL_qupe_BamCeType);
   }while(0);

   return result;
}


/**
 * This function shall perform command descriptor creation for 
 * given set of device parameters.
 * 
 * @param[in]  uPhysicalQupAddr     - Physical Qup Block address. 
 * @param[in]  uVirtualQupAddr       - Virtual Qup Block address. 
 * @param mHalDeviceParameters   - The pointer to the device parameters.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_Spi_SetDeviceParameters_CommandElement
(
   uint8         *uPhysicalQupAddr,
   uint8         *uVirtualQupAddr,
   HAL_QUPE_SPI_DeviceParametersType *mHalDeviceParameters,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
)
{
   int32 result = HAL_QUPE_RESULT_SUCCESS;
   uint32 data = 0;
   uint32 reg_data = 0;
   uint32 mask = 0;
   uint32 u32TempMask = 0;
   uint32 index = 0;
 
   do
   {
      if ((NULL == mHalDeviceParameters) || (NULL == halBamCmdDescInfo))
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }    

      if (NULL == halBamCmdDescInfo->cmd_element_base)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }
      index = halBamCmdDescInfo->cmd_element_index;


      reg_data = 0;
      mask = 0;   

      data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalDeviceParameters->eTransferMode,
                         HAL_QUPE_SPI_INPUT_FIRST_MODE);     
            

      HAL_QUPE_BAM_ADD_REG_INFO(SPI_CONFIG, INPUT_FIRST,data,reg_data,mask);
      
      data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalDeviceParameters->eSpiCoreMode,
                         HAL_QUPE_SPI_SLAVE);     

      HAL_QUPE_BAM_ADD_REG_INFO(SPI_CONFIG, SLAVE_OPERATION,data,reg_data,mask);     
         
      HAL_QUPE_BAM_FILL_CE_BY_REG(uPhysicalQupAddr,
         halBamCmdDescInfo->cmd_element_base,
         index,         
         0,
         SPI_CONFIG,reg_data,mask);             
      
      index++;
      reg_data = 0;
      mask = 0;   
      
      data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalDeviceParameters->eClockPolarity,
                         HAL_QUPE_SPI_CLK_IDLE_HIGH);  
     
      HAL_QUPE_BAM_ADD_REG_INFO(SPI_IO_CONTROL, CLK_IDLE_HIGH,data,reg_data,mask);

      data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalDeviceParameters->bClockAlwaysOn,1);
      
      HAL_QUPE_BAM_ADD_REG_INFO(SPI_IO_CONTROL, CLK_ALWAYS_ON,data,reg_data,mask);

      data = HAL_QUPE_SPI_BAM_CHECK_SET_DATA(mHalDeviceParameters->bChipSelectAlwaysOn,1);
      
      HAL_QUPE_BAM_ADD_REG_INFO(SPI_IO_CONTROL, MX_CS_MODE,data,reg_data,mask);

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
   
      data = QUPE_INF(uVirtualQupAddr, SPI_IO_CONTROL, CS_N_POLARITY);
      u32TempMask = 1 << mHalDeviceParameters->nChipSelectId;
      if(mHalDeviceParameters->eCSPolarity == HAL_QUPE_SPI_CS_ACTIVE_HIGH)
      {
         data |= u32TempMask;
      }
      else
      {
         data &= ~u32TempMask;
      }
      
      HAL_QUPE_BAM_ADD_REG_INFO(SPI_IO_CONTROL, CS_N_POLARITY,data,reg_data,mask);
      HAL_QUPE_BAM_ADD_REG_INFO(SPI_IO_CONTROL, CS_SELECT,
         mHalDeviceParameters->nChipSelectId,reg_data,mask);
      
         
      HAL_QUPE_BAM_FILL_CE_BY_REG(uPhysicalQupAddr,
         halBamCmdDescInfo->cmd_element_base,
         index,         
         0,
         SPI_IO_CONTROL,reg_data,mask);     

       index++;
   
      /* Configure the deassertion time (inter-byte delay) between transfers
       * to this device */
      if(mHalDeviceParameters->nDeassertionClockTicks > 0)
      {
         /* SPI_DEASSERT_WAIT waits the specified number of clock ticks plus one, 
          * so we need to subtract one from the desired number of clock ticks when 
          * writing to the register
          */
         data = HAL_QUPE_MAX(HAL_QUPE_SPI_DEVICE_PLATBAM_MAX_DEASSERT_CLK_TICKS,
            mHalDeviceParameters->nDeassertionClockTicks-1);
      }
      else
      {
         data = 0;
      }
   
      HAL_QUPE_BAM_FILL_CE_BY_FIELD(uPhysicalQupAddr, 
         halBamCmdDescInfo->cmd_element_base,
         index,         
         0,
         SPI_DEASSERT_WAIT,
         DEASSERT_WAIT, 
         data);

       index++;
       reg_data = 0;
       mask = 0;   

       /*Mask interrupts*/
       data = 1;
       HAL_QUPE_BAM_ADD_REG_INFO(QUP_OPERATIONAL_MASK,INPUT_SERVICE_MASK,
          data, reg_data, mask);
       HAL_QUPE_BAM_ADD_REG_INFO(QUP_OPERATIONAL_MASK,OUTPUT_SERVICE_MASK,
          data, reg_data, mask);
       HAL_QUPE_BAM_FILL_CE_BY_REG(
          uPhysicalQupAddr,
          halBamCmdDescInfo->cmd_element_base,
          index,         
          0,
          QUP_OPERATIONAL_MASK,
          reg_data,
          mask);             
       index++; 
       
      /* Save the index */
      halBamCmdDescInfo->cmd_element_index = index;
      halBamCmdDescInfo->cmd_size_in_bytes = 
      (halBamCmdDescInfo->cmd_element_index)*sizeof(HAL_qupe_BamCeType);
   }while(0);

   return result;
}

/**
 * This function shall perform command descriptor creation for 
 * setting Minicore state.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param eMiniCoreMode          - The minicore type to be set.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_SetCoreMode_CommandElement
(
   uint8         *pQupAddr,
   HAL_qupe_MiniCoreType eMiniCoreMode,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
)
{
   int32 result = HAL_QUPE_RESULT_SUCCESS;

   do
   {
      if (NULL == halBamCmdDescInfo)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }

      if (NULL == halBamCmdDescInfo->cmd_element_base)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }      

      /* Setup core mode */
      HAL_QUPE_BAM_FILL_CE_BY_FIELD(pQupAddr,
          halBamCmdDescInfo->cmd_element_base,
          halBamCmdDescInfo->cmd_element_index,                  
          0,
          QUP_CONFIG,
          MINI_CORE, 
          eMiniCoreMode);     

      halBamCmdDescInfo->cmd_element_index++;
      halBamCmdDescInfo->cmd_size_in_bytes = 
      (halBamCmdDescInfo->cmd_element_index)*sizeof(HAL_qupe_BamCeType);
   }while(0);

   return result;
}

/**
 * This function shall perform command descriptor creation for 
 * setting Minicore state.
 * 
 * @param[in]  pQupAddr Qup Block address.
 * @param ceQupeRunState         - The QUP core state to be set.
 * @param halBamCmdDescInfo      - The pointer to the command descriptor details.
 *
 */
int32
HAL_qupe_SetCoreState_CommandElement
(
   uint8         *pQupAddr,
   HAL_qupe_RunStateType ceQupeRunState,
   HAL_qupe_BamCommandDescType *halBamCmdDescInfo
)
{
   int32 result = HAL_QUPE_RESULT_SUCCESS;

   do
   {
      if (NULL == halBamCmdDescInfo)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }

      if (NULL == halBamCmdDescInfo->cmd_element_base)
      {
         result = HAL_QUPE_RESULT_INVALID_ARG;
         break;
      }      
      
      /* Setup core state */
      HAL_QUPE_BAM_FILL_CE_BY_FIELD(pQupAddr,
          halBamCmdDescInfo->cmd_element_base,
          halBamCmdDescInfo->cmd_element_index,                  
          0,
          QUP_STATE,
          STATE, 
          ceQupeRunState);     

      halBamCmdDescInfo->cmd_element_index++;
      halBamCmdDescInfo->cmd_size_in_bytes = 
      (halBamCmdDescInfo->cmd_element_index)*sizeof(HAL_qupe_BamCeType);
      
   }while(0);

   return result;
}
