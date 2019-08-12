#ifndef DSATGSMFAX_H
#define DSATGSMFAX_H
/*===========================================================================

                D A T A   S E R V I C E S

                A T   C O M M A N D 

                ( G S M   F A X   C O M M A N D S )

                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services GSM FAX AT commands processor.

Copyright (c) 2001 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatgsmfax.h_v   1.0   20 Feb 2002 14:20:08   doerfler  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatgsmfax.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
04/06/06   ar      Added support to updating fax protocol stack parameters.
01/10/02   wx      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_FTS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.  It excutes the Fax Transmit Silence command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatgsmfax_exec_fts_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_FRS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.  It executes Fax Receive Silence command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_frs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_FTH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.   It executes Fax Transmit HDLC frame command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_fth_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_FRH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes Fax Receive HDLC frame command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_frh_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_FTM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.  It executes Fax Transmit Message command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_ftm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_FRM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.  It executes Fax Receive Message command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : excuting the action command and waiting for the
                     callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_frm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATGSMFAX_FPS_T31_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async fax functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_fps_t31_status_handler
(
  ds_cmd_type * cmd_ptr
);

/*===========================================================================

FUNCTION DSATGSMFAX_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  FAX command should be aborted.  The fax commands include +FRS, 
  +FRM, +FRH.  It should be installed in the command table
  as the abort handler function for abortable AT GSM FAX commands.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
boolean dsatgsmfax_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);


/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it.  It processes the fax parameter commands.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatgsmfax_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATGSMFAX_SET_STACK_PARAMS

DESCRIPTION
  This function updates the fax protocol stack parameters stored in
  the T.31 layer.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void dsatgsmfax_set_stack_params( void );

#endif  /* FEATURE_DATA_GCSD_FAX */


#endif /* DSATGSMFAX_H */
