/*=============================================================================

  FILE:     I2cPlatConfig.c

  OVERVIEW: This file reads the *.xml config data 
 
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary 

  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatConfig.c#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  12/22/15 VG     Added fix to correct  i2c bus clock
  10/13/15 VG     Added changes to support 8 QUP Instance
  08/25/15 VG     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "I2cDriverTypes.h"
#include "I2cPlatSvc.h"
#include "I2cDevice.h"
#include "I2cPlatConfig.h"
#include "I2cDriverDalFwk.h"
#include "DALDeviceId.h"

/*-------------------------------------------------------------------------
 * Global Data Definitions
 * ----------------------------------------------------------------------*/
extern I2cPlat_PropertyType i2cPlatPropertyArray[];
extern I2cDev_PropertyType i2cDevPropertyArray[];
extern I2cDrv_DriverProperty i2cDrvPropertyArray[];
extern I2cDrv_DescType i2cDrvDescArray[];

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/
/** @brief Initializes bam.
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReadPlatConfig
(
   void
)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   
   int32             deviceCnt,max_devices=1;
   int32             result;
   const char        *i2cDevices[8] = {"I2CDRV_I2C_1", "I2CDRV_I2C_2", "I2CDRV_I2C_3","I2CDRV_I2C_4", 
   	                             "I2CDRV_I2C_5", "I2CDRV_I2C_6", "I2CDRV_I2C_7","I2CDRV_I2C_8" };
								 
   for ( deviceCnt  = 0; deviceCnt < max_devices; ++deviceCnt){
      
      if ( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(i2cDevices[deviceCnt], hProp ) ) {
         return (int32)I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE;
      }

      result = I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE; /* set the error. */
	  
      if ( DAL_SUCCESS !=
           DALSYS_GetPropertyValue(hProp, "NUM_DEVICES", 0, &PropVar) ) {
		   break;
		   }
		   
		   i2cPlatPropertyArray[deviceCnt].uNumdevices = (uint32)PropVar.Val.dwVal;
		   max_devices = i2cPlatPropertyArray[deviceCnt].uNumdevices ;
           
      do {
         i2cDrvPropertyArray[deviceCnt].eDevId = (I2cDrv_I2cBusId)deviceCnt;
         
         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "SW_USE_POLLING_MODE", 0, &PropVar) ) {
            break;
         }
         i2cDrvPropertyArray[deviceCnt].bInterruptBased = ((uint32)PropVar.Val.dwVal == 1)? 0: 1;
     
         if ( i2cDrvPropertyArray[deviceCnt].bInterruptBased ) {
            if ( DAL_SUCCESS !=
                  DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_ID", 0, &PropVar)) {
               break;
            }
            i2cDrvPropertyArray[deviceCnt].uInterruptId = (uint32)PropVar.Val.dwVal;
         }

         i2cDevPropertyArray[deviceCnt].virtBlockAddr = NULL;
   
   

         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "GPIO_I2C_SDA", 0, &PropVar )) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].aGpioCfg[0] = (uint32)PropVar.Val.dwVal;
         
         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "GPIO_I2C_SCL", 0, &PropVar )) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].aGpioCfg[1] = (uint32)PropVar.Val.dwVal;
         
         i2cPlatPropertyArray[deviceCnt].uNumGpios = 2;
         
         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_PERIPH_SS_BASE_NAME", 0, &PropVar )) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].pPeriphSsAddrName = (char*)PropVar.Val.dwVal;
         
         if ( DAL_SUCCESS != 
               DALSYS_GetPropertyValue(hProp,"CHIP_BLOCK_OFFSET", 0, &PropVar ) ) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].coreOffset = PropVar.Val.dwVal;
         
         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_APP_CLK_NAME", 0, &PropVar)) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].pQupAppClkName = PropVar.Val.pszVal;

         if ( DAL_SUCCESS !=
                  DALSYS_GetPropertyValue(hProp, "CHIP_APP_CLK_FREQ_KHZ", 0, &PropVar)) {
            break;     
         }
         i2cPlatPropertyArray[deviceCnt].uAppClkFreqKhz = (uint32)PropVar.Val.dwVal;
		 
		 i2cDevPropertyArray[deviceCnt].uI2cInputClkKhz = (uint32)PropVar.Val.dwVal;

         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_HCLK_NAME", 0, &PropVar )) {
            break;
         }
         i2cPlatPropertyArray[deviceCnt].pQupHClkName = PropVar.Val.pszVal;
         if ( DAL_SUCCESS ==
           DALSYS_GetPropertyValue(hProp, "I2C_PNOC_VOTE_ENABLE", 0, &PropVar) ) {
			   
		   i2cPlatPropertyArray[deviceCnt].bPnocEnable = (uint32)PropVar.Val.dwVal;
           
		   if(i2cPlatPropertyArray[deviceCnt].bPnocEnable)
			{
				if ( DAL_SUCCESS !=
					  DALSYS_GetPropertyValue(hProp, "I2C_PNOC_IB_VAL", 0, &PropVar) ){
				   break;
				}	
				i2cPlatPropertyArray[deviceCnt].uPnocIBVal = (uint64)PropVar.Val.dwVal;
			  
				if ( DAL_SUCCESS !=
				   DALSYS_GetPropertyValue(hProp, "I2C_PNOC_AB_VAL", 0, &PropVar) ){
				   break;
				}
				i2cPlatPropertyArray[deviceCnt].uPnocABVal = (uint64)PropVar.Val.dwVal;

				if ( DAL_SUCCESS !=
					  DALSYS_GetPropertyValue(hProp, "I2C_PNOC_ARB_MASTER", 0, &PropVar)){
				   break;
				}
				i2cPlatPropertyArray[deviceCnt].uPnocMaster = (uint32)PropVar.Val.dwVal;
			  
				if ( DAL_SUCCESS !=
					 DALSYS_GetPropertyValue(hProp, "I2C_PNOC_ARB_SLAVE", 0, &PropVar) )
			   {
				break;
				}
				i2cPlatPropertyArray[deviceCnt].uPnocSlave = (uint32)PropVar.Val.dwVal;
			}
		}
         result = I2C_RES_SUCCESS;
      } while ( 0 );
      if (I2C_RES_SUCCESS != result){
         break;
      }
   }
   return result;
}
