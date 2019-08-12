#ifndef _I2CPLATCONFIG_H_
#define _I2CPLATCONFIG_H_
/*=============================================================================

  FILE:     I2cPlatConfig.h

  OVERVIEW: This file reads the *.xml config data 
 
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary 

  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatConfig.h#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  06/02/15 NP     Created.

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

extern DALDEVICEID DalI2C_DeviceId[];

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
 
#define I2C_NUM_PLATFORM_DEVICES         (8)

typedef enum I2CPLATSVC_Error
{
   I2CPLATDEV_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_PLATFORM,

   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE,
   I2CPLATSVC_ERROR_DAL_GET_CHIP_BUS_INDEX_PROPERTY_VALUE,

} I2CPLATSVC_Error;

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
);
#endif 