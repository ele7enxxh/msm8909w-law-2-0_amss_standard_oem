#ifndef DS707_CCM_H
#define DS707_CCM_H
/*===========================================================================
                         D S 7 0 7 _ C C M
GENERAL DESCRIPTION
  This file contains the congestion control manager functionality. 
  Currently it runs in CFCM task context.

EXTERNALIZED FUNCTIONS
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

 Copyright (c) 2008 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.h_v   1.2   19 Nov 2002 19:23:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ds707_ccm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
07/02/14   sd         Interfacing with CFCM for CPU and Thermal based flow
                      control events.
04/08/11   ls         Add support for thermal PA and Diversity
01/31/11   sa         Made changes for Software decoupling.
10/15/10   ls         Changed flow control command value
08/06/10   ls         Add featurization to avoid compiler errors
04/27/10   ls         Add themal monitor PA support
10/21/08   sz         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

/*-------------------------------------------------------------------------
 Enum for identifying the clients registered with CCM.
 *-----------------------------------------------------------------------*/
typedef enum
{
  DS707_CCM_FTC_MAC_FC = 0,
  DS707_CCM_THERMAL_BACKOFF_FC = 1,
  DS707_CCM_THERMAL_DIVERSITY_FC = 2,
  DS707_CCM_CLIENT_MAX = 3
} ds707_ccm_client_enum_type;

/*-------------------------------------------------------------------------
 Enum for identifying the resource monitors.
 Make sure that the enums are consistent with fc_processor_type defined in fc.h. 
 Here the enum starts from CPU Monitors.
 *-----------------------------------------------------------------------*/
typedef enum
{
  DS707_CCM_CPU_MODEM   = 0, /* Local CPU Monitor */
  DS707_CCM_CPU_APPS    = 1, /* Remote CPU Monitor */
  DS707_CCM_MEM_WMK_SIO_FWD = 2,
  DS707_CCM_MEM_WMK_UM_FWD = 3,
  DS707_CCM_MEM_WMK_SMD_FWD = 4,  /* only for multiple processor */
  DS707_CCM_MEM_DSM = 5,          /* DSM Pool */
  DS707_CCM_THERM = 6,            /* Thermal monitor */
  DS707_CCM_MONITOR_MAX = 7
} ds707_ccm_monitor_enum_type;

/*-------------------------------------------------------------------------
 Enum of flow control commands.
 *-----------------------------------------------------------------------*/
typedef enum
{
  DS707_CCM_FLOW_OFF       = 0,        /* 0000000 */
  DS707_CCM_FLOW_UP        = 4,        /* 0000100 */
  DS707_CCM_FLOW_FREEZE    = 6,        /* 0000110 */
  DS707_CCM_FLOW_DOWN      = 7,        /* 0000111 */
  DS707_CCM_FLOW_SET_MIN   = 15,       /* 0001111 */
  DS707_CCM_FLOW_DOWN_MORE = 31,       /* 0011111 */
  DS707_CCM_FLOW_SHUT_DOWN = 63,       /* 0111111 */
  DS707_CCM_FLOW_TIME_OUT  = 64,       /* 1000000 */
  DS707_CCM_FLOW_MAX
} ds707_ccm_flow_control_cmd_enum_type;

typedef struct
{
  uint16 init_reduction_pattern;
  uint16 max_reduction_pattern;
  uint16 delay_time_ms;
  uint16 period_ms;
} ds707_ccm_fmac_flow_control_params_type;

typedef struct
{
  uint16 step_down_timer_ms;
  uint16 step_up_timer_ms;
  uint16 delay_time_ms;
  uint8  default_tx_power_bo_pattern_number;
  uint8  max_tx_power_bo_pattern_number;
  uint16 tx_power_bo_period_ms;
} ds707_ccm_pa_backoff_flow_control_params_type;

/*-------------------------------------------------------------------------
  Table of handler functions for CCM to control clients. Each client
  registers the callback functions with CCM, and CCM maintains such 
  a table for it. This is only applied for FMAC to consider backward
  compatibility.
 *-----------------------------------------------------------------------*/
typedef struct
{
  void (*ds707_ccm_config_fmac_func)(ds707_ccm_fmac_flow_control_params_type *params);
  void (*ds707_ccm_contrl_fmac_func)(ds707_ccm_flow_control_cmd_enum_type cmd);
} ds707_ccm_hdlr_func_tbl_type;

/*-------------------------------------------------------------------------
  Table of handler functions for CCM to control clients. Each client
  registers the callback functions with CCM, and CCM maintains such 
  a table for it. All the clients have to use this to register
  the call back functions except FMAC.
 *-----------------------------------------------------------------------*/
typedef struct
{
  /* Config and Control functions */
  void (*ds707_ccm_comm_config_func)( void *params);
  void (*ds707_ccm_comm_contrl_func)( ds707_ccm_flow_control_cmd_enum_type cmd);
} ds707_ccm_hdlr_func_comm_tbl_type;

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS707_CCM_MEM_FLOW_CONTROL_CMD_HDLR

DESCRIPTION   Command handler for commands from memory watermarks..
  
DEPENDENCIES  This function should be called when the FC_CCM_MEM_FC_SIG is set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_mem_flow_control_cmd_hdlr 
(
  void
);

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
);

/* ===========================================================================
FUNCTION      DS707_CCM_THERM_FLOW_CONTROL_CMD_HDLR

DESCRIPTION   Dummy function to resolve compilation issue with FC.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_ccm_therm_flow_control_cmd_hdlr 
(
  void
);

#endif /* DS707_CCM_H */
