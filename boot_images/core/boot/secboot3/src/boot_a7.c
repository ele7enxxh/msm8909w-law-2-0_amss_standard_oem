/*=============================================================================

                       Common Boot Sparrow Handler

GENERAL DESCRIPTION
  This file contains definitions of functions for A7 specific routines.

Copyright 2014 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_a7.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/05/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
01/08/15   jz      Disable SDI before reset
11/12/14   jz      Renamed to boot_a7.c and updated for A7 reference
06/17/14   ck      In Bear BLOCK_RESIN must be cleared before asserting PS_HOLD
04/22/14   jz      Updated/cleaned up boot_hw_reset
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg to properly configure reset and shutdown 
09/20/12   kedara  Use ps hold in boot_hw_reset
06/05/12   kedara  Intial creation.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_msm.h"
#include "boot_target.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_dload.h"
#include "boothw_target.h"
#include "boot_fastcall_tz.h"

/**
 * Infinite loop
 */
#define INFINITE_LOOP() while(1)

/**
 * Max number of retries before giving up to output to jtag terminal
 */
#define JTAG_TERM_MAX_NUM_RETRY  5

/**
 * Time out value for outputing a single char to jtag terminal
 */
#define JTAG_TERM_TIME_OUT       0x10000

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_clk_busy_wait

** ==========================================================================
*/
/*!
* 
* @brief
*   This function implements the clock busy wait.  
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_clk_busy_wait(uint32 delay)
{
  boot_busywait(delay);
} /* boot_clk_busy_wait */

/*===========================================================================

**  Function :  boot_hw_reset

** ==========================================================================
*/
/*!
* 
* @brief
*   This function resets the A7 processor.  Parameter specifies what kind
*   of reset to perform.
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is reset
* 
*/
void boot_hw_reset(boot_hw_reset_type reset_type)
{
  /* Clear TCSR_RESET_DEBUG before reset */
  if (boot_get_qsee_execute_status())
  {
    boot_fastcall_tz_2arg (TZ_OFFLINE_MEM_DUMP_CMD, 0x1, 0x0);
  } 
  else
  {
    HWIO_TCSR_RESET_DEBUG_SW_ENTRY_OUT(0);
  }
  
  /* Set the PMIC to do the requested type of reset when PS_HOLD is asserted */
  switch (reset_type)
  {
  case BOOT_WARM_RESET_TYPE:
    /* Configure PS_HOLD to do warm reset */
    boot_pm_pon_ps_hold_cfg(PMAPP_PS_HOLD_CFG_WARM_RESET);
    break;
  
  case BOOT_HARD_RESET_TYPE:
  default:
    /* Configure PS_HOLD to do hard reset.  This is also the default type.*/
    boot_pm_pon_ps_hold_cfg(PMAPP_PS_HOLD_CFG_HARD_RESET);
    break;
  }
   
  /* For Bear family BLOCK_RESIN must be cleared before asserting PS_HOLD */
  HWIO_GCC_RESET_DEBUG_OUTM(HWIO_GCC_RESET_DEBUG_BLOCK_RESIN_BMSK,
                            0);

  /* pull down pmic PS_HOLD */
  HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 );
  
  /* Loop forever */
  INFINITE_LOOP();
} /* boot_hw_reset */

/*===========================================================================

**  Function :  boot_hw_powerdown

** ==========================================================================
*/
/*!
* 
* @brief
*   This function powers off the PMIC
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   Target is switched off
* 
*/
void boot_hw_powerdown
(
  void
)
{
  /* Configure PS_HOLD to shutdown */
  BL_VERIFY(boot_pm_pon_ps_hold_cfg(PMAPP_PS_HOLD_CFG_NORMAL_SHUTDOWN) == PM_ERR_FLAG__SUCCESS, BL_ERR_SBL); 
  
  /* For Bear family BLOCK_RESIN must be cleared before asserting PS_HOLD */
  HWIO_GCC_RESET_DEBUG_OUTM(HWIO_GCC_RESET_DEBUG_BLOCK_RESIN_BMSK,
                            0);

  /* pull down pmic PS_HOLD */
  HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 );

  /* Loop forever */
  INFINITE_LOOP();
} /* boot_hw_powerdown */

/*===========================================================================

**  Function :  boot_jtag_term_putc

** ==========================================================================
*/
/*!
* 
* @brief
* Pushes a single character to the JTAG terminal window.
* It checks to make sure the previous character has been read by the terminal.
* If no JTAG terminal is open then it will skip any remaining output.  
* If it timeouts several times then it will never attempt
* to write out to JTAG again.
* This function only works on ARM11, Scorpion, A7
*
* @param[in] character single character to display.
*
* @par Dependencies
*   None
*   
* @retval
*   None
*
* @par Side Effects
*   None
* 
*/
void boot_jtag_term_putc(char character) 
{
  uint32 write_status;
  uint32 flag;
  uint32 timeout_count;

  /*Used to determine if writing to JTAG should even be attempted.  If 
    writing to the terminal fails JTAG_TERM_MAX_NUM_RETRY times,
    then it won't attempt anymore.*/
  static uint32 timeout_final = JTAG_TERM_MAX_NUM_RETRY;
  
  /*This section of assembly will output the given character to a JTAG 
    terminal If the terminal is not up then it will skip outputting.  If it
    misses writing to the terminal twice, then it will never attempt to write 
    to the terminal again.                                                 */
  /* First we check to see if any apps processor jtag debug disable fuse is blown, 
    if there is then we skip the jtag output
    When debug is disabled, accessing jtag register will likely result a crash*/
  if(!(DEBUG_DISABLED_APPS))
  {
    if(0 != timeout_final)
    {
      __asm 
      {
        mov  flag, #0x20000000                    /*The writeable flag is bit 29*/
        mov  timeout_count, #JTAG_TERM_TIME_OUT	   /*Used to watch for timeout*/
        loop:
          cmp     timeout_count, #0               
          beq     timeout                         /*Timed out, skip this output*/
          sub     timeout_count, timeout_count, #1   /*Decrement timeout_count*/
          mrc     p14, 0, write_status, c0, c1, 0       /*Grab writeable flag*/
          ands    write_status, write_status, flag   /*Check if writeable*/
          bne     loop                           /*Loop if not writeable*/
          mcr     p14, 0, character, c0, c5, 0      /*write to terminal*/
        b end
        timeout:
          sub timeout_final, timeout_final, #1   /*record timeout*/
        end:
      }
    }
  }
}
