#ifndef DSAT707SMS_H
#define DSAT707SMS_H

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

   Copyright (c) 2008, 2012 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsat707smsi.h_v   1.3   12 Jul 2002 10:07:40   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707sms.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
05/10/10   kk      Added support for ^HC SMS commands. 
11/13/08   nc      Added support for QCMGW,QCMSS,QCMGL Commands
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


/* this file should be included only for SMS in CDMA mode */
#ifdef FEATURE_CDMA_SMS
#include "dsati.h"
#include "dsat707smsctab.h"

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCSMP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSMP command.
  $QCSMP is used to set parameters for sending text sms messages.

  DEPENDENCIES
  None

  RETURN VALUE
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcsmp_cmd
(
  dsat_mode_enum_type mode,           /* AT command mode:            */
  const dsati_cmd_type *tab_entry,    /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr         /* Place to put response       */
);

/*===========================================================================

FUNCTION DSAT707SMS_EXEC_QCPMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPMS command.
  $QCPMS is used to select preferred memory storage for reading, 
  writing etc.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD: An async call has made to wms.

SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcpms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCNMI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCNMI command.
  $QCNMI is used to select the procedure of how receiving new messages
  from the network is indicated to TE.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_CMD_ERR_RSP : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcnmi_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGS command.
  $CMGS is used to send a message from TE to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmgs_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMSS command.
  $QCMSS is used to send a message already stored from Memory to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmss_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$CMGR command.
  $QCMGR is used to read a sms message.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmgr_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$CMGL command.
  $CMGL is used to list sms messages in memory.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmgl_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGD command.
  +CMGD is used to delete sms messages from <mem1>.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmgd_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *rb_ptr                 /* Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_QCMGW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT$QCMGW command. 
  $QCMGW is used to store message to memory storage <mem2>

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsat707sms_exec_qcmgw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION DSAT707SMS_EXEC_HSMSSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^HSMSSS command to choose SMS parameter
  commands. 
  
  <ack> whether or not need SMS status report, values can be:
  0 don’t need SMS status report.
  1 need SMS status report
  <prt> SMS PRI, values refer to ^HCMGW.
  <fmt> coding method, values refer to ^HCMGW
  <Prv> secrecy levels, values refer to ^HCMGW

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707sms_exec_hsmsss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/*===========================================================================

FUNCTION DSAT707SMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It is used in both text mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat707sms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
);

/*===========================================================================

FUNCTION    DSAT707SMSMI_ROUTE_CONFIG
DESCRIPTION
  Sets the SMS message routing configuration for CDMA mode.
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: wms accepts the config route command
  DSAT_ERROR:     wms rejects the config route command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707smsi_route_config
(
  dsat_num_item_type mt_val, /* what type of deliver routing we are setting */
  dsat_num_item_type ds_val  /* what type of status report routing */
);

/*===========================================================================

FUNCTION DSAT707SMSI_TELESERVICE_NUM_TO_ENUM

DESCRIPTION
  Function  convert teleservice number to wms teleservice enum type.

DEPENDENCIES
  None

RETURN VALUE
  wms teleservice enum.

SIDE EFFECTS
  None

===========================================================================*/
wms_teleservice_e_type dsat707smsi_teleservice_num_to_enum
(
 char * num_str
);

/*===========================================================================

FUNCTION DSAT707SMSI_STRING_TO_TIMESTAMP

DESCRIPTION
  Function converts string of "yy/MM/dd,hh:mm:ss" format to
  wms_timestamp_s_type struct.

  If ts_ptr is NULL, the function only checks the format of the time string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : The time_str has good format.
         The ts_ptr contains the output if it is not NULL.
  FALSE: The time_str has good format.
         The ts_ptr does not change.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsat707smsi_string_to_timestamp
(
  const char            * time_str,     /* in: string of "yy/MM/dd,hh:mm:ss" 
                                         *     format. Maybe quouted or 
                                         *     unquoted                    */
  wms_timestamp_s_type  * ts_ptr        /* out                             */
);

/*===========================================================================
FUNCTION    DSAT707SMSI_ADDR_TO_STR

DESCRIPTION
  Converts the TPDU address type (wms_address_s_type) to a 
  string. writes the result into the result buffer.
    
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of the result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsat707smsi_addr_to_str
( 
 wms_address_s_type addr,  /* Address data structure           */
 uint8 * type_of_addr,     /* pointer to type of address field */
 byte * res_ptr           /* pointer to result buffer         */
);

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION DSAT707SMSI_CURRENT_WMS_TIMESTAMP

DESCRIPTION
  Function returns the current time in the wms_timestamp_s_type struct.
  If ts_ptr is NULL, the function only checks the format of the time string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : The time_str has good format.
         The ts_ptr contains the output if it is not NULL.
  FALSE: The time_str has good format.
         The ts_ptr does not change.

SIDE EFFECTS
  None

===========================================================================*/
void dsat707smsi_current_wms_timestamp
(
  wms_timestamp_s_type  * ts_ptr        /* out                             */
);

/*===========================================================================

FUNCTION DSAT707SMSI_SET_WMS_TIMESTAMP

DESCRIPTION
  Function returns the current time in the wms_timestamp_s_type struct.
  If ts_ptr is NULL, the function only checks the format of the time string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : The time_str has good format.
         The ts_ptr contains the output if it is not NULL.
  FALSE: The time_str has good format.
         The ts_ptr does not change.

SIDE EFFECTS
  None

===========================================================================*/
void dsat707smsi_set_wms_timestamp
(
  wms_timestamp_s_type  * ts_ptr,        /* out                             */
  time_julian_type      * jtime          /* in                              */
);
#endif /* FEATURE_DSAT_EXTENDED_SMS */
#endif /* FEATURE_CDMA_SMS */
#endif /* DSAT707SMS_H */
