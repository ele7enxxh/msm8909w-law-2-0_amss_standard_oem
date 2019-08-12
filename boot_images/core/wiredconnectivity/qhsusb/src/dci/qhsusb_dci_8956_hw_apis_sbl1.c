//============================================================================
/**
 * @file        qhsusb_dci_8956_hw_apis_sbl1.c
 * @author      shreyasr
 * @date        12-Sept-2010
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) HW 
 *              interface for the MSM8960 platform. Implementation for the SBL1 image.
 * 
 *              Copyright (c) 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_8956_hw_apis_sbl1.c#1 $
// 
// when         who         what, where, why
// ----------   -----       ----------------------------------------------------------
// 2014-11-05   bs          first draft for MSM8956
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "busywait.h"  // busywait APIs
#include "qhsusb_hwio_dependency_8956.h" 
#include "qhsusb_dci_8956_hw_apis.h"
#include "boot_extern_clk_interface.h"

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------
//============================================================================
/**
 * @function  qhsusb_dci_clk_is_usb_system_clk_on
 *
 * @brief Check if USB system clock is enabled
 *
 * @note
 *
 * @param 
 *
 * @return TRUE if clocks are on 
 *
 * @ref 
 *
 */
//============================================================================
boolean qhsusb_dci_clk_is_usb_system_clk_on(void)
{
  /* In all the use cases that involves SBL1, safe to assume that PBL 
   *   already initialized the clocks for us */
  /*Enable the clock for USB*/
  qhsusb_dci_clk_enable();

  return (TRUE);
}

//============================================================================
/**
 * @function  qhsusb_dci_clk_enable
 *
 * @brief Enable USB related clock
 *
 * @note
 *
 * @param Clock to be enabled   
 *
 * @return none
 *
 * @ref 
 *
 */
//============================================================================
void qhsusb_dci_clk_enable()
{
#ifndef FEATURE_RUMI_BOOT
	boot_clock_InitUSB();
#endif
}
//============================================================================
/**
 * @function  qhsusb_dci_clk_disable
 *
 * @brief Disable USB related clock
 *
 * @note
 *
 * @param Clock to be disabled   
 *
 * @return none
 *
 * @ref 
 *
 */
//============================================================================
void qhsusb_dci_clk_disable()
{
#ifndef FEATURE_RUMI_BOOT
	boot_clock_DisableUSB();
#endif  
}
//============================================================================

/**
* @function  qhsusb_dci_delay_ms
*
* @brief Perform delay in milliseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param milliseconds
*
* @return none
*
*/
//============================================================================
void qhsusb_dci_delay_ms(uint32 msecs)
{
  int counter;

  for (counter = 0; counter < msecs; counter++)
  {
    busywait( 1000 ); // 1 ms
  }   
}


//============================================================================
/**
* @function  qhsusb_dci_delay_us
*
* @brief Perform delay in microseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param microseconds
*
* @return none
*
*/
//============================================================================
void qhsusb_dci_delay_us(uint32 usecs)
{
  busywait( usecs ); 
}

//============================================================================
/**
 * @function  qhsusb_dci_dcache_flush
 *
 * @brief Flush Data-Cache
 *
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void qhsusb_dci_dcache_flush(uint32 *addr, uint32 length)
{
  return; 
}
//============================================================================
/**
 * @function  qhsusb_dci_dcache_inval
 *
 * @brief Invalidate Data-Cache
 *
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
 //============================================================================
void qhsusb_dci_dcache_inval(uint32 *addr, uint32 length)
{
  return; 
}
