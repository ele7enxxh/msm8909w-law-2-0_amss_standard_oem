/*===========================================================================

                         D S 7 0 7 _ C C M
GENERAL DESCRIPTION
  This file contains the congestion control manager functionality.
  Currently it runs in CFCM task context.

EXTERNALIZED FUNCTIONS
 DS707_CCM_POWERUP_INIT
   Performs power-up initialization of CCM.

 DS707_CCM_REGISTER_HANDLER
   Registers a client-specific handler function table, for the calling
   client. This function only applies to FMAC client.

 DS707_CCM_COMMON_REGISTER_HANDLER
   Registers a common client-specific handler function table, for the
   calling client. All the clients have to use this to register
   the call back functions except FMAC.

 DS707_CCM_CPU_FLOW_CONTROL_CMD_HDLR
   Command handler for reports received from CPU Monitors.

 DS707_CCM_MEM_FLOW_CONTROL_CMD_HDLR
   Command handler for reports received from memory watermarks.

 DS707_CCM_MEM_SEND_FLOW_CONTROL_CMD
   Sends a Flow Control command from memory watermark to CCM.

 DS707_CCM_THERM_FLOW_CONTROL_CMD_HDLR
   Command handler for reporting from therm monitor.

 DS707_CCM_THERM_RLFC_DELAY_TIMER_CB
   Function is called when the thermal RLFC delay Timer is expired

 DS707_GET_CCM_THERM_TO_RMAC_INFO_PTR
   Accessor function for ds707_ccm_therm_to_rmac_info

 DS707_GET_CCM_ARB_RMAC_INFO_PTR
   Accessor function for ds707_ccm_arb_rmac_info

 DS707_CCM_TCHUP_INIT
   Performs configuration and registration with other functional blocks
   such as CPU Monitors and FTC MAC FC.
   This function must be called when traffic channel is setup.

 DS707_CCM_TCHDN_CLEANUP
   Performs cleaning up.
   Must be called when the traffic channel is torn down.

 DS707_CCM_CONFIG_PARAMS
   Initializes the parameters that are used for configuring CPU Monitor
   FMAC FC and thermal events.

 DS707_GET_CCM_RLFC_STATE
   Accessor function for ds707_ccm_rlfc_state

 DS707_SET_CCM_RLFC_STATE
   Accessor function to set ds707_ccm_rlfc_state

INTERNAL FUNCTIONS
 DS707_CCMI_EVENT_HANDLER_CB
   Callback when various iface events occur.

 DS707_CCMI_CFCM_EVENT_CB
   Registers CCM with the CFCM for CPU and Thermal monitors for 
   flow control events.

 DS707_CCMI_DECIDE_FC_CMD_TO_FMAC
   Arbitrates incoming flow control reports, and sends the resulting
   flow control commands to FMAC FC.

DS707_CCMI_DECIDE_FC_CMD_TO_RMAC
   Arbitrates incoming flow control reports, and sends the resulting
   flow control commands to RMAC FC.

 DS707_CCMI_GET_MEM_CMD_BUF
   Gets the command buffer for a command received from a memory watermark.

 DS707_CCMI_PUT_MEM_CMD
   Puts a command in CCM's memory watermark command queue, and sets
   FC_CCM_MEM_FC_SIG.

 DS707_CCMI_DECIDE_THERM_FC_CMD_TO_RMAC_MUX
   Arbitrates incoming flow control commands, and decide payload size
   before sending to RMAC/MUX.

 DS707_CCMI_DECIDE_THERM_FC_CMD_TO_PA_DIVERSITY
   Arbitrates incoming flow control commands and forward command
   to PA and diversity functions in HDR MAC layers

 DS707_CCMI_THERMAL_CLEANUP
   Clean up for thermal side including thermal monitor deregistration,
   delay timer cleanup, and send commands to HDRRMAC/MUX layer to use
   default parameters

 DS707_CCMI_SEND_FC_CMD_TO_HDR_MAC
   Sends ccm command to HDR MAC layer

 DS707_CCMI_REGISTER_WITH_CFCM_EVENTS
   Registers CCM with CFCM to get event based callbacks

 DS707_CCMI_DEREGISTER_WITH_CFCM_EVENTS
   De-registers CCM with CFCM

 Copyright (c) 2008 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.c_v   1.2   19 Nov 2002 19:23:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_ccm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/21/15   sd         Fixed thermal state 0 handling in DS, so that HDR gets 
                      the correct FC command.
08/06/14   sd         Fix to correctly handle thermal state 2.
07/04/14   sd         Fix to avoid QTF failures.
07/02/14   sd         Interfacing with CFCM for CPU and Thermal based flow
                      control events.
06/03/13   sd         ATCoP decoupling changes
03/21/12   msh        Changes due to Core BSP changes
12/21/11   ss         Compiler Error Fix 
12/09/11   msh        corrected functions under FEATURE_HDR_FL_FLOW_CONTROL 
12/07/11   msh        Replaced FEATURE_HDR_RL_FLOW_CONTROL by FEATURE_DATA_RL_FLOW_CONTROL 
11/18/11   vpk        HDR SU API cleanup
09/16/11   vpk        Add support for NPA thermal
08/18/11   dvk        Fixed Compilation errors
07/29/11   vpk        Support memory flow control logging
04/08/11   ls         Add support thermal PA and Diverisy
02/26/11   ls         Global variable cleanup
02/11/11   ack        Merged ds707_ccm.c and ds707_ccmi.c and cleanup
01/31/11   sa         Made changes for Software decoupling.
09/17/10   ls         Fixed CW
07/12/10   jee        Klocwork fixes
04/27/10   ls         Add themal monitor PA support
10/09/08   sz         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "task.h"
#include "queue.h"
#include "rex.h"
#include "ds707_ccmi.h"
#include "ds707_cback.h"
#include "ds707_ccm_log.h"
#include "rcinit_rex.h"

#if defined (FEATURE_CCM_FLOW_CONTROL)
#include "fctask.h"
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
#include "hdrcom_api.h"
#include "hdrl1_api.h"
#include "hdrrlp.h"
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
#include "sys.h"
#include "ds707_pkt_mgr.h"
#include "rlgc.h"
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

#include "cfcm.h"
/*===========================================================================
                            VARIABLES
===========================================================================*/
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
/*--------------------------------------------------------------------------
  Variables for storing the configuration parameters for CPU Monitors and
  FMAC FC.
----------------------------------------------------------------------------*/
static ds707_ccm_fmac_flow_control_params_type ccm_fmac_config_params;

/*---------------------------------------------------------------------------
  Flag: registered with CPU Monitors or not
---------------------------------------------------------------------------*/
static boolean ds707_ccm_register_cpu_f;

/*---------------------------------------------------------------------------
  Queue for reports received from memory watermarks
---------------------------------------------------------------------------*/
static q_type ds707_ccm_mem_fc_cmd_q;
static q_type ds707_ccm_mem_fc_free_q;
static ds707_ccm_fc_cmd_type ds707_ccm_mem_fc_buf[DS707_CCM_MEM_FC_CMD_MAX];

/*--------------------------------------------------------------------------
  Table of function ptrs for each registered client e.g. FTC MAC FC.
  There are two functions for each client: one for configuration and
  the other for control.
----------------------------------------------------------------------------*/
static ds707_ccm_hdlr_func_tbl_type ds707_ccm_hdlr_tbl[1];

/*--------------------------------------------------------------------------
  Table of common function ptrs for each registered client
  There are two functions for each client: one for configuration and
  the other for control.
----------------------------------------------------------------------------*/
static ds707_ccm_hdlr_func_comm_tbl_type ds707_ccm_comm_hdlr_tbl[DS707_CCM_CLIENT_MAX];

/*--------------------------------------------------------------------------
  Variable for storing the last flow control command sent to FTC MAC FC.
----------------------------------------------------------------------------*/
static ds707_ccm_flow_control_cmd_enum_type ds707_ccm_out_cmd_fmac;

/*--------------------------------------------------------------------------
  Variable for storing the FC TCB pointer
----------------------------------------------------------------------------*/
static rex_tcb_type* fc_tcb_ptr;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

/*--------------------------------------------------------------------------
  Vector variable for storing the flow control reports received from each monitor.
----------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_FL_FLOW_CONTROL) || defined(FEATURE_CCM_THERM_FLOW_CONTROL) \
	|| defined(FEATURE_DATA_RL_FLOW_CONTROL)
static ds707_ccm_flow_control_cmd_enum_type ds707_ccm_in_cmd[DS707_CCM_MONITOR_MAX];
#endif

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
static ds707_ccm_flow_control_cmd_enum_type ds707_ccm_out_cmd_rmac;
static ds707_ccm_rlfc_state_enum_type ds707_ccm_rlfc_state;
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

/*---------------------------------------------------------------------------
  Holding payload size and cpu flow control flag and send to RMAC
---------------------------------------------------------------------------*/
static ds707_ccm_arb_rmac_type ds707_ccm_arb_rmac_info;

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
/*
 * PA backoff configuration
 */
static ds707_ccm_pa_backoff_flow_control_params_type ccm_pa_backoff_config_params;

/*---------------------------------------------------------------------------
  Holding thermal arbitration info
---------------------------------------------------------------------------*/
static ds707_ccm_therm_arb_type ds707_ccm_therm_to_rmac_info;
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
static void ds707_ccmi_event_handler_cb
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);

void ds707_ccmi_cfcm_event_cb 
(
  cfcm_cmd_type_s* client_req
);

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CCM_POWERUP_INIT

DESCRIPTION   This function performs power-up initialization of CCM. This includes
              initialization of some parameters that are used to configure other
              functional blocks such as CPU Monitors and FTC MAC FC.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccm_powerup_init
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CCM powerup init.");

#ifdef FEATURE_DATA_FL_FLOW_CONTROL

#ifdef FEATURE_MODEM_RCINIT_PHASE2
#ifndef TEST_FRAMEWORK
  fc_tcb_ptr = rcinit_lookup_rextask("fc"); 
  if ( RCINIT_NULL == fc_tcb_ptr)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_ERROR, 
                     "RCINIT Lookup for fc tcb returned NULL");
    ASSERT(0);
  }
#endif /* TEST_FRAMEWORK */
#else
  fc_tcb_ptr = &fc_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2*/

#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  /* Define a timer used for thermal events */
  rex_def_timer_ex( &ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer,
                    ds707_ccm_therm_rlfc_delay_timer_cb,
                    (uint32)NULL );
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */
  /*-------------------------------------------------------------
    Configure CCM parameters.
    Please note: the above NV read must happen before
    ds707_ccm_config_params()
  ---------------------------------------------------------------*/
  (void)ds707_ccm_config_params();

  /* Register phy link down event */
  if (ds707_cback_register( DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_ccmi_event_handler_cb) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  /* Register phy link up event */
  if (ds707_cback_register(DS707_CBACK_PHYS_LINK_UP_EV,
                           ds707_ccmi_event_handler_cb) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

} /* ds707_ccm_powerup_init */

/*===========================================================================
                        EXTERNAL FUNCTIONS FOR FL FLOW CONTROL
===========================================================================*/
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
/*===========================================================================
FUNCTION DS707_CCM_REGISTER_HANDLER

DESCRIPTION
  This function registers a FMAC client-specific handler function table, for the
  calling client. To consider backward compatibility, this function only applies
  for FMAC client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_ccm_register_handler
(
  ds707_ccm_client_enum_type     client,
  ds707_ccm_hdlr_func_tbl_type   *hdlr_tbl_ptr
)
{
  if ( client == DS707_CCM_FTC_MAC_FC )
  {
    ds707_ccm_hdlr_tbl[client].ds707_ccm_config_fmac_func =
      hdlr_tbl_ptr->ds707_ccm_config_fmac_func;
    ds707_ccm_hdlr_tbl[client].ds707_ccm_contrl_fmac_func =
      hdlr_tbl_ptr->ds707_ccm_contrl_fmac_func;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Failed to register with CCM. Invalid Client %d.",
                     client );
  }
} /* ds707_ccm_register_handler */
/*===========================================================================
FUNCTION DS707_CCM_COMMON_REGISTER_HANDLER

DESCRIPTION
  This function registers a common client-specific handler function table, for the
  calling client. All the clients have to use this to register
  the call back functions except FMAC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_ccm_common_register_handler
(
  ds707_ccm_client_enum_type     client,
  ds707_ccm_hdlr_func_comm_tbl_type   *hdlr_tbl_ptr
)
{
  if (( client < DS707_CCM_CLIENT_MAX ) ||
      ( client >= DS707_CCM_THERMAL_BACKOFF_FC ))
  {
    ds707_ccm_comm_hdlr_tbl[client].ds707_ccm_comm_config_func =
      hdlr_tbl_ptr->ds707_ccm_comm_config_func;
    ds707_ccm_comm_hdlr_tbl[client].ds707_ccm_comm_contrl_func =
      hdlr_tbl_ptr->ds707_ccm_comm_contrl_func;
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Failed to register with CCM. Invalid Client %d.",
                     client );
  }
} /* ds707_ccm_common_register_handler */

/*===========================================================================
FUNCTION      DS707_CCM_CPU_FLOW_CONTROL_CMD_HDLR

DESCRIPTION   Dummy function to resolve compilation issue with FC.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_cpu_flow_control_cmd_hdlr
(
  void
)
{
  /* This function is retained to resolve compilation issues with FC.
        This can be removed once FC code is cleaned up. */
}

/*===========================================================================
FUNCTION      DS707_CCM_CPU_FLOW_CONTROL_CMD_HDLR_EX

DESCRIPTION   Command handler for reports received from CPU Monitors.
              This function calls ds707_ccmi_decide_fc_cmd_to_fmac and
              ds707_ccmi_decide_fc_cmd_to_rmac
              to arbitrate multiple reports. This function is running
              under CFCM task

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_cpu_flow_control_cmd_hdlr_ex
(
  ds707_ccm_flow_control_cmd_enum_type cpu_fc_cmd
)
{

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Rcvd cmd %0x for CPU monitor.", cpu_fc_cmd);

  if ( ds707_ccm_in_cmd[DS707_CCM_CPU_MODEM] != cpu_fc_cmd )
  {
    // new command
    ds707_ccm_in_cmd[DS707_CCM_CPU_MODEM] = cpu_fc_cmd;
    ds707_ccmi_decide_fc_cmd_to_fmac();
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
    ds707_ccmi_decide_fc_cmd_to_rmac();
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */
  }
  
} /* ds707_ccm_cpu_flow_control_cmd_hdlr_ex */

/*===========================================================================
FUNCTION      DS707_CCM_MEM_FLOW_CONTROL_CMD_HDLR

DESCRIPTION   Command handler for reports from memory watermarks.
              This function calls ds707_ccmi_decide_fc_cmd_to_fmac.
              This function is running under FC task

DEPENDENCIES  This function should be called when the FC_CCM_MEM_SIG is set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_mem_flow_control_cmd_hdlr
(
  void
)
{
  ds707_ccm_fc_cmd_type *cmd_ptr = NULL;
  ds707_ccm_flow_control_cmd_enum_type fc_cmd;

  /* Loop through the commands from memory watermarks */
  while ((cmd_ptr = (ds707_ccm_fc_cmd_type *)(q_get(&ds707_ccm_mem_fc_cmd_q))) != NULL)
  {
    /* Store wmk CMD locally before releasing the CMD buffer. */
    uint8 wmk_idx = (uint8)(cmd_ptr->monitor_idx);
    fc_cmd = cmd_ptr->fc_cmd;
    q_put( &ds707_ccm_mem_fc_free_q, &cmd_ptr->link );
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Rcvd cmd %0x from watermark %d", fc_cmd, wmk_idx);

    if (DS707_CCM_MONITOR_MAX <= wmk_idx)
    {
      /* Invalid wmk cmd index */
      return;
    }

    if ( ds707_ccm_in_cmd[wmk_idx] != fc_cmd ) // this is a double check
    {
      // new command
      ds707_ccm_in_cmd[wmk_idx] = fc_cmd;
      //Log FC 
      ds707_ccm_log_memory(fc_cmd);
      ds707_ccmi_decide_fc_cmd_to_fmac();
    }
  }
} /* ds707_ccm_mem_flow_control_cmd_hdlr */

/*===========================================================================
FUNCTION: DS707_CCM_MEM_SEND_FLOW_CONTROL_CMD

DESCRIPTION:
  This function sends a Flow Control command from memory watermark to CCM.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void ds707_ccm_mem_send_flow_control_cmd
(
  ds707_ccm_monitor_enum_type wmk,
  ds707_ccm_flow_control_cmd_enum_type cmd
)
{
  if ( ds707_ccm_in_cmd[(uint8)wmk] != cmd )
  {
    // new command
    ds707_ccm_fc_cmd_type *fc_ptr = ds707_ccmi_get_mem_cmd_buf();

    if (fc_ptr)
    {
      fc_ptr->monitor_idx = wmk;
      fc_ptr->fc_cmd = cmd;

      ds707_ccmi_put_mem_cmd(fc_ptr);

      DATA_IS707_MSG0(MSG_LEGACY_MED, "WMK sends fc cmd to CCM");
    }
    else
    {
      ERR_FATAL("Failed to alloc mem for FC cmd",0,0,0);
    }
  }
} /* ds707_ccm_mem_send_flow_control_cmd */

/*===========================================================================
                        INTERNAL FUNCTIONS FOR FL FLOW CONTROL
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CCMI_CONFIG_CLIENTS_PARAMS

DESCRIPTION   This function configures all the registered clients with init
                        parameters for FMAC and Thermal Backoff.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_config_clients_params
(
  void
)
{
  if ( ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_config_fmac_func != NULL )
  {
    ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_config_fmac_func(&ccm_fmac_config_params);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not get config function ptr for FTC_MAC client");
  }

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  if ( ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_BACKOFF_FC].ds707_ccm_comm_config_func 
         != NULL )
  {
    ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_BACKOFF_FC].ds707_ccm_comm_config_func(
      (void *)&ccm_pa_backoff_config_params);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "Could not get config function ptr for PA backoff client");
  }
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

  if ( ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_DIVERSITY_FC].ds707_ccm_comm_config_func
       != NULL )
  {
    ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_DIVERSITY_FC].ds707_ccm_comm_config_func(NULL);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Could not get config function ptr for Diversity client");
  }

} /* ds707_ccmi_config_client_params */

/*===========================================================================
FUNCTION      DS707_CCMI_DECIDE_FC_CMD_TO_FMAC

DESCRIPTION   This function arbitrates incoming flow control commands, and
              sends the resulting commands to FMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_decide_fc_cmd_to_fmac
(
  void
)
{
  uint8 i;
  ds707_ccm_flow_control_cmd_enum_type tmp = DS707_CCM_FLOW_OFF;
  //ds707_ccm_hdlr_func_tbl_type *hdlr_tbl_entry_ptr;

  for (i = 0; i <= DS707_CCM_MEM_DSM; i++) {
      tmp = (ds707_ccm_flow_control_cmd_enum_type)(tmp | ds707_ccm_in_cmd[i]);
  }
  if (tmp != ds707_ccm_out_cmd_fmac) {
      // not the same command as previous one
      ds707_ccm_out_cmd_fmac = tmp;

      if (ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_contrl_fmac_func != NULL )
        {
          DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                          "CCM arbitration done. Send new cmd %d to FMAC", 
                          ds707_ccm_out_cmd_fmac);
          ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_contrl_fmac_func(ds707_ccm_out_cmd_fmac);
        }
      else
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not get function ptr for FMAC command.");
  }
} /* ds707_ccmi_decide_fc_cmd_to_fmac */

/*===========================================================================
FUNCTION      DS707_CCMI_GET_MEM_CMD_BUF

DESCRIPTION   This function gets the command buffer for a command received from
              a memory watermark.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds707_ccm_fc_cmd_type *ds707_ccmi_get_mem_cmd_buf
(
  void
)
{
  ds707_ccm_fc_cmd_type    *cmd_ptr;                /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a command buffer from the free command queue.
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = (ds707_ccm_fc_cmd_type *) q_get( &ds707_ccm_mem_fc_free_q )) == NULL )
  {
    /*-----------------------------------------------------------------------
      No free command buffers available, log an error.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No items on ds707_ccm_mem_fc_free_q");
  }

  /*-------------------------------------------------------------------------
    Note that the cmd_ptr may be NULL if there were no free command buffers
    available. The calling task must either handle the NULL return value
    or ERR_FATAL.
  -------------------------------------------------------------------------*/
  return( cmd_ptr );


} /* ds707_ccmi_get_mem_cmd_buf */

/*===========================================================================
FUNCTION      DS707_CCMI_PUT_MEM_CMD

DESCRIPTION   This function puts a command in CCM's memory watermark command
              queue, and sets FC_CCM_MEM_SIG.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccmi_put_mem_cmd
(
  ds707_ccm_fc_cmd_type *cmd_ptr
)
{
  /* initialize link field */
  (void) q_link(cmd_ptr, &cmd_ptr->link);

  /* put item on queue */
  q_put(&ds707_ccm_mem_fc_cmd_q, &cmd_ptr->link);

  /* Set signal */
  (void) rex_set_sigs(fc_tcb_ptr, FC_CCM_MEM_SIG);
} /* ds707_ccmi_put_mem_cmd */

#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

/*===========================================================================
                        EXTERNAL FUNCTIONS FOR THERMAL FLOW CONTROL
===========================================================================*/
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
/*===========================================================================
FUNCTION      DS707_CCM_THERM_FLOW_CONTROL_CMD_HDLR

DESCRIPTION   Dummy function to resolve compilation issue with FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_therm_flow_control_cmd_hdlr
(
  void
)
{
  /* This function is retained to resolve compilation issues with FC.
        This can be removed once FC code is cleaned up. */
}

/*===========================================================================
FUNCTION      DS707_CCM_THERM_FLOW_CONTROL_CMD_HDLR_EX

DESCRIPTION   Command handler for reporting from therm monitor. This function
                        is running under CFCM task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_therm_flow_control_cmd_hdlr_ex
(
  ds707_ccm_flow_control_cmd_enum_type cfcm_cmd,
  uint32                               step_timer
)
{
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Rcvd cmd %d for thermal monitor.", 
  	               cfcm_cmd);

  /* If it is the expiration of time out, we need to invoke the arbitrition again */
  if ( cfcm_cmd == DS707_CCM_FLOW_TIME_OUT )
  {
    ds707_ccmi_decide_therm_fc_cmd_to_rmac_mux(step_timer);
  }

  /* Check the cmd and process the cmd if it has been changed */
  else if ( ds707_ccm_in_cmd[DS707_CCM_THERM] != cfcm_cmd ) // this is a double check
  {
    // new command
    ds707_ccm_in_cmd[DS707_CCM_THERM] = cfcm_cmd;
    ds707_ccmi_decide_therm_fc_cmd_to_rmac_mux(step_timer);
    ds707_ccmi_decide_therm_fc_cmd_to_pa_diversity();
  }
    
} /* ds707_ccm_therm_flow_control_cmd_hdlr_ex */

/*===========================================================================
FUNCTION       DS707_CCM_THERM_RLFC_DELAY_TIMER_CB

DESCRIPTION    Function is called when the thermal RLFC delay Timer
               is expired. This function is running under Timer task

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_ccm_therm_rlfc_delay_timer_cb
(
  unsigned long param /*param unused */   //What is its use here ???
)
{
  cfcm_default_step_timer_type_s step_timer_val = {0};
  boolean ret_val = FALSE;

  DATA_IS707_MSG0(MSG_LEGACY_MED, "CB: THERM RLFC DELAY expired");

  ret_val = cfcm_client_get_step_timer_vals(CFCM_CLIENT_HDR_DS, &step_timer_val);
  if(ret_val != TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "CFCM failed to provide data");
    step_timer_val.thermal_step_timer = (uint8)DS707_CCM_THERM_STEP_TIMER_MSEC;
  }

  ds707_ccm_therm_flow_control_cmd_hdlr_ex(DS707_CCM_FLOW_TIME_OUT,
  	                                        step_timer_val.thermal_step_timer);
  return;
} /* ds707_ccm_therm_rlfc_delay_timer_cb() */

/*===========================================================================
FUNCTION      DS707_GET_CCM_THERM_TO_RMAC_INFO_PTR

DESCRIPTION   Accessor function for ds707_ccm_therm_to_rmac_info

DEPENDENCIES  None

RETURN VALUE  Returns ptr to ds707_ccm_therm_to_rmac_info

SIDE EFFECTS  None
===========================================================================*/
ds707_ccm_therm_arb_type* ds707_get_ccm_therm_to_rmac_info_ptr
(
  void
)
{
  return &ds707_ccm_therm_to_rmac_info;
} /* ds707_get_ccm_therm_to_rmac_info_ptr */

/*===========================================================================
                        INTERNAL FUNCTIONS FOR THERMAL FLOW CONTROL
===========================================================================*/
 /*===========================================================================
 FUNCTION      DS707_CCMI_DECIDE_THERM_FC_CMD_TO_RMAC_MUX
 
 DESCRIPTION   This function arbitrates incoming flow control commands, and
               decide payload size before sending to RMAC/MUX. This function
               is running under CFCM task.
 
 DEPENDENCIES  None
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 ===========================================================================*/
 void ds707_ccmi_decide_therm_fc_cmd_to_rmac_mux
 (
   uint32 step_timer
 )
 {
#ifdef FEATURE_DATA_RL_FLOW_CONTROL  
   rex_timer_cnt_type therm_timer = 0;
   uint8 payload = ds707_ccm_arb_rmac_info.payload;
#endif  
   sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NO_SRV;
 
   DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                   "CCM arbitration: cmd %0x to RMAC/1xMUX",
                   ds707_ccm_in_cmd[DS707_CCM_THERM]);
 
   sys_mode = ds707_pkt_mgr_get_current_sys_mode();
   DATA_IS707_MSG1(MSG_LEGACY_MED,
                   "CCM arbitration in sys mode %d", sys_mode);
 
   /* Check the sys mode acquired and send request for DO/1xRTT */
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
   if (SYS_SYS_MODE_HDR == sys_mode )
   {
     switch (ds707_ccm_in_cmd[DS707_CCM_THERM])
     {
       case DS707_CCM_FLOW_DOWN:
         case DS707_CCM_FLOW_DOWN_MORE:
           if (ds707_ccm_therm_to_rmac_info.therm_rlfc == RLFC_OFF)
           {
             payload = DS707_CCM_MIN_PAYLOAD;
             ds707_ccm_therm_to_rmac_info.therm_rlfc = RLFC_ON;
             therm_timer = step_timer + DS707_CCM_THERM_DELAY_MSEC;
             (void) rex_set_timer( &ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer,
                                   therm_timer );
           }
           else if (ds707_ccm_therm_to_rmac_info.therm_rlfc == RLFC_ON)
           {
             payload =
               MAX(DS707_CCM_MIN_PAYLOAD,
                   ds707_ccm_arb_rmac_info.payload-1);
             therm_timer = step_timer + DS707_CCM_THERM_DELAY_MSEC;
             (void) rex_set_timer( &ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer,
                                   therm_timer );
           }
           break;
 
         case DS707_CCM_FLOW_OFF:
           if (ds707_ccm_therm_to_rmac_info.therm_rlfc == RLFC_ON)
           {
             payload =
               MIN(ds707_ccm_arb_rmac_info.payload+1,
                   DS707_CCM_MAX_PAYLOAD);
             therm_timer = step_timer + DS707_CCM_THERM_DELAY_MSEC;
             (void) rex_set_timer( &ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer,
                                   therm_timer );
             if ( payload == DS707_CCM_MAX_PAYLOAD )
             {
               (void) rex_clr_timer(&ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer);
               ds707_ccm_therm_to_rmac_info.therm_rlfc = RLFC_OFF;
               DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CCM clear therm flow control delay timer");
             }
           }
           break;
 
         case DS707_CCM_FLOW_SHUT_DOWN:
           payload = 0;
           (void) rex_clr_timer(&ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer);
           ds707_ccm_therm_to_rmac_info.therm_rlfc = RLFC_OFF;
           DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CCM clear therm flow control delay timer");
           break;
 
         default:
           DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Wrong Flow Control Command");
           return;
       }
 
       DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                       "CCM set therm flow control delay timer: %d state: %d "
                       "payload restriction level: %d",
                       therm_timer,
                       ds707_ccm_therm_to_rmac_info.therm_rlfc,
                       payload);
 
       // Send the request to HDRRMAC
       if ( payload != ds707_ccm_arb_rmac_info.payload )
       {
         DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                         "CCM send request to HDRMAC. "
                         "Payload restriction level:from %d to %d, cpu flow control:%d",
                         ds707_ccm_arb_rmac_info.payload,
                         payload,
                         ds707_ccm_arb_rmac_info.enable_cpu_flow_control);
         ds707_ccm_arb_rmac_info.payload = payload;
         hdrl1_rmac_flow_control( ds707_ccm_arb_rmac_info.payload,
                               ds707_ccm_arb_rmac_info.enable_cpu_flow_control );
       }
   }
   else
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */
   if ( SYS_SYS_MODE_CDMA == sys_mode )
   {
     if (( ds707_ccm_in_cmd[DS707_CCM_THERM] == DS707_CCM_FLOW_DOWN )||
         ( ds707_ccm_in_cmd[DS707_CCM_THERM] == DS707_CCM_FLOW_SHUT_DOWN ))
     {
       //send FALSE to MUX to disble SCRM/R-SCH
       rpc_set_scrm_allowed( FALSE );
       DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                       "CCM sends request to MUX to disable SCRM/R-SCH");
     }
     else
     {
       //send TRUE to MUX to enable SCRM/R-SCH
       rpc_set_scrm_allowed( TRUE );
       DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                       "CCM sends request to MUX to enable SCRM/R-SCH");
     }
   } /* switch */
 } /* ds707_ccmi_decide_therm_fc_cmd_to_rmac_mux */
 
 /*===========================================================================
 FUNCTION      DS707_CCMI_DECIDE_THERM_FC_CMD_TO_PA_DIVERSITY
 
 DESCRIPTION   This function arbitrates incoming flow control commands and
               forward command to PA and diversity functions in HDR MAC
               layers
 
 DEPENDENCIES  None
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 ===========================================================================*/
 void ds707_ccmi_decide_therm_fc_cmd_to_pa_diversity
 (
   void
 )
 {
   sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NO_SRV;
 
   DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                   "CCM arbitration: cmd %0x to PA/Diversity",
                   ds707_ccm_in_cmd[DS707_CCM_THERM]);
 
   sys_mode = ds707_pkt_mgr_get_current_sys_mode();
   DATA_IS707_MSG1(MSG_LEGACY_MED,
                   "CCM arbitration in sys mode %d", sys_mode);
 
     /* Check the sys mode acquired and send request for DO/1xRTT */
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
   if (SYS_SYS_MODE_HDR == sys_mode )
   {
     switch (ds707_ccm_in_cmd[DS707_CCM_THERM])
     {
       case DS707_CCM_FLOW_DOWN:
         /* We do not send this cmd to HDR */
         break;
 
       case DS707_CCM_FLOW_DOWN_MORE:
       case DS707_CCM_FLOW_OFF:
       case DS707_CCM_FLOW_SHUT_DOWN:
         /*
          * Need to Start Tx power backoff for PA and Disable Rx Diversity
          */
         ds707_ccmi_send_fc_cmd_to_hdr_mac(ds707_ccm_in_cmd[DS707_CCM_THERM]);
         break;
 
       default:
         DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Wrong Flow Control Command");
         break;
       }
   }
   else
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */
   {
     DATA_IS707_MSG0(MSG_LEGACY_MED,
                     "CCM arbitration is in non-HDR mode for PA and Diverisy. Ignore.");
   }
 } /* ds707_ccmi_decide_therm_fc_cmd_to_pa_diversity */
 
 /*===========================================================================
 FUNCTION      DS707_CCMI_THERMAL_CLEANUP
 
 DESCRIPTION   This function does clean up for thermal side including
               thermal monitor deregistration, delay timer cleanup,
               and send commands to HDRRMAC/MUX layer to use
               default parameters
 
 DEPENDENCIES  None.
 
 RETURN VALUE  None.
 
 SIDE EFFECTS  None.
 ===========================================================================*/
 void ds707_ccmi_thermal_cleanup
 (
   void
 )
 {
   // clear delay timer used by thermal
   (void) rex_clr_timer(&ds707_ccm_therm_to_rmac_info.therm_rlfc_delay_timer);
 
#ifdef FEATURE_DATA_RL_FLOW_CONTROL 
   DATA_IS707_MSG0(MSG_LEGACY_MED,
                   "CCM Thermal cleanup is sending default value to HDR RMAC");
     //send default value to HDRRMAC
   hdrl1_rmac_flow_control( DS707_CCM_MAX_PAYLOAD,
                         FALSE );
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */
 
   DATA_IS707_MSG0(MSG_LEGACY_MED,
                   "CCM Thermal cleanup is sending default value to 1xMUX");
   //send default value to MUX
   rpc_set_scrm_allowed( TRUE );
 
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
   //Send OFF command to HDR MAC
   ds707_ccmi_send_fc_cmd_to_hdr_mac(DS707_CCM_FLOW_OFF);
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */
 }
 
 /*===========================================================================
 FUNCTION      DS707_CCMI_SEND_FC_CMD_TO_HDR_MAC
 
 DESCRIPTION   This function sends ccm command to HDR MAC layer
 
 DEPENDENCIES  None.
 
 RETURN VALUE  None.
 
 SIDE EFFECTS  None.
 ===========================================================================*/
 void ds707_ccmi_send_fc_cmd_to_hdr_mac
 (
   ds707_ccm_flow_control_cmd_enum_type cmd
 )
 {
   DATA_IS707_MSG1(MSG_LEGACY_MED,
                   "CCM arbitration: Sending CMD %x to HDR MAC",
                   cmd );
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
   if ( ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_BACKOFF_FC].ds707_ccm_comm_contrl_func
        != NULL )
   {
     ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_BACKOFF_FC].ds707_ccm_comm_contrl_func(cmd);
   }
   else
   {
     DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                     "Could not get ctrl function ptr for PA backoff client");
   }
 
   if ( ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_DIVERSITY_FC].ds707_ccm_comm_contrl_func
        != NULL )
   {
     ds707_ccm_comm_hdlr_tbl[DS707_CCM_THERMAL_DIVERSITY_FC].ds707_ccm_comm_contrl_func(cmd);
   }
   else
   {
     DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                     "Could not get ctrl function ptr for Diversity client");
   }
 #endif /* FEATURE_DATA_FL_FLOW_CONTROL */
 
 } /* ds707_ccmi_send_fc_cmd_to_hdr_mac */

#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

/*===========================================================================
                        EXTERNAL FUNCTIONS FOR RL FLOW CONTROL
===========================================================================*/
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
/*===========================================================================
FUNCTION      DS707_GET_CCM_RLFC_STATE

DESCRIPTION   Accessor function for ds707_ccm_rlfc_state

DEPENDENCIES  None

RETURN VALUE  Returns value of ds707_ccm_rlfc_state

SIDE EFFECTS  None
===========================================================================*/
ds707_ccm_rlfc_state_enum_type ds707_get_ccm_rlfc_state
(
  void
)
{
  return ds707_ccm_rlfc_state;
} /* ds707_get_ccm_rlfc_state */

/*===========================================================================
FUNCTION      DS707_SET_CCM_RLFC_STATE

DESCRIPTION   Accessor function to set ds707_ccm_rlfc_state

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_set_ccm_rlfc_state
(
  ds707_ccm_rlfc_state_enum_type rlfc_state
)
{
  if ( (RLFC_INVALID < rlfc_state) && (RLFC_MAX > rlfc_state) )
  {
    ds707_ccm_rlfc_state = rlfc_state;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid RLFC State: %d passed",
                    rlfc_state);
  }
  return;
} /* ds707_set_ccm_rlfc_state */

/*===========================================================================
                        INTERNAL FUNCTIONS FOR RL FLOW CONTROL
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CCMI_DECIDE_FC_CMD_TO_RMAC

DESCRIPTION   This function arbitrates incoming flow control commands, and
              sends the resulting commands to RMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_decide_fc_cmd_to_rmac
(
  void
)
{
  ds707_ccm_flow_control_cmd_enum_type tmp = DS707_CCM_FLOW_OFF;

  tmp = ds707_ccm_in_cmd[DS707_CCM_CPU_MODEM];
  if (tmp != ds707_ccm_out_cmd_rmac)
  {
      // not the same command as previous one
    ds707_ccm_out_cmd_rmac = tmp;
    DATA_IS707_MSG2(MSG_LEGACY_HIGH,
          "CCM arbitration done. Send new cmd %0x to RMAC. CCM RLFC state %d",
          ds707_ccm_out_cmd_rmac, ds707_ccm_rlfc_state);

    switch (ds707_ccm_out_cmd_rmac)
    {
      case DS707_CCM_FLOW_DOWN:
        if (ds707_ccm_rlfc_state == RLFC_OFF)
        {
          (void) rex_set_timer( get_hdrrlp_rlfc_delay_timer_ptr(),
                 DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC);
          ds707_ccm_rlfc_state = RLFC_DELAY;
          DATA_IS707_MSG1(MSG_LEGACY_HIGH, "CCM set RL flow control delay timer: %d",
                          DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC);
        }
        else if (ds707_ccm_rlfc_state == RLFC_FREEZE)
        {
          ds707_ccm_rlfc_state = RLFC_DELAY;
        }
        break;

      case DS707_CCM_FLOW_FREEZE:
        if (ds707_ccm_rlfc_state == RLFC_DELAY)
        {
          (void) rex_set_timer( get_hdrrlp_rlfc_delay_timer_ptr(),
                 DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC);
          ds707_ccm_rlfc_state = RLFC_FREEZE;
          DATA_IS707_MSG1(MSG_LEGACY_HIGH, "CCM set RL flow control delay timer: %d",
                          DS707_CCM_CPU_REPORT_PERIOD_DEFAULT * 50 + DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC);
        }
        break;

      case DS707_CCM_FLOW_UP:
      case DS707_CCM_FLOW_OFF:
        if (ds707_ccm_rlfc_state == RLFC_FREEZE ||
            ds707_ccm_rlfc_state == RLFC_DELAY)
        {
          (void) rex_clr_timer(get_hdrrlp_rlfc_delay_timer_ptr());
          ds707_ccm_rlfc_state = RLFC_OFF;
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CCM clear RL flow control delay timer");
        }
        else if (ds707_ccm_rlfc_state == RLFC_ON)
        {
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
          ds707_ccm_arb_rmac_info.enable_cpu_flow_control = FALSE;
          //RL flow control disable
          hdrl1_rmac_flow_control( ds707_ccm_arb_rmac_info.payload,
                                ds707_ccm_arb_rmac_info.enable_cpu_flow_control );
#else
          hdrl1_rmac_stop_flow_control();
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

          ds707_ccm_rlfc_state = RLFC_OFF;
        }

        break;

      default:
        /* Invalid command */
        break;
    }

    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "CCM set RL flow control state: %d",
                    ds707_ccm_rlfc_state);
  }
} /* ds707_ccmi_decide_fc_cmd_to_rmac */

#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_GET_CCM_ARB_RMAC_INFO_PTR

DESCRIPTION   Accessor function for ds707_ccm_arb_rmac_info

DEPENDENCIES  None

RETURN VALUE  Returns ptr to ds707_ccm_arb_rmac_info

SIDE EFFECTS  None
===========================================================================*/
ds707_ccm_arb_rmac_type* ds707_get_ccm_arb_rmac_info_ptr
(
  void
)
{
  return &ds707_ccm_arb_rmac_info;
} /* ds707_get_ccm_arb_rmac_info_ptr */

/*===========================================================================
FUNCTION      DS707_CCM_TCHUP_INIT

DESCRIPTION   This function performs configuration and registration with other
              functional blocks such as CPU Monitors, FTC MAC FC, RMAC_FC
              and thermal monitors.
              This function must be called when traffic channel is setup.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccm_tchup_init (void)
{

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
   uint8 i;
  /*--------------------------------------------------------------
    Configure all registered clients with the parameters.
  ----------------------------------------------------------------*/
  ds707_ccmi_config_clients_params();

  /*--------------------------------------------------------------
    Initialize variables of the flow control commands.
  ----------------------------------------------------------------*/
  for (i = 0; i < DS707_CCM_MONITOR_MAX; i++)
  {
    ds707_ccm_in_cmd[i] = DS707_CCM_FLOW_OFF;
  }
  ds707_ccm_out_cmd_fmac = DS707_CCM_FLOW_OFF;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

#ifdef FEATURE_DATA_RL_FLOW_CONTROL
  ds707_ccm_out_cmd_rmac = DS707_CCM_FLOW_OFF;
  ds707_ccm_rlfc_state = RLFC_OFF;
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

  ds707_ccm_arb_rmac_info.payload = DS707_CCM_MAX_PAYLOAD;
  ds707_ccm_arb_rmac_info.enable_cpu_flow_control = FALSE;

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  /*-------------------------------------------------------------
    Set ccm_thermal_events_monitor flag. This will trigger
    CCM to handle thermal events if there is any
    ------------------------------------------------------------*/
  ds707_ccm_therm_to_rmac_info.therm_rlfc = RLFC_OFF;
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */ 

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
  /*--------------------------------------------------------------
    Register to CPU and thermal events. Also configure their parameters.
  ----------------------------------------------------------------*/
  ds707_ccmi_register_with_cfcm_events();
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Traffic channel is UP. Configuration is Done.");
} /* ds707_ccm_tchup_init() */

/*===========================================================================
FUNCTION      DS707_CCM_TCHDN_CLEANUP

DESCRIPTION   This function performs cleaning up.
              This function must be called when the traffic channel is torn
              down.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccm_tchdn_cleanup (void)
{ 
#ifdef FEATURE_DATA_FL_FLOW_CONTROL  
  ds707_ccm_fc_cmd_type *mem_cmd_ptr = NULL;

  ds707_ccmi_deregister_with_cfcm_events();

  /*--------------------------------------------------------------
    Clean up CCM MEM cmd queues
   ----------------------------------------------------------------*/
  if (ds707_ccm_mem_fc_cmd_q.cnt)
  {
    while ((mem_cmd_ptr = (ds707_ccm_fc_cmd_type *)q_get(&ds707_ccm_mem_fc_cmd_q)) != NULL)
    {
      q_put( &ds707_ccm_mem_fc_free_q, &mem_cmd_ptr->link );
    }
  }

  /*--------------------------------------------------------------
    Send OFF command to FMAC FC.
  ----------------------------------------------------------------*/
  if ( ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_contrl_fmac_func != NULL )
      ds707_ccm_hdlr_tbl[DS707_CCM_FTC_MAC_FC].ds707_ccm_contrl_fmac_func(DS707_CCM_FLOW_OFF);
  else
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not get function for FMAC control.");

  /* Log memory flow */
  ds707_ccm_log_memory(DS707_CCM_FLOW_OFF);
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  ds707_ccmi_thermal_cleanup();

#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

  /* Clean up the delay timer for CPU RMAC*/
#ifdef FEATURE_DATA_RL_FLOW_CONTROL
  (void) rex_clr_timer(get_hdrrlp_rlfc_delay_timer_ptr());
#endif /* FEATURE_DATA_RL_FLOW_CONTROL */

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Traffic channel is DOWN. Deregistration is Done.");

} /* ds707_ccm_tchdn_cleanup */

/*===========================================================================
FUNCTION      DS707_CCM_CONFIG_PARAMS

DESCRIPTION   This function initializes the parameters that are used for
              configuring CPU Monitor, FMAC FC and  RMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_config_params(void)
{
#ifdef FEATURE_CCM_THERM_FLOW_CONTROL  
  cfcm_default_step_timer_type_s step_timer_val = {0};
  boolean                        ret_val = FALSE;
#endif
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
  uint8 i = 0;
#endif

  DATA_IS707_MSG0(MSG_LEGACY_LOW, "CCM configure parameters Using default.");

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
  ccm_fmac_config_params.init_reduction_pattern = DS707_CCM_FMAC_INIT_PATTERN_DEFAULT;
  ccm_fmac_config_params.max_reduction_pattern = DS707_CCM_FMAC_MAX_PATTERN_DEFAULT;
  ccm_fmac_config_params.delay_time_ms = DS707_CCM_FMAC_DELAY_MS_DEFAULT;
  ccm_fmac_config_params.period_ms = DS707_CCM_FMAC_PERIOD_MS_DEFAULT;

  
  (void)q_init(&ds707_ccm_mem_fc_cmd_q);
  (void)q_init(&ds707_ccm_mem_fc_free_q);

  for( i = 0; i < DS707_CCM_MEM_FC_CMD_MAX; i++ )
  {
    (void)q_link( &ds707_ccm_mem_fc_buf[i], &ds707_ccm_mem_fc_buf[i].link );
    q_put( &ds707_ccm_mem_fc_free_q, &ds707_ccm_mem_fc_buf[i].link );
  }

  ds707_ccm_register_cpu_f = FALSE;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
  /* Set up the configuration parameters */
  ret_val = cfcm_client_get_step_timer_vals(CFCM_CLIENT_HDR_DS, &step_timer_val);
  if(ret_val != TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "CFCM failed to provide data");
    step_timer_val.thermal_step_timer = (uint8)DS707_CCM_THERM_STEP_TIMER_MSEC;
  }
  
  ccm_pa_backoff_config_params.step_down_timer_ms = step_timer_val.thermal_step_timer;
  ccm_pa_backoff_config_params.step_up_timer_ms = step_timer_val.thermal_step_timer;  
  ccm_pa_backoff_config_params.delay_time_ms = DS707_CCM_THERM_DELAY_MSEC;
  ccm_pa_backoff_config_params.default_tx_power_bo_pattern_number = DS707_CCM_THERM_DTPBPN;
  ccm_pa_backoff_config_params.max_tx_power_bo_pattern_number = DS707_CCM_THERM_MTPBPN;
  ccm_pa_backoff_config_params.tx_power_bo_period_ms = DS707_CCM_THERM_TPBP_MSEC;
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

} /* ds707_ccm_config_params */

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_CCMI_EVENT_HANDLER_CB

DESCRIPTION    Callback when various iface events occur.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
/* ARGSUSED */
static void ds707_ccmi_event_handler_cb
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, "CCM rcvd phys link event : %d",event);

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_UP_EV:
      ds707_ccm_tchup_init ();
      break;

    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      ds707_ccm_tchdn_cleanup ();
      break;

    default:
      /* Unknown event */
      break;
  }
} /* ds707_ccmi_event_handler_cb */

/*===========================================================================
FUNCTION      DS707_CCMI_REGISTER_WITH_CFCM_EVENTS

DESCRIPTION   Registers CCM with CFCM to get CFCM events for CPU and Thermal monitors.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_register_with_cfcm_events (void)
{
  cfcm_reg_req_type_s ds707_ccm_client_reg_req;
    
  ds707_ccm_client_reg_req.client_id = CFCM_CLIENT_HDR_DS;
  ds707_ccm_client_reg_req.req_umid = 0;
  ds707_ccm_client_reg_req.req_cb = ds707_ccmi_cfcm_event_cb; 
  ds707_ccm_client_reg_req.monitor_mask = 0;

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
  ds707_ccm_client_reg_req.monitor_mask |= CFCM_MONITOR_MASK_CPU;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

  ds707_ccm_client_reg_req.monitor_mask |= CFCM_MONITOR_MASK_THERMAL_PA;
  ds707_ccm_client_reg_req.monitor_mask |= CFCM_MONITOR_MASK_THERMAL_PA_EX;

  if (	ds707_ccm_client_reg_req.monitor_mask != 0 )
  {
    cfcm_client_register(&ds707_ccm_client_reg_req);

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
    ds707_ccm_register_cpu_f = TRUE;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */
  }

}

/*===========================================================================
FUNCTION      DS707_CCMI_DEREGISTER_WITH_CFCM_EVENTS

DESCRIPTION   This function registers CCM with CFCM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_deregister_with_cfcm_events (void)
{
  cfcm_dereg_req_type_s ds707_ccm_client_dereg_req;
 
  ds707_ccm_client_dereg_req.client_id = CFCM_CLIENT_HDR_DS;
  
  cfcm_client_deregister(&ds707_ccm_client_dereg_req);

#ifdef FEATURE_DATA_FL_FLOW_CONTROL
  ds707_ccm_register_cpu_f = FALSE;
#endif /* FEATURE_DATA_FL_FLOW_CONTROL */

}

/*===========================================================================
FUNCTION      DS707_CCMI_CFCM_EVENT_CB

DESCRIPTION   Callback registered with CFCM for CPU and Thermal monitor related events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_cfcm_event_cb 
(
  cfcm_cmd_type_s* fc_info
)
{

  ds707_ccm_flow_control_cmd_enum_type ccm_flow_cmd = DS707_CCM_FLOW_OFF;
  
  if ( fc_info->client_id != CFCM_CLIENT_HDR_DS )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "Invalid client_id %d passed to ds707_ccmi_cfcm_event_cb",
                    fc_info->client_id);
    return;
  }

  switch (fc_info->cmd)  
  {
    case CFCM_CMD_FC_OFF: 
      ccm_flow_cmd = DS707_CCM_FLOW_OFF;
      break;

    case CFCM_CMD_UP:
      ccm_flow_cmd = DS707_CCM_FLOW_UP;
      break;
       
    case CFCM_CMD_FREEZE:
      ccm_flow_cmd = DS707_CCM_FLOW_FREEZE;
      break;

    case CFCM_CMD_DOWN:
      ccm_flow_cmd = DS707_CCM_FLOW_DOWN;
      break;

    case CFCM_CMD_SET_MIN:
      ccm_flow_cmd = DS707_CCM_FLOW_SET_MIN;
      break;

    case CFCM_CMD_SHUT_DOWN:
      ccm_flow_cmd = DS707_CCM_FLOW_SHUT_DOWN;
      break;
      
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Invalid CFCM command: %d", fc_info->cmd);
      return;
  }
  
  if ( fc_info->monitors_active & CFCM_MONITOR_MASK_CPU )
  {
    ds707_ccm_cpu_flow_control_cmd_hdlr_ex(ccm_flow_cmd);
  }
  
  if ( fc_info->monitors_active & CFCM_MONITOR_MASK_THERMAL_PA ) 
  {
    if ( (fc_info->monitors_active & CFCM_MONITOR_MASK_THERMAL_PA_EX) &&
         (ccm_flow_cmd == DS707_CCM_FLOW_DOWN) )
    {
      ccm_flow_cmd = DS707_CCM_FLOW_DOWN_MORE;
    } 

    if ( ccm_flow_cmd == DS707_CCM_FLOW_UP )
    {
      ccm_flow_cmd = DS707_CCM_FLOW_OFF;
    }

    ds707_ccm_therm_flow_control_cmd_hdlr_ex(ccm_flow_cmd,fc_info->step_timer);
  }

  return;
}

#endif /* defined (FEATURE_CCM_FLOW_CONTROL)*/
