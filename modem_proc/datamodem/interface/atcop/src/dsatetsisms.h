#ifndef DSATETSISMS_H
#define DSATETSISMS_H
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

   Copyright (c) 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
   Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsisms.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
10/31/07   ua      Added support for +CSAS and +CRES commands. 
09/03/04   snb     Add support for +CMMS command.
11/28/03   sb      CDMA SMS changes
08/24/03   sb      Cell broadcast editions (first cut)
04/08/03   sb      Status-report and command support
03/05/03   wx      Add +CGSMS command
08/10/01   sb      Created module.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


/* this file should be included only for SMS in GSM or WCDMA mode */
#ifdef FEATURE_ETSI_SMS

#include "dsati.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSMS command. 
  +CSMS is used to select messaging service.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_csms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CPMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CPMS command. 
  +CPMS is used to select preferred memory storage for reading, writing etc.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cpms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSCA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSCA command. 
  +CSCA is used to select the SMSC address, through which mobile
  originated SMs are transmitted.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_csca_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSMP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSMP command. 
  +CSMP is used to select additional parameters that are needed to 
  send or to store SM.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_csmp_cmd
(
  dsat_mode_enum_type mode,         /*  AT command mode:            */
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSDH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSDH command. 
  +CSDH is used to control whether detailed information is shown in text
  mode result code.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_csdh_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CNMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CNMI command. 
  +CNMI is used to select the procedure of how receiving new messages
  from the network is indicated to TE.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cnmi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGL command. 
  +CMGL is used to list message with status value from message
  storage <mem1> to TE.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGR command. 
  +CMGR is used to read message with location value <index> from message
  storage <mem1>.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated


SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGD_CMD

DESCRIPTION
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMSS command. 
  +CMSS is used to send a message with location value (<index>) from
  preferred message storage to the network..

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGS command. 
  +CMGS is used to send a message from TE to the network.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGW command. 
  +CMGW is used to store message to memory storage <mem2>

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGC command. 
  +CMGC is used to senc command message to the network.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to UASMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cmgc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*  ------------------------------------------------------------------  */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CNMA_CMD

DESCRIPTION
  This function executes AT+CNMA command. +CNMI confirms correct reception 
  of a new message ( SMS-DELIVER, SMS-STATUS-REPORT ) which is routed 
  directly to TE.

DEPENDENCIES
  This acknowledment command is used only if <service> of +CSMS command
  equals 1.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cnma_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMMS_CMD

DESCRIPTION
  This function executes AT+CMMS command. +CMMS controls the continuity of SMS
  relay protocol link. 

DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSAS_CMD

DESCRIPTION
  This function executes AT+CSAS command. +CSAS saves the settings of +CSCA,
  +CSMP, +CSCB into NV. 

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ASYNC_CMD : if the command has been successfully executed
  DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_csas_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CRES_CMD

DESCRIPTION
  This function executes AT+CRES command. +CRES restores the settings of +CSCA,
  +CSMP, +CSCB from NV which got saved through +CSAS command. 

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ASYNC_CMD : if the command has been successfully executed
  DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cres_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_ETSI_SMS_PS
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CGSMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CGSMS command. 
  +CGSMS is used to select service for MO SMS messages.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD : if the command has been successfully executed
    DSAT_ERROR     : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsisms_exec_cgsms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_ETSI_SMS_PS */

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSCB_CMD

DESCRIPTION
  This function executes AT+CSCB command. +CSCB command is used to select
  which type of Cell Broadcast Messages are to be received by TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Will govern the incoming CB message types
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cscb_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_ETSI_SMS_CB */

#endif /* FEATURE_ETSI_SMS */

#endif /* DSATETSISMS_H */
