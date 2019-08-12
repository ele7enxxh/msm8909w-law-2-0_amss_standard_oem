
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S 
                  U T I L I T Y )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module is used by SMS AT commands. It mainly contains utility functions
  that are used in executing the sms commands. These functions are used 
  intenally in SMS module.

EXTERNALIZED FUNCTIONS INTERNAL TO ATCOP SMS UNIT
  dsatetsismsi_read_txt_msg
  Function is used to read a text message. Writes the message plus the 
  optional header fields into the result buffer.

  dsatetsismsi_read_pdu_msg
  Function is used to read a pdu message. Writes the message into the result 
  buffer.

  dsatetsismsi_modify_tag
  Function is used to modify the tag of a message. Checks to see if the 
  present tag is "rec unread" if so it changes it to "rec read". 
  Note that we are responsible for changing the tag on mobile terminated 
  messages. WMS takes care of tags for mobile originated messages

  dsatetsismsi_fill_pdu_fields
  This function sets the additional parameters needed when SM is sent to the
  network. It uses the values set in +CSMP command.

  dsatetsismsi_get_tpdu_type
  Calculates the TPDU type based on stat value and message type indicator

  dsatetsismsi_validate_vp
  Validates the format of validity period (<vp>). It checks both relative
  format and absoute format.

  dsatetsismsi_validate_sms_dcs
  Validates the SMS data coding scheme (<dcs>).

  dsatetsismsi_get_sca
  Get the string set by the +CSCA command and converts it into a 
  address type data structure.

  dsatetsismsi_encode_pdu
  Takes a Hex stream (in ASCII format) and converts it to stream of octets.

  dsatetsismsi_encode_data
  Takes a ASCII streamand converts it according to the selected data coding
  scheme

  dsatetsismsi_decode_data
  Takes a buffer of octets and converts it into an ASCII buffer of GSM 7 bit
  or Hex characters

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2002 - 2012,2014-2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsismsu.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/15   skc     Fixed KW Issues
03/11/14   tk      Optimized debug macros usage in ATCoP.
05/18/12   tk      Migrated to MSG 2.0 macros
01/19/12   sk      Feature cleanup.
10/25/10   ad      Init Changes for DSDS. 
08/30/10   ad      Fixed CSMP issue. 
05/10/10   kk      Updating pres_cmd from to struct to store more data.
09/10/09   nc      Fixed Klocwork errors. 
06/04/09   nc      Fixed the +CSCA updation during card power 
                   down/up scenarios.
05/26/09   nc      Removed the FEATURE_GWSMS_UDH specific code.
03/11/09   nc      Modified the prototype of 'dsatetsismsi_addr_to_str' 
                   to include the length of output ptr.
02/02/09   bs      Added support for extended IRA table.
12/12/08   ua      Fixed Off target lint errors.
11/13/08   nc      Mocving functionality of dsatetsismsi_timestamp() 
                   to dsatsmsi_timestamp()
09/25/08   bs      Fixed +CMGL command.
09/17/08   bs      Removing WMS macro depedancy.
08/27/08   nc      Fixing compilation issues.
07/23/08   bs      Fixed +CMGL command for reading large SMS.
07/03/08   nc      Fixed USSD broken charecter set problems
11/04/07   pp      Lint low - fixes.
04/26/07   sa      Fixed lint medium errors.
03/10/06   pkp     Fixed Klocwork (NULL dereference) errors.
01/29/06   ua      Added support for GSM 7bit '@' character (0x00).
                   Extended support for 0x00 - 0x7F  charactes in SMS
02/15/07   ua      Fixing compilation issue. 
02/15/07   ph      lint cleanup
02/13/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/13/07   pkp     Fixed RVCT compiler warnings.
01/30/07   ss      Changed prototypes for dsatetsismsi_read_txt_msg and 
                   dsatetsismsi_read_pdu_msg
10/18/05   snb     Lint cleanup
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
02/13/05   dvp     Added commands ^SMG[RL].
11/18/04   snb     Add support for additional +CSCS character sets.
06/03/04   snb     Changed wms_gw_dcs_s_type field name to match change in 
                   WMS definition.
02/24/04   snb     Changed argument types in support of SMS flow control.
11/28/03   sb      CDMA SMS changes
10/10/03   snb     Correction for case when +CMGW given with no parms
10/03/03   snb     Support for 8 bit and UCS2 alphabets, changes to +CSMP 
                   parameter validation
09/09/03   sb      Fix for ECR33797 and CR34269: Do not expect an ACK for an
                   message with WMS_GW_MSG_WAITING_DISCARD
08/24/03   sb      Cell broadcast editions (first cut)
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
04/08/03   sb      Status-report and command support
02/11/03   wx      Add functions for domain preference settings
01/07/02   sb      Fixed ECR 26432 : atcop sms: handle +cmgs when text mode, 
                   +csmp not set
09/26/02   sb      Code review changes
09/09/02   ar      Move dial character validation to utility function
                   dsatutil_validate_dial_char().
02/07/02   sb      Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_ETSI_SMS
/* this file should be included only for SMS in GSM or WCDMA mode */
#include "dsati.h"
#include "dsatetsismsi.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "msg.h"
#include "err.h"

#include <stringl/stringl.h>
#include "stdio.h"

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for external variables 
  that are declared else where in ATCOP SMS SUB SYSTEM
===========================================================================*/
/*--------------------------------------------------------------------------
  import variable from other files in sms module
---------------------------------------------------------------------------*/

/* Variable used in CMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type  dsatsmsi_pres_cmd;

/* Variable to retain destination address for send commands (text mode) */         
extern wms_address_s_type  dsatetsismsi_send_addr;

/* Variable to retain the <fo> of the CMGC command */
extern dsat_num_item_type dsatetsismsi_cmgc_fo;

/* Variable to retain the <pid> of the CMGC command */
extern dsat_num_item_type dsatetsismsi_cmgc_pid;

/* Variable to retain the <ct> of the CMGC command */
extern dsat_num_item_type dsatetsismsi_cmgc_ct;

/* Variable to retain the <mn> of the CMGC command */
extern dsat_num_item_type dsatetsismsi_cmgc_mn;

/* Variables to read name corresponding to phone number in the SMS */
LOCAL boolean is_alpha_present = FALSE;

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.
===========================================================================*/
/* LOCAL CONSTANTS: */

/* Klocwork fix: This macro is used inside dsatetsismsi_read_txt_msg
to check for NULL pointer before dereferencing it */
#define CONDITIONAL_NULL_CHECK(ptr)    \
  if (ptr == NULL)                   \
  {                                    \
    DS_AT_MSG0_ERROR("Unexpected NULL pointer");  \
    return 0;                          \
  }

/* UCS2 character set uses 4 bytes per character 
   MAX sms length is 4*WMS_GW_USER_DATA_MAX+1 */
#define DSAT_MAX_MT_SMS_LEN (641)

/* PROTOTYPES FOR LOCAL FUNCTIONS: */
LOCAL boolean string_to_timestamp
(
 const byte * fo_ptr,          /* Pointer to the <fo>   */
 byte       * vp_ptr,          /* Pointer to the string */
 wms_gw_validity_s_type * vp   /* Pointer to Time Stamp */
);

 

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATETSISMSI_READ_TXT_MSG

DESCRIPTION
  Local function that is used to read a text message. Writes the message 
  plus the optional header fields into the result buffer.

DEPENDENCIES
  None

RETURN VALUE
  Number of characters written into the result buffer

SIDE EFFECTS
  None
===========================================================================*/
unsigned int dsatetsismsi_read_txt_msg
(
 wms_client_ts_data_s_type * tpdu_msg_ptr,    /* Ptr to the TPDU         */
 dsm_item_type *res_buff_ptr,                 /*  Place to put response  */
 sms_msg_status_info_s_type * status_msg_ptr  /* Ptr to msg from WMS     */
)
{
  #define DSAT_MAX_DISPLAY_ALPHA_LEN PBM_TEXT_SIZE_CHARS
  #define UPDATE_USED(res_buff_ptr,buff_len)\
    res_buff_ptr->used = (uint16)(w_ptr - res_buff_ptr->data_ptr);\
    buff_len = res_buff_ptr->size - res_buff_ptr->used;

  byte * w_ptr;
  uint8 type_of_addr = 0;
  boolean is_csca = FALSE;
  const uint8 * raw_data = status_msg_ptr->u.gw_message.raw_ts_data.data;
  dsat_num_item_type  dcs_val;
  wms_gw_dcs_s_type dcs_rec;
  uint16 len = 0; 
  boolean gsm7b_alpha;
  uint16 buff_len = 0;
  unsigned char buffer[DSAT_MAX_MT_SMS_LEN] = {0};
  unsigned char * buf;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  w_ptr = (byte *)res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;
 
  if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
  {
    w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMGL: ");
  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    if ( tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER )
    {
      w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMT: ");
    }
    else
    {
      w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CDS: ");
    }
  }
  else
  {
    w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMGR: ");
  }

  if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
  {
    /* write <index> to the result buffer */
    w_ptr = dsatutil_itoa(
    dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index - 1],w_ptr,10
    );
    *w_ptr++ = ',';
  }
  
  if ( ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL ) || 
       ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGR ) )
  {
    /* write <stat> to the result buffer */
    w_ptr = dsatsmsi_tag_lookup(status_msg_ptr->msg_hdr.tag, 0, w_ptr);
    *w_ptr++ = ',';
  }

  switch ( tpdu_msg_ptr->u.gw_pp.tpdu_type )
  {
    case WMS_TPDU_DELIVER:
    {
      /* number of octets in originating address */
      uint8 tp_oa_len = ((raw_data[1] + 1) >> 1) + 2;
      wms_gw_deliver_s_type * deliver_msg_ptr = 
        &tpdu_msg_ptr->u.gw_pp.u.deliver;      
      /* must be extracted even when not outputting to TE */
      /* so alpha can be determined for decoding */
      dcs_val = raw_data[tp_oa_len + 2];
          
      dsatetsismsi_decode_sms_dcs(&dcs_rec, dcs_val);

      UPDATE_USED(res_buff_ptr,buff_len);

      /* write <oa> to the result buffer */
      w_ptr = dsatetsismsi_addr_to_str
              (deliver_msg_ptr->address, w_ptr, 
                  &type_of_addr, is_csca,buff_len);

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* optional <alpha> */
      *w_ptr++ = ',';

      /* write <scts> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatetsismsi_timestamp
              (deliver_msg_ptr->timestamp, w_ptr);

      CONDITIONAL_NULL_CHECK(w_ptr);

      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSDH_IDX,0,0,NUM_TYPE))
      {
        /* write <tooa> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);

        if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGR || 
            dsatsmsi_pres_cmd.cmd_name == SMS_CNMA)
        {
          /* write <fo> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) raw_data[0], w_ptr, 10);

          /* write <pid> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32)deliver_msg_ptr->pid, w_ptr, 10);

          /* write <dcs> to the result buffer */
          /* get length of TP-OA */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) dcs_val, w_ptr, 10);

          /* write <sca> to the result buffer */
          *w_ptr++ = ',';
          UPDATE_USED(res_buff_ptr,buff_len);
          w_ptr = dsatetsismsi_addr_to_str
                  (
                    status_msg_ptr->u.gw_message.sc_address,
                    w_ptr, 
                    &type_of_addr, 
                    is_csca,
                    buff_len
                  );

          CONDITIONAL_NULL_CHECK(w_ptr);

          /* write <tosca> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);
        }

        /* write <length> to the result buffer: for UCS2 alphabet */
        /* length must be halved to give the number of characters */
        /* in the message body */
        *w_ptr++ = ',';

        len = deliver_msg_ptr->user_data.sm_len;				
        if ( dcs_rec.alphabet == WMS_GW_ALPHABET_UCS2 )
        {
          len = len >> 1;
        }

        w_ptr = dsatutil_itoa((uint32) len, w_ptr, 10); 
      }/* if (dsat_csdh_val)*/

      *w_ptr++ = '\n';

      /* write actual message to result buffer */

      gsm7b_alpha = (dcs_rec.alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT &&
        !tpdu_msg_ptr->u.gw_pp.u.deliver.user_data_header_present);

      res_buff_ptr->used = (uint16)(w_ptr - res_buff_ptr->data_ptr);
      buf = dsatetsismsi_decode_data(
          deliver_msg_ptr->user_data.sm_data,
          (uint8)deliver_msg_ptr->user_data.sm_len,
          gsm7b_alpha,
          buffer);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer,
                             (uint16)(buf - &buffer[0]),
                             DSM_ITEM_POOL(res_buff_ptr),
                             FALSE);
      len = (uint16)(buf - &buffer[0]);
      DS_AT_MSG1_MED("Received SMS length is %d",len);
    }
    break;

    case WMS_TPDU_SUBMIT:
    {
      /* number of octets in destination address */
      uint8 tp_da_len = ((raw_data[2] + 1) >> 1) + 2;
      wms_gw_submit_s_type *submit_msg_ptr = 
        &tpdu_msg_ptr->u.gw_pp.u.submit;
          
      /* must be extracted even when not outputting to TE */
      /* so alpha can be determined for decoding */
      dcs_val = raw_data[tp_da_len + 3];

      dsatetsismsi_decode_sms_dcs(&dcs_rec, dcs_val);
      UPDATE_USED(res_buff_ptr,buff_len);
      /* write <da> to the result buffer */
      w_ptr = dsatetsismsi_addr_to_str(
                                        submit_msg_ptr->address, 
                                        w_ptr, 
                                        &type_of_addr, 
                                        is_csca,
                                        buff_len
                                      );

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* optional <alpha> */
      *w_ptr++ = ',';

      /* If there is a match of the phone number in the phone book
         display <alpha> as the corresponding name */
      if(is_alpha_present)
      {
        *w_ptr++ = '"';
         UPDATE_USED(res_buff_ptr,buff_len);         
        (void) dsatutil_memscpy((void*)w_ptr,
                buff_len,(void*)&me_ss_val->dsat_alpha_text[0],
                MIN(DSAT_MAX_DISPLAY_ALPHA_LEN,strlen(me_ss_val->dsat_alpha_text)));
        is_alpha_present = FALSE;
        w_ptr += strlen(me_ss_val->dsat_alpha_text);
        *w_ptr++ = '"';
      }

      if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
      {
        /* optional <scts> */ 
        *w_ptr++ = ',';
      }

      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSDH_IDX,0,0,NUM_TYPE))
      {
        /* write <toda> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);

        if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGR )
        {
          /* write <fo> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) raw_data[0], w_ptr, 10);

          /* write <pid> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) submit_msg_ptr->pid, w_ptr, 10);

          /* write <dcs> to the result buffer */
          /* get length of TP-OA */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) dcs_val, w_ptr, 10);
          
          /* optional <vp> */
          *w_ptr++ = ',';
          if (submit_msg_ptr->validity.format == WMS_GW_VALIDITY_RELATIVE)
          {
            w_ptr = dsatutil_itoa((uint32) raw_data[tp_da_len + 4], w_ptr, 10);
          }
          else if (submit_msg_ptr->validity.format == 
                   WMS_GW_VALIDITY_ABSOLUTE)
          {
            w_ptr = dsatetsismsi_timestamp(submit_msg_ptr->validity.u.time, 
                                           w_ptr);
          }

          CONDITIONAL_NULL_CHECK(w_ptr);

          /* write <sca> to the result buffer */
          *w_ptr++ = ',';
          UPDATE_USED(res_buff_ptr,buff_len);
          w_ptr = dsatetsismsi_addr_to_str (
                                    status_msg_ptr->u.gw_message.sc_address,
                                    w_ptr,
                                    &type_of_addr,
                                    is_csca,
                                    buff_len
                                    );

          CONDITIONAL_NULL_CHECK(w_ptr);

          /* write <tosca> to the result buffer */
          *w_ptr++ = ',';
          w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);
        }

        /* write <length> to the result buffer: for UCS2 alphabet */
        /* length must be halved to give the number of characters */
        /* in the message body */
        *w_ptr++ = ',';
	      
        len = submit_msg_ptr->user_data.sm_len;
        if ( dcs_rec.alphabet == WMS_GW_ALPHABET_UCS2 )
        {
          len = len >> 1;
        }

        w_ptr = dsatutil_itoa((uint32) len, w_ptr, 10);
      } /* if (dsat_csdh_val)*/

      *w_ptr++ = '\n';

      /* write actual message to result buffer */
      gsm7b_alpha = (dcs_rec.alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT &&
        !tpdu_msg_ptr->u.gw_pp.u.deliver.user_data_header_present);

      res_buff_ptr->used = (uint16)(w_ptr - res_buff_ptr->data_ptr);
      buf = dsatetsismsi_decode_data(
          submit_msg_ptr->user_data.sm_data, 
          (uint8)submit_msg_ptr->user_data.sm_len,
          gsm7b_alpha,
          buffer);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer,
                             (uint16)(buf - &buffer[0]),
                             DSM_ITEM_POOL(res_buff_ptr),
                             FALSE);
      len = (uint16)(buf - &buffer[0]);
      DS_AT_MSG1_MED("Submit SMS length is %d",len);
    }
    break;

    case WMS_TPDU_STATUS_REPORT:
    {
      wms_gw_status_report_s_type * status_report_ptr =
        &tpdu_msg_ptr->u.gw_pp.u.status_report;

      /* write <fo> to the result buffer */
      w_ptr = dsatutil_itoa((uint32) raw_data[0], w_ptr, 10);

      /* write <mr> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatutil_itoa(status_report_ptr->message_reference, w_ptr, 10);

      /* write <ra> to the result buffer */
      *w_ptr++ = ',';
      UPDATE_USED(res_buff_ptr,buff_len);
      w_ptr = dsatetsismsi_addr_to_str(
                                       status_report_ptr->address, 
                                       w_ptr,
                                       &type_of_addr,
                                       is_csca,
                                       buff_len
                                      );

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* write <tora> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);

      /* write <scts> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatetsismsi_timestamp(status_report_ptr->timestamp, w_ptr);

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* write <dt> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatetsismsi_timestamp(status_report_ptr->discharge_time, w_ptr);

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* write <st> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatutil_itoa((uint32) status_report_ptr->tp_status, w_ptr, 10);
      res_buff_ptr->used = (uint16)(w_ptr - res_buff_ptr->data_ptr);
    }
    break;

    case WMS_TPDU_COMMAND:
    {
      wms_gw_command_s_type * command_msg_ptr =
        &tpdu_msg_ptr->u.gw_pp.u.command;

      /* write <fo> to the result buffer */
      w_ptr = dsatutil_itoa((uint32) raw_data[0], w_ptr, 10);

      /* write <ct> to the result buffer */
      *w_ptr++ = ',';
      w_ptr = dsatutil_itoa((uint32) command_msg_ptr->command, w_ptr, 10);

      if(((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSDH_IDX,0,0,NUM_TYPE)) 
                                    && (dsatsmsi_pres_cmd.cmd_name == SMS_CMGR))
      {
        /* write <pid> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa((uint32) command_msg_ptr->pid, w_ptr, 10);

        /* write <mn> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa(command_msg_ptr->message_number, w_ptr, 10);

        /* write <da> to the result buffer */
        *w_ptr++ = ',';
        UPDATE_USED(res_buff_ptr,buff_len);
        w_ptr = dsatetsismsi_addr_to_str(
                                         command_msg_ptr->address, 
                                         w_ptr,
                                         &type_of_addr,
                                         is_csca,
                                         buff_len
                                         );

        CONDITIONAL_NULL_CHECK(w_ptr);

        /* write <toda> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa((uint32) type_of_addr, w_ptr, 10);

        /* write <length> to the result buffer */
        *w_ptr++ = ',';
        w_ptr = dsatutil_itoa((uint32) command_msg_ptr->command_data_len, 
                              w_ptr, 10);
        *w_ptr++ = '\n';

        /* write <cdata> to the result buffer */
        w_ptr = dsatetsismsi_decode_data(
          command_msg_ptr->command_data, 
          command_msg_ptr->command_data_len,
          FALSE,
          w_ptr);
        UPDATE_USED(res_buff_ptr,buff_len);
      }
    }
    break;

    default:
      break;
  }

  CONDITIONAL_NULL_CHECK(w_ptr);

  return (dsm_length_packet(res_buff_ptr));
}/* dsatetsismsi_read_txt_msg */

/*===========================================================================

FUNCTION DSATETSISMSI_READ_PDU_MSG

DESCRIPTION
  Local function that is used to read a pdu message. Writes the message 
  into the result buffer.

DEPENDENCIES
  None

RETURN VALUE
  Number of characters written into the result buffer

SIDE EFFECTS
  None
===========================================================================*/
unsigned int dsatetsismsi_read_pdu_msg
(
 const sms_msg_status_info_s_type * msg_ptr, /* Ptr to msg from WMS      */
 dsm_item_type *res_buff_ptr,           /*  Place to put response       */
 const wms_client_ts_data_s_type  * tpdu_msg_ptr  /* Ptr to the TPDU        */
)
{
  byte * w_ptr;
  uint32 i = 0;
  uint16 buff_len = 0;
  wms_address_s_type address;
  uint8 type_of_addr = 0;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  w_ptr = (byte *)res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;
  memset(&address,0,sizeof(address));

  if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
  {
    w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMGL: ");
  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    if ( msg_ptr->u.gw_message.raw_ts_data.tpdu_type == WMS_TPDU_DELIVER )
    {
      w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMT: ");
    }
    else
    {
      w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CDS: ");
    }
  }
  else
  {
    w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMGR: ");
  }

  if ( tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_DELIVER )
  {
    address = tpdu_msg_ptr->u.gw_pp.u.deliver.address;
  }
  else if ( tpdu_msg_ptr->u.gw_pp.tpdu_type == WMS_TPDU_SUBMIT)
  {
    address = tpdu_msg_ptr->u.gw_pp.u.submit.address;
  }
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL || dsatsmsi_pres_cmd.cmd_name == SMS_CMGR ||
       dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    if( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
    {
      w_ptr = dsatutil_itoa(
      dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index-1],w_ptr,10
      );
      *w_ptr++ = ',';
    }

    if( dsatsmsi_pres_cmd.cmd_name != SMS_CNMA )
    {
      w_ptr = dsatsmsi_tag_lookup(msg_ptr->msg_hdr.tag, 1, w_ptr);
      *w_ptr++ = ',';
    }

    if (msg_ptr->u.gw_message.raw_ts_data.tpdu_type != WMS_TPDU_STATUS_REPORT)
    {
      byte temp_w_ptr[50]; /* temporary variable */
      /* <ALPHA> does not apply to WMS_TPDU_STATUS_REPORT */

      /* write <oa> to the result buffer */
      memset((void *)temp_w_ptr, 0x0, sizeof(temp_w_ptr));

      (void) dsatetsismsi_addr_to_str
              (address, temp_w_ptr, &type_of_addr, FALSE,sizeof(temp_w_ptr) );

      CONDITIONAL_NULL_CHECK(w_ptr);

      /* If there is a match of the phone number in the phone book
         display <alpha> as the corresponding name.
         Convert the name into HEX for PDU mode 
      */
      if(is_alpha_present)
      {
        uint8 len = strlen(me_ss_val->dsat_alpha_text);
        for( i=0; i<len; i++)
        {
          if( me_ss_val->dsat_alpha_text[i] < 16 )
          {
            *w_ptr++ = '0';
          }
          w_ptr = dsatutil_itoa((uint32) me_ss_val->dsat_alpha_text[i], w_ptr, 16 );
        }
        is_alpha_present = FALSE;
      }
      *w_ptr++ = ',';
    }

    w_ptr = dsatutil_itoa((uint32)msg_ptr->u.gw_message.raw_ts_data.len,
                          w_ptr,10);
    *w_ptr++ = '\n';
  } /* if ( SMS_CMGL || SMS_CMGR || SMS_CNMA ) */

  w_ptr = 
    dsatetsismsi_scaddr_to_string ( msg_ptr->u.gw_message.sc_address, w_ptr );

  for( i=0; i<msg_ptr->u.gw_message.raw_ts_data.len; i++)
  {
    if( msg_ptr->u.gw_message.raw_ts_data.data[i] < 16 )
    {
      *w_ptr++ = '0';
    }

    w_ptr = dsatutil_itoa(
      (uint32) msg_ptr->u.gw_message.raw_ts_data.data[i], 
      w_ptr, 
      16
      );
  }
  res_buff_ptr->used = (uint16)(w_ptr - res_buff_ptr->data_ptr);
  return res_buff_ptr->used;
}/* dsatetsismsi_read_pdu_msg */

/*===========================================================================

FUNCTION    DSATETSISMSI_FILL_PDU_FIELDS

DESCRIPTION
  This function sets the additional parameters needed when SM is sent to the
  network. It uses the values set in +CSMP command.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatetsismsi_fill_pdu_fields
( 
 wms_gw_pp_ts_data_s_type * msg_ptr  /* TPDU data pointer */ 
)
{
  wms_gw_validity_s_type validity;
  byte default_fo[] = "17";
  byte *temp_fo;
  byte *temp_vp;
  dsat_num_item_type fo;
  dsat_num_item_type temp_val =0;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  memset( &validity, 0, sizeof(wms_gw_validity_s_type) );

  /* get +CSMP's current <fo> value or use a default for this tpdu type */
  /* only consider SMS-SUBMIT and -DELIVER cases as -COMMANDs don't */
  /* use +CSMP's <fo> parm */
   temp_fo = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
  if ( *temp_fo == '\0' )
  {
    fo = (msg_ptr->tpdu_type == WMS_TPDU_SUBMIT) ? 17 : 0;
    /* default <fo> as string needed for call to string_to_timestamp() */
    temp_fo = default_fo;
  }
  else
  {
    if (dsatutil_atoi(&fo, temp_fo, 10) != ATOI_OK)
    {
      DS_AT_MSG0_HIGH("Cannot convert +CSMP's <fo> parameter");
      return;
    }         
  }

  if (msg_ptr->tpdu_type == WMS_TPDU_SUBMIT)
  {
    /* set based on <fo> parm */
    msg_ptr->u.submit.reply_path_present       = (fo & 0x80) >> 7;
    msg_ptr->u.submit.user_data_header_present = (fo & 0x40) >> 6;
    msg_ptr->u.submit.status_report_enabled    = (fo & 0x20) >> 5;
    msg_ptr->u.submit.reject_duplicates        = (fo & 0x04) >> 2;
    msg_ptr->u.submit.message_reference = 100;
    msg_ptr->u.submit.address = dsatetsismsi_send_addr;
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
    msg_ptr->u.submit.pid = (wms_pid_e_type)temp_val;

    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
    dsatetsismsi_decode_sms_dcs(&msg_ptr->u.submit.dcs,
			temp_val);

    temp_vp = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,1,MIX_STR_TYPE);
    (void) string_to_timestamp(
                                temp_fo, 
                                temp_vp, 
                                &msg_ptr->u.submit.validity 
                              );
  }
  else if (msg_ptr->tpdu_type == WMS_TPDU_COMMAND)
  {
    msg_ptr->u.command.user_data_header_present = 
                      (dsatetsismsi_cmgc_fo & 0x40) >> 6;
    msg_ptr->u.command.status_report_enabled = 
                      (dsatetsismsi_cmgc_fo & 0x20) >> 5;
    msg_ptr->u.command.message_reference = 100;
    msg_ptr->u.command.pid = (wms_pid_e_type)dsatetsismsi_cmgc_pid;
    msg_ptr->u.command.command = (wms_gw_command_e_type)dsatetsismsi_cmgc_ct;
    msg_ptr->u.command.message_number = dsatetsismsi_cmgc_mn;
    msg_ptr->u.command.address = dsatetsismsi_send_addr;
  }
  else if (msg_ptr->tpdu_type == WMS_TPDU_DELIVER)
  {
    msg_ptr->u.deliver.reply_path_present       = (fo & 0x80) >> 7;
    msg_ptr->u.deliver.user_data_header_present = (fo & 0x40) >> 6;
    msg_ptr->u.deliver.status_report_enabled    = (fo & 0x20) >> 5;
    msg_ptr->u.deliver.more                     = (fo & 0x04) ? FALSE : TRUE;
    msg_ptr->u.deliver.address = dsatetsismsi_send_addr;
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
    msg_ptr->u.deliver.pid = (wms_pid_e_type)temp_val;

    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
    dsatetsismsi_decode_sms_dcs(&msg_ptr->u.deliver.dcs,
			temp_val);

    /* No need for time stamp in DELIVER         */
    /* We already are using struc that is memset */
    /* So will be defaulted to 0s                */
  }
  else
  {
    return;
  }

  /* clean up */
  memset( (void *) &dsatetsismsi_send_addr, 0, sizeof(wms_address_s_type));
  dsatetsismsi_cmgc_fo = 0;
  dsatetsismsi_cmgc_pid = 0;
  dsatetsismsi_cmgc_mn = 0;
  dsatetsismsi_cmgc_ct = 0;

  return;
} /* dsatetsismsi_fill_pdu_fields */

/*===========================================================================

FUNCTION    DSATETSISMSI_GET_TPDU_TYPE

DESCRIPTION
  Calculates the TPDU type based on stat value and message type indicator

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_gw_tpdu_type_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/
wms_gw_tpdu_type_e_type dsatetsismsi_get_tpdu_type
(
 wms_message_tag_e_type tag_type, /* Tag type    */
 uint8 first_octet                /* First octet */
)
{
  byte mti =0;
  wms_gw_tpdu_type_e_type tpdu;

  mti = first_octet & 0x03;

  if ( (tag_type == WMS_TAG_MT_NOT_READ) || (tag_type == WMS_TAG_MT_READ) )
  {
    switch ( mti )
    {
    case 0:
      tpdu = WMS_TPDU_DELIVER;
      break;
    case 1:
      /* tpdu = WMS_TPDU_SUBMIT_REPORT; */
      tpdu = WMS_TPDU_MAX;
      break;
    case 2:
      tpdu = WMS_TPDU_STATUS_REPORT;
      break;
    case 3:
      tpdu = WMS_TPDU_DELIVER;
      break;
    default:
      tpdu = WMS_TPDU_MAX;
      break;
    }
  }
  else if( (tag_type == WMS_TAG_MO_NOT_SENT) || (tag_type == WMS_TAG_MO_SENT) )
  {
    switch ( mti )
    {
    case 0:
      /* tpdu = WMS_TPDU_DELIVER_REPORT; */
      tpdu = WMS_TPDU_MAX;
      break;
    case 1:
      tpdu = WMS_TPDU_SUBMIT;
      break;
    case 2:
      tpdu = WMS_TPDU_COMMAND;
      break;
    default:
      tpdu = WMS_TPDU_MAX;
      break;
    }
  }
  else
  {
    tpdu = WMS_TPDU_DELIVER;
  }

  return tpdu;
} /* dsatetsismsi_get_tpdu_type */

/*===========================================================================

FUNCTION    DSATETSISMSI_VALIDATE_VP

DESCRIPTION
  Validates the format of validity period (<vp>). It checks both relative
  format and absoute format. EVPF is not supported.Also updates the vp with 
  the timestamp upon a valid vp found.

DEPENDENCIES
  None

RETURN VALUE
  Returns an boolean to indicate if <vp> is formatted correctly

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatetsismsi_validate_vp 
(
 const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
 wms_gw_validity_s_type *timestamp_ptr /*  timestamp for relative validity */
)
{
  byte *temp_fo=NULL, *temp_vp=NULL;
  dsat_num_item_type fo;
  boolean fo_flag=FALSE , vp_flag=FALSE;
  boolean result=TRUE;
  byte *temp_fo_val= NULL,*temp_vp_val =NULL;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /* Check to see we are dealing with a NULL arguments */
  /* It is OK to have both <vp> and <fo> NULL          */
  if(VALID_TOKEN(0))
  {
    fo_flag=TRUE;
  }
  if(VALID_TOKEN(1))
  {
    vp_flag=TRUE;
  }
  if ( vp_flag == FALSE && fo_flag == FALSE )
  {
    return TRUE;
  }
  
  temp_fo_val = (byte *)dsatutil_get_val(DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
  /* We have either <vp> or <fo> or even both */
  /* Use previous <fo> value if null parameter     */
  temp_fo = ( fo_flag == TRUE ) ? tok_ptr->arg[0] : temp_fo_val;

  if (dsatutil_atoi(&fo, temp_fo, 10))
  {
    /* We got an error in converting the <fo> */
    return FALSE;
  }
  
  /* if <fo> indicates SMS-SUBMIT */
  if ( (fo & 0x03) == 1 )
  {
    char *empty_str = "";

    temp_vp_val = (byte *)dsatutil_get_val(DSATETSI_EXT_CSMP_IDX,subs_id,1,MIX_STR_TYPE);
    /* if <vpf> of NONE */
    if ( (fo & 0x18) == 0 )
    {
      /* delete +CSMP's current <vp> setting */
      *temp_vp_val = '\0';

      /* use +CSMP cmd line's <vp> */
      temp_vp = (vp_flag == TRUE) ? 
      tok_ptr->arg[1] :
      (byte *)empty_str;
    }
    else
    {
      /* Use previous <vp> value if null parameter */
      temp_vp = (vp_flag == TRUE ) ? tok_ptr->arg[1] : temp_vp_val; 
    }

    if ( !string_to_timestamp (temp_fo, temp_vp, timestamp_ptr ) )
    {
      /* bad timestamp, timestamp provided when <vpf> indicates none */
      /* or <vpf> indicates absolute and no timestamp provided */
      return FALSE;
    }
    
  }
  else
  {
    /* Error if <vp> is given for non SMS-SUBMIT type <fo> */
    if( tok_ptr->args_found >= 2  &&  vp_flag == TRUE )
    {
      return FALSE;
    }

    /* delete +CSMP's current <vp> setting */
    temp_vp_val = (byte *)dsatutil_get_val(DSATETSI_EXT_CSMP_IDX,subs_id,1,MIX_STR_TYPE);
    *temp_vp_val = '\0';
  }

  return result;
}/* dsatetsismsi_validate_vp */

/*===========================================================================

FUNCTION    DSATETSISMSI_VALIDATE_SMS_DCS

DESCRIPTION
  Validates the SMS data coding scheme (<dcs>).

DEPENDENCIES
  None

RETURN VALUE
  Returns an boolean to indicate if <dcs> is right

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatetsismsi_validate_sms_dcs 
(
 const tokens_struct_type *tok_ptr  /* Command tokens from parser */
)
{	
  dsat_num_item_type dcs_val;
  wms_gw_dcs_s_type dcs_rec;        /* dcs structure */
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /* For details about the bit shifting patterns used in this function */
  /* refer to 3G TS 23.038 section 4: SMS Data Coding Scheme           */
  
  /* Use previous value if null parameter */
  if ( *tok_ptr->arg[3] == '\0' )
  {
    dcs_val = (dsat_num_item_type)dsatutil_get_val(
                  DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
  }
  else
  {
    if( dsatutil_atoi(&dcs_val, tok_ptr->arg[3], 10) != ATOI_OK )
    {
      return FALSE;
    }
  }

  dsatetsismsi_decode_sms_dcs(&dcs_rec, dcs_val);
	
  /* allow dcs values indicating reserved coding groups and with reserved */ 
  /* bits set, reject only compressed as UE doesn't know how to */
  if (dcs_rec.is_compressed)
    return FALSE;
  else
    return TRUE;
}/* dsatetsismsi_validate_sms_dcs */

/*===========================================================================

FUNCTION    DSATETSISMSI_GET_SCA

DESCRIPTION
  Get the string set by the +CSCA command and converts it into a 
  address type data structure.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatetsismsi_get_sca
(
 wms_address_s_type *addr, /* Pointer to address structure to be filled */
 sms_mode_e_type sms_mode    /* SMS mode Enum                             */
)
{
  /* Get the SC addr from the CSCA values */ 
  dsat_string_item_type * str_item1;
  dsat_string_item_type * str_item2;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  str_item1 = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,0,MIX_STR_TYPE);
  str_item2 = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,1,MIX_STR_TYPE);
  if ( *str_item1 != '\0' )
  {
    if ( *str_item2 != '\0' )
    {
      /* Type of address if given */
      return ( dsatsmsi_str_to_addr (
                                  addr,
                                  str_item1,
                                  str_item2,
                                  (dsatsms_fmt_mode_e_type)sms_mode
                                 ) 
              );
    }
    else
    {
      /* Type of address if not given */
      return ( dsatsmsi_str_to_addr (
                                  addr,
                                  str_item1,
                                  NULL,
                                  (dsatsms_fmt_mode_e_type)sms_mode
                                  ) 
              );
    }
  }
  else 
  {
    /* ERROR: SC addr not given */
    DS_AT_MSG0_ERROR("Service Center address not given");
    return FALSE;
  }
} /* dsatetsismsi_get_sca */

/*===========================================================================

FUNCTION    DSATETSISMSI_ENCODE_PDU

DESCRIPTION
  Takes a Hex stream (Given in ASCII format) and converts it to stream of 
  octets. Ex. "2A" is converted to 42.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  Possible values are:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsismsi_encode_pdu
(
 byte  * in_ptr,    /* Pointer to the Hex data (in ASCII format)    */ 
 uint8 * out_ptr,   /* Pointer to the result buffer(converted data) */
 int     len        /* Length of the input data pointer (Hex data)  */
)
{
  byte * s;
  byte temp_buf[3];
  int i;
  dsat_num_item_type num_val;
  dsat_result_enum_type result = DSAT_OK;

  s = in_ptr;

  len = len * 2;

  while(len > 0)
  {
    for (i=0; (i<2) && (*s != '\0'); i++)
    {
      temp_buf[i] = *s++;
    }
    temp_buf[i] = '\0';
    if ( dsatutil_atoi(&num_val, temp_buf, 16) != ATOI_OK )
    {
      /* We got a out of range value */
      result = DSAT_ERROR;
    }
    *out_ptr++ = (uint8)num_val;
    len -= i;
  }

  return result;
} /* dsatetsismsi_encode_pdu */

/*===========================================================================

FUNCTION    DSATETSISMSI_ENCODE_DATA

DESCRIPTION
  Takes a string in TE's character set or HEX character pairs and converts it
  according to the <fo> and <dcs> selected by +CSMP command: 
  if <dcs> indicates GSM 7 bit alphabet and <fo> indicates no UDH then 
  TE character set selected by +CSCS is converted to GSM 7 bit alphabet else
  HEX character pairs are converted to octets.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatetsismsi_encode_data 
( 
 wms_gw_user_data_s_type * data_ptr,/* result buffer(converted data)  */
 const byte              * txt_ptr, /* Pointer to the input string    */
 int                       len      /* Length of the input string     */
)
{
#define MAX_GSM_CHARS 160

  dsat_num_item_type fo, dcs;
  wms_gw_dcs_s_type  dcs_rec;       /* dcs structure */
  dsat_num_item_type cscs_val ;
  byte *temp_fo_val = NULL;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  temp_fo_val = (byte *)dsatutil_get_val(
                          DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
  
  /* get current <dcs> and <fo> values, use default for <fo> if not given */
  if ( *temp_fo_val == '\0' || 
       dsatutil_atoi (&fo, temp_fo_val, 10) != ATOI_OK )
  {
    fo = 17;
  }

  dcs = (dsat_num_item_type)dsatutil_get_val(
               DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);

  /* decode <dcs> value to find alphabet in use */
  dsatetsismsi_decode_sms_dcs(&dcs_rec, dcs);

  /* per 3 GPP TS 27.005: */
  /* if <dcs> indicates GSM 7 bit and <fo> indicates UDHI not set TE character
     set text entered is converted to GSM 7 bit. if <dcs> indicates UCS2 or 8 
     bit or <fo> indicates UDHI set, entered text should consist of two 
     character long hexadecimal numbers which ME/TA converts into 8-bit octet 
     (e.g. two characters 2A (IRA 50 and 65) will be converted to an octet with 
     integer value 42) */
     cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
  if ( dcs_rec.alphabet == WMS_GW_ALPHABET_7_BIT_DEFAULT &&
    !(fo & 0x40) )
  {
    switch ( (dsati_chset_type)cscs_val )
    {
      case ALPHA_IRA:
      case ALPHA_GSM:
        data_ptr->sm_len = (uint16)len;
        break;
      case ALPHA_UCS2:
        data_ptr->sm_len = ((uint16)len >> 2);
        break;
      default:
        return FALSE;
    }

    /* no more than 160 GSM 7 bit chars can be packed into 140 octets avail */
    if ( data_ptr->sm_len > MAX_GSM_CHARS )
    {
      return FALSE;
    }

    /* Convert from TE's character set to GSM 7 bit allowing room for 
       terminating NULL and returning FALSE on inconvertible character 
       Convert string to ALPHA_GSM_WITH_AT which replaces the special character 
       usage of GSM @ to its original values 0x00.(if any) apart 
       from conversion from present TE chset to GSM */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if ( dsatutil_convert_chset( (const char *)txt_ptr, 
                                 (dsati_chset_type)cscs_val,
                                 (char *)&data_ptr->sm_data[0],
                                 ALPHA_GSM_WITH_AT,
                                 data_ptr->sm_len + 1,
                                 FALSE ) == NULL )
    {
      DS_AT_MSG0_HIGH("Inconvertible character in SMS text");
      return FALSE;
    }
  }
  else 
  /* dcs_rec.alphabet != WMS_GW_ALPHABET_7_BIT_DEFAULT || */
  /* FEATURE_GWSMS_UDH && fo & 0x40 */
  {
    /* no more than 280 IRA 2 char long hexadecimal numbers can be packed */
    /* into 140 octets avail, must also be an even number of chars */
    if ( len > 280 || len % 2 )
      return FALSE;

    data_ptr->sm_len = (uint16)len >> 1;

    /* Convert from HEX character pairs to octets allowing room for 
       terminating NULL and returning FALSE on inconvertible character */
    if ( dsatutil_convert_chset( (const char *)txt_ptr, 
                                 ALPHA_HEX,
                                 (char *)&data_ptr->sm_data[0],
                                 ALPHA_8BIT, 
                                 data_ptr->sm_len + 1,
                                 FALSE ) == NULL )
    {
      DS_AT_MSG0_HIGH("Inconvertible character in SMS text");
      return FALSE;
    }
  }

  return TRUE;
} /* dsatetsismsi_encode_data */

/*===========================================================================

FUNCTION    DSATETSISMSI_DECODE_DATA

DESCRIPTION
  Takes a buffer of octets and converts it into an ASCII buffer of GSM 7 bit
  or Hex characters
  
DEPENDENCIES
  None

RETURN VALUE
  returns a byte pointer indicating the next unused location in ASCII buffer.

SIDE EFFECTS
  None
===========================================================================*/
byte *dsatetsismsi_decode_data 
( 
 uint8 *octets,  /* input octets */
 uint8 len,      /* length of input octets */
 boolean gsm7b,  /* indicates either GSM7 bit or HEX alphabet */
 byte *ascii     /* output ASCII */
)
{
  uint8 *tmp_oct = octets;
  byte *tmp_asc = ascii;

  if (gsm7b)
  {
    uint16 max_len = len;
    uint8 gsm7_val;
    int i;
    uint8 *gsm_in  = octets;
    uint8 *ira_out = octets;
    dsat_num_item_type cscs_val ;
    /* GSM character set contains NULL
       dsatutil_convert_chset and all of ATCOP uses NULL terminated strings 
       so convert to safer, NULL terminated IRA in place */
    for(i=0;i<len;i++)
    {
      gsm7_val = gsm7_to_ira[*gsm_in++];
      if( gsm7_val != 255 )
      {
        *ira_out++ = (char)gsm7_val;
      }
      else
      {
        *ira_out++ = (char)REPLACEMENT_CHAR;
      }
    }
    *ira_out = '\0';
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    switch ((dsati_chset_type)cscs_val)
    {
      default:
      case ALPHA_IRA:
      case ALPHA_GSM:
        max_len = max_len + 1;
        break;
      case ALPHA_UCS2:
        max_len = (uint16)(max_len << 2) + 1;
        break;
    }

    /* Convert from IRA to TE's char set,
       dropping inconvertible characters. Input buffer that tmp_oct points to
       will not be NULL terminated so rely on max_len to terminate. */
    tmp_asc = (byte *)dsatutil_convert_chset( (const char *)tmp_oct, 
                                              ALPHA_IRA,
                                              (char *)tmp_asc,
                                              (dsati_chset_type)cscs_val,
                                              max_len,
                                              TRUE );
  }
  else
  {
    /* Convert from octets to HEX char pairs,
       dropping inconvertible characters (won't be any).Input buffer that 
       tmp_oct points to will not be NULL terminated so rely on 
       (len << 1) + 1 to terminate.  */
    tmp_asc = (byte *)dsatutil_convert_chset( (const char *)tmp_oct, 
                                              ALPHA_8BIT,
                                              (char *)tmp_asc,
                                              ALPHA_HEX, 
                                              (len << 1) + 1,
                                              TRUE );
  }
    
  return tmp_asc;
} /* dsatetsismsi_decode_data */

/*===========================================================================

FUNCTION    DSATETSISMSI_TIMESTAMP

DESCRIPTION
  Converts the message SC timestamp type (wms_timestamp_s_type) to a 
  string. It converts it into "yy/MM/dd,hh:mm:ss+/-zz" format.
  writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatetsismsi_timestamp
( 
 wms_timestamp_s_type time,  /* Pointer to Time Stamp        */
 byte * res_ptr                /* Pointer to the result buffer */
)
{
  res_ptr = dsatsmsi_timestamp( time, res_ptr, FALSE);
  
  return res_ptr;
}/* dsatetsismsi_timestamp */

/*===========================================================================

FUNCTION    DSATETSISMSI_DECODE_SMS_DCS

DESCRIPTION
  Decode <dcs> value and fill up the data coding scheme structure.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatetsismsi_decode_sms_dcs
(
 wms_gw_dcs_s_type       * dcs_ptr,  /* pointer to the dcs structure */
 const dsat_num_item_type  dcs_val   /* dcs value */
)	
{
  byte coding_grp_bits = 0;
	
  /* init dcs structure to default settings */ 
  /* these are correct for Reserved Coding Groups 0100..1011 */
  dcs_ptr->msg_class = WMS_MESSAGE_CLASS_NONE;
  dcs_ptr->is_compressed = FALSE;
  dcs_ptr->alphabet = WMS_GW_ALPHABET_7_BIT_DEFAULT;
  dcs_ptr->msg_waiting = WMS_GW_MSG_WAITING_NONE;
  dcs_ptr->msg_waiting_active = FALSE;
  dcs_ptr->msg_waiting_kind = WMS_GW_MSG_WAITING_VOICEMAIL;

  /* perform coding group specific settings */
  coding_grp_bits = (dcs_val & 0xF0) >> 4;

  /* General Data Coding Indication */
  if( !(coding_grp_bits & 0x0C) )
  { 
    /* message class only if bit 4 set */
    if (dcs_val & 0x10) 
      dcs_ptr->msg_class = (wms_message_class_e_type)(dcs_val & 0x03);

  if (dcs_val & 0x20)
    dcs_ptr->is_compressed = TRUE;

  if ( (dcs_val & 0x0C) != 0x0C )
    dcs_ptr->alphabet = (wms_gw_alphabet_e_type)((dcs_val & 0x0C) >> 2);
  }
  /* Message Waiting Indication Group: Discard/Store, GSM7b/UCS2 */
  else if ( coding_grp_bits > 0x0B && coding_grp_bits < 0x0F )
  {
    /* alphabet depends on bit 5 within groups 1100..1110 */
    if (dcs_val & 0x20)
      dcs_ptr->alphabet = WMS_GW_ALPHABET_UCS2;

    /* store/discard depends on bits 4&5 within groups 1100..1110 */
    dcs_ptr->msg_waiting = 
      dcs_val & 0x30 ? WMS_GW_MSG_WAITING_STORE : WMS_GW_MSG_WAITING_DISCARD;

    if (dcs_val & 0x08) 
      dcs_ptr->msg_waiting_active = TRUE;

    dcs_ptr->msg_waiting_kind = (wms_gw_msg_waiting_kind_e_type)(dcs_val & 0x03);
  }
  /* Data Coding/Message Class */
  else if ( coding_grp_bits == 0x0F )
  {
    dcs_ptr->msg_class = (wms_message_class_e_type)(dcs_val & 0x03);

    if (dcs_val & 0x04)
      dcs_ptr->alphabet = WMS_GW_ALPHABET_8_BIT;		
  }

  return;
}/* dsatetsismsi_decode_sms_dcs */

/*===========================================================================

FUNCTION    DSATETSISMSI_ADDR_TO_STR

DESCRIPTION
  Converts the TPDU address type (wms_address_s_type) to a 
  string in the TE's character set. writes the result into the result buffer.
  It also calculates the type of address and assigns it to the type of 
  address pointer
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of the result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatetsismsi_addr_to_str
( 
 wms_address_s_type addr,  /* Address data structure           */
 byte * res_ptr,           /* pointer to result buffer         */
 uint8 * type_of_addr,     /* pointer to type of address field */
 boolean is_csca,           /* Indicates if it is +CSCA cmd     */
 uint16 res_ptr_len          /* Length of result buffer */
)
{
#define UCS2 2
  byte bcd_idx=0;
  uint8 temp = 0;
  dsati_chset_type local_chset = ALPHA_MAX;

  /* Address as BCD is first converted to IRA in this buffer */
  char addr_ira[WMS_ADDRESS_MAX + 1] = {0};
  char *addr_ira_ptr = &addr_ira[0];
  uint8 num_len = 0;
  boolean is_plus_presnt = FALSE;
  byte num_buf[50];
  dsat_num_item_type cscs_val ;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  ASSERT( (res_ptr != NULL) && (type_of_addr != NULL));

  if ( addr.number_of_digits > WMS_ADDRESS_MAX )
  {
    DS_AT_MSG1_ERROR("Invalid num of digits %d in address",addr.number_of_digits);
    return NULL;
  }
  cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
  if ( (cscs_val == UCS2) && 
       (res_ptr_len < (addr.number_of_digits)* UCS2_CHAR_LEN + 1) )
  {
    DS_AT_MSG2_ERROR("Invalid len %d passed in CSCS %d",res_ptr_len,cscs_val);
    return NULL;
  }
  memset( &num_buf[0], 0, sizeof(num_buf));
  *type_of_addr  = 0;

  temp = (uint8)((uint32)addr.number_type & 0x07);
  *type_of_addr = (uint8)((*type_of_addr | temp ) << 4);
  temp = (uint8)((uint32)addr.number_plan & 0x07);
  *type_of_addr = *type_of_addr | temp ;
  *type_of_addr = *type_of_addr | 0x80;

  *res_ptr++ = '"';

  /* Address as BCD is first converted to NULL terminated IRA into 
     addr_ira[] buffer. */
  /* Add '+' if it is a international number */
  /* Do it only if we are in text mode       */
  if ( (*type_of_addr == 145)  && ( ((dsat_num_item_type)dsatutil_get_val(
                        DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) ) || ( is_csca ) ) )
  {
    *addr_ira_ptr++ = '+';
    is_plus_presnt = TRUE;
  }

  while(bcd_idx < addr.number_of_digits)
  {
    if ( ((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE)) || is_csca )
    {
      switch(addr.digits[bcd_idx])
      {
          case 12:
            *addr_ira_ptr++ = 'A';
            break;
          case 13:
            *addr_ira_ptr++ = 'B';
            break;
          case 14:
            *addr_ira_ptr++ = 'C';
            break;
          case 10:
            *addr_ira_ptr++ = '*';
            break;
          case 11:
            *addr_ira_ptr++ = '#';
            break;
          default:
            addr_ira_ptr = 
              (char *)dsatutil_itoa( (uint32) addr.digits[bcd_idx], 
                                     (byte *)addr_ira_ptr, 
                                     10 );
            break;
      }/* switch */

      *addr_ira_ptr = '\0';
    }
    else /* SMS_PDU_MODE */
    {
      res_ptr = dsatutil_itoa((uint32) addr.digits[bcd_idx], res_ptr, 16);
      ++num_len;
    }
    bcd_idx++;
  }

  if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
  {
    num_len = strlen((char*)addr_ira);
    if(is_plus_presnt)
    {
      (void) dsatutil_memscpy((void*)num_buf, 50,(void*)&addr_ira[1],MIN(WMS_ADDRESS_MAX ,(num_len-1)));
    }
    else
    {
      (void) dsatutil_memscpy((void*)num_buf, 50,(void*)&addr_ira[0],MIN(WMS_ADDRESS_MAX+1 ,num_len));
    }
  }
  else
  {
    res_ptr -= num_len;
    (void) dsatutil_memscpy((void*)num_buf, 50,(void*)&res_ptr[0],num_len);
  }
  if((is_csca == FALSE ) && 
     ( PBM_SUCCESS == dsatetsime_get_alphanumeric_name(num_buf, MIN(50,num_len)) ))
  {
    DS_AT_MSG2_MED("Found name = %s for number =%s in recevied SMS",me_ss_val->dsat_alpha_text     
                                                            ,num_buf);
    is_alpha_present = TRUE;
  }


  /* This is a common function and converting the chset is differed based on 'is_csca' boolean. 
     When 'is_csca' is set, then it is just reading the CSCA value from NV/SIM and updating our CSCA data.
     Thus we should not convert the chset when 'is_csca' is TRUE.*/
  if ( TRUE == is_csca )
  {
    local_chset = ALPHA_IRA;
  }
  else if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE))
  {
    local_chset = (dsati_chset_type)cscs_val;
  }
	
  if( ALPHA_MAX != local_chset )
  {
    /* Then addr_ira[] buffer is converted from IRA to 
       TE's character set at location pointed to by res_ptr.
       Maximum possible length is :
       (WMS_ADDRESS_MAX + 1 for preceding '+') * UCS2_CHAR_LEN
       + 1 for terminating NULL */
    res_ptr = (byte *)dsatutil_convert_chset( 
      (const char *)&addr_ira[0],
      ALPHA_IRA, 
      (char *)res_ptr,
      (dsati_chset_type)local_chset,
       res_ptr_len,
      TRUE );
  }
  
  /* sanity check before updating */
  if ( res_ptr != NULL )
  {
    *res_ptr++ = '"';
  }

  return res_ptr;
}/* dsatetsismsi_addr_to_str */

/*===========================================================================

FUNCTION    DSATETSISMSI_SCADDR_TO_STRING

DESCRIPTION
  Converts the TPDU address type (wms_address_s_type) to a 
  string. writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to adthe end of result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatetsismsi_scaddr_to_string
( 
 wms_address_s_type addr,  /* Address data structure   */
 byte * res_ptr            /* pointer to result buffer */
)
{
  byte loop = 0;
  uint8 type_of_addr = 0;
  uint8 temp = 0;
  boolean f_flag = 0;
  int len;

  f_flag =  addr.number_of_digits & 0x01;

  if ( f_flag )
  {
    /* we have a odd length */
    /* len = (x+1)/2 + 1    */
    if ( addr.number_of_digits != 0 )
    {
      len = ( (addr.number_of_digits + 1)/2 ) + 1;
    }
    else
    {
      len = 0;
    }
  }
  else
  {
    /* we have a even length */
    /* len = x/2 + 1        */
    if ( addr.number_of_digits != 0 )
    {
      len = ( addr.number_of_digits/2 ) + 1;
    }
    else
    {
      len = 0;
    }
  }

  if ( len < 16 )
  {
    /* single digit length. Pad it with a 0 */
    *res_ptr++ = '0';
  }

  res_ptr = dsatutil_itoa( (uint32) len, res_ptr, 16 );
  
  if ( addr.number_of_digits )
  {
    temp = (uint8)((uint32)addr.number_type & 0x07);
    type_of_addr = (uint8)((type_of_addr | temp ) << 4);
    temp = (uint8)((uint32)addr.number_plan & 0x07);
    type_of_addr = type_of_addr | temp ;
    type_of_addr = type_of_addr | 0x80;

    if ( type_of_addr < 16 )
    {
      *res_ptr++ = '0';
    }
    res_ptr = dsatutil_itoa((uint32) type_of_addr, res_ptr, 16);

    while( (loop < addr.number_of_digits ) && (loop < (sizeof(addr.digits) -1)))
    {
      if ( ( loop == ( addr.number_of_digits - 1 ) ) && ( f_flag ) )
      {
        *res_ptr++ = 'F';
        res_ptr = dsatutil_itoa((uint32) addr.digits[loop], res_ptr, 16);
      }
      else
      {
        res_ptr = dsatutil_itoa((uint32) addr.digits[loop+1], res_ptr, 16);
        res_ptr = dsatutil_itoa((uint32) addr.digits[loop], res_ptr, 16);
      }
      loop = loop + 2;
    }
  }
  return res_ptr;
}/* dsatetsismsi_scaddr_to_string */

/* LOCAL FUNCTION DEFINITIONS */
/*===========================================================================

FUNCTION    STRING_TO_TIMESTAMP

DESCRIPTION
  Converts the message SC timestamp type (wms_timestamp_s_type) to a 
  string. It converts it into "yy/MM/dd,hh:mm:ss+/-zz" format.
  writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean string_to_timestamp
(
 const byte * fo_ptr,          /* Pointer to the <fo>   */
 byte       * vp_ptr,          /* Pointer to the string */
 wms_gw_validity_s_type * vp   /* Pointer to Time Stamp */
)
{
  char seperators[]   = "//,::+-";
  boolean result = TRUE;
  byte * data_ptr1;
  byte * data_ptr2;
  byte s[32];
  byte time_stamp_tokens[7][3];
  int i=0, k;
  uint8 j = 2;
  dsat_num_item_type temp, fo, temp_num;
  boolean neg_flag = FALSE;
  uint8 limit[7] = { 99, 12, 31, 23, 59, 59, 79 };
  uint8 time_val[7] = { 0, 0, 0, 0, 0, 0, 0 };

  if (vp_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Unexpected NULL ptr");
    return FALSE;
  }
  
  if (!dsatsmsi_strip_quotes( vp_ptr, s, sizeof(s) ))
  {
    data_ptr1 = vp_ptr;
    data_ptr2 = vp_ptr;
  }
  else
  {
    data_ptr1 = s;
    data_ptr2 = s;
  }

  if(dsatutil_atoi(&fo, fo_ptr, 10))
  {
    /* We got an error in converting the <fo> */
    return FALSE;
  }

  /* extract VPF */
  temp = ( fo & 0x18 ) >> 3;

  /* if VPF indicates no VP */
  if ( temp == 0 )
  {
    if ( *vp_ptr == '\0' )
    {
      /* set structure to be passed to WMS */
      vp->format = WMS_GW_VALIDITY_NONE;
      return TRUE;
    }
    else
    {
      /* no <vp> should be provided */
      return FALSE;
    }
  }
  /* if VPF indicates relative VP */
  else if( temp == 2 )
  {
    /* use default value if none provided */
    if ( *vp_ptr == '\0' )
    {
      vp->format = WMS_GW_VALIDITY_RELATIVE;
      wms_ts_decode_relative_time( 167, &vp->u.time );
      return TRUE;
    }
    else if(dsatutil_atoi(&temp_num, vp_ptr, 10))
    {
      /* overflow */
      return FALSE;
    }

    vp->format = WMS_GW_VALIDITY_RELATIVE;
    wms_ts_decode_relative_time( (uint8)temp_num, &vp->u.time );
    return TRUE;
  }
  /* if VPF indicates absolute VP */
  else if ( temp == 3 )
  {
    if ( *vp_ptr == '\0' )
    {
      /* it must be provided as 3GPP TS 27005 doesn't give a default value */
      return FALSE;
    }

    /* First check the format of the time stamp */
    while ( (*data_ptr2 != '\0') && (i < 6) )
    {   
      if(*(data_ptr1+j) == seperators[i])
      {
        for (k=0; k<2; k++)
        {
          time_stamp_tokens[i][k] = *data_ptr2++;
        }
        time_stamp_tokens[i][k] = '\0';
        data_ptr2++;
        j = j + 3;
        i++;
      }
      else if ( (i == 5) && (*(data_ptr1+j) == seperators[i+1]) )
      {
        for (k=0; k<2; k++)
        {
          time_stamp_tokens[i][k] = *data_ptr2++;
        }
        time_stamp_tokens[i][k] = '\0';
        data_ptr2++;
        j = j + 3;
        i++;
        /* We got a negative flag */
        neg_flag = TRUE;
      }
      else
      {
        return FALSE;
      }
    }

    /* If we are here that means that the formatting is right so far */
    /* We still have one more token to extract                       */
    k=0;
    while(*data_ptr2 != '\0' )
    {
      time_stamp_tokens[i][k] = *data_ptr2++;
      k++;
    }
    time_stamp_tokens[i][k] = '\0';
    
    
    /* Now fill in the time stamp */
    vp->format = WMS_GW_VALIDITY_ABSOLUTE;

    for ( i=0; i<7; i++)
    {
      if(dsatutil_atoi(&temp_num, time_stamp_tokens[i], 10))
      {
        /* We got an error in converting the <fo> */
        return FALSE;
      }
      else
      {
        if( temp_num > limit[i] )
        {
          return FALSE;
        }
        else
        {
          time_val[i] = (uint8)temp_num;
        }
      }
    }

    /* Convert integers to BCD */
    vp->u.time.year = 
      (uint8)( (time_val[0] % 10) + ((time_val[0] / 10) << 4) );
    vp->u.time.month = 
      (uint8)( (time_val[1] % 10) + ((time_val[1] / 10) << 4) );
    vp->u.time.day = 
      (uint8)( (time_val[2] % 10) + ((time_val[2] / 10) << 4) );
    vp->u.time.hour = 
      (uint8)( (time_val[3] % 10) + ((time_val[3] / 10) << 4) );
    vp->u.time.minute = 
      (uint8)( (time_val[4] % 10) + ((time_val[4] / 10) << 4) );
    vp->u.time.second = 
      (uint8)( (time_val[5] % 10) + ((time_val[5] / 10) << 4) );
    
    vp->u.time.timezone = (sint7)time_val[6];
    if(neg_flag)
    {
      vp->u.time.timezone *= -1;
    }
  }
  else
  {
    return FALSE;
  }

  /* Now parse the string to collect time stamp from it */

  return result;
}

#endif /* FEATURE_ETSI_SMS */
