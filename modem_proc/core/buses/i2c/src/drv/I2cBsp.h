#ifndef _I2CBSP_H_
#define _I2CBSP_H_
/*=============================================================================

  @file   I2cBsp.h

   This file contains the types for the platform services.
 
   Copyright (c) 2015 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cBsp.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cTypes.h"
#include "I2cDriverTypes.h"
/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

void I2cBsp_Init(void);
void I2cBsp_DeInit(void);


#endif /* _I2CBSP_H_ */

