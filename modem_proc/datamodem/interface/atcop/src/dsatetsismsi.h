#ifndef DSATETSISMSI_H
#define DSATETSISMSI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S )
                
                P R O C E S S I N G

                I N T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the sms commands internally. 

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2014 by Qualcomm Technologies, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsismsi.h_v   1.3   12 Jul 2002 10:07:40   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsismsi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
05/10/10   kk      Mainlining pending GOBI changes.
03/11/09   nc      Modified the prototype of 'dsatetsismsi_addr_to_str' 
                   to include the length of output ptr.
12/02/08   ss      Off target Lint fix.
07/23/08   bs      Fixed +CMGL command for reading large SMS.
07/03/08   nc      Fixed USSD broken charecter set problems
11/04/07   pp      Lint low - fixes.
01/30/07   ss      Changed prototypes for dsatetsismsi_read_txt_msg and 
                   dsatetsismsi_read_pdu_msg
02/24/04   snb     Changed argument types in support of SMS flow control.
11/28/03   sb      CDMA SMS changes
10/03/03   snb     Support for 8 bit and UCS2 alphabets, changes to +CSMP 
                   parameter validation
09/09/03   sb      Fix for ECR33797 and CR34269: Do not expect an ACK for an
                   message with WMS_GW_MSG_WAITING_DISCARD
08/24/03   sb      Cell broadcast editions (first cut)
02/07/02   sb      Created module.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ETSI_SMS

#include "dsatetsisms.h"
#include "dsatsmsi.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
  Type of destination address formatting used by the User
---------------------------------------------------------------------------*/
typedef enum
{
  SMS_TEXT_MODE,
  SMS_PDU_MODE
}sms_mode_e_type;

/*===========================================================================

                        PUBLIC DATA DECLARATION

===========================================================================*/
#ifdef FEATURE_ETSI_SMS
extern const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table;
#endif

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATETSISMSI_READ_TXT_MSG

DESCRIPTION
  Local function that is used to read a text message. Writes the message 
  plus the optional header fields into the result buffer.

DEPENDENCIES

RETURN VALUE
  Number of characters written into the result buffer

SIDE EFFECTS

===========================================================================*/
unsigned int dsatetsismsi_read_txt_msg
(
 wms_client_ts_data_s_type   * tpdu_msg_ptr,   /* Ptr to the TPDU         */
 dsm_item_type *res_buff_ptr,                  /* Place to put response   */
 sms_msg_status_info_s_type * status_msg_ptr   /* Ptr to msg from WMS     */
);

/*===========================================================================

FUNCTION DSATETSISMSI_READ_PDU_MSG

DESCRIPTION
  Local function that is used to read a pdu message. Writes the message 
  into the result buffer.

DEPENDENCIES

RETURN VALUE
  Number of characters written into the result buffer

SIDE EFFECTS

===========================================================================*/
unsigned int dsatetsismsi_read_pdu_msg
(
 const sms_msg_status_info_s_type * msg_ptr,/* Ptr to msg from WMS      */
 dsm_item_type *res_buff_ptr,          /* Place to put response   */
 const wms_client_ts_data_s_type  * tpdu_msg_ptr /* Ptr to the TPDU    */
);

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
);

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
);

/*===========================================================================

FUNCTION    DSATETSISMSI_VALIDATE_VP

DESCRIPTION
  Validates the format of validity period (<vp>). It checks both relative
  format and absoute format.Also updates the vp with the timestamp upon a 
  valid vp found.

DEPENDENCIES
  None

RETURN VALUE
  Returns an boolean to indicate if <vp> is formatted right

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsismsi_validate_vp 
(
 const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
 wms_gw_validity_s_type *timestamp     /*  timestamp for relative validity */
);

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
 const tokens_struct_type *tok_ptr    /*  Command tokens from parser  */
);

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
 wms_address_s_type *addr,   /* Pointer to address structure to be filled */
 sms_mode_e_type sms_mode    /* SMS mode Enum                             */
);

/*===========================================================================

FUNCTION    DSATETSISMSI_ENCODE_PDU

DESCRIPTION
  Takes a Hex stream (Given in ASCII format) and converts it to stream of 
  octets. Ex. "2A" is converted to 42.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsismsi_encode_pdu 
(
 byte  * in_ptr,    /* Pointer to the Hex data (in ASCII format)    */ 
 uint8 * out_ptr,   /* Pointer to the result buffer(converted data) */
 int     len        /* Length of the input data pointer (Hex data)  */
);

/*===========================================================================

FUNCTION    DSATETSISMSI_ENCODE_DATA

DESCRIPTION
  Takes a ASCII streamand converts it according to the selected data coding
  scheme
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsismsi_encode_data 
( 
 wms_gw_user_data_s_type   * data_ptr,/* result buffer(converted data)  */
 const byte                * txt_ptr, /* Pointer to the IRA char stream */
 int                         len      /* Lenght of the IRA char stream  */
);

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
);

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
 byte * res_ptr                /* POinter to the result buffer */
);

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
);

/*===========================================================================

FUNCTION    DSATETSISMSI_FREE_ELEMENT

DESCRIPTION
  Checkes to see if any message items have to be freed. Deletes the message
  from temporary memory of the desired subscription.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsismsi_free_element 
(
 boolean                 ack_required, /* Flag to tell if ACK is required */
 sys_modem_as_id_e_type  subs_id       /* Subscription id */
);

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
);

/*===========================================================================

FUNCTION    DSATETSISMSI_ADDR_TO_STR

DESCRIPTION
  Converts the TPDU address type (wms_address_s_type) to a 
  string. writes the result into the result buffer.
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
 byte     *res_ptr,           /* pointer to result buffer         */
 uint8    *type_of_addr,     /* pointer to type of address field */
 boolean   type_of_format,    /* Indicates if it is +CSCA cmd     */
 uint16     res_ptr_len          /* Length of result buffer */
);
#endif /* FEATURE_ETSI_SMS */

#endif /* DSATETSISMSI_H */








