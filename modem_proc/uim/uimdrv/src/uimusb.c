/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        U I M     U S B     L A Y E R

GENERAL DESCRIPTION
  This module contains the procedures for supporting USB technology based
  UICC cards.The module interacts with the USB driver at power up to
  determine if USB enumeration is successful. If USB enumeration is successful
  the module acts as a shim layer between the UIM server and the USB driver
  translating UIM commands and responses from UIM server APIs to USB driver
  APIs and vice versa.

EXTERNALIZED FUNCTIONS




INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimusb.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/15   ssr     Fixed KW error
11/10/14   akv     Support for user to customize LDO settings
09/18/14   akv     UIMDRV UT enhancements
07/25/14   lxu     Increase usb uicc command time out to 16 seconds by default
07/25/14   lxu     Read cmd_rsp_time from nv for usb mode, give cmd_rsp_time and
                   voltage_class to ap when send usb power up to ap
06/05/14   tkl     Add Vcc and response_timeout for power up request
05/22/14   lxu     Add usb enumeration retry mechanism when receiving iso fallback sig
                   and clear unused usb uicc variables
04/28/14   akv     Monitoring for QMI remote connection avail signal upon iso
                   fallback as it could be during initial pup with an iso card
04/21/14   lxu     Clear usb related timers properly
04/16/14   lxu     Add usb init timer for waiting connection available event
04/14/14   lxu     Fix hot-swap failed in slot2 when enable USB UICC for all slots
04/14/14   lxu     Fixes for Hotswap in USB mode when performed in quick succession
04/11/14   lxu     Unregister automatic UICC shutdown on card removal in USB mode
04/08/14   lxu     Fix ISO card try to turn off USB ICCD interface
04/08/14   lxu     Fix ISO fallback when ISO card inserted
04/02/14   lxu     Clear the remote_cmd_rsp_timer when command timeout
04/01/14   lxu     Use new uim remote power down API to deal with card power
                   down command,set nv 70210 default version to 2
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

This section contains header inclusions

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
/* The uimusb.h header inclusion should be kept out side the featurization.
   This is to ensure that when FEATURE_UIM_USB_UICC is defined in target
   specific cust files it gets included */
#include "uimusb.h"

#ifdef FEATURE_UIM_USB_UICC
#include "timer.h"
#include "comdef.h"
#include "dog.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "task.h"
#include "uimdrv.h"
#include "uim.h"
#include "uimi.h"
#include "uimgen.h"
#include "uimglobals.h"
#include "uimdrv_msg.h"
#ifdef FEATURE_UIM_REMOTE_UIM
#include "uim_remote.h"
#endif /* FEATURE_UIM_REMOTE_UIM */
#include "uimdrv_pmic.h"

/*===========================================================================

            DECLARATIONS FOR MODULE

This section contains declarations of function prototypes needed by this module

===========================================================================*/
#ifdef FEATURE_UIM_REMOTE_UIM
/* USB UICC registration function */
static void uim_usb_remote_initialize
(
  uim_instance_global_type *uim_ptr
);

/*Function to receive event from qmi service*/
void uim_usb_event_callback
(
  const qmi_uim_remote_event_data_type *evt_ptr
);

void uim_usb_attach
(
  uim_instance_global_type *uim_ptr,
  const qmi_uim_remote_event_data_type *evt_ptr
);

void uim_usb_process_card_error
(
  const qmi_uim_remote_event_data_type *evt_ptr
);
#endif /* FEATURE_UIM_REMOTE_UIM */

void uim_usb_init_timer_expiry_cb
(
  timer_cb_data_type param
);

void uim_usb_fallback_to_iso_timer_expiry_cb
(
  timer_cb_data_type param
);

boolean uim_usb_is_atr_valid
(
  uim_instance_global_type *uim_ptr
);
/*===========================================================================

            DEFINITIONS FOR MODULE

This section contains definitions for constants,variables, functions and other
items needed by this module.

===========================================================================*/
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
)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_usb_init");

  timer_def( &uim_ptr->usb.usb_fallback_to_iso_timer,
             &uim_ptr->task.always_on_timer_group,
             uim_ptr->tcb_ptr, 0,
             (timer_t1_cb_type)uim_usb_fallback_to_iso_timer_expiry_cb,
             (timer_cb_data_type)uim_ptr);

  timer_def( &uim_ptr->usb.usb_init_timer,
             &uim_ptr->task.always_on_timer_group,
             uim_ptr->tcb_ptr, 0,
             (timer_t1_cb_type)uim_usb_init_timer_expiry_cb,
             (timer_cb_data_type)uim_ptr);

  /* Initialize the USB interface status data structure */
  uim_ptr->usb.usb_mode_card_supported                              = FALSE;
  uim_ptr->usb.usb_interface_status.uim_remote_client_connected     = FALSE;
  uim_ptr->usb.usb_interface_status.uim_no_remote_client_connection = FALSE;
  uim_ptr->usb.usb_interface_status.usb_selected                    = FALSE;
  uim_ptr->usb.usb_interface_status.atr_data_ptr                    = NULL;
  uim_ptr->usb.usb_interface_status.atr_data_size                   = 0;
  uim_ptr->usb.usb_interface_status.atr_received                    = FALSE;
  uim_ptr->usb.usb_interface_status.powerup_waiting_time            = UIM_USB_POWERUP_WAITING_TIME;
  uim_ptr->usb.usb_interface_status.iso_fallback_occurred           = FALSE;
  uim_ptr->usb.usb_interface_status.usb_enumeration_retries         = 0;

#ifdef FEATURE_UIM_REMOTE_UIM
  /* Call the USB registration function */
  uim_usb_remote_initialize(uim_ptr);
#else
  UIMDRV_MSG_ERR_0(uim_ptr->id,"FEATURE_UIM_REMOTE_UIM undefined, fallback to iso ");
  uim_usb_fallback_to_iso(uim_ptr);
#endif /* FEATURE_UIM_REMOTE_UIM */
} /* uim_usb_init */

#ifdef FEATURE_UIM_REMOTE_UIM
/*===========================================================================

FUNCTION UIM_USB_REMOTE_INITIALIZE

DESCRIPTION
  UIM registers this callback with QMI UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_remote_initialize
(
  uim_instance_global_type *uim_ptr
)
{
  qmi_uim_remote_result_type result      = QMI_UIM_REMOTE_ERROR;
  ASSERT(uim_ptr->id < UIM_MAX_INSTANCES);

  UIMDRV_MSG_HIGH_0( uim_ptr->id,"UIM USB registering with remote qmi at power up");

  uim_ptr->remote.state = WAIT_FOR_CONNECTION_AVAILABLE;
  result = qmi_uim_remote_register(uim_usb_event_callback,(qmi_uim_remote_slot_type)uim_ptr->id);
  if(result == QMI_UIM_REMOTE_ERROR)
  {
    uim_ptr->usb.usb_interface_status.uim_no_remote_client_connection = TRUE;
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM USB registrates with QMI REMOTE FAILED");
    uim_usb_fallback_to_iso(uim_ptr);
    return;
  }
  (void)rex_set_timer(&uim_ptr->usb.usb_init_timer,
                       UIM_USB_REMOTE_CONNECT_WAITING_TIME
                       );
} /* uim_usb_remote_initialize */

/*===========================================================================

FUNCTION UIM_USB_ATTACH

DESCRIPTION
  This  function is called when the USB UICC enumeration
  is successful

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_attach
(
  uim_instance_global_type *uim_ptr,
  const qmi_uim_remote_event_data_type *evt_ptr
)
{

  ASSERT(uim_ptr->id < UIM_MAX_INSTANCES);

  /* Obtain the ATR size from the received buffer size */
  /* ATR size is the received buffer size minus the size of USB response */
  uim_ptr->usb.usb_interface_status.atr_data_size
    = (byte)evt_ptr->evt_data.card_inserted.atr.data_len;

  if (uim_ptr->usb.usb_interface_status.atr_data_size > UIM_USB_MAX_ATR_SIZE)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"ATR rsp cb:ATR size > max. Setting size to Max.");
    uim_ptr->usb.usb_interface_status.atr_data_size = UIM_USB_MAX_ATR_SIZE;
  }
  /* Copy the ATR data over */

  /* Allocate memory of ATR response size */
  uim_ptr->usb.usb_interface_status.atr_data_ptr =
  (uint8 *)uim_malloc(uim_ptr->usb.usb_interface_status.atr_data_size);

  if (uim_ptr->usb.usb_interface_status.atr_data_ptr == NULL)
  {
    ERR_FATAL("UIM USB:ATR rsp cb.Could not allocate memory",0,0,0);
  }
  uim_memscpy(uim_ptr->usb.usb_interface_status.atr_data_ptr,
         uim_ptr->usb.usb_interface_status.atr_data_size,
         evt_ptr->evt_data.card_inserted.atr.data_ptr,
         uim_ptr->usb.usb_interface_status.atr_data_size
        );

  if(uim_usb_is_atr_valid(uim_ptr))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM USB ICCD attach occurred");

    uim_ptr->usb.usb_interface_status.atr_received             = TRUE;
    uim_ptr->usb.usb_interface_status.usb_enumeration_retries  = 0;
    uim_ptr->usb.usb_interface_status.usb_selected             = TRUE;
     /*Clear the enumeration timer*/
    (void)rex_clr_timer(&uim_ptr->usb.usb_fallback_to_iso_timer);

#ifndef FEATURE_UIM_TEST_FRAMEWORK
    uim_set_shutdown_control(uim_ptr->id, FALSE);
#endif/* !FEATURE_UIM_TEST_FRAMEWORK */

    /* Notify UIM server that get ATR action is complete */
    (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
  }
  else
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM USB: ATR data is corrupted, fallback to iso");
    uim_usb_fallback_to_iso(uim_ptr);
    UIM_FREE(uim_ptr->usb.usb_interface_status.atr_data_ptr);
  }
} /* uim_usb_attach */


/*===========================================================================

FUNCTION UIM_USB_PROCESS_CARD_ERROR

DESCRIPTION
  This process card error receive from qmi remote api.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_process_card_error
(
  const qmi_uim_remote_event_data_type *evt_ptr
)
{
  uim_instance_enum_type    remote_slot      = 0;
  uim_instance_global_type *uim_ptr          = NULL;
  uim_global_remote_type   *remote_data_ptr  = NULL;

  if(NULL == evt_ptr)
  {
    UIM_MSG_ERR_0("Null input parameter");
    return;
  }

  remote_slot = evt_ptr->slot;
  RETURN_IF_INSTANCE_INVALID(remote_slot);
  uim_ptr = uim_get_instance_ptr(remote_slot);
  ASSERT(uim_ptr);
  remote_data_ptr = &(uim_ptr->remote);

  UIMDRV_MSG_HIGH_1(uim_ptr->id,
      "Receive card error. evt_ptr->evt_data.card_error=0x%x.",
      evt_ptr->evt_data.card_error.err_cause);

  switch(evt_ptr->evt_data.card_error.err_cause)
  {
    case QMI_UIM_REMOTE_CARD_ERROR_UNKNOWN_ERROR:
    case QMI_UIM_REMOTE_CARD_ERROR_COMMAND_TIMEOUT:
      {
        /* Clear the remote_cmd_rsp_timer first */
		rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
        /* Set the command response timeout signal which will trigger recovery.*/
        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
      }
      break;
    case QMI_UIM_REMOTE_CARD_ERROR_NO_LINK_ESTABLISHED:
      {
        uim_usb_clear_usb_timers(uim_ptr);
        /* fallback to iso */
        uim_usb_fallback_to_iso(uim_ptr);
      }
      break;
    case QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN:
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM USB:Power down card success");
#ifndef FEATURE_UIM_TEST_FRAMEWORK
        uim_power_off_ldo_slot(uim_ptr);
#endif/* !FEATURE_UIM_TEST_FRAMEWORK */
        uim_usb_clear_usb_timers(uim_ptr);
        /* Set the response command status as done */
        uim_ptr->command.rsp_buf.cmd_status = UIM_DONE;
        /* We are done, notify UIM */
        (void)rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
      }
      break;
    case QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_TELECOM:
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM USB:Power down telecom success");
        uim_usb_clear_usb_timers(uim_ptr);
        /* Set the response command status as done */
        uim_ptr->command.rsp_buf.cmd_status = UIM_DONE;
        /* We are done, notify UIM */
        (void)rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
      }
      break;
    default:
      break;
  }
  remote_data_ptr->state = WAIT_FOR_NOTHING;
}/* uim_usb_process_card_error */


/*===========================================================================

FUNCTION UIM_USB_EVENT_CALLBACK

DESCRIPTION
  This procedure is the callback that is called for every QMI REMOTE event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_usb_event_callback(const qmi_uim_remote_event_data_type *evt_ptr)
{
  uim_instance_enum_type    remote_slot      = 0;
  uim_instance_global_type *uim_ptr          = NULL;
  uim_global_remote_type   *remote_data_ptr  = NULL;

  if(NULL == evt_ptr)
  {
    UIM_MSG_ERR_0("Null input parameter");
    return;
  }

  remote_slot = evt_ptr->slot;
  RETURN_IF_INSTANCE_INVALID(remote_slot);
  uim_ptr = uim_get_instance_ptr(remote_slot);

  ASSERT(uim_ptr);
  remote_data_ptr = &(uim_ptr->remote);

  if(uim_usb_iso_fallback_occurred(uim_ptr) &&
     evt_ptr->evt != QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE)
  {
    /* When iso fallback occurred, we will never attempt usb mode,
     and will ignore all event come from remote QMI.
     For hotswap, need to clear the fallback_occurred flag*/
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Receive remote event after iso fallback occurred");
    return;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"receive event: evt_ptr->evt=0x%x",evt_ptr->evt);

  switch(evt_ptr->evt)
  {
    case QMI_UIM_REMOTE_EVT_CONNECTION_AVAILABLE :
    {
      /*qmi uim remote connect success*/
      if (WAIT_FOR_CONNECTION_AVAILABLE != remote_data_ptr->state)
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"receive connection available in wrong state: 0x%x", remote_data_ptr->state);
        return;
      }

      uim_ptr->usb.usb_interface_status.uim_remote_client_connected = TRUE;
      uim_ptr->usb.usb_interface_status.uim_no_remote_client_connection = FALSE;
      (void)rex_clr_timer(&uim_ptr->usb.usb_init_timer);
      remote_data_ptr->state = WAIT_FOR_NOTHING;
      (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_USB_CONNECT_AVAILABLE_SIG);
      break;
    }

    case QMI_UIM_REMOTE_EVT_CARD_RESET :
    {
      if(WAIT_FOR_ATR != remote_data_ptr->state)
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"receive card reset in wrong state: 0x%x", remote_data_ptr->state);
        return;
      }
      remote_data_ptr->state = WAIT_FOR_NOTHING;
      uim_usb_attach(uim_ptr, evt_ptr);
      break;
    }

    case QMI_UIM_REMOTE_EVT_CARD_ERROR :
    {
      uim_usb_process_card_error(evt_ptr);
      break;
    }

    case QMI_UIM_REMOTE_EVT_CARD_WAKEUP :
    {
      if (!uim_usb_is_usb_interface_selected(uim_ptr))
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"USB interface is not selected, this event is not applicable");
        return;
      }
      /* For remote wake up, modem sends STATUS command to poll the usb uicc. */
      (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_USB_REMOTE_WAKEUP_SIG);
      break;
    }

    case QMI_UIM_REMOTE_EVT_CONNECTION_UNAVAILABLE :
    {
      /* Fallback to iso interface when connection is unavailable */
      uim_usb_fallback_to_iso(uim_ptr);
      break;
    }

    default:
      break;
  }
  return;
}/* uim_usb_event_callback */

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
)
{
  qmi_uim_remote_result_type result  = QMI_UIM_REMOTE_ERROR;

  ASSERT(uim_ptr);
  uim_ptr->remote.state = WAIT_FOR_POWER_DOWN_RSP;

  /* Set different timer according to pdown_mode. If response wasn't
     received for POWER_DOWN_CARD, fallback to ISO. If response wasn't
     received for POWER_DOWN_TELECOM, do recovery*/
  if (pdown_mode == QMI_UIM_REMOTE_POWER_DOWN_CARD)
  {
    (void)rex_set_timer(&uim_ptr->usb.usb_fallback_to_iso_timer,
                        uim_usb_get_work_waiting_time(uim_ptr)
                       );
  }
  else
  {
    (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
                        uim_usb_get_work_waiting_time(uim_ptr)
                       );
  }
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIM USB UICC ICCD POWER OFF issued, mode is 0x%x",
                    pdown_mode);
  result = qmi_uim_remote_card_power_down((qmi_uim_remote_slot_type)uim_ptr->id,
                                          pdown_mode);

  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM USB: Could not powerdown the remote card");
    (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
  }
  return;
} /* uim_usb_iccd_power_off */


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
)
{
  qmi_uim_remote_result_type result              = QMI_UIM_REMOTE_ERROR;
  uint32                     voltage_class_id    = 0;
  boolean                    voltage_class_valid = FALSE;
  ASSERT(uim_ptr);
  uim_ptr->remote.state = WAIT_FOR_ATR;
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  voltage_class_id = uim_get_pmic_npa_mode(uim_ptr->id);
#endif /* #ifndef FEATURE_UIM_TEST_FRAMEWORK */

  (void)rex_set_timer(&uim_ptr->usb.usb_fallback_to_iso_timer,
                      uim_usb_get_powerup_waiting_time(uim_ptr)
                     );
  voltage_class_valid = ( (voltage_class_id-1) > 0)? TRUE:FALSE;
  UIMDRV_MSG_HIGH_3( uim_ptr->id,"UIM USB:Power up ICCD issued, command response time is 0x%x, voltage class is 0x%x,voltage_class_valid is 0x%x",
                                          uim_usb_get_work_waiting_time(uim_ptr),
                                          ((voltage_class_id-1 > 0)?(voltage_class_id-1):0),
                                          voltage_class_valid);

  result = qmi_uim_remote_card_power_up((qmi_uim_remote_slot_type)uim_ptr->id,
                                         TRUE,
                                         uim_usb_get_work_waiting_time(uim_ptr),
										 voltage_class_valid,
                                         ((voltage_class_id-1 > 0)?(voltage_class_id-1):0)
                                         );
  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM USB:Could not powerup the remote card");
    /* Clear the transaction timer */
    (void)rex_clr_timer(&uim_ptr->usb.usb_fallback_to_iso_timer);
     /*power up failure, fallback to iso interface*/
    uim_usb_fallback_to_iso(uim_ptr);
  }
  return;
} /* uim_usb_iccd_power_on */
#endif /* FEATURE_UIM_REMOTE_UIM */

/*===========================================================================

FUNCTION UIM_USB_IS_ATR_VALID

DESCRIPTION
  This  function is called to check the atr data received from qmi.

DEPENDENCIES
  None

RETURN VALUE
 TRUE indicate that the ATR DATA is valid
 FALSE indicate that the ATR DATA is valid
SIDE EFFECTS
  None

===========================================================================*/
boolean uim_usb_is_atr_valid
(
  uim_instance_global_type *uim_ptr
)
{
  uint32  i          = 0;
  uint8   check_data = 0x00;

  if ( uim_ptr->usb.usb_interface_status.atr_data_size < UIM_USB_MIN_ATR_SIZE
    || uim_ptr->usb.usb_interface_status.atr_data_size > UIM_USB_MAX_ATR_SIZE)
  {
    return FALSE;
  }

  /* XOR -- T0 to TCK, skip Ts */
  for (i = 1; i < uim_ptr->usb.usb_interface_status.atr_data_size; ++i)
  {
    check_data ^= uim_ptr->usb.usb_interface_status.atr_data_ptr[i];
  }

  if (0x00 == check_data)
  {
    return TRUE;
  }

  return FALSE;
}

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
)
{
  if ( !uim_usb_iso_fallback_occurred(uim_ptr))
  {
    uim_ptr->usb.usb_interface_status.usb_selected = FALSE;
    uim_ptr->usb.usb_interface_status.usb_enumeration_retries++;

    /* Try two more time of reseting uim and doing usb enumeration */
    if (uim_ptr->usb.usb_interface_status.usb_enumeration_retries >
        UIM_USB_MAX_ENUMERATION_RETRY_NUM)
    {
      /* when iso_fallback_occurred was set to true, never try usb enumeration*/
      uim_ptr->usb.usb_interface_status.iso_fallback_occurred = TRUE;
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Falling back to iso");
    }
    else
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id, "Retrying usb enumeration,usb_enumeration_retries is 0x%x",
                                uim_ptr->usb.usb_interface_status.usb_enumeration_retries);
    }
    /*Reset Poll timer value, since USB and ISO mode have different poll timer*/
    uim_reset_poll_timer_val(uim_ptr);
    uim_reset_globals(uim_ptr);
    uim_usb_clear_usb_timers(uim_ptr);
    (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_USB_FALLBACK_TO_ISO_SIG);
  }
}/* uim_usb_fallback_to_iso */

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
)
{
  return (uim_ptr->usb.usb_interface_status.usb_selected);
} /* uim_usb_is_usb_interface_selected */


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
)
{
  return (uim_ptr->usb.usb_interface_status.atr_received);
} /* uim_usb_is_usb_atr_received */


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
)
{
  return (uim_ptr->usb.usb_mode_card_supported);
} /* uim_usb_is_usb_mode_card_supported */

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
)
{
  return (uim_ptr->usb.usb_interface_status.atr_data_ptr);
} /* uim_usb_get_atr_data_ptr */


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_DATA_SIZE

DESCRIPTION
  This function returns to the caller the size of the ATR data

DEPENDENCIES
  None

RETURN VALUE
  uint8: ATR data size

SIDE EFFECTS
  None

===========================================================================*/
uint8 uim_usb_get_atr_data_size
(
  uim_instance_global_type *uim_ptr
)
{
  return ((uint8)uim_ptr->usb.usb_interface_status.atr_data_size);
} /* uim_usb_get_atr_data_size */


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
)
{
  return ((uim_ptr->usb.usb_interface_status.cmd_rsp_time > 0 ) ?
        uim_ptr->usb.usb_interface_status.cmd_rsp_time : UIM_USB_DEFAULT_CMD_RSP_TIME);
} /* uim_usb_get_work_waiting_time */


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
)
{
  return (uim_ptr->usb.usb_interface_status.powerup_waiting_time);
} /* uim_usb_get_powerup_waiting_time */


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
)
{
  /* The number of historical characters present in the ATR is encoded in the
     least significant nibble of the T0 character in the ATR per ISO 7816-3,
     section 6.4.2. Using a bit mask to determine the size. */
  return (*(uim_ptr->usb.usb_interface_status.atr_data_ptr+1) & UIM_K_FIELD);
} /* uim_usb_get_atr_hist_char_size */


/*===========================================================================

FUNCTION UIM_USB_GET_ATR_CHECK_CHAR_SIZE

DESCRIPTION
  This function returns to the caller the size of check character in TCK
  the ATR data

DEPENDENCIES
  ATR must be

RETURN VALUE
  uint8: 0 if only T=0 is indicated; 1 otherwise

SIDE EFFECTS
  None

===========================================================================*/
static uint8 uim_usb_get_atr_check_char_size
(
   uim_instance_global_type *uim_ptr
)
{
  /* Per ISO 7816-3 section 6.4.5, if only T=0 transmisison protocol is
     indicated as being supported then the check character shall be absent from
     the ATR. In all other cases TCK shall be present. Interpreting the spec
     there are two conditions under which only T=0 will be supported:
     (1) If TD1 is absent, only T=0 is supported by default
     (2) For each TD(i) present if the lower nibble = 0000,
         only T=0 is supported */

  boolean Teq0_only = FALSE;
  boolean parse_TDi = TRUE;
  uint8   Ti_cnt    = 0;
  /* Start processing TD(i) at T0 */
  uint8*  TDi_ptr   = uim_ptr->usb.usb_interface_status.atr_data_ptr+1;

  /* Check if TD1 is present */
  if (*TDi_ptr & UIM_ATR_TD_PRESENT)
  {
    while (parse_TDi) /* Check each TD(i) found in the interface chars */
    {
      /* Count the number of interface chars present in the i-th set */
      Ti_cnt = ((*TDi_ptr & UIM_ATR_TC_PRESENT) >> 6)
              + ((*TDi_ptr & UIM_ATR_TB_PRESENT) >> 5)
              + ((*TDi_ptr & UIM_ATR_TA_PRESENT) >> 4) + 1;
      if ((TDi_ptr + Ti_cnt) >=
          (uim_ptr->usb.usb_interface_status.atr_data_ptr+
           uim_usb_get_atr_data_size(uim_ptr)))
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Check char size calc err. Err in hist char possible.");
        parse_TDi = FALSE;
      }
      else if (*(TDi_ptr+Ti_cnt) & UIM_T_FIELD) /*Check lower nibble of TD(i)*/
      {
        /*If lower nibble is non-zero protocols other than T=0 are supported*/
        Teq0_only = FALSE;
        parse_TDi = FALSE;
      }
      else if (*(TDi_ptr+Ti_cnt) & UIM_ATR_TD_PRESENT) /*Check for more TD(i)*/
      {
        parse_TDi = TRUE;
        TDi_ptr = TDi_ptr + Ti_cnt;
      }
      else /* Checked all TD(i) and only T=0 is supported */
      {
        Teq0_only = TRUE;
        parse_TDi = FALSE;
      }
    }
  }
  else /* If TD1 is absent only T=0 is supported */
  {
    Teq0_only = TRUE;
  }

  if (Teq0_only)
  {
    return 0;
  }
  else
  {
    return 1;
  }
} /* uim_usb_get_atr_check_char_size */


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
)
{
  /* Pointer to hist chars =
     ATR pointer + ((ATR size - hist char size) - check char size) */
  return (uim_ptr->usb.usb_interface_status.atr_data_ptr +
          ((uim_usb_get_atr_data_size(uim_ptr)
         - uim_usb_get_atr_hist_char_size(uim_ptr)) -
            uim_usb_get_atr_check_char_size(uim_ptr)));
} /* uim_usb_get_atr_hist_char_ptr */

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
)
{
  return uim_ptr->usb.usb_interface_status.iso_fallback_occurred;
}/*uim_usb_iso_fallback_occurred*/

/*===========================================================================
FUNCTION UIM_USB_INIT_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the usb init timer. This call-back is called in the context of the high
  priority timer task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Set uim_no_remote_client_connection to TRUE if the init timer expired.
===========================================================================*/
void uim_usb_init_timer_expiry_cb(timer_cb_data_type param)
{
  uim_instance_global_type *uim_ptr = (uim_instance_global_type*)param;

  ASSERT(uim_ptr);
  UIMDRV_MSG_ERR_0(uim_ptr->id, "uim usb init timer expired");

  uim_ptr->usb.usb_interface_status.uim_no_remote_client_connection = TRUE;
  (void)rex_clr_timer(&uim_ptr->usb.usb_init_timer);

  uim_usb_fallback_to_iso(uim_ptr);
  return;
} /* uim_usb_init_timer_expiry_cb */

/*===========================================================================
FUNCTION UIM_USB_FALLBACK_TO_ISO_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the usb fallback to iso timer. This call-back is called in the context of
  the high priority timer task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Usb fallback to iso interface if the fallback to iso timer expired.
===========================================================================*/
void uim_usb_fallback_to_iso_timer_expiry_cb(timer_cb_data_type param)
{
  uim_instance_global_type *uim_ptr = (uim_instance_global_type*)param;

  ASSERT(uim_ptr);
  UIMDRV_MSG_ERR_0(uim_ptr->id, "uim usb fallback to iso timer expired");
  uim_usb_fallback_to_iso(uim_ptr);
  return;
} /* uim_usb_fallback_to_iso_timer_expiry_cb */

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
)
{
  UIMDRV_MSG_HIGH_1(uim_ptr->id, "uim_usb_reset_globals, telecom selected 0x%x",is_telecom);
  /* Initialize the USB interface status data structure */
  uim_ptr->usb.usb_interface_status.atr_data_ptr                    = NULL;
  uim_ptr->usb.usb_interface_status.atr_data_size                   = 0;
  uim_ptr->usb.usb_interface_status.atr_received                    = FALSE;
  uim_ptr->usb.usb_interface_status.powerup_waiting_time            = UIM_USB_POWERUP_WAITING_TIME;

  if (FALSE == is_telecom)
  {
    uim_ptr->usb.usb_interface_status.usb_selected                    = FALSE;
    uim_ptr->usb.usb_interface_status.iso_fallback_occurred           = FALSE;
    uim_ptr->usb.usb_interface_status.usb_enumeration_retries         = 0;
    uim_ptr->usb.usb_mode_card_supported                              = FALSE;
  }
}/* uim_usb_reset_globals */

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
)
{
  return !uim_ptr->usb.usb_interface_status.uim_no_remote_client_connection;
} /* uim_usb_has_remote_connection */

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
)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_usb_clear_usb_timers");
  (void)rex_clr_timer(&uim_ptr->usb.usb_fallback_to_iso_timer);
#ifdef FEATURE_UIM_REMOTE_UIM
  (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
#endif /* FEATURE_UIM_REMOTE_UIM */
}/* uim_usb_clear_usb_timers */

#endif /* FEATURE_UIM_USB_UICC */
