//============================================================================
/**
 * @file        qusb_dci_9x45_hw_apis_sbl1.c
 * @author      shreyasr
 * @date        19-June-2013
 *
 * @brief       QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) HW 
 *              interface for the MDM9x45 platform. Implementation for the SBL1 image.
 * 
 *              Copyright (c) 2013 QUALCOMM Technologies Incorporated.
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
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qusb/src/dci/qusb_dci_9x45_hw_apis_sbl1.c#1 $
// 
// when         who         what, where, why
// ----------   -----       ----------------------------------------------------------
// 2013-06-19   shreyasr     Porting the code for 9x45
// 2012-07-18   rthoorpu     Porting for 9x25
// 2012-05-24 	shreyasr	Porting the code for 8974
// 2011-07-23   dpatnaik     Re-factored clock ON/OFF and USB Reset routines.
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "busywait.h"  // busywait APIs
#include "qusb_hwio_dependency_9x45.h"
#include "qusb_log.h"
//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
#define QUSB_QTIMER_ACCESS_CONTROL      0x3F
#define QUSB_FRAME_NUMBER               0x00
#define QUSB_QTIMER_FREQUENCY_IN_HZ    19200000       //its 19.2MHz 

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------
//============================================================================

/**
* @function  qusb_dci_delay_ms
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
void qusb_dci_delay_ms(uint32 msecs)
{
  int counter;

  for (counter = 0; counter < msecs; counter++)
  {
    busywait( 1000 ); // 1 ms
  }   
}


//============================================================================
/**
* @function  qusb_dci_delay_us
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
void qusb_dci_delay_us(uint32 usecs)
{
  busywait( usecs ); 
}

//============================================================================
/**
* @function  qusb_timer_init
*
* @brief Internal function that is used to initialize the QTimer
*
* @Note : These timers are based on QTIMERS in APPS
*
* @param None
*
* @return None
*
*/
//============================================================================
static void qusb_timer_init(void)
{
  HWIO_QTMR_AC_CNTFRQ_OUT(QUSB_QTIMER_FREQUENCY_IN_HZ);
  HWIO_QTMR_AC_CNTACR_n_OUTI(QUSB_FRAME_NUMBER,QUSB_QTIMER_ACCESS_CONTROL);
  return; 
}
//============================================================================
/**
* @function  qusb_timer_start
*
* @brief Call the API to start the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param timer_value in uSec. 
*
* @return None
*
*/
//============================================================================
void qusb_timer_start(uint32 timer_value)
{
  qusb_log(QUSB_START_TIMER_LOG, 0, timer_value);
  //check if the timer is already running
  if(0 != HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_INM(HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK) ||
     (0x0 == (HWIO_APCS_F0_QTMR_V1_CNTP_CTL_INM(HWIO_APCS_F0_QTMR_V1_CNTP_CTL_ISTAT_BMSK) 
      >> HWIO_APCS_F0_QTMR_V1_CNTP_CTL_ISTAT_SHFT)))
  {
    qusb_log(QUSB_QTIMER_IS_ALREADY_RUNNING_LOG, 0x0, 0x0); 
    //loop till the timer value is not zero 
    while(0x0 !=HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_INM(HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK) ); 
    //Mask the interrupt 
    HWIO_APCS_F0_QTMR_V1_CNTP_CTL_OUTM(HWIO_APCS_F0_QTMR_V1_CNTP_CTL_IMSK_BMSK, (0x1<<HWIO_APCS_F0_QTMR_V1_CNTP_CTL_IMSK_SHFT)); 
  }
  //start the timer 
  qusb_timer_init();
  //set the timer value in the counter 
  HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_OUT(timer_value);
  //start the timer 
  HWIO_APCS_F0_QTMR_V1_CNTP_CTL_OUTM(HWIO_APCS_F0_QTMR_V1_CNTP_CTL_EN_BMSK,(0x1 << HWIO_APCS_F0_QTMR_V1_CNTP_CTL_EN_SHFT));
  return; 
}

//============================================================================
/**
* @function  qusb_timer_stop
*
* @brief Call the API to stop the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param  None 
*
* @return None
*
*/
//============================================================================
void qusb_timer_stop(void)
{
  qusb_log(QUSB_STOP_TIMER_LOG, 0, 0);
  //set the timer value in the counter 
  HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_OUT(0x0);
  //stop the timer 
  HWIO_APCS_F0_QTMR_V1_CNTP_CTL_OUTM(HWIO_APCS_F0_QTMR_V1_CNTP_CTL_EN_BMSK,(0x0 << HWIO_APCS_F0_QTMR_V1_CNTP_CTL_EN_SHFT)); 
}
//============================================================================
/**
* @function  qusb_timer_check_for_expiry
*
* @brief Call the API to stop the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param  None 
*
* @return boolean: TRUE if expired else FALSE
*
*/
//============================================================================
boolean qusb_timer_check_for_expiry(void)
{
  //check for timer expiry by reading the count value. If CNT was set to a non-zero
  // and now if it is zero, this means it has expired. No need to check the STATUS bit.
  if(0x0 == HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_INM(HWIO_APCS_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK))
  {
    //qtimer has expired
    //Mask the interrupt 
    HWIO_APCS_F0_QTMR_V1_CNTP_CTL_OUTM(HWIO_APCS_F0_QTMR_V1_CNTP_CTL_IMSK_BMSK, (0x1<<HWIO_APCS_F0_QTMR_V1_CNTP_CTL_IMSK_SHFT));
    qusb_log(QUSB_CHECK_QTIMER_FOR_EXPIRY_LOG, 0,TRUE);
    return TRUE;  
  }
  return FALSE; 
}

