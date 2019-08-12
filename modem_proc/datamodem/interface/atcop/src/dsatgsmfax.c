/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   

                ( G S M   F A X   C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the gsm fax
  commands.

EXTERNALIZED FUNCTIONS
  dsatgsmfax_exec_fts_cmd
    This function takes the result from the command line parser
    and executes it.  It excutes the Fax Transmit Silence command.

  dsatgsmfax_exec_frs_cmd
    This function takes the result from the command line parser
    and executes it.  It executes Fax Receive Silence command.

  dsatgsmfax_exec_fth_cmd
    This function takes the result from the command line parser
    and executes it.   It executes Fax Transmit HDLC frame command.

  dsatgsmfax_exec_frh_cmd
    This function takes the result from the command line parser
    and executes it. It executes Fax Receive HDLC frame command.

  dsatgsmfax_exec_ftm_cmd
    This function takes the result from the command line parser
    and executes it.  It executes Fax Transmit Message command.

  dsatgsmfax_exec_frm_cmd
    This function takes the result from the command line parser
    and executes it.  It executes Fax Receive Message command.

  dsatgsmfax_fps_t31_status_handler
    This function is the async event handler for async fax 
    functions.

  dsatgsmfax_abort_handler
    This function is the abort handlder for the abortable fax 
    action commands.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2012,2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatgsmfax.c_v   1.2   12 Jul 2002 10:08:50   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatgsmfax.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
03/11/14   tk      Optimized debug macros usage in ATCoP.
05/18/12   tk      Migrated to MSG 2.0 macros
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
12/15/09   nc      Mainling FEATURE_DATA_SERIALIZER.
04/29/09   ua      Fixed compiler warnings. 
12/17/08   sa      Fixed issue related to invalid modem_port during call end. 
05/26/08   sa      Allow FAX action commands only when FAX call is active.
06/22/07   sa      Changed tx_port setting from active port to modem port 
                   in dsatgsmfax_fps_t31_status_handler()
04/06/06   ar      Added support to updating fax protocol stack parameters.
10/28/02   sb      Fax action commands are applicable only in GSM mode.
01/10/02   wx      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX

#include "ds3gsiolib_ex.h"
#include "dsati.h"
#include "dsatgsmfax.h"
#include "dsatparm.h"
#include "err.h"
#include "amssassert.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "dsatetsictab.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include "dsfps.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Allow the FAX action commands only when FAX call is active */
#define CHECK_VALID_FAX_CMD() {\
  if((dsat_num_item_type)dsatutil_get_val(DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) == 0)\
  {\
    DS_AT_MSG0_ERROR("FAX call is not active");\
    return DSAT_ERROR;\
  }\
}
    
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL void fps_at_callback(dsat_result_enum_type result_code);

LOCAL void dsatgsmfax_exec_action_cmd
(
  ds_gcsd_cmd_enum_type     cmd
);

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION FPS_AT_CALLBACK

DESCRIPTION
  This function is the callback function for dsfps_at_exec_action_cmd,
  which runs fax action commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A ds command is submitted to ds task.  The command will be
  executed by the ATCOP command processor.

===========================================================================*/
LOCAL void fps_at_callback(dsat_result_enum_type result_code)
{
  ds_cmd_type               *cmd_ptr;
  ds_at_fps_t31_status_type *fps_t31_status = NULL;

  /* Allocate the command buffer and queue the command to GCSDPS */
  cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_fps_t31_status_type), FALSE);
  fps_t31_status = cmd_ptr->cmd_payload_ptr;

  DS_AT_MSG1_MED("GCSD-FPS: sending result code response %d to DSAT", result_code);
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_FPS_T31_CMD_STATUS;
  fps_t31_status->cmd_status = (int16) result_code;

  ds_put_cmd(cmd_ptr);
} /* fps_at_callback */



/*===========================================================================

FUNCTION DSATGSMFAX_EXEC_ACTION_CMD

DESCRIPTION
  This function invokes the fax prototcol state action command interface
  for asynchronous execution.  The command idenfifier and callback are
  passed.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatgsmfax_exec_action_cmd
(
  ds_gcsd_cmd_enum_type     cmd
)
{
  /* Invoke the fax protocol stack interface */
  dsfps_at_exec_action_cmd( cmd, fps_at_callback );
} /* dsatgsmfax_exec_action_cmd()*/


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
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  
  result = dsatparm_exec_param_cmd( mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr );

#ifdef FEATURE_MULTIPROCESSOR 
  if( (DSAT_OK == result) && (tok_ptr->op == (NA|EQ|AR)) )
  {
    /* Update the fax protocol stack */
    dsatgsmfax_set_stack_params();
  }
#endif /* ( FEATURE_MULTIPROCESSOR ) */
  
  return result;
}/* dsatgsmfax_exec_param_cmd () */



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
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_exec_fts_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FTS is not applicable in current mode");
    return DSAT_ERROR;
  }
  
  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FTS does not allow non-argument cmd");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FTS does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()

    DS_AT_MSG1_MED("DSAT: executing AT+FTS=%d", (dsat_num_item_type)
             dsatutil_get_val(DSATETSI_EXT_ACT_FTS_ETSI_IDX,0,0,NUM_TYPE));

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FTS_REQ );

    result = DSAT_ASYNC_CMD;
  }

  return result;
}/* dsatgsmfax_exec_fts_cmd () */



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
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FRS is not not applicable in current mode");
    return DSAT_ERROR;
  }

  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FRS does not allow non-argument cmd");
    return DSAT_ERROR;
  }
  
  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FRS does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()
    DS_AT_MSG1_LOW("DSAT: executing AT+FRS=%d", (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_FRS_ETSI_IDX,0,0,NUM_TYPE));

    /* The command is abortable during the command lifetime.
       Disabling the cmd-abort-complete prevents its abortibilty be 
       disabled when the fps calls atcop rsp to send CONNECT intermediate
       code. */
    dsatcmdp_disable_cmd_abort_complete();

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FRS_REQ );

    result = DSAT_ASYNC_CMD;
  }
  /* the command is not abortable for test command */
  else
  {
    dsatcmdp_command_abort_complete();
  }

  return result;
}/* dsatgsmfax_exec_frs_cmd */



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
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FTH is not not applicable in current mode");
    return DSAT_ERROR;
  }

  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FTH does not allow non-argument cmd");
    return DSAT_ERROR;
  }
  
  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FTH does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()
    
    DS_AT_MSG1_LOW("DSAT: AT+FTH=%d", (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_FTH_ETSI_IDX,0,0,NUM_TYPE));

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FTH_REQ );

    result = DSAT_ASYNC_CMD;
  }

  return result;
}/* dsatgsmfax_exec_fth_cmd () */



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
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FRH is not not applicable in current mode");
    return DSAT_ERROR;
  }
  
  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FRH does not allow non-argument cmd");
    return DSAT_ERROR;
  }
  
  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FRH does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()

    DS_AT_MSG1_LOW("DSAT: AT+FRH=%d", (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_FRH_ETSI_IDX,0,0,NUM_TYPE));

    /* The command is abortable during the command lifetime.
       Disabling the cmd-abort-complete prevents its abortibilty be 
       disabled when the fps calls atcop rsp to send CONNECT intermediate
       code. */
    dsatcmdp_disable_cmd_abort_complete();

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FRH_REQ );

    result = DSAT_ASYNC_CMD;
  }
  /* the command is not abortable for test command */
  else
  {
    dsatcmdp_command_abort_complete();
  }

  return result;
}/* dsatgsmfax_exec_frh_cmd () */



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
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FTM is not not applicable in current mode");
    return DSAT_ERROR;
  }

  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FTM does not allow non-argument cmd");
    return DSAT_ERROR;
  }
  
  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FTM does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()
    DS_AT_MSG1_LOW("DSAT: AT+FTM=%d", (dsat_num_item_type)dsatutil_get_val(
                               DSATETSI_EXT_ACT_FTM_ETSI_IDX,0,0,NUM_TYPE));

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FTM_REQ );

    result = DSAT_ASYNC_CMD;
  }

  return result;
}/* dsatgsmfax_exec_ftm_cmd () */



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
)
{
  dsat_result_enum_type result;

  ASSERT(mode != DSAT_ONLINE_DATA);
  
  if(DSAT_MODE_GSM != dsatcmdp_get_current_mode())
  {
    DS_AT_MSG0_MED("+FRM is not not applicable in current mode");
    return DSAT_ERROR;
  }

  if( (mode == DSAT_CMD) && (tok_ptr->op == (NA|EQ|AR)))
  {
    DS_AT_MSG0_MED("Fax action command is not expected in DSAT_CMD mode");
    return DSAT_ERROR;
  }

  if(tok_ptr->op == NA)
  {
    DS_AT_MSG0_MED("+FRM does not allow non-argument cmd");
    return DSAT_ERROR;
  }
  
  if(tok_ptr->op == (NA|QU))
  {
    DS_AT_MSG0_MED("+FRM does not allow query cmd");
    return DSAT_ERROR;
  }

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    CHECK_VALID_FAX_CMD()
    DS_AT_MSG1_LOW("DSAT: AT+FRM=%d", (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_FRM_ETSI_IDX,0,0,NUM_TYPE));

    /* The command is abortable during the command lifetime.
       Disabling the cmd-abort-complete prevents its abortibilty be 
       disabled when the fps calls atcop rsp to send CONNECT intermediate
       code. */
    dsatcmdp_disable_cmd_abort_complete();

    dsatgsmfax_exec_action_cmd( DS_GCSD_T31_AT_FRM_REQ );

    result = DSAT_ASYNC_CMD;
  }
  /* the command is not abortable for test command */
  else
  {
    dsatcmdp_command_abort_complete();
  }

  return result;
}/* dsatgsmfax_exec_frm_cmd () */



/*===========================================================================

FUNCTION DSATGSMFAX_FPS_T31_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async fax functions.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatgsmfax_fps_t31_status_handler
(
  ds_cmd_type * cmd_ptr
)
{
  ds3g_siolib_port_e_type port     = DS3G_SIOLIB_PORT_NONE;
  ds_at_fps_t31_status_type *fps_t31_status = dsat_get_cmd_payload_ptr(cmd_ptr);

  /* Configure SIO response handling */
  
  /* During FAX call, active port can be NONE,but modem port remains 
     VALID till the call ends. When call ends, the modem port will be 
     cleared.So setting the tx_port with modem port for sending the 
     result code if valid, else send it to DATA_PORT. */

  port = ds3g_siolib_ex_get_csd_port();
  
  if( DS3G_SIOLIB_PORT_NONE ==  port )
  {
    DS_AT_MSG0_ERROR("Modem port is INVALID, set port= DATA_PORT");
    port = DS3G_SIOLIB_DATA_PORT;
  }
  ds3g_siolib_set_tx_type( DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE,port );

  return (dsat_result_enum_type) fps_t31_status->cmd_status;
} /* dsatgsmfax_fps_t31_status_handler */



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
)
{
  /* dsfps_at_cmd_abort always return DSAT_NO_RSP */
  (void) dsfps_at_cmd_abort();
  return FALSE;
}

#endif  /* FEATURE_DATA_GCSD_FAX */

