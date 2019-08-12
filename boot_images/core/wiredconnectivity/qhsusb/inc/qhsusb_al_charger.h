#ifndef _QUSB_CHARGER_H_
#define _QUSB_CHARGER_H_
/*=======================================================================*//**
 * @file        qhsusb_al_charger.h
 * @author:     shreyasr  
 * @date        12-Mar-2013
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) for Charging exported API.
 *
 * @details     This header file contains the API to be used by the boot for charging via USB.
  *
 * @note        
 *
 *              Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

// ===========================================================================
//
//                           EDIT HISTORY FOR FILE
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
//  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/inc/qhsusb_al_charger.h#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $
//
// when       who      what, where, why
// --------   ---      --------------------------------------------------------
// 03/12/2013 shreyasr  Initial Draft created
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // basic types as byte word uint32 etc

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
typedef enum
{
  HSU_BOOT_POLL_RESULT_ERROR = -1,
  HSU_BOOT_POLL_RESULT_NO_NEW_EVENT,           /* No change in USB state resulted by polling */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC,   /* USB cable is connected to a standard HOST */
  HSU_BOOT_POLL_RESULT_DISCONNECTED,           /* USB cable disconnected */
  HSU_BOOT_POLL_RESULT_NOT_CONNECTED,          /* USB cable is not connected */
  HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED, /* Device is resumed, but not enumerated yet. 
                                                  When resuming *after* enumeration, the 
                                                  ENUMERATED event will be raised */
  HSU_BOOT_POLL_RESULT_ENUMERATED,             /* Device is enumerated successfully */
  HSU_BOOT_POLL_RESULT_SUSPENDED,              /* Device is suspended */
  HSU_BOOT_POLL_RESULT_ENUMERATED_100MA,       /* Device is enumerated successfully with 100ma configuration */
  HSU_BOOT_POLL_RESULT_MAX
} qhsusb_boot_poll_result;


typedef enum
{
  QHSUSB_CHARGER_PORT_SDP,
  QHSUSB_CHARGER_PORT_CDP,
  QHSUSB_CHARGER_PORT_DCP,
  QHSUSB_CHARGER_PORT_DCP_NON_COMPLIANT,
  QHSUSB_CHARGER_PORT_INVALID, 
  QHSUSB_CHARGER_PORT_UNKNOWN 
} qhsusb_charger_port_type;

typedef enum
{
	QHSUSB_ENUMERATION_SUCCESS,
	QHSUSB_ENUMERATION_FAILURE,
	QHSUSB_DEVICE_DISCONNECTED
} qhsusb_enumeration_result; 

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

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
qhsusb_charger_port_type qhsusb_al_chg_get_charger_port_type(void);
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
qhsusb_enumeration_result qhsusb_al_chg_enumerate_usb(void);
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
qhsusb_boot_poll_result qhsusb_al_chg_poll(void);
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
boolean qhsusb_al_chg_init(void);
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
void qhsusb_al_chg_deinit(void);
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
void qhsusb_al_chg_disconnect_from_host(void);
/*===========================================================================
 *
 * @function    qhsusb_al_get_charger_type_from_pbl_info
 * 
 * @brief       This function is used to get the charger type if FLCB has happened.
 *				It is determined by reading the TCSR register.
 *
 * @param       none
 * 
 * @return  	qhsusb_charger_port_type
 * 				port type which is detected from reading TCSR Register
 *
=============================================================================*/
qhsusb_charger_port_type qhsusb_al_get_charger_type_from_pbl_info(void);
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
boolean qhsusb_al_chg_should_chg_be_on(void);

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
boolean qhsusb_al_chg_is_vbus_present(void);

#endif /*_QUSB_CHARGER_H_*/
