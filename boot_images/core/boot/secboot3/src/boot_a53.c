/*=============================================================================

                       Common Boot A53 Handler

GENERAL DESCRIPTION
  This file contains definitions of functions for A53 specific routines.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/03/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
06/17/14   ck      In Bear BLOCK_RESIN must be cleared before asserting PS_HOLD
05/21/14   lm      Added common macro name in boot_switch_to_aarch64
03/18/14   ck      Removed boot_hw_warm_reset as it is redundant
03/18/14   ck      boot_hw_reset now takes a parameter specifying what kind of reset to perform
02/05/14   ck      Added A53 logic to switch from 32 to 64 bit mode.
12/06/13   ck      Removed GCC_WDOG_DEBUG as this does not exist on A53
12/06/13   ck      Initial creation from boot_krait.c, preserving change log
07/31/13   dh      backed out pmic hard reset per windows request
07/24/13   aus     Added boot_hw_warm_reset
07/09/13   dh      Perform hard pmic reset in boot_hw_reset if sbl is in dload mode  
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg to properly configure reset and shutdown 
01/22/13   dh      Disable watchdog debug feature in boot_hw_reset
12/10/12   plc     Fix Klockwork Errors
10/25/12   dh      Remove pragma to surpass inline assemly since armct5.01 allows it
09/19/12   dh      enable ps hold reset
08/08/12   dh      use self loop in boot_hw_reset ps hold doesn't work
06/26/12   dh      use ps hold in boot_hw_reset
08/05/11   kpa     Relocated boot_hw_powerdown() here from boothw_target.c.
05/19/11   kpa     1. Remove inclusion of boot_scorpion_mmu.h, code common to
                   boot_page_table.c. 
                   2. Check the jtag debug disable fuse before output to jtag
                   terminal to avoid crash
5/11/11    plc     Change to include boot_page_table_armv7.h
01/25/11   dh      fixed l2 page table overflow issue
09/10/10   aus     Enabling data cache and MMU in boot
07/22/10   aus     Intial creation.
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

**  Function :  boot_switch_to_aarch64

** ==========================================================================
*/
/*!
* 
* @brief
*   This function configures and then switches the processor to 64 bit mode
*   (AARCH64).  
* 
* @par Parameters
*   entry_point = The address of where the processor should start executing
*   from after the switch to AARCH64
*
*   carried_parameter = Parameter that will be passed to entry point after
*   the switch to AARCH64.
*
* @retval
*   None
* 
* @par Side Effects
*   This function will never return.
* 
*/
void boot_switch_to_aarch64(uint32 entry_point,
                            uint32 carried_parameter)
{
  /* Configure processor starting address after warm reset
     using boot remapper. */

  /* Boot remapper only supports 64kb aligned addresses */
  BL_VERIFY((entry_point & ~(APCS_MODULE_SEC_START_ADDR_BMSK)) == 0,
            BL_ERR_MISALIGNMENT);

  /* Write entry point address and enable remap bit */
  APCS_MODULE_SEC_OUT(entry_point |
                      APCS_MODULE_SEC_REMAP_EN_BMSK);

  /* Enable A64 in RMR */
  boot_enable_aarch64();
  
  /* Generate warm reset, shouldn't return from here */
  boot_generate_warm_reset(carried_parameter);
  
} 


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
*   This function resets the A53 processor.  Parameter specifies what kind
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
* This function only works on ARM11 and Scorpion
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
