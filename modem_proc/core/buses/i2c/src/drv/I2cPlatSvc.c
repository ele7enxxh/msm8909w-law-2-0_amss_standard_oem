/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatSvc.c#1 $
$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
 03/15/16 VG     Moving CLkAttach/detach to InitClock
 08/25/15 VG     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"
//#include "I2cSys.h"
#include "I2cError.h"

#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "DDIHWIO.h"
#include "I2cLog.h"

#include "DALDeviceId.h"
#include "I2cPlatSvcTypes.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUP_FREQ_HZ                            19200000

#define I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define I2C_DEVICE_PLATBAM_MAX_DESC_SIZE     0x100


/** @brief Initializes the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
 int32
I2CPLATDEV_InitClocks
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   I2cPlat_PropertyType   *pProps;
   uint32                 uSetClkFreq;

   pProps = &pPlat->props;

   /* Fails to attach to clocks, they are not available */
   dalRes = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &pProps->pClkHandle);
   if ( (DAL_SUCCESS != dalRes) || (NULL == pProps->pClkHandle) ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_CLOCK;
   }
   
   if ( DAL_SUCCESS !=
        DalClock_GetClockId(pProps->pClkHandle,
                            pProps->pQupAppClkName,
                            &pProps->QupAppClkId) ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_CLOCK;
   }

   dalRes = DalClock_SetClockFrequency(pProps->pClkHandle,
                                       pProps->QupAppClkId,
                                       pProps->uAppClkFreqKhz,
                                       CLOCK_FREQUENCY_KHZ_EXACT,
                                       &uSetClkFreq);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_CLOCK;     
   }    
   
   if ( DAL_SUCCESS != 
        DalClock_GetClockId(pProps->pClkHandle,
                            pProps->pQupHClkName,
                            &pProps->QupHClkId) ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_CLOCK;
   }
   
   pPlat->initState |= I2CPLAT_TGT_INIT_CLK_ALLOCATED ;
   return I2C_RES_SUCCESS; 
}

/** @brief Deinitializes the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_DeInitClocks
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   
   I2cPlat_PropertyType   *pProps;
   
   pProps = &pPlat->props;

   if ( pPlat->initState & I2CPLAT_TGT_INIT_CLK_ALLOCATED ) {
      dalRes = DAL_DeviceDetach(pProps->pClkHandle);
	  if ( DAL_SUCCESS != dalRes ) {
	     return I2CPLAT_ERROR_FAILED_TO_DISABLE_CLOCK;
	  }
   }
   
   pPlat->initState &= ~I2CPLAT_TGT_INIT_CLK_ALLOCATED;
   pProps->pClkHandle = NULL;
   return I2C_RES_SUCCESS; 
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Enables the I2C clocks.
   
    @param[in] pPlat platform device pointer.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32 I2cPlat_VoteI2cClkOn
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   I2cPlat_PropertyType   *pProps;
   //uint32                 uSetClkFreq;

   pProps = &pPlat->props;
   
   // Enable QUP Clock
   dalRes = DalClock_EnableClock(pProps->pClkHandle, pProps->QupAppClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_APPS_CLK;
   }

   // Enable HCLK
   dalRes = DalClock_EnableClock(pProps->pClkHandle, pProps->QupHClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_ENABLE_AHB_CLK;
   }
   
   return I2C_RES_SUCCESS;    
}

/** @brief Disables the I2C clocks.
  
    @param[in] pPlat platform device pointer.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32 I2cPlat_VoteI2cClkOff
(
   I2cPlat_DescType      *pPlat
)
{
   DALResult dalRes;
   I2cPlat_PropertyType   *pProps;

   pProps = &pPlat->props;

   // Disable QUP Clock
   dalRes = DalClock_DisableClock(pProps->pClkHandle, pProps->QupAppClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_DISABLE_APPS_CLK; 
   }

   // Disable HCLK
   dalRes = DalClock_DisableClock(pProps->pClkHandle, pProps->QupHClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_DISABLE_AHB_CLK;
   }
   
   return I2C_RES_SUCCESS;
}
