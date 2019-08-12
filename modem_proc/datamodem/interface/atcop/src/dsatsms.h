#ifndef DSATSMS_H
#define DSATSMS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the sms commands.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2003 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatsms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
07/05/10   nc      Changed the MT SMS Queue Initialisation independent of 
                   the current operating mode.
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
02/24/04   snb     Added support for handling ATCoP SMS via signals, flow
                   control of received messages via watermarks.
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)

#include "dsati.h"

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATSMS_EXEC_SMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes a SMS command.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatsms_exec_sms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATSMS_PROCESS_SMS_CMD

DESCRIPTION
  This function processes a SMS command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_process_sms_cmd(void);

/*===========================================================================

FUNCTION DSATSMS_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an SMS command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

DEPENDENCIES
  DS CMD is generated.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void dsatsms_abort_cmd
(
 dsati_cms_err_e_type error /* +CMS ERROR Number */
);

/*===========================================================================

FUNCTION DSATSMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It is used in both text mode as well as PDU mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
);

/**********************EVENT HANDLER FUNCTIONS******************************/

/*===========================================================================

FUNCTION  DSATSMS_ERR_HANDLER

DESCRIPTION
  When there is an error in processing a SMS AT command, UASMS sends a command
  to handle the error condition. This function is the handler function for the
  error case.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_err_handler 
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
);

/*===========================================================================

FUNCTION  DSATSMS_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a SMS command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_abt_handler
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
);


/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS_EX

DESCRIPTION
  SMS initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to WMS.
  - Registers the WMS message and configuration call back functions.
  - Activates ATCOP asa client
  - Sets up the message routing configuration.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_init_sms_ex ( void );

/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS

DESCRIPTION
  SMS initialization function. This function does the following:
  - Registers the WMS broadcast events callback function.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_init_sms ( void );

/*===========================================================================

FUNCTION  DSATSMS_SYNC_NV

DESCRIPTION
  This function syncs sms nv.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_sync_nv
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================

FUNCTION  DSATSMS_DEINIT_SMS

DESCRIPTION
  SMS deinitialization function. This function does the following:
  - Deregisters the WMS broadcast events callback function.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_deinit_sms ( void );

/*===========================================================================

FUNCTION DSATSMS_SIGNAL_HANDLER

DESCRIPTION
  This function processes the DS_AT_SMS_SIG, which indicates that an event
  has been received from WMS. Handler function for SMS events 
  (both configuration and message) from WMS

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_signal_handler
( 
 dsat_mode_enum_type mode
);

/*===========================================================================

FUNCTION DSATSMS_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsatsms_flush_sms_indications
(
 void
);

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#endif /* DSATSMS_H */
