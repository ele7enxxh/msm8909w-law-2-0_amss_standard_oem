/*===========================================================================
  Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  10/06/16   dpk     Fixed KW issue
  04/06/16   dpk     Created for 8909
  =============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "string.h"
#include "tzbsp_blsp.h"
#include "tzbsp_blsp_arch.h"
#include <HALhwio.h>
#include "tzbsp_xpu.h"
#include "tzbsp_errno.h"
#include "tzbsp_vmid_config.h"
#include "bam.h"
#include "bam_private.h"
#include "SMMUTranslation.h"
#include "msmhwiobase.h"
#include "ClockTZ.h"

#define SPI_UART_4_LINE_GPIO_COUNT    4
#define I2C_UART_2_LINE_GPIO_COUNT    2

/* TLMM Register Access */
#define TLMM_CSR_REG_BASE                  (TLMM_BASE + 0x00000000)
#define HWIO_TLMM_GPIO_CFGn_ADDR(n)        (TLMM_CSR_REG_BASE + 0x00000000 + 0x1000 * (n))

/* BLSP AHB Clocks */
#define TZBSP_BLSP1_AHB_CLOCK   "gcc_blsp1_ahb_clk"

/* BAM Pipe bit mask for Tx and Rx pipes */
#define TZBSP_BLSP_BAM_PIPE_MASK      0x3
#define TZBSP_BLSP_BAM_PIPES_PER_PERIPHERAL  2


extern int tzbsp_mpu_lock_area(uint32 mpu, uint32 index, uint32 start,
							   uint32 end, uint32 rvmids, uint32 wvmids);
extern int tzbsp_mpu_unlock_area(uint32 mpu, uint32 index);


static bam_handle blsp1_bamhandle = NULL;
//static bam_handle blsp2_bamhandle = NULL;
static ClockId Blsp1AhbClkId = NULL;
//static ClockId Blsp2AhbClkId = NULL;


/** 
 * @brief Validates GPIO count according to the protocol used and GPIO numbers.
 * 
 * @param[in] pTzBspPeripheralInfo   Peripheral information structure.
 *       
 * @return E_SUCCESS, if GPIO are proper. Else E_INVALID_ARG.
**/
static int tzbsp_blsp_validate_gpio(TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo)
{
   int32 res = E_INVALID_ARG;
   int i;
   
   
   /* Validate GPIO count */
   switch (pTzBspPeripheralInfo->protocol)
   {
      case PROTOCOL_SPI:
      case PROTOCOL_UART_4_LINE:
         if (pTzBspPeripheralInfo->uNumGpios == SPI_UART_4_LINE_GPIO_COUNT)
         {
            res = E_SUCCESS;
         }		 
         break;
      case PROTOCOL_I2C:
      case PROTOCOL_UART_2_LINE:
         if (pTzBspPeripheralInfo->uNumGpios == I2C_UART_2_LINE_GPIO_COUNT)
         {
            res = E_SUCCESS;
         }
         break;
      default:
         break;         
   };
   
   /* Validate GPIO numbers */
   if (res == E_SUCCESS)
   {
      for (i = 0;i < pTzBspPeripheralInfo->uNumGpios;i++)
      {
         if (pTzBspPeripheralInfo->uGpios[i] == 0xFFFFFFFF)
         {
            res = E_INVALID_ARG;
            TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_validate_gpio(): Invalid GPIO number for Peripheral %d\n", 
               pTzBspPeripheralInfo->peripheralId);
            break;
         }
      }
   }
   else
   {
      TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_validate_gpio(): Invalid GPIO count for Peripheral %d\n", 
         pTzBspPeripheralInfo->peripheralId);   
   }
   
   return res;
}

int tzbsp_blsp_set_clock(uint32 blsp, boolean flag)
{
   int res = E_FAILURE;
   ClockId *pClkId = NULL;
   char *pClkName = NULL;

 
   //TZBSP_BLSP_UNREFERENCED_PARAMETER(blsp);

   pClkId = &Blsp1AhbClkId;
   pClkName = TZBSP_BLSP1_AHB_CLOCK;

   if (pClkId != NULL)
   {
      if (*pClkId == NULL)
      {
         res = Clock_GetClockId(pClkName, pClkId);
      }

      if (*pClkId)
      {
         if (flag == TRUE)   /* Enable BLSP AHB Clock */
         {
            res = Clock_EnableClock(*pClkId);
         }
         else                /* Disable BLSP AHB Clock */
         {
            res = Clock_DisableClock(*pClkId);
         }
      }
   }

   return res;
}

int tzbsp_blsp_gpio_lock(TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo, uint32 vmidAccessBitMap)
{  
   int32 i = 0, j = 0,res = E_SUCCESS;
   HAL_xpu2_XPU2Type xpuTlmm = HAL_XPU2_TLMM;
   uint32 mpuIndex = 0;
   uint32 gpioStartAddr, gpioEndAddr;

   
   /* 
    * Configure GPIO VMID protection, only if, the QUP is used by TZ or 
    * by TZ along with other subsystem. Else return success;
	*/
   if ((pTzBspPeripheralInfo->uSubSystemId == TZBSP_NONE_ID) ||
       (pTzBspPeripheralInfo->bIsPeripheralTzUsed == FALSE))
   {
      return res;
   }  
   
   /* Validate GPIO count and GPIO numbers */
   res = tzbsp_blsp_validate_gpio(pTzBspPeripheralInfo);
   if (res != E_SUCCESS)
   {
      return res;
   }
   
   /* Configure GPIO MPU protection for Bear Targets */
   if(PROTECTION_TYPE_MPU == pTzBspPeripheralInfo->protectionType) 
   {
      /* Get GPIO Address ranges from GPIO numbers */
      while (i < pTzBspPeripheralInfo->uNumGpios)
      {
         gpioStartAddr = HWIO_TLMM_GPIO_CFGn_ADDR(pTzBspPeripheralInfo->uGpios[i]);
         for (j = i;j < (pTzBspPeripheralInfo->uNumGpios - 1);j++)
         {
            if ((pTzBspPeripheralInfo->uGpios[j + 1] - pTzBspPeripheralInfo->uGpios[j]) > 1)
            {
               break;
            }
         }
         gpioEndAddr = HWIO_TLMM_GPIO_CFGn_ADDR(pTzBspPeripheralInfo->uGpios[j] + 1);

         /* Configure GPIO MPU Protection */
         if (gpioStartAddr && gpioEndAddr)
         {
            /* Check for the max limit of mpuIndex, the index of the array uGpioIndex */
            if (mpuIndex >= TZBSP_BLSP_INDICES_SUPPORTED)
            {
               TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_gpio_lock(): Max GPIO MPU Index count reached: %d for QUP: %d \n", 
                  mpuIndex, pTzBspPeripheralInfo->peripheralId);
               res = E_INVALID_ARG;
               break;
            }
            /* Check whether valid index was given in the config file or not */
            if (pTzBspPeripheralInfo->uGpioIndex[mpuIndex] == 0xFFFFFFFF)
            {
               TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_gpio_lock(): Invalid TLMM MPU Index: %d for QUP: %d \n", 
                  pTzBspPeripheralInfo->uGpioIndex[mpuIndex], pTzBspPeripheralInfo->peripheralId);
               res = E_INVALID_ARG;
               break;
            }

            res = tzbsp_mpu_lock_area(xpuTlmm,
                                      pTzBspPeripheralInfo->uGpioIndex[mpuIndex],
                                      gpioStartAddr,
                                      gpioEndAddr,
                                      vmidAccessBitMap, vmidAccessBitMap);
            if (E_SUCCESS != res)
            {
               TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_gpio_lock(): GPIO Lock Failed. res: %08x \n", res);
               break;
            }
         }
         else
         {
            TZBSP_LOG(TZBSP_MSG_ERROR, "Invalid GPIO Addresses 0x%08x 0x%08x", gpioStartAddr, gpioEndAddr);
            res = E_INVALID_ARG;
            break;
         }
	 
	 i = j + 1;
	 
	 /* Fetch next MPU Index from config array*/
         mpuIndex++;
      }
   }		
   else 
   {
      TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_gpio_lock(): GPIO Protection type not supported \n");   
      res = -E_NOT_SUPPORTED;		   
   }		
   
   return res;
}

int tzbsp_blsp_bam_pipe_assign(uint32 blsp, uint32 coreId, uint32 bamEEId)
{
   bam_status_type res = BAM_SUCCESS;
   uint32 pipeMask = 0;
   bam_config_type bam_cfg;
   bam_callback_type bamCbDummy;
   bam_handle *pBamHandle = NULL;   


   //TZBSP_BLSP_UNREFERENCED_PARAMETER(blsp);
   
   /* Acquire BAM handle */
   bam_cfg.bam_va = 0;
   bam_cfg.bam_irq = 0;
   bam_cfg.sum_thresh = 0x2000;
   bam_cfg.bam_irq_mask = 0;
   bam_cfg.options = 0;

   do
   {
      if (bamEEId < TZBSP_EE_INVALID)
      {
         pBamHandle = &blsp1_bamhandle;
         bam_cfg.bam_pa = TZBSP_BLSP1_BAM_BASE_ADDR;
		 
		 if (NULL == *pBamHandle)
		 {
		    *pBamHandle = bam_init(&bam_cfg, &bamCbDummy);
		    if (NULL == *pBamHandle)
		    {
               TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_assign_bam_pipe(), bam_init failed");
		       res = BAM_FAILED;
		       break;			   
		    }
		 }
         
         pipeMask = TZBSP_BLSP_BAM_PIPE_MASK << (2 * coreId);
		 		 
         res = bam_pipe_reassign(*pBamHandle, bamEEId, pipeMask);
         if (BAM_SUCCESS != res)
         {
            TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_assign_bam_pipe(), bam_pipe_reassign failed. res: %08x", res);
            break;
         }      
      }
      else
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_assign_bam_pipe(), Invalid configuration provided");      
         res = BAM_INVALID_CONFIG;            
      }
   } while(0);
   
   return (int)res;
}

int tzbsp_blsp_qup_lock (TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo, uint32 vmidAccessBitMap, boolean lock)
{
   int res = E_SUCCESS;
   tzbsp_rpu_rg_t rg;
   uint32 xpu;
   

   rg.index = pTzBspPeripheralInfo->uPeripheralRgIndex;

   rg.flags = TZBSP_RWE | TZBSP_XPU_NON_SEC;
   if (lock)
   {
      rg.read_vmid =  vmidAccessBitMap;
      rg.write_vmid = vmidAccessBitMap;
   }
   else
   { 
      rg.read_vmid = ~(TZBSP_VMID_NOACCESS_BIT);
      rg.write_vmid = ~(TZBSP_VMID_NOACCESS_BIT);
   }

   xpu = pTzBspPeripheralInfo->uBlspXpuId;

   if (TZBSP_VMID_TZ_BIT == vmidAccessBitMap)
      res = tzbsp_rpu_reconfigure(xpu, &rg, TRUE); //Only TZ needs secure lock
   else
      res = tzbsp_rpu_reconfigure(xpu, &rg, FALSE);

   if (E_SUCCESS != res)
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_qup_lock(), res: %08x", res);
   }

   return res;
}

int tzbsp_blsp_smmu_config(uint32 blsp, uint32 coreId, boolean smmuConfig)
{
   SMMUResult_t res = SMMU_ERR_OK;
   uint32 pipeIndex = 0;
   int i;


   //TZBSP_BLSP_UNREFERENCED_PARAMETER(blsp);
   
   pipeIndex = BLSP1_PIPE0 + (2 * coreId);
   
   for (i = 0; i < TZBSP_BLSP_BAM_PIPES_PER_PERIPHERAL; i++)
   {   
      res =  SMMU_BLSPChangeSecureStatus((smmu_blsp_pipe_t)pipeIndex, smmuConfig);
      if (SMMU_ERR_OK != res)
      {
         TZBSP_LOG(TZBSP_MSG_ERROR,"tzbsp_blsp_smmu_config(): SMMU Config Failed for Pipe Index: %d. res: %08x \n", pipeIndex, res);
         break;
      }
      pipeIndex += 1;	  
   }
   
   return (int)res;
}



