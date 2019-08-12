/*===========================================================================
  Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  10/06/16   dpk     Added support to store default owner.
                     Updated peripheral enum numbering.
  04/06/16   dpk     Created for 8909
  =============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "tzbsp_blsp.h"
#include "tzbsp_blsp_arch.h"
#include "tzbsp_errno.h"
#include "tzbsp_vmid_config.h"

#include "DALSys.h"
#include "DALStdErr.h"


/*-------------------------------------------------------------------------
 * Static Function Declarations
 * ----------------------------------------------------------------------*/
static void tzbsp_blsp_save_current_owner(uint32 peripheralId, uint32 currentOwner);
static uint32 tzbsp_blsp_get_current_owner(uint32 peripheralId);
static void tzbsp_blsp_save_default_owner(uint32 peripheralId, uint32 defaultOwner);

static TzBsp_Blsp_Peripheral_Info *tzbsp_blsp_get_peripheral_info(uint32 peripheralId);

extern uint32 TzBspBlspConfigArrLen;
extern TzBsp_Blsp_Peripheral_Info  TzBspBlspPeripheralInfo[];


static boolean blspBootConfDone = FALSE;
static uint32 *puCurrentOwner = NULL;
static uint32 *puDefaultOwner = NULL;


int tzbsp_blsp_modify_ownership(uint32 peripheralId, uint32 owner_ss)
{
   uint32 vmidAccessBitMap;
   uint32 bamEEId;
   uint32 blsp = 0;
   uint32 coreId = 0;
   int res = E_SUCCESS;
   int retVal = E_SUCCESS;
   TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo = NULL;
   boolean smmuConfig = FALSE;

   
   pTzBspPeripheralInfo = tzbsp_blsp_get_peripheral_info(peripheralId);
   if (NULL == pTzBspPeripheralInfo)
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): PERIPHERAL information not found for ID %d", peripheralId);
      if((FALSE == blspBootConfDone) && (TzBspBlspConfigArrLen))
      {
         /*In cold boot stage we are expected to ignore invalid configuration*/
         return E_SUCCESS;
      }
      else
      {
         return E_FAILURE;
      }
   }
   
   if ((PERSISTENT == pTzBspPeripheralInfo->isPersistent) && (blspBootConfDone == TRUE))
   {
      /* 
       * Even though Persistent is set to TRUE, this function will return success,
       * if the ownership is assigning to the same SS which was assigned to at the 
       * cold boot stage. Otherwise NOT Supported.
       */
      if ((subsystem_id_t)owner_ss == pTzBspPeripheralInfo->uSubSystemId)
      {
         return E_SUCCESS;
      }
      else
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Persistent flag set TRUE, modify Ownership not supported with PERIPHERAL Id %d", peripheralId);
         return -E_NOT_SUPPORTED;
      }
   }
   
   switch ((subsystem_id_t)owner_ss) 
   {
      case TZBSP_NONE_ID:
         bamEEId = TZBSP_EE_AP;
         vmidAccessBitMap = TZBSP_ALL_VMID_BMSK;         
         break;
      case TZBSP_APSS_ID:
         bamEEId = TZBSP_EE_AP;
         vmidAccessBitMap = TZBSP_VMID_AP_BIT;
         break;
      case TZBSP_ADSP_ID:
         bamEEId = TZBSP_EE_LPASS;
         vmidAccessBitMap = TZBSP_VMID_LPASS_BIT;
         break;
      case TZBSP_TZ_ID:
         smmuConfig = TRUE;
         bamEEId = TZBSP_EE_TZ;
         vmidAccessBitMap = TZBSP_VMID_TZ_BIT;         
         break;

      default:
         return -E_NOT_SUPPORTED;
         //break;
   }
   
   switch (pTzBspPeripheralInfo->protocol)
   {
      case PROTOCOL_SPI:
      case PROTOCOL_I2C:
         blsp = (peripheralId < TZBSP_QUPS_PER_BLSP) ? 1 : 2;
         /* Extract QUP ID per BLSP */
         coreId = (blsp == 1) ? (peripheralId): (peripheralId - TZBSP_QUPS_PER_BLSP);
		
         /* 
          * Calculate the peripheral ID in BLSP in the order of their 
          * BLSP BAM pipe arrangement. First comes UART starts with 0 followed 
          * by QUP. 
          */
         coreId += TZBSP_UARTS_PER_BLSP;
		 
         break;
      case PROTOCOL_UART_2_LINE:
      case PROTOCOL_UART_4_LINE:
         blsp = (peripheralId < (BLSP_UART_START + TZBSP_UARTS_PER_BLSP)) ? 1 : 2;
         /* 
          * Calculate the peripheral ID in BLSP in the order of their 
          * BLSP BAM pipe arrangement. First comes UART starts with 0 followed 
          * by QUP. 
          */
		 coreId = (blsp == 1) ? (peripheralId - BLSP_UART_START): (peripheralId - (BLSP_UART_START + TZBSP_UARTS_PER_BLSP));

         break;
      default:
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Invalid Protocol %d assigned for Peripheral %d", 
            pTzBspPeripheralInfo->protocol, peripheralId);
         return -E_NOT_SUPPORTED;
         //break;	  
   };
   
   res = tzbsp_blsp_set_clock(blsp, TRUE);
   if (E_SUCCESS != res)
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Enable AHB clock failed for PERIPHERAL ID %d, res: %08x", peripheralId, res);
      return res;
   }
         
   do
   {
      res = tzbsp_blsp_bam_pipe_assign(blsp, coreId, bamEEId);
      if (E_SUCCESS != res)
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Assign BAM Pipe owner ship with bamEEId:%d failed for PERIPHERAL ID %d, res: %08x", bamEEId, peripheralId, res);
         break;	  
      }
	    
      res = tzbsp_blsp_smmu_config(blsp, coreId, smmuConfig);
      if (E_SUCCESS != res)
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): SMMU Configuration failed for PERIPHERAL ID %d, res: %08x", peripheralId, res);
	     break;	  
      }	
	  
      res = tzbsp_blsp_qup_lock(pTzBspPeripheralInfo, vmidAccessBitMap, TRUE);
      if (E_SUCCESS != res)
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): QUP lock failed for PERIPHERAL ID %d, res: %08x", peripheralId, res);
         break;	  
      }
	  
      res = tzbsp_blsp_gpio_lock(pTzBspPeripheralInfo, vmidAccessBitMap);
      if (E_SUCCESS != res)
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Obtaining GPIO ownership failed for PERIPHERAL ID %d, res: %08x", peripheralId, res);
	     break;	  
      }

      tzbsp_blsp_save_current_owner(peripheralId, owner_ss);
      
   } while (0);
   
   retVal = tzbsp_blsp_set_clock(blsp, FALSE);
   if (E_SUCCESS != retVal)
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_modify_ownership(): Disable AHB clock failed for PERIPHERAL ID %d, retVal: %08x", peripheralId, retVal);         
   }
      
   if ((E_SUCCESS == res) && (E_SUCCESS == retVal))
   {
      TZBSP_LOG(TZBSP_MSG_DEBUG, "tzbsp_blsp_modify_ownership(): Ownership for PERIPHERAL ID %d, successfully assigned to Subsystem with ID %d", peripheralId, (int)owner_ss);
   }
   
   return ((res != E_SUCCESS) ? res : retVal);
}

int tzbsp_blsp_modify_ownership_ex(uint32 peripheralId, uint32 switchToOwner)
{
   int res = E_NOT_SUPPORTED;
   uint32 currentOwner;

   currentOwner = tzbsp_blsp_get_current_owner(peripheralId);

   /*
    * Ownership switch to/from secure world is not supported using scm_call2()
    * API from HLOS Kernel	
    */
   if ((currentOwner != TZBSP_TZ_ID) && (switchToOwner != TZBSP_TZ_ID))
   {
      res = tzbsp_blsp_modify_ownership(peripheralId, switchToOwner);
   }

   return res;
}

int tzbsp_blsp_init(void)
{
   int res = E_SUCCESS;
   uint32 peripheralId;
   uint32 subSystemId;
   int i;
   
   blspBootConfDone = FALSE;
   
   if(TzBspBlspConfigArrLen)
   {
      /* Allocat memory to store the current owner of a particular BLSP instance */
      if (DAL_SUCCESS != DALSYS_Malloc
          ((TZBSP_TOTAL_QUPS + TZBSP_TOTAL_UARTS) * sizeof(uint32), 
           (void **)&puCurrentOwner))
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_init(), Mem alloc failed for the array to store the curr owner");
         return E_FAILURE;
      }

      memset(puCurrentOwner, 
             0xFF, 
             (TZBSP_TOTAL_QUPS + TZBSP_TOTAL_UARTS) * sizeof(uint32));

      /* Allocat memory to store the Default owner of a particular BLSP instance */
      if (DAL_SUCCESS != DALSYS_Malloc
          ((TZBSP_TOTAL_QUPS + TZBSP_TOTAL_UARTS) * sizeof(uint32), 
           (void **)&puDefaultOwner))
      {
         TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_init(), Mem alloc failed for the array to store the Default owner");

         DALSYS_Free(puCurrentOwner);
         puCurrentOwner = NULL;

         return E_FAILURE;
      }

      memset(puDefaultOwner, 
             0xFF, 
             (TZBSP_TOTAL_QUPS + TZBSP_TOTAL_UARTS) * sizeof(uint32));

      /* Assign QUP instances to a subsystem given by user */
      for (i = 0; i < TzBspBlspConfigArrLen; i++)
      {
         peripheralId = 0;
         subSystemId = 0;
      
         peripheralId = TzBspBlspPeripheralInfo[i].peripheralId;
         subSystemId = TzBspBlspPeripheralInfo[i].uSubSystemId;
         res = tzbsp_blsp_modify_ownership(peripheralId, subSystemId);
         if (E_SUCCESS != res)
         {
            DALSYS_Free(puCurrentOwner);
            puCurrentOwner = NULL;

            DALSYS_Free(puDefaultOwner);
            puDefaultOwner = NULL;			

            TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_init(), Failed. res: %08x ", res);
            break;
         }

         tzbsp_blsp_save_default_owner(peripheralId, subSystemId);		 
      }
   
      blspBootConfDone = TRUE;
   }
   return res;   
}

/** 
 * @brief Get QUP information from the Qup ID
 * 
 * @param[in] peripheralId   PeripheralId id 
 *       
 * @return If success, returns the peripheral information structure. Otherwise NULL.
**/
static TzBsp_Blsp_Peripheral_Info *tzbsp_blsp_get_peripheral_info(uint32 peripheralId)
{
   uint32 i;
   
   
   if (((peripheralId > TZBSP_TOTAL_QUPS) && (peripheralId < BLSP_UART_START)) ||
       (peripheralId >= (BLSP_UART_START + TZBSP_TOTAL_UARTS)))
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_get_peripheral_info(), PERIPHERAL ID %d Not supported", peripheralId);
   }
   else
   {
      for (i = 0; i < TzBspBlspConfigArrLen; i++)
      {
         if (peripheralId == TzBspBlspPeripheralInfo[i].peripheralId)
         {
            return &TzBspBlspPeripheralInfo[i]; 
         }
      }
   }
   
   return NULL;      
}

/** @brief Saves current owner of the BLSP instance in the table.

    @param[in] peripheralId        BLSP Peripheral Id (QUP/UART).
    @param[in] currentOwner        Current Owner Subsystem ID.
    
    @return Void
*/
static void tzbsp_blsp_save_current_owner(uint32 peripheralId, uint32 currentOwner)
{
   uint32 peripheralIndex;

   peripheralIndex = (peripheralId < BLSP_UART_START) ? (peripheralId) : (TZBSP_TOTAL_QUPS + (peripheralId - BLSP_UART_START));

   puCurrentOwner[peripheralIndex] = currentOwner;

   return;
}

/** @brief Get current owner of the BLSP instance from the table.

    @param[in] peripheralId        BLSP Peripheral Id (QUP/UART).
    
    @return Current Owner Subsystem ID.
*/
static uint32 tzbsp_blsp_get_current_owner(uint32 peripheralId)
{
   uint32 peripheralIndex;

   peripheralIndex = (peripheralId < BLSP_UART_START) ? (peripheralId) : (TZBSP_TOTAL_QUPS + (peripheralId - BLSP_UART_START));

   return puCurrentOwner[peripheralIndex];
}

/** @brief Saves Default owner of the BLSP instance in the table.

    @param[in] peripheralId        BLSP Peripheral Id (QUP/UART).
    @param[in] DefaultOwner        Default Owner Subsystem ID.
    
    @return Void
*/
static void tzbsp_blsp_save_default_owner(uint32 peripheralId, uint32 defaultOwner)
{
   uint32 peripheralIndex;

   peripheralIndex = (peripheralId < BLSP_UART_START) ? (peripheralId) : (TZBSP_TOTAL_QUPS + (peripheralId - BLSP_UART_START));

   puDefaultOwner[peripheralIndex] = defaultOwner;

   return;
}

/** @brief Get Default owner of the BLSP instance from the table.

    @param[in] peripheralId        BLSP Peripheral Id (QUP/UART).
    
    @param[out] defaultOwner        Default owner..

    @return E_SUCCESS is the function is success. otherwise failure.
*/
int tzbsp_blsp_get_default_owner(uint32 peripheralId, uint32 *defaultOwner)
{
   uint32 peripheralIndex;

   if (((peripheralId > TZBSP_TOTAL_QUPS) && (peripheralId < BLSP_UART_START)) ||
       (peripheralId >= (BLSP_UART_START + TZBSP_TOTAL_UARTS)))
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_get_default_owner(), PERIPHERAL ID %d Not supported", peripheralId);
      return E_INVALID_ARG;
   }

   if (defaultOwner == NULL)
   {
      TZBSP_LOG(TZBSP_MSG_ERROR, "tzbsp_blsp_get_default_owner(), defaultOwner is a NULL Pointer");
      return E_INVALID_ARG;
   }

   peripheralIndex = (peripheralId < BLSP_UART_START) ? (peripheralId) : (TZBSP_TOTAL_QUPS + (peripheralId - BLSP_UART_START));
   *defaultOwner = puDefaultOwner[peripheralIndex];

   return E_SUCCESS;
}
