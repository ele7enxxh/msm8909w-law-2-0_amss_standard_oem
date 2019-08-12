/**********************************************************************
 * HALsdhci_stub.c
 *
 * Empty functions for targets that do not support SDHCi.  
 * Hardware Abstraction Laye(HAL)
 *
 * Copyright (c) 2013-2014
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header:
$DateTime:

when        who     what, where, why
----------  ----    ---------------------------------------------------- 
2014-05-16  sb      Added SDHCi's PowerSave
2013-09-03  bn      Initial release

======================================================================*/

#include "HALsdhci.h"

/**
* HAL_sdcc_HCModeEnable
*
* Description:
*    Stub function for targets that do not support SDHCi 
*
*/
void HAL_sdcc_HCModeEnable (const uint32 nControllerId, enum HAL_sdhci_mode mode)
{
   return;
}

/**
* HAL_sdhci_PowerSave
*
* Description:
*    Enable or disable SCLK when bus is idle to save power.
*
*/
void HAL_sdhci_PowerSave (const uint32 nControllerId, boolean bEnable)
{
  return;
}


