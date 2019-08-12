/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       U I M  REMOTE . C

GENERAL DESCRIPTION
  This module contains the procedures for the uim remote client.
  The module receives the event from the QMI REMOTE CLIENT. The module handles the
  commands sent to the QMI remote client and also handles the responses from the
  QMI remote client.

Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_remote.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   ssr     Remove msg for malloc failure
10/26/15   sam     Max atr chars length check for Remote sim
06/24/15   ssr     Fixed KW errors
03/20/15   ks      DO not do GET RESPONSE for the 62 F3
10/01/14   ll      KW errors
09/18/14   akv     UIMDRV UT enhancements
05/07/14   tkl     Added Vcc and Rsp Timeout for power up request
04/02/14   lxu     Support USB UICC APDU logging
04/01/14   xj      set CMD_RSP_TIME_OUT if USB drv timeout or response size 0
03/27/14   lxu     USB UICC modem implement
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/06/14   tkl     Add power down mode for power down request
01/29/14   sam     Usage of newer style diag macros
12/12/13   ks      BTSAP in client mode, intial implementation
===========================================================================*/
#include "uim_remote.h"
#include "uimdrv_main.h"

#ifdef FEATURE_UIM_REMOTE_UIM
/* Response APDU Status Word Table - First two columns are combinations of SW1 and
   SW2. Third column indicates if the command status is default for all
   the SW2s that are not mentioned in the table for given SW1 value. Fourth column
   is the command response status that we need to send back to uim server for given
   combination of SW1 and SW2. */

static uim_remote_sw_data_type uim_remote_sw_data_table[] =
{
  /*SW1   SW2  Default Response Status */
  {0x90,  0x00, TRUE,  UIM_DONE},
  {0x91,  0x00, TRUE,  UIM_DONE},
  {0x9F,  0xFF, TRUE,  UIM_REMOTE_GET_RESPONSE_STATUS},
  {0x63,  0x00, FALSE, UIM_NO_INFO_AND_NV_CHANGED},
  {0x63,  0x81, FALSE, UIM_FILE_FILLED_UP_BY_LAST_WRITE},
  {0x63,  0x82, FALSE, UIM_EOF_BEFORE_READING_LE_BYTES},
  {0x63,  0x10, FALSE, UIM_REMOTE_GET_RESPONSE_STATUS},
  {0x63,  0xFF, TRUE,  UIM_MAXIMUM_STATUS},
  {0x93,  0x00, FALSE, UIM_TOOLKIT_BUSY},
  {0x93,  0xFF, TRUE,  UIM_PROBLEM},
  {0x92,  0x40, FALSE, UIM_MEMORY_PROBLEM},
  {0x92,  0xFF, TRUE , UIM_DONE},
  {0x94,  0x00, FALSE, UIM_NO_EF_SELECTED},
  {0x94,  0x02, FALSE, UIM_OUT_OF_RANGE},
  {0x94,  0x04, FALSE, UIM_NOT_FOUND},
  {0x94,  0x08, FALSE, UIM_INCONSISTENT_WITH_COMMAND},
  {0x94,  0x84, FALSE, UIM_ALGORITHM_NOT_SUPPORTED},
  {0x94,  0xFF, TRUE , UIM_PARITY},
  {0x98,  0x02, FALSE, UIM_NO_CHV_INITIALIZED},
  {0x98,  0x04, FALSE, UIM_ACCESS_NOT_FULFILLED},
  {0x98,  0x08, FALSE, UIM_CONTRADICTION_W_CHV_STATUS},
  {0x98,  0x10, FALSE, UIM_CONTRADICTION_W_INVALID_STATUS},
  {0x98,  0x34, FALSE, UIM_SSD_UPDATE_SEQ_PROBLEM},
  {0x98,  0x40, FALSE, UIM_CHV_BLOCKED},
  {0x98,  0x50, FALSE, UIM_MAX_VALUE_REACHED},
  {0x98,  0x62, FALSE, UIM_APPLCN_SPECIFIC_AUTH_ERROR},
  {0x98,  0xFF, TRUE,  UIM_PARITY},
  {0x60,  0xFF, TRUE,  UIM_DONE},
  {0x61,  0xFF, TRUE,  UIM_REMOTE_GET_RESPONSE_STATUS},
  {0x62,  0x00, FALSE, UIM_NO_INFO_AND_NV_UNCHANGED},
  {0x62,  0x81, FALSE, UIM_PART_DATA_MAY_BE_CORRUPTED},
  {0x62,  0x82, FALSE, UIM_EOF_BEFORE_READING_LE_BYTES},
  {0x62,  0x83, FALSE, UIM_SELECTED_FILE_INVALIDATED},
  {0x62,  0x84, FALSE, UIM_INCORRECT_FCI_FORMAT},
  {0x62,  0xF3, FALSE, UIM_DONE},
  {0x62,  0xFF, TRUE,  UIM_REMOTE_GET_RESPONSE_STATUS},
  {0x64,  0x00, FALSE, UIM_EXEC_ERR_AND_NV_UNCHANGED},
  {0x64,  0xFF, TRUE,  UIM_PARITY},
  {0x65,  0x00, FALSE, UIM_EXEC_ERR_AND_NV_CHANGED},
  {0x65,  0x81, FALSE, UIM_EXEC_ERR_MEMORY_PROBLEM},
  {0x65,  0xFF, TRUE,  UIM_PARITY},
  {0x66,  0xFF, TRUE,  UIM_SE_NOT_SET},
  {0x67,  0x00, FALSE, UIM_INCORRECT_PARAM_P3},
  {0x67,  0xFF, TRUE,  UIM_PARITY},
  {0x68,  0x00, FALSE, UIM_CLA_ERR_NO_INFO},
  {0x68,  0x81, FALSE, UIM_LOGICAL_CHAN_NOT_SUP},
  {0x68,  0x82, FALSE, UIM_SECURE_MESSAGING_NOT_SUP},
  {0x68,  0xFF, TRUE,  UIM_PARITY},
  {0x69,  0x00, FALSE, UIM_CMD_ERR_NO_INFO},
  {0x69,  0x81, FALSE, UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT},
  {0x69,  0x82, FALSE, UIM_SEC_STATUS_NOT_SATISFIED},
  {0x69,  0x83, FALSE, UIM_AUTH_PIN_BLOCKED},
  {0x69,  0x84, FALSE, UIM_REF_DATA_INVALIDATED},
  {0x69,  0x85, FALSE, UIM_USE_CONDNS_NOT_SATISFIED},
  {0x69,  0x86, FALSE, UIM_CMD_ERR_NO_EF_SELECTED},
  {0x69,  0x99, FALSE, UIM_WRONG_CLASS},
  {0x69,  0x01, FALSE, UIM_MFLO_UCAST_AUTH_KEY_NOT_INIT},
  {0x69,  0x05, FALSE, UIM_MFLO_LOCKED_INIT_PARAM},
  {0x69,  0x04, FALSE, UIM_MFLO_VERIFY_UCAST_FAILED},
  {0x69,  0x02, FALSE, UIM_MFLO_UCAST_DATA_BLOCK_NOT_INIT},
  {0x69,  0x06, FALSE, UIM_MFLO_APPLET_NOT_INIT},
  {0x69,  0x07, FALSE, UIM_MFLO_PUBLIC_KEY_OR_CERT_NOT_INIT},
  {0x69,  0xFF, TRUE,  UIM_PARITY},
  {0x6A,  0x80, FALSE, UIM_INCORRECT_PARAM_IN_DATA_FIELD},
  {0x6A,  0x81, FALSE, UIM_FUNCTION_NOT_SUPPORTED},
  {0x6A,  0x82, FALSE, UIM_FILE_NOT_FOUND},
  {0x6A,  0x83, FALSE, UIM_RECORD_NOT_FOUND},
  {0x6A,  0x85, FALSE, UIM_LC_INCONSISITENT_WITH_TLV},
  {0x6A,  0x86, FALSE, UIM_INCORRECT_PARAMS_P1_P2},
  {0x6A,  0x87, FALSE, UIM_LC_INCONSISITENT_WITH_P1_P2},
  {0x6A,  0x88, FALSE, UIM_REF_DATA_NOT_FOUND},
  {0x6A,  0x84, FALSE, UIM_NOT_ENOUGH_MEMORY_SPACE},
  {0x6A,  0xFF, TRUE,  UIM_PARITY},
  {0x6B,  0xFF, TRUE,  UIM_INCORRECT_PARAM_P1_2},
  {0x6C,  0xFF, TRUE,  UIM_REMOTE_GET_RESPONSE_STATUS},
  {0x6D,  0xFF, TRUE,  UIM_UNKNOWN_INS},
  {0x6E,  0xFF, TRUE,  UIM_WRONG_CLASS},
  {0x6F,  0xFF, TRUE,  UIM_PROBLEM},
  {0xFE,  0xFF, TRUE,  UIM_PARITY}
};

/*===========================================================================
FUNCTION UIM_REMOTE_APDU_RESPONSE_CB

DESCRIPTION
  This procedure is the callback that is called after every QMI REMOTE send command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_remote_apdu_response_cb(const qmi_uim_remote_rsp_apdu_data_type *remote_rsp_apdu_ptr)
{
  const uint8           *rsp_apdu_ptr     = NULL;
  uint16_t              rsp_apdu_size     = 0;
  uint8                 sw1               = 0x00;
  uint8                 sw2               = 0x00;
  uim_global_remote_type  *remote_data_ptr  = NULL;
  uim_instance_global_type *uim_ptr       = NULL;
  uint16                i                 = 0;

  if(NULL == remote_rsp_apdu_ptr)
  {
    UIM_MSG_ERR_0("Remote response pointer is NULL");
    return;
  }
  uim_ptr = uim_get_instance_ptr(remote_rsp_apdu_ptr->slot);

  if(NULL == uim_ptr)
  {
    UIM_MSG_ERR_0("uim_ptr is NULL");
    return;
  }

  remote_data_ptr   = &(uim_ptr->remote);

  if(NULL == remote_data_ptr)
  {
    UIMDRV_MSG_HIGH_0( uim_ptr->id, "Remote data pointer is NULL" );
    return;
  }

  /* Ignore response as it is not expected*/
  if((WAIT_FOR_RESPONSE != remote_data_ptr->state)||
     ( NULL == remote_data_ptr->cmd_ptr)||
     ( NULL == remote_data_ptr->cmd_ptr->rsp_ptr))
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,
                  "Either UIM btsap state : 0x%x not expecting a response or cmd_ptr is NULL",
remote_data_ptr->state);
    return;
  }

  remote_data_ptr->state = WAIT_FOR_NOTHING;
  /* Clear the btsap transaction timer */
  (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);

  if((QMI_UIM_REMOTE_SUCCESS != remote_rsp_apdu_ptr->result)
     ||(remote_rsp_apdu_ptr->rsp_apdu_len < UIM_REMOTE_SW1_SW2_LENGTH) )
  {
#ifdef FEATURE_UIM_USB_UICC
    if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,
                   "Invalid resp received setting command resp time out sig");
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
    }
    else
#endif /* FEATURE_UIM_USB_UICC */
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id, "Invalid resp received setting TRANSACTION SIG");
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    }
     return;
  }

  rsp_apdu_ptr  = remote_rsp_apdu_ptr->rsp_apdu_ptr;
  rsp_apdu_size = remote_rsp_apdu_ptr->rsp_apdu_len;

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                               uim_ptr) == TRUE)
  {
    /* add INS byte which has been dealed with in the ICCID layer,
       QCAT need this byte to parse */
    if( rsp_apdu_size > 2 )
    {
      uim_log_put_byte(UIM_LOG_RX_DATA, uim_ptr->debug.last_sent_apdu.instrn, uim_ptr);
    }
    for(i=0; i<rsp_apdu_size; i++)
    {
      uim_log_put_byte(UIM_LOG_RX_DATA, *(rsp_apdu_ptr+i), uim_ptr);
    }
    uim_log_put_last_byte(uim_ptr);
  }

  /* Determine the status words in the command */
  sw1 = *(rsp_apdu_ptr + rsp_apdu_size -2);
  sw2 = *(rsp_apdu_ptr + rsp_apdu_size -1);
  rsp_apdu_size = rsp_apdu_size - UIM_REMOTE_SW1_SW2_LENGTH;

  if((UIM_INSTRN_CASE_1 == remote_data_ptr->cmd_ptr->instrn_case)
      || (UIM_INSTRN_CASE_3 == remote_data_ptr->cmd_ptr->instrn_case))
  {
    rsp_apdu_size = 0;
  }

  /* Checking if accumulated response data is more than 256 bytes  */
  if (rsp_apdu_size + remote_data_ptr->rsp_buf_offset > UIM_REMOTE_MAX_RAPDU_LENGTH)
  {
    remote_data_ptr->extended_data.extended_data_ptr = (uint8 *) uim_malloc((rsp_apdu_size + remote_data_ptr->rsp_buf_offset));
    if(remote_data_ptr->extended_data.extended_data_ptr == NULL)
    {
      rsp_apdu_size  = UIM_REMOTE_MAX_RAPDU_LENGTH - remote_data_ptr->rsp_buf_offset;
    }
    else
    {
      if(remote_data_ptr->rsp_buf_offset > 0)
      {
        uim_memscpy(remote_data_ptr->extended_data.extended_data_ptr,
            (remote_data_ptr->rsp_buf_offset),
               &(remote_data_ptr->cmd_ptr->rsp_ptr->rsp.data),
                (remote_data_ptr->rsp_buf_offset));
      }
      uim_memscpy(remote_data_ptr->extended_data.extended_data_ptr + remote_data_ptr->rsp_buf_offset,
        rsp_apdu_size,
             rsp_apdu_ptr,
             rsp_apdu_size);

      remote_data_ptr->extended_data.length  = (rsp_apdu_size + remote_data_ptr->rsp_buf_offset);
      remote_data_ptr->extended_data.offset  = 0;
      remote_data_ptr->rsp_buf_offset        = 0;
      remote_data_ptr->extended_data.ext_sw1 = sw1;
      remote_data_ptr->extended_data.ext_sw1 = sw2;
      /* sw1 sw2 as 61 00 to indicate response is pending in eztended buffer */
      remote_data_ptr->cmd_ptr->rsp_ptr->sw1 = 0x61;
      remote_data_ptr->cmd_ptr->rsp_ptr->sw2 = 0x00;
      remote_data_ptr->cmd_ptr->rsp_ptr->cmd_status   = UIM_REMOTE_STATUS;
      remote_data_ptr->cmd_ptr->rsp_ptr->cmd_rsp_size = UIM_REMOTE_MAX_RAPDU_LENGTH;
      uim_command_response_callback(uim_ptr);
      return;
    }
  }

  /* Response data received is less than 256 bytes */
  if(rsp_apdu_size)
  {
    uim_memscpy(&(remote_data_ptr->cmd_ptr->rsp_ptr->rsp) + remote_data_ptr->rsp_buf_offset,
        rsp_apdu_size,
                rsp_apdu_ptr,
                rsp_apdu_size );
  }

  remote_data_ptr->cmd_ptr->rsp_ptr->cmd_status   = UIM_REMOTE_STATUS;
  remote_data_ptr->rsp_buf_offset = remote_data_ptr->rsp_buf_offset + rsp_apdu_size;
  remote_data_ptr->cmd_ptr->rsp_ptr->cmd_rsp_size = remote_data_ptr->rsp_buf_offset;
  remote_data_ptr->cmd_ptr->rsp_ptr->sw1          = sw1;
  remote_data_ptr->cmd_ptr->rsp_ptr->sw2          = sw2;
  uim_command_response_callback(uim_ptr);

  return;
}/* uim_remote_apdu_response_cb */

/*===========================================================================
FUNCTION UIM_REMOTE_GET_COMMAND_STATUS

DESCRIPTION
  This procedure updates the command status based on sw1 sw2 from remote card.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_remote_get_command_status(uim_instance_global_type *uim_ptr)
{
  uint16                 index              = 0;
  boolean                table_match_found  = FALSE;
  uim_apdu_status_type   cmd_status         = UIM_MAXIMUM_STATUS;
  uim_global_remote_type  *remote_data_ptr    = &(uim_ptr->remote);
  uint8                  sw1                = remote_data_ptr->cmd_ptr->rsp_ptr->sw1;
  uint8                  sw2                = remote_data_ptr->cmd_ptr->rsp_ptr->sw2;

  for(index = 0; index < (sizeof(uim_remote_sw_data_table)/sizeof(uim_remote_sw_data_type)); index++)
  {
    /* Match for sw1 */
    if(sw1 == uim_remote_sw_data_table[index].sw1)
    {
      /* Match for sw2 OR check for default process flag */
      if( (sw2 == uim_remote_sw_data_table[index].sw2) ||
          (uim_remote_sw_data_table[index].def_sw_proc)
        )
      {
        /* If match found then use the SW function handler */
        cmd_status = uim_remote_sw_data_table[index].cmd_status;
        table_match_found = TRUE;
        break;
      }
    }
  }

  /* If no match found for given SW1 and SW2 then send an error to UIM server */
  if(table_match_found == FALSE)
  {
    cmd_status = UIM_PROBLEM;
  }

  if((0x63 == sw1) || (cmd_status == UIM_MAXIMUM_STATUS))
  {
    /* SW1 - Warnings 2 SW2 - Success After X tries */
    if ((sw2 & 0xF0) == (uint8)SW2_CMD_SUCCESS_INT_X_RETRIES)
    {
      cmd_status = UIM_CMD_SUCCESS_WITH_X_RETRIES;
    }
    else
    {
      cmd_status = UIM_PARITY;
    }
  }

  remote_data_ptr->cmd_ptr->rsp_ptr->cmd_status = cmd_status;
  return;
}/* uim_remote_get_command_status */

/*===========================================================================
FUNCTION UIM_REMOTE_SEND_ATR

DESCRIPTION
  This procedure stores the ATR data of the remote sim in the appropriate buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_remote_send_atr(uim_instance_global_type *uim_ptr, const qmi_uim_remote_card_atr_data_type *atr_data_ptr)
{
  uint8   atr_len        = 0;
  uint8   num_hist_chars = 0;

  ASSERT(uim_ptr);
  if(atr_data_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id," ATR data pointer is NULL");
    return;
  }

  atr_len                        = (uint8)(atr_data_ptr->data_len);

  if(UIM_MAX_ATR_CHARS < atr_len)
  {
    atr_len = UIM_MAX_ATR_CHARS;
  }
  uim_ptr->atr.atr_buf.num_bytes = atr_len;

  uim_memscpy(uim_ptr->atr.atr_buf.data,atr_len,atr_data_ptr->data_ptr, atr_len);
  num_hist_chars = (uim_ptr->atr.atr_buf.data[UIM_ATR_TS_BYTE_INDEX]  & UIM_K_FIELD);

  /*discard the historical characters if atr_len is less that num_hist_chars*/
  if(num_hist_chars < uim_ptr->atr.atr_buf.num_bytes)
  {
    /* Copy the historical characters to the historical char  buffer. */
    uim_ptr->atr.hist_char_buf.length = num_hist_chars;
    uim_memscpy (uim_ptr->atr.hist_char_buf.data,num_hist_chars,
        (&(uim_ptr->atr.atr_buf.data[(atr_len - num_hist_chars)])),
         num_hist_chars);
  }

  uim_ptr->command.rsp_buf.cmd_status         = UIM_DONE;
  uim_ptr->remote.state                       = WAIT_FOR_NOTHING;
  /* Set the flag to TRUE */
  uim_ptr->atr.atr_pps_done                   = TRUE;
  (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
  return;
} /* uim_remote_send_atr */

#ifdef FEATURE_UIM_BTSAP_CLIENT
/*===========================================================================
 FUNCTION:      UIM_REMOTE_CARD_POWER_DOWN

 DESCRIPTION:
     This function powers down the remote SIM card.

 DEPENDENCIES
  None

 RETURN VALUE
  None

 SIDE EFFECTS
  None
=============================================================================*/
void uim_remote_card_power_down(uim_instance_global_type *uim_ptr)
{
  qmi_uim_remote_result_type result     = QMI_UIM_REMOTE_ERROR;

  ASSERT(uim_ptr);
  if(uim_ptr->remote.btsap.handle & UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Connection Unavailable event triggered.No need to call powerdown API");
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }

  result =   qmi_uim_remote_card_power_down((qmi_uim_remote_slot_type)uim_ptr->id,
                                             QMI_UIM_REMOTE_POWER_DOWN_CARD);

  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Could powerdown the remote card");
  }
  return;
}/* uim_remote_card_power_down */

/*===========================================================================
 FUNCTION:      UIM_REMOTE_CARD_CONNECT

 DESCRIPTION:
    This function initiates the establishment of a connection with the remote
    SIM and powers it up.

 DEPENDENCIES
  None

 RETURN VALUE
  None

 SIDE EFFECTS
  None
=============================================================================*/
void uim_remote_card_connect(uim_instance_global_type *uim_ptr)
{
  qmi_uim_remote_result_type result  = QMI_UIM_REMOTE_ERROR;

  ASSERT(uim_ptr);

  if(uim_ptr->remote.btsap.handle & UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK)
  {
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }

  uim_ptr->remote.btsap.handle = (uim_ptr->remote.btsap.handle | UIM_BTSAP_CARD_CONNCECTTION_ENABLE_MASK) ;
  uim_ptr->remote.btsap.handle = (uim_ptr->remote.btsap.handle & UIM_BTSAP_DISABLE_RECOVERY_MASK);
  uim_ptr->remote.state  = WAIT_FOR_ATR;
  /* Set the transaction timer prior to connecting to remote card.
     Clear it once ATR is received */
  (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
            uim_btsap_get_work_waiting_time(uim_ptr) );
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Sending the qmi remote connect API with timer set to 0x%x",
                             uim_btsap_get_work_waiting_time(uim_ptr));
  result = qmi_uim_remote_card_connect((qmi_uim_remote_slot_type)uim_ptr->id);

  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Could connect to the remote card");
    (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
  }
  return;
}/* uim_remote_card_connect */

/*===========================================================================
 FUNCTION:      UIM_REMOTE_CARD_POWER_UP

 DESCRIPTION:
  This function powers up the remote SIM card.

 DEPENDENCIES
  None

 RETURN VALUE
  None

 SIDE EFFECTS
  None
=============================================================================*/
void uim_remote_card_power_up(uim_instance_global_type *uim_ptr)
{
  qmi_uim_remote_result_type result  = QMI_UIM_REMOTE_ERROR;

  ASSERT(uim_ptr);

  if(uim_ptr->remote.btsap.handle & UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK)
  {
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }
  uim_ptr->remote.state  = WAIT_FOR_ATR;
  uim_ptr->remote.btsap.handle = (uim_ptr->remote.btsap.handle & UIM_BTSAP_DISABLE_RECOVERY_MASK);

  /* Set the transaction timer prior to power-up the remote card.
     Clear it once ATR is received */
  (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
                   uim_btsap_get_work_waiting_time(uim_ptr) );

  result = qmi_uim_remote_card_power_up((qmi_uim_remote_slot_type)uim_ptr->id,
                                        FALSE,
                                        0,
                                        FALSE,
                                        QMI_UIM_REMOTE_VOLTAGE_CLASS_NOT_APPLICABLE);
  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Couldn't powerup the remote card");
    /* Clear the transaction timer */
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
  }
  return;
}/* uim_remote_card_power_up */

/*===========================================================================
 FUNCTION:      UIM_REMOTE_CARD_RESET

 DESCRIPTION:
     This function can be used by the calling task to perform a warm reset
     of the remote SIM card.

 DEPENDENCIES
  None

 RETURN VALUE
  None

 SIDE EFFECTS
  None
=============================================================================*/
void uim_remote_card_reset(uim_instance_global_type *uim_ptr)
{
  qmi_uim_remote_result_type result = QMI_UIM_REMOTE_ERROR ;

  ASSERT(uim_ptr);
  if(uim_ptr->remote.btsap.handle & UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK)
  {
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }
  uim_ptr->remote.state  = WAIT_FOR_ATR;
  uim_ptr->remote.btsap.handle = (uim_ptr->remote.btsap.handle & UIM_BTSAP_DISABLE_RECOVERY_MASK);
  /* Set the transaction timer prior to power-up the remote card.
     Clear it once ATR is received */
  (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
                   uim_btsap_get_work_waiting_time(uim_ptr) );
  result = qmi_uim_remote_card_reset((qmi_uim_remote_slot_type)uim_ptr->id);
  if(result != QMI_UIM_REMOTE_SUCCESS)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Could reset the remote card");
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
  }
  return;
}/* uim_remote_card_reset */
#endif /* FEATURE_UIM_BTSAP_CLIENT */
/*===========================================================================
FUNCTION UIM_REMOTE_SEND_COMMAND

DESCRIPTION:
 This function transmits an APDU from the calling task to the remote SIM
 card. For each APDU, the calling task can indicate a response callback,
 used to receive the response from the SIM card.

LIMITATIONS:
 None

RETURN VALUE:
 void

SIDE EFFECTS:
 None.
===========================================================================*/
void uim_remote_send_command(uim_req_buf_type *uim_req_ptr , uim_instance_global_type *uim_ptr )
{
  uint16_t  cmd_apdu_size     = 0;
  uint8     *cmd_apdu_data_ptr    = NULL;
  uim_global_remote_type  *remote_data_ptr = NULL;
  uint16    i                     = 0;

  ASSERT(uim_ptr);
  remote_data_ptr = &(uim_ptr->remote);

  if(
#ifdef FEATURE_UIM_BTSAP_CLIENT
    !IS_BTSAP_HANDLE_ACTIVE(uim_ptr) ||
#endif /*FEATURE_UIM_BTSAP_CLIENT*/
    (uim_req_ptr == NULL))
  {
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }

#ifdef FEATURE_UIM_USB_UICC
  if( !(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr)))
  {
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_BTSAP_CLIENT
  if(remote_data_ptr->btsap.handle & UIM_BTSAP_CARD_CONN_UNAVAIL_ENABLE_MASK)
  {
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }
  if (remote_data_ptr->btsap.handle & UIM_BTSAP_TRIGGER_RECOVERY_MASK)
  {
    remote_data_ptr->btsap.handle = (remote_data_ptr->btsap.handle & UIM_BTSAP_DISABLE_RECOVERY_MASK);
    if(uim_ptr->command.mode != UIM_RECOVERY_MODE)
    {
      (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
      /* Set the command response timeout signal  */
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
      return;
    }
  }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

  /* Copy the contents of APDU header into last sent APDU header */
  uim_ptr->debug.last_sent_apdu = uim_req_ptr->apdu_hdr;

  /* Based on the instruction case determine the size of the
     command APDU to be sent to the card */
  switch (uim_req_ptr->instrn_case)
  {
    case UIM_INSTRN_CASE_1:  /* Case 1 - No data exchanged */
    case UIM_INSTRN_CASE_2:  /* Case 2 - Response data only */
    {
      cmd_apdu_size = UIM_REMOTE_APDU_HDR_SIZE;
      break;
    }

    case UIM_INSTRN_CASE_3:  /* Case 3 - Command data only */
    case UIM_INSTRN_CASE_4:  /* Case 4 - Response and command data */
    {
      cmd_apdu_size = UIM_REMOTE_APDU_HDR_SIZE + uim_req_ptr->apdu_hdr.p3;
      break;
    }
    default:
    {
      /* The instruction case is not valid. Cannot process command.
         Create a failure through the time out mechanism. */
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Err instrn case=0x%x.",
                                  uim_req_ptr->instrn_case);
      /* Clear the cmd_rsp_timer first */
      (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
      /* Set the command response timeout signal */
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
      return;
    }
  }

    /* Allocate memory based on computed size */
  cmd_apdu_data_ptr = (uint8 *)uim_malloc(cmd_apdu_size);
  if (cmd_apdu_data_ptr == NULL)
  {
    /* Clear the cmd_rsp_timer first */
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
    return;
  }

  /* Setup the command APDU to be sent to the card */
  *cmd_apdu_data_ptr       = uim_req_ptr->apdu_hdr.uim_class;
  *(cmd_apdu_data_ptr + 1) = uim_req_ptr->apdu_hdr.instrn;
  *(cmd_apdu_data_ptr + 2) = uim_req_ptr->apdu_hdr.p1;
  *(cmd_apdu_data_ptr + 3) = uim_req_ptr->apdu_hdr.p2;
  *(cmd_apdu_data_ptr + 4) = uim_req_ptr->apdu_hdr.p3;

  /* If the command APDU contains command data copy it into
       the command APDU buffer prior to sending it to the card */
  if (cmd_apdu_size > UIM_REMOTE_APDU_HDR_SIZE)
  {
    uim_memscpy((cmd_apdu_data_ptr + UIM_REMOTE_APDU_HDR_SIZE),
                (cmd_apdu_size - UIM_REMOTE_APDU_HDR_SIZE),
                uim_req_ptr->req.data,
                (cmd_apdu_size - UIM_REMOTE_APDU_HDR_SIZE));
  }

  if(uim_req_ptr->apdu_hdr.instrn == GET_RESPONSE)
  {
    if(remote_data_ptr->extended_data.is_get_response_allowed != TRUE)
    {
      uim_req_ptr->rsp_ptr->cmd_status   = UIM_REMOTE_STATUS;
      uim_req_ptr->rsp_ptr->sw1          = 0x61;
      uim_req_ptr->rsp_ptr->sw2          = 0x00;
      /*Data is already cached in extended buffer,so return */
      uim_command_response_callback(uim_ptr);
      return;
    }
  }
  else
  {
    remote_data_ptr->rsp_buf_offset       = 0;
    remote_data_ptr->extended_data.offset = 0;
    remote_data_ptr->extended_data.length = 0;
    remote_data_ptr->cmd_ptr = uim_req_ptr;
  }

  /* Clear the transaction signal prior to setting the timer */
  (void)rex_clr_sigs(uim_ptr->tcb_ptr,UIM_TRANSACTION_SIG);

  /* Set the transaction timer prior to issuing a command */
#ifdef FEATURE_UIM_BTSAP_CLIENT
  if (IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
  {
  (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
                      uim_btsap_get_work_waiting_time(uim_ptr) );
  }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_USB_UICC
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    (void)rex_set_timer(&uim_ptr->remote.remote_cmd_rsp_timer,
                        uim_usb_get_work_waiting_time(uim_ptr) );
  }
#endif /* FEATURE_UIM_USB_UICC */

  remote_data_ptr->state = WAIT_FOR_RESPONSE;

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DEBUG_LOG,
                               uim_ptr) == TRUE)
  {
    for(i=0; i<cmd_apdu_size; i++)
    {
      uim_log_put_byte(UIM_LOG_TX_DATA, *(cmd_apdu_data_ptr+i), uim_ptr);
    }
    uim_log_put_last_byte(uim_ptr);
  }
  qmi_uim_remote_send_apdu((qmi_uim_remote_slot_type)uim_ptr->id, NULL, cmd_apdu_size, cmd_apdu_data_ptr, uim_remote_apdu_response_cb);
  UIM_FREE(cmd_apdu_data_ptr);
  return;
}/* uim_remote_send_command */

/*===========================================================================
FUNCTION UIM_REMOTE_GET_ENTIRE_RESPONSE

DESCRIPTION:
 This function is to process the intermediate status words
 and send GET_RESPONSE command to remote card if needed

LIMITATIONS:
 None

RETURN VALUE:
 boolean.
 TRUE - UIM TASK contiues from the main 'for loop' in UIM_TASK_COMMON
 FALSE - Process the response.

SIDE EFFECTS:
 None.
===========================================================================*/
boolean uim_remote_get_entire_response(uim_instance_global_type *uim_ptr)
{
  uim_req_buf_type      *get_rsp_apdu_ptr  = NULL;
  uim_global_remote_type *remote_data_ptr    = NULL;
  uint8              sw1;
  uint8          sw2;
  uint16                 pending_size = 0;

  ASSERT(uim_ptr);
  remote_data_ptr = &(uim_ptr->remote);

  sw1 =   remote_data_ptr->cmd_ptr->rsp_ptr->sw1;
  sw2 =   remote_data_ptr->cmd_ptr->rsp_ptr->sw2;

  if((SW1_USIM_RESEND_APDU == sw1))
  {
    /* Send last command , replace P3 byte by SW2 */
    remote_data_ptr->cmd_ptr->apdu_hdr.p3 = remote_data_ptr->cmd_ptr->rsp_ptr->sw2;
    uim_remote_send_command(remote_data_ptr->cmd_ptr , uim_ptr );
    return TRUE;
  }

  /* This condition is to check if response data was less than 256 and
     hence we didnot use the extended data buffer */
  if(remote_data_ptr->extended_data.extended_data_ptr == NULL)
  {
    remote_data_ptr->extended_data.is_get_response_allowed = TRUE;
    get_rsp_apdu_ptr = (uim_req_buf_type *)uim_malloc(sizeof(uim_req_buf_type));
    if (get_rsp_apdu_ptr == NULL)
    {
      /* Clear the cmd_rsp_timer first */
      (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
      /* Set the command response timeout signal */
      (void) rex_set_sigs(uim_ptr->tcb_ptr , UIM_TRANSACTION_SIG );
      return TRUE;
    }

    /* Send last command APDU header (and no data), replace P3 byte by SW2 */
    uim_memscpy(get_rsp_apdu_ptr,(sizeof(uim_req_buf_type)),remote_data_ptr->cmd_ptr,(sizeof(uim_req_buf_type)));
    get_rsp_apdu_ptr->apdu_hdr.instrn   = GET_RESPONSE;
    get_rsp_apdu_ptr->instrn_case       = UIM_INSTRN_CASE_2;
    get_rsp_apdu_ptr->apdu_hdr.p1   = 0x00;
    get_rsp_apdu_ptr->apdu_hdr.p2   = 0x00;
    if((sw1 == SW1_WARNINGS1) || (sw1 == SW1_WARNINGS2))
    {
      get_rsp_apdu_ptr->apdu_hdr.p3 = 0x00;
    }
    else
    {
      get_rsp_apdu_ptr->apdu_hdr.p3 = sw2;
    }
    UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Received get response command. Issuing new command");
    uim_remote_send_command(get_rsp_apdu_ptr , uim_ptr);
    UIM_FREE(get_rsp_apdu_ptr);
    return TRUE;
  }
  else /* Response data was greater than 256 and hence we used extended data buffer to store it */
  {
    /* Since data is already cached in extended buffer,we dont send next GET_RESPONSE cmd to remote card*/
    remote_data_ptr->extended_data.is_get_response_allowed = FALSE;
    pending_size  = (remote_data_ptr->extended_data.length - remote_data_ptr->extended_data.offset);
    if(pending_size > 256)
    {
      uim_ptr->command.rsp_buf.sw1              = 0x61;
      uim_ptr->command.rsp_buf.sw2              = 0x00;
      uim_ptr->command.rsp_buf.cmd_status   = UIM_DONE;
      uim_memscpy((uim_ptr->command.rsp_buf.rsp.data),UIM_REMOTE_MAX_RAPDU_LENGTH,
            ((remote_data_ptr->extended_data.extended_data_ptr) + (remote_data_ptr->extended_data.offset)),
             (UIM_REMOTE_MAX_RAPDU_LENGTH ));
      remote_data_ptr->extended_data.offset = remote_data_ptr->extended_data.offset + UIM_REMOTE_MAX_RAPDU_LENGTH;
    }
    else
    {
      uim_memscpy(&(remote_data_ptr->cmd_ptr->rsp_ptr->rsp.data),pending_size,
              ((remote_data_ptr->extended_data.extended_data_ptr) + (remote_data_ptr->extended_data.offset)),
              (pending_size));
      uim_ptr->command.rsp_buf.sw1 = remote_data_ptr->extended_data.ext_sw1;
      uim_ptr->command.rsp_buf.sw2 = remote_data_ptr->extended_data.ext_sw2;
      uim_ptr->command.rsp_buf.cmd_status  = UIM_DONE;
      remote_data_ptr->rsp_buf_offset       = 0;
      remote_data_ptr->extended_data.offset = 0;
      remote_data_ptr->extended_data.length = 0;
      remote_data_ptr->extended_data.is_get_response_allowed = TRUE;
      UIM_FREE(remote_data_ptr->extended_data.extended_data_ptr);
    }
    return FALSE;
  }
}/* uim_remote_get_entire_response */

/*===========================================================================
FUNCTION UIM_BTSAP_CMD_RSP_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the btsap cmd rsp timer. This call-back is called in the context of the high
  priority timer task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Power down and power up the remote SIM interface if the command response timeout expired.
===========================================================================*/
void uim_remote_cmd_rsp_timer_expiry_cb(timer_cb_data_type param)
{
  uim_instance_global_type *uim_ptr = (uim_instance_global_type*)param;

  ASSERT(uim_ptr);
  UIMDRV_MSG_ERR_0(uim_ptr->id, "No response from remote sim for the command");
  /* Clear the cmd_rsp_timer_ptr first */
   (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
  /* Set the command response timeout signal */
#ifdef FEATURE_UIM_USB_UICC
  if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
  }
  else
#endif /* FEATURE_UIM_USB_UICC */
  {
  (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
  }
  return;
} /* uim_remote_cmd_rsp_timer_expiry_cb */
#endif /* FEATURE_UIM_REMOTE_UIM */
