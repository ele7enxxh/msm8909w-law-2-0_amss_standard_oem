#ifndef DS707_CCMI_H
#define DS707_CCMI_H
/*===========================================================================
                         D S 7 0 7 _ C C M I
GENERAL DESCRIPTION
  This file contains the congestion control manager functionality.

FUNCTIONS
 DS707_CCM_POWERUP_INIT
   Performs power-up initialization of CCM.

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

 DS707_CCMI_REGISTER_WITH_CFCM_EVENTS
   Registers CCM with CFCM to get event based callbacks

 DS707_CCMI_DEREGISTER_WITH_CFCM_EVENTS
   De-registers CCM with CFCM

 DS707_CCMI_CONFIG_CLIENTS_PARAMS
   Configures CCM clients with parameters.

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

 Copyright (c) 2008 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.h_v   1.2   19 Nov 2002 19:23:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_ccmi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/02/14   sd         Interfacing with CFCM for CPU and Thermal based flow
                      control events.
09/16/11   vpk        Add support for NPA thermal
04/08/11   ls         Add support thermal PA and Diverisy
02/11/11   ack        Global Variable Cleanup
01/31/11   sa         Made changes for Software decoupling.
04/27/10   ls         Add themal monitor PA support
10/21/08   sz         First version of file.
===========================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "queue.h"

#if defined (FEATURE_CCM_FLOW_CONTROL)
#include "fc.h"
#include "ds707_ccm.h"

#define DS707_CCM_MEM_FC_CMD_MAX   15

#define DS707_CCM_CPU_REPORT_PERIOD_DEFAULT 4

#define DS707_CCM_FMAC_INIT_PATTERN_DEFAULT 80
#define DS707_CCM_FMAC_MAX_PATTERN_DEFAULT 80
#define DS707_CCM_FMAC_PERIOD_MS_DEFAULT 200
#define DS707_CCM_FMAC_DELAY_MS_DEFAULT 2

#define DS707_CCM_THERM_STEP_TIMER_MSEC 1000

#define DS707_CCM_RLFC_DELAY_TIMER_DELTA_MSEC 2

#define DS707_CCM_MAX_PAYLOAD             12
#define DS707_CCM_MIN_PAYLOAD             2

#define DS707_CCM_THERM_DELAY_MSEC        1000  /* 1 second */
#define DS707_CCM_THERM_DTPBPN            2     /* default_tx_power_bo_pattern_number */
#define DS707_CCM_THERM_MTPBPN            2     /* max_tx_power_bo_pattern_number */
#define DS707_CCM_THERM_TPBP_MSEC         1200  /* tx_power_bo_period_ms */

typedef enum
{
  RLFC_INVALID  = -1,
  RLFC_OFF      = 0,
  RLFC_DELAY    = 1,
  RLFC_FREEZE   = 2,
  RLFC_ON       = 3,
  RLFC_MAX      = 4
} ds707_ccm_rlfc_state_enum_type;

/*===========================================================================
                            TYPEDEFS 
===========================================================================*/
/*-------------------------------------------------------------------------
 Message structure for holding reports from memory watermarks.
 *-----------------------------------------------------------------------*/
typedef struct
{
  q_link_type                           link;
  ds707_ccm_monitor_enum_type           monitor_idx;
  ds707_ccm_flow_control_cmd_enum_type  fc_cmd;
} ds707_ccm_fc_cmd_type;

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
/*-------------------------------------------------------------------------
 Message structure for holding reports from thermal events
 *-----------------------------------------------------------------------*/
typedef struct
{
  ds707_ccm_flow_control_cmd_enum_type ccm_thermal_events;
                /* Record ccm thermal events */
  uint32 step_timer;
                /* Record thermal step up timer */
} ds707_ccm_therm_fc_type;
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

/*-------------------------------------------------------------------------
 Message structure for holding the settings for thermal
 This setting shall be initialized during call bringing up
 *-----------------------------------------------------------------------*/
typedef struct
{
  ds707_ccm_rlfc_state_enum_type therm_rlfc;
                /* This is a thermal RL flag: RLFC_OFF/RLFC ON */
  rex_timer_type therm_rlfc_delay_timer;
                /* Thermal delay timer */
} ds707_ccm_therm_arb_type;
/*-------------------------------------------------------------------------
 Message structure for holding payload size and cpu flow control info
 *-----------------------------------------------------------------------*/
typedef struct
{
  uint8 payload;
                /* payload level from 0-12 */
  boolean enable_cpu_flow_control;
                /* TRUE: Enable flow control to RMAC
                 * FALSE: Disable flow control to RMAC */
} ds707_ccm_arb_rmac_type;

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
);

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
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
  unsigned long param /*param unused */
);

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
);
#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

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
);

/*===========================================================================
FUNCTION      DS707_CCM_TCHUP_INIT

DESCRIPTION   This function performs configuration and registration with other
              functional blocks such as CPU Monitors and FTC MAC FC.
              This function must be called when traffic channel is setup.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccm_tchup_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_CCM_TCHDN_CLEANUP

DESCRIPTION   This function performs cleaning up.
              This function must be called when the traffic channel is torn
              down.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccm_tchdn_cleanup
(
  void
);

/*===========================================================================
FUNCTION      DS707_CCM_CONFIG_PARAMS

DESCRIPTION   This function initializes the parameters that are used for
              configuring CPU Monitor and FMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_config_params
(
  void
);

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
);

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
);

/*===========================================================================
FUNCTION      DS707_CCMI_REGISTER_WITH_CFCM_EVENTS

DESCRIPTION   Registers CCM with CFCM to get CFCM events for CPU and Thermal monitors.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_register_with_cfcm_events 
(
  void
);

/*===========================================================================
FUNCTION      DS707_CCMI_DEREGISTER_WITH_CFCM_EVENTS

DESCRIPTION   This function registers CCM with CFCM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_deregister_with_cfcm_events 
(
  void
);

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
#ifdef FEATURE_DATA_FL_FLOW_CONTROL
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
);

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
);

/*===========================================================================
FUNCTION      DS707_CCMI_DECIDE_FC_CMD_TO_FMAC

DESCRIPTION   This function arbitrates incoming flow control reports, and
              sends the resulting commands to FMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_decide_fc_cmd_to_fmac
(
  void
);

/*===========================================================================
FUNCTION      DS707_CCMI_DECIDE_FC_CMD_TO_RMAC

DESCRIPTION   This function arbitrates incoming flow control reports, and
              sends the resulting commands to RMAC FC.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_decide_fc_cmd_to_rmac
(
  void
);

/*===========================================================================
FUNCTION      DS707_CCMI_GET_MEM_CMD_BUF

DESCRIPTION   This function gets the command buffer for a command received from
              a memory watermark.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds707_ccm_fc_cmd_type* ds707_ccmi_get_mem_cmd_buf
(
  void
);
/*===========================================================================
FUNCTION      DS707_CCMI_PUT_MEM_CMD

DESCRIPTION   This function puts a command in CCM's memory watermark command
              queue, and sets FC_CCM_MEM_FC_SIG.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ccmi_put_mem_cmd
(
  ds707_ccm_fc_cmd_type* cmd_ptr
);
#endif /*FEATURE_DATA_FL_FLOW_CONTROL*/

#ifdef FEATURE_CCM_THERM_FLOW_CONTROL
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
);

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
);
/*===========================================================================
FUNCTION      DS707_CCMI_DECIDE_THERM_FC_CMD_TO_PA_DIVERSITY

DESCRIPTION   This function arbitrates incoming flow control commands and
              forward command to PA and diversity function in HDR MAC
              layers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccmi_decide_therm_fc_cmd_to_pa_diversity
(
  void
);
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
);

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
);

#endif /* FEATURE_CCM_THERM_FLOW_CONTROL */

#endif /* defined (FEATURE_CCM_FLOW_CONTROL) */
#endif /* DS707_CCMI_H */
