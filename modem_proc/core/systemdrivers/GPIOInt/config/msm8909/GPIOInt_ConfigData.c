/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This file has the direct connect GPIO interrupt support for the 8960 target.

REFERENCES

        Copyright © 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/GPIOInt/config/msm8909/GPIOInt_ConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIGPIOInt.h"
#include "HALgpioint.h"
#include "GPIOInt.h"

/*
 * Modem interrupt config map
 */

GPIOIntConfigMapType interrupt_config_map[] =
{
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_0, 61, GPIOINTF_EXTERNAL_CONFIG}, /* Direct connect  0*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_1, 62, 0}, /* Direct connect  1 owned by audio Codec */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_2, 63, 0}, /* Direct connect  2 */
};
