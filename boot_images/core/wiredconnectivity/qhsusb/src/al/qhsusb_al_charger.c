/*=======================================================================*//**
 * @file        qhsusb_al_charger.c
 * @author:     shreyasr
 * @date        11-Mar-2013
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              PMIC charger
 *
 * @note        
 *
 *              Copyright 2013 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/al/qhsusb_al_charger.c#1 $ 
// $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
// 
// when         who        what, where, why
// --------   ---        ----------------------------------------------------------
// 03/11/13   shreyasr    Initial Revision created
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"
#include "qhsusb_fd_chg.h"
#include "qhsusb_dcd.h"
#include "qhsusb_al_charger.h"
#include "qhsusb_log.h"
#include "pm_chg_common.h"
//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------
#define QHSUSB_ENUMERATION_POLL_INTERVAL_MS  3
#define QHSUSB_ENUMERATION_RETRY_COUNT       10000
#define QHSUSB_ENUMERATION_COMPLETION_COUNT  1000


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static struct qhsusb_dcd_dsc_device* dev = NULL;

/*============================================================================= 
 * @function    qhsusb_al_chg_get_charger_port_type
 * 
 * @brief       Returns the charger Port type to the User. 
 *
 * @details
 *              Performs the charger detection and returns the type of
 *              USB Charger which is connected.  
 *
 * @param   	None
 * 
 * @return  	qhsusb_charger_port_type the charger port type 
 * 
===========================================================================*/
qhsusb_charger_port_type qhsusb_al_chg_get_charger_port_type(void)
{
	qhsusb_charger_port_type charger_port_type = QHSUSB_CHARGER_PORT_UNKNOWN;
	charger_port_type = (qhsusb_charger_port_type) qhsusb_fd_chg_detect_charger_type();		
	qhsusb_log(charger_port_type,QHSUSB_CHARGER_TYPE_OFFSET,0);
	return charger_port_type;	
}
/*============================================================================= 
 * @function    qhsusb_al_chg_enumerate_usb
 * 
 * @brief       Performs Enumeration if an SDP is connected to the target 
 *
 * @details
 *              It Polls for USB events and performs enumeration. 
 *
 * @param   	None
 * 
 * @return  	qhsusb_enumeration_result The enumeration result.  
 * 
===========================================================================*/
qhsusb_enumeration_result qhsusb_al_chg_enumerate_usb(void)
{
  qhsusb_boot_poll_result evt = HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
  uint32 enum_poll_count = 0;
  uint32 enum_completion_count = 0;
    boolean is_vbus_removed = FALSE; 

  qhsusb_log(QHSUSB_AL_CHG_ENUMERATE_USB_LOG,0,0);
   /* Poll USB stack until we see enumeration events or we timeout */
  enum_poll_count = 0;
  do
  {
      evt = qhsusb_al_chg_poll();
      enum_poll_count++;
      if(evt == HSU_BOOT_POLL_RESULT_ENUMERATED)
        {
          break;
        }
        if (!qhsusb_al_chg_is_vbus_present())
        {
            is_vbus_removed = TRUE; 
          break;
        }
      qhsusb_dci_delay_ms(QHSUSB_ENUMERATION_POLL_INTERVAL_MS);
  }
  while( enum_poll_count < QHSUSB_ENUMERATION_RETRY_COUNT );

  /* Poll some more to allow for enumeration to successfully complete.
     Needed so that the host receives all the descriptors from the device. */
  enum_completion_count = 0;
  do
  {
        /** Don't poll if we are already disconnected **/ 
        if(is_vbus_removed == TRUE)
        {
          break;
        }
        else if(!qhsusb_al_chg_is_vbus_present())
        {
            is_vbus_removed = TRUE; 
            break;
        }        
      evt = qhsusb_al_chg_poll();
      enum_completion_count++;

      qhsusb_dci_delay_ms(QHSUSB_ENUMERATION_POLL_INTERVAL_MS);
  }
  while( enum_completion_count < QHSUSB_ENUMERATION_COMPLETION_COUNT );
    
  /* If enumeration failed then enter trickle charging mode */
  if((enum_poll_count >= QHSUSB_ENUMERATION_RETRY_COUNT) && (FALSE == is_vbus_removed))
  {
        qhsusb_log(QHSUSB_AL_CHG_ENUMERATE_USB_FAILURE_LOG,0,0);
        return QHSUSB_ENUMERATION_FAILURE;
  }
  else if (is_vbus_removed == TRUE)
  {
      return QHSUSB_DEVICE_DISCONNECTED; 
  }

  qhsusb_log(QHSUSB_AL_CHG_ENUMERATE_USB_SUCCESS_LOG,0,0);
  return QHSUSB_ENUMERATION_SUCCESS;

}
/*============================================================================= 
 * @function    qhsusb_al_chg_poll
 * 
 * @brief       Polls for USB events like RESET, SUSPEND etc.  
 *
 * @details
 *              It Polls for USB events. 
 *
 * @param   	None
 * 
 * @return  	qhsusb_boot_poll_result The polling result.  
 * 
===========================================================================*/
qhsusb_boot_poll_result qhsusb_al_chg_poll(void)
{
	static uint32 a0   = 0; /**< old for attached */
   static uint32 c0   = 0; /**< old for configured */
   static uint32 s0   = 0; /**< old for suspended */
   uint32 a,c,s;
   qhsusb_boot_poll_result evt = HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
      
   qhsusb_dcd_poll(dev);
   a = (uint32)(dev->is_attached);
   c = (0 != dev->config_value);
   s = (uint32)(dev->is_suspended);
   if ( ( a ^ a0 ) | ( c ^ c0 ) | ( s ^ s0 ) ) { /* if anything changed */
      qhsusb_dcd_log_event(dev,evt_portsc,(a0<<2)|(c0<<1)|s0,(a<<2)|(c<<1)|s);
      if ( 0 == a ) { /* final state is detached */
         /* TRANSITION: acs = xxx -> 0xx */
         evt = HSU_BOOT_POLL_RESULT_DISCONNECTED;
      } else {        /* final state is attached */
         if ( 0 != s ) { /* final state is suspended */
            /* TRANSITION: acs = xxx -> 1x1 */
            evt = HSU_BOOT_POLL_RESULT_SUSPENDED;
         } else {        /* final state is not suspended */
            if ( 0 != c ) { /* final state is configured */
               /* TRANSITION: acs = xxx -> 110 */
               if ( dev->max_current < 500 ) {
                  evt = HSU_BOOT_POLL_RESULT_ENUMERATED_100MA;
               } else {
                  evt = HSU_BOOT_POLL_RESULT_ENUMERATED;
               }
            } else {        /* final state is unconfigured */
               /* TRANSITION: acs = xxx -> 100 */
               if ( 0 == a0 ) {          /* first time? */
                  /* TRANSITION: acs = 0xx -> 100 */
                  evt = HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC;
               } else {
                  /* TRANSITION: acs = 1xx -> 100 */
                  evt = HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED;
               }
            }
         }
      }
      a0 = a;
      c0 = c;
      s0 = s;
	  if(evt != HSU_BOOT_POLL_RESULT_NO_NEW_EVENT)
	  {
		qhsusb_log(evt,QHSUSB_ENUM_POLL_OFFSET,0);
	  }
   }
   return evt;
}
/*============================================================================= 
 * @function    qhsusb_al_chg_init
 * 
 * @brief       This API initializes the charging stack 
 *
 * @details
 *              Initializes the charging stack. 
 *
 * @param   	None
 * 
 * @return  	TRUE if success else FALSE  
 * 
===========================================================================*/
boolean qhsusb_al_chg_init(void)
{
	dev= qhsusb_fd_chg_init();
	if(NULL == dev)
	{
		qhsusb_log(QHSUSB_AL_CHG_INIT_LOG,0,0);
		return FALSE;
	}
	qhsusb_log(QHSUSB_AL_CHG_INIT_LOG,0,1);
	return TRUE;
}
/*============================================================================= 
 * @function    qhsusb_al_chg_deinit
 * 
 * @brief       This API deconfigures the USB charging stack 
 *
 * @details
 *              De-configures the USB charging stack
 *
 * @param   	None
 * 
 * @return  	None  
 * 
===========================================================================*/
void qhsusb_al_chg_deinit(void)
{
	qhsusb_log(QHSUSB_AL_CHG_DEINIT_LOG,0,0);
	qhsusb_fd_chg_shutdown();
	return;
}
/*============================================================================= 
 * @function    qhsusb_al_chg_disconnect_from_host
 * 
 * @brief       This API is used to perform a disconnect from the host. 
 *
 * @details
 *              Disconnects the device from the Host.
 *
 * @param   	None
 * 
 * @return  	None
 * 
===========================================================================*/
void qhsusb_al_chg_disconnect_from_host(void)
{
    qhsusb_log(QHSUSB_AL_CHG_DISCONNECT_FROM_HOST_LOG,0,0);
    qhsusb_dci_disconnect(0);
    return;  
}
/*===========================================================================
 *
 * @function    qhsusb_al_chg_should_chg_be_on
 * 
 * @brief       This function tells whether charging should be left ON/OFF before
 *              HLOS boot.
 *              Please enable feature FEATURE_QHSUSB_FLCB_STOP_CHARGING to disable
 *              this charging.
 *
 * @param       none
 * 
 * @return  	none
 *
=============================================================================*/
boolean qhsusb_al_chg_should_chg_be_on(void)
{
#ifdef FEATURE_QHSUSB_FLCB_STOP_CHARGING
    return FALSE;
#else 
    return TRUE;
#endif
}
/*===========================================================================
 *
 * @function    qhsusb_al_chg_is_vbus_present
 * 
 * @brief       This function determines the presence or absence of VBus from
 *              PMIC
 *
 * @param       none
 * 
 * @return  	  TRUE - If VBus is present
 *              FALSE - If the VBus is absent
 *
=============================================================================*/
boolean qhsusb_al_chg_is_vbus_present(void)
{
    /** On 8916 only USB charging is present **/
    boolean is_charger_present = FALSE; 
    pm_chg_is_charger_present(&is_charger_present); 
    if(TRUE == is_charger_present)
    {
        return TRUE; 
    }
    else
    {
        return FALSE; 
    }
}


