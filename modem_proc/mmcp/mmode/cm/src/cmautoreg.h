#ifndef CM_AUTOREG_H
#define CM_AUTOREG_H

/*===========================================================================

C A L L   M A N A G E R   AUTO REGISTER   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMAUTOREG.C

Copyright (c) 1998 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmautoreg.h#1 $

when          who            what, where, why
--------    ---             ----------------------------------------------------------
06/03/13    nvuchula     Implemented Auto Register Feature

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "queue.h"

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */

#include "cmnv.h"      /* Includes NV services */

#include "sd.h"
#include "sd_v.h"
#include "sd_i.h"

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#include "sys.h"
#include "sys_v.h"
#ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
#include "geran_eng_mode_read_api.h"
#else
#include "rr_cb_if.h"
#endif /*FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
#endif

#include "wms.h"
#include "timer.h"




/* Changes for QRD FRs Begin */
#define CDMAMIN 1
#define CM_AUTO_REG_CT_TELESERVICE_ID 65005


 /* Definitions used by the format registration information command to format
 * a message for transmission. */

#define CHINATEL_SEND_REG_ESN_ID       1
#define CHINATEL_SEND_REG_MEID_ID      2
#define CHINATEL_SEND_REG_MSG_TYPE     3
#define CHINATEL_SEND_ACK_MSG_TYPE     4
#define CHINATEL_ADDRESS_LENGTH        8
#define CHINATEL_ADDRESS_STRING        "1\n6594\n1"


#define CM_AUTO_REG_EFS_CU_IMSI_LEN 8
#define CM_AUTO_REG_EFS_CMCC_IMSI_LEN 8

#define REG_NV_IMSI_MAX_LENGTH 8

#define CM_AUTO_REG_SMS_MAX_RETRY 3

/* State machine for the auto registration subsystem. */
typedef enum cm_auto_reg_cmds_e
{
  /* The system has not been started. */
  CM_AUTO_REG_INFO_CLEAR_CMD = 0x00,
  /* The system is ready to be checked. */
  CM_AUTO_REG_INFO_CHECK_CMD = 0x01,
  /* The system has been checked and a timer has been started to send the 
  * registration message in 60 seconds. */
  CM_AUTO_REG_INFO_WAIT_CMD = 0x02,
  /* The timer has elapsed, and a sys info request has been queued 
  * to obtain information about the registered network. */
  CM_AUTO_REG_INFO_QUEUE_CMD = 0x04,
  /* The sys info has been obtained, and it is clear to send the 
  * registration message. */
  CM_AUTO_REG_INFO_CTS_CMD = 0x08,
  /* The ui task should send the registration message at its earliest
  * convenience. */
  CM_AUTO_REG_INFO_SEND_CMD = 0x10,
  /* The registration message has been sent. Check for an acknowledge
  * message receipt. */
  CM_AUTO_REG_INFO_WAIT_ACK_CMD = 0x20,
  /* The ESN tracking processing is complete. */
  CM_AUTO_REG_INFO_COMPLETE_CMD = 0x40,
  //#ifdef FEATURE_QRD_CU_AUTO_REGISTER_MESSAGE
   /* For CU, The ui task should send the registration message at its earliest
  * convenience. */
  CM_AUTO_REG_INFO_CU_SEND_CMD  = 0x100,
   /* For CU, The registration message has been sent. Check for an acknowledge
  * message receipt. */
  CM_AUTO_REG_INFO_CU_WAIT_ACK_CMD = 0x0200,
  /* For CU, auto register  processing is complete. */
  CM_AUTO_REG_INFO_CU_COMPLETE_CMD = 0x0400,
  //#endif /*FEATURE_QRD_CU_AUTO_REGISTER_MESSAGE*/
  CM_AUTO_REG_INFO_CMCC_SEND_CMD  = 0x0800,
  /* CMCC Auto Register Message */
} cm_auto_reg_cmds_e_type;


/* Variables used by the China Telecom code to determine 
if registration is necessary. 
*/
extern byte  cm_auto_reg_efs_cu_imsi[CM_AUTO_REG_EFS_CU_IMSI_LEN];
extern byte  cm_auto_reg_efs_cmcc_imsi[CM_AUTO_REG_EFS_CU_IMSI_LEN];

/* Default IMSI values for China Telecom */
#define IMSI_MCC_CHINATEL_DEFAULT      359  /* 460 */
#define IMSI_MNC_CHINATEL_DEFAULT      92   /* 03 */

typedef struct undp_feature_configs_type
{

  boolean reg_status;
 
}undp_feature_configs_type;

/* Changes for QRD FRs End */

/* Changes for QRD Features Begin */
/*===========================================================================
FUNCTION cm_auto_reg_send_reg_info_cu_8bit

DESCRIPTION
  Called when send the AUTO_REG_SMS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_info_send_cu_8bit( void );

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_info_cmcc_8bit

DESCRIPTION
  Called when send the AUTO_REG_SMS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_info_send_cmcc_8bit( void );


/*===========================================================================
FUNCTION cm_auto_reg_efs_model_data

DESCRIPTION
  Called to read device model info from EFS.
  
DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

boolean cm_auto_reg_efs_model_data( void );

/*===========================================================================
FUNCTION UIUTILS_GET_IMEI

DESCRIPTION
  This function fills in the imei into the given byte array.  The
  input string must be at least 2*NV_UE_IMEI_SIZE.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

int cm_auto_reg_get_imei(byte imei_ascii[], size_t imei_ascii_size);

/*===========================================================================
FUNCTION cm_auto_reg_assic_to_byte

DESCRIPTION
  Convert an SMS PDU from ASCII hex format to a byte array.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_assic_to_byte
(
  char * hex_pdu,
  byte * byte_pdu,
  uint32 num_hex_chars
);

/*===========================================================================
FUNCTION cm_auto_reg_hex_char_to_byte

DESCRIPTION
 Convert a hex character to a byte

DEPENDENCIES

SIDE EFFECTS
 None
===========================================================================*/

byte cm_auto_reg_hex_char_to_byte
(
  char hex_char
);

/*===========================================================================
FUNCTION cm_auto_reg_ceiling

DESCRIPTION
  Return the celing of divident/divisor.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int cm_auto_reg_ceiling(int divident,int divisor);

/*===========================================================================
FUNCTION cm_auto_reg_receive_msg_cfg_cu_cb

DESCRIPTION
  Called when received wms cfg event.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_cfg_cu_cb
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
);

/*===========================================================================
FUNCTION cm_auto_reg_receive_msg_cfg_cmcc_cb

DESCRIPTION
  Called when received wms cfg event.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_cfg_cmcc_cb
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
);

/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cu_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_msg_cu_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
);

/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cmcc_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

extern void cm_auto_reg_receive_reg_msg_cmcc_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
);


/*===========================================================================
FUNCTION cm_auto_reg_dereg

DESCRIPTION
  Called when a UI to de-register.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_dereg( void );

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cu_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

extern void cm_auto_reg_send_reg_msg_cu_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
);

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cmcc_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

extern void cm_auto_reg_send_reg_msg_cmcc_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
);


/*===========================================================================
FUNCTION   cm_auto_reg_ss_event_mdm_cb

DESCRIPTION
  This function is called when we get an SS event.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Updates LED announciator behavior
===========================================================================*/
extern void cm_auto_reg_ss_event_mdm_cb(cm_ss_event_e_type ss_event,
                      const cm_mm_ss_info_s_type *ii_info_ptr);

/*===========================================================================
FUNCTION cm_auto_reg_format_reg_info

DESCRIPTION
  Called when the SIM initialization has been competed to send a registration
  SMS message, if necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_info_check(void);

/*===========================================================================
FUNCTION cm_auto_reg_read_ruim_items

DESCRIPTION
  Reads needed UI parameters from RUIM to use later to see if registration
  is necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
nv_stat_enum_type cm_auto_reg_read_ruim_items(
  byte nam                            /* NAM to read RUIM items. */
);

/*===========================================================================
FUNCTION cm_auto_reg_read_efs_imsi

DESCRIPTION
  Reads needed UI parameters from NVRAM to use later to see if registration
  is necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_read_efs_imsi( void );

/*===========================================================================
FUNCTION cm_auto_reg_get_local_reg_ind

DESCRIPTION
  Reads local NV item to determine registration status.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_get_local_reg_ind(void);

/*===========================================================================
FUNCTION cm_auto_reg_set_local_reg_ind

DESCRIPTION
  Sets local NV item for local registration indicator.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_set_local_reg_ind(
  boolean local_reg_ind               /* Value of the LRI to set. */
);

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_info

DESCRIPTION
  Called when the client reg with WMS layer.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_info_send(void);

/*===========================================================================
FUNCTION cm_auto_reg_format_reg_msg

DESCRIPTION
  Formats the headers of a WMS message to send to the registration server.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_format_reg_msg(
  wms_client_ts_data_s_type *wms_ts,  /* teleservice data to format */
  uint8 *address,                     /* phone number to send message */
  uint8 digits                        /* number of digits in phone number */
);

/*===========================================================================
FUNCTION cm_auto_reg_format_reg_msg_data

DESCRIPTION
  Formats a registration message with the appropriate MEID/ESN and IMSI 
  fields.
  00: 0x01 ESN 0x02 MEID
  01: 0x03 REG 0x04 ACK
  02: 0x5C length
  03: 0x00 fill                                      MC 11  S      S
                              MEID/ESN               C  12  2      1
  <a1><b1>QC-GOBI2000</b1><b2>XXXXXXXXXXXXXX</b2><b3>NNNnnNNNnnnnnnn</b3>
  <b4>KBH309_C_CH_1.10.B08</b4></a1>XXXXXXXXX
      Software ver info             CRC 32
DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_format_reg_msg_data(
  wms_client_ts_data_s_type *wms_ts   /* teleservice data to format */
);

/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_msg_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
);

/*===========================================================================
FUNCTION cm_auto_reg_msg_parsing_cb

DESCRIPTION
  Called when WMS polls CM to check if it is receipient of the Msg.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_parsing_cb(
    const wms_client_message_s_type *msg_ptr,
    boolean *shared );

/*===========================================================================
FUNCTION cm_auto_reg_msg_cmd_cb

DESCRIPTION
  Called by WMS to inform CM status of Ack sent to Lower Layers

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_cmd_cb(
  wms_cmd_id_e_type        cmd,
  void                     *user_data,
  wms_cmd_err_e_type       cmd_err );

/*===========================================================================
FUNCTION cm_auto_reg_msg_send_ack

DESCRIPTION
  Called by CM to inform ACK to WMS

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_send_ack(
  wms_transaction_id_type   trans_id,
  wms_error_class_e_type    error_class,
  wms_cdma_tl_status_e_type tl_status);

/*===========================================================================
FUNCTION cm_auto_reg_send_sms

DESCRIPTION
  Called when the WMS message should be sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_send_sms(unsigned long data);

/*===========================================================================
FUNCTION cm_auto_reg_complete_reg_info

DESCRIPTION
  Called when the ACK message from the server is received.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_info_complete(void);

/*===========================================================================
FUNCTION cm_auto_reg_write_efs_imsi

DESCRIPTION
  Stores the values read from RUIM into the EFS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
nv_stat_enum_type cm_auto_reg_write_efs_imsi(void);

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_send_reg_msg_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
);

/*===========================================================================
FUNCTION cm_auto_reg_send_msg_timer_cb

DESCRIPTION
  Timer callback function used to transmit registration message.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
extern void cm_auto_reg_cts_msg_timer_cb(
  timer_cb_data_type timer_id         /* timer id for this event */
);

/*===========================================================================
FUNCTION cm_auto_reg_compare_efs_ruim_items

DESCRIPTION
  Compares UI parameters to see if registration is necessary.

RETURN
  False - A registration message does not need to be sent.
  True - A registration message was sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_compare_efs_ruim_items(void);


/*===========================================================================
FUNCTION
  cm_auto_reg_min1_to_str
DESCRIPTION
  Convert a MIN1 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_min1_to_str(dword dwPhoneNo, byte *pszText);


/*===========================================================================
FUNCTION
  cm_auto_reg_min2_to_str
DESCRIPTION
  Convert a MIN2 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_min2_to_str(word wAreaCode, byte *pszText);


/*===========================================================================
FUNCTION
  cm_auto_reg_mcn_to_str
DESCRIPTION
  Convert a MCN/11_12 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_mcn_to_str(byte bMCN, byte *pszText);

/*===========================================================================
FUNCTION handleAutoRegister

DESCRIPTION
  This function checks the prerequisites and triggers auto register sms.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

void handleAutoRegister(unsigned long data);

/*===========================================================================
FUNCTION cm_auto_reg_set_timer

DESCRIPTION
  Sets timer.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_set_timer(uint32 sec,timer_t1_cb_type cb_func);

/*===========================================================================
FUNCTION cm_auto_reg_set_autoreg_sms_sent

DESCRIPTION
  API to set the AUTO REG MSG STATUS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
 extern void cm_auto_reg_set_autoreg_sms_sent(
  boolean status
);

/*===========================================================================
FUNCTION cm_auto_reg_get_autoreg_sms_sent

DESCRIPTION
  API to set the AUTO REG MSG STATUS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
 boolean cm_auto_reg_get_autoreg_sms_sent(
void
);

#endif /*CM_AUTOREG_H*/

