/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2015 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cBsp.c#1 $
$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/25/15 VG     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cBsp.h"
#include "I2cPlatSvc.h"
#include "I2cDevice.h"
#include "I2cDriverTypes.h"
#include "I2cPlatConfig.h"
/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/


extern I2cDrv_DescType           i2cDrvDescArray[];
extern I2cPlat_PropertyType      i2cPlatPropertyArray[];
extern I2cDev_PropertyType       i2cDevPropertyArray[];
extern I2cDrv_DriverProperty     i2cDrvPropertyArray[];



int32
I2cDrv_Init
(
   void
);

int32
I2cDrv_InitDevice
(
   I2cDrv_I2cBusId  eI2cBusId,
   I2cDrv_DescType* pDesc
);

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/
//DEBUG:
//void i2c_test(void);
/** @brief Releases a device driver struct .
 
 @return             Pointer to dev if found, otherwise NULL.
 */
void
I2cBsp_ReleaseDriverObj
(
 I2cDrv_DescType* pDesc
)
{
   pDesc->opState = I2CDRV_OPSTATE_OBJ_FREE;
}


/** @brief Initialises all I2C driver devices for the platform .
 
 @return             Pointer to dev if found, otherwise NULL.
 */
void I2cBsp_Init(void)
{
   I2cDrv_DescType       *pDesc = NULL;
   I2cPlat_PropertyType  *pPlatProperty = NULL;
   I2cDev_PropertyType   *pDevProperty = NULL;
   I2cDrv_DriverProperty *pDrvProperty = NULL;
   int32                  drvPropertyIndex;
   uint32                 i2cDeviceNum;
   static boolean         bBspInitDone = FALSE;

   if (bBspInitDone) {
      return;     
   }

   I2cDrv_Init();
   I2CPLATDEV_ReadPlatConfig();
   i2cDeviceNum = i2cPlatPropertyArray[0].uNumdevices;

   for ( drvPropertyIndex=0; drvPropertyIndex < i2cDeviceNum; drvPropertyIndex++ ) {
      pDrvProperty = &i2cDrvPropertyArray[drvPropertyIndex];
      pPlatProperty = &i2cPlatPropertyArray[drvPropertyIndex];
      pDevProperty  = &i2cDevPropertyArray[drvPropertyIndex];
      pDesc         = &i2cDrvDescArray[drvPropertyIndex];
      pDesc->pDrvProps  = pDrvProperty;
      pDesc->pPlatProps = pPlatProperty;
      pDesc->pDevProps  = pDevProperty;
      pDesc->opState    = I2CDRV_OPSTATE_OBJ_FREE;
   
      I2cDrv_InitDevice(pDrvProperty->eDevId,pDesc);
   }
   bBspInitDone = TRUE;
}


