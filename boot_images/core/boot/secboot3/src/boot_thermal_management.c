/*=============================================================================

                        Boot Thermal Management

GENERAL DESCRIPTION
  This file does SBL thermal managememnt 

Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_thermal_management.c#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
04/04/13   dh        Call boot_hw_powerdown to power down
12/04/12   dh        Move boot_Tsens_Init to sbl1_hw_init
09/25/12   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boothw_target.h"
#include "boot_extern_busywait_interface.h"



/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef BOOT_TEMP_CHECK_THRESHOLD_DEGC

#include "boot_extern_tsensor_interface.h"

#ifndef MAX_TEMP_CHK_ITER
#define MAX_TEMP_CHK_ITER 40
#endif /*MAX_TEMP_CHK_ITER*/

#ifndef MAX_WAIT_TIME_MICROSEC
/* 500 ms wait time macro */
#define MAX_WAIT_TIME_MICROSEC 500000
#endif /*MAX_WAIT_TIME_MICROSEC*/


#endif /* BOOT_TEMP_CHECK_THRESHOLD_DEGC */


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_check_device_temp

** ==========================================================================
*/
/*!
* 
* @brief
*   This function does a temperature check. If the device is too hot or too cold
*   we wait few seconds to check again. If after a certain waiting period the temperature 
*   is still not valid we shutdown the device.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called after DALSYS_InitMod and busywait_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_check_device_temp( void )
{
#ifdef BOOT_TEMP_CHECK_THRESHOLD_DEGC  

  boot_boolean is_boot_temp_valid = FALSE;
  uint32 loop_count =0;
  BootTempCheckStatusType boot_temp = BOOT_TEMP_CHECK_TEMP_OK;
    
  while(loop_count < MAX_TEMP_CHK_ITER)
  {
    /* call BootTempCheck() ,to do a temperature check on the device.
    BootTempCheck() queries TSENS to get temp value and return
    BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD if too hot, BOOT_TEMP_CHECK_TEMP_BLOW_THRESHOLD
    if too cold.
    We wait max MAX_TEMP_CHK_ITER*MAX_WAIT_TIME_MICROSEC(10s using default values) seconds
    to allow device to go to acceptable levels or shut down the device for protection.
    */
    boot_temp = boot_BootTempCheck();  
    
    if((boot_temp != BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD) &&
       (boot_temp != BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD))
    {
      /* 
      Continue to boot: the device may not be calibrated, sensor time out
      or everything ok. In those cases do not delay boot.
      Value is either of below
      BOOT_TEMP_CHECK_TEMP_OK (0x0):           Temperature is in an acceptable 
                                               range.       
      BOOT_TEMP_CHECK_TSENS_TIMEOUT (0x1):     TSENS sensor timed out while 
                                               polling for a temperature ready
                                               status bit.
      BOOT_TEMP_CHECK_INVALID_TSENS_CAL (0x2): TSENS sensor does not have valid
                                               calibration data
      */
      
      is_boot_temp_valid = TRUE;
      break;
    }
    else
    {
      /* temperature is not in valid range. Wait before check again*/
      boot_busywait(MAX_WAIT_TIME_MICROSEC);
    }
    loop_count++;
  }

  /* If we still can't get a valid tempature here that means we have waited 
     MAX_TEMP_CHK_ITER*MAX_WAIT_TIME_MICROSEC time, shut down the device*/
  if(is_boot_temp_valid == FALSE)
  {
    boot_hw_powerdown();
  }
  
#endif /* BOOT_TEMP_CHECK_THRESHOLD_DEGC */
}




