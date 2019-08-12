#ifndef UIMUSB_H
#define UIMUSB_H
/*===========================================================================

                   F O R W A R D   D E C L E R A T I O N S   F O R
                      U I M   U S B   L A Y E R

DESCRIPTION
  This file contains all the forward declarations for the UIM USB layer

Copyright (c) 2008-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimusb.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/14   lxu     Increase usb uicc command time out to 16 seconds by default
07/25/14   lxu     Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                   voltage_class to ap when send usb power up to ap
05/22/14   lxu     Add usb enumeration retry mechanism when receiving iso fallback sig
                   and clear unused usb uicc variables
04/21/14   lxu     Clear usb related timers properly
04/16/14   lxu     Add usb init timer for waiting connection available event
04/14/14   lxu     Fix hot-swap failed in slot2 when enable USB UICC for all slots
04/14/14   lxu     Fixes for Hotswap in USB mode when performed in quick succession
04/01/14   lxu     Use new uim remote power down API to deal with card power
                   down command,set nv 70210 default version to 2
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement
02/18/14   sam     Usage of newer style diag macros
01/14/14   ak      Uim task signal handling re-factor/cleanup
09/26/13   yk      Clean up signals
07/12/13   nmb     Global Reorganization
02/08/13   js      Updated UIM USB code for parallel processing
12/05/12   js      UIM Parallel Processing changes
10/09/09   vs      Updated HS-USB init timer to accomodate long TMC init
09/24/09   vs      Updated enumeration timer and created new value
02/04/09   vs      Added access to historical bytes in ATR
10/07/08   vs      Reduced UIM_USB_MAX_NUM_DATA_BLOCK_RETRIES from 1000 to 50
09/23/08   js      Added LTK Support for USB UICC
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/15/08   vs      Added support for UIM USB ICCD pre-suspend timer
09/05/08   vs      Added support for cool down period between failed USB
                   enumeration and ISO fallback. Removed prototype for fallback
                   function that is no longer externalized
08/22/08   vs      Added functions to allow rex_sleep() removal. Added support
                   for powerup waiting time and ISO fallback
08/22/08   js      Re-designed R-APDU status word processing
08/05/08   vs      Moved declarations from uimusb source to header file. Added
                   declaration for uim_usb_is_usb_atr_received()
07/18/08   vs      Moved uimdrv.h header inclusion out of featurization
                   Added rex.h header inclusion
07/17/08   vs      Created the UIM USB module to support the UIM USB UICC
                   ICCD interface

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"
/* The uimdrv.h header inclusion should be kept out side the featurization.
   This is to ensure that when FEATURE_UIM_USB_UICC is defined in target
   specific cust files it gets included */
#include "uimdrv.h"
#include "uim_v.h"
#ifdef FEATURE_UIM_USB_UICC
#include "qmi_uim_remote.h"
#include "comdef.h"
#include "rex.h"


/* Max Size of the UIM USB ATR in bytes */
#define UIM_USB_MAX_ATR_SIZE                    32

/* Min Size of the UIM USB ATR in bytes */
#define UIM_USB_MIN_ATR_SIZE                    2

/* UIM USB command response time in ms */
#define UIM_USB_DEFAULT_CMD_RSP_TIME            16000

/* UIM USB remote connect waiting time in ms */
#define UIM_USB_REMOTE_CONNECT_WAITING_TIME     3000

/* UIM USB power up waiting time in ms */
#define UIM_USB_POWERUP_WAITING_TIME            5000

/* UIM USB max enumeration retry number */
#define UIM_USB_MAX_ENUMERATION_RETRY_NUM       2


/* UIM USB Interface Status Type */
typedef struct {
  boolean                    uim_remote_client_connected;
  boolean                    uim_no_remote_client_connection;
  boolean                    usb_selected;
  uint8                      *atr_data_ptr;
  uint16                     atr_data_size;
  boolean                    atr_received;
  rex_timer_cnt_type         cmd_rsp_time;
  rex_timer_cnt_type         powerup_waiting_time;
  boolean                    iso_fallback_occurred;
  uint8                      usb_enumeration_retries;
} uim_usb_interface_status_type;

/*===========================================================================

FUNCTION UIM_USB_INIT

DESCRIPTION
  This function initializes UIM USB related timers and other variables. It is
  called only once at power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  UIM starts to receive events from USB driver based on the callbacks
  registered if the registration was successful. Static structure
  uim_usb_interface_status is initialized

===========================================================================*/
void uim_usb_init
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_IS_USB_INTERFACE_SELECTED

DESCRIPTION
  This function returns to the caller whether the USB interface has been
  selected

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE if USB interface is selected and FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_is_usb_interface_selected
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_IS_USB_MODE_CARD_SUPPORTED

DESCRIPTION
  This function returns to the caller whether the card inserted support USB
  mode

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE card support USB mode and FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_is_usb_mode_card_supported
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_IS_USB_ATR_RECEIVED

DESCRIPTION
  This function returns to the caller whether the USB ATR has been
  received

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE if USB ATR is received and FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_is_usb_atr_received
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_ICCD_POWER_OFF

DESCRIPTION
  This function is called to send the ICCD_POWER_OFF command to the USB UICC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_iccd_power_off
(
   uim_instance_global_type *uim_ptr,
   qmi_uim_remote_power_down_mode_enum_type  pdown_mode
);


/*===========================================================================

FUNCTION UIM_USB_ICCD_POWER_ON

DESCRIPTION
  This function is called to send the ICCD_POWER_ON command to the USB UICC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_iccd_power_on
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_DATA_PTR

DESCRIPTION
  This function returns to the caller a pointer to the ATR data

DEPENDENCIES
  None

RETURN VALUE
  uint8 *: Pointer to ATR data

SIDE EFFECTS
  None

===========================================================================*/
uint8 * uim_usb_get_atr_data_ptr
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_DATA_SIZE

DESCRIPTION
  This function returns to the caller the size of the ATR data

DEPENDENCIES
  None

RETURN VALUE
  uint16: Size of ATR data

SIDE EFFECTS
  None

===========================================================================*/
uint8 uim_usb_get_atr_data_size
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_GET_WORK_WAITING_TIME

DESCRIPTION
  This function returns to the caller the time to wait before assuming the
  transaction with the card has resulted in no response

DEPENDENCIES
  None

RETURN VALUE
  rex_timer_cnt_type : Work waiting time

SIDE EFFECTS
  None

===========================================================================*/
rex_timer_cnt_type uim_usb_get_work_waiting_time
(
   uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_GET_POWERUP_WAITING_TIME

DESCRIPTION
  This function returns to the caller the maximum time to wait before assuming
  that a IC USB power up related transaction has resulted in no response

DEPENDENCIES
  None

RETURN VALUE
  rex_timer_cnt_type : Powerup waiting time

SIDE EFFECTS
  None

===========================================================================*/
rex_timer_cnt_type uim_usb_get_powerup_waiting_time
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_HIST_CHAR_SIZE

DESCRIPTION
  This function returns to the caller the size of historical characters in
  the ATR data

DEPENDENCIES
  ATR must be

RETURN VALUE
  uint8: Number of historical characters in the ATR data

SIDE EFFECTS
  None

===========================================================================*/
uint8 uim_usb_get_atr_hist_char_size
(
   uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_HIST_CHAR_PTR

DESCRIPTION
  This function returns to the caller a pointer to the starting byte of
  the historical characters in the ATR data

DEPENDENCIES
  None

RETURN VALUE
  uint8 *: Pointer to starting byte of historical characters in the ATR data

SIDE EFFECTS
  None

===========================================================================*/
uint8* uim_usb_get_atr_hist_char_ptr
(
   uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_ISO_FALLBACK_OCCURED

DESCRIPTION
  This function returns to the boolean value of iso_fallback_occurred

DEPENDENCIES
  None

RETURN VALUE
  TRUE when iso fallback occured
  FALSE when no iso fallback occured.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_iso_fallback_occurred
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_USB_FALLBACK_TO_ISO

DESCRIPTION
  This clear usb_init_or_enum_fail_timer and set UIM_USB_FALLBACK_TO_ISO_SIG
  signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_fallback_to_iso
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================
FUNCTION UIM_USB_RESET_GLOBALS

DESCRIPTION
  This function resets uim usb related global variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_usb_reset_globals
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_telecom
);

/*===========================================================================

FUNCTION UIM_USB_HAS_REMOTE_CONNECTION

DESCRIPTION
  This function returns to the boolean value of !uim_no_remote_client_connection

DEPENDENCIES
  None

RETURN VALUE
  TRUE when has remote connection
  FALSE when no remote connection.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_has_remote_connection
(
    uim_instance_global_type *uim_ptr
);

/*===========================================================================
FUNCTION UIM_USB_CLEAR_USB_TIMERS

DESCRIPTION
  This function clears uim usb related timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_usb_clear_usb_timers
(
  uim_instance_global_type *uim_ptr
);
#endif /* FEATURE_UIM_USB_UICC */
#endif /* UIMUSB_H */
