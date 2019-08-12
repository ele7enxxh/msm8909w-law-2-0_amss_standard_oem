#ifndef DSATACT_H
#define DSATACT_H
/*===========================================================================

                D A T A   S E R V I C E S

                A T   C O M M A N D 

                ( C O M M O N   A C T I O N  
                
                  C O M M A N D S )

                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the

  data services AT commands ( common action commands ) processor.


Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatact.h_v   1.3   08 Nov 2002 15:33:14   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatact.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
11/30/12   tk      ATCoP changes for Triton DSDA.
03/07/12   sk      Made CLCC common for all modes.
01/19/12   sk      Feature cleanup.
02/18/08   sa      Added modification for Passport Feature support.
10/26/07   ua      Added support for AT&S command. 
08/06/07   pp      Added support for AT\Q command.
02/07/07   pp      New commands: AT&W, AT\S.
06/12/07   ss      Added new declarations for +WS46.
04/11/07   ua      Modifications as per KDDI requirements. 
03/30/05   tkk     Featurized ATB command.
03/03/05   tkk     Added interdependency information for ATB command.
03/02/05   tkk     Added support for ATB command.
01/11/05   ar      Add support for +WS46 command.
09/17/04   snb     Add ATE command handler to allow echo enable as default.
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
11/07/02   ar      Incorporated code review comments
07/22/02   ak      Include dsati.h
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      created file
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES
===========================================================================*/

#include "datamodem_variation.h"
#include "dsati.h"
#include "dsatetsicall.h"
/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Declarations for WS46 */
#define DSAT_WS46_FUNC_MAX 0xFFFFFFFF
#define CALL_SEQNUM_INVALID -1
#ifdef FEATURE_DATA_MUX
extern rex_timer_type dsat_rdm_open_timer;
extern dsat_num_item_type dsat_port_open_count;
#endif /* FEATURE_DATA_MUX */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSATACT_EXEC_AMPC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&C command. 
  AT&C is used to set the behaviour of circuit 109 relates
  to the recieved line signal from the distant end.
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_ampc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr       /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_ICF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+ICF command. 
  AT+ICF is used to set the TE TA character framing .
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_icf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_IFC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+IFC command. 
  AT+IFC is used to set TE TA local flow control .
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_ifc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&S command. 
  AT&S is used to set the management of  DSR control.
  
DEPENDENCIES
  NONE
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  If the DUN fails, check the value of &C command. This has been verified
  on UART and USB which worked with &C set to 1 and 2. If different DUN 
  application is used and the call doesn't go through then try different 
  options of &C command. 
===========================================================================*/
dsat_result_enum_type dsatact_exec_amps_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr_ptr       /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&F command. 
  AT&F is used to set to factory defined configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
dsat_result_enum_type dsatact_exec_ampf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr_ptr       /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&w command.
  at&w is used to store S registers & V.250 into NV.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Stores S registers & V.250 command settings into NV.

===========================================================================*/
dsat_result_enum_type dsatact_exec_ampw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_SLASHS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at\s command.
  at\s is used to print AT command settings.

DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_slashs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_ATZ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes ATZ command. 
  ATZ is used to reset to default configuration.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
dsat_result_enum_type dsatact_exec_atz_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_SLASH_Q_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT\Q command. 
  AT\Q is used to set serial port flow control.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
  
SIDE EFFECTS
  Modifies flow control settings of a serial port.

===========================================================================*/
dsat_result_enum_type dsatact_exec_slash_q_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_ATD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATD command.
  ATD command is used to dial a call (ASYNC, PACKET, VOICE) using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_initiate_call() is called.

SIDE EFFECTS
  Calls ds3g_initiate_call() function to initiate a call

===========================================================================*/
dsat_result_enum_type dsatact_exec_atd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATACT_EXEC_ATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATA command.
  ATA command is used to answer a incoming call using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  Calls ds3g_answer_call() function to initiate a call

===========================================================================*/
dsat_result_enum_type dsatact_exec_ata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATACT_EXEC_ATH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATH command.
  ATH command is used to hangup both data nad voice calls
  using AT commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_hangup_call() is called.

SIDE EFFECTS
  Calls ds3g_hangup_call() function to terminate a call

===========================================================================*/
dsat_result_enum_type dsatact_exec_ath_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_GMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMI/+CGMI command.
  This command returns manufacturer information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_gmi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_GMM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMM/+CGMM command.
	This command returns model information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_gmm_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_GMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMR/+CGMR command.
	This command returns revision information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_gmr_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_GSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN/+CGSN command.
  This command returns serial number information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_gsn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_GCAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GCAP command.
  This command returns complete capabilities list information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_gcap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

#ifdef FEATURE_DSAT_ETSI_MODE
/* Exclude CDMA (for now, may be required later) */

/*===========================================================================

FUNCTION DSATACT_EXEC_WS46_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +WS46 command.
  This command manages wireless data service (WDS) side-stack selection.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD: if this function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ws46_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_PROCESS_WS46_CMD

DESCRIPTION
  This function process the response for the +WS46 command on return from
  asynchronous processing / synchronous function call. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_process_ws46_cmd
(
  ds_at_cm_ph_pref_type  *pref_mode  /* Network preference mode */
);

#endif /* FEATURE_DSAT_ETSI_MODE */


/*===========================================================================

FUNCTION DSATACT_EXEC_ATI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes I command.
  This command returns identification information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_ati_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATACT_EXEC_ATE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes E command.
  This command controls echoing of the command line to TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_ate_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_ATX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes X command.
  This command sends a CONNECT message when a connection is established by blind
  dialing and enables the additional result codes.
  
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : If it is a success.

SIDE EFFECTS
  None  

===========================================================================*/
dsat_result_enum_type dsatact_exec_atx_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_EXEC_ATO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes O command.
  This command causes the DCE to return to online data state.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  Calls ds3gmgr_return_to_online_data to switch mode

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ato_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DATA_MUX
/*===========================================================================
FUNCTION DSATACT_EXEC_CMUX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CMUX command.
  This command is used to enable/disable the 3GPP TS 27.010 multiplexing protocol control channel


DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_exec_cmux_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATACT_CMUX_HANDLER

DESCRIPTION
Handler function which initializes MUX module.

DEPENDENCIES
  Physical port passed on to MUX should not be in open state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatact_cmux_handler(void);

/*===========================================================================

FUNCTION DSATACT_CMUX_NOTIFY_HANDLER

DESCRIPTION
Handler function to handle MUX notification. Upon a failure to open the physical port or a close noification
of MUX, ATCoP would need to re-open the Physical port.

DEPENDENCIES
  Physical port to be opened in case of failures, need to be closed.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_cmux_notify_handler
(
  ds_cmd_type  * cmd_ptr   /* DS Command pointer		   */
);
/*===========================================================================

FUNCTION DSAT_INIT_PORT

DESCRIPTION
This function initializes the DATA_SRVC port to the one stored in EFS.

DEPENDENCIES
  EFS @ /atcop_persistent.txt 

RETURN VALUE
  None

SIDE EFFECTS
  In case, the EFS @ atcop_persistent.txt is absent we may never be able to re-open 

===========================================================================*/
void dsat_init_port(void);

/*===========================================================================

FUNCTION DSATACT_RDM_NOTIFY_HANDLER

DESCRIPTION
  RDM notify Handler on a physical port closure.
  On RDM falure to assign the port, we set a timer of 500ms and retry. We do a maximum of 3 retries to see if
  we can open the port back.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatact_rdm_notify_handler
(
  ds_cmd_type  * cmd_ptr   /* DS Command pointer		   */
);

/*===========================================================================

FUNCTION  DSATACT_RDM_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the RDM timer expired cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_rdm_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
#endif /* FEATURE_DATA_MUX */

/*===========================================================================

FUNCTION DSATACT_CALL_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  call should be aborted.  It should be installed in the command table
  as the abort handler function for abortable AT commands which bring
  up a call.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Call Manager is requested to end voice calls.

===========================================================================*/
/* ARGSUSED */
boolean dsatact_call_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSATACT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatact_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
);

#ifndef FEATURE_DSAT_CUST
/*===========================================================================

FUNCTION DSATACT_EXEC_CLCC_CMD

DESCRIPTION
  This is the command handler function for +CLCC.

  This function sends request to CM to get the all calls details

  Return OK for test command.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if the action command succeeds.
  DSAT_OK        : if this is a test command.
  DSAT_ERROR     : if the action command fails or other than test or execute 
                   command is received.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatact_exec_clcc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATACT_CALL_INFO_ABORT_HANDLER

DESCRIPTION
  This function generates an abort command to terminate asynchronous 
  processing for +CLCC command.  It should be
  installed in the command table as the abort handler function for
  +CLCC command.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatact_call_info_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);
#endif/* FEATURE_DSAT_CUST */

/*===========================================================================

FUNCTION  DSATACT_CALL_INFO_CLCC_CMD_HANDLER

DESCRIPTION
  This function processes the Call Manager call id information and send a 
  request to CM with remaining active CS calls call id to get complete 
  information about that Call Id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If a request is sent to CM to retrieve the given call id 
                    information
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_call_info_clcc_cmd_handler
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
);


/*===========================================================================

FUNCTION  DSATACT_CALL_INFO_LIST_CLCC_CMD_HANDLER

DESCRIPTION
  This function processes the Call Manager active call ids list and send 
  a request to CM with Call id of each active CS call to get complete 
  information about that Call Id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If a request is sent to CM to retrieve the given call id 
                    information
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_call_info_list_clcc_cmd_handler
(
  const ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type          subs_id
);


/*===========================================================================

FUNCTION  DSATACT_REPORT_CLCC

DESCRIPTION
  This function displays the required details of given Call id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If more call ids whose information need to be 
                    retrieved from CM are there
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_report_clcc
(
  sys_modem_as_id_e_type  subs_id,
  ds_at_call_info_s_type *call_info,
  cm_call_substate_type   call_sub_state
);

/*===========================================================================

FUNCTION DSATACT_TRACK_SEQUENCE_NUMBER

DESCRIPTION
  Track the sequence number (as described in 3GPP TS 22.030 section 6.5.5.1) 
  for this call ID and event: Set in call sequence number indexed array 
  dsat_seqnum_callid[] on call incoming or connected event and clear on call 
  end event.
  3GPP TS 22.030 section 6.5.5.1 reads - "X" is the numbering (starting with 1)
  of the call given by the sequence of setting up or receiving the calls 
  (active, held or waiting) as seen by the served subscriber. Calls hold their
  number until they are released. New calls take the lowest available number.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatact_track_sequence_number
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id,
  cm_call_event_e_type    call_event,
  cm_call_type_e_type     call_type
);


#endif /* DSATACT_H */
